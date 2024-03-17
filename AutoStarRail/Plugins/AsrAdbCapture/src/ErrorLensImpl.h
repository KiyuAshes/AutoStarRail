#ifndef ASR_PLUGINS_ASRADBCAPTURE_ERRORLENSIMPL_H
#define ASR_PLUGINS_ASRADBCAPTURE_ERRORLENSIMPL_H

#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/StringUtils.h>
#include <string>
#include <unordered_map>

ASR_NS_BEGIN

constexpr AsrResult CAPTURE_DATA_TOO_LESS = -1;
constexpr AsrResult UNSUPPORTED_COLOR_FORMAT = -2;

class AdbCaptureErrorLens final : public IAsrErrorLens
{
    using ErrorCodeMap =
        std::unordered_map<AsrResult, AsrPtr<IAsrReadOnlyString>>;
    using LocaleErrorCodeMap = std::unordered_map<
        AsrPtr<IAsrReadOnlyString>,
        ErrorCodeMap,
        Utils::AsrReadOnlyStringHash>;
    LocaleErrorCodeMap                          map_;
    ASR::Utils::RefCounter<AdbCaptureErrorLens> ref_counter_;
    std::vector<AsrGuid>                        iids_;

    static AsrPtr<IAsrReadOnlyString> p_default_locale_name;
    static std::string (*error_code_not_found_explanation_generator)(
        AsrResult,
        IAsrReadOnlyString**);

public:
    AdbCaptureErrorLens();
    ~AdbCaptureErrorLens();
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrErrorLens
    AsrResult GetSupportedIids(IAsrReadOnlyGuidVector** pp_out_iids) override;
    AsrResult GetErrorMessage(
        IAsrReadOnlyString*  locale_name,
        AsrResult            error_code,
        IAsrReadOnlyString** out_string) override;
    AsrResult RegisterErrorCode(
        const AsrResult            error_code,
        AsrPtr<IAsrReadOnlyString> locale_name,
        AsrPtr<IAsrReadOnlyString> p_explanation);
    AsrResult AddSupportedIid(const AsrGuid& iid);
};

ASR_NS_END

#endif // ASR_PLUGINS_ASRADBCAPTURE_ERRORLENSIMPL_H