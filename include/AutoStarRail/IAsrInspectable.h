#ifndef ASR_INSPECTABLE_H
#define ASR_INSPECTABLE_H

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrString.hpp>

// {EBC40F58-F1A6-49FF-9241-18D155576F9E}
ASR_DEFINE_GUID(
    ASR_IID_IID_VECTOR,
    IAsrIidVector,
    0xebc40f58,
    0xf1a6,
    0x49ff,
    0x92,
    0x41,
    0x18,
    0xd1,
    0x55,
    0x57,
    0x6f,
    0x9e)
SWIG_IGNORE(IAsrIidVector)
ASR_INTERFACE IAsrIidVector : public IAsrBase
{
    ASR_METHOD Size(uint32_t * p_out_size) = 0;
    ASR_METHOD At(uint32_t index, AsrGuid * p_out_iid) = 0;
    ASR_METHOD Find(const AsrGuid& p_iid) = 0;
};

class AsrSwigIidVector
{
    ASR::AsrPtr<IAsrIidVector> p_impl_{};

public:
    ASR_API AsrSwigIidVector() = default;
    ASR_API ~AsrSwigIidVector() = default;

#ifndef SWIG
    AsrSwigIidVector(ASR::AsrPtr<IAsrIidVector> p_impl);
    AsrResult GetImpl(IAsrIidVector** pp_out_impl);
    operator IAsrIidVector*() const;
#endif // SWIG

    ASR_API AsrRetUInt Size();
    ASR_API AsrRetGuid At(uint32_t index);
    ASR_API AsrRetBool Find(const AsrGuid* p_iid);
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetSwigIidVector)
    AsrSwigIidVector value{};
ASR_RET_TYPE_DECLARE_END

// {C66B4652-BEA9-4985-B8AC-F168BF0442E8}
ASR_DEFINE_GUID(
    ASR_IID_INSPECTABLE,
    IAsrInspectable,
    0xc66b4652,
    0xbea9,
    0x4985,
    0xb8,
    0xac,
    0xf1,
    0x68,
    0xbf,
    0x4,
    0x42,
    0xe8)
SWIG_IGNORE(IAsrInspectable)
ASR_INTERFACE IAsrInspectable : public IAsrBase
{
    /**
     * Allocate a vector which contains all the IIDs of this object.
     * @param pp_out_iid_vector
     * @return ASR_E_OUT_OF_MEMORY when failed to allocate memory.
     */
    ASR_METHOD GetIids(IAsrIidVector * *pp_out_iid_vector) = 0;
    /**
     * Get derived class name.
     * @param p_runtime_string
     * @return
     */
    ASR_METHOD GetRuntimeClassName(IAsrReadOnlyString * *pp_out_name) = 0;
};

// {B1090AA9-2AE8-4FBD-B486-CED42CE90915}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_INSPECTABLE,
    IAsrSwigInspectable,
    0xb1090aa9,
    0x2ae8,
    0x4fbd,
    0xb4,
    0x86,
    0xce,
    0xd4,
    0x2c,
    0xe9,
    0x9,
    0x15)
ASR_SWIG_DIRECTOR_ATTRIBUTE(IAsrSwigInspectable)
ASR_INTERFACE IAsrSwigInspectable : public IAsrSwigBase
{
    virtual AsrRetSwigIidVector  GetIids() = 0;
    virtual AsrRetReadOnlyString GetRuntimeClassName() = 0;
};

SWIG_IGNORE(CreateIAsrIidVector)
ASR_C_API AsrResult CreateIAsrIidVector(
    const AsrGuid*        p_iids,
    size_t          iid_count,
    IAsrIidVector** pp_out_iid_vector);

/**
 * @brief NOTE: DO NOT USE THIS API IN C++ PLUGIN. It may has ABI compatibility
 * issues.
 * @param iids
 * @return
 */
#ifdef SWIG
// clang-format off
%template(StdVectorTEAsrGuidET) std::vector<AsrGuid>;
// clang-format on
#endif // SWIG
ASR_API AsrRetSwigIidVector
CreateAsrSwigIidVector(const std::vector<AsrGuid>& iids);

#endif // ASR_INSPECTABLE_H
