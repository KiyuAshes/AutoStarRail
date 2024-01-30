#ifndef ASR_IAMGE_H
#define ASR_IAMGE_H

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/AsrPtr.hpp>

#ifndef SWIG

// {6C98E79F-2342-4B98-AC8A-2B29EA53F951}
ASR_DEFINE_GUID(
    ASR_IID_IMAGE,
    IAsrImage,
    0x6c98e79f,
    0x2342,
    0x4b98,
    0xac,
    0x8a,
    0x2b,
    0x29,
    0xea,
    0x53,
    0xf9,
    0x51);
ASR_INTERFACE IAsrImage : public IAsrBase
{
    ASR_METHOD GetWidth(int* p_width);
    ASR_METHOD GetHeight(int* p_height);
    ASR_METHOD Clip(IAsrImage * *p_out_image);
};

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

ASR_C_API AsrResult CreateIAsrImageFromRgba8888(
    ASR_INTERFACE IAsrMemory* p_alias_memory,
    IAsrImage**               pp_out_image);

#endif // SWIG

class AsrSwigImage
{
    ASR::AsrPtr<IAsrImage> p_image_;

public:
    ASR_API AsrSwigImage();
#ifndef SWIG
    AsrSwigImage(Asr::AsrPtr<IAsrImage> p_image);
#endif // SWIG
};

#endif // ASR_IAMGE_H
