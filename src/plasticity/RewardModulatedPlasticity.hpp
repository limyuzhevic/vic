#pragma once

#include "../../plasticity/PlasticityRule.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

// Reward-modulated plasticity rule
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
    
    // Reward modulation parameters
    void setRewardStrength(float strength);      // Reward sensitivity (alpha)
    float getRewardStrength() const;
    void setPredictionErrorWeight(float weight);  // Prediction error weight (beta)
    float getPredictionErrorWeight() const;
    
    // Integration with neuromodulators
    void setNeuromodulator(Neuromodulator* neuromodulator);
    Neuromodulator* getNeuromodulator() const;
    
    // Dopamine-dependent plasticity
    void setDopamineGain(float gain);              // Dopamine gain modulation
    float getDopamineGain() const;
    void setDopamineThreshold(float threshold);    // Dopamine threshold for learning
    float getDopamineThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity-driven exploration plasticity
class CuriosityDrivenPlasticity : public PlasticityRule {
public:
    CuriosityDrivenPlasticity();
    ~CuriosityDrivenPlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Curiosity parameters
    void setCuriosityStrength(float strength);      // Curiosity drive (kappa)
    float getCuriosityStrength() const;
    void setNoveltyThreshold(float threshold);       // Novelty detection threshold
    float getNoveltyThreshold() const;
    
    // Exploration-exploitation balance
    void setExplorationRate(float rate);            // Exploration probability
    float getExplorationRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty-based plasticity
class NoveltyBasedPlasticity : public PlasticityRule {
public:
    NoveltyBasedPlasticity();
    ~NoveltyBasedPlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Novelty detection parameters
    void setNoveltySensitivity(float sensitivity);   // Novelty sensitivity (gamma)
    float getNoveltySensitivity() const;
    void setAdaptationRate(float rate);              // Novelty adaptation rate
    float getAdaptationRate() const;
    
    // Memory integration for novelty detection
    void setMemorySystem(class MemorySystem* memory);
    MemorySystem* getMemorySystem() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error-based plasticity
class PredictionErrorPlasticity : public PlasticityRule {
public:
    PredictionErrorPlasticity();
    ~PredictionErrorPlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Prediction error parameters
    void setPEWeight(float weight);                  // Prediction error weight (lambda)
    float getPEWeight() const;
    void setPrecisionFactor(float factor);           // Precision weighting
    float getPrecisionFactor() const;
    
    // Prediction system integration
    void setPredictionSystem(class PredictionSystem* prediction);
    PredictionSystem* getPredictionSystem() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Developmental stage-dependent plasticity
class DevelopmentalStagePlasticity : public PlasticityRule {
public:
    DevelopmentalStagePlasticity();
    ~DevelopmentalStagePlasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Development system integration
    void setDevelopmentSystem(class DevelopmentSystem* development);
    DevelopmentSystem* getDevelopmentSystem() const;
    
    // Stage-specific plasticity parameters
    void setStagePlasticityRate(DevelopmentalStage stage, float rate);
    float getStagePlasticityRate(DevelopmentalStage stage) const;
    
    // Critical period modulation
    void setCriticalPeriodProgress(float progress);
    float getCriticalPeriodProgress() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Unified plasticity management system
class UnifiedPlasticityManager {
public:
    UnifiedPlasticityManager();
    ~UnifiedPlasticityManager();
    
    // Plasticity rule management
    void addPlasticityRule(PlasticityRule* rule);
    void removePlasticityRule(const char* name);
    PlasticityRule* getPlasticityRule(const char* name) const;
    
    // Rule application
    void applyPlasticityRules(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt);
    
    // Integration with brain systems
    void setBrain(class Brain* brain);
    Brain* getBrain() const;
    
    // Configuration and parameters
    void setGlobalLearningRate(float rate);
    float getGlobalLearningRate() const;
    void setPlasticityEnabled(bool enabled);
    bool isPlasticityEnabled() const;
    
    // Memory integration
    void setMemorySystem(class MemorySystem* memory);
    MemorySystem* getMemorySystem() const;
    
    // Neuromodulation integration
    void setNeuromodulator(Neuromodulator* neuromodulator);
    Neuromodulator* getNeuromodulator() const;
    
    // Development system integration
    void setDevelopmentSystem(DevelopmentSystem* development);
    DevelopmentSystem* getDevelopmentSystem() const;
    
    // Context-dependent plasticity
    void setContext(class Context* context);
    Context* getContext() const;
    
    // Update unified manager
    void update(TimestepDuration dt);
    
    // Get statistics
    float getTotalPlasticityWeight() const;
    size_t getActivePlasticityRules() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
