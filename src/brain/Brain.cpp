// Brain.cpp - Main Brain orchestrator using modular architecture

#include "Brain.hpp"
#include "BrainInitializer.hpp"
#include "BrainStep.hpp"
#include "BrainIntegration.hpp"
#include "BrainState.hpp"
#include "BrainMetrics.hpp"
#include "BrainUtils.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ========== BRAIN CONSTRUCTOR / DESTRUCTOR ==========

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

// ========== INITIALIZATION ==========

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Modular Integrated Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions using modular utilities
    BrainUtils::createRegions(pImpl.get(), regionCount);
    
    // Setup neurons across regions using modular utilities
    BrainUtils::setupNeurons(pImpl.get(), neuronCount, regionCount);
    
    // Setup connectivity using modular utilities
    BrainUtils::setupConnectivity(pImpl.get(), regionCount, connectionProbability);
    
    // Collect sensory and motor neurons for I/O using modular utilities
    BrainUtils::collectSensoryAndMotorNeurons(pImpl.get(), neuronCount / regionCount, regionCount);
    
    // Initialize all integrated systems using modular initialization
    if (!BrainInitializer::initializeBrainSystems(pImpl.get(), this, neuronCount, 
                                                 regionCount, connectionProbability, pImpl->currentTime)) {
        NLM_LOG_ERROR("Failed to initialize brain systems");
        return false;
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Modular)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(BrainMetrics::getTotalNeuronCount(pImpl.get())));
    NLM_LOG_INFO("Total synapses: " + std::to_string(BrainMetrics::getTotalSynapseCount(pImpl.get())));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

// ========== SIMULATION STEP ==========

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Execute the complete integrated brain loop using modular step execution
    BrainStep::executeStep(pImpl.get(), currentStep, currentTime);
}

// ========== SENSORY INPUT ==========

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    BrainIntegration::integrateSensoryInput(pImpl.get(), input);
}

// ========== MOTOR OUTPUT ==========

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    BrainIntegration::injectCurrentToNeuron(pImpl.get(), neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    BrainIntegration::injectCurrentToNeurons(pImpl.get(), type, current);
}

std::unique_ptr<class Action> Brain::produceAction() {
    return BrainIntegration::produceAction(pImpl.get());
}

// ========== NEUROMODULATION ==========

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    BrainIntegration::applyNeuromodulation(pImpl.get(), signal);
}

void Brain::updatePlasticity() {
    BrainIntegration::updatePlasticity(pImpl.get());
}

void Brain::develop() {
    BrainIntegration::develop(pImpl.get());
}

// ========== STATE MANAGEMENT ==========

void Brain::reset() {
    BrainState::resetBrain(pImpl.get());
}

bool Brain::save(const std::string& filepath) const {
    return BrainState::saveBrainState(pImpl.get(), filepath);
}

bool Brain::load(const std::string& filepath) {
    return BrainState::loadBrainState(pImpl.get(), filepath);
}

// ========== REGION MANAGEMENT ==========

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

// ========== STATISTICS ==========

size_t Brain::getTotalNeuronCount() const {
    return BrainMetrics::getTotalNeuronCount(pImpl.get());
}

size_t Brain::getTotalSynapseCount() const {
    return BrainMetrics::getTotalSynapseCount(pImpl.get());
}

size_t Brain::getActiveNeuronCount() const {
    return BrainMetrics::getActiveNeuronCount(pImpl.get());
}

size_t Brain::getFiringNeuronCount() const {
    return BrainMetrics::getFiringNeuronCount(pImpl.get());
}

float Brain::getAverageFiringRate() const {
    return BrainMetrics::getAverageFiringRate(pImpl.get());
}

float Brain::getExcitationInhibitionRatio() const {
    return BrainMetrics::getExcitationInhibitionRatio(pImpl.get());
}

size_t Brain::getTotalSpikeCount() const {
    return BrainMetrics::getTotalSpikeCount(pImpl.get());
}

size_t Brain::getPendingSpikeEventCount() const {
    return BrainMetrics::getPendingSpikeEventCount(pImpl.get());
}

DevelopmentStage Brain::getDevelopmentalStage() const {
    return BrainMetrics::getDevelopmentalStage(pImpl.get());
}

void Brain::setDevelopmentalStage(DevelopmentStage stage) {
    pImpl->developmentalStage = stage;
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

// ========== CONFIGURATION ==========

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

// ========== LOGGING ==========

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Modular) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm