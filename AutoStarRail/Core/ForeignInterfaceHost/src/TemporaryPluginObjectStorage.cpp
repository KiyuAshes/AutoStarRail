#include "TemporaryPluginObjectStorage.h"

Asr::Core::ForeignInterfaceHost::TemporaryPluginObjectStorage::
    TemporaryPluginObjectStorageReader::TemporaryPluginObjectStorageReader(
        TemporaryPluginObjectStorage& storage)
    : storage_{storage}
{
    storage_.ObtainOwnership();
}

Asr::Core::ForeignInterfaceHost::TemporaryPluginObjectStorage::
    TemporaryPluginObjectStorageReader::~TemporaryPluginObjectStorageReader()
{
    storage_.ReleaseOwnership();
}

auto Asr::Core::ForeignInterfaceHost::TemporaryPluginObjectStorage::
    TemporaryPluginObjectStorageReader::GetObject() -> Asr::AsrPtr<IAsrPlugin>
{
    return storage_.p_plugin_;
}

auto Asr::Core::ForeignInterfaceHost::TemporaryPluginObjectStorage::
    GetOwnership() -> Asr::Core::ForeignInterfaceHost::
        TemporaryPluginObjectStorage::TemporaryPluginObjectStorageReader
{
    return {*this};
}

void Asr::Core::ForeignInterfaceHost::TemporaryPluginObjectStorage::
    ObtainOwnership()
{
    mutex_.lock();
}

void Asr::Core::ForeignInterfaceHost::TemporaryPluginObjectStorage::
    ReleaseOwnership()
{
    p_plugin_ = {};
    mutex_.unlock();
}

