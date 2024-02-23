#ifndef ASR_IMAGE_H
#define ASR_IMAGE_H

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/IAsrBase.h>

typedef enum AsrImageFormat
{
    ASR_IMAGE_FORMAT_PNG = 0,
    ASR_IMAGE_FORMAT_RGBA_8888 = 1,
    ASR_IMAGE_FORMAT_RGBX_8888 = 2,
    ASR_IMAGE_FORMAT_RGB_888 = 3,
    ASR_IMAGE_FORMAT_JPG = 4,
    ASR_IMAGE_FORMAT_FORCE_DWORD = 0x7FFFFFFF
} AsrImageFormat;

struct AsrSize
{
    int32_t width;
    int32_t height;
};

struct AsrRect
{
    int32_t left_top_x;
    int32_t left_top_y;
    int32_t right_bottom_x;
    int32_t right_bottom_y;
};

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
    ASR_METHOD GetSize(AsrSize * p_out_size) = 0;
    ASR_METHOD GetChannelCount(int* p_out_channel_count) = 0;
    ASR_METHOD Clip(const AsrRect* p_rect, IAsrImage** p_out_image) = 0;
    ASR_METHOD GetDataSize(size_t * p_out_size) = 0;
    ASR_METHOD CopyTo(unsigned char* p_out_memory) = 0;
};

struct AsrImageDesc
{
    /**
     * @brief Pointer to the image data pointer.
     *
     */
    char* p_data;
    /**
     * @brief Size of image data in bytes. Can be 0 when both width and height
     * are set and data is decoded.
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
CreateIAsrImageFromEncodedData(AsrImageDesc* p_desc, IAsrImage** pp_out_image);

ASR_C_API AsrResult CreateIAsrImageFromDecodedData(
    AsrImageDesc* p_desc,
    AsrSize*      p_size,
    IAsrImage**   pp_out_image);

ASR_C_API AsrResult CreateIAsrImageFromRgb888(
    ASR_INTERFACE IAsrMemory* p_alias_memory,
    AsrSize*                  p_size,
    IAsrImage**               pp_out_image);

ASR_C_API AsrResult AsrPluginLoadImageFromResource(
    ASR_INTERFACE IAsrTypeInfo* p_type_info,
    IAsrReadOnlyString*         p_relative_path,
    IAsrImage**                 pp_out_image);

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

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetImage)
    AsrSwigImage value;
ASR_RET_TYPE_DECLARE_END

ASR_API AsrRetImage AsrPluginLoadImageFromResource(
    ASR_INTERFACE IAsrSwigTypeInfo* p_type_info,
    AsrReadOnlyString               relative_path);

#endif // ASR_IMAGE_H
