#include "Brain.hpp"
#include "BrainCore.hpp"
#include "BrainMemory.hpp"
#include "BrainPrediction.hpp"
#include "BrainCognition.hpp"
#include "BrainNeuromodulation.hpp"
#include "BrainDevelopment.hpp"
#include "BrainCheckpoint.hpp"
#include "BrainIO.hpp"
#include "BrainStatistics.hpp"
#include "BrainRegion.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/ExperimentRunner.hpp"

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED SYSTEMS ==========
    
    // Memory systems
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // Prediction system
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // Cognition systems
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
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
    
    // Subsystems
    BrainCore brainCore;
    BrainMemory brainMemory;
    BrainPrediction brainPrediction;
    BrainCognition brainCognition;
    BrainNeuromodulation brainNeuromodulation;
    BrainDevelopment brainDevelopment;
    BrainCheckpoint brainCheckpoint;
    BrainIO brainIO;
    BrainStatistics brainStatistics;
    BrainRegion brainRegion;
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
    
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
        , consolidationInterval(1000)  // Consolidate every 1000 steps)
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
    return pImpl->brainCore.initialize(*pImpl);
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->brainCore.processDelayedSpikes(*pImpl, currentStep, currentTime);
    
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
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->brainCore.processSpikes(*pImpl, currentStep);
    
    // ========== STEP 4: Update working memory ==========
    pImpl->brainMemory.updateWorkingMemory(*pImpl, pImpl->timestep);
    
    // ========== STEP 5: Apply neuromodulation effects ==========
    pImpl->brainNeuromodulation.updateNeuromodulationSystems(*pImpl, pImpl->timestep);
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
    pImpl->brainCore.updatePlasticity(*pImpl);
    
    // ========== STEP 7: Update episodic memory ==========
    pImpl->brainMemory.updateEpisodicMemory(*pImpl, currentStep);
    
    // ========== STEP 8: Update prediction system ==========
    pImpl->brainPrediction.updatePredictionSystem(*pImpl);
    
    // ========== STEP 9: Update attention system ==========
    pImpl->brainCognition.updateCognitionSystems(*pImpl, pImpl->timestep);
    
    // ========== STEP 10: Apply structural plasticity periodically ==========
    pImpl->brainCore.applyStructuralPlasticity(*pImpl, currentStep);
    
    // ========== STEP 11: Replay important memories ==========
    pImpl->brainCore.replayMemories(*pImpl, currentStep);
    
    // ========== STEP 12: Apply development effects ==========
    pImpl->brainDevelopment.updateDevelopmentSystem(*pImpl, pImpl->timestep * 1000, currentStep);
    
    // ========== STEP 13: Periodic memory consolidation ==========
    pImpl->brainCore.consolidateMemories(*pImpl, currentStep);
    
    // ========== STEP 14: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->brainIO.receiveSensoryInput(*pImpl, input);
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
    return pImpl->brainCore.produceAction(*pImpl);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->brainCore.applyNeuromodulation(*pImpl, signal);
}

void Brain::updatePlasticity() {
    pImpl->brainCore.updatePlasticity(*pImpl);
}

void Brain::develop() {
    pImpl->brainDevelopment.updateDevelopmentSystem(*pImpl, pImpl->timestep, pImpl->currentStep);
}

void Brain::reset() {
    pImpl->brainCore.reset(*pImpl);
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->brainCheckpoint.saveBrainState(*pImpl, filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->brainCheckpoint.loadBrainState(*pImpl, filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->brainRegion.addRegion(*pImpl, name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->brainRegion.getRegion(*pImpl, id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->brainRegion.getRegion(*pImpl, id);
}

size_t Brain::getRegionCount() const {
    return pImpl->brainRegion.getRegionCount(*pImpl);
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->brainRegion.getRegionIds(*pImpl);
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->brainRegion.getRegions(*pImpl);
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->brainRegion.addInterRegionConnection(*pImpl, source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->brainRegion.removeInterRegionConnection(*pImpl, source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->brainRegion.getTotalNeuronCount(*pImpl);
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->brainRegion.getTotalSynapseCount(*pImpl);
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->brainRegion.getActiveNeuronCount(*pImpl);
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->brainRegion.getFiringNeuronCount(*pImpl);
}

float Brain::getAverageFiringRate() const {
    return pImpl->brainRegion.getAverageFiringRate(*pImpl);
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
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

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
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

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    pImpl->brainStatistics.logBrainStatus(*pImpl);
    pImpl->brainMemory.logMemoryStats(*pImpl);
    pImpl->brainPrediction.logPredictionStats(*pImpl);
    pImpl->brainCognition.logCognitionStats(*pImpl);
    pImpl->brainNeuromodulation.logNeuromodulationStats(*pImpl);
    pImpl->brainDevelopment.logDevelopmentStats(*pImpl);
    pImpl->brainCheckpoint.logCheckpointStats(*pImpl);
    pImpl->brainIO.logIOStats(*pImpl);
}

} // namespace nlm
