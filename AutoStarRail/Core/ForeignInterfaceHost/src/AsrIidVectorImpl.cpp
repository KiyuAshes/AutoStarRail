#include "AsrIidVectorImpl.h"
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <algorithm>

AsrResult AsrIidVectorImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrGuidVector>(this, iid, pp_out_object);
}

AsrResult AsrIidVectorImpl::Size(size_t* p_out_size)
{
    const auto size = iids_.size();
    const auto size_32 =
        static_cast<std::remove_pointer_t<decltype(p_out_size)>>(size);
    *p_out_size = size_32;
    return ASR_S_OK;
}

AsrResult AsrIidVectorImpl::At(size_t index, AsrGuid* p_out_iid)
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

AsrResult AsrIidVectorImpl::PushBack(const AsrGuid& iid)
{
    try
    {
        iids_.push_back(iid);
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
    return ASR_S_OK;
}

auto AsrIidVectorImpl::GetImpl() -> decltype(iids_)& { return iids_; }

AsrResult CreateIAsrGuidVector(
    const AsrGuid*   p_iids,
    size_t           iid_count,
    IAsrGuidVector** pp_out_iid_vector)
{
    *pp_out_iid_vector = nullptr;
    try
    {
        // TODO:在带锁内存池synchronized_pool_resource中分配对象，以尽量避免OOM的情况出现
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

int64_t AsrSwigIidVectorImpl::AddRef() { return impl_.AddRef(); }

int64_t AsrSwigIidVectorImpl::Release() { return impl_.Release(); }

AsrRetSwigBase AsrSwigIidVectorImpl::QueryInterface(const AsrGuid& iid)
{
    return ASR::Utils::QueryInterface<IAsrSwigGuidVector>(this, iid);
}

AsrRetUInt AsrSwigIidVectorImpl::Size()
{
    AsrRetUInt result{};
    size_t     size;

    result.error_code = impl_.Size(&size);
    result.value = size;

    return result;
}

AsrRetGuid AsrSwigIidVectorImpl::At(size_t index)
{
    AsrRetGuid result{};

    result.error_code = impl_.At(index, &result.value);

    return result;
}

AsrResult AsrSwigIidVectorImpl::Find(const AsrGuid& guid)
{
    return impl_.Find(guid);
}

AsrResult AsrSwigIidVectorImpl::PushBack(const AsrGuid& guid)
{
    return impl_.PushBack(guid);
}

AsrRetSwigGuidVector CreateIAsrSwigGuidVector()
{
    AsrRetSwigGuidVector result{};
    try
    {
        const auto p_result = new AsrSwigIidVectorImpl{};
        result.value = p_result;
        p_result->AddRef();
    }
    catch (const std::bad_alloc&)
    {
        result.error_code = ASR_E_OUT_OF_MEMORY;
    }
    return result;
}
