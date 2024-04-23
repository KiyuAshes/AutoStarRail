#ifndef Controller_hpp
#define Controller_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/Global.hpp"
#include "dto/Profile.hpp"

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

    // 定义配置文件管理相关API
    // Define profile management related APIs
    ENDPOINT("GET", "/api/getProfileDescList", get_profile_desc_list)
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

        response->data = {profile1, profile2, profile3};
        // temp test code

        return createDtoResponse(
            Status::CODE_200,
            jsonObjectMapper->writeToString(response));
    }
};

#include OATPP_CODEGEN_END(ApiController)

#endif
