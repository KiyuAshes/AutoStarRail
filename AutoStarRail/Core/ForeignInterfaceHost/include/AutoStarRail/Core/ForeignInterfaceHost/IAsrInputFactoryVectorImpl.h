#ifndef ASR_CORE_FOREIGNINTERFACEHOST_IASRINPUTFACTORYVECTORIMPL_H
#define ASR_CORE_FOREIGNINTERFACEHOST_IASRINPUTFACTORYVECTORIMPL_H

#include <AutoStarRail/Core/ForeignInterfaceHost/Config.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/InputFactoryManager.h>
#include <AutoStarRail/ExportInterface/IAsrInputFactoryVector.h>
#include <AutoStarRail/Utils/CommonUtils.hpp>
#include <variant>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

using CommonInputPtr = std::variant<AsrPtr<IAsrInput>, AsrPtr<IAsrSwigInput>>;

class AsrInputFactoryVectorImpl;

class IAsrInputFactoryVectorImpl final : public IAsrInputFactoryVector
{
public:
    explicit IAsrInputFactoryVectorImpl(AsrInputFactoryVectorImpl& impl);
    // IAsrBase
    auto AddRef() -> int64_t override;
    auto Release() -> int64_t override;
    auto QueryInterface(const AsrGuid& iid, void** pp_object)
        -> AsrResult override;
    // IAsrInputFactoryVector
    ASR_IMPL Size(size_t* p_out_size) override;
    ASR_IMPL At(size_t index, IAsrInputFactory** pp_out_factory) override;
    ASR_IMPL Find(const AsrGuid& iid, IAsrInputFactory** pp_out_factory)
        override;

private:
    AsrInputFactoryVectorImpl& impl_;
};

class IAsrSwigInputFactoryVectorImpl final : public IAsrSwigInputFactoryVector
{
public:
    explicit IAsrSwigInputFactoryVectorImpl(AsrInputFactoryVectorImpl& impl);
    // IAsrBase
    auto AddRef() -> int64_t override;
    auto Release() -> int64_t override;
    auto QueryInterface(const AsrGuid& iid) -> AsrRetSwigBase override;
    // IAsrSwigInputFactoryVector
    auto Size() -> AsrRetUInt override;
    auto At(size_t index) -> AsrRetInputFactory override;
    auto Find(const AsrGuid& iid) -> AsrRetInputFactory override;

private:
    AsrInputFactoryVectorImpl& impl_;
};

class AsrInputFactoryVectorImpl final
    : ASR_UTILS_MULTIPLE_PROJECTION_GENERATORS(
          AsrInputFactoryVectorImpl,
          IAsrInputFactoryVectorImpl,
          IAsrSwigInputFactoryVectorImpl)
{
    using Type = typename InputFactoryManager::Type;
    using Container = std::vector<Type>;
    using ContainerIt = typename Container::const_iterator;

    ASR::Utils::RefCounter<AsrInputFactoryVectorImpl> ref_counter_{};
    Container                                         input_factory_vector_{};

    auto InternalFind(const AsrGuid& iid) -> ContainerIt;

public:
    AsrInputFactoryVectorImpl(const InputFactoryManager& InputFactoryManager);
    auto AddRef() -> int64_t;
    auto Release() -> int64_t;
    [[nodiscard]]
    auto Size() const noexcept -> size_t;
    auto At(size_t index, IAsrInputFactory** pp_out_factory) const -> AsrResult;
    auto Find(const AsrGuid& iid, IAsrInputFactory** pp_out_factory)
        -> AsrResult;
    auto At(size_t index) -> AsrRetInputFactory;
    auto Find(const AsrGuid& iid) -> AsrRetInputFactory;
};

ASR_CORE_FOREIGNINTERFACEHOST_NS_END

#endif // ASR_CORE_FOREIGNINTERFACEHOST_IASRINPUTFACTORYVECTORIMPL_H
