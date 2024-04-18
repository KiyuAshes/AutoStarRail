#ifndef ASR_IERRORLENS_H
#define ASR_IERRORLENS_H

#include <AutoStarRail/IAsrTypeInfo.h>
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/AsrString.hpp>

// {10963BC9-72FD-4B57-A7BA-98F431C234E4}
ASR_DEFINE_GUID(
    ASR_IID_ERROR_LENS,
    IAsrErrorLens,
    0x10963bc9,
    0x72fd,
    0x4b57,
    0xa7,
    0xba,
    0x98,
    0xf4,
    0x31,
    0xc2,
    0x34,
    0xe4);
SWIG_IGNORE(IAsrErrorLens)
ASR_INTERFACE IAsrErrorLens : public IAsrBase
{
    ASR_METHOD GetSupportedIids(IAsrReadOnlyGuidVector * *pp_out_iids) = 0;
    ASR_METHOD GetErrorMessage(
        IAsrReadOnlyString * locale_name,
        AsrResult error_code,
        IAsrReadOnlyString * *out_string) = 0;
};

// {0B9B86B2-F8A6-4EA4-90CF-3756AD318B83}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_ERROR_LENS,
    IAsrSwigErrorLens,
    0xb9b86b2,
    0xf8a6,
    0x4ea4,
    0x90,
    0xcf,
    0x37,
    0x56,
    0xad,
    0x31,
    0x8b,
    0x83);
ASR_SWIG_DIRECTOR_ATTRIBUTE(IAsrSwigErrorLens)
ASR_INTERFACE IAsrSwigErrorLens : public IAsrSwigBase
{
    virtual AsrRetReadOnlyGuidVector GetSupportedIids() = 0;
    virtual AsrRetReadOnlyString GetErrorMessage(
        const AsrReadOnlyString locale_name,
        AsrResult               error_code) = 0;
};

SWIG_IGNORE(AsrGetErrorMessage)
/**
 * @brief Get the error explanation. If return value is not ASR_S_OK,
        then the pp_out_error_explanation points to a string
        that explains why calling this function failed
 * @param p_error_generating_interface_iid
 * @param locale_name
 * @param error_code
 * @param pp_out_error_explanation It is an Error message when this function
  success. Otherwise it is a string that explains why calling this function
  failed.
 * @return AsrResult
 */
ASR_C_API AsrResult AsrGetErrorMessage(
    IAsrTypeInfo*        p_error_generator,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_message);

SWIG_IGNORE(AsrGetPredefinedErrorMessage)
ASR_C_API AsrResult AsrGetPredefinedErrorMessage(
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_error_message);

/**
 * @brief See AsrGetErrorMessage
 *
 * @param p_error_generating_interface_iid
 * @param locale_name
 * @param error_code
 * @return AsrRetReadOnlyString
 */
ASR_API AsrRetReadOnlyString
AsrGetErrorMessage(IAsrSwigTypeInfo* p_error_generator, AsrResult error_code);

ASR_API AsrRetReadOnlyString AsrGetPredefinedErrorMessage(AsrResult error_code);

#ifndef SWIG
/**
 * @brief Always return ASR_S_OK
 */
ASR_C_API AsrResult AsrSetDefaultLocale(IAsrReadOnlyString* locale_name);
/**
 * @brief See SetDefaultLocale
 */
ASR_C_API AsrResult AsrGetDefaultLocale(IAsrReadOnlyString** locale_name);
#endif // SWIG

ASR_API AsrResult            AsrSetDefaultLocale(AsrReadOnlyString locale_name);
ASR_API AsrRetReadOnlyString AsrGetDefaultLocale();

#endif // ASR_IERRORLENS_H
