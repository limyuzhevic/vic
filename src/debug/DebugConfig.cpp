#include "DebugConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

namespace nlm {

struct DebugConfig::Impl {
    std::shared_ptr<Config> globalConfig;
    DebugMode debugMode;
    bool performanceMonitoringEnabled;
    bool memoryMonitoringEnabled;
    size_t memoryMonitoringThreshold;
    bool alertSystemEnabled;
    std::map<std::string, double> alertThresholds;
    bool visualizationEnabled;
    std::string visualizationOutputFormat;
    bool sessionRecordingEnabled;
    std::string sessionDirectory;
    bool breakpointsEnabled;
    bool neuralActivityMonitoringEnabled;
    double neuralActivityThreshold;
    bool profilingEnabled;
    std::vector<std::string> profilerMetrics;
    bool realTimeMonitoringEnabled;
    int realTimeUpdateInterval;
    bool eventDrivenMonitoringEnabled;
    bool contextAwareDebuggingEnabled;
    bool errorRecoveryEnabled;
    int maxErrorRecoveryAttempts;
    bool simulationPerformanceProfilingEnabled;
    bool memoryProfilingEnabled;
    bool neuralEventTimingAnalysisEnabled;
    bool bottleneckDetectionEnabled;
    bool loggingIntegrationEnabled;
    bool integrationWithExistingSystemsEnabled;
    bool debugSessionManagementEnabled;
    std::string sessionId;
    
    Impl() : debugMode(DebugMode::None)
        , performanceMonitoringEnabled(false)
        , memoryMonitoringEnabled(false)
        , memoryMonitoringThreshold(1024 * 1024 * 1024) // 1GB
        , alertSystemEnabled(true)
        , visualizationEnabled(true)
        , visualizationOutputFormat("json")
        , sessionRecordingEnabled(true)
        , breakpointsEnabled(true)
        , neuralActivityMonitoringEnabled(true)
        , neuralActivityThreshold(0.8)
        , profilingEnabled(true)
        , profilerMetrics({"cpu_usage", "memory_usage", "simulation_time"})
        , realTimeMonitoringEnabled(true)
        , realTimeUpdateInterval(100) // milliseconds
        , eventDrivenMonitoringEnabled(true)
        , contextAwareDebuggingEnabled(true)
        , errorRecoveryEnabled(true)
        , maxErrorRecoveryAttempts(3)
        , simulationPerformanceProfilingEnabled(true)
        , memoryProfilingEnabled(true)
        , neuralEventTimingAnalysisEnabled(true)
        , bottleneckDetectionEnabled(true)
        , loggingIntegrationEnabled(true)
        , integrationWithExistingSystemsEnabled(true)
        , debugSessionManagementEnabled(true)
    {
        time_t now = std::time(nullptr);
        tm tm_buf;
        char buf[80];
        strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", localtime_r(&now, &tm_buf));
        sessionId = std::string(buf);
    }
};

DebugConfig::DebugConfig() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_DEBUG("DebugConfig created with default settings");
}

DebugConfig::DebugConfig(std::shared_ptr<Config> globalConfig) : pImpl(std::make_unique<Impl>()) {
    pImpl->globalConfig = globalConfig;
    NLM_LOG_DEBUG("DebugConfig created with global configuration");
}

DebugConfig::~DebugConfig() {
    NLM_LOG_DEBUG("DebugConfig destroyed");
}

DebugConfig::DebugConfig(DebugConfig&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

DebugConfig& DebugConfig::operator=(DebugConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool DebugConfig::loadFromFile(const std::string& filepath) {
    NLM_LOG_DEBUG("Loading debug configuration from " + filepath);
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Debug configuration file not found: " + filepath);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Parse configuration lines (simple key=value format)
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                set(key, value);
            }
        }
        
        file.close();
        NLM_LOG_INFO("Debug configuration loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading debug configuration: " + std::string(e.what()));
        return false;
    }
}

bool DebugConfig::loadFromGlobalConfig() {
    if (!pImpl->globalConfig) {
        return false;
    }
    
    NLM_LOG_DEBUG("Loading debug configuration from global config");
    
    // Load debug mode
    if (auto debugModeOpt = pImpl->globalConfig->getOr<std::string>("debug_mode", "")) {
        std::string debugModeStr = *debugModeOpt;
        if (debugModeStr == "debug") {
            setDebugMode(DebugMode::Debug);
        } else if (debugModeStr == "profile") {
            setDebugMode(DebugMode::Profile);
        } else if (debugModeStr == "trace") {
            setDebugMode(DebugMode::Trace);
        } else if (debugModeStr == "release") {
            setDebugMode(DebugMode::Release);
        }
    }
    
    // Load other settings
    setPerformanceMonitoringEnabled(pImpl->globalConfig->getOr<bool>("debug_performance_monitoring", true));
    setMemoryMonitoringEnabled(pImpl->globalConfig->getOr<bool>("debug_memory_monitoring", true));
    setAlertSystemEnabled(pImpl->globalConfig->getOr<bool>("debug_alert_system", true));
    setVisualizationEnabled(pImpl->globalConfig->getOr<bool>("debug_visualization", true));
    setSessionRecordingEnabled(pImpl->globalConfig->getOr<bool>("debug_session_recording", true));
    setBreakpointsEnabled(pImpl->globalConfig->getOr<bool>("debug_breakpoints", true));
    setNeuralActivityMonitoringEnabled(pImpl->globalConfig->getOr<bool>("debug_neural_activity_monitoring", true));
    setProfilingEnabled(pImpl->globalConfig->getOr<bool>("debug_profiling", true));
    setRealTimeMonitoringEnabled(pImpl->globalConfig->getOr<bool>("debug_real_time_monitoring", true));
    setEventDrivenMonitoringEnabled(pImpl->globalConfig->getOr<bool>("debug_event_driven_monitoring", true));
    setContextAwareDebuggingEnabled(pImpl->globalConfig->getOr<bool>("debug_context_aware_debugging", true));
    setErrorRecoveryEnabled(pImpl->globalConfig->getOr<bool>("debug_error_recovery", true));
    setSimulationPerformanceProfilingEnabled(pImpl->globalConfig->getOr<bool>("debug_simulation_performance_profiling", true));
    setMemoryProfilingEnabled(pImpl->globalConfig->getOr<bool>("debug_memory_profiling", true));
    setNeuralEventTimingAnalysisEnabled(pImpl->globalConfig->getOr<bool>("debug_neural_event_timing_analysis", true));
    setBottleneckDetectionEnabled(pImpl->globalConfig->getOr<bool>("debug_bottleneck_detection", true));
    setLoggingIntegrationEnabled(pImpl->globalConfig->getOr<bool>("debug_logging_integration", true));
    setIntegrationWithExistingSystemsEnabled(pImpl->globalConfig->getOr<bool>("debug_integration_with_existing_systems", true));
    setDebugSessionManagementEnabled(pImpl->globalConfig->getOr<bool>("debug_session_management", true));
    
    NLM_LOG_INFO("Debug configuration loaded from global config");
    return true;
}

DebugMode DebugConfig::getDebugMode() const {
    return pImpl->debugMode;
}

void DebugConfig::setDebugMode(DebugMode mode) {
    pImpl->debugMode = mode;
    std::string modeStr;
    switch (mode) {
        case DebugMode::None: modeStr = "none"; break;
        case DebugMode::Release: modeStr = "release"; break;
        case DebugMode::Debug: modeStr = "debug"; break;
        case DebugMode::Profile: modeStr = "profile"; break;
        case DebugMode::Trace: modeStr = "trace"; break;
    }
    NLM_LOG_INFO("Debug mode set to: " + modeStr);
}

bool DebugConfig::isPerformanceMonitoringEnabled() const {
    return pImpl->performanceMonitoringEnabled;
}

void DebugConfig::setPerformanceMonitoringEnabled(bool enabled) {
    pImpl->performanceMonitoringEnabled = enabled;
    NLM_LOG_INFO("Performance monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isMemoryMonitoringEnabled() const {
    return pImpl->memoryMonitoringEnabled;
}

void DebugConfig::setMemoryMonitoringEnabled(bool enabled) {
    pImpl->memoryMonitoringEnabled = enabled;
    NLM_LOG_INFO("Memory monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

size_t DebugConfig::getMemoryMonitoringThreshold() const {
    return pImpl->memoryMonitoringThreshold;
}

void DebugConfig::setMemoryMonitoringThreshold(size_t threshold) {
    pImpl->memoryMonitoringThreshold = threshold;
    NLM_LOG_INFO("Memory monitoring threshold set to: " + std::to_string(threshold) + " bytes");
}

bool DebugConfig::isAlertSystemEnabled() const {
    return pImpl->alertSystemEnabled;
}

void DebugConfig::setAlertSystemEnabled(bool enabled) {
    pImpl->alertSystemEnabled = enabled;
    NLM_LOG_INFO("Alert system " + std::string(enabled ? "enabled" : "disabled"));
}

void DebugConfig::setAlertThreshold(const std::string& metric, double threshold) {
    pImpl->alertThresholds[metric] = threshold;
    NLM_LOG_INFO("Alert threshold set for " + metric + ": " + std::to_string(threshold));
}

double DebugConfig::getAlertThreshold(const std::string& metric) const {
    auto it = pImpl->alertThresholds.find(metric);
    if (it != pImpl->alertThresholds.end()) {
        return it->second;
    }
    return 0.0;
}

bool DebugConfig::isVisualizationEnabled() const {
    return pImpl->visualizationEnabled;
}

void DebugConfig::setVisualizationEnabled(bool enabled) {
    pImpl->visualizationEnabled = enabled;
    NLM_LOG_INFO("Visualization " + std::string(enabled ? "enabled" : "disabled"));
}

std::string DebugConfig::getVisualizationOutputFormat() const {
    return pImpl->visualizationOutputFormat;
}

void DebugConfig::setVisualizationOutputFormat(const std::string& format) {
    pImpl->visualizationOutputFormat = format;
    NLM_LOG_INFO("Visualization output format set to: " + format);
}

bool DebugConfig::isSessionRecordingEnabled() const {
    return pImpl->sessionRecordingEnabled;
}

void DebugConfig::setSessionRecordingEnabled(bool enabled) {
    pImpl->sessionRecordingEnabled = enabled;
    NLM_LOG_INFO("Session recording " + std::string(enabled ? "enabled" : "disabled"));
}

std::string DebugConfig::getSessionDirectory() const {
    return pImpl->sessionDirectory;
}

void DebugConfig::setSessionDirectory(const std::string& directory) {
    pImpl->sessionDirectory = directory;
    NLM_LOG_INFO("Session directory set to: " + directory);
}

bool DebugConfig::areBreakpointsEnabled() const {
    return pImpl->breakpointsEnabled;
}

void DebugConfig::setBreakpointsEnabled(bool enabled) {
    pImpl->breakpointsEnabled = enabled;
    NLM_LOG_INFO("Breakpoints " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isNeuralActivityMonitoringEnabled() const {
    return pImpl->neuralActivityMonitoringEnabled;
}

void DebugConfig::setNeuralActivityMonitoringEnabled(bool enabled) {
    pImpl->neuralActivityMonitoringEnabled = enabled;
    NLM_LOG_INFO("Neural activity monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

double DebugConfig::getNeuralActivityThreshold() const {
    return pImpl->neuralActivityThreshold;
}

void DebugConfig::setNeuralActivityThreshold(double threshold) {
    pImpl->neuralActivityThreshold = threshold;
    NLM_LOG_INFO("Neural activity threshold set to: " + std::to_string(threshold));
}

bool DebugConfig::isProfilingEnabled() const {
    return pImpl->profilingEnabled;
}

void DebugConfig::setProfilingEnabled(bool enabled) {
    pImpl->profilingEnabled = enabled;
    NLM_LOG_INFO("Profiling " + std::string(enabled ? "enabled" : "disabled"));
}

std::vector<std::string> DebugConfig::getProfilerMetrics() const {
    return pImpl->profilerMetrics;
}

void DebugConfig::setProfilerMetrics(const std::vector<std::string>& metrics) {
    pImpl->profilerMetrics = metrics;
    NLM_LOG_INFO("Profiler metrics set to " + std::to_string(metrics.size()) + " metrics");
}

bool DebugConfig::isRealTimeMonitoringEnabled() const {
    return pImpl->realTimeMonitoringEnabled;
}

void DebugConfig::setRealTimeMonitoringEnabled(bool enabled) {
    pImpl->realTimeMonitoringEnabled = enabled;
    NLM_LOG_INFO("Real-time monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

int DebugConfig::getRealTimeUpdateInterval() const {
    return pImpl->realTimeUpdateInterval;
}

void DebugConfig::setRealTimeUpdateInterval(int interval) {
    pImpl->realTimeUpdateInterval = interval;
    NLM_LOG_INFO("Real-time update interval set to: " + std::to_string(interval) + " ms");
}

bool DebugConfig::isEventDrivenMonitoringEnabled() const {
    return pImpl->eventDrivenMonitoringEnabled;
}

void DebugConfig::setEventDrivenMonitoringEnabled(bool enabled) {
    pImpl->eventDrivenMonitoringEnabled = enabled;
    NLM_LOG_INFO("Event-driven monitoring " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isContextAwareDebuggingEnabled() const {
    return pImpl->contextAwareDebuggingEnabled;
}

void DebugConfig::setContextAwareDebuggingEnabled(bool enabled) {
    pImpl->contextAwareDebuggingEnabled = enabled;
    NLM_LOG_INFO("Context-aware debugging " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isErrorRecoveryEnabled() const {
    return pImpl->errorRecoveryEnabled;
}

void DebugConfig::setErrorRecoveryEnabled(bool enabled) {
    pImpl->errorRecoveryEnabled = enabled;
    NLM_LOG_INFO("Error recovery " + std::string(enabled ? "enabled" : "disabled"));
}

int DebugConfig::getMaxErrorRecoveryAttempts() const {
    return pImpl->maxErrorRecoveryAttempts;
}

void DebugConfig::setMaxErrorRecoveryAttempts(int attempts) {
    pImpl->maxErrorRecoveryAttempts = attempts;
    NLM_LOG_INFO("Max error recovery attempts set to: " + std::to_string(attempts));
}

bool DebugConfig::isSimulationPerformanceProfilingEnabled() const {
    return pImpl->simulationPerformanceProfilingEnabled;
}

void DebugConfig::setSimulationPerformanceProfilingEnabled(bool enabled) {
    pImpl->simulationPerformanceProfilingEnabled = enabled;
    NLM_LOG_INFO("Simulation performance profiling " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isMemoryProfilingEnabled() const {
    return pImpl->memoryProfilingEnabled;
}

void DebugConfig::setMemoryProfilingEnabled(bool enabled) {
    pImpl->memoryProfilingEnabled = enabled;
    NLM_LOG_INFO("Memory profiling " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isNeuralEventTimingAnalysisEnabled() const {
    return pImpl->neuralEventTimingAnalysisEnabled;
}

void DebugConfig::setNeuralEventTimingAnalysisEnabled(bool enabled) {
    pImpl->neuralEventTimingAnalysisEnabled = enabled;
    NLM_LOG_INFO("Neural event timing analysis " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isBottleneckDetectionEnabled() const {
    return pImpl->bottleneckDetectionEnabled;
}

void DebugConfig::setBottleneckDetectionEnabled(bool enabled) {
    pImpl->bottleneckDetectionEnabled = enabled;
    NLM_LOG_INFO("Bottleneck detection " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isLoggingIntegrationEnabled() const {
    return pImpl->loggingIntegrationEnabled;
}

void DebugConfig::setLoggingIntegrationEnabled(bool enabled) {
    pImpl->loggingIntegrationEnabled = enabled;
    NLM_LOG_INFO("Logging integration " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isIntegrationWithExistingSystemsEnabled() const {
    return pImpl->integrationWithExistingSystemsEnabled;
}

void DebugConfig::setIntegrationWithExistingSystemsEnabled(bool enabled) {
    pImpl->integrationWithExistingSystemsEnabled = enabled;
    NLM_LOG_INFO("Integration with existing systems " + std::string(enabled ? "enabled" : "disabled"));
}

bool DebugConfig::isDebugSessionManagementEnabled() const {
    return pImpl->debugSessionManagementEnabled;
}

void DebugConfig::setDebugSessionManagementEnabled(bool enabled) {
    pImpl->debugSessionManagementEnabled = enabled;
    NLM_LOG_INFO("Debug session management " + std::string(enabled ? "enabled" : "disabled"));
}

std::string DebugConfig::getSessionId() const {
    return pImpl->sessionId;
}

void DebugConfig::setSessionId(const std::string& sessionId) {
    pImpl->sessionId = sessionId;
    NLM_LOG_INFO("Debug session ID set to: " + sessionId);
}

void DebugConfig::set(const std::string& key, const ConfigValue& value) {
    // Convert string key to lowercase for consistency
    std::string lowerKey = key;
    for (char& c : lowerKey) {
        c = std::tolower(c);
    }
    
    // Set configuration based on key
    if (lowerKey == "debug_mode") {
        if (std::holds_alternative<std::string>(value)) {
            std::string modeStr = std::get<std::string>(value);
            if (modeStr == "debug") {
                setDebugMode(DebugMode::Debug);
            } else if (modeStr == "profile") {
                setDebugMode(DebugMode::Profile);
            } else if (modeStr == "trace") {
                setDebugMode(DebugMode::Trace);
            } else if (modeStr == "release") {
                setDebugMode(DebugMode::Release);
            }
        }
    } else if (lowerKey == "performance_monitoring_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setPerformanceMonitoringEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "memory_monitoring_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setMemoryMonitoringEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "memory_monitoring_threshold") {
        if (std::holds_alternative<size_t>(value)) {
            setMemoryMonitoringThreshold(std::get<size_t>(value));
        }
    } else if (lowerKey == "alert_system_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setAlertSystemEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "visualization_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setVisualizationEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "session_recording_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setSessionRecordingEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "breakpoints_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setBreakpointsEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "neural_activity_monitoring_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setNeuralActivityMonitoringEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "neural_activity_threshold") {
        if (std::holds_alternative<double>(value)) {
            setNeuralActivityThreshold(std::get<double>(value));
        }
    } else if (lowerKey == "profiling_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setProfilingEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "real_time_monitoring_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setRealTimeMonitoringEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "real_time_update_interval") {
        if (std::holds_alternative<int>(value)) {
            setRealTimeUpdateInterval(std::get<int>(value));
        }
    } else if (lowerKey == "event_driven_monitoring_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setEventDrivenMonitoringEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "context_aware_debugging_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setContextAwareDebuggingEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "error_recovery_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setErrorRecoveryEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "max_error_recovery_attempts") {
        if (std::holds_alternative<int>(value)) {
            setMaxErrorRecoveryAttempts(std::get<int>(value));
        }
    } else if (lowerKey == "simulation_performance_profiling_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setSimulationPerformanceProfilingEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "memory_profiling_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setMemoryProfilingEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "neural_event_timing_analysis_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setNeuralEventTimingAnalysisEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "bottleneck_detection_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setBottleneckDetectionEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "logging_integration_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setLoggingIntegrationEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "integration_with_existing_systems_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setIntegrationWithExistingSystemsEnabled(std::get<bool>(value));
        }
    } else if (lowerKey == "debug_session_management_enabled") {
        if (std::holds_alternative<bool>(value)) {
            setDebugSessionManagementEnabled(std::get<bool>(value));
        }
    }
}

std::string DebugConfig::summary() const {
    std::stringstream ss;
    ss << "Debug Configuration Summary:\n";
    ss << "  Debug Mode: ";
    switch (pImpl->debugMode) {
        case DebugMode::None: ss << "None"; break;
        case DebugMode::Release: ss << "Release"; break;
        case DebugMode::Debug: ss << "Debug"; break;
        case DebugMode::Profile: ss << "Profile"; break;
        case DebugMode::Trace: ss << "Trace"; break;
    }
    ss << "\n";
    ss << "  Performance Monitoring: " << (pImpl->performanceMonitoringEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Memory Monitoring: " << (pImpl->memoryMonitoringEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Alert System: " << (pImpl->alertSystemEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Visualization: " << (pImpl->visualizationEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Session Recording: " << (pImpl->sessionRecordingEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Breakpoints: " << (pImpl->breakpointsEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Neural Activity Monitoring: " << (pImpl->neuralActivityMonitoringEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Profiling: " << (pImpl->profilingEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Real-time Monitoring: " << (pImpl->realTimeMonitoringEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Event-driven Monitoring: " << (pImpl->eventDrivenMonitoringEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Context-aware Debugging: " << (pImpl->contextAwareDebuggingEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Error Recovery: " << (pImpl->errorRecoveryEnabled ? "Enabled" : "Disabled") << "\n";
    ss << "  Session ID: " << pImpl->sessionId << "\n";
    return ss.str();
}

std::shared_ptr<Config> DebugConfig::getGlobalConfig() const {
    return pImpl->globalConfig;
}

} // namespace nlm
