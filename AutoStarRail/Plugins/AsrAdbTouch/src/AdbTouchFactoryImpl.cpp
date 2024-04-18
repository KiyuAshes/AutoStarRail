#include "AdbTouchFactoryImpl.h"
#include "AdbTouch.h"
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/StringUtils.h>
#include <boost/url.hpp>
#include <nlohmann/json.hpp>

ASR_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

struct AdbConnectionDesc
{
    std::string type{};
    std::string url{};
    std::string adbPath{};

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AdbConnectionDesc, type, url, adbPath);
};

// {6B36D95E-96D1-4642-8426-3EA0514662E6}
const AsrGuid ASR_IID_ADB_INPUT_FACTORY = {
    0x6b36d95e,
    0x96d1,
    0x4642,
    {0x84, 0x26, 0x3e, 0xa0, 0x51, 0x46, 0x62, 0xe6}};

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult AdbTouchFactory::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    ASR_UTILS_CHECK_POINTER_FOR_PLUGIN(pp_object);

    return Utils::QueryInterface<IAsrInputFactory>(this, iid, pp_object);
}

AsrResult AdbTouchFactory::GetRuntimeClassName(IAsrReadOnlyString** pp_out_name)
{
    const auto u8_name =
        ASR_UTILS_STRINGUTILS_DEFINE_U8STR("Asr::AdbInputFactory");
    return ::CreateIAsrReadOnlyStringFromUtf8(u8_name, pp_out_name);
}

AsrResult AdbTouchFactory::GetGuid(AsrGuid* p_out_guid)
{
    ASR_UTILS_CHECK_POINTER_FOR_PLUGIN(p_out_guid);

    *p_out_guid = Details::ASR_IID_ADB_INPUT_FACTORY;

    return ASR_S_OK;
}

AsrResult AdbTouchFactory::CreateInstance(
    IAsrReadOnlyString* p_json_config,
    IAsrInput**         pp_out_input)
{
    ASR_UTILS_CHECK_POINTER_FOR_PLUGIN(pp_out_input);

    const char* p_u8_json_config{nullptr};
    if (const auto get_u8_string_result =
            p_json_config->GetUtf8(&p_u8_json_config);
        IsFailed(get_u8_string_result))
    {
        return get_u8_string_result;
    }

    Details::AdbConnectionDesc connection_desc{};

    try
    {
        const auto config = nlohmann::json::parse(p_u8_json_config);

        config.at("connection").get_to(connection_desc);

        const auto adb_url = boost::url{connection_desc.url};
        if (adb_url.scheme() != std::string_view{"adb"})
        {
            const auto error_message = fmt::format(
                "Unexpected adb url. Input = {} .",
                connection_desc.url);
            ASR_LOG_ERROR(error_message.c_str());
            return ASR_E_INVALID_URL;
        }
        const auto p_result =
            new AdbTouch{connection_desc.adbPath, adb_url.authority().buffer()};
        *pp_out_input = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    catch (const nlohmann::json::exception& ex)
    {
        ASR_LOG_ERROR(
            "Can not parse json config. Error message and json dump is below:");
        ASR_LOG_ERROR(ex.what());
        ASR_LOG_ERROR(p_u8_json_config);
        return ASR_E_INTERNAL_FATAL_ERROR;
    }
    catch (const std::bad_alloc& ex)
    {
        ASR_LOG_ERROR(ex.what());
        return ASR_E_OUT_OF_MEMORY;
    }
    catch (const boost::system::error_code& ex)
    {
        const auto error_message = fmt::format(
            "Parsing url failed. Error message = {}. Input = {}",
            ex.what(),
            connection_desc.url);
        ASR_LOG_ERROR(error_message.c_str());
        return ASR_E_INVALID_URL;
    }
}

ASR_NS_END
