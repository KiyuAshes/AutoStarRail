#include "PluginManager.h"
#include "AutoStarRail/Utils/StringUtils.h"
#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>
#include <AutoStarRail/ExportInterface/IAsrCaptureManager.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include "AutoStarRail/Utils/CommonUtils.hpp"
#include <utility>
#include <vector>

ASR_NS_BEGIN

class CaptureManager final : public IAsrCaptureManager
{
public:
    struct ErrorInfo
    {
        AsrPtr<IAsrReadOnlyString> p_error_message;
        int32_t                    time_spent_in_ms;
        AsrResult                  error_code;
    };

private:
    using ExpectedInstance = tl::expected<AsrPtr<IAsrCapture>, ErrorInfo>;
    struct [[nodiscard(
        "Do not acquire an instance and discard it.")]] CaptureInstance
    {
        AsrReadOnlyString name;
        ExpectedInstance  instance;
    };

    ASR::Utils::RefCounter<CaptureManager> ref_counter_;
    std::vector<CaptureInstance>           instances_;
    std::vector<ErrorInfo>                 performance_results_;

public:
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrCaptureManager
    AsrResult EnumCaptureLoadErrorState(
        const size_t         index,
        AsrResult*           p_out_error_code,
        IAsrReadOnlyString** pp_out_error_explanation) override;
    AsrResult EnumCaptureInterface(
        const size_t  index,
        IAsrCapture** pp_out_interface) override;
    AsrResult RunCapturePerformanceTest() override;
    AsrResult EnumCapturePerformanceTestResult(
        const size_t         index,
        AsrResult*           p_out_error_code,
        int32_t*             p_out_time_spent_in_ms,
        IAsrReadOnlyString** pp_out_error_explanation) override;
    // impl
    void AddInstance(
        AsrPtr<IAsrReadOnlyString> p_name,
        AsrPtr<IAsrCapture>        p_instance);
    void AddInstance(
        AsrPtr<IAsrReadOnlyString> p_name,
        const ErrorInfo&           error_info);
    /**
     * @brief Create a new capture instance with empty name.
     * @param error_info error code when getting the instance name.
     */
    void AddInstance(const ErrorInfo& error_info);
    void ReserveInstanceContainer(const std::size_t instance_count);
};

int64_t CaptureManager::AddRef() { return ref_counter_.AddRef(); }

int64_t CaptureManager::Release() { return ref_counter_.Release(this); }

AsrResult CaptureManager::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrCaptureManager>(
        this,
        iid,
        pp_out_object);
}

AsrResult CaptureManager::EnumCaptureLoadErrorState(
    const size_t         index,
    AsrResult*           p_out_error_code,
    IAsrReadOnlyString** pp_out_error_explanation)
{
    const auto size = instances_.size();
    if (index >= size)
    {
        return ASR_E_OUT_OF_RANGE;
    }
    const auto& instance = instances_[index];
    instance.instance
        .or_else(
            [p_out_error_code, pp_out_error_explanation](const auto& error_info)
            {
                if (p_out_error_code)
                {
                    *p_out_error_code = error_info.error_code;
                }
                if (pp_out_error_explanation)
                {
                    error_info.p_error_message->AddRef();
                    *pp_out_error_explanation =
                        error_info.p_error_message.Get();
                }
            })
        .map(
            [p_out_error_code, pp_out_error_explanation](const auto&)
            {
                *p_out_error_code = ASR_S_OK;
                const auto null_string = ASR::Details::CreateNullAsrString();
                null_string->AddRef();
                *pp_out_error_explanation = null_string.Get();
            });
    return ASR_S_OK;
}

AsrResult CaptureManager::EnumCaptureInterface(
    const size_t  index,
    IAsrCapture** pp_out_interface)
{
    AsrResult result{ASR_E_UNDEFINED_RETURN_VALUE};
    if (index >= instances_.size())
    {
        *pp_out_interface = nullptr;
        return ASR_E_OUT_OF_RANGE;
    }
    const auto& instance = instances_[index];
    instance.instance
        .or_else(
            [pp_out_interface, &result](const auto& error_info)
            {
                *pp_out_interface = nullptr;
                result = error_info.error_code;
            })
        .map(
            [pp_out_interface, &result](const auto& capture)
            {
                capture->AddRef();
                *pp_out_interface = capture.Get();
                result = ASR_S_OK;
            });
    return result;
}

AsrResult CaptureManager::RunCapturePerformanceTest()
{
    return ASR_E_NO_IMPLEMENTATION;
}

AsrResult CaptureManager::EnumCapturePerformanceTestResult(
    const size_t         index,
    AsrResult*           p_out_error_code,
    int32_t*             p_out_time_spent_in_ms,
    IAsrReadOnlyString** pp_out_error_explanation)
{
    ErrorInfo* p_error_info{};
    try
    {
        p_error_info = &performance_results_.at(index);
    }
    catch (const std::out_of_range& ex)
    {
        ASR_CORE_LOG_ERROR(
            "Index out of range when calling EnumCapturePerformanceTestResult. The error info size is {}. Input index is {}. Message: \"{}\".",
            performance_results_.size(),
            index,
            ex.what());
        return ASR_E_OUT_OF_RANGE;
    }
    if (p_out_error_code)
    {
        *p_out_error_code = p_error_info->error_code;
    }
    if (p_out_time_spent_in_ms)
    {
        *p_out_time_spent_in_ms = p_error_info->time_spent_in_ms;
    }
    if (pp_out_error_explanation)
    {
        *pp_out_error_explanation = p_error_info->p_error_message.Get();
        (*pp_out_error_explanation)->AddRef();
    }
    return ASR_S_OK;
}

void CaptureManager::ReserveInstanceContainer(const std::size_t instance_count)
{
    instances_.reserve(instance_count);
}

void CaptureManager::AddInstance(
    AsrPtr<IAsrReadOnlyString> p_name,
    AsrPtr<IAsrCapture>        p_instance)
{
    instances_.emplace_back(AsrReadOnlyString{std::move(p_name)}, p_instance);
}

void CaptureManager::AddInstance(
    AsrPtr<IAsrReadOnlyString>       p_name,
    const CaptureManager::ErrorInfo& error_info)
{
    instances_.emplace_back(
        AsrReadOnlyString{std::move(p_name)},
        tl::make_unexpected(error_info));
}

void CaptureManager::AddInstance(const CaptureManager::ErrorInfo& error_info)
{
    instances_.emplace_back(
        AsrReadOnlyString{},
        tl::make_unexpected(error_info));
}

ASR_NS_END

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T>
auto GetName(ASR::AsrPtr<T> p_object)
    -> ASR::Utils::Expected<ASR::AsrPtr<IAsrReadOnlyString>>
{
    ASR::AsrPtr<IAsrReadOnlyString> result{};
    const auto error_code = p_object->GetRuntimeClassName(result.Put());
    if (ASR::IsOk(error_code)) [[likely]]
    {
        return result;
    }
    return tl::make_unexpected(error_code);
}

auto LogAndGetErrorMessageWhenGetNameFailed(
    const AsrResult                        error_code,
    const ASR::AsrPtr<IAsrCaptureFactory>& p_factory)
    -> ASR::AsrPtr<IAsrReadOnlyString>
{
    ASR::AsrPtr<IAsrReadOnlyString> result{};
    ASR::AsrPtr<IAsrInspectable>    p_factory_base{};
    p_factory.As(p_factory_base);
    if (const auto get_error_message_result = ::AsrGetErrorMessage(
            p_factory_base.Get(),
            error_code,
            result.Put());
        ASR::IsOk(get_error_message_result)) [[unlikely]]
    {
        ASR::Core::i18n::GetExplanationWhenTranslateErrorFailed(
            error_code,
            get_error_message_result,
            result.Put());
    }
    return result;
}

void OnCreateCaptureInstanceFailed(
    ASR::CaptureManager::ErrorInfo&         in_error_info,
    const ASR::AsrPtr<IAsrReadOnlyString>&  p_capture_name,
    const ASR::AsrPtr<ASR::CaptureManager>& p_capture_manager)
{
    std::string                     error_message;
    ASR::AsrPtr<IAsrInspectable>    p_capture_base{};
    ASR::AsrPtr<IAsrReadOnlyString> p_error_message{};
    if (const auto get_error_message_result = ::AsrGetErrorMessage(
            p_capture_base.Get(),
            in_error_info.error_code,
            p_error_message.Put());
        ASR::IsOk(get_error_message_result))
    {
        const char* p_u8_explanation{nullptr};
        p_error_message->GetUtf8(&p_u8_explanation);
        error_message = ASR::fmt::format(
            R"(Error happened when creating capture instance. Error code: {}.\nError explanation: "{}".)",
            in_error_info.error_code,
            p_u8_explanation);
        in_error_info.p_error_message = p_error_message;
    }
    else
    {
        error_message = ASR::fmt::format(
            R"(Error happened when creating capture instance. Error code: {}.\nNo error explanation found.)",
            in_error_info.error_code);
    }
    p_capture_manager->AddInstance(p_capture_name, in_error_info);
    ASR_CORE_LOG_ERROR(error_message);
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult CreateIAsrCaptureManager(
    IAsrReadOnlyString*  p_json_config,
    IAsrCaptureManager** pp_out_capture_manager)
{
    AsrResult                        result{ASR_S_OK};
    ASR::AsrPtr<IAsrReadOnlyString>  p_locale_name{};
    ASR::AsrPtr<ASR::CaptureManager> p_capture_manager{};
    ASR::AsrPtr<IAsrCaptureManager>  p_result{};
    try
    {
        p_capture_manager = {new ASR::CaptureManager(), ASR::take_ownership};
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
    ::AsrGetDefaultLocale(p_locale_name.Put());
    p_capture_manager.As(p_result);
    const auto capture_factories =
        ASR::Core::ForeignInterfaceHost::g_plugin_manager
            .GetAllCaptureFactory();
    p_capture_manager->ReserveInstanceContainer(capture_factories.size());
    for (const auto& p_factory : capture_factories)
    {
        ASR::CaptureManager::ErrorInfo  error_info{};
        ASR::AsrPtr<IAsrReadOnlyString> p_capture_name{};
        Details::GetName(p_factory)
            .or_else(
                [&result, &error_info, &p_capture_manager, &p_factory](
                    const auto error_code)
                {
                    result = ASR_S_FALSE;
                    error_info.error_code = error_code;
                    error_info.p_error_message =
                        Details::LogAndGetErrorMessageWhenGetNameFailed(
                            error_code,
                            p_factory);
                    p_capture_manager->AddInstance(error_info);
                })
            // try to create instance
            .and_then(
                [&p_capture_name, &p_factory, &p_json_config](
                    const auto& p_capture_factory_name)
                    -> ASR::Utils::Expected<ASR::AsrPtr<IAsrCapture>>
                {
                    p_capture_name = p_capture_factory_name;
                    ASR::AsrPtr<IAsrCapture> p_instance{};

                    if (const auto error_code = p_factory->CreateInstance(
                            p_json_config,
                            p_instance.Put());
                        ASR::IsOk(error_code)) [[likely]]
                    {
                        return p_instance;
                    }
                    else
                    {
                        return tl::make_unexpected(error_code);
                    }
                })
            .or_else(
                [&error_info, &p_capture_name, &p_capture_manager, &result](
                    const auto error_code)
                {
                    result = ASR_S_FALSE;
                    error_info.error_code = error_code;
                    Details::OnCreateCaptureInstanceFailed(
                        error_info,
                        p_capture_name,
                        p_capture_manager);
                })
            // add instance to capture manager
            .map(
                [&p_capture_manager, &p_capture_name](const auto& p_instance) {
                    p_capture_manager->AddInstance(p_capture_name, p_instance);
                });
    }
    p_result->AddRef();
    *pp_out_capture_manager = p_result.Get();
    return result;
}
