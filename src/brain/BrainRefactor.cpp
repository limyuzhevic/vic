// Brain.cpp Refactoring Implementation
// This file contains the helper function implementations for Brain.cpp refactoring

#include "BrainRefactor.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// BrainRefactor implementation functions
namespace BrainRefactor {

// Initialization helper functions
void createRegions(Brain::Impl* impl) {
    for (size_t i = 0; i < impl->config->getOr<size_t>("region_count", 1); ++i) {
        auto brain = reinterpret_cast<Brain*>(impl);
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
}

void createNeuronsAcrossRegions(Brain::Impl* impl, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    impl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    impl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
}

void initializeConnectivity(Brain::Impl* impl, size_t regionCount, float connectionProbability) {
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = impl->getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*impl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void initializeMemorySystems(Brain::Impl* impl, size_t neuronCount) {
    // Initialize working memory
    impl->workingMemory->initialize(reinterpret_cast<Brain*>(impl));
    impl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    impl->episodicMemory->initialize(reinterpret_cast<Brain*>(impl));
    impl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    impl->associativeMemory->initialize(reinterpret_cast<Brain*>(impl));
}

void initializeCognitionSystems(Brain::Impl* impl) {
    impl->planner->initialize(reinterpret_cast<Brain*>(impl));
    impl->planner->setPlanningDepth(5);
    
    impl->conceptFormation->initialize(reinterpret_cast<Brain*>(impl));
    
    impl->attention->initialize(reinterpret_cast<Brain*>(impl));
    impl->attention->setInhibitionStrength(0.5f);
    impl->attention->setExcitationStrength(1.5f);
}

void initializeNeuromodulationSystems(Brain::Impl* impl) {
    impl->novelty->initialize(reinterpret_cast<Brain*>(impl));
    impl->curiosity->initialize(reinterpret_cast<Brain*>(impl));
    
    // Register spike handlers for event-driven processing
    impl->spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
        ++impl->totalSpikesThisStep;
        ++impl->totalSpikesTotal;
    });
    
    // Register delayed spike handler
    impl->spikeSystem->registerDelayedHandler([impl](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : impl->regions) {
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

void initializeDevelopmentSystem(Brain::Impl* impl) {
    impl->developmentSystem->initialize();
}

void setupSpikeHandlers(Brain::Impl* impl) {
    // Spike handlers are already set up in initializeNeuromodulationSystems
}

void configureCheckpointManager(Brain::Impl* impl) {
    std::string checkpointDir = impl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    impl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

// Step function helper implementations
void initializeStep(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    impl->currentStep = currentStep;
    impl->currentTime = currentTime;
    impl->totalSpikesThisStep = 0;
}

void processDelayedSpikes(Brain::Impl* impl, SimulationStep currentStep, Timestamp currentTime) {
    impl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void updateNeuronDynamics(Brain::Impl* impl) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(impl->currentTime, impl->timestep);
            }
        }
    }
}

void detectAndScheduleSpikes(Brain::Impl* impl) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                processNeuronSpike(impl, neuron, region.get());
            }
        }
    }
}

void processNeuronSpike(Brain::Impl* impl, Neuron* neuron, NeuralRegion* region) {
    const auto& state = neuron->getState();
    bool justFired = (state.firingState == FiringState::Refractory &&
                     state.lastSpikeTime >= 0.0f &&
                     std::abs(static_cast<float>(impl->currentTime) - state.lastSpikeTime) < impl->timestep * 2.0f);

    if (justFired) {
        // Queue the spike
        SpikeEvent event(neuron->getId(), impl->currentTime, impl->currentStep);
        impl->spikeSystem->queueSpike(event);

        // Record post-synaptic spike for plasticity
        auto incomingSynapses = region->getSynapsesTo(neuron->getId());
        for (Synapse* syn : incomingSynapses) {
            syn->recordPostSpike(impl->currentTime);
        }

        // Schedule delayed spike events
        auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
        for (Synapse* syn : outgoingSynapses) {
            DelayedSpikeEvent delayedEvent = createDelayedSpikeEvent(
                impl, neuron, syn, impl->currentStep, impl->currentTime
            );
            impl->spikeSystem->queueDelayedSpike(delayedEvent);
            syn->recordPreSpike(impl->currentTime);
        }

        // Store to working memory
        if (impl->workingMemory) {
            impl->workingMemory->storeToNeuron(neuron->getId(), 
                std::abs(state.membranePotential - state.restingPotential) / 10.0f);
        }
    }
}

DelayedSpikeEvent BrainRefactor::createDelayedSpikeEvent(Brain::Impl* impl, Neuron* neuron, 
    Synapse* synapse, SimulationStep currentStep, Timestamp currentTime) {
    Delay delay = synapse->getDelay();
    SimulationStep deliveryStep = currentStep + delay;
    Timestamp deliveryTime = currentTime + delay * impl->timestep;

    return DelayedSpikeEvent(
        neuron->getId(),
        synapse->getDestinationNeuron(),
        synapse->getId(),
        synapse->getWeight(),
        synapse->getType(),
        currentTime,
        deliveryTime,
        currentStep,
        deliveryStep
    );
}

void processSpikes(Brain::Impl* impl) {
    impl->spikeSystem->processSpikes(impl->currentStep);
}

void updateWorkingMemory(Brain::Impl* impl) {
    if (impl->workingMemory) {
        impl->workingMemory->update(impl->timestep);
    }
}

void applyNeuromodulationEffects(Brain::Impl* impl) {
    // Update novelty detection
    if (impl->novelty) {
        impl->novelty->update(impl->timestep);
    }

    // Update curiosity
    if (impl->curiosity) {
        impl->curiosity->update(impl->timestep);
    }

    // Update dopamine (reward prediction error)
    if (impl->dopamine) {
        impl->dopamine->update(impl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = impl->dopamine->getLevel();
        for (auto& region : impl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void updateEpisodicMemory(Brain::Impl* impl) {
    impl->stepsSinceLastEpisode++;
    if (impl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        impl->stepsSinceLastEpisode = 0;
        
        if (impl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = impl->currentStep;
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : impl->regions) {
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
            episode.reward = impl->dopamine ? impl->dopamine->getLevel() : 0.0f;
            
            impl->episodicMemory->storeEpisode(episode);
        }
    }
}

void updatePredictionSystem(Brain::Impl* impl) {
    if (impl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void updateAttentionSystem(Brain::Impl* impl) {
    if (impl->attention) {
        impl->attention->update(impl->timestep);
        
        // Apply attention to working memory winners
        if (impl->workingMemory && !impl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = impl->workingMemory->getMemoryNeurons();
            impl->attention->processCompetition(competitors);
        }
    }
}

void updateConceptFormation(Brain::Impl* impl) {
    if (impl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void applyStructuralPlasticity(Brain::Impl* impl) {
    if (impl->currentStep % 100 == 0) {
        // Get plasticity system from config if available
        // For now, we'll skip since we need access to the Brain class
    }
}

void replayImportantMemories(Brain::Impl* impl) {
    if (impl->currentStep % impl->replayInterval == 0 && impl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = impl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            impl->episodicMemory->replayEpisode(episode);
        }
    }
}

void applyDevelopmentEffects(Brain::Impl* impl) {
    if (impl->currentStep % 1000 == 0) {  // Update development every 1000 steps
        // Development system update would go here
        // Need access to Brain class for proper implementation
        
        // Development affects plasticity rates
        auto* sp = impl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = impl->developmentalStage;
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

void periodicMemoryConsolidation(Brain::Impl* impl) {
    if (impl->currentStep % impl->consolidationInterval == 0 && impl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        impl->episodicMemory->consolidate(0.3f);
    }
}

void checkpointManagement(Brain::Impl* impl) {
    if (impl->checkpointManager) {
        impl->checkpointManager->update(impl->currentStep, impl->currentTime);
    }
}

// Memory system helper functions
void setupSensoryNeurons(Brain::Impl* impl, NeuralRegion* region) {
    auto* sensoryPop = region->getPopulation(region->getPopulationIdByType(NeuronType::Sensory));
    if (sensoryPop) {
        for (auto* neuron : sensoryPop->getNeurons()) {
            impl->sensoryNeurons.push_back(neuron);
        }
    }
}

void setupMotorNeurons(Brain::Impl* impl, NeuralRegion* region) {
    auto* motorPop = region->getPopulation(region->getPopulationIdByType(NeuronType::Motor));
    if (motorPop) {
        for (auto* neuron : motorPop->getNeurons()) {
            impl->motorNeurons.push_back(neuron);
        }
    }
}

void collectMemorySystemMetrics(Brain::Impl* impl) {
    // Collect metrics from memory systems for logging
    if (impl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(impl->workingMemory->getActiveTraces()));
    }
    if (impl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(impl->episodicMemory->getEpisodeCount()));
    }
}

// Action and sensory processing helper functions
void processSensoryInput(Brain::Impl* impl, const SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = impl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        impl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (impl->workingMemory && normalizedValue > 0.5f) {
            impl->workingMemory->storeToNeuron(impl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void injectCurrentToNeurons(Brain::Impl* impl, NeuronId neuron, MembranePotential current) {
    for (auto& region : impl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void injectCurrentToNeuronsByType(Brain::Impl* impl, NeuronType type, MembranePotential current) {
    for (auto& region : impl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

// Statistics and logging helper functions
void updateBrainStatistics(Brain::Impl* impl) {
    // Update brain statistics
    if (impl->regions.empty()) return;
    
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : impl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    // Store E/I ratio for later use
    impl->excitatoryInhibitionRatio = (totalInhibitory > 0.0f) ? (totalExcitatory / totalInhibitory) : 
                                        (totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f);
}

void logBrainStatus(const Brain* brain) {
    if (!brain) return;
    
    const auto& impl = *brain->pImpl;
    
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Refactored) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(brain->getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(brain->getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
    
    // Log memory system status
    if (impl.workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(impl.workingMemory->getActiveTraces()));
    }
    if (impl.episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(impl.episodicMemory->getEpisodeCount()));
    }
    
    // Log neuromodulation status
    if (impl.dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(impl.dopamine->getLevel()));
    }
    
    // Log development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(impl.developmentalStage)));
    
    for (const auto& region : impl.regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

// Additional utility functions are implemented here
// Complete refactoring of Brain.cpp into modular helper functions

}} // namespace nlm