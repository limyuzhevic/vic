}  // spikeHandlersRegistered
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
    // Set up checkpoint providers for the checkpoint manager
    // These providers capture the Brain instance and provide data for checkpointing
    pImpl->checkpointManager->setNeuronProvider([this]() -> bool {
        NeuronCheckpointData data;
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    data.membranePotential.push_back(state.membranePotential);
                    data.restingPotential.push_back(state.restingPotential);
                    data.threshold.push_back(state.threshold);
                    data.resetPotential.push_back(state.resetPotential);
                    data.leakConductance.push_back(state.leakConductance);
                    data.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    data.refractoryRemaining.push_back(state.refractoryRemaining);
                    data.refractoryPeriod.push_back(state.refractoryPeriod);
                    data.lastSpikeTime.push_back(state.lastSpikeTime);
                    data.firingRate.push_back(state.firingRate);
                    data.neuronType.push_back(static_cast<uint64_t>(neuron->getType()));
                    data.regionId.push_back(region->getId().value);
                    data.populationId.push_back(pop->getId().value);
                }
            }
        }
        
        return true;
    });
    
    pImpl->checkpointManager->setSynapseProvider([this]() -> bool {
        SynapseCheckpointData data;
        
        for (const auto& region : pImpl->regions) {
            for (const auto& syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.plasticityFlags.push_back(syn->getPlasticityFlags());
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                data.efficacy.push_back(syn->getEfficacy());
                data.shortTermDepression.push_back(syn->getShortTermDepression());
                data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        return true;
    });
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}