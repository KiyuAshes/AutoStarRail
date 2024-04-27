#ifndef Profile_DTOs_hpp
#define Profile_DTOs_hpp

#include "Global.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  定义配置文件相关数据类型
 *  Define profile related data types
 */

class ProfileDesc : public oatpp::DTO
{

    DTO_INIT(ProfileDesc, DTO)

    DTO_FIELD(String, name, "label");
    DTO_FIELD(String, pid, "key");
    DTO_FIELD(Boolean, is_run, "is_run");
};

using ProfileDescList = ApiResponse<oatpp::List<oatpp::Object<ProfileDesc>>>;

#include OATPP_CODEGEN_END(DTO)

#endif
