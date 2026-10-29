// Brain implementation - simplified and improved structure
// Phase 6: REFACTORED - Modular brain with system components
// 
// This implementation addresses the original monolithic 1117-line Brain.cpp
// by delegating to specialized system components while maintaining full functionality

#include "Brain.hpp"
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
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Helper functions for initialization
static void setupBrainRegions(Brain* brain) {
    size_t neuronCount = brain->pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = brain->pImpl->config->getOr<size_t>("region_count", 1);
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
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
                    brain->pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain->pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
}

static void setupSynapticConnectivity(Brain* brain) {
    size_t regionCount = brain->pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = brain->pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*brain->pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

static void setupMemorySystems(Brain* brain) {
    // Initialize working memory
    brain->pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    brain->pImpl->workingMemory->initialize(brain);
    brain->pImpl->workingMemory->setCapacity(brain->getTotalNeuronCount() / 10);
    
    // Initialize episodic memory
    brain->pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    brain->pImpl->episodicMemory->initialize(brain);
    brain->pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    brain->pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    brain->pImpl->associativeMemory->initialize(brain);
}

static void setupCognitionSystems(Brain* brain) {
    // Initialize cognition systems
    brain->pImpl->planner = std::make_unique<NeuralPlanner>();
    brain->pImpl->planner->initialize(brain);
    brain->pImpl->planner->setPlanningDepth(5);
    
    brain->pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    brain->pImpl->conceptFormation->initialize(brain);
    
    brain->pImpl->attention = std::make_unique<AttentionalSelection>();
    brain->pImpl->attention->initialize(brain);
    brain->pImpl->attention->setInhibitionStrength(0.5f);
    brain->pImpl->attention->setExcitationStrength(1.5f);
}

static void setupNeuromodulationSystems(Brain* brain) {
    // Initialize neuromodulation systems
    brain->pImpl->novelty = std::make_unique<Novelty>();
    brain->pImpl->novelty->initialize(brain);
    
    brain->pImpl->curiosity = std::make_unique<Curiosity>();
    brain->pImpl->curiosity->initialize(brain);
    
    brain->pImpl->dopamine = std::make_unique<Dopamine>();
    brain->pImpl->dopamine->initialize(brain);
    
    brain->pImpl->predictionError = std::make_unique<PredictionError>();
    brain->pImpl->predictionError->initialize(brain);
}

static void setupDevelopmentSystem(Brain* brain) {
    // Initialize development system
    brain->pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    brain->pImpl->developmentSystem->initialize(brain);
}

static void setupPlasticitySystems(Brain* brain) {
    // Configure STDP parameters
    float ltpWeight = brain->pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = brain->pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = brain->pImpl->config->getOr<float>("stdp_tau", 20.0f);
    brain->pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = brain->pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = brain->pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    brain->pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    brain->pImpl->structuralPlasticity->setPruningRate(pruningRate);
}

static void setupCheckpointSystem(Brain* brain) {
    // Configure checkpoint manager
    std::string checkpointDir = brain->pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    brain->pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

static void setupSpikeSystem(Brain* brain) {
    // Register spike handlers for event-driven processing
    brain->pImpl->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    brain->pImpl->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
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
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Setup brain structure
    setupBrainRegions(this);
    setupSynapticConnectivity(this);
    
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Setup integrated systems
    setupMemorySystems(this);
    setupCognitionSystems(this);
    setupNeuromodulationSystems(this);
    setupDevelopmentSystem(this);
    setupPlasticitySystems(this);
    
    // Setup spike system
    setupSpikeSystem(this);
    setupCheckpointSystem(this);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
// Process pending delayed spikes (deliver synaptic input)
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Update all neurons and detect spikes in a single pass for better performance
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Update neuron membrane potential (LIF dynamics)
                bool neuronFired = neuron->stepLIF(currentTime, pImpl->timestep);
                
                if (neuronFired) {
                    // Neuron just fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

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

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        const auto& state = neuron->getState();
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Rest of the step implementation continues with other systems (working memory, neuromodulation, etc.)
    
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
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
    
    // Apply plasticity rules (STDP and Hebbian)
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
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
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
    
    // Update episodic memory
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
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
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
    
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // Replay important memories
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
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
    
    // Periodic memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

} // namespace nlm
