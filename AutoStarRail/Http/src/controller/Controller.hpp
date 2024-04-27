#ifndef Controller_hpp
#define Controller_hpp

#include "AutoStarRail/IAsrBase.h"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/Global.hpp"
#include "dto/Log.hpp"
#include "dto/Profile.hpp"
#include "dto/Settings.hpp"

#include "ControllerUtils.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class Controller : public oatpp::web::server::api::ApiController
{
private:
    std::shared_ptr<std::string> sp_one_message{
        std::make_shared<std::string>()};

    ASR::AsrPtr<AsrHttpLogReader> p_reader{
        new AsrHttpLogReader{sp_one_message}};

    ASR::AsrPtr<IAsrLogRequester> p_requester{};

    std::shared_ptr<ObjectMapper> jsonObjectMapper{
        oatpp::parser::json::mapping::ObjectMapper::createShared()};

public:
    Controller(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {
        ::CreateIAsrLogRequester(32, p_requester.Put());
    }

    /**
     *  定义日志相关API
     *  Define log related APIs
     */

    // 获取日志
    // Get logs

    ENDPOINT("GET", "/logs", get_logs)
    {

        auto response = Logs::createShared();
        response->code = ASR_S_OK;
        response->message = "";
        response->result = {};

        const auto error_code = p_requester->RequestOne(p_reader.Get());

        while (true)
        {
            const auto error_code = p_requester->RequestOne(p_reader.Get());

            if (error_code == ASR_S_OK)
            {
                response->result->logs->push_back(sp_one_message->data());
            }
            else if (
                !response->result->logs->empty()
                && error_code == ASR_E_OUT_OF_RANGE)
            {
                response->code = ASR_S_OK;
                break;
            }
            else
            {
                response->code = error_code;
                break;
            }
        }

        return createDtoResponse(
            Status::CODE_200,
            jsonObjectMapper->writeToString(response));
    }

    /**
     *  定义配置文件管理相关API
     *  Define profile related APIs
     */

    // 获取配置文件列表
    // Get profile list
    ENDPOINT("GET", "/api/profile/list", get_profile_list)
    {

        auto response = ProfileDescList::createShared();
        response->code = 200;
        response->message = "";

        // temp test code
        auto profile1 = ProfileDesc::createShared();
        profile1->name = reinterpret_cast<const char*>(u8"配置0");
        profile1->pid = "0";
        profile1->is_run = true;

        auto profile2 = ProfileDesc::createShared();
        profile2->name = reinterpret_cast<const char*>(u8"様式ファイル1");
        profile2->pid = "1";
        profile2->is_run = false;

        auto profile3 = ProfileDesc::createShared();
        profile3->name = reinterpret_cast<const char*>(
            u8"様式ファイル2 長さが制限を超えています");
        profile3->pid = "2";
        profile3->is_run = false;

        response->result = {profile1, profile2, profile3};
        // temp test code

        return createDtoResponse(
            Status::CODE_200,
            jsonObjectMapper->writeToString(response));
    }

    /**
     *  定义设置相关API
     *  Define settings related APIs
     */

    // 获取应用列表
    // Get app list
    ENDPOINT("GET", "/api/settings/app", get_app_list)
    {

        auto response = AppDescList::createShared();
        response->code = 200;
        response->message = "";

        // temp test code
        auto app1 = AppDesc::createShared(); // AzurPromilia
        app1->name = reinterpret_cast<const char*>(u8"蓝色星原-国服");
        app1->package_name = "com.manjuu.azurpromilia";

        auto app2 = AppDesc::createShared(); // Resonance
        app2->name = reinterpret_cast<const char*>(u8"雷索纳斯-国服");
        app1->package_name = "com.hermes.goda";

        response->result = {app1, app2};
        // temp test code

        return createDtoResponse(
            Status::CODE_200,
            jsonObjectMapper->writeToString(response));
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif
