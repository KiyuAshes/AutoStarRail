#ifndef ASR_CORE_FOREIGNINTERFACEHOST_IASRCONTEXTIMPL_H
#define ASR_CORE_FOREIGNINTERFACEHOST_IASRCONTEXTIMPL_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/ExportInterface/IAsrContext.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <nlohmann/json.hpp>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class AsrContextImpl;

class IAsrContextImpl final : public IAsrContext
{
    AsrContextImpl& impl_;

public:
    IAsrContextImpl(AsrContextImpl& impl);
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_object) override;
    // IAsrContext
    ASR_IMPL GetString(
        IAsrReadOnlyString*  key,
        IAsrReadOnlyString** pp_out_string) override;
    ASR_IMPL GetBool(IAsrReadOnlyString* key, bool* p_out_bool) override;
    ASR_IMPL GetInt(IAsrReadOnlyString* key, int64_t* p_out_int) override;
    ASR_IMPL GetFloat(IAsrReadOnlyString* key, float* p_out_float) override;

    ASR_IMPL SetString(IAsrReadOnlyString* key, IAsrReadOnlyString* value)
        override;
    ASR_IMPL SetBool(IAsrReadOnlyString* key, bool value) override;
    ASR_IMPL SetInt(IAsrReadOnlyString* key, int64_t value) override;
    ASR_IMPL SetFloat(IAsrReadOnlyString* key, float value) override;
};

class IAsrSwigContextImpl final : public IAsrSwigContext
{
    AsrContextImpl& impl_;

public:
    IAsrSwigContextImpl(AsrContextImpl& impl);
    // IAsrSwigBase
    int64_t        AddRef() override;
    int64_t        Release() override;
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    // IAsrSwigContext
    AsrRetReadOnlyString GetString(AsrReadOnlyString key) override;
    AsrRetBool           GetBool(AsrReadOnlyString key) override;
    AsrRetInt            GetInt(AsrReadOnlyString key) override;
    AsrRetFloat          GetFloat(AsrReadOnlyString key) override;

    AsrResult SetString(AsrReadOnlyString key, AsrReadOnlyString value)
        override;
    AsrResult SetBool(AsrReadOnlyString key, bool value) override;
    AsrResult SetInt(AsrReadOnlyString key, int64_t value) override;
    AsrResult SetFloat(AsrReadOnlyString key, float value) override;
};

class AsrContextImpl : ASR_UTILS_MULTIPLE_PROJECTION_GENERATORS(
                           AsrContextImpl,
                           IAsrContextImpl,
                           IAsrSwigContextImpl)
{
    Utils::RefCounter<AsrContextImpl> ref_counter_{};
    nlohmann::json                    context_{};

public:
    int64_t        AddRef();
    int64_t        Release();
    AsrResult      QueryInterface(const AsrGuid& iid, void** pp_object);
    AsrRetSwigBase QueryInterface(const AsrGuid& iid);

    AsrRetReadOnlyString GetString(AsrReadOnlyString key);
    AsrRetBool           GetBool(AsrReadOnlyString key);
    AsrRetInt            GetInt(AsrReadOnlyString key);
    AsrRetFloat          GetFloat(AsrReadOnlyString key);

    AsrResult SetString(AsrReadOnlyString key, AsrReadOnlyString value);
    AsrResult SetBool(AsrReadOnlyString key, bool value);
    AsrResult SetInt(AsrReadOnlyString key, int64_t value);
    AsrResult SetFloat(AsrReadOnlyString key, float value);
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_IASRCONTEXTIMPL_H
