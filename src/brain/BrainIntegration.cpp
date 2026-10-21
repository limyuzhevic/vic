#include "BrainIntegration.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== BRAIN INTEGRATION COMPONENT ==========

// BrainIntegration implementation - handles memory systems, neuromodulation, development, and simulation integration

struct BrainIntegration::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // Integrated systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    std::unique_ptr<PredictionSystem> predictionSystem;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg)
        , rng(nullptr)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)
        , consolidationInterval(1000) {
        
        // Initialize random generator
        uint64_t seed = 42;
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize integrated systems
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        predictionSystem = std::make_unique<PredictionSystem>();
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        developmentSystem = std::make_unique<DevelopmentSystem>();
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
        
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage{DevelopmentalStage::Initial};
    RegionId nextRegionId{1};
};

BrainIntegration::BrainIntegration(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

BrainIntegration::~BrainIntegration() = default;

bool BrainIntegration::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
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
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
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
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize memory systems
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    pImpl->associativeMemory->initialize(this);
    
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
    
    // Register spike handlers
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
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

// Forward declarations for method definitions
void BrainIntegration::runIntegratedSimulation(size_t maxSteps);
void BrainIntegration::simulateStep(SimulationStep currentStep, Timestamp currentTime);
void BrainIntegration::updateAllNeurons(Timestamp currentTime);
void BrainIntegration::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime);
void BrainIntegration::updateWorkingMemory();
void BrainIntegration::updateNeuromodulation();
void BrainIntegration::applyPlasticityRules();
void BrainIntegration::updateEpisodicMemory(SimulationStep currentStep);
void BrainIntegration::updatePredictionSystem();
void BrainIntegration::updateAttentionSystem(SimulationStep currentStep);
void BrainIntegration::updateConceptFormation();
void BrainIntegration::applyStructuralPlasticity(SimulationStep currentStep);
void BrainIntegration::replayMemories(SimulationStep currentStep);
void BrainIntegration::applyDevelopmentEffects(SimulationStep currentStep);
void BrainIntegration::consolidateMemories();

// Core simulation methods implementation
void BrainIntegration::runIntegratedSimulation(size_t maxSteps) {
    for (size_t step = 0; step < maxSteps; ++step) {
        simulateStep(step, step * pImpl->timestep);
        if (step % 1000 == 0) {
            logStatus();
        }
    }
}

void BrainIntegration::simulateStep(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // Step 1: Process pending delayed spikes
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // Step 2: Update all neurons (LIF dynamics)
    updateAllNeurons(currentTime);
    
    // Step 3: Detect spikes and schedule events
    detectAndProcessSpikes(currentStep, currentTime);
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
    
    // Step 4: Update working memory
    updateWorkingMemory();
    
    // Step 5: Apply neuromodulation
    updateNeuromodulation();
    
    // Step 6: Apply plasticity rules
    applyPlasticityRules();
    
    // Step 7: Update episodic memory
    updateEpisodicMemory(currentStep);
    
    // Step 8: Update prediction system
    updatePredictionSystem();
    
    // Step 9: Update attention system
    updateAttentionSystem(currentStep);
    
    // Step 10: Update concept formation
    updateConceptFormation();
    
    // Step 11: Apply structural plasticity
    if (currentStep % 100 == 0) {
        applyStructuralPlasticity(currentStep);
    }
    
    // Step 12: Replay memories
    if (currentStep % pImpl->replayInterval == 0) {
        replayMemories(currentStep);
    }
    
    // Step 13: Apply development effects
    if (currentStep % 1000 == 0) {
        applyDevelopmentEffects(currentStep);
    }
    
    // Step 14: Memory consolidation
    if (currentStep % pImpl->consolidationInterval == 0) {
        consolidateMemories();
    }
    
    // Step 15: Checkpoint management
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void BrainIntegration::updateAllNeurons(Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void BrainIntegration::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Queue spike event
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Process plasticity for incoming synapses
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Schedule delayed spike events for outgoing synapses
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
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
                        syn->recordPreSpike(currentTime);
                    }

                    // Store in working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
}

void BrainIntegration::updateWorkingMemory() {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void BrainIntegration::updateNeuromodulation() {
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
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
}

void BrainIntegration::applyPlasticityRules() {
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            syn->step(pImpl->currentTime);
        }
    }
}

void BrainIntegration::updateEpisodicMemory(SimulationStep currentStep) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
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
            
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainIntegration::updatePredictionSystem() {
    if (pImpl->predictionSystem) {
        // Prediction system update logic
    }
}

void BrainIntegration::updateAttentionSystem(SimulationStep currentStep) {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void BrainIntegration::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Concept formation update logic
    }
}

void BrainIntegration::applyStructuralPlasticity(SimulationStep currentStep) {
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void BrainIntegration::replayMemories(SimulationStep currentStep) {
    if (pImpl->episodicMemory) {
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void BrainIntegration::applyDevelopmentEffects(SimulationStep currentStep) {
    pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
    
    auto* sp = pImpl->structuralPlasticity;
    if (sp) {
        float plasticityMod = 1.0f;
        
        switch (pImpl->developmentalStage) {
            case DevelopmentalStage::Initial: plasticityMod = 1.0f; break;
            case DevelopmentalStage::CriticalPeriod: plasticityMod = 0.8f; break;
            case DevelopmentalStage::Maturation: plasticityMod = 0.5f; break;
            case DevelopmentalStage::Adult: plasticityMod = 0.2f; break;
        }
        
        sp->setSynaptogenesisRate(0.0001f * plasticityMod);
        sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void BrainIntegration::consolidateMemories() {
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

// Accessor methods implementation
NeuralWorkingMemory* BrainIntegration::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* BrainIntegration::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* BrainIntegration::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

PredictionSystem* BrainIntegration::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

NeuralPlanner* BrainIntegration::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* BrainIntegration::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* BrainIntegration::getAttention() {
    return pImpl->attention.get();
}

DevelopmentSystem* BrainIntegration::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage BrainIntegration::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void BrainIntegration::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

Dopamine* BrainIntegration::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* BrainIntegration::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* BrainIntegration::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* BrainIntegration::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> BrainIntegration::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainIntegration::getRandomGenerator() {
    return pImpl->rng.get();
}

SpikeSystem* BrainIntegration::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* BrainIntegration::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* BrainIntegration::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* BrainIntegration::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* BrainIntegration::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float BrainIntegration::getExcitationInhibitionRatio() const {
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

size_t BrainIntegration::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t BrainIntegration::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> BrainIntegration::produceAction() {
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    return std::make_unique<Action>(type);
}

void BrainIntegration::applyNeuromodulation(const class Neuromodulator& signal) {
    float modulation = signal.getLevel();
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void BrainIntegration::updatePlasticity() {
    // Plasticity is now applied during each step
}

void BrainIntegration::develop() {
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void BrainIntegration::reset() {
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
    
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
}

bool BrainIntegration::save(const std::string& filepath) const {
    // Save implementation...
    return true;
}

bool BrainIntegration::load(const std::string& filepath) {
    // Load implementation...
    return true;
}

// Region management methods
void BrainIntegration::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
}

NeuralRegion* BrainIntegration::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainIntegration::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t BrainIntegration::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainIntegration::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainIntegration::getRegions() const {
    return pImpl->regions;
}

void BrainIntegration::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void BrainIntegration::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t BrainIntegration::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t BrainIntegration::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t BrainIntegration::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

float BrainIntegration::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

void BrainIntegration::receiveSensoryInput(const class SensoryInput& input) {
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    for (size_t i = 0; i < numSensory; ++i) {
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainIntegration::injectCurrent(NeuronId neuron, MembranePotential current) {
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

void BrainIntegration::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
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

void BrainIntegration::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Integration Status ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm