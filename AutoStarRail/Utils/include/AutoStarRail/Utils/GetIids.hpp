#ifndef ASR_UTILS_GETIIDS_HPP
#define ASR_UTILS_GETIIDS_HPP

#include <AutoStarRail/Utils/PresetTypeInheritanceInfo.h>
#include <AutoStarRail/IAsrInspectable.h>
#include <vector>
#include <array>

ASR_UTILS_NS_BEGIN

#define ASR_UTILS_GET_RUNTIME_CLASS_NAME_IMPL(class_name, pp_out_class_name)   \
    static AsrReadOnlyString result{static_cast<const char*>(                  \
        ASR_UTILS_STRINGUTILS_DEFINE_U8STR(#class_name))};                     \
                                                                               \
    result.GetImpl(pp_out_class_name);                                         \
    return ASR_S_OK

/**
 * @brief Every member function MUST NOT modify the member variables.
 */
template <class... Ts>
class IAsrIidsVectorImpl final : public IAsrIidVector
{
    const std::array<AsrGuid, sizeof...(Ts)> impl_{AsrIidOf<Ts>()...};

public:
    constexpr IAsrIidsVectorImpl() = default;
    // IAsrBase
    constexpr int64_t AddRef() override { return 1; }
    constexpr int64_t Release() override { return 1; }
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override
    {
        if (pp_out_object == nullptr)
        {
            return ASR_E_INVALID_POINTER;
        }
        if (iid == AsrIidOf<IAsrIidVector>())
        {
            auto* const p_this = static_cast<IAsrIidVector*>(this);
            p_this->AddRef();
            *pp_out_object = p_this;
            return ASR_S_OK;
        }
        else if (iid == AsrIidOf<IAsrBase>())
        {
            auto* const p_this = static_cast<IAsrBase*>(this);
            p_this->AddRef();
            *pp_out_object = p_this;
            return ASR_S_OK;
        }
        return ASR_E_NO_INTERFACE;
    }
    // IAsrIidVector
    AsrResult Size(uint32_t* p_out_size) override
    {
        if (p_out_size == nullptr)
        {
            return ASR_E_INVALID_POINTER;
        }
        const uint32_t size = static_cast<uint32_t>(impl_.size());
        *p_out_size = size;
        return ASR_S_OK;
    }
    AsrResult At(uint32_t index, AsrGuid* p_out_iid) override
    {
        if (p_out_iid == nullptr)
        {
            return ASR_E_INVALID_POINTER;
        }
        if (index >= impl_.size())
        {
            return ASR_E_OUT_OF_RANGE;
        }
        *p_out_iid = impl_[index];
        return ASR_S_OK;
    };
    AsrResult Find(const AsrGuid& p_iid) override
    {
        for (const auto& iid : impl_)
        {
            if (iid == p_iid)
            {
                return ASR_TRUE;
            }
        }
        return ASR_FALSE;
    };
};

template <class... Ts>
AsrResult InternalGetIids(
    internal_type_holder<Ts...>,
    IAsrIidVector** pp_out_iid_vector)
{
    using Impl = IAsrIidsVectorImpl<Ts...>;
    constinit static Impl iids{};
    if (pp_out_iid_vector == nullptr)
    {
        return ASR_E_INVALID_POINTER;
    }
    *pp_out_iid_vector = const_cast<Impl*>(&iids);
    return ASR_S_OK;
}

/**
 * @brief 手动向指定预设继承链的最后面按顺序添加AdditionalTs中的类型
 * @tparam PresetTypeInheritanceInfo 预设继承链
 * @tparam AdditionalTs 要向预设继承链中添加的多个类型
 * @param pp_out_iid_vector
 * @return
 */
template <class PresetTypeInheritanceInfo, class... AdditionalTs>
AsrResult GetIids(IAsrIidVector** pp_out_iid_vector)
{
    using RhsType = internal_type_holder<AdditionalTs...>;
    return InternalGetIids(
        decltype(PresetTypeInheritanceInfo{} + RhsType{}){},
        pp_out_iid_vector);
}

/**
 * @brief 自动检测第一个参数的类型并作为继承链的最后一个类型
 * @tparam PresetTypeInheritanceInfo 预设继承链
 * @tparam T 第一个参数的类型
 * @param pp_out_iid_vector
 * @return
 */
template <class PresetTypeInheritanceInfo, class T>
AsrResult GetIids(T*, IAsrIidVector** pp_out_iid_vector)
{
    using RhsType = internal_type_holder<T>;
    return InternalGetIids(
        decltype(PresetTypeInheritanceInfo{} + RhsType{}){},
        pp_out_iid_vector);
}

ASR_UTILS_NS_END

#endif // ASR_UTILS_GETIIDS_HPP
