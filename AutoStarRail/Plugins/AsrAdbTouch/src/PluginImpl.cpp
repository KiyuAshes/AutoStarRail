#include "AdbTouchFactoryImpl.h"
#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <array>
#include <stdexcept>

#define ASR_BUILD_SHARED
#include "AdbTouchFactoryImpl.h"
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
    size_t index,
    void** pp_out_interface)
{
    if (pp_out_interface == nullptr)
    {
        ASR_LOG_ERROR("Nullptr found.");
        return ASR_E_INVALID_POINTER;
    }

    if (index == 0)
    {
        try
        {
            const auto p_factory =
                MakeAsrPtr<IAsrInputFactory, AdbTouchFactory>();
            *pp_out_interface = p_factory.Get();
            p_factory->AddRef();
            return ASR_S_OK;
        }
        catch (const std::bad_alloc&)
        {
            return ASR_E_OUT_OF_MEMORY;
        }
    }
    return ASR_E_OUT_OF_RANGE;
}

ASR_IMPL AsrAdbTouchPlugin::CanUnloadNow() { return ASR_E_NO_IMPLEMENTATION; }

ASR_NS_END
