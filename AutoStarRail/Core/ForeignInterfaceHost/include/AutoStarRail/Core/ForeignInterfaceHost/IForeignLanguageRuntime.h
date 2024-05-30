#ifndef ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H
#define ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/ForeignInterfaceHostEnum.h>
#include <variant>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/Utils/Expected.h>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

using CommonPluginPtr =
    std::variant<AsrPtr<IAsrPlugin>, AsrPtr<IAsrSwigPlugin>>;

/**
 * @brief 创建语言runtime必须的内容，一次性给够够全部语言runtime创建的信息
 *  未来可能被放入共享内存， 因此要注意内存分配问题
 */
struct ForeignLanguageRuntimeFactoryDesc
{
    ForeignInterfaceLanguage language;
};

ASR_INTERFACE IForeignLanguageRuntime : public IAsrBase
{
    virtual auto LoadPlugin(const std::filesystem::path& path)
        -> ASR::Utils::Expected<CommonPluginPtr> = 0;
};

/**
 *
 * @param desc_base
 * @return ASR_E_NO_IMPLEMENTATION 意味着对应语言的接口未实现
 */
auto CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDesc& desc_base)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_IFOREIGNLANGUAGERUNTIME_H
