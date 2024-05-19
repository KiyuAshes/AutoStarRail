#ifndef DAS_HTTP_CONTROLLER_CONTROLLERUTILS_HPP
#define DAS_HTTP_CONTROLLER_CONTROLLERUTILS_HPP


#include <memory>
#include <string>

#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/ExportInterface/AsrLogger.h"
#include "AutoStarRail/Utils/CommonUtils.hpp"
#include "AutoStarRail/Utils/QueryInterface.hpp"
#include "AutoStarRail/ExportInterface/AsrLogger.h"


class AsrHttpLogReader : public IAsrLogReader
{
    std::shared_ptr<std::string> sp_message;

    AsrResult ReadOne(const char* message, size_t size) override
    {
        *sp_message = {message, size};
        return ASR_S_OK;
    }

    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrHttpLogReader)
    AsrHttpLogReader(std::shared_ptr<std::string> sp_message)
        : sp_message{sp_message}
    {
    }

    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override
    {
        return ASR::Utils::QueryInterface<IAsrLogReader>(this, iid, pp_object);
    };
};

#endif // DAS_HTTP_CONTROLLER_CONTROLLERUTILS_HPP
