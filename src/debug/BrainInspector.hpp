#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Synapse.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../dynamics/SpikeEvent.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"

namespace nlm {

class BrainInspector {
public:
    BrainInspector();
    ~BrainInspector();
    
    // Inspection modes
    enum class InspectionMode {
        Full,
        Structural,
        Functional,
        Connectivity,
        Plasticity,
        Memory,
        Cognitive
    };
    
    // Initialize inspector
    bool initialize(InspectionMode mode = InspectionMode::Full);
    
    // Core inspection methods
    void inspectBrain(const Brain& brain);
    void inspectNeuron(const Neuron& neuron);
    void inspectSynapse(const Synapse& synapse);
    void inspectRegion(const NeuralRegion& region);
    
    // Advanced inspection features
    void analyzeNeuralDynamics(const Brain& brain, double timeWindow = 1.0);
    void analyzePlasticity(const Brain& brain);
    void analyzeMemorySystems(const Brain& brain);
    void analyzeCognitiveProcesses(const Brain& brain);
    
    // Debug utilities
    void highlightProblematicNeurons(const Brain& brain, double threshold = 0.8);
    void analyzeConnectivityGaps(const Brain& brain);
    void detectBottlenecks(const Brain& brain);
    
    // Get inspection results
    const std::string& getInspectionReport() const;
    double getInspectionProgress() const;
    bool isInspectionComplete() const;
    
    // Configuration
    void setInspectionMode(InspectionMode mode);
    void setDetailLevel(double level); // 0.0 to 1.0
    void setFocusRegion(const std::vector<RegionId>& regions);
    void enableRealTimeUpdates(bool enable);
    void setUpdateInterval(double interval); // seconds
    
    // Callbacks
    using InspectionCallback = std::function<void(const std::string& report, double progress)>;
    void setInspectionCompleteCallback(InspectionCallback callback);
    void setAnomalyDetectedCallback(std::function<void(const std::string& anomaly)> callback);
    
    // Export and analysis
    bool exportInspectionReport(const std::string& filename) const;
    bool generateAnalysisReport(const std::string& filename) const;
    std::vector<std::string> getDetectedAnomalies() const;
    
    // Memory analysis
    void analyzeMemoryEfficiency(const Brain& brain);
    void analyzeMemoryCorruption(const Brain& brain);
    void optimizeMemoryLayout(const Brain& brain);
    
    // Performance debugging
    void profilePerformance(const Brain& brain);
    void analyzeMemoryUsage(const Brain& brain);
    void identifyPerformanceBottlenecks(const Brain& brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
