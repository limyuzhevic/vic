#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <algorithm>

namespace nlm {

// Brain system accessors

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
    float totalExcitation = 0.0f;
    float totalInhibition = 0.0f;
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getType() == SynapseType::Excitatory) {
                totalExcitation += syn->getWeight();
            } else if (syn->getType() == SynapseType::Inhibitory) {
                totalInhibition += std::abs(syn->getWeight());
            }
        }
    }
    
    if (totalInhibition == 0.0f) return totalExcitation > 0.0f ? 1000.0f : 0.0f;
    
    return totalExcitation / totalInhibition;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem ? pImpl->spikeSystem->getPendingSpikeCount() : 0;
}

std::unique_ptr<Action> Brain::produceAction() {
    // This is implemented in BrainInputOutput.cpp - forward declaration for multiple inclusion
    return std::unique_ptr<Action>();
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulatory signals
    if (pImpl->dopamine) {
        pImpl->dopamine->applySignal(signal);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->applySignal(signal);
    }
    if (pImpl->novelty) {
        pImpl->novelty->applySignal(signal);
    }
    if (pImpl->predictionError) {
        pImpl->predictionError->applySignal(signal);
    }
}

void Brain::updatePlasticity() {
    // Update plasticity rules
    if (pImpl->stdp) {
        pImpl->stdp->update();
    }
    if (pImpl->hebbian) {
        pImpl->hebbian->update();
    }
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update();
    }
}

void Brain::develop() {
    // Apply developmental changes
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->develop();
    }
}

void Brain::reset() {
    // Reset brain state
    // Reset neurons
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->reset();
            }
        }
    }
    
    // Reset synapses
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            syn->reset();
        }
    }
    
    // Reset system state
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset integrated systems
    if (pImpl->workingMemory) pImpl->workingMemory->reset();
    if (pImpl->episodicMemory) pImpl->episodicMemory->reset();
    if (pImpl->associativeMemory) pImpl->associativeMemory->reset();
    if (pImpl->predictionSystem) pImpl->predictionSystem->reset();
    if (pImpl->planner) pImpl->planner->reset();
    if (pImpl->conceptFormation) pImpl->conceptFormation->reset();
    if (pImpl->attention) pImpl->attention->reset();
    if (pImpl->developmentSystem) pImpl->developmentSystem->reset();
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->predictionError) pImpl->predictionError->reset();
    if (pImpl->spikeSystem) pImpl->spikeSystem->reset();
    if (pImpl->stdp) pImpl->stdp->reset();
    if (pImpl->hebbian) pImpl->hebbian->reset();
    if (pImpl->structuralPlasticity) pImpl->structuralPlasticity->reset();
    if (pImpl->checkpointManager) pImpl->checkpointManager->clear();
    
    NLM_LOG_INFO("Brain state reset complete");
}

bool Brain::save(const std::string& filepath) const {
    // Save brain state to file (checkpointing)
    try {
        // Create checkpoint directory if it doesn't exist
        std::filesystem::path filepath_path(filepath);
        std::filesystem::path dir = filepath_path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Use checkpoint manager to save state
        if (pImpl->checkpointManager) {
            return pImpl->checkpointManager->save(filepath);
        }
        
        NLM_LOG_ERROR("Checkpoint manager not available for saving");
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while saving brain state: " + std::string(e.what()));
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    // Load brain state from file
    try {
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_ERROR("Checkpoint file does not exist: " + filepath);
            return false;
        }
        
        // Use checkpoint manager to load state
        if (pImpl->checkpointManager) {
            bool result = pImpl->checkpointManager->load(filepath);
            if (result) {
                NLM_LOG_INFO("Successfully loaded brain state from: " + filepath);
            } else {
                NLM_LOG_ERROR("Failed to load brain state from: " + filepath);
            }
            return result;
        }
        
        NLM_LOG_ERROR("Checkpoint manager not available for loading");
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while loading brain state: " + std::string(e.what()));
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    // Create and add a new region
    RegionId newId = ++pImpl->nextRegionId;
    
    auto region = std::make_unique<NeuralRegion>(newId, name);
    NeuralRegion* regionPtr = region.get();
    pImpl->regions.push_back(std::move(region));
    
    NLM_LOG_INFO("Added new region " + std::to_string(newId) + ": " + name);
    return newId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    // Find region by ID
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return const_cast<Brain*>(this)->getRegion(id);
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
    // Add long-range connection between regions
    InterRegionConnection conn(source, target, weight, delay);
    pImpl->interRegionConnections.push_back(conn);
    
    NLM_LOG_INFO("Added inter-region connection: " + std::to_string(source) + 
                 " -> " + std::to_string(target) + " (weight=" + 
                 std::to_string(weight) + ", delay=" + std::to_string(delay) + ")");
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    // Remove inter-region connection
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), 
                             pImpl->interRegionConnections.end(),
        [source, target](const InterRegionConnection& conn) {
            return conn.sourceRegion == source && conn.targetRegion == target;
        });
    
    if (it != pImpl->interRegionConnections.end()) {
        pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
        NLM_LOG_INFO("Removed inter-region connection: " + std::to_string(source) + 
                     " -> " + std::to_string(target));
    }
}

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
        total += region->getTotalSynapseCount();
    }
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getState().membranePotential > neuron->getState().restingPotential) {
                    active++;
                }
            }
        }
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    firing++;
                }
            }
        }
    }
    return firing;
}

float Brain::getAverageFiringRate() const {
    if (getTotalNeuronCount() == 0) return 0.0f;
    
    size_t totalFiring = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    totalFiring++;
                }
            }
        }
    }
    
    return static_cast<float>(totalFiring) / getTotalNeuronCount();
}

// Memory system accessors

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// Prediction system accessor

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// Cognition system accessors

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// Development system accessor

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// Neuromodulation system accessors

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

// Get current configuration

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

// Get random generator

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// Logging

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Working memory traces: " + std::to_string(
        pImpl->workingMemory ? pImpl->workingMemory->getActiveTraces() : 0));
    NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(
        pImpl->episodicMemory ? pImpl->episodicMemory->getEpisodeCount() : 0));
    NLM_LOG_INFO("Brain developmental stage: " + developmentalStageToString(pImpl->developmentalStage));
    NLM_LOG_INFO("=== End Brain Status ===");
}

// Helper function for developmental stage to string conversion

std::string Brain::developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::Growing: return "Growing";
        case DevelopmentalStage::Mature: return "Mature";
        case DevelopmentalStage::Declining: return "Declining";
        default: return "Unknown";
    }
}

} // namespace nlm