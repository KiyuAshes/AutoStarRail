#include "AutoStarRail/AsrConfig.h"
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrStringImpl.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Utils/QueryInterface.hpp>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <magic_enum_format.hpp>
#include <unicode/unistr.h>
#include <unicode/uversion.h>
#include <unicode/ustring.h>
#include <new>
#include <functional>
#include <cwchar>
#include <algorithm>
#include <cstring>
#include <nlohmann/json.hpp>

bool operator==(AsrReadOnlyString lhs, AsrReadOnlyString rhs)
{
    const char* p_lhs_str = lhs.GetUtf8();
    const char* p_rhs_str = rhs.GetUtf8();

    return std::strcmp(p_lhs_str, p_rhs_str) == 0;
}

auto(ASR_FMT_NS::formatter<ASR::AsrPtr<IAsrReadOnlyString>, char>::format)(
    const ASR::AsrPtr<IAsrReadOnlyString>& p_string,
    format_context&                        ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    const char* p_string_data{nullptr};
    const auto  result = p_string->GetUtf8(&p_string_data);
    if (ASR::IsOk(result))
    {
        return ASR_FMT_NS::format_to(ctx.out(), "{}", p_string_data);
    }
    return ASR_FMT_NS::format_to(
        ctx.out(),
        "(An error occurred when getting string, with error code = {})",
        result);
}

auto(ASR_FMT_NS::formatter<AsrReadOnlyString, char>::format)(
    const AsrReadOnlyString& asr_string,
    format_context&          ctx) const ->
    typename std::remove_reference_t<decltype(ctx)>::iterator
{
    return format_to(ctx.out(), "{}", asr_string.GetUtf8());
}

ASR_NS_BEGIN

[[nodiscard]]
std::size_t BkdrHash(const char16_t* p_begin, const char16_t* p_end)
{
    constexpr std::size_t seed = 31;
    std::size_t           hash = 0;
    while (p_begin != p_end)
    {
        hash = hash * seed + *p_begin;
        std::advance(p_begin, 1);
    }
    return hash;
}

std::size_t AsrStringHash::operator()(
    const AsrPtr<IAsrReadOnlyString>& str) const
{
    const char16_t* p_u16{nullptr};
    size_t          string_size{0};
    const auto      result = str->GetUtf16(&p_u16, &string_size);
    const auto      string_size_int64 = static_cast<int64_t>(string_size);
    if (ASR::IsOk(result)) [[likely]]
    {
        return BkdrHash(p_u16, std::next(p_u16, string_size_int64));
    }
    else [[unlikely]]
    {
        return std::hash<decltype(nullptr)>{}(nullptr);
    }
}

ASR_NS_END

void AsrStringCppImpl::InvalidateCache()
{
    is_cache_expired_ = {true, true, true};
}

void AsrStringCppImpl::UpdateUtf32Cache()
{
    if (IsCacheExpired<Encode::U32>())
    {
        const auto u32_char_count = impl_.countChar32();
        const auto p_cached_utf32_string =
            cached_utf32_string_.DiscardAndGetNullTerminateBufferPointer(
                u32_char_count);
        UErrorCode status = U_ZERO_ERROR;
        impl_.toUTF32(p_cached_utf32_string, u32_char_count, status);
        ValidateCache<Encode::U32>();
    }
}

AsrStringCppImpl::AsrStringCppImpl() = default;

AsrStringCppImpl::AsrStringCppImpl(
    const U_NAMESPACE_QUALIFIER UnicodeString& impl)
    : impl_{impl}
{
}

AsrStringCppImpl::AsrStringCppImpl(
    U_NAMESPACE_QUALIFIER UnicodeString&& impl) noexcept
    : impl_{std::move(impl)}
{
}

AsrStringCppImpl::~AsrStringCppImpl() = default;

int64_t AsrStringCppImpl::AddRef() { return ref_counter_.AddRef(); }

int64_t AsrStringCppImpl::Release() { return ref_counter_.Release(this); }

AsrResult AsrStringCppImpl::QueryInterface(const AsrGuid& iid, void** pp_object)
{
    return ASR::Utils::QueryInterface<IAsrReadOnlyString>(this, iid, pp_object);
}

const UChar32* AsrStringCppImpl::CBegin()
{
    UpdateUtf32Cache();
    return cached_utf32_string_.cbegin();
};

const UChar32* AsrStringCppImpl::CEnd()
{
    UpdateUtf32Cache();
    return cached_utf32_string_.cend();
}

AsrResult AsrStringCppImpl::SetUtf8(const char* p_string)
{
    InvalidateCache();
    impl_ = U_NAMESPACE_QUALIFIER UnicodeString::fromUTF8(p_string);

    cached_utf8_string_ = p_string;
    ValidateCache<Encode::U8>();

    return ASR_S_OK;
};

AsrResult AsrStringCppImpl::GetUtf8(const char** out_string)
{
    if (IsCacheExpired<Encode::U8>())
    {
        impl_.toUTF8String(cached_utf8_string_);
        ValidateCache<Encode::U8>();
    }
    *out_string = cached_utf8_string_.c_str();
    return ASR_S_OK;
};

AsrResult AsrStringCppImpl::SetUtf16(const char16_t* p_string, size_t length)
{
    InvalidateCache();
    const auto int_length = static_cast<int>(length);
    impl_ = {p_string, int_length};
    return ASR_S_OK;
}

AsrResult AsrStringCppImpl::GetUtf16(
    const char16_t** out_string,
    size_t*          out_string_size) noexcept
{
    *out_string = impl_.getBuffer();
    *out_string_size = impl_.length();
    return ASR_S_OK;
}

ASR_NS_ANONYMOUS_DETAILS_BEGIN

#define ANONYMOUS_DETAILS_MAX_SIZE 4096

/**
 * @brief 返回带L'\0'字符的空终止字符串长度
 * @param p_wstring
 * @return
 */
template <class = std::enable_if<sizeof(wchar_t) == 4, size_t>>
auto GetStringSize(const wchar_t* p_wstring) -> size_t
{
    for (size_t i = 0; i < ANONYMOUS_DETAILS_MAX_SIZE; ++i)
    {
        if (p_wstring[i] == L'\0')
        {
            return i + 1;
        }
    }

    ASR_CORE_LOG_ERROR(
        "Input string size is larger than expected. Expected max size is " ASR_STR(
            ANONYMOUS_DETAILS_MAX_SIZE) ".");

    const wchar_t char_at_i = p_wstring[ANONYMOUS_DETAILS_MAX_SIZE];
    const auto    char_at_i_value =
        static_cast<uint16_t>(static_cast<uint32_t>(char_at_i));

    // 前导代理
    if (0xD800 <= char_at_i_value && char_at_i_value <= 0xDBFF)
    {
        return ANONYMOUS_DETAILS_MAX_SIZE;
    }
    // 后尾代理
    if (0xDC00 <= char_at_i_value && char_at_i_value <= 0xDFFF)
    {
        return ANONYMOUS_DETAILS_MAX_SIZE - 1;
    }
    return ANONYMOUS_DETAILS_MAX_SIZE;
}

template <class C, class T>
auto SetSwigW(const C* p_wstring, T& u16_buffer)
    -> U_NAMESPACE_QUALIFIER UnicodeString
{
    if constexpr (sizeof(C) == sizeof(char16_t))
    {
        return {p_wstring};
    }
    else if constexpr (sizeof(C) == sizeof(char32_t))
    {
        const auto string_size = GetStringSize(p_wstring);
        const auto p_shadow_string =
            u16_buffer.DiscardAndGetNullTerminateBufferPointer(string_size);
        std::transform(
            p_wstring,
            p_wstring + string_size,
            p_shadow_string,
            [](const wchar_t c)
            {
                char16_t u16_char{};
                // Can be replaced to std::bit_cast
                std::memcpy(&u16_char, &c, sizeof(u16_char));
                return u16_char;
            });
        const auto size = u16_buffer.GetSize();
        const auto int_size = static_cast<int32_t>(size);
        const auto int_length = u_strlen(p_shadow_string);
        return {p_shadow_string, int_length, int_size};
    }
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult AsrStringCppImpl::SetSwigW(const wchar_t* p_string)
{
    InvalidateCache();

    impl_ = Details::SetSwigW(p_string, u16_buffer_);

    return ASR_S_OK;
}

AsrResult AsrStringCppImpl::SetW(const wchar_t* p_string, size_t length)
{
    InvalidateCache();

    const auto i32_length = static_cast<int32_t>(length);
    UErrorCode str_from_wcs_result{};
    int32_t    expected_capacity{0};

    str_from_wcs_result = U_ZERO_ERROR;
    ::u_strFromWCS(
        nullptr,
        0,
        &expected_capacity,
        p_string,
        i32_length,
        &str_from_wcs_result);
    const auto p_buffer =
        u16_buffer_.DiscardAndGetNullTerminateBufferPointer(expected_capacity);
    const auto i32_buffer_size = static_cast<int32_t>(u16_buffer_.GetSize());
    str_from_wcs_result = U_ZERO_ERROR;
    ::u_strFromWCS(
        p_buffer,
        i32_buffer_size,
        &expected_capacity,
        p_string,
        i32_length,
        &str_from_wcs_result);
    if (str_from_wcs_result != U_ZERO_ERROR)
    {
        ASR_CORE_LOG_ERROR(
            "Error happened when calling u_strFromWCS. Error code = {}",
            str_from_wcs_result);
        return ASR_E_INVALID_STRING;
    }

    impl_ = {p_buffer, i32_buffer_size, i32_buffer_size};

    return ASR_S_OK;
}

AsrResult AsrStringCppImpl::GetW(const wchar_t** out_wstring)
{
    if constexpr (sizeof(wchar_t) == sizeof(char16_t))
    {
        *out_wstring =
            reinterpret_cast<const wchar_t*>(impl_.getTerminatedBuffer());
        return ASR_S_OK;
    }
    else if constexpr (sizeof(wchar_t) == sizeof(char32_t))
    {
        UpdateUtf32Cache();
        *out_wstring =
            reinterpret_cast<const wchar_t*>(cached_utf32_string_.begin());
        return ASR_S_OK;
    }
}

/**
 * @brief C++侧使用此接口读取字符串，其它接口均供外部语言调用
 *
 * @return AsrResult
 */
AsrResult AsrStringCppImpl::GetImpl(ICUString** out_icu_string) noexcept
{
    InvalidateCache();
    *out_icu_string = &impl_;
    return ASR_S_OK;
};

AsrResult AsrStringCppImpl::GetImpl(
    const ICUString** out_icu_string) const noexcept
{
    *out_icu_string = &impl_;
    return ASR_S_OK;
};

void from_json(const nlohmann::json& from, AsrReadOnlyString& to)
{
    to = {from.get_ref<const std::string&>().data()};
}

ASR_NS_BEGIN

namespace Details
{
    template <class T>
    using NullString = std::array<T, 2>;

    class NullStringImpl final : public IAsrReadOnlyString
    {
        static std::string          null_u8string_;
        static NullString<wchar_t>  null_wstring_;
        static NullString<char16_t> null_u16string_;
        static NullString<UChar32>  null_u32string_;

    public:
        int64_t   AddRef() override { return 1; }
        int64_t   Release() override { return 1; }
        AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override
        {
            return ASR::Utils::QueryInterface<IAsrReadOnlyString>(
                this,
                iid,
                pp_object);
        }

        AsrResult GetUtf8(const char** out_string) override
        {
            *out_string = null_u8string_.data();
            return ASR_S_OK;
        }

        AsrResult GetUtf16(
            const char16_t** out_string,
            size_t*          out_string_size) noexcept override
        {
            *out_string = null_u16string_.data();
            *out_string_size = 0;
            return ASR_S_OK;
        };

        AsrResult GetW(const wchar_t** out_wstring) override
        {
            *out_wstring = null_wstring_.data();
            return ASR_S_OK;
        }

        const UChar32* CBegin() override { return null_u32string_.data(); }
        const UChar32* CEnd() override { return null_u32string_.data(); }
    };

    ASR_DEFINE_VARIABLE(NullStringImpl::null_u8string_){};
    ASR_DEFINE_VARIABLE(NullStringImpl::null_wstring_){};
    ASR_DEFINE_VARIABLE(NullStringImpl::null_u16string_){};
    ASR_DEFINE_VARIABLE(NullStringImpl::null_u32string_){};

    NullStringImpl null_asr_string_impl_{};

    AsrPtr<IAsrReadOnlyString> CreateNullAsrString()
    {
        return {&null_asr_string_impl_, take_ownership};
    }

    AsrPtr<IAsrString> CreateAsrString()
    {
        return {new AsrStringCppImpl(), take_ownership};
    }
}

ASR_NS_END

AsrResult CreateIAsrReadOnlyStringFromChar(
    const char*          p_char_literal,
    IAsrReadOnlyString** pp_out_readonly_string)
{
    try
    {
        auto                      icu_string =
            U_NAMESPACE_QUALIFIER UnicodeString(p_char_literal, "");
        auto                      p_string =
            std::make_unique<AsrStringCppImpl>(std::move(icu_string));
        p_string->AddRef();
        *pp_out_readonly_string = p_string.release();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrResult CreateIAsrStringFromUtf8(
    const char*  p_utf8_string,
    IAsrString** pp_out_string)
{
    try
    {
        auto p_string = std::make_unique<AsrStringCppImpl>();
        p_string->SetUtf8(p_utf8_string);
        p_string->AddRef();
        *pp_out_string = p_string.release();
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrResult CreateIAsrReadOnlyStringFromUtf8(
    const char*          p_utf8_string,
    IAsrReadOnlyString** pp_out_readonly_string)
{
    IAsrString* p_string = nullptr;
    auto        result = CreateIAsrStringFromUtf8(p_utf8_string, &p_string);
    *pp_out_readonly_string = p_string;
    return result;
}

AsrResult CreateIAsrStringFromWChar(
    const wchar_t* p_wstring,
    size_t         length,
    IAsrString**   pp_out_string)
{
    try
    {
        auto p_string = std::make_unique<AsrStringCppImpl>();
        p_string->SetW(p_wstring, length);
        p_string->AddRef();
        *pp_out_string = p_string.release();
        return ASR_S_OK;
    }
    catch (std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrResult CreateIAsrReadOnlyStringFromWChar(
    const wchar_t*       p_wstring,
    size_t               length,
    IAsrReadOnlyString** pp_out_readonly_string)
{
    IAsrString* p_string = nullptr;
    auto result = CreateIAsrStringFromWChar(p_wstring, length, &p_string);
    *pp_out_readonly_string = p_string;
    return result;
}