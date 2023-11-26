#ifndef ASR_CORE_FOREIGNINTERFACEHOST_CPPSWIGINTEROP_H
#define ASR_CORE_FOREIGNINTERFACEHOST_CPPSWIGINTEROP_H

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/IsCastAvailableImpl.hpp>
#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>
#include <AutoStarRail/Utils/Expected.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <cstdint>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

template <class T>
concept is_asr_swig_interface = std::is_base_of_v<IAsrSwigBase, T>;

template <class T>
concept is_asr_interface = std::is_base_of_v<IAsrBase, T>;

template <class T>
class SwigToCpp;

auto ConvertCppIidToSwigIid(const AsrGuid& cpp_iid)
    -> ASR::Utils::Expected<AsrGuid>;

// TODO: 实现这个函数
auto ConvertSwigIidToCppIid(const AsrGuid& swig_iid)
    -> ASR::Utils::Expected<AsrGuid>;

bool IsCppIid(const AsrGuid& cpp_iid);

bool IsSwigIid(const AsrGuid& swig_iid);

/**
 * @brief 使用SWIG接口iid和对象指针创建对应的C++对象包装
 * @param swig_iid swig 接口类型的iid
 * @param p_swig_object swig 对象指针
 * @param pp_out_cpp_object 输出的 cpp 对象
 * @return 操作结果
 */
AsrResult CreateCppToSwigObject(
    const AsrGuid& swig_iid,
    void*          p_swig_object,
    void**         pp_out_cpp_object);

template <is_asr_interface T>
auto ConvertCppIidToSwigIid()
{
    return ConvertCppIidToSwigIid(AsrIidOf<T>());
}

template <is_asr_swig_interface T>
auto ConvertSwigIidToCppIid() -> ASR::Utils::Expected<AsrGuid>
{
    return ConvertSwigIidToCppIid(AsrIidOf<T>());
}

struct FunctionArgumentsSeparator
{
};

/**
 * @brief
 * 这个函数适用于仅一个输出参数的情况，注意：在指定了输出参数后，再指定输入参数。
 * @tparam T
 * @tparam OutputArg
 * @tparam InputArgs
 * @param p_swig_object
 * @param output_arg
 * @param input_args
 * @return
 */
template <
    class T2Function,
    T2Function            T2FunctionPointer,
    is_asr_swig_interface SwigT,
    class OutputArg,
    class... InputArgs>
[[nodiscard]]
auto CallSwigMethod(
    SwigT*      p_swig_object,
    OutputArg&& output_arg,
    InputArgs&&... input_args)
{
    const auto result = (p_swig_object->*T2FunctionPointer)(
        std::forward<InputArgs>(input_args)...);
    *std::forward<OutputArg>(output_arg) =
        static_cast<std::remove_reference_t<decltype(*output_arg)>>(
            result.value);
    return result.error_code;
}

#define ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD(                        \
    p_swig_object,                                                             \
    function_pointer,                                                          \
    ...)                                                                       \
    CallSwigMethod<decltype(function_pointer), function_pointer>(              \
        p_swig_object,                                                         \
        __VA_ARGS__)

/**
 * @brief This class can be seen as a smart pointer.
 * @tparam SwigT
 * @tparam T
 */
template <is_asr_swig_interface SwigT, is_asr_interface T>
class SwigToCppBase : public T
{
protected:
    ASR::AsrPtr<SwigT> p_impl_;

public:
    template <class Other>
    explicit SwigToCppBase(ASR::AsrPtr<Other> p_impl) : p_impl_{p_impl}
    {
    }

    template <class Other>
    explicit SwigToCppBase(SwigToCpp<Other> other) : p_impl_{other.p_impl_}
    {
    }

    template <class Other, class = std::enable_if<is_asr_swig_interface<Other>>>
    explicit SwigToCppBase(Other* p_other) : p_impl_{p_other, take_ownership}
    {
    }

    SwigToCppBase() = default;

    int64_t AddRef() final
    {
        try
        {
            return p_impl_->AddRef();
        }
        catch (std::exception& ex)
        {
            ASR_CORE_LOG_ERROR(ex.what());
            throw;
        }
    }

    int64_t Release() final
    {
        try
        {
            return p_impl_->Release();
        }
        catch (std::exception& ex)
        {
            ASR_CORE_LOG_ERROR(ex.what());
            throw;
        }
    }

    /**
     * @brief 只会接收CPP版本的IID
     * @param iid
     * @param pp_out_object
     * @return
     */
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) final
    {
        const auto get_default_query_interface_result =
            ASR::Utils::QueryInterface<T>(this, iid, pp_out_object);
        if (IsOk(get_default_query_interface_result)
            || get_default_query_interface_result != ASR_E_NO_INTERFACE)
        {
            return get_default_query_interface_result;
        }

        if (const auto swig_iid = ConvertCppIidToSwigIid(iid); swig_iid)
        {
            AsrRetSwigBase result;
            try
            {
                result = p_impl_->QueryInterface(swig_iid.value());
            }
            catch (std::exception& ex)
            {
                ASR_CORE_LOG_EXCEPTION(ex);
                return ASR_E_SWIG_INTERNAL_ERROR;
            }
            if (IsOk(result.error_code))
            {
                if (pp_out_object == nullptr)
                {
                    return ASR_E_INVALID_POINTER;
                }
                return CreateCppToSwigObject(
                    swig_iid.value(),
                    result.value,
                    pp_out_object);
            }
            AsrPtr<IAsrReadOnlyString> predefined_error_explanation{};
            ::AsrGetPredefinedErrorMessage(
                result.error_code,
                predefined_error_explanation.Put());
            ASR_CORE_LOG_ERROR(
                "Error happened in class IAsrSwigBase. Error code: {}. Explanation: {}.",
                result.error_code,
                predefined_error_explanation);
            return ASR_E_NO_INTERFACE;
        }
        return ASR_E_NO_INTERFACE;
    };

    [[nodiscard]]
    auto Get() const noexcept
    {
        return p_impl_;
    }

    [[nodiscard]]
    T*
    operator->() const noexcept
    {
        return static_cast<T*>(this);
    }
    [[nodiscard]]
    T&
    operator*() const noexcept
    {
        return static_cast<T&>(*this);
    }
};

#define ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION( \
    p_swig_object,                                                                \
    function_pointer,                                                             \
    ...)                                                                          \
    try                                                                           \
    {                                                                             \
        return ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD(                    \
            p_swig_object,                                                        \
            function_pointer,                                                     \
            __VA_ARGS__);                                                         \
    }                                                                             \
    catch (const std::exception& ex)                                              \
    {                                                                             \
        ASR_CORE_LOG_ERROR(ex.what());                                            \
        return ASR_E_SWIG_INTERNAL_ERROR;                                         \
    }

template <is_asr_swig_interface SwigT, is_asr_interface T>
class SwigToCppInspectable : public SwigToCppBase<SwigT, T>
{
    using Base = SwigToCppBase<SwigT, T>;

public:
    using Base::Base;
    /**
     * @brief 返回SWIG对象定义所有IID
     * @param pp_out_vector
     * @return
     */
    AsrResult GetIids(IAsrIidVector** pp_out_vector) override{
        ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
            Base::p_impl_.Get(),
            &IAsrSwigInspectable::GetIids,
            pp_out_vector)} AsrResult
        GetRuntimeClassName(IAsrReadOnlyString** pp_out_name) override
    {
        ASR_CORE_FOREIGNINTERFACEHOST_CALL_SWIG_METHOD_IMPL_AND_HANDLE_EXCEPTION(
            Base::p_impl_.Get(),
            &IAsrSwigInspectable::GetRuntimeClassName,
            pp_out_name)
    }
};

template <>
class SwigToCpp<IAsrSwigBase> final
    : public SwigToCppBase<IAsrSwigBase, IAsrBase>
{
public:
    ASR_USING_BASE_CTOR(SwigToCppBase);
};

template <>
class SwigToCpp<IAsrSwigInspectable> final
    : public SwigToCppInspectable<IAsrSwigInspectable, IAsrInspectable>
{
public:
    ASR_USING_BASE_CTOR(SwigToCppInspectable);
};

template <>
class SwigToCpp<IAsrSwigErrorLens> final
    : public SwigToCppBase<IAsrSwigErrorLens, IAsrErrorLens>
{
public:
    ASR_USING_BASE_CTOR(SwigToCppBase);

    AsrResult GetSupportedIids(IAsrIidVector** pp_out_iids) override;
    AsrResult GetErrorMessage(
        IAsrReadOnlyString*  locale_name,
        AsrResult            error_code,
        IAsrReadOnlyString** pp_out_string) override;
};

AsrResult CommonPluginEnumFeature(
    const CommonPluginPtr& p_this,
    size_t                 index,
    AsrPluginFeature*      p_out_feature);

template <class SwigT>
class CppToSwig;

template <is_asr_swig_interface SwigT, is_asr_interface T>
class CppToSwigBase : public SwigT
{
protected:
    ASR::AsrPtr<T> p_impl_;

public:
    template <class Other>
    CppToSwigBase(ASR::AsrPtr<Other> p_impl) : p_impl_{p_impl}
    {
    }
    template <class Other>
    explicit CppToSwigBase(CppToSwig<Other> other) : p_impl_{other.p_impl_}
    {
    }
    ~CppToSwigBase() override = default;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_CPPSWIGINTEROP_H
