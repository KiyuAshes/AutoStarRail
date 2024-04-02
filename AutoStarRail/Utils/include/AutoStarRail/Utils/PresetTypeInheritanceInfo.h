// This file is automatically generated by generator.py
// !!! DO NOT EDIT !!!

#ifndef ASR_UTILS_PRESETTYPEINHERITANCEINFO_H
#define ASR_UTILS_PRESETTYPEINHERITANCEINFO_H

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/Utils/InternalTypeList.hpp>

ASR_UTILS_NS_BEGIN

template <class T>
struct PresetTypeInheritanceInfo;

#define ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(EndType, ...)             \
ASR_UTILS_NS_BEGIN                                                         \
using EndType##InheritanceInfo =                                           \
    ASR::Utils::internal_type_holder<__VA_ARGS__, EndType>;                \
template <>                                                                \
struct PresetTypeInheritanceInfo<EndType>                                  \
{                                                                          \
    using TypeInfo = EndType##InheritanceInfo;                             \
};                                                                         \
ASR_UTILS_NS_END

// IAsrBase.h
using IAsrBaseInheritanceInfo = internal_type_holder<IAsrBase>;
template <>
struct PresetTypeInheritanceInfo<::IAsrBase>
{
    using TypeInfo = IAsrBaseInheritanceInfo;
};

using IAsrSwigBaseInheritanceInfo = internal_type_holder<IAsrSwigBase>;
template <>
struct PresetTypeInheritanceInfo<::IAsrSwigBase>
{
    using TypeInfo = IAsrSwigBaseInheritanceInfo;
};

ASR_UTILS_NS_END

ASR_INTERFACE IAsrBase;
ASR_INTERFACE IAsrBasicErrorLens;
ASR_INTERFACE IAsrCapture;
ASR_INTERFACE IAsrCaptureFactory;
ASR_INTERFACE IAsrCaptureManager;
ASR_INTERFACE IAsrErrorLens;
ASR_INTERFACE IAsrGuidVector;
ASR_INTERFACE IAsrImage;
ASR_INTERFACE IAsrInput;
ASR_INTERFACE IAsrInputFactory;
ASR_INTERFACE IAsrInputFactoryVector;
ASR_INTERFACE IAsrLogReader;
ASR_INTERFACE IAsrLogRequester;
ASR_INTERFACE IAsrMemory;
ASR_INTERFACE IAsrPlugin;
ASR_INTERFACE IAsrPluginInfo;
ASR_INTERFACE IAsrPluginInfoVector;
ASR_INTERFACE IAsrPluginManager;
ASR_INTERFACE IAsrReadOnlyGuidVector;
ASR_INTERFACE IAsrReadOnlyString;
ASR_INTERFACE IAsrSettings;
ASR_INTERFACE IAsrString;
ASR_INTERFACE IAsrSwigBase;
ASR_INTERFACE IAsrSwigBasicErrorLens;
ASR_INTERFACE IAsrSwigCapture;
ASR_INTERFACE IAsrSwigCaptureManager;
ASR_INTERFACE IAsrSwigErrorLens;
ASR_INTERFACE IAsrSwigGuidVector;
ASR_INTERFACE IAsrSwigInput;
ASR_INTERFACE IAsrSwigInputFactory;
ASR_INTERFACE IAsrSwigInputFactoryVector;
ASR_INTERFACE IAsrSwigPlugin;
ASR_INTERFACE IAsrSwigPluginInfo;
ASR_INTERFACE IAsrSwigPluginInfoVector;
ASR_INTERFACE IAsrSwigReadOnlyGuidVector;
ASR_INTERFACE IAsrSwigSettings;
ASR_INTERFACE IAsrSwigTask;
ASR_INTERFACE IAsrSwigTouch;
ASR_INTERFACE IAsrSwigTypeInfo;
ASR_INTERFACE IAsrTask;
ASR_INTERFACE IAsrTaskManager;
ASR_INTERFACE IAsrTouch;
ASR_INTERFACE IAsrTypeInfo;

ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrBasicErrorLens, IAsrBase, IAsrErrorLens);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrCapture, IAsrBase, IAsrTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrCaptureFactory, IAsrBase, IAsrTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrCaptureManager, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrErrorLens, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrGuidVector, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrImage, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrInput, IAsrBase, IAsrTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrInputFactory, IAsrBase, IAsrTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrInputFactoryVector, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrLogReader, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrLogRequester, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrMemory, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPlugin, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPluginInfo, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPluginInfoVector, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPluginManager, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrReadOnlyGuidVector, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrReadOnlyString, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSettings, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrString, IAsrBase, IAsrReadOnlyString);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigBasicErrorLens, IAsrSwigBase, IAsrSwigErrorLens);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigCapture, IAsrSwigBase, IAsrSwigTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigCaptureManager, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigErrorLens, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigGuidVector, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigInput, IAsrSwigBase, IAsrSwigTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigInputFactory, IAsrSwigBase, IAsrSwigTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigInputFactoryVector, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigPlugin, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigPluginInfo, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigPluginInfoVector, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigReadOnlyGuidVector, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigSettings, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigTask, IAsrSwigBase, IAsrSwigTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigTouch, IAsrSwigBase, IAsrSwigTypeInfo, IAsrSwigInput);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigTypeInfo, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTask, IAsrBase, IAsrTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTaskManager, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTouch, IAsrBase, IAsrTypeInfo, IAsrInput);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTypeInfo, IAsrBase);

#endif // ASR_UTILS_PRESETTYPEINHERITANCEINFO_H
