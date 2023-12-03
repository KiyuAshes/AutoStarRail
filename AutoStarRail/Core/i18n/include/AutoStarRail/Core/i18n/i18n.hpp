#ifndef ASR_CORE_I18N_I18N_H
#define ASR_CORE_I18N_I18N_H

#include <AutoStarRail/Core/i18n/Config.h>
#include <AutoStarRail/Core/Exceptions/TypeError.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <fstream>
#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

ASR_NS_BEGIN

class AsrStringCppImpl;

ASR_NS_END

ASR_CORE_I18N_NS_BEGIN

struct AsrReadOnlyStringWrapper
{
    mutable AsrPtr<IAsrReadOnlyString> p_impl_;
    AsrReadOnlyStringWrapper();
    AsrReadOnlyStringWrapper(const char* p_u8_string);
    AsrReadOnlyStringWrapper(const char8_t* u8_string);
    ~AsrReadOnlyStringWrapper();
    void GetImpl(IAsrReadOnlyString** pp_impl) const;
};



template <class T, class Item>
using TranslateItemMap = std::map<T, Item>;

template <class T, class Item>
using TranslateResources =
    std::unordered_map<std::u8string, TranslateItemMap<T, Item>>;

namespace Details
{
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
}

void from_json(const ::nlohmann::json& input, AsrReadOnlyStringWrapper& output);

template <class T>
class I18n
{
    using InternalTranslateResource =
        TranslateResources<T, AsrReadOnlyStringWrapper>;
    using ConstLocaleToTranslateIt =
        typename InternalTranslateResource::const_iterator;

    InternalTranslateResource translate_resource_;
    ConstLocaleToTranslateIt  it_default_translate_map_;
    std::u8string             default_locale_;

public:
    explicit I18n(const std::filesystem::path& json_path)
    {
        std::ifstream ifs{};
        ASR::Utils::EnableStreamException(
            ifs,
            std::ios::badbit | std::ios::failbit,
            [&json_path](auto& stream) { stream.open(json_path.c_str()); });
        const auto json = ::nlohmann::json::parse(ifs);
        *this = I18n{json};
    }
    explicit I18n(const nlohmann::json& json)
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
    explicit I18n(const InternalTranslateResource& translate_resource)
        : translate_resource_{translate_resource}
    {
    }

    explicit I18n(InternalTranslateResource&& translate_resource)
        : translate_resource_{std::move(translate_resource)}
    {
    }

    ~I18n() = default;

    AsrResult SetDefaultLocale(const char8_t* const default_locale)
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
    std::u8string GetDefaultLocale() const { return default_locale_; }
    AsrResult     GetErrorMessage(
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
    AsrResult GetErrorMessage(
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
};

ASR_CORE_I18N_NS_END

#endif // ASR_CORE_I18N_I18N_H