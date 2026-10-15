// NLM Advanced Debugging and Profiling Tools - Debug Manager Implementation

#include "debug/DebugManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <json/json.h>

namespace nlm {

// Static members
std::unique_ptr<DebugManager> DebugManager::instance_ = nullptr;
std::mutex DebugManager::instanceMutex_;

// DebugConfig implementation
DebugConfig DebugConfig::loadFromConfig(const std::string& filename) {
    DebugConfig config;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return config; // Return defaults if file doesn't exist
    }
    
    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errors;
    
    if (Json::parseFromStream(readerBuilder, file, root, errors)) {
        config.enableBreakpointDebugging = root.get("enableBreakpointDebugging", true).asBool();
        config.enableWatchpointDebugging = root.get("enableWatchpointDebugging", true).asBool();
        config.enableTraceDebugging = root.get("enableTraceDebugging", true).asBool();
        config.enableMemoryDebugging = root.get("enableMemoryDebugging", true).asBool();
        config.enableConcurrencyDebugging = root.get("enableConcurrencyDebugging", true).asBool();
        config.enablePerformanceProfiling = root.get("enablePerformanceProfiling", true).asBool();
        config.enableSystemMonitoring = root.get("enableSystemMonitoring", true).asBool();
        config.enableBottleneckAnalysis = root.get("enableBottleneckAnalysis", true).asBool();
        config.enableOptimizationAnalysis = root.get("enableOptimizationAnalysis", true).asBool();
        config.enableVisualization = root.get("enableVisualization", true).asBool();
        config.enableReportGeneration = root.get("enableReportGeneration", true).asBool();
        
        config.profilingInterval = root.get("profilingInterval", 0.5).asDouble();
        config.maxTraceDepth = root.get("maxTraceDepth", 1000).asUInt();
        config.maxMemoryRecords = root.get("maxMemoryRecords", 10000).asUInt();
        config.realTimeAlerts = root.get("realTimeAlerts", true).asBool();
        config.alertThresholdsFile = root.get("alertThresholdsFile", "alert_thresholds.json").asString();
        config.nonIntrusiveMode = root.get("nonIntrusiveMode", true).asBool();
    }
    
    return config;
}

void DebugConfig::saveToConfig(const std::string& filename) const {
    Json::Value root;
    
    root["enableBreakpointDebugging"] = enableBreakpointDebugging;
    root["enableWatchpointDebugging"] = enableWatchpointDebugging;
    root["enableTraceDebugging"] = enableTraceDebugging;
    root["enableMemoryDebugging"] = enableMemoryDebugging;
    root["enableConcurrencyDebugging"] = enableConcurrencyDebugging;
    root["enablePerformanceProfiling"] = enablePerformanceProfiling;
    root["enableSystemMonitoring"] = enableSystemMonitoring;
    root["enableBottleneckAnalysis"] = enableBottleneckAnalysis;
    root["enableOptimizationAnalysis"] = enableOptimizationAnalysis;
    root["enableVisualization"] = enableVisualization;
    root["enableReportGeneration"] = enableReportGeneration;
    
    root["profilingInterval"] = profilingInterval;
    root["maxTraceDepth"] = static_cast<Json::UInt>(maxTraceDepth);
    root["maxMemoryRecords"] = static_cast<Json::UInt>(maxMemoryRecords);
    root["realTimeAlerts"] = realTimeAlerts;
    root["alertThresholdsFile"] = alertThresholdsFile;
    root["nonIntrusiveMode"] = nonIntrusiveMode;
    
    std::ofstream file(filename);
    if (file.is_open()) {
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
        writer->write(root, file);
    }
}

bool DebugConfig::validate() const {
    return true; // Basic validation - all defaults are valid
}

std::string DebugConfig::getValidationErrors() const {
    return "Debug configuration is valid.";
}

// DebugStatistics implementation
std::string DebugStatistics::getStatisticsSummary() const {
    std::ostringstream oss;
    oss << "Debug Statistics:\n";
    oss << "  Breakpoints triggered: " << breakpointsTriggered << "\n";
    oss << "  Watchpoints triggered: " << watchpointsTriggered << "\n";
    oss << "  Trace events recorded: " << traceEventsRecorded << "\n";
    oss << "  Memory allocations: " << memoryAllocations << "\n";
    oss << "  Memory deallocations: " << memoryDeallocations << "\n";
    oss << "  Memory leaks detected: " << memoryLeaksDetected << "\n";
    oss << "  Total CPU time: " << std::fixed << std::setprecision(3) << totalCPUTime << "s\n";
    oss << "  Peak memory usage: " << (peakMemoryUsage / 1024) << " KB\n";
    oss << "  Total memory allocated: " << (totalMemoryAllocated / 1024) << " KB\n";
    oss << "  System alerts: " << systemAlerts << "\n";
    oss << "  Optimization recommendations: " << optimizationRecommendations << "\n";
    oss << "  Average performance impact: " << std::fixed << std::setprecision(3) << averagePerformanceImpact << "%\n";
    
    return oss.str();
}

std::string DebugStatistics::getPerformanceReport() const {
    std::ostringstream oss;
    oss << "Performance Report:\n";
    oss << "  Total CPU time: " << std::fixed << std::setprecision(3) << totalCPUTime << "s\n";
    oss << "  Peak memory usage: " << (peakMemoryUsage / 1024) << " KB\n";
    oss << "  Total memory allocated: " << (totalMemoryAllocated / 1024) << " KB\n";
    oss << "  Average performance impact: " << std::fixed << std::setprecision(3) << averagePerformanceImpact << "%\n";
    oss << "  Debug overhead: " << std::fixed << std::setprecision(1) << (averagePerformanceImpact < 1.0 ? 0.1 : averagePerformanceImpact) << "%\n";
    
    return oss.str();
}

void DebugStatistics::reset() {
    *this = DebugStatistics();
}

// DebugManager implementation
DebugManager::DebugManager(const DebugConfig& config) : config_(config), running_(false) {
    logger_ = &Logger::getGlobal();
    advancedLogger_ = &getAdvancedLogger();
    metrics_ = std::make_shared<Metrics>();
    
    initialize(config_);
}

DebugManager::~DebugManager() {
    shutdown();
}

void DebugManager::initialize(const DebugConfig& config) {
    config_ = config;
    
    // Initialize all components based on configuration
    if (config_.enableBreakpointDebugging) {
        breakpointDebugger_ = std::make_unique<BreakpointDebugger>();
    }
    if (config_.enableWatchpointDebugging) {
        watchpointDebugger_ = std::make_unique<WatchpointDebugger>();
    }
    if (config_.enableTraceDebugging) {
        traceDebugger_ = std::make_unique<TraceDebugger>();
    }
    if (config_.enableMemoryDebugging) {
        memoryDebugger_ = std::make_unique<MemoryDebugger>();
    }
    if (config_.enableConcurrencyDebugging) {
        concurrencyDebugger_ = std::make_unique<ConcurrencyDebugger>();
    }
    
    if (config_.enablePerformanceProfiling) {
        performanceProfiler_ = std::make_unique<PerformanceProfiler>();
    }
    if (config_.enableSystemMonitoring) {
        systemMonitor_ = std::make_unique<SystemMonitor>();
        performanceDashboard_ = std::make_unique<PerformanceDashboard>();
    }
    
    if (config_.enableBottleneckAnalysis) {
        bottleneckAnalyzer_ = std::make_unique<BottleneckAnalyzer>();
    }
    if (config_.enableOptimizationAnalysis) {
        memoryOptimizer_ = std::make_unique<MemoryOptimizer>();
        cpuOptimizer_ = std::make_unique<CPUMOptimizer>();
        algorithmAnalyzer_ = std::make_unique<AlgorithmAnalyzer>();
    }
    
    if (config_.enableVisualization) {
        performanceVisualizer_ = std::make_unique<PerformanceVisualizer>();
        debugVisualizer_ = std::make_unique<DebugVisualizer>();
        architectureVisualizer_ = std::make_unique<ArchitectureVisualizer>();
    }
    
    if (config_.enableReportGeneration) {
        reportGenerator_ = std::make_unique<ReportGenerator>();
    }
    
    initializeDebuggers();
    startEventProcessingThread();
}

void DebugManager::initializeDebuggers() {
    // Connect debuggers to event handlers
    if (breakpointDebugger_) {
        registerEventHandler(DebugEventType::BreakpointTriggered, 
                           [this](const DebugEvent& event) {
                               statistics_.breakpointsTriggered++;
                               advancedLogger_->logPerformance("breakpoints_triggered", statistics_.breakpointsTriggered, "debug", "count");
                           });
    }
    
    if (watchpointDebugger_) {
        registerEventHandler(DebugEventType::WatchpointTriggered,
                           [this](const DebugEvent& event) {
                               statistics_.watchpointsTriggered++;
                               advancedLogger_->logPerformance("watchpoints_triggered", statistics_.watchpointsTriggered, "debug", "count");
                           });
    }
    
    if (traceDebugger_) {
        registerEventHandler(DebugEventType::TraceEvent,
                           [this](const DebugEvent& event) {
                               statistics_.traceEventsRecorded++;
                           });
    }
    
    if (memoryDebugger_) {
        registerEventHandler(DebugEventType::MemoryAllocation,
                           [this](const DebugEvent& event) {
                               statistics_.memoryAllocations++;
                               statistics_.totalMemoryAllocated += std::stoull(event.data["size"]);
                               if (statistics_.totalMemoryAllocated > statistics_.peakMemoryUsage) {
                                   statistics_.peakMemoryUsage = statistics_.totalMemoryAllocated;
                               }
                           });
        
        registerEventHandler(DebugEventType::MemoryDeallocation,
                           [this](const DebugEvent& event) {
                               statistics_.memoryDeallocations++;
                           });
        
        registerEventHandler(DebugEventType::MemoryLeakDetected,
                           [this](const DebugEvent& event) {
                               statistics_.memoryLeaksDetected++;
                           });
    }
}

void DebugManager::startEventProcessingThread() {
    if (!running_) {
        running_ = true;
        eventProcessingThread_ = std::thread(&DebugManager::eventProcessingWorker, this);
        if (systemMonitor_) {
            monitoringThread_ = std::thread(&DebugManager::monitoringWorker, this);
        }
        if (performanceProfiler_) {
            profilingThread_ = std::thread(&DebugManager::profilingWorker, this);
        }
    }
}

void DebugManager::logEvent(const DebugEvent& event) {
    std::lock_guard<std::mutex> lock(eventMutex_);
    eventQueue_.push_back(event);
    
    // Log event through advanced logger
    Json::Value data;
    for (const auto& pair : event.data) {
        data[pair.first] = pair.second;
    }
    
    if (event.errorId.empty()) {
        advancedLogger_->logInfo(event.message, event.component, event.source, data);
    } else {
        // For errors, we could create a mock Error object and log it
        // For now, log as error
        advancedLogger_->logWarning("[DEBUG] " + event.message, event.component, event.source, data);
    }
    
    // Handle event immediately for critical events
    if (event.type == DebugEventType::BreakpointTriggered || 
        event.type == DebugEventType::MemoryLeakDetected) {
        handleEvent(event);
    }
}

void DebugManager::registerEventHandler(DebugEventType type, std::function<void(const DebugEvent&)> handler) {
    std::lock_guard<std::mutex> lock(eventMutex_);
    eventHandlers_[type].push_back(handler);
}

void DebugManager::eventProcessingWorker() {
    while (running_) {
        processPendingEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        updateStatistics();
    }
}

void DebugManager::processPendingEvents() {
    std::lock_guard<std::mutex> lock(eventMutex_);
    if (eventQueue_.empty()) return;
    
    // Process events in batches
    std::vector<DebugEvent> batch;
    batch.reserve(std::min<size_t>(10, eventQueue_.size()));
    
    for (size_t i = 0; i < batch.capacity(); ++i) {
        if (!eventQueue_.empty()) {
            batch.push_back(eventQueue_.front());
            eventQueue_.pop_front();
        }
    }
    
    for (const auto& event : batch) {
        handleEvent(event);
    }
}

void DebugManager::handleEvent(const DebugEvent& event) {
    auto it = eventHandlers_.find(event.type);
    if (it != eventHandlers_.end()) {
        for (const auto& handler : it->second) {
            try {
                handler(event);
            } catch (const std::exception& e) {
                advancedLogger_->logError("Exception in event handler: " + std::string(e.what()));
            }
        }
    }
}

void DebugManager::monitoringWorker() {
    while (running_ && systemMonitor_) {
        // Update system metrics
        auto metrics = systemMonitor_->getCurrentMetrics();
        
        // Log performance metrics
        advancedLogger_->logPerformance("cpu_usage", metrics.cpuUsage, "monitoring", "%");
        advancedLogger_->logPerformance("memory_usage_mb", metrics.memoryUsageMB, "monitoring", "MB");
        advancedLogger_->logPerformance("memory_available_mb", metrics.memoryAvailableMB, "monitoring", "MB");
        
        // Check for alerts
        if (config_.realTimeAlerts) {
            checkForAlerts(metrics);
        }
        
        std::this_thread::sleep_for(std::chrono::duration<double>(config_.profilingInterval));
    }
}

void DebugManager::profilingWorker() {
    while (running_ && performanceProfiler_) {
        // Update performance statistics
        statistics_.totalCPUTime += performanceProfiler_->getCPUTime();
        
        // Record profiling metrics
        performanceProfiler_->recordMetric("debug_overhead", statistics_.averagePerformanceImpact);
        
        std::this_thread::sleep_for(std::chrono::duration<double>(config_.profilingInterval));
    }
}

void DebugManager::updateStatistics() {
    // Calculate average performance impact based on metrics
    if (metrics_) {
        auto names = metrics_->getMetricNames();
        double totalImpact = 0.0;
        size_t count = 0;
        
        for (const auto& name : names) {
            if (name.find("debug") != std::string::npos || name.find("overhead") != std::string::npos) {
                totalImpact += metrics_->getMean(name);
                count++;
            }
        }
        
        if (count > 0) {
            statistics_.averagePerformanceImpact = totalImpact / count;
        }
    }
}

void DebugManager::checkForAlerts(const SystemMonitor::SystemMetrics& metrics) {
    // Simple alert checking - can be enhanced with thresholds from config
    if (metrics.cpuUsage > 90.0) {
        DebugEvent alert(DebugEventType::SystemAlert, "system_monitor", "cpu_high",
                        "High CPU usage detected: " + std::to_string(metrics.cpuUsage) + "%");
        alert.data["severity"] = "high";
        alert.data["threshold"] = "90%";
        alert.data["current"] = std::to_string(metrics.cpuUsage) + "%";
        logEvent(alert);
        statistics_.systemAlerts++;
    }
    
    if (metrics.memoryUsageMB > 8192) { // 8GB threshold
        DebugEvent alert(DebugEventType::SystemAlert, "system_monitor", "memory_high",
                        "High memory usage detected: " + std::to_string(metrics.memoryUsageMB) + " MB");
        alert.data["severity"] = "high";
        alert.data["threshold"] = "8192 MB";
        alert.data["current"] = std::to_string(metrics.memoryUsageMB) + " MB";
        logEvent(alert);
        statistics_.systemAlerts++;
    }
}

void DebugManager::startAllDebuggers() {
    if (breakpointDebugger_) breakpointDebugger_->start();
    if (watchpointDebugger_) watchpointDebugger_->start();
    if (traceDebugger_) traceDebugger_->start();
    if (memoryDebugger_) memoryDebugger_->start();
    if (concurrencyDebugger_) concurrencyDebugger_->start();
}

void DebugManager::stopAllDebuggers() {
    if (breakpointDebugger_) breakpointDebugger_->stop();
    if (watchpointDebugger_) watchpointDebugger_->stop();
    if (traceDebugger_) traceDebugger_->stop();
    if (memoryDebugger_) memoryDebugger_->stop();
    if (concurrencyDebugger_) concurrencyDebugger_->stop();
}

void DebugManager::pauseAllDebuggers() {
    if (breakpointDebugger_) breakpointDebugger_->pause();
    if (watchpointDebugger_) watchpointDebugger_->pause();
    if (traceDebugger_) traceDebugger_->pause();
    if (memoryDebugger_) memoryDebugger_->pause();
    if (concurrencyDebugger_) concurrencyDebugger_->pause();
}

void DebugManager::resumeAllDebuggers() {
    if (breakpointDebugger_) breakpointDebugger_->resume();
    if (watchpointDebugger_) watchpointDebugger_->resume();
    if (traceDebugger_) traceDebugger_->resume();
    if (memoryDebugger_) memoryDebugger_->resume();
    if (concurrencyDebugger_) concurrencyDebugger_->resume();
}

void DebugManager::startProfiling(const std::string& operation) {
    if (performanceProfiler_) {
        performanceProfiler_->startProfile(operation);
    }
}

void DebugManager::stopProfiling(const std::string& operation) {
    if (performanceProfiler_) {
        performanceProfiler_->endProfile(operation);
        statistics_.totalCPUTime += performanceProfiler_->getCPUTime();
    }
}

void DebugManager::startSystemMonitoring() {
    if (systemMonitor_) {
        systemMonitor_->startMonitoring();
    }
}

void DebugManager::stopSystemMonitoring() {
    if (systemMonitor_) {
        systemMonitor_->stopMonitoring();
    }
}

std::string DebugManager::createDebugSession(const std::string& name) {
    std::string sessionId = "debug_session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    activeSessions_[sessionId] = std::chrono::system_clock::now();
    
    // Log session creation
    DebugEvent event(DebugEventType::TraceEvent, "debug_manager", "debug_session_created",
                    "Debug session created: " + name);
    event.data["session_id"] = sessionId;
    event.data["name"] = name;
    logEvent(event);
    
    return sessionId;
}

void DebugManager::endDebugSession(const std::string& sessionId) {
    auto it = activeSessions_.find(sessionId);
    if (it != activeSessions_.end()) {
        // Log session end
        DebugEvent event(DebugEventType::TraceEvent, "debug_manager", "debug_session_ended",
                        "Debug session ended: " + sessionId);
        event.data["session_id"] = sessionId;
        logEvent(event);
        
        activeSessions_.erase(it);
    }
}

void DebugManager::snapshotDebugState(const std::string& sessionId) {
    auto it = activeSessions_.find(sessionId);
    if (it != activeSessions_.end()) {
        DebugEvent event(DebugEventType::TraceEvent, "debug_manager", "debug_snapshot",
                        "Debug state snapshot for session: " + sessionId);
        event.data["session_id"] = sessionId;
        event.data["statistics"] = statistics_.getStatisticsSummary();
        logEvent(event);
    }
}

std::string DebugManager::getDebugSessionReport(const std::string& sessionId, const std::string& format) {
    if (format == "text") {
        std::ostringstream oss;
        oss << "Debug Session Report: " << sessionId << "\n";
        oss << "Created: " << std::chrono::system_clock::to_time_t(activeSessions_[sessionId]) << "\n";
        oss << statistics_.getStatisticsSummary() << "\n";
        return oss.str();
    } else if (format == "json") {
        // Generate JSON report
        Json::Value root;
        root["sessionId"] = sessionId;
        root["created"] = std::chrono::system_clock::to_time_t(activeSessions_[sessionId]);
        root["statistics"] = statistics_.getStatisticsSummary();
        
        Json::StreamWriterBuilder writerBuilder;
        writerBuilder["indentation"] = "  ";
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
        std::ostringstream oss;
        writer->write(root, oss);
        return oss.str();
    }
    
    return "Unsupported format: " + format;
}

bool DebugManager::exportDebugData(const std::string& filename, const std::string& format) {
    if (format == "json") {
        Json::Value root;
        root["debug_events"] = Json::Value(Json::arrayValue);
        root["statistics"] = statistics_.getStatisticsSummary();
        
        // Export event queue (simplified - would need full implementation)
        // For now, just export statistics
        std::ofstream file(filename);
        if (file.is_open()) {
            Json::StreamWriterBuilder writerBuilder;
            writerBuilder["indentation"] = "  ";
            std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
            writer->write(root, file);
            return true;
        }
    } else if (format == "csv") {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "Metric,Value\n";
            file << "breakpoints_triggered," << statistics_.breakpointsTriggered << "\n";
            file << "watchpoints_triggered," << statistics_.watchpointsTriggered << "\n";
            file << "trace_events_recorded," << statistics_.traceEventsRecorded << "\n";
            file << "cpu_time," << statistics_.totalCPUTime << "\n";
            return true;
        }
    }
    
    return false;
}

bool DebugManager::exportPerformanceData(const std::string& filename, const std::string& format) {
    if (performanceProfiler_) {
        return performanceProfiler_->exportData(filename, format);
    }
    return false;
}

bool DebugManager::exportDebugEvents(const std::string& filename, const std::string& format) {
    std::lock_guard<std::mutex> lock(eventMutex_);
    
    if (format == "xml") {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "<?xml version=\"1.0\"?>\n";
            file << "<debug_events>\n";
            for (const auto& event : eventQueue_) {
                file << "  <event type=\"" << static_cast<int>(event.type) << "\" ";
                file << "component=\"" << event.component << "\" ";
                file << "message=\"" << event.message << "\"/>\n";
            }
            file << "</debug_events>\n";
            return true;
        }
    }
    
    return false;
}

std::string DebugManager::generateComprehensiveReport() const {
    std::ostringstream oss;
    oss << "=== NLM DEBUGGING AND PROFILING COMPREHENSIVE REPORT ===\n\n";
    oss << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "\n\n";
    oss << statistics_.getStatisticsSummary() << "\n";
    
    if (performanceProfiler_) {
        oss << "\n" << performanceProfiler_->getPerformanceReport();
    }
    
    oss << "\n=== END OF REPORT ===\n";
    return oss.str();
}

std::string DebugManager::generatePerformanceReport() const {
    return statistics_.getPerformanceReport();
}

std::string DebugManager::generateDebugReport() const {
    std::ostringstream oss;
    oss << "Debug Activity Report\n";
    oss << "=====================\n\n";
    oss << "Active Components:\n";
    
    // List active components
    std::vector<std::string> components;
    if (breakpointDebugger_) components.push_back("BreakpointDebugger");
    if (watchpointDebugger_) components.push_back("WatchpointDebugger");
    if (traceDebugger_) components.push_back("TraceDebugger");
    if (memoryDebugger_) components.push_back("MemoryDebugger");
    if (concurrencyDebugger_) components.push_back("ConcurrencyDebugger");
    if (performanceProfiler_) components.push_back("PerformanceProfiler");
    if (systemMonitor_) components.push_back("SystemMonitor");
    
    for (const auto& component : components) {
        oss << "  - " << component << "\n";
    }
    
    oss << "\nDebug Statistics:\n";
    oss << statistics_.getStatisticsSummary();
    
    return oss.str();
}

bool DebugManager::loadDebugConfig(const std::string& filename) {
    DebugConfig newConfig = DebugConfig::loadFromConfig(filename);
    if (newConfig.validate()) {
        updateConfig(newConfig);
        return true;
    }
    return false;
}

bool DebugManager::saveDebugConfig(const std::string& filename) const {
    config_.saveToConfig(filename);
    return true;
}

bool DebugManager::validateDebugConfig() const {
    return config_.validate();
}

void DebugManager::attachToBrain(const std::string& brainId) {
    // Attach debuggers to brain
    DebugEvent event(DebugEventType::TraceEvent, "debug_manager", "brain_attached",
                    "Debug attached to brain: " + brainId);
    event.data["brain_id"] = brainId;
    logEvent(event);
}

void DebugManager::attachToExperiment(const std::string& experimentName) {
    DebugEvent event(DebugEventType::TraceEvent, "debug_manager", "experiment_attached",
                    "Debug attached to experiment: " + experimentName);
    event.data["experiment_name"] = experimentName;
    logEvent(event);
}

void DebugManager::attachToSimulation(const std::string& simulationId) {
    DebugEvent event(DebugEventType::TraceEvent, "debug_manager", "simulation_attached",
                    "Debug attached to simulation: " + simulationId);
    event.data["simulation_id"] = simulationId;
    logEvent(event);
}

std::string DebugManager::getStatus() const {
    std::ostringstream oss;
    oss << "DebugManager Status:\n";
    oss << "  Running: " << (running_ ? "Yes" : "No") << "\n";
    oss << "  Active Sessions: " << activeSessions_.size() << "\n";
    oss << "  Event Queue Size: " << eventQueue_.size() << "\n";
    oss << "  Components: " << getActiveComponents().size() << "\n";
    return oss.str();
}

bool DebugManager::isHealthy() const {
    return running_ && !eventQueue_.empty();
}

std::vector<std::string> DebugManager::getActiveComponents() const {
    std::vector<std::string> components;
    if (breakpointDebugger_) components.push_back("BreakpointDebugger");
    if (watchpointDebugger_) components.push_back("WatchpointDebugger");
    if (traceDebugger_) components.push_back("TraceDebugger");
    if (memoryDebugger_) components.push_back("MemoryDebugger");
    if (concurrencyDebugger_) components.push_back("ConcurrencyDebugger");
    if (performanceProfiler_) components.push_back("PerformanceProfiler");
    if (systemMonitor_) components.push_back("SystemMonitor");
    if (performanceDashboard_) components.push_back("PerformanceDashboard");
    if (bottleneckAnalyzer_) components.push_back("BottleneckAnalyzer");
    if (memoryOptimizer_) components.push_back("MemoryOptimizer");
    if (cpuOptimizer_) components.push_back("CPUMOptimizer");
    if (algorithmAnalyzer_) components.push_back("AlgorithmAnalyzer");
    
    return components;
}

void DebugManager::flushEvents() {
    std::lock_guard<std::mutex> lock(eventMutex_);
    eventQueue_.clear();
}

void DebugManager::shutdown() {
    running_ = false;
    
    // Stop all threads
    if (eventProcessingThread_.joinable()) {
        eventProcessingThread_.join();
    }
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
    if (profilingThread_.joinable()) {
        profilingThread_.join();
    }
    
    // Stop all debuggers
    stopAllDebuggers();
    
    // Flush events
    flushEvents();
}

DebugManager& DebugManager::getInstance(const DebugConfig& config) {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    if (!instance_) {
        instance_ = std::make_unique<DebugManager>(config);
    }
    return *instance_;
}

} // namespace nlm
