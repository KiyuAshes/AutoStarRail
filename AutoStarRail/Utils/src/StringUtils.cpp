#include <AutoStarRail/Utils/StringUtils.h>
#include <algorithm>

ASR_UTILS_NS_BEGIN

void ToLowerInPlace(std::string& in_out_str)
{
    std::transform(
        in_out_str.begin(),
        in_out_str.end(),
        in_out_str.begin(),
        [](unsigned char c) { return std::tolower(c); });
}

void ToLowerInPlace(std::vector<std::string>& in_out_str_vector)
{
    for (auto& str : in_out_str_vector)
    {
        ToLowerInPlace(str);
    }
}

auto MakeAsrReadOnlyStringFromUtf8(std::string_view u8_string)
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>
{
    IAsrReadOnlyString* p_result{};
    const auto          error_code =
        ::CreateIAsrReadOnlyStringFromUtf8(u8_string.data(), &p_result);

    if (IsOk(error_code))
    {
        AsrPtr<IAsrReadOnlyString> result{};
        *result.Put() = p_result;
        return result;
    }

    return ASR::Utils::MakeUnexpected(error_code);
}

ASR_UTILS_NS_END
