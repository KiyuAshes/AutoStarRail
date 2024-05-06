#ifndef ASR_UTILS_EXPECTED_H
#define ASR_UTILS_EXPECTED_H

#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Utils/Config.h>
#include <AutoStarRail/Utils/fmt.h>
#include <tl/expected.hpp>
#include <type_traits>
#include <variant>

#define ASTR_UTILS_LOG_ON_ERROR                                                \
    [](const auto& unexpected) { ASR_CORE_LOG_ERROR(unexpected.explanation); }

ASR_UTILS_NS_BEGIN

using VariantString =
    std::variant<AsrReadOnlyString, std::string, const char*, std::string_view>;

namespace Details
{
    extern const char* const NULL_STRING;
}

struct ErrorAndExplanation
{
    explicit ErrorAndExplanation(const AsrResult error_code) noexcept;
    template <class T>
    explicit ErrorAndExplanation(const AsrResult error_code, T&& explanation)
        : error_code{error_code}, explanation{std::forward<T>(explanation)}
    {
    }

    AsrResult     error_code;
    VariantString explanation{Details::NULL_STRING};
};

template <class... Args>
auto MakeUnexpected(Args&&... args)
{
    return tl::make_unexpected(std::forward<Args>(args)...);
}

template <class T>
using ExpectedWithExplanation = tl::expected<T, ErrorAndExplanation>;

template <class T>
using Expected = tl::expected<T, AsrResult>;

template <class T>
auto Map(T&& object) -> Expected<T>
{
    return std::forward<T>(object);
}

template <class T>
AsrResult GetResult(const Expected<T>& expected_result)
{
    if (expected_result.has_value())
    {
        return ASR_S_OK;
    }
    return expected_result.error();
}

ASR_UTILS_NS_END

template <>
struct ASR_FMT_NS::formatter<ASR::Utils::VariantString, char>
    : public formatter<const char*, char>
{
    auto format(const ASR::Utils::VariantString& string, format_context& ctx)
        const -> typename std::remove_reference_t<decltype(ctx)>::iterator;
};

using ASRE = ASR::Utils::ErrorAndExplanation;

#endif // ASR_UTILS_EXPECTED_H
