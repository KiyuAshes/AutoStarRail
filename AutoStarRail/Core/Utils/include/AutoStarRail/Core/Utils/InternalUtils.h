#ifndef ASR_CORE_UTILS_INTERNALUTILS_H
#define ASR_CORE_UTILS_INTERNALUTILS_H

#include <AutoStarRail/AsrString.hpp>
#include <AutoStarRail/Core/Exceptions/AsrException.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/Utils/Config.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/Expected.h>
#include <string_view>

ASR_CORE_UTILS_NS_BEGIN

inline void* VoidP(void* pointer) { return pointer; }

[[nodiscard]]
auto MakeAsrReadOnlyStringFromUtf8(std::string_view u8_string)
    -> ASR::Utils::Expected<AsrPtr<IAsrReadOnlyString>>;

template <class T>
auto GetGuidFrom(T* p_object) -> AsrGuid
{
    if constexpr (ASR::Core::ForeignInterfaceHost::is_asr_swig_interface<T>)
    {
        const auto ret_guid = p_object->GetGuid();
        if (IsFailed(ret_guid.error_code))
        {
            AsrException::Throw(ret_guid.error_code, p_object);
        }
        return ret_guid.value;
    }
    else if constexpr (ASR::Core::ForeignInterfaceHost::is_asr_interface<T>)
    {
        AsrGuid guid;
        if (const auto gg_result = p_object->GetGuid(&guid);
            IsFailed(gg_result))
        {
            AsrException::Throw(gg_result, p_object);
        }
        return guid;
    }
    else
    {
        static_assert(ASR::Utils::value<false, T>, "没有匹配的类型！");
    }
}

template <class T>
auto GetRuntimeClassNameFrom(T* p_object) -> AsrPtr<IAsrReadOnlyString>
{
    if constexpr (ASR::Core::ForeignInterfaceHost::is_asr_swig_interface<T>)
    {
        AsrPtr<IAsrReadOnlyString> result{};
        const auto                 ret_name = p_object->GetRuntimeClassName();
        if (IsFailed(ret_name.error_code))
        {
            AsrException::Throw(ret_name.error_code, p_object);
        }
        ret_name.value.GetImpl(result.Put());
        return result;
    }
    else if constexpr (ASR::Core::ForeignInterfaceHost::is_asr_interface<T>)
    {
        AsrPtr<IAsrReadOnlyString> result{};
        if (const auto error_code = p_object->GetRuntimeClassName(result.Put());
            IsFailed(error_code)) [[unlikely]]
        {
            AsrException::Throw(error_code, p_object);
        }
        return result;
    }
    else
    {
        static_assert(ASR::Utils::value<false, T>, "没有匹配的类型！");
    }
}

ASR_CORE_UTILS_NS_END

#endif // ASR_CORE_UTILS_INTERNALUTILS_H
