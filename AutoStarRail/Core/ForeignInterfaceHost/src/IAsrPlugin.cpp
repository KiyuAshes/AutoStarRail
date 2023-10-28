#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include "TemporaryPluginObjectStorage.h"

AsrResult AsrRegisterPluginObject(AsrRetSwigBase result_and_p_object)
{
    ASR::AsrPtr<IAsrPlugin> p_plugin{
        new ASR::Core::ForeignInterfaceHost::SwigToCpp<IAsrSwigPlugin>(
            static_cast<IAsrSwigPlugin*>(result_and_p_object.value.GetVoid())),
        ASR::take_ownership};

    if (p_plugin->Release() == 0)
    {
        ASR_CORE_LOG_ERROR(
            "The plugin object was unexpectedly released. Maybe the plugin author forget to call AddRef for plugin object.");
        return ASR_E_INVALID_POINTER;
    }

    // See AutoStarRail/Core/ForeignInterfaceHost/src/TemporaryPluginObjectStorage.h:16
    // The mutex has been already locked at PythonRuntime::GetPluginInitializer().
    ASR::Core::ForeignInterfaceHost::g_plugin_object.p_plugin_ = p_plugin;

    return ASR_S_OK;
}