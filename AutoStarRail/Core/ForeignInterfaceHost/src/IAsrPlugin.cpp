#include "TemporaryPluginObjectStorage.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>

AsrResult AsrRegisterPluginObject(AsrResult error_code, IAsrSwigPlugin* p_swig_plugin)
{
    AsrResult result{ASR_S_OK};

    if (ASR::IsFailed(error_code))
    {
        return error_code;
    }

    ASR::AsrPtr p_plugin{p_swig_plugin};

    switch (const auto ref_count = p_plugin->AddRef())
    {
    case 1:
        ASR_CORE_LOG_WARN(
            "The reference count inside the plugin object is too small.\n"
            "Maybe the plugin author forget to call AddRef for plugin object.\n"
            "AsrCore will try to fix it.");
        break;
    case 2:
        p_plugin->Release();
        break;
    default:
        ASR_CORE_LOG_ERROR(
            "Unexpected reference count inside the plugin object.\n"
            "Expected 3 but {} found.",
            ref_count);
        result = ASR_E_INTERNAL_FATAL_ERROR;
    }

    // See
    // AutoStarRail/Core/ForeignInterfaceHost/src/TemporaryPluginObjectStorage.h:16
    // The mutex has been already locked at
    // PythonRuntime::GetPluginInitializer().
    ASR::Core::ForeignInterfaceHost::g_plugin_object.p_plugin_ = p_plugin;

    return result;
}