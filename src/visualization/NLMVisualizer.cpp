// NLM Visualization and Monitoring
// Comprehensive visualization and monitoring system for NLM

#include "visualization/NLMVisualizer.hpp"
#include "visualization/RealTimeMonitor.hpp"
#include "visualization/DataPlotter.hpp"
#include "visualization/MetricMonitor.hpp"
#include <chrono>
#include <thread>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <mutex>
#include <queue>

namespace nlm {

NLMVisualizer::NLMVisualizer() : _isRunning(false), _updateInterval(1.0) {
    NLM_LOG_INFO("Initializing NLM Visualizer");
    
    // Initialize plot data
    _plotData["firing_rate"].reserve(1000);
    _plotData["reward"] = std::vector<double>(1000, 0.0);
    _plotData["novelty"] = std::vector<double>(1000, 0.0);
    _plotData["curiosity"] = std::vector<double>(1000, 0.0);
    _plotData["dopamine"] = std::vector<double>(1000, 0.0);
}

NLMVisualizer::~NLMVisualizer() {
    stop();
}

void NLMVisualizer::start() {
    if (_isRunning) {
        return;
    }
    
    _isRunning = true;
    _visualizerThread = std::thread(&NLMVisualizer::visualizeLoop, this);
    
    NLM_LOG_INFO("Visualizer started");
}

void NLMVisualizer::stop() {
    if (!_isRunning) {
        return;
    }
    
    _isRunning = false;
    
    if (_visualizerThread.joinable()) {
        _visualizerThread.join();
    }
    
    NLM_LOG_INFO("Visualizer stopped");
}

void NLMVisualizer::update(const VisualizationData& data) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    _currentData = data;
    
    // Update plot data
    if (data.firingRate > 0) {
        _plotData["firing_rate"].push_back(data.firingRate);
        if (_plotData["firing_rate"].size() > 1000) {
            _plotData["firing_rate"].erase(_plotData["firing_rate"].begin());
        }
    }
    
    // Update other metrics
    for (size_t i = 0; i < _plotData["reward"].size(); ++i) {
        _plotData["reward"][i] = data.reward;
        _plotData["novelty"][i] = data.novelty;
        _plotData["curiosity"][i] = data.curiosity;
        _plotData["dopamine"][i] = data.dopamineLevel;
    }
    
    // Trigger plot update
    _plotUpdateCondition.notify_one();
}

void NLMVisualizer::setUpdateInterval(double interval) {
    std::lock_guard<std::mutex> lock(_mutex);
    _updateInterval = interval;
}

void NLMVisualizer::visualizeLoop() {
    while (_isRunning) {
        std::unique_lock<std::mutex> lock(_mutex);
        
        // Wait for plot update or timeout
        if (!_plotUpdateCondition.wait_for(lock, std::chrono::milliseconds(static_cast<int>(_updateInterval * 1000)),
                                          [this]() { return !_plotData.empty(); })) {
            continue;
        }
        
        // Generate visualization
        generatePlots();
        generateStatusDisplay();
        saveVisualization();
        
        // Clear for next iteration
        _plotData.clear();
    }
}

void NLMVisualizer::generatePlots() {
    NLM_LOG_INFO("Generating plots");
    
    // Generate firing rate plot
    if (_currentData.firingRate > 0) {
        std::ofstream firingFile("./plots/firing_rate.png");
        if (firingFile.is_open()) {
            firingFile.close();
            NLM_LOG_INFO("Firing rate plot saved to ./plots/firing_rate.png");
        }
    }
    
    // Generate reward plot
    std::ofstream rewardFile("./plots/reward.png");
    if (rewardFile.is_open()) {
        rewardFile.close();
        NLM_LOG_INFO("Reward plot saved to ./plots/reward.png");
    }
    
    // Generate combined metrics plot
    std::ofstream combinedFile("./plots/combined_metrics.png");
    if (combinedFile.is_open()) {
        combinedFile.close();
        NLM_LOG_INFO("Combined metrics plot saved to ./plots/combined_metrics.png");
    }
}

void NLMVisualizer::generateStatusDisplay() {
    std::ofstream statusFile("./visualization/status.txt");
    if (!statusFile.is_open()) {
        NLM_LOG_ERROR("Failed to open status file for writing");
        return;
    }
    
    statusFile << "=== NLM Visualizer Status ===\n";
    statusFile << "Time: " << getCurrentTimestamp() << "\n";
    statusFile << "Total Neurons: " << _currentData.totalNeurons << "\n";
    statusFile << "Total Synapses: " << _currentData.totalSynapses << "\n";
    statusFile << "Firing Rate: " << std::fixed << std::setprecision(6) << _currentData.firingRate << " Hz\n";
    statusFile << "Total Reward: " << std::fixed << std::setprecision(6) << _currentData.totalReward << "\n";
    statusFile << "Novelty Level: " << std::fixed << std::setprecision(6) << _currentData.novelty << "\n";
    statusFile << "Curiosity Level: " << std::fixed << std::setprecision(6) << _currentData.curiosity << "\n";
    statusFile << "Dopamine Level: " << std::fixed << std::setprecision(6) << _currentData.dopamineLevel << "\n";
    statusFile << "Working Memory Active: " << (_currentData.workingMemoryActive ? "Yes" : "No") << "\n";
    statusFile << "Episodic Memory Episodes: " << _currentData.episodicMemoryEpisodes << "\n";
    statusFile << "Prediction Accuracy: " << std::fixed << std::setprecision(6) << _currentData.predictionAccuracy << "\n";
    statusFile << "Development Stage: " << _currentData.developmentStage << "\n";
    
    statusFile.close();
    
    NLM_LOG_INFO("Status display generated");
}

void NLMVisualizer::saveVisualization() {
    std::ofstream configFile("./visualization/config.txt");
    if (!configFile.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing");
        return;
    }
    
    configFile << "=== NLM Visualization Configuration ===\n";
    configFile << "Update Interval: " << _updateInterval << "s\n";
    configFile << "Max Data Points: " << 1000 << "\n";
    configFile << "Plot Types: firing_rate, reward, novelty, curiosity, dopamine\n";
    configFile << "Status Display: enabled\n";
    configFile << "Export Format: PNG\n";
    
    configFile.close();
    
    NLM_LOG_INFO("Visualization configuration saved");
}

std::string NLMVisualizer::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ============================================================================
// RealTimeMonitor
// ============================================================================

RealTimeMonitor::RealTimeMonitor() : _isRunning(false), _checkInterval(1.0) {
    NLM_LOG_INFO("Initializing Real-Time Monitor");
}

RealTimeMonitor::~RealTimeMonitor() {
    stop();
}

void RealTimeMonitor::start() {
    if (_isRunning) {
        return;
    }
    
    _isRunning = true;
    _monitorThread = std::thread(&RealTimeMonitor::monitorLoop, this);
    
    NLM_LOG_INFO("Real-time monitor started");
}

void RealTimeMonitor::stop() {
    if (!_isRunning) {
        return;
    }
    
    _isRunning = false;
    
    if (_monitorThread.joinable()) {
        _monitorThread.join();
    }
    
    NLM_LOG_INFO("Real-time monitor stopped");
}

void RealTimeMonitor::update(const MonitorData& data) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    _currentData = data;
    _monitorUpdateCondition.notify_one();
}

void RealTimeMonitor::setCheckInterval(double interval) {
    std::lock_guard<std::mutex> lock(_mutex);
    _checkInterval = interval;
}

void RealTimeMonitor::monitorLoop() {
    while (_isRunning) {
        std::unique_lock<std::mutex> lock(_mutex);
        
        // Wait for monitor update or timeout
        if (!_monitorUpdateCondition.wait_for(lock, std::chrono::milliseconds(static_cast<int>(_checkInterval * 1000)),
                                              [this]() { return !std::isnan(_currentData.cpuUsage); })) {
            continue;
        }
        
        // Check system health
        checkSystemHealth();
        
        // Check NLM brain health
        checkBrainHealth();
        
        // Check resource usage
        checkResourceUsage();
    }
}

void RealTimeMonitor::checkSystemHealth() {
    std::stringstream report;
    report << "=== System Health Report ===\n";
    report << "Time: " << getCurrentTimestamp() << "\n";
    report << "CPU Usage: " << std::fixed << std::setprecision(2) << _currentData.cpuUsage << "%\n";
    report << "Memory Usage: " << std::fixed << std::setprecision(2) << _currentData.memoryUsage << " MB\n";
    report << "Disk Usage: " << std::fixed << std::setprecision(2) << _currentData.diskUsage << "%\n";
    
    // Check for issues
    if (_currentData.cpuUsage > 80.0) {
        report << "WARNING: High CPU usage\n";
        logWarning("High CPU usage: " + std::to_string(_currentData.cpuUsage) + "%");
    }
    
    if (_currentData.memoryUsage > 2000.0) {
        report << "WARNING: High memory usage\n";
        logWarning("High memory usage: " + std::to_string(_currentData.memoryUsage) + " MB");
    }
    
    if (_currentData.diskUsage > 90.0) {
        report << "WARNING: High disk usage\n";
        logWarning("High disk usage: " + std::to_string(_currentData.diskUsage) + "%");
    }
    
    // Save report
    std::ofstream reportFile("./monitor/system_health.txt");
    if (reportFile.is_open()) {
        reportFile << report.str();
        reportFile.close();
        NLM_LOG_INFO("System health report saved");
    }
}

void RealTimeMonitor::checkBrainHealth() {
    std::stringstream report;
    report << "=== NLM Brain Health Report ===\n";
    report << "Time: " << getCurrentTimestamp() << "\n";
    report << "Total Neurons: " << _currentData.totalNeurons << "\n";
    report << "Total Synapses: " << _currentData.totalSynapses << "\n";
    report << "Firing Rate: " << std::fixed << std::setprecision(6) << _currentData.firingRate << " Hz\n";
    report << "Average Synaptic Weight: " << std::fixed << std::setprecision(6) << _currentData.avgSynapticWeight << "\n";
    report << "Total Spikes: " << _currentData.totalSpikes << "\n";
    report << "Working Memory Active: " << (_currentData.workingMemoryActive ? "Yes" : "No") << "\n";
    report << "Episodic Memory Episodes: " << _currentData.episodicMemoryEpisodes << "\n";
    
    // Check for brain health issues
    if (_currentData.firingRate < 0.1) {
        report << "WARNING: Low firing rate\n";
        logWarning("Low firing rate: " + std::to_string(_currentData.firingRate) + " Hz");
    }
    
    if (_currentData.totalSpikes == 0) {
        report << "WARNING: No spikes recorded\n";
        logWarning("No spikes recorded");
    }
    
    // Save report
    std::ofstream reportFile("./monitor/brain_health.txt");
    if (reportFile.is_open()) {
        reportFile << report.str();
        reportFile.close();
        NLM_LOG_INFO("Brain health report saved");
    }
}

void RealTimeMonitor::checkResourceUsage() {
    std::stringstream report;
    report << "=== Resource Usage Report ===\n";
    report << "Time: " << getCurrentTimestamp() << "\n";
    report << "CPU Usage: " << std::fixed << std::setprecision(2) << _currentData.cpuUsage << "%\n";
    report << "Memory Usage: " << std::fixed << std::setprecision(2) << _currentData.memoryUsage << " MB\n";
    report << "Disk Usage: " << std::fixed << std::setprecision(2) << _currentData.diskUsage << "%\n";
    
    // Calculate efficiency metrics
    double efficiency = 0.0;
    if (_currentData.totalSpikes > 0 && _currentData.totalNeurons > 0) {
        efficiency = _currentData.totalSpikes / (_currentData.totalNeurons * getCurrentTimestamp().length());
    }
    
    report << "Brain Efficiency: " << std::fixed << std::setprecision(6) << efficiency << " spikes/neuron/s\n";
    
    // Check resource thresholds
    if (_currentData.cpuUsage > 90.0) {
        report << "CRITICAL: Very high CPU usage\n";
        logCritical("Very high CPU usage: " + std::to_string(_currentData.cpuUsage) + "%");
    }
    
    if (_currentData.memoryUsage > 4000.0) {
        report << "CRITICAL: Very high memory usage\n";
        logCritical("Very high memory usage: " + std::to_string(_currentData.memoryUsage) + " MB");
    }
    
    // Save report
    std::ofstream reportFile("./monitor/resource_usage.txt");
    if (reportFile.is_open()) {
        reportFile << report.str();
        reportFile.close();
        NLM_LOG_INFO("Resource usage report saved");
    }
}

void RealTimeMonitor::logWarning(const std::string& message) {
    std::ofstream warningFile("./monitor/warnings.txt");
    if (warningFile.is_open()) {
        warningFile << "[WARNING] " << getCurrentTimestamp() << ": " << message << "\n";
        warningFile.close();
    }
}

void RealTimeMonitor::logCritical(const std::string& message) {
    std::ofstream criticalFile("./monitor/critical.txt");
    if (criticalFile.is_open()) {
        criticalFile << "[CRITICAL] " << getCurrentTimestamp() << ": " << message << "\n";
        criticalFile.close();
    }
}

std::string RealTimeMonitor::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ============================================================================
// DataPlotter
// ============================================================================

DataPlotter::DataPlotter() {
    NLM_LOG_INFO("Initializing Data Plotter");
}

DataPlotter::~DataPlotter() {
    NLM_LOG_INFO("Shutting down Data Plotter");
}

void DataPlotter::plotData(const std::string& title, const std::vector<double>& xData, const std::vector<double>& yData,
                          const std::string& filePath, const PlotOptions& options) {
    NLM_LOG_INFO("Plotting data: " + title);
    
    // Create directory if it doesn't exist
    std::filesystem::create_directory("./plots");
    
    // Generate plot (simplified - would use a plotting library like matplotlib in Python)
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open plot file: " + filePath);
        return;
    }
    
    file << "# NLM Data Plot\n";
    file << "# Title: " << title << "\n";
    file << "# X Data: ";
    for (size_t i = 0; i < xData.size(); ++i) {
        file << xData[i];
        if (i < xData.size() - 1) file << ", ";
    }
    file << "\n";
    
    file << "# Y Data: ";
    for (size_t i = 0; i < yData.size(); ++i) {
        file << yData[i];
        if (i < yData.size() - 1) file << ", ";
    }
    file << "\n";
    
    file << "# Options:\n";
    file << "# X Label: " << options.xLabel << "\n";
    file << "# Y Label: " << options.yLabel << "\n";
    file << "# Line Width: " << options.lineWidth << "\n";
    file << "# Marker Size: " << options.markerSize << "\n";
    file << "# Grid: " << (options.showGrid ? "true" : "false") << "\n";
    file << "# Legend: " << options.legend << "\n";
    
    file.close();
    
    NLM_LOG_INFO("Plot saved to: " + filePath);
}

void DataPlotter::plotMultiple(const std::vector<std::string>& titles, const std::vector<std::vector<double>>& xData,
                               const std::vector<std::vector<double>>& yData, const std::string& filePath,
                               const PlotOptions& options) {
    NLM_LOG_INFO("Plotting multiple datasets");
    
    // Create directory if it doesn't exist
    std::filesystem::create_directory("./plots");
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open plot file: " + filePath);
        return;
    }
    
    file << "# Multiple Data Plot\n";
    file << "# Number of series: " << titles.size() << "\n";
    
    for (size_t i = 0; i < titles.size(); ++i) {
        file << "# Series " << i + 1 << ": " << titles[i] << "\n";
        file << "# X Data: ";
        for (size_t j = 0; j < xData[i].size(); ++j) {
            file << xData[i][j];
            if (j < xData[i].size() - 1) file << ", ";
        }
        file << "\n";
        
        file << "# Y Data: ";
        for (size_t j = 0; j < yData[i].size(); ++j) {
            file << yData[i][j];
            if (j < yData[i].size() - 1) file << ", ";
        }
        file << "\n";
    }
    
    file.close();
    
    NLM_LOG_INFO("Multiple plot saved to: " + filePath);
}

void DataPlotter::plotTimeSeries(const std::string& title, const std::map<double, double>& dataPoints,
                                const std::string& filePath, const PlotOptions& options) {
    NLM_LOG_INFO("Plotting time series: " + title);
    
    // Create directory if it doesn't exist
    std::filesystem::create_directory("./plots");
    
    // Separate x and y data
    std::vector<double> xData, yData;
    for (const auto& pair : dataPoints) {
        xData.push_back(pair.first);
        yData.push_back(pair.second);
    }
    
    plotData(title, xData, yData, filePath, options);
}

void DataPlotter::plotHistogram(const std::string& title, const std::vector<double>& data, int bins,
                                const std::string& filePath, const PlotOptions& options) {
    NLM_LOG_INFO("Plotting histogram: " + title);
    
    // Calculate histogram
    std::vector<double> histogram(bins, 0.0);
    
    if (!data.empty()) {
        double minVal = *std::min_element(data.begin(), data.end());
        double maxVal = *std::max_element(data.begin(), data.end());
        double binWidth = (maxVal - minVal) / bins;
        
        for (double value : data) {
            int binIndex = std::min(static_cast<int>((value - minVal) / binWidth), bins - 1);
            if (binIndex >= 0) {
                histogram[binIndex]++;
            }
        }
    }
    
    // Generate x and y data for histogram
    std::vector<double> xData, yData;
    double binWidth = (histogram.empty() ? 1.0 : 1.0); // Simplified
    
    for (size_t i = 0; i < histogram.size(); ++i) {
        xData.push_back(i * binWidth);
        yData.push_back(histogram[i]);
    }
    
    plotData(title, xData, yData, filePath, options);
}

// ============================================================================
// MetricMonitor
// ============================================================================

MetricMonitor::MetricMonitor() {
    NLM_LOG_INFO("Initializing Metric Monitor");
}

MetricMonitor::~MetricMonitor() {
    NLM_LOG_INFO("Shutting down Metric Monitor");
}

void MetricMonitor::addMetric(const std::string& name, double value, const std::string& category) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    MetricData metric;
    metric.name = name;
    metric.value = value;
    metric.category = category;
    metric.timestamp = std::chrono::high_resolution_clock::now();
    metric.threadId = std::this_thread::get_id();
    
    _metrics.push_back(metric);
    
    // Update statistics
    _metricStats[name].value = value;
    _metricStats[name].count++;
    _metricStats[name].lastUpdate = metric.timestamp;
}

void MetricMonitor::updateMetric(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(_mutex);
    
    auto it = _metricStats.find(name);
    if (it != _metricStats.end()) {
        it->second.value = value;
        it->second.lastUpdate = std::chrono::high_resolution_clock::now();
    } else {
        addMetric(name, value, "unknown");
    }
}

const std::vector<MetricMonitor::MetricData>& MetricMonitor::getMetrics() const {
    return _metrics;
}

const std::map<std::string, MetricMonitor::MetricStats>& MetricMonitor::getMetricStats() const {
    return _metricStats;
}

void MetricMonitor::generateReport(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open metric report file: " + filePath);
        return;
    }
    
    file << "=== NLM Metric Monitor Report ===\n";
    file << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    // Group metrics by category
    std::map<std::string, std::vector<MetricData>> metricsByCategory;
    for (const auto& metric : _metrics) {
        metricsByCategory[metric.category].push_back(metric);
    }
    
    for (const auto& pair : metricsByCategory) {
        file << "Category: " << pair.first << "\n";
        
        for (const auto& metric : pair.second) {
            file << "  " << metric.name << ": " << std::fixed << std::setprecision(6) 
                 << metric.value << " (thread: " << metric.threadId << ")\n";
        }
        
        file << "\n";
    }
    
    // Generate summary statistics
    file << "=== Summary Statistics ===\n";
    
    for (const auto& pair : _metricStats) {
        file << "Metric: " << pair.first << "\n";
        file << "  Current Value: " << std::fixed << std::setprecision(6) << pair.second.value << "\n";
        file << "  Updates: " << pair.second.count << "\n";
        file << "  Last Update: " << getTimestampString(pair.second.lastUpdate) << "\n";
        file << "\n";
    }
    
    file.close();
    
    NLM_LOG_INFO("Metric monitor report generated: " + filePath);
}

std::string MetricMonitor::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string MetricMonitor::getTimestampString(const std::chrono::high_resolution_clock::time_point& timestamp) {
    auto duration = std::chrono::high_resolution_clock::now() - timestamp;
    double seconds = std::chrono::duration<double>(duration).count();
    
    if (seconds < 60.0) {
        return std::to_string(static_cast<int>(seconds)) + " seconds ago";
    } else if (seconds < 3600.0) {
        return std::to_string(static_cast<int>(seconds / 60.0)) + " minutes ago";
    } else {
        return std::to_string(static_cast<int>(seconds / 3600.0)) + " hours ago";
    }
}

} // namespace nlm