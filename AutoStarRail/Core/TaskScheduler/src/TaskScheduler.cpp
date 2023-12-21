
#include <algorithm>

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Core/TaskScheduler/TaskScheduler.h>

ASR_NS_BEGIN

namespace Core
{
    bool TaskScheduler::SchedulingUnit::operator==(const SchedulingUnit& rhs)
    {
        return this->p_task == rhs.p_task;
    }

    void TaskScheduler::AddTask(SchedulingUnit task)
    {
        task_queue.push_back(task);

        std::sort(
            ASR_FULL_RANGE_OF(task_queue),
            [](const SchedulingUnit& lhs, const SchedulingUnit& rhs)
            { return lhs.next_run_time <= rhs.next_run_time; });
    }

    void TaskScheduler::DeleteTask(SchedulingUnit task)
    {
        task_queue.erase(
            std::remove(ASR_FULL_RANGE_OF(task_queue), task),
            task_queue.end());

        std::sort(
            ASR_FULL_RANGE_OF(task_queue),
            [](const SchedulingUnit& lhs, const SchedulingUnit& rhs)
            { return lhs.next_run_time <= rhs.next_run_time; });
    }

    void TaskScheduler::RunTaskQueue()
    {
        current_task = task_queue.back();

        task_queue.pop_back();

        // TODO: 插件调用方式待实现
        // current_task.p_task->Do(p_connection_json, p_task_settings_json);
    }
} // namespace Core

ASR_NS_END