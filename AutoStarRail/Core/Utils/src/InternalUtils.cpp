#include <AutoStarRail/Core/Utils/InternalUtils.h>

ASR_CORE_UTILS_NS_BEGIN

auto MakeAsrReadOnlyStringFromUtf8(std::string_view u8_string)
    -> Asr::Utils::Expected<ASR::AsrPtr<IAsrReadOnlyString>>
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

ASR_CORE_UTILS_NS_END
