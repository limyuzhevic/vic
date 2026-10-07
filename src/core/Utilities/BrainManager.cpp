// BrainManager.cpp
// Implementation of Brain lifecycle management for NLM

#include "core/Utilities/BrainManager.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>

namespace nlm {

bool BrainManager::initializeBrain(const std::shared_ptr<Config>& config) {
    if (!config) {
        NLM_LOG_ERROR("Cannot initialize brain: config is null");
        return false;
    }
    
    this->brainConfig = config;
    brain = std::make_shared<Brain>(config);
    
    initializationStartTime = std::chrono::high_resolution_clock::now();
    initialized = brain->initialize();
    
    if (initialized) {
        NLM_LOG_INFO("Brain initialized successfully");
        logStatus();
    } else {
        NLM_LOG_ERROR("Failed to initialize brain");
        brain.reset();
    }
    
    return initialized;
}

bool BrainManager::resetBrain() {
    if (!checkInitialized("resetBrain")) {
        return false;
    }
    
    // Save current stats before reset
    auto oldStats = getStats();
    
    // Reset brain
    if (!brain->reset()) {
        NLM_LOG_ERROR("Failed to reset brain");
        return false;
    }
    
    NLM_LOG_INFO("Brain reset successfully");
    return true;
}

bool BrainManager::reinitializeBrain(const std::shared_ptr<Config>& config) {
    if (!resetBrain()) {
        NLM_LOG_ERROR("Failed to reinitialize brain: reset failed");
        return false;
    }
    
    if (!initializeBrain(config)) {
        NLM_LOG_ERROR("Failed to reinitialize brain: initialize failed");
        return false;
    }
    
    NLM_LOG_INFO("Brain reinitialized successfully");
    return true;
}

void BrainManager::logStatus() const {
    if (!checkInitialized("logStatus")) {
        NLM_LOG_ERROR("Cannot log status: brain not initialized");
        return;
    }
    
    NLM_LOG_INFO("=== Brain Status ===");
    NLM_LOG_INFO("Neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    
    if (hasDevelopmentSystem()) {
        NLM_LOG_INFO("Development system: ENABLED");
    }
    
    if (hasMemorySystems()) {
        NLM_LOG_INFO("Memory systems: ENABLED");
        NLM_LOG_INFO("  Working memory: " + std::to_string(getStats().workingMemoryTraces));
        NLM_LOG_INFO("  Episodic memory: " + std::to_string(getStats().episodicMemoryEpisodes));
    }
    
    if (hasNeuromodulationSystems()) {
        NLM_LOG_INFO("Neuromodulation systems: ENABLED");
    }
    
    if (hasPredictionSystem()) {
        NLM_LOG_INFO("Prediction system: ENABLED");
    }
    
    if (hasCognitionSystems()) {
        NLM_LOG_INFO("Cognition systems: ENABLED");
    }
    
    // Display plasticity status
    NLM_LOG_INFO("Plasticity: Hebbian=" + std::string(plasticityEnabled ? "YES" : "NO"));
    
    NLM_LOG_INFO("===================");
}

BrainManager::BrainStats BrainManager::getStats() const {
    BrainStats stats;
    
    if (!checkInitialized("getStats")) {
        NLM_LOG_WARNING("Cannot get stats: brain not initialized");
        return stats;
    }
    
    stats.totalNeurons = brain->getTotalNeuronCount();
    stats.totalSynapses = brain->getTotalSynapseCount();
    stats.totalSpikes = brain->getTotalSpikeCount();
    stats.averageFiringRate = brain->getAverageFiringRate();
    stats.firingNeuronCount = brain->getFiringNeuronCount();
    stats.regionCount = brain->getRegionCount();
    
    if (hasDevelopmentSystem()) {
        auto* devSystem = brain->getDevelopmentSystem();
        if (devSystem) {
            stats.maturationStage = devSystem->getCurrentStage();
            stats.developmentAge = devSystem->getAge();
        }
    }
    
    if (hasMemorySystems()) {
        auto* wm = brain->getWorkingMemory();
        auto* em = brain->getEpisodicMemory();
        
        if (wm) {
            stats.workingMemoryTraces = wm->getActiveTraces();
        }
        
        if (em) {
            stats.episodicMemoryEpisodes = em->getEpisodeCount();
        }
    }
    
    if (initialized) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - initializationStartTime).count();
        stats.initializationTime = duration;
    }
    
    return stats;
}

bool BrainManager::stepSimulation(uint64_t step, double timestep) {
    if (!checkInitialized("stepSimulation")) {
        return false;
    }
    
    currentSimulationTime = step * timestep;
    return brain->step(step, timestep);
}

bool BrainManager::injectCurrentToNeurons(NeuronType neuronType, float current, uint64_t count) {
    if (!checkInitialized("injectCurrentToNeurons")) {
        return false;
    }
    
    if (!validateParameters("injectCurrentToNeurons", count)) {
        return false;
    }
    
    return brain->injectCurrentToNeurons(neuronType, current, count);
}

std::vector<Neuron*> BrainManager::getNeuronsByType(NeuronType type) const {
    std::vector<Neuron*> neurons;
    
    if (!checkInitialized("getNeuronsByType")) {
        return neurons;
    }
    
    // Note: This assumes Brain has a method to get neurons by type
    // If not available, this would need to be implemented based on the actual Brain API
    
    return neurons;
}

std::vector<Synapse*> BrainManager::getSynapsesByType(SynapseType type) const {
    std::vector<Synapse*> synapses;
    
    if (!checkInitialized("getSynapsesByType")) {
        return synapses;
    }
    
    // Note: This assumes Brain has a method to get synapses by type
    // If not available, this would need to be implemented based on the actual Brain API
    
    return synapses;
}

void BrainManager::enablePlasticity(bool hebbian, bool stdp, bool structural) {
    if (!checkInitialized("enablePlasticity")) {
        return;
    }
    
    auto* region = brain->getRegion(RegionId(1);
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(hebbian, stdp, structural);
        }
    }
    
    plasticityEnabled = (hebbian || stdp || structural);
    
    if (plasticityEnabled) {
        NLM_LOG_INFO("Plasticity enabled: Hebbian=" + std::string(hebbian ? "YES" : "NO") +
                    ", STDP=" + std::string(stdp ? "YES" : "NO") +
                    ", Structural=" + std::string(structural ? "YES" : "NO"));
    } else {
        NLM_LOG_INFO("Plasticity disabled");
    }
}

void BrainManager::enableDevelopment(bool enable, float synaptogenesisRate, 
                                     float pruningRate, float maturationRate) {
    if (!checkInitialized("enableDevelopment")) {
        return;
    }
    
    // Store development parameters
    developmentEnabled = enable;
    this->synaptogenesisRate = synaptogenesisRate;
    this->pruningRate = pruningRate;
    this->maturationRate = maturationRate;
    
    if (enable) {
        NLM_LOG_INFO("Development enabled: synaptogenesis=" + std::to_string(synaptogenesisRate) +
                    ", pruning=" + std::to_string(pruningRate) +
                    ", maturation=" + std::to_string(maturationRate));
    } else {
        NLM_LOG_INFO("Development disabled");
    }
}

std::vector<NeuronId> BrainManager::getMostActiveNeurons(size_t count) const {
    std::vector<NeuronId> neuronIds;
    
    if (!checkInitialized("getMostActiveNeurons")) {
        return neuronIds;
    }
    
    if (auto* spikeSystem = brain->getSpikeSystem()) {
        neuronIds = spikeSystem->getMostActiveNeurons(count);
    }
    
    return neuronIds;
}

bool BrainManager::verifyIntegration() const {
    if (!checkInitialized("verifyIntegration")) {
        return false;
    }
    
    bool allSystemsPresent = true;
    
    // Check memory systems
    if (hasMemorySystems()) {
        NLM_LOG_INFO("[PASS] Memory systems are integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Memory systems are NOT integrated");
        allSystemsPresent = false;
    }
    
    // Check neuromodulation systems
    if (hasNeuromodulationSystems()) {
        NLM_LOG_INFO("[PASS] Neuromodulation systems are integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Neuromodulation systems are NOT integrated");
        allSystemsPresent = false;
    }
    
    // Check prediction system
    if (hasPredictionSystem()) {
        NLM_LOG_INFO("[PASS] Prediction system is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Prediction system is NOT integrated");
        allSystemsPresent = false;
    }
    
    // Check cognition systems
    if (hasCognitionSystems()) {
        NLM_LOG_INFO("[PASS] Cognition systems are integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Cognition systems are NOT integrated");
        allSystemsPresent = false;
    }
    
    // Check development system
    if (hasDevelopmentSystem()) {
        NLM_LOG_INFO("[PASS] Development system is integrated");
    } else {
        NLM_LOG_ERROR("[FAIL] Development system is NOT integrated");
        allSystemsPresent = false;
    }
    
    if (allSystemsPresent) {
        NLM_LOG_INFO("=== Integration Verification PASSED ===");
    } else {
        NLM_LOG_ERROR("=== Integration Verification FAILED ===");
    }
    
    return allSystemsPresent;
}

bool BrainManager::saveState(const std::string& filename) const {
    if (!checkInitialized("saveState")) {
        return false;
    }
    
    return brain->save(filename);
}

bool BrainManager::loadState(const std::string& filename) {
    if (!checkInitialized("loadState")) {
        return false;
    }
    
    if (!brain->load(filename)) {
        NLM_LOG_ERROR("Failed to load brain state from: " + filename);
        return false;
    }
    
    NLM_LOG_INFO("Brain state loaded successfully from: " + filename);
    return true;
}

bool BrainManager::hasDevelopmentSystem() const {
    if (!checkInitialized("hasDevelopmentSystem")) {
        return false;
    }
    
    return (brain->getDevelopmentSystem() != nullptr);
}

bool BrainManager::hasMemorySystems() const {
    if (!checkInitialized("hasMemorySystems")) {
        return false;
    }
    
    bool hasWorkingMemory = (brain->getWorkingMemory() != nullptr);
    bool hasEpisodicMemory = (brain->getEpisodicMemory() != nullptr);
    
    return hasWorkingMemory && hasEpisodicMemory;
}

bool BrainManager::hasNeuromodulationSystems() const {
    if (!checkInitialized("hasNeuromodulationSystems")) {
        return false;
    }
    
    bool hasDopamine = (brain->getDopamine() != nullptr);
    bool hasCuriosity = (brain->getCuriosity() != nullptr);
    bool hasNovelty = (brain->getNovelty() != nullptr);
    
    return hasDopamine && hasCuriosity && hasNovelty;
}

bool BrainManager::hasPredictionSystem() const {
    if (!checkInitialized("hasPredictionSystem")) {
        return false;
    }
    
    return (brain->getPredictionSystem() != nullptr);
}

bool BrainManager::hasCognitionSystems() const {
    if (!checkInitialized("hasCognitionSystems")) {
        return false;
    }
    
    bool hasPlanner = (brain->getPlanner() != nullptr);
    bool hasConceptFormation = (brain->getConceptFormation() != nullptr);
    bool hasAttention = (brain->getAttention() != nullptr);
    
    return hasPlanner && hasConceptFormation && hasAttention;
}

void BrainManager::resetStats() {
    currentSimulationTime = 0.0;
    plasticityEnabled = false;
    developmentEnabled = false;
    synaptogenesisRate = 0.0f;
    pruningRate = 0.0f;
    maturationRate = 0.0f;
}

void BrainManager::updateStats() {
    if (!initialized) return;
    
    auto stats = getStats();
    // Update current stats
}

bool BrainManager::checkInitialized(const std::string& operationName) const {
    if (!initialized) {
        NLM_LOG_ERROR("Cannot perform " + operationName + ": brain not initialized");
        return false;
    }
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot perform " + operationName + ": brain pointer is null");
        return false;
    }
    
    return true;
}

bool BrainManager::validateParameters(const std::string& operationName, 
                                     uint64_t param1, float param2) const {
    if (param1 == 0 && operationName.find("inject") != std::string::npos) {
        NLM_LOG_WARNING("Warning: Parameter count is zero in " + operationName);
    }
    
    if (param2 < 0.0f && operationName.find("current") != std::string::npos) {
        NLM_LOG_ERROR("Error: Negative current value in " + operationName);
        return false;
    }
    
    return true;
}

} // namespace nlm