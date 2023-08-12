#ifndef ASR_BASICERRORLENS_H
#define ASR_BASICERRORLENS_H

#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>

// {813FD58D-5632-4A43-A87E-26E412D9EADD}
ASR_DEFINE_GUID(
    ASR_IID_BASIC_ERROR_LENS,
    IAsrBasicErrorLens,
    0x813fd58d,
    0x5632,
    0x4a43,
    0xa8,
    0x7e,
    0x26,
    0xe4,
    0x12,
    0xd9,
    0xea,
    0xdd);
SWIG_IGNORE(IAsrBasicErrorLens)
ASR_INTERFACE IAsrBasicErrorLens : public IAsrErrorLens
{
    ASR_METHOD RegisterExplanation(
        IAsrReadOnlyString * locale_name,
        AsrResult error_code,
        IAsrReadOnlyString * p_explanation) = 0;
};

// {F44EBCCB-3110-4B0B-BB1A-E0C194E41F9B}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_BASIC_ERROR_LENS,
    IAsrSwigBasicErrorLens,
    0xf44ebccb,
    0x3110,
    0x4b0b,
    0xbb,
    0x1a,
    0xe0,
    0xc1,
    0x94,
    0xe4,
    0x1f,
    0x9b);
SWIG_ENABLE_SHARED_PTR(IAsrSwigBasicErrorLens)
SWIG_ENABLE_DIRECTOR(IAsrSwigBasicErrorLens)
ASR_INTERFACE IAsrSwigBasicErrorLens : public IAsrSwigErrorLens
{
    virtual AsrResult RegisterExplanation(
        AsrString locale_name,
        AsrResult error_code,
        AsrString explanation) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetBasicErrorLens)
    std::shared_ptr<IAsrSwigErrorLens> value;
ASR_RET_TYPE_DECLARE_END

SWIG_IGNORE(CreateIAsrBasicErrorLens)
ASR_C_API AsrResult
CreateIAsrBasicErrorLens(IAsrBasicErrorLens** pp_out_error_lens);

ASR_API AsrRetBasicErrorLens CreateIAsrSwigBasicErrorLens();

#endif // ASR_BASICERRORLENS_H
