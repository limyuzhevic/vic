#include "BrainSimulationLoop.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct BrainSimulationLoop::Impl {
    Brain* brain;
    
    // Manager references
    std::unique_ptr<BrainMemoryManager> memoryManager;
    std::unique_ptr<BrainCognitionManager> cognitionManager;
    std::unique_ptr<BrainNeuromodulationManager> neuromodulationManager;
    std::unique_ptr<BrainDevelopmentManager> developmentManager;
    std::unique_ptr<BrainPlasticityManager> plasticityManager;
    
    // Simulation state
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    Impl(Brain* b) : brain(b), totalSpikesThisStep(0), totalSpikesTotal(0) {
        // Create manager instances
        memoryManager = std::make_unique<BrainMemoryManager>();
        cognitionManager = std::make_unique<BrainCognitionManager>();
        neuromodulationManager = std::make_unique<BrainNeuromodulationManager>();
        developmentManager = std::make_unique<BrainDevelopmentManager>();
        plasticityManager = std::make_unique<BrainPlasticityManager>();
        
        // Initialize managers
        memoryManager->initialize(brain);
        cognitionManager->initialize(brain);
        neuromodulationManager->initialize(brain);
        developmentManager->initialize(brain);
        plasticityManager->initialize(brain);
    }
};

BrainSimulationLoop::BrainSimulationLoop(Brain* brain) : pImpl(std::make_unique<Impl>(brain)) {}

BrainSimulationLoop::~BrainSimulationLoop() = default;

void BrainSimulationLoop::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->totalSpikesThisStep = 0;
    
    // Phase 1: Process pending delayed spikes (deliver synaptic input)
    processDelayedSpikes(currentStep, currentTime);
    
    // Phase 2: Update all neurons (LIF dynamics)
    updateNeurons(currentTime, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    
    // Phase 3: Detect spikes and schedule spike events
    detectSpikes(currentStep, currentTime);
    
    // Phase 4: Process immediate spikes
    processSpikes(currentStep);
    
    // Phase 5: Update working memory
    pImpl->memoryManager->updateWorkingMemory(pImpl->brain, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    
    // Phase 6: Update episodic memory
    pImpl->memoryManager->updateEpisodicMemory(pImpl->brain, currentStep, currentTime);
    
    // Phase 7: Update neuromodulation
    updateNeuromodulation(currentTime, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    
    // Phase 8: Apply neuromodulation to neurons
    float neuromodulationLevel = pImpl->brain->getDopamine() ? pImpl->brain->getDopamine()->getLevel() : 0.0f;
    pImpl->neuromodulationManager->applyNeuromodulationToNeurons(pImpl->brain, neuromodulationLevel);
    
    // Phase 9: Apply plasticity rules (STDP and Hebbian)
    float plasticityMod = 1.0f;
    if (pImpl->brain->getDopamine()) {
        plasticityMod = pImpl->brain->getDopamine()->getPlasticityFactor();
    }
    applyPlasticity(currentTime, plasticityMod);
    
    // Phase 10: Update cognition systems
    pImpl->cognitionManager->updatePlanner(pImpl->brain, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    pImpl->cognitionManager->updateConceptFormation(pImpl->brain);
    pImpl->cognitionManager->updateAttention(pImpl->brain, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    pImpl->cognitionManager->processWorkingMemoryCompetition(pImpl->brain);
    
    // Phase 11: Apply structural plasticity periodically
    applyStructuralPlasticity(currentStep);
    
    // Phase 12: Replay important memories
    replayMemories(currentStep);
    
    // Phase 13: Apply development effects
    applyDevelopment(currentStep, currentTime, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
    
    // Phase 14: Apply memory consolidation
    applyMemoryConsolidation(currentStep);
    
    // Phase 15: Update checkpoint management
    updateCheckpoints(currentStep, currentTime);
    
    // Phase 16: Collect statistics
    collectStatistics();
}

void BrainSimulationLoop::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->brain->getSpikeSystem()) {
        pImpl->brain->getSpikeSystem()->processDelayedSpikes(currentStep, currentTime);
    }
}

void BrainSimulationLoop::updateNeurons(Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainSimulationLoop::detectSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep * 2.0f);

                if (justFired) {
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    if (pImpl->brain->getSpikeSystem()) {
                        pImpl->brain->getSpikeSystem()->queueSpike(event);
                    }
                    pImpl->totalSpikesThisStep++;
                    pImpl->totalSpikesTotal++;

                    // Record post-synaptic spike for incoming synapses
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Record pre-synaptic spike for plasticity
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        syn->recordPreSpike(currentTime);
                    }
                }
            }
        }
    }
}

void BrainSimulationLoop::processSpikes(SimulationStep currentStep) {
    if (pImpl->brain->getSpikeSystem()) {
        pImpl->brain->getSpikeSystem()->processSpikes(currentStep);
    }
}

void BrainSimulationLoop::updateNeuromodulation(Timestamp currentTime, TimestepDuration timestep) {
    pImpl->neuromodulationManager->updateNeuromodulators(pImpl->brain, timestep);
}

void BrainSimulationLoop::applyPlasticity(Timestamp currentTime, float plasticityMod) {
    for (auto& region : pImpl->brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (pImpl->brain->getSTDP()) {
                        pImpl->brain->getSTDP()->update(syn, preSpikes, postSpikes, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                        float weight = syn->getWeight();
                        weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                        syn->setWeight(weight);
                    }
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    if (pImpl->brain->getHebbian()) {
                        pImpl->brain->getHebbian()->update(syn, preSpikes, postSpikes, pImpl->brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                    }
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void BrainSimulationLoop::updateAttention(TimestepDuration timestep) {
    pImpl->cognitionManager->updateAttention(pImpl->brain, timestep);
}

void BrainSimulationLoop::updateConceptFormation() {
    pImpl->cognitionManager->updateConceptFormation(pImpl->brain);
}

void BrainSimulationLoop::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0 && pImpl->brain->getRandomGenerator()) {
        pImpl->plasticityManager->applyStructuralPlasticity(pImpl->brain, currentStep, *pImpl->brain->getRandomGenerator());
    }
}

void BrainSimulationLoop::replayMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->brain->getDopamine()->getReplayInterval() == 0 && pImpl->brain->getEpisodicMemory()) {
        auto episodesToReplay = pImpl->brain->getEpisodicMemory()->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->brain->getEpisodicMemory()->replayEpisode(episode);
        }
    }
}

void BrainSimulationLoop::applyDevelopment(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    if (currentStep % 1000 == 0) {
        pImpl->developmentManager->updateDevelopment(pImpl->brain, currentStep, currentTime, timestep);
    }
}

void BrainSimulationLoop::applyMemoryConsolidation(SimulationStep currentStep) {
    if (currentStep % pImpl->brain->getDopamine()->getConsolidationInterval() == 0 && pImpl->brain->getEpisodicMemory()) {
        pImpl->brain->getEpisodicMemory()->consolidate(0.3f);
    }
}

void BrainSimulationLoop::updateCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->brain->getSpikeSystem() && pImpl->brain->getSpikeSystem()->getCheckpointManager()) {
        pImpl->brain->getSpikeSystem()->getCheckpointManager()->update(currentStep, currentTime);
    }
}

void BrainSimulationLoop::collectStatistics() {
    // Collect simulation statistics
    NLM_LOG_INFO("=== Brain Simulation Step Complete ===");
    NLM_LOG_INFO("Spikes this step: " + std::to_string(pImpl->totalSpikesThisStep));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
}

size_t BrainSimulationLoop::getTotalSpikesThisStep() const {
    return pImpl->totalSpikesThisStep;
}

size_t BrainSimulationLoop::getTotalSpikesTotal() const {
    return pImpl->totalSpikesTotal;
}

} // namespace nlm
