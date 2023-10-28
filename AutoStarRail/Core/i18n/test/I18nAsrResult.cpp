#include <AutoStarRail/Core/i18n/i18n.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/StringUtils.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

const static auto text = ASR_UTILS_STRINGUTILS_DEFINE_U8STR(R"(
{
    "type": "ASR_TYPE_INT",
    "resource":
        {
            "en" :
            {
                "-1": "Test error message."
            },
            "zh-cn" :
            {
                "-1": "测试错误"
            }
        }
}
)");

auto GetJson()
{
    const static auto result = nlohmann::json::parse(text);
    return result;
}

TEST(AsrCoreI18n, AsrResultDefaultLocaleTest)
{
    ASR::Core::i18n::I18n<AsrResult> test_instance{GetJson()};
    ASR::AsrPtr<IAsrReadOnlyString>  en_expected{};
    ::CreateIAsrReadOnlyStringFromUtf8(
        "Test error message.",
        en_expected.Put());
    ASR::AsrPtr<IAsrReadOnlyString> en_result{};
    test_instance.GetErrorMessage(-1, en_result.Put());
    ASR_CORE_LOG_INFO("{}", en_result);

    EXPECT_EQ(AsrReadOnlyString{en_expected}, AsrReadOnlyString{en_result});
}

TEST(AsrCoreI18n, AsrResultUserDefinedLocaleTest)
{
    ASR::Core::i18n::I18n<AsrResult> test_instance{GetJson()};
    ASR::AsrPtr<IAsrReadOnlyString>  zh_cn_expected{};
    ::CreateIAsrReadOnlyStringFromUtf8(
        static_cast<const char*>(
            ASR_UTILS_STRINGUTILS_DEFINE_U8STR("测试错误")),
        zh_cn_expected.Put());
    ASR::AsrPtr<IAsrReadOnlyString> zh_cn_result{};
    test_instance.GetErrorMessage(u8"zh-cn", -1, zh_cn_result.Put());
    ASR_CORE_LOG_INFO(zh_cn_result);

    EXPECT_EQ(
        AsrReadOnlyString{zh_cn_expected},
        AsrReadOnlyString{zh_cn_result});
}
