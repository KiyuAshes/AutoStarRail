#ifndef ASR_GUIDVECTOR_H
#define ASR_GUIDVECTOR_H

#include <AutoStarRail/IAsrBase.h>

// {EBC40F58-F1A6-49FF-9241-18D155576F9E}
ASR_DEFINE_GUID(
    ASR_IID_GUID_VECTOR,
    IAsrGuidVector,
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
SWIG_IGNORE(IAsrGuidVector)
ASR_INTERFACE IAsrGuidVector : public IAsrBase
{
    ASR_METHOD Size(size_t * p_out_size) = 0;
    ASR_METHOD At(size_t index, AsrGuid * p_out_iid) = 0;
    ASR_METHOD Find(const AsrGuid& iid) = 0;
    ASR_METHOD PushBack(const AsrGuid& iid) = 0;
};

// {E00E7F36-A7BC-4E35-8E98-5C9BB6B1D19B}
ASR_DEFINE_GUID(
    ASR_IID_GUID_SWIG_VECTOR,
    IAsrSwigGuidVector,
    0xe00e7f36,
    0xa7bc,
    0x4e35,
    0x8e,
    0x98,
    0x5c,
    0x9b,
    0xb6,
    0xb1,
    0xd1,
    0x9b);
ASR_SWIG_EXPORT_ATTRIBUTE(IAsrSwigGuidVector)
ASR_INTERFACE IAsrSwigGuidVector : public IAsrSwigBase
{
    virtual AsrRetUInt Size() = 0;
    virtual AsrRetGuid At(size_t index) = 0;
    virtual AsrResult  Find(const AsrGuid& p_iid) = 0;
    virtual AsrResult  PushBack(const AsrGuid& p_iid) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetSwigGuidVector)
    IAsrSwigGuidVector* value{};
ASR_RET_TYPE_DECLARE_END

#endif // ASR_GUIDVECTOR_H
