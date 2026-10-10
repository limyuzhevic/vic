#include "NLMProfilingOutputFormatter.hpp"

namespace nlm {

// NLMProfilingOutputFormatter implementation
std::string NLMProfilingOutputFormatter::formatTextReport(const std::string& title, const std::string& data) {
    std::ostringstream oss;
    oss << "NLM Profiling Report: " << title << "\n";
    oss << "=================================" << std::string(title.length(), '=') << "\n\n";
    oss << data << "\n";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::formatHTMLDashboard(const std::string& title,
                                                            const std::string& data,
                                                            const std::vector<std::pair<std::string, double>>& metrics) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html>\n<head><title>" << title << "</title>";
    oss << "<style>body { font-family: Arial, sans-serif; margin: 20px; } ";
    oss << "h1 { color: #333; } h2 { color: #666; } .metric { margin: 10px 0; padding: 10px; ";
    oss << "background: #f5f5f5; border-radius: 5px; } .metric h3 { margin: 0 0 10px 0; }";
    oss << " .value { font-size: 1.2em; font-weight: bold; color: #2e6da4; }";
    oss << " table { border-collapse: collapse; width: 100%; } td, th { border: 1px solid #ddd; ";
    oss << "padding: 8px; text-align: left; } th { background-color: #f2f2f2; }";
    oss << " pre { background: #f8f8f8; padding: 10px; overflow-x: auto; }";
    oss << "</style></head>\n<body>\n";
    oss << "<h1>" << title << "</h1>\n";
    oss << "<h2>Metrics</h2>\n";
    oss << "<table>\n<tr><th>Name</th><th>Value</th><th>Unit</th></tr>\n";
    
    for (const auto& metric : metrics) {
        oss << "<tr><td>" << metric.first << "</td><td class=\"value\">";
        oss << metric.second << "</td><td></td></tr>\n";
    }
    
    oss << "</table>\n<h2>Profiling Data</h2>\n<pre>" << escapeHTML(data) << "</pre>\n";
    oss << "</body>\n</html>\n";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::formatCSVReport(const std::string& title,
                                                         const std::string& data,
                                                         const std::vector<std::pair<std::string, double>>& metrics) {
    std::ostringstream oss;
    oss << "NLM Profiling Report: " << title << "\n";
    oss << "Metrics:\n";
    oss << "Name,Value,Unit\n";
    
    for (const auto& metric : metrics) {
        oss << metric.first << "," << metric.second << ",\n";
    }
    
    oss << "\nProfiling Data:\n";
    oss << data << "\n";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::formatJSONReport(const std::string& title,
                                                          const std::string& data,
                                                          const std::vector<std::pair<std::string, double>>& metrics) {
    std::ostringstream oss;
    oss << "{\n  \"title\": \"" << escapeJSON(title) << "\",\n";
    oss << "  \"metrics\": [";
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        oss << "{\"name\": \"" << escapeJSON(metrics[i].first) << "\", \"value\": " << metrics[i].second;
        if (i < metrics.size() - 1) oss << "},";
        else oss << "}";
    }
    
    oss << "],\n  \"data\": \"" << escapeJSON(data) << "\"\n}\n";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::generateHTMLVisualization(const std::string& title,
                                                                   const std::vector<std::pair<std::string, double>>& metrics,
                                                                   const std::vector<std::string>& snapshots) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html>\n<head><title>" << title << " Visualization</title>";
    oss << "<style>body { font-family: Arial, sans-serif; margin: 20px; } ";
    oss << "h1 { color: #333; } .chart { margin: 20px 0; padding: 15px; ";
    oss << "border: 1px solid #ddd; border-radius: 5px; } .snapshot { margin: 10px 0; ";
    oss << "padding: 10px; background: #f9f9f9; } pre { background: #f5f5f5; ";
    oss << "padding: 10px; overflow-x: auto; white-space: pre-wrap; }";
    oss << "</style></head>\n<body>\n";
    oss << "<h1>" << title << " Visualization</h1>\n";
    
    for (const auto& metric : metrics) {
        oss << "<div class=\"chart\">\n<h2>" << metric.first << "</h2>\n";
        oss << "<div class=\"value\">" << metric.second << "</div>\n";
        oss << "<div>" << generateProgressBar(metric.second / 100.0) << "</div>\n";
        oss << "</div>\n";
    }
    
    oss << "<h2>Snapshots</h2>\n";
    for (const auto& snapshot : snapshots) {
        oss << "<div class=\"snapshot\">\n<pre>" << escapeHTML(snapshot) << "</pre>\n</div>\n";
    }
    
    oss << "</body>\n</html>\n";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::generateCSVChartData(const std::string& title,
                                                             const std::vector<std::pair<std::string, double>>& metrics) {
    std::ostringstream oss;
    oss << "Metric,Value\n";
    
    for (const auto& metric : metrics) {
        oss << metric.first << "," << metric.second << "\n";
    }
    
    return oss.str();
}

std::string NLMProfilingOutputFormatter::generateJSONProfile(const std::string& title,
                                                             const std::vector<std::pair<std::string, double>>& metrics,
                                                             const std::vector<std::string>& snapshots,
                                                             const std::string& metadata) {
    std::ostringstream oss;
    oss << "{\n  \"title\": \"" << escapeJSON(title) << "\",\n";
    oss << "  \"metadata\": \"" << escapeJSON(metadata) << "\",\n";
    oss << "  \"metrics\": [";
    
    for (size_t i = 0; i < metrics.size(); ++i) {
        oss << "{\"name\": \"" << escapeJSON(metrics[i].first) << "\", \"value\": " << metrics[i].second;
        if (i < metrics.size() - 1) oss << "},";
        else oss << "}";
    }
    
    oss << "],\n  \"snapshots\": [";
    
    for (size_t i = 0; i < snapshots.size(); ++i) {
        oss << "\"" << escapeJSON(snapshots[i]) << "\"";
        if (i < snapshots.size() - 1) oss << ", ";
    }
    
    oss << "]\n}\n";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::escapeJSON(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\b"; break;
            case '\\': oss << "\\\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string NLMProfilingOutputFormatter::escapeHTML(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '&': oss << "&amp;"; break;
            case '\"': oss << "&quot;"; break;
            case '\'': oss << "&#x27;"; break;
            case '/': oss << "&frasl;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string NLMProfilingOutputFormatter::generateProgressBar(double value, int width) {
    std::ostringstream oss;
    oss << "[";
    int filled = static_cast<int>(value * width);
    for (int i = 0; i < width; ++i) {
        if (i < filled) oss << "=";
        else if (i == filled) oss << ">";
        else oss << " ";
    }
    oss << "] " << std::fixed << std::setprecision(1) << (value * 100.0) << "%";
    return oss.str();
}

std::string NLMProfilingOutputFormatter::formatTime(double seconds) {
    if (seconds < 0.001) return std::to_string(seconds * 1000000.0) + " μs";
    if (seconds < 1.0) return std::to_string(seconds * 1000.0) + " ms";
    if (seconds < 60.0) return std::to_string(seconds) + " s";
    if (seconds < 3600.0) return std::to_string(seconds / 60.0) + " min";
    return std::to_string(seconds / 3600.0) + " h";
}

std::string NLMProfilingOutputFormatter::formatMemory(size_t bytes) {
    if (bytes < 1024) return std::to_string(bytes) + " B";
    if (bytes < 1024 * 1024) return std::to_string(bytes / 1024.0) + " KB";
    if (bytes < 1024 * 1024 * 1024) return std::to_string(bytes / (1024.0 * 1024.0)) + " MB";
    return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + " GB";
}

// NeuralSimulationProfiler implementation
NeuralSimulationProfiler::NeuralSimulationProfiler(const std::string& simulationName)
    : simulationName_(simulationName), isProfiling_(false),
      totalSimulationTime_(0.0), totalStepTime_(0.0), totalSteps_(0),
      totalNeuronUpdates_(0), totalSynapseUpdates_(0), peakMemoryUsage_(0),
      totalCPUUsage_(0.0), peakCPUUsage_(0.0) {
}

void NeuralSimulationProfiler::startProfiling() {
    isProfiling_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void NeuralSimulationProfiler::stopProfiling() {
    if (!isProfiling_) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    totalSimulationTime_ = std::chrono::duration<double>(now - startTime_).count();
    isProfiling_ = false;
}

void NeuralSimulationProfiler::recordStepTiming(double stepTime, size_t neuronCount, size_t synapseCount) {
    std::lock_guard<std::mutex> lock(mutex_);
    stepTimes_.push_back(stepTime);
    neuronCounts_.push_back(neuronCount);
    synapseCounts_.push_back(synapseCount);
    totalStepTime_ += stepTime;
    totalSteps_++;
}

void NeuralSimulationProfiler::recordNeuronProcessingTime(double time, size_t neuronId) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalNeuronUpdates_++;
    // Could store per-neuron timing if needed
}

void NeuralSimulationProfiler::recordSynapseProcessingTime(double time, size_t synapseId) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalSynapseUpdates_++;
    // Could store per-synapse timing if needed
}

void NeuralSimulationProfiler::recordMemoryUsage(size_t memoryUsage, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryUsages_.push_back(memoryUsage);
    if (memoryUsage > peakMemoryUsage_) peakMemoryUsage_ = memoryUsage;
}

void NeuralSimulationProfiler::recordCPUUsage(double cpuUsage) {
    std::lock_guard<std::mutex> lock(mutex_);
    cpuUsages_.push_back(cpuUsage);
    totalCPUUsage_ += cpuUsage;
    if (cpuUsage > peakCPUUsage_) peakCPUUsage_ = cpuUsage;
}

NeuralSimulationProfiler::SimulationStats NeuralSimulationProfiler::getSimulationStats() const {
    SimulationStats stats(simulationName_);
    
    if (totalSteps_ > 0) {
        stats.totalSimulationTime = totalSimulationTime_;
        stats.averageStepTime = totalStepTime_ / totalSteps_;
        if (!stepTimes_.empty()) {
            double sum = std::accumulate(stepTimes_.begin(), stepTimes_.end(), 0.0);
            stats.averageStepTime = sum / stepTimes_.size();
        }
    }
    
    stats.totalSteps = totalSteps_;
    stats.totalNeuronUpdates = totalNeuronUpdates_;
    stats.totalSynapseUpdates = totalSynapseUpdates_;
    stats.peakMemoryUsage = peakMemoryUsage_;
    
    if (!cpuUsages_.empty()) {
        double sum = std::accumulate(cpuUsages_.begin(), cpuUsages_.end(), 0.0);
        stats.averageCPUUsage = sum / cpuUsages_.size();
    }
    
    stats.peakCPUUsage = peakCPUUsage_;
    
    if (!neuronCounts_.empty()) {
        stats.currentNeuronCount = neuronCounts_.back();
    }
    
    if (!synapseCounts_.empty()) {
        stats.currentSynapseCount = synapseCounts_.back();
    }
    
    return stats;
}

std::string NeuralSimulationProfiler::exportProfilingData() const {
    std::ostringstream oss;
    oss << "Neural Simulation Profiler: " << simulationName_ << "\n";
    oss << "Total Simulation Time: " << totalSimulationTime_ << " seconds\n";
    oss << "Total Steps: " << totalSteps_ << "\n";
    oss << "Total Neuron Updates: " << totalNeuronUpdates_ << "\n";
    oss << "Total Synapse Updates: " << totalSynapseUpdates_ << "\n";
    oss << "Peak Memory Usage: " << peakMemoryUsage_ << " bytes\n";
    oss << "Peak CPU Usage: " << peakCPUUsage_ << "%\n";
    
    if (totalSteps_ > 0) {
        oss << "Average Step Time: " << (totalStepTime_ / totalSteps_) << " seconds\n";
    }
    
    if (!cpuUsages_.empty()) {
        double sum = std::accumulate(cpuUsages_.begin(), cpuUsages_.end(), 0.0);
        oss << "Average CPU Usage: " << (sum / cpuUsages_.size()) << "%\n";
    }
    
    oss << "\nStep-by-step data:\n";
    for (size_t i = 0; i < std::min(stepTimes_.size(), static_cast<size_t>(10)); ++i) {
        oss << "Step " << i << ": " << stepTimes_[i] << " s, "
            << "Neurons: " << (i < neuronCounts_.size() ? neuronCounts_[i] : 0) << ", "
            << "Synapses: " << (i < synapseCounts_.size() ? synapseCounts_[i] : 0) << "\n";
    }
    
    return oss.str();
}

std::string NeuralSimulationProfiler::generateProfilingReport() const {
    return exportProfilingData();
}

std::string NeuralSimulationProfiler::exportAsText(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatTextReport(title, exportProfilingData());
}

std::string NeuralSimulationProfiler::exportAsHTML(const std::string& title) const {
    SimulationStats stats = getSimulationStats();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total Time (s)", stats.totalSimulationTime},
        {"Average Step Time (s)", stats.averageStepTime},
        {"Total Steps", static_cast<double>(stats.totalSteps)},
        {"Neuron Updates", static_cast<double>(stats.totalNeuronUpdates)},
        {"Synapse Updates", static_cast<double>(stats.totalSynapseUpdates)},
        {"Peak Memory (B)", static_cast<double>(stats.peakMemoryUsage)},
        {"Average CPU Usage (%)", stats.averageCPUUsage},
        {"Peak CPU Usage (%)", stats.peakCPUUsage}
    };
    
    return NLMProfilingOutputFormatter::formatHTMLDashboard(title, exportProfilingData(), metrics);
}

std::string NeuralSimulationProfiler::exportAsCSV(const std::string& title) const {
    SimulationStats stats = getSimulationStats();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total Time (s)", stats.totalSimulationTime},
        {"Average Step Time (s)", stats.averageStepTime},
        {"Total Steps", static_cast<double>(stats.totalSteps)},
        {"Neuron Updates", static_cast<double>(stats.totalNeuronUpdates)},
        {"Synapse Updates", static_cast<double>(stats.totalSynapseUpdates)},
        {"Peak Memory (B)", static_cast<double>(stats.peakMemoryUsage)},
        {"Average CPU Usage (%)", stats.averageCPUUsage},
        {"Peak CPU Usage (%)", stats.peakCPUUsage}
    };
    
    return NLMProfilingOutputFormatter::formatCSVReport(title, exportProfilingData(), metrics);
}

std::string NeuralSimulationProfiler::exportAsJSON(const std::string& title) const {
    SimulationStats stats = getSimulationStats();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total Time (s)", stats.totalSimulationTime},
        {"Average Step Time (s)", stats.averageStepTime},
        {"Total Steps", static_cast<double>(stats.totalSteps)},
        {"Neuron Updates", static_cast<double>(stats.totalNeuronUpdates)},
        {"Synapse Updates", static_cast<double>(stats.totalSynapseUpdates)},
        {"Peak Memory (B)", static_cast<double>(stats.peakMemoryUsage)},
        {"Average CPU Usage (%)", stats.averageCPUUsage},
        {"Peak CPU Usage (%)", stats.peakCPUUsage}
    };
    
    return NLMProfilingOutputFormatter::formatJSONReport(title, exportProfilingData(), metrics);
}

// SystemResourceProfiler implementation
SystemResourceProfiler::SystemResourceProfiler() : isProfiling_(false),
    totalCPUUsage_(0.0), totalMemoryUsage_(0), totalNetworkBytes_(0),
    totalDiskBytes_(0), totalThreadCount_(0), peakMemoryUsage_(0) {
}

void SystemResourceProfiler::startProfiling(const std::string& context) {
    context_ = context;
    isProfiling_ = true;
    startTime_ = std::chrono::system_clock::now();
}

void SystemResourceProfiler::stopProfiling() {
    isProfiling_ = false;
}

void SystemResourceProfiler::recordResourceUsage(double cpuUsage, size_t memoryUsage,
                                                uint64_t networkBytes, uint64_t diskBytes,
                                                const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    cpuUsages_.push_back(cpuUsage);
    memoryUsages_.push_back(memoryUsage);
    networkBytesList_.push_back(networkBytes);
    diskBytesList_.push_back(diskBytes);
    
    totalCPUUsage_ += cpuUsage;
    totalMemoryUsage_ += memoryUsage;
    totalNetworkBytes_ += networkBytes;
    totalDiskBytes_ += diskBytes;
    
    if (memoryUsage > peakMemoryUsage_) peakMemoryUsage_ = memoryUsage;
}

void SystemResourceProfiler::recordThreadInfo(const std::string& threadName, size_t threadId,
                                             double cpuTime, size_t memoryUsage) {
    std::lock_guard<std::mutex> lock(mutex_);
    threadStats_.emplace_back(threadName, threadId, cpuTime, memoryUsage);
    totalThreadCount_++;
}

SystemResourceProfiler::SystemStats SystemResourceProfiler::getSystemStats() const {
    SystemStats stats(context_);
    
    size_t sampleCount = cpuUsages_.size();
    if (sampleCount > 0) {
        double cpuSum = std::accumulate(cpuUsages_.begin(), cpuUsages_.end(), 0.0);
        stats.averageCPUUsage = cpuSum / sampleCount;
        
        size_t memSum = std::accumulate(memoryUsages_.begin(), memoryUsages_.end(), 0);
        stats.averageMemoryUsage = memSum / sampleCount;
        
        uint64_t netSum = std::accumulate(networkBytesList_.begin(), networkBytesList_.end(), 0ULL);
        stats.averageNetworkBytes = netSum / sampleCount;
        
        uint64_t diskSum = std::accumulate(diskBytesList_.begin(), diskBytesList_.end(), 0ULL);
        stats.averageDiskBytes = diskSum / sampleCount;
        
        stats.totalCPUUsage = totalCPUUsage_;
        stats.peakMemoryUsage = peakMemoryUsage_;
        stats.totalNetworkBytes = totalNetworkBytes_;
        stats.totalDiskBytes = totalDiskBytes_;
    }
    
    return stats;
}

std::string SystemResourceProfiler::exportSystemProfilingData() const {
    std::ostringstream oss;
    oss << "System Resource Profiler: " << context_ << "\n";
    oss << "Total Samples: " << cpuUsages_.size() << "\n";
    oss << "Peak Memory Usage: " << peakMemoryUsage_ << " bytes\n";
    oss << "Total Thread Count: " << totalThreadCount_ << "\n";
    
    SystemStats stats = getSystemStats();
    if (cpuUsages_.size() > 0) {
        oss << "Average CPU Usage: " << stats.averageCPUUsage << "%\n";
        oss << "Average Memory Usage: " << stats.averageMemoryUsage << " bytes\n";
        oss << "Average Network Bytes: " << stats.averageNetworkBytes << "\n";
        oss << "Average Disk Bytes: " << stats.averageDiskBytes << "\n";
    }
    
    oss << "Total CPU Usage: " << stats.totalCPUUsage << "%\n";
    oss << "Total Memory Usage: " << stats.averageMemoryUsage * cpuUsages_.size() << " bytes\n";
    oss << "Total Network Bytes: " << stats.totalNetworkBytes << "\n";
    oss << "Total Disk Bytes: " << stats.totalDiskBytes << "\n";
    
    return oss.str();
}

std::string SystemResourceProfiler::generateSystemProfilingReport() const {
    return exportSystemProfilingData();
}

std::string SystemResourceProfiler::exportAsText(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatTextReport(title, exportSystemProfilingData());
}

std::string SystemResourceProfiler::exportAsHTML(const std::string& title) const {
    SystemStats stats = getSystemStats();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Average CPU Usage (%)", stats.averageCPUUsage},
        {"Average Memory Usage (B)", static_cast<double>(stats.averageMemoryUsage)},
        {"Average Network Bytes", static_cast<double>(stats.averageNetworkBytes)},
        {"Average Disk Bytes", static_cast<double>(stats.averageDiskBytes)},
        {"Total CPU Usage (%)", stats.totalCPUUsage},
        {"Peak Memory Usage (B)", static_cast<double>(stats.peakMemoryUsage)},
        {"Thread Count", static_cast<double>(stats.totalThreadCount)},
        {"Total Network Bytes", static_cast<double>(stats.totalNetworkBytes)},
        {"Total Disk Bytes", static_cast<double>(stats.totalDiskBytes)}
    };
    
    return NLMProfilingOutputFormatter::formatHTMLDashboard(title, exportSystemProfilingData(), metrics);
}

std::string SystemResourceProfiler::exportAsCSV(const std::string& title) const {
    SystemStats stats = getSystemStats();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Average CPU Usage (%)", stats.averageCPUUsage},
        {"Average Memory Usage (B)", static_cast<double>(stats.averageMemoryUsage)},
        {"Average Network Bytes", static_cast<double>(stats.averageNetworkBytes)},
        {"Average Disk Bytes", static_cast<double>(stats.averageDiskBytes)},
        {"Total CPU Usage (%)", stats.totalCPUUsage},
        {"Peak Memory Usage (B)", static_cast<double>(stats.peakMemoryUsage)},
        {"Thread Count", static_cast<double>(stats.totalThreadCount)},
        {"Total Network Bytes", static_cast<double>(stats.totalNetworkBytes)},
        {"Total Disk Bytes", static_cast<double>(stats.totalDiskBytes)}
    };
    
    return NLMProfilingOutputFormatter::formatCSVReport(title, exportSystemProfilingData(), metrics);
}

std::string SystemResourceProfiler::exportAsJSON(const std::string& title) const {
    SystemStats stats = getSystemStats();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Average CPU Usage (%)", stats.averageCPUUsage},
        {"Average Memory Usage (B)", static_cast<double>(stats.averageMemoryUsage)},
        {"Average Network Bytes", static_cast<double>(stats.averageNetworkBytes)},
        {"Average Disk Bytes", static_cast<double>(stats.averageDiskBytes)},
        {"Total CPU Usage (%)", stats.totalCPUUsage},
        {"Peak Memory Usage (B)", static_cast<double>(stats.peakMemoryUsage)},
        {"Thread Count", static_cast<double>(stats.totalThreadCount)},
        {"Total Network Bytes", static_cast<double>(stats.totalNetworkBytes)},
        {"Total Disk Bytes", static_cast<double>(stats.totalDiskBytes)}
    };
    
    return NLMProfilingOutputFormatter::formatJSONReport(title, exportSystemProfilingData(), metrics);
}

// AdvancedMemoryLeakDetector implementation
AdvancedMemoryLeakDetector::AdvancedMemoryLeakDetector() {
}

void AdvancedMemoryLeakDetector::trackAllocation(void* pointer, size_t size, const std::string& context,
                                                const std::string& file, int line,
                                                const std::string& function) {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.emplace_back(pointer, size, context, file, line, function);
}

void AdvancedMemoryLeakDetector::trackDeallocation(void* pointer, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& alloc : allocations_) {
        if (alloc.pointer == pointer) {
            alloc.isFreed = true;
            return;
        }
    }
}

AdvancedMemoryLeakDetector::MemoryLeakAnalysis AdvancedMemoryLeakDetector::analyzeLeaks() const {
    std::lock_guard<std::mutex> lock(mutex_);
    MemoryLeakAnalysis analysis;
    
    for (const auto& alloc : allocations_) {
        if (!alloc.isFreed) {
            analysis.totalLeaks++;
            analysis.totalLeakedMemory += alloc.size;
            analysis.contextCounts[alloc.context]++;
            analysis.fileCounts[alloc.file]++;
            
            if (alloc.line > 0) {
                analysis.mostCommonLine = alloc.line; // Simplified
            }
            
            analysis.leaks.emplace_back(alloc.pointer, alloc.size);
        }
    }
    
    // Find most common context
    if (!analysis.contextCounts.empty()) {
        auto maxIt = std::max_element(analysis.contextCounts.begin(), analysis.contextCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        analysis.mostCommonContext = maxIt->first;
    }
    
    // Find most common file
    if (!analysis.fileCounts.empty()) {
        auto maxIt = std::max_element(analysis.fileCounts.begin(), analysis.fileCounts.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        analysis.mostCommonFile = maxIt->first;
    }
    
    return analysis;
}

bool AdvancedMemoryLeakDetector::checkForBufferOverflows() const {
    // Simplified buffer overflow detection
    return false;
}

bool AdvancedMemoryLeakDetector::checkForUseAfterFree() const {
    // Simplified use-after-free detection
    return false;
}

bool AdvancedMemoryLeakDetector::checkForDoubleFree() const {
    // Simplified double-free detection
    return false;
}

std::string AdvancedMemoryLeakDetector::exportLeakAnalysis() const {
    MemoryLeakAnalysis analysis = analyzeLeaks();
    std::ostringstream oss;
    oss << "Memory Leak Analysis:\n";
    oss << "Total Leaks: " << analysis.totalLeaks << "\n";
    oss << "Total Leaked Memory: " << analysis.totalLeakedMemory << " bytes\n";
    oss << "Most Common Context: " << analysis.mostCommonContext << "\n";
    oss << "Most Common File: " << analysis.mostCommonFile << "\n";
    
    if (!analysis.contextCounts.empty()) {
        oss << "Context Counts:\n";
        for (const auto& pair : analysis.contextCounts) {
            oss << "  " << pair.first << ": " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

std::string AdvancedMemoryLeakDetector::generateLeakReport() const {
    return exportLeakAnalysis();
}

std::string AdvancedMemoryLeakDetector::exportAsText(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatTextReport(title, exportLeakAnalysis());
}

std::string AdvancedMemoryLeakDetector::exportAsHTML(const std::string& title) const {
    MemoryLeakAnalysis analysis = analyzeLeaks();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total Leaks", static_cast<double>(analysis.totalLeaks)},
        {"Total Leaked Memory (B)", static_cast<double>(analysis.totalLeakedMemory)},
    };
    
    std::ostringstream leakData;
    leakData << "Leaks found: " << analysis.totalLeaks << "\n";
    
    return NLMProfilingOutputFormatter::formatHTMLDashboard(title, leakData.str(), metrics);
}

std::string AdvancedMemoryLeakDetector::exportAsCSV(const std::string& title) const {
    MemoryLeakAnalysis analysis = analyzeLeaks();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total Leaks", static_cast<double>(analysis.totalLeaks)},
        {"Total Leaked Memory (B)", static_cast<double>(analysis.totalLeakedMemory)},
    };
    
    return NLMProfilingOutputFormatter::formatCSVReport(title, exportLeakAnalysis(), metrics);
}

std::string AdvancedMemoryLeakDetector::exportAsJSON(const std::string& title) const {
    MemoryLeakAnalysis analysis = analyzeLeaks();
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total Leaks", static_cast<double>(analysis.totalLeaks)},
        {"Total Leaked Memory (B)", static_cast<double>(analysis.totalLeakedMemory)},
    };
    
    return NLMProfilingOutputFormatter::formatJSONReport(title, exportLeakAnalysis(), metrics);
}

// EnhancedLoggingManager implementation
EnhancedLoggingManager::EnhancedLoggingManager() {
}

void EnhancedLoggingManager::logSystemEvent(const std::string& event, const std::string& severity,
                                          const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalSystemEvents++;
    logEntries_.emplace_back("System", event, severity, context, "");
}

void EnhancedLoggingManager::logNeuralSimulationEvent(const std::string& event,
                                                     const std::string& details,
                                                     const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalNeuralEvents++;
    logEntries_.emplace_back("Neural", event, "INFO", context, details);
}

void EnhancedLoggingManager::logMemoryEvent(const std::string& event, size_t size,
                                          const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalMemoryEvents++;
    std::ostringstream oss;
    oss << "Size: " << size << " bytes";
    logEntries_.emplace_back("Memory", event, "INFO", context, oss.str());
}

void EnhancedLoggingManager::logPerformanceEvent(const std::string& event, double value,
                                                const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalPerformanceEvents++;
    std::ostringstream oss;
    oss << "Value: " << value;
    logEntries_.emplace_back("Performance", event, "INFO", context, oss.str());
}

void EnhancedLoggingManager::logThreadEvent(const std::string& threadId, const std::string& event,
                                           const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    stats_.totalThreadEvents++;
    logEntries_.emplace_back("Thread", event, "INFO", context + " (Thread: " + threadId + ")", "");
}

EnhancedLoggingManager::LoggingStats EnhancedLoggingManager::getLoggingStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    LoggingStats stats = stats_;
    stats.totalSystemEvents = logEntries_.size(); // Simplified
    return stats;
}

std::string EnhancedLoggingManager::exportLoggingData() const {
    std::ostringstream oss;
    oss << "Enhanced Logging Data:\n";
    oss << "Total Log Entries: " << logEntries_.size() << "\n";
    
    for (const auto& entry : logEntries_) {
        oss << "[" << entry.timestamp.time_since_epoch().count() << "] "
            << entry.category << "." << entry.event
            << " [" << entry.severity << "]";
        if (!entry.context.empty()) oss << " Context: " << entry.context;
        if (!entry.details.empty()) oss << " Details: " << entry.details;
        oss << "\n";
    }
    
    return oss.str();
}

std::string EnhancedLoggingManager::generateLoggingReport() const {
    return exportLoggingData();
}

std::string EnhancedLoggingManager::exportAsText(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatTextReport(title, exportLoggingData());
}

std::string EnhancedLoggingManager::exportAsHTML(const std::string& title) const {
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total System Events", static_cast<double>(stats_.totalSystemEvents)},
        {"Total Neural Events", static_cast<double>(stats_.totalNeuralEvents)},
        {"Total Memory Events", static_cast<double>(stats_.totalMemoryEvents)},
        {"Total Performance Events", static_cast<double>(stats_.totalPerformanceEvents)},
        {"Total Thread Events", static_cast<double>(stats_.totalThreadEvents)}
    };
    
    return NLMProfilingOutputFormatter::formatHTMLDashboard(title, exportLoggingData(), metrics);
}

std::string EnhancedLoggingManager::exportAsCSV(const std::string& title) const {
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total System Events", static_cast<double>(stats_.totalSystemEvents)},
        {"Total Neural Events", static_cast<double>(stats_.totalNeuralEvents)},
        {"Total Memory Events", static_cast<double>(stats_.totalMemoryEvents)},
        {"Total Performance Events", static_cast<double>(stats_.totalPerformanceEvents)},
        {"Total Thread Events", static_cast<double>(stats_.totalThreadEvents)}
    };
    
    return NLMProfilingOutputFormatter::formatCSVReport(title, exportLoggingData(), metrics);
}

std::string EnhancedLoggingManager::exportAsJSON(const std::string& title) const {
    std::vector<std::pair<std::string, double>> metrics = {
        {"Total System Events", static_cast<double>(stats_.totalSystemEvents)},
        {"Total Neural Events", static_cast<double>(stats_.totalNeuralEvents)},
        {"Total Memory Events", static_cast<double>(stats_.totalMemoryEvents)},
        {"Total Performance Events", static_cast<double>(stats_.totalPerformanceEvents)},
        {"Total Thread Events", static_cast<double>(stats_.totalThreadEvents)}
    };
    
    return NLMProfilingOutputFormatter::formatJSONReport(title, exportLoggingData(), metrics);
}

// NLMProfiling implementation
NLMProfiling& NLMProfiling::getInstance() {
    static NLMProfiling instance;
    return instance;
}

void NLMProfiling::setupProfiling(const std::string& outputDirectory) {
    outputDirectory_ = outputDirectory;
}

void NLMProfiling::startNeuralSimulation(const std::string& simulationName) {
    std::lock_guard<std::mutex> lock(mutex_);
    neuralProfiler_ = std::make_unique<NeuralSimulationProfiler>(simulationName);
    neuralProfiler_->startProfiling();
}

void NLMProfiling::stopNeuralSimulation() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (neuralProfiler_) {
        neuralProfiler_->stopProfiling();
    }
}

void NLMProfiling::startSystemResourceProfiling(const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    systemProfiler_ = std::make_unique<SystemResourceProfiler>();
    systemProfiler_->startProfiling(context);
}

void NLMProfiling::stopSystemResourceProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (systemProfiler_) {
        systemProfiler_->stopProfiling();
    }
}

void NLMProfiling::recordMemoryAllocation(void* pointer, size_t size, const std::string& context,
                                         const std::string& file, int line,
                                         const std::string& function) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (memoryLeakDetector_) {
        memoryLeakDetector_->trackAllocation(pointer, size, context, file, line, function);
    }
    if (loggingManager_) {
        loggingManager_->logMemoryEvent("Allocation", size, context);
    }
}

void NLMProfiling::recordMemoryDeallocation(void* pointer, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (memoryLeakDetector_) {
        memoryLeakDetector_->trackDeallocation(pointer, context);
    }
    if (loggingManager_) {
        loggingManager_->logMemoryEvent("Deallocation", 0, context);
    }
}

std::string NLMProfiling::exportAllProfilingData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    
    if (neuralProfiler_) {
        oss << "=== NEURAL SIMULATION PROFILING ===\n";
        oss << neuralProfiler_->exportProfilingData() << "\n\n";
    }
    
    if (systemProfiler_) {
        oss << "=== SYSTEM RESOURCE PROFILING ===\n";
        oss << systemProfiler_->exportSystemProfilingData() << "\n\n";
    }
    
    if (memoryLeakDetector_) {
        oss << "=== MEMORY LEAK DETECTION ===\n";
        oss << memoryLeakDetector_->exportLeakAnalysis() << "\n\n";
    }
    
    if (loggingManager_) {
        oss << "=== ENHANCED LOGGING ===\n";
        oss << loggingManager_->exportLoggingData() << "\n\n";
    }
    
    return oss.str();
}

std::string NLMProfiling::generateComprehensiveReport() const {
    return exportAllProfilingData();
}

std::string NLMProfiling::exportAsText(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatTextReport(title, exportAllProfilingData());
}

std::string NLMProfiling::exportAsHTML(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatHTMLDashboard(title, exportAllProfilingData());
}

std::string NLMProfiling::exportAsCSV(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatCSVReport(title, exportAllProfilingData());
}

std::string NLMProfiling::exportAsJSON(const std::string& title) const {
    return NLMProfilingOutputFormatter::formatJSONReport(title, exportAllProfilingData());
}

} // namespace nlm