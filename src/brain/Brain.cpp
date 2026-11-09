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
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
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
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<int64_t>("random_seed")) {
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
        replayInterval = static_cast<size_t>(config->getOr<int64_t>("replay_interval", 100));
        consolidationInterval = static_cast<size_t>(config->getOr<int64_t>("consolidation_interval", 1000));
        
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
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Use BrainRegionManager to set up regions and neurons
    BrainRegionManager::setupRegionsAndNeurons(pImpl.get(), neuronCount, regionCount, connectionProbability);
    
    // Use BrainRegionManager to initialize integrated systems
    BrainRegionManager::initializeIntegratedSystems(pImpl.get());
    
    // Use BrainRegionManager to register spike handlers
    BrainRegionManager::registerSpikeHandlers(pImpl.get());
    
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
    // Use BrainStepLoop to execute the 15-step integration loop
    BrainStepLoop::executeIntegrationLoop(pImpl.get(), currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Use BrainIOSystem to process sensory input
    BrainIOSystem::processSensoryInput(pImpl.get(), input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Use BrainIOSystem to inject current into a specific neuron
    BrainIOSystem::injectCurrent(pImpl.get(), neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Use BrainIOSystem to inject current into neurons of a specific type
    BrainIOSystem::injectCurrentToNeurons(pImpl.get(), type, current);
}

SpikeSystem* Brain::getSpikeSystem() {
    // Use BrainIOSystem to get spike system
    return BrainIOSystem::getSpikeSystem(pImpl.get());
}

const SpikeSystem* Brain::getSpikeSystem() const {
    // Use BrainIOSystem to get spike system
    return BrainIOSystem::getSpikeSystem(pImpl.get());
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
    // Use BrainIOSystem to calculate E/I ratio
    return BrainIOSystem::getExcitationInhibitionRatio(pImpl.get());
}

size_t Brain::getTotalSpikeCount() const {
    // Use BrainIOSystem to get total spike count
    return BrainIOSystem::getTotalSpikeCount(pImpl.get());
}

size_t Brain::getPendingSpikeEventCount() const {
    // Use BrainIOSystem to get pending spike event count
    return BrainIOSystem::getPendingSpikeEventCount(pImpl.get());
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Use BrainIOSystem to produce action
    return BrainIOSystem::produceAction(pImpl.get());
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Use BrainIOSystem to apply neuromodulation
    BrainIOSystem::applyNeuromodulation(pImpl.get(), signal);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Use BrainIOSystem to apply developmental changes
    BrainIOSystem::develop(pImpl.get());
}

void Brain::reset() {
    // Use BrainIOSystem to reset brain state
    BrainIOSystem::reset(pImpl.get());
}

bool Brain::save(const std::string& filepath) const {
    // Use BrainIOSystem to save brain state
    return BrainIOSystem::save(pImpl.get(), filepath);
}

bool Brain::load(const std::string& filepath) {
    // Use BrainIOSystem to load brain state
    return BrainIOSystem::load(pImpl.get(), filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    // Use BrainRegionManager to add a region
    return BrainRegionManager::addRegion(pImpl.get(), name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    // Use BrainRegionManager to get a region
    return BrainRegionManager::getRegion(pImpl.get(), id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    // Use BrainRegionManager to get a region (const version)
    return BrainRegionManager::getRegion(pImpl.get(), id);
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

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                      float weight, Delay delay) {
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
    // Use BrainIOSystem to get total neuron count
    return BrainIOSystem::getTotalNeuronCount(pImpl.get());
}

size_t Brain::getTotalSynapseCount() const {
    // Use BrainIOSystem to get total synapse count
    return BrainIOSystem::getTotalSynapseCount(pImpl.get());
}

size_t Brain::getActiveNeuronCount() const {
    // Use BrainIOSystem to get active neuron count
    return BrainIOSystem::getActiveNeuronCount(pImpl.get());
}

size_t Brain::getFiringNeuronCount() const {
    // Use BrainIOSystem to get firing neuron count
    return BrainIOSystem::getFiringNeuronCount(pImpl.get());
}

float Brain::getAverageFiringRate() const {
    // Use BrainIOSystem to get average firing rate
    return BrainIOSystem::getAverageFiringRate(pImpl.get());
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

float Brain::getLearnedPreference() const {
    // Use BrainIOSystem to get learned preference
    return BrainIOSystem::getLearnedPreference(pImpl.get());
}

void Brain::setLearnedPreference(float preference) {
    // Use BrainIOSystem to set learned preference
    BrainIOSystem::setLearnedPreference(pImpl.get(), preference);
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
    // Use BrainIOSystem to get developmental stage
    return BrainIOSystem::getDevelopmentalStage(pImpl.get());
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    // Use BrainIOSystem to set developmental stage
    BrainIOSystem::setDevelopmentalStage(pImpl.get(), stage);
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
    // Use BrainIOSystem to log brain status
    BrainIOSystem::logStatus(pImpl.get());
}

} // namespace nlm
