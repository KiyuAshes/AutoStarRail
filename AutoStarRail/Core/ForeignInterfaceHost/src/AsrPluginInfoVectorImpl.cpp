#include "AsrPluginInfoVectorImpl.h"
#include <AutoStarRail/Utils/QueryInterface.hpp>

ASR_IMPL AsrPluginInfoVectorImpl:: QueryInterface(
    const AsrGuid& iid,
    void**         pp_out_objects)
{
    return ASR::Utils::QueryInterface<IAsrPluginInfoVector>(
        this,
        iid,
        pp_out_objects);
}

ASR_IMPL AsrPluginInfoVectorImpl::Size(size_t* p_out_size)
{
    if (!p_out_size)
    {
        return ASR_E_INVALID_POINTER;
    }
    *p_out_size = vector_.size();
    return ASR_S_OK;
}

ASR_IMPL AsrPluginInfoVectorImpl::At(size_t index, IAsrPluginInfo** pp_out_info)
{
    if (index < vector_.size())
    {
        if (!pp_out_info)
        {
            return ASR_E_INVALID_POINTER;
        }
        auto& result = *pp_out_info;
        result = vector_[index].Get();
        result->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_OUT_OF_RANGE;
}

void AsrPluginInfoVectorImpl::AddInfo(Asr::AsrPtr<IAsrPluginInfo> p_info)
{
    vector_.emplace_back(std::move(p_info));
}
