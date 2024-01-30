#ifndef ASR_CORE_FOREIGNINTERFACEHOST_IASRPLUGINMANAGERIMPL_H
#define ASR_CORE_FOREIGNINTERFACEHOST_IASRPLUGINMANAGERIMPL_H

#include "ForeignInterfaceHost.h"

#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <vector>

class AsrPluginInfoImpl final : public IAsrPluginInfo, public IAsrSwigPluginInfo
{
    using PluginDesc = ASR::Core::ForeignInterfaceHost::PluginDesc;

    template <auto MemberPointer>
    ASR_IMPL GetStringImpl(IAsrReadOnlyString** pp_out_string);
    template <auto MemberPointer>
    AsrRetReadOnlyString GetAsrStringImpl();

public:
    AsrPluginInfoImpl(std::shared_ptr<PluginDesc> sp_desc);
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrPluginInfoImpl);
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrPluginInfo
    ASR_IMPL GetName(IAsrReadOnlyString** pp_out_name) override;
    ASR_IMPL GetDescription(IAsrReadOnlyString** pp_out_description) override;
    ASR_IMPL GetAuthor(IAsrReadOnlyString** pp_out_author) override;
    ASR_IMPL GetVersion(IAsrReadOnlyString** pp_out_version) override;
    ASR_IMPL GetSupportedSystem(
        IAsrReadOnlyString** pp_out_supported_system) override;
    ASR_IMPL GetPluginIid(AsrGuid* p_out_guid) override;

    // IAsrSwigBase
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    // IAsrSwigPluginInfo
    AsrRetReadOnlyString GetName() override;
    AsrRetReadOnlyString GetDescription() override;
    AsrRetReadOnlyString GetAuthor() override;
    AsrRetReadOnlyString GetVersion() override;
    AsrRetReadOnlyString GetSupportedSystem() override;
    AsrRetGuid           GetPluginIid() override;

private:
    std::shared_ptr<PluginDesc> sp_desc_;
};

class AsrPluginInfoVectorImpl final : public IAsrPluginInfoVector,
                                      public IAsrSwigPluginInfoVector
{
public:
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrPluginInfoVectorImpl);
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_out_objects) override;
    // IAsrPluginInfoVector
    ASR_IMPL Size(size_t* p_out_size) override;
    ASR_IMPL At(size_t index, IAsrPluginInfo** pp_out_info) override;

    // IAsrSwigBase
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    // IAsrSwigPluginInfoVector
    AsrRetUInt       Size() override;
    AsrRetPluginInfo At(size_t index) override;

    void AddInfo(ASR::AsrPtr<AsrPluginInfoImpl> p_info);

private:
    std::vector<ASR::AsrPtr<AsrPluginInfoImpl>> vector_;
};

#endif // ASR_CORE_FOREIGNINTERFACEHOST_IASRPLUGINMANAGERIMPL_H
