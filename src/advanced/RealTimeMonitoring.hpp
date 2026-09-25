#ifndef NLM_REALTIME_MONITORING_HPP
#define NLM_REALTIME_MONITORING_HPP

#include <string>
#include <memory>
#include <chrono>
#include <thread>

namespace nlm {

class RealTimeMonitor {
private:
    bool running;
    std::chrono::steady_clock::time_point startTime;
    std::shared_ptr<Brain> brain;
    
public:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    void startMonitoring(const std::shared_ptr<Brain>& brainRef);
    void stopMonitoring();
    void displayRealTimeStats() const;
    void updateStatistics() const;
    double calculateCPUUsage() const;
    double calculateMemoryUsage() const;
    void printProgressBar(double progress) const;
};

} // namespace nlm

#endif // NLM_REALTIME_MONITORING_HPP