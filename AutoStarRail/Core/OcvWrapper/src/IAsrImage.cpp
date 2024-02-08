#include "Config.h"
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/ExportInterface/IAsrImage.h>
#include <AutoStarRail/ExportInterface/IAsrMemory.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <vector>
#include <fstream>
#include <utility>
#include <filesystem>

ASR_DISABLE_WARNING_BEGIN

ASR_IGNORE_OPENCV_WARNING
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

ASR_DISABLE_WARNING_END

AsrSwigImage::AsrSwigImage() = default;

AsrSwigImage::AsrSwigImage(ASR::AsrPtr<IAsrImage> p_image)
    : p_image_{std::move(p_image)}
{
}

ASR_CORE_OCVWRAPPER_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

auto ToOcvType(AsrImageFormat format) -> ASR::Utils::Expected<int>
{
    switch (format)
    {
    case ASR_IMAGE_FORMAT_RGB_888:
        return CV_8UC3;
    case ASR_IMAGE_FORMAT_RGBA_8888:
        [[fallthrough]];
    case ASR_IMAGE_FORMAT_RGBX_8888:
        return CV_8UC4;
    default:
        return tl::make_unexpected(ASR_E_INVALID_ENUM);
    }
}

ASR_NS_ANONYMOUS_DETAILS_END

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
        IAsrMemory* p_asr_data)
        : p_memory_{p_asr_data, take_ownership},
          mat_{height, width, type, p_data}
    {
    }

    IAsrImageImpl(cv::Mat mat) : p_memory_{}, mat_{std::move(mat)} {}

    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_out_object) override
    {
        return ASR::Utils::QueryInterface<IAsrImage>(this, iid, pp_out_object);
    }

    ASR_IMPL GetSize(AsrSize* p_out_size) override
    {
        ASR_UTILS_CHECK_POINTER(p_out_size)

        p_out_size->width = mat_.cols;
        p_out_size->height = mat_.rows;

        return ASR_S_OK;
    }

    ASR_IMPL GetChannelCount(int* p_out_channel_count) override
    {
        ASR_UTILS_CHECK_POINTER(p_out_channel_count)

        *p_out_channel_count = mat_.channels();

        return ASR_S_OK;
    }

    ASR_IMPL Clip(const AsrRect* p_rect, IAsrImage** pp_out_image) override
    {
        ASR_UTILS_CHECK_POINTER(p_rect)
        ASR_UTILS_CHECK_POINTER(pp_out_image)

        try
        {
            const auto& rect = *p_rect;
            const auto  clipped_mat = mat_(
                cv::Range(rect.left_top_y, rect.right_bottom_y),
                cv::Range(rect.left_top_x, rect.right_bottom_x));
            auto p_result = new IAsrImageImpl{clipped_mat};
            p_result->p_memory_ = p_memory_;
            p_result->AddRef();
            *pp_out_image = p_result;
            return ASR_S_OK;
        }
        catch (std::bad_alloc&)
        {
            return ASR_E_OUT_OF_MEMORY;
        }
    }

    ASR_IMPL GetDataSize(size_t* p_out_size) override
    {
        ASR_UTILS_CHECK_POINTER(p_out_size)

        size_t result = mat_.total();
        result *= mat_.elemSize1();
        *p_out_size = result;

        return ASR_S_OK;
    }

    ASR_IMPL CopyTo(unsigned char* p_out_memory) override
    {
        ASR_UTILS_CHECK_POINTER(p_out_memory)

        try
        {
            size_t data_size;
            GetDataSize(&data_size);
            const auto int_data_size = static_cast<int>(data_size);
            mat_.copyTo({p_out_memory, int_data_size});
        }
        catch (cv::Exception& ex)
        {
            ASR_CORE_LOG_ERROR(ex.err);
            ASR_CORE_LOG_ERROR(
                "NOTE:\nfile = {}\nline = {}\nfunction = {}",
                ex.file,
                ex.line,
                ex.func);
        }

        return ASR_S_OK;
    }
};

ASR_CORE_OCVWRAPPER_NS_END

AsrResult CreateIAsrImageFromEncodedData(
    AsrImageDesc* p_desc,
    IAsrImage**   pp_out_image)
{
    ASR_UTILS_CHECK_POINTER(p_desc)
    ASR_UTILS_CHECK_POINTER(pp_out_image)

    auto& desc = *p_desc;

    const auto int_data_size = static_cast<int>(desc.data_size);
    switch (desc.data_format)
    {
    case ASR_IMAGE_FORMAT_JPG:
        [[fallthrough]];
    case ASR_IMAGE_FORMAT_PNG:
    {
        if (desc.data_size == 0)
        {
            return ASR_E_INVALID_SIZE;
        }

        auto mat =
            cv::imdecode({desc.p_data, int_data_size}, cv::IMREAD_UNCHANGED);
        cv::Mat rgb_mat{};
        cv::cvtColor(mat, rgb_mat, cv::COLOR_BGR2RGB);

        auto p_result = new ASR::Core::OcvWrapper::IAsrImageImpl{rgb_mat};
        p_result->AddRef();
        *pp_out_image = p_result;
        return ASR_S_OK;
    }
    default:
        return ASR_E_INVALID_ENUM;
    }
}

AsrResult CreateIAsrImageFromDecodedData(
    AsrImageDesc* p_desc,
    AsrSize*      p_size,
    IAsrImage**   pp_out_image)
{
    ASR_UTILS_CHECK_POINTER(p_desc)
    ASR_UTILS_CHECK_POINTER(p_size)
    ASR_UTILS_CHECK_POINTER(pp_out_image)

    auto&      desc = *p_desc;
    auto&      size = *p_size;
    const auto expected_type =
        ASR::Core::OcvWrapper::Details::ToOcvType(desc.data_format);

    if (!expected_type)
    {
        return expected_type.error();
    }

    cv::Mat input_image{
        size.height,
        size.width,
        expected_type.value(),
        desc.p_data};
    auto owned_image = input_image.clone();

    auto p_result = new ASR::Core::OcvWrapper::IAsrImageImpl{owned_image};
    p_result->AddRef();
    *pp_out_image = p_result;

    return ASR_S_OK;
}

AsrResult CreateIAsrImageFromRgb888(
    struct IAsrMemory* p_alias_memory,
    AsrSize*           p_size,
    IAsrImage**        pp_out_image)
{
    ASR_UTILS_CHECK_POINTER(p_alias_memory)
    ASR_UTILS_CHECK_POINTER(p_size)
    ASR_UTILS_CHECK_POINTER(pp_out_image)

    const auto&    size = *p_size;
    size_t         data_size;
    unsigned char* p_data;

    if (const auto get_size_result = p_alias_memory->GetSize(&data_size);
        ASR::IsFailed(get_size_result)) [[unlikely]]
    {
        return get_size_result;
    }

    if (const auto get_pointer_result = p_alias_memory->GetData(&p_data);
        ASR::IsFailed(get_pointer_result)) [[unlikely]]
    {
        return get_pointer_result;
    }

    const size_t required_size = std::abs(size.height * size.width * 4);
    if (required_size > data_size)
    {
        return ASR_E_OUT_OF_RANGE;
    }

    auto p_result = new ASR::Core::OcvWrapper::IAsrImageImpl{
        size.height,
        size.width,
        CV_8UC4,
        p_data,
        p_alias_memory};
    p_result->AddRef();
    *pp_out_image = p_result;

    return ASR_S_OK;
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

// TODO: 实现下面函数
//auto ResourceRelativePathToFullPath(IAsrReadOnlyString* p_read_only_string) {}

ASR_NS_ANONYMOUS_DETAILS_END

// TODO: 实现下面函数
//AsrResult AsrPluginLoadImageFromResource(
//    IAsrReadOnlyString* p_relative_path,
//    IAsrImage**         pp_out_image)
//{
//}
//
//AsrRetImage AsrPluginLoadImageFromResource(AsrReadOnlyString relative_path) {}
