#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/IAsrBase.h>
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

AsrSwigBaseWrapper::AsrSwigBaseWrapper() = default;

AsrSwigBaseWrapper::AsrSwigBaseWrapper(const AsrSwigBaseWrapper& other)
    : p_object_{other}
{
    if (p_object_)
    {
        reinterpret_cast<IAsrSwigBase*>(p_object_)->AddRef();
    }
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper(AsrSwigBaseWrapper&& other) noexcept
    : p_object_{std::exchange(other.p_object_, nullptr)}
{
}

AsrSwigBaseWrapper& AsrSwigBaseWrapper::operator=(
    const AsrSwigBaseWrapper& other)
{
    return *this = AsrSwigBaseWrapper{other};
}

AsrSwigBaseWrapper& AsrSwigBaseWrapper::operator=(
    AsrSwigBaseWrapper&& other) noexcept
{
    p_object_ = std::exchange(other.p_object_, nullptr);
    return *this;
}

AsrSwigBaseWrapper::~AsrSwigBaseWrapper()
{
    if (p_object_)
    {
        reinterpret_cast<IAsrSwigBase*>(p_object_)->Release();
    }
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper(void* p_object) noexcept
    : p_object_{p_object}
{
}

AsrSwigBaseWrapper::AsrSwigBaseWrapper(
    ASR_INTERFACE IAsrSwigBase* p_base) noexcept
    : p_object_{p_base}
{
}
IAsrSwigBase* AsrSwigBaseWrapper::Get() const noexcept
{
    auto* const p_result = static_cast<IAsrSwigBase*>(p_object_);
    p_result->AddRef();
    return p_result;
}

void* AsrSwigBaseWrapper::GetVoid() const noexcept { return p_object_; }

AsrSwigBaseWrapper::operator void*() const noexcept { return p_object_; }
