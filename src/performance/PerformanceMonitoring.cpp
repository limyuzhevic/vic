// Performance Monitoring and Profiling Tools for NLM
// Implementation file

#include "PerformanceMonitoring.hpp"
#include <random>

namespace nlm {

// Global performance monitoring instances
PerfMonitor global_perf_monitor;
StepProfiler global_step_profiler;
SystemMonitor global_system_monitor;

// Convenience functions for easy performance monitoring
void startGlobalMonitoring() {
    global_perf_monitor.startMonitoring();
}

void stopGlobalMonitoring() {
    global_perf_monitor.stopMonitoring();
}

PerfCounter& getPerfCounter(const std::string& name) {
    return *global_perf_monitor.getCounter(name);
}

void profileStep(const std::string& step_name, std::function<void()> func) {
    global_step_profiler.profileStep(step_name, func);
}

void updateSystemMetrics() {
    global_system_monitor.updateSystemMetrics();
}

std::string generatePerformanceReport() {
    return global_perf_monitor.generateReport() + "\n" + 
           global_step_profiler.generateStepReport() + "\n" +
           global_system_monitor.generateSystemReport();
}

bool exportPerformanceReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generatePerformanceReport();
    file.close();
    return true;
}

} // namespace nlm
