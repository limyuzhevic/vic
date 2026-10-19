#pragma once

#include "PlasticityRule.hpp"
#include "../neuromodulation/neuromodulator_integration.hpp"
#include <memory>

namespace nlm
{

// Neuromodulator-integrated plasticity implementation
// Integrates dopaminergic, cholinergic, noradrenergic, and serotonergic signals
// with synaptic plasticity rules for comprehensive biological learning

class NeuromodulatorPlasticity : public PlasticityRule {
public:
    NeuromodulatorPlasticity();
    ~NeuromodulatorPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Neuromodulator system integration
    void setNeuromodulatorIntegration(NeuromodulatorIntegration* integration);
    NeuromodulatorIntegration* getNeuromodulatorIntegration() const;
    
    // Individual neuromodulator levels
    void setDopamineLevel(float level);
    void setAcetylcholineLevel(float level);
    void setNorepinephrineLevel(float level);
    void setSerotoninLevel(float level);
    
    // Neuromodulator-specific modulation parameters
    void setDopamineModulation(float modulation);
    void setAcetylcholineModulation(float modulation);
    void setNorepinephrineModulation(float modulation);
    void setSerotoninModulation(float modulation);
    
    // Get current neuromodulator levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    
    // Neuromodulator-specific modulation values
    float getDopamineModulation() const;
    float getAcetylcholineModulation() const;
    float getNorepinephrineModulation() const;
    float getSerotoninModulation() const;
    
    // Neuromodulator interaction effects on plasticity
    void integrateDopamineModulation(float baselineLearning, float rewardPrediction);
    void integrateAcetylcholineModulation(float attention, float novelty);
    void integrateNorepinephrineModulation(float arousal, float stress);
    void integrateSerotoninModulation(float mood, float social);
    
    // Neuromodulator-dependent plasticity rule selection
    void enableDopamineDependentPlasticity(bool enable);
    void enableAcetylcholineDependentPlasticity(bool enable);
    void enableNorepinephrineDependentPlasticity(bool enable);
    void enableSerotoninDependentPlasticity(bool enable);
    
    // Getters for neuromodulator-specific effects
    bool isDopamineDependent() const;
    bool isAcetylcholineDependent() const;
    bool isNorepinephrineDependent() const;
    bool isSerotoninDependent() const;
    
    // Neuromodulator state tracking
    void updateNeuromodulatorState(float dt);
    void resetNeuromodulatorState();
    
    // Neuromodulator-specific learning dynamics
    float calculateDopamineLearningModulation(float predictionError) const;
    float calculateAcetylcholineLearningModulation(float attention) const;
    float calculateNorepinephrineLearningModulation(float arousal) const;
    float calculateSerotoninLearningModulation(float mood) const;
    
    // Neuromodulator interaction matrix
    float getNeuromodulatorInteraction(const std::string& modulator1, const std::string& modulator2) const;
    void setNeuromodulatorInteraction(const std::string& modulator1, const std::string& modulator2, float interaction);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward-modulated plasticity with prediction error and dopamine
class RewardModulatedPlasticity : public PlasticityRule {
public:
    RewardModulatedPlasticity();
    ~RewardModulatedPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward prediction error system
    void setPredictionError(float error);
    void setExpectedReward(float expected);
    void setActualReward(float actual);
    void setDopamineResponse(float response);
    
    // Reward-modulated learning parameters
    void setLearningRate(float rate);
    void setAsymmetricRewardSensitivity(bool enable);
    void setDelayedRewardModulation(bool enable);
    void setTrialBasedModulation(bool enable);
    void setCreditAssignmentWindow(float window);
    
    // Reward prediction error calculation
    void calculatePredictionError();
    void updateEligibilityTrace(float delta, float reward, float dt);
    
    // Reward-modulated STDP variants
    void enableRPESTDP(bool enable);
    void enableDopamineGatedSTDP(bool enable);
    void enableRewardAugmentedHebbian(bool enable);
    
    // Getters
    float getPredictionError() const;
    float getExpectedReward() const;
    float getActualReward() const;
    float getDopamineResponse() const;
    float getLearningRate() const;
    bool getAsymmetricRewardSensitivity() const;
    bool getDelayedRewardModulation() const;
    bool getTrialBasedModulation() const;
    float getCreditAssignmentWindow() const;
    
    // Reward statistics
    void recordRewardHistory(float reward, float time);
    float getAverageReward() const;
    float getRewardVolatility() const;
    float getExpectedRewardPrediction() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metaplasticity - plasticity of plasticity mechanisms
class MetaPlasticity : public PlasticityRule {
public:
    MetaPlasticity();
    ~MetaPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplasticity parameters
    void setMetaplasticityRate(float rate);
    void setLearningHistoryCapacity(size_t capacity);
    void setStabilityFactor(float factor);
    void setExplorationBonus(float bonus);
    void setMetaplasticThreshold(float threshold);
    
    float getMetaplasticityRate() const;
    size_t getLearningHistoryCapacity() const;
    float getStabilityFactor() const;
    float getExplorationBonus() const;
    float getMetaplasticThreshold() const;
    
    // Learning history and adaptation
    void recordLearningEvent(float magnitude, float time);
    void updateMetaplasticState(float dt);
    
    float getAverageLearningMagnitude() const;
    float getLearningVolatility() const;
    float getStabilityIndex() const;
    float getAdaptiveLearningRate() const;
    
    // Metaplastic mechanisms
    void setHomeostaticMetaplasticity(bool enable);
    void setNeuromodulatorMetaplasticity(bool enable);
    void setActivityHistoryMetaplasticity(bool enable);
    
    bool isHomeostaticMetaplasticityEnabled() const;
    bool isNeuromodulatorMetaplasticityEnabled() const;
    bool isActivityHistoryMetaplasticityEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structural plasticity - synapse formation and elimination
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
    
    // Structural operations
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    bool removeSynapse(Brain* brain, SynapseId synapse);
    NeuronId createNeuron(Brain* brain, NeuronType type);
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
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
    
    // Statistics
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    float getNetStructuralChange() const;
    
    // Structural plasticity mechanisms
    float calculateFormationProbability(float preActivity, float postActivity) const;
    float calculatePruningProbability(float weight, float activity) const;
    void updateStructuralNetwork(Brain* brain, RandomGenerator& rng);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm