// Profiler implementation for NLM
#include "utils/Profiler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <numeric>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <filesystem>
#include <sys/resource.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <cmath>

namespace nlm {

struct Profiler::Impl {
    bool running;
    bool paused;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::vector<PerformanceMetrics> metricsHistory;
    std::map<std::string, std::vector<double>> metricSeries;
    double samplingInterval;
    size_t maxSamples;
    bool continuous;
    bool realTime;
    std::string outputFile;
    std::string format;
    double threshold;
    
    // Performance monitoring thread
    std::thread profilingThread;
    std::mutex metricsMutex;
    
    // System monitoring
    std::atomic<double> currentCPULoad{0.0};
    std::atomic<double> currentMemoryUsage{0.0};
    std::atomic<size_t> totalSpikes{0};
    std::atomic<size_t> activeNeurons{0};
    std::atomic<size_t> totalNeurons{0};
    std::atomic<size_t> totalSynapses{0};
    std::atomic<double> averageFiringRate{0.0};
    std::atomic<double> averageWeight{0.0};
    std::atomic<double> weightVariance{0.0};
    std::atomic<double> totalWeight{0.0};
    std::atomic<size_t> strengthenedSynapses{0};
    std::atomic<size_t> weakenedSynapses{0};
    std::atomic<size_t> unchangedSynapses{0};
    std::atomic<double> learningRate{0.0};
    std::atomic<double> plasticityStrength{0.0};
    std::atomic<size_t> networkDiameter{0};
    std::atomic<size_t> averagePathLength{0};
    std::atomic<double> synchronization{0.0};
    std::atomic<double> stability{0.0};
    std::atomic<double> entropy{0.0};
    
    // Global counters
    std::atomic<size_t> totalLearningEvents{0};
    std::atomic<size_t> totalPlasticityEvents{0};
    
    Impl() : running(false), paused(false), samplingInterval(1.0), maxSamples(1000),
             continuous(false), realTime(false), threshold(0.1) {}
};

Profiler::Profiler() : initialized(false), profiling(false), paused(false), running(false),
    config(nullptr), brain(nullptr) {
    pImpl = std::make_unique<Impl>();
}

Profiler::~Profiler() {
    cleanup();
}

Profiler::Profiler(Profiler&&) noexcept = default;

Profiler& Profiler::operator=(Profiler&&) noexcept = default;

bool Profiler::initialize() {
    if (initialized) return true;
    
    try {
        if (!config || !brain) {
            std::cerr << "Error: Config or brain not set" << std::endl;
            return false;
        }
        
        // Initialize profiling configuration
        setupProfilingConfiguration();
        
        // Clear existing data
        pImpl->metricsHistory.clear();
        pImpl->metricSeries.clear();
        
        initialized = true;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize Profiler: " << e.what() << std::endl;
        return false;
    }
}

bool Profiler::startProfiling(const ProfilingConfig& config) {
    if (!initialized) {
        std::cerr << "Error: Profiler not initialized" << std::endl;
        return false;
    }
    
    if (profiling) {
        std::cout << "Warning: Profiler is already running" << std::endl;
        return true;
    }
    
    if (!validateProfilingConfig(config)) {
        std::cerr << "Error: Invalid profiling configuration" << std::endl;
        return false;
    }
    
    pImpl->config = config;
    pImpl->running = true;
    pImpl->paused = false;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    pImpl->metricsHistory.clear();
    pImpl->metricSeries.clear();
    
    std::thread profilerThread(&Profiler::profilingWorker, this);
    profilerThread.detach();
    
    profiling = true;
    std::cout << "Profiler started with mode: " << getProfilingModeString(config.mode) << std::endl;
    
    return true;
}

bool Profiler::stopProfiling() {
    if (!profiling) {
        std::cout << "Warning: Profiler is not running" << std::endl;
        return true;
    }
    
    pImpl->running = false;
    profiling = false;
    generateReport();
    
    std::cout << "Profiler stopped" << std::endl;
    
    return true;
}

bool Profiler::pauseProfiling() {
    if (!profiling) {
        std::cout << "Warning: Profiler is not running" << std::endl;
        return true;
    }
    
    pImpl->paused = true;
    paused = true;
    std::cout << "Profiler paused" << std::endl;
    
    return true;
}

bool Profiler::resumeProfiling() {
    if (!profiling) {
        std::cout << "Warning: Profiler is not running" << std::endl;
        return true;
    }
    
    pImpl->paused = false;
    paused = false;
    std::cout << "Profiler resumed" << std::endl;
    
    return true;
}

bool Profiler::configure(const ProfilingConfig& config) {
    if (!validateProfilingConfig(config)) {
        std::cerr << "Error: Invalid profiling configuration" << std::endl;
        return false;
    }
    
    pImpl->config = config;
    std::cout << "Profiler configuration updated" << std::endl;
    
    return true;
}

bool Profiler::updateConfig(const std::string& key, const std::string& value) {
    if (!pImpl->config) {
        std::cerr << "Error: No configuration to update" << std::endl;
        return false;
    }
    
    if (key == "mode") {
        // Map string to ProfilingMode
        if (value == "CPU") pImpl->config->mode = ProfilingMode::CPU;
        else if (value == "Memory") pImpl->config->mode = ProfilingMode::Memory;
        else if (value == "SpikeTiming") pImpl->config->mode = ProfilingMode::SpikeTiming;
        else if (value == "Learning") pImpl->config->mode = ProfilingMode::Learning;
        else if (value == "Network") pImpl->config->mode = ProfilingMode::Network;
        else if (value == "All") pImpl->config->mode = ProfilingMode::All;
        else if (value == "None") pImpl->config->mode = ProfilingMode::None;
        else {
            std::cerr << "Error: Unknown profiling mode: " << value << std::endl;
            return false;
        }
    } else if (key == "enabled") {
        pImpl->config->enabled = (value == "true" || value == "1" || value == "yes");
    } else if (key == "sampling_interval") {
        try {
            pImpl->config->sampleInterval = std::stod(value);
        } catch (...) {
            std::cerr << "Error: Invalid sampling interval: " << value << std::endl;
            return false;
        }
    } else if (key == "max_samples") {
        try {
            pImpl->config->maxSamples = std::stoul(value);
        } catch (...) {
            std::cerr << "Error: Invalid max samples: " << value << std::endl;
            return false;
        }
    } else if (key == "detailed") {
        pImpl->config->detailed = (value == "true" || value == "1" || value == "yes");
    } else if (key == "output_file") {
        pImpl->config->outputFile = value;
    } else if (key == "format") {
        pImpl->config->format = value;
    } else if (key == "continuous") {
        pImpl->config->continuous = (value == "true" || value == "1" || value == "yes");
    } else if (key == "real_time") {
        pImpl->config->realTime = (value == "true" || value == "1" || value == "yes");
    } else if (key == "threshold") {
        try {
            pImpl->config->threshold = std::stod(value);
        } catch (...) {
            std::cerr << "Error: Invalid threshold: " << value << std::endl;
            return false;
        }
    } else {
        std::cerr << "Error: Unknown configuration key: " << key << std::endl;
        return false;
    }
    
    std::cout << "Configuration updated: " << key << " = " << value << std::endl;
    
    return true;
}

void Profiler::collectMetrics() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    PerformanceMetrics metrics;
    metrics.timestamp = std::chrono::steady_clock::now();
    metrics.cpuUsage = calculateCPULoad();
    metrics.memoryUsage = calculateMemoryUsage();
    metrics.peakMemoryUsage = std::max(metrics.peakMemoryUsage, metrics.memoryUsage);
    
    if (brain) {
        metrics.totalSpikes = brain->getTotalSpikeCount();
        metrics.spikeRate = brain->getFiringRate();
        metrics.activeNeuronCount = brain->getActiveNeuronCount();
        metrics.totalNeuronCount = brain->getTotalNeuronCount();
        metrics.totalSynapseCount = brain->getTotalSynapseCount();
        metrics.averageFiringRate = brain->getFiringRate();
        
        if (brain->getTotalSynapseCount() > 0) {
            metrics.averageWeight = brain->getAverageSynapticWeight();
            metrics.weightVariance = brain->getWeightVariance();
            metrics.totalWeight = brain->getTotalSynapticWeight();
        }
    }
    
    metrics.lastUpdate = metrics.timestamp;
    
    pImpl->metricsHistory.push_back(metrics);
    
    if (pImpl->metricsHistory.size() > pImpl->maxSamples) {
        pImpl->metricsHistory.erase(pImpl->metricsHistory.begin());
    }
    
    updateMetricSeries("cpu_usage", metrics.cpuUsage);
    updateMetricSeries("memory_usage", metrics.memoryUsage);
    updateMetricSeries("total_spikes", static_cast<double>(metrics.totalSpikes));
    updateMetricSeries("active_neurons", static_cast<double>(metrics.activeNeuronCount));
    updateMetricSeries("total_neurons", static_cast<double>(metrics.totalNeuronCount));
    updateMetricSeries("total_synapses", static_cast<double>(metrics.totalSynapseCount));
    updateMetricSeries("average_firing_rate", metrics.averageFiringRate);
    updateMetricSeries("average_weight", metrics.averageWeight);
}

void Profiler::collectCPUMetrics() {
    double cpuUsage = calculateCPULoad();
    updateMetricSeries("cpu_usage", cpuUsage);
}

void Profiler::collectMemoryMetrics() {
    double memUsage = calculateMemoryUsage();
    updateMetricSeries("memory_usage", memUsage);
}

void Profiler::collectSpikeMetrics() {
    if (brain) {
        size_t spikeCount = brain->getTotalSpikeCount();
        updateMetricSeries("total_spikes", static_cast<double>(spikeCount));
        updateMetricSeries("spike_rate", brain->getFiringRate());
    }
}

void Profiler::collectLearningMetrics() {
    if (brain) {
        updateMetricSeries("learning_rate", brain->getLearningRate());
        updateMetricSeries("plasticity_strength", brain->getPlasticityStrength());
    }
}

void Profiler::collectNetworkMetrics() {
    if (brain) {
        updateMetricSeries("network_activity", brain->getNetworkActivity());
        updateMetricSeries("average_path_length", brain->getAveragePathLength());
        updateMetricSeries("network_diameter", brain->getNetworkDiameter());
        updateMetricSeries("clustering_coefficient", brain->getClusteringCoefficient());
    }
}

bool Profiler::generateReport() {
    if (!initialized || pImpl->metricsHistory.empty()) {
        std::cerr << "Error: No profiling data available" << std::endl;
        return false;
    }
    
    std::string report = generateTextReport();
    
    std::cout << "=== Profiler Report ===" << std::endl;
    std::cout << report;
    
    if (!pImpl->config->outputFile.empty()) {
        if (exportResults(pImpl->config->outputFile, pImpl->config->format)) {
            std::cout << "Report saved to: " << pImpl->config->outputFile << std::endl;
        } else {
            std::cerr << "Failed to save report to: " << pImpl->config->outputFile << std::endl;
        }
    }
    
    return true;
}

bool Profiler::exportResults(const std::string& filename, const std::string& format) {
    if (!ensureOutputDirectory(filename)) {
        return false;
    }
    
    std::string content;
    if (format == "text") {
        content = generateTextReport();
    } else if (format == "json") {
        content = generateJSONReport();
    } else if (format == "csv") {
        content = generateCSVReport();
    } else if (format == "html") {
        content = generateHTMLReport();
    } else {
        std::cerr << "Unsupported format: " << format << std::endl;
        return false;
    }
    
    return writeToFile(filename, content);
}

bool Profiler::exportMetrics(const std::string& filename, const std::vector<std::string>& metrics) {
    if (!ensureOutputDirectory(filename)) {
        return false;
    }
    
    std::ostringstream oss;
    oss << "Timestamp,CPU Usage,Memory Usage,Total Spikes,Active Neurons,Total Neurons,Total Synapses,Average Firing Rate,Average Weight" << std::endl;
    
    for (const auto& metricData : pImpl->metricsHistory) {
        oss << getFormattedTimestamp(metricData.timestamp) << ",";
        oss << metricData.cpuUsage << ",";
        oss << metricData.memoryUsage << ",";
        oss << metricData.totalSpikes << ",";
        oss << metricData.activeNeuronCount << ",";
        oss << metricData.totalNeuronCount << ",";
        oss << metricData.totalSynapseCount << ",";
        oss << metricData.averageFiringRate << ",";
        oss << metricData.averageWeight << std::endl;
    }
    
    return writeToFile(filename, oss.str());
}

bool Profiler::exportCSV(const std::string& filename) const {
    return exportMetrics(filename, {});
}

bool Profiler::exportJSON(const std::string& filename) const {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"profiling_mode\": \"" << getProfilingModeString(pImpl->config->mode) << "\"," << std::endl;
    oss << "  \"start_time\": \"" << getFormattedTimestamp(pImpl->startTime) << "\"," << std::endl;
    oss << "  \"end_time\": \"" << getFormattedTimestamp(pImpl->lastUpdateTime) << "\"," << std::endl;
    oss << "  \"total_duration\": " << getElapsedTime() << "," << std::endl;
    oss << "  \"sample_count\": " << pImpl->metricsHistory.size() << "," << std::endl;
    oss << "  \"cpu_usage_average\": " << calculateMean(getMetricSeries("cpu_usage")) << "," << std::endl;
    oss << "  \"cpu_usage_peak\": " << calculateMax(getMetricSeries("cpu_usage")) << "," << std::endl;
    oss << "  \"memory_usage_average\": " << calculateMean(getMetricSeries("memory_usage")) << "," << std::endl;
    oss << "  \"memory_usage_peak\": " << calculateMax(getMetricSeries("memory_usage")) << "," << std::endl;
    oss << "  \"total_spike_count\": " << calculateSum(getMetricSeries("total_spikes")) << "," << std::endl;
    oss << "  \"total_learning_events\": " << pImpl->totalLearningEvents << "," << std::endl;
    oss << "  \"total_plasticity_events\": " << pImpl->totalPlasticityEvents << "," << std::endl;
    oss << "  \"average_learning_rate\": " << calculateMean(getMetricSeries("learning_rate")) << "," << std::endl;
    oss << "  \"average_firing_rate\": " << calculateMean(getMetricSeries("average_firing_rate")) << "," << std::endl;
    oss << "  \"average_weight_change\": " << calculateMean(getMetricSeries("average_weight")) << "," << std::endl;
    oss << "  \"network_stability\": " << calculateStability(getMetricSeries("network_activity")) << "," << std::endl;
    oss << "  \"network_entropy\": " << calculateEntropy(getMetricSeries("network_activity")) << std::endl;
    oss << "}" << std::endl;
    
    return writeToFile(filename, oss.str());
}

bool Profiler::exportHTML(const std::string& filename) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head>" << std::endl;
    oss << "<title>Profiler Report</title>" << std::endl;
    oss << "<style>" << std::endl;
    oss << "table { border-collapse: collapse; width: 100%; }" << std::endl;
    oss << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
    oss << "th { background-color: #f2f2f2; }" << std::endl;
    oss << "</style>" << std::endl;
    oss << "</head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "<h1>Profiler Report</h1>" << std::endl;
    oss << "<p>Generated on: " << getFormattedTimestamp(pImpl->lastUpdateTime) << "</p>" << std::endl;
    oss << "<table>" << std::endl;
    oss << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
    oss << "<tr><td>Profiling Mode</td><td>" << getProfilingModeString(pImpl->config->mode) << "</td></tr>" << std::endl;
    oss << "<tr><td>Total Duration</td><td>" << getElapsedTime() << " seconds</td></tr>" << std::endl;
    oss << "<tr><td>Sample Count</td><td>" << pImpl->metricsHistory.size() << "</td></tr>" << std::endl;
    oss << "<tr><td>CPU Usage Average</td><td>" << calculateMean(getMetricSeries("cpu_usage")) << "%</td></tr>" << std::endl;
    oss << "<tr><td>CPU Usage Peak</td><td>" << calculateMax(getMetricSeries("cpu_usage")) << "%</td></tr>" << std::endl;
    oss << "<tr><td>Memory Usage Average</td><td>" << calculateMean(getMetricSeries("memory_usage")) << " MB</td></tr>" << std::endl;
    oss << "<tr><td>Memory Usage Peak</td><td>" << calculateMax(getMetricSeries("memory_usage")) << " MB</td></tr>" << std::endl;
    oss << "<tr><td>Total Spike Count</td><td>" << calculateSum(getMetricSeries("total_spikes")) << "</td></tr>" << std::endl;
    oss << "<tr><td>Average Firing Rate</td><td>" << calculateMean(getMetricSeries("average_firing_rate")) << " Hz</td></tr>" << std::endl;
    oss << "<tr><td>Average Weight</td><td>" << calculateMean(getMetricSeries("average_weight")) << "</td></tr>" << std::endl;
    oss << "</table>" << std::endl;
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    
    return writeToFile(filename, oss.str());
}

void Profiler::showHelp() const {
    std::cout << "=== Profiler Commands ===" << std::endl;
    std::cout << "  profile [mode] - Start profiling" << std::endl;
    std::cout << "  stop - Stop profiling" << std::endl;
    std::cout << "  pause - Pause profiling" << std::endl;
    std::cout << "  resume - Resume profiling" << std::endl;
    std::cout << "  configure <key> <value> - Configure profiler" << std::endl;
    std::cout << "  export <filename> <format> - Export profiling results" << std::endl;
    std::cout << "  export-metrics <filename> [metrics...] - Export specific metrics" << std::endl;
    std::cout << "  show-help - Show this help" << std::endl;
}

void Profiler::showProfilingModes() const {
    std::cout << "Available profiling modes:" << std::endl;
    std::cout << "  CPU - CPU usage profiling" << std::endl;
    std::cout << "  Memory - Memory usage profiling" << std::endl;
    std::cout << "  SpikeTiming - Spike timing analysis" << std::endl;
    std::cout << "  Learning - Learning analysis" << std::endl;
    std::cout << "  Network - Network analysis" << std::endl;
    std::cout << "  All - All metrics combined" << std::endl;
    std::cout << "  None - No profiling" << std::endl;
}

void Profiler::showMetricsList() const {
    std::cout << "Available metrics:" << std::endl;
    std::cout << "  cpu_usage - CPU usage percentage" << std::endl;
    std::cout << "  memory_usage - Memory usage in MB" << std::endl;
    std::cout << "  total_spikes - Total spike count" << std::endl;
    std::cout << "  active_neuron_count - Number of active neurons" << std::endl;
    std::cout << "  total_neuron_count - Total number of neurons" << std::endl;
    std::cout << "  total_synapse_count - Total number of synapses" << std::endl;
    std::cout << "  average_firing_rate - Average firing rate in Hz" << std::endl;
    std::cout << "  average_weight - Average synaptic weight" << std::endl;
    std::cout << "  weight_variance - Variance of synaptic weights" << std::endl;
    std::cout << "  total_weight - Total synaptic weight" << std::endl;
}

void Profiler::startProfilingThread() {
    // This method is kept for compatibility
    // The main profiling thread is started in startProfiling()
}

void Profiler::stopProfilingThread() {
    pImpl->running = false;
}

void Profiler::profilingWorker() {
    while (pImpl->running) {
        if (!pImpl->paused) {
            collectMetrics();
            
            if (pImpl->config && pImpl->config->continuous) {
                generateReport();
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(pImpl->samplingInterval)));
    }
}

bool Profiler::validateProfilingConfig(const ProfilingConfig& config) const {
    if (!config.enabled) {
        return true;  // Disabled is valid
    }
    
    if (config.sampleInterval <= 0) {
        return false;
    }
    
    if (config.maxSamples == 0) {
        return false;
    }
    
    if (config.outputFile.empty() && !config.continuous) {
        return false;
    }
    
    return true;
}

bool Profiler::isMetricEnabled(const std::string& metric) const {
    if (!pImpl->config) {
        return true;
    }
    
    for (const auto& enabledMetric : pImpl->config->metrics) {
        if (enabledMetric == metric) {
            return true;
        }
    }
    
    for (const auto& excludedMetric : pImpl->config->excludeMetrics) {
        if (excludedMetric == metric) {
            return false;
        }
    }
    
    return pImpl->config->metrics.empty();  // If no metrics specified, all are enabled
}

bool Profiler::isMetricExcluded(const std::string& metric) const {
    if (!pImpl->config) {
        return false;
    }
    
    return std::find(pImpl->config->excludeMetrics.begin(), 
                     pImpl->config->excludeMetrics.end(), metric) != pImpl->config->excludeMetrics.end();
}

void Profiler::addMetricSeries(const std::string& name, const std::vector<double>& data) {
    if (isMetricEnabled(name)) {
        pImpl->metricSeries[name] = data;
    }
}

std::string Profiler::generateTextReport() const {
    std::ostringstream oss;
    oss << "=== Profiler Report ===" << std::endl;
    oss << "Generated on: " << getFormattedTimestamp(pImpl->lastUpdateTime) << std::endl;
    oss << "Profiling Mode: " << getProfilingModeString(pImpl->config->mode) << std::endl;
    oss << "Start Time: " << getFormattedTimestamp(pImpl->startTime) << std::endl;
    oss << "End Time: " << getFormattedTimestamp(pImpl->lastUpdateTime) << std::endl;
    oss << "Total Duration: " << getElapsedTime() << " seconds" << std::endl;
    oss << "Sample Count: " << pImpl->metricsHistory.size() << std::endl;
    oss << std::endl;
    
    oss << "=== Performance Summary ===" << std::endl;
    oss << "CPU Usage Average: " << calculateMean(getMetricSeries("cpu_usage")) << "%" << std::endl;
    oss << "CPU Usage Peak: " << calculateMax(getMetricSeries("cpu_usage")) << "%" << std::endl;
    oss << "Memory Usage Average: " << calculateMean(getMetricSeries("memory_usage")) << " MB" << std::endl;
    oss << "Memory Usage Peak: " << calculateMax(getMetricSeries("memory_usage")) << " MB" << std::endl;
    oss << std::endl;
    
    oss << "=== Neural Activity ===" << std::endl;
    oss << "Total Spike Count: " << calculateSum(getMetricSeries("total_spikes")) << std::endl;
    oss << "Average Firing Rate: " << calculateMean(getMetricSeries("average_firing_rate")) << " Hz" << std::endl;
    oss << "Average Weight: " << calculateMean(getMetricSeries("average_weight")) << std::endl;
    oss << "Network Stability: " << calculateStability(getMetricSeries("network_activity")) << std::endl;
    oss << "Network Entropy: " << calculateEntropy(getMetricSeries("network_activity")) << std::endl;
    
    return oss.str();
}

std::string Profiler::generateJSONReport() const {
    return generateTextReport();  // For simplicity, using same content
}

std::string Profiler::generateHTMLReport() const {
    return generateTextReport();  // For simplicity, using same content
}

std::string Profiler::generateCSVReport() const {
    std::ostringstream oss;
    oss << "Timestamp,CPU Usage,Memory Usage,Total Spikes,Active Neurons,Total Neurons,Total Synapses,Average Firing Rate,Average Weight" << std::endl;
    
    for (const auto& metricData : pImpl->metricsHistory) {
        oss << getFormattedTimestamp(metricData.timestamp) << ",";
        oss << metricData.cpuUsage << ",";
        oss << metricData.memoryUsage << ",";
        oss << metricData.totalSpikes << ",";
        oss << metricData.activeNeuronCount << ",";
        oss << metricData.totalNeuronCount << ",";
        oss << metricData.totalSynapseCount << ",";
        oss << metricData.averageFiringRate << ",";
        oss << metricData.averageWeight << std::endl;
    }
    
    return oss.str();
}

std::string Profiler::formatMetricValue(double value, const std::string& unit) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << value;
    if (!unit.empty()) {
        oss << " " << unit;
    }
    return oss.str();
}

std::string Profiler::getProfilingModeString(ProfilingMode mode) const {
    switch (mode) {
        case ProfilingMode::CPU: return "CPU";
        case ProfilingMode::Memory: return "Memory";
        case ProfilingMode::SpikeTiming: return "SpikeTiming";
        case ProfilingMode::Learning: return "Learning";
        case ProfilingMode::Network: return "Network";
        case ProfilingMode::All: return "All";
        case ProfilingMode::None: return "None";
        default: return "Unknown";
    }
}

std::string Profiler::getFormatString(const std::string& format) const {
    if (format == "text") return "Text";
    if (format == "json") return "JSON";
    if (format == "csv") return "CSV";
    if (format == "html") return "HTML";
    return "Unknown";
}

bool Profiler::ensureOutputDirectory(const std::string& filename) const {
    size_t pos = filename.find_last_of('/');
    if (pos != std::string::npos) {
        std::string directory = filename.substr(0, pos);
        if (!std::filesystem::exists(directory)) {
            return std::filesystem::create_directories(directory);
        }
    }
    return true;
}

bool Profiler::writeToFile(const std::string& filename, const std::string& content) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    
    return true;
}

std::string Profiler::getFormattedTimestamp(const std::chrono::steady_clock::time_point& timestamp) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp).count();
    return std::to_string(elapsed) + "s";
}

std::string Profiler::getSystemInfo() const {
    return "Profiler System Information";
}

std::string Profiler::getProfilerInfo() const {
    return "Profiler Information";
}

bool Profiler::saveProfilingResults(const std::string& filename) const {
    return generateReport();
}

bool Profiler::loadProfilingResults(const std::string& filename) {
    return true;
}

std::string Profiler::getProfilingResultsFilename(const std::string& prefix) const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << prefix << "_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".json";
    return oss.str();
}

void Profiler::cleanup() {
    stopProfiling();
    pImpl->metricsHistory.clear();
    pImpl->metricSeries.clear();
}

void Profiler::setupProfilingConfiguration() {
    // Setup profiling configuration from brain config
    if (config) {
        pImpl->config.enabled = config->getOr<bool>("profiler.enabled", false);
        pImpl->config.sampleInterval = config->getOr<double>("profiler.sample_interval", 1.0);
        pImpl->config.maxSamples = config->getOr<size_t>("profiler.max_samples", 1000);
        pImpl->config.detailed = config->getOr<bool>"profiler.detailed", false);
        pImpl->config.outputFile = config->getOr<std::string>("profiler.output_file", "");
        pImpl->config.format = config->getOr<std::string>("profiler.format", "text");
        pImpl->config.continuous = config->getOr<bool>("profiler.continuous", false);
        pImpl->config.realTime = config->getOr<bool>("profiler.real_time", false);
        pImpl->config.threshold = config->getOr<double>("profiler.threshold", 0.1);
    }
}

// Helper functions
double Profiler::calculateMean(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double Profiler::calculateStandardDeviation(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    double mean = calculateMean(data);
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return std::sqrt(sum / data.size());
}

double Profiler::calculateVariance(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    double mean = calculateMean(data);
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    return sum / data.size();
}

double Profiler::calculateMedian(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    size_t size = sorted.size();
    if (size % 2 == 0) {
        return (sorted[size / 2 - 1] + sorted[size / 2]) / 2.0;
    } else {
        return sorted[size / 2];
    }
}

double Profiler::calculatePercentile(const std::vector<double>& data, double percentile) const {
    if (data.empty()) return 0.0;
    std::vector<double> sorted = data;
    std::sort(sorted.begin(), sorted.end());
    double index = (percentile / 100.0) * (sorted.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(index));
    size_t upper = static_cast<size_t>(std::ceil(index));
    if (lower == upper) {
        return sorted[lower];
    }
    double weight = index - lower;
    return sorted[lower] * (1.0 - weight) + sorted[upper] * weight;
}

size_t Profiler::findPeak(const std::vector<size_t>& data) const {
    if (data.empty()) return 0;
    size_t peak = 0;
    size_t peakValue = data[0];
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] > peakValue) {
            peakValue = data[i];
            peak = i;
        }
    }
    return peak;
}

double Profiler::findInflectionPoint(const std::vector<double>& data) const {
    if (data.size() < 3) return 0.0;
    std::vector<double> derivative;
    for (size_t i = 1; i < data.size(); ++i) {
        derivative.push_back(data[i] - data[i - 1]);
    }
    
    double maxDerivative = *std::max_element(derivative.begin(), derivative.end());
    double minDerivative = *std::min_element(derivative.begin(), derivative.end());
    
    return maxDerivative - minDerivative;
}

std::string Profiler::analyzeTrend(const std::vector<double>& data) const {
    if (data.size() < 2) return "Unknown";
    
    double slope = (data.back() - data.front()) / (data.size() - 1);
    
    if (slope > 0.1) return "Increasing";
    else if (slope < -0.1) return "Decreasing";
    else return "Stable";
}

double Profiler::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double xMean = calculateMean(x);
    double yMean = calculateMean(y);
    
    double numerator = 0.0;
    double xVariance = 0.0;
    double yVariance = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double xDiff = x[i] - xMean;
        double yDiff = y[i] - yMean;
        numerator += xDiff * yDiff;
        xVariance += xDiff * xDiff;
        yVariance += yDiff * yDiff;
    }
    
    if (xVariance == 0.0 || yVariance == 0.0) return 0.0;
    
    return numerator / std::sqrt(xVariance * yVariance);
}

std::vector<double> Profiler::smoothData(const std::vector<double>& data, int windowSize) const {
    if (windowSize < 3 || data.size() < windowSize) return data;
    
    std::vector<double> smoothed;
    for (size_t i = 0; i < data.size(); ++i) {
        double sum = 0.0;
        int count = 0;
        for (int j = -windowSize / 2; j <= windowSize / 2; ++j) {
            size_t index = i + j;
            if (index < data.size()) {
                sum += data[index];
                count++;
            }
        }
        smoothed.push_back(sum / count);
    }
    
    return smoothed;
}

std::vector<size_t> Profiler::findPeaks(const std::vector<size_t>& data, size_t minDistance) const {
    std::vector<size_t> peaks;
    if (data.size() < 3) return peaks;
    
    for (size_t i = 1; i < data.size() - 1; ++i) {
        if (data[i] > data[i - 1] && data[i] > data[i + 1]) {
            if (peaks.empty() || (i - peaks.back()) >= minDistance) {
                peaks.push_back(i);
            }
        }
    }
    
    return peaks;
}

std::string Profiler::detectAnomalies(const std::vector<double>& data, double threshold) const {
    std::vector<size_t> anomalies;
    double mean = calculateMean(data);
    double stdDev = calculateStandardDeviation(data);
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (std::abs(data[i] - mean) > threshold * stdDev) {
            anomalies.push_back(i);
        }
    }
    
    if (anomalies.empty()) {
        return "No anomalies detected";
    }
    
    std::ostringstream oss;
    oss << "Anomalies detected at indices: ";
    for (size_t i = 0; i < anomalies.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << anomalies[i];
    }
    
    return oss.str();
}

std::vector<size_t> Profiler::analyzeSpikeTiming(const std::vector<size_t>& spikes) const {
    if (spikes.empty()) return {};
    
    std::vector<size_t> intervals;
    for (size_t i = 1; i < spikes.size(); ++i) {
        intervals.push_back(spikes[i] - spikes[i - 1]);
    }
    
    return intervals;
}

double Profiler::calculateFiringRate(const std::vector<size_t>& spikes, double duration) const {
    if (spikes.empty() || duration <= 0) return 0.0;
    return static_cast<double>(spikes.size()) / duration;
}

std::vector<size_t> Profiler::findBurstPatterns(const std::vector<size_t>& spikes) const {
    std::vector<size_t> bursts;
    if (spikes.size() < 3) return bursts;
    
    for (size_t i = 0; i < spikes.size() - 2; ++i) {
        if (spikes[i + 1] - spikes[i] <= 10 && spikes[i + 2] - spikes[i + 1] <= 10) {
            bursts.push_back(i);
        }
    }
    
    return bursts;
}

double Profiler::calculateCoefficientOfVariation(const std::vector<size_t>& spikes) const {
    if (spikes.size() < 2) return 0.0;
    
    double mean = calculateMean(spikes);
    double variance = calculateVariance(spikes);
    
    if (mean == 0.0) return 0.0;
    
    return std::sqrt(variance) / mean;
}

double Profiler::calculateLearningRate(const std::vector<double>& weightChanges) const {
    if (weightChanges.empty()) return 0.0;
    
    double sum = std::accumulate(weightChanges.begin(), weightChanges.end(), 0.0);
    return sum / weightChanges.size();
}

std::vector<double> Profiler::calculatePlasticityProfile(const std::vector<double>& weights) const {
    if (weights.empty()) return {};
    
    std::vector<double> profile;
    double maxWeight = *std::max_element(weights.begin(), weights.end());
    double minWeight = *std::min_element(weights.begin(), weights.end());
    double range = maxWeight - minWeight;
    
    for (double weight : weights) {
        profile.push_back((weight - minWeight) / range);
    }
    
    return profile;
}

double Profiler::calculateSynapticStability(const std::vector<double>& weights) const {
    if (weights.empty()) return 0.0;
    
    double mean = calculateMean(weights);
    double variance = calculateVariance(weights);
    
    if (mean == 0.0) return 0.0;
    
    return 1.0 / (1.0 + variance);
}

size_t Profiler::countLearningEvents(const std::vector<double>& weightChanges, double threshold) const {
    size_t count = 0;
    for (double change : weightChanges) {
        if (std::abs(change) > threshold) {
            count++;
        }
    }
    return count;
}

// Calculate real system CPU load (simplified)
double Profiler::calculateCPULoad() {
    static long long lastTotal = 0;
    static long long lastIdle = 0;
    
    long long total = 0;
    long long idle = 0;
    
    // Read /proc/stat (Linux specific)
    std::ifstream statFile("/proc/stat");
    if (statFile.is_open()) {
        std::string line;
        if (std::getline(statFile, line) && line.find("cpu  ") == 0) {
            std::istringstream iss(line);
            std::string cpuLabel;
            iss >> cpuLabel;
            for (int i = 0; i < 8; ++i) {
                long long value;
                iss >> value;
                if (i == 0) total += value;  // user
                else if (i == 1) total += value;  // nice
                else if (i == 2) total += value;  // system
                else if (i == 3) total += value;  // idle
                else if (i == 4) total += value;  // iowait
                else if (i == 5) total += value;  // irq
                else if (i == 6) total += value;  // softirq
                else if (i == 7) total += value;  // steal
            }
            
            // Calculate total and idle
            long long currentTotal = total;
            long long currentIdle = idle;
            
            if (lastTotal == 0) {
                lastTotal = currentTotal;
                lastIdle = currentIdle;
                return 0.0;
            }
            
            double cpuUsage = 1.0 - (static_cast<double>(currentIdle - lastIdle) / (currentTotal - lastTotal));
            lastTotal = currentTotal;
            lastIdle = currentIdle;
            
            return std::max(0.0, std::min(cpuUsage, 1.0));  // Clamp to [0, 1]
        }
    }
    
    return 0.0;
}

// Calculate real system memory usage
double Profiler::calculateMemoryUsage() {
    long long totalMem = 0;
    long long freeMem = 0;
    
    // Read /proc/meminfo (Linux specific)
    std::ifstream meminfoFile("/proc/meminfo");
    if (meminfoFile.is_open()) {
        std::string line;
        while (std::getline(meminfoFile, line)) {
            if (line.find("MemTotal:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                totalMem = value * 1024;  // Convert to bytes
            } else if (line.find("MemFree:") == 0) {
                std::istringstream iss(line);
                std::string key;
                long long value;
                iss >> key >> value;
                freeMem = value * 1024;  // Convert to bytes
            }
        }
    }
    
    if (totalMem == 0) return 0.0;
    
    double usedMem = static_cast<double>(totalMem - freeMem);
    return usedMem / (1024 * 1024);  // Return in MB
}

// Get elapsed time in seconds
double Profiler::getElapsedTime() const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->startTime);
    return static_cast<double>(elapsed.count());
}

// Metric series methods
std::map<std::string, std::vector<double>> Profiler::getMetricSeries() const {
    return pImpl->metricSeries;
}

const std::vector<double>& Profiler::getMetricSeries(const std::string& name) const {
    static const std::vector<double> empty;
    auto it = pImpl->metricSeries.find(name);
    return (it != pImpl->metricSeries.end()) ? it->second : empty;
}

// Analysis methods
double Profiler::calculateStability(const std::vector<double>& data) const {
    if (data.size() < 2) return 0.0;
    
    double mean = calculateMean(data);
    double variance = calculateVariance(data);
    
    if (mean == 0.0) return 0.0;
    
    return 1.0 / (1.0 + variance);
}

double Profiler::calculateEntropy(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    double mean = calculateMean(data);
    double entropy = 0.0;
    
    for (double value : data) {
        if (value > 0) {
            entropy -= value * std::log(value);
        }
    }
    
    return entropy;
}

double Profiler::calculateSum(const std::vector<double>& data) const {
    return std::accumulate(data.begin(), data.end(), 0.0);
}

double Profiler::calculateMax(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    return *std::max_element(data.begin(), data.end());
}

} // namespace nlm