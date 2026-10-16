#include "PerformanceMonitor/PerformanceMonitor.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <thread>
#include <chrono>

namespace nlm {

// Helper functions for JSON serialization
namespace {
    std::string escapeJSON(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '\"': result += "\\\""; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                default:
                    if (c < 32 || c == 127) {
                        result += "\\u";
                        char hex[9];
                        snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned int>(c));
                        result += hex;
                    } else {
                        result += c;
                    }
            }
        }
        return result;
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string categoryToString(PerformanceCategory category) {
        switch (category) {
            case PerformanceCategory::General: return "general";
            case PerformanceCategory::Neural: return "neural";
            case PerformanceCategory::Memory: return "memory";
            case PerformanceCategory::CPU: return "cpu";
            case PerformanceCategory::I/O: return "io";
            case PerformanceCategory::Network: return "network";
            case PerformanceCategory::Plasticity: return "plasticity";
            case PerformanceCategory::Simulation: return "simulation";
            case PerformanceCategory::Visualization: return "visualization";
            case PerformanceCategory::Optimization: return "optimization";
            case PerformanceCategory::Custom: return "custom";
            default: return "unknown";
        }
    }
    
    std::string alertLevelToString(PerformanceAlertLevel level) {
        switch (level) {
            case PerformanceAlertLevel::Info: return "info";
            case PerformanceAlertLevel::Warning: return "warning";
            case PerformanceAlertLevel::Critical: return "critical";
            case PerformanceAlertLevel::Error: return "error";
            default: return "info";
        }
    }
}

PerformanceReport PerformanceReport::toJSON() const {
    PerformanceReport report = *this;
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"startTime\": \"" << std::chrono::system_clock::to_time_t(startTime) << "\",\n";
    oss << "  \"endTime\": \"" << std::chrono::system_clock::to_time_t(endTime) << "\",\n";
    oss << "  \"duration\": " << duration << ",\n";
    oss << "  \"totalSteps\": " << totalSteps << ",\n";
    oss << "  \"totalSpikes\": " << totalSpikes << ",\n";
    
    oss << "  \"metrics\": {\n";
    bool firstMetric = true;
    for (const auto& pair : metrics) {
        if (!firstMetric) oss << ",\n";
        firstMetric = false;
        oss << "    \"" << escapeJSON(pair.first) << "\": " << pair.second;
    }
    oss << "\n  },\n";
    
    oss << "  \"alerts\": [\n";
    for (size_t i = 0; i < alerts.size(); ++i) {
        const auto& alert = alerts[i];
        oss << "    {\n";
        oss << "      \"level\": \"" << alertLevelToString(alert.level) << "\",\n";
        oss << "      \"message\": \"" << escapeJSON(alert.message) << "\",\n";
        oss << "      \"metricName\": \"" << escapeJSON(alert.metricName) << "\",\n";
        oss << "      \"threshold\": " << alert.threshold << ",\n";
        oss << "      \"actualValue\": " << alert.actualValue << ",\n";
        oss << "      \"timestamp\": \"" << std::chrono::system_clock::to_time_t(alert.timestamp) << "\",\n";
        oss << "      \"operation\": \"" << escapeJSON(alert.operation) << "\"\n";
        if (i < alerts.size() - 1) oss << "    },\n";
        else oss << "    }\n";
    }
    oss << "  ],\n";
    
    oss << "  \"metricHistory\": [\n";
    for (size_t i = 0; i < metricHistory.size(); ++i) {
        const auto& metric = metricHistory[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << escapeJSON(metric.name) << "\",\n";
        oss << "      \"value\": " << metric.value << ",\n";
        oss << "      \"unit\": \"" << escapeJSON(metric.unit) << "\",\n";
        oss << "      \"category\": \"" << categoryToString(metric.category) << "\",\n";
        oss << "      \"timestamp\": \"" << std::chrono::system_clock::to_time_t(metric.timestamp) << "\",\n";
        if (i < metricHistory.size() - 1) oss << "    },\n";
        else oss << "    }\n";
    }
    oss << "  ],\n";
    
    oss << "  \"summary\": \"" << escapeJSON(summary) << "\"\n";
    
    oss << "}\n";
    return report;
}

PerformanceReport PerformanceReport::toCSV() const {
    PerformanceReport report = *this;
    std::ostringstream oss;
    
    oss << "Start Time,End Time,Duration (s),Total Steps,Total Spikes\n";
    oss << std::chrono::system_clock::to_time_t(startTime) << ","
        << std::chrono::system_clock::to_time_t(endTime) << ","
        << duration << ","
        << totalSteps << ","
        << totalSpikes << "\n\n";
    
    oss << "Metrics\n";
    for (const auto& pair : metrics) {
        oss << pair.first << "," << pair.second << "\n";
    }
    oss << "\n";
    
    oss << "Alerts\n";
    for (const auto& alert : alerts) {
        oss << alertLevelToString(alert.level) << "," << alert.message << ","
            << alert.metricName << "," << alert.threshold << ","
            << alert.actualValue << ","
            << std::chrono::system_clock::to_time_t(alert.timestamp) << ","
            << alert.operation << "\n";
    }
    oss << "\n";
    
    oss << "Metric History\n";
    oss << "Name,Value,Unit,Category,Timestamp\n";
    for (const auto& metric : metricHistory) {
        oss << metric.name << "," << metric.value << ","
            << metric.unit << ","
            << categoryToString(metric.category) << ","
            << std::chrono::system_clock::to_time_t(metric.timestamp) << "\n";
    }
    
    return report;
}

PerformanceReport PerformanceReport::toText() const {
    PerformanceReport report = *this;
    std::ostringstream oss;
    
    oss << "Performance Report\n";
    oss << "==================\n\n";
    oss << "Period: " << std::chrono::system_clock::to_time_t(startTime)
        << " to " << std::chrono::system_clock::to_time_t(endTime) << "\n";
    oss << "Duration: " << duration << " seconds\n";
    oss << "Total Steps: " << totalSteps << "\n";
    oss << "Total Spikes: " << totalSpikes << "\n\n";
    
    if (!metrics.empty()) {
        oss << "Key Metrics:\n";
        for (const auto& pair : metrics) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
        oss << "\n";
    }
    
    if (!alerts.empty()) {
        oss << "Performance Alerts:\n";
        for (const auto& alert : alerts) {
            oss << "  [" << alertLevelToString(alert.level) << "] " << alert.message
                << " (metric: " << alert.metricName
                << ", threshold: " << alert.threshold
                << ", actual: " << alert.actualValue << ")\n";
        }
        oss << "\n";
    }
    
    if (!summary.empty()) {
        oss << "Summary:\n";
        oss << "  " << summary << "\n\n";
    }
    
    return report;
}

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()),
    initialized_(false), monitoring_(false), realTimeDisplayActive_(false) {
}

PerformanceMonitor::~PerformanceMonitor() {
    shutdown();
}

void PerformanceMonitor::initialize(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (initialized_) {
        return;
    }
    
    // Parse configuration file
    // For now, use default configuration
    
    // Initialize performance statistics
    totalSpikes_ = 0;
    totalSteps_ = 0;
    totalFiringRate_ = 0.0;
    totalStepDuration_ = 0.0;
    averageFiringRate_ = 0.0;
    averageStepDuration_ = 0.0;
    
    // Initialize performance monitoring
    initialized_ = true;
    
    // Log initialization
    if (logger_) {
        logger_->info("Performance monitoring initialized");
    }
}

void PerformanceMonitor::shutdown() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!initialized_) {
        return;
    }
    
    // Stop monitoring
    monitoring_ = false;
    
    // Stop real-time display
    stopRealTimeDisplay();
    
    // Clear performance data
    metricHistory_.clear();
    neuralMetrics_.clear();
    simulationMetrics_.clear();
    resourceMetrics_.clear();
    
    // Clear performance thresholds
    thresholds_.clear();
    
    // Clear performance statistics
    totalSpikes_ = 0;
    totalSteps_ = 0;
    totalFiringRate_ = 0.0;
    totalStepDuration_ = 0.0;
    averageFiringRate_ = 0.0;
    averageStepDuration_ = 0.0;
    
    // Clear integration objects
    logger_.reset();
    statisticsCollector_.reset();
    realTimeMonitor_.reset();
    logAnalyzer_.reset();
    
    // Reset flags
    initialized_ = false;
    monitoring_ = false;
    realTimeDisplayActive_ = false;
    
    // Log shutdown
    if (logger_) {
        logger_->info("Performance monitoring shutdown");
    }
}

void PerformanceMonitor::recordMetric(const PerformanceMetric& metric) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Record the metric
    recordPerformanceMetric(metric);
    
    // Notify listeners
    notifyMetricListeners(metric);
}

void PerformanceMonitor::recordMetric(const std::string& name, double value,
                                      const std::string& unit,
                                      PerformanceCategory category,
                                      const std::map<std::string, std::string>& metadata) {
    PerformanceMetric metric(name, value, unit, category);
    for (const auto& pair : metadata) {
        metric.metadata[pair.first] = pair.second;
    }
    recordMetric(metric);
}

void PerformanceMonitor::recordSpikeCount(size_t spikeCount, double time) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    totalSpikes_ = spikeCount;
    
    PerformanceMetric metric("spike_count", static_cast<double>(spikeCount),
                            "count", PerformanceCategory::Neural);
    metric.metadata["time"] = std::to_string(time);
    
    recordPerformanceMetric(metric);
}

void PerformanceMonitor::recordFiringRate(double rate, const std::string& population) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    totalFiringRate_ += rate;
    
    PerformanceMetric metric("firing_rate", rate, "Hz", PerformanceCategory::Neural);
    if (!population.empty()) {
        metric.metadata["population"] = population;
    }
    
    recordPerformanceMetric(metric);
}

void PerformanceMonitor::recordActiveNeurons(size_t activeCount, const std::string& population) {
    PerformanceMetric metric("active_neurons", static_cast<double>(activeCount),
                            "count", PerformanceCategory::Neural);
    if (!population.empty()) {
        metric.metadata["population"] = population;
    }
    recordMetric(metric);
}

void PerformanceMonitor::recordExcitationInhibitionRatio(double ratio, const std::string& population) {
    PerformanceMetric metric("excitation_inhibition_ratio", ratio, "ratio",
                            PerformanceCategory::Neural);
    if (!population.empty()) {
        metric.metadata["population"] = population;
    }
    recordMetric(metric);
}

void PerformanceMonitor::recordConnectionDensity(double density, const std::string& population) {
    PerformanceMetric metric("connection_density", density, "ratio",
                            PerformanceCategory::Neural);
    if (!population.empty()) {
        metric.metadata["population"] = population;
    }
    recordMetric(metric);
}

void PerformanceMonitor::recordStepPerformance(double duration, size_t step,
                                               double time, size_t spikeCount) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    totalSteps_ = step;
    totalStepDuration_ += duration;
    
    PerformanceMetric stepMetric("step_duration", duration, "seconds",
                                PerformanceCategory::Simulation);
    stepMetric.metadata["step"] = std::to_string(step);
    stepMetric.metadata["time"] = std::to_string(time);
    stepMetric.metadata["spike_count"] = std::to_string(spikeCount);
    
    recordPerformanceMetric(stepMetric);
}

void PerformanceMonitor::recordOperationPerformance(const std::string& operation,
                                                    double duration,
                                                    PerformanceCategory category,
                                                    uint64_t memoryUsage) {
    PerformanceMetric metric("operation_duration", duration, "seconds", category);
    metric.metadata["operation"] = operation;
    if (memoryUsage > 0) {
        metric.metadata["memory_usage"] = std::to_string(memoryUsage);
    }
    recordMetric(metric);
}

void PerformanceMonitor::recordCPUUsage(double usage) {
    PerformanceMetric metric("cpu_usage", usage, "percentage", PerformanceCategory::CPU);
    recordMetric(metric);
}

void PerformanceMonitor::recordMemoryUsage(uint64_t memoryBytes) {
    PerformanceMetric metric("memory_usage", static_cast<double>(memoryBytes),
                            "bytes", PerformanceCategory::Memory);
    recordMetric(metric);
}

void PerformanceMonitor::recordIOThroughput(double readMBps, double writeMBps) {
    PerformanceMetric readMetric("io_read_throughput", readMBps, "MB/s",
                                PerformanceCategory::I_O);
    readMetric.metadata["direction"] = "read";
    recordMetric(readMetric);
    
    PerformanceMetric writeMetric("io_write_throughput", writeMBps, "MB/s",
                                 PerformanceCategory::I_O);
    writeMetric.metadata["direction"] = "write";
    recordMetric(writeMetric);
}

void PerformanceMonitor::recordNetworkThroughput(double bytesPerSecond) {
    PerformanceMetric metric("network_throughput", bytesPerSecond, "bytes/s",
                            PerformanceCategory::Network);
    recordMetric(metric);
}

void PerformanceMonitor::addThreshold(const PerformanceThreshold& threshold) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    thresholds_.push_back(threshold);
    
    // Check if current value exceeds threshold
    double currentValue = 0.0;
    bool valueExists = false;
    
    for (const auto& metric : metricHistory_) {
        if (metric.name == threshold.metricName) {
            currentValue = metric.value;
            valueExists = true;
            break;
        }
    }
    
    if (valueExists) {
        checkPerformanceThresholds();
    }
}

void PerformanceMonitor::removeThreshold(const std::string& metricName) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = std::remove_if(thresholds_.begin(), thresholds_.end(),
                           [&metricName](const PerformanceThreshold& t) {
                               return t.metricName == metricName;
                           });
    thresholds_.erase(it, thresholds_.end());
}

void PerformanceMonitor::setAlertCallback(AlertCallback callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    alertCallback_ = std::move(callback);
}

void PerformanceMonitor::setMetricCallback(MetricCallback callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    metricCallback_ = std::move(callback);
}

void PerformanceMonitor::setReportCallback(ReportCallback callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    reportCallback_ = std::move(callback);
}

void PerformanceMonitor::recordPerformanceMetric(const PerformanceMetric& metric) {
    // Add to metric history
    metricHistory_.push_back(metric);
    
    // Keep history size manageable
    if (metricHistory_.size() > maxHistorySize_) {
        metricHistory_.pop_front();
    }
    
    // Update category-specific history
    switch (metric.category) {
        case PerformanceCategory::Neural:
            neuralMetrics_.push_back(metric);
            if (metric.name == "firing_rate") {
                averageFiringRate_ = totalFiringRate_ / totalSteps_;
            }
            break;
        case PerformanceCategory::Simulation:
            simulationMetrics_.push_back(metric);
            if (metric.name == "step_duration") {
                averageStepDuration_ = totalStepDuration_ / totalSteps_;
            }
            break;
        case PerformanceCategory::CPU:
        case PerformanceCategory::Memory:
        case PerformanceCategory::I_O:
        case PerformanceCategory::Network:
            resourceMetrics_.push_back(metric);
            break;
        default:
            // General category metrics are stored in main history
            break;
    }
}

void PerformanceMonitor::notifyMetricListeners(const PerformanceMetric& metric) {
    if (metricCallback_) {
        metricCallback_(metric);
    }
    
    // Also notify via logger if available
    if (logger_) {
        logger_->debug("Performance metric: " + metric.name + " = " + std::to_string(metric.value) +
                      " " + metric.unit);
    }
}

void PerformanceMonitor::notifyAlertListeners(const PerformanceAlert& alert) {
    if (alertCallback_) {
        alertCallback_(alert);
    }
    
    // Also notify via logger if available
    if (logger_) {
        std::string levelStr = alertLevelToString(alert.level);
        logger_->log(static_cast<LogLevel>(alert.level + 1), // Map to logger level
                    "Performance alert: " + alert.message);
    }
}

void PerformanceMonitor::notifyReportListeners(const PerformanceReport& report) {
    if (reportCallback_) {
        reportCallback_(report);
    }
}

void PerformanceMonitor::updatePerformanceStatistics() {
    // Update moving averages based on recent metrics
    if (metricHistory_.size() >= 10) {
        // Calculate average firing rate from recent neural metrics
        double totalFiringRate = 0.0;
        size_t firingRateCount = 0;
        
        auto recentEnd = metricHistory_.end();
        auto recentStart = recentEnd;
        std::advance(recentStart, std::max<size_t>(0, 
                         static_cast<ptrdiff_t>(metricHistory_.size()) - 100));
        
        for (auto it = recentStart; it != recentEnd; ++it) {
            if (it->name == "firing_rate") {
                totalFiringRate += it->value;
                firingRateCount++;
            } else if (it->name == "step_duration") {
                totalStepDuration_ += it->value;
            }
        }
        
        if (firingRateCount > 0) {
            averageFiringRate_ = totalFiringRate / firingRateCount;
        }
        
        if (totalSteps_ > 0) {
            averageStepDuration_ = totalStepDuration_ / totalSteps_;
        }
    }
}

void PerformanceMonitor::checkPerformanceThresholds() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!enablePerformanceAlerts_) {
        return;
    }
    
    // Check each threshold
    for (const auto& threshold : thresholds_) {
        double currentValue = 0.0;
        bool valueExists = false;
        
        // Find the latest value for this metric
        for (auto it = metricHistory_.rbegin(); it != metricHistory_.rend(); ++it) {
            if (it->name == threshold.metricName) {
                currentValue = it->value;
                valueExists = true;
                break;
            }
        }
        
        if (valueExists && threshold.condition(currentValue)) {
            // Generate performance alert
            std::string message = "Performance threshold exceeded: " + threshold.metricName +
                                " = " + std::to_string(currentValue) +
                                " (threshold: " + std::to_string(threshold.warningLevel) +
                                "/" + std::to_string(threshold.criticalLevel) + ")";
            
            PerformanceAlertLevel level = PerformanceAlertLevel::Warning;
            if (currentValue >= threshold.criticalLevel) {
                level = PerformanceAlertLevel::Critical;
            } else if (currentValue <= threshold.warningLevel) {
                level = PerformanceAlertLevel::Warning;
            }
            
            PerformanceAlert alert(level, message, threshold.metricName,
                                  threshold.warningLevel, currentValue);
            notifyAlertListeners(alert);
        }
    }
}

void PerformanceMonitor::startRealTimeDisplay(double refreshInterval) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (realTimeDisplayActive_) {
        return;
    }
    
    realTimeDisplayActive_ = true;
    
    // Start real-time display thread
    realTimeDisplayThread_ = std::thread([this, refreshInterval]() {
        generateRealTimeDisplay();
    });
    
    // Set thread to low priority
    if (realTimeDisplayThread_.joinable()) {
        realTimeDisplayThread_.detach();
    }
}

void PerformanceMonitor::stopRealTimeDisplay() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!realTimeDisplayActive_) {
        return;
    }
    
    realTimeDisplayActive_ = false;
    
    // Wait for thread to finish
    if (realTimeDisplayThread_.joinable()) {
        realTimeDisplayThread_.join();
    }
}

void PerformanceMonitor::generateRealTimeDisplay() {
    while (realTimeDisplayActive_) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        if (logger_) {
            // Display key performance metrics
            std::ostringstream oss;
            oss << "=== Performance Monitor ===" << std::endl;
            oss << "Total Steps: " << totalSteps_ << std::endl;
            oss << "Total Spikes: " << totalSpikes_ << std::endl;
            oss << "Average Firing Rate: " << averageFiringRate_ << " Hz" << std::endl;
            oss << "Average Step Duration: " << averageStepDuration_ << " s" << std::endl;
            oss << "Active Metrics: " << metricHistory_.size() << std::endl;
            oss << "==================================" << std::endl;
            
            if (realTimeMonitor_) {
                realTimeMonitor_->addCustomMetric("perf_steps", totalSteps_, "count");
                realTimeMonitor_->addCustomMetric("perf_spikes", totalSpikes_, "count");
                realTimeMonitor_->addCustomMetric("perf_firing_rate", averageFiringRate_, "Hz");
                realTimeMonitor_->addCustomMetric("perf_step_duration", averageStepDuration_, "s");
            }
        }
        
        // Sleep for refresh interval
        std::this_thread::sleep_for(std::chrono::duration<double>(samplingInterval_));
    }
}

void PerformanceMonitor::integrateWithLogger(std::shared_ptr<Logger> logger) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    logger_ = logger;
    
    if (logger_) {
        logger_->info("Performance monitoring integrated with Logger");
    }
}

void PerformanceMonitor::integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    statisticsCollector_ = collector;
    
    if (statisticsCollector_) {
        // Log integration
        if (logger_) {
            logger_->info("Performance monitoring integrated with StatisticsCollector");
        }
    }
}

void PerformanceMonitor::integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    realTimeMonitor_ = monitor;
    
    if (realTimeMonitor_) {
        if (logger_) {
            logger_->info("Performance monitoring integrated with RealTimeMonitor");
        }
    }
}

void PerformanceMonitor::integrateWithLogAnalyzer(std::shared_ptr<LogAnalyzer> analyzer) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    logAnalyzer_ = analyzer;
    
    if (logAnalyzer_) {
        if (logger_) {
            logger_->info("Performance monitoring integrated with LogAnalyzer");
        }
    }
}

void PerformanceMonitor::onStepStart(size_t step, double time) {
    if (logger_) {
        logger_->debug("Performance: Step " + std::to_string(step) + " started at time " +
                      std::to_string(time));
    }
    
    if (statisticsCollector_) {
        statisticsCollector_->recordSimulationStep(0.0, step); // duration = 0, step recorded
    }
}

void PerformanceMonitor::onStepEnd(size_t step, double time, double duration, size_t spikeCount) {
    // Record step performance
    recordStepPerformance(duration, step, time, spikeCount);
    
    // Update statistics collector
    if (statisticsCollector_) {
        statisticsCollector_->recordSimulationStep(duration, step);
        statisticsCollector_->recordSpike(spikeCount, time);
    }
    
    // Log step completion
    if (logger_) {
        logger_->debug("Performance: Step " + std::to_string(step) + " completed in " +
                      std::to_string(duration) + " s, " +
                      std::to_string(spikeCount) + " spikes at time " + std::to_string(time));
    }
}

void PerformanceMonitor::onOperationStart(const std::string& operation, PerformanceCategory category) {
    // Start timing for operation
    if (logger_) {
        logger_->debug("Performance: Operation started - " + operation);
    }
    
    if (realTimeMonitor_) {
        realTimeMonitor_->startProfile(operation, categoryToString(category));
    }
}

void PerformanceMonitor::onOperationEnd(const std::string& operation, double duration, uint64_t memoryUsage) {
    // Record operation performance
    recordOperationPerformance(operation, duration);
    
    // Stop timing and log completion
    if (logger_) {
        logger_->debug("Performance: Operation completed - " + operation +
                      " in " + std::to_string(duration) + " s");
    }
    
    if (realTimeMonitor_) {
        realTimeMonitor_->endProfile(operation);
    }
}

// Integration methods for different simulation modes
void PerformanceMonitor::monitorBasicConnectivityMode() {
    recordMetric("mode", 1.0, "", PerformanceCategory::General);
    recordMetric("connectivity_test", 1.0, "", PerformanceCategory::Neural);
    
    if (logger_) {
        logger_->info("Performance monitoring: Basic connectivity mode");
    }
}

void PerformanceMonitor::monitorPlasticityExperimentMode() {
    recordMetric("mode", 2.0, "", PerformanceCategory::General);
    recordMetric("plasticity_experiment", 1.0, "", PerformanceCategory::Plasticity);
    
    if (logger_) {
        logger_->info("Performance monitoring: Plasticity experiment mode");
    }
}

void PerformanceMonitor::monitorStdpVerificationMode() {
    recordMetric("mode", 3.0, "", PerformanceCategory::General);
    recordMetric("stdp_verification", 1.0, "", PerformanceCategory::Plasticity);
    
    if (logger_) {
        logger_->info("Performance monitoring: STDP verification mode");
    }
}

void PerformanceMonitor::monitorBenchmarkMode() {
    recordMetric("mode", 4.0, "", PerformanceCategory::General);
    recordMetric("benchmark_mode", 1.0, "", PerformanceCategory::Optimization);
    
    if (logger_) {
        logger_->info("Performance monitoring: Benchmark mode");
    }
}

void PerformanceMonitor::monitorContinuousMode() {
    recordMetric("mode", 5.0, "", PerformanceCategory::General);
    recordMetric("continuous_mode", 1.0, "", PerformanceCategory::Simulation);
    
    if (logger_) {
        logger_->info("Performance monitoring: Continuous mode");
    }
}

void PerformanceMonitor::monitorRuntimeConfigDemoMode() {
    recordMetric("mode", 6.0, "", PerformanceCategory::General);
    recordMetric("runtime_config_demo", 1.0, "", PerformanceCategory::General);
    
    if (logger_) {
        logger_->info("Performance monitoring: Runtime config demo mode");
    }
}

void PerformanceMonitor::monitorRuntimeConfigCommandInterfaceMode() {
    recordMetric("mode", 7.0, "", PerformanceCategory::General);
    recordMetric("runtime_config_cli", 1.0, "", PerformanceCategory::General);
    
    if (logger_) {
        logger_->info("Performance monitoring: Runtime config CLI mode");
    }
}

void PerformanceMonitor::monitorRuntimeConfigTestMode() {
    recordMetric("mode", 8.0, "", PerformanceCategory::General);
    recordMetric("runtime_config_test", 1.0, "", PerformanceCategory::General);
    
    if (logger_) {
        logger_->info("Performance monitoring: Runtime config test mode");
    }
}

void PerformanceMonitor::monitorDemoMode() {
    recordMetric("mode", 9.0, "", PerformanceCategory::General);
    recordMetric("demo_mode", 1.0, "", PerformanceCategory::Visualization);
    
    if (logger_) {
        logger_->info("Performance monitoring: Demo mode");
    }
}

void PerformanceMonitor::monitorContinuousVisualizationMode() {
    recordMetric("mode", 10.0, "", PerformanceCategory::General);
    recordMetric("continuous_visualization", 1.0, "", PerformanceCategory::Visualization);
    
    if (logger_) {
        logger_->info("Performance monitoring: Continuous visualization mode");
    }
}

void PerformanceMonitor::monitorPythonInterfaceMode() {
    recordMetric("mode", 11.0, "", PerformanceCategory::General);
    recordMetric("python_interface", 1.0, "", PerformanceCategory::General);
    
    if (logger_) {
        logger_->info("Performance monitoring: Python interface mode");
    }
}

void PerformanceMonitor::monitorScalingBenchmarkMode() {
    recordMetric("mode", 12.0, "", PerformanceCategory::General);
    recordMetric("scaling_benchmark", 1.0, "", PerformanceCategory::Optimization);
    
    if (logger_) {
        logger_->info("Performance monitoring: Scaling benchmark mode");
    }
}

void PerformanceMonitor::monitorResultsSaveMode(const std::string& filepath) {
    recordMetric("mode", 13.0, "", PerformanceCategory::General);
    recordMetric("results_save", 1.0, "", PerformanceCategory::General);
    
    if (logger_) {
        logger_->info("Performance monitoring: Results save mode to " + filepath);
    }
}

// Performance query implementation

double PerformanceMonitor::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    for (auto it = metricHistory_.rbegin(); it != metricHistory_.rend(); ++it) {
        if (it->name == name) {
            return it->value;
        }
    }
    return 0.0;
}

PerformanceMetric PerformanceMonitor::getLatestMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    for (auto it = metricHistory_.rbegin(); it != metricHistory_.rend(); ++it) {
        if (it->name == name) {
            return *it;
        }
    }
    return PerformanceMetric();
}

std::vector<PerformanceMetric> PerformanceMonitor::getMetrics(const std::string& name,
                                                               size_t count) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<PerformanceMetric> result;
    result.reserve(std::min(metricHistory_.size(), count));
    
    auto start = metricHistory_.rbegin();
    auto end = metricHistory_.rend();
    
    for (auto it = start; it != end && result.size() < count; ++it) {
        if (it->name == name) {
            result.push_back(*it);
        }
    }
    
    return result;
}

std::vector<PerformanceMetric> PerformanceMonitor::getMetricsByCategory(PerformanceCategory category,
                                                                        size_t count) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<PerformanceMetric> result;
    
    auto start = metricHistory_.rbegin();
    auto end = metricHistory_.rend();
    
    for (auto it = start; it != end && result.size() < count; ++it) {
        if (it->category == category) {
            result.push_back(*it);
        }
    }
    
    return result;
}

PerformanceReport PerformanceMonitor::generateReport() const {
    PerformanceReport report;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Set report time information
    report.startTime = std::chrono::system_clock::now(); // Simplified, should track actual start
    report.endTime = std::chrono::system_clock::now();
    report.duration = 0.1; // Placeholder
    
    // Set neural statistics
    report.totalSteps = totalSteps_;
    report.totalSpikes = totalSpikes_;
    
    // Generate key metrics
    report.metrics["average_firing_rate"] = averageFiringRate_;
    report.metrics["average_step_duration"] = averageStepDuration_;
    report.metrics["total_metrics"] = static_cast<double>(metricHistory_.size());
    report.metrics["neural_metrics"] = static_cast<double>(neuralMetrics_.size());
    report.metrics["simulation_metrics"] = static_cast<double>(simulationMetrics_.size());
    
    // Generate alerts based on thresholds
    report.alerts.clear(); // Would populate based on current thresholds
    
    // Add metric history for recent metrics
    report.metricHistory.clear();
    auto start = metricHistory_.rbegin();
    auto end = metricHistory_.rend();
    auto copyStart = start;
    
    // Copy recent metrics
    for (auto it = start; it != end && report.metricHistory.size() < 100; ++it) {
        report.metricHistory.push_back(*it);
    }
    
    // Generate summary
    std::ostringstream oss;
    oss << "Performance Report: " << metricHistory_.size() << " metrics recorded over "
        << totalSteps_ << " steps, " << totalSpikes_ << " spikes. "
        << "Average firing rate: " << averageFiringRate_ << " Hz, "
        << "average step duration: " << averageStepDuration_ << " s.";
    report.summary = oss.str();
    
    // Set additional data
    report.additionalData["monitoring_enabled"] = monitoring_ ? "true" : "false";
    report.additionalData["real_time_display"] = realTimeDisplayActive_ ? "true" : "false";
    
    return report;
}

PerformanceReport PerformanceMonitor::generateReport(const std::string& startTime,
                                                     const std::string& endTime) const {
    // This would filter metrics by time, simplified implementation
    return generateReport();
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath, const std::string& format) const {
    try {
        if (format == "json") {
            PerformanceReport report = generateReport();
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << report.toJSON();
                file.close();
                return true;
            }
        } else if (format == "csv") {
            PerformanceReport report = generateReport();
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << report.toCSV();
                file.close();
                return true;
            }
        } else if (format == "text") {
            PerformanceReport report = generateReport();
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << report.toText();
                file.close();
                return true;
            }
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Error exporting metrics: " + std::string(e.what()));
        }
    }
    
    return false;
}

bool PerformanceMonitor::importMetrics(const std::string& filepath) {
    try {
        // Simple JSON import implementation
        std::ifstream file(filepath);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                // Simple parsing for now
                if (line.find("metric:") != std::string::npos) {
                    // Parse metric and add to history
                }
            }
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        if (logger_) {
            logger_->error("Error importing metrics: " + std::string(e.what()));
        }
    }
    
    return false;
}

std::string PerformanceMonitor::analyzeTrends(const std::string& metricName, size_t windowSize) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<double> values;
    auto start = metricHistory_.rbegin();
    auto end = metricHistory_.rend();
    
    for (auto it = start; it != end && values.size() < windowSize; ++it) {
        if (it->name == metricName) {
            values.push_back(it->value);
        }
    }
    
    if (values.size() < 2) {
        return "Insufficient data for trend analysis";
    }
    
    // Simple linear regression
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    size_t n = values.size();
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    double intercept = (sumY - slope * sumX) / n;
    
    std::ostringstream oss;
    oss << "Trend analysis for " << metricName << ":\n";
    oss << "  Linear regression: y = " << intercept << " + " << slope << " * x\n";
    oss << "  Current value: " << values.back() << "\n";
    oss << "  Trend: ";
    
    if (std::abs(slope) < 0.01) {
        oss << "stable\n";
    } else if (slope > 0.01) {
        oss << "increasing\n";
    } else {
        oss << "decreasing\n";
    }
    
    oss << "  Data points: " << values.size();
    
    return oss.str();
}

std::string PerformanceMonitor::identifyBottlenecks() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::ostringstream oss;
    oss << "Performance Bottleneck Analysis\n";
    oss << "================================\n\n";
    
    // Check for common bottlenecks
    bool hasHighStepDuration = false;
    bool hasHighFiringRate = false;
    bool hasHighMemoryUsage = false;
    
    for (const auto& metric : metricHistory_) {
        if (metric.name == "step_duration" && metric.value > 1.0) {
            hasHighStepDuration = true;
        } else if (metric.name == "firing_rate" && metric.value > 100.0) {
            hasHighFiringRate = true;
        } else if (metric.name == "memory_usage" && metric.value > 1024.0 * 1024.0 * 1024.0) {
            hasHighMemoryUsage = true;
        }
    }
    
    if (hasHighStepDuration) {
        oss << "POTENTIAL BOTTLENECK: High step duration detected.\n";
        oss << "  This may indicate slow neural computation or inefficient algorithms.\n\n";
    }
    
    if (hasHighFiringRate) {
        oss << "POTENTIAL BOTTLENECK: High firing rate detected.\n";
        oss << "  This may indicate excessive neural activity or synchronization issues.\n\n";
    }
    
    if (hasHighMemoryUsage) {
        oss << "POTENTIAL BOTTLENECK: High memory usage detected.\n";
        oss << "  This may indicate memory leaks or inefficient memory management.\n\n";
    }
    
    if (!hasHighStepDuration && !hasHighFiringRate && !hasHighMemoryUsage) {
        oss << "No obvious bottlenecks detected in recent metrics.\n\n";
    }
    
    oss << "Recommendations:\n";
    oss << "  - Profile individual operations to identify specific bottlenecks\n";
    oss << "  - Monitor system resources during simulation runs\n";
    oss << "  - Use performance scaling analysis to identify scalability issues\n";
    oss << "  - Check for memory leaks or inefficient data structures\n";
    
    return oss.str();
}

std::string PerformanceMonitor::getPerformanceSummary() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::ostringstream oss;
    oss << "Performance Summary\n";
    oss << "==================\n\n";
    oss << "Simulation Statistics:\n";
    oss << "  Total Steps: " << totalSteps_ << "\n";
    oss << "  Total Spikes: " << totalSpikes_ << "\n";
    oss << "  Average Firing Rate: " << averageFiringRate_ << " Hz\n";
    oss << "  Average Step Duration: " << averageStepDuration_ << " s\n\n";
    
    oss << "Performance Metrics:\n";
    oss << "  Total Metrics Recorded: " << metricHistory_.size() << "\n";
    oss << "  Neural Metrics: " << neuralMetrics_.size() << "\n";
    oss << "  Simulation Metrics: " << simulationMetrics_.size() << "\n";
    oss << "  Resource Metrics: " << resourceMetrics_.size() << "\n\n";
    
    oss << "Monitoring Status:\n";
    oss << "  Monitoring Active: " << (monitoring_ ? "Yes" : "No") << "\n";
    oss << "  Real-time Display: " << (realTimeDisplayActive_ ? "Yes" : "No") << "\n";
    oss << "  Performance Alerts: " << (enablePerformanceAlerts_ ? "Enabled" : "Disabled") << "\n\n";
    
    oss << "Recent Metric Samples:\n";
    auto start = metricHistory_.rbegin();
    auto end = metricHistory_.rend();
    size_t count = 0;
    for (auto it = start; it != end && count < 5; ++it, ++count) {
        oss << "  " << it->name << " = " << it->value << " " << it->unit << "\n";
    }
    
    return oss.str();
}

void PerformanceMonitor::startMonitoring() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!initialized_) {
        return;
    }
    
    monitoring_ = true;
    
    if (logger_) {
        logger_->info("Performance monitoring started");
    }
}

void PerformanceMonitor::stopMonitoring() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (!monitoring_) {
        return;
    }
    
    monitoring_ = false;
    
    // Update statistics
    updatePerformanceStatistics();
    
    // Check for performance issues
    checkPerformanceThresholds();
    
    if (logger_) {
        logger_->info("Performance monitoring stopped");
    }
}

} // namespace nlm
