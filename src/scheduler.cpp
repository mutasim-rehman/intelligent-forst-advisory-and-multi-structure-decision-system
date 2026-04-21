#include "scheduler.h"

namespace ifamds {

bool TaskPriorityCompare::operator()(const Task& a, const Task& b) const {
    return a.priority < b.priority;
}

void SchedulerModule::enqueueRoutine(const Task& task) {
    routineQueue_.push(task);
}

void SchedulerModule::enqueueSurveillance(const Task& task) {
    surveillanceQueue_.push(task);
}

void SchedulerModule::enqueueEmergency(const Task& task) {
    emergencyQueue_.push(task);
}

bool SchedulerModule::processNextTask(Task& outTask) {
    if (!emergencyQueue_.empty()) {
        outTask = emergencyQueue_.top();
        emergencyQueue_.pop();
        return true;
    }
    if (!surveillanceQueue_.empty()) {
        outTask = surveillanceQueue_.front();
        surveillanceQueue_.pop();
        return true;
    }
    if (!routineQueue_.empty()) {
        outTask = routineQueue_.front();
        routineQueue_.pop();
        return true;
    }
    return false;
}

std::size_t SchedulerModule::routineSize() const {
    return routineQueue_.size();
}

std::size_t SchedulerModule::surveillanceSize() const {
    return surveillanceQueue_.size();
}

std::size_t SchedulerModule::emergencySize() const {
    return emergencyQueue_.size();
}

}  // namespace ifamds
