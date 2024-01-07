#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H

#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/IAsrBase.h>
#include "ForeignInterfaceHost.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/ForeignInterfaceHostEnum.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>
#include <variant>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

/**
 * @brief 尚不支持热重载
 *
 */
class Plugin
{
    friend class PluginManager;

    AsrPtr<IForeignLanguageRuntime> p_runtime_{};
    CommonPluginPtr                 p_plugin_{};
    std::shared_ptr<PluginDesc>     sp_desc_{};
    AsrResult                       load_state_{
        ASR_E_UNDEFINED_RETURN_VALUE}; // NOTE: 4 byte padding here.
    AsrPtr<IAsrReadOnlyString> load_error_message_{};

public:
    Plugin(
        AsrPtr<IForeignLanguageRuntime> p_runtime,
        CommonPluginPtr                 p_plugin,
        std::unique_ptr<PluginDesc>     up_desc);
    /**
     * @brief 出错时使用此构造函数
     *
     * @param load_state
     * @param p_error_message
     */
    Plugin(AsrResult load_state, IAsrReadOnlyString* p_error_message);
    Plugin(Plugin&& other) noexcept;

    explicit operator bool() const noexcept;

    auto GetInfo() const -> AsrPtr<IAsrPluginInfo>;

    ~Plugin();
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
