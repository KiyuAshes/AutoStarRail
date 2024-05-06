#ifndef ASR_LOGGER_H
#define ASR_LOGGER_H

#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/AsrString.hpp>

#ifndef SWIG

typedef struct _asr_SourceLocation
{
    const char* file_name;
    int         line;
    const char* function_name;
} AsrSourceLocation;

#ifdef __cplusplus
#ifdef _MSC_VER
#define ASR_INTERNAL_FUNCTION_NAME __FUNCSIG__
#else
#define ASR_INTERNAL_FUNCTION_NAME __PRETTY_FUNCTION__
#endif // _MSC_VER
#else
#define ASR_INTERNAL_FUNCTION_NAME __func__
#endif // __cplusplus

#define ASR_LOG_WITH_SOURCE_LOCATION(type, ...)                                \
    do                                                                         \
    {                                                                          \
        AsrSourceLocation _asr_internal_source_location = {                    \
            __FILE__,                                                          \
            __LINE__,                                                          \
            ASR_INTERNAL_FUNCTION_NAME};                                       \
        AsrLog##type##U8WithSourceLocation(                                    \
            __VA_ARGS__,                                                       \
            &_asr_internal_source_location);                                   \
    } while (false)

#define ASR_LOG_ERROR(...) ASR_LOG_WITH_SOURCE_LOCATION(Error, __VA_ARGS__)
#define ASR_LOG_WARNING(...) ASR_LOG_WITH_SOURCE_LOCATION(Warning, __VA_ARGS__)
#define ASR_LOG_INFO(...) ASR_LOG_WITH_SOURCE_LOCATION(Info, __VA_ARGS__)

ASR_C_API void AsrLogError(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogErrorU8(const char* p_string);
ASR_C_API void AsrLogErrorU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location);

ASR_C_API void AsrLogWarning(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogWarningU8(const char* p_string);
ASR_C_API void AsrLogWarningU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location);

ASR_C_API void AsrLogInfo(IAsrReadOnlyString* p_readonly_string);
ASR_C_API void AsrLogInfoU8(const char* p_string);
ASR_C_API void AsrLogInfoU8WithSourceLocation(
    const char*              p_string,
    const AsrSourceLocation* p_location);

// {9BC34D72-E442-4944-ACE6-69257D262568}
ASR_DEFINE_GUID(
    ASR_IID_LOG_READER,
    IAsrLogReader,
    0x9bc34d72,
    0xe442,
    0x4944,
    0xac,
    0xe6,
    0x69,
    0x25,
    0x7d,
    0x26,
    0x25,
    0x68);
ASR_INTERFACE IAsrLogReader : public IAsrBase
{
<<<<<<< HEAD
    ASR_METHOD ReadOne(const char* message) = 0;
=======
    ASR_METHOD ReadOne(const char* message, size_t size) = 0;
>>>>>>> origin/HEAD
};

// {806E244C-CCF0-4DC3-AD54-6886FDF9B1F4}
ASR_DEFINE_GUID(
    ASR_IID_LOG_REQUESTER,
    IAsrLogRequester,
    0x806e244c,
    0xccf0,
    0x4dc3,
    0xad,
    0x54,
    0x68,
    0x86,
    0xfd,
    0xf9,
    0xb1,
    0xf4);
ASR_INTERFACE IAsrLogRequester : public IAsrBase
{
    /**
     * @brief 使用用户自定义的方法对数据进行读取
     * @param p_reader 在内部加锁后进行阅读的操作，由用户继承并实现
     * @return 指示操作是否成功
     */
    ASR_METHOD RequestOne(IAsrLogReader * p_reader) = 0;
};

ASR_C_API AsrResult CreateIAsrLogRequester(
    uint32_t           max_line_count,
    IAsrLogRequester** pp_out_requester);

#endif // SWIG

ASR_API void AsrLogError(AsrReadOnlyString asr_string);

ASR_API void AsrLogWarning(AsrReadOnlyString asr_string);

ASR_API void AsrLogInfo(AsrReadOnlyString asr_string);

#endif // ASR_LOGGER_H
