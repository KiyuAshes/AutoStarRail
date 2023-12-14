#ifndef ASR_PLUGINMANAGER_H
#define ASR_PLUGINMANAGER_H

/**
 * @file IAsrPluginManager.h
 * @brief The content in this file will NOT be processed by SWIG.
    The exported interface in this file should only be used by GUI programs.
 * @version 0.1
 * @date 2023-07-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/IAsrBase.h>
#include <cstddef>

// {8179F162-5E1A-4248-AC67-758D2AFF18A7}
ASR_DEFINE_GUID(
    ASR_IID_PLUGIN_INFO,
    IAsrPluginInfo,
    0x8179f162,
    0x5e1a,
    0x4248,
    0xac,
    0x67,
    0x75,
    0x8d,
    0x2a,
    0xff,
    0x18,
    0xa7);
ASR_INTERFACE IAsrPluginInfo : public IAsrBase
{
    ASR_METHOD GetName(IAsrReadOnlyString * *pp_out_name) = 0;
    ASR_METHOD GetDescription(IAsrReadOnlyString * *pp_out_description) = 0;
    ASR_METHOD GetAuthor(IAsrReadOnlyString * *pp_out_author) = 0;
    ASR_METHOD GetVersion(IAsrReadOnlyString * *pp_out_version) = 0;
    ASR_METHOD GetSupportedSystem(
        IAsrReadOnlyString * *pp_out_supported_system) = 0;
    ASR_METHOD GetPluginIid(AsrGuid * p_out_guid) = 0;
};

class AsrSwigPluginInfo
{
private:
    ASR::AsrPtr<IAsrPluginInfo> p_plugin_info;

public:
    AsrRetReadOnlyString GetName();
    AsrRetReadOnlyString GetDescription();
    AsrRetReadOnlyString GetAuthor();
    AsrRetReadOnlyString GetVersion();
    AsrRetReadOnlyString GetSupportedSystem();
    AsrRetGuid           GetPluginIid();
};

// {B2678FF8-720C-48E6-AC00-77D43D08F580}
ASR_DEFINE_GUID(
    ASR_IID_PLUGIN_MANAGER,
    IAsrPluginManager,
    0xb2678ff8,
    0x720c,
    0x48e6,
    0xac,
    0x0,
    0x77,
    0xd4,
    0x3d,
    0x8,
    0xf5,
    0x80);
ASR_INTERFACE IAsrPluginManager : public IAsrBase
{
    AsrResult GetCount(size_t * p_out_size);
};

/**
 * @brief Call this function to load all plugin.
 *
 * @param pp_out_result
 * @return ASR_C_API
 */
ASR_C_API AsrResult LoadPluginAndGetResult(
    IAsrGuidVector*     p_ignore_plugins_guid,
    IAsrPluginManager** pp_out_result);

#endif // ASR_PLUGINMANAGER_H
