#include "IAsrBasicErrorLensImpl.h"

#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>

ASR_CORE_UTILS_NS_BEGIN

IAsrBasicErrorLensImpl::IAsrBasicErrorLensImpl(AsrBasicErrorLensImpl& impl)
    : impl_{impl}
{
}

int64_t IAsrBasicErrorLensImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrBasicErrorLensImpl::Release() { return impl_.Release(); }

AsrResult IAsrBasicErrorLensImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_object)
{
    return Utils::QueryInterface<IAsrBasicErrorLens>(this, iid, pp_object);
}

AsrResult IAsrBasicErrorLensImpl::GetSupportedIids(
    IAsrReadOnlyGuidVector** pp_out_iids)
{
    return impl_.GetSupportedIids(pp_out_iids);
}

AsrResult IAsrBasicErrorLensImpl::GetErrorMessage(
    IAsrReadOnlyString*  locale_name,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_string)
{
    return impl_.GetErrorMessage(locale_name, error_code, pp_out_string);
}

AsrResult IAsrBasicErrorLensImpl::RegisterErrorMessage(
    IAsrReadOnlyString* locale_name,
    AsrResult           error_code,
    IAsrReadOnlyString* p_explanation)
{
    return impl_.RegisterErrorMessage(locale_name, error_code, p_explanation);
}

AsrResult IAsrBasicErrorLensImpl::GetWritableSupportedIids(
    IAsrGuidVector** pp_out_iids)
{
    return impl_.GetWritableSupportedIids(pp_out_iids);
}

IAsrSwigBasicErrorLensImpl::IAsrSwigBasicErrorLensImpl(
    AsrBasicErrorLensImpl& impl)
    : impl_{impl}
{
}

int64_t IAsrSwigBasicErrorLensImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSwigBasicErrorLensImpl::Release() { return impl_.Release(); }

AsrRetSwigBase IAsrSwigBasicErrorLensImpl::QueryInterface(const AsrGuid& iid)
{
    return Utils::QueryInterface<IAsrSwigBasicErrorLens>(this, iid);
}

AsrRetReadOnlyGuidVector IAsrSwigBasicErrorLensImpl::GetSupportedIids()
{
    return impl_.GetSupportedIids();
}

AsrRetReadOnlyString IAsrSwigBasicErrorLensImpl::GetErrorMessage(
    const AsrReadOnlyString locale_name,
    AsrResult               error_code)
{
    AsrRetReadOnlyString       result{};
    AsrPtr<IAsrReadOnlyString> p_result{};
    result.error_code =
        impl_.GetErrorMessage(locale_name.Get(), error_code, p_result.Put());
    result.value = std::move(p_result);
    return result;
}

AsrResult IAsrSwigBasicErrorLensImpl::RegisterErrorMessage(
    AsrReadOnlyString locale_name,
    AsrResult         error_code,
    AsrReadOnlyString error_message)
{
    return impl_.RegisterErrorMessage(
        locale_name.Get(),
        error_code,
        error_message.Get());
}

AsrRetGuidVector IAsrSwigBasicErrorLensImpl::GetWritableSupportedIids()
{
    return impl_.GetWritableSupportedIids();
}

int64_t AsrBasicErrorLensImpl::AddRef() { return ref_counter_.AddRef(); }

int64_t AsrBasicErrorLensImpl::Release() { return ref_counter_.Release(this); }

AsrResult AsrBasicErrorLensImpl::GetSupportedIids(
    IAsrReadOnlyGuidVector** pp_out_iids)
{
    ASR_UTILS_CHECK_POINTER(pp_out_iids)

    *pp_out_iids = suppored_guid_vector_;

    return ASR_S_OK;
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

AsrResult FindErrorMessage(
    const std::unordered_map<AsrResult, AsrPtr<IAsrReadOnlyString>>&
                         error_message_map,
    AsrResult            error_code,
    IAsrReadOnlyString*& p_out_string)
{
    if (const auto error_message_it = error_message_map.find(error_code);
        error_message_it != error_message_map.end())
    {
        p_out_string = error_message_it->second.Get();
        p_out_string->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult AsrBasicErrorLensImpl::GetErrorMessage(
    IAsrReadOnlyString*  locale_name,
    AsrResult            error_code,
    IAsrReadOnlyString** out_string)
{
    ASR_UTILS_CHECK_POINTER(out_string)

    AsrPtr locale_name_holder{locale_name};

    const auto locale_it = map_.find(locale_name_holder);
    if (locale_it != map_.end())
    {
        return Details::FindErrorMessage(
            locale_it->second,
            error_code,
            *out_string);
    }
    for (const auto& [loacle_name, error_message_map] : map_)
    {
        const auto fem_result = Details::FindErrorMessage(
            error_message_map,
            error_code,
            *out_string);

        if (fem_result == ASR_E_OUT_OF_RANGE)
        {
            continue;
        }

        return fem_result;
    }
    return ASR_S_OK;
}

AsrResult AsrBasicErrorLensImpl::RegisterErrorMessage(
    IAsrReadOnlyString* locale_name,
    AsrResult           error_code,
    IAsrReadOnlyString* p_error_message)
{
    AsrPtr locale_name_holder{locale_name};
    try
    {
        AsrPtr error_message_holder = {p_error_message};
        map_[std::move(locale_name_holder)][error_code] = error_message_holder;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
    return ASR_S_OK;
}

AsrRetReadOnlyGuidVector AsrBasicErrorLensImpl::GetSupportedIids()
{
    return {ASR_S_OK, suppored_guid_vector_};
}

AsrResult AsrBasicErrorLensImpl::GetWritableSupportedIids(
    IAsrGuidVector** pp_out_iids)
{
    ASR_UTILS_CHECK_POINTER(pp_out_iids)

    *pp_out_iids = suppored_guid_vector_;

    return ASR_S_OK;
}

AsrRetGuidVector AsrBasicErrorLensImpl::GetWritableSupportedIids()
{
    return {ASR_S_OK, suppored_guid_vector_};
}

ASR_CORE_UTILS_NS_END

AsrResult CreateIAsrBasicErrorLens(IAsrBasicErrorLens** pp_out_error_lens)
{
    ASR_UTILS_CHECK_POINTER(pp_out_error_lens)

    try
    {
        const auto p_result =
            Asr::MakeAsrPtr<ASR::Core::Utils::AsrBasicErrorLensImpl>();
        *pp_out_error_lens = *p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrRetBasicErrorLens CreateIAsrSwigBasicErrorLens()
{
    try
    {
        const auto p_result =
            Asr::MakeAsrPtr<ASR::Core::Utils::AsrBasicErrorLensImpl>();

        p_result->AddRef();
        return {ASR_S_OK, *p_result};
    }
    catch (const std::bad_alloc&)
    {
        return {ASR_E_OUT_OF_MEMORY, nullptr};
    }
}
