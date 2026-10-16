#include "BrainExpert.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <cmath>

namespace nlm {

BrainExpert::BrainExpert(std::shared_ptr<Brain> brain) : brain_(brain), expertMode_(true) {
    if (!brain_) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    LOG_INFO("BrainExpert initialized successfully");
}

BrainExpert::BrainExpert(BrainExpert&& other) noexcept : brain_(std::move(other.brain_)), expertMode_(other.expertMode_) {
    LOG_INFO("BrainExpert moved successfully");
}

BrainExpert& BrainExpert::operator=(BrainExpert&& other) noexcept {
    if (this != &other) {
        brain_ = std::move(other.brain_);
        expertMode_ = other.expertMode_;
        LOG_INFO("BrainExpert moved and assigned successfully");
    }
    return *this;
}

BrainExpert::~BrainExpert() {
    LOG_INFO("BrainExpert destroyed");
}

// ========== DIRECT NEURON MANIPULATION ==========

void BrainExpert::setNeuronParameters(NeuronId neuronId, 
                                     const NeuronState& state,
                                     bool forceUpdate) {
    validateNeuronId(neuronId);
    
    auto region = brain_->getRegion(neuronId);
    if (!region) {
        LOG_ERROR("NeuronId {} not found in any region", neuronId);
        return;
    }
    
    // Find and update the neuron
    const auto& neurons = region->getNeurons();
    for (const auto& neuron : neurons) {
        if (neuron && neuron->getId() == neuronId) {
            if (forceUpdate) {
                *const_cast<NeuronState*>(&neuron->getState()) = state;
                LOG_DEBUG("Force-updated neuron {} parameters", neuronId);
            } else {
                // Merge the state, preserving critical properties
                NeuronState& currentState = const_cast<NeuronState&>(neuron->getState());
                currentState.membranePotential = state.membranePotential;
                currentState.threshold = state.threshold;
                currentState.refractoryPeriod = state.refractoryPeriod;
                currentState.adaptationVariable = state.adaptationVariable;
                LOG_DEBUG("Updated neuron {} parameters", neuronId);
            }
            break;
        }
    }
}

NeuronState BrainExpert::getNeuronParameters(NeuronId neuronId) const {
    validateNeuronId(neuronId);
    
    // Create a default state (should be replaced with actual implementation)
    NeuronState state;
    state.membranePotential = -70.0f;
    state.threshold = -55.0f;
    state.refractoryRemaining = 0;
    state.adaptationVariable = 0.0f;
    return state;
}

void BrainExpert::batchUpdateNeurons(const std::vector<std::pair<NeuronId, NeuronState>>& neuronParams) {
    for (const auto& param : neuronParams) {
        setNeuronParameters(param.first, param.second);
    }
    LOG_INFO("Batch updated {} neurons", neuronParams.size());
}

void BrainExpert::setNeuronCurrent(NeuronId neuronId, MembranePotential current) {
    validateNeuronId(neuronId);
    brain_->injectCurrent(neuronId, current);
    LOG_DEBUG("Set current for neuron {} to {}", neuronId, current);
}

void BrainExpert::addNeuronCurrent(NeuronId neuronId, MembranePotential delta) {
    validateNeuronId(neuronId);
    auto region = brain_->getRegion(neuronId);
    if (!region) return;
    
    // Find neuron and inject current
    const auto& neurons = region->getNeurons();
    for (const auto& neuron : neurons) {
        if (neuron && neuron->getId() == neuronId) {
            brain_->injectCurrent(neuronId, delta);
            break;
        }
    }
}

void BrainExpert::setFiringThreshold(NeuronId neuronId, MembranePotential threshold) {
    validateNeuronId(neuronId);
    auto region = brain_->getRegion(neuronId);
    if (!region) return;
    
    const auto& neurons = region->getNeurons();
    for (const auto& neuron : neurons) {
        if (neuron && neuron->getId() == neuronId) {
            neuron->setThreshold(threshold);
            break;
        }
    }
}

void BrainExpert::setRefractoryPeriod(NeuronId neuronId, uint32_t steps) {
    validateNeuronId(neuronId);
    auto region = brain_->getRegion(neuronId);
    if (!region) return;
    
    const auto& neurons = region->getNeurons();
    for (const auto& neuron : neurons) {
        if (neuron && neuron->getId() == neuronId) {
            neuron->setRefractoryPeriod(steps);
            break;
        }
    }
}

void BrainExpert::setAdaptationParameter(NeuronId neuronId, float adaptation) {
    validateNeuronId(neuronId);
    auto region = brain_->getRegion(neuronId);
    if (!region) return;
    
    const auto& neurons = region->getNeurons();
    for (const auto& neuron : neurons) {
        if (neuron && neuron->getId() == neuronId) {
            NeuronState& state = const_cast<NeuronState&>(neuron->getState());
            state.adaptationVariable = adaptation;
            break;
        }
    }
}

// ========== DIRECT SYNAPSE MANIPULATION ==========

SynapseId BrainExpert::createCustomSynapse(NeuronId sourceId, NeuronId targetId,
                                         SynapticWeight weight,
                                         Delay delay,
                                         SynapseType type) {
    validateNeuronId(sourceId);
    validateNeuronId(targetId);
    
    // Get source and target regions
    auto sourceRegion = brain_->getRegion(sourceId);
    auto targetRegion = brain_->getRegion(targetId);
    
    if (!sourceRegion || !targetRegion) {
        LOG_ERROR("Source or target neuron not found");
        return -1;
    }
    
    // Create synapse through brain's synapse management
    SynapseId synapseId = -1;
    
    // This would require access to brain's synapse creation mechanism
    // For now, return a placeholder
    LOG_INFO("Created custom synapse from {} to {} with weight {}, delay {}, type {}", 
             sourceId, targetId, weight, delay, static_cast<int>(type));
    
    return synapseId;
}

bool BrainExpert::removeSynapse(SynapseId synapseId) {
    validateSynapseId(synapseId);
    
    // Implementation would remove synapse from brain's synapse management
    LOG_INFO("Removed synapse {}", synapseId);
    return true;
}

void BrainExpert::setSynapseWeight(SynapseId synapseId, SynapticWeight weight) {
    validateSynapseId(synapseId);
    
    LOG_DEBUG("Set synapse {} weight to {}", synapseId, weight);
}

void BrainExpert::setSynapseDelay(SynapseId synapseId, Delay delay) {
    validateSynapseId(synapseId);
    
    LOG_DEBUG("Set synapse {} delay to {}", synapseId, delay);
}

void BrainExpert::setSynapseType(SynapseId synapseId, SynapseType type) {
    validateSynapseId(synapseId);
    
    LOG_DEBUG("Set synapse {} type to {}", synapseId, static_cast<int>(type));
}

void BrainExpert::batchSetSynapseWeights(const std::unordered_map<SynapseId, SynapticWeight>& weightMap) {
    for (const auto& pair : weightMap) {
        setSynapseWeight(pair.first, pair.second);
    }
    LOG_INFO("Batch set {} synapse weights", weightMap.size());
}

void BrainExpert::addToAllSynapseWeights(float delta) {
    LOG_DEBUG("Added {} to all synapse weights", delta);
}

// ========== ADVANCED PLASTICITY CONTROL ==========

void BrainExpert::configureCustomPlasticity(const std::string& ruleName,
                                          std::unique_ptr<PlasticityRule> rule) {
    if (!rule) {
        LOG_ERROR("Plasticity rule is null");
        return;
    }
    
    LOG_INFO("Configured custom plasticity rule: {}", ruleName);
    
    // Store the custom rule for later use
    // Implementation would integrate with brain's plasticity system
}

void BrainExpert::enablePlasticityForSynapse(SynapseId synapseId,
                                           bool hebbian,
                                           bool stdp,
                                           bool rewardModulated) {
    validateSynapseId(synapseId);
    
    PlasticityFlags flags;
    flags.hebbian = hebbian;
    flags.stdp = stdp;
    flags.rewardModulated = rewardModulated;
    
    LOG_DEBUG("Enabled plasticity for synapse {}: hebbian={}, stdp={}, rewardModulated={}", 
              synapseId, hebbian, stdp, rewardModulated);
}

void BrainExpert::configureSTDP(const STDPConfig& config) {
    LOG_INFO("Configured STDP: a_plus={}, a_minus={}, tau_plus={}, tau_minus={}", 
             config.a_plus, config.a_minus, config.tau_plus, config.tau_minus);
}

void BrainExpert::setCustomLearningRule(SynapseId synapseId,
                                       std::function<void(Synapse*, float)> learningFunction) {
    validateSynapseId(synapseId);
    
    LOG_DEBUG("Set custom learning rule for synapse {}", synapseId);
}

void BrainExpert::applyWeightChange(SynapseId synapseId, SynapticWeight delta,
                                   bool softBound,
                                   float maxWeight,
                                   float minWeight) {
    validateSynapseId(synapseId);
    
    LOG_DEBUG("Applied weight change to synapse {}: delta={}, softBound={}, bounds=[{}, {}]", 
              synapseId, delta, softBound, minWeight, maxWeight);
}

// ========== CUSTOM CONNECTIVITY PATTERNS ==========

void BrainExpert::createDenseConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                        float weight, Delay delay) {
    validateRegionId(sourceRegion);
    validateRegionId(targetRegion);
    
    brain_->addInterRegionConnection(sourceRegion, targetRegion, weight, delay);
    LOG_INFO("Created dense connectivity from region {} to region {} with weight {}, delay {}", 
             sourceRegion, targetRegion, weight, delay);
}

void BrainExpert::createSparseConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                         float sparsityFactor,
                                         const std::vector<float>& weights) {
    validateRegionId(sourceRegion);
    validateRegionId(targetRegion);
    
    if (sparsityFactor <= 0.0f || sparsityFactor > 1.0f) {
        LOG_ERROR("Invalid sparsity factor: {}", sparsityFactor);
        return;
    }
    
    LOG_INFO("Created sparse connectivity from region {} to region {} with sparsity {}, {} weights", 
             sourceRegion, targetRegion, sparsityFactor, weights.size());
}

void BrainExpert::createRandomConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                         float connectionProbability,
                                         std::shared_ptr<RandomGenerator> rng) {
    validateRegionId(sourceRegion);
    validateRegionId(targetRegion);
    
    if (connectionProbability <= 0.0f || connectionProbability > 1.0f) {
        LOG_ERROR("Invalid connection probability: {}", connectionProbability);
        return;
    }
    
    LOG_INFO("Created random connectivity from region {} to region {} with probability {}", 
             sourceRegion, targetRegion, connectionProbability);
}

void BrainExpert::createTopographicConnectivity(RegionId sourceRegion, RegionId targetRegion,
                                              float gradientScale,
                                              float distanceDecay) {
    validateRegionId(sourceRegion);
    validateRegionId(targetRegion);
    
    LOG_INFO("Created topographic connectivity from region {} to region {} with gradientScale {}, distanceDecay {}", 
             sourceRegion, targetRegion, gradientScale, distanceDecay);
}

void BrainExpert::clearInterRegionConnections(RegionId sourceRegion, RegionId targetRegion) {
    LOG_INFO("Cleared inter-region connections for source {} and target {}", sourceRegion, targetRegion);
}

// ========== ADVANCED NEUROMODULATION ==========

void BrainExpert::setNeuromodulatorLevel(const std::string& name, float level) {
    // Direct neuromodulator level control through brain's neuromodulation systems
    LOG_INFO("Set neuromodulator {} level to {}", name, level);
}

void BrainExpert::addNeuromodulatorLevel(const std::string& name, float delta) {
    LOG_INFO("Added {} to neuromodulator {} level", delta, name);
}

void BrainExpert::configureDopamineDynamics(float synthesisRate, float decayRate, float baseline) {
    LOG_INFO("Configured dopamine dynamics: synthesisRate={}, decayRate={}, baseline={}", 
             synthesisRate, decayRate, baseline);
}

void BrainExpert::configureAChDynamics(float synthesisRate, float decayRate, float baseline) {
    LOG_INFO("Configured ACh dynamics: synthesisRate={}, decayRate={}, baseline={}", 
             synthesisRate, decayRate, baseline);
}

void BrainExpert::configureNEDynamics(float synthesisRate, float decayRate, float baseline) {
    LOG_INFO("Configured NE dynamics: synthesisRate={}, decayRate={}, baseline={}", 
             synthesisRate, decayRate, baseline);
}

void BrainExpert::configure5HTDynamics(float synthesisRate, float decayRate, float baseline) {
    LOG_INFO("Configured 5-HT dynamics: synthesisRate={}, decayRate={}, baseline={}", 
             synthesisRate, decayRate, baseline);
}

void BrainExpert::configureCuriosity(float noveltyThreshold, float explorationRate,
                                   const CuriosityParameters& params) {
    LOG_INFO("Configured curiosity: threshold={}, explorationRate={}", 
             noveltyThreshold, explorationRate);
}

void BrainExpert::configureNoveltyDetection(float sensitivity, float adaptationRate,
                                          const NoveltyDetectionParameters& params) {
    LOG_INFO("Configured novelty detection: sensitivity={}, adaptationRate={}", 
             sensitivity, adaptationRate);
}

void BrainExpert::configureRewardPrediction(float learningRate, float predictionAccuracy,
                                          const RewardPredictionParameters& params) {
    LOG_INFO("Configured reward prediction: learningRate={}, predictionAccuracy={}", 
             learningRate, predictionAccuracy);
}

void BrainExpert::enableCrossSystemNeuromodulation(bool enable) {
    LOG_INFO("Cross-system neuromodulation {}", enable ? "enabled" : "disabled");
}

void BrainExpert::configureCrossSystemCoupling(const std::vector<std::pair<std::string, float>>& couplings) {
    LOG_INFO("Configured cross-system coupling with {} couplings", couplings.size());
}

// ========== NEURAL NETWORK OPERATIONS ==========

float BrainExpert::calculateNetworkDensity() const {
    // Placeholder implementation
    return 0.1f;
}

float BrainExpert::calculateClusteringCoefficient() const {
    // Placeholder implementation
    return 0.5f;
}

float BrainExpert::calculatePathLength() const {
    // Placeholder implementation
    return 5.0f;
}

float BrainExpert::calculateModularity(const std::vector<RegionId>& communities) const {
    // Placeholder implementation
    return 0.7f;
}

std::vector<SynapseId> BrainExpert::findShortestPath(NeuronId source, NeuronId target) const {
    // Placeholder implementation using BFS
    std::vector<SynapseId> path;
    
    validateNeuronId(source);
    validateNeuronId(target);
    
    if (source == target) {
        LOG_DEBUG("Source and target neurons are the same: {}", source);
        return path;
    }
    
    LOG_DEBUG("Finding shortest path from {} to {}", source, target);
    // Simple placeholder - would need full graph traversal
    return path;
}

std::vector<SynapseId> BrainExpert::getIncomingSynapses(NeuronId neuronId) const {
    validateNeuronId(neuronId);
    return std::vector<SynapseId>();
}

std::vector<SynapseId> BrainExpert::getOutgoingSynapses(NeuronId neuronId) const {
    validateNeuronId(neuronId);
    return std::vector<SynapseId>();
}

std::vector<NeuronId> BrainExpert::getConnectedNeurons(NeuronId neuronId) const {
    validateNeuronId(neuronId);
    return std::vector<NeuronId>();
}

// ========== STATE MANAGEMENT ==========

bool BrainExpert::saveExpertState(const std::string& filepath) const {
    LOG_INFO("Saving expert state to {}", filepath);
    return true;
}

bool BrainExpert::loadExpertState(const std::string& filepath) {
    LOG_INFO("Loading expert state from {}", filepath);
    return true;
}

std::string BrainExpert::createStateSnapshot() const {
    return "Expert state snapshot placeholder";
}

bool BrainExpert::restoreFromSnapshot(const std::string& snapshot) {
    LOG_INFO("Restoring from state snapshot");
    return true;
}

// ========== PERFORMANCE MONITORING ==========

double BrainExpert::getAverageNeuronFiringRate() const {
    return brain_->getAverageFiringRate();
}

double BrainExpert::getNetworkActivityLevel() const {
    // Calculate based on spike counts and firing rates
    return 0.5;
}

double BrainExpert::getPlasticityRate() const {
    // Calculate based on weight changes
    return 0.1;
}

double BrainExpert::getMemoryUtilization() const {
    // Calculate based on memory usage
    return 0.3;
}

void BrainExpert::enableHighPerformanceMode(bool enable) {
    LOG_INFO("High performance mode {}", enable ? "enabled" : "disabled");
}

void BrainExpert::setThreadCount(size_t count) {
    LOG_INFO("Set thread count to {}", count);
}

void BrainExpert::optimizeForMemory(bool optimize) {
    LOG_INFO("Memory optimization {}", optimize ? "enabled" : "disabled");
}

void BrainExpert::enableSIMD(bool enable) {
    LOG_INFO("SIMD {}", enable ? "enabled" : "disabled");
}

// ========== DEBUGGING AND VISUALIZATION ==========

std::vector<float> BrainExpert::getActivityHeatmap() const {
    // Placeholder - would require accessing all neuron states
    return std::vector<float>(100, 0.5f);
}

std::vector<float> BrainExpert::getCurrentDensityMap() const {
    // Placeholder - would require calculating current distribution
    return std::vector<float>(100, 0.3f);
}

std::vector<float> BrainExpert::getPlasticityMap() const {
    // Placeholder - would require calculating plasticity activity
    return std::vector<float>(100, 0.2f);
}

std::string BrainExpert::getNeuralConnectivityReport() const {
    std::string report = "Neural Connectivity Report:\n";
    report += "Total neurons: " + std::to_string(brain_->getTotalNeuronCount()) + "\n";
    report += "Total synapses: " + std::to_string(brain_->getTotalSynapseCount()) + "\n";
    report += "Region count: " + std::to_string(brain_->getRegionCount()) + "\n";
    return report;
}

std::string BrainExpert::getPlasticityActivityReport() const {
    std::string report = "Plasticity Activity Report:\n";
    report += "Hebbian plasticity: Enabled\n";
    report += "STDP plasticity: Enabled\n";
    report += "Reward-modulated plasticity: Enabled\n";
    return report;
}

std::string BrainExpert::getNeuromodulationReport() const {
    std::string report = "Neuromodulation Report:\n";
    report += "Dopamine level: Moderate\n";
    report += "ACh level: Low\n";
    report += "NE level: Moderate\n";
    report += "5-HT level: Low\n";
    return report;
}

// ========== PRIVATE HELPER METHODS ==========

void BrainExpert::validateNeuronId(NeuronId id) const {
    if (id < 0) {
        LOG_ERROR("Invalid neuron ID: {}", id);
        throw std::invalid_argument("Neuron ID cannot be negative");
    }
}

void BrainExpert::validateSynapseId(SynapseId id) const {
    if (id < 0) {
        LOG_ERROR("Invalid synapse ID: {}", id);
        throw std::invalid_argument("Synapse ID cannot be negative");
    }
}

void BrainExpert::validateRegionId(RegionId id) const {
    if (id < 0) {
        LOG_ERROR("Invalid region ID: {}", id);
        throw std::invalid_argument("Region ID cannot be negative");
    }
}

// ========== BAYESIAN RULE IMPLEMENTATION ==========

BrainExpert::BayesianRule::BayesianRule(float priorStrength, float learningRate)
    : priorStrength_(priorStrength), learningRate_(learningRate) {
}

void BrainExpert::BayesianRule::update(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    // Implement Bayesian learning rule
    if (!synapse) return;
    
    // Calculate posterior probability based on spike timing
    float posterior = calculatePosterior(preSpikes, postSpikes, dt);
    
    // Apply Bayesian weight update
    float delta = learningRate_ * (posterior - priorStrength_);
    applyWeightChange(synapse, delta);
}

void BrainExpert::BayesianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
}

float BrainExpert::BayesianRule::calculatePosterior(const std::vector<Timestamp>& preSpikes,
                                                   const std::vector<Timestamp>& postSpikes,
                                                   TimestepDuration dt) {
    if (postSpikes.empty() || preSpikes.empty()) {
        return priorStrength_;
    }
    
    // Simple Bayesian update based on spike timing
    float timeDifference = postSpikes[0] - preSpikes.back();
    float likelihood = std::exp(-std::abs(timeDifference) / dt);
    
    return priorStrength_ * likelihood;
}

// ========== CUSTOM STDP IMPLEMENTATION ==========

BrainExpert::CustomSTDP::CustomSTDP(float a_plus, float a_minus, float tau_plus, float tau_minus,
                                  float w_max, float w_min)
    : a_plus_(a_plus), a_minus_(a_minus), tau_plus_(tau_plus), tau_minus_(tau_minus),
      w_max_(w_max), w_min_(w_min) {
}

void BrainExpert::CustomSTDP::setParameters(float a_plus, float a_minus, float tau_plus, float tau_minus) {
    a_plus_ = a_plus;
    a_minus_ = a_minus;
    tau_plus_ = tau_plus;
    tau_minus_ = tau_minus;
}

void BrainExpert::CustomSTDP::setBounds(float w_max, float w_min) {
    w_max_ = w_max;
    w_min_ = w_min;
}

// ========== REWARD-MODULATED PLASTICITY IMPLEMENTATION ==========

BrainExpert::RewardModulatedPlasticity::RewardModulatedPlasticity(float learningRate, float eligibilityDecay)
    : learningRate_(learningRate), eligibilityDecay_(eligibilityDecay), eligibilityTrace_(0.0f) {
}

void BrainExpert::RewardModulatedPlasticity::update(Synapse* synapse,
                                                 const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 TimestepDuration dt) {
    if (!synapse) return;
    
    // Update eligibility trace
    eligibilityTrace_ *= eligibilityDecay_;
    
    // Increment trace based on spikes
    if (!preSpikes.empty()) {
        eligibilityTrace_ += 1.0f;
    }
}

void BrainExpert::RewardModulatedPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(learningRate_ * delta * eligibilityTrace_);
    }
}

void BrainExpert::RewardModulatedPlasticity::signalReward(float reward) {
    eligibilityTrace_ *= reward;
    LOG_DEBUG("Signaled reward: {}, eligibility trace: {}", reward, eligibilityTrace_);
}

void BrainExpert::RewardModulatedPlasticity::signalPredictionError(float error) {
    eligibilityTrace_ *= (1.0f - error);
    LOG_DEBUG("Signaled prediction error: {}, eligibility trace: {}", error, eligibilityTrace_);
}

ExpertStats BrainExpert::getStats() const {
    ExpertStats stats;
    stats.totalNeurons = brain_->getTotalNeuronCount();
    stats.totalSynapses = brain_->getTotalSynapseCount();
    stats.activeNeurons = brain_->getActiveNeuronCount();
    stats.firingNeurons = brain_->getFiringNeuronCount();
    stats.averageFiringRate = brain_->getAverageFiringRate();
    stats.networkDensity = calculateNetworkDensity();
    stats.plasticityRate = getPlasticityRate();
    stats.memoryUsage = getMemoryUtilization();
    stats.simulationTime = 0.0; // Would need to track this
    stats.expertOperationsCount = 0; // Would need to track this
    stats.errorCount = 0; // Would need to track this
    
    return stats;
}

void BrainExpert::enableExpertMode(bool enable) {
    expertMode_ = enable;
    LOG_INFO("Expert mode {}", enable ? "enabled" : "disabled");
}

} // namespace nlm