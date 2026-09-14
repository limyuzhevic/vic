#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development metrics for tracking biological maturation
struct DevelopmentMetrics {
    double neuronMaturity;           // Average neuron maturity (0.0-1.0)
    double connectivityDensity;      // Synapse density relative to target
    double functionalSpecialization;  // Degree of specialized function development
    double activityLevel;            // Average neural activity level
    double learningRate;             // Current learning capacity
    size_t synapseCount;             // Current number of synapses
    size_t neuronCount;              // Current number of neurons
};

// Developmental stage goals and characteristics
struct StageCharacteristics {
    const char* name;
    double duration;                    // Target duration in simulation seconds
    double synaptogenesisTarget;        // Target synapse formation rate
    double pruningTarget;               // Target synapse removal rate  
    double criticalPeriodDuration;       // For critical period stage
    double maturationThreshold;          // Threshold for maturation stage
    bool requiresActivityTrigger;       // Whether stage requires activity-dependent transitions
    double activityThreshold;            // Minimum activity required for stage progression
};

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    
    // Update development (simple version)
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Get current developmental metrics
    DevelopmentMetrics getMetrics() const;
    
    // Check if stage progression conditions are met
    bool canAdvanceStage() const;
    
    // Get developmental window information
    bool isInDevelopmentWindow(DevelopmentalStage stage) const;
    double getWindowProgress(DevelopmentalStage stage) const;
    
    // Update biological metrics based on current brain state
    void updateMetrics(Brain* brain);
    
    // Calculate activity-based triggers
    double calculateActivityScore(Brain* brain) const;
    double calculateConnectivityDensity(Brain* brain) const;
    double calculateFunctionalSpecialization(Brain* brain) const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    
    // Stage characteristics for each developmental stage
    static const std::unordered_map<DevelopmentalStage, StageCharacteristics> stageMap;
    
    // Initialize stage characteristics
    void initializeStageMap();
};

} // namespace nlm

class DevelopmentSystem;
class Neuromodulator;
class Curiosity;
class PredictionError;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class SemanticMemory;
class ProceduralMemory;
class AssociativeMemory;
class NeuralPlanner;
class ConceptFormation;
class PredictionSystem;
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;