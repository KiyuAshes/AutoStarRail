#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

using CppSwigMap = boost::bimaps::bimap<
    // cpp iid
    boost::bimaps::unordered_set_of<AsrGuid, std::hash<AsrGuid>>,
    // swig iid
    boost::bimaps::unordered_set_of<AsrGuid, std::hash<AsrGuid>>>;

#define ASR_CORE_FOREIGNINTERFACEHOST_DEFINE_CPP_TO_SWIG_MAP_ITEM(name)        \
    {                                                                          \
        AsrIidOf<IAsr##name>(), AsrIidOf<IAsrSwig##name>()                     \
    }

/**
 * @brief The left side is cpp iid, while the right side is swig iid.
 */
const CppSwigMap g_cpp_swig_map = []() -> CppSwigMap
{
    std::initializer_list<CppSwigMap::value_type> list{
        ASR_CORE_FOREIGNINTERFACEHOST_DEFINE_CPP_TO_SWIG_MAP_ITEM(Base),
        ASR_CORE_FOREIGNINTERFACEHOST_DEFINE_CPP_TO_SWIG_MAP_ITEM(Inspectable)

    };
    return {list.begin(), list.end()};
}();

auto ConvertCppIidToSwigIid(const AsrGuid& cpp_iid)
    -> ASR::Utils::Expected<AsrGuid>
{
    auto it = g_cpp_swig_map.left.find(cpp_iid);
    if (it == g_cpp_swig_map.left.end())
    {
        return tl::make_unexpected(ASR_E_NO_INTERFACE);
    }
    return it->second;
}

bool IsCppIid(const AsrGuid& cpp_iid)
{
    const auto it = g_cpp_swig_map.left.find(cpp_iid);
    return it != g_cpp_swig_map.left.end();
}

bool IsSwigIid(const AsrGuid& swig_iid)
{
    const auto it = g_cpp_swig_map.right.find(swig_iid);
    return it != g_cpp_swig_map.right.end();
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class T>
auto CreateCppToSwigObjectImpl(
    const AsrGuid& swig_iid,
    void*          p_swig_object,
    void**         pp_out_cpp_object) -> ASR::Utils::Expected<void>
{
    if (swig_iid == AsrIidOf<T>())
    {
        auto* const p_cpp_object =
            new SwigToCpp<T>(static_cast<T*>(p_swig_object));
        p_cpp_object->AddRef();
        *pp_out_cpp_object = p_cpp_object;
        return tl::make_unexpected(ASR_S_OK);
    }
    return {};
}

ASR_NS_ANONYMOUS_DETAILS_END

#define ASR_CORE_FOREIGNINTERFACEHOST_CREATE_CPP_TO_SWIG_OBJECT(Type)          \
    [&]()                                                                      \
    {                                                                          \
        return Details::CreateCppToSwigObjectImpl<Type>(                       \
            swig_iid,                                                          \
            p_swig_object,                                                     \
            pp_out_cpp_object);                                                \
    }

AsrResult CreateCppToSwigObject(
    const AsrGuid& swig_iid,
    void*          p_swig_object,
    void**         pp_out_cpp_object)
{
    try
    {
        AsrResult result{ASR_E_UNDEFINED_RETURN_VALUE};

        // TODO: 添加所有PluginInterface中的导出类型
        Details::CreateCppToSwigObjectImpl<IAsrSwigBase>(
            swig_iid,
            p_swig_object,
            pp_out_cpp_object)
            .and_then(ASR_CORE_FOREIGNINTERFACEHOST_CREATE_CPP_TO_SWIG_OBJECT(
                IAsrSwigInspectable))
            .and_then(ASR_CORE_FOREIGNINTERFACEHOST_CREATE_CPP_TO_SWIG_OBJECT(
                IAsrSwigErrorLens))
            .or_else([&result](const auto error_code) { result = error_code; });

        return result;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

// -------------------- implementation of SwigToCpp class --------------------

AsrResult SwigToCpp<IAsrSwigErrorLens>::GetSupportedIids(
    IAsrIidVector** pp_out_iids)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigErrorLens::GetSupportedIids,
        pp_out_iids);
}

AsrResult SwigToCpp<IAsrSwigErrorLens>::GetErrorMessage(
    IAsrReadOnlyString*  locale_name,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_string)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigErrorLens::GetErrorMessage,
        pp_out_string,
        locale_name,
        error_code);
}

AsrResult SwigToCpp<IAsrSwigTask>::GetIids(IAsrIidVector** pp_out_iid_vector)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetIids,
        pp_out_iid_vector);
}
AsrResult SwigToCpp<IAsrSwigTask>::GetRuntimeClassName(
    IAsrReadOnlyString** pp_out_name)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetRuntimeClassName,
        pp_out_name);
}

AsrResult SwigToCpp<IAsrSwigTask>::Do(
    IAsrReadOnlyString* p_connection_json,
    IAsrReadOnlyString* p_task_settings_json)
{
    try
    {
        const auto result = (p_impl_.Get()->IAsrSwigTask::Do)(
            p_connection_json,
            p_task_settings_json);
        return result;
    }
    catch (const std::exception& ex)
    {
        ASR_CORE_LOG_ERROR(ex.what());
        return ASR_E_SWIG_INTERNAL_ERROR;
    }
}

AsrResult SwigToCpp<IAsrSwigTask>::GetNextExecutionTime(AsrDate* p_out_date)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetNextExecutionTime,
        p_out_date);
}

AsrResult SwigToCpp<IAsrSwigTask>::GetName(IAsrReadOnlyString** pp_out_name)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetName,
        pp_out_name);
}

AsrResult SwigToCpp<IAsrSwigTask>::GetDescription(
    IAsrReadOnlyString** pp_out_settings)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetDescription,
        pp_out_settings);
}

AsrResult SwigToCpp<IAsrSwigTask>::GetLabel(IAsrReadOnlyString** pp_out_label)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetLabel,
        pp_out_label);
}

AsrResult SwigToCpp<IAsrSwigTask>::GetType(AsrTaskType* p_out_type)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigTask::GetType,
        p_out_type);
}

// TODO: IAsrSwigCaptureFactory CreateInstance

AsrResult CommonPluginEnumFeature(
    const CommonPluginPtr& p_this,
    size_t                 index,
    AsrPluginFeature*      p_out_feature)
{
    if (p_out_feature == nullptr)
    {
        return ASR_E_INVALID_POINTER;
    }

    return std::visit(
        ASR::Utils::overload_set{
            [index, p_out_feature](AsrPtr<IAsrPlugin> p_plugin) -> AsrResult
            { return p_plugin->EnumFeature(index, p_out_feature); },
            [index,
             p_out_feature](AsrPtr<IAsrSwigPlugin> p_swig_plugin) -> AsrResult
            {
                const auto result = p_swig_plugin->EnumFeature(index);
                if (ASR::IsOk(result.error_code))
                {
                    *p_out_feature = result.value;
                }
                return result.error_code;
            }},
        p_this);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
