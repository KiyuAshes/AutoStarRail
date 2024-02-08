#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H

#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <vector>

class AsrIidVectorImpl final : public IAsrGuidVector
{
    std::vector<AsrGuid> iids_;

public:
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrIidVectorImpl);
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrGuidVector
    AsrResult Size(size_t* p_out_size) override;
    AsrResult At(size_t index, AsrGuid* p_out_iid) override;
    AsrResult Find(const AsrGuid& iid) override;
    AsrResult PushBack(const AsrGuid& iid) override;
    // AsrIidVectorImpl
    auto GetImpl() -> decltype(iids_)&;
};

class AsrSwigIidVectorImpl final : public IAsrSwigGuidVector
{
    AsrIidVectorImpl impl_{};

public:
    // IAsrSwigBase
    int64_t        AddRef() override;
    int64_t        Release() override;
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    // IAsrSwigGuidVector
    AsrRetUInt Size() override;
    AsrRetGuid At(size_t index) override;
    AsrResult  Find(const AsrGuid& p_iid) override;
    AsrResult  PushBack(const AsrGuid& p_iid) override;
};

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
