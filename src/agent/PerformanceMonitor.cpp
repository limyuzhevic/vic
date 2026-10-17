#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cmath>
#include <sys/sysinfo.h> // For sysinfo()
#include <unistd.h> // For getpid()

namespace nlm {

// BasicSystemResourceMonitor implementation
SystemResourceMonitor::~SystemResourceMonitor() = default;

// Get CPU usage (percentage 0-100)
double BasicSystemResourceMonitor::getCPUUsage() const {
    updateResourceUsage();
    return cpuUsage_;
}

// Get memory usage (MB)
double BasicSystemResourceMonitor::getMemoryUsage() const {
    updateResourceUsage();
    return memoryUsage_;
}

// Get memory peak (MB)
double BasicSystemResourceMonitor::getMemoryPeak() const {
    return memoryPeak_;
}

// Get thread count
size_t BasicSystemResourceMonitor::getThreadCount() const {
    return std::thread::hardware_concurrency();
}

// Get process ID
pid_t BasicSystemResourceMonitor::getProcessID() const {
    return processID_;
}

// Get system information
std::string BasicSystemResourceMonitor::getSystemInfo() const {
    std::ostringstream oss;
    oss << "CPU Usage: " << std::fixed << std::setprecision(1) << getCPUUsage() << "%" << std::endl;
    oss << "Memory Usage: " << std::fixed << std::setprecision(2) << getMemoryUsage() << " MB" << std::endl;
    oss << "Memory Peak: " << std::fixed << std::setprecision(2) << getMemoryPeak() << " MB" << std::endl;
    oss << "Thread Count: " << getThreadCount() << std::endl;
    oss << "Process ID: " << getProcessID() << std::endl;
    return oss.str();
}

// Update resource usage
void BasicSystemResourceMonitor::updateResourceUsage() const {
    // Get system information
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        // Calculate memory usage
        double totalMemory = info.totalram * info.mem_unit / (1024.0 * 1024.0); // MB
        double freeMemory = info.freeram * info.mem_unit / (1024.0 * 1024.0); // MB
        memoryUsage_ = totalMemory - freeMemory;
        memoryPeak_ = info.maxrss * info.mem_unit / (1024.0 * 1024.0); // MB
        
        // CPU usage calculation is simplified - in production would use /proc/stat
        // For now, return a placeholder value
        cpuUsage_ = 0.0;
    }
}

// SimulationProfiler implementation
SimulationProfiler::SimulationProfiler(std::shared_ptr<SystemResourceMonitor> resourceMonitor,
                                       std::shared_ptr<Brain> brain)
    : resourceMonitor_(resourceMonitor), brain_(brain) {
    currentMetrics_ = std::make_shared<PerformanceMetrics>();
}

bool SimulationProfiler::startProfiling(const std::string& sessionName) {
    auto session = std::make_unique<ProfilingSession>(sessionName);
    session->startTime = std::chrono::high_resolution_clock::now();
    session->isActive = true;
    
    sessions_[sessionName] = std::move(session);
    updateCurrentMetrics();
    return true;
}

std::shared_ptr<PerformanceMetrics> SimulationProfiler::stopProfiling(const std::string& sessionName) {
    auto it = sessions_.find(sessionName);
    if (it == sessions_.end()) {
        return nullptr;
    }
    
    it->second->endTime = std::chrono::high_resolution_clock::now();
    it->second->isActive = false;
    
    // Calculate session metrics
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
        it->second->endTime - it->second->startTime);
    
    auto sessionMetrics = std::make_shared<PerformanceMetrics>();
    sessionMetrics->simulationTime = duration.count();
    sessionMetrics->wallClockTime = duration.count();
    sessionMetrics->simulationSteps = currentMetrics_->simulationSteps;
    sessionMetrics->spikeCount = currentMetrics_->spikeCount;
    sessionMetrics->cpuUsage = resourceMonitor_->getCPUUsage();
    sessionMetrics->memoryUsage = resourceMonitor_->getMemoryUsage();
    sessionMetrics->memoryPeak = resourceMonitor_->getMemoryPeak();
    
    // Store session metrics
    it->second->metricsHistory.push_back(sessionMetrics);
    
    return sessionMetrics;
}

std::shared_ptr<PerformanceMetrics> SimulationProfiler::getCurrentMetrics() const {
    updateCurrentMetrics();
    return currentMetrics_;
}

std::vector<std::shared_ptr<PerformanceMetrics>> SimulationProfiler::getSessionData(const std::string& sessionName) const {
    auto it = sessions_.find(sessionName);
    if (it == sessions_.end()) {
        return std::vector<std::shared_ptr<PerformanceMetrics>>();
    }
    
    return it->second->metricsHistory;
}

std::vector<std::string> SimulationProfiler::getSessionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : sessions_) {
        names.push_back(pair.first);
    }
    return names;
}

std::map<std::string, double> SimulationProfiler::calculateStatistics(const std::string& sessionName) const {
    auto metrics = getSessionData(sessionName);
    std::map<std::string, double> stats;
    
    if (metrics.empty()) return stats;
    
    // Calculate average metrics
    double totalSteps = 0.0;
    double totalSpikes = 0.0;
    double totalCPU = 0.0;
    double totalMemory = 0.0;
    
    for (const auto& metric : metrics) {
        totalSteps += metric->simulationSteps;
        totalSpikes += metric->spikeCount;
        totalCPU += metric->cpuUsage;
        totalMemory += metric->memoryUsage;
    }
    
    size_t count = metrics.size();
    stats["avg_simulation_steps"] = totalSteps / count;
    stats["avg_spike_count"] = totalSpikes / count;
    stats["avg_cpu_usage"] = totalCPU / count;
    stats["avg_memory_usage"] = totalMemory / count;
    stats["total_steps"] = totalSteps;
    stats["total_spikes"] = totalSpikes;
    
    return stats;
}

std::string SimulationProfiler::generateReport(const std::string& sessionName, 
                                             const std::string& format) const {
    auto metrics = getSessionData(sessionName);
    
    if (format == "text") {
        TextReportGenerator generator;
        return generator.generateTextReport(sessionName, metrics);
    } else if (format == "json") {
        TextReportGenerator generator;
        return generator.generateJSONReport(sessionName, metrics);
    } else if (format == "csv") {
        TextReportGenerator generator;
        return generator.generateCSVReport(sessionName, metrics);
    } else if (format == "html") {
        TextReportGenerator generator;
        return generator.generateHTMLReport(sessionName, metrics);
    }
    
    return "Unsupported format: " + format;
}

void SimulationProfiler::updateCurrentMetrics() {
    currentMetrics_->simulationSteps = brain_->getTotalSpikeCount(); // Using spike count as proxy for steps
    currentMetrics_->spikeCount = brain_->getTotalSpikeCount();
    currentMetrics_->firingRate = brain_->getAverageFiringRate();
    currentMetrics_->totalNeurons = brain_->getTotalNeuronCount();
    currentMetrics_->totalSynapses = brain_->getTotalSynapseCount();
    currentMetrics_->cpuUsage = resourceMonitor_->getCPUUsage();
    currentMetrics_->memoryUsage = resourceMonitor_->getMemoryUsage();
    currentMetrics_->memoryPeak = resourceMonitor_->getMemoryPeak();
}

// PerformanceReportGenerator implementation
std::string TextReportGenerator::generateTextReport(const std::string& sessionName,
                                                   const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const {
    std::ostringstream oss;
    oss << "Performance Report: " << sessionName << std::endl;
    oss << "===================" << std::endl;
    oss << std::endl;
    
    if (metrics.empty()) {
        oss << "No metrics available." << std::endl;
        return oss.str();
    }
    
    oss << "Performance Statistics:" << std::endl;
    oss << "----------------------" << std::endl;
    
    double totalSteps = 0.0;
    double totalSpikes = 0.0;
    double totalCPU = 0.0;
    double totalMemory = 0.0;
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        oss << "Snapshot " << i << ":" << std::endl;
        oss << "  Simulation Time: " << formatTime(metrics[i]->simulationTime) << std::endl;
        oss << "  Wall Clock Time: " << formatTime(metrics[i]->wallClockTime) << std::endl;
        oss << "  CPU Usage: " << formatCPU(metrics[i]->cpuUsage) << std::endl;
        oss << "  Memory Usage: " << formatMemory(metrics[i]->memoryUsage) << std::endl;
        oss << "  Memory Peak: " << formatMemory(metrics[i]->memoryPeak) << std::endl;
        oss << "  Total Steps: " << metrics[i]->simulationSteps << std::endl;
        oss << "  Spike Count: " << metrics[i]->spikeCount << std::endl;
        oss << "  Firing Rate: " << formatRate(metrics[i]->firingRate) << std::endl;
        oss << "  Total Neurons: " << metrics[i]->totalNeurons << std::endl;
        oss << "  Total Synapses: " << metrics[i]->totalSynapses << std::endl;
        
        totalSteps += metrics[i]->simulationSteps;
        totalSpikes += metrics[i]->spikeCount;
        totalCPU += metrics[i]->cpuUsage;
        totalMemory += metrics[i]->memoryUsage;
        oss << std::endl;
    }
    
    // Calculate averages
    size_t count = metrics.size();
    if (count > 0) {
        oss << "Averages:" << std::endl;
        oss << "----------" << std::endl;
        oss << "  Average CPU Usage: " << formatCPU(totalCPU / count) << std::endl;
        oss << "  Average Memory Usage: " << formatMemory(totalMemory / count) << std::endl;
        oss << "  Total Steps: " << totalSteps << std::endl;
        oss << "  Total Spikes: " << totalSpikes << std::endl;
        oss << "  Average Firing Rate: " << formatRate((totalSpikes / totalSteps) * 1000.0) << std::endl; // Hz
    }
    
    return oss.str();
}

std::string TextReportGenerator::generateJSONReport(const std::string& sessionName,
                                                   const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const {
    nlohmann::json report;
    report["session_name"] = sessionName;
    report["timestamp"] = PerformanceUtils::getTimestamp();
    report["metric_count"] = metrics.size();
    
    nlohmann::json metricsJson = nlohmann::json::array();
    for (const auto& metric : metrics) {
        nlohmann::json metricJson;
        metricJson["simulation_time"] = metric->simulationTime;
        metricJson["wall_clock_time"] = metric->wallClockTime;
        metricJson["cpu_usage"] = metric->cpuUsage;
        metricJson["memory_usage"] = metric->memoryUsage;
        metricJson["memory_peak"] = metric->memoryPeak;
        metricJson["simulation_steps"] = metric->simulationSteps;
        metricJson["spike_count"] = metric->spikeCount;
        metricJson["firing_rate"] = metric->firingRate;
        metricJson["total_neurons"] = metric->totalNeurons;
        metricJson["total_synapses"] = metric->totalSynapses;
        metricsJson.push_back(metricJson);
    }
    
    report["metrics"] = metricsJson;
    
    return report.dump(2);
}

std::string TextReportGenerator::generateCSVReport(const std::string& sessionName,
                                                   const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const {
    std::ostringstream oss;
    oss << "Metric,Value" << std::endl;
    oss << "Session Name," << sessionName << std::endl;
    oss << "Timestamp," << PerformanceUtils::getTimestamp() << std::endl;
    oss << "Metric Count," << metrics.size() << std::endl;
    oss << std::endl;
    
    // Headers
    oss << "Snapshot,Simulation_Time(s),Wall_Clock_Time(s),CPU_Usage(%)," << std::endl;
    oss << "Memory_Usage(MB),Memory_Peak(MB),Simulation_Steps,Spike_Count,Firing_Rate(Hz)," << std::endl;
    
    // Data
    for (size_t i = 0; i < metrics.size(); ++i) {
        oss << i << ",";
        oss << std::fixed << std::setprecision(3) << metrics[i]->simulationTime << ",";
        oss << std::fixed << std::setprecision(3) << metrics[i]->wallClockTime << ",";
        oss << std::fixed << std::setprecision(1) << metrics[i]->cpuUsage << ",";
        oss << std::fixed << std::setprecision(2) << metrics[i]->memoryUsage << ",";
        oss << std::fixed << std::setprecision(2) << metrics[i]->memoryPeak << ",";
        oss << metrics[i]->simulationSteps << ",";
        oss << metrics[i]->spikeCount << ",";
        oss << std::fixed << std::setprecision(3) << metrics[i]->firingRate << ",";
        oss << std::endl;
    }
    
    return oss.str();
}

std::string TextReportGenerator::generateHTMLReport(const std::string& sessionName,
                                                    const std::vector<std::shared_ptr<PerformanceMetrics>>& metrics) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "    <title>Performance Report: " << sessionName << "</title>" << std::endl;
    oss << "    <style>" << std::endl;
    oss << "        body { font-family: Arial, sans-serif; margin: 40px; }" << std::endl;
    oss << "        table { border-collapse: collapse; width: 100%; }" << std::endl;
    oss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "        th { background-color: #f2f2f2; }" << std::endl;
    oss << "        .metric { font-weight: bold; }" << std::endl;
    oss << "    </style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "    <h1>Performance Report: " << sessionName << "</h1>" << std::endl;
    oss << "    <p>Generated: " << PerformanceUtils::getTimestamp() << "</p>" << std::endl;
    oss << "    <p>Total snapshots: " << metrics.size() << "</p>" << std::endl;
    oss << "    <table>" << std::endl;
    oss << "        <tr><th>Snapshot</th><th>Simulation Time (s)</th><th>Wall Clock Time (s)</th>";
    oss << "<th>CPU Usage (%)</th><th>Memory Usage (MB)</th><th>Memory Peak (MB)</th>";
    oss << "<th>Simulation Steps</th><th>Spike Count</th><th>Firing Rate (Hz)</th></tr>" << std::endl;
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        oss << "        <tr>" << std::endl;
        oss << "            <td>" << i << "</td>" << std::endl;
        oss << "            <td>" << std::fixed << std::setprecision(3) << metrics[i]->simulationTime << "</td>" << std::endl;
        oss << "            <td>" << std::fixed << std::setprecision(3) << metrics[i]->wallClockTime << "</td>" << std::endl;
        oss << "            <td>" << std::fixed << std::setprecision(1) << metrics[i]->cpuUsage << "</td>" << std::endl;
        oss << "            <td>" << std::fixed << std::setprecision(2) << metrics[i]->memoryUsage << "</td>" << std::endl;
        oss << "            <td>" << std::fixed << std::setprecision(2) << metrics[i]->memoryPeak << "</td>" << std::endl;
        oss << "            <td>" << metrics[i]->simulationSteps << "</td>" << std::endl;
        oss << "            <td>" << metrics[i]->spikeCount << "</td>" << std::endl;
        oss << "            <td>" << std::fixed << std::setprecision(3) << metrics[i]->firingRate << "</td>" << std::endl;
        oss << "        </tr>" << std::endl;
    }
    
    oss << "    </table>" << std::endl;
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return oss.str();
}

std::string TextReportGenerator::formatTime(double seconds) const {
    if (seconds < 60.0) {
        return std::to_string(static_cast<int>(seconds)) + "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        int minutes = static_cast<int>(seconds / 60.0) % 60;
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    }
}

std::string TextReportGenerator::formatMemory(double mb) const {
    if (mb >= 1024.0) {
        return std::to_string(std::fixed) << std::setprecision(1) << (mb / 1024.0) << " GB";
    } else {
        return std::to_string(std::fixed) << std::setprecision(2) << mb << " MB";
    }
}

std::string TextReportGenerator::formatCPU(double percentage) const {
    return std::to_string(std::fixed) << std::setprecision(1) << percentage << "%";
}

std::string TextReportGenerator::formatRate(double rate) const {
    if (rate >= 1000.0) {
        return std::to_string(std::fixed) << std::setprecision(2) << (rate / 1000.0) << " kHz";
    } else {
        return std::to_string(std::fixed) << std::setprecision(3) << rate << " Hz";
    }
}

// PerformanceUtils implementation
std::string PerformanceUtils::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

double PerformanceUtils::percentage(double value, double total) {
    return (total > 0.0) ? (value / total) * 100.0 : 0.0;
}

PerformanceUtils::MovingAverage::MovingAverage(size_t windowSize) : windowSize_(windowSize) {}

void PerformanceUtils::MovingAverage::addValue(double value) {
    values_.push_back(value);
    if (values_.size() > windowSize_) {
        values_.erase(values_.begin());
    }
}

double PerformanceUtils::MovingAverage::getAverage() const {
    if (values_.empty()) return 0.0;
    double sum = 0.0;
    for (double value : values_) {
        sum += value;
    }
    return sum / values_.size();
}

void PerformanceUtils::MovingAverage::reset() {
    values_.clear();
}

std::map<std::string, double> PerformanceUtils::calculateStats(const std::vector<double>& values) {
    std::map<std::string, double> stats;
    
    if (values.empty()) return stats;
    
    double sum = 0.0;
    double min = values[0];
    double max = values[0];
    
    for (double value : values) {
        sum += value;
        if (value < min) min = value;
        if (value > max) max = value;
    }
    
    double mean = sum / values.size();
    double variance = 0.0;
    for (double value : values) {
        variance += (value - mean) * (value - mean);
    }
    variance /= values.size();
    double stddev = std::sqrt(variance);
    
    stats["mean"] = mean;
    stats["min"] = min;
    stats["max"] = max;
    stats["stddev"] = stddev;
    stats["count"] = values.size();
    
    return stats;
}

std::string PerformanceUtils::formatBytes(size_t bytes) {
    if (bytes >= 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    } else if (bytes >= 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else if (bytes >= 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else {
        return std::to_string(bytes) + " B";
    }
}

bool PerformanceUtils::inRange(double value, double min, double max) {
    return value >= min && value <= max;
}

// PerformanceMonitor implementation
struct PerformanceMonitor::Impl {
    std::shared_ptr<SystemResourceMonitor> resourceMonitor;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimulationProfiler> profiler;
    std::shared_ptr<TextReportGenerator> reportGenerator;
    std::map<std::string, std::vector<std::shared_ptr<PerformanceMetrics>>> sessionData;
    std::shared_ptr<PerformanceMetrics> currentMetrics;
    std::chrono::high_resolution_clock::time_point lastUpdateTime;
    double updateInterval;
    bool monitoringEnabled;
    
    Impl() : updateInterval(1.0), monitoringEnabled(false) {}
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default components
    pImpl->resourceMonitor = std::make_shared<BasicSystemResourceMonitor>();
    pImpl->profiler = std::make_shared<SimulationProfiler>(pImpl->resourceMonitor, nullptr);
    pImpl->reportGenerator = std::make_shared<TextReportGenerator>();
    
    // Set process ID for resource monitor
    pid_t pid = getpid();
    static_cast<BasicSystemResourceMonitor*>(pImpl->resourceMonitor.get())->processID_ = pid;
}

PerformanceMonitor::~PerformanceMonitor() = default;

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&&) noexcept = default;

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&&) noexcept = default;

PerformanceMonitor& PerformanceMonitor::getInstance() {
    static PerformanceMonitor instance;
    return instance;
}

bool PerformanceMonitor::startProfiling(const std::string& sessionName) {
    if (!pImpl->profiler->startProfiling(sessionName)) {
        return false;
    }
    
    pImpl->monitoringEnabled = true;
    pImpl->lastUpdateTime = std::chrono::high_resolution_clock::now();
    return true;
}

std::shared_ptr<PerformanceMetrics> PerformanceMonitor::stopProfiling(const std::string& sessionName) {
    pImpl->monitoringEnabled = false;
    return pImpl->profiler->stopProfiling(sessionName);
}

std::shared_ptr<PerformanceMetrics> PerformanceMonitor::getCurrentMetrics() const {
    return pImpl->profiler->getCurrentMetrics();
}

std::vector<std::shared_ptr<PerformanceMetrics>> PerformanceMonitor::getSessionData(const std::string& sessionName) const {
    return pImpl->profiler->getSessionData(sessionName);
}

std::vector<std::string> PerformanceMonitor::getSessionNames() const {
    return pImpl->profiler->getSessionNames();
}

std::string PerformanceMonitor::generateReport(const std::string& sessionName, const std::string& format) const {
    return pImpl->profiler->generateReport(sessionName, format);
}

bool PerformanceMonitor::exportReport(const std::string& sessionName, const std::string& format, const std::string& filepath) {
    std::string report = generateReport(sessionName, format);
    if (report.empty()) return false;
    
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    file << report;
    return true;
}

std::string PerformanceMonitor::getSystemInfo() const {
    return pImpl->resourceMonitor->getSystemInfo();
}

double PerformanceMonitor::getCPUUsage() const {
    return pImpl->resourceMonitor->getCPUUsage();
}

double PerformanceMonitor::getMemoryUsage() const {
    return pImpl->resourceMonitor->getMemoryUsage();
}

std::map<std::string, double> PerformanceMonitor::analyzePerformance(const std::string& sessionName) const {
    return pImpl->profiler->calculateStatistics(sessionName);
}

void PerformanceMonitor::configure(std::shared_ptr<const Config> config) {
    // Configure performance monitoring based on config
    if (config->has("performance.update_interval")) {
        pImpl->updateInterval = config->getOr<double>("performance.update_interval", 1.0);
    }
    
    if (config->has("performance.monitoring_enabled")) {
        pImpl->monitoringEnabled = config->getOr<bool>("performance.monitoring_enabled", true);
    }
}

void PerformanceMonitor::reset() {
    pImpl->sessionData.clear();
    pImpl->currentMetrics.reset();
    pImpl->monitoringEnabled = false;
}

void PerformanceMonitor::updateMetrics() {
    if (!pImpl->monitoringEnabled) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(
        now - pImpl->lastUpdateTime);
    
    if (duration.count() >= pImpl->updateInterval) {
        pImpl->profiler->getCurrentMetrics();
        pImpl->lastUpdateTime = now;
    }
}

} // namespace nlm