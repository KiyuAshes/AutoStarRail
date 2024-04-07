#ifndef ASR_PLUGINS_ASRADBTOUCH_ADBTOUCHFACTORYIMPL_H
#define ASR_PLUGINS_ASRADBTOUCH_ADBTOUCHFACTORYIMPL_H

#include <AutoStarRail/PluginInterface/IAsrInput.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>

ASR_NS_BEGIN

class AdbTouchFactory final : public IAsrInputFactory
{
public:
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AdbTouchFactory);
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrTypeInfo
    AsrResult GetRuntimeClassName(IAsrReadOnlyString** pp_out_name) override;
    AsrResult GetGuid(AsrGuid* p_out_guid) override;
    // IAsrInputFactory
    AsrResult CreateInstance(
        IAsrReadOnlyString* p_json_config,
        IAsrInput**         pp_out_input) override;
};

ASR_NS_END

#endif // ASR_PLUGINS_ASRADBTOUCH_ADBTOUCHFACTORYIMPL_H
