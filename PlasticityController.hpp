// Plasticity Controller - handles STDP, Hebbian, structural plasticity
class PlasticityController {
public:
    PlasticityController(Brain* brain,
                       SpikeSystem* spikeSystem,
                       STDP* stdp,
                       Hebbian* hebbian,
                       StructuralPlasticity* structuralPlasticity)
        : brain(brain)
        , spikeSystem(spikeSystem)
        , stdp(stdp)
        , hebbian(hebbian)
        , structuralPlasticity(structuralPlasticity)
        , plasticityMod(1.0f) {}

    bool initialize() {
        // Register spike handlers for event-driven processing
        spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++brain->pImpl->totalSpikesThisStep;
            ++brain->pImpl->totalSpikesTotal;
        });

        // Register delayed spike handler to deliver synaptic input
        spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : brain->pImpl->regions) {
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

        return true;
    }

    void step(TimestepDuration timestep, Timestamp currentTime, NeuromodulationController* neuromodulationController) {
        // Update neuromodulation effects for plasticity
        neuromodulationController->update(this);

        // Step 6: Apply plasticity rules (STDP and Hebbian)
        for (auto& region : brain->pImpl->regions) {
            for (auto& syn : region->getSynapses()) {
                // Apply STDP with neuromodulation
                if (syn->getPlasticityFlags().stdp) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();

                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        // Modify weight change based on dopamine
                        stdp->update(syn, preSpikes, postSpikes, timestep);
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }

                // Apply Hebbian learning
                if (syn->getPlasticityFlags().hebbian) {
                    const auto& preSpikes = syn->getPreSpikeHistory();
                    const auto& postSpikes = syn->getPostSpikeHistory();

                    if (!preSpikes.empty() && !postSpikes.empty()) {
                        hebbian->update(syn, preSpikes, postSpikes, timestep);
                    }
                }

                // Update synapse state
                syn->step(currentTime);
            }
        }

        // Step 11: Apply structural plasticity periodically
        if (currentTime % 100 == 0) {  // Use timestep percentage
            structuralPlasticity->update(brain, *brain->pImpl->rng);
        }
    }

    void updateDevelopmentStage(DevelopmentalStage stage) {
        // Development affects plasticity rates
        if (!structuralPlasticity) return;

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

        setPlasticityModulator(plasticityMod);
        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }

    void setPlasticityModulator(float mod) { plasticityMod = mod; }

    SpikeSystem* getSpikeSystem() { return spikeSystem; }
    STDP* getSTDP() { return stdp; }
    Hebbian* getHebbian() { return hebbian; }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity; }

private:
    Brain* brain;
    SpikeSystem* spikeSystem;
    STDP* stdp;
    Hebbian* hebbian;
    StructuralPlasticity* structuralPlasticity;
    float plasticityMod;
};

// Development Controller - handles developmental stages
class DevelopmentController {
public:
    DevelopmentController(Brain* brain,
                         DevelopmentSystem* developmentSystem,
                         PlasticityController* plasticityController)
        : brain(brain)
        , developmentSystem(developmentSystem)
        , plasticityController(plasticityController) {}

    void step(Timestamp currentTime, DevelopmentalStage& developmentalStage) {
        // Step 13: Apply development effects
        if (currentTime % 1000 == 0) {  // Update development every 1000 steps
            developmentSystem->update(brain, *brain->pImpl->rng, 1000.0f);  // timestep * 1000

            // Development affects plasticity rates
            if (plasticityController) {
                plasticityController->updateDevelopmentStage(developmentalStage);
            }
        }
    }

    void setDevelopmentalStage(DevelopmentalStage stage) {
        developmentalStage = stage;
    }

    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem; }

private:
    Brain* brain;
    DevelopmentSystem* developmentSystem;
    PlasticityController* plasticityController;
    DevelopmentalStage developmentalStage;
};

// Checkpoint Controller - handles checkpoint management
class CheckpointController {
public:
    CheckpointController(Brain* brain, CheckpointManager* checkpointManager)
        : brain(brain)
        , checkpointManager(checkpointManager) {}

    void step(Timestamp currentTime, SimulationStep currentStep) {
        // Step 15: Checkpoint management
        if (checkpointManager) {
            checkpointManager->update(currentStep, currentTime);
        }
    }

    bool save(const std::string& filepath) const {
        try {
            CheckpointWriter writer;
            if (!writer.create(filepath, CompressionLevel::Balanced)) {
                return false;
            }

            // Set metadata
            writer.setMetadata(
                brain->getTotalNeuronCount(),
                brain->getTotalSynapseCount(),
                brain->getRegionCount(),
                brain->pImpl->currentStep,
                brain->pImpl->currentTime
            );

            // Write neurons
            NeuronCheckpointData neuronData;
            neuronData.membranePotential.reserve(brain->getTotalNeuronCount());
            neuronData.restingPotential.reserve(brain->getTotalNeuronCount());
            neuronData.threshold.reserve(brain->getTotalNeuronCount());
            neuronData.resetPotential.reserve(brain->getTotalNeuronCount());
            neuronData.leakConductance.reserve(brain->getTotalNeuronCount());

            for (const auto& region : brain->pImpl->regions) {
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
                    }
                }
            }

            if (!writer.writeNeurons(neuronData)) {
                return false;
            }

            // Write synapses
            SynapseCheckpointData synapseData;
            for (const auto& region : brain->pImpl->regions) {
                for (const auto* syn : region->getSynapses()) {
                    synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                    synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                    synapseData.weight.push_back(syn->getWeight());
                    synapseData.delay.push_back(syn->getDelay());
                    synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                    synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                }
            }

            if (!writer.writeSynapses(synapseData)) {
                return false;
            }

            // Finalize
            if (!writer.finalize()) {
                return false;
            }

            return true;

        } catch (const std::exception& e) {
            return false;
        }
    }

    bool load(const std::string& filepath) {
        try {
            CheckpointReader reader;
            if (!reader.open(filepath)) {
                return false;
            }

            if (!reader.validate()) {
                return false;
            }

            // Read neurons
            NeuronCheckpointData neuronData;
            if (!reader.readNeurons(neuronData)) {
                return false;
            }

            // Apply neuron states
            size_t idx = 0;
            for (auto& region : brain->pImpl->regions) {
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
                return false;
            }

            // Apply synapse states - this is complex because we need to find matching synapses
            // For now, just log the count

            return true;

        } catch (const std::exception& e) {
            return false;
        }
    }

    void configure(const std::string& checkpointDir, size_t maxCheckpoints, size_t maxCheckpointSize, bool compression) {
        if (checkpointManager) {
            checkpointManager->configure(checkpointDir, maxCheckpoints, maxCheckpointSize, compression);
        }
    }

private:
    Brain* brain;
    CheckpointManager* checkpointManager;
};

// Replay Controller - handles memory replay
class ReplayController {
public:
    ReplayController(Brain* brain, NeuralEpisodicMemory* episodicMemory) : brain(brain), episodicMemory(episodicMemory) {}

    void step(SimulationStep currentStep) {
        // Step 12: Replay important memories
        if (episodicMemory && currentStep % 100 == 0) {  // Replay every 100 steps
            // Get episodes for replay
            auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMemory->replayEpisode(episode);
            }
        }
    }

    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory; }

private:
    Brain* brain;
    NeuralEpisodicMemory* episodicMemory;
};