#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>

AsrRetUInt AsrSwigPluginInfoVector::Size()
{
    AsrRetUInt result{};
    result.error_code = p_vector->Size(&result.value);
    return result;
}

AsrRetPluginInfo AsrSwigPluginInfoVector::At(size_t index)
{
    return ASR::Core::ForeignInterfaceHost::CallCppMethod<
        AsrRetPluginInfo,
        IAsrPluginInfo,
        ASR_DV_V(&IAsrPluginInfoVector::At)>(p_vector.Get(), index);
}
