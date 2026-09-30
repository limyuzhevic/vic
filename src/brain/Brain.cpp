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
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
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
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
        // Initialize memory systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
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
                    pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
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
     * This implements the complete integrated brain simulation through a series of focused operations:
     * Each step method handles one aspect of brain function, improving modularity and testability
     */
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Execute brain operation phases in sequence
    processDelayedSpikes(currentStep, currentTime);
    updateAllNeurons(currentTime);
    detectAndScheduleSpikes(currentStep, currentTime);
    updateWorkingMemory();
    applyNeuromodulation();
    applyPlasticityRules();
    updateEpisodicMemory();
    updatePredictionSystem();
    updateAttentionSystem();
    updateConceptFormation();
    applyStructuralPlasticity(currentStep);
    replayImportantMemories(currentStep);
    applyDevelopmentEffects(currentStep);
    consolidateMemory(currentStep);
    manageCheckpoints(currentStep, currentTime);
}

void Brain::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::updateAllNeurons(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::detectAndScheduleSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                detectSpikeAndScheduleEvents(neuron, region.get(), currentStep, currentTime);
            }
        }
    }
    
    pImpl->spikeSystem->processSpikes(currentStep);
}

void Brain::detectSpikeAndScheduleEvents(Neuron* neuron, NeuralRegion* region, 
                                       SimulationStep currentStep, Timestamp currentTime) {
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
            scheduleDelayedSpikeEvent(neuron, syn, currentStep, currentTime);
        }
        
        // Store to working memory - neurons that fire become part of working memory
        storeToWorkingMemory(neuron, state);
    }
}

void Brain::scheduleDelayedSpikeEvent(Neuron* neuron, Synapse* synapse,
                                    SimulationStep currentStep, Timestamp currentTime) {
    Delay delay = synapse->getDelay();
    SimulationStep deliveryStep = currentStep + delay;
    Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

    DelayedSpikeEvent delayedEvent(
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

    pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

    // Record pre-synaptic spike for plasticity
    synapse->recordPreSpike(currentTime);
}

void Brain::storeToWorkingMemory(Neuron* neuron, const NeuronState& state) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
    }
}

void Brain::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulation() {
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
        applyDopamineExcitabilityEffects();
    }
}

void Brain::applyDopamineExcitabilityEffects() {
    float dopamineLevel = pImpl->dopamine->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    
    if (excitabilityMod > 0.0f) {
        iterateAllNeurons([excitabilityMod](Neuron* neuron) {
            neuron->injectCurrent(excitabilityMod);
        });
    }
}

void Brain::applyPlasticityRules() {
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    iterateAllSynapses([this, plasticityMod](Synapse* syn) {
        applySingleSynapsePlasticity(syn, plasticityMod);
    });
}

void Brain::applySingleSynapsePlasticity(Synapse* synapse, float plasticityMod) {
    // Apply STDP with neuromodulation
    if (synapse->getPlasticityFlags().stdp) {
        const auto& preSpikes = synapse->getPreSpikeHistory();
        const auto& postSpikes = synapse->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            // Modify weight change based on dopamine
            pImpl->stdp->update(synapse, preSpikes, postSpikes, pImpl->timestep);
            float weight = synapse->getWeight();
            weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
            synapse->setWeight(weight);
        }
    }
    
    // Apply Hebbian learning
    if (synapse->getPlasticityFlags().hebbian) {
        const auto& preSpikes = synapse->getPreSpikeHistory();
        const auto& postSpikes = synapse->getPostSpikeHistory();
        
        if (!preSpikes.empty() && !postSpikes.empty()) {
            pImpl->hebbian->update(synapse, preSpikes, postSpikes, pImpl->timestep);
        }
    }
    
    // Update synapse state
    synapse->step(pImpl->currentTime);
}

void Brain::updateEpisodicMemory() {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            storeCurrentBrainStateAsEpisode();
        }
    }
}

void Brain::storeCurrentBrainStateAsEpisode() {
    EpisodicMemoryItem episode;
    episode.timestamp = pImpl->currentStep;
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

void Brain::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateAttentionSystem() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayImportantMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            adjustPlasticityRatesBasedOnDevelopment();
        }
    }
}

void Brain::adjustPlasticityRatesBasedOnDevelopment() {
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
    
    pImpl->structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
    pImpl->structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

void Brain::consolidateMemory(SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::manageCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

// Template methods for common iteration patterns

void Brain::iterateAllNeurons(std::function<void(Neuron*)> neuronFunc) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuronFunc(neuron);
            }
        }
    }
}

void Brain::iterateAllSynapses(std::function<void(Synapse*)> synapseFunc) {
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            synapseFunc(syn);
        }
    }
}

// Convenience methods for common operations

void Brain::injectCurrentToAllSensoryNeurons(MembranePotential current) {
    for (auto* neuron : pImpl->sensoryNeurons) {
        neuron->injectCurrent(current);
    }
}

void Brain::injectCurrentToAllMotorNeurons(MembranePotential current) {
    for (auto* neuron : pImpl->motorNeurons) {
        neuron->injectCurrent(current);
    }
}

std::vector<NeuronId> Brain::collectActiveNeuronIds() const {
    std::vector<NeuronId> activeNeurons;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getState().membranePotential > neuron->getState().threshold) {
                    activeNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    return activeNeurons;
}

float Brain::calculateAverageNeuronActivity() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalActivity = 0.0f;
    iterateAllNeurons([&totalActivity](Neuron* neuron) {
        const auto& state = neuron->getState();
        totalActivity += std::abs(state.membranePotential - state.restingPotential);
    });
    
    return totalActivity / totalNeurons;
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
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
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
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
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
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states
        size_t idx = 0;
        for (auto& region : pImpl->regions) {
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
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Apply synapse states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(synapseData.weight.size()) + " synapses");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

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

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

// Command pattern interfaces for brain operations
struct BrainOperation {
    virtual ~BrainOperation() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual bool canExecute() const = 0;
};

struct PlasticityCommand : public BrainOperation {
    std::shared_ptr<Brain> brain;
    float plasticityMod;
    
    PlasticityCommand(std::shared_ptr<Brain> b, float mod) : brain(b), plasticityMod(mod) {}
    
    void execute() override {
        brain->applyPlasticityRules();
    }
    
    void undo() override {
        // Could revert synaptic changes by storing previous state
        // For now, we can't undo complex neural changes
    }
    
    bool canExecute() const override {
        return brain && brain->getSTDP() != nullptr;
    }
};

struct MemoryCommand : public BrainOperation {
    std::shared_ptr<Brain> brain;
    std::string operation; // "working", "episodic", "consolidate"
    
    MemoryCommand(std::shared_ptr<Brain> b, const std::string& op) : brain(b), operation(op) {}
    
    void execute() override {
        if (operation == "working") {
            brain->updateWorkingMemory();
        } else if (operation == "episodic") {
            brain->updateEpisodicMemory();
        }
    }
    
    void undo() override {
        // Memory operations are generally not undone in neural systems
    }
    
    bool canExecute() const override {
        return brain != nullptr;
    }
};

struct NeuromodulationCommand : public BrainOperation {
    std::shared_ptr<Brain> brain;
    
    NeuromodulationCommand(std::shared_ptr<Brain> b) : brain(b) {}
    
    void execute() override {
        brain->applyNeuromodulation();
    }
    
    void undo() override {
        // Neuromodulation effects are typically not undone
    }
    
    bool canExecute() const override {
        return brain != nullptr;
    }
};

struct DevelopmentCommand : public BrainOperation {
    std::shared_ptr<Brain> brain;
    SimulationStep currentStep;
    
    DevelopmentCommand(std::shared_ptr<Brain> b, SimulationStep step) : brain(b), currentStep(step) {}
    
    void execute() override {
        brain->applyDevelopmentEffects(currentStep);
    }
    
    void undo() override {
        // Development effects are generally not undone
    }
    
    bool canExecute() const override {
        return brain != nullptr && brain->getDevelopmentSystem() != nullptr;
    }
};

// Command executor for brain operations
class BrainCommandExecutor {
private:
    std::shared_ptr<Brain> brain;
    std::vector<std::unique_ptr<BrainOperation>> commandHistory;
    
public:
    BrainCommandExecutor(std::shared_ptr<Brain> b) : brain(b) {}
    
    template<typename T, typename... Args>
    void executeCommand(Args&&... args) {
        auto command = std::make_unique<T>(brain, std::forward<Args>(args)...);
        if (command && command->canExecute()) {
            command->execute();
            commandHistory.push_back(std::move(command));
        } else {
            NLM_LOG_ERROR("Command cannot be executed");
        }
    }
    
    void undoLastCommand() {
        if (!commandHistory.empty()) {
            auto& lastCommand = commandHistory.back();
            lastCommand->undo();
            commandHistory.pop_back();
        }
    }
    
    size_t getCommandHistorySize() const {
        return commandHistory.size();
    }
    
    void clearCommandHistory() {
        commandHistory.clear();
    }
};

// Enhanced step method with command execution
void Brain::stepWithCommands(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Create command executor for this step
    BrainCommandExecutor executor(shared_from_this());
    
    // Execute operation commands in sequence
    executor.executeCommand<NeuromodulationCommand>();
    executor.executeCommand<PlasticityCommand>(1.0f);
    executor.executeCommand<MemoryCommand>("working");
    executor.executeCommand<MemoryCommand>("episodic");
    
}

}

// Configuration constants for better configurability
namespace BrainConfig {
    // Neural parameters
    constexpr float DEFAULT_LEAK_CONDUCTANCE = -70.0f;      // mV
    constexpr float DEFAULT_THRESHOLD = -55.0f;            // mV
    constexpr float DEFAULT_RESET_POTENTIAL = -65.0f;      // mV
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;   // nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;         // ms
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;       // Learning rate for LTP
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;      // Learning rate for LTD
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;    // STDP tau
    constexpr float DEFAULT_HEBBIAN_STRENGTH = 0.005f;     // Hebbian learning rate
    
    // Structural plasticity parameters
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f; // Rate of new synapse formation
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;       // Rate of synapse elimination
    
    // Memory parameters
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 1000;      // Number of memory traces
    constexpr size_t DEFAULT_MAX_EPISODIC_EPISODES = 10000;        // Maximum stored episodes
    constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.3f;        // Memory consolidation threshold
    
    // Neuromodulation parameters
    constexpr float DEFAULT_DOPAMINE_MAX_LEVEL = 1.0f;     // Maximum dopamine level
    constexpr float DEFAULT_DOPAMINE_DECAY = 0.95f;        // Dopamine decay rate
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.1f;      // Novelty detection threshold
    
    // Simulation parameters
    constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;  // Time step (1ms)
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;        // Steps between memory replay
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Steps between memory consolidation
    
    // Development parameters
    constexpr float DEVELOPMENT_FACTOR_INITIAL = 1.0f;     // Initial plasticity factor
    constexpr float DEVELOPMENT_FACTOR_CRITICAL = 0.8f;   // Critical period factor
    constexpr float DEVELOPMENT_FACTOR_MATURATION = 0.5f;  // Maturation factor
    constexpr float DEVELOPMENT_FACTOR_ADULT = 0.2f;       // Adult stability factor
    
    // Performance parameters
    constexpr size_t CHECKPOINT_FREQUENCY = 10000;         // Steps between checkpoints
    constexpr size_t MAX_CHECKPOINT_SIZE = 5;              // Maximum checkpoints to keep
    constexpr float CHECKPOINT_COMPRESSION_RATIO = 0.7f;   // Checkpoint compression
}

// Enhanced error handling and validation framework
class BrainErrorHandler {
private:
    enum class ErrorType {
        INITIALIZATION_FAILED,
        MEMORY_ALLOCATION_FAILED,
        CONFIG_INVALID,
        SYSTEM_NOT_INITIALIZED,
        NEURON_LIMIT_EXCEEDED,
        SYNAPSE_LIMIT_EXCEEDED,
        PLASTICITY_FAILED,
        NETWORK_UNCONNECTED,
        MEMORY_CORRUPTION,
        CHECKPOINT_FAILED
    };
    
    std::shared_ptr<Brain> brain;
    std::vector<std::pair<ErrorType, std::string>> errorLog;
    bool continueOnError;
    
public:
    BrainErrorHandler(std::shared_ptr<Brain> b, bool continueOnErr = false)
        : brain(b), continueOnError(continueOnErr) {}
    
    template<typename F>
    bool executeSafely(F&& operation, const std::string& operationName) {
        try {
            if (!validateOperation(operationName)) {
                logError(ErrorType::SYSTEM_NOT_INITIALIZED, "Operation not validated: " + operationName);
                return false;
            }
            
            operation();
            logInfo("Operation completed successfully: " + operationName);
            return true;
        } catch (const std::bad_alloc& e) {
            logError(ErrorType::MEMORY_ALLOCATION_FAILED, "Memory allocation failed in " + operationName + ": " + std::string(e.what()));
            return !continueOnError;
        } catch (const std::exception& e) {
            logError(ErrorType::INITIALIZATION_FAILED, "Exception in " + operationName + ": " + std::string(e.what()));
            return !continueOnError;
        } catch (...) {
            logError(ErrorType::MEMORY_CORRUPTION, "Unknown error in " + operationName);
            return !continueOnError;
        }
    }
    
private:
    bool validateOperation(const std::string& operationName) const {
        if (!brain) {
            logError(ErrorType::SYSTEM_NOT_INITIALIZED, "Brain system not initialized for operation: " + operationName);
            return false;
        }
        
        if (operationName.find("neuron") != std::string::npos && 
            brain->getTotalNeuronCount() == 0) {
            logError(ErrorType::NEURON_LIMIT_EXCEEDED, "No neurons available for operation: " + operationName);
            return false;
        }
        
        if (operationName.find("synapse") != std::string::npos && 
            brain->getTotalSynapseCount() == 0) {
            logError(ErrorType::SYNAPSE_LIMIT_EXCEEDED, "No synapses available for operation: " + operationName);
            return false;
        }
        
        return true;
    }
    
    void logError(ErrorType type, const std::string& message) {
        errorLog.push_back({type, message});
        NLM_LOG_ERROR("[ERROR] " + message);
    }
    
    void logInfo(const std::string& message) {
        NLM_LOG_INFO("[INFO] " + message);
    }
    
public:
    void printErrorSummary() const {
        if (errorLog.empty()) {
            NLM_LOG_INFO("No errors encountered");
            return;
        }
        
        NLM_LOG_INFO("=== ERROR SUMMARY ===");
        for (const auto& error : errorLog) {
            NLM_LOG_INFO("  Type: " + std::to_string(static_cast<int>(error.first)) + 
                        " Message: " + error.second);
        }
    }
    
    size_t getErrorCount() const {
        return errorLog.size();
    }
    
    bool hasErrors() const {
        return !errorLog.empty();
    }
    
    void clearErrors() {
        errorLog.clear();
    }
};

// Abstract interfaces for better separation of concerns
class IBrainSystem {
public:
    virtual ~IBrainSystem() = default;
    virtual bool initialize() = 0;
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    virtual bool isHealthy() const = 0;
    virtual std::string getStatus() const = 0;
};

class IBrainComponent {
public:
    virtual ~IBrainComponent() = default;
    virtual void initializeComponent(std::shared_ptr<Brain> brain) = 0;
    virtual void updateComponent(double timestep) = 0;
    virtual void resetComponent() = 0;
    virtual bool validateComponent() const = 0;
    virtual std::string getComponentStatus() const = 0;
};

class IPlasticityRule : public IBrainSystem {
public:
    virtual void applyPlasticity(Synapse* synapse, const std::vector<Timestamp>& preSpikes, 
                               const std::vector<Timestamp>& postSpikes, Timestamp currentTime) = 0;
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;
};

class IMemorySystem : public IBrainSystem {
public:
    virtual void storeExperience(const ExperienceData& data) = 0;
    virtual std::vector<ExperienceData> retrieveExperience(size_t maxCount) = 0;
    virtual void consolidateMemory(float threshold) = 0;
    virtual size_t getMemoryCapacity() const = 0;
};

class INeuromodulationSystem : public IBrainSystem {
public:
    virtual float getModulationLevel() const = 0;
    virtual void setModulationLevel(float level) = 0;
    virtual float getPlasticityFactor() const = 0;
    virtual void applyModulation(float amount) = 0;
};

}

// Configuration constants for better configurability
namespace BrainConfig {
    // Neural parameters
    constexpr float DEFAULT_LEAK_CONDUCTANCE = -70.0f;      // mV
    constexpr float DEFAULT_THRESHOLD = -55.0f;            // mV
    constexpr float DEFAULT_RESET_POTENTIAL = -65.0f;      // mV
    constexpr float DEFAULT_MEMBRANE_CAPACITANCE = 1.0f;   // nF
    constexpr float DEFAULT_TIME_CONSTANT = 20.0f;         // ms
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;       // Learning rate for LTP
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;      // Learning rate for LTD
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;    // STDP tau
    constexpr float DEFAULT_HEBBIAN_STRENGTH = 0.005f;     // Hebbian learning rate
    
    // Structural plasticity parameters
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f; // Rate of new synapse formation
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;       // Rate of synapse elimination
    
    // Memory parameters
    constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 1000;      // Number of memory traces
    constexpr size_t DEFAULT_MAX_EPISODIC_EPISODES = 10000;        // Maximum stored episodes
    constexpr float DEFAULT_CONSOLIDATION_THRESHOLD = 0.3f;        // Memory consolidation threshold
    
    // Neuromodulation parameters
    constexpr float DEFAULT_DOPAMINE_MAX_LEVEL = 1.0f;     // Maximum dopamine level
    constexpr float DEFAULT_DOPAMINE_DECAY = 0.95f;        // Dopamine decay rate
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.1f;      // Novelty detection threshold
    
    // Simulation parameters
    constexpr float DEFAULT_SIMULATION_TIMESTEP = 0.001f;  // Time step (1ms)
    constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;        // Steps between memory replay
    constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // Steps between memory consolidation
    
    // Development parameters
    constexpr float DEVELOPMENT_FACTOR_INITIAL = 1.0f;     // Initial plasticity factor
    constexpr float DEVELOPMENT_FACTOR_CRITICAL = 0.8f;   // Critical period factor
    constexpr float DEVELOPMENT_FACTOR_MATURATION = 0.5f;  // Maturation factor
    constexpr float DEVELOPMENT_FACTOR_ADULT = 0.2f;       // Adult stability factor
    
    // Performance parameters
    constexpr size_t CHECKPOINT_FREQUENCY = 10000;         // Steps between checkpoints
    constexpr size_t MAX_CHECKPOINT_SIZE = 5;              // Maximum checkpoints to keep
    constexpr float CHECKPOINT_COMPRESSION_RATIO = 0.7f;   // Checkpoint compression
}

// Strategy pattern implementation for neural computation components

// Neuron dynamics strategy interface
class INeuronDynamicsStrategy {
public:
    virtual ~INeuronDynamicsStrategy() = default;
    virtual bool update(Neuron& neuron, Timestamp currentTime, TimestepDuration dt) = 0;
    virtual void configure(const NeuronConfig& config) = 0;
    virtual std::string getStrategyName() const = 0;
    virtual float getThreshold() const = 0;
    virtual void setThreshold(float threshold) = 0;
};

// LIF (Leaky Integrate-and-Fire) strategy implementation
class LIFNeuronDynamicsStrategy : public INeuronDynamicsStrategy {
private:
    float leakConductance;
    float threshold;
    float resetPotential;
    float restingPotential;
    float membraneCapacitance;
    float timeConstant;
    
public:
    LIFNeuronDynamicsStrategy() : leakConductance(-70.0f), threshold(-55.0f), 
                                  resetPotential(-65.0f), restingPotential(-70.0f),
                                  membraneCapacitance(1.0f), timeConstant(20.0f) {}
    
    bool update(Neuron& neuron, Timestamp currentTime, TimestepDuration dt) override {
        NeuronState& state = neuron.getState();
        
        // LIF dynamics: Leaky Integrate-and-Fire
        // dV/dt = (V_rest - V)/tau + I/C
        // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
        
        float V = state.membranePotential;
        float V_rest = restingPotential;
        float V_reset = resetPotential;
        float I = neuron.getMembranePotential(); // Total current input
        
        // Leak contribution
        float leakContribution = (V_rest - V) / timeConstant;
        
        // Synaptic contribution
        float synapticContribution = I / membraneCapacitance;
        
        // Update membrane potential using exponential Euler integration
        V = V + dt * (leakContribution + synapticContribution);
        
        // Clamp membrane potential to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= threshold) {
            state.firingState = FiringState::Active;
            state.lastSpikeTime = currentTime;
            
            // Record spike
            neuron.recordSpike(currentTime);
            
            // Reset membrane potential
            V = V_reset;
            
            // Enter refractory period
            state.refractoryRemaining = state.refractoryPeriod;
            state.firingState = FiringState::Refractory;
            
            // Update adaptation for spike-frequency adaptation
            state.adaptationVariable += 1.0f;
            
            return true; // Neuron fired
        } else {
            state.firingState = FiringState::Active;
            state.membranePotential = V;
            return false; // No spike
        }
    }
    
    void configure(const NeuronConfig& config) override {
        if (config.restingPotential.has_value()) restingPotential = config.restingPotential.value();
        if (config.threshold.has_value()) threshold = config.threshold.value();
        if (config.resetPotential.has_value()) resetPotential = config.resetPotential.value();
        if (config.leakConductance.has_value()) leakConductance = config.leakConductance.value();
        if (config.membraneCapacitance.has_value()) membraneCapacitance = config.membraneCapacitance.value();
        if (config.timeConstant.has_value()) timeConstant = config.timeConstant.value();
    }
    
    std::string getStrategyName() const override {
        return "LIF";
    }
    
    float getThreshold() const override {
        return threshold;
    }
    
    void setThreshold(float thresh) override {
        threshold = thresh;
    }
    
    // Getters for LIF parameters
    float getRestingPotential() const { return restingPotential; }
    float getResetPotential() const { return resetPotential; }
    float getLeakConductance() const { return leakConductance; }
    float getMembraneCapacitance() const { return membraneCapacitance; }
    float getTimeConstant() const { return timeConstant; }
};

// Adaptive LIF strategy (with spike-frequency adaptation)
class AdaptiveLIFNeuronDynamicsStrategy : public LIFNeuronDynamicsStrategy {
private:
    float adaptationStrength;
    float adaptationDecay;
    
public:
    AdaptiveLIFNeuronDynamicsStrategy() : adaptationStrength(0.1f), adaptationDecay(0.95f) {}
    
    bool update(Neuron& neuron, Timestamp currentTime, TimestepDuration dt) override {
        bool fired = LIFNeuronDynamicsStrategy::update(neuron, currentTime, dt);
        
        if (fired) {
            // Apply spike-frequency adaptation
            NeuronState& state = neuron.getState();
            state.membranePotential -= state.adaptationVariable * adaptationStrength;
            state.adaptationVariable *= adaptationDecay;
        }
        
        return fired;
    }
    
    std::string getStrategyName() const override {
        return "Adaptive LIF";
    }
};

// Neuron dynamics strategy factory
class NeuronDynamicsStrategyFactory {
public:
    static std::unique_ptr<INeuronDynamicsStrategy> createStrategy(const std::string& strategyName) {
        if (strategyName == "LIF") {
            return std::make_unique<LIFNeuronDynamicsStrategy>();
        } else if (strategyName == "Adaptive LIF") {
            return std::make_unique<AdaptiveLIFNeuronDynamicsStrategy>();
        } else {
            // Default to LIF
            return std::make_unique<LIFNeuronDynamicsStrategy>();
        }
    }
};

// Plasticity rule strategy interface
class IPlasticityRuleStrategy {
public:
    virtual ~IPlasticityRuleStrategy() = default;
    virtual void applyPlasticity(Synapse& synapse, float preSpikeTime, float postSpikeTime, 
                               float learningRate, Timestamp currentTime) = 0;
    virtual std::string getRuleName() const = 0;
    virtual void configure(float learningRate, float timeConstant) = 0;
};

// STDP (Spike-Timing-Dependent Plasticity) strategy
class STDPPlasticityRuleStrategy : public IPlasticityRuleStrategy {
private:
    float ltpWeight;
    float ltdWeight;
    float tau;
    
public:
    STDPPlasticityRuleStrategy() : ltpWeight(0.01f), ltdWeight(0.012f), tau(20.0f) {}
    
    void applyPlasticity(Synapse& synapse, float preSpikeTime, float postSpikeTime,
                        float learningRate, Timestamp currentTime) override {
        float timeDifference = postSpikeTime - preSpikeTime;
        
        if (std::abs(timeDifference) < 1000.0f) { // Only consider spikes within 1000ms
            if (timeDifference > 0) {
                // LTP: Post before pre = long-term potentiation
                float weightChange = ltpWeight * std::exp(-std::abs(timeDifference) / tau);
                float newWeight = synapse.getWeight() + weightChange;
                synapse.setWeight(newWeight);
            } else {
                // LTD: Pre before post = long-term depression
                float weightChange = -ltdWeight * std::exp(-std::abs(timeDifference) / tau);
                float newWeight = synapse.getWeight() + weightChange;
                synapse.setWeight(newWeight);
            }
        }
    }
    
    std::string getRuleName() const override {
        return "STDP";
    }
    
    void configure(float learningRate, float timeConstant) override {
        ltpWeight = learningRate;
        ltdWeight = learningRate * 1.2f; // LTD typically slightly stronger
        tau = timeConstant;
    }
    
    // Getters
    float getLTPWeight() const { return ltpWeight; }
    float getLTDWeight() const { return ltdWeight; }
    float getTau() const { return tau; }
};

// Hebbian learning strategy
class HebbianPlasticityRuleStrategy : public IPlasticityRuleStrategy {
private:
    float strength;
    float threshold;
    
public:
    HebbianPlasticityRuleStrategy() : strength(0.005f), threshold(0.1f) {}
    
    void applyPlasticity(Synapse& synapse, float preSpikeTime, float postSpikeTime,
                        float learningRate, Timestamp currentTime) override {
        // Simple Hebbian: strengthen if both pre and post spiked
        if (preSpikeTime > 0 && postSpikeTime > 0) {
            float preDelta = currentTime - preSpikeTime;
            float postDelta = currentTime - postSpikeTime;
            
            // Both neurons fired recently - strengthen connection
            if (preDelta < 50.0f && postDelta < 50.0f) {
                float weightChange = strength * learningRate;
                float newWeight = synapse.getWeight() + weightChange;
                synapse.setWeight(newWeight);
            }
        }
    }
    
    std::string getRuleName() const override {
        return "Hebbian";
    }
    
    void configure(float learningRate, float timeConstant) override {
        strength = learningRate;
        threshold = timeConstant * 0.01f;
    }
};

// Plasticity rule strategy factory
class PlasticityRuleStrategyFactory {
public:
    static std::unique_ptr<IPlasticityRuleStrategy> createStrategy(const std::string& ruleName) {
        if (ruleName == "STDP") {
            return std::make_unique<STDPPlasticityRuleStrategy>();
        } else if (ruleName == "Hebbian") {
            return std::make_unique<HebbianPlasticityRuleStrategy>();
        } else {
            // Default to STDP
            return std::make_unique<STDPPlasticityRuleStrategy>();
        }
    }
};

// Memory consolidation strategy interface
class IMemoryConsolidationStrategy {
public:
    virtual ~IMemoryConsolidationStrategy() = default;
    virtual void consolidate(WorkingMemory& memory, float threshold) = 0;
    virtual void replay(EpisodicMemory& memory, size_t maxEpisodes) = 0;
    virtual std::string getStrategyName() const = 0;
};

// Standard memory consolidation strategy
class StandardMemoryConsolidationStrategy : public IMemoryConsolidationStrategy {
public:
    void consolidate(WorkingMemory& memory, float threshold) override {
        // Remove traces with low activation strength
        auto traces = memory.getTraces();
        for (auto it = traces.begin(); it != traces.end(); ) {
            if ((*it)->getStrength() < threshold) {
                it = traces.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void replay(EpisodicMemory& memory, size_t maxEpisodes) override {
        auto episodes = memory.getEpisodes();
        // Sort episodes by importance/relevance
        std::sort(episodes.begin(), episodes.end(),
                 [](const ExperienceData* a, const ExperienceData* b) {
                     return a->importance > b->importance;
                 });
        
        // Replay most important episodes
        size_t replayCount = std::min(maxEpisodes, episodes.size());
        for (size_t i = 0; i < replayCount; ++i) {
            memory.replayEpisode(episodes[i]);
        }
    }
    
    std::string getStrategyName() const override {
        return "Standard";
    }
};

// Memory consolidation strategy factory
class MemoryConsolidationStrategyFactory {
public:
    static std::unique_ptr<IMemoryConsolidationStrategy> createStrategy(const std::string& strategyName) {
        if (strategyName == "Standard") {
            return std::make_unique<StandardMemoryConsolidationStrategy>();
        } else {
            // Default to Standard
            return std::make_unique<StandardMemoryConsolidationStrategy>();
        }
    }
};

// Template methods for common iteration patterns
void Brain::iterateAllNeurons(std::function<void(Neuron*)> neuronFunc) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuronFunc(neuron);
            }
        }
    }
}

void Brain::iterateAllSynapses(std::function<void(Synapse*)> synapseFunc) {
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            synapseFunc(syn);
        }
    }
}

// Convenience methods for common operations
void Brain::injectCurrentToAllSensoryNeurons(MembranePotential current) {
    for (auto* neuron : pImpl->sensoryNeurons) {
        neuron->injectCurrent(current);
    }
}

void Brain::injectCurrentToAllMotorNeurons(MembranePotential current) {
    for (auto* neuron : pImpl->motorNeurons) {
        neuron->injectCurrent(current);
    }
}

std::vector<NeuronId> Brain::collectActiveNeuronIds() const {
    std::vector<NeuronId> activeNeurons;
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring() || neuron->getState().membranePotential > neuron->getState().threshold) {
                    activeNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    return activeNeurons;
}

float Brain::calculateAverageNeuronActivity() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    float totalActivity = 0.0f;
    iterateAllNeurons([&totalActivity](Neuron* neuron) {
        const auto& state = neuron->getState();
        totalActivity += std::abs(state.membranePotential - state.restingPotential);
    });
    
    return totalActivity / totalNeurons;
}

} // namespace nlm
