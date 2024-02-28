#include "Config.h"
#include "IAsrImageImpl.h"
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/ExportInterface/AsrCV.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/Timer.hpp>

ASR_DISABLE_WARNING_BEGIN

ASR_IGNORE_OPENCV_WARNING
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

ASR_DISABLE_WARNING_END

ASR_CORE_OCVWRAPPER_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

auto GetAsrImageImpl(IAsrImage* p_image)
    -> ASR::Utils::Expected<AsrPtr<IAsrImageImpl>>
{
    AsrPtr<IAsrImageImpl> p_result{};

    if (const auto qi_result = p_image->QueryInterface(
            AsrIidOf<IAsrImageImpl>(),
            p_result.PutVoid());
        IsFailed(qi_result))
    {
        ASR_CORE_LOG_ERROR(
            "Can not find interface Asr::Core::OcvWrapper::IAsrImageImpl in IAsrImage object. Pointer = {}.",
            Utils::VoidP(p_image));
        return tl::make_unexpected(qi_result);
    }

    return p_result;
}

ASR_NS_ANONYMOUS_DETAILS_END

ASR_CORE_OCVWRAPPER_NS_END

AsrResult TemplateMatchBest(
    IAsrImage*              p_image,
    IAsrImage*              p_template,
    AsrTemplateMatchType    type,
    AsrTemplateMatchResult* p_out_result)
{
    ASR_UTILS_CHECK_POINTER(p_out_result)

    const auto expected_p_image =
        ASR::Core::OcvWrapper::Details::GetAsrImageImpl(p_image);

    if (!expected_p_image)
    {
        return expected_p_image.error();
    }

    const auto expected_p_template =
        ASR::Core::OcvWrapper::Details::GetAsrImageImpl(p_template);

    if (!expected_p_template)
    {
        return expected_p_template.error();
    }

    const auto& image_mat = expected_p_image.value()->GetImpl();
    const auto& template_mat = expected_p_template.value()->GetImpl();

    ASR::Utils::Timer timer{};
    timer.Begin();

    // 由于已经限制了type，可以使用的都是规范化后的值，所以输出应当出于0-1的区间
    double    min_score = 0.0;
    double    max_score = 0.0;
    cv::Point min_location{};
    cv::Point max_location{};
    cv::Mat   output;
    cv::matchTemplate(
        image_mat,
        template_mat,
        output,
        ASR::Utils::ToUnderlying(type));
    cv::minMaxLoc(output, &min_score, &max_score, &min_location, &max_location);

    const auto cv_cost = timer.End();
    ASR_CORE_LOG_INFO("Function matchTemplate and minMaxLoc cost {} ms.", cv_cost);

    if (std::isnan(max_score) || std::isinf(max_score))
    {
        max_score = 0;
    }

    cv::Point matched_location{};
    double    score{};
    if (type == ASR_TEMPLATE_MATCH_TYPE_SQDIFF_NORMED)
    {
        matched_location = min_location;
        score = 1 - min_score;
    }
    else
    {
        matched_location = max_location;
        score = max_score;
    }

    p_out_result->match_rect = AsrRect{
        matched_location.x,
        matched_location.y,
        template_mat.cols,
        template_mat.rows};
    p_out_result->score = score;

    return ASR_S_OK;
}

AsrRetTemplateMatchResult TemplateMatchBest(
    AsrSwigImage         image,
    AsrSwigImage         template_image,
    AsrTemplateMatchType type)
{
    auto* const p_image = image.Get();
    auto* const p_template = template_image.Get();

    AsrRetTemplateMatchResult result;
    result.error_code =
        TemplateMatchBest(p_image, p_template, type, &result.value);

    return result;
}