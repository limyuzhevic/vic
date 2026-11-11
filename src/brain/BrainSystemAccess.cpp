#pragma once

#include "Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void Brain::Impl::receiveSensoryInput(const SensoryInput& input) {
    if (input.empty()) return;
    
    // Process sensory input based on type and inject into appropriate neurons
    for (const auto& item : input.getItems()) {
        NeuronId neuronId = neuronById[item.neuronId];
        if (neuronId != 0) {
            injectCurrent(neuronId, item.current);
        }
    }
}

void Brain::Impl::applyReward(float reward) {
    if (rewardModulationEnabled) {
        // Apply reward through neuromodulatory systems
        if (dopamine) {
            dopamine->update(reward);
        }
        if (predictionError) {
            predictionError->update(reward);
        }
        if (novelty) {
            novelty->update(reward);
        }
        if (curiosity) {
            curiosity->update(reward);
        }
    }
}

void Brain::Impl::applyNeuromodulation() {
    if (dopamine) dopamine->apply();
    if (novelty) novelty->apply();
    if (predictionError) predictionError->apply();
    if (curiosity) curiosity->apply();
}

void Brain::Impl::save() {
    if (checkpointManager) {
        // Save brain state to checkpoint
        checkpointManager->save(this);
    }
}

void Brain::Impl::load() {
    if (checkpointManager) {
        // Load brain state from checkpoint
        checkpointManager->load(this);
    }
}

void Brain::Impl::logStatus() const {
    NLM_LOG_INFO("Brain Status:");
    NLM_LOG_INFO("  Developmental Stage: " + std::to_string(static_cast<int>(developmentalStage)));
    NLM_LOG_INFO("  Total Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("  Total Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("  Total Spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("  Firing Neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("  Average Firing Rate: " + std::to_string(getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("  E/I Ratio: " + std::to_string(getExcitatoryInhibitionRatio()));
    NLM_LOG_INFO("  Current Injection: " + std::to_string(getInjectedCurrent()) + " pA");
    NLM_LOG_INFO("  Learning Rate: " + std::to_string(plasticityModifier));
}

bool Brain::Impl::injectCurrent(NeuronId neuronId, CurrentInjection current) {
    if (!neuronId || current == 0.0f) return false;
    
    auto it = neuronById.find(neuronId);
    if (it != neuronById.end()) {
        Neuron* neuron = it->second;
        if (neuron) {
            neuron->injectCurrent(current);
            return true;
        }
    }
    return false;
}

float Brain::Impl::getInjectedCurrent() const {
    float totalCurrent = 0.0f;
    for (Neuron* neuron : sensoryNeurons) {
        if (neuron) {
            totalCurrent += neuron->getTotalCurrent();
        }
    }
    for (Neuron* neuron : motorNeurons) {
        if (neuron) {
            totalCurrent += neuron->getTotalCurrent();
        }
    }
    return totalCurrent;
}

void Brain::Impl::clearInjection() {
    // Clear all injected currents
    // In the actual implementation, this would involve more complex logic
}

const NeuralRegion* Brain::Impl::getRegion(RegionId regionId) const {
    auto it = std::find_if(regions.begin(), regions.end(),
        [regionId](const std::unique_ptr<NeuralRegion>& r) { return r->getId() == regionId; });
    return it != regions.end() ? it->get() : nullptr;
}

const NeuralPopulation* Brain::Impl::getPopulation(RegionId regionId, PopulationId populationId) const {
    const NeuralRegion* region = getRegion(regionId);
    if (!region) return nullptr;
    
    return region->getPopulation(populationId);
}

const Neuron* Brain::Impl::getNeuron(NeuronId neuronId) const {
    auto it = neuronById.find(neuronId);
    return it != neuronById.end() ? it->second : nullptr;
}

const Synapse* Brain::Impl::getSynapse(SynapseHandle synapseHandle) const {
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const auto& synapse : population->getSynapses()) {
                if (synapse->getHandle() == synapseHandle) {
                    return synapse.get();
                }
            }
        }
    }
    return nullptr;
}

std::vector<RegionId> Brain::Impl::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

size_t Brain::Impl::getTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getNeuronCount();
        }
    }
    return count;
}

size_t Brain::Impl::getTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            count += population->getSynapseCount();
        }
    }
    return count;
}

size_t Brain::Impl::getTotalSpikeCount() const {
    return totalSpikesTotal;
}

std::size_t Brain::Impl::getFiringNeuronCount() const {
    std::size_t count = 0;
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron && neuron->getFiringState() != FiringState::Resting) {
                    count++;
                }
            }
        }
    }
    return count;
}

float Brain::Impl::getAverageFiringRate() const {
    if (getTotalNeuronCount() == 0) return 0.0f;
    
    float totalRate = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    totalRate += neuron->getFiringRate();
                    neuronCount++;
                }
            }
        }
    }
    
    return totalRate / neuronCount;
}

float Brain::Impl::getExcitatoryInhibitionRatio() const {
    float excitatory = 0.0f, inhibitory = 0.0f;
    
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    const NeuralPopulation* pop = population.get();
                    if (pop->getNeuronType() == NeuronType::Excitatory) {
                        excitatory += neuron->getFiringRate();
                    } else if (pop->getNeuronType() == NeuronType::Inhibitory) {
                        inhibitory += neuron->getFiringRate();
                    }
                }
            }
        }
    }
    
    return inhibitory > 0.0f ? excitatory / inhibitory : 0.0f;
}

std::size_t Brain::Impl::getMostActiveNeuronCount() const {
    std::size_t count = 0;
    float maxRate = -1.0f;
    
    for (const auto& region : regions) {
        for (const auto& population : region->getPopulations()) {
            for (const Neuron* neuron : population->getNeurons()) {
                if (neuron) {
                    float rate = neuron->getFiringRate();
                    if (rate > maxRate) {
                        maxRate = rate;
                        count = 1;
                    } else if (std::abs(rate - maxRate) < 0.01f) {
                        count++;
                    }
                }
            }
        }
    }
    
    return count;
}

const PredictionSystem* Brain::Impl::getPredictionSystem() const {
    return predictionSystem.get();
}

const NeuralPlanner* Brain::Impl::getPlanner() const {
    return planner.get();
}

const ConceptFormation* Brain::Impl::getConceptFormation() const {
    return conceptFormation.get();
}

const NeuralWorkingMemory* Brain::Impl::getWorkingMemory() const {
    return workingMemory.get();
}

const NeuralEpisodicMemory* Brain::Impl::getEpisodicMemory() const {
    return episodicMemory.get();
}

const NeuralAssociativeMemory* Brain::Impl::getAssociativeMemory() const {
    return associativeMemory.get();
}

const DevelopmentSystem* Brain::Impl::getDevelopmentSystem() const {
    return developmentSystem.get();
}

const Neuromodulator* Brain::Impl::getDopamine() const {
    return dopamine.get();
}

const Curiosity* Brain::Impl::getCuriosity() const {
    return curiosity.get();
}

const Novelty* Brain::Impl::getNovelty() const {
    return novelty.get();
}

const PredictionError* Brain::Impl::getPredictionError() const {
    return predictionError.get();
}

const SpikeSystem* Brain::Impl::getSpikeSystem() const {
    return spikeSystem.get();
}

const STDP* Brain::Impl::getSTDP() const {
    return stdp.get();
}

const Hebbian* Brain::Impl::getHebbian() const {
    return hebbian.get();
}

const StructuralPlasticity* Brain::Impl::getStructuralPlasticity() const {
    return structuralPlasticity.get();
}

const RandomGenerator* Brain::Impl::getRandomGenerator() const {
    return rng.get();
}

const CheckpointManager* Brain::Impl::getCheckpointManager() const {
    return checkpointManager.get();
}

const AttentionSystem* Brain::Impl::getAttentionSystem() const {
    return attention.get();
}

void Brain::Impl::setLearnedPreference(float preference) {
    learnedPreference = preference;
}

float Brain::Impl::getLearnedPreference() const {
    return learnedPreference;
}

void Brain::Impl::setDevelopmentalStage(DevelopmentalStage stage) {
    developmentalStage = stage;
}

DevelopmentalStage Brain::Impl::getDevelopmentalStage() const {
    return developmentalStage;
}

} // namespace nlm