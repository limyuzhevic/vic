// nlm/api.cpp - Implementation of high-level NLM API
#include "api.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nlm {

// BrainController implementation
struct BrainController::Impl {
    std::shared_ptr<Brain> brain_;
    BrainFactoryConfig config_;
    std::vector<BehavioralRule> behavioralRules_;
    std::string currentTargetBehavior_;
    std::string environmentalConditions_;
    std::vector<std::function<void()>> scheduledEvents_;
    std::map<double, std::function<void()>> eventSchedule_;
    double currentTime_ = 0.0;
    bool isHealthy_ = true;
    size_t stepCount_ = 0;
};

BrainController::BrainController() : pImpl(std::make_unique<Impl>()) {
}

BrainController::BrainController(const BrainFactoryConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config_ = config;
}

BrainController::BrainController(std::shared_ptr<Brain> brain) : pImpl(std::make_unique<Impl>()) {
    pImpl->brain_ = brain;
}

BrainController::~BrainController() = default;

bool BrainController::initialize() {
    if (pImpl->brain_) {
        return pImpl->brain_->initialize();
    }
    
    try {
        pImpl->brain_ = BrainFactory::createStandardBrain(pImpl->config_);
        if (!pImpl->brain_->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain");
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during brain initialization: " + std::string(e.what()));
        return false;
    }
    
    NLM_LOG_INFO("BrainController initialized successfully");
    return true;
}

void BrainController::reset() {
    if (pImpl->brain_) {
        pImpl->brain_->reset();
    }
    pImpl->stepCount_ = 0;
    pImpl->currentTime_ = 0.0;
    NLM_LOG_INFO("BrainController reset");
}

void BrainController::step(double timestep) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot step: brain not initialized");
        return;
    }
    
    // Execute scheduled events
    std::vector<std::function<void()>> eventsToExecute;
    for (const auto& pair : pImpl->eventSchedule_) {
        if (pair.first <= pImpl->currentTime_) {
            eventsToExecute.push_back(pair.second);
        }
    }
    
    for (const auto& event : eventsToExecute) {
        try {
            event();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in scheduled event: " + std::string(e.what()));
        }
    }
    
    // Remove executed events
    for (const auto& pair : pImpl->eventSchedule_) {
        if (pair.first <= pImpl->currentTime_) {
            pImpl->eventSchedule_.erase(pair.first);
        }
    }
    
    // Update brain
    pImpl->brain_->step(pImpl->stepCount_, timestep);
    pImpl->stepCount_++;
    pImpl->currentTime_ += timestep;
    
    // Apply behavioral rules
    for (const auto& rule : pImpl->behavioralRules_) {
        applyBehavioralRule(rule);
    }
}

void BrainController::runSteps(size_t steps, double timestep) {
    for (size_t i = 0; i < steps; ++i) {
        step(timestep);
    }
}

void BrainController::injectPattern(const Pattern& pattern) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot inject pattern: brain not initialized");
        return;
    }
    
    // Simple pattern injection: distribute pattern values across neurons
    size_t neuronCount = pImpl->brain_->getTotalNeuronCount();
    size_t patternSize = std::min(pattern.neurons.size(), neuronCount);
    
    for (size_t i = 0; i < patternSize; ++i) {
        NeuronId neuronId(i);
        float current = pattern.neurons[i] * pattern.weight;
        pImpl->brain_->injectCurrent(neuronId, current);
    }
}

void BrainController::injectCurrent(NeuronId neuronId, float current) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot inject current: brain not initialized");
        return;
    }
    
    pImpl->brain_->injectCurrent(neuronId, current);
}

void BrainController::injectCurrentToType(NeuronType type, float current) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot inject current: brain not initialized");
        return;
    }
    
    pImpl->brain_->injectCurrentToNeurons(type, current);
}

std::vector<float> BrainController::getInternalState() const {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot get internal state: brain not initialized");
        return {};
    }
    
    // Get firing rates of all neurons as internal state
    std::vector<float> internalState;
    auto regions = pImpl->brain_->getRegions();
    
    for (const auto& region : regions) {
        auto neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            internalState.push_back(neuron->getMembranePotential());
        }
    }
    
    return internalState;
}

MotorCommand BrainController::getLastAction() const {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot get last action: brain not initialized");
        return MotorCommand::Wait;
    }
    
    auto action = pImpl->brain_->produceAction();
    return action->getCommand();
}

void BrainController::applyLearningScenario(const LearningScenario& scenario) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot apply learning scenario: brain not initialized");
        return;
    }
    
    for (const auto& pattern : scenario.inputPatterns) {
        trainPattern(pattern, 0.0f); // Target output placeholder
        runSteps(scenario.duration);
    }
}

void BrainController::trainPattern(const Pattern& input, float targetOutput) {
    injectPattern(input);
    runSteps(10); // Allow pattern to propagate
    
    // Simple learning rule: strengthen connections based on activity
    if (pImpl->brain_) {
        // This is a simplified learning implementation
        // In a real implementation, you would use the brain's plasticity system
    }
}

void BrainController::enablePlasticity(bool enable) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot enable plasticity: brain not initialized");
        return;
    }
    
    // Enable/disable plasticity rules in the brain
    // This would typically involve accessing the brain's plasticity system
    NLM_LOG_INFO("Plasticity " + std::string(enable ? "enabled" : "disabled"));
}

void BrainController::setLearningRate(float rate) {
    // Store learning rate for future use
    NLM_LOG_INFO("Learning rate set to: " + std::to_string(rate));
}

void BrainController::addBehavioralRule(const BehavioralRule& rule) {
    pImpl->behavioralRules_.push_back(rule);
    NLM_LOG_INFO("Added behavioral rule: " + rule.name);
}

void BrainController::removeBehavioralRule(const std::string& ruleName) {
    auto it = std::remove_if(pImpl->behavioralRules_.begin(), pImpl->behavioralRules_.end(),
        [&ruleName](const BehavioralRule& rule) { return rule.name == ruleName; });
    
    if (it != pImpl->behavioralRules_.end()) {
        pImpl->behavioralRules_.erase(it, pImpl->behavioralRules_.end());
        NLM_LOG_INFO("Removed behavioral rule: " + ruleName);
    } else {
        NLM_LOG_WARNING("Behavioral rule not found: " + ruleName);
    }
}

void BrainController::modifyBehavioralRule(const std::string& ruleName, const BehavioralRule& newRule) {
    for (auto& rule : pImpl->behavioralRules_) {
        if (rule.name == ruleName) {
            rule = newRule;
            NLM_LOG_INFO("Modified behavioral rule: " + ruleName);
            return;
        }
    }
    NLM_LOG_WARNING("Cannot modify behavioral rule: not found " + ruleName);
}

std::vector<MotorCommand> BrainController::getAvailableActions() const {
    // Return typical set of available motor commands
    return {
        MotorCommand::MoveForward,
        MotorCommand::MoveBackward,
        MotorCommand::TurnLeft,
        MotorCommand::TurnRight,
        MotorCommand::Interact,
        MotorCommand::LookLeft,
        MotorCommand::LookRight,
        MotorCommand::Wait
    };
}

float BrainController::getCurrentFiringRate() const {
    if (!pImpl->brain_) {
        return 0.0f;
    }
    return pImpl->brain_->getAverageFiringRate();
}

 size_t BrainController::getTotalSpikeCount() const {
    if (!pImpl->brain_) {
        return 0;
    }
    return pImpl->brain_->getTotalSpikeCount();
}

 size_t BrainController::getActiveNeuronCount() const {
    if (!pImpl->brain_) {
        return 0;
    }
    return pImpl->brain_->getActiveNeuronCount();
}

float BrainController::getLearningProgress() const {
    // Simple learning progress based on spike count
    static size_t lastSpikeCount = 0;
    if (!pImpl->brain_) {
        return 0.0f;
    }
    
    size_t currentSpikeCount = pImpl->brain_->getTotalSpikeCount();
    size_t spikeIncrease = currentSpikeCount - lastSpikeCount;
    lastSpikeCount = currentSpikeCount;
    
    return static_cast<float>(spikeIncrease) / 1000.0f; // Normalized progress
}

std::map<std::string, float> BrainController::getPerformanceMetrics() const {
    std::map<std::string, float> metrics;
    
    if (!pImpl->brain_) {
        return metrics;
    }
    
    metrics["firing_rate"] = getCurrentFiringRate();
    metrics["total_spikes"] = static_cast<float>(getTotalSpikeCount());
    metrics["active_neurons"] = static_cast<float>(getActiveNeuronCount());
    metrics["learning_progress"] = getLearningProgress();
    metrics["step_count"] = static_cast<float>(pImpl->stepCount_);
    
    return metrics;
}

bool BrainController::saveState(const std::string& filepath) const {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot save state: brain not initialized");
        return false;
    }
    
    return pImpl->brain_->save(filepath);
}

bool BrainController::loadState(const std::string& filepath) {
    if (!pImpl->brain_) {
        NLM_LOG_ERROR("Cannot load state: brain not initialized");
        return false;
    }
    
    return pImpl->brain_->load(filepath);
}

bool BrainController::exportBrain(const std::string& filepath) const {
    return saveState(filepath);
}

bool BrainController::importBrain(const std::string& filepath) {
    return loadState(filepath);
}

bool BrainController::registerPlugin(std::unique_ptr<plugins::IntegrationPlugin> plugin) {
    // This would integrate with the global plugin manager
    NLM_LOG_INFO("Plugin registration requested (not yet implemented): " + plugin->getName());
    return false;
}

bool BrainController::unregisterPlugin(const std::string& name) {
    NLM_LOG_INFO("Plugin unregistration requested (not yet implemented): " + name);
    return false;
}

bool BrainController::enablePlugin(const std::string& name) {
    NLM_LOG_INFO("Plugin enable requested (not yet implemented): " + name);
    return false;
}

bool BrainController::disablePlugin(const std::string& name) {
    NLM_LOG_INFO("Plugin disable requested (not yet implemented): " + name);
    return false;
}

void BrainController::setTargetBehavior(const std::string& behavior) {
    pImpl->currentTargetBehavior_ = behavior;
    NLM_LOG_INFO("Target behavior set to: " + behavior);
}

void BrainController::setEnvironmentalConditions(const std::string& conditions) {
    pImpl->environmentalConditions_ = conditions;
    NLM_LOG_INFO("Environmental conditions set to: " + conditions);
}

void BrainController::scheduleEvent(double time, const std::function<void()>& event) {
    pImpl->eventSchedule_[time] = event;
    NLM_LOG_INFO("Scheduled event at time " + std::to_string(time));
}

std::string BrainController::getBrainInfo() const {
    std::ostringstream oss;
    oss << "BrainController Info:" << std::endl;
    oss << "  Brain initialized: " << (pImpl->brain_ ? "yes" : "no") << std::endl;
    oss << "  Neuron count: " << (pImpl->brain_ ? pImpl->brain_->getTotalNeuronCount() : 0) << std::endl;
    oss << "  Synapse count: " << (pImpl->brain_ ? pImpl->brain_->getTotalSynapseCount() : 0) << std::endl;
    oss << "  Step count: " << pImpl->stepCount_ << std::endl;
    oss << "  Current time: " << pImpl->currentTime_ << std::endl;
    oss << "  Behavioral rules: " << pImpl->behavioralRules_.size() << std::endl;
    oss << "  Target behavior: " << pImpl->currentTargetBehavior_ << std::endl;
    oss << "  Environmental conditions: " << pImpl->environmentalConditions_ << std::endl;
    
    return oss.str();
}

std::vector<std::string> BrainController::getPluginStatus() const {
    return {"Plugin status reporting not yet implemented"};
}

bool BrainController::isHealthy() const {
    return pImpl->isHealthy_ && pImpl->brain_ && pImpl->brain_->initialize();
}

void BrainController::applyBehavioralRule(const BehavioralRule& rule) {
    // Apply behavioral rule based on type
    switch (rule.type) {
        case BehavioralRule::Type::Reflex:
            // Reflex-based behavior
            break;
        case BehavioralRule::Type::Habit:
            // Habit-based behavior
            break;
        case BehavioralRule::Type::Exploration:
            // Exploration-based behavior
            break;
        case BehavioralRule::Type::SocialImitation:
            // Social imitation behavior
            break;
        case BehavioralRule::Type::RewardBased:
            // Reward-based behavior
            break;
        case BehavioralRule::Type::NoveltyBased:
            // Novelty-based behavior
            break;
        case BehavioralRule::Type::Predictive:
            // Predictive behavior
            break;
    }
}

void BrainController::updateConfig(const BrainFactoryConfig& config) {
    pImpl->config_ = config;
    if (pImpl->brain_) {
        BrainFactory::applyConfigToBrain(pImpl->brain_, config);
    }
}

BrainFactoryConfig BrainController::getConfig() const {
    return pImpl->config_;
}

// Other class implementations would continue here...

// Global API instances
namespace {
    BrainController globalBrainController;
    ExperimentRunner globalExperimentRunner;
    VisualizationInterface globalVisualizationInterface;
    MetricsCollector globalMetricsCollector;
    bool nlmInitialized = false;
}

BrainController& getGlobalBrainController() {
    return globalBrainController;
}

ExperimentRunner& getGlobalExperimentRunner() {
    return globalExperimentRunner;
}

VisualizationInterface& getGlobalVisualizationInterface() {
    return globalVisualizationInterface;
}

MetricsCollector& getGlobalMetricsCollector() {
    return globalMetricsCollector;
}

void initializeNLM() {
    if (!nlmInitialized) {
        Logger::setGlobal(std::make_shared<Logger>());
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        Logger::getGlobal().addLogger(consoleLogger);
        
        NLM_LOG_INFO("NLM API initialized");
        nlmInitialized = true;
    }
}

void shutdownNLM() {
    if (nlmInitialized) {
        NLM_LOG_INFO("NLM API shutdown");
        nlmInitialized = false;
    }
}

bool isNLMInitialized() {
    return nlmInitialized;
}

std::string getNLMVersion() {
    return "0.1.0";
}

void printSystemInfo() {
    std::cout << "=== NLM System Information ===" << std::endl;
    std::cout << "Version: " << getNLMVersion() << std::endl;
    std::cout << "Initialized: " << (isNLMInitialized() ? "Yes" : "No") << std::endl;
    std::cout << "================================" << std::endl;
}

// Convenience functions implementation
std::shared_ptr<BrainController> api::createBrainController(const std::string& profile) {
    BrainFactoryConfig config = getConfigFromProfile(profile);
    auto controller = std::make_shared<BrainController>(config);
    if (controller->initialize()) {
        return controller;
    }
    return nullptr;
}

std::shared_ptr<BrainController> api::createMinimalBrainController() {
    return createBrainController("quick");
}

std::shared_ptr<BrainController> api::createResearchBrainController() {
    return createBrainController("research");
}

std::shared_ptr<BrainController> api::createDemoBrainController(const std::string& demoType) {
    return createBrainController(demoType);
}

// Remaining implementations would continue...

} // namespace nlm
