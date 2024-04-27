#ifndef Logs_DTOs_hpp
#define Logs_DTOs_hpp

#include "Global.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  定义日志相关数据类型
 *  Define log related data types
 */

class LogsData : public oatpp::DTO
{

    DTO_INIT(LogsData, DTO)

    DTO_FIELD(List<String>, logs, "logs");
};

using Logs = ApiResponse<oatpp::Object<LogsData>>;

#include OATPP_CODEGEN_END(DTO)

#endif