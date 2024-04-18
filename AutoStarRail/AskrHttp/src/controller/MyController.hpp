#ifndef MyController_hpp
#define MyController_hpp

#include "AutoStarRail/AsrPtr.hpp"
#include "AutoStarRail/IAsrBase.h"
#include "dto/DTOs.hpp"

#include "AutoStarRail/AsrString.hpp"
#include "AutoStarRail/ExportInterface/AsrLogger.h"
#include "AutoStarRail/Utils/CommonUtils.hpp"
#include "AutoStarRail/Utils/QueryInterface.hpp"
#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include <memory>

// #include "oatpp/encoding/Unicode.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin Codegen

/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController
{
    std::shared_ptr<std::string> sp_one_message{};
    class AsrHttpLogReader : public IAsrLogReader
        {
            std::shared_ptr<std::string> sp_message;
            AsrResult ReadOne(const char* message) override{
                *sp_message = {message};
                return ASR_S_OK;
            }

            ASR_UTILS_IASRBASE_AUTO_IMPL(AsrHttpLogReader)
            public:
            AsrHttpLogReader(std::shared_ptr<std::string> sp_message): sp_message{sp_message} {}
            // IAsrBase
            ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object)
            {
                return ASR::Utils::QueryInterface<IAsrLogReader>(this, iid, pp_object);
            };
        };

    ASR::AsrPtr<AsrHttpLogReader> p_reader{new AsrHttpLogReader{sp_one_message}};
    ASR::AsrPtr<IAsrLogRequester> p_requester{};

public:
    /**
     * Constructor with object mapper.
     * @param objectMapper - default object mapper used to serialize/deserialize
     * DTOs.
     */
    MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    {
        ::CreateIAsrLogRequester(32, p_requester.Put());
    }

    ENDPOINT("GET", "/api/getprofile", root)
    {
        auto jsonObjectMapper =
            oatpp::parser::json::mapping::ObjectMapper::createShared();

        auto dto = ProfileList::createShared();
        dto->code = 200;
        dto->message = reinterpret_cast<const char*>(u8"配置1");

        ASR_LOG_ERROR("严は馬鹿です");
        const auto error_code = p_requester->RequestOne(p_reader.Get());
        // OATPP_LOGE("A", sp_one_message->data());

        auto profile1 = ProfileListItem::createShared();
        profile1->name = sp_one_message->data();
        profile1->path = "config1";

        using namespace Asr;
        AsrPtr<IAsrReadOnlyString> p_string;
        const auto                 create_result =
            ::CreateIAsrReadOnlyStringFromUtf8("test", p_string.Put());
        if (IsFailed(create_result))
        {
            // cuo wu chu li
        }
        // usage
        const char* p_u8_string;
        const auto  get_u8_result = p_string->GetUtf8(&p_u8_string);
        if (IsFailed(get_u8_result))
        {
            //
        }
        // p_u8_string you zhi




        auto profile2 = ProfileListItem::createShared();
        profile2->name = "様式う2";
        profile2->path = "config2";

        dto->data = {profile1, profile2};

        oatpp::String json = jsonObjectMapper->writeToString(dto);

        return createDtoResponse(Status::CODE_200, json);
    }

    // TODO Insert Your endpoints here !!!
};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif /* MyController_hpp */
