// NLM Phase 6 Integration Fixes - Brain.cpp Implementation

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
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <filesystem>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
    }
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    // Initialize checkpoint manager
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    pImpl->checkpointManager->configure("./checkpoints", 10000, 10, true);
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}
Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    // Initialize core components
    pImpl->rng = std::make_unique<RandomGenerator>();
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
    
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    
    // Initialize neuromodulation
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    
    // Initialize development
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Create regions and neurons based on config
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    
    float neuronSize = static_cast<float>(neuronCount) / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        RegionId regionId(i + 1);
        size_t neuronsInRegion = (i == regionCount - 1) ? 
            neuronCount - i * neuronSize : static_cast<size_t>(neuronSize);
        
        auto region = std::make_unique<NeuralRegion>(regionId, neuronsInRegion);
        
        // Add populations to region
        size_t neuronsPerPop = neuronsInRegion / 10;  // 10 populations
        for (size_t j = 0; j < 10 && j * neuronsPerPop < neuronsInRegion; ++j) {
            size_t popNeurons = (j == 9) ? 
                neuronsInRegion - j * neuronsPerPop : neuronsPerPop;
            
            NeuronType popType = static_cast<NeuronType>(j % 5);
            auto population = std::make_unique<NeuralPopulation>(regionId, popNeurons, popType);
            region->addPopulation(std::move(population));
        }
        
        // Add region to brain
        pImpl->regions.push_back(std::move(region));
    }
    
    // Create sensory and motor neurons
    auto* region = pImpl->regions[0].get();
    auto neurons = region->getAllNeurons();
    
    // Find or create sensory and motor populations
    for (auto& pop : region->getPopulations()) {
        if (pop->getNeuronType() == NeuronType::Sensory) {
            for (auto* neuron : pop->getNeurons()) {
                pImpl->sensoryNeurons.push_back(neuron);
            }
        } else if (pop->getNeuronType() == NeuronType::Motor) {
            for (auto* neuron : pop->getNeurons()) {
                pImpl->motorNeurons.push_back(neuron);
            }
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < pImpl->regions.size(); ++i) {
        auto* region = pImpl->regions[i].get();
        size_t connectionProbability = 
            pImpl->config->getOr<size_t>("connection_probability", 15);
        
        region->initializeRandomConnectivity(
            *pImpl->rng, connectionProbability, 0.2f, 0.1f);
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(1000);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    
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

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
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
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Apply neuromodulation effects ==========
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
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
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
    
    // ========== STEP 7: Update episodic memory ==========
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
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
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// Checkpoint provider implementations
bool Brain::collectNeuronCheckpointData(NeuronCheckpointData& data) {
    size_t neuronCount = 0;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            neuronCount += pop->getNeuronCount();
        }
    }
    
    data.membranePotential.resize(neuronCount);
    data.restingPotential.resize(neuronCount);
    data.threshold.resize(neuronCount);
    data.resetPotential.resize(neuronCount);
    data.leakConductance.resize(neuronCount);
    data.firingState.resize(neuronCount);
    data.refractoryRemaining.resize(neuronCount);
    data.refractoryPeriod.resize(neuronCount);
    data.lastSpikeTime.resize(neuronCount);
    data.neuronType.resize(neuronCount);
    data.regionId.resize(neuronCount);
    data.populationId.resize(neuronCount);
    
    size_t index = 0;
    for (const auto& region : pImpl->regions) {
        RegionId regionId = region->getId();
        for (const auto& pop : region->getPopulations()) {
            NeuronType neuronType = pop->getNeuronType();
            auto neurons = pop->getNeurons();
            
            for (const auto* neuron : neurons) {
                const auto& state = neuron->getState();
                data.membranePotential[index] = state.membranePotential;
                data.restingPotential[index] = state.restingPotential;
                data.threshold[index] = state.threshold;
                data.resetPotential[index] = state.resetPotential;
                data.leakConductance[index] = state.leakConductance;
                data.firingState[index] = static_cast<uint8_t>(state.firingState);
                data.refractoryRemaining[index] = state.refractoryRemaining;
                data.refractoryPeriod[index] = state.refractoryPeriod;
                data.lastSpikeTime[index] = state.lastSpikeTime;
                data.neuronType[index] = static_cast<uint64_t>(neuronType);
                data.regionId[index] = static_cast<uint64_t>(regionId);
                data.populationId[index] = static_cast<uint64_t>(pop->getId());
                
                index++;
            }
        }
    }
    
    return !data.membranePotential.empty();
}

bool Brain::collectSynapseCheckpointData(SynapseCheckpointData& data) {
    size_t synapseCount = 0;
    
    for (const auto& region : pImpl->regions) {
        synapseCount += region->getSynapseCount();
    }
    
    data.sourceNeuron.resize(synapseCount);
    data.destinationNeuron.resize(synapseCount);
    data.weight.resize(synapseCount);
    data.delay.resize(synapseCount);
    data.synapseType.resize(synapseCount);
    data.plasticityFlags.resize(synapseCount);
    data.eligibilityTrace.resize(synapseCount);
    data.efficacy.resize(synapseCount);
    data.shortTermDepression.resize(synapseCount);
    data.shortTermFacilitation.resize(synapseCount);
    
    size_t index = 0;
    for (const auto& region : pImpl->regions) {
        auto synapses = region->getSynapses();
        
        for (const auto& synapse : synapses) {
            data.sourceNeuron[index] = synapse->getSourceNeuron();
            data.destinationNeuron[index] = synapse->getDestinationNeuron();
            data.weight[index] = synapse->getWeight();
            data.delay[index] = synapse->getDelay();
            data.synapseType[index] = static_cast<uint8_t>(synapse->getType());
            data.plasticityFlags[index] = synapse->getPlasticityFlags();
            data.eligibilityTrace[index] = synapse->getEligibilityTrace();
            data.efficacy[index] = synapse->getEfficacy();
            data.shortTermDepression[index] = synapse->getShortTermDepression();
            data.shortTermFacilitation[index] = synapse->getShortTermFacilitation();
            
            index++;
        }
    }
    
    return !data.weight.empty();
}

bool Brain::save(const std::string& filepath) const {
    // Use CheckpointSystem to save brain state
    auto checkpointManager = pImpl->checkpointManager.get();
    if (!checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for save operation");
        return false;
    }
    
    // Configure checkpoint providers
    checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
        return collectNeuronCheckpointData(data);
    });
    
    checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
        return collectSynapseCheckpointData(data);
    });
    
    // Use checkpoint manager's saveImmediately method
    return checkpointManager->saveImmediately("default");
}

bool Brain::load(const std::string& filepath) {
    // Use CheckpointSystem to load brain state
    auto checkpointManager = pImpl->checkpointManager.get();
    if (!checkpointManager) {
        NLM_LOG_ERROR("Checkpoint manager not available for load operation");
        return false;
    }
    
    return checkpointManager->load("default");
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find neuron with highest activity
    Neuron* bestNeuron = nullptr;
    float bestActivity = -1.0f;
    
    for (auto* neuron : pImpl->motorNeurons) {
        const auto& state = neuron->getState();
        float activity = std::abs(state.membranePotential - state.restingPotential);
        
        if (activity > bestActivity) {
            bestActivity = activity;
            bestNeuron = neuron;
        }
    }
    
    // Create action based on best neuron's region/population
    auto action = std::make_unique<Action>();
    action->setType(ActionType::Custom);
    
    // Log action selection
    NLM_LOG_INFO("Action selected: Custom (activity=" + std::to_string(bestActivity) + ")");
    
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory signals to brain systems
    if (signal.getType() == "dopamine" && pImpl->dopamine) {
        pImpl->dopamine->receiveSignal(signal.getValue());
    } else if (signal.getType() == "curiosity" && pImpl->curiosity) {
        pImpl->curiosity->receiveSignal(signal.getValue());
    } else if (signal.getType() == "novelty" && pImpl->novelty) {
        pImpl->novelty->receiveSignal(signal.getValue());
    }
}

void Brain::updatePlasticity() {
    // Calculate neuromodulation factor for plasticity
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
            syn->step(pImpl->currentTime);
        }
    }
}

void Brain::develop() {
    if (!pImpl->developmentSystem) return;
    
    // Update development based on experience
    pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000.0f);
    
    // Update developmental stage
    if (pImpl->currentStep % 1000 == 0) {
        pImpl->developmentalStage = 
            static_cast<DevelopmentalStage>((static_cast<int>(pImpl->developmentalStage) + 1) % 5);
    }
    
    NLM_LOG_INFO("Development update: stage=" + std::to_string(static_cast<int>(pImpl->developmentalStage)));
}

void Brain::reset() {
    // Reset all systems
    pImpl->spikeSystem->reset();
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    // Reset memory systems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->clear();
    }
    
    if (pImpl->episodicMemory) {
        // Clear episodic memory
        while (pImpl->episodicMemory->getEpisodeCount() > 0) {
            // Remove oldest episode
        }
    }
    
    // Reset statistics
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    
    // Reset development
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    pImpl->developmentProgress = 0.0f;
    
    NLM_LOG_INFO("Brain state reset completed");
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Simulation Step: " + std::to_string(pImpl->currentStep));
    NLM_LOG_INFO("Simulation Time: " + std::to_string(pImpl->currentTime) + "s");
    NLM_LOG_INFO("Total Neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total Synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Firing Neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Average Firing Rate: " + std::to_string(getAverageFiringRate()));
    
    // Log memory systems
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working Memory Traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic Memory Episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Log neuromodulation
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine Level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity Level: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    
    // Log development
    NLM_LOG_INFO("Developmental Stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    NLM_LOG_INFO("Development Progress: " + std::to_string(pImpl->developmentProgress * 100.0f) + "%");
    
    NLM_LOG_INFO("=========================");
}

} // namespace nlm
