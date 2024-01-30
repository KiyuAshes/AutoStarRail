#include "IAsrPluginManagerImpl.h"
#include <AutoStarRail/Utils/QueryInterface.hpp>

template <auto MemberPointer>
ASR_IMPL AsrPluginInfoImpl::GetStringImpl(IAsrReadOnlyString** pp_out_string)
{
    ASR_UTILS_CHECK_POINTER(pp_out_string)
    return ::CreateIAsrReadOnlyStringFromUtf8(
        (sp_desc_.get()->*MemberPointer).c_str(),
        pp_out_string);
}

template <auto MemberPointer>
AsrRetReadOnlyString AsrPluginInfoImpl::GetAsrStringImpl()
{
    ASR::AsrPtr<IAsrReadOnlyString> p_result;
    const auto error_code = ::CreateIAsrReadOnlyStringFromUtf8(
        (sp_desc_.get()->*MemberPointer).c_str(),
        p_result.Put());
    return {error_code, AsrReadOnlyString{std::move(p_result)}};
}

AsrPluginInfoImpl::AsrPluginInfoImpl(std::shared_ptr<PluginDesc> sp_desc)
    : sp_desc_{std::move(sp_desc)}
{
}

ASR_IMPL AsrPluginInfoImpl::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return ASR::Utils::QueryInterface<IAsrPluginInfo>(this, iid, pp_object);
}

ASR_IMPL AsrPluginInfoImpl::GetName(IAsrReadOnlyString** pp_out_name)
{
    return GetStringImpl<&PluginDesc::name>(pp_out_name);
}

ASR_IMPL AsrPluginInfoImpl::GetDescription(
    IAsrReadOnlyString** pp_out_description)
{
    return GetStringImpl<&PluginDesc::description>(pp_out_description);
}

ASR_IMPL AsrPluginInfoImpl::GetAuthor(IAsrReadOnlyString** pp_out_author)
{
    return GetStringImpl<&PluginDesc::author>(pp_out_author);
}

ASR_IMPL AsrPluginInfoImpl::GetVersion(IAsrReadOnlyString** pp_out_version)
{
    return GetStringImpl<&PluginDesc::version>(pp_out_version);
}

ASR_IMPL AsrPluginInfoImpl::GetSupportedSystem(
    IAsrReadOnlyString** pp_out_supported_system)
{
    return GetStringImpl<&PluginDesc::supported_system>(
        pp_out_supported_system);
}

ASR_IMPL AsrPluginInfoImpl::GetPluginIid(AsrGuid* p_out_guid)
{
    ASR_UTILS_CHECK_POINTER(p_out_guid)
    *p_out_guid = sp_desc_->guid;
    return ASR_S_OK;
}

AsrRetSwigBase AsrPluginInfoImpl::QueryInterface(const AsrGuid& iid)
{
    return ASR::Utils::QueryInterface<IAsrSwigPluginInfo>(this, iid);
}

AsrRetReadOnlyString AsrPluginInfoImpl::GetName()
{
    return GetAsrStringImpl<&PluginDesc::name>();
}

AsrRetReadOnlyString AsrPluginInfoImpl::GetDescription()
{
    return GetAsrStringImpl<&PluginDesc::description>();
}

AsrRetReadOnlyString AsrPluginInfoImpl::GetAuthor()
{
    return GetAsrStringImpl<&PluginDesc::author>();
}

AsrRetReadOnlyString AsrPluginInfoImpl::GetVersion()
{
    return GetAsrStringImpl<&PluginDesc::version>();
}

AsrRetReadOnlyString AsrPluginInfoImpl::GetSupportedSystem()
{
    return GetAsrStringImpl<&PluginDesc::supported_system>();
}

AsrRetGuid AsrPluginInfoImpl::GetPluginIid()
{
    AsrRetGuid result{};
    result.value = sp_desc_->guid;
    result.error_code = ASR_S_OK;
    return result;
}

ASR_IMPL AsrPluginInfoVectorImpl::QueryInterface(
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

AsrRetSwigBase AsrPluginInfoVectorImpl::QueryInterface(const AsrGuid& iid)
{
    return ASR::Utils::QueryInterface<IAsrSwigPluginInfoVector>(this, iid);
}

AsrRetUInt AsrPluginInfoVectorImpl::Size()
{
    size_t     size;
    const auto error_code = Size(&size);
    return {error_code, size};
}

AsrRetPluginInfo AsrPluginInfoVectorImpl::At(size_t index)
{
    AsrRetPluginInfo result{};
    if (index < vector_.size())
    {
        result.value = vector_[index].Get();
        result.value->AddRef();
        result.error_code = ASR_S_OK;
        return result;
    }
    result.error_code = ASR_E_OUT_OF_RANGE;
    return result;
}

void AsrPluginInfoVectorImpl::AddInfo(Asr::AsrPtr<AsrPluginInfoImpl> p_info)
{
    vector_.emplace_back(std::move(p_info));
}
