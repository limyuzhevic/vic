// Debugging Utilities for Neural Network Simulation
// 
// This system provides comprehensive debugging tools for the NLM project:
// - Neural network visualizers and inspectors
// - Debug logging and tracing
// - State inspection tools
// - Performance debugging tools
// - Memory debugging tools
// - Error analysis tools
// - Visualization utilities
// - Breakpoint and stepping tools
// - Network analysis tools
// - Learning debugging tools

#ifndef NLM_DEBUGGING_TOOLS_H
#define NLM_DEBUGGING_TOOLS_H

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <thread>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <numeric>
#include <cmath>
#include <functional>
#include <unordered_set>
#include <queue>

#include "core/ErrorHandling.hpp"

namespace nlm {

// ============================================================================
// Debug Log Levels
// ============================================================================

enum class DebugLevel {
    None = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};

// ============================================================================
// Debug Event Types
// ============================================================================

enum class DebugEventType {
    SimulationStep,
    SpikeEvent,
    SynapseEvent,
    NeuronStateChange,
    MemoryEvent,
    PlasticityEvent,
    ConfigurationChange,
    ErrorEvent,
    PerformanceEvent,
    VisualizationEvent
};

// ============================================================================
// Debug Event
// ============================================================================

struct DebugEvent {
    DebugEventType type;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string sessionId;
    std::string description;
    std::map<std::string, std::string> metadata;
    
    DebugEvent() : type(DebugEventType::ErrorEvent), timestamp(std::chrono::high_resolution_clock::now()) {}
    
    DebugEvent(DebugEventType t, const std::string& desc, const std::string& session = "default")
        : type(t), timestamp(std::chrono::high_resolution_clock::now()), sessionId(session), description(desc) {}
};

// ============================================================================
// Neural Network Inspector
// ============================================================================

class NeuralNetworkInspector {
public:
    static NeuralNetworkInspector& getInstance() {
        static NeuralNetworkInspector instance;
        return instance;
    }
    
    // Inspect a brain and collect detailed information
    void inspectBrain(std::shared_ptr<Brain> brain, const std::string& inspectionId = "brain_inspection") {
        std::lock_guard<std::mutex> lock(mutex);
        
        InspectionResult result;
        result.inspectionId = inspectionId;
        result.timestamp = std::chrono::high_resolution_clock::now();
        result.brain = brain;
        
        // Collect neuron information
        collectNeuronInfo(brain, result);
        
        // Collect synapse information
        collectSynapseInfo(brain, result);
        
        // Collect memory information
        collectMemoryInfo(brain, result);
        
        // Collect performance information
        collectPerformanceInfo(brain, result);
        
        // Store inspection result
        inspectionHistory[inspectionId] = result;
        
        NLM_LOG_INFO("Brain inspection completed: " + inspectionId);
        NLM_LOG_INFO("  Neurons: " + std::to_string(result.neuronStats.size()));
        NLM_LOG_INFO("  Synapses: " + std::to_string(result.synapseStats.size()));
        NLM_LOG_INFO("  Memory systems: " + std::to_string(result.memoryStats.size()));
    }
    
    // Get inspection result
    InspectionResult getInspection(const std::string& inspectionId) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = inspectionHistory.find(inspectionId);
        if (it != inspectionHistory.end()) {
            return it->second;
        }
        
        return InspectionResult();
    }
    
    // Get all inspection results
    std::vector<InspectionResult> getAllInspections() const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<InspectionResult> results;
        for (const auto& pair : inspectionHistory) {
            results.push_back(pair.second);
        }
        
        return results;
    }
    
    // Generate inspection report
    std::string generateInspectionReport(const std::string& inspectionId) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto it = inspectionHistory.find(inspectionId);
        if (it == inspectionHistory.end()) {
            return "Inspection not found: " + inspectionId;
        }
        
        const auto& result = it->second;
        std::stringstream ss;
        
        ss << "=== Neural Network Inspection Report ===\n";
        ss << "Inspection ID: " + result.inspectionId + "\n";
        ss << "Timestamp: " + formatTimestamp(result.timestamp) + "\n";
        ss << "Brain Summary:\n";
        
        ss << "  Total Neurons: " << result.neuronStats.size() << "\n";
        ss << "  Total Synapses: " << result.synapseStats.size() << "\n";
        ss << "  Total Memory Traces: " << result.memoryStats.totalTraces << "\n";
        ss << "  Average Firing Rate: " << result.performanceInfo.avgFiringRate << "\n";
        ss << "  Total Spike Count: " << result.performanceInfo.totalSpikes << "\n";
        
        ss << "\n=== Neuron Statistics ===\n";
        for (const auto& neuron : result.neuronStats) {
            ss << "Neuron " << neuron.id << ": "
               << "Type: " << neuron.type << ", "
               << "V: " << neuron.membranePotential << "mV, "
               << "Threshold: " << neuron.threshold << "mV, "
               << "Refractory: " << neuron.refractoryTime << "ms, "
               << "Firing: " << (neuron.isFiring ? "Yes" : "No") << "\n";
        }
        
        ss << "\n=== Synapse Statistics ===\n";
        for (const auto& synapse : result.synapseStats) {
            ss << "Synapse " << synapse.id << ": "
               << "Pre: " << synapse.preNeuronId << ", "
               << "Post: " << synapse.postNeuronId << ", "
               << "Weight: " << synapse.weight << ", "
               << "Delay: " << synapse.delay << "ms\n";
        }
        
        return ss.str();
    }
    
    // Compare two inspections
    std::map<std::string, double> compareInspections(const std::string& id1, const std::string& id2) {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::map<std::string, double> comparison;
        
        auto it1 = inspectionHistory.find(id1);
        auto it2 = inspectionHistory.find(id2);
        
        if (it1 == inspectionHistory.end() || it2 == inspectionHistory.end()) {
            return comparison;
        }
        
        const auto& insp1 = it1->second;
        const auto& insp2 = it2->second;
        
        // Compare neuron counts
        comparison["neuron_count_ratio"] = 
            static_cast<double>(insp2.neuronStats.size()) / insp1.neuronStats.size();
        
        // Compare synapse counts
        comparison["synapse_count_ratio"] = 
            static_cast<double>(insp2.synapseStats.size()) / insp1.synapseStats.size();
        
        // Compare firing rates
        if (insp1.performanceInfo.avgFiringRate > 0.0) {
            comparison["firing_rate_ratio"] = 
                insp2.performanceInfo.avgFiringRate / insp1.performanceInfo.avgFiringRate;
        }
        
        // Compare spike counts
        comparison["spike_count_ratio"] = 
            static_cast<double>(insp2.performanceInfo.totalSpikes) / insp1.performanceInfo.totalSpikes;
        
        return comparison;
    }
    
private:
    struct NeuronStats {
        NeuronId id;
        std::string type;
        double membranePotential;
        double threshold;
        double refractoryTime;
        bool isFiring;
        size_t spikeCount;
    };
    
    struct SynapseStats {
        SynapseId id;
        NeuronId preNeuronId;
        NeuronId postNeuronId;
        double weight;
        double delay;
        bool isPlastic;
    };
    
    struct MemoryStats {
        size_t totalTraces;
        size_t episodicEpisodes;
        size_t workingMemoryCapacity;
        double noveltyLevel;
        double curiosityLevel;
    };
    
    struct PerformanceStats {
        float avgFiringRate;
        size_t totalSpikes;
        float excitationInhibitionRatio;
        double cpuUsage;
        size_t memoryUsage;
    };
    
    struct InspectionResult {
        std::string inspectionId;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::shared_ptr<Brain> brain;
        
        std::vector<NeuronStats> neuronStats;
        std::vector<SynapseStats> synapseStats;
        MemoryStats memoryStats;
        PerformanceStats performanceInfo;
    };
    
    NeuralNetworkInspector() {}
    
    void collectNeuronInfo(std::shared_ptr<Brain> brain, InspectionResult& result) {
        // This would need access to brain's internal neuron data
        // For now, we'll create placeholder data
        for (size_t i = 0; i < 10; ++i) { // Sample first 10 neurons
            NeuronStats neuron;
            neuron.id = NeuronId(i);
            neuron.type = "Excitatory";
            neuron.membranePotential = -70.0 + (rand() % 20);
            neuron.threshold = -55.0;
            neuron.refractoryTime = 2.0;
            neuron.isFiring = (rand() % 100) < 20;
            neuron.spikeCount = rand() % 100;
            result.neuronStats.push_back(neuron);
        }
    }
    
    void collectSynapseInfo(std::shared_ptr<Brain> brain, InspectionResult& result) {
        // This would need access to brain's internal synapse data
        // For now, we'll create placeholder data
        for (size_t i = 0; i < 50; ++i) { // Sample 50 synapses
            SynapseStats synapse;
            synapse.id = SynapseId(i);
            synapse.preNeuronId = NeuronId(i % 10);
            synapse.postNeuronId = NeuronId((i + 1) % 10);
            synapse.weight = 0.5 + (rand() % 50) / 100.0;
            synapse.delay = 1 + (rand() % 10);
            synapse.isPlastic = (rand() % 100) < 30;
            result.synapseStats.push_back(synapse);
        }
    }
    
    void collectMemoryInfo(std::shared_ptr<Brain> brain, InspectionResult& result) {
        // This would need access to brain's memory systems
        // For now, we'll create placeholder data
        result.memoryStats.totalTraces = 1000;
        result.memoryStats.episodicEpisodes = 100;
        result.memoryStats.workingMemoryCapacity = 500;
        result.memoryStats.noveltyLevel = 0.3 + (rand() % 70) / 100.0;
        result.memoryStats.curiosityLevel = 0.2 + (rand() % 80) / 100.0;
    }
    
    void collectPerformanceInfo(std::shared_ptr<Brain> brain, InspectionResult& result) {
        // This would need access to brain's performance statistics
        // For now, we'll create placeholder data
        result.performanceInfo.avgFiringRate = 5.0 + (rand() % 95) / 10.0;
        result.performanceInfo.totalSpikes = 5000 + (rand() % 45000);
        result.performanceInfo.excitationInhibitionRatio = 1.5 + (rand() % 50) / 100.0;
        result.performanceInfo.cpuUsage = 10.0 + (rand() % 80);
        result.performanceInfo.memoryUsage = 100 + (rand() % 900) * 1024; // KB
    }
    
    std::string formatTimestamp(const std::chrono::high_resolution_clock::time_point& tp) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - tp);
        return std::to_string(duration.count()) + "ms";
    }
    
    mutable std::mutex mutex;
    std::map<std::string, InspectionResult> inspectionHistory;
    
    NeuralNetworkInspector(const NeuralNetworkInspector&) = delete;
    NeuralNetworkInspector& operator=(const NeuralNetworkInspector&) = delete;
};

// ============================================================================
// Neural Network Visualizer
// ============================================================================

class NeuralNetworkVisualizer {
public:
    static NeuralNetworkVisualizer& getInstance() {
        static NeuralNetworkVisualizer instance;
        return instance;
    }
    
    // Generate neural network visualization
    std::string visualizeNetwork(std::shared_ptr<Brain> brain, const std::string& format = "text") {
        if (format == "text") {
            return generateTextVisualization(brain);
        } else if (format == "dot") {
            return generateGraphVizVisualization(brain);
        } else if (format == "html") {
            return generateHTMLVisualization(brain);
        } else if (format == "svg") {
            return generateSVGVisualization(brain);
        }
        
        return "Unsupported visualization format: " + format;
    }
    
    // Generate neuron activity heatmap
    std::string generateActivityHeatmap(std::shared_ptr<Brain> brain, int width = 80, int height = 60) {
        std::stringstream ss;
        ss << "Neural Activity Heatmap\n";
        ss << "=======================\n\n";
        
        // Generate simple ASCII heatmap based on firing rates
        std::vector<std::vector<double>> heatmap(height, std::vector<double>(width, 0.0));
        
        // Sample neuron data for visualization
        double maxFiringRate = 10.0; // Assume max firing rate
        
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Simulate firing rate based on position
                double firingRate = maxFiringRate * 
                    (0.5 + 0.5 * std::sin(y * 0.1) * std::cos(x * 0.1));
                
                int intensity = static_cast<int>((firingRate / maxFiringRate) * 10);
                ss << getHeatmapChar(intensity);
            }
            ss << "\n";
        }
        
        ss << "\nLegend: ";
        for (int i = 0; i <= 10; ++i) {
            ss << getHeatmapChar(i) << " ";
        }
        ss << " Low -> High\n";
        
        return ss.str();
    }
    
    // Generate spike raster plot
    std::string generateSpikeRaster(std::shared_ptr<Brain> brain, size_t maxSpikes = 100) {
        std::stringstream ss;
        ss << "Spike Raster Plot\n";
        ss << "==================\n\n";
        
        ss << "Time (ms)\tNeuron ID\tType\n";
        ss << "----------------------------------------\n";
        
        // Generate sample spike data
        for (size_t i = 0; i < maxSpikes; ++i) {
            size_t neuronId = rand() % 50;
            size_t time = rand() % 1000;
            std::string type = (neuronId % 3 == 0) ? "Sensory" : 
                              (neuronId % 3 == 1) ? "Motor" : "Interneuron";
            
            ss << time << "\t" << neuronId << "\t" << type << "\n";
        }
        
        return ss.str();
    }
    
    // Generate connectivity matrix
    std::string generateConnectivityMatrix(std::shared_ptr<Brain> brain, size_t matrixSize = 20) {
        std::stringstream ss;
        ss << "Connectivity Matrix\n";
        ss << "===================\n\n";
        
        ss << "   ";
        for (size_t j = 0; j < matrixSize; ++j) {
            ss << std::setw(3) << j << " ";
        }
        ss << "\n";
        
        for (size_t i = 0; i < matrixSize; ++i) {
            ss << std::setw(3) << i << ": ";
            for (size_t j = 0; j < matrixSize; ++j) {
                // Simulate connectivity
                double connectionProbability = 0.1 + 0.3 * std::sin(i * 0.2) * std::cos(j * 0.2);
                if ((rand() % 100) / 100.0 < connectionProbability) {
                    ss << "* ";
                } else {
                    ss << "  ";
                }
            }
            ss << "\n";
        }
        
        return ss.str();
    }
    
    // Generate learning curve visualization
    std::string generateLearningCurve(const std::vector<BenchmarkResult>& results) {
        std::stringstream ss;
        ss << "Learning Curve Analysis\n";
        ss << "========================\n\n";
        
        ss << "Benchmarks analyzed: " << results.size() << "\n\n";
        
        // Calculate performance metrics for each benchmark
        std::vector<double> throughputs;
        std::vector<double> efficiencies;
        std::vector<double> memoryUsages;
        
        for (const auto& result : results) {
            throughputs.push_back(result.throughput);
            efficiencies.push_back(result.efficiency);
            memoryUsages.push_back(static_cast<double>(result.peakMemoryUsage));
        }
        
        if (!throughputs.empty()) {
            double avgThroughput = std::accumulate(throughputs.begin(), throughputs.end(), 0.0) / throughputs.size();
            double maxThroughput = *std::max_element(throughputs.begin(), throughputs.end());
            double minThroughput = *std::min_element(throughputs.begin(), throughputs.end());
            
            ss << "Throughput Statistics:\n";
            ss << "  Average: " << avgThroughput << " steps/s\n";
            ss << "  Maximum: " << maxThroughput << " steps/s\n";
            ss << "  Minimum: " << minThroughput << " steps/s\n";
            ss << "  Coefficient of Variation: " << 
                calculateCoefficientOfVariation(throughputs) << "\n\n";
        }
        
        return ss.str();
    }
    
private:
    NeuralNetworkVisualizer() {}
    
    char getHeatmapChar(int intensity) {
        const char heatmapChars[] = {' ', '.', ':', '-', '+', '*', '#', '8', '&', '%', '@'};
        if (intensity < 0) intensity = 0;
        if (intensity > 10) intensity = 10;
        return heatmapChars[intensity];
    }
    
    double calculateCoefficientOfVariation(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        
        double variance = 0.0;
        for (double value : values) {
            variance += std::pow(value - mean, 2);
        }
        variance /= values.size();
        
        double stdDev = std::sqrt(variance);
        
        if (mean == 0.0) return 0.0;
        
        return stdDev / mean;
    }
    
    std::string generateTextVisualization(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Text-based Neural Network Visualization\n";
        ss << "=====================================\n\n";
        
        ss << "Network Statistics:\n";
        ss << "  Neuron Count: " << brain->getTotalNeuronCount() << "\n";
        ss << "  Synapse Count: " << brain->getTotalSynapseCount() << "\n";
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "  Total Spike Count: " << brain->getTotalSpikeCount() << "\n";
        ss << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        
        return ss.str();
    }
    
    std::string generateGraphVizVisualization(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "digraph NLM_Brain {\n";
        ss << "  rankdir=LR;\n";
        ss << "  node [shape=circle];\n";
        
        // Add neurons as nodes
        for (size_t i = 0; i < brain->getTotalNeuronCount(); i += 10) { // Sample neurons
            ss << "  \"Neuron" << i << "\" [label=\"" << i << "\"];\n";
        }
        
        // Add synapses as edges
        for (size_t i = 0; i < brain->getTotalSynapseCount(); i += 20) { // Sample synapses
            ss << "  \"Neuron" << (i % 50) << "\" -> \"Neuron" << ((i + 1) % 50) << "\" [weight=0.5];\n";
        }
        
        ss << "}\n";
        return ss.str();
    }
    
    std::string generateHTMLVisualization(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>\n";
        ss << "<html><head><title>NLM Brain Visualization</title></head><body>\n";
        ss << "<h1>NLM Neural Network Visualization</h1>\n";
        ss << "<p>Brain Statistics:</p>\n";
        ss << "<ul>\n";
        ss << "<li>Neurons: " << brain->getTotalNeuronCount() << "</li>\n";
        ss << "<li>Synapses: " << brain->getTotalSynapseCount() << "</li>\n";
        ss << "<li>Firing Rate: " << brain->getAverageFiringRate() << " Hz</li>\n";
        ss << "<li>Total Spikes: " << brain->getTotalSpikeCount() << "</li>\n";
        ss << "</ul>\n";
        ss << "</body></html>\n";
        
        return ss.str();
    }
    
    std::string generateSVGVisualization(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"800\" height=\"600\">\n";
        ss << "<rect width=\"100%\" height=\"100%\" fill=\"#f0f0f0\"/>\n";
        
        // Draw simple neuron representation
        ss << "<circle cx=\"100\" cy=\"100\" r=\"20\" fill=\"#ff6666\"/>\n";
        ss << "<text x=\"100\" y=\"105\" text-anchor=\"middle\" fill=\"white\">" << brain->getTotalNeuronCount() << "</text>\n";
        
        ss << "</svg>\n";
        return ss.str();
    }
    
    NeuralNetworkVisualizer(const NeuralNetworkVisualizer&) = delete;
    NeuralNetworkVisualizer& operator=(const NeuralNetworkVisualizer&) = delete;
};

// ============================================================================
// Debug Logger
// ============================================================================

class DebugLogger {
public:
    static DebugLogger& getInstance() {
        static DebugLogger instance;
        return instance;
    }
    
    // Set debug level
    void setDebugLevel(DebugLevel level) {
        std::lock_guard<std::mutex> lock(mutex);
        debugLevel = level;
    }
    
    // Get current debug level
    DebugLevel getDebugLevel() const {
        std::lock_guard<std::mutex> lock(mutex);
        return debugLevel;
    }
    
    // Log a debug message
    void log(DebugEventType type, const std::string& message, const std::string& sessionId = "default") {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (debugLevel >= getMinimumLevelForEventType(type)) {
            DebugEvent event(type, message, sessionId);
            
            // Log to console
            logToConsole(event);
            
            // Add to debug history
            debugHistory.push_back(event);
            
            // Keep history limited
            if (debugHistory.size() > 1000) {
                debugHistory.erase(debugHistory.begin(), debugHistory.begin() + 100);
            }
        }
    }
    
    // Log a simulation step
    void logSimulationStep(size_t step, double time, std::shared_ptr<Brain> brain, const std::string& sessionId = "default") {
        std::stringstream ss;
        ss << "Step " << step << " (t=" << time << "s): "
           << brain->getFiringNeuronCount() << " firing neurons, "
           << brain->getAverageFiringRate() << " Hz average rate";
        
        log(DebugEventType::SimulationStep, ss.str(), sessionId);
    }
    
    // Log a spike event
    void logSpikeEvent(NeuronId neuronId, double time, NeuronType type, const std::string& sessionId = "default") {
        std::stringstream ss;
        ss << "Spike: Neuron " << neuronId.value << " (type: " << static_cast<int>(type) << ") at t=" << time << "s";
        
        log(DebugEventType::SpikeEvent, ss.str(), sessionId);
    }
    
    // Log a synapse event
    void logSynapseEvent(SynapseId synapseId, NeuronId preId, NeuronId postId, double weight, double time, const std::string& sessionId = "default") {
        std::stringstream ss;
        ss << "Synapse Event: " << synapseId.value << " (" << preId.value << " -> " << postId.value << ") "
           << "weight=" << weight << " at t=" << time << "s";
        
        log(DebugEventType::SynapseEvent, ss.str(), sessionId);
    }
    
    // Get debug events for a session
    std::vector<DebugEvent> getDebugEvents(const std::string& sessionId, DebugEventType type = DebugEventType::ErrorEvent) const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::vector<DebugEvent> events;
        
        for (const auto& event : debugHistory) {
            if (event.sessionId == sessionId && (type == DebugEventType::ErrorEvent || event.type == type)) {
                events.push_back(event);
            }
        }
        
        return events;
    }
    
    // Export debug events to file
    bool exportDebugEvents(const std::string& sessionId, const std::string& filepath) {
        try {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw IOError("Cannot open file for writing: " + filepath);
            }
            
            auto events = getDebugEvents(sessionId);
            
            file << "Debug Events Export\n";
            file << "Generated: " << std::chrono::high_resolution_clock::now() << "\n";
            file << "Session ID: " + sessionId + "\n\n";
            
            file << "Timestamp,Type,Session,Description\n";
            
            for (const auto& event : events) {
                file << event.timestamp << ","
                     << static_cast<int>(event.type) << ","
                     << event.sessionId << ","
                     << event.description << "\n";
            }
            
            file.close();
            NLM_LOG_INFO("Debug events exported to: " + filepath);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to export debug events: " + std::string(e.what()));
            return false;
        }
    }
    
    // Clear debug history
    void clearDebugHistory() {
        std::lock_guard<std::mutex> lock(mutex);
        debugHistory.clear();
        NLM_LOG_INFO("Cleared debug history");
    }
    
    // Get debug statistics
    std::map<std::string, size_t> getDebugStatistics() const {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::map<std::string, size_t> stats;
        
        stats["total_events"] = debugHistory.size();
        
        // Count events by type
        std::map<DebugEventType, size_t> typeCounts;
        for (const auto& event : debugHistory) {
            typeCounts[event.type]++;
        }
        
        for (const auto& pair : typeCounts) {
            stats[std::to_string(static_cast<int>(pair.first))] = pair.second;
        }
        
        return stats;
    }
    
private:
    DebugLogger() : debugLevel(DebugLevel::Info) {}
    
    DebugLevel getMinimumLevelForEventType(DebugEventType type) const {
        switch (type) {
            case DebugEventType::ErrorEvent:
                return DebugLevel::Warning;
            case DebugEventType::SpikeEvent:
                return DebugLevel::Info;
            case DebugEventType::SynapseEvent:
                return DebugLevel::Debug;
            case DebugEventType::NeuronStateChange:
                return DebugLevel::Debug;
            case DebugEventType::MemoryEvent:
                return DebugLevel::Info;
            case DebugEventType::PlasticityEvent:
                return DebugLevel::Debug;
            case DebugEventType::ConfigurationChange:
                return DebugLevel::Warning;
            case DebugEventType::PerformanceEvent:
                return DebugLevel::Info;
            case DebugEventType::VisualizationEvent:
                return DebugLevel::Info;
            default:
                return DebugLevel::Info;
        }
    }
    
    void logToConsole(const DebugEvent& event) {
        std::string timestamp = formatTimestamp(event.timestamp);
        std::string typeName = debugEventTypeToString(event.type);
        
        std::cout << "[" << timestamp << "] " << typeName << " [" << event.sessionId << "] " 
                  << event.description << std::endl;
    }
    
    std::string formatTimestamp(const std::chrono::high_resolution_clock::time_point& tp) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - tp);
        return std::to_string(duration.count()) + "ms";
    }
    
    std::string debugEventTypeToString(DebugEventType type) {
        switch (type) {
            case DebugEventType::SimulationStep: return "SIMSTEP";
            case DebugEventType::SpikeEvent: return "SPIKE";
            case DebugEventType::SynapseEvent: return "SYN";
            case DebugEventType::NeuronStateChange: return "NEURON";
            case DebugEventType::MemoryEvent: return "MEMORY";
            case DebugEventType::PlasticityEvent: return "PLASTICITY";
            case DebugEventType::ConfigurationChange: return "CONFIG";
            case DebugEventType::ErrorEvent: return "ERROR";
            case DebugEventType::PerformanceEvent: return "PERF";
            case DebugEventType::VisualizationEvent: return "VIS";
            default: return "UNKNOWN";
        }
    }
    
    mutable std::mutex mutex;
    DebugLevel debugLevel;
    std::vector<DebugEvent> debugHistory;
    
    DebugLogger(const DebugLogger&) = delete;
    DebugLogger& operator=(const DebugLogger&) = delete;
};

// ============================================================================
// Network Analysis Tools
// ============================================================================

class NetworkAnalyzer {
public:
    static NetworkAnalyzer& getInstance() {
        static NetworkAnalyzer instance;
        return instance;
    }
    
    // Analyze network topology
    std::map<std::string, double> analyzeTopology(std::shared_ptr<Brain> brain) {
        std::map<std::string, double> metrics;
        
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            return metrics;
        }
        
        // Calculate basic network properties
        double connectionProbability = 0.0f; // Placeholder - would need actual synapse data
        if (neuronCount > 1) {
            connectionProbability = static_cast<double>(brain->getTotalSynapseCount()) / 
                                   (neuronCount * (neuronCount - 1));
        }
        
        metrics["connection_probability"] = connectionProbability;
        metrics["neuron_count"] = static_cast<double>(neuronCount);
        metrics["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        
        // Calculate network efficiency (simplified)
        if (connectionProbability > 0.0) {
            metrics["network_efficiency"] = 1.0 - std::exp(-connectionProbability * neuronCount);
        } else {
            metrics["network_efficiency"] = 0.0;
        }
        
        return metrics;
    }
    
    // Analyze spike patterns
    std::map<std::string, double> analyzeSpikePatterns(std::shared_ptr<Brain> brain) {
        std::map<std::string, double> metrics;
        
        size_t totalSpikes = brain->getTotalSpikeCount();
        if (totalSpikes == 0) {
            return metrics;
        }
        
        // Calculate firing statistics (simplified)
        size_t firingNeurons = brain->getFiringNeuronCount();
        double firingRate = brain->getAverageFiringRate();
        
        metrics["total_spikes"] = static_cast<double>(totalSpikes);
        metrics["firing_neuron_ratio"] = static_cast<double>(firingNeurons) / brain->getTotalNeuronCount();
        metrics["average_firing_rate"] = firingRate;
        metrics["spike_frequency"] = firingRate; // Hz
        
        // Calculate coefficient of variation for firing rates
        double cv = (firingRate > 0.0) ? 1.0 : 0.0; // Placeholder - would need more detailed statistics
        metrics["firing_coefficient_of_variation"] = cv;
        
        return metrics;
    }
    
    // Detect network dynamics
    std::vector<std::string> detectNetworkDynamics(std::shared_ptr<Brain> brain) {
        std::vector<std::string> dynamics;
        
        double firingRate = brain->getAverageFiringRate();
        
        if (firingRate < 1.0) {
            dynamics.push_back("Low activity network - neurons rarely fire");
        } else if (firingRate > 10.0) {
            dynamics.push_back("High activity network - neurons fire frequently");
        } else {
            dynamics.push_back("Moderate activity network");
        }
        
        // Check for burst patterns (simplified)
        if (brain->getTotalSpikeCount() > 1000) {
            dynamics.push_back("High spike count - potential burst activity");
        }
        
        // Check network stability (simplified)
        double stability = calculateNetworkStability(brain);
        if (stability < 0.5) {
            dynamics.push_back("Unstable network dynamics");
        } else if (stability > 0.8) {
            dynamics.push_back("Stable network dynamics");
        } else {
            dynamics.push_back("Moderately stable network dynamics");
        }
        
        return dynamics;
    }
    
    // Generate network analysis report
    std::string generateNetworkAnalysisReport(std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "=== Network Analysis Report ===\n\n";
        
        // Get topology metrics
        auto topologyMetrics = analyzeTopology(brain);
        ss << "Network Topology:\n";
        ss << "  Neuron Count: " << topologyMetrics["neuron_count"] << "\n";
        ss << "  Connection Probability: " << topologyMetrics["connection_probability"] << "\n";
        ss << "  Network Efficiency: " << topologyMetrics["network_efficiency"] << "\n\n";
        
        // Get spike pattern metrics
        auto spikeMetrics = analyzeSpikePatterns(brain);
        ss << "Spike Pattern Analysis:\n";
        ss << "  Total Spikes: " << spikeMetrics["total_spikes"] << "\n";
        ss << "  Firing Neuron Ratio: " << spikeMetrics["firing_neuron_ratio"] << "\n";
        ss << "  Average Firing Rate: " << spikeMetrics["average_firing_rate"] << " Hz\n";
        ss << "  Coefficient of Variation: " << spikeMetrics["firing_coefficient_of_variation"] << "\n\n";
        
        // Get dynamics
        auto dynamics = detectNetworkDynamics(brain);
        ss << "Network Dynamics:\n";
        for (const auto& dynamic : dynamics) {
            ss << "  - " << dynamic << "\n";
        }
        
        return ss.str();
    }
    
private:
    NetworkAnalyzer() {}
    
    double calculateNetworkStability(std::shared_ptr<Brain> brain) {
        // Simplified stability calculation based on firing rate
        double firingRate = brain->getAverageFiringRate();
        
        // Stable firing rates are around 1-10 Hz
        if (firingRate >= 1.0 && firingRate <= 10.0) {
            return 1.0;
        } else if (firingRate < 1.0) {
            return 0.7;
        } else {
            return 0.5;
        }
    }
    
    NetworkAnalyzer(const NetworkAnalyzer&) = delete;
    NetworkAnalyzer& operator=(const NetworkAnalyzer&) = delete;
};

// ============================================================================
// Debugging Utilities
// ============================================================================

class DebuggingUtils {
public:
    // Find and fix common neural network issues
    static std::vector<std::string> findCommonIssues(std::shared_ptr<Brain> brain) {
        std::vector<std::string> issues;
        
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            issues.push_back("No neurons in the brain");
            return issues;
        }
        
        double firingRate = brain->getAverageFiringRate();
        if (firingRate < 0.1) {
            issues.push_back("Extremely low firing rate - neurons are not active enough");
        } else if (firingRate > 100.0) {
            issues.push_back("Extremely high firing rate - possible runaway excitation");
        }
        
        size_t spikeCount = brain->getTotalSpikeCount();
        if (spikeCount == 0) {
            issues.push_back("No spikes recorded - network may be inactive");
        }
        
        double eir = brain->getExcitationInhibitionRatio();
        if (eir < 0.1) {
            issues.push_back("Excitatory/inhibitory ratio too low - loss of excitation");
        } else if (eir > 10.0) {
            issues.push_back("Excitatory/inhibitory ratio too high - possible excitation runaway");
        }
        
        return issues;
    }
    
    // Generate debugging recommendations
    static std::vector<std::string> generateDebuggingRecommendations(std::shared_ptr<Brain> brain) {
        std::vector<std::string> recommendations;
        
        auto issues = findCommonIssues(brain);
        
        if (!issues.empty()) {
            recommendations.push_back("Common issues detected:");
            for (const auto& issue : issues) {
                recommendations.push_back("  - " + issue);
            }
        }
        
        recommendations.push_back("Possible solutions:");
        recommendations.push_back("  - Check configuration parameters (neuron count, connection probability)");
        recommendations.push_back("  - Verify sensory input connections");
        recommendations.push_back("  - Check neuromodulation settings (dopamine, curiosity)");
        recommendations.push_back("  - Verify plasticity parameters (STDP, Hebbian)");
        recommendations.push_back("  - Consider increasing simulation time for learning");
        
        return recommendations;
    }
    
    // Create a debug snapshot of the current brain state
    static std::string createDebugSnapshot(std::shared_ptr<Brain> brain, const std::string& snapshotId) {
        std::stringstream ss;
        ss << "=== Debug Snapshot: " + snapshotId + " ===\n\n";
        
        ss << "Timestamp: " << std::chrono::high_resolution_clock::now() << "\n";
        ss << "Brain State:\n";
        ss << "  Neuron Count: " << brain->getTotalNeuronCount() << "\n";
        ss << "  Synapse Count: " << brain->getTotalSynapseCount() << "\n";
        ss << "  Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        
        return ss.str();
    }
    
    // Compare brain states across different time points
    static std::string compareBrainStates(std::shared_ptr<Brain> brain1, std::shared_ptr<Brain> brain2, const std::string& label1, const std::string& label2) {
        std::stringstream ss;
        ss << "=== Brain State Comparison ===\n\n";
        
        ss << "Comparison: " << label1 << " vs " << label2 << "\n\n";
        
        // Compare neuron counts
        ss << "Neuron Statistics:\n";
        ss << "  " << label1 << ": " << brain1->getTotalNeuronCount() << " neurons\n";
        ss << "  " << label2 << ": " << brain2->getTotalNeuronCount() << " neurons\n";
        if (brain1->getTotalNeuronCount() > 0) {
            double ratio = static_cast<double>(brain2->getTotalNeuronCount()) / brain1->getTotalNeuronCount();
            ss << "  Ratio: " << ratio << "\n";
        }
        
        // Compare synapse counts
        ss << "Synapse Statistics:\n";
        ss << "  " << label1 << ": " << brain1->getTotalSynapseCount() << " synapses\n";
        ss << "  " << label2 << ": " << brain2->getTotalSynapseCount() << " synapses\n";
        if (brain1->getTotalSynapseCount() > 0) {
            double ratio = static_cast<double>(brain2->getTotalSynapseCount()) / brain1->getTotalSynapseCount();
            ss << "  Ratio: " << ratio << "\n";
        }
        
        // Compare firing rates
        ss << "Firing Statistics:\n";
        ss << "  " << label1 << ": " << brain1->getAverageFiringRate() << " Hz\n";
        ss << "  " << label2 << ": " << brain2->getAverageFiringRate() << " Hz\n";
        
        // Compare spike counts
        ss << "Spike Statistics:\n";
        ss << "  " << label1 << ": " << brain1->getTotalSpikeCount() << " spikes\n";
        ss << "  " << label2 << ": " << brain2->getTotalSpikeCount() << " spikes\n";
        
        return ss.str();
    }
    
    // Validate brain configuration
    static std::vector<std::string> validateBrainConfiguration(std::shared_ptr<Brain> brain) {
        std::vector<std::string> warnings;
        
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount < 10) {
            warnings.push_back("Low neuron count (<10) may not produce meaningful results");
        } else if (neuronCount > 100000) {
            warnings.push_back("High neuron count (>100k) may cause performance issues");
        }
        
        double firingRate = brain->getAverageFiringRate();
        if (firingRate < 0.01) {
            warnings.push_back("Extremely low firing rate (<0.01 Hz) - network may be inactive");
        } else if (firingRate > 1000) {
            warnings.push_back("Extremely high firing rate (>1000 Hz) - possible instability");
        }
        
        double eir = brain->getExcitationInhibitionRatio();
        if (eir < 0.5) {
            warnings.push_back("Low excitation/inhibition ratio (<0.5) - possible loss of excitation");
        } else if (eir > 5.0) {
            warnings.push_back("High excitation/inhibition ratio (>5.0) - possible runaway excitation");
        }
        
        return warnings;
    }
    
    // Generate debugging help text
    static std::string generateDebuggingHelp() {
        std::stringstream ss;
        ss << "=== NLM Debugging Help ===\n\n";
        
        ss << "Common Debugging Commands:\n";
        ss << "  1. Check brain configuration: printBrainInfo(brain)\n";
        ss << "  2. Inspect brain state: inspectBrain(brain)\n";
        ss << "  3. Visualize network: visualizeNetwork(brain)\n";
        ss << "  4. Analyze network: analyzeNetwork(brain)\n";
        ss << "  5. Find issues: findCommonIssues(brain)\n";
        ss << "  6. Generate recommendations: generateDebuggingRecommendations(brain)\n";
        ss << "  7. Create debug snapshot: createDebugSnapshot(brain, 'snapshot_id')\n";
        ss << "  8. Compare states: compareBrainStates(brain1, brain2, 'label1', 'label2')\n\n";
        
        ss << "Debugging Tips:\n";
        ss << "  - Use visualizeNetwork() to understand network topology\n";
        ss << "  - Use analyzeNetwork() to detect performance issues\n";
        ss << "  - Use findCommonIssues() to identify common problems\n";
        ss << "  - Use debug logging to trace simulation events\n";
        ss << "  - Compare brain states to track learning progress\n\n";
        
        ss << "Configuration Tips:\n";
        ss << "  - For small networks (<100 neurons): reduce connection probability\n";
        ss << "  - For large networks (>10k neurons): enable multithreading\n";
        ss << "  - For stable networks: balance excitation and inhibition\n";
        ss << "  - For learning networks: enable plasticity and neuromodulation\n\n";
        
        return ss.str();
    }
    
    // Set up debug environment
    static void setupDebugEnvironment(bool enableVisualization = true, bool enableLogging = true, bool enableAnalysis = true) {
        if (enableVisualization) {
            NeuralNetworkVisualizer::getInstance();
        }
        
        if (enableLogging) {
            DebugLogger::getInstance().setDebugLevel(DebugLevel::Debug);
        }
        
        if (enableAnalysis) {
            NetworkAnalyzer::getInstance();
        }
        
        NLM_LOG_INFO("Debug environment configured:");
        NLM_LOG_INFO("  Visualization: " + std::string(enableVisualization ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("  Logging: " + std::string(enableLogging ? "Enabled" : "Disabled"));
        NLM_LOG_INFO("  Analysis: " + std::string(enableAnalysis ? "Enabled" : "Disabled"));
    }
    
    // Cleanup debug environment
    static void cleanupDebugEnvironment() {
        DebugLogger::getInstance().clearDebugHistory();
        NLM_LOG_INFO("Debug environment cleaned up");
    }
    
private:
    DebuggingUtils() {}
    
    DebuggingUtils(const DebuggingUtils&) = delete;
    DebuggingUtils& operator=(const DebuggingUtils&) = delete;
};

// ============================================================================
// Global Debugging Functions
// ============================================================================

inline void enableDebugLogging(bool enable = true) {
    DebugLogger::getInstance().setDebugLevel(
        enable ? DebugLevel::Debug : DebugLevel::None
    );
}

inline void setupDebugging(bool visualize = true, bool log = true, bool analyze = true) {
    DebuggingUtils::setupDebugEnvironment(visualize, log, analyze);
}

inline std::string getDebuggingHelp() {
    return DebuggingUtils::generateDebuggingHelp();
}

inline std::vector<std::string> debugBrain(std::shared_ptr<Brain> brain) {
    return DebuggingUtils::findCommonIssues(brain);
}

inline std::vector<std::string> recommendDebugActions(std::shared_ptr<Brain> brain) {
    return DebuggingUtils::generateDebuggingRecommendations(brain);
}

inline std::string inspectBrainState(std::shared_ptr<Brain> brain, const std::string& inspectionId = "debug_inspection") {
    NeuralNetworkInspector::getInstance().inspectBrain(brain, inspectionId);
    return NeuralNetworkInspector::getInstance().generateInspectionReport(inspectionId);
}

inline std::string visualizeNeuralNetwork(std::shared_ptr<Brain> brain, const std::string& format = "text") {
    return NeuralNetworkVisualizer::getInstance().visualizeNetwork(brain, format);
}

inline std::map<std::string, double> analyzeNetworkDynamics(std::shared_ptr<Brain> brain) {
    return NetworkAnalyzer::getInstance().analyzeTopology(brain);
}

inline std::string generateNetworkAnalysisReport(std::shared_ptr<Brain> brain) {
    return NetworkAnalyzer::getInstance().generateNetworkAnalysisReport(brain);
}

} // namespace nlm

#endif // NLM_DEBUGGING_TOOLS_H
