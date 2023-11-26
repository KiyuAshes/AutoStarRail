#ifndef ASR_CORE_FOREIGNINTERFACEHOST_TEMPORARYPLUGINOBJECTSTORAGE_H
#define ASR_CORE_FOREIGNINTERFACEHOST_TEMPORARYPLUGINOBJECTSTORAGE_H

#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <mutex>
#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class TemporaryPluginObjectStorage
{
    std::mutex      mutex_{};
    CommonPluginPtr p_plugin_{};

    friend AsrResult(::AsrRegisterPluginObject)(
        AsrRetSwigBase result_and_p_object);

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

        auto GetObject() -> CommonPluginPtr;
    };

    friend TemporaryPluginObjectStorageReader;

    auto GetOwnership() -> TemporaryPluginObjectStorageReader;
};

extern TemporaryPluginObjectStorage g_plugin_object;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_TEMPORARYPLUGINOBJECTSTORAGE_H
