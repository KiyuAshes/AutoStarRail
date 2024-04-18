#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IAsrInputFactoryVectorImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <algorithm>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

IAsrInputFactoryVectorImpl::IAsrInputFactoryVectorImpl(
    AsrInputFactoryVectorImpl& impl)
    : impl_{impl}
{
}

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

IAsrSwigInputFactoryVectorImpl::IAsrSwigInputFactoryVectorImpl(
    AsrInputFactoryVectorImpl& impl)
    : impl_{impl}
{
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

    const auto expected_result =
        MakeInterop<IAsrSwigInputFactory>(p_cpp_result.Get());
    ToAsrRetType(expected_result, result);
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
auto IAsrSwigInputFactoryVectorImpl::QueryInterface(const AsrGuid& iid)
    -> AsrRetSwigBase
{
    return Utils::QueryInterface<IAsrSwigInputFactoryVector>(this, iid);
}

auto AsrInputFactoryVectorImpl::InternalFind(const AsrGuid& iid)
    -> AsrInputFactoryVectorImpl::ContainerIt
{
    return std::ranges::find_if(
        input_factory_vector_,
        [iid](const auto& pair)
        {
            const auto ret_guid = pair.second->GetGuid();
            if (IsFailed(ret_guid.error_code))
            {
                return false;
            }
            return ret_guid.value == iid;
        });
}

AsrInputFactoryVectorImpl::AsrInputFactoryVectorImpl(
    const InputFactoryManager& manager)
    : input_factory_vector_{manager.GetVector()}
{
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

        *pp_out_factory = input_factory_vector_[index].first.Get();
        (*pp_out_factory)->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
}

auto AsrInputFactoryVectorImpl::Find(
    const AsrGuid&     iid,
    IAsrInputFactory** pp_out_factory) -> AsrResult
{
    const auto it = InternalFind(iid);
    if (it != input_factory_vector_.end())
    {
        ASR_UTILS_CHECK_POINTER(pp_out_factory)

        const auto p_result = it->first.Get();
        *pp_out_factory = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
}

auto AsrInputFactoryVectorImpl::At(size_t index) -> AsrRetInputFactory
{
    if (index < input_factory_vector_.size())
    {
        const auto p_result = input_factory_vector_[index].second.Get();
        p_result->AddRef();
        return {ASR_S_OK, p_result};
    }
    return {ASR_E_OUT_OF_RANGE, nullptr};
}

auto AsrInputFactoryVectorImpl::Find(const AsrGuid& iid) -> AsrRetInputFactory
{
    const auto it = InternalFind(iid);
    if (it != input_factory_vector_.end())
    {
        const auto p_result = it->second.Get();
        p_result->AddRef();
        return {ASR_S_OK, p_result};
    }
    return {ASR_E_OUT_OF_RANGE, nullptr};
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
