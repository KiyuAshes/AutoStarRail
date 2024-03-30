#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrInputFactoryVectorImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <algorithm>

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
    return Utils::QueryInterface<IAsrInputFactoryVector>(this, iid, pp_object);
}

AsrResult IAsrInputFactoryVectorImpl::Size(size_t* p_out_size)
{
    ASR_UTILS_CHECK_POINTER(p_out_size);

    *p_out_size = impl_.Size();
    return ASR_S_OK;
}

AsrResult IAsrInputFactoryVectorImpl::At(
    size_t             index,
    IAsrInputFactory** pp_out_factory)
{
    return impl_.At(index, pp_out_factory);
}

AsrResult IAsrInputFactoryVectorImpl::Find(
    const AsrGuid&     iid,
    IAsrInputFactory** pp_out_factory)
{
    return impl_.Find(iid, pp_out_factory);
}

auto IAsrSwigInputFactoryVectorImpl::AddRef() -> int64_t
{
    return impl_.AddRef();
}

auto IAsrSwigInputFactoryVectorImpl::Release() -> int64_t
{
    return impl_.Release();
}

auto IAsrSwigInputFactoryVectorImpl::Size() -> AsrRetUInt
{
    return {ASR_S_OK, impl_.Size()};
}

auto IAsrSwigInputFactoryVectorImpl::At(size_t index) -> AsrRetInputFactory
{
    AsrRetInputFactory       result{};
    AsrPtr<IAsrInputFactory> p_cpp_result;

    result.error_code = impl_.At(index, p_cpp_result.Put());
    if (IsFailed(result.error_code))
    {
        return result;
    }

    const auto exptected_result =
        MakeInterop<IAsrSwigInputFactory>(p_cpp_result.Get());
    ToAsrRetType(exptected_result, result);
    return result;
}

auto IAsrSwigInputFactoryVectorImpl::Find(const AsrGuid& iid)
    -> AsrRetInputFactory
{
    AsrRetInputFactory       result{};
    AsrPtr<IAsrInputFactory> p_cpp_result{};

    result.error_code = impl_.Find(iid, p_cpp_result.Put());
    if (IsFailed(result.error_code))
    {
        return result;
    }

    const auto p_result = MakeInterop<IAsrSwigInputFactory>(p_cpp_result.Get());
    ToAsrRetType(p_result, result);
    return result;
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

auto AsrInputFactoryVectorImpl::Find(
    const AsrGuid&     iid,
    IAsrInputFactory** pp_out_factory) -> AsrResult
{
    const auto it = std::ranges::find_if(
        input_factory_vector_,
        [iid](const AsrPtr<IAsrInputFactory>& p_factory) -> bool
        {
            AsrGuid factory_iid;
            if (IsOk(p_factory->GetGuid(&factory_iid)))
            {
                return factory_iid == iid;
            }
            return false;
        });
    if (it != input_factory_vector_.end())
    {
        ASR_UTILS_CHECK_POINTER(pp_out_factory)

        const auto value = it->Get();
        *pp_out_factory = value;
        value->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
