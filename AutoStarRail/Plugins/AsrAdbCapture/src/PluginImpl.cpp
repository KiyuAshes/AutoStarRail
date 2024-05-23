#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/StringUtils.h>

#define ASR_BUILD_SHARED

#include "AdbCaptureFactoryImpl.h"
#include "PluginImpl.h"

#include <array>
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
    size_t index,
    void** pp_out_interface)
{
    ASR_UTILS_CHECK_POINTER_FOR_PLUGIN(pp_out_interface);
    switch (index)
    {
        // Capture Factory
    case 0:
    {
        const auto p_result =
            MakeAsrPtr<IAsrCaptureFactory, AdbCaptureFactoryImpl>();
        *pp_out_interface = p_result.Get();
        p_result->AddRef();
        return ASR_S_OK;
    }
        // Error lens 暂时用不到，先不启用
    case 1:
        [[fallthrough]];
    default:
        *pp_out_interface = nullptr;
        return ASR_E_OUT_OF_RANGE;
    }
}

static std::atomic_int32_t g_ref_count;

AsrResult AdbCapturePlugin::CanUnloadNow()
{
    return g_ref_count == 0 ? ASR_TRUE : ASR_FALSE;
}

void AdbCaptureAddRef() { g_ref_count++; }

void AdbCaptureRelease() { g_ref_count--; }

ASR_NS_END
