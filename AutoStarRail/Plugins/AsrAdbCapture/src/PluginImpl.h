#ifndef ASR_PLUGINS_ASRADBCAPTURE_PLUGINIMPL_H
#define ASR_PLUGINS_ASRADBCAPTURE_PLUGINIMPL_H

#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <array>
#include <cstdint>
#include <unordered_map>

// {EAC73FD2-5674-4796-8298-71B80727E993}
ASR_DEFINE_CLASS_IN_NAMESPACE(
    Asr,
    AdbCapturePlugin,
    0xeac73fd2,
    0x5674,
    0x4796,
    0x82,
    0x98,
    0x71,
    0xb8,
    0x7,
    0x27,
    0xe9,
    0x93);

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
    AsrResult CreateFeatureInterface(size_t index, void** pp_out_interface)
        override;
    AsrResult CanUnloadNow() override;
};

void AdbCaptureAddRef();

void AdbCaptureRelease();

ASR_NS_END

#endif // ASR_PLUGINS_ASRADBCAPTURE_PLUGINIMPL_H