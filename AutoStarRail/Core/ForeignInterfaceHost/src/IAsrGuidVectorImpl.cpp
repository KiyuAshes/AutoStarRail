#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrGuidVectorImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <algorithm>

IAsrReadOnlyGuidVectorImpl::IAsrReadOnlyGuidVectorImpl(
    Asr::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl)
    : impl_{impl}
{
}

int64_t IAsrReadOnlyGuidVectorImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrReadOnlyGuidVectorImpl::Release() { return impl_.Release(); }

AsrResult IAsrReadOnlyGuidVectorImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrReadOnlyGuidVector>(
        this,
        iid,
        pp_out_object);
}

AsrResult IAsrReadOnlyGuidVectorImpl::Size(size_t* p_out_size)
{
    ASR_UTILS_CHECK_POINTER(p_out_size);

    *p_out_size = impl_.Size();

    return ASR_S_OK;
}

AsrResult IAsrReadOnlyGuidVectorImpl::At(size_t index, AsrGuid* p_out_iid)
{
    ASR_UTILS_CHECK_POINTER(p_out_iid)

    return impl_.At(index, *p_out_iid);
}

AsrResult IAsrReadOnlyGuidVectorImpl::Find(const AsrGuid& iid)
{
    return impl_.Find(iid);
}

IAsrGuidVectorImpl::IAsrGuidVectorImpl(
    Asr::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl)
    : impl_{impl}
{
}

int64_t IAsrGuidVectorImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrGuidVectorImpl::Release() { return impl_.Release(); }

AsrResult IAsrGuidVectorImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrGuidVector>(this, iid, pp_out_object);
}

AsrResult IAsrGuidVectorImpl::Size(size_t* p_out_size)
{
    ASR_UTILS_CHECK_POINTER(p_out_size);

    *p_out_size = impl_.Size();

    return ASR_S_OK;
}

AsrResult IAsrGuidVectorImpl::At(size_t index, AsrGuid* p_out_iid)
{
    ASR_UTILS_CHECK_POINTER(p_out_iid)

    return impl_.At(index, *p_out_iid);
}

AsrResult IAsrGuidVectorImpl::Find(const AsrGuid& iid)
{
    return impl_.Find(iid);
}

AsrResult IAsrGuidVectorImpl::PushBack(const AsrGuid& iid)
{
    return impl_.PushBack(iid);
}

AsrResult IAsrGuidVectorImpl::ToConst(IAsrReadOnlyGuidVector** pp_out_object)
{
    ASR_UTILS_CHECK_POINTER(pp_out_object)

    *pp_out_object = impl_;

    return ASR_S_OK;
}

auto IAsrGuidVectorImpl::GetImpl() noexcept -> std::vector<AsrGuid>&
{
    return impl_.GetImpl();
}

auto IAsrGuidVectorImpl::Get()
    -> Asr::Core::ForeignInterfaceHost::AsrGuidVectorImpl&
{
    return impl_;
}
IAsrSwigReadOnlyGuidVectorImpl::IAsrSwigReadOnlyGuidVectorImpl(
    Asr::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl)
    : impl_{impl}
{
}

int64_t IAsrSwigReadOnlyGuidVectorImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSwigReadOnlyGuidVectorImpl::Release() { return impl_.Release(); }

AsrRetSwigBase IAsrSwigReadOnlyGuidVectorImpl::QueryInterface(
    const AsrGuid& iid)
{
    return ASR::Utils::QueryInterface<IAsrSwigReadOnlyGuidVector>(this, iid);
}

AsrRetUInt IAsrSwigReadOnlyGuidVectorImpl::Size()
{
    return {ASR_S_OK, impl_.Size()};
}

AsrRetGuid IAsrSwigReadOnlyGuidVectorImpl::At(size_t index)
{
    AsrRetGuid result{};

    result.error_code = impl_.At(index, result.value);

    return result;
}

AsrResult IAsrSwigReadOnlyGuidVectorImpl::Find(const AsrGuid& iid)
{
    return impl_.Find(iid);
}

IAsrSwigGuidVectorImpl::IAsrSwigGuidVectorImpl(
    Asr::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl)
    : impl_{impl}
{
}

int64_t IAsrSwigGuidVectorImpl::AddRef() { return impl_.AddRef(); }

int64_t IAsrSwigGuidVectorImpl::Release() { return impl_.Release(); }

AsrRetSwigBase IAsrSwigGuidVectorImpl::QueryInterface(const AsrGuid& iid)
{
    return ASR::Utils::QueryInterface<IAsrSwigGuidVector>(this, iid);
}

AsrRetUInt IAsrSwigGuidVectorImpl::Size() { return {ASR_S_OK, impl_.Size()}; }

AsrRetGuid IAsrSwigGuidVectorImpl::At(size_t index)
{
    AsrRetGuid result{};

    result.error_code = impl_.At(index, result.value);

    return result;
}

AsrResult IAsrSwigGuidVectorImpl::Find(const AsrGuid& iid)
{
    return impl_.Find(iid);
}

AsrResult IAsrSwigGuidVectorImpl::PushBack(const AsrGuid& iid)
{
    return impl_.PushBack(iid);
}

AsrRetReadOnlyGuidVector IAsrSwigGuidVectorImpl::ToConst()
{
    return {ASR_S_OK, {static_cast<IAsrSwigReadOnlyGuidVector*>(impl_)}};
}

auto IAsrSwigGuidVectorImpl::Get()
    -> Asr::Core::ForeignInterfaceHost::AsrGuidVectorImpl&
{
    return impl_;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

int64_t AsrGuidVectorImpl::AddRef() { return ref_counter_.AddRef(); }

int64_t AsrGuidVectorImpl::Release() { return ref_counter_.Release(this); }

auto AsrGuidVectorImpl::Size() const noexcept -> size_t { return iids_.size(); }

auto AsrGuidVectorImpl::At(size_t index, AsrGuid& out_guid) const noexcept
    -> AsrResult
{
    if (index >= iids_.size())
    {
        return ASR_E_OUT_OF_RANGE;
    }

    out_guid = iids_[index];

    return ASR_S_OK;
}

auto AsrGuidVectorImpl::Find(const AsrGuid guid) noexcept -> AsrResult
{
    const auto it_guid = std::find(ASR_FULL_RANGE_OF(iids_), guid);
    return it_guid != iids_.end() ? ASR_TRUE : ASR_FALSE;
}

auto AsrGuidVectorImpl::PushBack(const AsrGuid guid) noexcept -> AsrResult
{
    try
    {
        iids_.push_back(guid);
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
    return ASR_S_OK;
}

auto AsrGuidVectorImpl::GetImpl() noexcept -> std::vector<AsrGuid>&
{
    return iids_;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

AsrResult CreateIAsrGuidVector(
    const AsrGuid*   p_iids,
    size_t           iid_count,
    IAsrGuidVector** pp_out_iid_vector)
{
    *pp_out_iid_vector = nullptr;
    try
    {
        auto* const result =
            new ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl{};
        result->AddRef();
        auto& impl = result->GetImpl();
        impl.resize(iid_count);
        ASR::Utils::CopyArray(p_iids, iid_count, impl.data());
        *pp_out_iid_vector = *result;
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

AsrRetGuidVector CreateIAsrSwigGuidVector()
{
    AsrRetGuidVector result{};
    try
    {
        const auto p_result =
            new ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl{};
        result.value = {static_cast<IAsrSwigGuidVector*>(*p_result)};
    }
    catch (const std::bad_alloc&)
    {
        result.error_code = ASR_E_OUT_OF_MEMORY;
    }
    return result;
}
