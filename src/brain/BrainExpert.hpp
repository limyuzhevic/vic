#pragma once

#include "Brain.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace nlm {

// Expert-level brain control API
// Provides direct access to neural components and advanced manipulation capabilities

class BrainExpert {
public:
    // Constructor - wraps a brain instance
    explicit BrainExpert(std::shared_ptr<Brain> brain);
    
    // Disable copying, enable moving
    BrainExpert(const BrainExpert&) = delete;
    BrainExpert& operator=(const BrainExpert&) = delete;
    BrainExpert(BrainExpert&&) noexcept;
    BrainExpert& operator=(BrainExpert&&) noexcept;
    
    ~BrainExpert();
    
    // ========== DIRECT NEURON MANIPULATION ==========
    
    // Set comprehensive neuron parameters
    void setNeuronParameters(NeuronId neuronId, 
                           const NeuronState& state,
                           bool forceUpdate = false);
    
    // Get neuron parameters
    NeuronState getNeuronParameters(NeuronId neuronId) const;
    
    // Batch update neuron parameters
    void batchUpdateNeurons(const std::vector<std::pair<NeuronId, NeuronState>>& neuronParams);
    
    // Manipulate synaptic currents
    void setNeuronCurrent(NeuronId neuronId, MembranePotential current);
    void addNeuronCurrent(NeuronId neuronId, MembranePotential delta);
    
    // Configure neuron firing behavior
    void setFiringThreshold(NeuronId neuronId, MembranePotential threshold);
    void setRefractoryPeriod(NeuronId neuronId, uint32_t steps);
    void setAdaptationParameter(NeuronId neuronId, float adaptation);
    
    // ========== DIRECT SYNAPSE MANIPULATION ==========
    
    // Create custom synapse with precise parameters
    SynapseId createCustomSynapse(NeuronId sourceId, NeuronId targetId,
                                 SynapticWeight weight = 1.0f,
                                 Delay delay = 1,
                                 SynapseType type = SynapseType::Excitatory);
    
    // Remove synapse
    bool removeSynapse(SynapseId synapseId);
    
    // Modify synapse properties
    void setSynapseWeight(SynapseId synapseId, SynapticWeight weight);
    void setSynapseDelay(SynapseId synapseId, Delay delay);
    void setSynapseType(SynapseId synapseId, SynapseType type);
    
    // Bulk synapse operations
    void batchSetSynapseWeights(const std::unordered_map<SynapseId, SynapticWeight>& weightMap);
    void addToAllSynapseWeights(float delta);
    
    // ========== ADVANCED PLASTICITY CONTROL ==========
    
    // Configure custom plasticity rules
    void configureCustomPlasticity(const std::string& ruleName,
                                  std::unique_ptr<PlasticityRule> rule);
    
    // Enable/disable plasticity rules per synapse
    void enablePlasticityForSynapse(SynapseId synapseId, 
                                   bool hebbian = true,
                                   bool stdp = true,
                                   bool rewardModulated = true);
    
    // Configure STDP parameters
    void configureSTDP(const STDPConfig& config);
    
    // Custom learning rule for specific synapse pair
    void setCustomLearningRule(SynapseId synapseId,
                              std::function<void(Synapse*, float)> learningFunction);
    
    // Apply immediate weight changes
    void applyWeightChange(SynapseId synapseId, SynapticWeight delta,
                          bool softBound = true,
                          float maxWeight = 1.0f,
                          float minWeight = -1.0f);
    
    // ========== CUSTOM CONNECTIVITY PATTERNS ==========
    
    // Create dense connectivity matrix
    void createDenseConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                float weight = 1.0f, Delay delay = 1);
    
    // Create sparse connectivity with specific pattern
    void createSparseConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                 float sparsityFactor = 0.1f,
                                 const std::vector<float>& weights = {});
    
    // Create random connectivity
    void createRandomConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                 float connectionProbability = 0.2f,
                                 std::shared_ptr<RandomGenerator> rng = nullptr);
    
    // Create topographic connectivity
    void createTopographicConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                      float gradientScale = 1.0f,
                                      float distanceDecay = 0.5f);
    
    // Remove all inter-region connections
    void clearInterRegionConnections(RegionId sourceRegion = -1, 
                                    RegionId targetRegion = -1);
    
    // ========== ADVANCED NEUROMODULATION ==========
    
    // Direct neuromodulator level control
    void setNeuromodulatorLevel(const std::string& name, float level);
    void addNeuromodulatorLevel(const std::string& name, float delta);
    
    // Configure neuromodulator dynamics
    void configureDopamineDynamics(float synthesisRate, float decayRate, float baseline);
    void configureAChDynamics(float synthesisRate, float decayRate, float baseline);
    void configureNEDynamics(float synthesisRate, float decayRate, float baseline);
    void configure5HTDynamics(float synthesisRate, float decayRate, float baseline);
    
    // Configure curiosity system
    void configureCuriosity(float noveltyThreshold, float explorationRate,
                           const CuriosityParameters& params = {});
    
    // Configure novelty detection
    void configureNoveltyDetection(float sensitivity, float adaptationRate,
                                  const NoveltyDetectionParameters& params = {});
    
    // Configure reward prediction
    void configureRewardPrediction(float learningRate, float predictionAccuracy,
                                  const RewardPredictionParameters& params = {});
    
    // Cross-system neuromodulation
    void enableCrossSystemNeuromodulation(bool enable);
    void configureCrossSystemCoupling(const std::vector<std::pair<std::string, float>>& couplings);
    
    // ========== CUSTOM LEARNING RULES ==========
    
    // Implement Bayesian learning rule
    class BayesianRule : public PlasticityRule {
    public:
        BayesianRule(float priorStrength = 1.0f, float learningRate = 0.1f);
        void update(Synapse* synapse, 
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "Bayesian"; }
        
        void setPriorStrength(float strength) { priorStrength_ = strength; }
        void setLearningRate(float rate) { learningRate_ = rate; }
    
    private:
        float priorStrength_;
        float learningRate_;
    };
    
    // Implement spike-timing dependent plasticity with custom parameters
    class CustomSTDP : public STDP {
    public:
        CustomSTDP(float a_plus = 0.01f, float a_minus = 0.01f,
                  float tau_plus = 20.0f, float tau_minus = 20.0f,
                  float w_max = 1.0f, float w_min = -1.0f);
        
        void setParameters(float a_plus, float a_minus, float tau_plus, float tau_minus);
        void setBounds(float w_max, float w_min);
        
    private:
        float a_plus_;
        float a_minus_;
        float tau_plus_;
        float tau_minus_;
        float w_max_;
        float w_min_;
    };
    
    // Implement reward-modulated plasticity
    class RewardModulatedPlasticity : public PlasticityRule {
    public:
        RewardModulatedPlasticity(float learningRate = 0.01f, 
                                 float eligibilityDecay = 0.95f);
        void update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) override;
        void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
        const char* getName() const override { return "RewardModulated"; }
        
        void signalReward(float reward);
        void signalPredictionError(float error);
    
    private:
        float learningRate_;
        float eligibilityDecay_;
        float eligibilityTrace_;
    };
    
    // ========== NEURAL NETWORK OPERATIONS ==========
    
    // Calculate network metrics
    float calculateNetworkDensity() const;
    float calculateClusteringCoefficient() const;
    float calculatePathLength() const;
    float calculateModularity(const std::vector<RegionId>& communities) const;
    
    // Find shortest paths between neurons
    std::vector<SynapseId> findShortestPath(NeuronId source, NeuronId target) const;
    
    // Get neighborhood information
    std::vector<SynapseId> getIncomingSynapses(NeuronId neuronId) const;
    std::vector<SynapseId> getOutgoingSynapses(NeuronId neuronId) const;
    std::vector<NeuronId> getConnectedNeurons(NeuronId neuronId) const;
    
    // ========== STATE MANAGEMENT ==========
    
    // Save expert state
    bool saveExpertState(const std::string& filepath) const;
    
    // Load expert state
    bool loadExpertState(const std::string& filepath);
    
    // Create snapshot of current brain state
    std::string createStateSnapshot() const;
    
    // Restore from snapshot
    bool restoreFromSnapshot(const std::string& snapshot);
    
    // ========== PERFORMANCE MONITORING ==========
    
    // Get current performance metrics
    double getAverageNeuronFiringRate() const;
    double getNetworkActivityLevel() const;
    double getPlasticityRate() const;
    double getMemoryUtilization() const;
    
    // Performance tuning
    void enableHighPerformanceMode(bool enable);
    void setThreadCount(size_t count);
    void optimizeForMemory(bool optimize);
    void enableSIMD(bool enable);
    
    // ========== DEBUGGING AND VISUALIZATION ==========
    
    // Get neural activity visualization data
    std::vector<float> getActivityHeatmap() const;
    std::vector<float> getCurrentDensityMap() const;
    std::vector<float> getPlasticityMap() const;
    
    // Get debugging information
    std::string getNeuralConnectivityReport() const;
    std::string getPlasticityActivityReport() const;
    std::string getNeuromodulationReport() const;
    
    // ========== EXPERT CONTROL ACCESS ==========
    
    // Get underlying brain instance
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Get expert statistics
    ExpertStats getStats() const;
    
    // Check if expert features are enabled
    bool isExpertModeEnabled() const { return expertMode_; }
    void enableExpertMode(bool enable);

private:
    std::shared_ptr<Brain> brain_;
    bool expertMode_;
    
    // Internal helper methods
    void validateNeuronId(NeuronId id) const;
    void validateSynapseId(SynapseId id) const;
    void validateRegionId(RegionId id) const;
};

// Expert system parameters
struct NeuronParameters {
    MembranePotential membranePotential = -70.0f;
    MembranePotential restingPotential = -70.0f;
    MembranePotential threshold = -55.0f;
    MembranePotential resetPotential = -70.0f;
    MembranePotential leakConductance = 10.0f;
    MembranePotential synapseConductance = 0.0f;
    FiringRate firingRate = 0.0f;
    FiringState firingState = FiringState::Resting;
    uint32_t refractoryRemaining = 0;
    uint32_t refractoryPeriod = 5;
    float adaptationVariable = 0.0f;
    float lastSpikeTime = -1.0f;
};

struct SynapseParameters {
    SynapticWeight weight = 1.0f;
    Delay delay = 1;
    SynapseType type = SynapseType::Excitatory;
    PlasticityFlags plasticityFlags;
};

struct STDPConfig {
    float a_plus = 0.01f;           // LTP learning rate
    float a_minus = 0.01f;          // LTD learning rate
    float tau_plus = 20.0f;         // LTP time constant (ms)
    float tau_minus = 20.0f;        // LTD time constant (ms)
    float w_max = 1.0f;             // Maximum weight
    float w_min = -1.0f;            // Minimum weight
    bool enableSTDP = true;
};

struct NeuromodulatorConfig {
    float synthesisRate = 0.1f;
    float decayRate = 0.95f;
    float baseline = 0.0f;
    float maxLevel = 1.0f;
};

struct CuriosityParameters {
    float noveltyThreshold = 0.5f;
    float explorationRate = 0.1f;
    float adaptationRate = 0.01f;
    float maxExploration = 1.0f;
};

struct NoveltyDetectionParameters {
    float sensitivity = 1.0f;
    float adaptationRate = 0.01f;
    float maxNovelty = 1.0f;
    float decayRate = 0.9f;
};

struct RewardPredictionParameters {
    float learningRate = 0.01f;
    float predictionAccuracy = 0.8f;
    float errorDecayRate = 0.9f;
    float maxPrediction = 1.0f;
};

struct ExpertStats {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float networkDensity;
    float plasticityRate;
    float memoryUsage;
    double simulationTime;
    size_t expertOperationsCount;
    size_t errorCount;
};

} // namespace nlm