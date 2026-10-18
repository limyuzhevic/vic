#include "ExperimentRunner.hpp"
#include "../core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>

namespace nlm {

struct ExperimentRunner::Impl {
    std::vector<std::shared_ptr<Experiment>> experiments;
    uint64_t globalSeed;
    std::map<std::string, std::shared_ptr<Brain>> brainTemplates;
    std::map<std::string, std::shared_ptr<Environment>> environmentTemplates;
    BatchExperimentManager::BatchConfig batchConfig;
    CloningConfig cloningConfig;
    std::string runnerName;
    bool loggingEnabled;
    
    Impl() : globalSeed(42), loggingEnabled(true) {
        creationTime_ = std::chrono::system_clock::now();
    }
};

ExperimentRunner::ExperimentRunner() : pImpl(new Impl), loggingEnabled(true) {
    pImpl->creationTime_ = std::chrono::system_clock::now();
}

ExperimentRunner::~ExperimentRunner() = default;

std::shared_ptr<Experiment> ExperimentRunner::createExperiment(const std::string& name) {
    auto exp = std::make_shared<Experiment>(name);
    exp->setSeed(pImpl->globalSeed++);
    pImpl->experiments.push_back(exp);
    return exp;
}

bool ExperimentRunner::runExperiment(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     SimulationStep maxSteps) {
    if (!experiment || !brain || !environment) {
        return false;
    }
    
    experiment->setStartStep(0);
    
    // Reset environment and brain
    auto observation = environment->reset();
    brain->receiveSensoryInput(*observation);
    
    // Run simulation loop
    for (SimulationStep step = 0; step < maxSteps && !environment->isDone(); ++step) {
        // Brain step
        brain->step(step);
        
        // Record data
        experiment->recordStep(
            step,
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getFiringNeuronCount(),
            brain->getAverageFiringRate()
        );
        
        // Get action from brain
        auto action = brain->produceAction();
        
        // Environment step
        observation = environment->step(*action);
        
        // Send observation to brain
        brain->receiveSensoryInput(*observation);
        
        // Record reward
        float reward = environment->getLastReward();
        experiment->recordMetric("total_reward", 
            experiment->getMetric("total_reward") + reward);
    }
    
    experiment->setEndStep(maxSteps);
    return true;
}

std::vector<std::shared_ptr<Experiment>> ExperimentRunner::runBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    SimulationStep maxSteps) {
    
    std::vector<std::shared_ptr<Experiment>> results;
    
    for (auto& exp : experiments) {
        // Create fresh brain and environment for each experiment
        // PLACEHOLDER: In real implementation, would clone from template
        auto brain = std::make_shared<Brain>(brainTemplate->getConfig());
        brain->initialize();
        
        // Would also create fresh environment here
        
        if (runExperiment(exp, brain, environmentTemplate, maxSteps)) {
            results.push_back(exp);
        }
    }
    
    return results;
}

const std::vector<std::shared_ptr<Experiment>>& ExperimentRunner::getExperiments() const {
    return pImpl->experiments;
}

std::shared_ptr<Experiment> ExperimentRunner::getExperiment(const std::string& name) const {
    for (auto& exp : pImpl->experiments) {
        if (exp->getName() == name) {
            return exp;
        }
    }
    return nullptr;
}

void ExperimentRunner::clearExperiments() {
    pImpl->experiments.clear();
}

void ExperimentRunner::setGlobalSeed(uint64_t seed) {
    pImpl->globalSeed = seed;
}

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    // TODO PHASE 2: Implement results saving
    return false;
}

// ========== CLONING FUNCTIONALITY ==========

std::shared_ptr<Brain> ExperimentRunner::cloneBrain(std::shared_ptr<Brain> original,
                                                    const std::string& templateName,
                                                    BrainClone::CloneMode mode) {
    if (!original) return nullptr;
    
    std::shared_ptr<Config> templateConfig;
    if (!templateName.empty()) {
        auto it = pImpl->brainTemplates.find(templateName);
        if (it != pImpl->brainTemplates.end()) {
            templateConfig = it->second->getConfig();
        }
    }
    
    if (!templateConfig) {
        templateConfig = original->getConfig();
    }
    
    return BrainClone::cloneFromTemplate(original, *templateConfig, mode);
}

std::shared_ptr<Environment> ExperimentRunner::cloneEnvironment(std::shared_ptr<Environment> original,
                                                                 EnvironmentClone::CloneMode mode) {
    if (!original) return nullptr;
    
    return EnvironmentClone::cloneFromTemplate(original, mode);
}

std::shared_ptr<Experiment> ExperimentRunner::cloneExperiment(std::shared_ptr<Experiment> original) {
    if (!original) return nullptr;
    
    auto cloned = std::make_shared<Experiment>(original->getName());
    cloned->setSeed(original->getSeed());
    
    // Copy metrics
    auto metrics = original->getMetrics();
    for (const auto& pair : metrics) {
        cloned->recordMetric(pair.first, pair.second);
    }
    
    // Copy step data
    auto stepData = original->getStepData();
    for (const auto& step : stepData) {
        cloned->recordStep(step.step, step.neuronCount, step.synapseCount,
                          step.firingCount, step.avgFiringRate);
    }
    
    // Copy notes
    auto notes = original->getNotes();
    for (const auto& note : notes) {
        cloned->addNote(note);
    }
    
    return cloned;
}

void ExperimentRunner::setBrainTemplate(const std::string& name, std::shared_ptr<Brain> templateBrain) {
    if (templateBrain) {
        pImpl->brainTemplates[name] = templateBrain;
    }
}

void ExperimentRunner::setEnvironmentTemplate(const std::string& name, std::shared_ptr<Environment> templateEnvironment) {
    if (templateEnvironment) {
        pImpl->environmentTemplates[name] = templateEnvironment;
    }
}

std::shared_ptr<Brain> ExperimentRunner::getBrainTemplate(const std::string& name) const {
    auto it = pImpl->brainTemplates.find(name);
    return (it != pImpl->brainTemplates.end()) ? it->second : nullptr;
}

std::shared_ptr<Environment> ExperimentRunner::getEnvironmentTemplate(const std::string& name) const {
    auto it = pImpl->environmentTemplates.find(name);
    return (it != pImpl->environmentTemplates.end()) ? it->second : nullptr;
}

// ========== BATCH EXPERIMENT MANAGEMENT ==========

std::vector<ExperimentResult> ExperimentRunner::executeBatchExperiments(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    BatchExperimentManager::BatchConfig config
) {
    if (brainTemplate) {
        pImpl->brainTemplates["default"] = brainTemplate;
    }
    if (environmentTemplate) {
        pImpl->environmentTemplates["default"] = environmentTemplate;
    }
    
    batchManager_.configure(config);
    
    std::shared_ptr<Brain> templateBrain = brainTemplate ? brainTemplate : getBrainTemplate("default");
    std::shared_ptr<Environment> templateEnv = environmentTemplate ? environmentTemplate : getEnvironmentTemplate("default");
    
    if (!templateBrain || !templateEnv) {
        throw std::invalid_argument("Brain or environment template is required");
    }
    
    return batchManager_.executeBatch(experiments, templateBrain, templateEnv);
}

void ExperimentRunner::configureBatchManager(const BatchExperimentManager::BatchConfig& config) {
    pImpl->batchConfig = config;
    batchManager_.configure(config);
}

// ========== EXPERIMENT RESULT AGGREGATION ==========

ExperimentResult ExperimentRunner::aggregateResults(const std::vector<ExperimentResult>& results,
                                                    const std::string& metricName,
                                                    const std::string& aggregationType) {
    return ExperimentResult::aggregateResults(results, metricName, aggregationType);
}

std::vector<bool> ExperimentRunner::compareExperimentResults(const std::vector<ExperimentResult>& results,
                                                            double tolerance) {
    return ExperimentResult::compareExperimentResults(results, tolerance);
}

bool ExperimentRunner::saveExperimentResults(const std::vector<ExperimentResult>& results,
                                             const std::string& filepath) {
    return ExperimentResult::saveResults(results, filepath);
}

std::vector<ExperimentResult> ExperimentRunner::loadExperimentResults(const std::string& filepath) {
    return ExperimentResult::loadResults(filepath);
}

// ========== PAUSE/RESUME FUNCTIONALITY ==========

void ExperimentRunner::pauseBatchExecution() {
    batchManager_.pause();
}

void ExperimentRunner::resumeBatchExecution() {
    batchManager_.resume();
}

bool ExperimentRunner::isBatchPaused() const {
    return batchManager_.isPaused();
}

std::pair<size_t, size_t> ExperimentRunner::getBatchProgress() const {
    const auto& summary = batchManager_.getResultSummary();
    return std::make_pair(summary.completedExperiments, summary.totalExperiments);
}

std::string ExperimentRunner::getBatchStatusMessage() const {
    switch (batchManager_.getStatus()) {
        case BatchExperimentManager::BatchStatus::IDLE:
            return "Batch execution is idle";
        case BatchExperimentManager::BatchStatus::RUNNING:
            return "Batch execution is in progress";
        case BatchExperimentManager::BatchStatus::PAUSED:
            return "Batch execution is paused";
        case BatchExperimentManager::BatchStatus::COMPLETED:
            return "Batch execution completed";
        case BatchExperimentManager::BatchStatus::FAILED:
            return "Batch execution failed";
        case BatchExperimentManager::BatchStatus::CANCELLED:
            return "Batch execution cancelled";
        default:
            return "Unknown batch status";
    }
}

// ========== CHECKPOINT FUNCTIONALITY ==========

bool ExperimentRunner::saveExperimentCheckpoint(std::shared_ptr<Experiment> experiment,
                                                const std::string& checkpointPath) const {
    if (!experiment) return false;
    
    std::ofstream file(checkpointPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "ExperimentCheckpoint:" << std::endl;
    file << "  Name: " << experiment->getName() << std::endl;
    file << "  Seed: " << experiment->getSeed() << std::endl;
    file << "  Metrics:" << std::endl;
    
    auto metrics = experiment->getMetrics();
    for (const auto& pair : metrics) {
        file << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    return true;
}

std::shared_ptr<Experiment> ExperimentRunner::loadExperimentCheckpoint(const std::string& checkpointPath) {
    // In real implementation, would parse checkpoint file
    auto experiment = std::make_shared<Experiment>("LoadedCheckpoint");
    experiment->setSeed(42);
    return experiment;
}

bool ExperimentRunner::saveBrainCheckpoint(std::shared_ptr<Brain> brain,
                                          const std::string& checkpointPath) const {
    if (!brain) return false;
    
    std::ofstream file(checkpointPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "BrainCheckpoint:" << std::endl;
    file << "  NeuronCount: " << brain->getTotalNeuronCount() << std::endl;
    file << "  SynapseCount: " << brain->getTotalSynapseCount() << std::endl;
    file << "  FiringCount: " << brain->getFiringNeuronCount() << std::endl;
    file << "  AverageFiringRate: " << brain->getAverageFiringRate() << std::endl;
    
    return true;
}

std::shared_ptr<Brain> ExperimentRunner::loadBrainCheckpoint(const std::string& checkpointPath) {
    // In real implementation, would load from checkpoint
    return nullptr;
}

bool ExperimentRunner::saveEnvironmentCheckpoint(std::shared_ptr<Environment> environment,
                                                 const std::string& checkpointPath) const {
    if (!environment) return false;
    
    std::ofstream file(checkpointPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "EnvironmentCheckpoint:" << std::endl;
    file << "  Name: " << environment->getName() << std::endl;
    file << "  ActionSpaceSize: " << environment->getActionSpaceSize() << std::endl;
    file << "  ObservationSpaceSize: " << environment->getObservationSpaceSize() << std::endl;
    
    return true;
}

std::shared_ptr<Environment> ExperimentRunner::loadEnvironmentCheckpoint(const std::string& checkpointPath) {
    // In real implementation, would load from checkpoint
    return nullptr;
}

// ========== ADVANCED EXPERIMENT MANAGEMENT ==========

std::vector<ExperimentResult> ExperimentRunner::executeExperimentRun(const ExperimentRunConfig& config) {
    // Create experiments if they don't exist
    std::vector<std::shared_ptr<Experiment>> experiments;
    for (const auto& name : config.experimentNames) {
        auto exp = getExperiment(name);
        if (!exp) {
            exp = createExperiment(name);
        }
        experiments.push_back(exp);
    }
    
    // Set up templates if needed
    std::shared_ptr<Brain> brainTemplate = nullptr;
    std::shared_ptr<Environment> envTemplate = nullptr;
    
    if (config.cloningConfig.enableBrainCloning) {
        brainTemplate = std::make_shared<Brain>(std::make_shared<Config>());
        brainTemplate->initialize();
        setBrainTemplate("run_default", brainTemplate);
    }
    
    if (config.cloningConfig.enableEnvironmentCloning) {
        envTemplate = std::make_shared<GridWorldEnvironment>();
        setEnvironmentTemplate("run_default", envTemplate);
    }
    
    // Configure batch manager
    BatchExperimentManager::BatchConfig batchConfig;
    batchConfig.maxConcurrentExperiments = config.batchConfig.maxConcurrentExperiments;
    batchConfig.maxStepsPerExperiment = config.batchConfig.maxStepsPerExperiment;
    batchConfig.timeoutSeconds = config.batchConfig.timeoutSeconds;
    batchConfig.checkpointEnabled = config.batchConfig.checkpointEnabled;
    batchConfig.checkpointPath = config.batchConfig.checkpointPath;
    batchConfig.pauseOnError = config.batchConfig.pauseOnError;
    
    configureBatchManager(batchConfig);
    
    // Execute batch
    return executeBatchExperiments(experiments, brainTemplate, envTemplate, batchConfig);
}

// ========== UTILITY FUNCTIONALITY ==========

std::string ExperimentRunner::getVersion() {
    return "1.0.0";
}

void ExperimentRunner::reset() {
    std::lock_guard<std::mutex> lock(pImpl->statusMutex_);
    
    clearExperiments();
    pImpl->brainTemplates.clear();
    pImpl->environmentTemplates.clear();
    pImpl->batchConfig = BatchExperimentManager::BatchConfig();
    pImpl->cloningConfig = CloningConfig();
    pImpl->runnerName = "";
    pImpl->loggingEnabled = true;
    
    batchManager_.reset();
}

BrainClone::CloneStats ExperimentRunner::getBrainCloneStats() {
    return BrainClone::getClonePerformanceMetrics();
}

EnvironmentClone::CloneStats ExperimentRunner::getEnvironmentCloneStats() {
    return EnvironmentClone::getClonePerformanceMetrics();
}

} // namespace nlm
