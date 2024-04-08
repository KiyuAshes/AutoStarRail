#ifndef ASR_PLUGINS_ASRADBTOUCH_PLUGINIMPL_H
#define ASR_PLUGINS_ASRADBTOUCH_PLUGINIMPL_H

#include <AutoStarRail/ExportInterface/IAsrBasicErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>

ASR_NS_BEGIN

class AsrAdbTouchPlugin final : public IAsrPlugin
{

    AsrPtr<IAsrBasicErrorLens> g_error_lens;

public:
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrAdbTouchPlugin)
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrPlugin
    ASR_IMPL EnumFeature(size_t index, AsrPluginFeature* p_out_feature)
        override;
    ASR_IMPL CreateFeatureInterface(
        size_t           index,
        void**           pp_out_interface) override;
    ASR_IMPL CanUnloadNow() override;
};

ASR_NS_END

#endif // ASR_PLUGINS_ASRADBTOUCH_PLUGINIMPL_H
