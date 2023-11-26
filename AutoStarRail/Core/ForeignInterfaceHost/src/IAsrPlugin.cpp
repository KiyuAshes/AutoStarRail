#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include "TemporaryPluginObjectStorage.h"

AsrResult AsrRegisterPluginObject(AsrRetSwigBase result_and_p_object)
{
    ASR::AsrPtr<IAsrSwigPlugin> p_plugin{
        static_cast<IAsrSwigPlugin*>(result_and_p_object.value.GetVoid()),
        Asr::take_ownership};

    // TODO: 确认其它语言中生命周期管理的规范后，尝试对plugin的ref_count进行检验
    if (p_plugin->Release() == 0)
    {
        ASR_CORE_LOG_ERROR(
            "The plugin object was unexpectedly released. Maybe the plugin author forget to call AddRef for plugin object.");
        return ASR_E_INVALID_POINTER;
    }

    // See
    // AutoStarRail/Core/ForeignInterfaceHost/src/TemporaryPluginObjectStorage.h:16
    // The mutex has been already locked at
    // PythonRuntime::GetPluginInitializer().
    ASR::Core::ForeignInterfaceHost::g_plugin_object.p_plugin_ = p_plugin;

    return ASR_S_OK;
}