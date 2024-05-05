#ifndef ASR_ASRCONTEXT_H
#define ASR_ASRCONTEXT_H

#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/IAsrBase.h>

// {E46BA67B-E08E-4C89-9087-F3038E3D1997}
ASR_DEFINE_GUID(
    ASR_IID_CONTEXT,
    IAsrContext,
    0xe46ba67b,
    0xe08e,
    0x4c89,
    0x90,
    0x87,
    0xf3,
    0x3,
    0x8e,
    0x3d,
    0x19,
    0x97);
SWIG_IGNORE(IAsrContext)
ASR_INTERFACE IAsrContext : public IAsrBase
{
    ASR_METHOD GetString(
        IAsrReadOnlyString * key,
        IAsrReadOnlyString * *pp_out_string) = 0;
    ASR_METHOD GetBool(IAsrReadOnlyString * key, bool* p_out_bool) = 0;
    ASR_METHOD GetInt(IAsrReadOnlyString * key, int64_t * p_out_int) = 0;
    ASR_METHOD GetFloat(IAsrReadOnlyString * key, float* p_out_float) = 0;

    ASR_METHOD SetString(IAsrReadOnlyString * key, IAsrReadOnlyString * value) =
        0;
    ASR_METHOD SetBool(IAsrReadOnlyString * key, bool value) = 0;
    ASR_METHOD SetInt(IAsrReadOnlyString * key, int64_t value) = 0;
    ASR_METHOD SetFloat(IAsrReadOnlyString * key, float value) = 0;
};

// {5A2873AB-8061-44B1-9592-AA904F2B8247}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_CONTEXT,
    IAsrSwigContext,
    0x5a2873ab,
    0x8061,
    0x44b1,
    0x95,
    0x92,
    0xaa,
    0x90,
    0x4f,
    0x2b,
    0x82,
    0x47);
ASR_SWIG_EXPORT_ATTRIBUTE(IAsrSwigContext)
ASR_INTERFACE IAsrSwigContext : public IAsrSwigBase
{
    virtual AsrRetReadOnlyString GetString(AsrReadOnlyString key) = 0;
    virtual AsrRetBool           GetBool(AsrReadOnlyString key) = 0;
    virtual AsrRetInt            GetInt(AsrReadOnlyString key) = 0;
    virtual AsrRetFloat          GetFloat(AsrReadOnlyString key) = 0;

    virtual AsrResult SetString(
        AsrReadOnlyString key,
        AsrReadOnlyString value) = 0;
    virtual AsrResult SetBool(AsrReadOnlyString key, bool value) = 0;
    virtual AsrResult SetInt(AsrReadOnlyString key, int64_t value) = 0;
    virtual AsrResult SetFloat(AsrReadOnlyString key, float value) = 0;
};

ASR_DEFINE_RET_POINTER(AsrRetContext, IAsrSwigContext);

SWIG_IGNORE(CreateIAsrContext)
ASR_C_API AsrResult CreateIAsrContext(IAsrContext** pp_out_context);

ASR_API AsrRetContext CreateIAsrSwigContext();

#endif // ASR_ASRCONTEXT_H
