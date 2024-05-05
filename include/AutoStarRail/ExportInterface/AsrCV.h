#ifndef ASR_CV_H
#define ASR_CV_H

#include <AutoStarRail/ExportInterface/IAsrImage.h>

typedef struct AsrTemplateMatchResult
{
    double  score;
    AsrRect match_rect;
} AsrTemplateMatchResult;

typedef enum AsrTemplateMatchType
{
    /**
     * @brief 归一化差的平方和（Normalized Sum of Squared Difference）
     */
    ASR_TEMPLATE_MATCH_TYPE_SQDIFF_NORMED = 1,
    /**
     * @brief 归一化互相关（Normalized Cross Correlation ）
     */
    ASR_TEMPLATE_MATCH_TYPE_CCORR_NORMED = 3,
    /**
     * @brief 归一化相关系数，即零均值归一化互相关（Zero-mean Normalized Cross
     * Correlation）
     */
    ASR_TEMPLATE_MATCH_TYPE_CCOEFF_NORMED = 5,
    ASR_TEMPLATE_MATCH_TYPE_FORCE_DWORD = 0x7FFFFFFF
} AsrTemplateMatchType;

#ifndef SWIG

ASR_C_API AsrResult TemplateMatchBest(
    IAsrImage*              p_image,
    IAsrImage*              p_template,
    AsrTemplateMatchType    type,
    AsrTemplateMatchResult* p_out_result);

#endif // SWIG

ASR_DEFINE_RET_TYPE(AsrRetTemplateMatchResult, AsrTemplateMatchResult);

ASR_API AsrRetTemplateMatchResult TemplateMatchBest(
    AsrSwigImage         image,
    AsrSwigImage         template_image,
    AsrTemplateMatchType type);

#endif // ASR_CV_H
