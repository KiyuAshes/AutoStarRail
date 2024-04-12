#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include <cstdint>

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */

template <class T>
class ApiResult : public oatpp::DTO
{

    DTO_INIT(ApiResult, DTO)

    DTO_FIELD(Int32, code);
    DTO_FIELD(String, message);

    DTO_FIELD(T, data);
};

class ProfileListItem : public oatpp::DTO
{

    DTO_INIT(ProfileListItem, DTO)

    DTO_FIELD(String, name, "label");
    DTO_FIELD(String, path, "key");
};

using ProfileList = ApiResult<oatpp::List<oatpp::Object<ProfileListItem>>>;

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */
