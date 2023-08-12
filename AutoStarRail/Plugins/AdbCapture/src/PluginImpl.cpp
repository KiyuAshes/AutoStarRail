#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/PluginInterface/IAsrPlugin.h"
#include "AutoStarRail/Utils/QueryInterfaceImpl.hpp"
#define ASR_BUILD_SHARED

#include "PluginImpl.h"
#include <array>

ASR_NS_BEGIN

int64_t AdbCapturePlugin::AddRef() { return ref_counter_.AddRef(); }

int64_t AdbCapturePlugin::Release() { return ref_counter_.Release(this); }

AsrResult AdbCapturePlugin::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrPlugin>(this, iid, pp_out_object);
}

AsrResult AdbCapturePlugin::EnumFeature(const size_t index, AsrPluginFeature* p_out_feature)
{
    static std::array features{
        ASR_PLUGIN_FEATURE_CAPTURE_FACTORY,
        ASR_PLUGIN_FEATURE_ERROR_LENS};
    try
    {
        const auto result = features.at(index);
        *p_out_feature = result;
        return result;
    }
    catch (const std::out_of_range& ex)
    {
        return ASR_E_OUT_OF_RANGE;
    }
}

AsrResult AdbCapturePlugin::GetFeatureInterface(
    AsrPluginFeature feature,
    IAsrBase**       pp_out_interface)
{
    // TODO: Create instance for every feature.
    switch (feature)
    {
    case ASR_PLUGIN_FEATURE_CAPTURE_FACTORY:
    case ASR_PLUGIN_FEATURE_ERROR_LENS:
    default:
        *pp_out_interface = nullptr;
        return ASR_E_OUT_OF_RANGE;
    }
}

ASR_NS_END