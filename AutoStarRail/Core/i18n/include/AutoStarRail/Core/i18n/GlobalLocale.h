#ifndef ASR_CORE_I18N_GLOBALLOCALE_H
#define ASR_CORE_I18N_GLOBALLOCALE_H

#include <AutoStarRail/Core/i18n/Config.h>
#include <AutoStarRail/AsrString.hpp>

ASR_CORE_I18N_NS_BEGIN

class GlobalLocaleSingleton
{
    friend GlobalLocaleSingleton InitializeGlobalLocaleSingleton();

    AsrPtr<IAsrReadOnlyString> p_locale_name;

    GlobalLocaleSingleton();

public:
    const AsrPtr<IAsrReadOnlyString>& GetInstance() const;
    void SetInstance(AsrPtr<IAsrReadOnlyString> p_new_locale_name);
};

extern GlobalLocaleSingleton g_locale;

auto GetFallbackLocale() -> AsrPtr<IAsrReadOnlyString>;

ASR_CORE_I18N_NS_END

#endif // ASR_CORE_I18N_GLOBALLOCALE_H
