#include "PluginManager.h"
#include "ForeignInterfaceHost.h"
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>
#include <AutoStarRail/Core/i18n/GlobalLocale.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <AutoStarRail/Utils/StringUtils.h>

#include <boost/pfr/core.hpp>
#include <cstddef>
#include <fstream>
#include <functional>
#include <magic_enum.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

struct _asr_internal_GetPluginFeatureContext
{
    const size_t            index;
    AsrResult* const        p_out_enum_result;
    AsrPluginFeature* const p_out_feature;
};

template <class T>
constexpr auto MakePluginFeatureGetter()
{
    if constexpr (std::is_same_v<AsrPtr<IAsrPlugin>, T>)
    {
        return [](T& p_plugin, _asr_internal_GetPluginFeatureContext context)
        {
            *context.p_out_enum_result =
                p_plugin->EnumFeature(context.index, context.p_out_enum_result);
            return *context.p_out_enum_result == ASR_S_OK;
        };
    }
    else if constexpr (std::is_same_v<std::shared_ptr<IAsrSwigPlugin>, T>)
    {
        return [](T& p_plugin, _asr_internal_GetPluginFeatureContext context)
        {
            const auto swig_enum_result = p_plugin->EnumFeature(context.index);
            *context.p_out_feature = swig_enum_result.value;
            *context.p_out_enum_result = swig_enum_result.error_code;
            return *context.p_out_enum_result == ASR_S_OK;
        };
    }
    else
    {
        throw std::runtime_error("Unsupported plugin type");
    }
}

template <class T>
auto GetSupportedFeatures(T& p_plugin, std::string_view plugin_name)
    -> ASR::Utils::ExpectedWithExplanation<std::vector<AsrPluginFeature>>
{
    std::vector<AsrPluginFeature> result{};
    constexpr size_t              max_enum_count =
        magic_enum::enum_count<AsrPluginFeature>() + static_cast<size_t>(10);

    size_t index{};

    AsrPluginFeature feature{};
    AsrResult        enum_feature_result{ASR_E_UNDEFINED_RETURN_VALUE};

    constexpr auto checker = MakePluginFeatureGetter<T>();

    for (; checker(p_plugin, {index, &enum_feature_result, &feature}); index++)
    {
        if (index < max_enum_count)
        {
            result.push_back(feature);
        }
        else
        {
            return ASR::Utils::MakeUnexpected(
                ASR_E_MAYBE_OVERFLOW,
                ASR::fmt::format(
                    "Executing function \"EnumFeature\" in plugin(name = {}) more than the maximum limit of {} times, stopping.",
                    plugin_name,
                    max_enum_count));
        }
    }

    if (enum_feature_result != ASR_E_OUT_OF_RANGE)
    {
        // TODO: Call function below when unexpected.
        // LogErrorWhenGettingPluginFeature(enum_feature_result, plugin_name)
        return ASR::Utils::MakeUnexpected(
            enum_feature_result,
            ASR::fmt::format(
                "Getting plugin(name = {}) feature failed with error code {}.",
                plugin_name,
                enum_feature_result));
    }

    return {result};
}

template <class T>
auto GetSupportedInterface(
    T&                                   p_plugin,
    const std::vector<AsrPluginFeature>& features)
{
    for (auto feature : features)
    {
        AsrPtr<IAsrBase> p_interface{};
        auto             get_interface_result =
            p_plugin->GetInterface(feature, &p_interface);
        if (!ASR::IsOk(get_interface_result))
        {
            // TODO: Call internal error lens to interpret the error code.
            ASR_CORE_LOG_ERROR(
                "Get plugin interface for feature {} (value={}) failed: {}",
                magic_enum::enum_name(feature),
                feature,
                get_interface_result);
        }
    }
}

/**
 *
 * @tparam F
 * @param error_code
 * @param p_locale_name
 * @param callback The function who return struct { AsrResult error_code,
 * AsrReadOnlyString value; };
 * @return
 */
template <class F>
auto GetErrorMessageAndAutoFallBack(
    const AsrResult     error_code,
    IAsrReadOnlyString* p_locale_name,
    F&& callback) -> ASR::Utils::Expected<ASR::AsrPtr<IAsrReadOnlyString>>
{
    constexpr size_t ERROR_CODE_INDEX = 0;
    constexpr size_t VALUE_INDEX = 1;

    ASR::AsrPtr<IAsrReadOnlyString> result{};
    auto&&     get_error_message = std::forward<F>(callback);
    const auto result_1 = get_error_message(error_code, p_locale_name);
    const auto result_1_error_code =
        boost::pfr::get<ERROR_CODE_INDEX>(result_1);

    if (ASR::IsOk(result_1_error_code))
    {
        // use default locale and retry.
        if (result_1_error_code == ASR_E_OUT_OF_RANGE)
        {
            const auto p_fallback_locale_name =
                ASR::Core::i18n::GetFallbackLocale();
            const auto result_2 =
                get_error_message(error_code, p_fallback_locale_name.Get());
            if (const auto result_2_error_code =
                    boost::pfr::get<ERROR_CODE_INDEX>(result_2);
                !IsOk(result_2_error_code))
            {
                return tl::make_unexpected(result_2_error_code);
            }
            boost::pfr::get<VALUE_INDEX>(result_2).GetImpl(result.Put());
            return result;
        }
        return tl::make_unexpected(result_1_error_code);
    }

    boost::pfr::get<VALUE_INDEX>(result_1).GetImpl(result.Put());
    return result;
}

auto GetPredefinedErrorMessage(
    const AsrResult     error_code,
    IAsrReadOnlyString* p_locale_name)
    -> ASR::Utils::Expected<ASR::AsrPtr<IAsrReadOnlyString>>
{
    // 不是插件自定义错误时
    if (error_code < ASR_E_RESERVED)
    {
        return ASR::Core::ForeignInterfaceHost::Details::
            GetErrorMessageAndAutoFallBack(
                error_code,
                p_locale_name,
                [](auto&& internal_error_code, auto&& internal_p_locale_name)
                {
                    ASR::AsrPtr<IAsrReadOnlyString> p_result{};
                    AsrRetReadOnlyString            result{};
                    result.error_code = ASR::Core::i18n::TranslateError(
                        internal_p_locale_name,
                        internal_error_code,
                        p_result.Put());
                    result.value = AsrReadOnlyString{p_result};
                    return result;
                });
    }
    ASR_CORE_LOG_ERROR(
        "The error code {} ({} >= " ASR_STR(
            ASR_E_RESERVED) ") which is not a predefined error code.",
        error_code,
        error_code);
    return tl::make_unexpected(ASR_E_OUT_OF_RANGE);
}

/**
 * @brief Get the error explanation for the predefined error code.\n
 *  NOTE: When error happened, this function will automatically log.
 * @param pointer
 * @return
 */
auto GetIidsFrom(IAsrInspectable* pointer)
    -> Utils::Expected<AsrPtr<IAsrIidVector>>
{
    ASR::AsrPtr<IAsrIidVector> p_iid_vector{};
    if (const auto get_iid_result =
            pointer->GetIids(p_iid_vector.Put());
        !ASR::IsOk(get_iid_result))
    {
        ASR_CORE_LOG_ERROR(
            "Call GetIids failed. Pointer = {}. Error code = {}",
            static_cast<void*>(pointer),
            get_iid_result);
        ASR::AsrPtr<IAsrReadOnlyString> p_get_iid_error_explanation{};
        ASR::Core::ForeignInterfaceHost::Details::GetPredefinedErrorMessage(
            get_iid_result,
            ASR::Core::i18n::GetFallbackLocale().Get());
        ASR_CORE_LOG_ERROR(
            "NOTE: The explanation for error code {} is \"{}\".",
            get_iid_result,
            p_get_iid_error_explanation);
        return tl::make_unexpected(get_iid_result);
    }
    return p_iid_vector;
}

// TODO: 考虑添加boost::stacktrace模块方便debug
auto GetIidVectorSize(IAsrIidVector* p_iid_vector)
    -> ASR::Utils::Expected<uint32_t>
{
    uint32_t   iid_size{};
    const auto get_iid_size_result = p_iid_vector->Size(&iid_size);
    if (!IsOk(get_iid_size_result))
    {
        AsrPtr<IAsrReadOnlyString> p_error_message{};
        ::AsrGetPredefinedErrorMessage(
            get_iid_size_result,
            p_error_message.Put());
        ASR_CORE_LOG_ERROR(
            "Error happened in class IAsrIidVector. Pointer = {}. Error code = {}. Error message = \"{}\".",
            static_cast<void*>(p_iid_vector),
            get_iid_size_result,
            p_error_message);
        return tl::make_unexpected(get_iid_size_result);
    }
    return iid_size;
}

auto GetIidFromIidVector(IAsrIidVector* p_iid_vector, uint32_t iid_index)
    -> ASR::Utils::Expected<AsrGuid>
{
    AsrGuid    iid{AsrIidOf<IAsrBase>()};
    const auto get_iid_result = p_iid_vector->At(iid_index, &iid);
    if (!IsOk(get_iid_result))
    {
        AsrPtr<IAsrReadOnlyString> p_error_message{};
        ::AsrGetPredefinedErrorMessage(get_iid_result, p_error_message.Put());
        ASR_CORE_LOG_ERROR(
            "Error happened in class IAsrIidVector. Pointer = {}. Error code = {}. Error message = \"{}\".",
            static_cast<void*>(p_iid_vector),
            get_iid_result,
            p_error_message);
        return tl::make_unexpected(get_iid_result);
    }
    return iid;
}

void LogWarnWhenReceiveUnexpectedAsrOutOfRange(
    IAsrIidVector* p_iid_vector,
    uint32_t       size,
    uint32_t       iid_index)
{
    ASR_CORE_LOG_WARN(
        "Received ASR_E_OUT_OF_RANGE when calling IAsrIidVector::At(). Pointer = {}. Size = {}. Index = {}.",
        static_cast<void*>(p_iid_vector),
        size,
        iid_index);
}

ASR_NS_ANONYMOUS_DETAILS_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

AsrResult AsrGetErrorMessage(
    IAsrInspectable*     p_error_generator,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_explanation)
{
    AsrResult result{ASR_E_UNDEFINED_RETURN_VALUE};

    const auto get_iid_result =
        ASR::Core::ForeignInterfaceHost::Details::GetIidsFrom(
            p_error_generator);
    if (get_iid_result)
    {
        result =
            Asr::Core::ForeignInterfaceHost::g_plugin_manager.GetErrorMessage(
                get_iid_result.value().Get(),
                error_code,
                pp_out_error_explanation);
    }
    else
    {
        result = get_iid_result.error();
    }

    return result;
}

AsrResult AsrGetPredefinedErrorMessage(
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_explanation)
{
    ASR::AsrPtr<IAsrReadOnlyString> p_default_locale_name{};
    std::ignore = ::AsrGetDefaultLocale(p_default_locale_name.Put());
    const auto result =
        ASR::Core::ForeignInterfaceHost::Details::GetPredefinedErrorMessage(
            error_code,
            p_default_locale_name.Get());
    if (result)
    {
        auto* const p_result = result.value().Get();
        p_result->AddRef();
        *pp_out_error_explanation = p_result;
        return ASR_S_OK;
    }
    return result.error();
}

AsrRetReadOnlyString AsrGetErrorMessage(
    IAsrSwigInspectable* p_error_generator,
    AsrResult            error_code)
{
    AsrRetReadOnlyString result{};
    ASR::Core::ForeignInterfaceHost::SwigToCpp<IAsrSwigInspectable>
        p_cpp_error_generator{p_error_generator};

    const auto get_iid_result =
        ASR::Core::ForeignInterfaceHost::Details::GetIidsFrom(
            &p_cpp_error_generator);
    if (get_iid_result)
    {
        ASR::AsrPtr<IAsrReadOnlyString> p_error_message{};
        result.error_code =
            Asr::Core::ForeignInterfaceHost::g_plugin_manager.GetErrorMessage(
                get_iid_result.value().Get(),
                error_code,
                p_error_message.Put());
        result.value = AsrReadOnlyString{p_error_message};
    }
    else
    {
        result.error_code = get_iid_result.error();
    }
    return result;
}

AsrRetReadOnlyString AsrGetPredefinedErrorMessage(AsrResult error_code)
{
    AsrRetReadOnlyString            result{};
    ASR::AsrPtr<IAsrReadOnlyString> p_result{};
    result.error_code =
        AsrGetPredefinedErrorMessage(error_code, p_result.Put());
    if (ASR::IsOk(result.error_code))
    {
        result.value = p_result;
    }
    return result;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

// TODO:检查添加的iid是否覆盖了所有预定义的接口，需要包含C++和SWIG版本的
const std::unordered_set<AsrGuid> g_official_iids{
    []()
    {
        std::unordered_set<AsrGuid> result{{
            // IAsrBase.h
            AsrIidOf<IAsrBase>(),
            AsrIidOf<IAsrSwigBase>(),
            // IAsrInspectable.h
            AsrIidOf<IAsrInspectable>(),
            AsrIidOf<IAsrSwigInspectable>(),
            AsrIidOf<IAsrIidVector>(),
            // AsrReadOnlyString.hpp
            AsrIidOf<IAsrReadOnlyString>(),
            AsrIidOf<IAsrString>(),
            // PluginInterface/IAsrCapture.h
            AsrIidOf<IAsrCapture>(),
            AsrIidOf<IAsrSwigCapture>(),
            AsrIidOf<IAsrCaptureFactory>(),
            AsrIidOf<IAsrSwigCaptureFactory>(),
            // PluginInterface/IAsrErrorLens.h
            AsrIidOf<IAsrErrorLens>(),
            AsrIidOf<IAsrSwigErrorLens>(),
            // PluginInterface/IAsrInput.h
            // PluginInterface/IAsrPlugin.h
            AsrIidOf<IAsrPlugin>(),
            AsrIidOf<IAsrSwigPlugin>(),
            // PluginInterface/IAsrTask.h
            AsrIidOf<IAsrTask>(),
            AsrIidOf<IAsrSwigTask>()
            // ExportInterface
        }};
        return result;
    }()};

AsrResult ErrorLensManager::Register(
    IAsrIidVector* p_iid_vector,
    IAsrErrorLens* p_error_lens)
{
    const auto get_iid_size_result = Details::GetIidVectorSize(p_iid_vector);
    if (!get_iid_size_result)
    {
        return get_iid_size_result.error();
    }
    const auto iid_size = get_iid_size_result.value();
    // try to use all iids to register IAsrErrorLens instance.
    for (uint32_t i = 0; i < iid_size; ++i)
    {
        const auto get_iid_from_iid_vector_result =
            Details::GetIidFromIidVector(p_iid_vector, i);
        if (!get_iid_from_iid_vector_result)
        {
            if (get_iid_size_result.error() == ASR_E_OUT_OF_RANGE)
            {
                Details::LogWarnWhenReceiveUnexpectedAsrOutOfRange(
                    p_iid_vector,
                    iid_size,
                    i);
                break;
            }
            return get_iid_from_iid_vector_result.error();
        }
        const auto& iid = get_iid_from_iid_vector_result.value();
        if (g_official_iids.find(iid) != g_official_iids.end())
        {
            if (map_.count(iid) == 1)
            {
                ASR_CORE_LOG_WARN(
                    "Trying to register duplicate IAsrErrorLens instance. Pointer = {}. Iid = {}.",
                    static_cast<void*>(p_error_lens),
                    iid);
            }
            // register IAsrErrorLens instance.
            map_[iid] = {p_error_lens, take_ownership};
        }
    }
    return ASR_S_OK;
}

AsrResult ErrorLensManager::Register(
    IAsrIidVector*     p_iids,
    IAsrSwigErrorLens* p_error_lens)
{
    AsrPtr<IAsrErrorLens> p_cpp_error_lens{
        new SwigToCpp<IAsrSwigErrorLens>{p_error_lens},
        take_ownership};
    return Register(p_iids, p_cpp_error_lens.Get());
}

auto ErrorLensManager::GetErrorMessage(
    IAsrIidVector*      p_iids,
    IAsrReadOnlyString* locale_name,
    AsrResult           error_code) const
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>
{
    const auto get_iid_result = Details::GetIidVectorSize(p_iids);
    if (!get_iid_result) [[unlikely]]
    {
        return tl::make_unexpected(get_iid_result.error());
    }

    const auto iid_size = get_iid_result.value();
    for (uint32_t i = 0; i < iid_size; i++)
    {
        const auto get_iid_from_iid_vector_result =
            Details::GetIidFromIidVector(p_iids, i);

        if (!get_iid_from_iid_vector_result) [[unlikely]]
        {
            if (get_iid_result.error() == ASR_E_OUT_OF_RANGE)
            {
                Details::LogWarnWhenReceiveUnexpectedAsrOutOfRange(
                    p_iids,
                    iid_size,
                    i);
            }
            return tl::make_unexpected(get_iid_from_iid_vector_result.error());
        }

        const auto& iid = get_iid_from_iid_vector_result.value();
        if (g_official_iids.contains(iid))
        {
            continue;
        }
        if (const auto it = map_.find(iid); it != map_.end())
        {
            AsrPtr<IAsrReadOnlyString> p_result{};
            const auto get_error_message_result = it->second->GetErrorMessage(
                locale_name,
                error_code,
                p_result.Put());
            if (IsOk(get_error_message_result))
            {
                return p_result;
            }

            return tl::make_unexpected(get_error_message_result);
        }
    }
    return tl::make_unexpected(ASR_E_OUT_OF_RANGE);
}

ASR_DEFINE_VARIABLE(g_plugin_manager){};

AsrResult PluginManager::AddInterface(ASR::AsrPtr<IAsrTask> p_task)
{
    (void)p_task;
    // TODO: finish this implementation.
    return ASR_E_NO_IMPLEMENTATION;
}

AsrResult PluginManager::AddInterface(IAsrSwigTask* p_task)
{
    (void)p_task;
    // TODO: finish this implementation.
    return ASR_E_NO_IMPLEMENTATION;
}

std::unique_ptr<PluginDesc> PluginManager::GetPluginDesc(
    const boost::filesystem::path& metadata_path)
{
    std::unique_ptr<PluginDesc> result{};
    std::ifstream               plugin_config_stream{};

    ASR::Utils::EnableStreamException(
        plugin_config_stream,
        std::ios::badbit | std::ios::failbit,
        [&metadata_path](auto& stream) { stream.open(metadata_path.c_str()); });

    const auto config = nlohmann::json::parse(plugin_config_stream);
    result = std::make_unique<PluginDesc>(config.get<PluginDesc>());

    return result;
}

AsrResult PluginManager::GetInterface(const Plugin& plugin)
{
    (void)plugin;
    return ASR_E_NO_IMPLEMENTATION;
}

std::vector<AsrResult> PluginManager::Refresh()
{
    auto result = ASR::Utils::MakeEmptyContainer<std::vector<AsrResult>>();
    constexpr std::size_t POSSIBLE_COUNT_OF_PLUGINS = 50;
    result.reserve(POSSIBLE_COUNT_OF_PLUGINS);

    for (const auto  current_path = boost::filesystem::path{"./plugins"};
         const auto& it : boost::filesystem::directory_iterator{current_path})
    {
        const auto& it_path = it.path();
        const auto  extension = it_path.extension();
        if (ASR_UTILS_STRINGUTILS_COMPARE_STRING(extension, "json"))
        {
            AsrResult plugin_result{ASR_S_OK};

            try
            {
                const auto up_plugin_desc = GetPluginDesc(it_path);

                if (const auto* const CURRENT_PLATFORM =
                        static_cast<const char*>(
                            ASR_UTILS_STRINGUTILS_DEFINE_U8STR(
                                ASR_STR(ASR_PLATFORM)));
                    up_plugin_desc->supported_system.find_first_of(
                        CURRENT_PLATFORM)
                    != decltype(up_plugin_desc->supported_system)::npos)
                {
                    result.push_back(ASR_E_UNSUPPORTED_SYSTEM);
                    ASR_CORE_LOG_INFO(
                        "Error when checking system requirement. Error code (" ASR_STR(
                            ASR_E_UNSUPPORTED_SYSTEM) "): {}",
                        result.back());
                    continue;
                }

                boost::filesystem::path plugin_path =
                    it_path.parent_path()
                    / boost::filesystem::path{
                        it_path.stem().string()
                        + up_plugin_desc->plugin_filename_extension};
                if (!boost::filesystem::exists(plugin_path))
                {
                    result.push_back(ASR_E_FILE_NOT_FOUND);
                    ASR_CORE_LOG_ERROR(
                        "Error when checking plugin file. Expected file path:\"{}\". Error code (" ASR_STR(
                            ASR_E_FILE_NOT_FOUND) "): {}",
                        plugin_path.string(),
                        result.back());
                    continue;
                }
            }
            catch (const nlohmann::json::exception& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                plugin_result = ASR_E_INVALID_JSON;
            }
            catch (const std::ios_base::failure& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                plugin_result = ASR_E_INVALID_FILE;
            }
            result.push_back(plugin_result);
        }
    }
    return result;
}

AsrResult PluginManager::GetErrorMessage(
    IAsrIidVector*       p_iids,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_message)
{
    if (p_iids == nullptr || pp_out_error_message == nullptr)
    {
        return ASR_E_INVALID_POINTER;
    }

    AsrResult result{ASR_E_UNDEFINED_RETURN_VALUE};

    AsrPtr<IAsrReadOnlyString> p_default_locale_name{};
    ::AsrGetDefaultLocale(p_default_locale_name.Put());

    error_lens_manager_
        .GetErrorMessage(p_iids, p_default_locale_name.Get(), error_code)
        .map(
            [&result, pp_out_error_message](const auto& p_error_message)
            {
                p_error_message->AddRef();
                *pp_out_error_message = p_error_message.Get();
                result = ASR_S_OK;
            })
        .or_else([&result](const auto ec) { result = ec; });

    return result;
}

auto PluginManager::GetAllCaptureFactory()
    -> std::vector<AsrPtr<IAsrCaptureFactory>>
{
    return asr_capture_interfaces_;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END