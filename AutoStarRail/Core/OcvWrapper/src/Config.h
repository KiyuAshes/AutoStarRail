#ifndef ASR_CORE_OCVWRAPPER_CONFIG_H
#define ASR_CORE_OCVWRAPPER_CONFIG_H

#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/ExportInterface/IAsrImage.h>

ASR_DISABLE_WARNING_BEGIN

ASR_IGNORE_OPENCV_WARNING
#include <opencv2/core/types.hpp>

ASR_DISABLE_WARNING_END

#define ASR_CORE_OCVWRAPPER_NS_BEGIN                                           \
    ASR_NS_BEGIN namespace Core                                                \
    {                                                                          \
        namespace OcvWrapper                                                   \
        {

#define ASR_CORE_OCVWRAPPER_NS_END                                             \
    }                                                                          \
    }                                                                          \
    ASR_NS_END

ASR_CORE_OCVWRAPPER_NS_BEGIN

inline auto ToMat(AsrRect rect) -> cv::Rect
{
    return {rect.x, rect.y, rect.width, rect.height};
}

ASR_CORE_OCVWRAPPER_NS_END

#endif // ASR_CORE_OCVWRAPPER_CONFIG_H
