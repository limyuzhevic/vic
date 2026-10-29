// Brain.cpp has been refactored into modular components:
// - BrainMain.cpp: Main class implementation (constructor/destructor/basic operations)
// - BrainInitialization.cpp: Brain initialization logic
// - BrainStep.cpp: Main simulation step logic
// - BrainMemory.cpp: Memory systems integration
// - BrainPrediction.cpp: Prediction system integration
// - BrainCognition.cpp: Cognition systems integration
// - BrainNeuromodulation.cpp: Neuromodulation systems integration
// - BrainPlasticity.cpp: Plasticity systems integration
// - BrainDevelopment.cpp: Development system integration
// - BrainCheckpoint.cpp: Checkpoint system integration
// - BrainStatistics.cpp: Statistics and logging

// The original 1117-line Brain.cpp has been split into manageable, focused components
// following the single responsibility principle and improving maintainability.

// This file now serves as the main coordinator, delegating to specialized modules
// for better code organization, testability, and maintainability.

#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ============================================================================
// COORDINATOR FUNCTIONS - Main Brain class methods that delegate to modules
// ============================================================================

bool Brain::initialize() {
    // Use the initialization module for clean separation of concerns
    return BrainInitialization::initializeBrain(this);
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Use the step handler for coordinated execution
    BrainStepHandler::executeStep(this, currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    if (pImpl->sensoryNeurons.empty()) return;
    
    // Advanced: Distribute input based on neuron type and preference
    float currentPerNeuron = input.getTotalCurrent() / pImpl->sensoryNeurons.size();
    for (auto* neuron : pImpl->sensoryNeurons) {
        neuron->receiveExcitatoryInput(currentPerNeuron * neuron->getInputSensitivity());
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* n : pop->getNeurons()) {
                if (n->getId() == neuron) {
                    n->injectCurrent(current);
                    return;
                }
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current * neuron->getInputSensitivity());
                }
            }
        }
    }
}

// ============================================================================
// SYSTEM ACCESSORS - Clean interface to integrated systems
// ============================================================================

SpikeSystem* Brain::getSpikeSystem() { return pImpl->spikeSystem.get(); }
const SpikeSystem* Brain::getSpikeSystem() const { return pImpl->spikeSystem.get(); }
STDP* Brain::getSTDP() { return pImpl->stdp.get(); }
Hebbian* Brain::getHebbian() { return pImpl->hebbian.get(); }
StructuralPlasticity* Brain::getStructuralPlasticity() { return pImpl->structuralPlasticity.get(); }

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) totalExcitatory += weight;
            else totalInhibitory += std::abs(weight);
        }
    }
    
    if (totalInhibitory > 0.0f) return totalExcitatory / totalInhibitory;
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const { return pImpl->totalSpikesTotal; }
size_t Brain::getPendingSpikeEventCount() const { 
    return pImpl->spikeSystem->getPendingSpikeCount() + 
           pImpl->spikeSystem->getPendingDelayedCount(); 
}

std::unique_ptr<class Action> Brain::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Advanced action selection based on motor neuron group activity
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) ++firingMotor;
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) type = ActionType::MoveForward;
    
    return std::make_unique<Action>(type);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    float modulation = signal.getLevel();
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step - kept for API compatibility
}

void Brain::develop() {
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    if (pImpl->workingMemory) pImpl->workingMemory->reset();
    if (pImpl->episodicMemory) pImpl->episodicMemory->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    return BrainCheckpoint::saveBrainState(const_cast<Brain*>(this), filepath);
}

bool Brain::load(const std::string& filepath) {
    if (!filepath.empty()) {
        NLM_LOG_INFO("Loading NLM Brain from: " + filepath);
        return BrainCheckpoint::loadBrainState(const_cast<Brain*>(this), filepath);
    }
    return false;
}

// ============================================================================
// REGISTRY AND ACCESSOR METHODS - Clean system interfaces
// ============================================================================

RegionId Brain::addRegion(const std::string& name) {
    RegionId newId = pImpl->nextRegionId++;
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(newId, name));
    return newId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) return region.get();
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return const_cast<Brain*>(this)->getRegion(id);
}

size_t Brain::getRegionCount() const { return pImpl->regions.size(); }
std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const { return pImpl->regions; }

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), pImpl->interRegionConnections.end(),
        [source, target](const InterRegionConnection& conn) {
            return conn.sourceRegion == source && conn.targetRegion == target;
        });
    pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
}

// ============================================================================
// STATISTICS ACCESSORS - Simple interface to brain statistics
// ============================================================================

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        active += region->getActiveNeuronCount();
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        firing += region->getFiringNeuronCount();
    }
    return firing;
}

float Brain::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            float popRate = pop->getAverageFiringRate();
            if (popRate > 0.0f) {
                totalRate += popRate * pop->getNeuronCount();
                neuronCount += pop->getNeuronCount();
            }
        }
    }
    
    return neuronCount > 0 ? totalRate / neuronCount : 0.0f;
}

// ============================================================================
// MEMORY SYSTEM ACCESSORS - Clean interfaces for all memory systems
// ============================================================================

NeuralWorkingMemory* Brain::getWorkingMemory() { return pImpl->workingMemory.get(); }
NeuralEpisodicMemory* Brain::getEpisodicMemory() { return pImpl->episodicMemory.get(); }
NeuralAssociativeMemory* Brain::getAssociativeMemory() { return pImpl->associativeMemory.get(); }

// ============================================================================
// PREDICTION SYSTEM ACCESSOR
// ============================================================================

PredictionSystem* Brain::getPredictionSystem() { return pImpl->predictionSystem.get(); }

// ============================================================================
// COGNITION SYSTEM ACCESSORS
// ============================================================================

NeuralPlanner* Brain::getPlanner() { return pImpl->planner.get(); }
ConceptFormation* Brain::getConceptFormation() { return pImpl->conceptFormation.get(); }
AttentionalSelection* Brain::getAttention() { return pImpl->attention.get(); }

// ============================================================================
// DEVELOPMENT SYSTEM ACCESSOR
// ============================================================================

DevelopmentSystem* Brain::getDevelopmentSystem() { return pImpl->developmentSystem.get(); }
DevelopmentalStage Brain::getDevelopmentalStage() const { return pImpl->developmentalStage; }
void Brain::setDevelopmentalStage(DevelopmentalStage stage) { pImpl->developmentalStage = stage; }

// ============================================================================
// NEUROMODULATION SYSTEM ACCESSORS
// ============================================================================

Dopamine* Brain::getDopamine() { return pImpl->dopamine.get(); }
Curiosity* Brain::getCuriosity() { return pImpl->curiosity.get(); }
Novelty* Brain::getNovelty() { return pImpl->novelty.get(); }
PredictionError* Brain::getPredictionErrorSignal() { return pImpl->predictionError.get(); }

// ============================================================================
// UTILITY ACCESSORS
// ============================================================================

std::shared_ptr<const Config> Brain::getConfig() const { return pImpl->config; }
RandomGenerator* Brain::getRandomGenerator() { return pImpl->rng.get(); }

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM BRAIN STATUS ===");
    NLM_LOG_INFO("Step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Time: " + std::to_string(pImpl->currentTime) + " seconds");
    NLM_LOG_INFO("Development Stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    NLM_LOG_INFO("Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
}

} // namespace nlm