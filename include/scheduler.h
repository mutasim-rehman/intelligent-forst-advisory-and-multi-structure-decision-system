#ifndef IFAMDS_SCHEDULER_H
#define IFAMDS_SCHEDULER_H

#include "models.h"
#include <queue>

namespace ifamds {

struct TaskPriorityCompare {
    bool operator()(const Task& a, const Task& b) const;
};

class SchedulerModule {
public:
    void enqueueRoutine(const Task& task);
    void enqueueSurveillance(const Task& task);
    void enqueueEmergency(const Task& task);
    bool processNextTask(Task& outTask);
    std::size_t routineSize() const;
    std::size_t surveillanceSize() const;
    std::size_t emergencySize() const;

private:
    std::queue<Task> routineQueue_;
    std::queue<Task> surveillanceQueue_;
    std::priority_queue<Task, std::vector<Task>, TaskPriorityCompare> emergencyQueue_;
};

}  // namespace ifamds

#endif
