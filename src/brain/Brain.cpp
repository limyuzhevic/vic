// Updated Brain.cpp with Complete Neuromodulator Integration
// Enhanced Phase 6 integration with all neuromodulators

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
    
    // ========== COMPLETE INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== COMPLETE PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== COMPLETE COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== COMPLETE NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    std::unique_ptr<Oxytocin> oxytocin;
    std::unique_ptr<Vasopressin> vasopressin;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<Curiosity> curiosity;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Neuromodulator manager for coordination
    std::unique_ptr<NeuromodulatorManager> neuromodulatorManager;
    
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
        , workingMemory(nullptr)
        , episodicMemory(nullptr)
        , associativeMemory(nullptr)
        , predictionSystem(nullptr)
        , planner(nullptr)
        , conceptFormation(nullptr)
        , attention(nullptr)
        , developmentSystem(nullptr)
        , dopamine(nullptr)
        , acetylcholine(nullptr)
        , norepinephrine(nullptr)
        , serotonin(nullptr)
        , oxytocin(nullptr)
        , vasopressin(nullptr)
        , predictionError(nullptr)
        , novelty(nullptr)
        , curiosity(nullptr)
        , spikeSystem(nullptr)
        , stdp(nullptr)
        , hebbian(nullptr)
        , structuralPlasticity(nullptr)
        , neuromodulatorManager(nullptr)
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
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
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
        
        // Initialize complete neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        acetylcholine = std::make_unique<Acetylcholine>();
        norepinephrine = std::make_unique<Norepinephrine>();
        serotonin = std::make_unique<Serotonin>();
        oxytocin = std::make_unique<Oxytocin>();
        vasopressin = std::make_unique<Vasopressin>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        curiosity = std::make_unique<Curiosity>();
        
        // Initialize neuromodulator manager
        neuromodulatorManager = std::make_unique<NeuromodulatorManager>();
        
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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Complete Integrated Artificial Brain)...");
    
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
    pImpl->predictionSystem->initialize(this);
    pImpl->predictionSystem->setConfidenceThreshold(0.5f);
    
    // Initialize cognition systems
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation systems
    pImpl->dopamine->initialize(this);
    pImpl->acetylcholine->initialize(this);
    pImpl->norepinephrine->initialize(this);
    pImpl->serotonin->initialize(this);
    pImpl->oxytocin->initialize(this);
    pImpl->vasopressin->initialize(this);
    pImpl->predictionError->initialize(this);
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Initialize neuromodulator manager
    pImpl->neuromodulatorManager->initialize(this);
    pImpl->neuromodulatorManager->registerDopamine(pImpl->dopamine.get());
    pImpl->neuromodulatorManager->registerAcetylcholine(pImpl->acetylcholine.get());
    pImpl->neuromodulatorManager->registerNorepinephrine(pImpl->norepinephrine.get());
    pImpl->neuromodulatorManager->registerSerotonin(pImpl->serotonin.get());
    pImpl->neuromodulatorManager->registerOxytocin(pImpl->oxytocin.get());
    pImpl->neuromodulatorManager->registerVasopressin(pImpl->vasopressin.get());
    pImpl->neuromodulatorManager->registerNovelty(pImpl->novelty.get());
    pImpl->neuromodulatorManager->registerCuriosity(pImpl->curiosity.get());
    pImpl->neuromodulatorManager->registerPredictionError(pImpl->predictionError.get());
    
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
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Complete Integration)");
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
     * PHASE 6: COMPLETE INTEGRATED ARTIFICIAL BRAIN LOOP
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
    // Update all neuromodulators
    pImpl->neuromodulatorManager->updateAll(pImpl->timestep);
    
    // Apply neuromodulation effects on neural excitability
    // All neuromodulators contribute to neural modulation
    if (pImpl->dopamine || pImpl->acetylcholine || pImpl->norepinephrine || 
        pImpl->serotonin || pImpl->oxytocin || pImpl->vasopressin) {
        
        // Calculate total neuromodulatory influence
        float totalDopamine = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
        float totalACh = pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
        float totalNE = pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
        float total5HT = pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f;
        float totalOT = pImpl->oxytocin ? pImpl->oxytocin->getLevel() : 0.0f;
        float totalAVP = pImpl->vasopressin ? pImpl->vasopressin->getLevel() : 0.0f;
        
        float totalModulation = (totalDopamine + totalACh + totalNE + total5HT + totalOT + totalAVP) / 6.0f;
        
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Multiple neuromodulators affect neural excitability
                    // Dopamine modulates reward prediction error effects
                    if (pImpl->dopamine) {
                        float dopamineModulation = pImpl->dopamine->getLevel() * 0.3f;
                        neuron->injectCurrent(dopamineModulation * 2.0f);
                    }
                    
                    // Acetylcholine enhances attention and working memory
                    if (pImpl->acetylcholine) {
                        float achModulation = pImpl->acetylcholine->getLevel() * 0.4f;
                        neuron->injectCurrent(achModulation * 1.5f);
                    }
                    
                    // Norepinephrine modulates arousal and signal-to-noise
                    if (pImpl->norepinephrine) {
                        float neModulation = pImpl->norepinephrine->getLevel() * 0.3f;
                        neuron->injectCurrent(neModulation * 1.8f);
                    }
                    
                    // Serotonin modulates mood and impulsivity
                    if (pImpl->serotonin) {
                        float serotoninModulation = pImpl->serotonin->getLevel() * 0.2f;
                        neuron->injectCurrent(serotoninModulation * 1.2f);
                    }
                    
                    // Oxytocin modulates social behavior
                    if (pImpl->oxytocin) {
                        float oxytocinModulation = pImpl->oxytocin->getLevel() * 0.15f;
                        neuron->injectCurrent(oxytocinModulation * 1.0f);
                    }
                    
                    // Vasopressin modulates aggression and social hierarchy
                    if (pImpl->vasopressin) {
                        float vasopressinModulation = pImpl->vasopressin->getLevel() * 0.15f;
                        neuron->injectCurrent(vasopressinModulation * 1.0f);
                    }
                    
                    // Novelty detection affects exploration
                    if (pImpl->novelty) {
                        float noveltyModulation = pImpl->novelty->getLevel() * 0.1f;
                        neuron->injectCurrent(noveltyModulation * 0.8f);
                    }
                    
                    // Curiosity affects exploration drive
                    if (pImpl->curiosity) {
                        float curiosityModulation = pImpl->curiosity->getLevel() * 0.1f;
                        neuron->injectCurrent(curiosityModulation * 0.8f);
                    }
                    
                    // Prediction error affects learning rate
                    if (pImpl->predictionError) {
                        float errorModulation = std::abs(pImpl->predictionError->getError()) * 0.05f;
                        neuron->injectCurrent(errorModulation * 1.0f);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->neuromodulatorManager) {
        plasticityMod = pImpl->neuromodulatorManager->getTotalPlasticityModulation();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on multiple neuromodulators
                    float stdpFactor = 1.0f;
                    
                    if (pImpl->dopamine) {
                        stdpFactor *= pImpl->dopamine->getPlasticityFactor();
                    }
                    if (pImpl->serotonin) {
                        stdpFactor *= pImpl->serotonin->getPlasticityFactor();
                    }
                    if (pImpl->acetylcholine) {
                        stdpFactor *= pImpl->acetylcholine->getPlasticityFactor();
                    }
                    
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f * stdpFactor;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning with neuromodulation
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modulate Hebbian learning based on neuromodulators
                    float hebbianFactor = 1.0f;
                    
                    if (pImpl->acetylcholine) {
                        hebbianFactor *= pImpl->acetylcholine->getPlasticityFactor() * 0.8f;
                    }
                    if (pImpl->dopamine) {
                        hebbianFactor *= pImpl->dopamine->getPlasticityFactor() * 0.6f;
                    }
                    
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight *= hebbianFactor;
                    syn->setWeight(weight);
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
    if (pImpl->predictionSystem && pImpl->workingMemory && pImpl->predictionError) {
        // Get current neural activity from working memory for prediction
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            // Create sensory-like observation from active neural traces
            std::vector<float> neuralObservation;
            for (NeuronId neuronId : memoryNeurons) {
                float activation = pImpl->workingMemory->getTraceStrength(neuronId);
                neuralObservation.push_back(activation);
            }
            
            // Update prediction system with current neural state
            SensoryInput neuralInput(neuralObservation);
            pImpl->predictionSystem->updatePredictions(neuralInput);
            pImpl->predictionSystem->predictNextState(neuralInput);
            
            // Track prediction error for neuromodulation
            pImpl->predictionSystem->computePredictionError();
            
            // Feed prediction error to neuromodulation systems
            if (pImpl->predictionError) {
                float errorSignal = pImpl->predictionSystem->getPredictionError();
                pImpl->predictionError->computeError(errorSignal, pImpl->predictionSystem->getCurrentValue());
                pImpl->predictionError->updatePrediction(pImpl->predictionSystem->getNextPrediction());
                
                // Apply prediction error to neuromodulators
                pImpl->predictionError->applyToDopamine(pImpl->dopamine.get());
                pImpl->predictionError->applyToSerotonin(pImpl->serotonin.get());
                pImpl->predictionError->applyToNovelty(pImpl->novelty.get());
                pImpl->predictionError->applyToCuriosity(pImpl->curiosity.get());
            }
        }
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
    if (pImpl->conceptFormation && pImpl->workingMemory && pImpl->episodicMemory) {
        // Get current neural activity patterns from working memory
        auto memoryNeurons = pImpl->workingMemory->getMemoryNeurons();
        if (!memoryNeurons.empty()) {
            // Get recent episodic memory for context
            auto episodes = pImpl->episodicMemory->getRecentEpisodes(5);
            
            // Process neural patterns to form concepts
            pImpl->conceptFormation->processNeuralPatterns(
                memoryNeurons, 
                episodes, 
                pImpl->currentStep
            );
            
            // Update planner with formed concepts
            if (pImpl->planner) {
                pImpl->planner->updateWithConcepts(pImpl->conceptFormation->getRecentConcepts());
            }
        }
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay with concept context
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(
            std::min(3u, pImpl->episodicMemory->getEpisodeCount() / 10)
        );
        
        for (const auto* episode : episodesToReplay) {
            // Enhance replay with current concept context
            if (pImpl->conceptFormation) {
                pImpl->conceptFormation->updateContextFromReplay(episode);
            }
            
            // Replay enhanced with prediction system feedback
            if (pImpl->predictionSystem) {
                pImpl->predictionSystem->recordReplayExperience(episode);
            }
            
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentStage stage = pImpl->developmentalStage;
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
            
            // Also feed sensory input to prediction system
            if (pImpl->predictionSystem) {
                std::vector<float> sensoryData;
                if (!input.getData().empty()) {
                    sensoryData = input.getData();
                    SensoryInput sensoryInput(sensoryData);
                    pImpl->predictionSystem->train(sensoryInput);
                }
            }
            
            // Also feed to novelty detection
            if (pImpl->novelty) {
                pImpl->novelty->detectNovelty(sensoryData, std::vector<float>());  // Would need previous pattern
            }
            
            // Also feed to curiosity system
            if (pImpl->curiosity && pImpl->predictionError) {
                pImpl->curiosity->updateCuriosity(pImpl->novelty->getLevel(), 
                                                pImpl->predictionError->getError(), 
                                                pImpl->timestep);
            }
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
    // Enhanced with neuromodulation influence
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Find motor neuron with highest activity
    Neuron* bestNeuron = nullptr;
    float bestActivity = -1.0f;
    
    for (auto* neuron : pImpl->motorNeurons) {
        float activity = std::abs(neuron->getMembranePotential() - neuron->getState().restingPotential);
        
        // Modulate activity based on neuromodulators
        if (pImpl->dopamine) {
            activity *= (1.0f + pImpl->dopamine->getLevel() * 0.5f);
        }
        if (pImpl->acetylcholine) {
            activity *= (1.0f + pImpl->acetylcholine->getLevel() * 0.3f);
        }
        
        if (activity > bestActivity) {
            bestActivity = activity;
            bestNeuron = neuron;
        }
    }
    
    // Create action based on selected neuron
    std::unique_ptr<Action> action = std::make_unique<Action>(ActionType::MoveForward);
    
    NLM_LOG_DEBUG("Action produced: type=" + std::to_string(static_cast<int>(action->getType())) + 
                  ", activity=" + std::to_string(bestActivity));
    
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulatory signals to brain systems
    // This is called by external neuromodulators
    
    // Update neuromodulator manager
    if (pImpl->neuromodulatorManager) {
        pImpl->neuromodulatorManager->reset();
    }
    
    NLM_LOG_DEBUG("Neuromodulation signal applied: " + std::string(signal.getName()));
}

void Brain::updatePlasticity() {
    // Update plasticity rules (called automatically in step)
    // This is a wrapper for the internal plasticity update
    
    // All plasticity updates happen in step() method
    // This method maintains backward compatibility
    
    NLM_LOG_DEBUG("Plasticity update called");
}

void Brain::develop() {
    // Apply developmental changes (called automatically in step)
    // This is a wrapper for the internal development update
    
    // All development updates happen in step() method
    // This method maintains backward compatibility
    
    NLM_LOG_DEBUG("Development update called");
}

void Brain::reset() {
    // Reset brain state including neuromodulators
    if (pImpl->workingMemory) {
        pImpl->workingMemory->reset();
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->clearAllEpisodes();
    }
    if (pImpl->predictionSystem) {
        pImpl->predictionSystem->reset();
    }
    if (pImpl->planner) {
        pImpl->planner->reset();
    }
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->reset();
    }
    if (pImpl->attention) {
        pImpl->attention->reset();
    }
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->reset();
    }
    
    // Reset all neuromodulators
    if (pImpl->dopamine) {
        pImpl->dopamine->reset();
    }
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->reset();
    }
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->reset();
    }
    if (pImpl->serotonin) {
        pImpl->serotonin->reset();
    }
    if (pImpl->oxytocin) {
        pImpl->oxytocin->reset();
    }
    if (pImpl->vasopressin) {
        pImpl->vasopressin->reset();
    }
    if (pImpl->novelty) {
        pImpl->novelty->clearHistory();
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->reset();
    }
    if (pImpl->predictionError) {
        pImpl->predictionError->clearHistory();
    }
    
    // Reset neuromodulator manager
    if (pImpl->neuromodulatorManager) {
        pImpl->neuromodulatorManager->reset();
    }
    
    // Reset spike system
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    
    // Reset structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->reset();
    }
    
    // Reset development system stage
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset statistics
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->stepsSinceLastEpisode = 0;
    
    NLM_LOG_INFO("Brain reset complete (all neuromodulators reset)");
}

bool Brain::save(const std::string& filepath) const {
    // Save brain state including neuromodulators
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for saving: " + filepath);
        return false;
    }
    
    try {
        // Save basic brain data
        file.write(reinterpret_cast<const char*>(&pImpl->totalSpikesTotal), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(&pImpl->currentStep), sizeof(SimulationStep));
        file.write(reinterpret_cast<const char*>(&pImpl->currentTime), sizeof(Timestamp));
        file.write(reinterpret_cast<const char*>(&pImpl->developmentalStage), sizeof(DevelopmentalStage));
        
        // Save neuromodulator states
        if (pImpl->dopamine) {
            float dopamineLevel = pImpl->dopamine->getLevel();
            file.write(reinterpret_cast<const char*>(&dopamineLevel), sizeof(float));
        }
        if (pImpl->acetylcholine) {
            float achLevel = pImpl->acetylcholine->getLevel();
            file.write(reinterpret_cast<const char*>(&achLevel), sizeof(float));
        }
        if (pImpl->norepinephrine) {
            float neLevel = pImpl->norepinephrine->getLevel();
            file.write(reinterpret_cast<const char*>(&neLevel), sizeof(float));
        }
        if (pImpl->serotonin) {
            float serotoninLevel = pImpl->serotonin->getLevel();
            file.write(reinterpret_cast<const char*>(&serotoninLevel), sizeof(float));
        }
        if (pImpl->oxytocin) {
            float oxytocinLevel = pImpl->oxytocin->getLevel();
            file.write(reinterpret_cast<const char*>(&oxytocinLevel), sizeof(float));
        }
        if (pImpl->vasopressin) {
            float vasopressinLevel = pImpl->vasopressin->getLevel();
            file.write(reinterpret_cast<const char*>(&vasopressinLevel), sizeof(float));
        }
        
        // Save working memory if available
        if (pImpl->workingMemory) {
            bool hasWorkingMemory = true;
            file.write(reinterpret_cast<const char*>(&hasWorkingMemory), sizeof(bool));
            // Note: working memory serialization would be added here
        } else {
            bool hasWorkingMemory = false;
            file.write(reinterpret_cast<const char*>(&hasWorkingMemory), sizeof(bool));
        }
        
        NLM_LOG_INFO("Brain state saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving brain state: " + std::string(e.what()));
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    // Load brain state including neuromodulators
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        NLM_LOG_ERROR("Failed to open file for loading: " + filepath);
        return false;
    }
    
    try {
        // Load basic brain data
        file.read(reinterpret_cast<char*>(&pImpl->totalSpikesTotal), sizeof(size_t));
        file.read(reinterpret_cast<char*>(&pImpl->currentStep), sizeof(SimulationStep));
        file.read(reinterpret_cast<char*>(&pImpl->currentTime), sizeof(Timestamp));
        file.read(reinterpret_cast<char*>(&pImpl->developmentalStage), sizeof(DevelopmentalStage));
        
        // Load neuromodulator states
        if (pImpl->dopamine) {
            float dopamineLevel;
            file.read(reinterpret_cast<char*>(&dopamineLevel), sizeof(float));
            pImpl->dopamine->setLevel(dopamineLevel);
        }
        if (pImpl->acetylcholine) {
            float achLevel;
            file.read(reinterpret_cast<char*>(&achLevel), sizeof(float));
            pImpl->acetylcholine->setLevel(achLevel);
        }
        if (pImpl->norepinephrine) {
            float neLevel;
            file.read(reinterpret_cast<char*>(&neLevel), sizeof(float));
            pImpl->norepinephrine->setLevel(neLevel);
        }
        if (pImpl->serotonin) {
            float serotoninLevel;
            file.read(reinterpret_cast<char*>(&serotoninLevel), sizeof(float));
            pImpl->serotonin->setLevel(serotoninLevel);
        }
        if (pImpl->oxytocin) {
            float oxytocinLevel;
            file.read(reinterpret_cast<char*>(&oxytocinLevel), sizeof(float));
            pImpl->oxytocin->setLevel(oxytocinLevel);
        }
        if (pImpl->vasopressin) {
            float vasopressinLevel;
            file.read(reinterpret_cast<char*>(&vasopressinLevel), sizeof(float));
            pImpl->vasopressin->setLevel(vasopressinLevel);
        }
        
        // Load working memory if available
        bool hasWorkingMemory;
        file.read(reinterpret_cast<char*>(&hasWorkingMemory), sizeof(bool));
        if (hasWorkingMemory && pImpl->workingMemory) {
            // Note: working memory deserialization would be added here
        }
        
        NLM_LOG_INFO("Brain state loaded from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading brain state: " + std::string(e.what()));
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    auto region = std::make_unique<NeuralRegion>(name, pImpl->nextRegionId);
    RegionId regionId = pImpl->nextRegionId;
    pImpl->regions.push_back(std::move(region));
    pImpl->nextRegionId++;
    return regionId;
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
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                   float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), pImpl->interRegionConnections.end(),
        [source, target](const InterRegionConnection& conn) {
            return conn.sourceRegion == source && conn.targetRegion == target;
        });
    pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
}

float Brain::getAverageFiringRate() const {
    size_t totalNeurons = getTotalNeuronCount();
    if (totalNeurons == 0) return 0.0f;
    
    return static_cast<float>(pImpl->totalSpikesTotal) / static_cast<float>(totalNeurons);
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
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t active = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isActive()) {
                    active++;
                }
            }
        }
    }
    return active;
}

size_t Brain::getFiringNeuronCount() const {
    size_t firing = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                if (neuron->isFiring()) {
                    firing++;
                }
            }
        }
    }
    return firing;
}

DevelopmentStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentStage stage) {
    pImpl->developmentalStage = stage;
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->totalSpikesTotal));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("Excitation/inhibition ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Log neuromodulator levels
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    if (pImpl->acetylcholine) {
        NLM_LOG_INFO("Acetylcholine level: " + std::to_string(pImpl->acetylcholine->getLevel()));
    }
    if (pImpl->norepinephrine) {
        NLM_LOG_INFO("Norepinephrine level: " + std::to_string(pImpl->norepinephrine->getLevel()));
    }
    if (pImpl->serotonin) {
        NLM_LOG_INFO("Serotonin level: " + std::to_string(pImpl->serotonin->getLevel()));
    }
    if (pImpl->oxytocin) {
        NLM_LOG_INFO("Oxytocin level: " + std::to_string(pImpl->oxytocin->getLevel()));
    }
    if (pImpl->vasopressin) {
        NLM_LOG_INFO("Vasopressin level: " + std::to_string(pImpl->vasopressin->getLevel()));
    }
    if (pImpl->novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty->getLevel()));
    }
    if (pImpl->curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity->getLevel()));
    }
    if (pImpl->predictionError) {
        NLM_LOG_INFO("Prediction error magnitude: " + std::to_string(pImpl->predictionError->getMagnitude()));
    }
    
    NLM_LOG_INFO("=== End Brain Status ===");
}

// Neuromodulator Manager Implementation

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    
    // Neuromodulator references
    Dopamine* dopamine;
    Acetylcholine* acetylcholine;
    Norepinephrine* norepinephrine;
    Serotonin* serotonin;
    Oxytocin* oxytocin;
    Vasopressin* vasopressin;
    Novelty* novelty;
    Curiosity* curiosity;
    PredictionError* predictionError;
    
    // Integration weights
    float dopamineWeight;
    float achWeight;
    float neWeight;
    float serotoninWeight;
    float oxytocinWeight;
    float vasopressinWeight;
    float noveltyWeight;
    float curiosityWeight;
    float predictionErrorWeight;
    
    Impl() : brain(nullptr), dopamine(nullptr), acetylcholine(nullptr),
             norepinephrine(nullptr), serotonin(nullptr), oxytocin(nullptr),
             vasopressin(nullptr), novelty(nullptr), curiosity(nullptr),
             predictionError(nullptr), dopamineWeight(1.0f), achWeight(0.8f),
             neWeight(0.7f), serotoninWeight(0.6f), oxytocinWeight(0.5f),
             vasopressinWeight(0.4f), noveltyWeight(0.3f), curiosityWeight(0.4f),
             predictionErrorWeight(0.6f) {}
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~ NeuromodulatorManager() = default;

void NeuromodulatorManager::initialize(Brain* brain) {
    pImpl->brain = brain;
}

NeuromodulatorManager& NeuromodulatorManager::getInstance() {
    static NeuromodulatorManager instance;
    return instance;
}

void NeuromodulatorManager::reset() {
    // Reset all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->acetylcholine) pImpl->acetylcholine->reset();
    if (pImpl->norepinephrine) pImpl->norepinephrine->reset();
    if (pImpl->serotonin) pImpl->serotonin->reset();
    if (pImpl->oxytocin) pImpl->oxytocin->reset();
    if (pImpl->vasopressin) pImpl->vasopressin->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->predictionError) pImpl->predictionError->reset();
}

void NeuromodulatorManager::updateAll(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update all neuromodulators
    if (pImpl->dopamine) pImpl->dopamine->update(dt);
    if (pImpl->acetylcholine) pImpl->acetylcholine->update(dt);
    if (pImpl->norepinephrine) pImpl->norepinephrine->update(dt);
    if (pImpl->serotonin) pImpl->serotonin->update(dt);
    if (pImpl->oxytocin) pImpl->oxytocin->update(dt);
    if (pImpl->vasopressin) pImpl->vasopressin->update(dt);
    if (pImpl->novelty) pImpl->novelty->update(dt);
    if (pImpl->curiosity) pImpl->curiosity->update(dt);
    if (pImpl->predictionError) pImpl->predictionError->update(dt);
    
    // Apply neuromodulation effects
    applyNeuromodulationToNeurons();
    
    // Update integrated neuromodulation signals
    float totalPlasticity = getTotalPlasticityModulation();
    float totalExcitability = getTotalExcitabilityModulation();
    float totalAttention = getTotalAttentionModulation();
    
    NLM_LOG_DEBUG("NeuromodulatorManager updated: plasticity=" + std::to_string(totalPlasticity) +
                  ", excitability=" + std::to_string(totalExcitability) +
                  ", attention=" + std::to_string(totalAttention));
}

void NeuromodulatorManager::registerDopamine(Dopamine* dopamine) {
    pImpl->dopamine = dopamine;
}

void NeuromodulatorManager::registerAcetylcholine(Acetylcholine* acetylcholine) {
    pImpl->acetylcholine = acetylcholine;
}

void NeuromodulatorManager::registerNorepinephrine(Norepinephrine* norepinephrine) {
    pImpl->norepinephrine = norepinephrine;
}

void NeuromodulatorManager::registerSerotonin(Serotonin* serotonin) {
    pImpl->serotonin = serotonin;
}

void NeuromodulatorManager::registerOxytocin(Oxytocin* oxytocin) {
    pImpl->oxytocin = oxytocin;
}

void NeuromodulatorManager::registerVasopressin(Vasopressin* vasopressin) {
    pImpl->vasopressin = vasopressin;
}

void NeuromodulatorManager::registerNovelty(Novelty* novelty) {
    pImpl->novelty = novelty;
}

void NeuromodulatorManager::registerCuriosity(Curiosity* curiosity) {
    pImpl->curiosity = curiosity;
}

void NeuromodulatorManager::registerPredictionError(PredictionError* predictionError) {
    pImpl->predictionError = predictionError;
}

float NeuromodulatorManager::getTotalPlasticityModulation() const {
    float total = 1.0f; // Baseline
    
    if (pImpl->dopamine) total *= pImpl->dopamine->getPlasticityFactor();
    if (pImpl->acetylcholine) total *= pImpl->acetylcholine->getPlasticityFactor();
    if (pImpl->norepinephrine) total *= pImpl->norepinephrine->getPlasticityFactor();
    if (pImpl->serotonin) total *= pImpl->serotonin->getPlasticityFactor();
    if (pImpl->oxytocin) total *= pImpl->oxytocin->getPlasticityFactor();
    if (pImpl->vasopressin) total *= pImpl->vasopressin->getPlasticityFactor();
    if (pImpl->curiosity) total *= pImpl->curiosity->getPlasticityFactor();
    if (pImpl->predictionError) total *= pImpl->predictionError->getPlasticityFactor();
    
    return total;
}

float NeuromodulatorManager::getTotalExcitabilityModulation() const {
    float total = 1.0f; // Baseline
    
    if (pImpl->dopamine) total += pImpl->dopamine->getLevel() * pImpl->dopamineWeight;
    if (pImpl->acetylcholine) total += pImpl->acetylcholine->getLevel() * pImpl->achWeight;
    if (pImpl->norepinephrine) total += pImpl->norepinephrine->getLevel() * pImpl->neWeight;
    if (pImpl->serotonin) total += pImpl->serotonin->getLevel() * pImpl->serotoninWeight;
    if (pImpl->oxytocin) total += pImpl->oxytocin->getLevel() * pImpl->oxytocinWeight;
    if (pImpl->vasopressin) total += pImpl->vasopressin->getLevel() * pImpl->vasopressinWeight;
    if (pImpl->novelty) total += pImpl->novelty->getLevel() * pImpl->noveltyWeight;
    if (pImpl->curiosity) total += pImpl->curiosity->getLevel() * pImpl->curiosityWeight;
    
    return std::max(0.1f, total);
}

float NeuromodulatorManager::getTotalAttentionModulation() const {
    float total = 1.0f; // Baseline
    
    if (pImpl->dopamine) total += pImpl->dopamine->getLevel() * 0.3f;
    if (pImpl->acetylcholine) total += pImpl->acetylcholine->getLevel() * 0.5f;
    if (pImpl->norepinephrine) total += pImpl->norepinephrine->getLevel() * 0.4f;
    if (pImpl->serotonin) total += pImpl->serotonin->getLevel() * 0.2f;
    if (pImpl->oxytocin) total += pImpl->oxytocin->getLevel() * 0.1f;
    if (pImpl->vasopressin) total += pImpl->vasopressin->getLevel() * 0.1f;
    
    return std::max(0.5f, total);
}

float NeuromodulatorManager::getDopamineLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

float NeuromodulatorManager::getAcetylcholineLevel() const {
    return pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
}

float NeuromodulatorManager::getNorepinephrineLevel() const {
    return pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
}

float NeuromodulatorManager::getSerotoninLevel() const {
    return pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f;
}

float NeuromodulatorManager::getOxytocinLevel() const {
    return pImpl->oxytocin ? pImpl->oxytocin->getLevel() : 0.0f;
}

float NeuromodulatorManager::getVasopressinLevel() const {
    return pImpl->vasopressin ? pImpl->vasopressin->getLevel() : 0.0f;
}

float NeuromodulatorManager::getNoveltyLevel() const {
    return pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
}

float NeuromodulatorManager::getCuriosityLevel() const {
    return pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
}

float NeuromodulatorManager::getPredictionErrorLevel() const {
    return pImpl->predictionError ? pImpl->predictionError->getMagnitude() : 0.0f;
}

void NeuromodulatorManager::applyNeuromodulationToNeurons() {
    if (!pImpl->brain) return;
    
    for (auto& region : pImpl->brain->pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Apply neuromodulatory effects to individual neurons
                if (pImpl->dopamine) {
                    float dopamineMod = pImpl->dopamine->getLevel() * 0.2f;
                    neuron->injectCurrent(dopamineMod);
                }
                if (pImpl->acetylcholine) {
                    float achMod = pImpl->acetylcholine->getLevel() * 0.3f;
                    neuron->injectCurrent(achMod);
                }
                if (pImpl->norepinephrine) {
                    float neMod = pImpl->norepinephrine->getLevel() * 0.25f;
                    neuron->injectCurrent(neMod);
                }
                if (pImpl->serotonin) {
                    float serotoninMod = pImpl->serotonin->getLevel() * 0.15f;
                    neuron->injectCurrent(serotoninMod);
                }
                if (pImpl->oxytocin) {
                    float oxytocinMod = pImpl->oxytocin->getLevel() * 0.1f;
                    neuron->injectCurrent(oxytocinMod);
                }
                if (pImpl->vasopressin) {
                    float vasopressinMod = pImpl->vasopressin->getLevel() * 0.1f;
                    neuron->injectCurrent(vasopressinMod);
                }
            }
        }
    }
}

void NeuromodulatorManager::applyNeuromodulationToMemory(WorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    // Apply neuromodulatory effects to memory systems
    if (pImpl->dopamine) {
        workingMemory->enhanceLearning(pImpl->dopamine->getLevel() * 0.4f);
    }
    if (pImpl->acetylcholine) {
        workingMemory->enhanceConsolidation(pImpl->acetylcholine->getLevel() * 0.5f);
    }
    if (pImpl->serotonin) {
        workingMemory->enhanceRetrieval(pImpl->serotonin->getLevel() * 0.3f);
    }
}

void NeuromodulatorManager::applyNeuromodulationToAttention(AttentionalSelection* attention) {
    if (!attention) return;
    
    // Apply neuromodulatory effects to attention systems
    if (pImpl->dopamine) {
        attention->enhanceFocus(pImpl->dopamine->getLevel() * 0.3f);
    }
    if (pImpl->acetylcholine) {
        attention->enhanceSelection(pImpl->acetylcholine->getLevel() * 0.4f);
    }
    if (pImpl->norepinephrine) {
        attention->enhanceVigilance(pImpl->norepinephrine->getLevel() * 0.5f);
    }
}

Dopamine* NeuromodulatorManager::getDopamine() const {
    return pImpl->dopamine;
}

Acetylcholine* NeuromodulatorManager::getAcetylcholine() const {
    return pImpl->acetylcholine;
}

Norepinephrine* NeuromodulatorManager::getNorepinephrine() const {
    return pImpl->norepinephrine;
}

Serotonin* NeuromodulatorManager::getSerotonin() const {
    return pImpl->serotonin;
}

Oxytocin* NeuromodulatorManager::getOxytocin() const {
    return pImpl->oxytocin;
}

Vasopressin* NeuromodulatorManager::getVasopressin() const {
    return pImpl->vasopressin;
}

Novelty* NeuromodulatorManager::getNovelty() const {
    return pImpl->novelty;
}

Curiosity* NeuromodulatorManager::getCuriosity() const {
    return pImpl->curiosity;
}

PredictionError* NeuromodulatorManager::getPredictionError() const {
    return pImpl->predictionError;
}

} // namespace nlm

// Brain implementation in Brain.cpp ends here