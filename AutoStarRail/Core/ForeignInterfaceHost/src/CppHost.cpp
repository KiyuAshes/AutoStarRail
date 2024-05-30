#include "CppHost.h"
#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <boost/dll/shared_library.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_CPPHOST_BEGIN

class CppRuntime final : public IForeignLanguageRuntime
{
    ASR::Utils::RefCounter<CppRuntime> ref_counter_{};
    boost::dll::shared_library         plugin_lib_{};

public:
    int64_t   AddRef() override { return ref_counter_.AddRef(); }
    int64_t   Release() override { return ref_counter_.Release(this); }
    AsrResult QueryInterface(const AsrGuid&, void**) override
    {
        return ASR_E_NO_IMPLEMENTATION;
    }
    auto LoadPlugin(const std::filesystem::path& path)
        -> ASR::Utils::Expected<CommonPluginPtr> override
    {
        try
        {
            plugin_lib_.load(path.c_str());
            // Get function pointer without heap allocation.
            const auto& p_init_function =
                plugin_lib_.get<::AsrCoCreatePluginFunction>(
                    ASRCOCREATEPLUGIN_NAME);
            AsrPtr<IAsrPlugin> p_plugin{};
            const auto         error_code = p_init_function(p_plugin.Put());
            if (ASR::IsOk(error_code))
            {
                return p_plugin;
            }
            return tl::make_unexpected(error_code);
        }
        catch (const boost::wrapexcept<boost::system::system_error>& ex)
        {
            ASR_CORE_LOG_EXCEPTION(ex);
            return tl::make_unexpected(ASR_E_SYMBOL_NOT_FOUND);
        }
    }
};

auto CreateForeignLanguageRuntime(const ForeignLanguageRuntimeFactoryDesc&)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>
{
    return MakeAsrPtr<IForeignLanguageRuntime, CppRuntime>();
}

ASR_NS_CPPHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
