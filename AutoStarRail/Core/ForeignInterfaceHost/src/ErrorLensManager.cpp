#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/ErrorLensManager.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <DAS/_autogen/OfficialIids.h>
#include <unordered_set>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

auto GetIidVectorSize(IAsrReadOnlyGuidVector* p_iid_vector)
    -> ASR::Utils::Expected<size_t>
{
    size_t     iid_size{};
    const auto get_iid_size_result = p_iid_vector->Size(&iid_size);
    if (!IsOk(get_iid_size_result))
    {
        AsrPtr<IAsrReadOnlyString> p_error_message{};
        ::AsrGetPredefinedErrorMessage(
            get_iid_size_result,
            p_error_message.Put());
        ASR_CORE_LOG_ERROR(
            "Error happened in class IAsrGuidVector. Pointer = {}. Error code = {}. Error message = \"{}\".",
            static_cast<void*>(p_iid_vector),
            get_iid_size_result,
            p_error_message);
        return tl::make_unexpected(get_iid_size_result);
    }
    return iid_size;
}

auto GetIidFromIidVector(IAsrReadOnlyGuidVector* p_iid_vector, size_t iid_index)
    -> ASR::Utils::Expected<AsrGuid>
{
    AsrGuid    iid{AsrIidOf<IAsrBase>()};
    const auto get_iid_result = p_iid_vector->At(iid_index, &iid);
    if (!IsOk(get_iid_result))
    {
        AsrPtr<IAsrReadOnlyString> p_error_message{};
        ::AsrGetPredefinedErrorMessage(get_iid_result, p_error_message.Put());
        ASR_CORE_LOG_ERROR(
            "Error happened in class IAsrGuidVector. Pointer = {}. Error code = {}. Error message = \"{}\".",
            static_cast<void*>(p_iid_vector),
            get_iid_result,
            p_error_message);
        return tl::make_unexpected(get_iid_result);
    }
    return iid;
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult ErrorLensManager::Register(
    IAsrReadOnlyGuidVector* p_iid_vector,
    IAsrErrorLens*          p_error_lens)
{
    const auto get_iid_size_result = Details::GetIidVectorSize(p_iid_vector);
    if (!get_iid_size_result)
    {
        return get_iid_size_result.error();
    }
    const auto iid_size = get_iid_size_result.value();
    // try to use all iids to register IAsrErrorLens instance.
    for (size_t i = 0; i < iid_size; ++i)
    {
        const auto get_iid_from_iid_vector_result =
            Details::GetIidFromIidVector(p_iid_vector, i);
        if (!get_iid_from_iid_vector_result)
        {
            if (get_iid_size_result.error() == ASR_E_OUT_OF_RANGE)
            {
                ASR_CORE_LOG_WARN(
                    "Received ASR_E_OUT_OF_RANGE when calling IAsrIidVector::At()."
                    "Pointer = {}. Size = {}. Index = {}.",
                    static_cast<void*>(p_iid_vector),
                    iid_size,
                    i);
                break;
            }
            return get_iid_from_iid_vector_result.error();
        }
        const auto& g_official_iids = Das::_autogen::g_official_iids;
        const auto& iid = get_iid_from_iid_vector_result.value();
        if (g_official_iids.find(iid) != g_official_iids.end())
        {
            if (map_.count(iid) == 1)
            {
                ASR_CORE_LOG_WARN(
                    "Trying to register duplicate IAsrErrorLens instance. Operation ignored."
                    "Pointer = {}. Iid = {}.",
                    static_cast<void*>(p_error_lens),
                    iid);
            }
            // register IAsrErrorLens instance.
            map_[iid] = {p_error_lens};
        }
    }
    return ASR_S_OK;
}

AsrResult ErrorLensManager::Register(
    IAsrSwigReadOnlyGuidVector* p_guid_vector,
    IAsrSwigErrorLens*          p_error_lens)
{
    const AsrPtr<IAsrErrorLens> p_cpp_error_lens =
        MakeAsrPtr<SwigToCpp<IAsrSwigErrorLens>>(p_error_lens);
    const auto p_cpp_guid_vector = ASR::MakeAsrPtr<
        IAsrReadOnlyGuidVector,
        SwigToCpp<IAsrSwigReadOnlyGuidVector>>(p_guid_vector);
    return Register(p_cpp_guid_vector.Get(), p_cpp_error_lens.Get());
}
AsrResult ErrorLensManager::FindInterface(
    const AsrGuid&  iid,
    IAsrErrorLens** pp_out_lens)
{
    ASR_UTILS_CHECK_POINTER(pp_out_lens)
    if (const auto it = map_.find(iid); it != map_.end())
    {
        const auto& p_factory = it->second;
        *pp_out_lens = p_factory.Get();
        p_factory->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_NO_INTERFACE;
}

auto ErrorLensManager::GetErrorMessage(
    const AsrGuid&      iid,
    IAsrReadOnlyString* locale_name,
    AsrResult           error_code) const
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>
{
    if (const auto it = map_.find(iid); it != map_.end())
    {
        AsrPtr<IAsrReadOnlyString> p_result{};
        const auto get_error_message_result = it->second->GetErrorMessage(
            locale_name,
            error_code,
            p_result.Put());
        if (IsOk(get_error_message_result))
        {
            return p_result;
        }

        return tl::make_unexpected(get_error_message_result);
    }
    return tl::make_unexpected(ASR_E_OUT_OF_RANGE);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END