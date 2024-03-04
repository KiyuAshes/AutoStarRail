#include <AutoStarRail/Core/ForeignInterfaceHost/ForeignInterfaceHost.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <AutoStarRail/Utils/UnexpectedEnumException.h>
#include <AutoStarRail/Utils/EnumUtils.hpp>
#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <stdexcept>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <magic_enum_format.hpp>
#include <AutoStarRail/Utils/StringUtils.h>

template <class T>
struct ASR_FMT_NS::formatter<std::vector<T>, char>
    : public formatter<std::string, char>
{
    auto format(const std::vector<T>& value, format_context& ctx) const ->
        typename std::remove_reference_t<decltype(ctx)>::iterator
    {
        auto result = ASR_FMT_NS::format_to(ctx.out(), "[");
        std::for_each(
            value.begin(),
            std::prev(value.end()),
            [&result](const auto& s)
            { result = ASR_FMT_NS::format_to(result, "{},", s); });
        if (!value.empty() && value.begin() != value.end())
        {
            result = ASR_FMT_NS::format_to(result, "{}", value.back());
        }
        result = ASR_FMT_NS::format_to(result, "]");
        return result;
    }
};

template <class T>
struct ASR_FMT_NS::formatter<std::optional<std::vector<T>>, char>
    : public formatter<std::string, char>
{
    auto format(
        const std::optional<std::vector<T>>& opt_value,
        format_context&                      ctx) const ->
        typename std::remove_reference_t<decltype(ctx)>::iterator
    {
        auto result = ctx.out();
        if (opt_value)
        {
            result = ASR_FMT_NS::format_to(result, "{}", opt_value.value());
        }
        else
        {
            result = ASR_FMT_NS::format_to(result, "null");
        }
        return result;
    }
};

template <class T>
struct ASR_FMT_NS::formatter<std::optional<T>, char>
    : public formatter<std::string, char>
{
    auto format(const std::optional<T>& opt_value, format_context& ctx) const ->
        typename std::remove_reference_t<decltype(ctx)>::iterator
    {
        auto result = ctx.out();
        if (opt_value)
        {
            ASR_FMT_NS::format_to(result, "{}", opt_value.value());
        }
        else
        {
            ASR_FMT_NS::format_to(result, "null");
        }
        return result;
    }
};

#define ASR_CORE_FOREIGNINTERFACEHOST_VERIFY_AND_FORMAT(enum_value, type)      \
    case enum_value:                                                           \
    {                                                                          \
        auto* const p_value = std::get_if<type>(&desc.default_value);          \
        if (p_value == nullptr)                                                \
        {                                                                      \
            result = ASR_FMT_NS::format_to(                                    \
                result,                                                        \
                "Unexpected value. Expected type is \"" #type "\".");          \
            break;                                                             \
        }                                                                      \
        result =                                                               \
            ASR_FMT_NS::format_to(result, "default_value = {}\n", *p_value);   \
        break;                                                                 \
    }

#define ASR_CORE_FOREIGNINTERFACEHOST_VAR(x) #x " = {}\n", desc.x

auto(ASR_FMT_NS::formatter<
     ASR::Core::ForeignInterfaceHost::PluginSettingDesc,
     char>::format)(
    const ASR::Core::ForeignInterfaceHost::PluginSettingDesc& desc,
    format_context&                                           ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    auto result = ctx.out();
    // 当PluginSettingDesc更新时记得更新这里
    result =
        ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(name));
    if (std::get_if<std::monostate>(&desc.default_value) == nullptr)
    {
        switch (desc.type)
        {
            ASR_CORE_FOREIGNINTERFACEHOST_VERIFY_AND_FORMAT(
                ASR_TYPE_BOOL,
                bool);
            ASR_CORE_FOREIGNINTERFACEHOST_VERIFY_AND_FORMAT(
                ASR_TYPE_INT,
                std::int64_t);
            ASR_CORE_FOREIGNINTERFACEHOST_VERIFY_AND_FORMAT(
                ASR_TYPE_FLOAT,
                float);
            ASR_CORE_FOREIGNINTERFACEHOST_VERIFY_AND_FORMAT(
                ASR_TYPE_STRING,
                std::string);
        default:
            throw ASR::Utils::UnexpectedEnumException::FromEnum(desc.type);
        }
    }
    else
    {
        result = ASR_FMT_NS::format_to(result, "empty default value\n");
    }
    result = ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(description));
    result = ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(enum_values));
    result = ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(enum_descriptions));
    result = ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(deprecation_message));
    result =
        ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(type));

    return result;
}

auto(ASR_FMT_NS::formatter<ASR::Core::ForeignInterfaceHost::PluginDesc, char>::
         format)(
    const ASR::Core::ForeignInterfaceHost::PluginDesc& desc,
    format_context&                                    ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    auto result = ctx.out();

    ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(plugin_metadata_version));
    ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(language));
    ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(name));
    ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(description));
    ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(author));
    ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(version));
    ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(supported_system));
    ASR_FMT_NS::format_to(
        result,
        ASR_CORE_FOREIGNINTERFACEHOST_VAR(plugin_filename_extension));
    ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(guid));
    ASR_FMT_NS::format_to(result, ASR_CORE_FOREIGNINTERFACEHOST_VAR(settings));

    return result;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

namespace Details
{
    template <class T>
    void OptionalFromJson(
        const nlohmann::json& json,
        const char*           key,
        std::optional<T>&     opt_value)
    {
        const auto it = json.find(key);
        if (it != json.end())
        {
            opt_value = it->template get<T>();
        }
        else
        {
            opt_value = std::nullopt;
        }
    }

    template <class T>
    void OptionalToJson(
        nlohmann::json&         json,
        const char*             key,
        const std::optional<T>& opt_value)
    {
        if (opt_value)
        {
            json[key] = opt_value.value();
        }
    }
}

void from_json(const nlohmann::json& input, PluginSettingDesc& output)
{
    ASR_CORE_TRACE_SCOPE;

    output.name = input.at("name").get<std::string>();
    output.type = Utils::JsonToEnum<AsrType>(input, "type");
    switch (output.type)
    {
    case ASR_TYPE_BOOL:
        output.default_value = input.at("defaultValue").get<bool>();
        break;
    case ASR_TYPE_INT:
        output.default_value = input.at("defaultValue").get<std::int64_t>();
        break;
    case ASR_TYPE_FLOAT:
        output.default_value = input.at("defaultValue").get<float>();
        break;
    case ASR_TYPE_STRING:
        output.default_value = input.at("defaultValue").get<std::string>();
        break;
    default:
        throw Utils::UnexpectedEnumException::FromEnum(output.type);
    }
    Details::OptionalFromJson(input, "description", output.description);
    Details::OptionalFromJson(input, "enumValues", output.enum_values);
    Details::OptionalFromJson(
        input,
        "enumDescriptions",
        output.enum_descriptions);
    Details::OptionalFromJson(
        input,
        "deprecation_message",
        output.deprecation_message);
}

void from_json(const nlohmann::json& input, PluginDesc& output)
{
    ASR_CORE_TRACE_SCOPE;

    input.at("pluginMetadataVersion").get_to(output.plugin_metadata_version);
    input.at("language").get_to(output.language);
    input.at("name").get_to(output.name);
    input.at("description").get_to(output.description);
    input.at("author").get_to(output.author);
    input.at("version").get_to(output.version);
    input.at("supportedSystem").get_to(output.supported_system);
    input.at("pluginFilenameExtension")
        .get_to(output.plugin_filename_extension);
    if (const auto it_resource_path = input.find("resourcePath");
        it_resource_path != input.end())
    {
        output.opt_resource_path = it_resource_path->get<std::string>();
    }
    else
    {
        output.opt_resource_path =
            ASR_UTILS_STRINGUTILS_DEFINE_U8STR("resource");
    }
    const auto guid_string = input.at("guid").get<std::string>();
    output.guid = MakeAsrGuid(guid_string);
    input.at("settings").get_to(output.settings);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
