#include "Plugin.h"
#include "ForeignInterfaceHost.h"

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

Plugin::Plugin(
    Asr::AsrPtr<Asr::Core::ForeignInterfaceHost::IForeignLanguageRuntime>
                                p_runtime,
    Asr::AsrPtr<IAsrPlugin>     p_plugin,
    std::unique_ptr<PluginDesc> up_desc)
    : p_runtime_{std::move(p_runtime)}, p_plugin_{std::move(p_plugin)},
      up_desc_{std::move(up_desc)}, load_state_{ASR_S_OK}, load_error_message{}
{
}

Plugin::~Plugin() = default;

Plugin::operator bool() const noexcept { return IsOk(load_state_); }

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
