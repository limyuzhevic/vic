// Code Quality Improvements for Brain.cpp

// Simplify plasticity application (around lines 515-545):

// BEFORE: Complex nested conditionals
void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // ... initialization code ...
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // AFTER: Simplified using helper functions
    
    applyPlasticityToAllSynapses(pImpl->regions, plasticityMod);
}

// Create helper functions for code organization:
void applyPlasticityToAllSynapses(std::vector<std::unique_ptr<NeuralRegion>>& regions, float plasticityMod) {
    for (auto& region : regions) {
        for (auto& syn : region->getSynapses()) {
            applySynapticPlasticity(syn, plasticityMod);
        }
    }
}

void applySynapticPlasticity(std::unique_ptr<Synapse>& syn, float plasticityMod) {
    if (!syn) return;
    
    if (syn->getPlasticityFlags().stdp) {
        applySTDPPlasticity(syn, plasticityMod);
    }
    
    if (syn->getPlasticityFlags().hebbian) {
        applyHebbianPlasticity(syn);
    }
    
    syn->step(currentTime); // Current time needed here
}

void applySTDPPlasticity(std::unique_ptr<Synapse>& syn, float plasticityMod) {
    if (!syn) return;
    
    const auto& preSpikes = syn->getPreSpikeHistory();
    const auto& postSpikes = syn->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    pImpl->stdp->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
    float weight = syn->getWeight();
    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
    syn->setWeight(weight);
}

void applyHebbianPlasticity(std::unique_ptr<Synapse>& syn) {
    if (!syn) return;
    
    const auto& preSpikes = syn->getPreSpikeHistory();
    const auto& postSpikes = syn->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) return;
    
    pImpl->hebbian->update(syn.get(), preSpikes, postSpikes, pImpl->timestep);
}

// Simplify episodic memory update (around lines 583-613):

void updateEpisodicMemory() {
    // Increment counter and check if we should store an episode
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode < 10) return; // Early exit
    
    pImpl->stepsSinceLastEpisode = 0;
    
    if (!pImpl->episodicMemory) return;
    
    // Create and populate episode
    EpisodicMemoryItem episode = createCurrentEpisode();
    pImpl->episodicMemory->storeEpisode(episode);
}

EpisodicMemoryItem createCurrentEpisode() {
    EpisodicMemoryItem episode;
    episode.timestamp = pImpl->currentStep;
    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    
    // Extract firing neurons from all regions
    extractFiringNeuronsToEpisode(episode);
    
    // Calculate activations
    calculateNeuronActivations(episode);
    
    // Store reward again (redundant but maintained for consistency)
    episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
    
    return episode;
}

void extractFiringNeuronsToEpisode(EpisodicMemoryItem& episode) {
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (isNeuronActiveForEpisode(neuron)) {
                    episode.activeNeurons.push_back(neuron->getId());
                }
            }
        }
    }
}

bool isNeuronActiveForEpisode(const Neuron* neuron) {
    if (!neuron) return false;
    
    // Check if neuron is firing or has significant activation
    if (neuron->isFiring()) return true;
    
    const auto& state = neuron->getState();
    return std::abs(state.membranePotential - state.restingPotential) > 5.0f;
}

void calculateNeuronActivations(EpisodicMemoryItem& episode) {
    for (size_t i = 0; i < episode.activeNeurons.size(); ++i) {
        const auto* neuron = findNeuronById(episode.activeNeurons[i]);
        if (neuron) {
            const auto& state = neuron->getState();
            float activation = std::abs(state.membranePotential - state.restingPotential) / 20.0f;
            episode.neuronActivations.push_back(activation);
        }
    }
}

Neuron* findNeuronById(NeuronId id) {
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->getId() == id) {
                    return neuron;
                }
            }
        }
    }
    return nullptr;
}

// Simplify homeostatic synaptic scaling (around lines 562-580):

void applyHomeostaticSynapticScaling() {
    if (currentStep <= 0 || currentStep % 100 != 0) return;
    
    constexpr float kIncomingExcitatoryCap = 3.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto* neuron : region->getAllNeurons()) {
            float totalExcitatoryWeight = calculateTotalExcitatoryInput( neuron);
            if (totalExcitatoryWeight > kIncomingExcitatoryCap) {
                float scale = kIncomingExcitatoryCap / totalExcitatoryWeight;
                scaleExcitatoryWeightsToNeuron(neuron, scale);
            }
        }
    }
}

float calculateTotalExcitatoryInput(const Neuron* neuron) {
    if (!neuron) return 0.0f;
    
    auto* region = findRegionByNeuron(neuron);
    if (!region) return 0.0f;
    
    float sum = 0.0f;
    auto incoming = region->getSynapsesTo(neuron->getId());
    for (const auto* syn : incoming) {
        if (syn && syn->getWeight() > 0.0f) {
            sum += syn->getWeight();
        }
    }
    return sum;
}

void scaleExcitatoryWeightsToNeuron(const Neuron* neuron, float scale) {
    if (!neuron) return;
    
    auto* region = findRegionByNeuron(neuron);
    if (!region) return;
    
    auto incoming = region->getSynapsesTo(neuron->getId());
    for (auto* syn : incoming) {
        if (syn && syn->getWeight() > 0.0f) {
            float newWeight = syn->getWeight() * scale;
            syn->setWeight(newWeight);
        }
    }
}

// Simplify neuromodulation application (around lines 476-507):

void applyNeuromodulation() {
    // Apply all neuromodulators
    applyNoveltyModulation();
    applyCuriosityModulation();
    applyDopamineModulation();
    applyAChModulation();
    applyNEModulation();
    applySerotoninModulation();
}

void applyNoveltyModulation() {
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
}

void applyCuriosityModulation() {
    if (pImpl->curiosity) {
        float noveltyLevel = pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
        float predictionError = pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f;
        pImpl->curiosity->update(noveltyLevel, predictionError, pImpl->timestep);
    }
}

void applyDopamineModulation() {
    if (!pImpl->dopamine) return;
    
    pImpl->dopamine->update(pImpl->timestep);
    
    // Apply dopamine effects on neural excitability
    float dopamineLevel = pImpl->dopamine->getLevel();
    if (dopamineLevel > 0.0f) {
        applyDopamineToAllNeurons(dopamineLevel);
    }
    
    // Calculate neuromodulation factor for plasticity
    plasticityMod_ = pImpl->dopamine->getPlasticityFactor();
}

void applyAChModulation() {
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(pImpl->timestep);
    }
}

void applyNEModulation() {
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(pImpl->timestep);
    }
}

void applySerotoninModulation() {
    if (pImpl->serotonin) {
        pImpl->serotonin->update(pImpl->timestep);
    }
}

void applyDopamineToAllNeurons(float dopamineLevel) {
    float excitabilityMod = dopamineLevel * 0.5f;
    
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->injectCurrent(excitabilityMod);
            }
        }
    }
}

// Simplify working memory update (around lines 471-474):

void updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

// Simplify attention update (around lines 621-630):

void updateAttention() {
    if (!pImpl->attention) return;
    
    pImpl->attention->update(pImpl->timestep);
    
    // Apply attention to working memory winners
    if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
        pImpl->attention->processCompetition(competitors);
    }
}

// Create helper for development system update:

void updateDevelopmentSystem() {
    if (currentStep % 1000 != 0) return;
    
    if (!pImpl->developmentSystem) return;
    
    pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
    
    // Development affects plasticity rates
    updateDevelopmentPlasticityRates();
}

void updateDevelopmentPlasticityRates() {
    auto* sp = pImpl->structuralPlasticity.get();
    if (!sp) return;
    
    DevelopmentalStage stage = pImpl->developmentalStage;
    float plasticityMod = getPlasticityModForStage(stage);
    
    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

float getPlasticityModForStage(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return 1.0f; // High plasticity
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f; // Stable
        case DevelopmentalStage::Aging: return 0.1f; // Declining
        default: return 1.0f;
    }
}