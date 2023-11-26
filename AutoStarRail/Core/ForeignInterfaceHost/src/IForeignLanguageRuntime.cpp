#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>
#include "PythonHost.h"
#include "JavaHost.h"
#include "CppHost.h"
#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <tl/expected.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

auto CreateForeignLanguageRuntime(
    const ForeignLanguageRuntimeFactoryDesc& desc_base)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>
{
    switch (desc_base.language)
    {
        using enum ForeignInterfaceLanguage;
    case Python:
#ifndef ASR_EXPORT_PYTHON
        goto on_no_interface;
#else

#endif // ASR_EXPORT_PYTHON
    case CSharp:
#ifndef ASR_EXPORT_CSHARP
        goto on_no_interface;
#else
#endif // ASR_EXPORT_CSHARP
    case Java:
#ifndef ASR_EXPORT_JAVA
        goto on_no_interface;
#else
#endif // ASR_EXPORT_JAVA
    case Lua:
        return tl::make_unexpected(ASR_E_NO_IMPLEMENTATION);
    case Cpp:
        return CppHost::CreateForeignLanguageRuntime(desc_base);
    default:
        throw ASR::Utils::UnexpectedEnumException::FromEnum(desc_base.language);
    }
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END