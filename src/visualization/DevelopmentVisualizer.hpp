#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <deque>
#include <algorithm>
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../development/Synaptogenesis.hpp"
#include "../development/Maturation.hpp"
#include "../development/Pruning.hpp"

namespace nlm {

// Developmental stage
struct DevelopmentalStage {
    Timestamp timestamp;
    DevelopmentStage stage;
    float progression;
    std::string description;
    std::vector<std::string> milestones; // Achievements at this stage
    float globalLearningRate;
    float synapticStability;
    float memoryCapacity;
    float explorationDrive;
    std::map<RegionId, float> regionalDevelopment; // Regional development metrics
    std::vector<std::string> activeProcesses;
};

// Structural plasticity (changes in network structure)
struct StructuralPlasticityEvent {
    Timestamp timestamp;
    PlasticityEventType type; // Creation, elimination, strengthening, weakening
    std::string description;
    RegionId sourceRegion;
    RegionId targetRegion;
    NeuronId neuronId;
    SynapseId synapseId;
    float magnitude;
    float efficiencyGain;
    std::vector<NeuronId> involvedNeurons;
    bool isCompleted;
    std::string influencingFactors; // What caused this change
};

// Developmental metrics
struct DevelopmentalMetrics {
    Timestamp timestamp;
    float averageMaturationLevel;
    float averageSynaptogenesisActivity;
    float averagePruningActivity;
    uint32_t newConnectionsCreated;
    uint32_t connectionsEliminated;
    float totalStructuralChange;
    float networkEfficiency;
    float learningSpeed;
    float memoryRetention;
    float adaptability; // Ability to adapt to new inputs
    std::map<RegionId, float> regionalEfficiency;
    std::map<RegionId, float> regionalLearningRate;
    std::vector<std::string> activeDevelopmentalProcesses;
};

class DevelopmentVisualizer {
public:
    DevelopmentVisualizer();
    ~DevelopmentVisualizer();
    
    // Initialize visualizer
    bool initialize();
    
    // Update development data
    void update(double deltaTime);
    
    // Record developmental stage
    void recordDevelopmentalStage(const DevelopmentalStage& stage);
    void recordDevelopmentalStages(const std::vector<DevelopmentalStage>& stages);
    
    // Record structural plasticity events
    void recordStructuralPlasticityEvent(const StructuralPlasticityEvent& event);
    void recordStructuralPlasticityEvents(const std::vector<StructuralPlasticityEvent>& events);
    
    // Update developmental metrics
    void updateDevelopmentalMetrics(const DevelopmentalMetrics& metrics);
    
    // Render developmental stages
    void renderDevelopmentalStages(const std::vector<DevelopmentalStage>& stages);
    void renderDevelopmentalStages() { renderDevelopmentalStages(currentDevelopmentalStages); }
    
    // Render structural plasticity
    void renderStructuralPlasticity(const std::vector<StructuralPlasticityEvent>& events);
    void renderStructuralPlasticity() { renderStructuralPlasticity(currentPlasticityEvents); }
    
    // Render developmental metrics
    void renderDevelopmentalMetrics(const DevelopmentalMetrics& metrics);
    void renderDevelopmentalMetrics() { renderDevelopmentalMetrics(currentDevelopmentalMetrics); }
    
    // Configuration
    void setStageHistorySize(size_t size);
    void setPlasticityEventHistorySize(size_t size);
    void setAutoUpdate(bool autoUpdate);
    
    // Data access
    const std::vector<DevelopmentalStage>& getCurrentDevelopmentalStages() const { return currentDevelopmentalStages; }
    const std::vector<StructuralPlasticityEvent>& getCurrentPlasticityEvents() const { return currentPlasticityEvents; }
    const DevelopmentalMetrics& getCurrentDevelopmentalMetrics() const { return currentDevelopmentalMetrics; }
    
    // Statistics
    float getAverageMaturationLevel() const;
    float getCurrentSynaptogenesisActivity() const;
    float getCurrentPruningActivity() const;
    uint32_t getTotalConnectionsCreated() const;
    uint32_t getTotalConnectionsEliminated() const;
    float getAverageEfficiency() const;
    float getLearningSpeed() const;
    float getAdaptability() const;
    
    // Export capabilities
    bool exportDevelopmentalStageData(const std::string& filename);
    bool exportStructuralPlasticityData(const std::string& filename);
    bool exportDevelopmentalMetrics(const std::string& filename);
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm