#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Core Hebbian learning methods
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt);
    void computeCorrelatedActivity(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    float& correlation,
                                    float& covariance);
    void applyHebbianLearning(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt);
    void computeHebbianWeightChange(Synapse* synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     float& weightChange,
                                     float& eligibility);
    void computeSTDPComponent(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               float& stdpPotentiation,
                               float& stdpDepression);
    
    // Neuromodulation
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setCorrelationThreshold(float threshold);
    float getCorrelationThreshold() const;
    void setNeuromodulationFactor(float factor);
    float getNeuromodulationFactor() const;
    
    // Memory integration
    void storeHebbianMemory(const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            SynapseId synapseId);
    void retrieveHebbianMemory(SynapseId synapseId,
                               std::vector<Timestamp>& preSpikes,
                               std::vector<Timestamp>& postSpikes);
    void consolidateHebbianLearning();
    void updateHebbianFromEpisodicMemory(const class NeuralEpisodicMemory* episodicMemory);
    
    // Optimization and error handling
    void optimizeHebbianParameters();
    void normalizeHebbianWeights();
    void regularizeHebbianLearning(float lambda);
    bool validateHebbianLearning() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
