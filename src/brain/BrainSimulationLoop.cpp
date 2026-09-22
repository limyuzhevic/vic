#include "BrainSimulationLoop.hpp"
#include "Brain.hpp"

namespace nlm {

void processDelayedSpikeEvents(Brain& brain) {
    brain.pImpl->spikeSystem->processDelayedSpikes(brain.pImpl->currentStep, brain.pImpl->currentTime);
}

void updateNeuronDynamics(Brain& brain) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(brain.pImpl->currentTime, brain.pImpl->timestep);
            }
        }
    }
}

void detectSpikesAndScheduleEvents(Brain& brain) {
    for (auto& region : brain.pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(brain.pImpl->currentTime) - state.lastSpikeTime) < brain.pImpl->timestep * 2.0f);

                if (justFired) {
                    SpikeEvent event(neuron->getId(), brain.pImpl->currentTime, brain.pImpl->currentStep);
                    brain.pImpl->spikeSystem->queueSpike(event);

                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(brain.pImpl->currentTime);
                    }

                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = brain.pImpl->currentStep + delay;
                        Timestamp deliveryTime = brain.pImpl->currentTime + delay * brain.pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            brain.pImpl->currentTime,
                            deliveryTime,
                            brain.pImpl->currentStep,
                            deliveryStep
                        );

                        brain.pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        syn->recordPreSpike(brain.pImpl->currentTime);
                    }

                    if (brain.pImpl->workingMemory) {
                        brain.pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }

    brain.pImpl->spikeSystem->processSpikes(brain.pImpl->currentStep);
}

void updateWorkingMemory(Brain& brain) {
    if (brain.pImpl->workingMemory) {
        brain.pImpl->workingMemory->update(brain.pImpl->timestep);
    }
}

void applyNeuromodulationEffects(Brain& brain) {
    if (brain.pImpl->novelty) {
        brain.pImpl->novelty->update(brain.pImpl->timestep);
    }

    if (brain.pImpl->curiosity) {
        brain.pImpl->curiosity->update(brain.pImpl->timestep);
    }

    if (brain.pImpl->dopamine) {
        brain.pImpl->dopamine->update(brain.pImpl->timestep);

        float dopamineLevel = brain.pImpl->dopamine->getLevel();
        for (auto& region : brain.pImpl->regions) {
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

void applyPlasticityRules(Brain& brain) {
    float plasticityMod = 1.0f;
    if (brain.pImpl->dopamine) {
        plasticityMod = brain.pImpl->dopamine->getPlasticityFactor();
    }

    for (auto& region : brain.pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();

                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brain.pImpl->stdp->update(syn, preSpikes, postSpikes, brain.pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }

            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();

                if (!preSpikes.empty() && !postSpikes.empty()) {
                    brain.pImpl->hebbian->update(syn, preSpikes, postSpikes, brain.pImpl->timestep);
                }
            }

            syn->step(brain.pImpl->currentTime);
        }
    }
}

void updateEpisodicMemory(Brain& brain) {
    brain.pImpl->stepsSinceLastEpisode++;
    if (brain.pImpl->stepsSinceLastEpisode >= 10) {
        brain.pImpl->stepsSinceLastEpisode = 0;

        if (brain.pImpl->episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = brain.pImpl->currentStep;
            episode.reward = brain.pImpl->dopamine ? brain.pImpl->dopamine->getLevel() : 0.0f;

            for (auto& region : brain.pImpl->regions) {
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

            episode.reward = brain.pImpl->dopamine ? brain.pImpl->dopamine->getLevel() : 0.0f;
            brain.pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void updatePredictionSystem(Brain& brain) {
    if (brain.pImpl->predictionSystem) {
    }
}

void updateAttentionSystem(Brain& brain) {
    if (brain.pImpl->attention) {
        brain.pImpl->attention->update(brain.pImpl->timestep);

        if (brain.pImpl->workingMemory && !brain.pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain.pImpl->workingMemory->getMemoryNeurons();
            brain.pImpl->attention->processCompetition(competitors);
        }
    }
}

void updateConceptFormation(Brain& brain) {
    if (brain.pImpl->conceptFormation) {
    }
}

void applyStructuralPlasticity(Brain& brain) {
    if (brain.pImpl->currentStep % 100 == 0) {
        brain.pImpl->structuralPlasticity->update(&brain, *brain.pImpl->rng);
    }
}

void replayImportantMemories(Brain& brain) {
    if (brain.pImpl->currentStep % brain.pImpl->replayInterval == 0 && brain.pImpl->episodicMemory) {
        auto episodesToReplay = brain.pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            brain.pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void applyDevelopmentEffects(Brain& brain) {
    if (brain.pImpl->currentStep % 1000 == 0) {
        brain.pImpl->developmentSystem->update(&brain, *brain.pImpl->rng, brain.pImpl->timestep * 1000);

        auto* sp = brain.pImpl->structuralPlasticity;
        if (sp) {
            float plasticityMod = 1.0f;
            switch (brain.pImpl->developmentalStage) {
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

            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void performMemoryConsolidation(Brain& brain) {
    if (brain.pImpl->currentStep % brain.pImpl->consolidationInterval == 0 && brain.pImpl->episodicMemory) {
        brain.pImpl->episodicMemory->consolidate(0.3f);
    }
}

void manageCheckpoints(Brain& brain) {
    if (brain.pImpl->checkpointManager) {
        brain.pImpl->checkpointManager->update(brain.pImpl->currentStep, brain.pImpl->currentTime);
    }
}

} // namespace nlm