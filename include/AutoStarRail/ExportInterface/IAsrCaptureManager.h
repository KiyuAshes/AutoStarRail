#ifndef ASR_ASRCAPTUREMANAGER_H
#define ASR_ASRCAPTUREMANAGER_H

#include "AutoStarRail/AsrString.hpp"
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/IAsrBase.h>

// {9ED8685E-050E-4FF5-9E6C-2A2C25CAC117}
ASR_DEFINE_GUID(
    ASR_IID_CAPTURE_MANAGER,
    IAsrCaptureManager,
    0x9ed8685e,
    0x50e,
    0x4ff5,
    0x9e,
    0x6c,
    0x2a,
    0x2c,
    0x25,
    0xca,
    0xc1,
    0x17);
ASR_INTERFACE IAsrCaptureManager : public IAsrBase
{
    ASR_METHOD EnumCaptureLoadErrorState(
        const size_t         index,
        AsrResult*           p_error_code,
        IAsrReadOnlyString** pp_out_error_explanation) = 0;
    /**
     * @brief Enumerates all interfaces.
     * @param index
     * @param pp_out_interface
     * @return ASR_S_OK if interface is valid. Otherwise return error code which
     * is created by IAsrCaptureFactory.
     */
    ASR_METHOD EnumCaptureInterface(
        const size_t  index,
        IAsrCapture** pp_out_interface) = 0;
};

ASR_INTERFACE IAsrSwigCaptureManager : public IAsrSwigBase
{
    virtual AsrRetCapture EnumCaptureInterface(const size_t index) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetCaptureManager)
    std::shared_ptr<IAsrSwigCaptureManager> value;
ASR_RET_TYPE_DECLARE_END

ASR_API AsrRetCaptureManager CreateAsrCaptureManager(AsrString json_config);

ASR_C_API AsrResult CreateIAsrCaptureManager(
    IAsrReadOnlyString*  p_json_config,
    IAsrCaptureManager** pp_out_capture_manager);

#endif // ASR_ASRCAPTUREMANAGER_H
