#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include "../../memory/NeuralWorkingMemory.hpp"
#include "../../memory/NeuralEpisodicMemory.hpp"
#include "../../neuromodulation/PredictionError.hpp"
#include "../../brain/Brain.hpp"

namespace nlm {

// Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // PlasticityRule interface
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
    void updateHebbianFromEpisodicMemory(const NeuralEpisodicMemory* episodicMemory);
    
    // Optimization and error handling
    void optimizeHebbianParameters();
    void normalizeHebbianWeights();
    void regularizeHebbianLearning(float lambda);
    bool validateHebbianLearning() const;
    
    // Parameter setters for other components
    void setSTDPParameters(float ltpWeight, float ltdWeight, float tau);
    void setMemoryParameters(float decayRate, float consolidationRate);
    void setNeuromodulationParameters(float dopamine, float curiosity, float novelty);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
