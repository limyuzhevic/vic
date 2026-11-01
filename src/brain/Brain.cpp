#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>

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
    std::unique_ptr<Reward> reward;
    
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

    // Phase 5 learned preference trait (see Brain.hpp). Updated only
    // from sandbox reward via AgentBrain; checkpointed with the
    // simulation state so a reloaded brain keeps what it learned.
    float learnedPreference = 0.0f;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    // NeuronId -> neuron, for O(1) synaptic delivery (was a linear scan
    // over all neurons per delayed spike, which made real networks unusable).
    std::unordered_map<uint64_t, Neuron*> neuronById;
    bool spikeHandlersRegistered = false;
    
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
        , replayInterval(100)
        , consolidationInterval(1000) {}
};

Brain::Brain(std::shared_ptr<Config> config) 
    : pImpl(new Impl(config))
    , initializer(std::make_unique<BrainInitializer>())
    , simulator(std::make_unique<BrainSimulator>())
    , checkpoint(std::make_unique<BrainCheckpoint>())
    , regionManager(std::make_unique<BrainRegionManager>()) {
    // Initialize core components
    pImpl->config = config;
    
    // Initialize random generator
    uint64_t seed = 42;
    if (auto seedOpt = config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl), 
    initializer(std::move(other.initializer)),
    simulator(std::move(other.simulator)),
    checkpoint(std::move(other.checkpoint)),
    regionManager(std::move(other.regionManager)) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
        
        initializer = std::move(other.initializer);
        simulator = std::move(other.simulator);
        checkpoint = std::move(other.checkpoint);
        regionManager = std::move(other.regionManager);
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Use BrainInitializer to setup all components
    if (!initializer->initializeBrain(this, pImpl->config)) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        return false;
    }
    
    // Use BrainRegionManager to setup regions
    initializer->setupRegions(this);
    
    // Use BrainInitializer to setup integrated systems
    initializer->initializeIntegratedSystems(this);
    
    // Configure plasticity
    initializer->configurePlasticity(this);
    
    // Setup spike system handlers
    initializer->setupSpikeSystem(this);
    
    // Configure checkpoint system
    initializer->configureCheckpointSystem(this);
    
    // Log status
    logStatus();
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = static_cast<Timestamp>(currentStep) * pImpl->timestep;
    simulator->simulate(this, currentStep, pImpl->currentTime);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    simulator->simulate(this, currentStep, currentTime);
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
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
    auto it = pImpl->neuronById.find(neuron.index());
    if (it != pImpl->neuronById.end()) {
        it->second->injectCurrent(current);
        return;
    }
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
    return regionManager->getExcitationInhibitionRatio(this);
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<Action> Brain::produceAction() {
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

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    if (pImpl->stdp) {
        pImpl->stdp->setLTPWeight(0.01f * modulation);
        pImpl->stdp->setLTDWeight(0.012f * modulation);
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    if (pImpl->structuralPlasticity) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
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
    
    if (pImpl->spikeSystem) {
        pImpl->spikeSystem->reset();
    }
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    return checkpoint->save(const_cast<Brain*>(this), filepath);
}

bool Brain::load(const std::string& filepath) {
    return checkpoint->load(this, filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return regionManager->addRegion(this, name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return regionManager->getRegion(this, id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return regionManager->getRegion(this, id);
}

size_t Brain::getRegionCount() const {
    return regionManager->getRegionCount(this);
}

std::vector<RegionId> Brain::getRegionIds() const {
    return regionManager->getRegionIds(this);
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return regionManager->getRegions(this);
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight, Delay delay) {
    regionManager->addInterRegionConnection(this, source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    regionManager->removeInterRegionConnection(this, source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return regionManager->getTotalNeuronCount(this);
}

size_t Brain::getTotalSynapseCount() const {
    return regionManager->getTotalSynapseCount(this);
}
size_t Brain::getActiveNeuronCount() const {
    return regionManager->getActiveNeuronCount(this);
}

size_t Brain::getFiringNeuronCount() const {
    return regionManager->getFiringNeuronCount(this);
}

float Brain::getAverageFiringRate() const {
    return regionManager->getAverageFiringRate(this);
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

Reward* Brain::getReward() {
    return pImpl->reward.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

float Brain::getLearnedPreference() const {
    return pImpl->learnedPreference;
}

void Brain::setLearnedPreference(float preference) {
    pImpl->learnedPreference = preference;
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== Final Brain Status ===");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
}

} // namespace nlm
