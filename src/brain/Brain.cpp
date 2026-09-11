// Implementation of private methods for Brain::Impl
    void Brain::Impl::processDelayedSpikeEvents(SimulationStep currentStep, Timestamp currentTime) {
        spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }

    void Brain::Impl::updateAllNeurons(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, timestep);
                }
            }
        }
    }

    void Brain::Impl::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                    if (justFired) {
                        SpikeEvent event(neuron->getId(), currentTime, currentStep);
                        spikeSystem->queueSpike(event);

                        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                        for (Synapse* syn : incomingSynapses) {
                            syn->recordPostSpike(currentTime);
                        }

                        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                        for (Synapse* syn : outgoingSynapses) {
                            Delay delay = syn->getDelay();
                            SimulationStep deliveryStep = currentStep + delay;
                            Timestamp deliveryTime = currentTime + delay * timestep;

                            DelayedSpikeEvent delayedEvent(
                                neuron->getId(),
                                syn->getDestinationNeuron(),
                                syn->getId(),
                                syn->getWeight(),
                                syn->getType(),
                                currentTime,
                                deliveryTime,
                                currentStep,
                                deliveryStep
                            );

                            spikeSystem->queueDelayedSpike(delayedEvent);

                            syn->recordPreSpike(currentTime);
                        }
                        
                        if (workingMemory) {
                            workingMemory->storeToNeuron(neuron->getId(), 
                                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                        }
                    }
                }
            }
        }

        spikeSystem->processSpikes(currentStep);
    }

    void Brain::Impl::updateWorkingMemory() {
        if (workingMemory) {
            workingMemory->update(timestep);
        }
    }

    void Brain::Impl::applyNeuromodulationEffects() {
        if (novelty) {
            novelty->update(timestep);
        }
        if (curiosity) {
            curiosity->update(timestep);
        }
        if (dopamine) {
            dopamine->update(timestep);
            float dopamineLevel = dopamine->getLevel();
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        float excitabilityMod = dopamineLevel * 0.5f;
                        if (excitabilityMod > 0.0f) {
                            neuron->injectCurrent(excitabilityMod);
                        }
                    }
                }
            }
        }
    }

    void Brain::Impl::applyPlasticityRules() {
        float plasticityMod = 1.0f;
        if (dopamine) {
            plasticityMod = dopamine->getPlasticityFactor();
        }
        for (auto& region : regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        stdp->update(syn, preSpikes, postSpikes, timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        hebbian->update(syn, preSpikes, postSpikes, timestep);
                    }
                }
                syn->step(currentTime);
            }
        }
    }

    void Brain::Impl::updateEpisodicMemory(SimulationStep currentStep) {
        stepsSinceLastEpisode++;
        if (stepsSinceLastEpisode >= 10) {
            stepsSinceLastEpisode = 0;
            if (episodicMemory) {
                EpisodicMemoryItem episode;
                episode.timestamp = currentStep;
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                for (auto& region : regions) {
                    for (auto& pop : region->getPopulations()) {
                        for (auto* neuron : pop->getNeurons()) {
                            if (neuron->isFiring() || 
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                                episode.activeNeurons.push_back(neuron->getId());
                                episode.neuronActivations.push_back(
                                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                            }
                        }
                    }
                }
                episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
                episodicMemory->storeEpisode(episode);
            }
        }
    }

    void Brain::Impl::updatePredictionSystem() {
        if (predictionSystem) {
        }
    }

    void Brain::Impl::updateAttentionSystem() {
        if (attention) {
            attention->update(timestep);
            if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
                attention->processCompetition(competitors);
            }
        }
    }

    void Brain::Impl::updateConceptFormation() {
        if (conceptFormation) {
        }
    }

    void Brain::Impl::applyStructuralPlasticity(SimulationStep currentStep) {
        if (currentStep % 100 == 0) {
            structuralPlasticity->update(nullptr, *rng);
        }
    }

    void Brain::Impl::replayMemories(SimulationStep currentStep) {
        if (currentStep % replayInterval == 0 && episodicMemory) {
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }

    void Brain::Impl::applyDevelopmentEffects(SimulationStep currentStep) {
        if (currentStep % 1000 == 0) {
            developmentSystem->update(nullptr, *rng, timestep * 1000);
            if (structuralPlasticity) {
                DevelopmentalStage stage = developmentalStage;
                float plasticityMod = 1.0f;
                switch (stage) {
                    case DevelopmentalStage::Initial:
                        plasticityMod = 1.0f;
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityMod = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityMod = 0.5f;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityMod = 0.2f;
                        break;
                }
                structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
                structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }

    void Brain::Impl::consolidateMemories(SimulationStep currentStep) {
        if (currentStep % consolidationInterval == 0 && episodicMemory) {
            episodicMemory->consolidate(0.3f);
        }
    }

    void Brain::Impl::updateCheckpoint(SimulationStep currentStep, Timestamp currentTime) {
        if (checkpointManager) {
            checkpointManager->update(currentStep, currentTime);
        }
    }

    void Brain::Impl::collectStatistics() {
        totalSpikesThisStep = 0;
        for (auto& region : regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuron->isFiring()) {
                        totalSpikesThisStep++;
                    }
                }
            }
        }
    }