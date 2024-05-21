#ifndef DAS_HTTP_CONTROLLER_CONTROLLERUTILS_HPP
#define DAS_HTTP_CONTROLLER_CONTROLLERUTILS_HPP

#include <memory>
#include <string>

#include "AutoStarRail/ExportInterface/AsrLogger.h"
#include "AutoStarRail/IAsrBase.h"
#include "AutoStarRail/Utils/CommonUtils.hpp"
#include "AutoStarRail/Utils/QueryInterface.hpp"

class AsrHttpLogReader : public IAsrLogReader
{
    std::string message_{};

    AsrResult ReadOne(const char* message, size_t size) override
    {
        message_ = {message, size};
        return ASR_S_OK;
    }

    ASR_UTILS_IASRBASE_AUTO_IMPL(AsrHttpLogReader)

    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override
    {
        return ASR::Utils::QueryInterface<IAsrLogReader>(this, iid, pp_object);
    };

    auto GetMessage() const noexcept -> std::string_view { return message_; }
};

#endif // DAS_HTTP_CONTROLLER_CONTROLLERUTILS_HPP
