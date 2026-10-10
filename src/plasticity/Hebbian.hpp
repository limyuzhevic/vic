#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Neuron.hpp"
#include "../../memory/NeuralWorkingMemory.hpp"
#include "../../cognition/ConceptFormation.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <unordered_map>
#include <queue>
#include <chrono>

namespace nlm {

// Synaptic tag for marking coactive synapses during Hebbian learning
struct SynapticTag {
    Timestamp lastActivation;
    float tagStrength;
    bool captured;
    std::unordered_map<SynapseId, float> partnerTags;
};

// Eligibility trace for reward-modulated Hebbian learning
class EligibilityTrace {
public:
    EligibilityTrace(float decayRate = 0.01f, float maxTrace = 1.0f);
    void update(float dt);
    void add(SynapticWeight change);
    float getValue() const;
    void reset();
private:
    float traceValue;
    float decayRate;
    float maxTrace;
};

// Multi-stage Hebbian learning with different time windows
class MultiStageHebbian {
public:
    MultiStageHebbian();
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);
    SynapticWeight getTotalDelta() const;
    void reset();
    
    // Configure stages for different time windows
    void addLearningStage(float weight, float timeConstant, float threshold, const std::string& stageName);
    void removeStage(const std::string& stageName);
    void clearStages();
    
private:
    struct Stage {
        std::string name;
        float weight;              // Learning rate for this stage
        float timeConstant;        // Time constant for exponential decay
        float threshold;           // Activity threshold for learning
        float currentActivity;     // Current filtered activity
        float accumulatedChange;   // Total weight change from this stage
        
        Stage(float w, float tc, float th, const std::string& n) 
            : weight(w), timeConstant(tc), threshold(th), name(n), 
              currentActivity(0.0f), accumulatedChange(0.0f) {}
    };
    
    std::vector<Stage> stages;
    SynapticWeight totalDelta;
};

// Population-level Hebbian learning across neural ensembles
class PopulationHebbian {
public:
    PopulationHebbian();
    ~PopulationHebbian();
    void addNeuron(NeuronId neuronId);
    void removeNeuron(NeuronId neuronId);
    void updateCrossCorrelation(const std::vector<Timestamp>& spikeTimes,
                                Timestamp currentTime);
    SynapticWeight getCrossCorrelation(NeuronId neuron1, NeuronId neuron2) const;
    void decayCorrelations(float decayRate);
    void reset();
    
    // Configuration
    void setTimeWindow(float window);
    void setLearningRate(float rate);
    void setBaselineActivity(float baseline);
    
private:
    struct CorrelationMatrix {
        CorrelationMatrix(int size);
        std::vector<std::vector<float>> correlations;
        std::vector<NeuronId> neuronIds;
        std::unordered_map<NeuronId, int> idToIndex;
    };
    
    CorrelationMatrix correlations;
    float timeWindow;
    float learningRate;
    float baselineActivity;
    float minCorrelation;
    std::queue<std::pair<Timestamp, std::vector<NeuronId>>> recentSpikeHistory;
};

// Spike-Timing-Dependent Hebbian (STDHebbian) learning
class STDHebbian {
public:
    STDHebbian();
    ~STDHebbian();
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);
    SynapticWeight getWeightChange() const;
    void reset();
    
    // Configuration
    void setLTPWeight(float weight);
    void setLTDWeight(float weight);
    void setTimeConstant(float tau);
    void setASAPTime(float asapTime);
    void setTraceDecay(float decayRate);
    
    // Multiplicative modulation
    void setModulation(float baseline, float gain);
    
private:
    struct STDPWindow {
        float ltpWeight;      // A+: potentiation amplitude
        float ltdWeight;      // A-: depression amplitude
        float timeConstant;   // τ for exponential window
        float asapTime;       // After-Stimulus Action Potential window
    };
    
    STDPWindow window;
    float baseline;
    float gain;
    float trace;
    SynapticWeight totalDelta;
    
    // Internal trace management
    void updateTraces(const std::vector<Timestamp>& preSpikes, 
                      const std::vector<Timestamp>& postSpikes,
                      Timestamp currentTime);
};

// Enhanced Hebbian learning implementation with sophisticated features
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Enhanced parameter settings
    void setLearningRate(float rate);
    void setCovarianceThreshold(float threshold);
    void setNMDAConductance(float conductance);
    void setTagDecayRate(float rate);
    void setEligibilityDecay(float rate);
    void setWorkingMemoryCapacity(float capacity);
    void setAttentionModulation(float weight);
    void setNeuromodulationGate(float gate);
    void setModulationParameters(float gain, float baseline);
    void setConsolidationThreshold(float threshold);
    void setDevelopmentalPruningAge(float age);
    void setPruningRate(float rate);
    
    // Multi-stage learning
    void addLearningStage(float weight, float timeConstant, float threshold, const std::string& stageName = "");
    void removeLearningStage(const std::string& stageName);
    void clearLearningStages();
    
    // Population learning
    void addToPopulation(NeuronId neuronId);
    void enablePopulationLearning(bool enable);
    void setPopulationTimeWindow(float window);
    void setPopulationLearningRate(float rate);
    
    // Neuromodulation integration
    void setNeuromodulator(Neuromodulator* modulator);
    
    // State management
    void resetSynapticTags();
    void decayEligibilityTraces(float dt);
    void consolidateMemory(float consolidationThreshold);
    void systemsConsolidation(float consolidationRate);
    void applyDevelopmentalPruning(Synapse* synapse, float age);
    
    // Advanced features
    void enableSTDHebbian(bool enable);
    void setSTDHebbianParameters(float ltp, float ltd, float tau);
    void setWorkingMemoryIntegration(NeuralWorkingMemory* workingMemory);
    void setCognitiveStrategyIntegration(ConceptFormation* strategy);
    
private:
    // Full covariance rule implementation
    SynapticWeight computeCovarianceDelta(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt) const;
    
    // Synaptic tagging and capture
    void createSynapticTag(SynapseId synapseId, float strength);
    void captureTags(SynapseId source, SynapseId target);
    void propagateTagging(const std::vector<SynapseId>& activeSynapses);
    void decaySynapticTags(float dt);
    
    // Temporal filtering with exponential decay
    float applyTemporalFilter(const std::vector<Timestamp>& spikes,
                              float timeConstant,
                              float decayRate) const;
    
    // NMDA receptor modulation
    void updateNMDAConductance(Synapse* synapse, float preActivity, float postActivity);
    void modifyNMDAReceptors(Synapse* synapse, float delta);
    
    // Working memory and attention effects
    void updateWorkingMemoryCapacity(Synapse* synapse, float delta);
    void modulateAttention(Synapse* synapse, float delta);
    void integrateWithWorkingMemory(Synapse* synapse, float delta);
    
    // Memory consolidation
    void consolidateWithEpisodicMemory(Synapse* synapse, SynapticWeight delta);
    void systemsConsolidation(float consolidationRate);
    void stabilizeMemoryTrace(Synapse* synapse, float delta);
    
    // Neuromodulation gating
    float computeModulationGate(const Timestamp& spikeTime) const;
    void applyNeuromodulationGating(Synapse* synapse, float delta, float gateValue);
    
    // Cognitive strategy modulation
    void modulateCognitiveStrategy(Synapse* synapse, float delta);
    void updateBehavioralPattern(Synapse* synapse, float delta);
    
    // Cross-correlation across populations
    void updatePopulationCrossCorrelation(Synapse* synapse,
                                          const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt);
    
    // STDHebbian integration
    void applySTDHebbianLearning(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt);
    
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
