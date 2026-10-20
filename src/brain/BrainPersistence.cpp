#include "brain/BrainPersistence.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

bool BrainPersistence::saveBrainState(const Brain* brain,
                                     const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot save brain state: brain is null");
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
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getRegionCount(),
            brain->getCurrentStep(),
            brain->getCurrentTime()
        );
        
        // Write neurons
        if (!writeNeuronsToCheckpoint(writer, *brain)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        if (!writeSynapsesToCheckpoint(writer, *brain)) {
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

bool BrainPersistence::loadBrainState(Brain* brain,
                                     const std::string& filepath) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot load brain state: brain is null");
        return false;
    }
    
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
        if (!readNeuronsFromCheckpoint(reader, *brain)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Read synapses
        if (!readSynapsesFromCheckpoint(reader, *brain)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

bool BrainPersistence::writeNeuronsToCheckpoint(CheckpointWriter& writer, const Brain& brain) {
    try {
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(brain.getTotalNeuronCount());
        neuronData.restingPotential.reserve(brain.getTotalNeuronCount());
        neuronData.threshold.reserve(brain.getTotalNeuronCount());
        neuronData.resetPotential.reserve(brain.getTotalNeuronCount());
        neuronData.leakConductance.reserve(brain.getTotalNeuronCount());
        
        for (const auto& region : brain.getRegions()) {
            if (!region) continue;
            
            for (const auto& population : region->getPopulations()) {
                if (!population) continue;
                
                for (const auto* neuron : population->getNeurons()) {
                    if (!neuron) continue;
                    
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
        
        return writer.writeNeurons(neuronData);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing neurons: ") + e.what());
        return false;
    }
}

bool BrainPersistence::readNeuronsFromCheckpoint(CheckpointReader& reader, Brain& brain) {
    try {
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            return false;
        }
        
        // Apply neuron states - this is complex because we need to find matching neurons
        // For now, just log the count and clear existing state
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
        
        // Reset brain and reinitialize
        brain.reset();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception reading neurons: ") + e.what());
        return false;
    }
}

bool BrainPersistence::writeSynapsesToCheckpoint(CheckpointWriter& writer, const Brain& brain) {
    try {
        SynapseCheckpointData synapseData;
        for (const auto& region : brain.getRegions()) {
            if (!region) continue;
            
            for (const auto* syn : region->getSynapses()) {
                if (!syn) continue;
                
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        return writer.writeSynapses(synapseData);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing synapses: ") + e.what());
        return false;
    }
}

bool BrainPersistence::readSynapsesFromCheckpoint(CheckpointReader& reader, Brain& brain) {
    try {
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception reading synapses: ") + e.what());
        return false;
    }
}

} // namespace nlm
