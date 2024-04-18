#ifndef ASR_CORE_LOGGER_IASRLOGREQUESTERIMPL_H
#define ASR_CORE_LOGGER_IASRLOGREQUESTERIMPL_H

#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/fmt.h>
#include <boost/circular_buffer.hpp>
#include <mutex>
#include <spdlog/sinks/base_sink.h>

template <typename Mutex>
class AsrLogRequesterSink;

class IAsrLogRequesterImpl final : public IAsrLogRequester
{
    using Type = std::shared_ptr<std::string>;
    using SpLogRequesterSink = std::shared_ptr<AsrLogRequesterSink<std::mutex>>;
    std::mutex                                         mutex_{};
    boost::circular_buffer<Type, std::allocator<Type>> buffer_;
    SpLogRequesterSink                                 sp_log_requester_sink_;
    ASR_UTILS_IASRBASE_AUTO_IMPL(IAsrLogRequesterImpl)
public:
    IAsrLogRequesterImpl(uint32_t max_buffer_size, SpLogRequesterSink sp_sink);
    ~IAsrLogRequesterImpl();
    // IAsrBase
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrLogRequester
    AsrResult RequestOne(IAsrLogReader* p_reader) override;
    // IAsrLogRequesterImpl
    void Accept(std::shared_ptr<std::string> sp_message);
};

template <typename Mutex>
class AsrLogRequesterSink final : public spdlog::sinks::base_sink<Mutex>
{
private:
    Mutex                                          mutex_;
    std::vector<ASR::AsrPtr<IAsrLogRequesterImpl>> logger_requester_vector_ =
        ASR::Utils::MakeEmptyCOntainerOfReservedSize<
            decltype(logger_requester_vector_)>(5);

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {

        // log_msg is a struct containing the log entry info like level,
        // timestamp, thread id etc. msg.raw contains pre formatted log

        // If needed (very likely but not mandatory), the sink formats
        // the message before sending it to its final destination:
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        auto       message = ASR::FmtCommon::to_string(formatted);
        const auto sp_message =
            std::make_shared<std::string>(std::move(message));
        std::lock_guard guard{mutex_};
        for (const auto& p_requester : logger_requester_vector_)
        {
            p_requester->Accept(sp_message);
        }
    }

    void flush_() override
    {
        // ! 不存在需要flush_的地方
    }

public:
    void Remove(IAsrLogRequesterImpl* p_requester)
    {
        std::lock_guard lock_guard{mutex_};
        std::erase(logger_requester_vector_, p_requester);
    }

    void Add(IAsrLogRequesterImpl* p_requester)
    {
        std::lock_guard lock_guard{mutex_};
        logger_requester_vector_.emplace_back(p_requester);
    }
};

extern std::shared_ptr<AsrLogRequesterSink<std::mutex>>
    g_asr_log_requester_sink;

#endif // ASR_CORE_LOGGER_IASRLOGREQUESTERIMPL_H
