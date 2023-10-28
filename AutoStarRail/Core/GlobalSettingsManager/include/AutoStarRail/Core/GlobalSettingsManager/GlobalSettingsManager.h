#ifndef ASR_CORE_GLOBALSETTINGSMANAGER_GLOBALSETTINGSMANAGER_H
#define ASR_CORE_GLOBALSETTINGSMANAGER_GLOBALSETTINGSMANAGER_H

#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Core/GlobalSettingsManager/Config.h>
#include <nlohmann/json.hpp>

NLOHMANN_JSON_SERIALIZE_ENUM(
    AsrType,
    {{ASR_TYPE_INT, "int"},
     {ASR_TYPE_FLOAT, "float"},
     {ASR_TYPE_STRING, "string"},
     {ASR_TYPE_BOOL, "bool"}})

#endif // ASR_CORE_GLOBALSETTINGSMANAGER_GLOBALSETTINGSMANAGER_H
