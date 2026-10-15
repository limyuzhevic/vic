#include "BrainState.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>

namespace nlm {

void BrainState::initializeState(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    NLM_LOG_INFO("Initializing brain state management...");
    
    // Configure state management from config
    configureStateManagement(config);
    
    // Initialize developmental stage
    initializeDevelopmentalStage(brain, config);
    
    // Initialize statistics
    initializeStatistics(brain);
    
    // Setup checkpoint system
    setupCheckpointSystem(brain, config);
    
    // Initialize memory pools if enabled
    if (config_->enableMemoryPools) {
        initializeMemoryPools(brain);
    }
    
    NLM_LOG_INFO("Brain state management initialized successfully");
}

void BrainState::resetState(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("Resetting brain state...");
    
    // Reset all integrated systems
    brain->reset();
    
    // Reset developmental stage
    brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    
    // Reset statistics
    resetStatistics(brain);
    
    // Reset checkpoints
    checkpoints_.clear();
    
    // Reset performance metrics
    perfMetrics_.totalStepsProcessed = 0;
    perfMetrics_.memoryAllocations = 0;
    perfMetrics_.memoryDeallocations = 0;
    
    NLM_LOG_INFO("Brain state reset complete");
}

void BrainState::checkpointState(std::shared_ptr<Brain> brain, const std::string& label) {
    std::string checkpointLabel = label.empty() ? "checkpoint_" + std::to_string(state_.currentStep) : label;
    
    // Save current state
    std::string filepath = "./checkpoints/" + checkpointLabel + ".bin";
    
    if (saveCompleteState(brain, filepath)) {
        CheckpointInfo info(checkpointLabel, filepath, state_.currentStep, state_.currentTime);
        checkpoints_.push_back(info);
        
        NLM_LOG_INFO("Checkpoint saved: " + checkpointLabel + " at step " + std::to_string(state_.currentStep));
    } else {
        NLM_LOG_ERROR("Failed to save checkpoint: " + checkpointLabel);
    }
}

void BrainState::loadState(std::shared_ptr<Brain> brain, const std::string& label) {
    // Find checkpoint
    CheckpointInfo* checkpoint = nullptr;
    for (auto& cp : checkpoints_) {
        if (cp.label == label) {
            checkpoint = &cp;
            break;
        }
    }
    
    if (!checkpoint) {
        NLM_LOG_ERROR("Checkpoint not found: " + label);
        return;
    }
    
    // Load state
    if (loadCompleteState(brain, checkpoint->filepath)) {
        NLM_LOG_INFO("State loaded from checkpoint: " + label);
    } else {
        NLM_LOG_ERROR("Failed to load checkpoint: " + label);
    }
}

void BrainState::initializeDevelopmentalStage(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    // Initialize developmental stage based on configuration
    brain->setDevelopmentalStage(DevelopmentalStage::Initial);
    
    // Set developmental parameters from config
    config->set("development_maturation_rate", 0.001f, ConfigSource::Default);
    config->set("development_pruning_rate", 0.0001f, ConfigSource::Default);
    config->set("development_synaptogenesis_rate", 0.0002f, ConfigSource::Default);
}

void BrainState::updateDevelopmentalStage(std::shared_ptr<Brain> brain, SimulationStep currentStep,
                                          Timestamp currentTime) {
    // Update developmental stage based on time and activity
    updateDevelopmentalProgression(brain, currentStep);
    
    // Apply development effects
    applyDevelopmentEffects(brain, currentStep, currentTime);
}

DevelopmentalStage BrainState::getCurrentDevelopmentalStage() const {
    return state_.developmentalStage;
}

void BrainState::setDevelopmentalStage(DevelopmentalStage stage) {
    state_.developmentalStage = stage;
}

void BrainState::updateTime(SimulationStep currentStep, Timestamp currentTime) {
    state_.currentStep = currentStep;
    state_.currentTime = currentTime;
}

SimulationStep BrainState::getCurrentStep() const {
    return state_.currentStep;
}

Timestamp BrainState::getCurrentTime() const {
    return state_.currentTime;
}

TimestepDuration BrainState::getTimestep() const {
    return state_.timestep;
}

void BrainState::setTimestep(TimestepDuration timestep) {
    state_.timestep = timestep;
}

void BrainState::initializeStatistics(std::shared_ptr<Brain> brain) {
    // Initialize performance and activity statistics
    perfMetrics_.totalStepsProcessed = 0;
    perfMetrics_.totalProcessingTime = 0.0;
    
    // Start performance measurement
    startPerformanceMeasurement("statistics_update");
}

void BrainState::updateStatistics(std::shared_ptr<Brain> brain) {
    // Update comprehensive statistics
    perfMetrics_.totalStepsProcessed++;
    
    // Record performance
    endPerformanceMeasurement("statistics_update");
    
    // Update brain-specific statistics
    updateBrainStatistics(brain);
    
    // Monitor memory usage
    if (config_->enableMemoryPools) {
        monitorMemoryUsage();
    }
}

void BrainState::resetStatistics(std::shared_ptr<Brain> brain) {
    // Reset all statistics
    perfMetrics_.totalStepsProcessed = 0;
    perfMetrics_.totalProcessingTime = 0.0;
    perfMetrics_.operationTimes.clear();
    perfMetrics_.operationCounts.clear();
    
    // Reset brain statistics would go here
}

void BrainState::startPerformanceMeasurement(const std::string& operation) {
    // Start timing an operation
    perfMetrics_.operationCounts[operation]++;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    perfMetrics_.operationTimes[operation].push_back(startTime);
}

void BrainState::endPerformanceMeasurement(const std::string& operation) {
    // End timing an operation and calculate duration
    auto& startTimes = perfMetrics_.operationTimes[operation];
    
    if (!startTimes.empty()) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = startTimes.back();
        startTimes.pop_back();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        perfMetrics_.totalProcessingTime += duration.count() / 1000000.0; // Convert to seconds
    }
}

double BrainState::getPerformanceMetric(const std::string& operation) const {
    // Get average time for an operation
    auto it = perfMetrics_.operationTimes.find(operation);
    if (it == perfMetrics_.operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double totalTime = 0.0;
    for (const auto& startTime : it->second) {
        // In a real implementation, we would track end times separately
        // For now, return 0 as placeholder
    }
    
    return totalTime / it->second.size();
}

void BrainState::logPerformanceSummary() const {
    NLM_LOG_INFO("=== Performance Summary ===");
    NLM_LOG_INFO("Total steps processed: " + std::to_string(perfMetrics_.totalStepsProcessed));
    NLM_LOG_INFO("Total processing time: " + std::to_string(perfMetrics_.totalProcessingTime) + "s");
    
    if (perfMetrics_.totalStepsProcessed > 0) {
        double avgTimePerStep = perfMetrics_.totalProcessingTime / perfMetrics_.totalStepsProcessed;
        NLM_LOG_INFO("Average time per step: " + std::to_string(avgTimePerStep) + "s");
    }
    
    NLM_LOG_INFO("Memory allocations: " + std::to_string(perfMetrics_.memoryAllocations));
    NLM_LOG_INFO("Memory deallocations: " + std::to_string(perfMetrics_.memoryDeallocations));
}

void BrainState::initializeMemoryPools(std::shared_ptr<Brain> brain) {
    // Initialize memory pools for efficient allocation
    NLM_LOG_INFO("Initializing memory pools...");
    
    // In a real implementation, this would set up memory pools
    // For now, just log the action
    perfMetrics_.memoryAllocations++;
    
    NLM_LOG_INFO("Memory pools initialized");
}

void BrainState::monitorMemoryUsage() const {
    // Monitor memory usage (placeholder implementation)
    NLM_LOG_DEBUG("Memory monitoring: enabled");
}

bool BrainState::checkMemoryHealth() const {
    // Check memory health (placeholder implementation)
    return true;
}

bool BrainState::saveCompleteState(std::shared_ptr<Brain> brain, const std::string& filepath) const {
    // Save complete brain state
    try {
        // Create checkpoint writer
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getRegionCount(),
            state_.currentStep,
            state_.currentTime
        );
        
        // Save neurons
        serializeNeurons(brain.get(), writer);
        
        // Save synapses
        serializeSynapses(brain.get(), writer);
        
        // Save integrated system states
        serializeSystems(brain.get(), writer);
        
        // Finalize
        if (!writer.finalize()) {
            return false;
        }
        
        NLM_LOG_INFO("Complete state saved: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving state: ") + e.what());
        return false;
    }
}

bool BrainState::loadCompleteState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    // Load complete brain state
    try {
        // Open checkpoint
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            return false;
        }
        
        // Validate
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Load neurons
        deserializeNeurons(brain, reader);
        
        // Load synapses
        deserializeSynapses(brain, reader);
        
        // Load integrated systems
        deserializeSystems(brain, reader);
        
        NLM_LOG_INFO("Complete state loaded: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading state: ") + e.what());
        return false;
    }
}

std::vector<std::string> BrainState::getAvailableCheckpoints(const std::string& directory) const {
    // Get list of available checkpoints (placeholder implementation)
    std::vector<std::string> checkpoints;
    
    // In a real implementation, this would scan the directory
    // For now, return empty vector
    return checkpoints;
}

bool BrainState::handleError(std::shared_ptr<Brain> brain, const std::string& errorMessage) {
    // Handle errors and attempt recovery
    NLM_LOG_ERROR("Error handled: " + errorMessage);
    
    // Log error for recovery
    if (config_->enableStateValidation) {
        validateStateIntegrity(brain);
    }
    
    // Attempt recovery if possible
    return recoverFromError(brain);
}

bool BrainState::recoverFromError(std::shared_ptr<Brain> brain) {
    // Attempt to recover from error
    NLM_LOG_INFO("Attempting error recovery...");
    
    try {
        // Reset to last known good state if available
        if (!checkpoints_.empty()) {
            auto lastCheckpoint = checkpoints_.back();
            loadState(brain, lastCheckpoint.label);
            NLM_LOG_INFO("Recovered from checkpoint: " + lastCheckpoint.label);
            return true;
        }
        
        // If no checkpoint, try resetting state
        resetState(brain);
        NLM_LOG_INFO("Recovered by resetting state");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Recovery failed: ") + e.what());
        return false;
    }
}

bool BrainState::validateStateIntegrity(std::shared_ptr<Brain> brain) const {
    // Validate state integrity
    NLM_LOG_INFO("Validating state integrity...");
    
    bool isValid = validateBrainState(brain.get());
    isValid = validateIntegrationState(brain.get()) && isValid;
    isValid = validatePlasticityState(brain.get()) && isValid;
    
    if (isValid) {
        NLM_LOG_INFO("State integrity check passed");
    } else {
        NLM_LOG_ERROR("State integrity check failed");
    }
    
    return isValid;
}

void BrainState::configureStateManagement(std::shared_ptr<Config> config) {
    // Configure state management from config
    config_->checkpointInterval = config->getOr<size_t>("checkpoint_interval", 1000);
    config_->consolidationInterval = config->getOr<size_t>("consolidation_interval", 10000);
    config_->replayInterval = config->getOr<size_t>("replay_interval", 100);
    config_->developmentUpdateInterval = config->getOr<size_t>("development_update_interval", 1000);
    config_->enablePerformanceMonitoring = config->getOr<bool>("enable_performance_monitoring", true);
    config_->memoryLimitBytes = config->getOr<size_t>("memory_limit_bytes", 1024 * 1024 * 1024); // 1GB
    config_->enableStateValidation = config->getOr<bool>("enable_state_validation", true);
    config_->enableMemoryPools = config->getOr<bool>("enable_memory_pools", true);
}

void BrainState::setCheckpointInterval(size_t interval) {
    config_->checkpointInterval = interval;
}

void BrainState::setConsolidationInterval(size_t interval) {
    config_->consolidationInterval = interval;
}

void BrainState::enablePerformanceMonitoring(bool enable) {
    config_->enablePerformanceMonitoring = enable;
}

void BrainState::setMemoryLimit(size_t limitBytes) {
    config_->memoryLimitBytes = limitBytes;
}

bool BrainState::validateBrainState(const Brain* brain) const {
    // Validate brain state
    bool isValid = true;
    
    // Check neuron count
    size_t neuronCount = brain->getTotalNeuronCount();
    if (neuronCount == 0) {
        NLM_LOG_WARNING("No neurons in brain");
        isValid = false;
    }
    
    // Check region count
    size_t regionCount = brain->getRegionCount();
    if (regionCount == 0) {
        NLM_LOG_WARNING("No regions in brain");
        isValid = false;
    }
    
    // Check for consistency
    isValid = checkNeuronConsistency(brain) && isValid;
    isValid = checkSynapseConsistency(brain) && isValid;
    
    return isValid;
}

bool BrainState::validateIntegrationState(const Brain* brain) const {
    // Validate integration state (placeholder)
    NLM_LOG_DEBUG("Validating integration state...");
    return true;
}

bool BrainState::validatePlasticityState(const Brain* brain) const {
    // Validate plasticity state (placeholder)
    NLM_LOG_DEBUG("Validating plasticity state...");
    return true;
}

bool BrainState::checkNeuronConsistency(const Brain* brain) const {
    // Check neuron consistency (placeholder)
    return true;
}

bool BrainState::checkSynapseConsistency(const Brain* brain) const {
    // Check synapse consistency (placeholder)
    return true;
}

std::string BrainState::serializeState(const Brain* brain) const {
    // Serialize brain state to string (placeholder)
    std::ostringstream oss;
    oss << "BrainState:" << std::endl;
    oss << "  Step: " << state_.currentStep << std::endl;
    oss << "  Time: " << state_.currentTime << std::endl;
    oss << "  Developmental Stage: " << static_cast<int>(state_.developmentalStage) << std::endl;
    oss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
    oss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    return oss.str();
}

bool BrainState::deserializeState(std::shared_ptr<Brain> brain, const std::string& serializedState) {
    // Deserialize brain state from string (placeholder)
    NLM_LOG_INFO("Deserializing state (placeholder implementation)");
    
    // In a real implementation, this would parse the serialized state
    // For now, just reset state
    resetState(brain);
    
    return true;
}

void BrainState::serializeNeurons(const Brain* brain, CheckpointWriter& writer) const {
    // Serialize neurons to checkpoint (placeholder)
    NeuronCheckpointData neuronData;
    
    // Collect neuron states
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                
                neuronData.membranePotential.push_back(state.membranePotential);
                neuronData.restingPotential.push_back(state.restingPotential);
                neuronData.threshold.push_back(state.threshold);
                neuronData.resetPotential.push_back(state.resetPotential);
                neuronData.leakConductance.push_back(state.leakConductance);
                neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
            }
        }
    }
    
    writer.writeNeurons(neuronData);
}

void BrainState::serializeSynapses(const Brain* brain, CheckpointWriter& writer) const {
    // Serialize synapses to checkpoint (placeholder)
    SynapseCheckpointData synapseData;
    
    // Collect synapse states
    for (const auto& region : brain->getRegions()) {
        for (const auto* syn : region->getSynapses()) {
            synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
            synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
            synapseData.weight.push_back(syn->getWeight());
            synapseData.delay.push_back(syn->getDelay());
            synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
            synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
        }
    }
    
    writer.writeSynapses(synapseData);
}

void BrainState::serializeSystems(const Brain* brain, CheckpointWriter& writer) const {
    // Serialize integrated system states (placeholder)
    NLM_LOG_DEBUG("Serializing integrated system states...");
    
    // In a real implementation, this would serialize each system's state
    // For now, just log
}

void BrainState::deserializeNeurons(std::shared_ptr<Brain> brain, CheckpointReader& reader) {
    // Deserialize neurons from checkpoint (placeholder)
    NeuronCheckpointData neuronData;
    
    if (reader.readNeurons(neuronData)) {
        NLM_LOG_DEBUG("Deserialized " + std::to_string(neuronData.membranePotential.size()) + " neurons");
    }
}

void BrainState::deserializeSynapses(std::shared_ptr<Brain> brain, CheckpointReader& reader) {
    // Deserialize synapses from checkpoint (placeholder)
    SynapseCheckpointData synapseData;
    
    if (reader.readSynapses(synapseData)) {
        NLM_LOG_DEBUG("Deserialized " + std::to_string(synapseData.weight.size()) + " synapses");
    }
}

void BrainState::deserializeSystems(std::shared_ptr<Brain> brain, CheckpointReader& reader) {
    // Deserialize integrated system states (placeholder)
    NLM_LOG_DEBUG("Deserializing integrated system states...");
}

void BrainState::updateDevelopmentalProgression(std::shared_ptr<Brain> brain, SimulationStep currentStep) {
    // Update developmental stage based on time
    DevelopmentalStage newStage = state_.developmentalStage;
    
    if (currentStep >= 0 && currentStep < 100) {
        newStage = DevelopmentalStage::Initial;
    } else if (currentStep >= 100 && currentStep < 500) {
        newStage = DevelopmentalStage::CriticalPeriod;
    } else if (currentStep >= 500 && currentStep < 1000) {
        newStage = DevelopmentalStage::Maturation;
    } else {
        newStage = DevelopmentalStage::Adult;
    }
    
    if (newStage != state_.developmentalStage) {
        NLM_LOG_INFO("Developmental stage changed from " +
                    std::to_string(static_cast<int>(state_.developmentalStage)) + " to " +
                    std::to_string(static_cast<int>(newStage)));
        brain->setDevelopmentalStage(newStage);
    }
}

void BrainState::applyDevelopmentEffects(std::shared_ptr<Brain> brain, SimulationStep currentStep,
                                         Timestamp currentTime) {
    // Apply development effects
    if (currentStep % config_->developmentUpdateInterval == 0 && currentStep > 0) {
        brain->develop();
    }
}

void BrainState::updateBrainStatistics(std::shared_ptr<Brain> brain) {
    // Update brain-specific statistics
    NLM_LOG_DEBUG("Updating brain statistics...");
    
    // Log key statistics
    NLM_LOG_INFO("Brain stats - Neurons: " + std::to_string(brain->getTotalNeuronCount()) +
                ", Synapses: " + std::to_string(brain->getTotalSynapseCount()) +
                ", Active: " + std::to_string(brain->getActiveNeuronCount()) +
                ", Firing: " + std::to_string(brain->getFiringNeuronCount()));
// ========== STEP 6: Handle state persistence and statistics ==========
    pImpl->brainState->updateStatistics(std::shared_ptr<Brain>(this, [](Brain*) {}));
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->brainIO->processSensoryInput(std::shared_ptr<Brain>(this, [](Brain*) {}), input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->brainCore->injectCurrentToNeurons(this, type, current);
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Modular) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Log integrated system status
    pImpl->brainIntegration->logIntegrationStatus(this);
    
    // Log performance metrics
    pImpl->brainState->logPerformanceSummary();
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->brainCore->getExcitationInhibitionRatio(this);
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Use BrainIO to generate action
    pImpl->brainIO->generateMotorOutput(std::shared_ptr<Brain>(this, [](Brain*) {}));
    
    // Simple action selection based on motor neuron activity
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->brainIntegration->integrateNeuromodulationWithPlasticity(std::shared_ptr<Brain>(this, [](Brain*) {}));
}

void Brain::updatePlasticity() {
    pImpl->brainIntegration->updatePlasticityFromNeuromodulation(std::shared_ptr<Brain>(this, [](Brain*) {}));
}

void Brain::develop() {
    pImpl->brainIntegration->applyDevelopmentEffectsToPlasticity(
        std::shared_ptr<Brain>(this, [](Brain*) {}), pImpl->developmentalStage);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset integrated systems
    pImpl->brainIntegration->resetAllIntegratedSystems(std::shared_ptr<Brain>(this, [](Brain*) {}));
    pImpl->brainState->resetStatistics(std::shared_ptr<Brain>(this, [](Brain*) {}));
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        // Use checkpoint system to save state
        if (pImpl->checkpointManager) {
            return pImpl->checkpointManager->save(const_cast<Brain*>(this), filepath);
        }
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        // Use checkpoint system to load state
        if (pImpl->checkpointManager) {
            return pImpl->checkpointManager->load(const_cast<Brain*>(this), filepath);
        }
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                    float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->brainCore->getTotalNeuronCount(this);
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->brainCore->getTotalSynapseCount(this);
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->brainCore->getActiveNeuronCount(this);
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    return pImpl->brainCore->getAverageFiringRate(this);
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

CheckpointManager* Brain::getCheckpointManager() {
    return pImpl->checkpointManager.get();
}

// Module-specific accessors
BrainCore* Brain::getBrainCore() {
    return pImpl->brainCore.get();
}

BrainIntegration* Brain::getBrainIntegration() {
    return pImpl->brainIntegration.get();
}

BrainIO* Brain::getBrainIO() {
    return pImpl->brainIO.get();
}

BrainState* Brain::getBrainState() {
    return pImpl->brainState.get();
}

Environment* Brain::getEnvironment() {
    return pImpl->environment;
}

void Brain::setEnvironment(Environment* env) {
    pImpl->environment = env;
}

} // namespace nlm