#include <AutoStarRail/Core/Exceptions/AsrException.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/SettingsManager/IAsrSettingsImpl.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

// TODO: support plugin set configuration. See
// https://code.visualstudio.com/api/references/contribution-points#contributes.configuration

ASR_CORE_SETTINGSMANAGER_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T, class OnTypeError, class OnJsonError>
T GetJsonValue(
    const nlohmann::json& j,
    const char*           key,
    OnTypeError           on_type_error,
    OnJsonError           on_json_error)
{
    T result{};
    try
    {
        result.value = j.at(key).get<decltype(T::value)>();
        result.error_code = ASR_S_OK;
    }
    catch (const nlohmann::json::type_error& ex)
    {
        result.error_code = ASR_E_TYPE_ERROR;
        on_type_error(ex, j, key);
    }
    catch (const nlohmann::json::exception& ex)
    {
        result.error_code = ASR_E_INVALID_JSON;
        on_json_error(ex, j, key);
    }
    return result;
}

constexpr auto default_on_type_error = [](const nlohmann::json::exception& ex,
                                          const nlohmann::json&            j,
                                          const char*                      key)
{ ASR_CORE_LOG_JSON_EXCEPTION(ex, key, j); };

constexpr auto default_on_json_error = [](const nlohmann::json::exception& ex,
                                          const nlohmann::json&            j,
                                          const char*                      key)
{ ASR_CORE_LOG_JSON_EXCEPTION(ex, key, j); };

template <class T>
T GetJsonValueOnDefaultErrorHandle(const nlohmann::json& j, const char* key)
{
    return GetJsonValue<T>(
        j,
        key,
        default_on_type_error,
        default_on_json_error);
}

ASR_NS_ANONYMOUS_DETAILS_END

IAsrSettingsImpl::IAsrSettingsImpl(
    Asr::Core::SettingsManager::AsrSettings& impl,
    const char*                              u8_type_name)
    : impl_{impl}, u8_type_name_{u8_type_name}
{
}

int64_t IAsrSettingsImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSettingsImpl::Release() { return impl_.Release(); }

AsrResult IAsrSettingsImpl::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return Utils::QueryInterface<IAsrSettings>(this, iid, pp_object);
}

ASR_IMPL IAsrSettingsImpl::GetString(
    IAsrReadOnlyString*  key,
    IAsrReadOnlyString** pp_out_string)
{
    ASR_UTILS_CHECK_POINTER(pp_out_string)

    const auto ret_string = impl_.GetString(u8_type_name_, key);
    if (IsOk(ret_string.error_code))
    {
        *pp_out_string = ret_string.value.Get();
        (*pp_out_string)->AddRef();
    }
    return ret_string.error_code;
}

ASR_IMPL IAsrSettingsImpl::GetBool(IAsrReadOnlyString* key, bool* p_out_bool)
{
    ASR_UTILS_CHECK_POINTER(p_out_bool)

    const auto ret_bool = impl_.GetBool(u8_type_name_, key);
    if (IsOk(ret_bool.error_code))
    {
        *p_out_bool = ret_bool.value;
    }
    return ret_bool.error_code;
}

ASR_IMPL IAsrSettingsImpl::GetInt(IAsrReadOnlyString* key, int64_t* p_out_int)
{
    ASR_UTILS_CHECK_POINTER(p_out_int)

    const auto ret_int = impl_.GetInt(u8_type_name_, key);
    if (IsOk(ret_int.error_code))
    {
        *p_out_int = ret_int.value;
    }
    return ret_int.error_code;
}

ASR_IMPL IAsrSettingsImpl::GetFloat(IAsrReadOnlyString* key, float* p_out_float)
{
    ASR_UTILS_CHECK_POINTER(p_out_float)

    const auto ret_float = impl_.GetFloat(u8_type_name_, key);
    if (IsOk(ret_float.error_code))
    {
        *p_out_float = ret_float.value;
    }
    return ret_float.error_code;
}

IAsrSwigSettingsImpl::IAsrSwigSettingsImpl(
    Asr::Core::SettingsManager::AsrSettings& impl,
    const char*                              u8_type_name)
    : impl_{impl}, u8_type_name_{u8_type_name}
{
}

int64_t IAsrSwigSettingsImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSwigSettingsImpl::Release() { return impl_.Release(); }

AsrRetSwigBase IAsrSwigSettingsImpl::QueryInterface(const AsrGuid& iid)
{
    return Utils::QueryInterface<IAsrSwigSettings>(this, iid);
}

AsrRetReadOnlyString IAsrSwigSettingsImpl::GetString(
    const AsrReadOnlyString key)
{
    return impl_.GetString(u8_type_name_, key);
}
AsrRetBool IAsrSwigSettingsImpl::GetBool(const AsrReadOnlyString key)
{
    return impl_.GetBool(u8_type_name_, key);
}
AsrRetInt IAsrSwigSettingsImpl::GetInt(const AsrReadOnlyString key)
{
    return impl_.GetInt(u8_type_name_, key);
}

AsrRetFloat IAsrSwigSettingsImpl::GetFloat(const AsrReadOnlyString key)
{
    return impl_.GetFloat(u8_type_name_, key);
}

IAsrSettingsForUiImpl::IAsrSettingsForUiImpl(AsrSettings& impl) : impl_{impl} {}

int64_t IAsrSettingsForUiImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSettingsForUiImpl::Release() { return impl_.Release(); }

ASR_IMPL IAsrSettingsForUiImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_object)
{
    return Utils::QueryInterface<IAsrSettingsForUi>(this, iid, pp_object);
}

ASR_IMPL IAsrSettingsForUiImpl::ToString(IAsrReadOnlyString** pp_out_string)
{
    return impl_.ToString(pp_out_string);
}

ASR_IMPL IAsrSettingsForUiImpl::FromString(IAsrReadOnlyString* p_in_settings)
{
    return impl_.FromString(p_in_settings);
}

ASR_IMPL IAsrSettingsForUiImpl::SaveTo(IAsrReadOnlyString* p_path)
{

}

    auto AsrSettings::GetKey(const char* p_type_name, const char* key)
        -> Utils::Expected<std::reference_wrapper<const nlohmann::json>>
{
    if (const auto global_setting_it = settings_.find(p_type_name);
        global_setting_it != settings_.end())
    {
        if (const auto setting_it = global_setting_it->find(key);
            setting_it != global_setting_it->end())
        {
            return std::cref(*setting_it);
        }
    }
    if (const auto global_setting_it = default_values_.find(p_type_name);
        global_setting_it != default_values_.end())
    {
        if (const auto setting_it = global_setting_it->find(key);
            setting_it != global_setting_it->end())
        {
            return std::cref(*setting_it);
        }
    }
    return tl::make_unexpected(ASR_E_OUT_OF_RANGE);
}

auto AsrSettings::ToString(IAsrReadOnlyString* p_string)
    -> Utils::Expected<const char*>
{
    const char* p_u8_string;
    if (const auto get_u8_string_result = p_string->GetUtf8(&p_u8_string);
        IsFailed(get_u8_string_result)) [[unlikely]]
    {
        ASR_CORE_LOG_ERROR(
            "GetUtf8 failed with error code = {}.",
            get_u8_string_result);
        return tl::make_unexpected(get_u8_string_result);
    }
    return p_u8_string;
}

AsrSettings::AsrSettings(IAsrReadOnlyString* p_path)
{
    if (p_path == nullptr) [[unlikely]]
    {
        ASR_CORE_LOG_ERROR("Null pointer found! Variable name is p_path."
                           " Please check your code.");
        AsrException::Throw(ASR_E_INVALID_POINTER);
    }

#ifdef ASR_WINDOWS
    const wchar_t* w_path;
    if (const auto get_result = p_path->GetW(&w_path); IsFailed(get_result))
    {
        AsrException::Throw(get_result);
    }
    std::filesystem::path path{w_path};
#else
    const char* u8_path;
    if (const auto get_result = p_path->GetUtf8(&u8_path); IsFailed(get_result))
    {
        AsrException::Throw(get_result);
    }
    std::filesystem::path path{u8_path};
#endif // ASR_WINDOWS

    std::ifstream ifs;
    Utils::EnableStreamException(
        ifs,
        std::ios::badbit | std::ios::failbit,
        [&path](auto& stream) { stream.open(path); });
    settings_ = nlohmann::json::parse(ifs);
}

int64_t AsrSettings::AddRef() { return 1; }

int64_t AsrSettings::Release() { return 1; }

AsrRetReadOnlyString AsrSettings::GetString(
    std::string_view        u8_type_string,
    const AsrReadOnlyString key)
{
    std::lock_guard guard{mutex_};

    AsrRetReadOnlyString result{};
    ToString(key.Get())
        .and_then(
            [this, p_type_string = u8_type_string.data()](const char* p_key)
            { return GetKey(p_type_string, p_key); })
        .map(
            [&result](std::reference_wrapper<const nlohmann::json> json)
            {
                try
                {
                    const auto& string_result =
                        json.get().get_ref<const std::string&>();
                    const auto p_result = new AsrStringCppImpl{};
                    if (const auto set_u8_result =
                            p_result->SetUtf8(string_result.c_str());
                        IsFailed(set_u8_result))
                    {
                        result.error_code = set_u8_result;
                        return;
                    }
                    result.value = AsrReadOnlyString{p_result};
                    result.error_code = ASR_S_OK;
                }
                catch (const std::bad_alloc&)
                {
                    result.error_code = ASR_E_OUT_OF_MEMORY;
                    return;
                }
                catch (const nlohmann::json::exception& ex)
                {
                    ASR_CORE_LOG_EXCEPTION(ex);
                    result.error_code = ASR_E_INVALID_JSON;
                    return;
                }
            })
        .or_else([&result](AsrResult error_code)
                 { result.error_code = error_code; });
    return result;
}

AsrRetBool AsrSettings::GetBool(
    std::string_view        u8_type_string,
    const AsrReadOnlyString key)
{
    std::lock_guard guard{mutex_};
    AsrRetBool      result{};
    ToString(key.Get())
        .and_then(
            [this, p_type_string = u8_type_string.data()](const char* p_key)
            { return GetKey(p_type_string, p_key); })
        .map(
            [&result](std::reference_wrapper<const nlohmann::json> json)
            {
                try
                {
                    json.get().get_to(result.value);
                    result.error_code = ASR_S_OK;
                }
                catch (const nlohmann::json::exception& ex)
                {
                    ASR_CORE_LOG_EXCEPTION(ex);
                    result.error_code = ASR_E_INVALID_JSON;
                    return;
                }
            })
        .or_else([&result](AsrResult error_code)
                 { result.error_code = error_code; });
    return result;
}

AsrRetInt AsrSettings::GetInt(
    std::string_view        u8_type_string,
    const AsrReadOnlyString key)
{
    std::lock_guard guard{mutex_};
    AsrRetInt       result{};
    ToString(key.Get())
        .and_then(
            [this, p_type_string = u8_type_string.data()](const char* p_key)
            { return GetKey(p_type_string, p_key); })
        .map(
            [&result](std::reference_wrapper<const nlohmann::json> json)
            {
                try
                {
                    json.get().get_to(result.value);
                    result.error_code = ASR_S_OK;
                }
                catch (const nlohmann::json::exception& ex)
                {
                    ASR_CORE_LOG_EXCEPTION(ex);
                    result.error_code = ASR_E_INVALID_JSON;
                    return;
                }
            })
        .or_else([&result](AsrResult error_code)
                 { result.error_code = error_code; });
    return result;
}

AsrRetFloat AsrSettings::GetFloat(
    std::string_view        u8_type_string,
    const AsrReadOnlyString key)
{
    std::lock_guard guard{mutex_};
    AsrRetFloat     result{};
    ToString(key.Get())
        .and_then(
            [this, p_type_string = u8_type_string.data()](const char* p_key)
            { return GetKey(p_type_string, p_key); })
        .map(
            [&result](std::reference_wrapper<const nlohmann::json> json)
            {
                try
                {
                    json.get().get_to(result.value);
                    result.error_code = ASR_S_OK;
                }
                catch (const nlohmann::json::exception& ex)
                {
                    ASR_CORE_LOG_EXCEPTION(ex);
                    result.error_code = ASR_E_INVALID_JSON;
                    return;
                }
            })
        .or_else([&result](AsrResult error_code)
                 { result.error_code = error_code; });
    return result;
}

AsrResult AsrSettings::ToString(IAsrReadOnlyString** pp_out_string)
{
    ASR_UTILS_CHECK_POINTER(pp_out_string)

    std::lock_guard lock{mutex_};

    try
    {
        auto       json_string = settings_.dump();
        const auto p_result = MakeAsrPtr<AsrStringCppImpl>();
        const auto set_utf_8_result = p_result->SetUtf8(json_string.data());
        if (IsFailed(set_utf_8_result))
        {
            return set_utf_8_result;
        }
        *pp_out_string = p_result.Get();
        p_result->AddRef();
        return set_utf_8_result;
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrResult AsrSettings::FromString(IAsrReadOnlyString* p_in_settings)
{
    ASR_UTILS_CHECK_POINTER(p_in_settings)

    std::lock_guard lock{mutex_};

    try
    {
        const char* p_u8_string{};
        if (const auto get_u8_result = p_in_settings->GetUtf8(&p_u8_string);
            IsFailed(get_u8_result))
        {
            ASR_CORE_LOG_ERROR(
                "Can not get utf8 string from pointer {}.",
                Utils::VoidP(p_in_settings));
            return get_u8_result;
        }
        auto tmp_result = nlohmann::json::parse(p_u8_string);
        settings_ = std::move(tmp_result);
        return ASR_S_OK;
    }
    catch (const nlohmann::json::exception& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        return ASR_E_INTERNAL_FATAL_ERROR;
    }
}

AsrResult AsrSettings::SetDefaultValues(nlohmann::json&& rv_json)
{
    std::lock_guard lock{mutex_};

    default_values_ = std::move(rv_json);

    return ASR_S_OK;
}

ASR_CORE_SETTINGSMANAGER_NS_END

