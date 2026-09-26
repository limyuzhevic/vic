namespace nlm {

// Helper class for neuron stepping operations
class NeuronUpdater {
public:
    explicit NeuronUpdater(Impl& impl) : impl_(impl) {}
    
    void updateAllNeurons(Timestamp currentTime) {
        for (auto& region : impl_.regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, impl_.timestep);
                }
            }
        }
    }
    
    void injectCurrentToAllNeurons(float current) {
        for (auto& region : impl_.regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
    
private:
    Impl& impl_;
};

// Helper class for spike processing and handling
class SpikeHandler {
public:
    explicit SpikeHandler(Impl& impl) : impl_(impl) {}
    
    void detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : impl_.regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    bool justFired = (state.firingState == FiringState::Refractory &&
                                     state.lastSpikeTime >= 0.0f &&
                                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < impl_.timestep * 2.0f);

                    if (justFired) {
                        scheduleSpike(neuron, currentStep, currentTime, region);
                    }
                }
            }
        }
    }
    
    void processRegisteredSpikes(SimulationStep currentStep) {
        impl_.spikeSystem->processSpikes(currentStep);
    }
    
private:
    void scheduleSpike(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime, NeuralRegion& region) {
        // Count spike
        ++impl_.totalSpikesThisStep;
        ++impl_.totalSpikesTotal;
        
        // Queue the spike
        SpikeEvent event(neuron->getId(), currentTime, currentStep);
        impl_.spikeSystem->queueSpike(event);
        
        // Record post-synaptic spike for incoming synapses (plasticity)
        auto incomingSynapses = region.getSynapsesTo(neuron->getId());
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(currentTime);
        }
        
        // Get outgoing synapses and schedule delayed spike events
        auto outgoingSynapses = region.getSynapsesFrom(neuron->getId());
        for (Synapse* syn : outgoingSynapses) {
            // Create delayed spike event
            Delay delay = syn->getDelay();
            SimulationStep deliveryStep = currentStep + delay;
            Timestamp deliveryTime = currentTime + delay * impl_.timestep;

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

            impl_.spikeSystem->queueDelayedSpike(delayedEvent);
            
            // Record pre-synaptic spike for plasticity
            syn->recordPreSpike(currentTime);
        }
        
        // Store to working memory
        if (impl_.workingMemory) {
            const auto& state = neuron->getState();
            impl_.workingMemory->storeToNeuron(neuron->getId(), 
                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
        }
    }
    
    Impl& impl_;
};

// Helper class for memory system updates
class MemorySystemUpdater {
public:
    explicit MemorySystemUpdater(Impl& impl) : impl_(impl) {}
    
    void updateWorkingMemory() {
        if (impl_.workingMemory) {
            impl_.workingMemory->update(impl_.timestep);
        }
    }
    
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
        impl_.stepsSinceLastEpisode++;
        if (impl_.stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
            impl_.stepsSinceLastEpisode = 0;
            
            if (impl_.episodicMemory) {
                storeCurrentStateAsEpisode(currentStep, currentTime);
            }
        }
    }
    
    void replayMemories(SimulationStep currentStep) {
        if (currentStep % impl_.replayInterval == 0 && impl_.episodicMemory) {
            // Get episodes for replay
            auto episodesToReplay = impl_.episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                impl_.episodicMemory->replayEpisode(episode);
            }
        }
    }
    
    void consolidateMemories(SimulationStep currentStep) {
        if (currentStep % impl_.consolidationInterval == 0 && impl_.episodicMemory) {
            // Consolidate important memories, remove weak ones
            impl_.episodicMemory->consolidate(0.3f);
        }
    }
    
private:
    void storeCurrentStateAsEpisode(SimulationStep currentStep, Timestamp currentTime) {
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep;
        episode.reward = impl_.dopamine ? impl_.dopamine->getLevel() : 0.0f;
        
        // Store active neurons
        for (auto& region : impl_.regions) {
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
        
        // Store reward in episode
        episode.reward = impl_.dopamine ? impl_.dopamine->getLevel() : 0.0f;
        
        impl_.episodicMemory->storeEpisode(episode);
    }
    
    Impl& impl_;
};

// Helper class for neuromodulation effects
class NeuromodulationApplier {
public:
    explicit NeuromodulationApplier(Impl& impl) : impl_(impl) {}
    
    void updateAllNeuromodulators() {
        // Update novelty detection
        if (impl_.novelty) {
            impl_.novelty->update(impl_.timestep);
        }
        
        // Update curiosity
        if (impl_.curiosity) {
            impl_.curiosity->update(impl_.timestep);
        }
        
        // Update dopamine (reward prediction error)
        if (impl_.dopamine) {
            impl_.dopamine->update(impl_.timestep);
            
            // Apply dopamine effects on neural excitability
            float dopamineLevel = impl_.dopamine->getLevel();
            for (auto& region : impl_.regions) {
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
    
    float calculatePlasticityModulator() const {
        return impl_.dopamine ? impl_.dopamine->getPlasticityFactor() : 1.0f;
    }
    
    void applyNeuromodulationToPlasticity(STDP* stdp, Hebbian* hebbian, float plasticityMod) {
        if (stdp && hebbian) {
            // Scale STDP learning rates
            stdp->setLTPWeight(0.01f * plasticityMod);
            stdp->setLTDWeight(0.012f * plasticityMod);
        }
    }
    
    void applyNeuromodulationSignal(const class Neuromodulator& signal) {
        float modulation = signal.getLevel();
        
        // Scale STDP learning rates
        if (impl_.stdp) {
            impl_.stdp->setLTPWeight(0.01f * modulation);
            impl_.stdp->setLTDWeight(0.012f * modulation);
        }
    }
    
private:
    Impl& impl_;
};

// Helper class for plasticity updates
class PlasticityUpdater {
public:
    explicit PlasticityUpdater(Impl& impl) : impl_(impl) {}
    
    void applySTDP(SimulationStep currentStep, Timestamp currentTime, float plasticityMod) {
        for (auto& region : impl_.regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Apply STDP with neuromodulation
                        impl_.stdp->update(syn, preSpikes, postSpikes, impl_.timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
            }
        }
    }
    
    void applyHebbianLearning(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : impl_.regions) {
            for (auto& syn : region->getSynapses()) {
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();
                    
                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        impl_.hebbian->update(syn, preSpikes, postSpikes, impl_.timestep);
                    }
                }
            }
        }
    }
    
    void updateSynapseStates(SimulationStep currentStep, Timestamp currentTime) {
        for (auto& region : impl_.regions) {
            for (auto& syn : region->getSynapses()) {
                syn->step(currentTime);
            }
        }
    }
    
    void applyStructuralPlasticity(SimulationStep currentStep) {
        if (currentStep % 100 == 0) {
            impl_.structuralPlasticity->update(nullptr, *impl_.rng);
        }
    }
    
private:
    Impl& impl_;
};

// Helper class for development effects
class DevelopmentApplier {
public:
    explicit DevelopmentApplier(Impl& impl) : impl_(impl) {}
    
    void applyDevelopmentEffects(SimulationStep currentStep) {
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            impl_.developmentSystem->update(nullptr, *impl_.rng, impl_.timestep * 1000);
            
            // Development affects plasticity rates
            auto* sp = impl_.structuralPlasticity;
            if (sp) {
                DevelopmentalStage stage = impl_.developmentalStage;
                float plasticityMod = 1.0f;
                
                switch (stage) {
                    case DevelopmentalStage::Initial:
                        plasticityMod = 1.0f;  // High plasticity
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityMod = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityMod = 0.5f;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityMod = 0.2f;  // Stable
                        break;
                }
                
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
    
private:
    Impl& impl_;
};

// Helper class for cognition systems
class CognitionSystemUpdater {
public:
    explicit CognitionSystemUpdater(Impl& impl) : impl_(impl) {}
    
    void updateAttentionSystem() {
        if (impl_.attention) {
            impl_.attention->update(impl_.timestep);
            
            // Apply attention to working memory winners
            if (impl_.workingMemory && !impl_.workingMemory->getMemoryNeurons().empty()) {
                std::vector<NeuronId> competitors = impl_.workingMemory->getMemoryNeurons();
                impl_.attention->processCompetition(competitors);
            }
        }
    }
    
    void updateConceptFormation() {
        if (impl_.conceptFormation) {
            // Would process current neural activity patterns to form concepts
            // This requires sensory state encoding
        }
    }
    
    void updatePredictionSystem() {
        if (impl_.predictionSystem) {
            // The prediction system would be updated with sensory observations
            // For now, just track prediction error history
        }
    }
    
private:
    Impl& impl_;
};

// Helper class for checkpoint management
class CheckpointManagerUpdater {
public:
    explicit CheckpointManagerUpdater(Impl& impl) : impl_(impl) {}
    
    void updateCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
        if (impl_.checkpointManager) {
            impl_.checkpointManager->update(currentStep, currentTime);
        }
    }
    
private:
    Impl& impl_;
};

// Helper class for spike processing registration
class SpikeSystemInitializer {
public:
    explicit SpikeSystemInitializer(Impl& impl) : impl_(impl) {}
    
    void registerSpikeHandlers() {
        // Register spike handlers for event-driven processing
        impl_.spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++impl_.totalSpikesThisStep;
            ++impl_.totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        impl_.spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : impl_.regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        // Apply synaptic weight as current
                        MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                        if (event.is_excitatory) {
                            neuron->receiveExcitatoryInput(synapticCurrent);
                        } else {
                            neuron->receiveInhibitoryInput(-synapticCurrent);
                        }
                        return;
                    }
                }
            }
        });
    }
    
private:
    Impl& impl_;
};

} // namespace nlm
