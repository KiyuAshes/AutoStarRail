#include "Plugin.h"

#include "ForeignInterfaceHost.h"
#include <utility>

#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/QueryInterface.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

Plugin::Plugin(
    AsrPtr<IForeignLanguageRuntime> p_runtime,
    CommonPluginPtr                 p_plugin,
    std::unique_ptr<PluginDesc>     up_desc)
    : p_runtime_{std::move(p_runtime)}, p_plugin_{std::move(p_plugin)},
      sp_desc_{std::move(up_desc)}, load_state_{ASR_S_OK}, load_error_message_{}
{
}

Plugin::Plugin(AsrResult load_state, IAsrReadOnlyString* p_error_message)
    : load_state_{load_state},
      load_error_message_{p_error_message, take_ownership}
{
}

Plugin::~Plugin() = default;

Plugin::operator bool() const noexcept { return IsOk(load_state_); }

ASR_NS_ANONYMOUS_DETAILS_BEGIN

class AsrPluginInfoImpl final : public IAsrPluginInfo
{
    template <auto MemberPointer>
    ASR_IMPL GetStringImpl(IAsrReadOnlyString** pp_out_string)
    {
        ASR_UTILS_CHECK_POINTER(pp_out_string)
        return ::CreateIAsrReadOnlyStringFromUtf8(
            (sp_desc_.get()->*MemberPointer).c_str(),
            pp_out_string);
    }

public:
    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrPluginInfoImpl);
    AsrPluginInfoImpl(std::shared_ptr<PluginDesc> sp_desc)
        : sp_desc_{std::move(sp_desc)}
    {
    }

    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override
    {
        return ASR::Utils::QueryInterface<IAsrPluginInfo>(this, iid, pp_object);
    }
    ASR_IMPL GetName(IAsrReadOnlyString** pp_out_name) override
    {
        return GetStringImpl<&PluginDesc::name>(pp_out_name);
    }
    ASR_IMPL GetDescription(IAsrReadOnlyString** pp_out_description) override
    {
        return GetStringImpl<&PluginDesc::description>(pp_out_description);
    }
    ASR_IMPL GetAuthor(IAsrReadOnlyString** pp_out_author) override
    {
        return GetStringImpl<&PluginDesc::author>(pp_out_author);
    }
    ASR_IMPL GetVersion(IAsrReadOnlyString** pp_out_version) override
    {
        return GetStringImpl<&PluginDesc::version>(pp_out_version);
    }
    ASR_IMPL GetSupportedSystem(
        IAsrReadOnlyString** pp_out_supported_system) override
    {
        return GetStringImpl<&PluginDesc::supported_system>(
            pp_out_supported_system);
    }
    ASR_IMPL GetPluginIid(AsrGuid* p_out_guid) override
    {
        ASR_UTILS_CHECK_POINTER(p_out_guid)
        *p_out_guid = sp_desc_->guid;
        return ASR_S_OK;
    }

private:
    std::shared_ptr<PluginDesc> sp_desc_;
};

ASR_NS_ANONYMOUS_DETAILS_END

auto Plugin::GetInfo() const -> AsrPtr<IAsrPluginInfo>
{
    return MakeAsrPtr<IAsrPluginInfo, Details::AsrPluginInfoImpl>(sp_desc_);
}

Plugin::Plugin(Plugin&& other) noexcept
    : p_runtime_{std::move(other.p_runtime_)},
      p_plugin_{std::move(other.p_plugin_)},
      sp_desc_{std::move(other.sp_desc_)},
      load_state_{
          std::exchange(other.load_state_, ASR_E_UNDEFINED_RETURN_VALUE)},
      load_error_message_{std::move(other.load_error_message_)}
{
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
