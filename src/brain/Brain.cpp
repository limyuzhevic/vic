#include "Brain.hpp"
#include "BrainRefactor.hpp"
#include <mutex>

namespace nlm {

// Thread safety for concurrent access
static std::mutex brainStepMutex;

// Implementation details (Pimpl)
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
        , replayInterval(100)
        , consolidationInterval(1000)
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
    return BrainInitializer::initializeBrain(this);
}

void Brain::step(SimulationStep currentStep) {
    std::lock_guard<std::mutex> lock(brainStepMutex);
    BrainSimulator::simulateStep(this, currentStep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    std::lock_guard<std::mutex> lock(brainStepMutex);
    BrainSimulator::simulateStep(this, currentStep, currentTime);
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
    BrainIO::handleSensoryInput(this, input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    BrainIO::injectCurrent(this, neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    BrainIO::injectCurrentToNeurons(this, type, current);
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
    return BrainStatistics::getExcitationInhibitionRatio(this);
}

size_t Brain::getTotalSpikeCount() const {
    return BrainStatistics::getTotalSpikeCount(this);
}

size_t Brain::getPendingSpikeEventCount() const {
    return BrainStatistics::getPendingSpikeEventCount(this);
}

std::unique_ptr<Action> Brain::produceAction() {
    return BrainIO::produceMotorOutput(this);
}

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
    BrainIO::applyNeuromodulation(this, signal);
}

void Brain::updatePlasticity() {
    BrainManager::updatePlasticity(this);
}

void Brain::develop() {
    BrainManager::develop(this);
}

void Brain::reset() {
    BrainManager::reset(this);
}

bool Brain::save(const std::string& filepath) const {
    return BrainManager::saveBrainState(const_cast<Brain*>(this), filepath);
}

bool Brain::load(const std::string& filepath) {
    return BrainManager::loadBrainState(this, filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId newId = pImpl->nextRegionId++;
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(name));
    return newId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return BrainManager::getRegion(this, id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return BrainManager::getRegion(this, id);
}

size_t Brain::getRegionCount() const {
    return BrainStatistics::getRegionCount(this);
}

std::vector<RegionId> Brain::getRegionIds() const {
    return BrainStatistics::getRegionIds(this);
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                      float weight, Delay delay) {
    InterRegionConnection conn(source, target, weight, delay);
    pImpl->interRegionConnections.push_back(conn);
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
    return BrainStatistics::getTotalNeuronCount(this);
}

size_t Brain::getTotalSynapseCount() const {
    return BrainStatistics::getTotalSynapseCount(this);
}

size_t Brain::getActiveNeuronCount() const {
    return BrainStatistics::getActiveNeuronCount(this);
}

size_t Brain::getFiringNeuronCount() const {
    return BrainStatistics::getFiringNeuronCount(this);
}

float Brain::getAverageFiringRate() const {
    return BrainStatistics::getAverageFiringRate(this);
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

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    BrainStatistics::logStatus(this);
}

} // namespace nlm
