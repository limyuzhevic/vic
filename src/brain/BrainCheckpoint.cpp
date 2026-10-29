#include "BrainHeaders.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

void BrainCheckpoint::configureCheckpointSystem(Brain* brain) {
    if (!brain) return;
    auto* checkpointManager = brain->pImpl->checkpointManager.get();
    if (checkpointManager) {
        std::string checkpointDir = brain->pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
}

bool BrainCheckpoint::saveBrainState(Brain* brain, const std::string& filepath) const {
    if (!brain) return false;
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot save brain state: empty file path");
        return false;
    }
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Create brain state object
    BrainState state;
    state.timestamp = std::chrono::system_clock::now();
    state.step = brain->pImpl->currentStep;
    state.time = brain->pImpl->currentTime;
    
    // Capture neural state
    for (auto& region : brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                NeuralState neuralState;
                neuralState.id = neuron->getId();
                neuralState.type = neuron->getType();
                neuralState.regionId = region->getId();
                neuralState.populationId = pop->getId();
                neuralState.membranePotential = neuron->getState().membranePotential;
                neuralState.restingPotential = neuron->getState().restingPotential;
                neuralState.firingState = neuron->getState().firingState;
                neuralState.lastSpikeTime = neuron->getState().lastSpikeTime;
                neuralState.activationLevel = neuron->getActivationLevel();
                state.neuralStates.push_back(neuralState);
            }
        }
    }
    
    // Capture synaptic state
    for (auto& region : brain->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            SynapseState synState;
            synState.id = syn->getId();
            synState.sourceNeuron = syn->getSourceNeuron();
            synState.destinationNeuron = syn->getDestinationNeuron();
            synState.weight = syn->getWeight();
            synState.type = syn->getType();
            synState.delay = syn->getDelay();
            synState.plasticityFlags = syn->getPlasticityFlags();
            synState.plastic = syn->isPlastic();
            synState.lastPreSpike = syn->getLastPreSpike();
            synState.lastPostSpike = syn->getLastPostSpike();
            state.synapseStates.push_back(synState);
        }
    }
    
    // Serialize to binary format
    file.write(reinterpret_cast<const char*>(&state), sizeof(BrainState));
    
    file.close();
    
    NLM_LOG_INFO("Brain state saved to: " + filepath);
    return true;
}

bool BrainCheckpoint::loadBrainState(Brain* brain, const std::string& filepath) {
    if (!brain) return false;
    
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot load brain state: empty file path");
        return false;
    }
    
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
        return false;
    }
    
    BrainState state;
    file.read(reinterpret_cast<char*>(&state), sizeof(BrainState));
    
    // Restore neural state
    brain->reset();
    
    for (const auto& neuralState : state.neuralStates) {
        // Find neuron and restore state
        for (auto& region : brain->pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->getId() == neuralState.id) {
                        // Restore neuron state
                        neuron->setMembranePotential(neuralState.membranePotential);
                        neuron->setRestingPotential(neuralState.restingPotential);
                        neuron->setFiringState(neuralState.firingState);
                        neuron->setLastSpikeTime(neuralState.lastSpikeTime);
                        neuron->setActivationLevel(neuralState.activationLevel);
                        break;
                    }
                }
            }
        }
    }
    
    // Restore synaptic state
    for (const auto& synState : state.synapseStates) {
        // Find synapse and restore state
        for (auto& region : brain->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getId() == synState.id) {
                    syn->setWeight(synState.weight);
                    syn->setPlasticityFlags(synState.plasticityFlags);
                    syn->setPlastic(synState.plastic);
                    syn->setLastPreSpike(synState.lastPreSpike);
                    syn->setLastPostSpike(synState.lastPostSpike);
                    break;
                }
            }
        }
    }
    
    file.close();
    
    NLM_LOG_INFO("Brain state loaded from: " + filepath);
    return true;
}

void BrainCheckpoint::manageCheckpoints(Brain* brain) {
    if (!brain) return;
    auto* checkpointManager = brain->pImpl->checkpointManager.get();
    if (checkpointManager) {
        checkpointManager->update();
        checkpointManager->cleanupOldCheckpoints();
    }
}

void BrainCheckpoint::cleanupOldCheckpoints(Brain* brain) {
    if (!brain) return;
    auto* checkpointManager = brain->pImpl->checkpointManager.get();
    if (checkpointManager) {
        checkpointManager->cleanupOldCheckpoints();
    }
}

} // namespace nlm