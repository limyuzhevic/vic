#include "BrainOrchestrator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct DefaultProcessingStrategy::Impl {
    TimestepDuration timestep;
    size_t replayInterval;
    size_t consolidationInterval;
    size_t stepsSinceLastEpisode;
};

DefaultProcessingStrategy::DefaultProcessingStrategy()
    : pImpl(new Impl())
{
    timestep = 0.001f;
    replayInterval = 100;
    consolidationInterval = 1000;
    stepsSinceLastEpisode = 0;
}

DefaultProcessingStrategy::~DefaultProcessingStrategy() = default;

void DefaultProcessingStrategy::processStep(Brain* brain, SimulationStep currentStep, Timestamp currentTime) {
    // This method can be extended if needed
    // For now, it just delegates to the BrainOrchestrator
    // The actual processing logic is in BrainOrchestrator::processStep()
}

struct BrainOrchestrator::Impl {
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
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001f)
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
};

BrainOrchestrator::BrainOrchestrator(Brain* brain)
    : pImpl(new Impl(brain->getConfig()))
    , brain_(brain)
    , currentStrategy_(std::make_unique<DefaultProcessingStrategy>())
{
    // Set up brain reference for subsystems
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(brain);
    }
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(brain);
    }
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(brain);
    }
    if (pImpl->planner) {
        pImpl->planner->initialize(brain);
    }
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(brain);
    }
    if (pImpl->attention) {
        pImpl->attention->initialize(brain);
    }
    if (pImpl->novelty) {
        pImpl->novelty->initialize(brain);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(brain);
    }
    
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
    
    // Set providers for checkpoint manager
    pImpl->checkpointManager->setNeuronProvider([this](NeuronCheckpointData& data) {
        data.membranePotential.reserve(getTotalNeuronCount());
        data.restingPotential.reserve(getTotalNeuronCount());
        data.threshold.reserve(getTotalNeuronCount());
        data.resetPotential.reserve(getTotalNeuronCount());
        data.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    data.membranePotential.push_back(state.membranePotential);
                    data.restingPotential.push_back(state.restingPotential);
                    data.threshold.push_back(state.threshold);
                    data.resetPotential.push_back(state.resetPotential);
                    data.leakConductance.push_back(state.leakConductance);
                }
            }
        }
        return true;
    });
    
    pImpl->checkpointManager->setSynapseProvider([this](SynapseCheckpointData& data) {
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                data.sourceNeuron.push_back(syn->getSourceNeuron().index());
                data.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                data.weight.push_back(syn->getWeight());
                data.delay.push_back(syn->getDelay());
                data.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                data.plasticityFlags.push_back(static_cast<uint8_t>(syn->getPlasticityFlags()));
                data.eligibilityTrace.push_back(syn->getEligibilityTrace());
                data.efficacy.push_back(syn->getEfficacy());
                data.shortTermDepression.push_back(syn->getShortTermDepression());
                data.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        return true;
    });
}

BrainOrchestrator::~BrainOrchestrator() = default;

BrainOrchestrator::BrainOrchestrator(BrainOrchestrator&& other) noexcept
    : pImpl(other.pImpl)
    , brain_(other.brain_)
    , currentStrategy_(std::move(other.currentStrategy_))
{
    other.pImpl = nullptr;
    other.brain_ = nullptr;
}

BrainOrchestrator& BrainOrchestrator::operator=(BrainOrchestrator&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        brain_ = other.brain_;
        currentStrategy_ = std::move(other.currentStrategy_);
        
        other.pImpl = nullptr;
        other.brain_ = nullptr;
    }
    return *this;
}

void BrainOrchestrator::setProcessingStrategy(std::unique_ptr<ProcessingStrategy> strategy) {
    currentStrategy_ = std::move(strategy);
}

const char* BrainOrchestrator::getProcessingStrategyName() const {
    return currentStrategy_ ? currentStrategy_->getName() : "None";
}

bool BrainOrchestrator::initialize() {
    NLM_LOG_INFO("Initializing BrainOrchestrator");
    
    // TODO: Add actual initialization logic here
    return true;
}

void BrainOrchestrator::processStep(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Use the current strategy to process the step
    if (currentStrategy_) {
        currentStrategy_->processStep(brain_, currentStep, currentTime);
    } else {
        // Fallback to default processing
        processDefaultStep(currentStep, currentTime);
    }
}

void BrainOrchestrator::processDefaultStep(SimulationStep currentStep, Timestamp currentTime) {
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    processDelayedSpikeEvents(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    updateNeuronDynamics(currentTime, pImpl->timestep);
    
    // ========== STEP 3: Detect spikes and schedule spike events ==========
    detectAndProcessSpikes(currentStep, currentTime);
    
    // Process immediate spikes
    processImmediateSpikes(currentStep);
    
    // ========== STEP 4: Update working memory ==========
    updateWorkingMemory(pImpl->timestep);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    applyNeuromodulationEffects();
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    applyPlasticityRules(plasticityMod);
    
    // ========== STEP 7: Update episodic memory ==========
    updateEpisodicMemory(currentStep, currentTime);
    
    // ========== STEP 8: Update prediction system ==========
    updatePredictionSystem();
    
    // ========== STEP 9: Update attention system ==========
    updateAttentionSystem();
    
    // ========== STEP 10: Update concept formation ==========
    updateConceptFormation();
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    updateStructuralPlasticity(currentStep);
    
    // ========== STEP 12: Replay important memories ==========
    replayMemories(currentStep);
    
    // ========== STEP 13: Apply development effects ==========
    applyDevelopmentEffects(currentStep);
    
    // ========== STEP 14: Periodic memory consolidation ==========
    consolidateMemories(currentStep);
    
    // ========== STEP 15: Checkpoint management ==========
    updateCheckpointManagement(currentStep, currentTime);
    
    // ========== STEP 16: Collect statistics ==========
    collectStatistics();
}

void BrainOrchestrator::processDelayedSpikeEvents(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void BrainOrchestrator::updateNeuronDynamics(Timestamp currentTime, TimestepDuration timestep) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
}

void BrainOrchestrator::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < timestep_ * 2.0f);

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
}

void BrainOrchestrator::processImmediateSpikes(SimulationStep currentStep) {
    pImpl->spikeSystem->processSpikes(currentStep);
}

void BrainOrchestrator::updateWorkingMemory(TimestepDuration dt) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(dt);
    }
}

void BrainOrchestrator::applyNeuromodulationEffects() {
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
}

void BrainOrchestrator::applyPlasticityRules(float plasticityMod) {
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

void BrainOrchestrator::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
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
}

void BrainOrchestrator::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void BrainOrchestrator::updateAttentionSystem() {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void BrainOrchestrator::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void BrainOrchestrator::updateStructuralPlasticity(SimulationStep currentStep) {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(brain_, *pImpl->rng);
    }
}

void BrainOrchestrator::replayMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainOrchestrator::applyDevelopmentEffects(SimulationStep currentStep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(brain_, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            applyDevelopmentToPlasticity(pImpl->developmentalStage);
        }
    }
}

void BrainOrchestrator::consolidateMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void BrainOrchestrator::updateCheckpointManagement(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainOrchestrator::collectStatistics() {
    // Statistics collection can be extended as needed
}

void BrainOrchestrator::applyNeuromodulation(const Neuromodulator& signal) {
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void BrainOrchestrator::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void BrainOrchestrator::develop() {
    // Development updates structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(brain_, *pImpl->rng);
    }
}

void BrainOrchestrator::applyDevelopmentToPlasticity(DevelopmentalStage stage) {
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
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

DevelopmentalStage BrainOrchestrator::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void BrainOrchestrator::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

void BrainOrchestrator::applyNeuromodulationToPlasticity(float modulation) {
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

std::vector<const EpisodicMemoryItem*> BrainOrchestrator::getEpisodesForReplay(size_t count) const {
    if (pImpl->episodicMemory) {
        return pImpl->episodicMemory->getEpisodesForReplay(count);
    }
    return {};
}

void BrainOrchestrator::replayEpisode(const EpisodicMemoryItem* episode) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->replayEpisode(episode);
    }
}

void BrainOrchestrator::consolidateMemories(float threshold) {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(threshold);
    }
}

void BrainOrchestrator::resetState() {
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    // Reset regions
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
}

} // namespace nlm
