#ifndef ASR_CORE_OCVWRAPPER_IASRIMAGEIMPL_H
#define ASR_CORE_OCVWRAPPER_IASRIMAGEIMPL_H

#include "Config.h"

#include <AutoStarRail/ExportInterface/IAsrImage.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>

ASR_DISABLE_WARNING_BEGIN

ASR_IGNORE_OPENCV_WARNING
#include <opencv2/core/mat.hpp>

ASR_DISABLE_WARNING_END

// {911CF30B-352D-4979-9C9C-DF7AF97362DF}
ASR_DEFINE_CLASS_IN_NAMESPACE(
    Asr::Core::OcvWrapper,
    IAsrImageImpl,
    0x911cf30b,
    0x352d,
    0x4979,
    0x9c,
    0x9c,
    0xdf,
    0x7a,
    0xf9,
    0x73,
    0x62,
    0xdf);

ASR_CORE_OCVWRAPPER_NS_BEGIN

class IAsrImageImpl final : public IAsrImage
{
    AsrPtr<IAsrMemory> p_memory_;
    cv::Mat            mat_;

    ASR_UTILS_IASRBASE_AUTO_IMPL(IAsrImageImpl);

public:
    /**
     * @brief 参数校验由外部完成！
     * @param height
     * @param width
     * @param type
     * @param p_data
     * @param p_asr_data
     */
    IAsrImageImpl(
        int         height,
        int         width,
        int         type,
        void*       p_data,
        IAsrMemory* p_asr_data);

    IAsrImageImpl(cv::Mat mat);

    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    ASR_IMPL GetSize(AsrSize* p_out_size) override;
    ASR_IMPL GetChannelCount(int* p_out_channel_count) override;
    ASR_IMPL Clip(const AsrRect* p_rect, IAsrImage** pp_out_image) override;
    ASR_IMPL GetDataSize(size_t* p_out_size) override;
    ASR_IMPL CopyTo(unsigned char* p_out_memory) override;

    auto GetImpl() -> cv::Mat;
};

ASR_CORE_OCVWRAPPER_NS_END

#endif // ASR_CORE_OCVWRAPPER_IASRIMAGEIMPL_H
