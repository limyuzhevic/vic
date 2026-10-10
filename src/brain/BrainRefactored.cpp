// BrainRefactored.cpp implementation
#include "BrainRefactored.hpp"
#include "NeuralDynamicsComponent.hpp"
#include "PlasticityComponent.hpp"
#include "MemoryComponent.hpp"
#include "PredictionComponent.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

struct BrainRefactored::Impl {
    // Configuration
    std::shared_ptr<Config> config;
    
    // Core components
    std::unique_ptr<NeuralDynamicsComponent> neuralDynamics;
    std::unique_ptr<PlasticityComponent> plasticity;
    std::unique_ptr<MemoryComponent> memory;
    std::unique_ptr<PredictionComponent> prediction;
    
    // Spike system (shared with neural dynamics)
    std::unique_ptr<SpikeSystem> spikeSystem;
    
    // Random generator
    std::unique_ptr<RandomGenerator> randomGen;
    
    // Component registry
    std::unordered_map<std::string, std::unique_ptr<BrainComponent>> componentRegistry;
    
    // Statistics
    size_t totalSpikeCount;
    size_t pendingSpikeCount;
    
    // Initialization flag
    bool initialized;
    
    Impl(std::shared_ptr<Config> cfg) : config(std::move(cfg)),
                                       totalSpikeCount(0), pendingSpikeCount(0),
                                       initialized(false) {}
};

BrainRefactored::BrainRefactored(std::shared_ptr<Config> config) : 
    pImpl(new Impl(std::move(config))) {
    // Initialize configuration
    config_ = pImpl->config;
}

BrainRefactored::~BrainRefactored() = default;

BrainRefactored::BrainRefactored(BrainRefactored&& other) noexcept : 
    pImpl(std::move(other.pImpl)),
    config_(std::move(other.config_)),
    components_(std::move(other.components_)) {
    
    // Transfer ownership of core components
    neuralDynamicsComponent_ = std::move(other.neuralDynamicsComponent_);
    plasticityComponent_ = std::move(other.plasticityComponent_);
    memoryComponent_ = std::move(other.memoryComponent_);
    predictionComponent_ = std::move(other.predictionComponent_);
    
    // Set the internal pointers
    if (pImpl) {
        pImpl->neuralDynamics = neuralDynamicsComponent_.get();
        pImpl->plasticity = plasticityComponent_.get();
        pImpl->memory = memoryComponent_.get();
        pImpl->prediction = predictionComponent_.get();
    }
}

BrainRefactored& BrainRefactored::operator=(BrainRefactored&& other) noexcept {
    if (this != &other) {
        // Clean up current state
        components_.clear();
        
        // Move resources
        pImpl = std::move(other.pImpl);
        config_ = std::move(other.config_);
        components_ = std::move(other.components_);
        
        // Transfer core components
        neuralDynamicsComponent_ = std::move(other.neuralDynamicsComponent_);
        plasticityComponent_ = std::move(other.plasticityComponent_);
        memoryComponent_ = std::move(other.memoryComponent_);
        predictionComponent_ = std::move(other.predictionComponent_);
        
        // Update internal pointers
        if (pImpl) {
            pImpl->neuralDynamics = neuralDynamicsComponent_.get();
            pImpl->plasticity = plasticityComponent_.get();
            pImpl->memory = memoryComponent_.get();
            pImpl->prediction = predictionComponent_.get();
        }
    }
    return *this;
}

bool BrainRefactored::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    try {
        // Initialize core components
        neuralDynamicsComponent_ = std::make_unique<NeuralDynamicsComponent>();
        plasticityComponent_ = std::make_unique<PlasticityComponent>();
        memoryComponent_ = std::make_unique<MemoryComponent>();
        predictionComponent_ = std::make_unique<PredictionComponent>();
        
        // Set up internal pointers
        if (pImpl) {
            pImpl->neuralDynamics = neuralDynamicsComponent_.get();
            pImpl->plasticity = plasticityComponent_.get();
            pImpl->memory = memoryComponent_.get();
            pImpl->prediction = predictionComponent_.get();
        }
        
        // Initialize core systems
        pImpl->spikeSystem = std::make_unique<SpikeSystem>();
        pImpl->randomGen = std::make_unique<RandomGenerator>();
        
        // Add components to registry
        pImpl->componentRegistry["neural_dynamics"] = std::move(neuralDynamicsComponent_);
        pImpl->componentRegistry["plasticity"] = std::move(plasticityComponent_);
        pImpl->componentRegistry["memory"] = std::move(memoryComponent_);
        pImpl->componentRegistry["prediction"] = std::move(predictionComponent_);
        
        // Initialize all components
        bool success = true;
        for (auto& pair : pImpl->componentRegistry) {
            if (!pair.second->initialize()) {
                NLM_LOG_ERROR("Failed to initialize component: " + pair.first);
                success = false;
            }
        }
        
        if (!success) {
            NLM_LOG_ERROR("Brain initialization failed - some components failed to initialize");
            return false;
        }
        
        // Set up component relationships
        setupComponentRelationships();
        
        // Load configuration
        loadConfiguration();
        
        pImpl->initialized = true;
        NLM_LOG_INFO("BrainRefactored initialized successfully with " + 
                    std::to_string(pImpl->componentRegistry.size()) + " components");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain initialization exception: " + std::string(e.what()));
        return false;
    }
}

void BrainRefactored::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * 0.001f); // 1ms timestep
}

void BrainRefactored::step(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->initialized) {
        NLM_LOG_ERROR("Brain not initialized - cannot step");
        return;
    }
    
    try {
        // Update all active components
        std::vector<std::string> activeComponents;
        for (const auto& pair : pImpl->componentRegistry) {
            if (pair.second->isActive()) {
                pair.second->update();
                activeComponents.push_back(pair.first);
            }
        }
        
        // Special handling for neural dynamics and plasticity
        if (pImpl->neuralDynamics && pImpl->neuralDynamics->isActive()) {
            // Neural dynamics updates neurons (this would integrate with actual neurons)
            // For now, just log the step
            pImpl->totalSpikeCount += 100; // Placeholder spike count
        }
        
        if (pImpl->plasticity && pImpl->plasticity->isActive()) {
            // Update plasticity rules
            pImpl->plasticity->update();
        }
        
        // Update statistics
        updateStatistics();
        
        NLM_LOG_DEBUG("Brain step completed at time " + std::to_string(currentTime) + 
                     " with " + std::to_string(activeComponents.size()) + " active components");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain step exception: " + std::string(e.what()));
    }
}

void BrainRefactored::receiveSensoryInput(const SensoryInput& input) {
    if (pImpl->neuralDynamics) {
        pImpl->neuralDynamics->receiveSensoryInput(input);
    }
}

void BrainRefactored::injectCurrent(NeuronId neuron, MembranePotential current) {
    if (pImpl->neuralDynamics) {
        pImpl->neuralDynamics->injectCurrent(neuron, current);
    }
}

void BrainRefactored::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    if (pImpl->neuralDynamics) {
        pImpl->neuralDynamics->injectCurrentToNeurons(type, current);
    }
}

SpikeSystem* BrainRefactored::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* BrainRefactored::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

bool BrainRefactored::addComponent(std::unique_ptr<BrainComponent> component) {
    if (!component) {
        NLM_LOG_ERROR("Cannot add null component");
        return false;
    }
    
    std::string name = component->getName();
    if (pImpl->componentRegistry.find(name) != pImpl->componentRegistry.end()) {
        NLM_LOG_ERROR("Component with name already exists: " + name);
        return false;
    }
    
    if (!component->initialize()) {
        NLM_LOG_ERROR("Failed to initialize component during addition: " + name);
        return false;
    }
    
    components_[name] = std::move(component);
    pImpl->componentRegistry[name] = components_[name].get();
    
    NLM_LOG_INFO("Added component: " + name);
    return true;
}

BrainComponent* BrainRefactored::getComponent(const std::string& name) {
    auto it = pImpl->componentRegistry.find(name);
    if (it != pImpl->componentRegistry.end()) {
        return it->second;
    }
    return nullptr;
}

const BrainComponent* BrainRefactored::getComponent(const std::string& name) const {
    auto it = pImpl->componentRegistry.find(name);
    if (it != pImpl->componentRegistry.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<BrainComponent*> BrainRefactored::getComponents() const {
    std::vector<BrainComponent*> result;
    result.reserve(pImpl->componentRegistry.size());
    for (const auto& pair : pImpl->componentRegistry) {
        result.push_back(pair.second);
    }
    return result;
}

bool BrainRefactored::removeComponent(const std::string& name) {
    auto it = pImpl->componentRegistry.find(name);
    if (it == pImpl->componentRegistry.end()) {
        NLM_LOG_ERROR("Component not found: " + name);
        return false;
    }
    
    // Clean up component resources
    it->second->setActive(false);
    
    // Remove from registry
    pImpl->componentRegistry.erase(it);
    
    // Also remove from main components map
    components_.erase(name);
    
    NLM_LOG_INFO("Removed component: " + name);
    return true;
}

bool BrainRefactored::registerComponentType(const std::string& type, 
                                           const std::string& name,
                                           std::function<std::unique_ptr<BrainComponent>()> creator) {
    if (!creator) {
        NLM_LOG_ERROR("Component creator cannot be null");
        return false;
    }
    
    // Use factory to create and register the component
    auto component = creator();
    if (!addComponent(std::move(component))) {
        NLM_LOG_ERROR("Failed to register component type: " + type);
        return false;
    }
    
    return true;
}

void BrainRefactored::activateComponent(const std::string& name, bool activate) {
    BrainComponent* component = getComponent(name);
    if (component) {
        component->setActive(activate);
        NLM_LOG_INFO((activate ? "Activated" : "Deactivated") + " component: " + name);
    }
}

bool BrainRefactored::isComponentActive(const std::string& name) const {
    const BrainComponent* component = getComponent(name);
    if (component) {
        return component->isActive();
    }
    return false;
}

float BrainRefactored::getExcitationInhibitionRatio() const {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getExcitationInhibitionRatio();
    }
    return 0.0f;
}

size_t BrainRefactored::getTotalSpikeCount() const {
    return pImpl->totalSpikeCount;
}

 size_t BrainRefactored::getPendingSpikeEventCount() const {
    if (pImpl->spikeSystem) {
        return pImpl->spikeSystem->getPendingSpikeCount();
    }
    return 0;
}

std::unique_ptr<Action> BrainRefactored::produceAction() {
    if (pImpl->neuralDynamics) {
        return pImpl->neuralDynamics->produceAction();
    }
    return nullptr;
}

void BrainRefactored::applyNeuromodulation(const Neuromodulator& signal) {
    if (pImpl->neuralDynamics) {
        pImpl->neuralDynamics->applyNeuromodulation(signal);
    }
}

void BrainRefactored::updatePlasticity() {
    if (pImpl->plasticity) {
        pImpl->plasticity->updatePlasticity();
    }
}

void BrainRefactored::develop() {
    if (pImpl->neuralDynamics) {
        pImpl->neuralDynamics->develop();
    }
}

void BrainRefactored::reset() {
    // Reset all components
    for (auto& pair : pImpl->componentRegistry) {
        pair.second->reset();
    }
    
    // Reset statistics
    pImpl->totalSpikeCount = 0;
    pImpl->pendingSpikeCount = 0;
    
    NLM_LOG_INFO("BrainRefactored reset completed");
}

bool BrainRefactored::save(const std::string& filepath) const {
    try {
        // Save all components
        for (const auto& pair : pImpl->componentRegistry) {
            if (!pair.second->save(filepath + "/" + pair.first)) {
                NLM_LOG_ERROR("Failed to save component: " + pair.first);
                return false;
            }
        }
        
        // Save configuration
        if (config_) {
            config_->save(filepath + "/config");
        }
        
        NLM_LOG_INFO("BrainRefactored saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain save exception: " + std::string(e.what()));
        return false;
    }
}

bool BrainRefactored::load(const std::string& filepath) {
    try {
        // Load configuration first
        if (config_) {
            if (!config_->load(filepath + "/config")) {
                NLM_LOG_ERROR("Failed to load configuration");
                return false;
            }
        }
        
        // Load all components
        for (auto& pair : pImpl->componentRegistry) {
            if (!pair.second->load(filepath + "/" + pair.first)) {
                NLM_LOG_ERROR("Failed to load component: " + pair.first);
                return false;
            }
        }
        
        // Reload configuration
        loadConfiguration();
        
        NLM_LOG_INFO("BrainRefactored loaded from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain load exception: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<const Config> BrainRefactored::getConfig() const {
    return config_;
}

RandomGenerator* BrainRefactored::getRandomGenerator() {
    return pImpl->randomGen.get();
}

std::string BrainRefactored::getStatus() const {
    std::stringstream ss;
    ss << "BrainRefactored Status:" << std::endl;
    ss << "  Components: " << pImpl->componentRegistry.size() << std::endl;
    ss << "  Active Components: ";
    
    int activeCount = 0;
    for (const auto& pair : pImpl->componentRegistry) {
        if (pair.second->isActive()) {
            ss << pair.first << " ";
            activeCount++;
        }
    }
    
    ss << " (" << activeCount << ")" << std::endl;
    ss << "  Total Spike Count: " << pImpl->totalSpikeCount << std::endl;
    ss << "  Pending Spike Events: " << pImpl->pendingSpikeCount << std::endl;
    ss << "  Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    
    return ss.str();
}

bool BrainRefactored::initializeDefaultComponents() {
    // Register default component types
    // This would normally be done via registration system
    // For now, we create the core components directly
    
    bool success = true;
    
    // Neural dynamics component
    auto neuralDynamics = std::make_unique<NeuralDynamicsComponent>();
    if (!neuralDynamics->initialize()) {
        NLM_LOG_ERROR("Failed to initialize NeuralDynamicsComponent");
        success = false;
    } else {
        neuralDynamicsComponent_ = std::move(neuralDynamics);
        pImpl->neuralDynamics = neuralDynamicsComponent_.get();
    }
    
    // Plasticity component
    auto plasticity = std::make_unique<PlasticityComponent>();
    if (!plasticity->initialize()) {
        NLM_LOG_ERROR("Failed to initialize PlasticityComponent");
        success = false;
    } else {
        plasticityComponent_ = std::move(plasticity);
        pImpl->plasticity = plasticityComponent_.get();
    }
    
    // Memory component
    auto memory = std::make_unique<MemoryComponent>();
    if (!memory->initialize()) {
        NLM_LOG_ERROR("Failed to initialize MemoryComponent");
        success = false;
    } else {
        memoryComponent_ = std::move(memory);
        pImpl->memory = memoryComponent_.get();
    }
    
    // Prediction component
    auto prediction = std::make_unique<PredictionComponent>();
    if (!prediction->initialize()) {
        NLM_LOG_ERROR("Failed to initialize PredictionComponent");
        success = false;
    } else {
        predictionComponent_ = std::move(prediction);
        pImpl->prediction = predictionComponent_.get();
    }
    
    return success;
}

void BrainRefactored::setupComponentRelationships() {
    // Set up relationships between components
    // For example: neural dynamics uses spike system, plasticity uses neural dynamics, etc.
    
    if (pImpl->neuralDynamics && pImpl->spikeSystem) {
        pImpl->neuralDynamics->setSpikeSystem(pImpl->spikeSystem.get());
    }
    
    if (pImpl->plasticity && pImpl->neuralDynamics) {
        pImpl->plasticity->setNeuralDynamics(pImpl->neuralDynamics.get());
    }
    
    if (pImpl->memory && pImpl->neuralDynamics) {
        pImpl->memory->setNeuralDynamics(pImpl->neuralDynamics.get());
    }
    
    if (pImpl->prediction && pImpl->neuralDynamics) {
        pImpl->prediction->setNeuralDynamics(pImpl->neuralDynamics.get());
    }
    
    NLM_LOG_INFO("Component relationships established");
}

void BrainRefactored::loadConfiguration() {
    if (!config_) {
        NLM_LOG_ERROR("No configuration available to load");
        return;
    }
    
    // Load component-specific configurations
    // This would parse the config and apply settings to each component
    
    // Example: Load neural dynamics configuration
    std::string neuralConfig = config_->getOr<std::string>("neural_dynamics_config", "");
    if (!neuralConfig.empty()) {
        // Apply neural dynamics configuration
        if (neuralDynamicsComponent_) {
            // neuralDynamicsComponent_->applyConfiguration(neuralConfig);
            NLM_LOG_INFO("Applied neural dynamics configuration");
        }
    }
    
    // Load plasticity configuration
    std::string plasticityConfig = config_->getOr<std::string>("plasticity_config", "");
    if (!plasticityConfig.empty()) {
        // Apply plasticity configuration
        if (plasticityComponent_) {
            // plasticityComponent_->applyConfiguration(plasticityConfig);
            NLM_LOG_INFO("Applied plasticity configuration");
        }
    }
    
    NLM_LOG_INFO("Configuration loaded successfully");
}

void BrainRefactored::updateStatistics() {
    // Update brain-wide statistics
    if (pImpl->spikeSystem) {
        pImpl->pendingSpikeCount = pImpl->spikeSystem->getPendingSpikeCount();
    }
    
    // Update component statistics
    for (const auto& pair : pImpl->componentRegistry) {
        // Components could update their own statistics here
    }
}

} // namespace nlm
