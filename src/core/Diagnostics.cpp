// System Monitor Implementation for NLM
// Runtime monitoring and health checking

#include "core/Diagnostics.hpp"
#include <iostream>
#include <mutex>
#include <iomanip>
#include <algorithm>

namespace nlm {

// System monitor implementation
void SystemMonitor::updateResourceUsage(HealthStatus& status) {
    // Mock resource usage (in real implementation, this would query system resources)
    status.systemResources["cpu"] = getAverageCPUUsage();
    status.systemResources["memory"] = getAverageMemoryUsage();
    status.systemResources["threads"] = diagnosticInfo_.componentHealth.size();
    
    // Check for resource issues
    if (status.systemResources["cpu"] > 80.0) {
        status.warnings.push_back("CPU usage is high: " + std::to_string(status.systemResources["cpu"]) + "%");
    }
    
    if (status.systemResources["memory"] > 2000.0) {  // 2GB threshold
        status.warnings.push_back("Memory usage is high: " + std::to_string(status.systemResources["memory"]) + "MB");
    }
}

void SystemMonitor::calculateHealthScore(HealthStatus& status) {
    double score = 100.0;
    
    // Deduct points for issues
    score -= status.issues.size() * 10.0;
    
    // Deduct points for warnings
    score -= status.warnings.size() * 5.0;
    
    // Deduct points for resource issues
    auto& resources = status.systemResources;
    if (resources["cpu"] > 90.0) score -= 20.0;
    if (resources["memory"] > 4000.0) score -= 20.0;
    
    // Ensure score is within bounds
    score = std::max(0.0, std::min(100.0, score));
    status.healthScore = score;
    
    if (score < 50.0) {
        status.isHealthy = false;
    }
}

} // namespace nlm
