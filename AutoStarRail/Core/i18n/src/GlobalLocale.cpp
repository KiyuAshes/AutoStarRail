#include <AutoStarRail/Core/i18n/GlobalLocale.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>

ASR_CORE_I18N_NS_BEGIN

GlobalLocaleSingleton InitializeGlobalLocaleSingleton()
{
    return GlobalLocaleSingleton{};
}

GlobalLocaleSingleton::GlobalLocaleSingleton()
{
    ::CreateIAsrReadOnlyStringFromUtf8("en", p_locale_name.Put());
}

const Asr::AsrPtr<IAsrReadOnlyString>& GlobalLocaleSingleton::GetInstance()
    const
{
    return p_locale_name;
}

void GlobalLocaleSingleton::SetInstance(
    AsrPtr<IAsrReadOnlyString> p_new_locale_name)
{
    p_locale_name = p_new_locale_name;
}

ASR_DEFINE_VARIABLE(g_locale) = InitializeGlobalLocaleSingleton();

const AsrPtr<IAsrReadOnlyString> g_fallback_locale_name{
    new ::AsrStringCppImpl{U_NAMESPACE_QUALIFIER UnicodeString::fromUTF8("en")},
    take_ownership};

auto GetFallbackLocale() -> AsrPtr<IAsrReadOnlyString>
{
    return g_fallback_locale_name;
}

ASR_CORE_I18N_NS_END

// ----------------------------------------------------------------

AsrResult AsrSetDefaultLocale(IAsrReadOnlyString* locale_name)
{
    ASR::AsrPtr holder{locale_name, ASR::take_ownership};
    ASR::Core::i18n::g_locale.SetInstance(holder);
    return ASR_S_OK;
}

AsrResult AsrGetDefaultLocale(IAsrReadOnlyString** pp_out_locale_name)
{
    auto* p_result = ASR::Core::i18n::g_locale.GetInstance().Get();
    p_result->AddRef();
    *pp_out_locale_name = p_result;
    return ASR_S_OK;
}

AsrResult AsrSetDefaultLocale(AsrReadOnlyString locale_name)
{
    ASR::AsrPtr<IAsrReadOnlyString> p_locale_name{};
    locale_name.GetImpl(p_locale_name.Put());
    return ::AsrSetDefaultLocale(p_locale_name.Get());
}

AsrRetReadOnlyString AsrGetDefaultLocale()
{
    AsrRetReadOnlyString            result{};
    ASR::AsrPtr<IAsrReadOnlyString> p_locale_name{};
    result.error_code = ::AsrGetDefaultLocale(p_locale_name.Put());
    result.value = AsrReadOnlyString{p_locale_name};
    return result;
}
