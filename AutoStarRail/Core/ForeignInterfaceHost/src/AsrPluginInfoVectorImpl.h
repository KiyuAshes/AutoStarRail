#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ASRPLUGININFOVECTORIMPL_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ASRPLUGININFOVECTORIMPL_H

#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <vector>

class AsrPluginInfoVectorImpl final : public IAsrPluginInfoVector
{
public:
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrPluginInfoVectorImpl);
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_out_objects) override;
    // IAsrPluginInfoVector
    ASR_IMPL Size(size_t* p_out_size) override;
    ASR_IMPL At(size_t index, IAsrPluginInfo** pp_out_info) override;

    void AddInfo(ASR::AsrPtr<IAsrPluginInfo> p_info);

private:
    std::vector<ASR::AsrPtr<IAsrPluginInfo>> vector_;
};

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ASRPLUGININFOVECTORIMPL_H
