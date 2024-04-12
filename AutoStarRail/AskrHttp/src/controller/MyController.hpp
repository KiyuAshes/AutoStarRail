#ifndef MyController_hpp
#define MyController_hpp

#include "dto/DTOs.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"


#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin Codegen

/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController
{
public:
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize
     * DTOs.
     */
    MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {
    }

    ENDPOINT("GET", "/api/getprofile", root)
    {
        auto jsonObjectMapper =
            oatpp::parser::json::mapping::ObjectMapper::createShared();

        auto dto = ProfileList::createShared();
        dto->code = 200;
        dto->message = "Hello World!";

        auto profile1 = ProfileListItem::createShared();
        profile1->name = "配置1";
        profile1->path = "config1";

        auto profile2 = ProfileListItem::createShared();
        profile2->name = "配置2";
        profile2->path = "config2";

        dto->data = {profile1, profile2};

        oatpp::String json = jsonObjectMapper->writeToString(dto);


        return createDtoResponse(Status::CODE_200, json);
    }

    // TODO Insert Your endpoints here !!!
};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif /* MyController_hpp */
