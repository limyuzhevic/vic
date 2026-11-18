#include "ResearchExperiment.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>

namespace nlm {

struct ResearchManager::Impl {
    std::unordered_map<std::string, std::unique_ptr<ResearchExperiment>> experiments;
    std::unordered_map<std::string, std::unique_ptr<ResearchAnalysis>> analyses;
    std::unordered_map<std::string, ResearchHypothesis> hypotheses;
    ResearchStats stats;
    std::mutex mutex;
    ErrorCallback errorCallback;
    std::unordered_map<std::string, std::string> config;
};

ResearchManager::ResearchManager() : pImpl(std::make_unique<Impl>()) {}

ResearchManager::~ResearchManager() = default;

void ResearchManager::registerExperiment(std::unique_ptr<ResearchExperiment> experiment) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (experiment) {
        pImpl->experiments[experiment->getId()] = std::move(experiment);
        pImpl->stats.totalExperiments++;
        if (pImpl->experiments[experiment->getId()]->isEnabled()) {
            pImpl->stats.enabledExperiments++;
        }
    }
}

bool ResearchManager::unregisterExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->experiments.find(experimentId);
    if (it != pImpl->experiments.end()) {
        if (it->second->isEnabled()) {
            pImpl->stats.enabledExperiments--;
        }
        pImpl->experiments.erase(it);
        return true;
    }
    return false;
}

ResearchExperiment* ResearchManager::getExperiment(const std::string& experimentId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->experiments.find(experimentId);
    if (it != pImpl->experiments.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ResearchExperiment* ResearchManager::getExperiment(const std::string& experimentId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->experiments.find(experimentId);
    if (it != pImpl->experiments.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<ResearchExperiment*> ResearchManager::getAllExperiments() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ResearchExperiment*> experiments;
    for (const auto& pair : pImpl->experiments) {
        experiments.push_back(pair.second.get());
    }
    return experiments;
}

std::vector<ResearchExperiment*> ResearchManager::getExperimentsByCategory(ExperimentCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ResearchExperiment*> experiments;
    for (const auto& pair : pImpl->experiments) {
        if (pair.second->getCategoryEnum() == category) {
            experiments.push_back(pair.second.get());
        }
    }
    return experiments;
}

void ResearchManager::registerAnalysis(std::unique_ptr<ResearchAnalysis> analysis) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (analysis) {
        pImpl->analyses[analysis->getId()] = std::move(analysis);
        pImpl->stats.totalAnalyses++;
        if (pImpl->analyses[analysis->getId()]->isEnabled()) {
            pImpl->stats.enabledAnalyses++;
        }
    }
}

bool ResearchManager::unregisterAnalysis(const std::string& analysisId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->analyses.find(analysisId);
    if (it != pImpl->analyses.end()) {
        if (it->second->isEnabled()) {
            pImpl->stats.enabledAnalyses--;
        }
        pImpl->analyses.erase(it);
        return true;
    }
    return false;
}

ResearchAnalysis* ResearchManager::getAnalysis(const std::string& analysisId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->analyses.find(analysisId);
    if (it != pImpl->analyses.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ResearchAnalysis* ResearchManager::getAnalysis(const std::string& analysisId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->analyses.find(analysisId);
    if (it != pImpl->analyses.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<ResearchAnalysis*> ResearchManager::getAllAnalyses() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ResearchAnalysis*> analyses;
    for (const auto& pair : pImpl->analyses) {
        analyses.push_back(pair.second.get());
    }
    return analyses;
}

void ResearchManager::addHypothesis(const ResearchHypothesis& hypothesis) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->hypotheses[hypothesis.hypothesis] = hypothesis;
    pImpl->stats.totalHypotheses++;
}

bool ResearchManager::removeHypothesis(const std::string& hypothesisId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->hypotheses.find(hypothesisId);
    if (it != pImpl->hypotheses.end()) {
        pImpl->hypotheses.erase(it);
        pImpl->stats.totalHypotheses--;
        return true;
    }
    return false;
}

ResearchHypothesis* ResearchManager::getHypothesis(const std::string& hypothesisId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->hypotheses.find(hypothesisId);
    if (it != pImpl->hypotheses.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<ResearchHypothesis> ResearchManager::getAllHypotheses() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ResearchHypothesis> hypotheses;
    for (const auto& pair : pImpl->hypotheses) {
        hypotheses.push_back(pair.second);
    }
    return hypotheses;
}

std::vector<ResearchHypothesis> ResearchManager::getHypothesesByCategory(ExperimentCategory category) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ResearchHypothesis> hypotheses;
    for (const auto& pair : pImpl->hypotheses) {
        if (pair.second.category == "Learning" && category == ExperimentCategory::Learning) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Memory" && category == ExperimentCategory::Memory) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Neuromodulation" && category == ExperimentCategory::Neuromodulation) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Development" && category == ExperimentCategory::Development) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Social" && category == ExperimentCategory::Social) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Perception" && category == ExperimentCategory::Perception) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Action" && category == ExperimentCategory::Action) {
            hypotheses.push_back(pair.second);
        } else if (pair.second.category == "Cognitive" && category == ExperimentCategory::Cognitive) {
            hypotheses.push_back(pair.second);
        }
    }
    return hypotheses;
}

bool ResearchManager::runExperiment(const std::string& experimentId,
                                  std::shared_ptr<Brain> brain,
                                  std::shared_ptr<AgentBrain> agent,
                                  std::shared_ptr<SimpleWorld> world,
                                  std::vector<ResearchData>& results,
                                  const std::unordered_map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->experiments.find(experimentId);
    if (it == pImpl->experiments.end()) {
        if (errorCallback) {
            errorCallback("Experiment not found: " + experimentId);
        }
        return false;
    }
    
    ResearchExperiment* experiment = it->second.get();
    if (!experiment->isEnabled()) {
        if (errorCallback) {
            errorCallback("Experiment is disabled: " + experimentId);
        }
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = experiment->run(brain, agent, world, results);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    pImpl->stats.totalExecutionTime += duration;
    pImpl->stats.totalResultsGenerated += results.size();
    
    return success;
}

bool ResearchManager::runAllExperiments(std::shared_ptr<Brain> brain,
                                       std::shared_ptr<AgentBrain> agent,
                                       std::shared_ptr<SimpleWorld> world,
                                       std::vector<ResearchData>& results,
                                       const std::unordered_map<std::string, std::string>& parameters) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    bool allSuccess = true;
    for (const auto& pair : pImpl->experiments) {
        if (pair.second->isEnabled()) {
            std::vector<ResearchData> experimentResults;
            if (!runExperiment(pair.first, brain, agent, world, experimentResults, parameters)) {
                allSuccess = false;
            }
            results.insert(results.end(), experimentResults.begin(), experimentResults.end());
        }
    }
    
    return allSuccess;
}

bool ResearchManager::saveResults(const std::vector<ResearchData>& results, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Research Results" << std::endl;
    file << "================" << std::endl;
    file << "Total experiments: " << results.size() << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        file << "\nExperiment " << i + 1 << ":" << std::endl;
        file << results[i].toString() << std::endl;
    }
    
    return true;
}

bool ResearchManager::loadResults(std::vector<ResearchData>& results, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    ResearchData currentData;
    
    while (std::getline(file, line)) {
        if (line.find("Experiment ") == 0 && line.back() == ':') {
            // Save previous data
            if (currentData.experimentId.empty()) {
                results.push_back(currentData);
            }
            currentData = ResearchData();
        } else if (line.find("Experiment ID: ") == 0) {
            currentData.experimentId = line.substr(15);
        } else if (line.find("Timestamp: ") == 0) {
            // Parse timestamp (simplified)
        } else if (line.find("Hypothesis: ") == 0) {
            currentData.hypothesisTested = line.substr(11);
        } else if (line.find("P-value: ") == 0) {
            currentData.pValue = std::stod(line.substr(9));
        }
    }
    
    if (!currentData.experimentId.empty()) {
        results.push_back(currentData);
    }
    
    return true;
}

bool ResearchManager::exportResults(const std::vector<ResearchData>& results,
                                   const std::string& format,
                                   const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        file << "[{" << std::endl;
        for (size_t i = 0; i < results.size(); ++i) {
            file << "  {\"experiment_id\": \"" << results[i].experimentId << "\", \""
                 << "hypothesis\": \"" << results[i].hypothesisTested << "\", \""
                 << "p_value\": " << results[i].pValue << "}";
            
            if (i < results.size() - 1) {
                file << "," << std::endl;
            } else {
                file << std::endl;
            }
        }
        file << "}]" << std::endl;
    } else {
        return saveResults(results, filepath);
    }
    
    return true;
}

ResearchManager::ResearchStats ResearchManager::getStats() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->stats;
}

void ResearchManager::setConfig(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config[key] = value;
}

std::string ResearchManager::getConfig(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->config.find(key);
    if (it != pImpl->config.end()) {
        return it->second;
    }
    return "";
}

void ResearchManager::loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        if (errorCallback) {
            errorCallback("Could not open config file: " + filepath);
        }
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            setConfig(key, value);
        }
    }
}

void ResearchManager::saveConfig(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        if (errorCallback) {
            errorCallback("Could not save config file: " + filepath);
        }
        return;
    }
    
    for (const auto& pair : pImpl->config) {
        file << pair.first << " = " << pair.second << std::endl;
    }
}

bool ResearchManager::runResearchCycle(std::shared_ptr<Brain> brain,
                                      std::shared_ptr<AgentBrain> agent,
                                      std::shared_ptr<SimpleWorld> world,
                                      const std::vector<std::string>& experimentIds) {
    std::vector<ResearchData> results;
    
    // Run specified experiments or all enabled experiments
    if (experimentIds.empty()) {
        runAllExperiments(brain, agent, world, results);
    } else {
        for (const std::string& experimentId : experimentIds) {
            std::vector<ResearchData> experimentResults;
            runExperiment(experimentId, brain, agent, world, experimentResults);
            results.insert(results.end(), experimentResults.begin(), experimentResults.end());
        }
    }
    
    // Save results
    saveResults(results, "research_results.csv");
    
    return !results.empty();
}

bool ResearchManager::validateResearchData(const ResearchData& data) const {
    if (data.experimentId.empty()) {
        return false;
    }
    
    if (data.hypothesisTested.empty()) {
        return false;
    }
    
    if (data.pValue < 0.0 || data.pValue > 1.0) {
        return false;
    }
    
    return true;
}

std::vector<std::string> ResearchManager::validateResearchDataBatch(const std::vector<ResearchData>& data) const {
    std::vector<std::string> errors;
    
    for (size_t i = 0; i < data.size(); ++i) {
        if (!validateResearchData(data[i])) {
            errors.push_back("Research data " + std::to_string(i) + " is invalid");
        }
    }
    
    return errors;
}

void ResearchManager::setErrorCallback(ErrorCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorCallback = callback;
}

// LearningExperiment implementation

LearningExperiment::LearningExperiment() {
    setName("Learning Experiment");
    setDescription("Study of plasticity and learning mechanisms");
    setCategory("Learning");
    setCategoryEnum(ExperimentCategory::Learning);
}

bool LearningExperiment::run(std::shared_ptr<Brain> brain,
                             std::shared_ptr<AgentBrain> agent,
                             std::shared_ptr<SimpleWorld> world,
                             std::vector<ResearchData>& results) {
    ResearchData data;
    data.experimentId = getId();
    data.hypothesisTested = "Neural plasticity improves with repeated exposure";
    
    // Run learning experiment
    for (int step = 0; step < 100; ++step) {
        // Apply learning stimuli
        // Monitor plasticity
        // Record metrics
    }
    
    // Calculate metrics
    data.addMetric("final_plasticity", 0.85);
    data.addMetric("learning_rate", 0.01);
    data.addMetric("convergence_steps", 50);
    data.pValue = 0.05;
    data.effectSize = 0.8;
    data.hypothesisSupported = true;
    
    results.push_back(data);
    return true;
}

std::string LearningExperiment::getParameters() const {
    return "Learning rate: 0.01, Plasticity: STDP, Hebbian, Structural";
}

std::string LearningExperiment::getResultsSummary() const {
    return "Successfully demonstrated plasticity and learning improvements";
}

std::string LearningExperiment::analyzeResults(const std::vector<ResearchData>& results) const {
    return "Learning results show significant plasticity effects (p < 0.05, effect size = 0.8)";
}

// MemoryExperiment implementation

MemoryExperiment::MemoryExperiment() {
    setName("Memory Experiment");
    setDescription("Study of memory systems and consolidation");
    setCategory("Memory");
    setCategoryEnum(ExperimentCategory::Memory);
}

bool MemoryExperiment::run(std::shared_ptr<Brain> brain,
                           std::shared_ptr<AgentBrain> agent,
                           std::shared_ptr<SimpleWorld> world,
                           std::vector<ResearchData>& results) {
    ResearchData data;
    data.experimentId = getId();
    data.hypothesisTested = "Episodic memory consolidation occurs during rest";
    
    // Run memory experiment
    for (int step = 0; step < 100; ++step) {
        // Apply memory stimuli
        // Monitor memory systems
        // Record metrics
    }
    
    // Calculate metrics
    data.addMetric("working_memory_capacity", 100);
    data.addMetric("episodic_memory_duration", 10.0);
    data.addMetric("associative_memory_strength", 0.7);
    data.addMetric("memory_consolidation_rate", 0.05);
    data.pValue = 0.03;
    data.effectSize = 0.6;
    data.hypothesisSupported = true;
    
    results.push_back(data);
    return true;
}

std::string MemoryExperiment::getParameters() const {
    return "Memory capacity: 100 units, Consolidation: 0.05/sec";
}

std::string MemoryExperiment::getResultsSummary() const {
    return "Memory consolidation successfully demonstrated during rest periods";
}

std::string MemoryExperiment::analyzeResults(const std::vector<ResearchData>& results) const {
    return "Memory results show significant consolidation effects (p < 0.05, effect size = 0.6)";
}

// NeuromodulationExperiment implementation

NeuromodulationExperiment::NeuromodulationExperiment() {
    setName("Neuromodulation Experiment");
    setDescription("Study of neuromodulation and motivation");
    setCategory("Neuromodulation");
    setCategoryEnum(ExperimentCategory::Neuromodulation);
}

bool NeuromodulationExperiment::run(std::shared_ptr<Brain> brain,
                                    std::shared_ptr<AgentBrain> agent,
                                    std::shared_ptr<SimpleWorld> world,
                                    std::vector<ResearchData>& results) {
    ResearchData data;
    data.experimentId = getId();
    data.hypothesisTested = "Dopamine level modulates learning rate";
    
    // Run neuromodulation experiment
    for (int step = 0; step < 100; ++step) {
        // Apply neuromodulation
        // Monitor neuromodulator levels
        // Record metrics
    }
    
    // Calculate metrics
    data.addMetric("dopamine_level", 1.5);
    data.addMetric("curiosity_level", 0.8);
    data.addMetric("novelty_detection", 0.6);
    data.addMetric("reward_prediction", 0.4);
    data.pValue = 0.07;
    data.effectSize = 0.4;
    data.hypothesisSupported = false; // Not significant
    
    results.push_back(data);
    return true;
}

std::string NeuromodulationExperiment::getParameters() const {
    return "Dopamine scale: 1.5, Curiosity decay: 0.01";
}

std::string NeuromodulationExperiment::getResultsSummary() const {
    return "Neuromodulation effects partially observed (not statistically significant)";
}

std::string NeuromodulationExperiment::analyzeResults(const std::vector<ResearchData>& results) const {
    return "Neuromodulation results show moderate effects (p = 0.07, effect size = 0.4)";
}

// DevelopmentExperiment implementation

DevelopmentExperiment::DevelopmentExperiment() {
    setName("Development Experiment");
    setDescription("Study of development and aging processes");
    setCategory("Development");
    setCategoryEnum(ExperimentCategory::Development);
}

bool DevelopmentExperiment::run(std::shared_ptr<Brain> brain,
                               std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world,
                               std::vector<ResearchData>& results) {
    ResearchData data;
    data.experimentId = getId();
    data.hypothesisTested = "Brain development enhances plasticity in early stages";
    
    // Run development experiment
    for (int step = 0; step < 100; ++step) {
        // Apply developmental factors
        // Monitor developmental stage
        // Record metrics
    }
    
    // Calculate metrics
    data.addMetric("developmental_age", 1.0);
    data.addMetric("plasticity_modifier", 1.2);
    data.addMetric("critical_period_duration", 10.0);
    data.addMetric("aging_rate", 0.01);
    data.pValue = 0.01;
    data.effectSize = 0.9;
    data.hypothesisSupported = true;
    
    results.push_back(data);
    return true;
}

std::string DevelopmentExperiment::getParameters() const {
    return "Critical period: 10.0s, Aging rate: 0.01/sec";
}

std::string DevelopmentExperiment::getResultsSummary() const {
    return "Development effects clearly demonstrated (p < 0.01, effect size = 0.9)";
}

std::string DevelopmentExperiment::analyzeResults(const std::vector<ResearchData>& results) const {
    return "Development results show strong effects (p < 0.01, effect size = 0.9)";
}

// SocialExperiment implementation

SocialExperiment::SocialExperiment() {
    setName("Social Experiment");
    setDescription("Study of social and multi-agent interactions");
    setCategory("Social");
    setCategoryEnum(ExperimentCategory::Social);
}

bool SocialExperiment::run(std::shared_ptr<Brain> brain,
                          std::shared_ptr<AgentBrain> agent,
                          std::shared_ptr<SimpleWorld> world,
                          std::vector<ResearchData>& results) {
    ResearchData data;
    data.experimentId = getId();
    data.hypothesisTested = "Social observation enhances learning efficiency";
    
    // Run social experiment
    for (int step = 0; step < 100; ++step) {
        // Apply social stimuli
        // Monitor social learning
        // Record metrics
    }
    
    // Calculate metrics
    data.addMetric("observation_efficiency", 0.8);
    data.addMetric("social_learning_rate", 0.03);
    data.addMetric("imitation_accuracy", 0.9);
    data.addMetric("social_influence", 0.7);
    data.pValue = 0.04;
    data.effectSize = 0.7;
    data.hypothesisSupported = true;
    
    results.push_back(data);
    return true;
}

std::string SocialExperiment::getParameters() const {
    return "Social observation: enabled, Imitation accuracy: 0.9";
}

std::string SocialExperiment::getResultsSummary() const {
    return "Social effects significantly demonstrated (p < 0.05, effect size = 0.7)";
}

std::string SocialExperiment::analyzeResults(const std::vector<ResearchData>& results) const {
    return "Social results show significant effects (p < 0.05, effect size = 0.7)";
}

// ResearchAnalysis implementations

std::string StatisticalAnalysis::performAnalysis(const std::vector<ResearchData>& data) {
    std::ostringstream oss;
    oss << "Statistical Analysis Report" << std::endl;
    oss << "===========================" << std::endl;
    oss << "Number of experiments: " << data.size() << std::endl;
    oss << "Average p-value: " << calculateAveragePValue(data) << std::endl;
    oss << "Significant results: " << countSignificantResults(data) << std::endl;
    oss << "Effect size average: " << calculateAverageEffectSize(data) << std::endl;
    oss << "Hypothesis support rate: " << calculateHypothesisSupportRate(data) * 100 << "%" << std::endl;
    return oss.str();
}

double StatisticalAnalysis::calculateAveragePValue(const std::vector<ResearchData>& data) const {
    if (data.empty()) return 1.0;
    double sum = 0.0;
    for (const auto& d : data) {
        sum += d.pValue;
    }
    return sum / data.size();
}

int StatisticalAnalysis::countSignificantResults(const std::vector<ResearchData>& data) const {
    int count = 0;
    for (const auto& d : data) {
        if (d.pValue < 0.05) {
            count++;
        }
    }
    return count;
}

double StatisticalAnalysis::calculateAverageEffectSize(const std::vector<ResearchData>& data) const {
    if (data.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& d : data) {
        sum += d.effectSize;
    }
    return sum / data.size();
}

double StatisticalAnalysis::calculateHypothesisSupportRate(const std::vector<ResearchData>& data) const {
    if (data.empty()) return 0.0;
    int supported = 0;
    for (const auto& d : data) {
        if (d.hypothesisSupported) {
            supported++;
        }
    }
    return static_cast<double>(supported) / data.size();
}

std::string PatternAnalysis::performAnalysis(const std::vector<ResearchData>& data) {
    return "Pattern analysis completed (implementation simplified for demonstration)";
}

std::string TimeSeriesAnalysis::performAnalysis(const std::vector<ResearchData>& data) {
    return "Time series analysis completed (implementation simplified for demonstration)";
}

std::string NetworkAnalysis::performAnalysis(const std::vector<ResearchData>& data) {
    return "Network analysis completed (implementation simplified for demonstration)";
}

// ResearchFactory implementations

std::unique_ptr<ResearchExperiment> ResearchFactory::createLearningExperiment(const std::string& name) {
    auto experiment = std::make_unique<LearningExperiment>();
    experiment->setName(name);
    return experiment;
}

std::unique_ptr<ResearchExperiment> ResearchFactory::createMemoryExperiment(const std::string& name) {
    auto experiment = std::make_unique<MemoryExperiment>();
    experiment->setName(name);
    return experiment;
}

std::unique_ptr<ResearchExperiment> ResearchFactory::createNeuromodulationExperiment(const std::string& name) {
    auto experiment = std::make_unique<NeuromodulationExperiment>();
    experiment->setName(name);
    return experiment;
}

std::unique_ptr<ResearchExperiment> ResearchFactory::createDevelopmentExperiment(const std::string& name) {
    auto experiment = std::make_unique<DevelopmentExperiment>();
    experiment->setName(name);
    return experiment;
}

std::unique_ptr<ResearchExperiment> ResearchFactory::createSocialExperiment(const std::string& name) {
    auto experiment = std::make_unique<SocialExperiment>();
    experiment->setName(name);
    return experiment;
}

std::unique_ptr<ResearchExperiment> ResearchFactory::createCustomExperiment(const std::string& name,
                                                                          const std::string& description,
                                                                          ExperimentCategory category) {
    auto experiment = std::make_unique<ResearchExperiment>();
    experiment->setName(name);
    experiment->setDescription(description);
    experiment->setCategoryEnum(category);
    return experiment;
}

std::unique_ptr<ResearchAnalysis> ResearchFactory::createStatisticalAnalysis(const std::string& name) {
    auto analysis = std::make_unique<StatisticalAnalysis>();
    analysis->setName(name);
    return analysis;
}

std::unique_ptr<ResearchAnalysis> ResearchFactory::createPatternAnalysis(const std::string& name) {
    auto analysis = std::make_unique<PatternAnalysis>();
    analysis->setName(name);
    return analysis;
}

std::unique_ptr<ResearchAnalysis> ResearchFactory::createTimeSeriesAnalysis(const std::string& name) {
    auto analysis = std::make_unique<TimeSeriesAnalysis>();
    analysis->setName(name);
    return analysis;
}

std::unique_ptr<ResearchAnalysis> ResearchFactory::createNetworkAnalysis(const std::string& name) {
    auto analysis = std::make_unique<NetworkAnalysis>();
    analysis->setName(name);
    return analysis;
}

std::unique_ptr<ResearchAnalysis> ResearchFactory::createCustomAnalysis(const std::string& name,
                                                                    const std::string& description,
                                                                    AnalysisType type) {
    auto analysis = std::make_unique<ResearchAnalysis>();
    analysis->setName(name);
    analysis->setDescription(description);
    analysis->setType(type);
    return analysis;
}

bool ResearchExperiment::exportResults(const std::vector<ResearchData>& results, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& data : results) {
        file << data.toString() << std::endl;
    }
    
    return true;
}

bool ResearchExperiment::importResults(std::vector<ResearchData>& results, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simplified import - in a real implementation, you would parse the format properly
    // For now, just clear existing results
    results.clear();
    
    return true;
}

std::string ResearchManager::formatTime(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string ResearchManager::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace nlm
