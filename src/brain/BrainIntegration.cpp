#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

BrainIntegration::BrainIntegration(Brain::Impl* pImpl) : pImpl(pImpl) {}

void BrainIntegration::performNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void BrainIntegration::performActionProduction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        // Return a simple action
        // This would be handled by the Action class
        return;
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    // This would be handled by the Action class
    if (firingMotor > 0) {
        // Action would be MoveForward
    }
}

void BrainIntegration::performPlasticityUpdate() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainIntegration::performDevelopment() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(pImpl, *pImpl->rng);
}

void BrainIntegration::performReset() {
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
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
}

void BrainIntegration::performCheckpointSave(const std::string& filepath) const {
    try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CheckpointManager::CompressionLevel::Standard)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return;
            }
            
            // Set metadata
            writer.setMetadata(
                pImpl->statistics->performTotalNeuronCount(),
                pImpl->statistics->performTotalSynapseCount(),
                pImpl->regions.size(),
                pImpl->currentStep,
                pImpl->currentTime
            );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(performTotalNeuronCount());
        neuronData.restingPotential.reserve(performTotalNeuronCount());
        neuronData.threshold.reserve(performTotalNeuronCount());
        neuronData.resetPotential.reserve(performTotalNeuronCount());
        neuronData.leakConductance.reserve(performTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
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
                    neuronData.firingRate.push_back(state.firingRate);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto& syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return;
        }
        
        // Pending delayed spikes are in-flight synaptic input; a reload
        // without them drops every spike scheduled for the next 1-5
        // steps. Save them alongside neurons and synapses.
        if (pImpl->spikeSystem) {
            auto pending = pImpl->spikeSystem->getPendingDelayedSpikes();
            if (!writer.writeDelayedSpikes(pending)) {
                NLM_LOG_ERROR("Failed to write pending spikes to checkpoint");
                return;
            }
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
    }
}

void BrainIntegration::performCheckpointLoad(const std::string& filepath) {
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (idx < neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(neuronData.membranePotential[idx]);
                        neuron->setRestingPotential(neuronData.restingPotential[idx]);
                        neuron->setThreshold(neuronData.threshold[idx]);
                        neuron->setResetPotential(neuronData.resetPotential[idx]);
                        neuron->setLeakConductance(neuronData.leakConductance[idx]);
                        auto& state = neuron->getState();
                        if (idx < neuronData.firingState.size()) {
                            state.firingState = static_cast<FiringState>(neuronData.firingState[idx]);
                        }
                        if (idx < neuronData.refractoryRemaining.size()) {
                            state.refractoryRemaining = neuronData.refractoryRemaining[idx];
                        }
                        if (idx < neuronData.refractoryPeriod.size()) {
                            state.refractoryPeriod = neuronData.refractoryPeriod[idx];
                        }
                        if (idx < neuronData.lastSpikeTime.size()) {
                            state.lastSpikeTime = neuronData.lastSpikeTime[idx];
                        }
                        if (idx < neuronData.firingRate.size()) {
                            state.firingRate = neuronData.firingRate[idx];
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
            return;
        }
        
        // Apply synapse states
        {
            std::unordered_map<uint64_t, std::vector<Synapse*>> byPair;
            auto pairKey = [](uint64_t src, uint64_t dst) {
                return (src << 32) ^ dst;
            };
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    byPair[pairKey(syn->getSourceNeuron().index(),
                                   syn->getDestinationNeuron().index())].push_back(syn.get());
                }
            }
            std::unordered_map<uint64_t, size_t> usedCount;
            size_t restored = 0;
            for (size_t i = 0; i < synapseData.weight.size(); ++i) {
                uint64_t key = pairKey(synapseData.sourceNeuron[i],
                                       synapseData.destinationNeuron[i]);
                auto it = byPair.find(key);
                if (it != byPair.end()) {
                    // Restore matching synapse
                    std::vector<Synapse*>& matches = it->second;
                    if (usedCount[key] < matches.size()) {
                        Synapse* syn = matches[usedCount[key]++];
                        syn->setWeight(synapseData.weight[i]);
                        syn->setDelay(synapseData.delay[i]);
                        syn->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                        syn->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                        syn->setEfficacy(synapseData.efficacy[i]);
                        restored++;
                    }
                }
            }
            NLM_LOG_INFO("Restored " + std::to_string(restored) + " synapses from checkpoint");
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
    }
}

} // namespace nlm