
#include <algorithm>

#include <AutoStarRail/AsrPtr.hpp>
#include <AutoStarRail/Core/TaskScheduler/TaskScheduler.h>

ASR_NS_BEGIN

namespace Core
{
    bool TaskScheduler::SchedulingUnit::operator==(const SchedulingUnit& t)
    {
        return this->p_task == t.p_task;
    }

    void TaskScheduler::AddTask(SchedulingUnit Task)
    {
        task_queue.push_back(Task);

        std::sort(
            task_queue.begin(),
            task_queue.end(),
            [](SchedulingUnit t1, SchedulingUnit t2)
            { return t1.next_run_time <= t2.next_run_time; });
    }

    void TaskScheduler::DeleteTask(SchedulingUnit Task)
    {
        std::ignore = std::remove(task_queue.begin(), task_queue.end(), Task);

        std::sort(
            task_queue.begin(),
            task_queue.end(),
            [](SchedulingUnit t1, SchedulingUnit t2)
            { return t1.next_run_time <= t2.next_run_time; });
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