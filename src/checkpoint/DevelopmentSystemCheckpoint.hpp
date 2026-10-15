// DevelopmentSystem checkpoint data structures

#pragma once

#include <vector>
#include <unordered_map>
#include "../core/Types/Types.hpp"

namespace nlm {

// DevelopmentParameters for checkpointing
struct DevelopmentParametersCheckpoint {
    // Current developmental stage
    DevelopmentalStage stage;
    
    // Developmental age
    double age;
    
    // Stage-specific parameters
    float plasticityModifier;
    float criticalPeriodProgress;
    float maturationProgress;
    
    // Growth metrics
    float neuralGrowthRate;
    float synapticComplexity;
    float behavioralComplexity;
    
    // Experience integration
    size_t learningEvents;
    float experienceIntegration;
    
    // State tracking
    SimulationStep lastDevelopmentUpdate;
    float developmentalMomentum;
    
    DevelopmentParametersCheckpoint()
        : stage(DevelopmentalStage::Initial), age(0)
        , plasticityModifier(1.0f), criticalPeriodProgress(0)
        , maturationProgress(0), neuralGrowthRate(0.1f), synapticComplexity(1.0f)
        , behavioralComplexity(1.0f), learningEvents(0), experienceIntegration(0)
        , lastDevelopmentUpdate(0), developmentalMomentum(0) {}
};

// DevelopmentSystem checkpoint state
struct DevelopmentSystemCheckpoint {
    // Current development parameters
    DevelopmentParametersCheckpoint parameters;
    
    // Developmental milestones
    std::vector<size_t> achievedMilestones;
    std::vector<float> milestoneProgress;
    
    // Plasticity system state
    std::vector<float> plasticityHistory;
    float cumulativePlasticity;
    float effectivePlasticity;
    
    // Integration with other systems
    std::vector<float> integratedNeuralState;
    std::vector<float> integratedMemoryState;
    std::vector<float> integratedCognitiveState;
    
    // Neuromodulation influence
    std::vector<float> neuromodulationHistory;
    float developmentalNeuromodulation;
    
    // Structural development
    size_t synaptogenesisCount;
    size_t pruningCount;
    float connectivityDensity;
    
    // System parameters
    float developmentRate;
    float maturityThreshold;
    float criticalPeriodDuration;
    
    // State tracking
    SimulationStep developmentalTime;
    float developmentalStageProgress;
    
    // Environmental interaction
    float environmentalStability;
    float stressLevel;
    float resourceAvailability;
    
    DevelopmentSystemCheckpoint()
        : synaptogenesisCount(0), pruningCount(0), connectivityDensity(0.1f)
        , developmentRate(0.01f), maturityThreshold(1.0f), criticalPeriodDuration(100)
        , developmentalTime(0), developmentalStageProgress(0)
        , environmentalStability(1.0f), stressLevel(0), resourceAvailability(1.0f) {}
};

// DevelopmentSystem save/load interface
class DevelopmentSystemSaverLoader {
public:
    virtual ~DevelopmentSystemSaverLoader() = default;
    
    // Save development system state
    virtual bool save(const DevelopmentSystemCheckpoint& checkpoint) const = 0;
    
    // Load development system state
    virtual bool load(DevelopmentSystemCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual DevelopmentSystemCheckpoint createEmptyCheckpoint() const = 0;
    
    // Get developmental stage for validation
    virtual const char* getDevelopmentalStageName(DevelopmentalStage stage) const = 0;
};

} // namespace nlm
