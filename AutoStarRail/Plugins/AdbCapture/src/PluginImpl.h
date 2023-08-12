#ifndef ASR_PLUGINS_PLUGINIMPL_H
#define ASR_PLUGINS_PLUGINIMPL_H

#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/Utils.hpp>
#include <AutoStarRail/Utils/QueryInterfaceImpl.hpp>
#include <cstdint>
#include <unordered_map>
#include <array>

ASR_NS_BEGIN

class AdbCapturePlugin final : public IAsrPlugin
{
private:
    ASR::Utils::RefCounter<AdbCapturePlugin> ref_counter_;

public:
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& guid, void** pp_out_object)
        override;
    // IAsrPlugin

    AsrResult EnumFeature(const size_t index, AsrPluginFeature* p_out_feature)
        override;
    AsrResult GetFeatureInterface(
        AsrPluginFeature feature,
        IAsrBase**       pp_out_interface) override;
};

ASR_NS_END

#endif // ASR_PLUGINS_PLUGINIMPL_H