#include "experiment_manager.hpp"
#include "enhanced_experiments.hpp"
#include "experiment_factory.hpp"
#include "parameter_sweep.hpp"
#include "result_analyzer.hpp"
#include "experiment_scheduler.hpp"
#include "reproducibility_manager.hpp"
#include "experiment_utils.hpp"
#include "parameter_parser.hpp"
#include "result_exporter.hpp"
#include "legacy_experiment.hpp"
#include "legacy_runner.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

namespace nlm {

ExperimentManager::ExperimentManager(std::shared_ptr<Config> config) : config_(config) {
    initializeComponents();
    startTime_ = std::chrono::high_resolution_clock::now();
    
    Logger::getInstance().log("Experiment Manager initialized", LogLevel::Info);
}

void ExperimentManager::initializeComponents() {
    experimentFactory_ = std::make_unique<ExperimentFactory>(config_);
    parameterSweepManager_ = std::make_unique<ParameterSweepManager>(config_);
    resultAnalyzer_ = std::make_unique<ResultAnalyzer>();
    experimentScheduler_ = std::make_unique<ExperimentScheduler>(config_);
    reproducibilityManager_ = std::make_unique<ReproducibilityManager>(config_);
    experimentUtils_ = std::make_unique<ExperimentUtils>();
    parameterParser_ = std::make_unique<ParameterParser>();
    resultExporter_ = std::make_unique<ResultExporter>();
    legacyRunner_ = std::make_unique<LegacyExperimentRunner>();
    
    // Load experiment configurations
    loadExperimentConfigurations();
}

bool ExperimentManager::loadExperimentConfigurations() {
    std::string configFile = "experiment_config.json";
    if (std::filesystem::exists(configFile)) {
        try {
            std::ifstream file(configFile);
            if (!file.is_open()) {
                return false;
            }
            
            json j;
            file >> j;
            
            // Load experiment templates
            if (j.contains("experiment_templates")) {
                // Would load templates from config
            }
            
            // Load default parameters
            if (j.contains("default_parameters")) {
                // Would set default parameters
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error loading experiment configurations: " << e.what() << std::endl;
            return false;
        }
    }
    
    return true;
}

std::shared_ptr<BaseExperiment> ExperimentManager::createExperiment(const std::string& type, const std::string& name) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = experimentFactory_->createExperiment(type, name);
    if (experiment) {
        experiments_.push_back(experiment);
        
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_.totalExperimentsCreated++;
        
        Logger::getInstance().log("Created experiment: " + name + " (type: " + type + ")", LogLevel::Info);
    }
    
    return experiment;
}

std::shared_ptr<BaseExperiment> ExperimentManager::createExperimentFromTemplate(
    const std::string& templateName,
    const std::map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = experimentFactory_->createExperimentFromTemplate(templateName, parameters);
    if (experiment) {
        experiments_.push_back(experiment);
        
        std::lock_guard<std::mutex> statsLock(statsMutex_);
        stats_.totalExperimentsCreated++;
        
        Logger::getInstance().log("Created experiment from template: " + templateName, LogLevel::Info);
    }
    
    return experiment;
}

std::vector<std::string> ExperimentManager::getAvailableExperimentTypes() const {
    return experimentFactory_->getAvailableExperimentTypes();
}

std::vector<std::string> ExperimentManager::getExperimentTemplates() const {
    // In a real implementation, this would return template names
    return {"plasticity_default", "learning_curve_default", "network_connectivity_default", 
            "developmental_default", "neuromodulation_default"};
}

bool ExperimentManager::runExperiment(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return false;
    }
    
    return experimentScheduler_->scheduleExperiment(experiment);
}

bool ExperimentManager::runExperimentToCompletion(const std::string& experimentName, size_t maxSteps) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return false;
    }
    
    experiment->setTotalSteps(maxSteps);
    
    return experiment->run();
}

bool ExperimentManager::pauseExperiment(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return false;
    }
    
    experiment->setState(BaseExperiment::State::Paused);
    return true;
}

bool ExperimentManager::resumeExperiment(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return false;
    }
    
    return experiment->resume();
}

bool ExperimentManager::stopExperiment(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return false;
    }
    
    experiment->setState(BaseExperiment::State::Failed);
    return true;
}

bool ExperimentManager::deleteExperiment(const std::string& experimentName) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    for (auto it = experiments_.begin(); it != experiments_.end(); ++it) {
        if ((*it)->getName() == experimentName) {
            experiments_.erase(it);
            Logger::getInstance().log("Deleted experiment: " + experimentName, LogLevel::Info);
            return true;
        }
    }
    
    return false;
}

bool ExperimentManager::renameExperiment(const std::string& oldName, const std::string& newName) {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    for (auto& experiment : experiments_) {
        if (experiment->getName() == oldName) {
            experiment->setName(newName);
            Logger::getInstance().log("Renamed experiment: " + oldName + " -> " + newName, LogLevel::Info);
            return true;
        }
    }
    
    return false;
}

std::shared_ptr<BaseExperiment> ExperimentManager::getExperiment(const std::string& experimentName) const {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    
    for (const auto& experiment : experiments_) {
        if (experiment->getName() == experimentName) {
            return experiment;
        }
    }
    
    return nullptr;
}

const std::vector<std::shared_ptr<BaseExperiment>>& ExperimentManager::getAllExperiments() const {
    return experiments_;
}

void ExperimentManager::clearExperiments() {
    std::lock_guard<std::mutex> lock(experimentsMutex_);
    experiments_.clear();
    Logger::getInstance().log("Cleared all experiments", LogLevel::Info);
}

std::vector<SweepResult> ExperimentManager::runParameterSweep(const std::string& templateName,
                                                             const std::map<std::string, std::vector<std::string>>& parameterMatrix,
                                                             size_t numIterations) {
    return parameterSweepManager_->runSweep(templateName, parameterMatrix, numIterations);
}

OptimizationResult ExperimentManager::optimizeParameters(const std::string& templateName,
                                                         const std::map<std::string, std::vector<std::string>>& parameterSpace,
                                                         const std::string& objective) {
    return parameterSweepManager_->optimizeParameters(templateName, parameterSpace, objective);
}

ResultAnalyzer::ExperimentStatistics ExperimentManager::analyzeExperiment(const std::string& experimentName) const {
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return ResultAnalyzer::ExperimentStatistics(); // Empty stats
    }
    
    return resultAnalyzer_->calculateStatistics({experiment->getMetric("performance_score")});
}

ResultAnalyzer::ComparisonResult ExperimentManager::compareExperiments(const std::vector<std::string>& experimentNames) const {
    std::vector<double> results1, results2;
    
    // Extract performance metrics from experiments
    for (const auto& experimentName : experimentNames) {
        auto experiment = getExperiment(experimentName);
        if (experiment) {
            results1.push_back(experiment->calculatePerformanceScore());
        }
    }
    
    if (results1.size() >= 2) {
        return resultAnalyzer_->compareExperiments(results1, results1);
    }
    
    return ResultAnalyzer::ComparisonResult(); // Empty result
}

std::string ExperimentManager::generateReport(const std::vector<std::string>& experimentNames, const std::string& format) const {
    std::vector<std::shared_ptr<BaseExperiment>> experiments;
    
    for (const auto& experimentName : experimentNames) {
        auto experiment = getExperiment(experimentName);
        if (experiment) {
            experiments.push_back(experiment);
        }
    }
    
    if (format == "csv") {
        return resultAnalyzer_->generateCSV(experiments);
    } else if (format == "json") {
        return resultAnalyzer_->generateJSON(experiments);
    } else if (format == "markdown") {
        std::stringstream markdown;
        markdown << "# Experiment Results Report" << std::endl;
        markdown << std::endl;
        
        for (const auto& experiment : experiments) {
            markdown << "## " << experiment->getName() << " (" << experiment->getExperimentType() << ")" << std::endl;
            markdown << std::endl;
            markdown << "- **Progress**: " << (experiment->getProgress() * 100) << "%" << std::endl;
            markdown << "- **Performance Score**: " << experiment->calculatePerformanceScore() << std::endl;
            markdown << std::endl;
        }
        
        return markdown.str();
    }
    
    return "Unsupported format: " + format;
}

void ExperimentManager::startScheduler() {
    experimentScheduler_->start();
}

void ExperimentManager::pauseScheduler() {
    experimentScheduler_->pause();
}

void ExperimentManager::stopScheduler() {
    experimentScheduler_->stop();
}

bool ExperimentManager::isSchedulerRunning() const {
    return experimentScheduler_->isRunning();
}

ExperimentScheduler::QueueStatus ExperimentManager::getSchedulerStatus() const {
    return experimentScheduler_->getQueueStatus();
}

std::shared_ptr<BaseExperiment> ExperimentManager::getCurrentExperiment() const {
    return experimentScheduler_->getCurrentExperiment();
}

std::vector<std::shared_ptr<BaseExperiment>> ExperimentManager::getCompletedExperiments() const {
    return experimentScheduler_->getCompletedExperiments();
}

ReproducibilityManager::ExperimentSnapshot ExperimentManager::createSnapshot(const std::string& experimentName) {
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return ReproducibilityManager::ExperimentSnapshot(); // Empty snapshot
    }
    
    return reproducibilityManager_->createSnapshot(experiment);
}

bool ExperimentManager::restoreSnapshot(const ReproducibilityManager::ExperimentSnapshot& snapshot) {
    std::shared_ptr<BaseExperiment> experiment;
    if (reproducibilityManager_->restoreSnapshot(snapshot, experiment) && experiment) {
        std::lock_guard<std::mutex> lock(experimentsMutex_);
        experiments_.push_back(experiment);
        return true;
    }
    
    return false;
}

std::string ExperimentManager::createVersion(const std::string& experimentName, const std::string& description) {
    return reproducibilityManager_->createVersion(experimentName, description);
}

ReproducibilityManager::VersionComparison ExperimentManager::compareVersions(const std::string& experimentName,
                                                                             const std::string& version1,
                                                                             const std::string& version2) {
    return reproducibilityManager_->compareVersions(version1, version2);
}

std::vector<std::string> ExperimentManager::getVersionHistory(const std::string& experimentName) const {
    return reproducibilityManager_->getVersionHistory(experimentName);
}

bool ExperimentManager::exportExperimentResults(const std::string& experimentName, const std::string& filepath, const std::string& format) {
    auto experiment = getExperiment(experimentName);
    if (!experiment) {
        return false;
    }
    
    return resultExporter_->exportToCSV(experiment, filepath);
}

bool ExperimentManager::exportBatchResults(const std::vector<std::string>& experimentNames, const std::string& outputDirectory, const std::string& format) {
    std::vector<std::shared_ptr<BaseExperiment>> experiments;
    
    for (const auto& experimentName : experimentNames) {
        auto experiment = getExperiment(experimentName);
        if (experiment) {
            experiments.push_back(experiment);
        }
    }
    
    return resultExporter_->exportBatch(experiments, outputDirectory, format);
}

ExperimentManager::ManagerStatistics ExperimentManager::getStatistics() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    return stats_;
}

std::shared_ptr<LegacyExperiment> ExperimentManager::createLegacyExperiment(const std::string& name) {
    return legacyRunner_->createExperiment(name);
}

bool ExperimentManager::runLegacyExperiment(std::shared_ptr<LegacyExperiment> experiment,
                                            std::shared_ptr<Brain> brain,
                                            std::shared_ptr<Environment> environment,
                                            SimulationStep maxSteps) {
    return legacyRunner_->runExperimentLegacy(experiment, brain, environment, maxSteps);
}

bool ExperimentManager::migrateLegacyExperiments() {
    // Would implement legacy experiment migration
    Logger::getInstance().log("Legacy experiment migration started", LogLevel::Info);
    
    // Example migration logic
    auto legacyExps = legacyRunner_->getLegacyExperimentNames();
    for (const auto& expName : legacyExps) {
        auto legacyExp = legacyRunner_->getExperiment(expName);
        if (legacyExp) {
            // Convert legacy experiment to new format
            auto newExp = createLegacyExperiment(expName);
            if (newExp) {
                Logger::getInstance().log("Migrated legacy experiment: " + expName, LogLevel::Info);
            }
        }
    }
    
    Logger::getInstance().log("Legacy experiment migration completed", LogLevel::Info);
    return true;
}

std::vector<std::string> ExperimentManager::getLegacyExperimentNames() const {
    return legacyRunner_->getLegacyExperimentNames();
}

void ExperimentManager::cleanupCompletedExperiments() {
    experimentScheduler_->cleanupCompletedExperiments();
}

} // namespace nlm