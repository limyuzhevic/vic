// NLM Advanced Debugging and Profiling Tools - Performance Profiler Implementation

#include "profiling/PerformanceProfiler.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <filesystem>

namespace nlm {

PerformanceProfiler::PerformanceProfiler(const PerformanceProfilerConfig& config) 
    : isProfiling_(false) {
    updateConfig(config);
    
    // Create output directory
    std::filesystem::create_directories(config_.outputDirectory);
}

PerformanceProfiler::~PerformanceProfiler() {
    stopSystemProfiling();
}

void PerformanceProfiler::updateConfig(const PerformanceProfilerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

void PerformanceProfiler::startProfile(const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!isProfiling_) {
        isProfiling_ = true;
        NLM_LOG_INFO("PerformanceProfiler started for operation: " + operation);
    }
    
    // Start new profiling session
    std::string sessionId = createProfilingSession("profile_" + operation);
    activeSessions_[sessionId] = std::chrono::system_clock::now();
    
    // Record start time
    NLM_LOG_INFO("Profiling started for operation: " + operation + " (session: " + sessionId + ")");
    
    // Call callback
    if (profilingStartCallback_) {
        profilingStartCallback_(operation);
    }
}

void PerformanceProfiler::stopProfile(const std::string& operation) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (activeSessions_.empty()) {
        return;
    }
    
    // Find most recent session
    auto it = activeSessions_.end();
    it--;
    std::string sessionId = it->first;
    
    // End session
    endProfilingSession(sessionId);
    activeSessions_.erase(it);
    
    isProfiling_ = false;
    
    // Get duration
    auto sessionIt = sessions_.find(sessionId);
    double duration = 0.0;
    if (sessionIt != sessions_.end()) {
        duration = sessionIt->second.getDuration();
    }
    
    NLM_LOG_INFO("Profiling stopped for operation: " + operation + " (duration: " + 
                std::to_string(duration) + "s)");
    
    // Call callback
    if (profilingStopCallback_) {
        profilingStopCallback_(operation, duration);
    }
}

void PerformanceProfiler::startSystemProfiling() {
    if (config_.enableSystemProfiling) {
        NLM_LOG_INFO("System profiling started");
    }
}

void PerformanceProfiler::stopSystemProfiling() {
    NLM_LOG_INFO("System profiling stopped");
}

void PerformanceProfiler::pauseProfiling() {
    NLM_LOG_INFO("PerformanceProfiler paused");
}

void PerformanceProfiler::resumeProfiling() {
    NLM_LOG_INFO("PerformanceProfiler resumed");
}

void PerformanceProfiler::recordMetric(const std::string& name, double value, const std::string& unit,
                                      const std::string& component, const std::map<std::string, std::string>& tags) {
    if (!config_.enablePerformanceProfiling) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    PerformanceMetric metric(name, value, unit);
    metric.component = component;
    metric.tags = tags;
    
    // Store metric by name
    metrics_[name].push_back(metric);
    
    // Store in history (with limit)
    metricsHistory_.push_back(metric);
    if (metricsHistory_.size() > config_.maxMetricsHistory) {
        metricsHistory_.erase(metricsHistory_.begin());
    }
    
    // Call callback
    if (metricRecordedCallback_) {
        metricRecordedCallback_(metric);
    }
}

void PerformanceProfiler::recordCPUTime(const std::string& operation, double cpuTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    recordMetric("cpu_time_" + operation, cpuTime, "seconds", "profiler");
    
    // Update session data
    for (auto& pair : sessions_) {
        if (pair.first.find(operation) != std::string::npos) {
            pair.second.cpuTimeByOperation[operation] = cpuTime;
        }
    }
}

void PerformanceProfiler::recordMemoryStats(size_t allocated, size_t deallocated) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    recordMetric("memory_allocated", static_cast<double>(allocated), "bytes", "profiler");
    recordMetric("memory_deallocated", static_cast<double>(deallocated), "bytes", "profiler");
    
    for (auto& pair : sessions_) {
        pair.second.memoryAllocated += allocated;
        pair.second.memoryDeallocated += deallocated;
    }
}

void PerformanceProfiler::recordSystemMetric(const std::string& name, double value, const std::string& component) {
    if (!config_.enableSystemProfiling) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    recordMetric(name, value, "unknown", component);
    systemMetrics_[name].push_back(value);
}

const std::vector<PerformanceMetric>& PerformanceProfiler::getMetrics(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    static const std::vector<PerformanceMetric> empty;
    
    if (name.empty()) {
        return metricsHistory_;
    }
    
    auto it = metrics_.find(name);
    return (it != metrics_.end()) ? it->second : empty;
}

double PerformanceProfiler::getMetricMean(const std::string& name) const {
    const auto& values = getMetricValues(name);
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceProfiler::getMetricStdDev(const std::string& name) const {
    const auto& values = getMetricValues(name);
    if (values.size() < 2) return 0.0;
    
    double mean = getMetricMean(name);
    double sumSquares = 0.0;
    
    for (double value : values) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (values.size() - 1));
}

double PerformanceProfiler::getMetricMin(const std::string& name) const {
    const auto& values = getMetricValues(name);
    if (values.empty()) return 0.0;
    
    return *std::min_element(values.begin(), values.end());
}

double PerformanceProfiler::getMetricMax(const std::string& name) const {
    const auto& values = getMetricValues(name);
    if (values.empty()) return 0.0;
    
    return *std::max_element(values.begin(), values.end());
}

std::string PerformanceProfiler::createProfilingSession(const std::string& name) {
    std::string sessionId = "session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    sessions_[sessionId] = ProfilingSession(name);
    return sessionId;
}

void PerformanceProfiler::endProfilingSession(const std::string& sessionId) {
    auto it = sessions_.find(sessionId);
    if (it != sessions_.end()) {
        it->second.endTime = std::chrono::system_clock::now();
        
        // Add session metrics to global metrics
        for (const auto& metric : it->second.metrics) {
            recordMetric(metric.name, metric.value, metric.unit, metric.component, metric.tags);
        }
    }
}

ProfilingSession* PerformanceProfiler::getProfilingSession(const std::string& sessionId) {
    auto it = sessions_.find(sessionId);
    return (it != sessions_.end()) ? &it->second : nullptr;
}

std::vector<std::string> PerformanceProfiler::getActiveSessionIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> sessionIds;
    for (const auto& pair : activeSessions_) {
        sessionIds.push_back(pair.first);
    }
    return sessionIds;
}

std::string PerformanceProfiler::getPerformanceReport(const std::string& sessionId) const {
    std::ostringstream oss;
    oss << "Performance Report\n";
    oss << "==================\n\n";
    
    if (sessionId.empty()) {
        oss << "Global Performance Metrics:\n";
        oss << "  Total metrics recorded: " << metricsHistory_.size() << "\n";
        oss << "  Unique metric names: " << metrics_.size() << "\n";
        oss << "  Active sessions: " << activeSessions_.size() << "\n\n";
        
        for (const auto& pair : metrics_) {
            oss << pair.first << ":\n";
            oss << "  Count: " << pair.second.size() << "\n";
            oss << "  Mean: " << getMetricMean(pair.first) << "\n";
            oss << "  Std Dev: " << getMetricStdDev(pair.first) << "\n";
            oss << "  Min: " << getMetricMin(pair.first) << "\n";
            oss << "  Max: " << getMetricMax(pair.first) << "\n\n";
        }
    } else {
        auto sessionIt = sessions_.find(sessionId);
        if (sessionIt != sessions_.end()) {
            const auto& session = sessionIt->second;
            oss << "Session: " << session.name << "\n";
            oss << "Session ID: " << sessionId << "\n";
            oss << "Duration: " << session.getDurationString() << "\n";
            oss << "Metrics recorded: " << session.metrics.size() << "\n";
            oss << "Memory allocated: " << session.memoryAllocated << " bytes\n";
            oss << "Memory deallocated: " << session.memoryDeallocated << " bytes\n\n";
            
            oss << "CPU time by operation:\n";
            for (const auto& pair : session.cpuTimeByOperation) {
                oss << "  " << pair.first << ": " << pair.second << "s\n";
            }
        } else {
            oss << "Session not found: " << sessionId << "\n";
        }
    }
    
    return oss.str();
}

std::string PerformanceProfiler::getCPUPerformanceReport() const {
    std::ostringstream oss;
    oss << "CPU Performance Report\n";
    oss << "======================\n\n";
    
    // Find CPU-related metrics
    std::vector<std::string> cpuMetrics;
    for (const auto& pair : metrics_) {
        if (pair.first.find("cpu") != std::string::npos) {
            cpuMetrics.push_back(pair.first);
        }
    }
    
    if (cpuMetrics.empty()) {
        oss << "No CPU metrics recorded.\n";
        return oss.str();
    }
    
    for (const auto& metricName : cpuMetrics) {
        oss << metricName << ":\n";
        oss << "  Mean: " << getMetricMean(metricName) << "\n";
        oss << "  Std Dev: " << getMetricStdDev(metricName) << "\n";
        oss << "  Min: " << getMetricMin(metricName) << "\n";
        oss << "  Max: " << getMetricMax(metricName) << "\n\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::getMemoryPerformanceReport() const {
    std::ostringstream oss;
    oss << "Memory Performance Report\n";
    oss << "========================\n\n";
    
    // Find memory-related metrics
    std::vector<std::string> memoryMetrics;
    for (const auto& pair : metrics_) {
        if (pair.first.find("memory") != std::string::npos) {
            memoryMetrics.push_back(pair.first);
        }
    }
    
    if (memoryMetrics.empty()) {
        oss << "No memory metrics recorded.\n";
        return oss.str();
    }
    
    for (const auto& metricName : memoryMetrics) {
        oss << metricName << ":\n";
        oss << "  Mean: " << getMetricMean(metricName) << "\n";
        oss << "  Std Dev: " << getMetricStdDev(metricName) << "\n";
        oss << "  Min: " << getMetricMin(metricName) << "\n";
        oss << "  Max: " << getMetricMax(metricName) << "\n\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::getSystemPerformanceReport() const {
    std::ostringstream oss;
    oss << "System Performance Report\n";
    oss << "=========================\n\n";
    
    oss << "System metrics recorded:\n";
    for (const auto& pair : systemMetrics_) {
        oss << "  " << pair.first << ": " << pair.second.size() << " samples\n";
    }
    
    return oss.str();
}

std::vector<std::string> PerformanceProfiler::detectCPU bottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // Check for high CPU usage metrics
    auto it = metrics_.find("cpu_usage");
    if (it != metrics_.end()) {
        const auto& values = getMetricValues("cpu_usage");
        double maxUsage = 0.0;
        if (!values.empty()) {
            maxUsage = *std::max_element(values.begin(), values.end());
        }
        
        if (maxUsage > 90.0) {
            bottlenecks.push_back("High CPU usage detected (>90%): " + std::to_string(maxUsage) + "%");
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceProfiler::detectMemoryBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // Check for memory allocation patterns
    auto it = metrics_.find("memory_allocated");
    if (it != metrics_.end()) {
        const auto& values = getMetricValues("memory_allocated");
        double maxAllocation = 0.0;
        if (!values.empty()) {
            maxAllocation = *std::max_element(values.begin(), values.end());
        }
        
        if (maxAllocation > 1024 * 1024 * 1024) { // 1GB
            bottlenecks.push_back("High memory allocation detected (>1GB): " + 
                                 std::to_string(maxAllocation / (1024*1024)) + " MB");
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceProfiler::detectSystemBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // Check system metrics for potential issues
    for (const auto& pair : systemMetrics_) {
        if (pair.first == "cpu_usage" || pair.first == "memory_usage") {
            const auto& values = pair.second;
            if (values.empty()) continue;
            
            double maxValue = *std::max_element(values.begin(), values.end());
            
            if (pair.first == "cpu_usage" && maxValue > 80.0) {
                bottlenecks.push_back("High CPU usage: " + std::to_string(maxValue) + "%");
            } else if (pair.first == "memory_usage" && maxValue > 80.0) {
                bottlenecks.push_back("High memory usage: " + std::to_string(maxValue) + "%");
            }
        }
    }
    
    return bottlenecks;
}

bool PerformanceProfiler::exportMetrics(const std::string& filename, const std::string& format) {
    if (format == "csv") {
        return generateCSVExport(filename);
    } else if (format == "json") {
        return generateJSONExport(filename);
    } else if (format == "xml") {
        return generateXMLEmport(filename);
    }
    
    return false;
}

bool PerformanceProfiler::exportProfilingData(const std::string& filename, const std::string& format) {
    // For now, just export as CSV
    return exportMetrics(filename + ".metrics", "csv");
}

bool PerformanceProfiler::exportSessions(const std::string& filename, const std::string& format) {
    if (format == "xml") {
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        
        file << "<?xml version=\"1.0\"?>\n";
        file << "<profiling_sessions>\n";
        
        for (const auto& pair : sessions_) {
            file << "  <session id=\"" << pair.first << "\" name=\"" << pair.second.name << "\">\n";
            file << "    <duration>" << pair.second.getDuration() << "</duration>\n";
            file << "    <metrics>" << pair.second.metrics.size() << "</metrics>\n";
            file << "  </session>\n";
        }
        
        file << "</profiling_sessions>\n";
        return true;
    }
    
    return false;
}

size_t PerformanceProfiler::getMetricCount(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (name.empty()) {
        return metricsHistory_.size();
    }
    
    auto it = metrics_.find(name);
    return (it != metrics_.end()) ? it->second.size() : 0;
}

size_t PerformanceProfiler::getSessionCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.size();
}

std::string PerformanceProfiler::getProfilerStatistics() const {
    std::ostringstream oss;
    oss << "PerformanceProfiler Statistics:\n";
    oss << "  Profiling active: " << (isProfiling_ ? "Yes" : "No") << "\n";
    oss << "  Total metrics: " << metricsHistory_.size() << "\n";
    oss << "  Unique metric names: " << metrics_.size() << "\n";
    oss << "  Active sessions: " << activeSessions_.size() << "\n";
    oss << "  Total sessions: " << sessions_.size() << "\n";
    
    return oss.str();
}

void PerformanceProfiler::setProfilingStartCallback(std::function<void(const std::string&)> callback) {
    profilingStartCallback_ = callback;
}

void PerformanceProfiler::setProfilingStopCallback(std::function<void(const std::string&, double)> callback) {
    profilingStopCallback_ = callback;
}

void PerformanceProfiler::setMetricRecordedCallback(std::function<void(const PerformanceMetric&)> callback) {
    metricRecordedCallback_ = callback;
}

void PerformanceProfiler::recordSystemMetrics() {
    if (config_.enableSystemProfiling) {
        // Record basic system metrics
        recordMetric("timestamp", std::chrono::system_clock::now().time_since_epoch().count(), "epoch", "system");
        recordMetric("profiler_active", isProfiling_ ? 1.0 : 0.0, "boolean", "system");
    }
}

void PerformanceProfiler::cleanupOldMetrics() {
    if (metricsHistory_.size() > config_.maxMetricsHistory) {
        metricsHistory_.erase(metricsHistory_.begin(),
                            metricsHistory_.begin() + (metricsHistory_.size() - config_.maxMetricsHistory));
    }
}

void PerformanceProfiler::processProfilingCallbacks(const std::string& operation, double duration) {
    // Process callbacks for profiling events
}

std::vector<double> PerformanceProfiler::getMetricValues(const std::string& name) const {
    std::vector<double> values;
    auto it = metrics_.find(name);
    if (it != metrics_.end()) {
        for (const auto& metric : it->second) {
            values.push_back(metric.value);
        }
    }
    return values;
}

std::string PerformanceProfiler::formatMetricValue(double value, const std::string& unit) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3) << value;
    if (!unit.empty()) {
        oss << " " << unit;
    }
    return oss.str();
}

std::string PerformanceProfiler::getProfilerTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string PerformanceProfiler::generateCSVExport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return "Failed to open file for writing: " + filename;
    }
    
    file << "timestamp,name,value,unit,component\n";
    
    for (const auto& metric : metricsHistory_) {
        file << metric.timestamp.time_since_epoch().count() << ","
             << metric.name << ","
             << metric.value << ","
             << metric.unit << ","
             << metric.component << "\n";
    }
    
    return "Successfully exported " + std::to_string(metricsHistory_.size()) + " metrics to CSV";
}

std::string PerformanceProfiler::generateJSONExport(const std::string& filename) const {
    Json::Value root;
    root["metrics"] = Json::Value(Json::arrayValue);
    
    for (const auto& metric : metricsHistory_) {
        Json::Value metricJson;
        metricJson["timestamp"] = metric.timestamp.time_since_epoch().count();
        metricJson["name"] = metric.name;
        metricJson["value"] = metric.value;
        metricJson["unit"] = metric.unit;
        metricJson["component"] = metric.component;
        
        root["metrics"].append(metricJson);
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return "Failed to open file for writing: " + filename;
    }
    
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    writer->write(root, file);
    
    return "Successfully exported " + std::to_string(metricsHistory_.size()) + " metrics to JSON";
}

std::string PerformanceProfiler::generateXMLEmport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return "Failed to open file for writing: " + filename;
    }
    
    file << "<?xml version=\"1.0\"?>\n";
    file << "<performance_metrics>\n";
    
    for (const auto& metric : metricsHistory_) {
        file << "  <metric>\n";
        file << "    <timestamp>" << metric.timestamp.time_since_epoch().count() << "</timestamp>\n";
        file << "    <name>" << metric.name << "</name>\n";
        file << "    <value>" << metric.value << "</value>\n";
        file << "    <unit>" << metric.unit << "</unit>\n";
        file << "    <component>" << metric.component << "</component>\n";
        file << "  </metric>\n";
    }
    
    file << "</performance_metrics>\n";
    
    return "Successfully exported " + std::to_string(metricsHistory_.size()) + " metrics to XML";
}

std::vector<std::string> PerformanceProfiler::detectPerformanceBottlenecks() const {
    std::vector<std::string> bottlenecks;
    
    // Check CPU bottlenecks
    auto cpuBottlenecks = detectCPUBottlenecks();
    bottlenecks.insert(bottlenecks.end(), cpuBottlenecks.begin(), cpuBottlenecks.end());
    
    // Check memory bottlenecks
    auto memoryBottlenecks = detectMemoryBottlenecks();
    bottlenecks.insert(bottlenecks.end(), memoryBottlenecks.begin(), memoryBottlenecks.end());
    
    // Check system bottlenecks
    auto systemBottlenecks = detectSystemBottlenecks();
    bottlenecks.insert(bottlenecks.end(), systemBottlenecks.begin(), systemBottlenecks.end());
    
    return bottlenecks;
}

void PerformanceProfiler::attachToBrain(const std::string& brainId) {
    NLM_LOG_INFO("PerformanceProfiler attached to brain: " + brainId);
}

void PerformanceProfiler::attachToExperiment(const std::string& experimentName) {
    NLM_LOG_INFO("PerformanceProfiler attached to experiment: " + experimentName);
}

} // namespace nlm
