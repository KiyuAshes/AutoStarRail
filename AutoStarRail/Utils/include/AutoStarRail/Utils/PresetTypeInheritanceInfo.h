#ifndef ASR_UTILS_PRESETTYPEINHERITANCEINFO_H
#define ASR_UTILS_PRESETTYPEINHERITANCEINFO_H

#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/Utils/InternalTypeList.hpp>

ASR_UTILS_NS_BEGIN

template <class T>
struct PresetTypeInheritanceInfo;

#define ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(EndType, ...)                 \
    ASR_INTERFACE EndType;                                                     \
    ASR_UTILS_NS_BEGIN using EndType##InheritanceInfo =                        \
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

// IAsrTypeInfo.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTypeInfo, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrGuidVector, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrReadOnlyGuidVector, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigTypeInfo, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigGuidVector, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigReadOnlyGuidVector,
    IAsrSwigBase);

// AsrReadOnlyString.hpp
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrReadOnlyString, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrString,
    IAsrBase,
    IAsrReadOnlyString);
// -------------------------------------

// PluginInterface/IAsrCapture.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrCapture, IAsrBase, IAsrTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrCaptureFactory,
    IAsrBase,
    IAsrTypeInfo);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigCapture,
    IAsrSwigBase,
    IAsrSwigTypeInfo);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigCaptureFactory,
    IAsrSwigBase,
    IAsrSwigTypeInfo);

// PluginInterface/IAsrPlugin.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPlugin, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigPlugin, IAsrSwigBase);

// PluginInterface/IAsrErrorLens.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrErrorLens, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigErrorLens, IAsrSwigBase);

// PluginInterface/IAsrTask.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTask, IAsrBase, IAsrTypeInfo);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigTask,
    IAsrSwigBase,
    IAsrSwigTypeInfo);

// ExportInterface/AsrCV.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTemplateMatchResult, IAsrBase)
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigTemplateMatchResult,
    IAsrSwigBase)

// ExportInterface/IAsrBasicErrorLens.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrBasicErrorLens, IAsrBase)
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigBasicErrorLens, IAsrSwigBase)

// ExportInterface/IAsrCaptureManager.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrCaptureManager, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigCaptureManager, IAsrSwigBase);

// ExportInterface/IAsrImage.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrImage, IAsrBase);
// -------------------------------------

// ExportInterface/IAsrMemory.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrMemory, IAsrBase)

// ExportInterface/IAsrPluginManager
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPluginInfo, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPluginInfoVector, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPluginManager, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigPluginInfo, IAsrSwigBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigPluginInfoVector,
    IAsrSwigBase);

// ExportInterface/IAsrSettings.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSettings, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigSettings, IAsrSwigBase);

#endif // ASR_UTILS_PRESETTYPEINHERITANCEINFO_H
