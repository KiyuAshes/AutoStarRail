#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <utility>

AsrSwigPluginInfo::AsrSwigPluginInfo(::Asr::AsrPtr<IAsrPluginInfo> p_impl)
    : p_plugin_info{std::move(p_impl)}
{
}

AsrRetReadOnlyString AsrSwigPluginInfo::GetName()
{
    return Asr::Core::ForeignInterfaceHost::CallCppMethod<
        AsrRetReadOnlyString,
        IAsrReadOnlyString,
        ASR_DV_V(&IAsrPluginInfo::GetName)>(p_plugin_info.Get());
}

AsrRetReadOnlyString AsrSwigPluginInfo::GetDescription()
{
    return Asr::Core::ForeignInterfaceHost::CallCppMethod<
        AsrRetReadOnlyString,
        IAsrReadOnlyString,
        ASR_DV_V(&IAsrPluginInfo::GetDescription)>(p_plugin_info.Get());
}

AsrRetReadOnlyString AsrSwigPluginInfo::GetAuthor()
{
    return Asr::Core::ForeignInterfaceHost::CallCppMethod<
        AsrRetReadOnlyString,
        IAsrReadOnlyString,
        ASR_DV_V(&IAsrPluginInfo::GetAuthor)>(p_plugin_info.Get());
}

AsrRetReadOnlyString AsrSwigPluginInfo::GetVersion()
{
    return Asr::Core::ForeignInterfaceHost::CallCppMethod<
        AsrRetReadOnlyString,
        IAsrReadOnlyString,
        ASR_DV_V(&IAsrPluginInfo::GetVersion)>(p_plugin_info.Get());
}

AsrRetReadOnlyString AsrSwigPluginInfo::GetSupportedSystem()
{
    return Asr::Core::ForeignInterfaceHost::CallCppMethod<
        AsrRetReadOnlyString,
        IAsrReadOnlyString,
        ASR_DV_V(&IAsrPluginInfo::GetSupportedSystem)>(p_plugin_info.Get());
}

AsrRetGuid AsrSwigPluginInfo::GetPluginIid()
{
    AsrRetGuid result{};
    AsrGuid    iid{};
    result.error_code = p_plugin_info->GetPluginIid(&result.value);
    return result;
}
