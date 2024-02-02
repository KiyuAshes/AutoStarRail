#include "AsrOrt.h"
#include <AutoStarRail/AsrString.hpp>
#include <boost/predef/os.h>

ASR_CORE_ORTWRAPPER_NS_BEGIN

const ORTCHAR_T* ToOrtChar(AsrReadOnlyString string)
{
#if (BOOST_OS_WINDOWS)
    return string.GetW();
#else
    return string.GetUtf8();
#endif
}

const ORTCHAR_T* ToOrtChar(IAsrReadOnlyString* p_string)
{
    auto string = AsrReadOnlyString(p_string);
    return ToOrtChar(string);
}

Ort::MemoryInfo& AsrOrt::GetDefaultCpuMemoryInfo()
{
    static auto result = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator,
        OrtMemType::OrtMemTypeCPU);
    return result;
}

AsrOrt::AsrOrt(const char* model_name)
    : env_{ORT_LOGGING_LEVEL_WARNING, model_name}
{
}

ASR_CORE_ORTWRAPPER_NS_END