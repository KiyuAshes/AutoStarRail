#include "AutoStarRail/ExportInterface/IAsrSettings.h"
#include <AutoStarRail/Core/Exceptions/TypeError.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/GlobalSettingsManager/GlobalSettingsManager.h>
#include <AutoStarRail/Core/i18n/i18n.h>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string_view>

ASR_CORE_I18N_NS_BEGIN

template class I18n<AsrResult>;

AsrReadOnlyStringWrapper::AsrReadOnlyStringWrapper(const char* p_u8_string)
{
    ::CreateIAsrReadOnlyStringFromUtf8(p_u8_string, p_impl_.Put());
}

AsrReadOnlyStringWrapper::AsrReadOnlyStringWrapper(const char8_t* u8_string)
{
    ::CreateIAsrReadOnlyStringFromUtf8(
        reinterpret_cast<const char*>(u8_string),
        p_impl_.Put());
}

void AsrReadOnlyStringWrapper::GetImpl(IAsrReadOnlyString** pp_impl) const
{
    p_impl_->AddRef();
    *pp_impl = p_impl_.Get();
}

AsrReadOnlyStringWrapper::AsrReadOnlyStringWrapper() = default;

AsrReadOnlyStringWrapper::~AsrReadOnlyStringWrapper() = default;

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T>
T FromString(std::string_view input)
{
    if constexpr (std::is_same_v<T, int>)
    {
        return std::stoi(input.data());
    }
    else if (std::is_same_v<T, float>)
    {
        return std::stof(input.data());
    }
}

ASR_NS_ANONYMOUS_DETAILS_END

template <class T, class Item>
void from_json(const ::nlohmann::json& input, TranslateItemMap<T, Item>& output)
{
    for (const auto& [key, value] : input.items())
    {
        const auto t_key = Details::FromString<T>(key);
        if constexpr (std::is_same_v<Item, AsrReadOnlyStringWrapper>)
        {
            const auto string_value = value.template get<std::string>();
            output[t_key] = {string_value.c_str()};
        }
        else
        {
            output[t_key] = value.template get<std::string>();
        }
    }
}

template <class T>
I18n<T>::I18n(const boost::filesystem::path& json_path)
{
    std::ifstream ifs{};
    ASR::Utils::EnableStreamException(
        ifs,
        std::ios::badbit | std::ios::failbit,
        [&json_path](auto& stream) { stream.open(json_path.c_str()); });
    const auto json = ::nlohmann::json::parse(ifs);
    *this = I18n{json};
}

template <class T>
I18n<T>::I18n(const nlohmann::json& json)
{
    const auto type = json.at("type").get<AsrType>();
    // NOTE: If T changes, we need to add code to handle this situation.
    if constexpr (std::is_same_v<T, int>)
    {
        if (type != ASR_TYPE_INT)
        {
            throw ASR::Core::TypeError{ASR_TYPE_INT, type};
        }
    }
    for (const auto& [k, v] : json.at("resource").items())
    {
        TranslateItemMap<T, AsrReadOnlyStringWrapper> m{};
        v.get_to(m);
        translate_resource_[{k.begin(), k.end()}] = m;
    }
    SetDefaultLocale(u8"en");
}

template <class T>
I18n<T>::I18n(const InternalTranslateResource& translate_resource)
    : translate_resource_{translate_resource}
{
}

template <class T>
I18n<T>::I18n(InternalTranslateResource&& translate_resource)
    : translate_resource_{std::move(translate_resource)}
{
}

template <class T>
I18n<T>::~I18n() = default;

template <class T>
AsrResult I18n<T>::SetDefaultLocale(const char8_t* const default_locale)
{
    default_locale_ = std::u8string(default_locale);
    if (const auto it = translate_resource_.find(default_locale_);
        it != translate_resource_.end())
    {
        it_default_translate_map_ = it;
        return ASR_S_OK;
    }
    return ASR_E_NO_IMPLEMENTATION;
}

template <class T>
std::u8string I18n<T>::GetDefaultLocale() const
{
    return default_locale_;
}

template <class T>
AsrResult I18n<T>::GetErrorMessage(
    const T&             result,
    IAsrReadOnlyString** pp_out_error_explanation) const
{
    if (pp_out_error_explanation == nullptr)
    {
        return ASR_E_INVALID_POINTER;
    }

    const auto& translate_table = it_default_translate_map_->second;
    if (const auto it = translate_table.find(result);
        it != translate_table.end())
    {
        it->second.GetImpl(pp_out_error_explanation);
        return ASR_S_OK;
    }
    *pp_out_error_explanation = nullptr;
    return ASR_E_OUT_OF_RANGE;
}

template <class T>
AsrResult I18n<T>::GetErrorMessage(
    const char8_t* const locale,
    const T&             result,
    IAsrReadOnlyString** pp_out_error_message) const
{
    if (const auto resource_it = translate_resource_.find(locale);
        resource_it != translate_resource_.end())
    {
        const auto& table = resource_it->second;
        if (const auto it = table.find(result); it != table.end())
        {
            if (pp_out_error_message == nullptr)
            {
                return ASR_E_INVALID_POINTER;
            }
            it->second.GetImpl(pp_out_error_message);
            return ASR_S_OK;
        }
        *pp_out_error_message = nullptr;
        return ASR_E_OUT_OF_RANGE;
    }
    // fallback to default locale
    if (const auto en_us_resource_it = translate_resource_.find(u8"en");
        en_us_resource_it != translate_resource_.end())
    {
        const auto& table = en_us_resource_it->second;
        if (const auto it = table.find(result); it != table.end())
        {
            if (pp_out_error_message == nullptr)
            {
                return ASR_E_INVALID_POINTER;
            }
            it->second.GetImpl(pp_out_error_message);
            return ASR_S_OK;
        }
    }
    return ASR_E_NO_IMPLEMENTATION;
}

ASR_CORE_I18N_NS_END
