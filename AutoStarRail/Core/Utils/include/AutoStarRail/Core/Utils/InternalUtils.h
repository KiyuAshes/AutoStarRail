#ifndef ASR_CORE_UTILS_INTERNALUTILS_H
#define ASR_CORE_UTILS_INTERNALUTILS_H

#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/Utils/Config.h>
#include <AutoStarRail/Utils/Expected.h>
#include <string_view>

ASR_CORE_UTILS_NS_BEGIN

[[nodiscard]]
auto MakeAsrReadOnlyStringFromUtf8(std::string_view u8_string)
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>;

ASR_CORE_UTILS_NS_END

#endif // ASR_CORE_UTILS_INTERNALUTILS_H
