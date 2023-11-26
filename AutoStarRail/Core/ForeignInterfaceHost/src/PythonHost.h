#ifndef ASR_CORE_FOREIGNINTERFACEHOST_PYTHONHOST_H
#define ASR_CORE_FOREIGNINTERFACEHOST_PYTHONHOST_H

#ifdef ASR_EXPORT_PYTHON

#include <AutoStarRail/Core/ForeignInterfaceHost/IForeignLanguageRuntime.h>

/**
 * @brief 在Python.h中声明，其中：typedef _object* PyObject;
 *
 */
struct _object;

#define ASR_NS_PYTHONHOST_BEGIN                                                \
    namespace PythonHost                                                       \
    {

#define ASR_NS_PYTHONHOST_END }

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_PYTHONHOST_BEGIN

struct PythonRuntimeDesc : public ForeignLanguageRuntimeFactoryDesc
{
};

auto CreateForeignLanguageRuntime(const PythonRuntimeDesc& desc)
    -> ASR::Utils::Expected<AsrPtr<IForeignLanguageRuntime>>;

class PyObjectPtr
{
    struct AttachOnly
    {
    };
    _object* ptr_{nullptr};
    explicit PyObjectPtr(_object* ptr, [[maybe_unused]] AttachOnly) noexcept;

public:
    explicit PyObjectPtr(decltype(nullptr) p = nullptr) noexcept;
    explicit PyObjectPtr(_object* ptr);
    PyObjectPtr(const PyObjectPtr& other);
    PyObjectPtr& operator=(const PyObjectPtr& other);
    PyObjectPtr(PyObjectPtr&& other) noexcept;
    PyObjectPtr& operator=(PyObjectPtr&& other) noexcept;
    ~PyObjectPtr();

    [[nodiscard]]
    static PyObjectPtr Attach(_object* ptr);
    [[nodiscard]]
    _object** Put() noexcept;
    _object*  Get() const noexcept;
    [[nodiscard]]
    _object* Detach() noexcept;
    bool     operator==(const _object* const other) const noexcept;
    bool     operator==(PyObjectPtr other) const noexcept;
    explicit operator bool() const noexcept;
};

class PythonRuntime final : public IForeignLanguageRuntime
{
private:
    PyObjectPtr p_plugin_module;

public:
    PythonRuntime();

    int64_t   AddRef() override { return 1; };
    int64_t   Release() override { return 1; };
    AsrResult QueryInterface(const AsrGuid&, void**) override
    {
        return ASR_E_NO_IMPLEMENTATION;
    }
    auto LoadPlugin(const std::filesystem::path& path)
        -> ASR::Utils::Expected<CommonPluginPtr> override;

    static auto ImportPluginModule(
        const std::filesystem::path& py_plugin_initializer)
        -> ASR::Utils::Expected<PyObjectPtr>;
    static auto ResolveClassName(const std::filesystem::path& relative_path)
        -> ASR::Utils::Expected<std::wstring>;
    auto GetPluginInitializer(_object& py_module) -> PyObjectPtr;
};

ASR_NS_PYTHONHOST_END

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_EXPORT_PYTHON

#endif // ASR_CORE_FOREIGNINTERFACEHOST_PYTHONHOST_H
