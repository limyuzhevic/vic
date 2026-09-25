#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "<string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace nlm {

// Advanced neuron manipulation for expert users
class NeuronInjector {
public:
    // Direct neuron manipulation
    static Neuron* createNeuron(NeuralRegion& region, NeuronType type = NeuronType::Internal);
    static bool deleteNeuron(NeuralRegion& region, NeuronId neuronId);
    static bool cloneNeuron(NeuralRegion& region, NeuronId sourceId, NeuronType targetType = NeuronType::Internal);
    static bool injectCurrent(NeuralRegion& region, NeuronId neuronId, float currentAmplitude);
    static bool setMembranePotential(NeuralRegion& region, NeuronId neuronId, float potential);
    static bool setThreshold(NeuralRegion& region, NeuronId neuronId, float threshold);
    static bool setRefractoryPeriod(NeuralRegion& region, NeuronId neuronId, float period);
};

// Advanced synapse manipulation
class SynapseInjector {
public:
    // Direct synapse manipulation
    static Synapse* createSynapse(NeuralRegion& region, NeuronId sourceId, NeuronId targetId, 
                                SynapticWeight weight = 0.0f, Delay delay = 1,
                                SynapseType type = SynapseType::Excitatory);
    static bool deleteSynapse(NeuralRegion& region, SynapseId synapseId);
    static bool setWeight(NeuralRegion& region, SynapseId synapseId, SynapticWeight weight);
    static bool setDelay(NeuralRegion& region, SynapseId synapseId, Delay delay);
    static bool setType(NeuralRegion& region, SynapseId synapseId, SynapseType type);
    static bool createReciprocalConnection(NeuralRegion& region, NeuronId neuronA, NeuronId neuronB,
                                          SynapticWeight weightA = 0.0f, SynapticWeight weightB = 0.0f);
};

// Custom plasticity rule designer
class PlasticityDesigner {
public:
    // Create custom plasticity rules
    static std::unique_ptr<PlasticityRule> createHebbianRule(float learningRate = 0.01f, float maxWeight = 1.0f);
    static std::unique_ptr<PlasticityRule> createSTDPRule(float ltpWeight = 0.02f, float ltdWeight = 0.015f, float timeConstant = 20.0f);
    static std::unique_ptr<PlasticityRule> createCustomSTDP(float (*weightUpdate)(Timestamp, Timestamp), float (*locus)(float, float));
    
    // Plasticity rule testing
    static bool testPlasticityRule(NeuralRegion& region, std::unique_ptr<PlasticityRule>& rule,
                                  const std::vector<NeuronId>& preSynchronous, 
                                  const std::vector<NeuronId>& postSynchronous,
                                  float expectedWeightChange);
    
    // Plasticity optimization
    static float optimizePlasticityParameters(NeuralRegion& region, float learningRate, float timeConstant,
                                              const std::vector<float>& targetPatterns);
};

// Advanced neuromodulation control
class NeuromodulationController {
public:
    // Neuromodulator injection
    static bool injectDopamine(NeuralRegion& region, float concentration, float duration);
    static bool injectSerotonin(NeuralRegion& region, float concentration, float duration);
    static bool injectAcetylcholine(NeuralRegion& region, float concentration, float duration);
    static bool injectNoradrenaline(NeuralRegion& region, float concentration, float duration);
    
    // Neuromodulator dynamics
    static float getDopamineLevel(NeuralRegion& region);
    static float getSerotoninLevel(NeuralRegion& region);
    static float getAcetylcholineLevel(NeuralRegion& region);
    static float getNoradrenalineLevel(NeuralRegion& region);
    
    // Neuromodulator effects
    static void applyNeuromodulatoryEffects(NeuralRegion& region, float dt);
};

// Custom development stage management
class DevelopmentStageManager {
public:
    // Stage-specific operations
    static void advanceStage(NeuralRegion& region, DevelopmentalStage newStage);
    static void setStageSpecificPlasticity(NeuralRegion& region, DevelopmentalStage stage);
    static void setStageSpecificExcitability(NeuralRegion& region, DevelopmentalStage stage);
    static void setStageSpecificConnectivity(NeuralRegion& region, DevelopmentalStage stage);
    
    // Stage transitions
    static bool canTransitionToStage(NeuralRegion& region, DevelopmentalStage targetStage);
    static std::vector<DevelopmentalStage> getReachableStages(NeuralRegion& region);
};

// Advanced region management and connectivity
class RegionConnectivityManager {
public:
    // Advanced region operations
    static bool mergeRegions(std::vector<NeuralRegion*>& regions, NeuralRegion& targetRegion);
    static bool splitRegion(NeuralRegion& sourceRegion, RegionId newRegionId, const std::vector<NeuronId>& neuronsToMove);
    static bool establishCrossRegionConnections(NeuralRegion& sourceRegion, NeuralRegion& targetRegion,
                                              float connectionProbability, float weightScale = 1.0f);
    static bool optimizeRegionConnectivity(NeuralRegion& region);
    static float calculateRegionCohesion(NeuralRegion& region);
    static std::vector<NeuralRegion*> findRegionBuddies(NeuralRegion& region, float similarityThreshold = 0.7f);
};

// Expert-level brain manipulation utilities
class BrainManipulation {
public:
    // Bulk operations
    static void bulkInjectCurrent(std::vector<NeuralRegion*>& regions, const std::vector<NeuronId>& neuronIds, float current);
    static void bulkSetWeights(std::vector<NeuralRegion*>& regions, const std::vector<SynapseId>& synapseIds, SynapticWeight weight);
    static void bulkCreatePattern(std::vector<NeuralRegion*>& regions, const std::vector<NeuronId>& neuronIds, NeuronType patternType);
    
    // Advanced pattern operations
    static bool createAttractorPattern(NeuralRegion& region, size_t patternSize, float patternStability);
    static bool createOscillatoryPattern(NeuralRegion& region, float frequency, float phaseOffset);
    static bool createHomeostaticPattern(NeuralRegion& region, float targetFiringRate);
    
    // Memory-preserving operations
    static std::vector<float> captureBrainState(NeuralRegion& region);
    static bool restoreBrainState(NeuralRegion& region, const std::vector<float>& stateData);
};

} // namespace nlm
