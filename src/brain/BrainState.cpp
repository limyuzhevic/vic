#include "BrainState.hpp"
#include "Brain.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

void BrainState::reset(Brain* parent) {
    if (!parent) return;
    
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    auto* core = parent->getCore();
    if (!core) return;
    
    core->currentStep = 0;
    core->currentTime = 0.0;
    core->totalSpikesThisStep = 0;
    core->totalSpikesTotal = 0;
    core->isResting = false;
    core->stepsSinceLastEpisode = 0;
    
    for (const auto& region : parent->getRegions()) {
        region->reset();
    }
    
    core->spikeSystem->reset();
    core->developmentSystem = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (auto workingMemory = getWorkingMemory(parent)) {
        workingMemory->clear();
    }
    if (auto episodicMemory = getEpisodicMemory(parent)) {
        episodicMemory->clear();
    }
    if (auto associativeMemory = getAssociativeMemory(parent)) {
        associativeMemory->clear();
    }
    if (auto attention = getAttention(parent)) {
        attention->reset();
    }
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool BrainState::save(const Brain* parent, const std::string& filepath) const {
    if (!parent) return false;
    
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(parent),
            getTotalSynapseCount(parent),
            getRegionCount(parent),
            parent->getCurrentStep(),
            parent->getCurrentTime()
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount(parent));
        neuronData.restingPotential.reserve(getTotalNeuronCount(parent));
        neuronData.threshold.reserve(getTotalNeuronCount(parent));
        neuronData.resetPotential.reserve(getTotalNeuronCount(parent));
        neuronData.leakConductance.reserve(getTotalNeuronCount(parent));
        
        for (const auto& region : getRegions(parent)) {
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
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : getRegions(parent)) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool BrainState::load(Brain* parent, const std::string& filepath) {
    if (!parent) return false;
    
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (const auto& region : getRegions(parent)) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        if (idx < neuronData.firingState.size()) {
                            neuron->setFiringState(static_cast<FiringState>(neuronData.firingState[idx]));
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            neuron->setRefractoryPeriod(neuronData.refractoryPeriod[idx]);
                        }
                    }
                    idx++;
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

size_t BrainState::getTotalNeuronCount(const Brain* parent) const {
    size_t total = 0;
    for (const auto& region : getRegions(parent)) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainState::getTotalSynapseCount(const Brain* parent) const {
    size_t total = 0;
    for (const auto& region : getRegions(parent)) {
        total += region->getSynapseCount();
    }
    total += parent->getInterRegionConnectionCount();
    return total;
}

size_t BrainState::getRegionCount(const Brain* parent) const {
    return getRegions(parent).size();
}

size_t BrainState::getActiveNeuronCount(const Brain* parent) const {
    size_t total = 0;
    for (const auto& region : getRegions(parent)) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t BrainState::getFiringNeuronCount(const Brain* parent) const {
    auto* core = parent->getCore();
    return core ? core->totalSpikesThisStep : 0;
}

float BrainState::getAverageFiringRate(const Brain* parent) const {
    size_t regionCount = getRegionCount(parent);
    if (regionCount == 0) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& region : getRegions(parent)) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(regionCount);
}

float BrainState::getExcitationInhibitionRatio(const Brain* parent) const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : getRegions(parent)) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t BrainState::getTotalSpikeCount(const Brain* parent) const {
    auto* core = parent->getCore();
    return core ? core->totalSpikesTotal : 0;
}

size_t BrainState::getPendingSpikeEventCount(const Brain* parent) const {
    auto* spikeSystem = getSpikeSystem(parent);
    if (!spikeSystem) return 0;
    
    return spikeSystem->getPendingSpikeCount() + spikeSystem->getPendingDelayedCount();
}

void BrainState::logStatus(const Brain* parent) const {
    if (!parent) return;
    
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount(parent)));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount(parent)));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount(parent)));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount(parent)));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount(parent)));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount(parent)));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount(parent)));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate(parent)));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio(parent)));
    
    // Memory system status
    if (auto workingMemory = getWorkingMemory(const_cast<Brain*>(parent))) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(workingMemory->getActiveTraces()));
    }
    if (auto episodicMemory = getEpisodicMemory(const_cast<Brain*>(parent))) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (auto dopamine = getDopamine(const_cast<Brain*>(parent))) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(getDevelopmentalStage(parent))));
    
    for (const auto& region : getRegions(parent)) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

std::vector<RegionId> BrainState::getRegionIds(const Brain* parent) const {
    std::vector<RegionId> ids;
    ids.reserve(getRegionCount(parent));
    for (const auto& region : getRegions(parent)) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainState::getRegions(const Brain* parent) const {
    return parent->getRegions();
}

NeuralWorkingMemory* BrainState::getWorkingMemory(Brain* parent) {
    return parent->getWorkingMemory();
}

NeuralEpisodicMemory* BrainState::getEpisodicMemory(Brain* parent) {
    return parent->getEpisodicMemory();
}

NeuralAssociativeMemory* BrainState::getAssociativeMemory(Brain* parent) {
    return parent->getAssociativeMemory();
}

PredictionSystem* BrainState::getPredictionSystem(Brain* parent) {
    return parent->getPredictionSystem();
}

NeuralPlanner* BrainState::getPlanner(Brain* parent) {
    return parent->getPlanner();
}

ConceptFormation* BrainState::getConceptFormation(Brain* parent) {
    return parent->getConceptFormation();
}

AttentionalSelection* BrainState::getAttention(Brain* parent) {
    return parent->getAttention();
}

DevelopmentSystem* BrainState::getDevelopmentSystem(Brain* parent) {
    return parent->getDevelopmentSystem();
}

DevelopmentalStage BrainState::getDevelopmentalStage(const Brain* parent) const {
    return parent->getDevelopmentalStage();
}

Dopamine* BrainState::getDopamine(Brain* parent) {
    return parent->getDopamine();
}

Curiosity* BrainState::getCuriosity(Brain* parent) {
    return parent->getCuriosity();
}

Novelty* BrainState::getNovelty(Brain* parent) {
    return parent->getNovelty();n}
    PredictionError* BrainState::getPredictionErrorSignal(Brain* parent) {
    return parent->getPredictionErrorSignal();
}

std::shared_ptr<const Config> BrainState::getConfig(const Brain* parent) const {
    return parent->getConfig();
}

RandomGenerator* BrainState::getRandomGenerator(Brain* parent) {
    return parent->getRandomGenerator();
}

SpikeSystem* BrainState::getSpikeSystem(Brain* parent) {
    return parent->getSpikeSystem();
}

const SpikeSystem* BrainState::getSpikeSystem(const Brain* parent) const {
    return parent->getSpikeSystem();
}

STDP* BrainState::getSTDP(Brain* parent) {
    return parent->getSTDP();
}

Hebbian* BrainState::getHebbian(Brain* parent) {
    return parent->getHebbian();
}

StructuralPlasticity* BrainState::getStructuralPlasticity(Brain* parent) {
    return parent->getStructuralPlasticity();
}

void BrainState::collectStatistics(Brain* parent) {
    // Statistics are collected in BrainSimulation
}

void BrainState::updateIntegrationState(Brain* parent) {
    // Integration state is updated in BrainSimulation
}

} // namespace nlm