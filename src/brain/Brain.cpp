// Implementation of component headers

#include "brain/brain_core.hpp"
#include "brain/brain_neural.hpp"
#include "brain/brain_memory.hpp"
#include "brain/brain_prediction.hpp"
#include "brain/brain_cognition.hpp"
#include "brain/brain_neuromodulation.hpp"
#include "brain/brain_development.hpp"
#include "brain/brain_checkpoint.hpp"
#include "brain/brain_stats.hpp"
#include "brain/brain_utils.hpp"

#include "brain/Brain.hpp"
#include "brain/NeuralRegion.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralPopulation.hpp"
#include "dynamics/SpikeSystem.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace nlm;

// ========== BRAIN CORE IMPLEMENTATION ==========

struct BrainCore::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    SimulationClock* simulationClock = nullptr;
    DevelopmentalStage developmentalStage = DevelopmentalStage::Initial;
    RegionId nextRegionId = RegionId(1);
    size_t totalNeuronCount = 0;
    size_t totalSynapseCount = 0;
};

BrainCore::BrainCore(std::shared_ptr<Config> cfg) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = cfg;
    pImpl->rng = std::make_unique<RandomGenerator>(42);
}

BrainCore::~BrainCore() = default;

bool BrainCore::initialize() {
    // Initialize simulation clock
    pImpl->simulationClock = new SimulationClock(pImpl->config->getOr<double>("simulation_timestep", 0.001));
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(pImpl->config->getOr<int64_t>("region_count", 1));
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.15f);
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        pImpl->regions.push_back(std::make_unique<NeuralRegion>("Region_" + std::to_string(i + 1)));
    }
    
    pImpl->totalNeuronCount = neuronCount;
    return true;
}

RegionId BrainCore::addRegion(const std::string& name) {
    RegionId newId = pImpl->nextRegionId++;
    pImpl->regions.push_back(std::make_unique<NeuralRegion>(name, newId));
    return newId;
}

NeuralRegion* BrainCore::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* BrainCore::getRegion(RegionId id) const {
    return const_cast<BrainCore*>(this)->getRegion(id);
}

size_t BrainCore::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> BrainCore::getRegionIds() const {
    std::vector<RegionId> ids;
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& BrainCore::getRegions() const {
    return pImpl->regions;
}

void BrainCore::addInterRegionConnection(RegionId source, RegionId target, 
                                        float weight, Delay delay) {
    InterRegionConnection conn;
    conn.sourceRegion = source;
    conn.targetRegion = target;
    conn.weight = weight;
    conn.delay = delay;
    pImpl->interRegionConnections.push_back(conn);
}

void BrainCore::removeInterRegionConnection(RegionId source, RegionId target) {
    auto it = std::remove_if(pImpl->interRegionConnections.begin(), 
                           pImpl->interRegionConnections.end(),
                           [source, target](const InterRegionConnection& conn) {
                               return conn.sourceRegion == source && conn.targetRegion == target;
                           });
    pImpl->interRegionConnections.erase(it, pImpl->interRegionConnections.end());
}

std::shared_ptr<Config> BrainCore::getConfig() const {
    return pImpl->config;
}

RandomGenerator* BrainCore::getRandomGenerator() {
    return pImpl->rng.get();
}

SimulationClock* BrainCore::getSimulationClock() {
    return pImpl->simulationClock;
}

size_t BrainCore::getTotalNeuronCount() const {
    return pImpl->totalNeuronCount;
}

size_t BrainCore::getTotalSynapseCount() const {
    return pImpl->totalSynapseCount;
}

DevelopmentalStage BrainCore::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void BrainCore::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== BRAIN NEURAL IMPLEMENTATION ==========

struct BrainNeural::Impl {
    SpikeSystem* spikeSystem = nullptr;
    STDP* stdp = nullptr;
    Hebbian* hebbian = nullptr;
    StructuralPlasticity* structuralPlasticity = nullptr;
};

void BrainNeural::step(double timestep, SimulationStep currentStep, Timestamp currentTime,
                      const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    // Neural computation: update all neurons (LIF dynamics)
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, timestep);
            }
        }
    }
    
    // Spike processing
    if (spikeSystem) {
        spikeSystem->processSpikes(currentStep);
    }
}

void BrainNeural::receiveSensoryInput(const SensoryInput& input,
                                     std::vector<Neuron*>& sensoryNeurons,
                                     std::shared_ptr<class NeuralWorkingMemory> workingMemory) {
    // Inject current into sensory neurons based on input
    for (size_t i = 0; i < sensoryNeurons.size() && i < input.getData().size(); ++i) {
        float normalizedValue = input.getData()[i] * 10.0f;
        sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Store in working memory
        if (workingMemory && normalizedValue > 0.5f) {
            workingMemory->storeToNeuron(sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void BrainNeural::injectCurrent(NeuronId neuron, MembranePotential current,
                               const std::unordered_map<uint64_t, Neuron*>& neuronById) {
    auto it = neuronById.find(neuron.index());
    if (it != neuronById.end()) {
        it->second->injectCurrent(current);
    }
}

void BrainNeural::injectCurrentToNeurons(NeuronType type, MembranePotential current,
                                        const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

void BrainNeural::processSpikes(SimulationStep currentStep, SpikeSystem* spikeSystem) {
    pImpl->spikeSystem = spikeSystem;
    if (spikeSystem) {
        spikeSystem->processSpikes(currentStep);
    }
}

SpikeSystem* BrainNeural::getSpikeSystem() const {
    return pImpl->spikeSystem;
}

STDP* BrainNeural::getSTDP() const {
    return pImpl->stdp;
}

Hebbian* BrainNeural::getHebbian() const {
    return pImpl->hebbian;
}

StructuralPlasticity* BrainNeural::getStructuralPlasticity() const {
    return pImpl->structuralPlasticity;
}

// ========== BRAIN MEMORY IMPLEMENTATION ==========

struct BrainMemory::Impl {
    NeuralWorkingMemory* workingMemory = nullptr;
    NeuralEpisodicMemory* episodicMemory = nullptr;
    NeuralAssociativeMemory* associativeMemory = nullptr;
};

void BrainMemory::initialize(std::shared_ptr<class Brain> brain, size_t neuronCount) {
    // This would initialize the memory systems using the brain
    // For now, this is a placeholder
}

void BrainMemory::updateWorkingMemory(double timestep,
                                     std::shared_ptr<NeuralWorkingMemory> workingMemory,
                                     const std::vector<std::unique_ptr<class NeuralRegion>>& regions) {
    if (workingMemory) {
        workingMemory->update(timestep);
    }
}

void BrainMemory::storeEpisodicMemory(const std::vector<std::unique_ptr<class NeuralRegion>>& regions,
                                     SimulationStep currentStep,
                                     float reward,
                                     std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                     std::shared_ptr<class Dopamine> dopamine,
                                     SimulationStep& stepsSinceLastEpisode,
                                     const SimulationStep replayInterval,
                                     const SimulationStep consolidationInterval) {
    stepsSinceLastEpisode++;
    if (stepsSinceLastEpisode >= replayInterval) {
        stepsSinceLastEpisode = 0;
        
        if (episodicMemory) {
            // Capture current brain state as an episode
            // This would implement episodic memory storage
        }
    }
}

void BrainMemory::updateMemoryConsolidation(SimulationStep currentStep,
                                           std::shared_ptr<NeuralEpisodicMemory> episodicMemory,
                                           const SimulationStep replayInterval,
                                           const SimulationStep consolidationInterval) {
    if (currentStep % consolidationInterval == 0 && episodicMemory) {
        // Consolidate important memories
        episodicMemory->consolidate(0.3f);
    }
}

NeuralWorkingMemory* BrainMemory::getWorkingMemory() const {
    return pImpl->workingMemory;
}

NeuralEpisodicMemory* BrainMemory::getEpisodicMemory() const {
    return pImpl->episodicMemory;
}

NeuralAssociativeMemory* BrainMemory::getAssociativeMemory() const {
    return pImpl->associativeMemory;
}

void BrainMemory::clearWorkingMemory(std::shared_ptr<NeuralWorkingMemory> workingMemory) {
    if (workingMemory) workingMemory->clear();
}

void BrainMemory::clearEpisodicMemory(std::shared_ptr<NeuralEpisodicMemory> episodicMemory) {
    if (episodicMemory) episodicMemory->clear();
}

void BrainMemory::clearAssociativeMemory(std::shared_ptr<NeuralAssociativeMemory> associativeMemory) {
    if (associativeMemory) associativeMemory->clear();
}

// ========== BRAIN CHECKPOINT IMPLEMENTATION ==========

bool BrainCheckpoint::save(const std::string& filepath,
                          const std::vector<std::unique_ptr<class NeuralRegion>>& regions,
                          size_t totalSpikesTotal,
                          size_t totalSpikesThisStep,
                          float learnedPreference,
                          SimulationStep stepsSinceLastEpisode,
                          std::shared_ptr<class RandomGenerator> rng,
                          std::shared_ptr<class SpikeSystem> spikeSystem) {
    // Placeholder for checkpoint saving
    // In a real implementation, this would save the brain state to file
    return true;
}

bool BrainCheckpoint::load(const std::string& filepath,
                          std::vector<std::unique_ptr<class NeuralRegion>>& regions,
                          size_t& totalSpikesTotal,
                          size_t& totalSpikesThisStep,
                          float& learnedPreference,
                          SimulationStep& stepsSinceLastEpisode,
                          std::shared_ptr<class RandomGenerator> rng,
                          std::shared_ptr<class SpikeSystem> spikeSystem) {
    // Placeholder for checkpoint loading
    // In a real implementation, this would load the brain state from file
    return true;
}

std::string BrainCheckpoint::getCheckpointStatus() const {
    return "Checkpoint system available (placeholder implementation)";
}

// ========== BRAIN STATS IMPLEMENTATION ==========

float BrainStats::calculateExcitationInhibitionRatio(
    const std::vector<std::unique_ptr<class NeuralRegion>>& regions) const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : regions) {
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

float BrainStats::calculateAverageFiringRate(
    const std::vector<std::unique_ptr<class NeuralRegion>>& regions) const {
    // Placeholder calculation
    return 50.0f;
}

size_t BrainStats::getActiveNeuronCount(
    const std::vector<std::unique_ptr<class NeuralRegion>>& regions) const {
    size_t count = 0;
    for (const auto& region : regions) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->isFiring()) {
                count++;
            }
        }
    }
    return count;
}

size_t BrainStats::getFiringNeuronCount(
    const std::vector<Neuron*>& motorNeurons) const {
    size_t count = 0;
    for (auto* neuron : motorNeurons) {
        if (neuron->isFiring()) {
            count++;
        }
    }
    return count;
}

std::string BrainStats::generateStatusLog(size_t totalSpikesTotal,
                                         size_t totalSpikesThisStep,
                                         size_t totalNeuronCount,
                                         size_t totalSynapseCount,
                                         size_t regionCount,
                                         size_t sensoryNeuronsCount,
                                         size_t motorNeuronsCount,
                                         const DevelopmentalStage& developmentalStage) const {
    std::stringstream ss;
    ss << "=== NLM Brain Status ===" << std::endl;
    ss << "Total Spikes: " << totalSpikesTotal << std::endl;
    ss << "Spikes This Step: " << totalSpikesThisStep << std::endl;
    ss << "Neurons: " << totalNeuronCount << std::endl;
    ss << "Synapses: " << totalSynapseCount << std::endl;
    ss << "Regions: " << regionCount << std::endl;
    ss << "Sensory Neurons: " << sensoryNeuronsCount << std::endl;
    ss << "Motor Neurons: " << motorNeuronsCount << std::endl;
    ss << "Developmental Stage: " << static_cast<int>(developmentalStage) << std::endl;
    return ss.str();
}

std::string BrainStats::getStatisticsSummary(size_t totalSpikesTotal,
                                            size_t totalSpikesThisStep,
                                            size_t totalNeuronCount,
                                            size_t totalSynapseCount) const {
    std::stringstream ss;
    ss << "Spikes: " << totalSpikesTotal << " (this step: " << totalSpikesThisStep << ")";
    ss << "\nNeurons: " << totalNeuronCount << ", Synapses: " << totalSynapseCount;
    return ss.str();
}

// ========== BRAIN UTILS IMPLEMENTATION ==========

void BrainUtils::addInterRegionConnection(
    std::vector<InterRegionConnection>& connections,
    RegionId source, RegionId target, float weight, Delay delay) {
    InterRegionConnection conn;
    conn.sourceRegion = source;
    conn.targetRegion = target;
    conn.weight = weight;
    conn.delay = delay;
    connections.push_back(conn);
}

NeuralRegion* BrainUtils::findRegionById(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions,
    RegionId id) {
    for (auto& region : regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

Neuron* BrainUtils::findNeuronById(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions,
    NeuronId id) {
    for (auto& region : regions) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getId() == id) {
                return neuron;
            }
        }
    }
    return nullptr;
}

std::vector<Neuron*> BrainUtils::getMotorNeurons(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    std::vector<Neuron*> motorNeurons;
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Motor) {
                for (auto* neuron : pop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
    return motorNeurons;
}

std::vector<Neuron*> BrainUtils::getSensoryNeurons(
    const std::vector<std::unique_ptr<NeuralRegion>>& regions) {
    std::vector<Neuron*> sensoryNeurons;
    for (auto& region : regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == NeuronType::Sensory) {
                for (auto* neuron : pop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
        }
    }
    return sensoryNeurons;
}

std::unique_ptr<Action> BrainUtils::createActionFromMotorCommand(MotorCommand cmd) {
    return std::make_unique<Action>(cmd);
}

size_t BrainUtils::getPendingSpikeEventCount(
    std::shared_ptr<class SpikeSystem> spikeSystem) const {
    if (spikeSystem) {
        return spikeSystem->getPendingSpikeCount();
    }
    return 0;
}

uint64_t BrainUtils::generateSpikeEventKey(NeuronId neuron, Timestamp time) const {
    return (neuron.index() << 32) | static_cast<uint64_t>(time);
}

float BrainUtils::normalizeValue(float value, float min, float max) const {
    if (max <= min) return 0.0f;
    return (value - min) / (max - min);
}

float BrainUtils::clampValue(float value, float min, float max) const {
    return std::clamp(value, min, max);
}

std::string BrainUtils::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
       << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// Update Brain class to use components
Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>()) {
    // Initialize components
    pImpl->core = std::make_unique<BrainCore>(config);
    pImpl->neural = std::make_unique<BrainNeural>();
    pImpl->memory = std::make_unique<BrainMemory>();
    pImpl->prediction = std::make_unique<BrainPrediction>();
    pImpl->cognition = std::make_unique<BrainCognition>();
    pImpl->neuromodulation = std::make_unique<BrainNeuromodulation>();
    pImpl->development = std::make_unique<BrainDevelopment>();
    pImpl->checkpoint = std::make_unique<BrainCheckpoint>();
    pImpl->stats = std::make_unique<BrainStats>();
    pImpl->utils = std::make_unique<BrainUtils>();
}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    return pImpl->core->initialize();
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->core->getSimulationClock()->getTimestep());
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Get timestep from config
    double timestep = pImpl->core->getConfig()->getOr<double>("simulation_timestep", 0.001);
    
    // Neural computation
    pImpl->neural->step(timestep, currentStep, currentTime, pImpl->core->getRegions());
    
    // Memory updates
    pImpl->memory->updateWorkingMemory(timestep, getWorkingMemory(), pImpl->core->getRegions());
    
    // Prediction updates
    auto predictionSystem = getPredictionSystem();
    if (predictionSystem) {
        pImpl->prediction->update(timestep, predictionSystem, getSensoryInput());
    }
    
    // Cognitive updates
    pImpl->cognition->update(timestep, getPlanner(), getConceptFormation(), getAttention(), getWorkingMemory());
    
    // Neuromodulation updates
    auto dopamine = getDopamine();
    auto curiosity = getCuriosity();
    auto novelty = getNovelty();
    auto predictionError = getPredictionErrorSignal();
    pImpl->neuromodulation->update(timestep, dopamine, curiosity, novelty, predictionError, getSensoryInput());
    
    // Development updates
    auto developmentSystem = getDevelopmentSystem();
    auto structuralPlasticity = getStructuralPlasticity();
    auto rng = getRandomGenerator();
    pImpl->development->update(currentStep, shared_from_this(), developmentSystem, structuralPlasticity, rng);
    
    // Memory consolidation
    pImpl->memory->updateMemoryConsolidation(currentStep, getEpisodicMemory(), 100, 1000);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    auto sensoryNeurons = pImpl->utils->getSensoryNeurons(pImpl->core->getRegions());
    pImpl->neural->receiveSensoryInput(input, sensoryNeurons, getWorkingMemory());
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    auto neuronById = getNeuronByIdMap();
    pImpl->neural->injectCurrent(neuron, current, neuronById);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->neural->injectCurrentToNeurons(type, current, pImpl->core->getRegions());
}

// [Continue with all other Brain methods...]

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->neural->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->neural->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->neural->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->neural->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->neural->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->stats->calculateExcitationInhibitionRatio(pImpl->core->getRegions());
}

size_t Brain::getTotalSpikeCount() const {
    // Placeholder - would need to track this in the Impl
    return 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->utils->getPendingSpikeEventCount(getSpikeSystem());
}

std::unique_ptr<class Action> Brain::produceAction() {
    auto motorNeurons = pImpl->utils->getMotorNeurons(pImpl->core->getRegions());
    return pImpl->neural->produceAction(motorNeurons);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    auto dopamine = getDopamine();
    if (dopamine) {
        // Apply neuromodulation effects
    }
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    auto developmentSystem = getDevelopmentSystem();
    auto structuralPlasticity = getStructuralPlasticity();
    auto rng = getRandomGenerator();
    if (developmentSystem && structuralPlasticity && rng) {
        developmentSystem->update();
    }
}

void Brain::reset() {
    pImpl->core->reset();
    // Reset memory systems
    if (getWorkingMemory()) getWorkingMemory()->clear();
    if (getEpisodicMemory()) getEpisodicMemory()->clear();
    if (getAssociativeMemory()) getAssociativeMemory()->clear();
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->checkpoint->save(filepath, pImpl->core->getRegions(),
                                 getTotalSpikeCount(), 0, getLearnedPreference(),
                                 0, getRandomGenerator(), getSpikeSystem());
}

bool Brain::load(const std::string& filepath) {
    return pImpl->checkpoint->load(filepath, pImpl->core->getRegions(),
                                  const_cast<size_t&>(getTotalSpikeCount()),
                                  0, const_cast<float&>(getLearnedPreference()),
                                  0, getRandomGenerator(), getSpikeSystem());
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->core->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->core->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->core->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->core->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->core->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->core->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                     float weight, Delay delay) {
    pImpl->core->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->core->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->core->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->core->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->stats->getActiveNeuronCount(pImpl->core->getRegions());
}

size_t Brain::getFiringNeuronCount() const {
    auto motorNeurons = pImpl->utils->getMotorNeurons(pImpl->core->getRegions());
    return pImpl->stats->getFiringNeuronCount(motorNeurons);
}

float Brain::getAverageFiringRate() const {
    return pImpl->stats->calculateAverageFiringRate(pImpl->core->getRegions());
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->memory->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->memory->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->memory->getAssociativeMemory();
}

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->prediction->getPredictionSystem();
}

NeuralPlanner* Brain::getPlanner() {
    return pImpl->cognition->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->cognition->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->cognition->getAttention();
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->development->getDevelopmentSystem();
}

Dopamine* Brain::getDopamine() {
    return pImpl->neuromodulation->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->neuromodulation->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->neuromodulation->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->neuromodulation->getPredictionErrorSignal();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->core->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->core->getRandomGenerator();
}

float Brain::getLearnedPreference() const {
    return 0.0f; // Placeholder
}

void Brain::setLearnedPreference(float preference) {
    // Placeholder
}

void Brain::logStatus() const {
    auto status = pImpl->stats->generateStatusLog(
        getTotalSpikeCount(), 0, getTotalNeuronCount(), getTotalSynapseCount(),
        getRegionCount(), 0, getFiringNeuronCount(), getDevelopmentalStage());
    std::cout << status << std::endl;
}

void Brain::declare_phase3_test_functions() {
    // Forward declaration for test functions
}