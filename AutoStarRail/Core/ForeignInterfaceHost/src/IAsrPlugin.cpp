#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include "TemporaryPluginObjectStorage.h"

AsrResult AsrRegisterPluginObject(AsrRetSwigBase result_and_p_object)
{
    ASR::AsrPtr p_plugin{
        static_cast<IAsrSwigPlugin*>(result_and_p_object.value.GetVoid()),
        Asr::take_ownership};

    switch (const auto ref_count = p_plugin->AddRef())
    {
    case 1:
        ASR_CORE_LOG_WARN(
            "The reference count inside the plugin object is too small. "
            "Maybe the plugin author forget to call AddRef for plugin object.");
        break;
    case 2:
        p_plugin->Release();
        break;
    default:
        ASR_CORE_LOG_ERROR(
            "Unexpected reference count inside the plugin object."
            "Expected 3 but {} found.",
            ref_count);
    }

    // See
    // AutoStarRail/Core/ForeignInterfaceHost/src/TemporaryPluginObjectStorage.h:16
    // The mutex has been already locked at
    // PythonRuntime::GetPluginInitializer().
    ASR::Core::ForeignInterfaceHost::g_plugin_object.p_plugin_ = p_plugin;

    return ASR_S_OK;
}