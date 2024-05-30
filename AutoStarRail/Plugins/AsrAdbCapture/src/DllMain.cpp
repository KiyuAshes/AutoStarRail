#define ASR_BUILD_SHARED

#include "PluginImpl.h"
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>

ASR_C_API AsrResult AsrCoCreatePlugin(IAsrPlugin** pp_out_plugin)
{
    ASR_UTILS_CHECK_POINTER_FOR_PLUGIN(pp_out_plugin)
    try
    {
        const auto p_result = new ASR::AdbCapturePlugin{};
        *pp_out_plugin = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        ASR_LOG_ERROR("Out of memory");
        return ASR_E_OUT_OF_MEMORY;
    }
    catch (...)
    {
        return ASR_E_INTERNAL_FATAL_ERROR;
    }
}