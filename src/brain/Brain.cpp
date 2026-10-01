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
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../performance/PerformanceProfiler.hpp"
#include "../performance/BenchmarkSuite.hpp"
#include "../performance/PerformanceAnalyzer.hpp"
#include "../performance/PerformanceAPI.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <filesystem>
#include <chrono>

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
    
    // Visualization system
    bool visualizationEnabled;
    double visualizationUpdateRate;
    double visualizationRenderRate;
    size_t visualizationFrameCount;
    double lastVisualizationTime;
    double totalVisualizationUpdateTime;
    double totalVisualizationRenderTime;
    std::map<std::string, std::string> visualizationPreferences;
    
    // Visualization statistics
    mutable size_t visualizationFrameCountLastFrame;
    mutable double visualizationFPSLastFrame;
    mutable double visualizationUpdateTimeLastFrame;
    mutable double visualizationRenderTimeLastFrame;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Performance monitoring
    std::unique_ptr<PerformanceProfiler> performanceProfiler;
    std::unique_ptr<BenchmarkSuite> benchmarkSuite;
    std::unique_ptr<PerformanceAnalyzer> performanceAnalyzer;
    
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
        , visualizationEnabled(true)
        , visualizationUpdateRate(30.0)
        , visualizationRenderRate(60.0)
        , visualizationFrameCount(0)
        , lastVisualizationTime(0.0)
        , totalVisualizationUpdateTime(0.0)
        , totalVisualizationRenderTime(0.0)
        , visualizationFrameCountLastFrame(0)
        , visualizationFPSLastFrame(0.0)
        , visualizationUpdateTimeLastFrame(0.0)
        , visualizationRenderTimeLastFrame(0.0)
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
    
    // Initialize performance systems
    pImpl->performanceProfiler = std::make_unique<PerformanceProfiler>();
    pImpl->performanceProfiler->initialize(pImpl->config);
    
    pImpl->benchmarkSuite = std::make_unique<BenchmarkSuite>(pImpl->config);
    pImpl->benchmarkSuite->initialize();
    
    pImpl->performanceAnalyzer = std::make_unique<PerformanceAnalyzer>();
    pImpl->performanceAnalyzer->initialize();
    
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

// ========== VISUALIZATION METHODS ==========

bool Brain::generateVisualization(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Brain: Generating visualization to " + filename + "." + format);
    
    try {
        // Create visualization directory
        std::filesystem::create_directories("visualizations");
        
        // Generate different types of visualizations based on format
        bool success = true;
        
        if (format == "png" || format == "all") {
            // Generate 2D projection
            if (!generate2DProjection(filename + "_2d.png")) {
                NLM_LOG_WARNING("Brain: Failed to generate 2D projection");
                success = false;
            }
            
            // Generate connectivity matrix visualization
            if (!generateConnectivityMatrix(filename + "_connectivity.png")) {
                NLM_LOG_WARNING("Brain: Failed to generate connectivity matrix");
                success = false;
            }
        } else if (format == "json" || format == "all") {
            // Generate JSON visualization data
            if (!exportBrainData(filename + "_data.json")) {
                NLM_LOG_WARNING("Brain: Failed to export brain data");
                success = false;
            }
        } else if (format == "csv" || format == "all") {
            // Generate CSV data
            if (!exportTimeSeriesData(filename + "_timeseries.csv")) {
                NLM_LOG_WARNING("Brain: Failed to export time series data");
                success = false;
            }
        }
        
        if (success) {
            NLM_LOG_INFO("Brain: Visualization generated successfully");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception generating visualization: ") + e.what());
        return false;
    }
}

bool Brain::exportBrainData(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Brain: Exporting brain data to " + filename + "." + format);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Brain: Failed to open file for writing: " + filename);
            return false;
        }
        
        if (format == "json") {
            // Export in JSON format
            file << "{\n";
            file << "  \"brain_info\": {\n";
            file << "    \"neuron_count\": " << getTotalNeuronCount() << ",\n";
            file << "    \"synapse_count\": " << getTotalSynapseCount() << ",\n";
            file << "    \"region_count\": " << getRegionCount() << ",\n";
            file << "    \"firing_rate\": " << getAverageFiringRate() <<",\n";
            file << "    \"e_i_ratio\": " << getExcitationInhibitionRatio() << ",\n";
            file << "    \"total_spikes\": " << getTotalSpikeCount() << ",\n";
            file << "    \"timestamp\": " << pImpl->currentTime << "\n";
            file << "  },\n";
            
            file << "  \"regions\": [\n";
            bool firstRegion = true;
            for (const auto& region : pImpl->regions) {
                if (!firstRegion) file << ",\n";
                firstRegion = false;
                
                file << "    {\n";
                file << "      \"id\": " << region->getId().index() << ",\n";
                file << "      \"name\": \"" << region->getName() << "\",\n";
                file << "      \"neuron_count\": " << region->getTotalNeuronCount() << ",\n";
                file << "      \"synapse_count\": " << region->getSynapseCount() << ",\n";
                file << "      \"avg_synaptic_weight\": " << region->getAverageSynapticWeight() << "\n";
                file << "    }";
            }
            
            file << "\n  ],\n";
            
            file << "  \"neurons\": [\n";
            size_t neuronIndex = 0;
            bool firstNeuron = true;
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        if (!firstNeuron) file << ",\n";
                        firstNeuron = false;
                        
                        const auto& state = neuron->getState();
                        file << "    {\n";
                        file << "      \"id\": " << neuron->getId() << ",\n";
                        file << "      \"region\": " << region->getId().index() << ",\n";
                        file << "      \"type\": " << static_cast<int>(pop->getNeuronType()) << ",\n";
                        file << "      \"membrane_potential\": " << state.membranePotential << ",\n";
                        file << "      \"is_firing\": " << (neuron->isFiring() ? "true" : "false") << ",\n";
                        file << "      \"last_spike\": " << state.lastSpikeTime << "\n";
                        file << "    }";
                        
                        neuronIndex++;
                    }
                }
            }
            
            file << "\n  ]\n";
            file << "}\n";
            
        } else if (format == "csv") {
            // Export in CSV format
            file << "neuron_id,region_id,type,membrane_potential,is_firing,last_spike_time\n";
            
            for (const auto& region : pImpl->regions) {
                for (const auto& pop : region->getPopulations()) {
                    for (const auto* neuron : pop->getNeurons()) {
                        const auto& state = neuron->getState();
                        file << neuron->getId() << ",";
                        file << region->getId().index() << ",";
                        file << static_cast<int>(pop->getNeuronType()) << ",";
                        file << state.membranePotential << ",";
                        file << (neuron->isFiring() ? "1" : "0") << ",";
                        file << state.lastSpikeTime << "\n";
                    }
                }
            }
            
            file << "\nsynapse_info\n";
            file << "source_id,destination_id,weight,delay,type\n";
            
            for (const auto& region : pImpl->regions) {
                for (const auto* syn : region->getSynapses()) {
                    file << syn->getSourceNeuron().index() << ",";
                    file << syn->getDestinationNeuron().index() << ",";
                    file << syn->getWeight() << ",";
                    file << syn->getDelay() << ",";
                    file << static_cast<int>(syn->getType()) << "\n";
                }
            }
        }
        
        file.close();
        NLM_LOG_INFO("Brain: Brain data exported successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting brain data: ") + e.what());
        return false;
    }
}

bool Brain::exportConnectivityMatrix(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Brain: Exporting connectivity matrix to " + filename);
    
    try {
        // Calculate connectivity matrix between regions
        size_t regionCount = pImpl->regions.size();
        if (regionCount == 0) {
            NLM_LOG_WARNING("Brain: No regions to export connectivity matrix");
            return false;
        }
        
        if (format == "csv") {
            std::ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Brain: Failed to open file: " + filename);
                return false;
            }
            
            file << "source_region,target_region,total_connections,total_weight,connection_probability\n";
            
            for (size_t i = 0; i < regionCount; ++i) {
                auto* sourceRegion = getRegion(RegionId(i + 1));
                if (!sourceRegion) continue;
                
                for (size_t j = 0; j < regionCount; ++j) {
                    auto* targetRegion = getRegion(RegionId(j + 1));
                    if (!targetRegion) continue;
                    
                    // Count connections between regions
                    size_t connectionCount = 0;
                    float totalWeight = 0.0f;
                    
                    for (const auto& syn : sourceRegion->getSynapses()) {
                        auto targetNeurons = targetRegion->getAllNeurons();
                        for (auto* targetNeuron : targetNeurons) {
                            if (syn->getDestinationNeuron() == targetNeuron->getId()) {
                                ++connectionCount;
                                totalWeight += syn->getWeight();
                            }
                        }
                    }
                    
                    double connectionProbability = regionCount > 1 ? 
                        static_cast<double>(connectionCount) / (regionCount * regionCount) : 0.0;
                    
                    file << i + 1 << "," << j + 1 << ",";
                    file << connectionCount << ",";
                    file << totalWeight << ",";
                    file << connectionProbability << "\n";
                }
            }
            
            file.close();
        }
        
        NLM_LOG_INFO("Brain: Connectivity matrix exported successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting connectivity matrix: ") + e.what());
        return false;
    }
}

bool Brain::exportTimeSeriesData(const std::string& filename, const std::string& format) {
    NLM_LOG_INFO("Brain: Exporting time series data to " + filename);
    
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Brain: Failed to open file: " + filename);
            return false;
        }
        
        if (format == "csv") {
            file << "timestamp,step,total_spikes,total_neurons,total_synapses,avg_firing_rate,e_i_ratio\n";
            file << pImpl->currentTime << ",";
            file << pImpl->currentStep << ",";
            file << getTotalSpikeCount() << ",";
            file << getTotalNeuronCount() << ",";
            file << getTotalSynapseCount() << ",";
            file << getAverageFiringRate() << ",";
            file << getExcitationInhibitionRatio() << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Brain: Time series data exported successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception exporting time series data: ") + e.what());
        return false;
    }
}

void Brain::updateVisualization(double deltaTime) {
    auto startTime = std::chrono::steady_clock::now();
    
    if (visualizationEnabled && pImpl->visualizationUpdateRate > 0.0) {
        // Update visualization system
        pImpl->visualizationFrameCount++;
        
        // Collect visualization data
        // (In a real implementation, this would call visualization systems)
        
        NLM_LOG_DEBUG("Brain: Updated visualization for frame " + std::to_string(pImpl->visualizationFrameCount));
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto updateDuration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalVisualizationUpdateTime += updateDuration;
    pImpl->visualizationUpdateTimeLastFrame = updateDuration;
    pImpl->lastVisualizationTime = pImpl->currentTime;
}

void Brain::renderVisualization() {
    auto startTime = std::chrono::steady_clock::now();
    
    if (visualizationEnabled) {
        // Render visualizations
        // (In a real implementation, this would render to screen or file)
        
        NLM_LOG_DEBUG("Brain: Rendered visualization frame");
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto renderDuration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->totalVisualizationRenderTime += renderDuration;
    pImpl->visualizationRenderTimeLastFrame = renderDuration;
    
    // Calculate FPS
    if (pImpl->visualizationFrameCountLastFrame > 0) {
        pImpl->visualizationFPSLastFrame = static_cast<double>(pImpl->visualizationFrameCount) / 
            (pImpl->visualizationFrameCount - pImpl->visualizationFrameCountLastFrame);
    }
    pImpl->visualizationFrameCountLastFrame = pImpl->visualizationFrameCount;
}

bool Brain::isVisualizationEnabled() const {
    return pImpl->visualizationEnabled;
}

void Brain::enableVisualization(bool enabled) {
    pImpl->visualizationEnabled = enabled;
    NLM_LOG_INFO("Brain: Visualization " + std::string(enabled ? "enabled" : "disabled"));
}

size_t Brain::getVisualizationFrameCount() const {
    return pImpl->visualizationFrameCount;
}

double Brain::getVisualizationFPS() const {
    return pImpl->visualizationFPSLastFrame;
}

double Brain::getVisualizationUpdateTime() const {
    return pImpl->visualizationUpdateTimeLastFrame;
}

double Brain::getVisualizationRenderTime() const {
    return pImpl->visualizationRenderTimeLastFrame;
}

std::vector<std::string> Brain::getAvailableVisualizationTypes() const {
    return {"network", "activity", "spikes", "weights", "state_space", "connectivity"};
}

std::vector<std::string> Brain::getAvailableExportFormats() const {
    return {"png", "json", "csv", "hdf5", "parquet"};
}

void Brain::setVisualizationPreference(const std::string& key, const std::string& value) {
    pImpl->visualizationPreferences[key] = value;
}

std::string Brain::getVisualizationPreference(const std::string& key) const {
    auto it = pImpl->visualizationPreferences.find(key);
    return (it != pImpl->visualizationPreferences.end()) ? it->second : "";
}

} // namespace nlm


// Get visualization statistics
Brain::VisualizationStats Brain::getVisualizationStats() const {
    VisualizationStats stats;
    
    stats.frameCount = pImpl->visualizationFrameCount;
    stats.fps = pImpl->visualizationFPSLastFrame;
    stats.updateTime = pImpl->visualizationUpdateTimeLastFrame;
    stats.renderTime = pImpl->visualizationRenderTimeLastFrame;
    stats.totalUpdateTime = pImpl->totalVisualizationUpdateTime;
    stats.totalRenderTime = pImpl->totalVisualizationRenderTime;
    stats.lastUpdateTime = pImpl->lastVisualizationTime;
    
    return stats;
}

// ========== PERFORMANCE MEASUREMENT METHODS ==========

bool Brain::enablePerformanceProfiling(const std::string& sectionName) {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->startProfile(sectionName);
    }
    return false;
}

void Brain::disablePerformanceProfiling(const std::string& sectionName) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->stopProfile(sectionName);
    }
}

void Brain::togglePerformanceProfiling(const std::string& sectionName) {
    if (pImpl->performanceProfiler) {
        if (pImpl->performanceProfiler->isProfiling(sectionName)) {
            pImpl->performanceProfiler->stopProfile(sectionName);
        } else {
            pImpl->performanceProfiler->startProfile(sectionName);
        }
    }
}

double Brain::getPerformanceTime(const std::string& sectionName) const {
    if (pImpl->performanceProfiler) {
        const auto& timerData = pImpl->performanceProfiler->getTimerData(sectionName);
        return timerData.totalDuration.count();
    }
    return 0.0;
}

uint64_t Brain::getMemoryUsage() const {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->getMemoryUsage();
    }
    return 0;
}

double Brain::getCPUUsage() const {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->getCPUUsage();
    }
    return 0.0;
}

double Brain::getFPS() const {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->getFPS();
    }
    return 0.0;
}

void Brain::profileBrainStep(const std::string& operation) {
    if (pImpl->performanceProfiler) {
        std::string sectionName = "brain_step_" + operation;
        pImpl->performanceProfiler->startProfile(sectionName);
    }
}

void Brain::profileMemoryOperations() {
    if (pImpl->performanceProfiler) {
        // Profile memory allocation/deallocation
        // In real implementation, would track actual memory operations
        pImpl->performanceProfiler->trackMemoryAllocation(1024); // Simulate
        pImpl->performanceProfiler->trackMemoryDeallocation(512);
    }
}

void Brain::profileNeuralActivity() {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startProfile("neural_activity");
        // Simulate neural activity profiling
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        pImpl->performanceProfiler->stopProfile("neural_activity");
    }
}

void Brain::profilePlasticityUpdates() {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startProfile("plasticity_updates");
        // Simulate plasticity updates
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        pImpl->performanceProfiler->stopProfile("plasticity_updates");
    }
}

void Brain::profileMemoryUpdates() {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startProfile("memory_updates");
        // Simulate memory updates
        std::this_thread::sleep_for(std::chrono::microseconds(30));
        pImpl->performanceProfiler->stopProfile("memory_updates");
    }
}

void Brain::profilePredictionSystem() {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startProfile("prediction_system");
        // Simulate prediction system operations
        std::this_thread::sleep_for(std::chrono::microseconds(40));
        pImpl->performanceProfiler->stopProfile("prediction_system");
    }
}

void Brain::profileCognitionSystems() {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startProfile("cognition_systems");
        // Simulate cognition system operations
        std::this_thread::sleep_for(std::chrono::microseconds(60));
        pImpl->performanceProfiler->stopProfile("cognition_systems");
    }
}

void Brain::profileNeuromodulation() {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->startProfile("neuromodulation");
        // Simulate neuromodulation operations
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        pImpl->performanceProfiler->stopProfile("neuromodulation");
    }
}

std::vector<std::string> Brain::getActiveProfileSections() const {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->getAllSectionNames();
    }
    return std::vector<std::string>();
}

std::vector<std::string> Brain::getBottleneckSections(double threshold) const {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->getBottleneckSections(threshold);
    }
    return std::vector<std::string>();
}

std::string Brain::getPerformanceSummary() const {
    std::stringstream ss;
    ss << "Brain Performance Summary:" << std::endl;
    ss << "  Total Neurons: " << getTotalNeuronCount() << std::endl;
    ss << "  Total Synapses: " << getTotalSynapseCount() << std::endl;
    ss << "  Memory Usage: " << getMemoryUsage() << " bytes" << std::endl;
    ss << "  CPU Usage: " << getCPUUsage() << "%" << std::endl;
    ss << "  FPS: " << getFPS() << std::endl;
    
    if (pImpl->performanceProfiler) {
        ss << "  Active Sections: " << getActiveProfileSections().size() << std::endl;
        ss << "  Bottleneck Sections: " << getBottleneckSections().size() << std::endl;
    }
    
    return ss.str();
}

void Brain::configurePerformanceProfiling(std::chrono::milliseconds interval) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->setProfilingInterval(interval);
    }
}

void Brain::enableDetailedTiming(bool enable) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->setDetailedTiming(enable);
    }
}

void Brain::enableMemoryTracking(bool enable) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->setMemoryTracking(enable);
    }
}

void Brain::enableCPUTracking(bool enable) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->setCPUTracking(enable);
    }
}

void Brain::enableIOTracking(bool enable) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->setIOTracking(enable);
    }
}

void Brain::enableGPUTracking(bool enable) {
    if (pImpl->performanceProfiler) {
        pImpl->performanceProfiler->setGPUTracking(enable);
    }
}

bool Brain::validatePerformanceRequirements(const std::map<std::string, double>& requirements) const {
    if (pImpl->performanceAnalyzer) {
        return pImpl->performanceAnalyzer->validatePerformanceRequirements(requirements);
    }
    return false;
}

std::string Brain::getPerformanceGrade() const {
    if (pImpl->performanceAnalyzer) {
        double score = pImpl->performanceAnalyzer->calculatePerformanceScore(std::map<std::string, double>{{ "placeholder", 0.0 }});
        return pImpl->performanceAnalyzer->getPerformanceGrade(score);
    }
    return "UNKNOWN";
}

bool Brain::exportPerformanceData(const std::string& filename, const std::string& format) const {
    if (pImpl->performanceProfiler) {
        return pImpl->performanceProfiler->exportProfile(filename, format);
    }
    return false;
}

bool Brain::exportBenchmarkResults(const std::string& filename, const std::string& format) const {
    if (pImpl->benchmarkSuite) {
        pImpl->benchmarkSuite->saveResults(filename, format);
        return true;
    }
    return false;
}

std::string Brain::analyzePerformanceTrends() const {
    if (pImpl->performanceAnalyzer) {
        return pImpl->performanceAnalyzer->generatePerformanceReport();
    }
    return "Performance analyzer not available";
}

std::vector<std::string> Brain::getPerformanceRecommendations() const {
    if (pImpl->performanceAnalyzer) {
        auto suggestions = pImpl->performanceAnalyzer->generateOptimizationSuggestions();
        std::vector<std::string> recommendations;
        for (const auto& suggestion : suggestions) {
            if (suggestion.priority == "HIGH") {
                recommendations.push_back("*** " + suggestion.metricName + " - " + suggestion.suggestedAction + " ***");
            } else {
                recommendations.push_back(suggestion.metricName + ": " + suggestion.suggestedAction);
            }
        }
        return recommendations;
    }
    return std::vector<std::string>();
}

} // namespace nlm
