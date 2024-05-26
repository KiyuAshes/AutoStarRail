#ifndef ASR_CORE_FOREIGNINTERFACEHOST_TEMPORARYPLUGINOBJECTSTORAGE_H
#define ASR_CORE_FOREIGNINTERFACEHOST_TEMPORARYPLUGINOBJECTSTORAGE_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <mutex>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class TemporaryPluginObjectStorage
{
    std::mutex             mutex_{};
    AsrPtr<IAsrSwigPlugin> p_plugin_{};

    friend AsrResult(::AsrRegisterPluginObject)(
        AsrResult       error_code,
        IAsrSwigPlugin* p_swig_plugin);

    void ObtainOwnership();
    void ReleaseOwnership();

public:
    class TemporaryPluginObjectStorageReader
    {
        TemporaryPluginObjectStorage& storage_;

    public:
        TemporaryPluginObjectStorageReader(
            TemporaryPluginObjectStorage& storage);
        ~TemporaryPluginObjectStorageReader();

        auto GetObject() -> AsrPtr<IAsrSwigPlugin>;
    };

    friend TemporaryPluginObjectStorageReader;

    auto GetOwnership() -> TemporaryPluginObjectStorageReader;
};

extern TemporaryPluginObjectStorage g_plugin_object;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_TEMPORARYPLUGINOBJECTSTORAGE_H
