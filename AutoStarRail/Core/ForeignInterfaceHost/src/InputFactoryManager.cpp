#include <AutoStarRail/Core/Exceptions/AsrException.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/AsrGuid.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/InputFactoryManager.h>
#include <AutoStarRail/Core/Logger/Logger.h>
#include <AutoStarRail/Core/Utils/InternalUtils.h>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

AsrResult ForeignInterfaceHost::InputFactoryManager::Register(
    IAsrInputFactory* p_factory)
{
    try
    {
        auto       p_factory_holder = AsrPtr{p_factory};
        const auto expected_p_swig_factory =
            MakeInterop<IAsrSwigInputFactory>(p_factory);
        if (!expected_p_swig_factory)
        {
            const auto error_code = expected_p_swig_factory.error();
            ASR_CORE_LOG_ERROR(
                "Failed when calling MakeInterop<IAsrSwigInputFactory>(p_factory)."
                "Error code = {}.",
                error_code);
            return error_code;
        }
        common_input_factory_vector_.emplace_back(
            p_factory_holder,
            expected_p_swig_factory.value());
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}

AsrResult InputFactoryManager::Register(IAsrSwigInputFactory* p_factory)
{
    try
    {
        auto       p_factory_holder = AsrPtr{p_factory};
        const auto expected_p_cpp_factory =
            MakeInterop<IAsrInputFactory>(p_factory);
        if (!expected_p_cpp_factory)
        {
            const auto error_code = expected_p_cpp_factory.error();
            ASR_CORE_LOG_ERROR(
                "Failed when calling MakeInterop<IAsrSwigInputFactory>(p_factory)."
                "Error code = {}.",
                error_code);
            return error_code;
        }
        common_input_factory_vector_.emplace_back(
            expected_p_cpp_factory.value(),
            p_factory_holder);
        return ASR_S_OK;
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }
}
AsrResult InputFactoryManager::FindInterface(
    const AsrGuid&     iid,
    IAsrInputFactory** pp_out_factory)
{
    ASR_UTILS_CHECK_POINTER(pp_out_factory)
    if (const auto factory_it = std::find_if(
            ASR_FULL_RANGE_OF(common_input_factory_vector_),
            [&iid](const Type& cpp_swig_object)
            {
                try
                {
                    const auto factory_iid =
                        Utils::GetGuidFrom(cpp_swig_object.first.Get());
                    return factory_iid == iid;
                }
                catch (const AsrException& ex)
                {
                    ASR_CORE_LOG_EXCEPTION(ex);
                    return false;
                }
            });
        factory_it != common_input_factory_vector_.end())
    {
        const auto p_result = factory_it->first.Get();
        *pp_out_factory = p_result;
        p_result->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_NO_INTERFACE;
}

void InputFactoryManager::At(
    size_t                    index,
    AsrPtr<IAsrInputFactory>& ref_out_factory)
{
    ref_out_factory = common_input_factory_vector_.at(index).first;
}

void InputFactoryManager::At(
    size_t                        index,
    AsrPtr<IAsrSwigInputFactory>& ref_out_factory)
{
    ref_out_factory = common_input_factory_vector_.at(index).second;
}

void InputFactoryManager::Find(
    const AsrGuid&     iid,
    IAsrInputFactory** pp_out_factory)
{
    if (pp_out_factory == nullptr)
    {
        AsrException::Throw(ASR_E_INVALID_POINTER);
        return;
    }

    const auto result = std::ranges::find_if(
        common_input_factory_vector_,
        [iid](const Type& item)
        {
            const auto gg_result = item.second->GetGuid();
            if (IsFailed(gg_result.error_code))
            {
                return false;
            }
            return gg_result.value == iid;
        });

    if (result == common_input_factory_vector_.end())
    {
        AsrException::Throw(ASR_E_OUT_OF_RANGE);
    }

    const auto p_result = result->first.Get();
    *pp_out_factory = p_result;
    p_result->AddRef();
}

void InputFactoryManager::Find(
    const AsrGuid&         iid,
    IAsrSwigInputFactory** pp_out_swig_factory)
{
    if (pp_out_swig_factory == nullptr)
    {
        AsrException::Throw(ASR_E_INVALID_POINTER);
        return;
    }

    const auto result = std::ranges::find_if(
        common_input_factory_vector_,
        [iid](const Type& item)
        {
            const auto gg_result = item.second->GetGuid();
            if (IsFailed(gg_result.error_code))
            {
                return false;
            }
            return gg_result.value == iid;
        });

    if (result == common_input_factory_vector_.end())
    {
        AsrException::Throw(ASR_E_OUT_OF_RANGE);
    }

    const auto p_result = result->second.Get();
    *pp_out_swig_factory = p_result;
    p_result->AddRef();
}

auto InputFactoryManager::GetVector() const
    -> decltype(common_input_factory_vector_)
{
    return common_input_factory_vector_;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
