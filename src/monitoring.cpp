#include "monitoring.h"

#include <chrono>

namespace ifamds {

void MonitoringModule::startTimer() {
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    startNs_ = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void MonitoringModule::stopTimer(SystemMetrics& metrics, const std::string& moduleName) {
    const auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    const long long endNs = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    const double elapsedMs = static_cast<double>(endNs - startNs_) / 1000000.0;
    metrics.lastLatencyMs = elapsedMs;
    metrics.bottleneckDetected = elapsedMs > 25.0;
    metrics.bottleneckModule = metrics.bottleneckDetected ? moduleName : "";
}

void MonitoringModule::updateQueueMetrics(SystemMetrics& metrics, std::size_t routine, std::size_t emergency) const {
    metrics.routineQueueSize = routine;
    metrics.emergencyQueueSize = emergency;
}

}  // namespace ifamds
