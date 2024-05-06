#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrContextImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T>
auto GetFrom(const nlohmann::json& json, const char* key) -> Utils::Expected<T>
{
    try
    {
        T value{};
        json[key].get_to(value);
        return value;
    }
    catch (const nlohmann::json::exception& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        ASR_CORE_LOG_INFO(
            "Get value from json failed. "
            "NOTE: key = {}. Context = {}.",
            key,
            json.dump());
        return tl::make_unexpected(ASR_E_TYPE_ERROR);
    }
}

template <class T>
auto SetTo(nlohmann::json& json, const char* key, const T& value)
    -> Utils::Expected<void>
{
    try
    {
        json[key] = value;
        return {};
    }
    catch (const nlohmann::json::exception& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        ASR_CORE_LOG_INFO(
            "Set value to json failed. "
            "NOTE: key = {}. Context = {}.",
            key,
            json.dump());
        return tl::make_unexpected(ASR_E_INTERNAL_FATAL_ERROR);
    }
    catch (const std::bad_alloc&)
    {
        return tl::make_unexpected(ASR_E_OUT_OF_MEMORY);
    }
}

ASR_NS_ANONYMOUS_DETAILS_END

IAsrContextImpl::IAsrContextImpl(AsrContextImpl& impl) : impl_{impl} {}

int64_t IAsrContextImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrContextImpl::Release() { return impl_.Release(); }

AsrResult IAsrContextImpl::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    const auto swig_result = impl_.QueryInterface(iid);
    if (IsFailed(swig_result.error_code))
    {
        return impl_.QueryInterface(iid, pp_object);
    }

    ASR_UTILS_CHECK_POINTER(pp_object);
    *pp_object = swig_result.value;
    return swig_result.error_code;
}

AsrResult IAsrContextImpl::GetString(
    IAsrReadOnlyString*  key,
    IAsrReadOnlyString** pp_out_string)
{
    ASR_UTILS_CHECK_POINTER(key)
    ASR_UTILS_CHECK_POINTER(pp_out_string)

    const auto result = impl_.GetString(key);
    if (IsFailed(result.error_code))
    {
        return result.error_code;
    }
    *pp_out_string = result.value.Get();
    return result.error_code;
}

AsrResult IAsrContextImpl::GetBool(IAsrReadOnlyString* key, bool* p_out_bool)
{
    ASR_UTILS_CHECK_POINTER(key)
    ASR_UTILS_CHECK_POINTER(p_out_bool)

    const auto result = impl_.GetBool(key);
    if (IsFailed(result.error_code))
    {
        return result.error_code;
    }
    *p_out_bool = result.value;
    return result.error_code;
}

AsrResult IAsrContextImpl::GetInt(IAsrReadOnlyString* key, int64_t* p_out_int)
{
    ASR_UTILS_CHECK_POINTER(key)
    ASR_UTILS_CHECK_POINTER(p_out_int)

    const auto result = impl_.GetInt(key);
    if (IsFailed(result.error_code))
    {
        return result.error_code;
    }
    *p_out_int = result.value;
    return result.error_code;
}

AsrResult IAsrContextImpl::GetFloat(IAsrReadOnlyString* key, float* p_out_float)
{
    ASR_UTILS_CHECK_POINTER(key)
    ASR_UTILS_CHECK_POINTER(p_out_float)

    const auto result = impl_.GetFloat(key);
    if (IsFailed(result.error_code))
    {
        return result.error_code;
    }
    *p_out_float = result.value;
    return result.error_code;
}

AsrResult IAsrContextImpl::SetString(
    IAsrReadOnlyString* key,
    IAsrReadOnlyString* value)
{
    return impl_.SetString(key, value);
}

AsrResult IAsrContextImpl::SetBool(IAsrReadOnlyString* key, bool value)
{
    return impl_.SetBool(key, value);
}

AsrResult IAsrContextImpl::SetInt(IAsrReadOnlyString* key, int64_t value)
{
    return impl_.SetInt(key, value);
}

AsrResult IAsrContextImpl::SetFloat(IAsrReadOnlyString* key, float value)
{
    return impl_.SetFloat(key, value);
}

IAsrSwigContextImpl::IAsrSwigContextImpl(AsrContextImpl& impl) : impl_{impl} {}

int64_t IAsrSwigContextImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSwigContextImpl::Release() { return impl_.Release(); }

AsrRetSwigBase IAsrSwigContextImpl::QueryInterface(const AsrGuid& iid)
{
    void*      p_result;
    const auto cpp_result = impl_.QueryInterface(iid, &p_result);
    if (IsFailed(cpp_result))
    {
        return QueryInterface(iid);
    }

    return {cpp_result, AsrSwigBaseWrapper{p_result}};
}

AsrRetReadOnlyString IAsrSwigContextImpl::GetString(AsrReadOnlyString key)
{
    return impl_.GetString(key);
}

AsrRetBool IAsrSwigContextImpl::GetBool(AsrReadOnlyString key)
{
    return impl_.GetBool(key);
}

AsrRetInt IAsrSwigContextImpl::GetInt(AsrReadOnlyString key)
{
    return impl_.GetInt(key);
}

AsrRetFloat IAsrSwigContextImpl::GetFloat(AsrReadOnlyString key)
{
    return impl_.GetFloat(key);
}

AsrResult IAsrSwigContextImpl::SetString(
    AsrReadOnlyString key,
    AsrReadOnlyString value)
{
    return impl_.SetString(key, value);
}

AsrResult IAsrSwigContextImpl::SetBool(AsrReadOnlyString key, bool value)
{
    return impl_.SetBool(key, value);
}

AsrResult IAsrSwigContextImpl::SetInt(AsrReadOnlyString key, int64_t value)
{
    return impl_.SetInt(key, value);
}

AsrResult IAsrSwigContextImpl::SetFloat(AsrReadOnlyString key, float value)
{
    return impl_.SetFloat(key, value);
}

int64_t AsrContextImpl::AddRef() { return ref_counter_.AddRef(); }

int64_t AsrContextImpl::Release() { return ref_counter_.Release(this); }

AsrResult AsrContextImpl::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    const auto p_this = static_cast<IAsrContextImpl*>(*this);
    return Utils::QueryInterface<IAsrContext>(p_this, iid, pp_object);
}

AsrRetSwigBase AsrContextImpl::QueryInterface(const AsrGuid& iid)
{
    const auto p_this = static_cast<IAsrSwigContextImpl*>(*this);
    return Utils::QueryInterface<IAsrSwigContext>(p_this, iid);
}

AsrRetReadOnlyString AsrContextImpl::GetString(AsrReadOnlyString key)
{
    const auto exptected_result =
        Utils::ToU8StringWithoutOwnership(key.Get())
            .and_then(
                [this](const char* p_u8_key)
                { return Details::GetFrom<std::string>(context_, p_u8_key); })
            .and_then(
                [](const std::string_view result_string)
                    -> Utils::Expected<AsrPtr<IAsrReadOnlyString>>
                {
                    AsrPtr<IAsrReadOnlyString> p_result{};
                    if (const auto create_result =
                            ::CreateIAsrReadOnlyStringFromUtf8(
                                result_string.data(),
                                p_result.Put());
                        IsFailed(create_result))
                    {
                        ASR_CORE_LOG_ERROR(
                            "Call CreateIAsrReadOnlyStringFromUtf8 failed."
                            "Error code = {}.",
                            create_result);
                        return tl::make_unexpected(create_result);
                    }

                    return p_result;
                });
    return {
        Utils::GetResult(exptected_result),
        exptected_result
        ? exptected_result.value().Get()->AddRef(),
        exptected_result.value().Get()
        : AsrReadOnlyString{}};
}

AsrRetBool AsrContextImpl::GetBool(AsrReadOnlyString key)
{
    const auto exptected_result =
        Utils::ToU8StringWithoutOwnership(key.Get()).and_then(
            [this](const char* p_u8_key)
            { return Details::GetFrom<bool>(context_, p_u8_key); });
    return {
        Utils::GetResult(exptected_result),
        exptected_result.value_or(bool{})};
}

AsrRetInt AsrContextImpl::GetInt(AsrReadOnlyString key)
{
    const auto exptected_result =
        Utils::ToU8StringWithoutOwnership(key.Get()).and_then(
            [this](const char* p_u8_key)
            { return Details::GetFrom<std::int64_t>(context_, p_u8_key); });
    return {
        Utils::GetResult(exptected_result),
        exptected_result.value_or(std::int64_t{})};
}

AsrRetFloat AsrContextImpl::GetFloat(AsrReadOnlyString key)
{
    const auto exptected_result =
        Utils::ToU8StringWithoutOwnership(key.Get()).and_then(
            [this](const char* p_u8_key)
            { return Details::GetFrom<float>(context_, p_u8_key); });
    return {
        Utils::GetResult(exptected_result),
        exptected_result.value_or(float{})};
}

AsrResult AsrContextImpl::SetString(
    AsrReadOnlyString key,
    AsrReadOnlyString value)
{
    const auto expected_key = Utils::ToU8StringWithoutOwnership(key.Get());
    if (!expected_key)
    {
        return expected_key.error();
    }

    const auto expected_value = Utils::ToU8StringWithoutOwnership(value.Get());
    if (!expected_value)
    {
        return expected_value.error();
    }

    const auto expected_result =
        Details::SetTo(context_, expected_key.value(), expected_value.value());

    return Utils::GetResult(expected_result);
}

AsrResult AsrContextImpl::SetBool(AsrReadOnlyString key, bool value)
{
    const auto expected_result =
        Utils::ToU8StringWithoutOwnership(key.Get()).and_then(
            [this, value](const char* p_u8_key)
            { return Details::SetTo(context_, p_u8_key, value); });

    return Utils::GetResult(expected_result);
}

AsrResult AsrContextImpl::SetInt(AsrReadOnlyString key, int64_t value)
{
    const auto expected_result =
        Utils::ToU8StringWithoutOwnership(key.Get()).and_then(
            [this, value](const char* p_u8_key)
            { return Details::SetTo(context_, p_u8_key, value); });

    return Utils::GetResult(expected_result);
}

AsrResult AsrContextImpl::SetFloat(AsrReadOnlyString key, float value)
{
    const auto expected_result =
        Utils::ToU8StringWithoutOwnership(key.Get()).and_then(
            [this, value](const char* p_u8_key)
            { return Details::SetTo(context_, p_u8_key, value); });

    return Utils::GetResult(expected_result);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

AsrResult CreateIAsrContext(IAsrContext** pp_out_context)
{
    ASR_UTILS_CHECK_POINTER(pp_out_context)

    try
    {
        const auto p_result =
            new Asr::Core::ForeignInterfaceHost::AsrContextImpl{};
        *pp_out_context = *p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrResult CreateIAsrSwigContext(IAsrSwigContext** pp_out_context)
{
    ASR_UTILS_CHECK_POINTER(pp_out_context)

    try
    {
        const auto p_result =
            new Asr::Core::ForeignInterfaceHost::AsrContextImpl{};
        *pp_out_context = *p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}
