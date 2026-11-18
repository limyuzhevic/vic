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

// Include modular components
#include "BrainInitialization.hpp"
#include "BrainStep.hpp"
#include "BrainIO.hpp"
#include "BrainMemory.hpp"
#include "BrainStatistics.hpp"
#include "BrainIntegration.hpp"

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
    
    // Modular components
    BrainInitialization* initialization;
    BrainStep* step;
    BrainIO* io;
    BrainMemory* memory;
    BrainStatistics* statistics;
    BrainIntegration* integration;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(RegionId(1))
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
        , initialization(nullptr)
        , step(nullptr)
        , io(nullptr)
        , memory(nullptr)
        , statistics(nullptr)
        , integration(nullptr)
    {}
    
    ~Impl() {
        delete initialization;
        delete step;
        delete io;
        delete memory;
        delete statistics;
        delete integration;
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {
    // Initialize modular components
    pImpl->initialization = new BrainInitialization(pImpl, config);
    pImpl->step = new BrainStep(pImpl);
    pImpl->io = new BrainIO(pImpl);
    pImpl->memory = new BrainMemory(pImpl);
    pImpl->statistics = new BrainStatistics(pImpl);
    pImpl->integration = new BrainIntegration(pImpl);
    
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

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
    return pImpl->initialization->perform(pImpl, pImpl->config);
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->step->perform(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->io->performSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->io->performInjectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->io->performInjectCurrentToNeurons(type, current);
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
    return pImpl->statistics->performExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->statistics->performTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->statistics->performPendingSpikeEventCount();
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
    pImpl->integration->performNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    pImpl->integration->performPlasticityUpdate();
}

void Brain::develop() {
    pImpl->integration->performDevelopment();
}

void Brain::reset() {
    pImpl->integration->performReset();
}

bool Brain::save(const std::string& filepath) const {
    pImpl->integration->performCheckpointSave(filepath);
    return true;
}

bool Brain::load(const std::string& filepath) {
    pImpl->integration->performCheckpointLoad(filepath);
    return true;
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId regionId = pImpl->nextRegionId++;
    pImpl->regions.emplace_back(std::make_unique<NeuralRegion>(name.empty() ? "Region_" + std::to_string(regionId) : name));
    pImpl->regions.back()->setRegionId(regionId);
    return regionId;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getRegionId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getRegionId() == id) {
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
        ids.push_back(region->getRegionId());
    }
    return ids;
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
        std::remove_if(pImpl->interRegionConnections.begin(), pImpl->interRegionConnections.end(),
            [source, target](const InterRegionConnection& conn) {
                return conn.sourceRegion == source && conn.targetRegion == target;
            }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getTotalNeuronCount();
    }
    return count;
}

size_t Brain::getTotalSynapseCount() const {
    size_t count = 0;
    for (const auto& region : pImpl->regions) {
        count += region->getSynapseCount();
    }
    return count;
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->statistics->performActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->statistics->performFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->statistics->performAverageFiringRate();
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

float Brain::getLearnedPreference() const {
    return pImpl->learnedPreference;
}

void Brain::setLearnedPreference(float preference) {
    pImpl->learnedPreference = preference;
}

void Brain::logStatus() const {
    pImpl->statistics->performLogStatus();
}

} // namespace nlm