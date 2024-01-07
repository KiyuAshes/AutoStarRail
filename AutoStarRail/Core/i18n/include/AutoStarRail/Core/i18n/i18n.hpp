#ifndef ASR_CORE_I18N_I18N_H
#define ASR_CORE_I18N_I18N_H

#include <AutoStarRail/Core/Exceptions/TypeError.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/i18n/Config.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/StreamUtils.hpp>
#include <filesystem>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

class AsrStringCppImpl;

ASR_CORE_I18N_NS_BEGIN

template <class T, class Item>
using TranslateItemMap = std::map<T, Item>;

template <class T, class Item>
using TranslateResources =
    std::unordered_map<std::u8string, TranslateItemMap<T, Item>>;

namespace Details
{
    template <class T>
    void CheckInput(const AsrType type)
    {
        if constexpr (std::is_same_v<T, std::int32_t>)
        {
            if (type != ASR_TYPE_INT)
            {
                throw ASR::Core::TypeError{ASR_TYPE_INT, type};
            }
        }
        else
        {
            static_assert(
                ASR::Utils::value<false, T>,
                "Incompatible type detected!");
        }
    }

    template <class T>
    constexpr auto GetConverter()
    {
        if constexpr (std::is_same_v<T, std::int32_t>)
        {
            return [](const std::string_view view)
            {
                char*      p_end{};
                const auto long_result = std::strtol(view.data(), &p_end, 0);
                const auto result = static_cast<std::int32_t>(long_result);
                if (long_result > std::numeric_limits<std::int32_t>::max()
                    || long_result < std::numeric_limits<std::int32_t>::min())
                {
                    ASR_CORE_LOG_WARN_USING_EXTRA_FUNCTION_NAME(
                        ASR_FUNCTION,
                        "Overflow detected: expected {}, std::int32_t value is {}.",
                        long_result,
                        result);
                }
                return result;
            };
        }
        else if constexpr (std::is_same_v<T, long long>)
        {
            return [](const std::string_view view)
            {
                char* p_end{};
                return std::strtoll(view.data(), &p_end, 0);
            };
        }
        else
        {
            static_assert(
                 ASR::Utils::value<false, T>,
                "Incompatible type detected!");
        }
    }
} // namespace Details

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
        Details::CheckInput<T>(type);
        const auto string_to_number_converter = Details::GetConverter<T>();
        for (const auto& [locale_name, i18n_resource] :
             json.at("resource").items())
        {
            TranslateItemMap<T, AsrReadOnlyStringWrapper> tmp_map{};
            for (const auto& [error_code_string, error_message] :
                 i18n_resource.items())
            {
                T    error_code = string_to_number_converter(error_code_string);
                auto error_message_string =
                    error_message.get<AsrReadOnlyStringWrapper>();
                tmp_map.emplace(std::make_pair(error_code, error_message));
            }
            translate_resource_[{ASR_FULL_RANGE_OF(locale_name)}] =
                std::move(tmp_map);
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