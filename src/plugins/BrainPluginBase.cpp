#include "BrainPluginInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>

namespace nlm {

BrainPluginBase::BrainPluginBase(const std::string& name, const std::string& version)
    : name(name), version(version), brainIntegrated(false), loadStatus(PluginLoadStatus::NotLoaded) {
    metadata.name = name;
    metadata.version = version;
    metadata.creationDate = std::chrono::system_clock::now();
    metadata.lastUpdated = metadata.creationDate;
    metadata.securityLevel = PluginSecurityLevel::Medium;
}

std::string BrainPluginBase::getName() const {
    return name;
}

std::string BrainPluginBase::getVersion() const {
    return version;
}

std::string BrainPluginBase::getDescription() const {
    return metadata.description;
}

std::string BrainPluginBase::getAuthor() const {
    return metadata.author;
}

const PluginMetadata& BrainPluginBase::getMetadata() const {
    return metadata;
}

std::shared_ptr<Brain> BrainPluginBase::getBrain() const {
    return brain;
}

bool BrainPluginBase::setBrain(std::shared_ptr<Brain> brain) {
    if (brainIntegrated) {
        NLM_LOG_WARNING("BrainPluginBase: Brain already integrated");
        return false;
    }
    
    this->brain = brain;
    brainIntegrated = true;
    
    NLM_LOG_INFO("BrainPluginBase: Brain integrated for plugin " + name);
    
    return true;
}

bool BrainPluginBase::integrateWithBrain(std::shared_ptr<Brain> brain) {
    return setBrain(brain);
}

bool BrainPluginBase::removeIntegrationWithBrain(std::shared_ptr<Brain> brain) {
    if (!brainIntegrated || this->brain != brain) {
        return false;
    }
    
    this->brain.reset();
    brainIntegrated = false;
    
    NLM_LOG_INFO("BrainPluginBase: Brain integration removed for plugin " + name);
    
    return true;
}

bool BrainPluginBase::isIntegratedWithBrain(std::shared_ptr<Brain> brain) const {
    return brainIntegrated && this->brain == brain;
}

bool BrainPluginBase::processSpike(const class Neuron& neuron, double time) {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::storeMemory(const std::string& key, const std::vector<float>& data) {
    memoryStore[key] = data;
    return true;
}

std::vector<float> BrainPluginBase::retrieveMemory(const std::string& key) {
    auto it = memoryStore.find(key);
    if (it != memoryStore.end()) {
        return it->second;
    }
    return std::vector<float>();
}

bool BrainPluginBase::updatePlasticity(const class Synapse& synapse, double time) {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::applyNeuromodulation(const class Neuromodulator& signal) {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::processSensoryInput(const class SensoryInput& input) {
    // Default implementation: do nothing
    return true;
}

std::unique_ptr<class Action> BrainPluginBase::processMotorOutput() {
    // Default implementation: return null action
    return std::make_unique<Action>(ActionType::Wait);
}

bool BrainPluginBase::updateDevelopment(const class DevelopmentalStage& stage) {
    // Default implementation: do nothing
    return true;
}

std::vector<float> BrainPluginBase::processCognition(const std::vector<float>& input) {
    // Default implementation: return input
    return input;
}

bool BrainPluginBase::initiateCognitiveProcess(const std::string& processName) {
    // Default implementation: do nothing
    return true;
}

std::map<std::string, std::vector<float>> BrainPluginBase::getCognitiveResults() {
    return cognitiveResults;
}

bool BrainPluginBase::learnFromExperience(const std::vector<float>& experience) {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::adaptToEnvironment(const std::map<std::string, float>& environmentState) {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::consolidateMemory() {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::prepareForBrainIntegration() {
    // Default implementation: nothing to prepare
    return true;
}

bool BrainPluginBase::finalizeBrainIntegration() {
    // Default implementation: nothing to finalize
    return true;
}

bool BrainPluginBase::cleanupBrainIntegration() {
    // Default implementation: cleanup integration
    if (brainIntegrated) {
        removeIntegrationWithBrain(brain);
    }
    return true;
}

bool BrainPluginBase::loadConfiguration(const std::string& filepath) {
    // TODO: Implement configuration loading
    return true;
}

bool BrainPluginBase::saveConfiguration(const std::string& filepath) const {
    // TODO: Implement configuration saving
    return true;
}

bool BrainPluginBase::resetConfiguration() {
    config.clear();
    return true;
}

void BrainPluginBase::addBrainEventCallback(BrainPluginEventCallback callback) {
    brainEventCallbacks.push_back(callback);
}

void BrainPluginBase::removeBrainEventCallback(BrainPluginEventCallback callback) {
    auto it = std::find(brainEventCallbacks.begin(), brainEventCallbacks.end(), callback);
    if (it != brainEventCallbacks.end()) {
        brainEventCallbacks.erase(it);
    }
}

void BrainPluginBase::broadcastBrainEvent(const BrainPluginEvent& event) {
    for (const auto& callback : brainEventCallbacks) {
        try {
            callback(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("BrainPluginBase: Exception in brain event callback: " + std::string(e.what()));
        }
    }
}

bool BrainPluginBase::initializeBrainIntegration() {
    // Default implementation: call prepare and finalize
    if (!prepareForBrainIntegration()) {
        NLM_LOG_ERROR("BrainPluginBase: Failed to prepare brain integration for plugin " + name);
        return false;
    }
    
    return finalizeBrainIntegration();
}

bool BrainPluginBase::shutdownBrainIntegration() {
    return cleanupBrainIntegration();
}

bool BrainPluginBase::validateBrainConfiguration() {
    // Default implementation: validate configuration
    return true;
}

bool BrainPluginBase::updateBrainParameters(const std::map<std::string, float>& parameters) {
    // Default implementation: update parameters
    return true;
}

std::map<std::string, float> BrainPluginBase::getBrainParameters() const {
    return std::map<std::string, float>();
}

bool BrainPluginBase::onBrainEvent(const BrainPluginEvent& event) {
    // Default implementation: broadcast event
    broadcastBrainEvent(event);
    return true;
}

bool BrainPluginBase::onBrainStep(SimulationStep step, Timestamp time) {
    // Default implementation: do nothing
    return true;
}

bool BrainPluginBase::onBrainReset() {
    // Default implementation: reset state
    memoryStore.clear();
    cognitiveResults.clear();
    return true;
}

} // namespace nlm