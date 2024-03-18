#ifndef ASR_CORE_UTILS_IASRBASICERRORLENSIMPL_H
#define ASR_CORE_UTILS_IASRBASICERRORLENSIMPL_H

#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrGuidVectorImpl.h>
#include <AutoStarRail/Core/Utils/Config.h>
#include <AutoStarRail/ExportInterface/IAsrBasicErrorLens.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/StringUtils.h>

ASR_CORE_UTILS_NS_BEGIN

class AsrBasicErrorLensImpl;

class IAsrBasicErrorLensImpl : public IAsrBasicErrorLens
{
    AsrBasicErrorLensImpl& impl_;

public:
    IAsrBasicErrorLensImpl(AsrBasicErrorLensImpl& impl);
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrErrorLens
    AsrResult GetSupportedIids(IAsrReadOnlyGuidVector** pp_out_iids) override;
    AsrResult GetErrorMessage(
        IAsrReadOnlyString*  locale_name,
        AsrResult            error_code,
        IAsrReadOnlyString** pp_out_string) override;
    // IAsrBasicErrorLens
    AsrResult RegisterErrorMessage(
        IAsrReadOnlyString* locale_name,
        AsrResult           error_code,
        IAsrReadOnlyString* p_explanation) override;
    AsrResult GetWritableSupportedIids(IAsrGuidVector** pp_out_iids) override;
};

class IAsrSwigBasicErrorLensImpl : public IAsrSwigBasicErrorLens
{
    AsrBasicErrorLensImpl& impl_;

public:
    IAsrSwigBasicErrorLensImpl(AsrBasicErrorLensImpl& impl);
    int64_t                  AddRef() override;
    int64_t                  Release() override;
    AsrRetSwigBase           QueryInterface(const AsrGuid& iid) override;
    AsrRetReadOnlyGuidVector GetSupportedIids() override;
    AsrRetReadOnlyString     GetErrorMessage(
            const AsrReadOnlyString locale_name,
            AsrResult               error_code) override;
    AsrResult RegisterErrorMessage(
        AsrReadOnlyString locale_name,
        AsrResult         error_code,
        AsrReadOnlyString error_message) override;
    AsrRetGuidVector GetWritableSupportedIids() override;
};

class AsrBasicErrorLensImpl : ASR_UTILS_MULTIPLE_PROJECTION_GENERATORS(
                                  AsrBasicErrorLensImpl,
                                  IAsrBasicErrorLensImpl,
                                  IAsrSwigBasicErrorLensImpl)
{
    using ErrorCodeMap =
        std::unordered_map<AsrResult, AsrPtr<IAsrReadOnlyString>>;
    using LocaleErrorCodeMap = std::unordered_map<
        AsrPtr<IAsrReadOnlyString>,
        ErrorCodeMap,
        AsrReadOnlyStringHash>;

    RefCounter<AsrBasicErrorLensImpl>       ref_counter_{};
    LocaleErrorCodeMap                      map_{};
    ForeignInterfaceHost::AsrGuidVectorImpl suppored_guid_vector_{};

public:
    int64_t AddRef();
    int64_t Release();

    AsrResult GetSupportedIids(IAsrReadOnlyGuidVector** pp_out_iids);
    AsrResult GetErrorMessage(
        IAsrReadOnlyString*  locale_name,
        AsrResult            error_code,
        IAsrReadOnlyString** out_string);

    AsrResult RegisterErrorMessage(
        IAsrReadOnlyString* locale_name,
        AsrResult           error_code,
        IAsrReadOnlyString* p_explanation);

    AsrRetReadOnlyGuidVector GetSupportedIids();

    AsrResult        GetWritableSupportedIids(IAsrGuidVector** pp_out_iids);
    AsrRetGuidVector GetWritableSupportedIids();
};

ASR_CORE_UTILS_NS_END

#endif // ASR_CORE_UTILS_IASRBASICERRORLENSIMPL_H
