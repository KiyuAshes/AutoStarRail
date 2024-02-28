#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/ForeignInterfaceHost.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrPluginManagerImpl.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/PluginManager.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/Utils/InternalUtils.h>
#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>
#include <AutoStarRail/Core/i18n/GlobalLocale.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <AutoStarRail/Utils/StringUtils.h>
#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <boost/pfr/core.hpp>
#include <fstream>
#include <functional>
#include <magic_enum.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <utility>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

using CommonBasePtr = std::variant<AsrPtr<IAsrBase>, AsrPtr<IAsrSwigBase>>;

ASR_NS_ANONYMOUS_DETAILS_BEGIN

auto GetFeaturesFrom(const CommonPluginPtr& p_plugin)
    -> ASR::Utils::Expected<std::vector<AsrPluginFeature>>
{
    constexpr size_t MAX_ENUM_COUNT =
        magic_enum::enum_count<AsrPluginFeature>() + static_cast<size_t>(30);

    AsrPluginFeature feature{};
    auto             result = std::vector<AsrPluginFeature>{0};
    result.reserve(magic_enum::enum_count<AsrPluginFeature>());

    for (size_t i = 0; i < MAX_ENUM_COUNT; ++i)
    {
        const auto get_feature_result =
            CommonPluginEnumFeature(p_plugin, i, &feature);
        if (IsOk(get_feature_result))
        {
            result.push_back(feature);
            continue;
        }
        if (get_feature_result == ASR_E_OUT_OF_RANGE)
        {
            return result;
        }
        return tl::make_unexpected(get_feature_result);
    }
    ASR_CORE_LOG_WARN(
        "Executing function \"EnumFeature\" in plugin more than the maximum limit of {} times, stopping.",
        MAX_ENUM_COUNT);
    return result;
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

auto GetIidVectorSize(IAsrGuidVector* p_iid_vector)
    -> ASR::Utils::Expected<size_t>
{
    size_t     iid_size{};
    const auto get_iid_size_result = p_iid_vector->Size(&iid_size);
    if (!IsOk(get_iid_size_result))
    {
        AsrPtr<IAsrReadOnlyString> p_error_message{};
        ::AsrGetPredefinedErrorMessage(
            get_iid_size_result,
            p_error_message.Put());
        ASR_CORE_LOG_ERROR(
            "Error happened in class IAsrGuidVector. Pointer = {}. Error code = {}. Error message = \"{}\".",
            static_cast<void*>(p_iid_vector),
            get_iid_size_result,
            p_error_message);
        return tl::make_unexpected(get_iid_size_result);
    }
    return iid_size;
}

auto GetIidFromIidVector(IAsrGuidVector* p_iid_vector, uint32_t iid_index)
    -> ASR::Utils::Expected<AsrGuid>
{
    AsrGuid    iid{AsrIidOf<IAsrBase>()};
    const auto get_iid_result = p_iid_vector->At(iid_index, &iid);
    if (!IsOk(get_iid_result))
    {
        AsrPtr<IAsrReadOnlyString> p_error_message{};
        ::AsrGetPredefinedErrorMessage(get_iid_result, p_error_message.Put());
        ASR_CORE_LOG_ERROR(
            "Error happened in class IAsrGuidVector. Pointer = {}. Error code = {}. Error message = \"{}\".",
            static_cast<void*>(p_iid_vector),
            get_iid_result,
            p_error_message);
        return tl::make_unexpected(get_iid_result);
    }
    return iid;
}

auto CreateInterface(
    const char*            u8_plugin_name,
    const CommonPluginPtr& common_p_plugin,
    AsrPluginFeature       feature) -> std::optional<CommonBasePtr>
{
    constexpr auto& CREATE_FEATURE_INTERFACE_FAILED_MESSAGE =
        "Error happened when calling p_plugin->CreateFeatureInterface."
        "Error code = {}. Plugin Name = {}.";
    return std::visit(
        ASR::Utils::overload_set{
            [feature, u8_plugin_name](
                AsrPtr<IAsrPlugin> p_plugin) -> std::optional<CommonBasePtr>
            {
                AsrPtr<IAsrBase> result{};
                if (const auto cfi_result = p_plugin->CreateFeatureInterface(
                        feature,
                        result.PutVoid());
                    IsFailed(cfi_result))
                {
                    ASR_CORE_LOG_ERROR(
                        CREATE_FEATURE_INTERFACE_FAILED_MESSAGE,
                        cfi_result,
                        u8_plugin_name);
                    return {};
                }
                return result;
            },
            [feature, u8_plugin_name](
                AsrPtr<IAsrSwigPlugin> p_plugin) -> std::optional<CommonBasePtr>
            {
                const auto cfi_result =
                    p_plugin->CreateFeatureInterface(feature);
                if (IsFailed(cfi_result.error_code))
                {
                    ASR_CORE_LOG_ERROR(
                        CREATE_FEATURE_INTERFACE_FAILED_MESSAGE,
                        cfi_result.error_code,
                        u8_plugin_name);
                    return {};
                }
                AsrPtr<IAsrSwigBase> result{};
                *result.PutVoid() = cfi_result.value.GetVoid();
                return result;
            }},
        common_p_plugin);
}

auto QueryErrorLensFrom(
    const char*          u8_plugin_name,
    const CommonBasePtr& common_p_base) -> std::optional<AsrPtr<IAsrErrorLens>>
{
    constexpr auto& QUERY_ERROR_LENS_FAILED_MESSAGE =
        "Failed when calling QueryInterface. ErrorCode = {}. Pointer = {}. Plugin name = {}.";

    return std::visit(
        ASR::Utils::overload_set{
            [u8_plugin_name](const AsrPtr<IAsrBase>& p_base)
                -> std::optional<AsrPtr<IAsrErrorLens>>
            {
                AsrPtr<IAsrErrorLens> result;
                if (const auto qi_result = p_base.As(result);
                    ASR::IsFailed(qi_result))
                {
                    ASR_CORE_LOG_ERROR(
                        QUERY_ERROR_LENS_FAILED_MESSAGE,
                        qi_result,
                        static_cast<void*>(p_base.Get()),
                        u8_plugin_name);
                    return {};
                }

                return result;
            },
            [u8_plugin_name](const AsrPtr<IAsrSwigBase>& p_base)
                -> std::optional<AsrPtr<IAsrErrorLens>>
            {
                auto qi_result =
                    p_base->QueryInterface(AsrIidOf<IAsrSwigErrorLens>());
                if (ASR::IsFailed(qi_result.error_code))
                {
                    ASR_CORE_LOG_ERROR(
                        QUERY_ERROR_LENS_FAILED_MESSAGE,
                        qi_result.error_code,
                        static_cast<void*>(p_base.Get()),
                        u8_plugin_name);
                    return {};
                }

                AsrPtr<IAsrErrorLens> result{
                    new SwigToCpp<IAsrSwigErrorLens>{
                        static_cast<IAsrSwigErrorLens*>(
                            qi_result.value.GetVoid())},
                    take_ownership};

                qi_result.value.Get()->Release();

                return result;
            }},
        common_p_base);
}

auto QueryTypeInfoFrom(
    const char*          p_plugin_name,
    const CommonBasePtr& common_p_base) -> std::optional<CommonTypeInfoPtr>
{
    constexpr auto& QUERY_TYPE_INFO_FAILED_MESSAGE =
        "Failed when querying IAsrTypeInfo or IAsrSwigTypeInfo. ErrorCode = {}. Pointer = {}. Plugin name = {}.";
    return std::visit(
        ASR::Utils::overload_set{
            [p_plugin_name](const AsrPtr<IAsrBase>& p_base)
                -> std::optional<CommonTypeInfoPtr>
            {
                AsrPtr<IAsrTypeInfo> result;
                if (const auto qi_result = p_base.As(result);
                    IsFailed(qi_result))
                {
                    ASR_CORE_LOG_ERROR(
                        QUERY_TYPE_INFO_FAILED_MESSAGE,
                        qi_result,
                        static_cast<void*>(p_base.Get()),
                        p_plugin_name);
                    return {};
                }
                return result;
            },
            [p_plugin_name](const AsrPtr<IAsrSwigBase>& p_base)
                -> std::optional<CommonTypeInfoPtr>
            {
                const auto qi_result =
                    p_base->QueryInterface(AsrIidOf<IAsrSwigTypeInfo>());
                if (IsFailed(qi_result.error_code))
                {
                    ASR_CORE_LOG_ERROR(
                        QUERY_TYPE_INFO_FAILED_MESSAGE,
                        qi_result.error_code,
                        static_cast<void*>(p_base.Get()),
                        p_plugin_name);
                    return {};
                }
                AsrPtr<IAsrSwigTypeInfo> result;
                *result.PutVoid() = qi_result.value.GetVoid();
                return result;
            }},
        common_p_base);
}

constexpr auto& GET_GUID_FAILED_MESSAGE =
    "Get guid from interface failed. Error code = {}";

auto GetInterfaceStaticStorage(
    const PluginManager::InterfaceStaticStorageMap& guid_storage_map,
    const AsrGuid&                                  guid)
    -> ASR::Utils::Expected<
        std::reference_wrapper<const PluginManager::InterfaceStaticStorage>>
{
    const auto it_storage = guid_storage_map.find(guid);
    if (it_storage == guid_storage_map.end())
    {
        ASR_CORE_LOG_ERROR(
            "No vaild interface static storage foud. Guid = {}.",
            guid);
    }

    return std::cref(it_storage->second);
}

ASR_NS_ANONYMOUS_DETAILS_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

AsrResult AsrGetErrorMessage(
    IAsrTypeInfo*        p_error_generator,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_explanation)
{
    AsrGuid guid;
    if (const auto get_guid_result = p_error_generator->GetGuid(&guid);
        ASR::IsFailed(get_guid_result))
    {
        return get_guid_result;
    }

    return Asr::Core::ForeignInterfaceHost::g_plugin_manager.GetErrorMessage(
        guid,
        error_code,
        pp_out_error_explanation);
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
    IAsrSwigTypeInfo* p_error_generator,
    AsrResult         error_code)
{
    AsrRetReadOnlyString            result{};
    ASR::AsrPtr<IAsrReadOnlyString> p_result;

    const auto ret_guid = p_error_generator->GetGuid();
    if (ASR::IsFailed(ret_guid.error_code))
    {
        result.error_code = ret_guid.error_code;
    }
    result.error_code =
        ASR::Core::ForeignInterfaceHost::g_plugin_manager.GetErrorMessage(
            ret_guid.value,
            error_code,
            p_result.Put());

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
            // IAsrTypeInfo.h
            AsrIidOf<IAsrTypeInfo>(),
            AsrIidOf<IAsrSwigTypeInfo>(),
            AsrIidOf<IAsrGuidVector>(),
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
    IAsrGuidVector* p_iid_vector,
    IAsrErrorLens*  p_error_lens)
{
    const auto get_iid_size_result = Details::GetIidVectorSize(p_iid_vector);
    if (!get_iid_size_result)
    {
        return get_iid_size_result.error();
    }
    const auto iid_size = get_iid_size_result.value();
    // try to use all iids to register IAsrErrorLens instance.
    for (size_t i = 0; i < iid_size; ++i)
    {
        const auto get_iid_from_iid_vector_result =
            Details::GetIidFromIidVector(p_iid_vector, i);
        if (!get_iid_from_iid_vector_result)
        {
            if (get_iid_size_result.error() == ASR_E_OUT_OF_RANGE)
            {
                ASR_CORE_LOG_WARN(
                    "Received ASR_E_OUT_OF_RANGE when calling IAsrIidVector::At(). Pointer = {}. Size = {}. Index = {}.",
                    static_cast<void*>(p_iid_vector),
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
                    "Trying to register duplicate IAsrErrorLens instance. Operation ignored."
                    "Pointer = {}. Iid = {}.",
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
    IAsrSwigGuidVector* p_guid_vector,
    IAsrSwigErrorLens*  p_error_lens)
{
    AsrPtr<IAsrErrorLens> p_cpp_error_lens{
        new SwigToCpp<IAsrSwigErrorLens>{p_error_lens},
        take_ownership};
    const auto p_cpp_guid_vector =
        ASR::MakeAsrPtr<IAsrGuidVector, SwigToCpp<IAsrSwigGuidVector>>(
            p_guid_vector);
    return Register(p_cpp_guid_vector.Get(), p_cpp_error_lens.Get());
}

auto ErrorLensManager::GetErrorMessage(
    const AsrGuid&      iid,
    IAsrReadOnlyString* locale_name,
    AsrResult           error_code) const
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>
{
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
    return tl::make_unexpected(ASR_E_OUT_OF_RANGE);
}

ASR_DEFINE_VARIABLE(g_plugin_manager){};

const std::unordered_map<AsrPluginFeature, std::function<void*()>>
    g_plugin_object_factory{};

AsrResult PluginManager::AddInterface(
    const Plugin& plugin,
    const char*   u8_plugin_name)
{
    const auto& common_p_plugin = plugin.p_plugin_;
    const auto& opt_resource_path = plugin.sp_desc_->opt_resource_path;

    const auto expected_features = Details::GetFeaturesFrom(common_p_plugin);
    if (!expected_features)
    {
        return expected_features.error();
    }
    for (const auto& features = expected_features.value();
         const auto  feature : features)
    {
        // TODO: 根据feature枚举将对应接口添加到对应manager。

        auto opt_common_p_base =
            Details::CreateInterface(u8_plugin_name, common_p_plugin, feature);

        if (!opt_common_p_base)
        {
            // NOTE: Error message will be printed by CreateInterface.
            continue;
        }

        if (opt_resource_path)
        {
            if (const auto opt_common_p_type_info = Details::QueryTypeInfoFrom(
                    u8_plugin_name,
                    opt_common_p_base.value()))
            {
                const auto&   relative_path = opt_resource_path.value();
                std::u8string u8_relative_path{
                    ASR_FULL_RANGE_OF(relative_path)};
                InterfaceStaticStorage storage{
                    {std::filesystem::current_path() / u8_relative_path},
                    plugin.sp_desc_};
                std::visit(
                    ASR::Utils::overload_set{
                        [this,
                         &storage](const AsrPtr<IAsrTypeInfo>& p_type_info) {
                            RegisterInterfaceStaticStorage(
                                p_type_info.Get(),
                                storage);
                        },
                        [this, &storage](
                            const AsrPtr<IAsrSwigTypeInfo>& p_type_info) {
                            RegisterInterfaceStaticStorage(
                                p_type_info.Get(),
                                storage);
                        }},
                    opt_common_p_type_info.value());
            }
        }

        switch (feature)
        {
        case ASR_PLUGIN_FEATURE_ERROR_LENS:
        {
            AsrPtr<IAsrGuidVector> p_guid_vector{};
            // TODO: LOG时包含Plugin本身的名称
            const auto opt_p_error_lens = Details::QueryErrorLensFrom(
                u8_plugin_name,
                opt_common_p_base.value());
            if (!opt_p_error_lens)
            {
                continue;
            }

            const auto& p_error_lens = opt_p_error_lens.value();

            if (const auto get_iids_result =
                    p_error_lens->GetSupportedIids(p_guid_vector.Put());
                ASR::IsFailed(get_iids_result))
            {
                ASR_CORE_LOG_ERROR(
                    "Try to get supported iids failed. Error code = {}. Plugin name = {}.",
                    get_iids_result,
                    u8_plugin_name);
                break;
            }
            error_lens_manager_.Register(
                p_guid_vector.Get(),
                p_error_lens.Get());
            break;
        }
        default:
            throw ASR::Utils::UnexpectedEnumException::FromEnum(feature);
        }
    }
    return ASR_E_NO_IMPLEMENTATION;
}

void PluginManager::RegisterInterfaceStaticStorage(
    IAsrTypeInfo*                 p_interface,
    const InterfaceStaticStorage& storage)
{
    AsrGuid guid;
    if (const auto get_guid_result = p_interface->GetGuid(&guid);
        IsFailed(get_guid_result))
    {
        ASR_CORE_LOG_ERROR(Details::GET_GUID_FAILED_MESSAGE, get_guid_result);
    }

    guid_storage_map_[guid] = storage;
}

void PluginManager::RegisterInterfaceStaticStorage(
    IAsrSwigTypeInfo*             p_swig_interface,
    const InterfaceStaticStorage& storage)
{
    const auto ret_guid = p_swig_interface->GetGuid();
    if (IsFailed(ret_guid.error_code))
    {
        ASR_CORE_LOG_ERROR(
            Details::GET_GUID_FAILED_MESSAGE,
            ret_guid.error_code);
    }

    guid_storage_map_[ret_guid.value] = storage;
}

std::unique_ptr<PluginDesc> PluginManager::GetPluginDesc(
    const std::filesystem::path& metadata_path,
    bool                         is_directory)
{
    std::unique_ptr<PluginDesc> result{};
    std::ifstream               plugin_config_stream{};

    ASR::Utils::EnableStreamException(
        plugin_config_stream,
        std::ios::badbit | std::ios::failbit,
        [&metadata_path](auto& stream) { stream.open(metadata_path.c_str()); });

    const auto config = nlohmann::json::parse(plugin_config_stream);
    result = std::make_unique<PluginDesc>(config.get<PluginDesc>());

    if (!is_directory)
    {
        result->opt_resource_path = {};
    }

    return result;
}

AsrResult PluginManager::GetInterface(const Plugin& plugin)
{
    (void)plugin;
    return ASR_E_NO_IMPLEMENTATION;
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

/**
 * @brief 插件创建失败时，产生这个类，负责生成报错的信息和错误状态的插件
 */
struct FailedPluginProxy : public ASR::Utils::NonCopyableAndNonMovable
{
    AsrPtr<IAsrReadOnlyString> error_message;
    AsrPtr<IAsrReadOnlyString> name;
    AsrResult                  error_code;

    FailedPluginProxy(
        const std::filesystem::path& metadata_path,
        AsrResult                    error_code)
        : error_message{[](AsrResult error_code)
                        {
                            AsrPtr<IAsrReadOnlyString> result{};
                            AsrGetPredefinedErrorMessage(
                                error_code,
                                result.Put());
                            return result;
                        }(error_code)},
          name{MakeAsrPtr<IAsrReadOnlyString, ::AsrStringCppImpl>(
              metadata_path)},
          error_code{error_code}
    {
    }

    FailedPluginProxy(IAsrReadOnlyString* plugin_name, AsrResult error_code)
        : error_message{[](AsrResult error_code)
                        {
                            AsrPtr<IAsrReadOnlyString> result{};
                            AsrGetPredefinedErrorMessage(
                                error_code,
                                result.Put());
                            return result;
                        }(error_code)},
          name{plugin_name, take_ownership}, error_code{error_code}
    {
    }

    void AddPluginTo(PluginManager::NamePluginMap& map)
    {
        map.emplace(name, Plugin{error_code, error_message.Get()});
    }
};

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult PluginManager::Refresh()
{
    AsrResult result{ASR_S_OK};

    NamePluginMap map{};

    for (const auto  current_path = std::filesystem::path{"./plugins"};
         const auto& it : std::filesystem::directory_iterator{current_path})
    {
        std::filesystem::path it_path;
        if (it.is_directory())
        {
            const auto plugin_metadata_name = it.path().filename();
            it_path =
                it
                / std::filesystem::path{
                    plugin_metadata_name.u8string() + std::u8string{u8".json"}};
        }
        else
        {
            it_path = it.path();
        }
        const auto extension = it_path.extension();
        if (ASR_UTILS_STRINGUTILS_COMPARE_STRING(extension, "json"))
        {
            AsrResult plugin_create_result{ASR_E_UNDEFINED_RETURN_VALUE};
            std::unique_ptr<PluginDesc> up_plugin_desc{};

            try
            {
                auto tmp_up_plugin_desc =
                    GetPluginDesc(it_path, it.is_directory());
                up_plugin_desc = std::move(tmp_up_plugin_desc);

                if (const auto* const CURRENT_PLATFORM =
                        static_cast<const char*>(
                            ASR_UTILS_STRINGUTILS_DEFINE_U8STR(ASR_PLATFORM));
                    up_plugin_desc->supported_system.find_first_of(
                        CURRENT_PLATFORM)
                    == decltype(up_plugin_desc->supported_system)::npos)
                {
                    plugin_create_result = ASR_E_UNSUPPORTED_SYSTEM;

                    auto failed_plugin = Details::FailedPluginProxy{
                        it_path,
                        plugin_create_result};

                    ASR_CORE_LOG_ERROR(
                        "Error when checking system requirement. Error code = " ASR_STR(
                            ASR_E_UNSUPPORTED_SYSTEM) ".");
                    // 此处，plugin_name即为metadata路径，见
                    // Details::AddFailedPluginAndReturnTmpPluginName
                    ASR_CORE_LOG_ERROR(
                        "NOTE: plugin meta data file path:\"{}\"",
                        failed_plugin.name);

                    failed_plugin.AddPluginTo(map);
                    result = ASR_S_FALSE;
                    continue;
                }
            }
            catch (const nlohmann::json::exception& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                ASR_CORE_LOG_INFO(
                    "Error happened when parsing plugin metadata file. Error code = " ASR_STR(
                        ASR_CORE_LOG_INFO) ".");
                plugin_create_result = ASR_E_INVALID_JSON;
            }
            catch (const std::ios_base::failure& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                ASR_CORE_LOG_INFO(
                    "Error happened when reading plugin metadata file. Error code = " ASR_STR(
                        ASR_E_INVALID_FILE) ".");
                plugin_create_result = ASR_E_INVALID_FILE;
            }

            if (plugin_create_result != ASR_E_UNDEFINED_RETURN_VALUE)
            {
                auto failed_plugin =
                    Details::FailedPluginProxy{it_path, plugin_create_result};
                ASR_CORE_LOG_INFO(
                    "NOTE: plugin meta data file path:\"{}\"",
                    failed_plugin.name);

                failed_plugin.AddPluginTo(map);
                result = ASR_S_FALSE;
                continue;
            }

            AsrPtr<IAsrReadOnlyString> plugin_name{};

            if (auto expected_plugin_name =
                    ASR::Core::Utils::MakeAsrReadOnlyStringFromUtf8(
                        up_plugin_desc->name);
                !expected_plugin_name)
            {
                const auto error_code = expected_plugin_name.error();
                ASR_CORE_LOG_ERROR(
                    "Can not convert std::string to IAsrReadOnlyString when getting plugin name."
                    "\nError code = {}",
                    error_code);
                auto failed_plugin =
                    Details::FailedPluginProxy{it_path, error_code};
                failed_plugin.AddPluginTo(map);
                result = ASR_S_FALSE;
                continue;
            }
            else
            {
                plugin_name = expected_plugin_name.value();
            }

            std::filesystem::path plugin_path =
                it_path.parent_path()
                / std::filesystem::path{
                    it_path.stem().u8string()
                    + std::u8string{ASR_FULL_RANGE_OF(
                        up_plugin_desc->plugin_filename_extension)}};
            if (!std::filesystem::exists(plugin_path))
            {
                plugin_create_result = ASR_E_FILE_NOT_FOUND;

                ASR_CORE_LOG_ERROR(
                    "Error when checking plugin file. Error code = " ASR_STR(
                        ASR_E_FILE_NOT_FOUND) ".");

                Details::FailedPluginProxy failed_plugin{
                    plugin_name.Get(),
                    plugin_create_result};

                ASR_CORE_LOG_INFO(
                    "Expected file path:\"{}\".",
                    MakeAsrPtr<IAsrReadOnlyString, AsrStringCppImpl>(it_path));

                failed_plugin.AddPluginTo(map);
                result = ASR_S_FALSE;
                continue;
            }

            ForeignLanguageRuntimeFactoryDesc desc{};
            desc.language = up_plugin_desc->language;
            const auto expected_p_runtime = CreateForeignLanguageRuntime(desc);
            if (!expected_p_runtime)
            {
                plugin_create_result = expected_p_runtime.error();

                ASR_CORE_LOG_ERROR(
                    "Error happened when calling CreateForeignLanguageRuntime.\n"
                    "----ForeignLanguageRuntimeFactoryDesc dump begin----");
                ASR_CORE_LOG_ERROR("{{\n{}\n}}", *up_plugin_desc);
                ASR_CORE_LOG_ERROR(
                    "----ForeignLanguageRuntimeFactoryDesc dump end----");

                Details::FailedPluginProxy failed_plugin{
                    plugin_name.Get(),
                    plugin_create_result};
                failed_plugin.AddPluginTo(map);
                result = ASR_S_FALSE;
                continue;
            }
            auto& runtime = expected_p_runtime.value();

            auto expected_p_plugin = runtime->LoadPlugin(plugin_path);
            if (!expected_p_plugin)
            {
                plugin_create_result = expected_p_runtime.error();

                Details::FailedPluginProxy failed_plugin{
                    plugin_name.Get(),
                    plugin_create_result};
                failed_plugin.AddPluginTo(map);
                result = ASR_S_FALSE;
                continue;
            }

            map.emplace(
                plugin_name,
                Plugin{
                    runtime,
                    expected_p_plugin.value(),
                    std::move(up_plugin_desc)});
        }
    }

    name_plugin_map_ = std::move(map);

    return result;
}

AsrResult PluginManager::GetErrorMessage(
    const AsrGuid&       iid,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_message)
{
    ASR_UTILS_CHECK_POINTER(pp_out_error_message)

    AsrResult result{ASR_E_UNDEFINED_RETURN_VALUE};

    AsrPtr<IAsrReadOnlyString> p_default_locale_name{};
    ::AsrGetDefaultLocale(p_default_locale_name.Put());

    error_lens_manager_
        .GetErrorMessage(iid, p_default_locale_name.Get(), error_code)
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

AsrResult PluginManager::GetAllPluginInfo(
    IAsrPluginInfoVector** pp_out_plugin_info_vector)
{
    if (pp_out_plugin_info_vector == nullptr)
    {
        return ASR_E_INVALID_POINTER;
    }

    const auto p_vector = MakeAsrPtr<AsrPluginInfoVectorImpl>();
    for (const auto& pair : name_plugin_map_)
    {
        const auto& plugin_desc = pair.second;
        p_vector->AddInfo(plugin_desc.GetInfo());
    }
    p_vector.As(pp_out_plugin_info_vector);
    return ASR_S_OK;
}

auto PluginManager::GetInterfaceStaticStorage(IAsrTypeInfo* p_type_info) const
    -> Asr::Utils::Expected<
        std::reference_wrapper<const InterfaceStaticStorage>>
{
    if (p_type_info == nullptr)
    {
        ASR_CORE_LOG_ERROR("Null pointer found. Please check your code.");
        return tl::make_unexpected(ASR_E_INVALID_POINTER);
    }

    AsrGuid guid;
    if (const auto gg_result = p_type_info->GetGuid(&guid); IsFailed(gg_result))
    {
        ASR_CORE_LOG_ERROR(
            "Get GUID failed. Pointer = {}",
            Utils::VoidP(p_type_info));
        return tl::make_unexpected(gg_result);
    }

    return Details::GetInterfaceStaticStorage(guid_storage_map_, guid);
}

auto PluginManager::GetInterfaceStaticStorage(IAsrSwigTypeInfo* p_type_info)
    const -> Asr::Utils::Expected<
        std::reference_wrapper<const InterfaceStaticStorage>>
{
    if (p_type_info == nullptr)
    {
        ASR_CORE_LOG_ERROR("Null pointer found. Please check your code.");
        return tl::make_unexpected(ASR_E_INVALID_POINTER);
    }

    const auto gg_result = p_type_info->GetGuid();
    if (IsFailed(gg_result.error_code))
    {
        ASR_CORE_LOG_ERROR(
            "Get GUID failed. Pointer = {}",
            Utils::VoidP(p_type_info));
        return tl::make_unexpected(gg_result.error_code);
    }

    return Details::GetInterfaceStaticStorage(guid_storage_map_, gg_result.value);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END