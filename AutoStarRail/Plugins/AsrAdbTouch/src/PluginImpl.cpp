#include "AdbTouchFactoryImpl.h"
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <array>
#include <stdexcept>

#define ASR_BUILD_SHARED
#include "PluginImpl.h"

ASR_NS_BEGIN

ASR_IMPL AsrAdbTouchPlugin::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return Utils::QueryInterface<IAsrPlugin>(this, iid, pp_object);
}

ASR_IMPL AsrAdbTouchPlugin::EnumFeature(
    size_t            index,
    AsrPluginFeature* p_out_feature)
{
    if (p_out_feature == nullptr)
    {
        ASR_LOG_ERROR("Nullptr found.");
        return ASR_E_INVALID_POINTER;
    }

    std::array features{ASR_PLUGIN_FEATURE_INPUT_FACTORY};

    try
    {
        const auto value = features.at(index);
        *p_out_feature = value;
        return ASR_S_OK;
    }
    catch (const std::out_of_range&)
    {
        return ASR_E_OUT_OF_RANGE;
    }
}

ASR_IMPL AsrAdbTouchPlugin::CreateFeatureInterface(
    AsrPluginFeature feature,
    void**           pp_out_interface)
{
    if (pp_out_interface == nullptr)
    {
        ASR_LOG_ERROR("Nullptr found.");
        return ASR_E_INVALID_POINTER;
    }

    switch (feature)
    {
    case ASR_PLUGIN_FEATURE_INPUT_FACTORY:
    {
        try
        {
            const auto p_factory = new AdbTouchFactory{};
            *pp_out_interface = p_factory;
            p_factory->AddRef();
        }
        catch (const std::bad_alloc&)
        {
            return ASR_E_OUT_OF_MEMORY;
        }
        break;
    }
    default:
        return ASR_E_NO_IMPLEMENTATION;
    }
    return ASR_S_OK;
}

ASR_IMPL AsrAdbTouchPlugin::CanUnloadNow() { return ASR_E_NO_IMPLEMENTATION; }

ASR_NS_END
