#ifndef ASR_GUIDVECTOR_H
#define ASR_GUIDVECTOR_H

#include <AutoStarRail/IAsrBase.h>

// {8AE436FE-590B-4B70-B24F-ED1327E9841C}
ASR_DEFINE_GUID(
    ASR_IID_READ_ONLY_GUID_VECTOR,
    IAsrReadOnlyGuidVector,
    0x8ae436fe,
    0x590b,
    0x4b70,
    0xb2,
    0x4f,
    0xed,
    0x13,
    0x27,
    0xe9,
    0x84,
    0x1c);
SWIG_IGNORE(IAsrReadOnlyGuidVector)
ASR_INTERFACE IAsrReadOnlyGuidVector : public IAsrBase
{
    ASR_METHOD Size(size_t * p_out_size) = 0;
    ASR_METHOD At(size_t index, AsrGuid * p_out_iid) = 0;
    ASR_METHOD Find(const AsrGuid& iid) = 0;
};

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
    ASR_METHOD ToConst(IAsrReadOnlyGuidVector * *pp_out_object) = 0;
};

SWIG_IGNORE(CreateIAsrGuidVector)
ASR_C_API AsrResult CreateIAsrGuidVector(
    const AsrGuid*   p_data,
    size_t           size,
    IAsrGuidVector** pp_out_guid);

// {60A09918-04E3-44E8-936E-730EB72024F5}
ASR_DEFINE_GUID(
    ASR_SWIG_READ_ONLY_GUID_VECTOR,
    IAsrSwigReadOnlyGuidVector,
    0x60a09918,
    0x4e3,
    0x44e8,
    0x93,
    0x6e,
    0x73,
    0xe,
    0xb7,
    0x20,
    0x24,
    0xf5);
ASR_SWIG_EXPORT_ATTRIBUTE(IAsrSwigReadOnlyGuidVector)
ASR_INTERFACE IAsrSwigReadOnlyGuidVector : public IAsrSwigBase
{
    virtual AsrRetUInt Size() = 0;
    virtual AsrRetGuid At(size_t index) = 0;
    virtual AsrResult  Find(const AsrGuid& p_iid) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetReadOnlyGuidVector)
    IAsrSwigReadOnlyGuidVector* value{};
ASR_RET_TYPE_DECLARE_END

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
    virtual AsrRetUInt               Size() = 0;
    virtual AsrRetGuid               At(size_t index) = 0;
    virtual AsrResult                Find(const AsrGuid& p_iid) = 0;
    virtual AsrResult                PushBack(const AsrGuid& p_iid) = 0;
    virtual AsrRetReadOnlyGuidVector ToConst() = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetGuidVector)
    IAsrSwigGuidVector* value{};
ASR_RET_TYPE_DECLARE_END

ASR_API AsrRetGuidVector CreateIAsrSwigGuidVector();

#endif // ASR_GUIDVECTOR_H
