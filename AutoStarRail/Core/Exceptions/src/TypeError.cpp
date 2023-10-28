#include <AutoStarRail/Core/Exceptions/TypeError.h>
#include <AutoStarRail/Utils/fmt.h>
#include <magic_enum.hpp>

Asr::Core::Exceptions::TypeError::TypeError(
    const AsrType expected,
    const AsrType actual)
    : Base{
        [](const AsrType expected, const AsrType actual)
        {
            const auto expected_int = magic_enum::enum_integer(expected);
            const auto actual_int = magic_enum::enum_integer(actual);
            const auto expected_string = magic_enum::enum_name(expected);
            const auto actual_string = magic_enum::enum_name(actual);
            return ASR::fmt::format(
                "Unexpected type {}(value = {}) found. Expected type {}(value = {}).",
                expected_string,
                expected_int,
                actual_string,
                actual_int);
        }(expected, actual)}
{
}
