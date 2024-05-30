#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ERRORLENSMANAGER_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ERRORLENSMANAGER_H

#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/Utils/Expected.h>
#include <unordered_map>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class ErrorLensManager
{
private:
    std::unordered_map<AsrGuid, AsrPtr<IAsrErrorLens>> map_{};

public:
    AsrResult Register(
        IAsrReadOnlyGuidVector* p_guid_vector,
        IAsrErrorLens*          p_error_lens);
    AsrResult Register(
        IAsrSwigReadOnlyGuidVector* p_guid_vector,
        IAsrSwigErrorLens*          p_error_lens);

    AsrResult FindInterface(const AsrGuid& iid, IAsrErrorLens** pp_out_lens);

    auto GetErrorMessage(
        const AsrGuid&      iid,
        IAsrReadOnlyString* locale_name,
        AsrResult           error_code) const
        -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ERRORLENSMANAGER_H
