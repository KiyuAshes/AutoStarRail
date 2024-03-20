#include "IAsrLogRequesterImpl.h"
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>

IAsrLogRequesterImpl::IAsrLogRequesterImpl(uint32_t max_buffer_size)
    : buffer_{max_buffer_size},
      sp_sink_{std::make_shared<spdlog::sinks::callback_sink_mt>(
          [this](const spdlog::details::log_msg& message)
          {
              const auto&     payload = message.payload;
              std::lock_guard guard{mutex_};
              buffer_.push_back({ASR_FULL_RANGE_OF(payload)});
          })}
{
    ASR_CORE_LOG_INFO(
        "Initialize IAsrLogRequesterImpl successfully! This = {}. max_buffer_size = {}.",
        ASR::Utils::VoidP(this),
        max_buffer_size);
}

IAsrLogRequesterImpl::~IAsrLogRequesterImpl()
{
    auto& sinks = ASR::Core::g_logger->sinks();
    std::erase(sinks, sp_sink_);
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
    const auto  result = p_reader->ReadOne(message.c_str());
    buffer_.pop_front();

    return result;
}

auto IAsrLogRequesterImpl::GetSink() const noexcept -> decltype(sp_sink_)
{
    return sp_sink_;
}

AsrResult CreateIAsrLogRequester(
    uint32_t           max_line_count,
    IAsrLogRequester** pp_out_requester)
{
    ASR_UTILS_CHECK_POINTER(pp_out_requester)

    try
    {
        const auto p_result = new IAsrLogRequesterImpl{max_line_count};
        auto&      sinks = ASR::Core::g_logger->sinks();
        sinks.emplace_back(p_result->GetSink());
        *pp_out_requester = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}
