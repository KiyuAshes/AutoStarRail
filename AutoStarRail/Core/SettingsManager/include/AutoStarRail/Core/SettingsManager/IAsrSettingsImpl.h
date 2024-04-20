#ifndef ASR_CORE_SETTINGSMANAGER_GLOBALSETTINGSMANAGER_H
#define ASR_CORE_SETTINGSMANAGER_GLOBALSETTINGSMANAGER_H

#include <AutoStarRail/Core/SettingsManager/Config.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Utils/Expected.h>
#include <mutex>
#include <nlohmann/json.hpp>

NLOHMANN_JSON_SERIALIZE_ENUM(
    AsrType,
    {{ASR_TYPE_INT, "int"},
     {ASR_TYPE_FLOAT, "float"},
     {ASR_TYPE_STRING, "string"},
     {ASR_TYPE_BOOL, "bool"}})

ASR_CORE_SETTINGSMANAGER_NS_BEGIN

class AsrSettings;

class IAsrSettingsImpl final : public IAsrSettings
{
    AsrSettings& impl_;
    std::string  u8_type_name_{};

public:
    IAsrSettingsImpl(AsrSettings& impl, const char* u8_type_name);
    // IAsrBase
    int64_t  AddRef() override;
    int64_t  Release() override;
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrSettings
    ASR_IMPL GetString(
        IAsrReadOnlyString*  key,
        IAsrReadOnlyString** pp_out_string) override;
    ASR_IMPL GetBool(IAsrReadOnlyString* key, bool* p_out_bool) override;
    ASR_IMPL GetInt(IAsrReadOnlyString* key, int64_t* p_out_int) override;
    ASR_IMPL GetFloat(IAsrReadOnlyString* key, float* p_out_float) override;
};

class IAsrSwigSettingsImpl final : public IAsrSwigSettings
{
    AsrSettings& impl_;
    std::string  u8_type_name_{};

public:
    IAsrSwigSettingsImpl(AsrSettings& impl, const char* u8_type_name);
    // IAsrSwigBase
    int64_t        AddRef() override;
    int64_t        Release() override;
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    // IAsrSwigSettings
    AsrRetReadOnlyString GetString(const AsrReadOnlyString key) override;
    AsrRetBool           GetBool(const AsrReadOnlyString key) override;
    AsrRetInt            GetInt(const AsrReadOnlyString key) override;
    AsrRetFloat          GetFloat(const AsrReadOnlyString key) override;
};

class IAsrSettingsForUiImpl final : public IAsrSettingsForUi
{
    AsrSettings& impl_;

public:
    IAsrSettingsForUiImpl(AsrSettings& impl);
    // IAsrBase
    int64_t  AddRef() override;
    int64_t  Release() override;
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrSettingsForUi
    ASR_IMPL ToString(IAsrReadOnlyString** pp_out_string) override;
    ASR_IMPL FromString(IAsrReadOnlyString* p_in_settings) override;
    ASR_IMPL SaveTo(IAsrReadOnlyString* p_path) override;
};

/**`
 * @brief 全局单例
 */
class AsrSettings
{
    std::mutex     mutex_;
    nlohmann::json settings_;
    /**
     * @brief 默认设置就是和设置一样的结构，只是里面存了默认值
     *
     */
    nlohmann::json default_values_;

    auto GetKey(const char* p_type_name, const char* key)
        -> Utils::Expected<std::reference_wrapper<const nlohmann::json>>;

    auto ToString(IAsrReadOnlyString* p_string) -> Utils::Expected<const char*>;

public:
    int64_t AddRef();
    int64_t Release();

    // IAsrSettings
    AsrRetReadOnlyString GetString(
        std::string_view        u8_type_string,
        const AsrReadOnlyString key);
    AsrRetBool GetBool(
        std::string_view        u8_type_string,
        const AsrReadOnlyString key);
    AsrRetInt GetInt(
        std::string_view        u8_type_string,
        const AsrReadOnlyString key);
    AsrRetFloat GetFloat(
        std::string_view        u8_type_string,
        const AsrReadOnlyString key);
    // IAsrSettingsForUi
    AsrResult ToString(IAsrReadOnlyString** pp_out_string);
    AsrResult FromString(IAsrReadOnlyString* p_in_settings);
    AsrResult SaveTo(IAsrReadOnlyString* p_path);
    // AsrSettings
    /**
     * @brief Set the Default Values object
     *
     * @param rv_json rvalue of json. You should move it to this function.
     * @return AsrResult
     */
    AsrResult SetDefaultValues(nlohmann::json&& rv_json);
    AsrResult LoadSettings(IAsrReadOnlyString* p_path);
};

extern AsrSettings g_settings;

ASR_CORE_SETTINGSMANAGER_NS_END

#endif // ASR_CORE_SETTINGSMANAGER_GLOBALSETTINGSMANAGER_H
