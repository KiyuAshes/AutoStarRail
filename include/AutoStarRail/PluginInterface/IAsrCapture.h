#ifndef ASR_ICAPTURE_H
#define ASR_ICAPTURE_H

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrString.hpp>
#include <memory>
#include <cstddef>
#include <cstdint>

ASR_INTERFACE IAsrImage;

// {69A9BDB0-4657-45B6-8ECB-E4A8F0428E95}
ASR_DEFINE_GUID(
    ASR_IID_CAPTURE,
    IAsrCapture,
    0x69a9bdb0,
    0x4657,
    0x45b6,
    0x8e,
    0xcb,
    0xe4,
    0xa8,
    0xf0,
    0x42,
    0x8e,
    0x95);
SWIG_IGNORE(IAsrCapture)
ASR_INTERFACE IAsrCapture : public IAsrBase
{
    ASR_METHOD Capture(IAsrImage * *pp_out_image) = 0;
};

// {35264072-8F42-46B5-99EA-3A83E0227CF9}
ASR_DEFINE_GUID(
    ASR_IID_CAPTURE_FACTORY,
    IAsrCaptureFactory,
    0x35264072,
    0x8f42,
    0x46b5,
    0x99,
    0xea,
    0x3a,
    0x83,
    0xe0,
    0x22,
    0x7c,
    0xf9);
SWIG_IGNORE(IAsrCaptureFactory)
ASR_INTERFACE IAsrCaptureFactory : public IAsrBase
{
    /**
     * @brief Create an instance
     *
     * @param p_json_config
     * @param pp_object
     * @return ASR_METHOD
     */
    ASR_METHOD CreateInstance(
        IAsrReadOnlyString * p_json_config,
        IAsrCapture * *pp_object) = 0;
    ASR_METHOD GetName(IAsrReadOnlyString * *pp_out_name) = 0;
};

ASR_INTERFACE IAsrSwigCaptureFactory;
ASR_RET_TYPE_DECLARE_BEGIN(AsrRetCaptureFactory)
    std::shared_ptr<IAsrSwigCaptureFactory> value;
ASR_RET_TYPE_DECLARE_END

ASR_INTERFACE IAsrSwigCapture;
ASR_RET_TYPE_DECLARE_BEGIN(AsrRetCapture)
    std::shared_ptr<IAsrSwigCapture> value;
ASR_RET_TYPE_DECLARE_END

ASR_INTERFACE IAsrSwigCaptureFactory
{
    virtual AsrRetCapture CreateInstance(AsrString json_config) = 0;
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetImage)
    std::shared_ptr<IAsrImage> value;
ASR_RET_TYPE_DECLARE_END

ASR_INTERFACE IAsrSwigCapture : public IAsrSwigBase
{
    virtual AsrRetImage Capture() = 0;
};

#ifndef SWIG

typedef enum AsrImageFormat
{
    ASR_IMAGE_FORMAT_PNG = 0,
    ASR_IMAGE_FORMAT_RGBA_8888 = 1,
    ASR_IMAGE_FORMAT_RGBX_8888 = 2,
    ASR_IMAGE_FORMAT_RGB_888 = 3,
    ASR_IMAGE_FORMAT_JPG = 4,
    ASR_IMAGE_FORMAT_FORCE_DWORD = 0x7FFFFFFF
} AsrImageFormat;

struct AsrImageDesc
{
    /**
     * @brief Pointer to the image data pointer.
     *
     */
    char* p_data;
    /**
     * @brief Size of image data in bytes.
     *
     */
    size_t data_size;
    /**
     * @brief Supported image format. @see AsrImageFormat
     *
     */
    AsrImageFormat data_format;
};

/**
 * @brief ASR Core will copy the image data.
 *
 * @param p_desc
 * @param pp_out_image
 * @return ASR_C_API
 */
ASR_C_API AsrResult
CreateIAsrImage(AsrImageDesc* p_desc, IAsrImage** pp_out_image);

#endif // SWIG

#endif // ASR_ICAPTURE_H
