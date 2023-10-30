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
                IAsrSwigPlugin))
            .and_then(ASR_CORE_FOREIGNINTERFACEHOST_CREATE_CPP_TO_SWIG_OBJECT(
                IAsrSwigErrorLens))
            .or_else([&result](const auto error_code) { result = error_code; });

        return result;
    }
    catch (const std::bad_alloc& ex)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

// -------------------- implementation of SwigToCpp class --------------------

AsrResult SwigToCpp<IAsrSwigErrorLens>::GetErrorMessage(
    IAsrReadOnlyString*  locale_name,
    AsrResult            error_code,
    IAsrReadOnlyString** pp_out_string){
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigErrorLens::GetErrorMessage,
        pp_out_string,
        locale_name,
        error_code)}

AsrResult SwigToCpp<IAsrSwigPlugin>::EnumFeature(
    const size_t      index,
    AsrPluginFeature* p_out_feature)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigPlugin::EnumFeature,
        p_out_feature,
        index);
}

AsrResult SwigToCpp<IAsrSwigPlugin>::CreateFeatureInterface(
    AsrPluginFeature feature,
    void**           pp_out_interface)
{
    ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
        p_impl_.Get(),
        &IAsrSwigPlugin::CreateFeatureInterface,
        pp_out_interface,
        feature);
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
