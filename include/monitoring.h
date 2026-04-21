#ifndef IFAMDS_MONITORING_H
#define IFAMDS_MONITORING_H

#include "models.h"
#include <string>

namespace ifamds {

class MonitoringModule {
public:
    void startTimer();
    void stopTimer(SystemMetrics& metrics, const std::string& moduleName);
    void updateQueueMetrics(SystemMetrics& metrics, std::size_t routine, std::size_t emergency) const;

private:
    long long startNs_{0};
};

}  // namespace ifamds

#endif
