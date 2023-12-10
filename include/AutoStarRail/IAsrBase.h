#ifndef ASR_BASE_H
#define ASR_BASE_H

#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/AsrGuidHolder.h>
#include <string.h>
#include <stdint.h>
#include <new>

#define ASR_RET_TYPE_DECLARE_BEGIN(type_name)                                  \
    struct type_name                                                           \
    {                                                                          \
        AsrResult error_code{ASR_E_UNDEFINED_RETURN_VALUE};

#define ASR_RET_TYPE_DECLARE_END                                               \
    }                                                                          \
    ;

// clang-format off
#ifdef SWIG
#define SWIG_IGNORE(x) %ignore x;
#define SWIG_ENABLE_DIRECTOR(x) %feature("director") x;
#define SWIG_POINTER_CLASS(x, y) %pointer_class(x, y);
#define SWIG_NEW_OBJECT(x) %newobject x;
#define SWIG_DEL_OBJECT(x) %delobject x::Release;
#define SWIG_UNREF_OBJECT(x) %feature("unref") x "$this->Release();"
#define SWIG_ENABLE_SHARED_PTR(x) %shared_ptr(x);
#define SWIG_NO_DEFAULT_CTOR(x) %nodefaultctor x;
#define ASR_DEFINE_GUID(name, type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const AsrGuid name =                                                \
        {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}};
#define ASR_DEFINE_CLASS_GUID(name, type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const AsrGuid name =                                                      \
        {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}};
#else
#define SWIG_IGNORE(x)
#define SWIG_ENABLE_DIRECTOR(x)
#define SWIG_POINTER_CLASS(x, y)
#define SWIG_NEW_OBJECT(x)
#define SWIG_DEL_OBJECT(x)
#define SWIG_UNREF_OBJECT(x)
#define SWIG_ENABLE_SHARED_PTR(x)
#define SWIG_NO_DEFAULT_CTOR(x)
#define ASR_DEFINE_GUID(name, type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    static const AsrGuid name =                                                \
        {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}};                         \
    ASR_DEFINE_GUID_HOLDER(type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
#define ASR_DEFINE_CLASS_GUID(type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    ASR_DEFINE_CLASS_GUID_HOLDER(type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
#define ASR_DEFINE_CLASS_IN_NAMESPACE(ns, type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    namespace ns { class type; }\
    ASR_DEFINE_CLASS_GUID_HOLDER_IN_NAMESPACE(ns ,type, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);
#endif
// clang-format on

#define ASR_SWIG_EXPORT_ATTRIBUTE(x)                                        \
    SWIG_ENABLE_DIRECTOR(x)                                                    \
    SWIG_UNREF_OBJECT(x)

#define ASR_S_OK 1
#define ASR_S_FALSE 0
#define ASR_E_RESERVED -1073741824
#define ASR_E_NO_INTERFACE ASR_E_RESERVED - 1
#define ASR_E_UNDEFINED_RETURN_VALUE ASR_E_RESERVED - 2
#define ASR_E_INVALID_STRING ASR_E_RESERVED - 3
#define ASR_E_INVALID_STRING_SIZE ASR_E_RESERVED - 4
#define ASR_E_NO_IMPLEMENTATION ASR_E_RESERVED - 5
#define ASR_E_UNSUPPORTED_SYSTEM ASR_E_RESERVED - 6
#define ASR_E_INVALID_JSON ASR_E_RESERVED - 7
#define ASR_E_TYPE_ERROR ASR_E_RESERVED - 8
#define ASR_E_INVALID_FILE ASR_E_RESERVED - 9
#define ASR_E_INVALID_URL ASR_E_RESERVED - 10
/**
 * @brief 返回此值可以表示枚举结束
 *
 */
#define ASR_E_OUT_OF_RANGE ASR_E_RESERVED - 11
#define ASR_E_DUPLICATE_ELEMENT ASR_E_RESERVED - 12
#define ASR_E_FILE_NOT_FOUND ASR_E_RESERVED - 13
#define ASR_E_MAYBE_OVERFLOW ASR_E_RESERVED - 14
#define ASR_E_OUT_OF_MEMORY ASR_E_RESERVED - 15
#define ASR_E_INVALID_PATH ASR_E_RESERVED - 16
#define ASR_E_INVALID_POINTER ASR_E_RESERVED - 17
#define ASR_E_SWIG_INTERNAL_ERROR ASR_E_RESERVED - 18
#define ASR_E_PYTHON_ERROR ASR_E_RESERVED - 19
#define ASR_E_JAVA_ERROR ASR_E_RESERVED - 20
#define ASR_E_CSHARP_ERROR ASR_E_RESERVED - 21

#ifdef ASR_WINDOWS
// MSVC
#ifdef _MSC_VER
#define ASR_STD_CALL __stdcall
#else
// GCC AND CLANG
#define ASR_STD_CALL __attribute__((__stdcall__))
#endif // _MSC_VER
#endif // ASR_WINDOWS

#ifndef ASR_STD_CALL
#define ASR_STD_CALL
#endif // ASR_STD_CALL

#define ASR_INTERFACE struct

#define ASR_METHOD virtual AsrResult ASR_STD_CALL
#define ASR_IMPL AsrResult

using AsrResult = int32_t;

/**
 * @brief NOTE: Be careful about the lifetime of this structure.\n
 *       If you want to keep it, you MUST make a copy of it.
 */
typedef struct _asr_GUID
{
#ifdef SWIG
private:
#endif // SWIG
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint8_t  data4[8];
} AsrGuid;

typedef struct _asr_RetGuid
{
    AsrResult error_code;
    AsrGuid   value;
} AsrRetGuid;

typedef char AsrBool;

#define ASR_TRUE ASR_S_OK
#define ASR_FALSE ASR_S_FALSE

/**
 * @brief input format should be "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
 *
 * @param p_guid_string
 * @return ASR_C_API
 */
ASR_C_API AsrRetGuid AsrMakeAsrGuid(const char* p_guid_string);

#ifndef SWIG

#ifdef __cplusplus
inline bool operator==(const AsrGuid& lhs, const AsrGuid& rhs)
{
    const auto result = ::memcmp(&lhs, &rhs, sizeof(lhs));
    return result == 0;
}

ASR_NS_BEGIN

inline bool IsOk(const AsrResult result) { return result >= 0; }

ASR_NS_END

#endif // __cplusplus

ASR_DEFINE_GUID(
    ASR_IID_BASE,
    IAsrBase,
    0x00000000,
    0x0000,
    0x0000,
    0xc0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x46)
ASR_INTERFACE IAsrBase
{
    virtual int64_t AddRef() = 0;
    virtual int64_t Release() = 0;
    ASR_METHOD      QueryInterface(const AsrGuid& iid, void** pp_object) = 0;
};

ASR_INTERFACE IAsrGuidVector : public IAsrBase
{
    ASR_METHOD At(size_t index, AsrGuid * p_out_guid) = 0;
    ASR_METHOD Add(AsrGuid * p_guid) = 0;
    ASR_METHOD DeleteAt(size_t index) = 0;
};

ASR_C_API AsrResult
CreateIAsrReadOnlyStringVector(AsrGuid** p_in_guid_array, const size_t size);

#endif // SWIG

class AsrSwigBaseWrapper
{
    void* p_object_{nullptr};

public:
    AsrSwigBaseWrapper() = default;
#ifndef SWIG
    explicit AsrSwigBaseWrapper(void* p_object) noexcept : p_object_{p_object}
    {
    }
#endif // SWIG
    explicit AsrSwigBaseWrapper(ASR_INTERFACE IAsrSwigBase* p_base) noexcept
        : p_object_{p_base}
    {
    }
    ASR_INTERFACE IAsrSwigBase* Get() const noexcept
    {
        return static_cast<IAsrSwigBase*>(p_object_);
    }
#ifndef SWIG
    void* GetVoid() const noexcept { return p_object_; }
    operator void*() const noexcept { return p_object_; }
#endif // SWIG
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetSwigBase)
    AsrSwigBaseWrapper value;
ASR_RET_TYPE_DECLARE_END

// {FAF64DEB-0C0A-48CC-BA10-FCDE420350A2}
ASR_DEFINE_GUID(
    ASR_IID_SWIG_BASE,
    IAsrSwigBase,
    0xfaf64deb,
    0xc0a,
    0x48cc,
    0xba,
    0x10,
    0xfc,
    0xde,
    0x42,
    0x3,
    0x50,
    0xa2)
ASR_SWIG_EXPORT_ATTRIBUTE(IAsrSwigBase)
ASR_INTERFACE IAsrSwigBase
{
    virtual int64_t AddRef() = 0;
    virtual int64_t Release() = 0;
    // TODO: Remove this code until all code can be compiled.
    ASR_METHOD IsCastAvailable(const AsrGuid& iid) = 0;
    /**
     * @brief Implementation should only return ASR_S_OK or ASR_E_NO_INTERFACE.
     * NOTICE: If returned value is not equal to ASR_S_OK, then the interface is
     * considered not supported.
     *
     * @param iid
     * @return AsrResult
     */
    virtual AsrRetSwigBase QueryInterface(const AsrGuid& iid) = 0;
#ifdef SWIG
    /*
     * @brief Avoid SWIG warning.
     */
    virtual ~IAsrSwigBase() {}
#endif // SWIG
};

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetBool)
    bool value;
ASR_RET_TYPE_DECLARE_END

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetInt)
    int64_t value;
ASR_RET_TYPE_DECLARE_END

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetUInt)
    uint64_t value;
ASR_RET_TYPE_DECLARE_END

ASR_RET_TYPE_DECLARE_BEGIN(AsrRetFloat)
    float value;
ASR_RET_TYPE_DECLARE_END

#endif // ASR_BASE_H
