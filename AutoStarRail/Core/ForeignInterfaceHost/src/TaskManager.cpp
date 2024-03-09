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
    map[kv.first] = AsrPtr<T>{kv.second, take_ownership};
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
    AsrPtr<IAsrTask> p_task{};

    try
    {
        p_task = MakeAsrPtr<IAsrTask, SwigToCpp<IAsrSwigTask>>(p_swig_task);
    }
    catch (const std::bad_alloc&)
    {
        return ASR_E_OUT_OF_MEMORY;
    }

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

ASR_CORE_FOREIGNINTERFACEHOST_NS_END
