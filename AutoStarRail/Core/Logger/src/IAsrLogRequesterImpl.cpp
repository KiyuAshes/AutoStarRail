#include "IAsrLogRequesterImpl.h"
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/QueryInterface.hpp>

IAsrLogRequesterImpl::IAsrLogRequesterImpl(
    uint32_t           max_buffer_size,
    SpLogRequesterSink sp_sink)
    : buffer_{max_buffer_size}, sp_log_requester_sink_{sp_sink}
{
    ASR_CORE_LOG_INFO(
        "Initialize IAsrLogRequesterImpl successfully! This = {}. max_buffer_size = {}.",
        ASR::Utils::VoidP(this),
        max_buffer_size);
}

IAsrLogRequesterImpl::~IAsrLogRequesterImpl()
{
    sp_log_requester_sink_->Remove(this);
}

AsrResult IAsrLogRequesterImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_object)
{
    return ASR::Utils::QueryInterface<IAsrLogRequester>(this, iid, pp_object);
}

AsrResult IAsrLogRequesterImpl::RequestOne(IAsrLogReader* p_reader)
{
    std::lock_guard guard{mutex_};

    const auto& message = buffer_.front();
    const auto  result = p_reader->ReadOne(message->c_str());
    buffer_.pop_front();

    return result;
}

void IAsrLogRequesterImpl::Accept(std::shared_ptr<std::string> sp_message)
{
    buffer_.push_back(std::move(sp_message));
}

AsrResult CreateIAsrLogRequester(
    uint32_t           max_line_count,
    IAsrLogRequester** pp_out_requester)
{
    ASR_UTILS_CHECK_POINTER(pp_out_requester)

    try
    {
        const auto p_result =
            new IAsrLogRequesterImpl{max_line_count, g_asr_log_requester_sink};
        *pp_out_requester = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

ASR_DEFINE_VARIABLE(g_asr_log_requester_sink);
