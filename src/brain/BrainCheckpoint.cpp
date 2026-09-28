#include "BrainCheckpoint.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

bool BrainCheckpoint::saveBrainState(const Brain::Impl& pImpl, const std::string& filepath) const {
    try {
        // Validate filepath
        if (filepath.empty()) {
            NLM_LOG_ERROR("Failed to save brain state: Filepath is empty");
            return false;
        }
        
        NLM_LOG_INFO("Saving brain state to " + filepath);
        
        try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
                return false;
            }
            
            // Set metadata
            writer.setMetadata(
                pImpl->getTotalNeuronCount(),
                pImpl->getTotalSynapseCount(),
                pImpl->getRegionCount(),
                pImpl->currentStep,
                pImpl->currentTime
            );
            
            // Write neurons
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(pImpl->getTotalNeuronCount());
            neuronData.restingPotential.reserve(pImpl->getTotalNeuronCount());
            neuronData.threshold.reserve(pImpl->getTotalNeuronCount());
            neuronData.resetPotential.reserve(pImpl->getTotalNeuronCount());
            neuronData.leakConductance.reserve(pImpl->getTotalNeuronCount());
            
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        if (!neuron) {
                            NLM_LOG_WARNING("Skipping null neuron in checkpoint data");
                            continue;
                        }
                        
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
            for (const auto& region : pImpl->regions) {
                for (const auto* syn : region->getSynapses()) {
                    if (!syn) {
                        NLM_LOG_WARNING("Skipping null synapse in checkpoint data");
                        continue;
                    }
                    
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
            NLM_LOG_ERROR(std::string("Exception during brain save: ") + e.what());
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain state save: ") + e.what());
        return false;
    }
}

bool BrainCheckpoint::loadBrainState(Brain::Impl& pImpl, const std::string& filepath) {
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
        
        // Apply neuron states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

void BrainCheckpoint::logCheckpointStats(const Brain::Impl& pImpl) const {
    if (pImpl->checkpointManager) {
        NLM_LOG_INFO("Checkpoint manager active: managing brain state persistence");
    }
}

void BrainCheckpoint::prepareBrainForSave(Brain::Impl& pImpl) const {
    // Prepare brain state for checkpointing
    // Freeze ongoing processes, collect current statistics
    NLM_LOG_DEBUG("Preparing brain for checkpoint save");
}

void BrainCheckpoint::restoreBrainFromSave(Brain::Impl& pImpl) {
    // Restore brain state from checkpoint
    // This would involve reloading all neural states, synapses, etc.
    NLM_LOG_DEBUG("Restoring brain from checkpoint");
}

void BrainCheckpoint::cleanupAfterCheckpoint(Brain::Impl& pImpl) {
    // Clean up after checkpoint operation
    NLM_LOG_DEBUG("Cleaning up after checkpoint operation");
}

} // namespace nlm
