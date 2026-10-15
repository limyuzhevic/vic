#include "ExperimentManager.hpp"

namespace nlm {

struct ExperimentManager::Impl {
    // Experiment storage
    std::map<std::string, ExperimentConfig> experimentConfigs;
    std::map<std::string, std::shared_ptr<Experiment>> experimentResults;
    std::map<std::string, std::shared_ptr<Experiment>> completedExperiments;
    
    // Templates
    std::map<std::string, ExperimentConfig> templates;
    
    // Experiment runner
    std::shared_ptr<ExperimentRunner> runner;
    
    // Progress tracking
    std::map<std::string, float> experimentProgress;
    std::map<std::string, std::chrono::system_clock::time_point> scheduledExperiments;
    
    // Automation settings
    bool autoRunEnabled = false;
    size_t maxConcurrentExperiments = 4;
    size_t maxCompletedExperimentsToKeep = 100;
    uint64_t globalSeed = 42;
    
    Impl() {
        runner = std::make_shared<ExperimentRunner>();
    }
};

ExperimentManager::ExperimentManager() : pImpl(std::make_unique<Impl>()) {}

ExperimentManager::~ExperimentManager() = default;

bool ExperimentManager::createExperiment(const std::string& name, const ExperimentConfig& config) {
    if (experimentConfigs.find(name) != experimentConfigs.end()) {
        return false; // Experiment already exists
    }
    
    ExperimentConfig newConfig = config;
    newConfig.name = name;
    experimentConfigs[name] = newConfig;
    
    // Create empty experiment result
    auto experiment = std::make_shared<Experiment>(name);
    experiment->setSeed(pImpl->globalSeed++);
    experimentResults[name] = experiment;
    
    return true;
}

bool ExperimentManager::createExperimentFromTemplate(const std::string& name, 
                                                     const std::string& templateName) {
    if (pImpl->templates.find(templateName) == pImpl->templates.end()) {
        return false;
    }
    
    ExperimentConfig config = pImpl->templates[templateName];
    config.name = name;
    
    return createExperiment(name, config);
}

bool ExperimentManager::runExperiment(const std::string& name, std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment, size_t maxSteps) {
    if (experimentConfigs.find(name) == experimentConfigs.end()) {
        return false;
    }
    
    auto it = experimentResults.find(name);
    if (it == experimentResults.end()) {
        return false;
    }
    
    // Set experiment parameters
    const auto& config = experimentConfigs[name];
    for (const auto& param : config.parameters) {
        it->second->setConfigValue(param.first, param.second);
    }
    
    // Run experiment
    bool success = pImpl->runner->runExperiment(it->second, brain, environment, maxSteps);
    
    if (success) {
        // Move from active to completed
        completedExperiments[name] = it->second;
        experimentResults.erase(it);
        pImpl->experimentProgress[name] = 1.0f;
    }
    
    return success;
}

bool ExperimentManager::runExperiment(const std::string& name) {
    // For this implementation, we'll just return false as we'd need to create brain/environment
    // In a full implementation, this would use default configurations
    return false;
}

std::vector<std::string> ExperimentManager::runBatch(const std::vector<std::string>& experimentNames,
                                                     std::shared_ptr<Brain> brainTemplate,
                                                     std::shared_ptr<Environment> environmentTemplate,
                                                     size_t maxSteps) {
    std::vector<std::string> successfulRuns;
    
    for (const auto& expName : experimentNames) {
        if (runExperiment(expName, brainTemplate, environmentTemplate, maxSteps)) {
            successfulRuns.push_back(expName);
        }
    }
    
    return successfulRuns;
}

bool ExperimentManager::createTemplate(const std::string& name, const ExperimentConfig& config) {
    pImpl->templates[name] = config;
    return true;
}

bool ExperimentManager::updateTemplate(const std::string& name, const ExperimentConfig& config) {
    if (pImpl->templates.find(name) == pImpl->templates.end()) {
        return false;
    }
    
    pImpl->templates[name] = config;
    return true;
}

bool ExperimentManager::deleteTemplate(const std::string& name) {
    auto it = pImpl->templates.find(name);
    if (it == pImpl->templates.end()) {
        return false;
    }
    pImpl->templates.erase(it);
    return true;
}

const std::map<std::string, ExperimentConfig>& ExperimentManager::getTemplates() const {
    return pImpl->templates;
}

bool ExperimentManager::storeResult(const std::string& experimentName, 
                                   std::shared_ptr<Experiment> experiment) {
    if (experimentConfigs.find(experimentName) == experimentConfigs.end()) {
        return false;
    }
    
    completedExperiments[experimentName] = experiment;
    return true;
}

std::shared_ptr<Experiment> ExperimentManager::getResult(const std::string& experimentName) const {
    auto it = completedExperiments.find(experimentName);
    if (it != completedExperiments.end()) {
        return it->second;
    }
    
    it = experimentResults.find(experimentName);
    if (it != experimentResults.end()) {
        return it->second;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<Experiment>> ExperimentManager::getAllResults() const {
    std::vector<std::shared_ptr<Experiment>> allResults;
    
    for (const auto& pair : completedExperiments) {
        allResults.push_back(pair.second);
    }
    
    for (const auto& pair : experimentResults) {
        allResults.push_back(pair.second);
    }
    
    return allResults;
}

bool ExperimentManager::setParameter(const std::string& experimentName, 
                                     const std::string& key, const std::string& value) {
    auto it = experimentConfigs.find(experimentName);
    if (it == experimentConfigs.end()) {
        return false;
    }
    
    it->second.parameters[key] = value;
    return true;
}

std::string ExperimentManager::getParameter(const std::string& experimentName,
                                            const std::string& key) const {
    auto it = experimentConfigs.find(experimentName);
    if (it == experimentConfigs.end()) {
        return "";
    }
    
    auto paramIt = it->second.parameters.find(key);
    if (paramIt == it->second.parameters.end()) {
        return "";
    }
    
    return paramIt->second;
}

std::map<std::string, std::string> ExperimentManager::getExperimentParameters(
    const std::string& experimentName) const {
    auto it = experimentConfigs.find(experimentName);
    if (it == experimentConfigs.end()) {
        return {};
    }
    
    return it->second.parameters;
}

bool ExperimentManager::compareExperiments(const std::string& exp1Name, 
                                           const std::string& exp2Name,
                                           std::map<std::string, double>& comparisonResults) const {
    auto exp1 = getResult(exp1Name);
    auto exp2 = getResult(exp2Name);
    
    if (!exp1 || !exp2) {
        return false;
    }
    
    // Compare metrics
    const auto& metrics1 = exp1->getMetrics();
    const auto& metrics2 = exp2->getMetrics();
    
    for (const auto& metric1 : metrics1) {
        auto metric2It = std::find_if(metrics2.begin(), metrics2.end(),
            [&metric1](const auto& m) { return m.first == metric1.first; });
        
        if (metric2It != metrics2.end()) {
            double difference = metric1.second - metric2It->second;
            double relativeDifference = (metric1.second != 0) ? difference / metric1.second : 0;
            comparisonResults[metric1.first + "_difference"] = difference;
            comparisonResults[metric1.first + "_relative_difference"] = relativeDifference;
        }
    }
    
    return true;
}

bool ExperimentManager::scheduleExperiment(const std::string& experimentName,
                                           const std::chrono::system_clock::time_point& scheduledTime) {
    if (experimentConfigs.find(experimentName) == experimentConfigs.end()) {
        return false;
    }
    
    pImpl->scheduledExperiments[experimentName] = scheduledTime;
    return true;
}

bool ExperimentManager::runScheduledExperiments() {
    bool anyRun = false;
    auto now = std::chrono::system_clock::now();
    
    for (auto it = pImpl->scheduledExperiments.begin(); it != pImpl->scheduledExperiments.end(); ) {
        if (now >= it->second) {
            // Run experiment (placeholder - would need brain/environment)
            // For now, just remove from schedule
            it = pImpl->scheduledExperiments.erase(it);
            anyRun = true;
        } else {
            ++it;
        }
    }
    
    return anyRun;
}

bool ExperimentManager::saveManagerConfig(const std::string& filepath) const {
    // TODO: Implement config saving
    return false;
}

bool ExperimentManager::loadManagerConfig(const std::string& filepath) {
    // TODO: Implement config loading
    return false;
}

std::string ExperimentManager::generateResultSummary(const std::string& experimentName) const {
    auto result = getResult(experimentName);
    if (!result) {
        return "Experiment not found: " + experimentName;
    }
    
    std::stringstream ss;
    ss << "Experiment: " << result->getName() << "\n";
    ss << "Metrics: ";
    for (const auto& metric : result->getMetrics()) {
        ss << metric.first << "=" << metric.second << "; ";
    }
    ss << "\n";
    
    return ss.str();
}

std::string ExperimentManager::generateComparativeAnalysis(
    const std::vector<std::string>& experimentNames) const {
    std::stringstream ss;
    ss << "Comparative Analysis:\n";
    
    for (const auto& name : experimentNames) {
        ss << "- " << name << ": " << generateResultSummary(name) << "\n";
    }
    
    return ss.str();
}

bool ExperimentManager::validateExperimentConfig(const ExperimentConfig& config) const {
    if (config.name.empty()) {
        return false;
    }
    
    // Check for parameter conflicts
    for (const auto& param : config.parameters) {
        if (param.first.empty() || param.second.empty()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ExperimentManager::getAllExperimentNames() const {
    std::vector<std::string> names;
    
    for (const auto& pair : experimentConfigs) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> ExperimentManager::getAllExperimentNames(bool includeCompleted) const {
    std::vector<std::string> names;
    
    if (includeCompleted) {
        for (const auto& pair : completedExperiments) {
            names.push_back(pair.first);
        }
    }
    
    for (const auto& pair : experimentConfigs) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool ExperimentManager::clearCompletedExperiments(size_t maxKeep) {
    if (completedExperiments.size() <= maxKeep) {
        return true;
    }
    
    size_t toRemove = completedExperiments.size() - maxKeep;
    auto it = completedExperiments.begin();
    
    for (size_t i = 0; i < toRemove && it != completedExperiments.end(); ++i, ++it) {
        it = completedExperiments.erase(it);
    }
    
    return true;
}

float ExperimentManager::getExperimentProgress(const std::string& experimentName) const {
    auto it = pImpl->experimentProgress.find(experimentName);
    if (it == pImpl->experimentProgress.end()) {
        return 0.0f;
    }
    return it->second;
}

std::map<std::string, float> ExperimentManager::getBatchProgress(
    const std::vector<std::string>& experimentNames) const {
    std::map<std::string, float> progress;
    
    for (const auto& name : experimentNames) {
        progress[name] = getExperimentProgress(name);
    }
    
    return progress;
}

} // namespace nlm
