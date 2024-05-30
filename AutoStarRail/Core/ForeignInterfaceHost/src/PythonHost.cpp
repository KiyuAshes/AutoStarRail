#include <AutoStarRail/Utils/StringUtils.h>

#ifdef ASR_EXPORT_PYTHON

#include "TemporaryPluginObjectStorage.h"
#include <AutoStarRail/Core/Exceptions/PythonException.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/PythonHost.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>

ASR_DISABLE_WARNING_BEGIN
ASR_IGNORE_UNUSED_PARAMETER

// Avoid MSVC Warning C4100
// This warning exists in some specific versions of Python header files
// 一些特定版本的Python头文件中存在这一警告
#include <Python.h>

ASR_DISABLE_WARNING_END

#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

static_assert(
    std::is_same_v<_object*, PyObject*>,
    "Type PyObject is not type _object. Consider to check if \"Python.h\" change "
    "the type declaration.");

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_PYTHONHOST_BEGIN

auto CreateForeignLanguageRuntime(
    [[maybe_unused]] const ForeignLanguageRuntimeFactoryDesc& desc)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>
{
    const auto                      p_runtime = new PythonRuntime{};
    AsrPtr<IForeignLanguageRuntime> result{p_runtime};
    return result;
}

PyObjectPtr::PyObjectPtr(
    PyObject* ptr,
    [[maybe_unused]] PyObjectPtr::AttachOnly) noexcept
    : ptr_(ptr)
{
}

PyObjectPtr::PyObjectPtr(decltype(nullptr) p) noexcept : ptr_(p) {}

PyObjectPtr::PyObjectPtr(PyObject* ptr) : ptr_(ptr) { Py_INCREF(ptr_); }

PyObjectPtr::PyObjectPtr(const PyObjectPtr& other) : ptr_(other.ptr_)
{
    Py_XINCREF(ptr_);
}

PyObjectPtr& PyObjectPtr::operator=(const PyObjectPtr& other)
{
    ptr_ = other.ptr_;
    Py_XINCREF(ptr_);
    return *this;
}

PyObjectPtr::PyObjectPtr(PyObjectPtr&& other) noexcept
    : ptr_(std::exchange(other.ptr_, nullptr))
{
}

PyObjectPtr& PyObjectPtr::operator=(PyObjectPtr&& other) noexcept
{
    ptr_ = std::exchange(other.ptr_, nullptr);
    return *this;
}

PyObjectPtr::~PyObjectPtr() { Py_XDECREF(ptr_); }

PyObjectPtr PyObjectPtr::Attach(PyObject* ptr)
{
    return PyObjectPtr(ptr, AttachOnly{});
}

PyObject** PyObjectPtr::Put() noexcept
{
    Py_XDECREF(ptr_);
    ptr_ = nullptr;
    return &ptr_;
}

bool PyObjectPtr::operator==(const PyObject* const p) const noexcept
{
    return p == ptr_;
}

bool PyObjectPtr::operator==(PyObjectPtr p) const noexcept
{
    return p.Get() == ptr_;
}

PyObject* PyObjectPtr::Get() const noexcept { return ptr_; }

PyObjectPtr::operator bool() const noexcept
{
    return !(Get() == PyObjectPtr());
}

PyObject* PyObjectPtr::Detach() noexcept
{
    return std::exchange(ptr_, nullptr);
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

PyObjectPtr PyUnicodeFromU8String(const char8_t* u8_string)
{
    return PyObjectPtr::Attach(
        PyUnicode_FromString(reinterpret_cast<const char*>(u8_string)));
}

PyObjectPtr PyUnicodeFromU8String(const std::string_view std_string_view)
{
    return PyObjectPtr::Attach(PyUnicode_FromString(std_string_view.data()));
}

template <class T>
bool IsSubDirectory(T path, T root)
{
    while (path != T{})
    {
        if (path == root)
        {
            return true;
        }
        path = path.parent_path();
    }
    return false;
}

auto GetPreferredSeparator() -> const std::u8string&
{
    static std::u8string result{
        []() -> decltype(result)
        {
            std::string                               tmp_result;
            const U_NAMESPACE_QUALIFIER UnicodeString icu_string{
                std::filesystem::path::preferred_separator};
            icu_string.toUTF8String(tmp_result);
            return {ASR_FULL_RANGE_OF(tmp_result)};
        }()};
    return result;
}

ASR_NS_ANONYMOUS_DETAILS_END

class PythonResult : public ASR::Utils::NonCopyableAndNonMovable
{
    PyObjectPtr ptr_;
    bool        owned_;

    void DetachIfNotOwned()
    {
        if (!owned_)
        {
            std::ignore = ptr_.Detach();
        }
    }

    void CheckPointer() const
    {
        if (!ptr_)
        {
            RaiseIfError();
        }
    }

    constexpr static PyObject* TEST_PTR = nullptr;

public:
    explicit PythonResult(PyObjectPtr ptr) : ptr_{ptr}, owned_{true}
    {
        CheckPointer();
    }
    explicit PythonResult(PyObject* ptr) : ptr_{ptr}, owned_{false}
    {
        CheckPointer();
    }
    ~PythonResult() { DetachIfNotOwned(); }
    /**
     * @brief 验证构造此方法的指针是否为空，若为空，抛出
     PythonError异常； 否则将指针作为参数执行传入的函数
     *
     * @tparam F Callable对象的类型
     * @param func Callable对象，接收一个类型为PyObject*的输入，
        输出为PyObject*时，暗示这一用户不具有对象所有权；
        输出为PyObjectPtr时，暗示用户已经妥善处理的对象引用问题；
        输出为其它时，暗示下一次调用then时仍然使用当前的PyObject。
     * @return PythonResult& 本对象的引用
     */
    template <class F>
    PythonResult& then(F func)
    {
        if constexpr (std::is_same_v<decltype(func(TEST_PTR)), PyObject*>)
        {
            auto func_result = func(ptr_.Get());
            DetachIfNotOwned();
            ptr_ = PyObjectPtr::Attach(func_result);
            owned_ = false;
        }
        else if constexpr (std::
                               is_same_v<decltype(func(TEST_PTR)), PyObjectPtr>)
        {
            auto func_result = func(ptr_.Get());
            DetachIfNotOwned();
            ptr_ = func_result;
            owned_ = true;
        }
        else
        {
            func(ptr_.Get());
        }

        CheckPointer();
        return *this;
    }

    auto CheckAndGet() -> PyObjectPtr
    {
        CheckPointer();
        return ptr_;
    }

    /**
     * @brief If internal pointer is null, it indicates that an error occurred
     * in the last operation。 Then this function will THROW an exception.
     */
    void Check() { CheckPointer(); };

    [[noreturn]]
    static void RaiseIfError()
    {
        PyObject* p_type{nullptr};
        PyObject* p_value{nullptr};
        PyObject* p_trace_back{nullptr};

        // TODO: 这些C_API已经在Python 3.12中弃用
        // TODO: 请在未来提供对Python3.12版本的支持
        ::PyErr_Fetch(&p_type, &p_value, &p_trace_back);

        const auto traceback =
            PyObjectPtr::Attach(::PyImport_ImportModule("traceback"));
        const auto traceback_format_exception = PyObjectPtr::Attach(
            ::PyObject_GetAttrString(traceback.Get(), "format_exception"));

        const auto format_args = PyObjectPtr::Attach(PyTuple_New(3));

        if (p_type == nullptr)
        {
            // 未知错误，直接Print试试
            ::PyErr_Print();
            throw PythonException{
                "Error happened when calling python code,"
                "but it seems that no error is set in Python."};
        }
        ::PyErr_NormalizeException(&p_type, &p_value, &p_trace_back);
        Py_INCREF(p_type);
        ::PyTuple_SetItem(format_args.Get(), 0, p_type);
        Py_IncRef(p_value);
        ::PyTuple_SetItem(format_args.Get(), 1, p_value);
        if (p_trace_back != nullptr)
        {
            Py_INCREF(p_trace_back);
            ::PyException_SetTraceback(p_value, p_trace_back);
            ::PyTuple_SetItem(format_args.Get(), 2, p_trace_back);
        }
        else
        {
            Py_INCREF(Py_None);
            ::PyTuple_SetItem(format_args.Get(), 2, Py_None);
        }
        const auto formatted_list = PyObjectPtr::Attach(PyObject_Call(
            traceback_format_exception.Get(),
            format_args.Get(),
            nullptr));
        if (::PyList_Check(formatted_list.Get()) == false)
        {
            throw PythonException{"Error happened when calling python code,"
                                  "but formatted_list is not a list."};
        }

        size_t stack_trace_message_size{0};
        auto   string_list = ASR::Utils::MakeEmptyCOntainerOfReservedSize<
            std::vector<std::pair<const char*, size_t>>>(10);

        const auto list_size = ::PyList_Size(formatted_list.Get());
        for (Py_ssize_t i = 0; i < list_size; ++i)
        {
            const auto formatted_string =
                ::PyList_GetItem(formatted_list.Get(), i);
            if (::PyUnicode_Check(formatted_string) == false)
            {
                continue;
            }
            Py_ssize_t a_line_message_size{0};
            const auto a_line_message = ::PyUnicode_AsUTF8AndSize(
                formatted_string,
                &a_line_message_size);
            if (a_line_message != nullptr)
            {
                string_list.emplace_back(a_line_message, a_line_message_size);
                stack_trace_message_size += a_line_message_size;
            }
        }
        std::string stack_trace_message{};
        stack_trace_message.reserve(stack_trace_message_size);
        std::for_each(
            ASR_FULL_RANGE_OF(string_list),
            [&stack_trace_message](auto string_size_pair)
            {
                stack_trace_message.append(
                    string_size_pair.first,
                    string_size_pair.second);
            });

        ::PyErr_Restore(p_type, p_value, p_trace_back);

        throw PythonException{stack_trace_message};
    }
};

class PyInterpreter
{
    PyObjectPtr sys_module_;

    struct InitException : std::runtime_error
    {
        using Base = std::runtime_error;
        InitException()
            : Base{"Error occurred when initialize Python interpreter."}
        {
        }
        [[noreturn]]
        static void Raise()
        {
            throw InitException();
        }
    };

    void Init()
    {
        if (Py_IsInitialized())
        {
            return;
        }

        Py_Initialize();
        if (!Py_IsInitialized())
        {
            InitException::Raise();
        }

        // create folder path string
        const auto u8_plugin_folder =
            std::u8string{u8"\""} + std::filesystem::current_path().u8string()
            + std::u8string{u8"\""};
        // {anonymous variable 0} = "CURRENT PATH"
        auto py_plugin_folder_path =
            PythonResult{
                Details::PyUnicodeFromU8String(u8_plugin_folder.c_str())}
                .CheckAndGet();
        // {anonymous variable 1} = 0
        auto py_zero =
            PythonResult{PyObjectPtr::Attach(PyLong_FromLong(0))}.CheckAndGet();
        // {anonymous variable 2} = ({anonymous variable 0}, {anonymous variable
        // 1})
        auto py_2_element_tuple =
            PythonResult{PyObjectPtr::Attach(PyTuple_New(2))}
                .then(
                    [&py_zero, &py_plugin_folder_path](auto py_2_element_tuple)
                    {
                        int set_tuple_result{0};
                        set_tuple_result = PyTuple_SetItem(
                            py_2_element_tuple,
                            0,
                            py_zero.Get());
                        if (set_tuple_result == -1)
                        {
                            PythonResult::RaiseIfError();
                        }
                        set_tuple_result = PyTuple_SetItem(
                            py_2_element_tuple,
                            1,
                            py_plugin_folder_path.Get());
                        if (set_tuple_result == -1)
                        {
                            PythonResult::RaiseIfError();
                        }
                        return PyObjectPtr{py_2_element_tuple};
                    })
                .CheckAndGet();
        // import sys
        auto py_str_sys = Details::PyUnicodeFromU8String(
            ASR_UTILS_STRINGUTILS_DEFINE_U8STR("sys"));
        sys_module_ = PyObjectPtr::Attach(PyImport_Import(py_str_sys.Get()));
        PythonResult{sys_module_}
            .then(
                // {anonymous variable 3} = sys.path
                [](auto py_sys_module) {
                    return PyObjectPtr::Attach(
                        PyObject_GetAttrString(py_sys_module, "path"));
                })
            .then(
                // {anonymous variable 4} = sys.path.insert
                [](auto py_sys_path) {
                    return PyObjectPtr::Attach(
                        PyObject_GetAttrString(py_sys_path, "path"));
                })
            .then(
                // {anonymous variable 4}({anonymous variable 2})
                [&py_2_element_tuple](auto py_sys_path_insert)
                {
                    return PyObjectPtr::Attach(PyObject_Call(
                        py_sys_path_insert,
                        py_2_element_tuple.Get(),
                        nullptr));
                })
            .Check();
    }
};

ASR_NS_ANONYMOUS_DETAILS_BEGIN

class PythonRuntimeSingleton
{
    PythonRuntimeSingleton() = delete;
    ~PythonRuntimeSingleton() = delete;

public:
    static bool Initialize()
    {
        static bool is_initialized = []
        {
            ::Py_Initialize();
            if (!::Py_IsInitialized())
            {
                throw std::runtime_error("Failed to initialize Python");
            }
            return true;
        }();
        return is_initialized;
    }
};

ASR_NS_ANONYMOUS_DETAILS_END

PythonRuntime::PythonRuntime()
{
    Details::PythonRuntimeSingleton::Initialize();
}

PythonRuntime::~PythonRuntime() = default;

auto PythonRuntime::LoadPlugin(const std::filesystem::path& path)
    -> ASR::Utils::Expected<CommonPluginPtr>
{
    ASR::Utils::OnExit on_exit{
        [] { ::PyEval_ReleaseThread(::PyThreadState_Get()); }};
    AsrPtr<IAsrSwigPlugin> result{};

    const auto expected_py_module = ImportPluginModule(path);
    if (!expected_py_module)
    {
        return tl::make_unexpected(expected_py_module.error());
    }
    const auto& py_module = expected_py_module.value();
    p_plugin_module = py_module;

    try
    {
        const auto py_plugin_initializer =
            GetPluginInitializer(*py_module.Get());
        PythonResult{PyObjectPtr::Attach(PyTuple_New(0))}
            .then(
                [&result, &py_plugin_initializer](auto py_null_tuple)
                {
                    auto owner = g_plugin_object.GetOwnership();
                    auto lambda_result = PyObjectPtr::Attach(PyObject_Call(
                        py_plugin_initializer.Get(),
                        py_null_tuple,
                        nullptr));
                    if (lambda_result)
                    {
                        result = owner.GetObject();
                    }
                    return lambda_result;
                })
            .Check();

        // Initialize successfully. Store the module to member variable.
        p_plugin_module = py_module;
        return result;
    }
    catch (const PythonException& ex)
    {
        ASR_CORE_LOG_EXCEPTION(ex);
        return tl::make_unexpected(ASR_E_PYTHON_ERROR);
    }
}

auto PythonRuntime::ResolveClassName(const std::filesystem::path& relative_path)
    -> ASR::Utils::Expected<std::u8string>
{
    std::u8string result{};

    if (relative_path.begin() == relative_path.end())
    {
        return tl::make_unexpected(ASR_E_INVALID_PATH);
    }

    const auto it_end = std::end(relative_path);
    auto       it = relative_path.begin();

    std::u8string part_string{};

    for (auto it_next = std::next(relative_path.begin()); it_next != it_end;
         ++it, ++it_next)
    {
        part_string = it->u8string();
        if (part_string == Details::GetPreferredSeparator())
        {
            return tl::make_unexpected(ASR_E_INVALID_PATH);
        }
        result += part_string;
        result += u8'.';
    }
    result += it->stem().u8string();
    return result;
}

auto PythonRuntime::ImportPluginModule(
    const std::filesystem::path& py_plugin_initializer)
    -> ASR::Utils::Expected<PyObjectPtr>
{
    const auto current_path = std::filesystem::current_path();
    if (!Details::IsSubDirectory(
            py_plugin_initializer,
            std::filesystem::current_path()))
    {
        ASR_CORE_LOG_ERROR(
            "The given path is not  is not a subdirectory of the current working directory.");

        const auto& w_path_string = py_plugin_initializer.wstring();
        const auto& w_current_path_string = current_path.wstring();
        // Use IAsrReadOnlySting to convert wchar string to utf8 string.
        AsrPtr<IAsrReadOnlyString> p_path_string{};
        AsrPtr<IAsrReadOnlyString> p_current_path_string{};
        CreateIAsrReadOnlyStringFromWChar(
            w_path_string.c_str(),
            w_path_string.size(),
            p_path_string.Put());
        CreateIAsrReadOnlyStringFromWChar(
            w_current_path_string.c_str(),
            w_current_path_string.size(),
            p_current_path_string.Put());
        ASR_CORE_LOG_ERROR(
            "NOTE: The given path is \"{}\".\nThe current path is \"{}\".",
            p_path_string,
            p_current_path_string);
        return tl::make_unexpected(ASR_E_INVALID_PATH);
    }

    const auto relative_path = std::filesystem::relative(py_plugin_initializer);
    const auto package_path = ResolveClassName(relative_path);
    if (!package_path)
    {
        return tl::make_unexpected(package_path.error());
    }

    try
    {
        return PythonResult{
            Details::PyUnicodeFromU8String(package_path.value().c_str())}
            .then(
                [](auto py_package_path) {
                    return PyObjectPtr::Attach(
                        PyImport_Import(py_package_path));
                })
            .CheckAndGet();
    }
    catch (const PythonException& ex)
    {
        ASR_CORE_LOG_ERROR("Import python plugin module failed.");

        ASR_CORE_LOG_EXCEPTION(ex);

        ASR_CORE_LOG_ERROR(
            "NOTE: The python plugin module name is \"{}\".",
            reinterpret_cast<const char*>(package_path.value().c_str()));
        return tl::make_unexpected(ASR_E_PYTHON_ERROR);
    }
}

auto PythonRuntime::GetPluginInitializer(PyObject& py_module) -> PyObjectPtr
{
    return PythonResult{
        PyObjectPtr::Attach(
            PyObject_GetAttrString(&py_module, ASRCOCREATEPLUGIN_NAME))}
        .CheckAndGet();
}

void RaisePythonInterpreterException() { PythonResult::RaiseIfError(); }

ASR_NS_PYTHONHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_EXPORT_PYTHON