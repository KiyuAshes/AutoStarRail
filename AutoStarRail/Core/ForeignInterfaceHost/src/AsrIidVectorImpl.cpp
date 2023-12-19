#include "AsrIidVectorImpl.h"
#include "AutoStarRail/IAsrInspectable.h"

#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <algorithm>

AsrResult AsrIidVectorImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrIidVector>(this, iid, pp_out_object);
}

AsrResult AsrIidVectorImpl::Size(uint32_t* p_out_size)
{
    const auto size = iids_.size();
    const auto size_32 =
        static_cast<std::remove_pointer_t<decltype(p_out_size)>>(size);
    *p_out_size = size_32;
    return ASR_S_OK;
}

AsrResult AsrIidVectorImpl::At(uint32_t index, AsrGuid* p_out_iid)
{
    if (index < iids_.size())
    {
        *p_out_iid = iids_[index];
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
}

AsrResult AsrIidVectorImpl::Find(const AsrGuid& iid)
{
    const auto it = std::find(iids_.begin(), iids_.end(), iid);
    return it != iids_.end() ? ASR_TRUE : ASR_FALSE;
}

auto AsrIidVectorImpl::GetImpl() -> decltype(iids_)& { return iids_; }

AsrResult CreateIAsrIidVector(
    const AsrGuid*  p_iids,
    size_t          iid_count,
    IAsrIidVector** pp_out_iid_vector)
{
    *pp_out_iid_vector = nullptr;
    try
    {
        // TODO: 在带锁内存池synchronized_pool_resource中分配对象，以尽量避免OOM的情况出现
        auto* const result = new AsrIidVectorImpl{};
        result->AddRef();
        auto& impl = result->GetImpl();
        impl.resize(iid_count);
        ASR::Utils::CopyArray(p_iids, iid_count, impl.data());
        *pp_out_iid_vector = result;
        return ASR_S_OK;
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
    catch (std::length_error&)
    {
        ASR_CORE_LOG_ERROR(
            "Error happened because iids_count > iids_.max_size().]\nNOTE: iid_count = {}.",
            iid_count);
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrRetSwigIidVector CreateAsrSwigIidVector(const std::vector<AsrGuid>& iids)
{
    AsrRetSwigIidVector        result{};
    ASR::AsrPtr<IAsrIidVector> p_result{};
    result.error_code =
        ::CreateIAsrIidVector(iids.data(), iids.size(), p_result.Put());
    result.value = {std::move(p_result)};
    return result;
}
