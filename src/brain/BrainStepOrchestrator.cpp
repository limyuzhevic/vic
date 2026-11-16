#include "BrainStepOrchestrator.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

BrainStepOrchestrator::BrainStepOrchestrator(Brain* brain) 
    : brain_(brain), replayInterval_(100), consolidationInterval_(1000), 
      isResting_(false), stepsSinceLastEpisode_(0) {}

BrainStepOrchestrator::~BrainStepOrchestrator() = default;

void BrainStepOrchestrator::executeStep(SimulationStep currentStep, Timestamp currentTime) {
    // Phase 6: INTEGRATED ARTIFICIAL BRAIN LOOP
    
    // Update state
    brain_->pImpl->currentStep = currentStep;
    brain_->pImpl->currentTime = currentTime;
    brain_->pImpl->totalSpikesThisStep = 0;
    
    // Execute each step in order
    processDelayedSpikes(currentStep, currentTime);
    updateNeuronDynamics(currentTime, brain_->pImpl->timestep);
    detectAndScheduleSpikes(currentStep, currentTime);
    updateWorkingMemory(brain_->pImpl->timestep);
    applyNeuromodulationEffects(brain_->pImpl->timestep);
    brain_->pImpl->stepsSinceLastEpisode++;
    
    // Apply plasticity before memory updates
    float plasticityMod = 1.0f;
    calculateNeuromodulationFactor(plasticityMod);
    applyPlasticityRules(plasticityMod, brain_->pImpl->timestep);
    
    updateEpisodicMemory(currentStep);
    updatePredictionSystem();
    updateAttentionSystem();
    updateConceptFormation();
    applyStructuralPlasticity(currentStep);
    replayImportantMemories(currentStep);
    applyDevelopmentEffects(currentStep);
    performMemoryConsolidation(currentStep);
    checkpointManagement(currentStep, currentTime);
    
    // Collect statistics
    collectStatistics();
}

void BrainStepOrchestrator::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (brain_->pImpl->spikeSystem) {
        brain_->pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    }
}

void BrainStepOrchestrator::updateNeuronDynamics(Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : brain_->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainStepOrchestrator::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : brain_->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                detectAndHandleSingleSpike(neuron, currentStep, currentTime, region);
            }
        }
    }
    
    if (brain_->pImpl->spikeSystem) {
        brain_->pImpl->spikeSystem->processSpikes(currentStep);
    }
}

void BrainStepOrchestrator::detectAndHandleSingleSpike(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime, NeuralRegion& region) {
    const auto& state = neuron->getState();
    bool justFired = (state.firingState == FiringState::Refractory &&
                     state.lastSpikeTime >= 0.0f &&
                     std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < brain_->pImpl->timestep * 2.0f);

    if (justFired) {
        scheduleSpikeEvent(neuron, currentStep, currentTime);
        recordPlasticityEvents(neuron, currentStep, currentTime, region);
        storeSpikedNeuronsToWorkingMemory(neuron);
    }
}

void BrainStepOrchestrator::scheduleSpikeEvent(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime) {
    SpikeEvent event(neuron->getId(), currentTime, currentStep);
    brain_->pImpl->spikeSystem->queueSpike(event);
}

void BrainStepOrchestrator::recordPlasticityEvents(Neuron* neuron, SimulationStep currentStep, Timestamp currentTime, NeuralRegion& region) {
    auto incomingSynapses = region.getSynapsesTo(neuron->getId());
    for (Synapse* syn : incomingSynapses) {
        syn->recordPostSpike(currentTime);
    }
    
    auto outgoingSynapses = region.getSynapsesFrom(neuron->getId());
    for (Synapse* syn : outgoingSynapses) {
        scheduleDelayedSpikeEvent(neuron, syn, currentStep, currentTime);
        syn->recordPreSpike(currentTime);
    }
}

void BrainStepOrchestrator::scheduleDelayedSpikeEvent(Neuron* neuron, Synapse* syn, SimulationStep currentStep, Timestamp currentTime) {
    Delay delay = syn->getDelay();
    SimulationStep deliveryStep = currentStep + delay;
    Timestamp deliveryTime = currentTime + delay * brain_->pImpl->timestep;

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

    brain_->pImpl->spikeSystem->queueDelayedSpike(delayedEvent);
}

void BrainStepOrchestrator::storeSpikedNeuronsToWorkingMemory(Neuron* neuron) {
    if (brain_->pImpl->workingMemory) {
        brain_->pImpl->workingMemory->storeToNeuron(neuron->getId(), 1.0f);
    }
}

void BrainStepOrchestrator::updateWorkingMemory(TimestepDuration timestep) {
    if (brain_->pImpl->workingMemory) {
        brain_->pImpl->workingMemory->update(timestep);
    }
}

void BrainStepOrchestrator::applyNeuromodulationEffects(TimestepDuration timestep) {
    updateNoveltyDetection(timestep);
    updateCuriosity(timestep);
    updateDopamineEffects(timestep);
}

void BrainStepOrchestrator::updateNoveltyDetection(TimestepDuration timestep) {
    if (brain_->pImpl->novelty) {
        brain_->pImpl->novelty->update(timestep);
    }
}

void BrainStepOrchestrator::updateCuriosity(TimestepDuration timestep) {
    if (brain_->pImpl->curiosity) {
        brain_->pImpl->curiosity->update(
            brain_->pImpl->novelty ? brain_->pImpl->novelty->getLevel() : 0.0f,
            brain_->pImpl->predictionError ? brain_->pImpl->predictionError->getError() : 0.0f,
            timestep
        );
    }
}

void BrainStepOrchestrator::updateDopamineEffects(TimestepDuration timestep) {
    if (brain_->pImpl->dopamine) {
        brain_->pImpl->dopamine->update(timestep);
        applyDopamineEffectsOnNeurons();
    }
}

void BrainStepOrchestrator::applyDopamineEffectsOnNeurons() {
    float dopamineLevel = brain_->pImpl->dopamine->getLevel();
    for (auto& region : brain_->pImpl->regions) {
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

void BrainStepOrchestrator::calculateNeuromodulationFactor(float& plasticityMod) {
    plasticityMod = 1.0f;
    if (brain_->pImpl->dopamine) {
        plasticityMod = brain_->pImpl->dopamine->getPlasticityFactor();
    }
}

void BrainStepOrchestrator::applyPlasticityRules(float plasticityMod, TimestepDuration timestep) {
    for (auto& region : brain_->pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            applySTDPWithNeuromodulation(syn, plasticityMod, timestep);
            applyHebbianLearning(syn, plasticityMod, timestep);
        }
    }
    
    applySynapticScaling(brain_->pImpl->currentStep);
}

void BrainStepOrchestrator::applySTDPWithNeuromodulation(Synapse* syn, float plasticityMod, TimestepDuration timestep) {
    if (syn->getPlasticityFlags().stdp) {
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            brain_->pImpl->stdp->update(syn, preSpikes, postSpikes, timestep);
            float weight = syn->getWeight();
            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
            syn->setWeight(weight);
        }
    }
}

void BrainStepOrchestrator::applyHebbianLearning(Synapse* syn, float plasticityMod, TimestepDuration timestep) {
    if (syn->getPlasticityFlags().hebbian) {
        const auto& preSpikes = syn->getPreSpikeHistory();
        const auto& postSpikes = syn->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            brain_->pImpl->hebbian->update(syn, preSpikes, postSpikes, timestep);
        }
    }
}

void BrainStepOrchestrator::applySynapticScaling(SimulationStep currentStep) {
    if (currentStep > 0 && currentStep % 100 == 0) {
        constexpr float kIncomingExcitatoryCap = 3.0f;
        for (auto& region : brain_->pImpl->regions) {
            for (auto* neuron : region->getAllNeurons()) {
                auto incoming = region->getSynapsesTo(neuron->getId());
                float sum = 0.0f;
                for (Synapse* syn : incoming) {
                    if (syn->getWeight() > 0.0f) sum += syn->getWeight();
                }
                if (sum > kIncomingExcitatoryCap) {
                    float scale = kIncomingExcitatoryCap / sum;
                    for (Synapse* syn : incoming) {
                        if (syn->getWeight() > 0.0f) {
                            syn->setWeight(syn->getWeight() * scale);
                        }
                    }
                }
            }
        }
    }
}

void BrainStepOrchestrator::updateEpisodicMemory(SimulationStep currentStep) {
    brain_->pImpl->stepsSinceLastEpisode++;
    if (brain_->pImpl->stepsSinceLastEpisode >= 10) {
        brain_->pImpl->stepsSinceLastEpisode = 0;
        if (brain_->pImpl->episodicMemory) {
            captureAndStoreCurrentEpisode(currentStep);
        }
    }
}

void BrainStepOrchestrator::captureAndStoreCurrentEpisode(SimulationStep currentStep) {
    if (!brain_->pImpl->episodicMemory) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = currentStep;
    episode.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f;
    
    captureActiveNeurons(episode);
    episode.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f;
    
    brain_->pImpl->episodicMemory->storeEpisode(episode);
}

void BrainStepOrchestrator::captureActiveNeurons(EpisodicMemoryItem& episode) {
    for (auto& region : brain_->pImpl->regions) {
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
}

void BrainStepOrchestrator::updatePredictionSystem() {
    if (brain_->pImpl->predictionSystem && brain_->pImpl->sensoryInput) {
        makePredictionsBasedOnSensory();
        updatePredictionsWithActualObservations();
    }
}

void BrainStepOrchestrator::makePredictionsBasedOnSensory() {
    auto predicted = brain_->pImpl->predictionSystem->predictNextState(*brain_->pImpl->sensoryInput);
    // Predictions are made and stored internally
}

void BrainStepOrchestrator::updatePredictionsWithActualObservations() {
    if (brain_->pImpl->predictionSystem) {
        // Actual observations are already available from sensory input
        // This is handled by the caller of this step method
    }
}

void BrainStepOrchestrator::updateAttentionSystem() {
    if (brain_->pImpl->attention) {
        brain_->pImpl->attention->update(brain_->pImpl->timestep);
        processAttentionWithWorkingMemory();
    }
}

void BrainStepOrchestrator::processAttentionWithWorkingMemory() {
    if (brain_->pImpl->workingMemory && !brain_->pImpl->workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = brain_->pImpl->workingMemory->getMemoryNeurons();
        brain_->pImpl->attention->processCompetition(competitors);
    }
}

void BrainStepOrchestrator::updateConceptFormation() {
    if (brain_->pImpl::conceptFormation && brain_->pImpl->workingMemory) {
        processWorkingMemoryForConcepts();
    }
}

void BrainStepOrchestrator::processWorkingMemoryForConcepts() {
    std::vector<float> currentPattern;
    for (auto& region : brain_->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                currentPattern.push_back(neuron->isFiring() ? 1.0f : 0.0f);
            }
        }
    }
    
    if (!currentPattern.empty()) {
        std::vector<float> features;
        for (size_t i = 0; i < currentPattern.size() && i < 10; ++i) {
            features.push_back(currentPattern[i]);
        }
        
        size_t conceptId = brain_->pImpl->conceptFormation->presentExperience(
            currentPattern, features,
            brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f,
            brain_->pImpl->currentStep
        );
        
        // Note: Concept formation updates are now tracked internally
    }
}

void BrainStepOrchestrator::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0 && brain_->pImpl->structuralPlasticity) {
        brain_->pImpl->structuralPlasticity->update(brain_, *brain_->pImpl->rng);
    }
}

void BrainStepOrchestrator::replayImportantMemories(SimulationStep currentStep) {
    if (shouldReplayMemories(currentStep) && brain_->pImpl->episodicMemory) {
        performMemoryReplay(currentStep);
    }
}

bool BrainStepOrchestrator::shouldReplayMemories(SimulationStep currentStep) const {
    return currentStep % replayInterval_ == 0 && brain_->pImpl->episodicMemory;
}

void BrainStepOrchestrator::performMemoryReplay(SimulationStep currentStep) {
    size_t replayCount = 3;
    
    // Enhanced replay based on neuromodulation
    if (brain_->pImpl->dopamine && brain_->pImpl->dopamine->getLevel() > 0.5f) {
        replayCount = 5;
    }
    if (brain_->pImpl->acetylcholine && brain_->pImpl->acetylcholine->getLevel() > 0.3f) {
        replayCount = std::min(replayCount + 2, 8);
    }
    
    auto episodesToReplay = brain_->pImpl->episodicMemory->getEpisodesForReplay(replayCount);
    for (const auto* episode : episodesToReplay) {
        brain_->pImpl->episodicMemory->replayEpisode(episode);
    }
}

void BrainStepOrchestrator::applyDevelopmentEffects(SimulationStep currentStep) {
    if (shouldApplyDevelopmentEffects(currentStep) && brain_->pImpl->developmentSystem) {
        brain_->pImpl->developmentSystem->update(brain_, *brain_->pImpl->rng, brain_->pImpl->timestep * 1000);
        modifyPlasticityBasedOnDevelopment();
    }
}

bool BrainStepOrchestrator::shouldApplyDevelopmentEffects(SimulationStep currentStep) const {
    return currentStep % 1000 == 0;
}

void BrainStepOrchestrator::modifyPlasticityBasedOnDevelopment() {
    auto* sp = brain_->pImpl->structuralPlasticity.get();
    if (!sp) return;
    
    DevelopmentalStage stage = brain_->pImpl->developmentalStage;
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
    
    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

void BrainStepOrchestrator::performMemoryConsolidation(SimulationStep currentStep) {
    if (shouldConsolidateMemories(currentStep) && brain_->pImpl->episodicMemory) {
        consolidateMemories(currentStep);
    }
}

bool BrainStepOrchestrator::shouldConsolidateMemories(SimulationStep currentStep) const {
    return currentStep % consolidationInterval_ == 0 && brain_->pImpl->episodicMemory;
}

void BrainStepOrchestrator::consolidateMemories(SimulationStep currentStep) {
    // Working memory to episodic memory transfer
    if (brain_->pImpl->workingMemory) {
        std::vector<NeuronId> activeMemory = brain_->pImpl->workingMemory->getMemoryNeurons();
        if (!activeMemory.empty()) {
            EpisodicMemoryItem memory;
            memory.timestamp = currentStep;
            memory.activeNeurons = activeMemory;
            memory.reward = brain_->pImpl->dopamine ? brain_->pImpl->dopamine->getLevel() : 0.0f;
            
            float memoryStrength = 0.5f;
            if (brain_->pImpl->dopamine) memoryStrength += brain_->pImpl->dopamine->getLevel() * 0.3f;
            if (brain_->pImpl->acetylcholine) memoryStrength += brain_->pImpl->acetylcholine->getLevel() * 0.4f;
            
            memory.strength = memoryStrength;
            brain_->pImpl->episodicMemory->storeEpisode(memory);
        }
    }
    
    // Enhanced consolidation based on neuromodulators
    brain_->pImpl->episodicMemory->consolidate(0.3f);
    
    if (brain_->pImpl->serotonin && brain_->pImpl->serotonin->getLevel() > 0.5f) {
        brain_->pImpl->episodicMemory->consolidate(0.7f);
    }
    if (brain_->pImpl->norepinephrine && brain_->pImpl->norepinephrine->getLevel() > 0.3f) {
        brain_->pImpl->episodicMemory->consolidate(0.5f);
    }
}

void BrainStepOrchestrator::checkpointManagement(SimulationStep currentStep, Timestamp currentTime) {
    if (brain_->pImpl->checkpointManager) {
        brain_->pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainStepOrchestrator::collectStatistics() const {
    // Basic statistics collection
    if (brain_->pImpl->logger) {
        brain_->pImpl->logger->logStepStatistics(
            brain_->pImpl->currentStep,
            brain_->pImpl->currentTime,
            brain_->pImpl->totalSpikesThisStep,
            brain_->pImpl->totalSpikesTotal
        );
    }
}

} // namespace nlm