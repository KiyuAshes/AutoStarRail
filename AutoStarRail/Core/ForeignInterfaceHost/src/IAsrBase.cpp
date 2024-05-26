#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <cstring>


AsrRetGuid AsrMakeAsrGuid(const char* p_guid_string)
{
    ASR_CORE_TRACE_SCOPE;

    AsrRetGuid result;
    try
    {
        result.value =
            ASR::Core::ForeignInterfaceHost::MakeAsrGuid(p_guid_string);
        result.error_code = ASR_S_OK;
    }
    catch (const ASR::Core::InvalidGuidStringException& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        result = {ASR_E_INVALID_STRING, ASR_IID_BASE};
    }
    catch (const ASR::Core::InvalidGuidStringSizeException& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        result = {ASR_E_INVALID_STRING_SIZE, ASR_IID_BASE};
    }
    return result;
}

AsrResult AsrMakeAsrGuid(const char* p_guid_string, AsrGuid* p_out_guid)
{
    ASR_UTILS_CHECK_POINTER(p_out_guid)

    const auto result = AsrMakeAsrGuid(p_guid_string);
    if (result.error_code == ASR_S_OK)
    {
        *p_out_guid = result.value;
        return ASR_S_OK;
    }
    return result.error_code;
}

void AsrSwigBaseWrapper::InternalAddRef()
{
    if (p_object_)
    {
        reinterpret_cast<IAsrSwigBase*>(p_object_)->AddRef();
    }
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper() = default;

AsrSwigBaseWrapper::AsrSwigBaseWrapper(const AsrSwigBaseWrapper& other)
    : p_object_{other}
{
    ASR_CORE_TRACE_SCOPE;
    InternalAddRef();
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper(AsrSwigBaseWrapper&& other) noexcept
    : p_object_{std::exchange(other.p_object_, nullptr)}
{
    ASR_CORE_TRACE_SCOPE;
}

AsrSwigBaseWrapper& AsrSwigBaseWrapper::operator=(
    const AsrSwigBaseWrapper& other)
{
    ASR_CORE_TRACE_SCOPE;
    *this = AsrSwigBaseWrapper{other};
    InternalAddRef();
    return *this;
}

AsrSwigBaseWrapper& AsrSwigBaseWrapper::operator=(
    AsrSwigBaseWrapper&& other) noexcept
{
    ASR_CORE_TRACE_SCOPE;
    p_object_ = std::exchange(other.p_object_, nullptr);
    return *this;
}

AsrSwigBaseWrapper::~AsrSwigBaseWrapper()
{
    ASR_CORE_TRACE_SCOPE;
    if (p_object_)
    {
        reinterpret_cast<IAsrSwigBase*>(p_object_)->Release();
    }
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper(void* p_object) noexcept
    : p_object_{p_object}
{
    ASR_CORE_TRACE_SCOPE;
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper(
    ASR_INTERFACE IAsrSwigBase* p_base) noexcept
    : p_object_{p_base}
{
    ASR_CORE_TRACE_SCOPE;
}

IAsrSwigBase* AsrSwigBaseWrapper::Get() const noexcept
{
    ASR_CORE_TRACE_SCOPE;
    auto* const p_result = static_cast<IAsrSwigBase*>(p_object_);
    p_result->AddRef();
    return p_result;
}

void* AsrSwigBaseWrapper::GetVoidNoAddRef() const noexcept
{
    ASR_CORE_TRACE_SCOPE;
    return p_object_;
}

AsrSwigBaseWrapper::operator void*() const noexcept { return p_object_; }
