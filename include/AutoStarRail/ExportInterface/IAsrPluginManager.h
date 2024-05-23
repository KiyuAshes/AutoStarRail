#ifndef ASR_PLUGINMANAGER_H
#define ASR_PLUGINMANAGER_H

/**
 * @file IAsrPluginManager.h
 * @brief The exported interface in this file should only be used by GUI
 * programs.
 * @version 0.1
 * @date 2023-07-18
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
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
SWIG_IGNORE(IAsrPluginInfo)
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

// {138DF2D2-A9E9-4A73-9B4F-AA6C754601CC}
ASR_DEFINE_GUID(
    ASR_IID_PLUGIN_INFO_VECTOR,
    IAsrPluginInfoVector,
    0x138df2d2,
    0xa9e9,
    0x4a73,
    0x9b,
    0x4f,
    0xaa,
    0x6c,
    0x75,
    0x46,
    0x1,
    0xcc);
SWIG_IGNORE(IAsrPluginInfoVector)
ASR_INTERFACE IAsrPluginInfoVector : public IAsrBase
{
    ASR_METHOD Size(size_t * p_out_size) = 0;
    ASR_METHOD At(size_t index, IAsrPluginInfo * *pp_out_info) = 0;
};

// {CBEBF351-F4EE-4981-A0AB-69EC5562F08D}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_PLGUIN_INFO,
    IAsrSwigPluginInfo,
    0xcbebf351,
    0xf4ee,
    0x4981,
    0xa0,
    0xab,
    0x69,
    0xec,
    0x55,
    0x62,
    0xf0,
    0x8d);
ASR_SWIG_EXPORT_ATTRIBUTE(IAsrSwigPluginInfo)
ASR_INTERFACE IAsrSwigPluginInfo : public IAsrSwigBase
{
    virtual AsrRetReadOnlyString GetName() = 0;
    virtual AsrRetReadOnlyString GetDescription() = 0;
    virtual AsrRetReadOnlyString GetAuthor() = 0;
    virtual AsrRetReadOnlyString GetVersion() = 0;
    virtual AsrRetReadOnlyString GetSupportedSystem() = 0;
    virtual AsrRetGuid           GetPluginIid() = 0;
};

ASR_DEFINE_RET_POINTER(AsrRetPluginInfo, IAsrSwigPluginInfo);

// {30CCAE61-3884-43F4-AE78-976410156370}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_PLUGIN_INFO_VECTOR,
    IAsrSwigPluginInfoVector,
    0x30ccae61,
    0x3884,
    0x43f4,
    0xae,
    0x78,
    0x97,
    0x64,
    0x10,
    0x15,
    0x63,
    0x70);
ASR_SWIG_EXPORT_ATTRIBUTE(IAsrSwigPluginInfoVector)
ASR_INTERFACE IAsrSwigPluginInfoVector : public IAsrSwigBase
{
    virtual AsrRetUInt       Size() = 0;
    virtual AsrRetPluginInfo At(size_t index) = 0;
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
SWIG_IGNORE(IAsrPluginManager)
ASR_INTERFACE IAsrPluginManager : public IAsrBase
{
    ASR_METHOD GetAllPluginInfo(
        IAsrPluginInfoVector * *pp_out_plugin_info_vector) = 0;
};

/**
 * @brief Call this function to load all plugin.
 *
 * @param pp_out_result
 * @return AsrResult
 */
SWIG_IGNORE(CreateIAsrPluginManagerAndGetResult)
ASR_C_API AsrResult CreateIAsrPluginManagerAndGetResult(
    IAsrReadOnlyGuidVector* p_ignore_plugins_guid,
    IAsrPluginManager**     pp_out_result);

#endif // ASR_PLUGINMANAGER_H
