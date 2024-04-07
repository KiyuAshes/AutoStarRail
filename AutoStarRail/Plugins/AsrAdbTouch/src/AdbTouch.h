#ifndef ASR_PLUGINS_ASRADBTOUCH_ADBTOUCH_H
#define ASR_PLUGINS_ASRADBTOUCH_ADBTOUCH_H

#include <AutoStarRail/PluginInterface/IAsrInput.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <string_view>

ASR_NS_BEGIN

class AdbTouch final : public IAsrTouch
{
    // should be f"{adb_path} -s {adb_serial} "
    std::string adb_cmd_;

public:
    AdbTouch(std::string_view adb_path, std::string_view adb_serial);
    // IAsrBase
    ASR_UTILS_IASRBASE_AUTO_IMPL(AdbTouch);
    ASR_IMPL QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrTypeInfo
    ASR_IMPL GetGuid(AsrGuid* p_out_guid) override;
    ASR_IMPL GetRuntimeClassName(IAsrReadOnlyString** pp_out_name) override;
    // IAsrInput
    ASR_IMPL Click(int32_t x, int32_t y) override;
    // IAsrTouch
    ASR_IMPL Swipe(AsrPoint from, AsrPoint to, int32_t duration_ms) override;
};

ASR_NS_END

#endif // ASR_PLUGINS_ASRADBTOUCH_ADBTOUCH_H
