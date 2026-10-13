# Performance Monitoring Tools for NLM
# This file contains performance monitoring and analysis utilities

#include "PerformanceMonitor.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <thread>
#include <numeric>
#include <cmath>
#include <fstream>
#include <sstream>
#include <atomic>
#include <mutex>

namespace nlm {

struct PerformanceMonitor::Impl {
    std::shared_ptr<Config> config;
    std::atomic<bool> running;
    std::thread monitoringThread;
    std::mutex dataMutex;
    
    // Performance metrics
    struct MetricsData {
        double cpuUsage;
        double memoryUsage;
        double networkUsage;
        std::vector<double> neuronFiringRates;
        std::vector<double> synapticActivity;
        std::vector<double> energyConsumption;
        std::chrono::system_clock::time_point timestamp;
        
        MetricsData() : cpuUsage(0.0), memoryUsage(0.0), networkUsage(0.0), 
                       timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<MetricsData> metricsHistory;
    std::vector<double> memoryUsageHistory;
    std::vector<double> spikeRateHistory;
    std::vector<double> energyUsageHistory;
    
    // Visualization data
    struct VisualizationData {
        std::vector<double> timePoints;
        std::vector<double> memoryUsage;
        std::vector<double> cpuUsage;
        std::vector<double> spikeRate;
        std::vector<double> energyUsage;
    };
    
    // Bottleneck detection
    struct BottleneckInfo {
        std::string type;
        std::string description;
        double severity;
        std::string recommendation;
    };
    
    std::vector<BottleneckInfo> bottlenecks;
    
    Impl() : running(false) {}
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}
PerformanceMonitor::~PerformanceMonitor() {
    stopMonitoring();
}

void PerformanceMonitor::setConfig(std::shared_ptr<Config> config) {
    pImpl->config = config;
}

void PerformanceMonitor::startMonitoring() {
    if (pImpl->running) {
        return;
    }
    
    pImpl->running = true;
    pImpl->monitoringThread = std::thread(&PerformanceMonitor::monitoringLoop, this);
    
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMonitor::stopMonitoring() {
    if (!pImpl->running) {
        return;
    }
    
    pImpl->running = false;
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringThread.join();
    }
    
    NLM_LOG_INFO("Performance monitoring stopped");
}

void PerformanceMonitor::pauseMonitoring(bool paused) {
    // This is a placeholder - actual implementation would use condition variables
    if (paused) {
        NLM_LOG_INFO("Performance monitoring paused");
    } else {
        NLM_LOG_INFO("Performance monitoring resumed");
    }
}

void PerformanceMonitor::monitoringLoop() {
    while (pImpl->running) {
        collectMetrics();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Collect every 100ms
    }
}

void PerformanceMonitor::collectMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    Impl::MetricsData data;
    
    // Collect system metrics
    data.cpuUsage = getCpuUsage();
    data.memoryUsage = getMemoryUsage();
    data.networkUsage = getNetworkUsage();
    
    // Collect neural metrics
    data.neuronFiringRates = getNeuronFiringRates();
    data.synapticActivity = getSynapticActivity();
    data.energyConsumption = getEnergyConsumption();
    
    pImpl->metricsHistory.push_back(data);
    
    // Keep history limited
    if (pImpl->metricsHistory.size() > 1000) {
        pImpl->metricsHistory.erase(pImpl->metricsHistory.begin());
    }
    
    // Update history vectors
    pImpl->memoryUsageHistory.push_back(data.memoryUsage);
    pImpl->spikeRateHistory.push_back(calculateAverageFiringRate(data.neuronFiringRates));
    pImpl->energyUsageHistory.push_back(calculateAverageEnergy(data.energyConsumption));
    
    if (pImpl->memoryUsageHistory.size() > 1000) {
        pImpl->memoryUsageHistory.erase(pImpl->memoryUsageHistory.begin());
    }
    if (pImpl->spikeRateHistory.size() > 1000) {
        pImpl->spikeRateHistory.erase(pImpl->spikeRateHistory.begin());
    }
    if (pImpl->energyUsageHistory.size() > 1000) {
        pImpl->energyUsageHistory.erase(pImpl->energyUsageHistory.begin());
    }
}

double PerformanceMonitor::getCpuUsage() {
    // Simplified CPU usage calculation
    // In a real implementation, this would read from system files or use platform-specific APIs
    return 0.0; // Placeholder
}

double PerformanceMonitor::getMemoryUsage() {
    // Simplified memory usage calculation
    // In a real implementation, this would read from system files or use platform-specific APIs
    return 0.0; // Placeholder
}

double PerformanceMonitor::getNetworkUsage() {
    // Simplified network usage calculation
    // In a real implementation, this would read from system files or use platform-specific APIs
    return 0.0; // Placeholder
}

std::vector<double> PerformanceMonitor::getNeuronFiringRates() {
    // This would need access to the brain object
    // For now, return empty vector
    return std::vector<double>();
}

std::vector<double> PerformanceMonitor::getSynapticActivity() {
    // This would need access to the brain object
    // For now, return empty vector
    return std::vector<double>();
}

std::vector<double> PerformanceMonitor::getEnergyConsumption() {
    // This would need access to the brain object
    // For now, return empty vector
    return std::vector<double>();
}

double PerformanceMonitor::calculateAverageFiringRate(const std::vector<double>& rates) {
    if (rates.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(rates.begin(), rates.end(), 0.0);
    return sum / rates.size();
}

double PerformanceMonitor::calculateAverageEnergy(const std::vector<double>& energy) {
    if (energy.empty()) {
        return 0.0;
    }
    double sum = std::accumulate(energy.begin(), energy.end(), 0.0);
    return sum / energy.size();
}

std::vector<double> PerformanceMonitor::getTimePoints() const {
    std::vector<double> times;
    for (size_t i = 0; i < pImpl->metricsHistory.size(); ++i) {
        double timeSeconds = i * 0.1; // 100ms intervals
        times.push_back(timeSeconds);
    }
    return times;
}

std::vector<double> PerformanceMonitor::getMemoryUsageHistory() const {
    return pImpl->memoryUsageHistory;
}

std::vector<double> PerformanceMonitor::getSpikeRateHistory() const {
    return pImpl->spikeRateHistory;
}

std::vector<double> PerformanceMonitor::getEnergyUsageHistory() const {
    return pImpl->energyUsageHistory;
}

// Real-time performance metrics visualization
bool PerformanceMonitor::generateRealTimeVisualization(
    const std::string& outputFile,
    const std::string& format,
    double timeWindow) {
    
    NLM_LOG_INFO("Generating real-time performance visualization: " + outputFile);
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return false;
    }
    
    // Generate HTML visualization
    if (format == "html") {
        generateHtmlVisualization(file, timeWindow);
    } else if (format == "svg") {
        generateSvgVisualization(file, timeWindow);
    } else if (format == "csv") {
        generateCsvVisualization(file, timeWindow);
    } else {
        NLM_LOG_ERROR("Unsupported visualization format: " + format);
        return false;
    }
    
    file.close();
    return true;
}

void PerformanceMonitor::generateHtmlVisualization(
    std::ofstream& file,
    double timeWindow) {
    
    file << "<!DOCTYPE html>\n";
    file << "<html>\n";
    file << "<head>\n";
    file << "<title>NLM Performance Visualization</title>\n";
    file << "<script src=\"https://cdn.plot.ly/plotly-latest.min.js\"></script>\n";
    file << "<style>\n";
    file << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    file << ".plot { margin-bottom: 20px; }\n";
    file << "</style>\n";
    file << "</head>\n";
    file << "<body>\n";
    file << "<h1>NLM Performance Monitoring</h1>\n";
    file << "\n";
    
    // Memory usage plot
    file << "<div id=\"memoryPlot\" class=\"plot\"></div>\n";
    
    // Spike rate plot
    file << "<div id=\"spikeRatePlot\" class=\"plot\"></div>\n";
    
    // CPU and network usage plot
    file << "<div id=\"systemResourcesPlot\" class=\"plot\"></div>\n";
    
    // Energy consumption plot
    file << "<div id=\"energyPlot\" class=\"plot\"></div>\n";
    
    file << "<script>\n";
    file << "// Generate plots using Plotly\n";
    file << "const timePoints = " << getJavaScriptArray(getTimePoints()) << ";\n";
    file << "const memoryData = {\n";
    file << "  x: timePoints,\n";
    file << "  y: " << getJavaScriptArray(getMemoryUsageHistory()) << ",\n";
    file << "  type: 'scatter',\n";
    file << "  name: 'Memory Usage',\n";
    file << "  line: {color: '#1f77b4'}\n";
    file << "};\n";
    file << "\n";
    file << "const spikeRateData = {\n";
    file << "  x: timePoints,\n";
    file << "  y: " << getJavaScriptArray(getSpikeRateHistory()) << ",\n";
    file << "  type: 'scatter',\n";
    file << "  name: 'Spike Rate',\n";
    file << "  line: {color: '#ff7f0e'}\n";
    file << "};\n";
    file << "\n";
    file << "Plotly.newPlot('memoryPlot', [memoryData], {\n";
    file << "  title: 'Memory Usage Over Time',\n";
    file << "  xaxis: {title: 'Time (seconds)'},\n";
    file << "  yaxis: {title: 'Memory Usage (%)'}\n";
    file << "});\n";
    file << "\n";
    file << "Plotly.newPlot('spikeRatePlot', [spikeRateData], {\n";
    file << "  title: 'Neuron Spike Rate Over Time',\n";
    file << "  xaxis: {title: 'Time (seconds)'},\n";
    file << "  yaxis: {title: 'Spike Rate (Hz)'}\n";
    file << "});\n";
    file << "</script>\n";
    file << "</body>\n";
    file << "</html>\n";
}

void PerformanceMonitor::generateSvgVisualization(
    std::ofstream& file,
    double timeWindow) {
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    file << "<title>NLM Performance Monitoring</title>\n";
    file << "\n";
    
    // SVG plots would go here
    // This is a simplified version - actual implementation would draw proper charts
    
    file << "</svg>\n";
}

void PerformanceMonitor::generateCsvVisualization(
    std::ofstream& file,
    double timeWindow) {
    
    file << "time,memory_usage,spike_rate,cpu_usage,network_usage,energy_usage\n";
    
    size_t minSize = std::min({pImpl->metricsHistory.size(),
                              pImpl->memoryUsageHistory.size(),
                              pImpl->spikeRateHistory.size(),
                              pImpl->energyUsageHistory.size()});
    
    for (size_t i = 0; i < minSize; ++i) {
        double time = i * 0.1; // 100ms intervals
        
        // Note: CPU and network usage are not stored in history vectors
        // In a real implementation, they would be
        double cpu = 0.0;
        double network = 0.0;
        
        file << time << "," 
             << pImpl->memoryUsageHistory[i] << "," 
             << pImpl->spikeRateHistory[i] << "," 
             << cpu << "," 
             << network << "," 
             << pImpl->energyUsageHistory[i] << "\n";
    }
}

// Neural activity patterns analysis
bool PerformanceMonitor::analyzeNeuralActivityPatterns(
    const std::string& outputFile,
    const std::string& analysisType,
    double threshold) {
    
    NLM_LOG_INFO("Analyzing neural activity patterns: " + analysisType);
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return false;
    }
    
    file << "Neural Activity Pattern Analysis\n";
    file << "=============================\n\n";
    
    // Get neural activity data
    auto firingRates = getNeuronFiringRates();
    auto synapticActivity = getSynapticActivity();
    
    if (firingRates.empty()) {
        file << "No neural activity data available\n";
        file.close();
        return true;
    }
    
    if (analysisType == "burst_detection") {
        analyzeBurstPatterns(file, firingRates, threshold);
    } else if (analysisType == "oscillation_detection") {
        analyzeOscillationPatterns(file, firingRates, threshold);
    } else if (analysisType == "correlation") {
        analyzeNeuralCorrelations(file, firingRates, synapticActivity);
    } else if (analysisType == "cluster_analysis") {
        analyzeNeuralClusters(file, firingRates);
    }
    
    file.close();
    return true;
}

void PerformanceMonitor::analyzeBurstPatterns(
    std::ofstream& file,
    const std::vector<double>& firingRates,
    double threshold) {
    
    file << "Burst Pattern Analysis\n";
    file << "-------------------\n\n";
    
    // Count bursts (sequences of high firing rates)
    int burstCount = 0;
    int currentBurstLength = 0;
    
    for (double rate : firingRates) {
        if (rate > threshold) {
            currentBurstLength++;
        } else {
            if (currentBurstLength > 0) {
                burstCount++;
            }
            currentBurstLength = 0;
        }
    }
    
    if (currentBurstLength > 0) {
        burstCount++;
    }
    
    file << "Total bursts detected: " << burstCount << "\n";
    file << "Average burst length: " << (firingRates.size() > 0 ? 
        static_cast<double>(currentBurstLength) / firingRates.size() : 0.0) << "\n";
    file << "Burst threshold: " << threshold << " Hz\n";
}

void PerformanceMonitor::analyzeOscillationPatterns(
    std::ofstream& file,
    const std::vector<double>& firingRates,
    double threshold) {
    
    file << "Oscillation Pattern Analysis\n";
    file << "------------------------\n\n";
    
    // Simple oscillation detection using zero crossings
    int oscillations = 0;
    bool above = false;
    
    for (double rate : firingRates) {
        bool currentlyAbove = rate > threshold;
        
        if (above && !currentlyAbove) {
            oscillations++;
        }
        
        above = currentlyAbove;
    }
    
    file << "Oscillations detected: " << oscillations << "\n";
    file << "Oscillation threshold: " << threshold << " Hz\n";
    file << "Average oscillation frequency: " << 
        (firingRates.size() > 0 ? static_cast<double>(oscillations) / firingRates.size() : 0.0) << " Hz\n";
}

void PerformanceMonitor::analyzeNeuralCorrelations(
    std::ofstream& file,
    const std::vector<double>& firingRates,
    const std::vector<double>& synapticActivity) {
    
    file << "Neural Correlation Analysis\n";
    file << "------------------------\n\n";
    
    if (firingRates.size() != synapticActivity.size()) {
        file << "Firing rate and synaptic activity data have different sizes\n";
        return;
    }
    
    // Calculate correlation coefficient
    double sumX = std::accumulate(firingRates.begin(), firingRates.end(), 0.0);
    double sumY = std::accumulate(synapticActivity.begin(), synapticActivity.end(), 0.0);
    
    double sumXY = 0.0;
    double sumX2 = 0.0;
    double sumY2 = 0.0;
    
    for (size_t i = 0; i < firingRates.size(); ++i) {
        sumXY += firingRates[i] * synapticActivity[i];
        sumX2 += firingRates[i] * firingRates[i];
        sumY2 += synapticActivity[i] * synapticActivity[i];
    }
    
    double n = firingRates.size();
    double correlation = (n * sumXY - sumX * sumY) / 
                         std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
    
    file << "Correlation coefficient: " << correlation << "\n";
    
    if (std::abs(correlation) > 0.7) {
        file << "Strong correlation\n";
    } else if (std::abs(correlation) > 0.3) {
        file << "Moderate correlation\n";
    } else {
        file << "Weak correlation\n";
    }
}

void PerformanceMonitor::analyzeNeuralClusters(
    std::ofstream& file,
    const std::vector<double>& firingRates) {
    
    file << "Neural Cluster Analysis\n";
    file << "----------------------\n\n";
    
    // Simple clustering based on firing rate thresholds
    std::vector<double> highFiringRates;
    std::vector<double> lowFiringRates;
    
    double threshold = *std::max_element(firingRates.begin(), firingRates.end()) * 0.5;
    
    for (double rate : firingRates) {
        if (rate > threshold) {
            highFiringRates.push_back(rate);
        } else {
            lowFiringRates.push_back(rate);
        }
    }
    
    file << "High firing rate neurons: " << highFiringRates.size() << "\n";
    file << "Low firing rate neurons: " << lowFiringRates.size() << "\n";
    file << "High firing rate threshold: " << threshold << " Hz\n";
    
    // Calculate cluster statistics
    if (!highFiringRates.empty()) {
        double sum = std::accumulate(highFiringRates.begin(), highFiringRates.end(), 0.0);
        double mean = sum / highFiringRates.size();
        file << "High firing rate cluster mean: " << mean << " Hz\n";
    }
    
    if (!lowFiringRates.empty()) {
        double sum = std::accumulate(lowFiringRates.begin(), lowFiringRates.end(), 0.0);
        double mean = sum / lowFiringRates.size();
        file << "Low firing rate cluster mean: " << mean << " Hz\n";
    }
}

// Bottleneck identification tools
bool PerformanceMonitor::identifyPerformanceBottlenecks(
    const std::string& outputFile,
    const std::string& analysisType) {
    
    NLM_LOG_INFO("Identifying performance bottlenecks: " + analysisType);
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return false;
    }
    
    // Clear existing bottlenecks
    pImpl->bottlenecks.clear();
    
    // Analyze bottlenecks based on different criteria
    if (analysisType == "comprehensive" || analysisType == "all") {
        detectMemoryBottlenecks();
        detectCpuBottlenecks();
        detectNetworkBottlenecks();
        detectNeuralBottlenecks();
    } else {
        if (analysisType == "memory") {
            detectMemoryBottlenecks();
        } else if (analysisType == "cpu") {
            detectCpuBottlenecks();
        } else if (analysisType == "network") {
            detectNetworkBottlenecks();
        } else if (analysisType == "neural") {
            detectNeuralBottlenecks();
        }
    }
    
    // Generate bottleneck report
    file << "Performance Bottleneck Analysis\n";
    file << "=============================\n\n";
    
    if (pImpl->bottlenecks.empty()) {
        file << "No performance bottlenecks detected.\n";
    } else {
        file << "Detected " << pImpl->bottlenecks.size() << " performance bottleneck(s):\n\n";
        
        for (size_t i = 0; i < pImpl->bottlenecks.size(); ++i) {
            const auto& bottleneck = pImpl->bottlenecks[i];
            file << (i + 1) << ". " << bottleneck.type << "\n";
            file << "   Description: " << bottleneck.description << "\n";
            file << "   Severity: " << bottleneck.severity << "\n";
            file << "   Recommendation: " << bottleneck.recommendation << "\n\n";
        }
    }
    
    file.close();
    return true;
}

void PerformanceMonitor::detectMemoryBottlenecks() {
    if (pImpl->memoryUsageHistory.empty()) {
        return;
    }
    
    double maxMemory = *std::max_element(pImpl->memoryUsageHistory.begin(), 
                                        pImpl->memoryUsageHistory.end());
    
    Impl::BottleneckInfo bottleneck;
    bottleneck.type = "Memory Usage";
    bottleneck.severity = maxMemory / 100.0; // Assuming 100% is the limit
    
    if (maxMemory > 80.0) {
        bottleneck.description = "High memory usage detected, potentially causing swapping or memory pressure";
        bottleneck.recommendation = "Consider reducing neural population size or using memory-efficient representations";
    } else if (maxMemory > 60.0) {
        bottleneck.description = "Moderate memory usage, approaching limits";
        bottleneck.recommendation = "Monitor memory usage and consider optimizations";
    }
    
    pImpl->bottlenecks.push_back(bottleneck);
}

void PerformanceMonitor::detectCpuBottlenecks() {
    // This would use actual CPU usage data
    // For now, it's a placeholder
    
    double cpuUsage = getCpuUsage();
    
    if (cpuUsage > 80.0) {
        Impl::BottleneckInfo bottleneck;
        bottleneck.type = "CPU Usage";
        bottleneck.severity = cpuUsage / 100.0;
        bottleneck.description = "High CPU usage, potentially indicating computational bottlenecks";
        bottleneck.recommendation = "Consider parallel processing or optimizing neural computations";
        pImpl->bottlenecks.push_back(bottleneck);
    }
}

void PerformanceMonitor::detectNetworkBottlenecks() {
    // This would use actual network usage data
    // For now, it's a placeholder
    
    double networkUsage = getNetworkUsage();
    
    if (networkUsage > 80.0) {
        Impl::BottleneckInfo bottleneck;
        bottleneck.type = "Network Usage";
        bottleneck.severity = networkUsage / 100.0;
        bottleneck.description = "High network usage, potentially indicating communication bottlenecks";
        bottleneck.recommendation = "Consider reducing network I/O or using batch processing";
        pImpl->bottlenecks.push_back(bottleneck);
    }
}

void PerformanceMonitor::detectNeuralBottlenecks() {
    auto firingRates = getNeuronFiringRates();
    
    if (firingRates.empty()) {
        return;
    }
    
    double maxFiringRate = *std::max_element(firingRates.begin(), firingRates.end());
    
    if (maxFiringRate > 1000.0) { // 1000 Hz threshold
        Impl::BottleneckInfo bottleneck;
        bottleneck.type = "Neural Firing Rate";
        bottleneck.severity = std::min(1.0, maxFiringRate / 5000.0); // Scale to 0-1
        bottleneck.description = "Extremely high firing rates, potentially causing instability";
        bottleneck.recommendation = "Consider reducing excitability or implementing refractory period limits";
        pImpl->bottlenecks.push_back(bottleneck);
    }
}

// Helper methods
std::string PerformanceMonitor::getJavaScriptArray(const std::vector<double>& data) const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < data.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << data[i];
    }
    oss << "]";
    return oss.str();
}

} // namespace nlm
