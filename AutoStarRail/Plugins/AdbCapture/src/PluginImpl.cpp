#include "AutoStarRail/IAsrBase.h"
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/StringUtils.h>
#include <AutoStarRail/Utils/GetIids.hpp>
#define ASR_BUILD_SHARED

#include "PluginImpl.h"
#include <stdexcept>

ASR_NS_BEGIN

int64_t AdbCapturePlugin::AddRef() { return ref_counter_.AddRef(); }

int64_t AdbCapturePlugin::Release() { return ref_counter_.Release(this); }

AsrResult AdbCapturePlugin::QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_object)
{
    return ASR::Utils::QueryInterface<IAsrPlugin>(this, iid, pp_out_object);
}

AsrResult AdbCapturePlugin::EnumFeature(
    const size_t      index,
    AsrPluginFeature* p_out_feature)
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
        ASR_LOG_ERROR(ex.what());
        return ASR_E_OUT_OF_RANGE;
    }
}

AsrResult AdbCapturePlugin::CreateFeatureInterface(
    AsrPluginFeature feature,
    void**           pp_out_interface)
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

AsrResult AdbCapturePlugin::GetIids(IAsrIidVector** pp_out_iids)
{
    return ASR::Utils::GetIids<
        ASR::Utils::IAsrCaptureFactoryInheritanceInfo,
        AdbCapturePlugin>(pp_out_iids);
}

AsrResult AdbCapturePlugin::GetRuntimeClassName(
    IAsrReadOnlyString** pp_out_class_name)
{
    ASR_UTILS_GET_RUNTIME_CLASS_NAME_IMPL(Asr::AdbCapturePlugin, pp_out_class_name);
}

ASR_NS_END