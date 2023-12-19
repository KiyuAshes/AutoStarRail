#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H

#include <AutoStarRail/IAsrInspectable.h>
#include "AutoStarRail/Utils/CommonUtils.hpp"
#include <vector>

class AsrIidVectorImpl final : public IAsrIidVector
{
    std::vector<AsrGuid> iids_;

public:
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrIidVectorImpl);
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrIidVector
    AsrResult Size(size_t* p_out_size) override;
    AsrResult At(size_t index, AsrGuid* p_out_iid) override;
    AsrResult Find(const AsrGuid& iid) override;
    // AsrIidVectorImpl
    auto GetImpl() -> decltype(iids_)&;
};

#endif // AUTOSTARRAIL_ASRIIDVECTOR_H
