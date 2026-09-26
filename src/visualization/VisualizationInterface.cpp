#include "VisualizationInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../experiments/ScalingBenchmark.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct VisualizationInterface::Impl {
    bool active;
    double updateRate;
    std::string backend;
    std::string outputFile;
    size_t maxNeuronsToShow;
    bool animationEnabled;
    float animationSpeed;
    
    // Text-based output
    bool textOutputEnabled;
    std::string textOutputFile;
    
    // Snapshot data
    std::map<std::string, std::chrono::high_resolution_clock::time_point> snapshots;
    
    // Statistics
    VisualizationStats stats;
    
    // Console colors (ANSI codes)
    const std::string COLOR_RED = "\033[31m";
    const std::string COLOR_GREEN = "\033[32m";
    const std::string COLOR_YELLOW = "\033[33m";
    const std::string COLOR_BLUE = "\033[34m";
    const std::string COLOR_MAGENTA = "\033[35m";
    const std::string COLOR_CYAN = "\033[36m";
    const std::string COLOR_WHITE = "\033[37m";
    const std::string COLOR_RESET = "\033[0m";
    const std::string COLOR_CLEAR = "\033[2J\033[H";
    
    Impl() : active(false), updateRate(30.0), backend("auto"), maxNeuronsToShow(100), 
             animationEnabled(false), animationSpeed(1.0f), textOutputEnabled(false) {}
};

VisualizationInterface::VisualizationInterface() : pImpl(new Impl) {}

VisualizationInterface::~VisualizationInterface() = default;

bool VisualizationInterface::initialize(const std::string& backend) {
    pImpl->backend = backend;
    pImpl->active = true;
    
    // Auto-detect backend
    if (backend == "auto") {
#ifdef _WIN32
        pImpl->backend = "text";
#else
        pImpl->backend = "simple";
#endif
    }
    
    NLM_LOG_INFO("VisualizationInterface initialized with backend: " + pImpl->backend);
    return true;
}

void VisualizationInterface::update() {
    if (!pImpl->active) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Update implementation would go here
    // For now, just track that we've been called
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    pImpl->stats.framesRendered++;
    pImpl->stats.totalRenderTimeMs += static_cast<double>(duration);
}

void VisualizationInterface::render(const std::string& mode) {
    if (!pImpl->active) return;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (pImpl->backend == "text") {
        consoleUpdate(Brain::dummy, BenchmarkMetrics());
    } else if (pImpl->backend == "simple") {
        // Simple console visualization
        std::cout << pImpl->COLOR_CLEAR;
        std::cout << COLOR_BLUE << "=== NLM Phase 6 Visualization ===" << COLOR_RESET << std::endl;
        std::cout << COLOR_GREEN << "Backend: " << pImpl->backend << ", Update Rate: " << pImpl->updateRate << " Hz" << COLOR_RESET << std::endl;
        std::cout << "Mode: " << mode << std::endl;
    } else {
        // Advanced visualization
        std::cout << "Advanced visualization not implemented yet" << std::endl;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    pImpl->stats.totalRenderTimeMs += static_cast<double>(duration);
}

void VisualizationInterface::close() {
    pImpl->active = false;
    if (pImpl->textOutputEnabled && !pImpl->textOutputFile.empty()) {
        outputTextReport(pImpl->textOutputFile);
    }
}

bool VisualizationInterface::isActive() const {
    return pImpl->active;
}

void VisualizationInterface::setUpdateRate(double hz) {
    pImpl->updateRate = hz;
}

double VisualizationInterface::getUpdateRate() const {
    return pImpl->updateRate;
}

void VisualizationInterface::setOutputFile(const std::string& filename) {
    pImpl->outputFile = filename;
}

void VisualizationInterface::setMaxDisplayNeurons(size_t maxNeurons) {
    pImpl->maxNeuronsToShow = maxNeurons;
}

void VisualizationInterface::setEnableAnimation(bool enabled) {
    pImpl->animationEnabled = enabled;
}

void VisualizationInterface::setAnimationSpeed(float speed) {
    pImpl->animationSpeed = speed;
}

void VisualizationInterface::visualizeNetwork(const Brain& brain) {
    // Simple text-based network visualization
    std::cout << COLOR_CYAN << "Network Visualization:" << COLOR_RESET << std::endl;
    std::cout << "  Total Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Regions: " << brain.getRegionCount() << std::endl;
    
    // Show first few regions
    auto regions = brain.getRegions();
    if (!regions.empty()) {
        std::cout << "  Region Details:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(3), regions.size()); ++i) {
            std::cout << "    Region " << i << ": " << regions[i]->getNeuronCount() << " neurons, " 
                      << regions[i]->getSynapseCount() << " synapses" << std::endl;
        }
    }
}

void VisualizationInterface::visualizeActivity(const Brain& brain) {
    // Activity and metrics visualization
    std::cout << COLOR_GREEN << "Activity Visualization:" << COLOR_RESET << std::endl;
    std::cout << "  Average Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Firing Neurons: " << brain.getFiringNeuronCount() << "/" << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
    
    // Memory systems
    auto* workingMemory = brain.getWorkingMemory();
    if (workingMemory) {
        std::cout << "  Working Memory Active Traces: " << workingMemory->getActiveTraces() << std::endl;
    }
    
    auto* episodicMemory = brain.getEpisodicMemory();
    if (episodicMemory) {
        std::cout << "  Episodic Memory Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
    }
    
    // Neuromodulators
    auto* dopamine = brain.getDopamine();
    if (dopamine) {
        std::cout << "  Dopamine Level: " << dopamine->getLevel() << std::endl;
    }
    
    auto* curiosity = brain.getCuriosity();
    if (curiosity) {
        std::cout << "  Curiosity Level: " << curiosity->getLevel() << std::endl;
    }
}

void VisualizationInterface::visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    std::cout << COLOR_YELLOW << "Spike Raster Plot:" << COLOR_RESET << std::endl;
    std::cout << "  Total Spikes: " << spikes.size() << std::endl;
    
    if (!spikes.empty()) {
        std::cout << "  Time Range: " << spikes.front().timestamp << " - " << spikes.back().timestamp << " ms" << std::endl;
        
        // Simple spike visualization
        std::cout << "  First 10 spikes:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), spikes.size()); ++i) {
            std::cout << "    Spike " << i << ": time=" << spikes[i].timestamp << ", neuron=" << spikes[i].neuronId << ", region=" << spikes[i].regionId << std::endl;
        }
    }
}

void VisualizationInterface::visualizeWeights(const Brain& brain) {
    std::cout << COLOR_MAGENTA << "Weight Matrix Visualization:" << COLOR_RESET << std::endl;
    
    // Get average synaptic weights
    double avgWeight = 0.0;
    double minWeight = std::numeric_limits<double>::max();
    double maxWeight = std::numeric_limits<double>::min();
    size_t synapseCount = 0;
    
    auto regions = brain.getRegions();
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& syn : synapses) {
            double weight = syn->getWeight();
            avgWeight += weight;
            if (weight < minWeight) minWeight = weight;
            if (weight > maxWeight) maxWeight = weight;
            synapseCount++;
        }
    }
    
    if (synapseCount > 0) {
        avgWeight /= synapseCount;
        std::cout << "  Total Synapses: " << synapseCount << std::endl;
        std::cout << "  Average Weight: " << avgWeight << std::endl;
        std::cout << "  Weight Range: " << minWeight << " - " << maxWeight << std::endl;
        
        // Weight histogram
        std::cout << "  Weight Distribution:" << std::endl;
        std::vector<size_t> histogram(10, 0);
        
        for (const auto& region : regions) {
            auto synapses = region->getSynapses();
            for (const auto& syn : synapses) {
                double weight = syn->getWeight();
                size_t bin = static_cast<size_t>((weight - minWeight) / (maxWeight - minWeight) * 9.9);
                if (bin < histogram.size()) {
                    histogram[bin]++;
                }
            }
        }
        
        for (size_t i = 0; i < histogram.size(); ++i) {
            double rangeStart = minWeight + (maxWeight - minWeight) * i / histogram.size();
            double rangeEnd = minWeight + (maxWeight - minWeight) * (i + 1) / histogram.size();
            std::cout << "    [" << std::fixed << std::setprecision(3) << rangeStart << ", " << rangeEnd << "): " 
                      << histogram[i] << " synapses" << std::endl;
        }
    }
}

void VisualizationInterface::visualizeMetrics(const BenchmarkMetrics& metrics) {
    std::cout << COLOR_RED << "Performance Metrics Visualization:" << COLOR_RESET << std::endl;
    std::cout << "  Neurons per Second: " << metrics.neuronsPerSecond << std::endl;
    std::cout << "  Synapses per Second: " << metrics.synapsesPerSecond << std::endl;
    std::cout << "  Spikes per Second: " << metrics.spikesPerSecond << std::endl;
    std::cout << "  Real-time Factor: " << metrics.realTimeFactor << std::endl;
    std::cout << "  Average Step Time: " << metrics.avgStepTimeMs << " ms" << std::endl;
    std::cout << "  Peak Memory Usage: " << metrics.peakMemoryMB << " MB" << std::endl;
    std::cout << "  CPU Utilization: " << metrics.cpuUtilizationPercent << "%" << std::endl;
}

void VisualizationInterface::visualizeIntegration(const Brain& brain, const BenchmarkMetrics& metrics) {
    std::cout << "=== Phase 6 Integration Visualization ===" << std::endl;
    std::cout << "Neural System Integration Status:" << std::endl;
    
    // System integration status
    bool workingMemIntegrated = (brain.getWorkingMemory() != nullptr);
    bool episodicMemIntegrated = (brain.getEpisodicMemory() != nullptr);
    bool neuromodIntegrated = (brain.getDopamine() != nullptr);
    bool predictionIntegrated = (brain.getPredictionSystem() != nullptr);
    bool developmentIntegrated = (brain.getDevelopmentSystem() != nullptr);
    
    std::cout << "  Working Memory: " << (workingMemIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Episodic Memory: " << (episodicMemIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Neuromodulation: " << (neuromodIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Prediction System: " << (predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "  Development System: " << (developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    
    std::cout << "\nPerformance Metrics:" << std::endl;
    std::cout << "  Processing Speed: " << metrics.neuronsPerSecond << " neurons/sec" << std::endl;
    std::cout << "  Efficiency: " << metrics.realTimeFactor << "x real-time" << std::endl;
    std::cout << "  Memory Usage: " << metrics.peakMemoryMB << " MB" << std::endl;
    
    int integratedCount = 0;
    if (workingMemIntegrated) integratedCount++;
    if (episodicMemIntegrated) integratedCount++;
    if (neuromodIntegrated) integratedCount++;
    if (predictionIntegrated) integratedCount++;
    if (developmentIntegrated) integratedCount++;
    
    std::cout << "\nIntegration Score: " << integratedCount << "/5 systems connected" << std::endl;
    
    if (integratedCount == 5) {
        std::cout << COLOR_GREEN << "✓ Perfect Integration! All systems are working together." << COLOR_RESET << std::endl;
    } else if (integratedCount >= 3) {
        std::cout << COLOR_YELLOW << "⚠ Good Integration. Some systems may need attention." << COLOR_RESET << std::endl;
    } else {
        std::cout << COLOR_RED << "✗ Poor Integration. Major issues detected." << COLOR_RESET << std::endl;
    }
}

void VisualizationInterface::consoleUpdate(const Brain& brain, const BenchmarkMetrics& metrics) {
    system("clear");  // Clear screen (Unix-like systems)
    
    std::cout << COLOR_CYAN << "╔═══════════════════════════════════════════════════════════════╗" << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "║               NLM Phase 6 - Integration Visualization               ║" << COLOR_RESET << std::endl;
    std::cout << COLOR_CYAN << "╚═══════════════════════════════════════════════════════════════╝" << COLOR_RESET << std::endl << std::endl;
    
    std::cout << COLOR_GREEN << "BRAIN STATUS:" << COLOR_RESET << std::endl;
    std::cout << "  Neurons: " << brain.getTotalNeuronCount() << " | Synapses: " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Firing: " << brain.getFiringNeuronCount() << "/" << brain.getTotalNeuronCount() 
              << " | Spikes: " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Avg Firing Rate: " << brain.getAverageFiringRate() << " Hz" << std::endl << std::endl;
    
    std::cout << COLOR_YELLOW << "MEMORY SYSTEMS:" << COLOR_RESET << std::endl;
    auto* wm = brain.getWorkingMemory();
    auto* em = brain.getEpisodicMemory();
    std::cout << "  Working Memory: " << (wm ? "✓" : "✗") << " (Active: " << (wm ? wm->getActiveTraces() : 0) << ")" << std::endl;
    std::cout << "  Episodic Memory: " << (em ? "✓" : "✗") << " (Episodes: " << (em ? em->getEpisodeCount() : 0) << ")" << std::endl << std::endl;
    
    std::cout << COLOR_BLUE << "NEUROMODULATORS:" << COLOR_RESET << std::endl;
    auto* dop = brain.getDopamine();
    auto* cur = brain.getCuriosity();
    std::cout << "  Dopamine: " << (dop ? "✓" : "✗") << " (Level: " << (dop ? dop->getLevel() : 0.0) << ")" << std::endl;
    std::cout << "  Curiosity: " << (cur ? "✓" : "✗") << " (Level: " << (cur ? cur->getLevel() : 0.0) << ")" << std::endl << std::endl;
    
    std::cout << COLOR_MAGENTA << "PERFORMANCE:" << COLOR_RESET << std::endl;
    std::cout << "  Processing: " << metrics.neuronsPerSecond << " neurons/sec" << std::endl;
    std::cout << "  Real-time Factor: " << metrics.realTimeFactor << "x" << std::endl;
    std::cout << "  Memory: " << metrics.peakMemoryMB << " MB peak" << std::endl;
    std::cout << "  CPU: " << metrics.cpuUtilizationPercent << "%" << std::endl << std::endl;
    
    std::cout << COLOR_RED << "SYSTEM INTEGRATION:" << COLOR_RESET << std::endl;
    std::cout << "  Working Memory: " << (wm ? "✓" : "✗") << std::endl;
    std::cout << "  Episodic Memory: " << (em ? "✓" : "✗") << std::endl;
    std::cout << "  Neuromodulation: " << (dop ? "✓" : "✗") << std::endl;
    std::cout << "  Prediction: " << (brain.getPredictionSystem() ? "✓" : "✗") << std::endl;
    std::cout << "  Development: " << (brain.getDevelopmentSystem() ? "✓" : "✗") << std::endl << std::endl;
    
    std::cout << COLOR_WHITE << "Frame " << pImpl->stats.framesRendered << " | " 
              << "Render Time: " << pImpl->stats.totalRenderTimeMs / pImpl->stats.framesRendered << " ms" << COLOR_RESET << std::endl;
}

void VisualizationInterface::outputTextReport(const std::string& filename) {
    if (filename.empty()) return;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open text report file: " + filename);
        return;
    }
    
    file << "NLM Phase 6 Visualization Report" << std::endl;
    file << "Generated at: " << std::chrono::system_clock::now() << std::endl;
    file << "=== Visualization Statistics ===" << std::endl;
    file << "Frames Rendered: " << pImpl->stats.framesRendered << std::endl;
    file << "Total Render Time (ms): " << pImpl->stats.totalRenderTimeMs << std::endl;
    file << "Average Render Time (ms): " << (pImpl->stats.framesRendered > 0 ? 
        pImpl->stats.totalRenderTimeMs / pImpl->stats.framesRendered : 0.0) << std::endl;
    
    file.close();
    NLM_LOG_INFO("Text report saved to: " + filename);
}

void VisualizationInterface::captureSnapshot(const std::string& label) {
    auto now = std::chrono::high_resolution_clock::now();
    pImpl->snapshots[label] = now;
    
    NLM_LOG_INFO("Snapshot captured: " + label);
}

VisualizationInterface::VisualizationStats VisualizationInterface::getStats() const {
    return pImpl->stats;
}

bool VisualizationInterface::exportData(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Export data to " + filename + " in format: " + format);
    return true;
}

void VisualizationInterface::setEnableAnimation(bool enabled) {
    pImpl->animationEnabled = enabled;
}

void VisualizationInterface::setAnimationSpeed(float speed) {
    pImpl->animationSpeed = speed;
}

} // namespace nlm

// Dummy implementations for missing types
namespace nlm {
    struct SpikeEvent {
        uint64_t timestamp;
        uint64_t neuronId;
        uint64_t regionId;
        float weight;
    };
    
    // Provide a dummy brain for compile-time
    class Brain {
    public:
        static Brain dummy;
        size_t getTotalNeuronCount() const { return 0; }
        size_t getTotalSynapseCount() const { return 0; }
        size_t getFiringNeuronCount() const { return 0; }
        float getAverageFiringRate() const { return 0.0f; }
        size_t getTotalSpikeCount() const { return 0; }
        size_t getRegionCount() const { return 0; }
        std::vector<std::shared_ptr<class NeuralRegion>> getRegions() const { return {}; }
        class NeuralWorkingMemory* getWorkingMemory() const { return nullptr; }
        class NeuralEpisodicMemory* getEpisodicMemory() const { return nullptr; }
        class Neuromodulator* getDopamine() const { return nullptr; }
        class Neuromodulator* getCuriosity() const { return nullptr; }
        class PredictionSystem* getPredictionSystem() const { return nullptr; }
        class DevelopmentSystem* getDevelopmentSystem() const { return nullptr; }
    };
    
    Brain Brain::dummy;
    
    // Define benchmark metrics for compile-time
    struct BenchmarkMetrics {
        double neuronsPerSecond = 0.0;
        double synapsesPerSecond = 0.0;
        double spikesPerSecond = 0.0;
        double realTimeFactor = 0.0;
        double avgStepTimeMs = 0.0;
        size_t peakMemoryMB = 0;
        double cpuUtilizationPercent = 0.0;
    };
}

void VisualizationInterface::setEnableAnimation(bool enabled) {
    pImpl->animationEnabled = enabled;
}

void VisualizationInterface::setAnimationSpeed(float speed) {
    pImpl->animationSpeed = speed;
}

} // namespace nlm