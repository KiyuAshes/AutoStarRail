#ifndef ASR_CORE_FOREIGNINTERFACEHOST_INPUTFACTORYMANAGER_H
#define ASR_CORE_FOREIGNINTERFACEHOST_INPUTFACTORYMANAGER_H

#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/PluginInterface/IAsrInput.h>
#include <utility>
#include <vector>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class InputFactoryManager
{
public:
    using Type =
        std::pair<AsrPtr<IAsrInputFactory>, AsrPtr<IAsrSwigInputFactory>>;

private:
    std::vector<Type> common_input_factory_vector_;

public:
    AsrResult Register(IAsrInputFactory* p_factory);
    AsrResult Register(IAsrSwigInputFactory* p_factory);

    // 反正都是在自己模块中调用，直接内部用at实现，外面记得接异常
    void At(size_t index, AsrPtr<IAsrInputFactory>& ref_out_factory);
    void At(size_t index, AsrPtr<IAsrSwigInputFactory>& ref_out_factory);
    // 这里也是
    void Find(const AsrGuid& iid, IAsrInputFactory** pp_out_factory);
    void Find(const AsrGuid& iid, IAsrSwigInputFactory** pp_out_swig_factory);

    auto GetVector() const -> decltype(common_input_factory_vector_);
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_INPUTFACTORYMANAGER_H
