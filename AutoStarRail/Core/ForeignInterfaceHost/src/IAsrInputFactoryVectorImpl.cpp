#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrInputFactoryVectorImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

auto IAsrInputFactoryVectorImpl::AddRef() -> int64_t { return impl_.AddRef(); }

auto IAsrInputFactoryVectorImpl::Release() -> int64_t
{
    return impl_.Release();
}

auto IAsrInputFactoryVectorImpl::QueryInterface(
    const AsrGuid& iid,
    void**         pp_object) -> AsrResult
{
    return Utils::QueryInterface<IAsrInputFactory>(this, iid, pp_object);
}

int64_t AsrInputFactoryVectorImpl::AddRef() { return ref_counter_.AddRef(); }

int64_t AsrInputFactoryVectorImpl::Release()
{
    return ref_counter_.Release(this);
}

auto AsrInputFactoryVectorImpl::Size() const noexcept -> size_t
{
    return input_factory_vector_.size();
}

auto AsrInputFactoryVectorImpl::At(
    size_t             index,
    IAsrInputFactory** pp_out_factory) const -> AsrResult
{
    if (index < input_factory_vector_.size())
    {
        ASR_UTILS_CHECK_POINTER(pp_out_factory)

        *pp_out_factory = input_factory_vector_[index].Get();
        (*pp_out_factory)->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
