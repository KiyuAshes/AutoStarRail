#include <AutoStarRail/IAsrInspectable.h>
#include <utility>

ASR_NS_ANONYMOUS_DETAILS_BEGIN

/**
 * NOTE: You must hold p_iid_vector outside the function.
 * @tparam R
 * @tparam F
 * @param p_iid_vector
 * @param do_if_not_null
 * @return
 */
template <class R, class F>
R CallIfNotNull(IAsrIidVector* p_iid_vector, F&& do_if_not_null)
{
    if (p_iid_vector)
    {
        return std::forward<F>(do_if_not_null)(*p_iid_vector);
    }

    R result{};
    result.error_code = ASR_E_UNDEFINED_RETURN_VALUE;
    return result;
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrSwigIidVector::AsrSwigIidVector(ASR::AsrPtr<IAsrIidVector> p_impl)
    : p_impl_{std::move(p_impl)}
{
}

AsrSwigIidVector::operator IAsrIidVector*() const
{
    if (p_impl_)
    {
        p_impl_->AddRef();
    }
    return p_impl_.Get();
}

AsrResult AsrSwigIidVector::GetImpl(IAsrIidVector** pp_out_impl)
{
    if (p_impl_)
    {
        *pp_out_impl = p_impl_.Get();
        p_impl_->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_UNDEFINED_RETURN_VALUE;
}

AsrRetUInt AsrSwigIidVector::Size()
{
    return Details::CallIfNotNull<AsrRetUInt>(
        p_impl_.Get(),
        [](IAsrIidVector& iid_vector)
        {
            AsrRetUInt result{};
            size_t     size{};
            result.error_code = iid_vector.Size(&size);
            result.value = size;
            return result;
        });
}

AsrRetGuid AsrSwigIidVector::At(size_t index)
{
    return Details::CallIfNotNull<AsrRetGuid>(
        p_impl_.Get(),
        [index](IAsrIidVector& iid_vector)
        {
            AsrRetGuid result{};
            result.error_code = iid_vector.At(index, &result.value);
            return result;
        });
}

AsrRetBool AsrSwigIidVector::Find(const AsrGuid* p_iid)
{
    if (p_iid == nullptr)
    {
        AsrRetBool result{};
        result.error_code = ASR_E_INVALID_POINTER;
        return result;
    }
    return Details::CallIfNotNull<AsrRetBool>(
        p_impl_.Get(),
        [p_iid](IAsrIidVector& iid_vector)
        {
            AsrRetBool result{};
            const auto value = iid_vector.Find(*p_iid);
            result.error_code = ASR_S_OK;
            result.value = (value == ASR_TRUE);
            return result;
        });
}
