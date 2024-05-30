#include <AutoStarRail/Core/ForeignInterfaceHost/CppSwigInterop.h>
#include <AutoStarRail/Core/ForeignInterfaceHost/TaskManager.h>
#include <AutoStarRail/Core/Logger/Logger.h>

ASR_CORE_FOREIGNINTERFACEHOST_NS_BEGIN

ASR_NS_ANONYMOUS_DETAILS_BEGIN

template <class Map, class T>
AsrResult AddTask(Map& map, std::pair<AsrGuid, T*> kv)
{
    if (const auto it = map.find(kv.first); it != map.end())
    {
        return ASR_E_DUPLICATE_ELEMENT;
    }
    map[kv.first] = AsrPtr<T>{kv.second};
    return ASR_S_OK;
}

ASR_NS_ANONYMOUS_DETAILS_END

AsrResult TaskManager::Register(IAsrTask* p_task, AsrGuid guid)
{
    const auto error_code =
        Details::AddTask(map_, std::make_pair(guid, p_task));
    if (IsFailed(error_code))
    {
        ASR_CORE_LOG_WARN(
            "Duplicate IAsrTask object registered."
            "Guid = {}. Error code = {}. ",
            error_code,
            guid);
    }
    return error_code;
}

AsrResult TaskManager::Register(IAsrSwigTask* p_swig_task, AsrGuid guid)
{
    auto expected_p_task = MakeInterop<IAsrTask>(p_swig_task);
    if (!expected_p_task)
    {
        return expected_p_task.error();
    }
    const AsrPtr<IAsrTask> p_task = std::move(expected_p_task.value());

    const auto error_code =
        Details::AddTask(map_, std::make_pair(guid, p_task.Get()));
    if (IsFailed(error_code))
    {
        ASR_CORE_LOG_WARN(
            "Duplicate IAsrSwigTask object registered."
            "Guid = {}. Error code = {}. ",
            error_code,
            guid);
    }
    return error_code;
}

AsrResult TaskManager::FindInterface(
    const AsrGuid& guid,
    IAsrTask**     pp_out_task)
{
    ASR_UTILS_CHECK_POINTER(pp_out_task)
    if (const auto it = map_.find(guid); it != map_.end())
    {
        const auto& p_task = it->second;
        *pp_out_task = p_task.Get();
        p_task->AddRef();
        return ASR_S_OK;
    }
    return ASR_E_NO_INTERFACE;
}

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
