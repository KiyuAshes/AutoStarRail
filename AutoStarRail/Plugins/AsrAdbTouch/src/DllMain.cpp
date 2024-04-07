
#define ASR_BUILD_SHARED

#include "PluginImpl.h"
#include <new>

ASR_C_API AsrResult AsrCoCreatePlugin(IAsrPlugin** pp_out_plugin)
{
    if (pp_out_plugin == nullptr)
    {
        return ASR_E_INVALID_POINTER;
    }

    try
    {
        const auto p_result = new ASR::AsrAdbTouchPlugin{};
        *pp_out_plugin = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}