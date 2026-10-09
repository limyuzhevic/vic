// brain/Brain.cpp - Core Brain class implementation
// This is the main interface class that users interact with

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

// Forward declaration for the implementation class
class Brain::Impl;

// Static factory method implementation with documentation
/**
 * @brief Create a new Brain instance with the specified configuration
 * 
 * This static factory method creates a Brain instance with the given configuration.
 * It provides better error handling and logging than the constructor.
 * 
 * @param config Configuration to use for the brain. If nullptr, a default
 *              configuration will be created.
 * @return Unique pointer to the newly created Brain instance
 * 
 * @throws std::invalid_argument if config is invalid
 * @throws std::runtime_error if brain creation fails
 * 
 * @note This method is preferred over the constructor for better error handling
 *       and logging. The configuration is copied internally and can be modified
 *       after brain creation without affecting the brain's operation.
 * 
 * @param config Configuration for the brain
 * @return Pointer to new Brain instance
 * 
 * @see nlm::Config::createDefault() for creating default configurations
 * @see nlm::Brain::initialize() for initialization requirements
 */
std::unique_ptr<Brain> Brain::create(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_WARNING("Null configuration provided to Brain::create, using default");
        config = Config::createDefault();
    }
    
    if (!config) {
        NLM_LOG_ERROR("Failed to create default configuration");
        throw std::runtime_error("Brain creation failed: unable to create configuration");
    }
    
    try {
        NLM_LOG_INFO("Creating Brain with configuration: " + config->summary());
        return std::make_unique<Brain>(config);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain creation failed: ") + e.what());
        throw;
    }
}

/**
 * @brief Create a new Brain instance with default configuration
 * 
 * This static factory method provides a convenient way to create Brain instances
 * without having to manually create configuration objects. It creates a default
 * configuration and uses it to construct the brain.
 * 
 * @return Unique pointer to the newly created Brain instance
 * 
 * @note This is the recommended way to create Brain instances in user code:
 * ```cpp
 * #include "brain/Brain.hpp"
 * 
 * int main() {
 *     // Create brain with default configuration
 *     auto brain = nlm::Brain::createDefault();
 *     
 *     // Initialize brain
 *     brain->initialize();
 *     
 *     // Run simulation
 *     for (int step = 0; step < 100; ++step) {
 *         brain->step(step);
 *     }
 *     
 *     return 0;
 * }
 * ```
 * 
 * @see nlm::Brain::create(std::shared_ptr<Config> config)
 *      for custom configuration
 */
std::unique_ptr<Brain> Brain::createDefault() {
    auto config = Config::createDefault();
    return Brain::create(config);
}

// Public Brain class implementation
Brain::Brain(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    return pImpl->initialize();
}

void Brain::step(SimulationStep currentStep) {
    pImpl->step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->injectCurrentToNeurons(type, current);
}

std::shared_ptr<SpikeSystem> Brain::getSpikeSystem() {
    return pImpl->spikeSystem.)
}

std::shared_ptr<const SpikeSystem> Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.)
}

std::shared_ptr<STDP> Brain::getSTDP() {
    return pImpl->stdp.)
}

std::shared_ptr<Hebbian> Brain::getHebbian() {
    return pImpl->hebbian.)
}

std::shared_ptr<StructuralPlasticity> Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.)
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + 
           pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    pImpl->updatePlasticity();
}

void Brain::develop() {
    pImpl->develop();
}

void Brain::reset() {
    pImpl->reset();
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, 
                                     float weight, Delay delay) {
    pImpl->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    return pImpl->getAverageFiringRate();
}

std::shared_ptr<NeuralWorkingMemory> Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

std::shared_ptr<NeuralEpisodicMemory> Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

std::shared_ptr<NeuralAssociativeMemory> Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

std::shared_ptr<PredictionSystem> Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

std::shared_ptr<NeuralPlanner> Brain::getPlanner() {
    return pImpl->planner.get();
}

std::shared_ptr<ConceptFormation> Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

std::shared_ptr<AttentionalSelection> Brain::getAttention() {
    return pImpl->attention.get();
}

std::shared_ptr<DevelopmentSystem> Brain::getDevelopmentSystem() {
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
    pImpl->logStatus();
}

} // namespace nlm