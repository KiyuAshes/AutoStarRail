#ifndef ASR_PLUGINS_ADBCAPTUREIMPL_H
#define ASR_PLUGINS_ADBCAPTUREIMPL_H

#include <AutoStarRail/AsrConfig.h>
#include <AutoStarRail/IAsrBase.h>
#include <AutoStarRail/PluginInterface/IAsrCapture.h>
#include "AutoStarRail/Utils/CommonUtils.hpp"
#include <AutoStarRail/Utils/Expected.h>
#include <filesystem>
#include <cstdint>

// {C2300184-A311-4880-8966-53F57519F32A}
ASR_DEFINE_CLASS_IN_NAMESPACE(
    Asr,
    AdbCapture,
    0xc2300184,
    0xa311,
    0x4880,
    0x89,
    0x66,
    0x53,
    0xf5,
    0x75,
    0x19,
    0xf3,
    0x2a)

ASR_NS_BEGIN

class AdbCapture final : public IAsrCapture
{
private:
    ASR::Utils::RefCounter<AdbCapture> ref_counter_{};
    std::string                        capture_png_command_;
    std::string                        capture_raw_by_nc_command_;
    std::string                        capture_gzip_raw_command_;
    std::string                        get_screen_size_command_;

    enum class Type
    {
        Png,
        RawByNc,
        RawWithGZip,
        Raw
    };

    AsrResult (AdbCapture::*current_capture_method)() = {nullptr};
    Type type_{Type::RawWithGZip};

public:
    struct Size
    {
        int32_t width;
        int32_t height;
    };

private:
    Size adb_device_screen_size_{0, 0};

    ASR::Utils::Expected<Size> GetDeviceSize() const;

    AsrResult CaptureRawWithGZip();
    AsrResult CaptureRaw();
    AsrResult CapturePng();
    AsrResult CaptureRawByNc();
    auto AutoDetectType() -> ASR::Utils::Expected<AsrResult (AdbCapture::*)()>;

public:
    AdbCapture(
        const std::filesystem::path& adb_path,
        std::string_view             adb_device_serial);
    ~AdbCapture() = default;
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrInspectable
    ASR_IMPL GetIids(IAsrIidVector** pp_out_iid_vector) override;
    ASR_IMPL GetRuntimeClassName(
        IAsrReadOnlyString** pp_out_class_name) override;
    // IAsrCapture
    ASR_IMPL Capture(IAsrImage** pp_out_image) override;
};

ASR_NS_END

#endif
