#include "Brain.hpp"
#include "BrainCore.hpp"
#include <memory>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<BrainCore> brainCore;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , brainCore(std::make_unique<BrainCore>(cfg)) {}
    
    ~Impl() = default;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

/**
 * @brief Destroy the Brain object
 * 
 * Cleans up the brain system and all its internal resources. This includes
 * destroying the BrainCore instance and releasing any allocated memory.
 */
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
    /**
     * @brief Initializes the brain with configuration settings
     * 
     * This method initializes the brain core with the given configuration.
     * It delegates the actual initialization to BrainCore and performs
     * additional logging and statistics collection after successful initialization.
     * 
     * @return bool true if initialization succeeds, false otherwise
     */
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Delegate initialization to BrainCore
    bool result = pImpl->brainCore->initialize();
    
    if (result) {
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->brainCore->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->brainCore->getTotalSynapseCount()));
    }
    
    return result;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->brainCore->getTimestep());
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /**
     * @brief Executes one simulation step with explicit time
     * 
     * Advances the neural simulation by one complete time step, including all neural
     * computations, plasticity updates, memory updates, and neuromodulation. This
     * is the core method that drives the entire brain simulation.
     * 
     * @param currentStep The current simulation step number (discrete time)
     * @param currentTime The absolute timestamp for this simulation step
     */
    // Delegate step execution to BrainCore
    pImpl->brainCore->step(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Delegate to BrainCore for implementation
    if (pImpl->brainCore) {
        pImpl->brainCore->receiveSensoryInput(input);
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    // Delegate to BrainCore for implementation
    if (pImpl->brainCore) {
        pImpl->brainCore->injectCurrent(neuron, current);
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    // Delegate to BrainCore for implementation
    if (pImpl->brainCore) {
        pImpl->brainCore->injectCurrentToNeurons(type, current);
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getSpikeSystem();
    }
    return nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getSpikeSystem();
    }
    return nullptr;
}

STDP* Brain::getSTDP() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getSTDP();
    }
    return nullptr;
}

Hebbian* Brain::getHebbian() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getHebbian();
    }
    return nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getStructuralPlasticity();
    }
    return nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getExcitationInhibitionRatio();
    }
    return 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getTotalSpikeCount();
    }
    return 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getPendingSpikeEventCount();
    }
    return 0;
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    if (pImpl->brainCore) {
        return pImpl->brainCore->produceAction();
    }
    return std::make_unique<Action>(ActionType::Wait);
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    if (pImpl->brainCore) {
        pImpl->brainCore->applyNeuromodulation(signal);
    }
}

void Brain::updatePlasticity() {
    if (pImpl->brainCore) {
        pImpl->brainCore->updatePlasticity();
    }
}

void Brain::develop() {
    if (pImpl->brainCore) {
        pImpl->brainCore->develop();
    }
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    if (pImpl->brainCore) {
        pImpl->brainCore->reset();
    }
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    if (pImpl->brainCore) {
        return pImpl->brainCore->save(filepath);
    }
    
    NLM_LOG_ERROR("Failed to save brain state: BrainCore not initialized");
    return false;
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    if (pImpl->brainCore) {
        return pImpl->brainCore->load(filepath);
    }
    
    NLM_LOG_ERROR("Failed to load brain state: BrainCore not initialized");
    return false;
}

void Brain::setBrainCore(BrainCore* core) {
    pImpl->brainCore.reset(core);
}

RegionId Brain::addRegion(const std::string& name) {
    if (pImpl->brainCore) {
        return pImpl->brainCore->addRegion(name);
    }
    return RegionId();
}

NeuralRegion* Brain::getRegion(RegionId id) {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getRegion(id);
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getRegion(id);
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getRegionCount();
    }
    return 0;
}

std::vector<RegionId> Brain::getRegionIds() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getRegionIds();
    }
    return std::vector<RegionId>();
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getRegions();
    }
    static const std::vector<std::unique_ptr<NeuralRegion>> empty;
    return empty;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    if (pImpl->brainCore) {
        pImpl->brainCore->addInterRegionConnection(source, target, weight, delay);
    }
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    if (pImpl->brainCore) {
        pImpl->brainCore->removeInterRegionConnection(source, target);
    }
}

size_t Brain::getTotalNeuronCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getTotalNeuronCount();
    }
    return 0;
}

size_t Brain::getTotalSynapseCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getTotalSynapseCount();
    }
    return 0;
}

size_t Brain::getActiveNeuronCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getActiveNeuronCount();
    }
    return 0;
}

size_t Brain::getFiringNeuronCount() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getFiringNeuronCount();
    }
    return 0;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getAverageFiringRate();
    }
    return 0.0f;
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getWorkingMemory();
    }
    return nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getEpisodicMemory();
    }
    return nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getAssociativeMemory();
    }
    return nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getPredictionSystem();
    }
    return nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getPlanner();
    }
    return nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getConceptFormation();
    }
    return nullptr;
}

AttentionalSelection* Brain::getAttention() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getAttention();
    }
    return nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getDevelopmentSystem();
    }
    return nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getDevelopmentalStage();
    }
    return DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (pImpl->brainCore) {
        pImpl->brainCore->setDevelopmentalStage(stage);
    }
}

Dopamine* Brain::getDopamine() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getDopamine();
    }
    return nullptr;
}

Curiosity* Brain::getCuriosity() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getCuriosity();
    }
    return nullptr;
}

Novelty* Brain::getNovelty() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getNovelty();
    }
    return nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getPredictionErrorSignal();
    }
    return nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getConfig();
    }
    return nullptr;
}

RandomGenerator* Brain::getRandomGenerator() {
    if (pImpl->brainCore) {
        return pImpl->brainCore->getRandomGenerator();
    }
    return nullptr;
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    
    if (pImpl->brainCore) {
        NLM_LOG_INFO("Regions: " + std::to_string(pImpl->brainCore->getRegionCount()));
        NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->brainCore->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->brainCore->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(pImpl->brainCore->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(pImpl->brainCore->getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(pImpl->brainCore->getTotalSpikeCount()));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(pImpl->brainCore->getPendingSpikeEventCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(pImpl->brainCore->getAverageFiringRate()));
        NLM_LOG_INFO("E/I ratio: " + std::to_string(pImpl->brainCore->getExcitationInhibitionRatio()));
        
        // Additional BrainCore-specific status
        NLM_LOG_INFO("BrainCore initialized: Yes");
        NLM_LOG_INFO("Current step: " + std::to_string(pImpl->brainCore->getCurrentStep()));
        NLM_LOG_INFO("Current time: " + std::to_string(pImpl->brainCore->getCurrentTime()));
        NLM_LOG_INFO("Timestep: " + std::to_string(pImpl->brainCore->getTimestep()));
        
        if (pImpl->brainCore->isResting()) {
            NLM_LOG_INFO("Brain state: Resting");
        } else {
            NLM_LOG_INFO("Brain state: Active");
        }
        
        NLM_LOG_INFO("Steps since last episode: " + std::to_string(pImpl->brainCore->getStepsSinceLastEpisode()));
        NLM_LOG_INFO("Developmental age: " + std::to_string(pImpl->brainCore->getDevelopmentalAge()));
        
        for (const auto& region : pImpl->brainCore->getRegions()) {
            NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                        " (" + region->getName() + "): " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                        std::to_string(region->getSynapseCount()) + " synapses, " +
                        "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
        }
    }
}

} // namespace nlm
