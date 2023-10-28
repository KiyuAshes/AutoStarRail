#ifndef ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H
#define ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include "ForeignInterfaceHostEnum.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/Utils/Expected.h>

#define ASR_CORE_FOREIGNINTERFACEHOST_ASRCOCREATEPLUGIN_NAME "AsrCoCreatePlugin"

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

struct ForeignLanguageRuntimeFactoryDescBase
{
    ForeignInterfaceLanguage language;
};

ASR_INTERFACE IForeignLanguageRuntime : public IAsrBase
{
    virtual auto LoadPlugin(const std::filesystem::path& path)
        ->ASR::Utils::Expected<AsrPtr<IAsrPlugin>> = 0;
};

auto CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDescBase& desc_base)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H
