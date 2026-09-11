bool Brain::load(const std::string& filepath) {
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
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
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
        // Match synapses by source/destination neuron IDs and update their properties
        size_t matchedCount = 0;
        for (size_t i = 0; i < synapseData.weight.size(); ++i) {
            uint64_t srcId = synapseData.sourceNeuron[i];
            uint64_t dstId = synapseData.destinationNeuron[i];
            
            // Find matching synapse in the brain regions
            bool found = false;
            for (auto& region : pImpl->regions) {
                for (auto& syn : region->getSynapses()) {
                    if (syn->getSourceNeuron().value == srcId && 
                        syn->getDestinationNeuron().value == dstId) {
                        // Found matching synapse - update its state
                        syn->setWeight(synapseData.weight[i]);
                        syn->setDelay(synapseData.delay[i]);
                        syn->setType(static_cast<SynapseType>(synapseData.synapseType[i]));
                        
                        // Update plasticity flags
                        auto& flags = syn->getPlasticityFlags();
                        flags.hebbian = (synapseData.plasticityFlags[i] & 1) != 0;
                        flags.stdp = (synapseData.plasticityFlags[i] & 2) != 0;
                        flags.reward_modulated = (synapseData.plasticityFlags[i] & 4) != 0;
                        
                        // Update short-term plasticity state
                        syn->setEligibilityTrace(synapseData.eligibilityTrace[i]);
                        
                        matchedCount++;
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
        }
        
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses (matched " + 
                    std::to_string(matchedCount) + ")");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}