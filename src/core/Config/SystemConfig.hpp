#include "SystemConfig.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct SystemConfig::Impl {
    int threadCount = std::thread::hardware_concurrency();
    size_t memoryLimit = 2 * 1024 * 1024 * 1024; // 2GB default
    double timeStep = 0.001;
    double maxTime = 100.0;
    std::string logLevel = "info";
    std::string logFile = "nlm.log";
    bool performanceMonitoring = true;
    bool profilingEnabled = false;
};

SystemConfig::SystemConfig() {
    pImplSystem = std::make_unique<Impl>();
    
    // Set up default configuration
    setThreadCount(pImplSystem->threadCount);
    setMemoryLimit(pImplSystem->memoryLimit);
    setTimeStep(pImplSystem->timeStep);
    setMaxTime(pImplSystem->maxTime);
    
    setLogLevel(pImplSystem->logLevel);
    setLogFile(pImplSystem->logFile);
    setPerformanceMonitoring(pImplSystem->performanceMonitoring);
    setProfilingEnabled(pImplSystem->profilingEnabled);
}

SystemConfig::~SystemConfig() = default;

SystemConfig::SystemConfig(SystemConfig&&) noexcept = default;
SystemConfig& SystemConfig::operator=(SystemConfig&&) noexcept = default;

void SystemConfig::setThreadCount(int threads) {
    pImplSystem->threadCount = threads;
    set("sys_thread_count", threads, ConfigSource::Runtime,
        "Number of threads to use", 
        SYSTEM_CATEGORY, TYPE_INT, 1, 64);
}

void SystemConfig::setMemoryLimit(size_t limit) {
    pImplSystem->memoryLimit = limit;
    set("sys_memory_limit", static_cast<int64_t>(limit), ConfigSource::Runtime,
        "Memory limit in bytes", 
        SYSTEM_CATEGORY, TYPE_INT, 1024, 16384);
}

void SystemConfig::setTimeStep(double timestep) {
    pImplSystem->timeStep = timestep;
    set("sys_time_step", timestep, ConfigSource::Runtime,
        "Simulation time step", 
        SYSTEM_CATEGORY, TYPE_DOUBLE, 0.0, 1.0);
}

void SystemConfig::setMaxTime(double maxTime) {
    pImplSystem->maxTime = maxTime;
    set("sys_max_time", maxTime, ConfigSource::Runtime,
        "Maximum simulation time", 
        SYSTEM_CATEGORY, TYPE_DOUBLE, 1.0, 1000.0);
}

void SystemConfig::setLogLevel(const std::string& level) {
    pImplSystem->logLevel = level;
    set("sys_log_level", level, ConfigSource::Runtime,
        "Logging level", 
        SYSTEM_CATEGORY, TYPE_STRING);
}

void SystemConfig::setLogFile(const std::string& file) {
    pImplSystem->logFile = file;
    set("sys_log_file", file, ConfigSource::Runtime,
        "Log file path", 
        SYSTEM_CATEGORY, TYPE_STRING);
}

void SystemConfig::setPerformanceMonitoring(bool enabled) {
    pImplSystem->performanceMonitoring = enabled;
    set("sys_performance_monitoring", static_cast<bool>(enabled), ConfigSource::Runtime,
        "Whether to monitor performance metrics", 
        SYSTEM_CATEGORY, TYPE_BOOL);
}

void SystemConfig::setProfilingEnabled(bool enabled) {
    pImplSystem->profilingEnabled = enabled;
    set("sys_profiling_enabled", static_cast<bool>(enabled), ConfigSource::Runtime,
        "Whether to enable profiling", 
        SYSTEM_CATEGORY, TYPE_BOOL);
}

std::string SystemConfig::getSystemSummary() const {
    std::ostringstream oss;
    oss << "System Configuration:";
    oss << "\n  Thread count: " << pImplSystem->threadCount;
    oss << "\n  Memory limit: " << (pImplSystem->memoryLimit / (1024 * 1024)) << " MB";
    oss << "\n  Time step: " << pImplSystem->timeStep << "s";
    oss << "\n  Max time: " << pImplSystem->maxTime << "s";
    oss << "\n  Log level: " << pImplSystem->logLevel;
    oss << "\n  Log file: " << pImplSystem->logFile;
    oss << "\n  Performance monitoring: " << (pImplSystem->performanceMonitoring ? "yes" : "no");
    oss << "\n  Profiling enabled: " << (pImplSystem->profilingEnabled ? "yes" : "no");
    oss << "\n  Validation: " << (validateWithErrors().empty() ? "PASS" : "FAIL");
    return oss.str();
}

} // namespace nlm
