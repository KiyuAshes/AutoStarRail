#include <AutoStarRail/Core/i18n/AsrResultTranslator.h>
#include <AutoStarRail/Core/i18n/GlobalLocale.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Utils/fmt.h>
#include <AutoStarRail/Utils/StringUtils.h>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>

ASR_CORE_I18N_NS_BEGIN

AsrResult TranslateError(
    IAsrReadOnlyString*  locale_name,
    AsrResult            error_code,
    IAsrReadOnlyString** out_string)
{
    const char*    p_locale_name{nullptr};
    const char8_t* p_u8_locale_name{};
    std::u8string  u8_return_string{};

    auto result = locale_name->GetUtf8(&p_locale_name);
    if (!ASR::IsOk(result))
    {
        ASR_CORE_LOG_WARN(
            "Failed to get local name from string pointer. AsrCore will use \"en\" instead.");
        p_u8_locale_name = u8"en";
    }
    else
    {
        p_u8_locale_name = reinterpret_cast<const char8_t*>(p_locale_name);
    }

    result = g_translator_data.GetErrorExplanation(
        p_u8_locale_name,
        error_code,
        &u8_return_string);

    if (!ASR::IsOk(result))
    {
        const auto error_string = ASR::fmt::format(
            ASR_UTILS_STRINGUTILS_DEFINE_U8STR(
                "Error happened when getting error explanation. Code = {} ."),
            result);
        ASR_CORE_LOG_ERROR(error_string);
        AsrPtr<IAsrString> p_error_string;
        ::CreateIAsrStringFromUtf8(error_string.c_str(), p_error_string.Put());
        p_error_string->AddRef();
        *out_string = p_error_string.Get();
        return result;
    }

    AsrPtr<IAsrString> p_error_string;
    ::CreateIAsrStringFromUtf8(
        reinterpret_cast<const char*>(u8_return_string.c_str()),
        p_error_string.Put());
    p_error_string->AddRef();
    *out_string = p_error_string.Get();
    return result;
}

I18n<AsrResult> MakeAsrResultTranslatorData()
{
    TranslateResources<AsrResult> translate_resource{
        {u8"en",
         {{ASR_S_OK, u8"Success"},
          {ASR_E_NO_INTERFACE, u8"No interface"},
          {ASR_E_UNDEFINED_RETURN_VALUE, u8"Return value not defined"},
          {ASR_E_INVALID_STRING, u8"Invalid string"},
          {ASR_E_INVALID_STRING_SIZE, u8"Invalid string size"},
          {ASR_E_NO_IMPLEMENTATION, u8"No implementation"},
          {ASR_E_UNSUPPORTED_SYSTEM, u8"Unsupported system"},
          {ASR_E_INVALID_JSON, u8"Invalid JSON"}}},
        {
            u8"zh-cn",
            {{{ASR_S_OK, u8"成功"},
              {ASR_E_NO_INTERFACE, u8"接口未找到"},
              {ASR_E_UNDEFINED_RETURN_VALUE, u8"接口没有处理返回值"},
              {ASR_E_INVALID_STRING, u8"非法字符串"},
              {ASR_E_INVALID_STRING_SIZE, u8"非法字符串长度"},
              {ASR_E_NO_IMPLEMENTATION, u8"未实现"},
              {ASR_E_UNSUPPORTED_SYSTEM, u8"不支持的操作系统"},
              {ASR_E_INVALID_JSON, u8"非法的JSON数据"}}},
        }};
    decltype(g_translator_data) result{std::move(translate_resource)};
    return result;
}

ASR_DEFINE_VARIABLE(g_translator_data) = MakeAsrResultTranslatorData();

ASR_NS_ANONYMOUS_DETAILS_BEGIN

struct CharComparator
{
    bool operator()(const char* const p_lhs, const char* const p_rhs)
        const noexcept
    {
        return (std::strcmp(p_lhs, p_rhs) < 0);
    }
};

const std::map<const char*, const char*, CharComparator>
    g_translate_error_failed_explanation{
        {ASR_UTILS_STRINGUTILS_DEFINE_U8STR("zh-cn"),
         ASR_UTILS_STRINGUTILS_DEFINE_U8STR(
             "无法检索到错误码（值为{}）的解释。错误码：{}。")},
        {ASR_UTILS_STRINGUTILS_DEFINE_U8STR("us"),
         ASR_UTILS_STRINGUTILS_DEFINE_U8STR(
             "Can not find error code (value = {}) explanation. Error code: {}.")}};

auto FormatUnexplainableError(
    const AsrResult   error_code_that_failed_at_getting_error_explanation,
    const AsrResult   error_code,
    const char* const p_explanation_template,
    AsrPtr<IAsrReadOnlyString>& in_out_error_string,
    IAsrReadOnlyString**        pp_out_string) -> ASR::Utils::Expected<void>
{
    const auto explanation = ASR::fmt::vformat(
        p_explanation_template,
        ASR::fmt::make_format_args(
            error_code_that_failed_at_getting_error_explanation,
            error_code));
    const auto create_string_result = ::CreateIAsrReadOnlyStringFromUtf8(
        explanation.c_str(),
        in_out_error_string.Put());
    if (ASR::IsOk(create_string_result)) [[likely]]
    {
        in_out_error_string->AddRef();
        *pp_out_string = in_out_error_string.Get();
        return {};
    }
    else [[unlikely]]
    {
        ASR_CORE_LOG_ERROR(
            "Failed to create IAsrReadOnlyString. Error code: {}.",
            create_string_result);
        *pp_out_string = nullptr;
        return tl::make_unexpected(create_string_result);
    }
}

ASR_NS_ANONYMOUS_DETAILS_END

/**
 *
 * @param unexplainable_error_code 无法解释的错误码
 * @param error_code_that_failed_at_getting_error_explanation
 * 请求解释时失败的错误码
 * @param pp_out_string 输出的文本
 */
AsrResult GetExplanationWhenTranslateErrorFailed(
    const AsrResult      unexplainable_error_code,
    const AsrResult      error_code_that_failed_at_getting_error_explanation,
    IAsrReadOnlyString** pp_out_string)
{
    AsrResult                       result{ASR_E_UNDEFINED_RETURN_VALUE};
    ASR::AsrPtr<IAsrReadOnlyString> p_error_string{};
    auto                            default_locale = ::AsrGetDefaultLocale();
    const auto* const p_locale_name = default_locale.value.GetUtf8();

    if (const auto it =
            Details::g_translate_error_failed_explanation.find(p_locale_name);
        it != Details::g_translate_error_failed_explanation.end())
    {
        const auto* const p_explanation_template = it->second;
        Details::FormatUnexplainableError(
            error_code_that_failed_at_getting_error_explanation,
            unexplainable_error_code,
            p_explanation_template,
            p_error_string,
            pp_out_string)
            .or_else([&result](const AsrResult ec) { result = ec; });
    }
    else
    {
        const auto  fallback_locale = ASR::Core::i18n::GetFallbackLocale();
        const char* u8_fallback_locale{};
        fallback_locale->GetUtf8(&u8_fallback_locale);
        // 这里不应该抛出异常
        const auto* const p_explanation_template =
            Details::g_translate_error_failed_explanation.at(
                u8_fallback_locale);
        Details::FormatUnexplainableError(
            error_code_that_failed_at_getting_error_explanation,
            unexplainable_error_code,
            p_explanation_template,
            p_error_string,
            pp_out_string)
            .or_else([&result](const AsrResult ec) { result = ec; });
    }
    return result;
}

ASR_CORE_I18N_NS_END