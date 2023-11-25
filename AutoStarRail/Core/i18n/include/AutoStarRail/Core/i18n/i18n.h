#ifndef ASR_CORE_I18N_I18N_H
#define ASR_CORE_I18N_I18N_H

#include <AutoStarRail/Core/i18n/Config.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <filesystem>
#include <map>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>

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

/**
 * @brief 使用前必须在i18n.h文件中显式实例化
 *
 */
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
    explicit I18n(const std::filesystem::path& json_path);
    explicit I18n(const nlohmann::json& json);
    explicit I18n(const InternalTranslateResource& translate_resource);
    explicit I18n(InternalTranslateResource&& translate_resource);
    ~I18n();

    AsrResult     SetDefaultLocale(const char8_t* const default_locale);
    std::u8string GetDefaultLocale() const;
    AsrResult     GetErrorMessage(
            const T&             result,
            IAsrReadOnlyString** pp_out_error_explanation) const;
    AsrResult GetErrorMessage(
        const char8_t* const locale,
        const T&             result,
        IAsrReadOnlyString** pp_out_error_message) const;
};

extern template class I18n<AsrResult>;

ASR_CORE_I18N_NS_END

#endif // ASR_CORE_I18N_I18N_H