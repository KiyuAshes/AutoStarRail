#ifndef Settings_DTOs_hpp
#define Settings_DTOs_hpp

#include "Global.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  定义设置相关数据类型
 *  Define settings related data type
 */

class AppDesc : public oatpp::DTO
{

    DTO_INIT(AppDesc, DTO)

    DTO_FIELD(String, name, "name");
    DTO_FIELD(String, package_name, "package_name");
};

using AppDescList = ApiResponse<oatpp::List<oatpp::Object<AppDesc>>>;

#include OATPP_CODEGEN_END(DTO)

#endif

