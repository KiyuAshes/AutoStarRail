#ifndef ASR_UTILS_PRESETTYPEINHERITANCEINFO_H
#define ASR_UTILS_PRESETTYPEINHERITANCEINFO_H

#include <AutoStarRail/Utils/InternalTypeList.hpp>
#include <AutoStarRail/IAsrBase.h>

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

// IAsrInspectable.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrInspectable, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrIidVector, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigInspectable, IAsrSwigBase);

// AsrReadOnlyString.hpp
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrReadOnlyString, IAsrBase);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrString,
    IAsrBase,
    IAsrReadOnlyString);
// -------------------------------------

// PluginInterface/IAsrCapture.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrCapture,
    IAsrBase,
    IAsrInspectable);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrCaptureFactory,
    IAsrBase,
    IAsrInspectable);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigCapture,
    IAsrSwigBase,
    IAsrSwigInspectable);
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigCaptureFactory,
    IAsrSwigBase,
    IAsrSwigInspectable);

// PluginInterface/IAsrPlugin.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrPlugin, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigPlugin, IAsrSwigBase);

// PluginInterface/IAsrErrorLens.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrErrorLens, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigErrorLens, IAsrSwigBase);

// PluginInterface/IAsrTask.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrTask, IAsrBase, IAsrInspectable);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(
    IAsrSwigTask,
    IAsrSwigBase,
    IAsrSwigInspectable);

// ExportInterface/IAsrCaptureManager.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrCaptureManager, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigCaptureManager, IAsrSwigBase);
// ExportInterface/IAsrSettings.h
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSettings, IAsrBase);
// -------------------------------------
ASR_UTILS_DEFINE_PRESET_INHERITANCE_INFO(IAsrSwigSettings, IAsrSwigBase);

#endif // ASR_UTILS_PRESETTYPEINHERITANCEINFO_H
