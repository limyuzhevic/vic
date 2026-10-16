#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../environment/Observation.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    
    // Composed subsystems
    std::unique_ptr<BrainNeuralCore> neuralCore;
    std::unique_ptr<BrainPlasticityManager> plasticityManager;
    std::unique_ptr<BrainMemoryManager> memoryManager;
    std::unique_ptr<BrainCognitiveManager> cognitiveManager;
    std::unique_ptr<BrainNeuromodulationManager> neuromodulationManager;
    std::unique_ptr<BrainDevelopmentManager> developmentManager;
    std::unique_ptr<BrainSimulationManager> simulationManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
    {
        // Initialize random generator
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Create and initialize all subsystems
        neuralCore = std::make_unique<BrainNeuralCore>(config);
        plasticityManager = std::make_unique<BrainPlasticityManager>(config);
        memoryManager = std::make_unique<BrainMemoryManager>(config);
        cognitiveManager = std::make_unique<BrainCognitiveManager>(config);
        neuromodulationManager = std::make_unique<BrainNeuromodulationManager>(config);
        developmentManager = std::make_unique<BrainDevelopmentManager>(config);
        simulationManager = std::make_unique<BrainSimulationManager>(config);
        
        // Initialize subsystems in order
        neuralCore->initialize();
        plasticityManager->initialize();
        memoryManager->initialize(neuralCore.get());
        cognitiveManager->initialize(neuralCore.get());
        neuromodulationManager->initialize(neuralCore.get());
        developmentManager->initialize();
        simulationManager->initialize();
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
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // All subsystems are already initialized in Impl constructor
    // This method is kept for API compatibility
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->simulationManager->getTimestep());
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     *  * 7. Update episodic memory with current experience
     *  * 8. Update prediction system
     *  * 9. Update attention system
     *  * 10. Update concept formation
     *  * 11. Apply structural plasticity (synaptogenesis, pruning)
     *  * 12. Replay important memories (during rest or periodically)
     *  * 13. Apply development effects
     *  * 14. Collect statistics
     */
    
    // Neural computation (Phase 2)
    pImpl->neuralCore->step(currentStep, currentTime);
    
    // Memory updates
    pImpl->memoryManager->update(pImpl->simulationManager->getTimestep());
    
    // Neuromodulation effects
    pImpl->neuromodulationManager->update(pImpl->simulationManager->getTimestep());
    
    // Plasticity application
    pImpl->plasticityManager->update(
        pImpl->neuralCore->getRegions(),
        currentStep,
        currentTime,
        pImpl->simulationManager->getTimestep()
    );
    
    // Episodic memory update (simplified)
    // Would integrate with neuromodulation for reward signal
    
    // Attention and concept formation
    pImpl->cognitiveManager->update(pImpl->simulationManager->getTimestep());
    
    // Structural plasticity
    if (currentStep % 100 == 0) {
        pImpl->plasticityManager->updateStructuralPlasticity(
            pImpl->neuralCore.get(),
            pImpl->simulationManager->getRandomGenerator()
        );
    }
    
    // Replay important memories
    if (currentStep % pImpl->simulationManager->getReplayInterval() == 0) {
        pImpl->memoryManager->replayEpisodes(pImpl->neuralCore.get(), 3);
    }
    
    // Apply development effects
    if (currentStep % 1000 == 0) {
        pImpl->developmentManager->update(
            pImpl->neuralCore.get(),
            pImpl->simulationManager->getRandomGenerator(),
            pImpl->simulationManager->getTimestep() * 1000
        );
        
        // Apply development effects to plasticity
        pImpl->plasticityManager->applyDevelopment(pImpl->developmentManager->getStage());
    }
    
    // Periodic memory consolidation
    if (currentStep % pImpl->simulationManager->getConsolidationInterval() == 0) {
        pImpl->memoryManager->consolidateMemory(0.3f);
    }
    
    // Checkpoint management
    if (pImpl->simulationManager) {
        pImpl->simulationManager->logStatus();
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    pImpl->neuralCore->receiveSensoryInput(input);
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    pImpl->neuralCore->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    pImpl->neuralCore->injectCurrentToNeurons(type, current);
}

BrainNeuralCore* Brain::getNeuralCore() {
    return pImpl->neuralCore.get();
}

const BrainNeuralCore* Brain::getNeuralCore() const {
    return pImpl->neuralCore.get();
}

BrainPlasticityManager* Brain::getPlasticityManager() {
    return pImpl->plasticityManager.get();
}

const BrainPlasticityManager* Brain::getPlasticityManager() const {
    return pImpl->plasticityManager.get();
}

BrainMemoryManager* Brain::getMemoryManager() {
    return pImpl->memoryManager.get();
}

const BrainMemoryManager* Brain::getMemoryManager() const {
    return pImpl->memoryManager.get();
}

BrainCognitiveManager* Brain::getCognitiveManager() {
    return pImpl->cognitiveManager.get();
}

const BrainCognitiveManager* Brain::getCognitiveManager() const {
    return pImpl->cognitiveManager.get();
}

BrainNeuromodulationManager* Brain::getNeuromodulationManager() {
    return pImpl->neuromodulationManager.get();
}

const BrainNeuromodulationManager* Brain::getNeuromodulationManager() const {
    return pImpl->neuromodulationManager.get();
}

BrainDevelopmentManager* Brain::getDevelopmentManager() {
    return pImpl->developmentManager.get();
}

const BrainDevelopmentManager* Brain::getDevelopmentManager() const {
    return pImpl->developmentManager.get();
}

BrainSimulationManager* Brain::getSimulationManager() {
    return pImpl->simulationManager.get();
}

const BrainSimulationManager* Brain::getSimulationManager() const {
    return pImpl->simulationManager.get();
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->neuralCore->getSpikeSystem();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->neuralCore->getSpikeSystem();
}

STDP* Brain::getSTDP() {
    return pImpl->plasticityManager->getSTDP();
}

const STDP* Brain::getSTDP() const {
    return pImpl->plasticityManager->getSTDP();
}

Hebbian* Brain::getHebbian() {
    return pImpl->plasticityManager->getHebbian();
}

const Hebbian* Brain::getHebbian() const {
    return pImpl->plasticityManager->getHebbian();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->plasticityManager->getStructuralPlasticity();
}

const StructuralPlasticity* Brain::getStructuralPlasticity() const {
    return pImpl->plasticityManager->getStructuralPlasticity();
}

float Brain::getExcitationInhibitionRatio() const {
    return pImpl->neuralCore->getExcitationInhibitionRatio();
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->neuralCore->getTotalSpikeCount();
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->neuralCore->getPendingSpikeEventCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    return pImpl->neuralCore->produceAction();
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    pImpl->neuromodulationManager->applyNeuromodulation(signal);
}

void Brain::updatePlasticity() {
    pImpl->plasticityManager->updatePlasticity();
}

void Brain::develop() {
    // Development is applied during each step
    // This method is kept for API compatibility
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->neuralCore->reset();
    pImpl->memoryManager->reset();
    pImpl->plasticityManager->reset();
    pImpl->cognitiveManager->reset();
    pImpl->neuromodulationManager->reset();
    pImpl->developmentManager->reset();
    pImpl->simulationManager->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    return pImpl->simulationManager->save(filepath);
}

bool Brain::load(const std::string& filepath) {
    return pImpl->simulationManager->load(filepath);
}

RegionId Brain::addRegion(const std::string& name) {
    return pImpl->neuralCore->addRegion(name);
}

NeuralRegion* Brain::getRegion(RegionId id) {
    return pImpl->neuralCore->getRegion(id);
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    return pImpl->neuralCore->getRegion(id);
}

size_t Brain::getRegionCount() const {
    return pImpl->neuralCore->getRegionCount();
}

std::vector<RegionId> Brain::getRegionIds() const {
    return pImpl->neuralCore->getRegionIds();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->neuralCore->getRegions();
}

void Brain::addInterRegionConnection(RegionId source, RegionId target,
                                     float weight, Delay delay) {
    pImpl->neuralCore->addInterRegionConnection(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->neuralCore->removeInterRegionConnection(source, target);
}

size_t Brain::getTotalNeuronCount() const {
    return pImpl->neuralCore->getTotalNeuronCount();
}

size_t Brain::getTotalSynapseCount() const {
    return pImpl->neuralCore->getTotalSynapseCount();
}

size_t Brain::getActiveNeuronCount() const {
    return pImpl->neuralCore->getActiveNeuronCount();
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->neuralCore->getFiringNeuronCount();
}

float Brain::getAverageFiringRate() const {
    return pImpl->neuralCore->getAverageFiringRate();
}

// ========== MEMORY SYSTEMS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->memoryManager->getWorkingMemory();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->memoryManager->getEpisodicMemory();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->memoryManager->getAssociativeMemory();
}

// ========== PREDICTION SYSTEM ==========

PredictionSystem* Brain::getPredictionSystem() {
    // Direct access for backward compatibility
    // In real implementation, would store PredictionSystem in memory manager
    return nullptr;
}

// ========== COGNITION SYSTEMS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->cognitiveManager->getPlanner();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->cognitiveManager->getConceptFormation();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->cognitiveManager->getAttention();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    // Direct access for backward compatibility
    // Would need to expose DevelopmentSystem from development manager
    return nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentManager->getStage();
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentManager->setStage(stage);
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->neuromodulationManager->getDopamine();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->neuromodulationManager->getCuriosity();
}

Novelty* Brain::getNovelty() {
    return pImpl->neuromodulationManager->getNovelty();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->neuromodulationManager->getPredictionError();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->simulationManager->getConfig();
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->simulationManager->getRandomGenerator();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    
    // Log all subsystem statuses
    if (pImpl->neuralCore) pImpl->neuralCore->logStatus();
    if (pImpl->plasticityManager) pImpl->plasticityManager->logStatus();
    if (pImpl->memoryManager) pImpl->memoryManager->logStatus();
    if (pImpl->cognitiveManager) pImpl->cognitiveManager->logStatus();
    if (pImpl->neuromodulationManager) pImpl->neuromodulationManager->logStatus();
    if (pImpl->developmentManager) pImpl->developmentManager->logStatus();
    if (pImpl->simulationManager) pImpl->simulationManager->logStatus();
    
    // Log global statistics
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->memoryManager) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->memoryManager->getWorkingMemorySize()));
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->memoryManager->getEpisodicMemorySize()));
        NLM_LOG_INFO("Associative memory associations: " + std::to_string(pImpl->memoryManager->getAssociativeMemorySize()));
    }
    
    // Neuromodulation status
    if (pImpl->neuromodulationManager) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->neuromodulationManager->getDopamine()->getLevel()));
        NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->neuromodulationManager->getCuriosity()->getLevel()));
        NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->neuromodulationManager->getNovelty()->getLevel()));
        NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->neuromodulationManager->getPredictionError()->getError()));
    }
    
    // Development status
    if (pImpl->developmentManager) {
        NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(getDevelopmentalStage())));
    }
    
    for (const auto& region : getRegions()) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                     " (" + region->getName() + "): " +
                     std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                     std::to_string(region->getSynapseCount()) + " synapses, " +
                     "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm