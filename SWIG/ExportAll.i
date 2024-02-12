%module(directors="1") AutoStarRail

%include <stdint.i>
%include <typemaps.i>
%include <cpointer.i>
%include <std_string.i>
%include <std_vector.i>
%include <wchar.i>
%include <std_wstring.i>
%include <std_shared_ptr.i>

%{
#include <AutoStarRail/AsrExport.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/IAsrTypeInfo.h>
#include <AutoStarRail/AsrString.hpp>

#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
#include <AutoStarRail/PluginInterface/IAsrInput.h>
#include <AutoStarRail/PluginInterface/IAsrPlugin.h>
#include <AutoStarRail/PluginInterface/IAsrTask.h>

#include <AutoStarRail/ExportInterface/AsrLogger.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/ExportInterface/IAsrImage.h>
#include <AutoStarRail/ExportInterface/IAsrCaptureManager.h>
#include <AutoStarRail/ExportInterface/IAsrOcr.h>
#include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/ExportInterface/IAsrTaskManager.h>
%}

#ifdef SWIGJAVA
%typemap(jni) char16_t* "jstring"
%typemap(jtype) char16_t* "String"
%typemap(jstype) char16_t* "String"
%typemap(javadirectorin) char16_t* "$jniinput"
%typemap(javadirectorout) char16_t* "$javacall"

%typemap(jni) void AsrReadOnlyString::GetUtf16 "jstring"
%typemap(jtype) void AsrReadOnlyString::GetUtf16 "String"
%typemap(jstype) void AsrReadOnlyString::GetUtf16 "String"
%typemap(javaout) void AsrReadOnlyString::GetUtf16 {
    return $jnicall;
}

%typemap(in, numinputs=0) (const char16_t** out_string, size_t* out_string_size) %{
    char16_t* p_u16string;
    $1 = &p_u16string;

    size_t u16string_size;
    $2 = &u16string_size;
%}

%typemap(argout) (const char16_t** out_string, size_t* out_string_size) {
    if($1 && $2)
    {
        jsize j_length = (jsize)u16string_size;
        $result = jenv->NewString((jchar*)p_u16string, j_length);
    }
    else
    {
        jclass null_pointer_exception = jenv->FindClass("java/lang/NullPointerException");
        jenv->ThrowNew(null_pointer_exception, "Input pointer is null");
    }
}

%typemap(javain) (const char16_t* p_u16string, size_t length) "p_u16string"

%typemap(in, numinputs=1) (const char16_t* p_u16string, size_t length) %{
    const jchar* p_jstring = jenv->GetStringChars(jarg1, nullptr);
    const jsize string_length = jenv->GetStringLength(jarg1);
    static_assert(sizeof(jchar) == sizeof(char16_t), "Size of jchar is NOT equal to size of char16_t.");
    jchar* p_non_const_jstring = const_cast<jchar*>(p_jstring);
    $1 = reinterpret_cast<decltype($1)>(p_non_const_jstring);
    $2 = string_length;
%}

%typemap(out) (const char16_t* p_u16string, size_t length) {
    jenv->ReleaseStringChars(jarg1, p_jstring);
}

#endif

%include <AutoStarRail/AsrExport.h>
%include <AutoStarRail/IAsrBase.h>
%include <AutoStarRail/IAsrTypeInfo.h>
%include <AutoStarRail/AsrString.hpp>

// 以下文件按照字母顺序排列！ The following files are in alphabetical order!
// 例外：由于依赖关系的原因，IAsrImage.h必须在IAsrCaptureManager.h前

%include <AutoStarRail/PluginInterface/IAsrCapture.h>
%include <AutoStarRail/PluginInterface/IAsrErrorLens.h>
%include <AutoStarRail/PluginInterface/IAsrInput.h>
%include <AutoStarRail/PluginInterface/IAsrPlugin.h>
%include <AutoStarRail/PluginInterface/IAsrTask.h>

%include <AutoStarRail/ExportInterface/AsrLogger.h>
%include <AutoStarRail/ExportInterface/IAsrSettings.h>
%include <AutoStarRail/ExportInterface/IAsrImage.h>
%include <AutoStarRail/ExportInterface/IAsrCaptureManager.h>
%include <AutoStarRail/ExportInterface/IAsrOcr.h>
%include <AutoStarRail/ExportInterface/IAsrPluginManager.h>
%include <AutoStarRail/ExportInterface/IAsrSettings.h>
%include <AutoStarRail/ExportInterface/IAsrTaskManager.h>