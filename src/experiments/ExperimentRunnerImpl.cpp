#include "ExperimentRunnerImpl.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include "Phase6ConfigManager.hpp"
#include "ExperimentFactory.hpp"
#include "core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct Phase6ExperimentValidator::Impl {
    std::shared_ptr<Phase6ConfigManager> configManager_;
    std::vector<std::string> validationErrors_;
    std::vector<std::string> validationWarnings_;
    bool strictValidation_;
    
    Impl(std::shared_ptr<Phase6ConfigManager> configManager) 
        : configManager_(configManager), strictValidation_(false) {}
    
    void clearValidationErrors() {
        validationErrors_.clear();
        validationWarnings_.clear();
    }
    
    void addError(const std::string& error) {
        validationErrors_.push_back(error);
        NLM_LOG_ERROR("Validation error: " + error);
    }
    
    void addWarning(const std::string& warning) {
        validationWarnings_.push_back(warning);
        NLM_LOG_WARNING("Validation warning: " + warning);
    }
};

Phase6ExperimentValidator::Phase6ExperimentValidator(std::shared_ptr<Phase6ConfigManager> configManager)
    : pImpl(std::make_unique<Impl>(configManager)) {}

Phase6ExperimentValidator::~Phase6ExperimentValidator() = default;

bool Phase6ExperimentValidator::validateConfiguration(const std::string& configName) const {
    pImpl->clearValidationErrors();
    
    if (!configManager_) {
        pImpl->addError("Configuration manager not initialized");
        return false;
    }
    
    // Validate neuron count
    auto neuronCountOpt = configManager_->getConfig()->get<int64_t>("neuron_count");
    if (neuronCountOpt) {
        if (!validateNeuronCount(static_cast<size_t>(neuronCountOpt.value()))) {
            pImpl->addError("Invalid neuron count: must be positive");
        }
    }
    
    // Validate max steps
    auto maxStepsOpt = configManager_->getConfig()->get<int64_t>("max_steps");
    if (maxStepsOpt) {
        if (!validateMaxSteps(maxStepsOpt.value())) {
            pImpl->addError("Invalid max steps: must be positive");
        }
    }
    
    // Validate world dimensions
    auto worldWidthOpt = configManager_->getConfig()->get<float>("world_width");
    auto worldHeightOpt = configManager_->getConfig()->get<float>("world_height");
    if (worldWidthOpt && worldHeightOpt) {
        if (!validateWorldDimensions(worldWidthOpt.value(), worldHeightOpt.value())) {
            pImpl->addError("Invalid world dimensions");
        }
    }
    
    // Validate connection probability
    auto connProbOpt = configManager_->getConfig()->get<float>("connection_probability");
    if (connProbOpt) {
        if (!validateConnectionProbability(connProbOpt.value())) {
            pImpl->addError("Invalid connection probability");
        }
    }
    
    // Validate checkpoint path
    auto checkpointPathOpt = configManager_->getConfig()->get<std::string>("checkpoint_path");
    if (checkpointPathOpt && !checkpointPathOpt.value().empty()) {
        // Check if checkpoint directory exists or can be created
        // (simple check - can be extended)
    }
    
    return !pImpl->validationErrors_.empty();
}

bool Phase6ExperimentValidator::validateSetup(const std::string& configName) const {
    pImpl->clearValidationErrors();
    
    // Validate system readiness
    if (!configManager_->isValid()) {
        for (const auto& error : configManager_->getValidationErrors()) {
            pImpl->addError("Configuration error: " + error);
        }
    }
    
    // Validate experiment factory
    auto& factoryRegistry = ExperimentFactoryRegistry::getInstance();
    if (!factoryRegistry.hasFactory("default")) {
        pImpl->addWarning("Default experiment factory not registered");
    }
    
    // Validate system creators
    auto& creatorRegistry = SystemCreatorRegistry::getInstance();
    auto brainCreators = creatorRegistry.getBrainCreators();
    auto worldCreators = creatorRegistry.getWorldCreators();
    
    if (brainCreators.empty()) {
        pImpl->addError("No brain creators available");
    }
    
    if (worldCreators.empty()) {
        pImpl->addError("No world creators available");
    }
    
    return pImpl->validationErrors_.empty();
}

std::vector<std::string> Phase6ExperimentValidator::getValidationErrors() const {
    return pImpl->validationErrors_;
}

std::vector<std::string> Phase6ExperimentValidator::getValidationWarnings() const {
    return pImpl->validationWarnings_;
}

bool Phase6ExperimentValidator::validateNeuronCount(size_t count) const {
    return count > 0 && count < 1000000; // Reasonable upper bound
}

bool Phase6ExperimentValidator::validateMaxSteps(uint64_t steps) const {
    return steps > 0 && steps < 1000000000; // Reasonable upper bound
}

bool Phase6ExperimentValidator::validateWorldDimensions(float width, float height) const {
    return width > 0 && height > 0 && width < 10000 && height < 10000;
}

bool Phase6ExperimentValidator::validateConnectionProbability(float probability) const {
    return probability >= 0.0f && probability <= 1.0f;
}

struct Phase6ExperimentRunner::Impl {
    std::shared_ptr<Phase6ConfigManager> configManager_;
    std::string currentConfigName_;
    std::shared_ptr<Phase6IntegratedExperiment> experiment_;
    std::shared_ptr<Phase6ConfigManager> experimentConfig_;
    std::vector<double> results_;
    std::vector<std::string> metrics_;
    std::string status_;
    bool running_;
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    uint64_t stepsCompleted_;
    uint64_t totalSteps_;
    double executionTime_;
    
    Impl(std::shared_ptr<Phase6ConfigManager> configManager)
        : configManager_(configManager), running_(false), stepsCompleted_(0), 
          totalSteps_(0), executionTime_(0.0) {}
    
    void updateExecutionTime() {
        if (startTime_ > endTime_) {
            auto now = std::chrono::steady_clock::now();
            executionTime_ = std::chrono::duration<double>(now - startTime_).count();
        }
    }
    
    void resetState() {
        currentConfigName_.clear();
        experiment_.reset();
        experimentConfig_.reset();
        results_.clear();
        metrics_.clear();
        status_ = "Not running";
        running_ = false;
        stepsCompleted_ = 0;
        totalSteps_ = 0;
        executionTime_ = 0.0;
    }
};

Phase6ExperimentRunner::Phase6ExperimentRunner(std::shared_ptr<Phase6ConfigManager> configManager)
    : pImpl(std::make_unique<Impl>(configManager)), strictExecution_(false), executionInterval_(0.0) {}

Phase6ExperimentRunner::~Phase6ExperimentRunner() = default;

bool Phase6ExperimentRunner::run(const std::string& configName) {
    if (pImpl->running_) {
        NLM_LOG_WARNING("Experiment already running");
        return false;
    }
    
    if (!setConfiguration(configName)) {
        return false;
    }
    
    // Set up experiment
    auto& factoryRegistry = ExperimentFactoryRegistry::getInstance();
    auto experiment = factoryRegistry.createExperiment("default", *pImpl->experimentConfig_);
    
    if (!experiment) {
        NLM_LOG_ERROR("Failed to create experiment");
        setStatus("Failed to create experiment");
        return false;
    }
    
    pImpl->experiment_ = experiment;
    pImpl->experimentConfig_ = pImpl->configManager_;
    pImpl->running_ = true;
    pImpl->startTime_ = std::chrono::steady_clock::now();
    pImpl->status_ = "Running";
    
    NLM_LOG_INFO("Starting Phase 6 experiment with configuration: " + configName);
    
    // Run the experiment
    auto result = pImpl->experiment_->run(*pImpl->experimentConfig_);
    
    pImpl->endTime_ = std::chrono::steady_clock::now();
    pImpl->running_ = false;
    
    if (result.memoryWorkingMemoryIntegrated) {
        NLM_LOG_INFO("Phase 6 experiment completed successfully");
        setStatus("Completed successfully");
        
        // Collect metrics
        addResult("total_reward", result.totalReward);
        addResult("avg_firing_rate", result.avgFiringRate);
        addResult("memory_episodes_stored", result.memoryEpisodesStored);
        addResult("novelty_level", result.noveltyLevel);
        addResult("curiosity_level", result.curiosityLevel);
        addResult("dopamine_level", result.dopamineLevel);
        
        metrics_ = {
            "total_reward", "avg_firing_rate", "memory_episodes_stored",
            "novelty_level", "curiosity_level", "dopamine_level"
        };
        
        return true;
    } else {
        NLM_LOG_ERROR("Phase 6 experiment failed");
        setStatus("Failed");
        return false;
    }
}

std::vector<double> Phase6ExperimentRunner::getResults() const {
    return pImpl->results_;
}

std::vector<std::string> Phase6ExperimentRunner::getMetrics() const {
    return pImpl->metrics_;
}

std::string Phase6ExperimentRunner::getStatus() const {
    return pImpl->status_;
}

bool Phase6ExperimentRunner::isComplete() const {
    return !pImpl->running_ && !pImpl->status_.empty() && pImpl->status_ != "Not running";
}

void Phase6ExperimentRunner::stop() {
    if (pImpl->running_) {
        NLM_LOG_INFO("Stopping Phase 6 experiment");
        pImpl->running_ = false;
        setStatus("Stopped");
    }
}

bool Phase6ExperimentRunner::setConfiguration(const std::string& configName) {
    if (!pImpl->configManager_) {
        NLM_LOG_ERROR("Configuration manager not available");
        return false;
    }
    
    pImpl->resetState();
    pImpl->currentConfigName_ = configName;
    
    // Load configuration (simplified - in real implementation would load from file)
    pImpl->experimentConfig_ = std::make_shared<Phase6ConfigManager>(*pImpl->configManager_);
    
    // Set experiment configuration
    Phase6Config config;
    config.maxSteps = pImpl->experimentConfig_->getMaxSteps();
    config.neuronCount = pImpl->experimentConfig_->getNeuronCount();
    config.regionCount = pImpl->experimentConfig_->getRegionCount();
    config.connectionProbability = pImpl->experimentConfig_->getConnectionProbability();
    config.enableCheckpointing = pImpl->experimentConfig_->isCheckpointingEnabled();
    config.enableReplay = pImpl->experimentConfig_->isReplayEnabled();
    config.enableDevelopment = pImpl->experimentConfig_->isDevelopmentEnabled();
    config.checkpointPath = pImpl->experimentConfig_->getCheckpointPath();
    
    NLM_LOG_INFO("Loaded configuration: " + configName);
    NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("  Steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Checkpointing: " + std::string(config.enableCheckpointing ? "YES" : "NO"));
    
    return true;
}

void Phase6ExperimentRunner::addResult(const std::string& metric, double value) {
    pImpl->results_.push_back(value);
    NLM_LOG_INFO("Metric " + metric + ": " + std::to_string(value));
}

void Phase6ExperimentRunner::setStatus(const std::string& status) {
    pImpl->status_ = status;
    NLM_LOG_INFO("Experiment status: " + status);
}

struct ExperimentValidatorRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IExperimentValidator>> validators;
    
    Impl() {
        // Register default validator
        registerValidator("phase6", std::make_unique<Phase6ExperimentValidator>(
            std::make_shared<Phase6ConfigManager>()));
    }
    
    void registerValidator(const std::string& name, std::unique_ptr<IExperimentValidator> validator) {
        if (validator) {
            validators[name] = std::move(validator);
            NLM_LOG_INFO("Registered experiment validator: " + name);
        }
    }
    
    std::shared_ptr<IExperimentValidator> getValidator(const std::string& name) {
        auto it = validators.find(name);
        if (it != validators.end()) {
            return std::shared_ptr<IExperimentValidator>(it->second.get(),
                [this, name](IExperimentValidator* ptr) {
                    // Custom deleter to not free the pointer
                });
        }
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredValidators() const {
        std::vector<std::string> names;
        names.reserve(validators.size());
        for (const auto& pair : validators) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool hasValidator(const std::string& name) const {
        return validators.find(name) != validators.end();
    }
    
    void unregisterValidator(const std::string& name) {
        auto it = validators.find(name);
        if (it != validators.end()) {
            NLM_LOG_INFO("Unregistered experiment validator: " + name);
            validators.erase(it);
        }
    }
};

ExperimentValidatorRegistry::ExperimentValidatorRegistry() : pImpl(std::make_unique<Impl>()) {}

ExperimentValidatorRegistry& ExperimentValidatorRegistry::getInstance() {
    static ExperimentValidatorRegistry instance;
    return instance;
}

void ExperimentValidatorRegistry::registerValidator(const std::string& name, std::unique_ptr<IExperimentValidator> validator) {
    pImpl->registerValidator(name, std::move(validator));
}

std::shared_ptr<IExperimentValidator> ExperimentValidatorRegistry::getValidator(const std::string& name) {
    return pImpl->getValidator(name);
}

std::vector<std::string> ExperimentValidatorRegistry::getRegisteredValidators() const {
    return pImpl->getRegisteredValidators();
}

bool ExperimentValidatorRegistry::hasValidator(const std::string& name) const {
    return pImpl->hasValidator(name);
}

void ExperimentValidatorRegistry::unregisterValidator(const std::string& name) {
    pImpl->unregisterValidator(name);
}

struct ExperimentRunnerRegistry::Impl {
    std::unordered_map<std::string, std::unique_ptr<IExperimentRunner>> runners;
    
    Impl() {
        // Register default runner
        registerRunner("phase6", std::make_unique<Phase6ExperimentRunner>(
            std::make_shared<Phase6ConfigManager>()));
    }
    
    void registerRunner(const std::string& name, std::unique_ptr<IExperimentRunner> runner) {
        if (runner) {
            runners[name] = std::move(runner);
            NLM_LOG_INFO("Registered experiment runner: " + name);
        }
    }
    
    std::shared_ptr<IExperimentRunner> getRunner(const std::string& name) {
        auto it = runners.find(name);
        if (it != runners.end()) {
            return std::shared_ptr<IExperimentRunner>(it->second.get(),
                [this, name](IExperimentRunner* ptr) {
                    // Custom deleter to not free the pointer
                });
        }
        return nullptr;
    }
    
    std::vector<std::string> getRegisteredRunners() const {
        std::vector<std::string> names;
        names.reserve(runners.size());
        for (const auto& pair : runners) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    bool hasRunner(const std::string& name) const {
        return runners.find(name) != runners.end();
    }
    
    void unregisterRunner(const std::string& name) {
        auto it = runners.find(name);
        if (it != runners.end()) {
            NLM_LOG_INFO("Unregistered experiment runner: " + name);
            runners.erase(it);
        }
    }
};

ExperimentRunnerRegistry::ExperimentRunnerRegistry() : pImpl(std::make_unique<Impl>()) {}

ExperimentRunnerRegistry& ExperimentRunnerRegistry::getInstance() {
    static ExperimentRunnerRegistry instance;
    return instance;
}

void ExperimentRunnerRegistry::registerRunner(const std::string& name, std::unique_ptr<IExperimentRunner> runner) {
    pImpl->registerRunner(name, std::move(runner));
}

std::shared_ptr<IExperimentRunner> ExperimentRunnerRegistry::getRunner(const std::string& name) {
    return pImpl->getRunner(name);
}

std::vector<std::string> ExperimentRunnerRegistry::getRegisteredRunners() const {
    return pImpl->getRegisteredRunners();
}

bool ExperimentRunnerRegistry::hasRunner(const std::string& name) const {
    return pImpl->hasRunner(name);
}

void ExperimentRunnerRegistry::unregisterRunner(const std::string& name) {
    pImpl->unregisterRunner(name);
}

} // namespace nlm
