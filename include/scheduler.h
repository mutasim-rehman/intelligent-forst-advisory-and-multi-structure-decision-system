#ifndef IFAMDS_SCHEDULER_H
#define IFAMDS_SCHEDULER_H

#include "models.h"
#include "custom_queue.h"

namespace ifamds {

// ============================================================
// Comparator for the emergency priority queue
// Higher priority value = higher urgency
// ============================================================
// Comparator removed since Task implements operator>
// ============================================================
// Scheduler Module
// Uses custom data structures instead of STL:
//   Q1: CustomQueue<Task> for routine monitoring tasks (FIFO)
//   Q2: CustomQueue<Task> for surveillance tasks (FIFO)
//   Q3: CustomPriorityQueue<Task> for emergency tasks (max-heap)
//   Q4: CustomQueue<Task> for multi-factor decision tasks (FIFO)
// ============================================================
class SchedulerModule {
public:
    void enqueueRoutine(const Task& task);           // Q1
    void enqueueSurveillance(const Task& task);      // Q2
    void enqueueEmergency(const Task& task);         // Q3
    void enqueueMultiFactor(const Task& task);       // Q4
    bool processNextTask(Task& outTask);
    std::size_t routineSize() const;
    std::size_t surveillanceSize() const;
    std::size_t emergencySize() const;
    std::size_t multiFactorSize() const;

private:
    CustomQueue<Task> routineQueue_;                                       // Q1
    CustomQueue<Task> surveillanceQueue_;                                  // Q2
    CustomPriorityQueue<Task> emergencyQueue_;                             // Q3
    CustomQueue<Task> multiFactorQueue_;                                   // Q4
};

}  // namespace ifamds

#endif
