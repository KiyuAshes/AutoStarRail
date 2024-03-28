#ifndef ASR_CORE_LOGGER_IASRLOGREQUESTERIMPL_H
#define ASR_CORE_LOGGER_IASRLOGREQUESTERIMPL_H

#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <boost/circular_buffer.hpp>
#include <spdlog/sinks/callback_sink.h>

class IAsrLogRequesterImpl final : public IAsrLogRequester
{
    using Type = std::string;
    std::mutex                                         mutex_{};
    boost::circular_buffer<Type, std::allocator<Type>> buffer_;
    std::shared_ptr<spdlog::sinks::callback_sink_mt>   sp_sink_;
    ASR_UTILS_IASRBASE_AUTO_IMPL(IAsrLogRequesterImpl)
public:
     IAsrLogRequesterImpl(uint32_t max_buffer_size);
    ~IAsrLogRequesterImpl();
    // IAsrBase
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrLogRequester
    AsrResult RequestOne(IAsrLogReader* p_reader) override;
    // IAsrLogRequesterImpl
    auto GetSink() const noexcept -> decltype(sp_sink_);
};

#endif // ASR_CORE_LOGGER_IASRLOGREQUESTERIMPL_H
