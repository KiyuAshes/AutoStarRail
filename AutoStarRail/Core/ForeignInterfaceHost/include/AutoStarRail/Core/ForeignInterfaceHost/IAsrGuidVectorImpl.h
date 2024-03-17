#ifndef ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
#define ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/ExportInterface/IAsrGuidVector.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <vector>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class AsrGuidVectorImpl;

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

class IAsrReadOnlyGuidVectorImpl final : public IAsrReadOnlyGuidVector
{
    ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl_;

public:
    IAsrReadOnlyGuidVectorImpl(
        ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl);
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrReadOnlyGuidVector
    AsrResult Size(size_t* p_out_size) override;
    AsrResult At(size_t index, AsrGuid* p_out_iid) override;
    AsrResult Find(const AsrGuid& iid) override;
};

class IAsrGuidVectorImpl final : public IAsrGuidVector
{
    ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl_;

public:
    IAsrGuidVectorImpl(
        ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl);
    // IAsrBase
    int64_t   AddRef() override;
    int64_t   Release() override;
    AsrResult QueryInterface(const AsrGuid& iid, void** pp_out_object) override;
    // IAsrGuidVector
    AsrResult Size(size_t* p_out_size) override;
    AsrResult At(size_t index, AsrGuid* p_out_iid) override;
    AsrResult Find(const AsrGuid& iid) override;
    AsrResult PushBack(const AsrGuid& iid) override;
    AsrResult ToConst(IAsrReadOnlyGuidVector** pp_out_object) override;
    // IAsrGuidVectorImpl
    auto GetImpl() noexcept -> std::vector<AsrGuid>&;
    auto Get() -> ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl&;
};

class IAsrSwigReadOnlyGuidVectorImpl final : public IAsrSwigReadOnlyGuidVector
{
    ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl_;

public:
    IAsrSwigReadOnlyGuidVectorImpl(
        ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl);
    int64_t        AddRef() override;
    int64_t        Release() override;
    AsrRetSwigBase QueryInterface(const AsrGuid& iid) override;
    AsrRetUInt     Size() override;
    AsrRetGuid     At(size_t index) override;
    AsrResult      Find(const AsrGuid& iid) override;
};

class IAsrSwigGuidVectorImpl final : public IAsrSwigGuidVector
{
    ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl_;

public:
    IAsrSwigGuidVectorImpl(
        ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl& impl);
    int64_t                  AddRef() override;
    int64_t                  Release() override;
    AsrRetSwigBase           QueryInterface(const AsrGuid& iid) override;
    AsrRetUInt               Size() override;
    AsrRetGuid               At(size_t index) override;
    AsrResult                Find(const AsrGuid& iid) override;
    AsrResult                PushBack(const AsrGuid& iid) override;
    AsrRetReadOnlyGuidVector ToConst() override;

    auto Get() -> ASR::Core::ForeignInterfaceHost::AsrGuidVectorImpl&;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

class AsrGuidVectorImpl : ASR_UTILS_MULTIPLE_PROJECTION_GENERATORS(
                              AsrGuidVectorImpl,
                              IAsrGuidVectorImpl,
                              IAsrSwigGuidVectorImpl),
                          ASR_UTILS_MULTIPLE_PROJECTION_GENERATORS(
                              AsrGuidVectorImpl,
                              IAsrReadOnlyGuidVectorImpl,
                              IAsrSwigReadOnlyGuidVectorImpl)
{
    Utils::RefCounter<AsrGuidVectorImpl> ref_counter_{};
    std::vector<AsrGuid>                 iids_{};

public:
    int64_t AddRef();
    int64_t Release();
    auto    Size() const noexcept -> size_t;
    auto    At(size_t index, AsrGuid& out_guid) const noexcept -> AsrResult;
    auto    Find(const AsrGuid guid) noexcept -> AsrResult;
    auto    PushBack(const AsrGuid guid) noexcept -> AsrResult;

    auto GetImpl() noexcept -> std::vector<AsrGuid>&;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_ASRIIDVECTOR_H
