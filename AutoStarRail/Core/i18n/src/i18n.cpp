#include <AutoStarRail/ExportInterface/IAsrSettings.h>
#include <AutoStarRail/Core/Exceptions/TypeError.h>
#include <AutoStarRail/Core/i18n/i18n.hpp>

ASR_CORE_I18N_NS_BEGIN

AsrReadOnlyStringWrapper::AsrReadOnlyStringWrapper(const char* p_u8_string)
{
    ::CreateIAsrReadOnlyStringFromUtf8(p_u8_string, p_impl_.Put());
}

AsrReadOnlyStringWrapper::AsrReadOnlyStringWrapper(const char8_t* u8_string)
{
    ::CreateIAsrReadOnlyStringFromUtf8(
        reinterpret_cast<const char*>(u8_string),
        p_impl_.Put());
}

void AsrReadOnlyStringWrapper::GetImpl(IAsrReadOnlyString** pp_impl) const
{
    p_impl_->AddRef();
    *pp_impl = p_impl_.Get();
}

AsrReadOnlyStringWrapper::AsrReadOnlyStringWrapper() = default;

AsrReadOnlyStringWrapper::~AsrReadOnlyStringWrapper() = default;

void from_json(const ::nlohmann::json& input, AsrReadOnlyStringWrapper& output)
{
    std::string value{};
    input.get_to(value);
    output = {value.c_str()};
}

ASR_CORE_I18N_NS_END
