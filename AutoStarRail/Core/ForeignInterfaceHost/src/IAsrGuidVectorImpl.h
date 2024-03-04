#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <vector>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class AsrGuidVectorImpl
{
    std::vector<AsrGuid> iids_;

public:
    auto Size() const noexcept -> size_t;
    auto At(size_t index, AsrGuid& out_guid) noexcept -> AsrResult;
    auto Find(const AsrGuid guid) noexcept -> AsrResult;
    auto PushBack(const AsrGuid guid) noexcept -> AsrResult;
    auto GetImpl() noexcept -> std::vector<AsrGuid>&;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

class IAsrGuidVectorImpl final : public IAsrGuidVector
{
    ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl impl_{};

public:
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(IAsrGuidVectorImpl)
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrGuidVector
    AsrResult Size(size_t* p_out_size) override;
    AsrResult At(size_t index, AsrGuid* p_out_iid) override;
    // IAsrGuidVector
    AsrResult Find(const AsrGuid& iid) override;
    AsrResult PushBack(const AsrGuid& iid) override;
    // IAsrGuidVectorImpl
    auto GetImpl() noexcept -> std::vector<AsrGuid>&;
};

class IAsrSwigGuidVectorImpl final : public IAsrSwigGuidVector
{
    ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl impl_{};

public:
    ASR_UTILS_IASRBASE_AUTO_IMPL(IAsrSwigGuidVectorImpl)
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    AsrRetUInt     Size() override;
    AsrRetGuid     At(size_t index) override;
    AsrResult      Find(const AsrGuid& iid) override;
    AsrResult      PushBack(const AsrGuid& iid) override;
};

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
