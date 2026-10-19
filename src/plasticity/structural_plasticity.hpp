#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <string>
#include <algorithm>

namespace nlm
{

// Structural plasticity implementation
// Implements synapse formation and elimination, axonal/dendritic growth rules,
// homeostatic structural plasticity, activity-dependent structural changes,
// and topological constraints

class StructuralPlasticity : public PlasticityRule {
public:
    StructuralPlasticity();
    ~StructuralPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Structural plasticity parameters
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    void setMinWeightThreshold(float threshold);
    void setActivityThreshold(float threshold);
    void setMaxSynapsesPerNeuron(size_t max);
    void setMaxTotalSynapses(size_t max);
    void enableActivityDependentFormation(bool enable);
    void enableActivityDependentPruning(bool enable);
    void enableHomeostaticStructuralPlasticity(bool enable);
    void enableAxonalGrowth(bool enable);
    void enableDendriticGrowth(bool enable);
    
    // Getters
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    float getMinWeightThreshold() const;
    float getActivityThreshold() const;
    size_t getMaxSynapsesPerNeuron() const;
    size_t getMaxTotalSynapses() const;
    bool isActivityDependentFormationEnabled() const;
    bool isActivityDependentPruningEnabled() const;
    bool isHomeostaticStructuralPlasticityEnabled() const;
    bool isAxonalGrowthEnabled() const;
    bool isDendriticGrowthEnabled() const;
    
    // Statistics
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    float getNetStructuralChange() const;
    
    // Structural operations
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    bool removeSynapse(Brain* brain, SynapseId synapse);
    NeuronId createNeuron(Brain* brain, NeuronType type);
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Structural plasticity mechanisms
    float calculateFormationProbability(float preActivity, float postActivity) const;
    float calculatePruningProbability(float weight, float activity) const;
    void updateStructuralNetwork(Brain* brain, RandomGenerator& rng);
    void applyHomeostaticStructuralConstraints(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Axonal growth plasticity
class AxonalGrowthPlasticity : public PlasticityRule {
public:
    AxonalGrowthPlasticity();
    ~AxonalGrowthPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Axonal growth parameters
    void setAxonalGrowthRate(float rate);
    void setAxonalRetractionRate(float rate);
    void setGrowthActivityThreshold(float threshold);
    void setRetractionActivityThreshold(float threshold);
    void enableBranching(bool enable);
    void enableSprouting(bool enable);
    
    float getAxonalGrowthRate() const;
    float getAxonalRetractionRate() const;
    float getGrowthActivityThreshold() const;
    float getRetractionActivityThreshold() const;
    bool isBranchingEnabled() const;
    bool isSproutingEnabled() const;
    
    // Axonal growth dynamics
    float calculateAxonalGrowth(float preActivity, float postActivity) const;
    float calculateAxonalRetraction(float preActivity, float postActivity) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dendritic growth plasticity
class DendriticGrowthPlasticity : public PlasticityRule {
public:
    DendriticGrowthPlasticity();
    ~DendriticGrowthPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Dendritic growth parameters
    void setDendriticGrowthRate(float rate);
    void setDendriticRetractionRate(float rate);
    void setDendriticSpineFormationRate(float rate);
    void setSpineEliminationRate(float rate);
    void enableSpineFormation(bool enable);
    void enableSpineElimination(bool enable);
    
    float getDendriticGrowthRate() const;
    float getDendriticRetractionRate() const;
    float getDendriticSpineFormationRate() const;
    float getSpineEliminationRate() const;
    bool isSpineFormationEnabled() const;
    bool isSpineEliminationEnabled() const;
    
    // Dendritic growth dynamics
    float calculateDendriticGrowth(float preActivity, float postActivity) const;
    float calculateDendriticRetraction(float preActivity, float postActivity) const;
    float calculateSpineFormation(float activity) const;
    float calculateSpineElimination(float activity) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Homeostatic structural plasticity
class HomeostaticStructuralPlasticity : public PlasticityRule {
public:
    HomeostaticStructuralPlasticity();
    ~HomeostaticStructuralPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homeostatic structural parameters
    void setHomeostaticTarget(float target);
    void setHomeostaticDeviation(float deviation);
    void setStructuralPlasticityRate(float rate);
    void setSynapticTargetSize(float size);
    void setCompensatoryGrowth(bool enable);
    void setCompensatoryPruning(bool enable);
    
    float getHomeostaticTarget() const;
    float getHomeostaticDeviation() const;
    float getStructuralPlasticityRate() const;
    float getSynapticTargetSize() const;
    bool isCompensatoryGrowthEnabled() const;
    bool isCompensatoryPruningEnabled() const;
    
    // Homeostatic structural mechanisms
    float calculateCompensatoryGrowth(float currentSize, float targetSize) const;
    float calculateCompensatoryPruning(float currentSize, float targetSize) const;
    void applyHomeostaticStructuralConstraints(Brain* brain);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Activity-dependent structural changes
class ActivityDependentStructuralPlasticity : public PlasticityRule {
public:
    ActivityDependentStructuralPlasticity();
    ~ActivityDependentStructuralPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Activity-dependent parameters
    void setActivityDependentGrowthRate(float rate);
    void setActivityDependentPruningRate(float rate);
    void setActivityThreshold(float threshold);
    void setCoactivityThreshold(float threshold);
    void enableHebbianStructuralLearning(bool enable);
    void enableAntiHebbianStructuralLearning(bool enable);
    
    float getActivityDependentGrowthRate() const;
    float getActivityDependentPruningRate() const;
    float getActivityThreshold() const;
    float getCoactivityThreshold() const;
    bool isHebbianStructuralLearningEnabled() const;
    bool isAntiHebbianStructuralLearningEnabled() const;
    
    // Activity-dependent calculations
    float calculateActivityDependentGrowth(float preActivity, float postActivity) const;
    float calculateActivityDependentPruning(float preActivity, float postActivity) const;
    float calculateHebbianStructuralChange(float preActivity, float postActivity) const;
    float calculateAntiHebbianStructuralChange(float preActivity, float postActivity) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structural plasticity system coordinator
class StructuralPlasticitySystem {
public:
    StructuralPlasticitySystem();
    ~StructuralPlasticitySystem();
    
    // Add structural plasticity rule
    void addStructuralPlasticityRule(std::unique_ptr<PlasticityRule> rule);
    
    // Get specific structural rule types
    StructuralPlasticity* getStructuralPlasticity() const;
    AxonalGrowthPlasticity* getAxonalGrowthPlasticity() const;
    DendriticGrowthPlasticity* getDendriticGrowthPlasticity() const;
    HomeostaticStructuralPlasticity* getHomeostaticStructuralPlasticity() const;
    ActivityDependentStructuralPlasticity* getActivityDependentStructuralPlasticity() const;
    
    // Get all structural rules
    const std::vector<std::unique_ptr<PlasticityRule>>& getAllStructuralRules() const;
    
    // System-wide structural plasticity update
    void updateAll(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Structural system configuration
    void configureFromDefaults();
    void configureFromConfig(const class Config& config);
    
    // Structural system statistics and diagnostics
    void logStructuralPlasticityState() const;
    float getSystemStructuralPlasticityLoad() const;
    float getNetworkStructuralStability() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm