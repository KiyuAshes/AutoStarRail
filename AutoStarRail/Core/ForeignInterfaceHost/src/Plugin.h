#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/Exceptions/InterfaceNotFoundException.h>
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/IAsrBase.h>
#include "ForeignInterfaceHost.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/ForeignInterfaceHostEnum.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>
#include <stdexcept>
#include <memory>
#include <ctime>
#include <variant>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

/**
 * @brief 尚不支持热重载
 *
 */
class Plugin
{
    friend class PluginManager;

    AsrPtr<IForeignLanguageRuntime> p_runtime_;
    CommonPluginPtr                 p_plugin_{};
    std::unique_ptr<PluginDesc>     up_desc_;
    AsrResult                       load_state_; // NOTE: 4 byte padding here.
    AsrPtr<IAsrReadOnlyString>      load_error_message;

public:
    Plugin(
        AsrPtr<IForeignLanguageRuntime> p_runtime,
        AsrPtr<IAsrPlugin>              p_plugin,
        std::unique_ptr<PluginDesc>     up_desc);
    Plugin(
        AsrPtr<IForeignLanguageRuntime> p_runtime,
        AsrPtr<IAsrSwigPlugin>          p_swig_plugin,
        std::unique_ptr<PluginDesc>     up_desc);
    /**
     * @brief 出错时使用此构造函数
     *
     * @param load_state
     * @param p_error_message
     */
    Plugin(AsrResult load_state, IAsrReadOnlyString* p_error_message);

    explicit operator bool() const noexcept;

    ~Plugin();
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PLUGIN_H
