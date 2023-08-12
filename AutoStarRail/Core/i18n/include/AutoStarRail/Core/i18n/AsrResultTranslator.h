#ifndef ASR_CORE_I18N_ASRRESULTTRANSLATOR_H
#define ASR_CORE_I18N_ASRRESULTTRANSLATOR_H

#include <AutoStarRail/Core/i18n/i18n.h>

ASR_CORE_I18N_NS_BEGIN

/**
 * @brief 在项目使用全局locale以后，参数locale_name应当填入全局locale_name
 * @param locale_name
 * @param error_code
 * @param out_string
 * @return
 */
AsrResult TranslateError(
    IAsrReadOnlyString*  locale_name,
    AsrResult            error_code,
    IAsrReadOnlyString** out_string);

extern const I18n<AsrResult> g_translator_data;

AsrResult GetExplanationWhenTranslateErrorFailed(
    const AsrResult      unexplainable_error_code,
    const AsrResult      error_code_that_failed_at_getting_error_explanation,
    IAsrReadOnlyString** pp_out_string);

ASR_CORE_I18N_NS_END

#endif // ASR_CORE_I18N_ASRRESULTTRANSLATOR_H