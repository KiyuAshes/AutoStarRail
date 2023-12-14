#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H

#include "Plugin.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>
#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/fmt.h>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

struct PluginLoadError
{
    AsrResult   error_code;
    std::string error_message;
};

class ErrorLensManager
{
private:
    std::unordered_map<AsrGuid, AsrPtr<IAsrErrorLens>> map_{};

public:
    AsrResult Register(IAsrIidVector* p_iids, IAsrErrorLens* p_error_lens);
    AsrResult Register(IAsrIidVector* p_iids, IAsrSwigErrorLens* p_error_lens);

    auto GetErrorMessage(
        IAsrIidVector*      p_iids,
        IAsrReadOnlyString* locale_name,
        AsrResult           error_code) const
        -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>;
};

template <class T, class SwigT>
class InterfaceManager
{
private:
    struct PluginInterface
    {
        AsrPtr<T>              cpp_interface;
        std::shared_ptr<SwigT> swig_interface;
    };

    using GuidInterfaceMap = std::map<AsrGuid, PluginInterface>;

    GuidInterfaceMap map_;

    void InternalAddInterface(
        const PluginInterface& plugin_interface,
        const AsrGuid&         plugin_guid)
    {
        if (auto plugin_item = map_.find(plugin_guid);
            plugin_item != map_.end())
        {
            ASR_CORE_LOG_WARN(
                "Duplicate interface registration for plugin: {}",
                plugin_guid);
        }
        map_[plugin_guid] = plugin_interface;
    }

public:
    InterfaceManager() = default;

    AsrResult AddInterface(AsrPtr<T> p_interface, const AsrGuid& plugin_guid)
    {
        PluginInterface plugin_interface;
        plugin_interface.cpp_interface = p_interface;
        plugin_interface.swig_interface = CppToSwig<T>(p_interface);

        return InternalAddInterface(plugin_interface, plugin_guid);
    }

    AsrResult AddInterface(
        std::shared_ptr<SwigT> sp_interface,
        const AsrGuid&         plugin_guid)
    {
        PluginInterface plugin_interface;
        plugin_interface.cpp_interface = SwigToCpp<SwigT>(sp_interface);
        plugin_interface.swig_interface = sp_interface;

        return InternalAddInterface(plugin_interface, plugin_guid);
    }
};

class PluginManager
{
private:
    std::map<AsrPtr<IAsrReadOnlyString>, Plugin, AsrStringLess>
                                             plugin_file_paths_{};
    InterfaceManager<IAsrTask, IAsrSwigTask> asr_task_interface_manager_;
    std::vector<AsrPtr<IAsrCaptureFactory>>  asr_capture_interfaces_;
    ErrorLensManager                         error_lens_manager_;

    AsrResult AddInterface(CommonPluginPtr p_plugin);

    static std::unique_ptr<PluginDesc> GetPluginDesc(
        const std::filesystem::path& metadata_path);

    AsrResult GetInterface(const Plugin& plugin);

public:
    /**
     * @brief try to load all plugin. And get all interface.
     * @return AsrResult ASR_S_OK when all plugin are loaded successfully.\n
     *         ASR_S_FALSE when some plugin have error.\n
     *         ASR_E_INTERNAL_FATAL_ERROR when any plugin have ASR_E_SWIG_INTERNAL_ERROR or even worse.
     */
    std::vector<AsrResult> Refresh();

    /**
     * @brief Get the Error Explanation from AsrResult.
     *
     * @param iids
     * @param locale_name
     * @param error_code
     * @param pp_out_error_message
     * @return AsrResult
     */
    AsrResult GetErrorMessage(
        IAsrIidVector*       p_iids,
        AsrResult            error_code,
        IAsrReadOnlyString** pp_out_error_message);
    auto GetAllCaptureFactory() -> std::vector<AsrPtr<IAsrCaptureFactory>>;
};

extern PluginManager g_plugin_manager;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGINFILEMANAGER_H
