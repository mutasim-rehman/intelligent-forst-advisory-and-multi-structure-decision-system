#include "scheduler.h"

namespace ifamds {

// Enqueue a routine monitoring task into Q1 (FIFO) - O(1)
void SchedulerModule::enqueueRoutine(const Task& task) {
    routineQueue_.enqueue(task);
}

// Enqueue a surveillance task into Q2 (FIFO) - O(1)
void SchedulerModule::enqueueSurveillance(const Task& task) {
    surveillanceQueue_.enqueue(task);
}

// Enqueue an emergency task into Q3 (priority queue / max-heap) - O(log n)
void SchedulerModule::enqueueEmergency(const Task& task) {
    emergencyQueue_.push(task);
}

// Enqueue a multi-factor decision task into Q4 (FIFO) - O(1)
void SchedulerModule::enqueueMultiFactor(const Task& task) {
    multiFactorQueue_.enqueue(task);
}

// Process the next task with priority: Emergency > Surveillance > MultiF > Routine
// Emergency dequeue: O(log n), others: O(1)
bool SchedulerModule::processNextTask(Task& outTask) {
    // Q3: Emergency tasks have highest priority
    if (emergencyQueue_.pop(outTask)) {
        return true;
    }
    // Q2: Surveillance tasks next
    if (surveillanceQueue_.dequeue(outTask)) {
        return true;
    }
    // Q4: Multi-factor decision tasks
    if (multiFactorQueue_.dequeue(outTask)) {
        return true;
    }
    // Q1: Routine tasks last
    if (routineQueue_.dequeue(outTask)) {
        return true;
    }
    return false;
}

std::size_t SchedulerModule::routineSize() const { return routineQueue_.size(); }
std::size_t SchedulerModule::surveillanceSize() const { return surveillanceQueue_.size(); }
std::size_t SchedulerModule::emergencySize() const { return emergencyQueue_.size(); }
std::size_t SchedulerModule::multiFactorSize() const { return multiFactorQueue_.size(); }

}  // namespace ifamds
