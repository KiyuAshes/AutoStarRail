#include <AutoStarRail/ExportInterface/IAsrImage.h>

AsrSwigImage::AsrSwigImage() = default;

AsrSwigImage::AsrSwigImage(ASR::AsrPtr<IAsrImage> p_image)
    : p_image_{std::move(p_image)}
{
}