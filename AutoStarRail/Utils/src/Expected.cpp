#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>

ASR_UTILS_NS_BEGIN

ASR_DEFINE_VARIABLE(Details::NULL_STRING){""};

ErrorAndExplanation::ErrorAndExplanation(const AsrResult error_code) noexcept
    : error_code{error_code}
{
}

ASR_UTILS_NS_END

auto ASR_FMT_NS::formatter<ASR::Utils::VariantString, char>::format(
    const ASR::Utils::VariantString& string,
    format_context&                  ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    const char* p_string_data = std::visit(
        ASR::Utils::overload_set{
            [](const char* string_in_variant) { return string_in_variant; },
            // match std::string_view and std::string
            [](const std::string_view string_in_variant)
            { return string_in_variant.data(); },
            [](const AsrReadOnlyString string_in_variant)
            { return string_in_variant.GetUtf8(); }},
        string);
    return ASR_FMT_NS::format_to(ctx.out(), "{}", p_string_data);
}
