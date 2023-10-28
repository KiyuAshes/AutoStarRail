#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ASRGUID_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ASRGUID_H

#include <AutoStarRail/Core/Exceptions/InvalidGuidStringException.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/Utils/fmt.h>
#include <nlohmann/json_fwd.hpp>

template <>
struct std::hash<AsrGuid>
{
    std::size_t operator()(const AsrGuid& guid) const noexcept;
};

template <>
struct ASR_FMT_NS::formatter<AsrGuid, char>
    : public formatter<std::string, char>
{
    auto format(const AsrGuid& guid, format_context& ctx) const ->
        typename std::remove_reference_t<decltype(ctx)>::iterator;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

AsrGuid MakeAsrGuid(const std::string_view guid_string);

void from_json(const nlohmann::json& input, AsrGuid& output);

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ASRGUID_H
