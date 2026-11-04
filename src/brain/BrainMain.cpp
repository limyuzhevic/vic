#include "BrainMain.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

Brain::Brain(std::shared_ptr<Config> config)
    : config(config)
    , core(nullptr)
    , integration(nullptr)
    , checkpoint(nullptr)
    , development(nullptr)
    , configManager(nullptr) {
}

Brain::~Brain() = default;

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Initialize sub-components
    core = std::make_unique<BrainCore>(config);
    integration = std::make_unique<BrainIntegration>();
    checkpoint = std::make_unique<BrainCheckpoint>();
    development = std::make_unique<BrainDevelopment>();
    configManager = std::make_unique<BrainConfig>();
    
    // Initialize integration
    if (!integration->initialize(core.get())) {
        NLM_LOG_ERROR("Failed to initialize integration systems");
        return false;
    }
    
    if (!checkpoint->initialize(core.get())) {
        NLM_LOG_ERROR("Failed to initialize checkpoint system");
        return false;
    }
    
    if (!development->initialize(core.get())) {
        NLM_LOG_ERROR("Failed to initialize development system");
        return false;
    }
    
    if (!configManager->initialize(core.get())) {
        NLM_LOG_ERROR("Failed to initialize configuration system");
        return false;
    }
    
    // Load configuration
    if (!configManager->loadFromFile("configs/default.cfg")) {
        NLM_LOG_INFO("Using default configuration");
    }
    
    // Initialize core brain functionality
    if (!core->initialize()) {
        NLM_LOG_ERROR("Failed to initialize core brain functionality");
        return false;
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    return true;
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    if (core) {
        core->reset();
    }
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * 0.001);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!core || !integration) return;
    
    // Update core neural computation
    core->step(currentStep, currentTime);
    
    // Update integrated systems
    integration->step(core.get(), 0.001);
    
    // Update development system
    development->update(core.get(), 1.0);  // 1 second elapsed
    
    // Update checkpoint system
    checkpoint->update(currentStep, currentTime);
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    if (!core) return;
    
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = core->getSensoryNeurons().size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        core->getSensoryNeurons()[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (integration->getWorkingMemory() && normalizedValue > 0.5f) {
            integration->getWorkingMemory()->storeToNeuron(
                core->getSensoryNeurons()[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

std::unique_ptr<class Action> Brain::produceAction() {
    if (!core) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (core->getMotorNeurons().empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : core->getMotorNeurons()) {
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

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (!core) return;
    core->injectCurrent(neuron, current);
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (!core) return;
    // Note: BrainCore doesn't have this method, would need to add it
}

SpikeSystem* Brain::getSpikeSystem() {
    return core ? core->getSpikeSystem() : nullptr;
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return core ? core->getSpikeSystem() : nullptr;
}

STDP* Brain::getSTDP() {
    return core ? core->getSTDP() : nullptr;
}

Hebbian* Brain::getHebbian() {
    return core ? core->getHebbian() : nullptr;
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return core ? core->getStructuralPlasticity() : nullptr;
}

float Brain::getExcitationInhibitionRatio() const {
    return core ? core->getExcitationInhibitionRatio() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return core ? core->getTotalSpikeCount() : 0;
}

size_t Brain::getPendingSpikeEventCount() const {
    return core ? core->getSpikeSystem() ? 
        core->getSpikeSystem()->getPendingSpikeCount() + 
        core->getSpikeSystem()->getPendingDelayedCount() : 0 : 0;
}

size_t Brain::getTotalNeuronCount() const {
    return core ? core->getTotalNeuronCount() : 0;
}

size_t Brain::getTotalSynapseCount() const {
    return core ? core->getTotalSynapseCount() : 0;
}

size_t Brain::getActiveNeuronCount() const {
    return core ? core->getActiveNeuronCount() : 0;
}

size_t Brain::getFiringNeuronCount() const {
    return core ? core->getSpikesThisStep() : 0;
}

float Brain::getAverageFiringRate() const {
    return core ? core->getAverageFiringRate() : 0.0f;
}

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return integration ? integration->getWorkingMemory() : nullptr;
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return integration ? integration->getEpisodicMemory() : nullptr;
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return integration ? integration->getAssociativeMemory() : nullptr;
}

PredictionSystem* Brain::getPredictionSystem() {
    return integration ? integration->getPredictionSystem() : nullptr;
}

NeuralPlanner* Brain::getPlanner() {
    return integration ? integration->getPlanner() : nullptr;
}

ConceptFormation* Brain::getConceptFormation() {
    return integration ? integration->getConceptFormation() : nullptr;
}

AttentionalSelection* Brain::getAttention() {
    return integration ? integration->getAttention() : nullptr;
}

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return development ? development->getDevelopmentSystem() : nullptr;
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return core ? core->getDevelopmentalStage() : DevelopmentalStage::Initial;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    if (core) {
        core->setDevelopmentalStage(stage);
    }
}

Dopamine* Brain::getDopamine() {
    return integration ? integration->getDopamine() : nullptr;
}

Curiosity* Brain::getCuriosity() {
    return integration ? integration->getCuriosity() : nullptr;
}

Novelty* Brain::getNovelty() {
    return integration ? integration->getNovelty() : nullptr;
}

PredictionError* Brain::getPredictionErrorSignal() {
    return integration ? integration->getPredictionErrorSignal() : nullptr;
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return core ? core->getRandomGenerator() : nullptr;
}

bool Brain::save(const std::string& filepath) {
    return checkpoint ? checkpoint->save(filepath) : false;
}

bool Brain::load(const std::string& filepath) {
    return checkpoint ? checkpoint->load(filepath) : false;
}

float Brain::getLearnedPreference() const {
    return core ? core->getLearnedPreference() : 0.0f;
}

void Brain::setLearnedPreference(float preference) {
    if (core) {
        core->setLearnedPreference(preference);
    }
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    
    if (core) {
        NLM_LOG_INFO("Core brain: " + std::to_string(core->getTotalNeuronCount()) + " neurons");
        NLM_LOG_INFO("Total spikes: " + std::to_string(core->getTotalSpikeCount()));
        NLM_LOG_INFO("Spike system active");
    }
    
    if (integration) {
        if (integration->getWorkingMemory()) {
            NLM_LOG_INFO("Working memory: active traces");
        }
        if (integration->getEpisodicMemory()) {
            NLM_LOG_INFO("Episodic memory: " + std::to_string(
                integration->getEpisodicMemory()->getEpisodeCount()) + " episodes");
        }
    }
    
    if (development) {
        NLM_LOG_INFO("Development stage: " + std::to_string(
            static_cast<int>(development->getDevelopmentalStage())));
    }
}

} // namespace nlm
