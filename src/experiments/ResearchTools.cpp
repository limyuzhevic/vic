#include "ResearchTools.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <thread>
#include <atomic>

namespace nlm {

struct ResearchTools::Impl {
    // Experimental conditions
    std::map<std::string, ExperimentalCondition> experimentalConditions;
    std::string currentCondition;
    std::string baselineCondition;
    
    // Parameter tuning
    std::map<std::string, ParameterTune> parameterTunes;
    std::vector<ParameterTuningResult> parameterTuningResults;
    
    // Data comparison
    std::map<std::string, Metrics> comparisonMetrics;
    std::map<std::string, ResearchMetrics> comparisonResearchMetrics;
    
    // Statistical tests
    std::vector<StatisticalTestResult> statisticalTestResults;
    
    // Ablation studies
    std::vector<AblationConfig> ablationConfigs;
    
    // System control
    bool researchMode = false;
    bool debugMode = false;
    bool systemsPaused = false;
    bool running = false;
    
    // Session tracking
    std::chrono::system_clock::time_point sessionStart;
    std::chrono::system_clock::time_point lastUpdate;
    
    // Active systems tracking
    std::vector<std::string> activeSystems;
    
    // Default parameters
    std::map<std::string, double> defaultParameters;
    
    Impl() {
        running = true;
        sessionStart = std::chrono::system_clock::now();
        lastUpdate = sessionStart;
        
        // Initialize default parameters
        defaultParameters["neuron_count"] = 1000.0;
        defaultParameters["max_steps"] = 10000.0;
        defaultParameters["learning_rate"] = 0.01;
        defaultParameters["plasticity_rate"] = 0.1;
        defaultParameters["attention_threshold"] = 0.5;
        defaultParameters["memory_decay_rate"] = 0.01;
        defaultParameters["novelty_threshold"] = 0.7;
        
        // Initialize default active systems
        activeSystems = {
            "brain_core",
            "sensory_processing",
            "motor_control",
            "working_memory",
            "episodic_memory",
            "neuromodulation",
            "prediction_system",
            "cognition_systems",
            "development_system"
        };
    }
};

ResearchTools::ResearchTools() : pImpl(std::make_unique<Impl>()) {
}

ResearchTools::~ResearchTools() {
    stop();
}

void ResearchTools::createExperimentalCondition(const ExperimentalCondition& condition) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->experimentalConditions[condition.name] = condition;
    NLM_LOG_INFO("ResearchTools: Created experimental condition '" + condition.name + "' - " + condition.description);
}

bool ResearchTools::runExperimentalCondition(const std::string& conditionName) {
    auto it = pImpl->experimentalConditions.find(conditionName);
    if (it == pImpl->experimentalConditions.end()) {
        NLM_LOG_ERROR("ResearchTools: Experimental condition not found: " + conditionName);
        return false;
    }
    
    const ExperimentalCondition& condition = it->second;
    
    NLM_LOG_INFO("ResearchTools: Running experimental condition '" + conditionName + "'");
    
    // Apply condition parameters
    for (const auto& param : condition.parameters) {
        setParameter(param.first, param.second);
    }
    
    // Enable/disable systems
    for (const std::string& system : condition.enabledSystems) {
        // In a real implementation, this would actually enable the system
        NLM_LOG_INFO("ResearchTools: Enabling system '" + system + "' for condition '" + conditionName + "'");
    }
    
    for (const std::string& system : condition.disabledSystems) {
        // In a real implementation, this would actually disable the system
        NLM_LOG_INFO("ResearchTools: Disabling system '" + system + "' for condition '" + conditionName + "'");
    }
    
    pImpl->currentCondition = conditionName;
    pImpl->lastUpdate = std::chrono::system_clock::now();
    
    return true;
}

bool ResearchTools::restoreExperimentalCondition(const std::string& conditionName) {
    // For now, just switch to a different condition
    return runExperimentalCondition(conditionName);
}

std::vector<ResearchTools::ExperimentalCondition> ResearchTools::getExperimentalConditions() const {
    std::vector<ExperimentalCondition> conditions;
    for (const auto& pair : pImpl->experimentalConditions) {
        conditions.push_back(pair.second);
    }
    return conditions;
}

bool ResearchTools::deleteExperimentalCondition(const std::string& conditionName) {
    auto it = pImpl->experimentalConditions.find(conditionName);
    if (it == pImpl->experimentalConditions.end()) {
        NLM_LOG_ERROR("ResearchTools: Experimental condition not found: " + conditionName);
        return false;
    }
    
    pImpl->experimentalConditions.erase(it);
    NLM_LOG_INFO("ResearchTools: Deleted experimental condition '" + conditionName + "'");
    return true;
}

void ResearchTools::addParameterTune(const ParameterTune& tune) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->parameterTunes[tune.parameterName] = tune;
    NLM_LOG_INFO("ResearchTools: Added parameter tuning for '" + tune.parameterName + "'");
}

bool ResearchTools::runParameterTuning(const std::string& tuneName) {
    auto it = pImpl->parameterTunes.find(tuneName);
    if (it == pImpl->parameterTunes.end()) {
        NLM_LOG_ERROR("ResearchTools: Parameter tuning not found: " + tuneName);
        return false;
    }
    
    ParameterTune& tune = it->second;
    ParameterTuningResult result;
    result.parameterName = tune.parameterName;
    
    NLM_LOG_INFO("ResearchTools: Running parameter tuning for '" + tune.parameterName + "'");
    
    // Perform parameter tuning (simplified - just test a few values)
    std::vector<double> values;
    double currentValue = getParameter(tune.parameterName);
    
    for (double value = tune.minValue; value <= tune.maxValue; value += tune.stepSize) {
        values.push_back(value);
    }
    
    result.testedValues = values;
    result.performanceValues.reserve(values.size());
    
    for (double value : values) {
        setParameter(tune.parameterName, value);
        
        // Calculate performance (simplified)
        double performance = 0.0;
        if (tune.performanceFunction) {
            performance = tune.performanceFunction(pImpl->defaultParameters);
        } else {
            performance = value; // Simplified
        }
        
        if (tune.optimizeForMax) {
            if (performance > result.optimalPerformance) {
                result.optimalPerformance = performance;
                result.optimalValue = value;
            }
        } else {
            if (performance < result.optimalPerformance || result.optimalPerformance == 0.0) {
                result.optimalPerformance = performance;
                result.optimalValue = value;
            }
        }
        
        result.performanceValues.push_back(performance);
    }
    
    result.tuningCompleted = true;
    pImpl->parameterTuningResults.push_back(result);
    
    // Restore original value
    setParameter(tune.parameterName, currentValue);
    
    NLM_LOG_INFO("ResearchTools: Parameter tuning completed for '" + tune.parameterName + 
                "' - optimal value: " + std::to_string(result.optimalValue) + 
                ", performance: " + std::to_string(result.optimalPerformance));
    
    return true;
}

std::vector<ResearchTools::ParameterTuningResult> ResearchTools::getParameterTuningResults() const {
    return pImpl->parameterTuningResults;
}

void ResearchTools::clearParameterTuningResults() {
    pImpl->parameterTuningResults.clear();
    NLM_LOG_INFO("ResearchTools: Cleared parameter tuning results");
}

void ResearchTools::addComparisonData(const std::string& conditionName, const Metrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->comparisonMetrics[conditionName] = metrics;
    NLM_LOG_INFO("ResearchTools: Added comparison data for condition '" + conditionName + "'");
}

void ResearchTools::addComparisonData(const std::string& conditionName, const ResearchMetrics& metrics) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->comparisonResearchMetrics[conditionName] = metrics;
    NLM_LOG_INFO("ResearchTools: Added comparison data for condition '" + conditionName + "' (ResearchMetrics)");
}

std::vector<ResearchTools::DataComparisonResult> ResearchTools::compareConditions(
    const std::string& condition1Name, const std::string& condition2Name) const {
    std::vector<DataComparisonResult> comparisons;
    
    auto it1 = pImpl->comparisonMetrics.find(condition1Name);
    auto it2 = pImpl->comparisonMetrics.find(condition2Name);
    
    if (it1 == pImpl->comparisonMetrics.end() || it2 == pImpl->comparisonMetrics.end()) {
        NLM_LOG_ERROR("ResearchTools: Comparison data not found for one or both conditions");
        return comparisons;
    }
    
    const Metrics& metrics1 = it1->second;
    const Metrics& metrics2 = it2->second;
    
    // Compare metric names
    std::vector<std::string> metricNames1 = metrics1.getMetricNames();
    std::vector<std::string> metricNames2 = metrics2.getMetricNames();
    
    std::unordered_set<std::string> allNames;
    for (const std::string& name : metricNames1) allNames.insert(name);
    for (const std::string& name : metricNames2) allNames.insert(name);
    
    for (const std::string& name : allNames) {
        double value1 = 0.0, value2 = 0.0;
        
        // Check if metric exists by getting values and checking if empty
        const std::vector<double>& values1 = metrics1.getValues(name);
        const std::vector<double>& values2 = metrics2.getValues(name);
        
        if (!values1.empty()) value1 = values1.back(); // Use latest value
        if (!values2.empty()) value2 = values2.back(); // Use latest value
        
        if (value1 != 0.0 || value2 != 0.0) {
            DataComparisonResult result;
            result.condition1Name = condition1Name;
            result.condition2Name = condition2Name;
            result.metricName = name;
            result.value1 = value1;
            result.value2 = value2;
            result.difference = value2 - value1;
            result.relativeDifference = value1 != 0.0 ? (value2 - value1) / value1 : 0.0;
            result.isBetter = value2 > value1; // Assuming higher is better
            result.comparisonType = "metric_comparison";
            
            comparisons.push_back(result);
        }
    }
    
    NLM_LOG_INFO("ResearchTools: Compared conditions '" + condition1Name + "' and '" + condition2Name + 
                "' - " + std::to_string(comparisons.size()) + " comparisons made");
    
    return comparisons;
}

std::vector<ResearchTools::DataComparisonResult> ResearchTools::compareAllConditions() const {
    std::vector<DataComparisonResult> comparisons;
    
    if (pImpl->comparisonMetrics.size() < 2) {
        return comparisons;
    }
    
    std::vector<std::string> conditionNames;
    for (const auto& pair : pImpl->comparisonMetrics) {
        conditionNames.push_back(pair.first);
    }
    
    for (size_t i = 0; i < conditionNames.size(); ++i) {
        for (size_t j = i + 1; j < conditionNames.size(); ++j) {
            std::vector<DataComparisonResult> condComparisons = compareConditions(conditionNames[i], conditionNames[j]);
            comparisons.insert(comparisons.end(), condComparisons.begin(), condComparisons.end());
        }
    }
    
    return comparisons;
}

void ResearchTools::addStatisticalTest(const StatisticalTestResult& test) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->statisticalTestResults.push_back(test);
    NLM_LOG_INFO("ResearchTools: Added statistical test '" + test.testName + "' - p = " + std::to_string(test.pValue));
}

std::vector<ResearchTools::StatisticalTestResult> ResearchTools::runStatisticalTests() const {
    // For now, return existing results
    return pImpl->statisticalTestResults;
}

std::vector<ResearchTools::StatisticalTestResult> ResearchTools::getStatisticalTestResults() const {
    return pImpl->statisticalTestResults;
}

bool ResearchTools::clearStatisticalTestResults() {
    pImpl->statisticalTestResults.clear();
    NLM_LOG_INFO("ResearchTools: Cleared statistical test results");
    return true;
}

void ResearchTools::configureAblation(const AblationConfig& config) {
    std::lock_guard<std::mutex> lock(/*TODO: Add mutex if needed*/);
    
    pImpl->ablationConfigs.push_back(config);
    NLM_LOG_INFO("ResearchTools: Configured ablation for system '" + config.systemName + 
                "' - ablated: " + std::string(config.isAblated ? "yes" : "no"));
}

bool ResearchTools::enableAblation(const std::string& systemName) {
    for (auto& config : pImpl->ablationConfigs) {
        if (config.systemName == systemName) {
            config.isAblated = true;
            config.ablationTime = std::chrono::system_clock::now();
            config.isRestored = false;
            
            NLM_LOG_INFO("ResearchTools: Enabled ablation for system '" + systemName + "'");
            return true;
        }
    }
    
    NLM_LOG_ERROR("ResearchTools: System not found for ablation: " + systemName);
    return false;
}

bool ResearchTools::disableAblation(const std::string& systemName) {
    for (auto& config : pImpl->ablationConfigs) {
        if (config.systemName == systemName) {
            config.isAblated = false;
            config.restorationTime = std::chrono::system_clock::now();
            config.isRestored = true;
            
            NLM_LOG_INFO("ResearchTools: Disabled ablation for system '" + systemName + "'");
            return true;
        }
    }
    
    NLM_LOG_ERROR("ResearchTools: System not found for ablation disable: " + systemName);
    return false;
}

std::vector<ResearchTools::AblationConfig> ResearchTools::getAblationConfigs() const {
    return pImpl->ablationConfigs;
}

bool ResearchTools::restoreAllAblations() {
    bool success = true;
    
    for (auto& config : pImpl->ablationConfigs) {
        if (config.isAblated) {
            config.isAblated = false;
            config.restorationTime = std::chrono::system_clock::now();
            config.isRestored = true;
            
            NLM_LOG_INFO("ResearchTools: Restored ablation for system '" + config.systemName + "'");
        }
    }
    
    return success;
}

bool ResearchTools::exportResults(const std::string& filepath) const {
    try {
        nlohmann::json j;
        
        // Export experimental conditions
        std::vector<nlohmann::json> conditionsJson;
        for (const auto& pair : pImpl->experimentalConditions) {
            nlohmann::json conditionJson;
            conditionJson["name"] = pair.second.name;
            conditionJson["description"] = pair.second.description;
            conditionJson["parameters"] = pair.second.parameters;
            conditionJson["enabledSystems"] = pair.second.enabledSystems;
            conditionJson["disabledSystems"] = pair.second.disabledSystems;
            conditionJson["isBaseline"] = pair.second.isBaseline;
            conditionsJson.push_back(conditionJson);
        }
        j["experimentalConditions"] = conditionsJson;
        
        // Export parameter tuning results
        std::vector<nlohmann::json> tuningResultsJson;
        for (const auto& result : pImpl->parameterTuningResults) {
            nlohmann::json resultJson;
            resultJson["parameterName"] = result.parameterName;
            resultJson["optimalValue"] = result.optimalValue;
            resultJson["optimalPerformance"] = result.optimalPerformance;
            resultJson["testedValues"] = result.testedValues;
            resultJson["performanceValues"] = result.performanceValues;
            tuningResultsJson.push_back(resultJson);
        }
        j["parameterTuningResults"] = tuningResultsJson;
        
        // Export ablation configs
        std::vector<nlohmann::json> ablationConfigsJson;
        for (const auto& config : pImpl->ablationConfigs) {
            nlohmann::json configJson;
            configJson["systemName"] = config.systemName;
            configJson["isAblated"] = config.isAblated;
            configJson["ablationMethod"] = config.ablationMethod;
            configJson["ablationTime"] = std::chrono::system_clock::to_time_t(config.ablationTime);
            configJson["restorationTime"] = config.isRestored ? std::chrono::system_clock::to_time_t(config.restorationTime) : "";
            configJson["isRestored"] = config.isRestored;
            ablationConfigsJson.push_back(configJson);
        }
        j["ablationConfigs"] = ablationConfigsJson;
        
        // Export session info
        j["sessionStart"] = std::chrono::system_clock::to_time_t(pImpl->sessionStart);
        j["currentCondition"] = pImpl->currentCondition;
        j["baselineCondition"] = pImpl->baselineCondition;
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("ResearchTools: Exported results to " + filepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to export results: " + std::string(e.what()));
    }
    
    return false;
}

bool ResearchTools::importResults(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("ResearchTools: Could not open file for import: " + filepath);
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        file.close();
        
        // Import experimental conditions
        if (j.contains("experimentalConditions")) {
            for (const auto& conditionJson : j["experimentalConditions"]) {
                ExperimentalCondition condition;
                condition.name = conditionJson["name"];
                condition.description = conditionJson["description"];
                condition.parameters = conditionJson["parameters"].get<std::map<std::string, double>>();
                condition.enabledSystems = conditionJson["enabledSystems"].get<std::vector<std::string>>();
                condition.disabledSystems = conditionJson["disabledSystems"].get<std::vector<std::string>>();
                condition.isBaseline = conditionJson["isBaseline"];
                
                createExperimentalCondition(condition);
            }
        }
        
        // Import ablation configs
        if (j.contains("ablationConfigs")) {
            for (const auto& configJson : j["ablationConfigs"]) {
                AblationConfig config;
                config.systemName = configJson["systemName"];
                config.isAblated = configJson["isAblated"];
                config.ablationMethod = configJson["ablationMethod"];
                config.ablationTime = std::chrono::system_clock::from_time_t(configJson["ablationTime"]);
                config.isRestored = configJson.contains("restorationTime") && !configJson["restorationTime"].empty();
                if (config.isRestored) {
                    config.restorationTime = std::chrono::system_clock::from_time_t(configJson["restorationTime"]);
                }
                
                configureAblation(config);
            }
        }
        
        // Import session info
        if (j.contains("currentCondition")) {
            pImpl->currentCondition = j["currentCondition"];
        }
        if (j.contains("baselineCondition")) {
            pImpl->baselineCondition = j["baselineCondition"];
        }
        
        NLM_LOG_INFO("ResearchTools: Imported results from " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to import results: " + std::string(e.what()));
        return false;
    }
}

bool ResearchTools::exportConditions(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("ResearchTools: Could not open conditions file for writing: " + filepath);
            return false;
        }
        
        file << "Experimental Condition Export\n";
        file << "======================\n\n";
        
        for (const auto& pair : pImpl->experimentalConditions) {
            const ExperimentalCondition& condition = pair.second;
            file << "Condition: " << condition.name << "\n";
            file << "Description: " << condition.description << "\n";
            file << "Parameters:\n";
            for (const auto& param : condition.parameters) {
                file << "  " << param.first << ": " << param.second << "\n";
            }
            file << "Enabled Systems:\n";
            for (const std::string& system : condition.enabledSystems) {
                file << "  " << system << "\n";
            }
            file << "Disabled Systems:\n";
            for (const std::string& system : condition.disabledSystems) {
                file << "  " << system << "\n";
            }
            file << "Is Baseline: " << (condition.isBaseline ? "yes" : "no") << "\n\n";
        }
        
        file.close();
        NLM_LOG_INFO("ResearchTools: Exported conditions to " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to export conditions: " + std::string(e.what()));
        return false;
    }
}

bool ResearchTools::importConditions(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("ResearchTools: Could not open conditions file for reading: " + filepath);
            return false;
        }
        
        std::string line;
        ExperimentalCondition currentCondition;
        bool inCondition = false;
        
        while (std::getline(file, line)) {
            if (line.empty()) {
                if (inCondition) {
                    createExperimentalCondition(currentCondition);
                    inCondition = false;
                }
                continue;
            }
            
            if (line.find("Condition:") == 0) {
                if (inCondition) {
                    createExperimentalCondition(currentCondition);
                }
                currentCondition = ExperimentalCondition();
                currentCondition.name = line.substr(11);
                inCondition = true;
            } else if (line.find("Description:") == 0 && inCondition) {
                currentCondition.description = line.substr(13);
            } else if (line.find("Parameters:") == 0 && inCondition) {
                // Skip parameter section header
                continue;
            } else if (line.find("Enabled Systems:") == 0 && inCondition) {
                // Skip enabled systems section header
                continue;
            } else if (line.find("Disabled Systems:") == 0 && inCondition) {
                // Skip disabled systems section header
                continue;
            } else if (line.find("Is Baseline:") == 0 && inCondition) {
                currentCondition.isBaseline = (line.substr(13) == "yes");
            } else if (!line.empty() && inCondition) {
                // Check if it's a parameter line (contains colon)
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = line.substr(2, colonPos - 3);
                    std::string valueStr = line.substr(colonPos + 1);
                    try {
                        double value = std::stod(valueStr);
                        currentCondition.parameters[key] = value;
                    } catch (...) {
                        // Skip invalid values
                    }
                } else if (line.find("  ") == 0) {
                    // Check if it's a system name (starts with two spaces)
                    std::string systemName = line.substr(2);
                    if (!currentCondition.enabledSystems.empty() && systemName.find_last_of("Enabled Systems") == std::string::npos) {
                        currentCondition.enabledSystems.push_back(systemName);
                    } else if (!currentCondition.disabledSystems.empty() && systemName.find_last_of("Disabled Systems") == std::string::npos) {
                        currentCondition.disabledSystems.push_back(systemName);
                    }
                }
            }
        }
        
        if (inCondition) {
            createExperimentalCondition(currentCondition);
        }
        
        file.close();
        NLM_LOG_INFO("ResearchTools: Imported conditions from " + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to import conditions: " + std::string(e.what()));
        return false;
    }
}

void ResearchTools::setBaselineCondition(const std::string& conditionName) {
    pImpl->baselineCondition = conditionName;
    NLM_LOG_INFO("ResearchTools: Set baseline condition to '" + conditionName + "'");
}

std::string ResearchTools::getBaselineCondition() const {
    return pImpl->baselineCondition;
}

void ResearchTools::setCurrentCondition(const std::string& conditionName) {
    pImpl->currentCondition = conditionName;
    NLM_LOG_INFO("ResearchTools: Set current condition to '" + conditionName + "'");
}

std::string ResearchTools::getCurrentCondition() const {
    return pImpl->currentCondition;
}

void ResearchTools::pauseAllSystems(bool pause) {
    pImpl->systemsPaused = pause;
    NLM_LOG_INFO("ResearchTools: Systems " + std::string(pause ? "paused" : "resumed"));
}

void ResearchTools::enableDebugMode(bool enable) {
    pImpl->debugMode = enable;
    NLM_LOG_INFO("ResearchTools: Debug mode " + std::string(enable ? "enabled" : "disabled"));
}

void ResearchTools::setResearchMode(bool enable) {
    pImpl->researchMode = enable;
    NLM_LOG_INFO("ResearchTools: Research mode " + std::string(enable ? "enabled" : "disabled"));
}

bool ResearchTools::isResearchMode() const {
    return pImpl->researchMode;
}

bool ResearchTools::isDebugMode() const {
    return pImpl->debugMode;
}

bool ResearchTools::areSystemsPaused() const {
    return pImpl->systemsPaused;
}

void ResearchTools::integrateWithMetrics(const Metrics& metrics) {
    addComparisonData(pImpl->currentCondition, metrics);
    NLM_LOG_INFO("ResearchTools: Integrated with Metrics system");
}

void ResearchTools::integrateWithResearchMetrics(const ResearchMetrics& metrics) {
    addComparisonData(pImpl->currentCondition, metrics);
    NLM_LOG_INFO("ResearchTools: Integrated with ResearchMetrics system");
}

std::vector<ResearchTools::DataComparisonResult> ResearchTools::generatePerformanceReport() const {
    std::vector<DataComparisonResult> comparisons;
    
    if (pImpl->comparisonMetrics.empty()) {
        return comparisons;
    }
    
    std::vector<std::string> conditionNames;
    for (const auto& pair : pImpl->comparisonMetrics) {
        conditionNames.push_back(pair.first);
    }
    
    // Generate comparisons between all conditions
    for (size_t i = 0; i < conditionNames.size(); ++i) {
        for (size_t j = i + 1; j < conditionNames.size(); ++j) {
            std::vector<DataComparisonResult> condComparisons = compareConditions(conditionNames[i], conditionNames[j]);
            comparisons.insert(comparisons.end(), condComparisons.begin(), condComparisons.end());
        }
    }
    
    NLM_LOG_INFO("ResearchTools: Generated performance report with " + std::to_string(comparisons.size()) + " comparisons");
    
    return comparisons;
}

std::vector<ResearchTools::DataComparisonResult> ResearchTools::generateLearningReport() const {
    std::vector<DataComparisonResult> comparisons;
    
    // Similar to performance report but focuses on learning metrics
    if (pImpl->comparisonResearchMetrics.empty()) {
        return comparisons;
    }
    
    std::vector<std::string> conditionNames;
    for (const auto& pair : pImpl->comparisonResearchMetrics) {
        conditionNames.push_back(pair.first);
    }
    
    for (size_t i = 0; i < conditionNames.size(); ++i) {
        for (size_t j = i + 1; j < conditionNames.size(); ++j) {
            std::vector<DataComparisonResult> condComparisons = compareConditions(conditionNames[i], conditionNames[j]);
            comparisons.insert(comparisons.end(), condComparisons.begin(), condComparisons.end());
        }
    }
    
    NLM_LOG_INFO("ResearchTools: Generated learning report with " + std::to_string(comparisons.size()) + " comparisons");
    
    return comparisons;
}

bool ResearchTools::generateVisualizationData(const std::string& outputFilepath) const {
    try {
        nlohmann::json j;
        
        j["type"] = "visualization_data";
        j["sessionStart"] = std::chrono::system_clock::to_time_t(pImpl->sessionStart);
        j["currentCondition"] = pImpl->currentCondition;
        j["activeSystems"] = pImpl->activeSystems;
        j["parameters"] = pImpl->defaultParameters;
        
        // In a real implementation, this would generate actual visualization data
        // For now, we'll create a placeholder
        j["neuralNodes"] = std::vector<nlohmann::json>();
        j["connections"] = std::vector<nlohmann::json>();
        j["metrics"] = std::vector<nlohmann::json>();
        
        std::ofstream file(outputFilepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("ResearchTools: Generated visualization data at " + outputFilepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to generate visualization data: " + std::string(e.what()));
    }
    
    return false;
}

bool ResearchTools::generateStatisticalVisualization(const std::string& outputFilepath) const {
    NLM_LOG_INFO("ResearchTools: Generated statistical visualization at " + outputFilepath);
    // In a real implementation, this would create statistical plots
    return true;
}

bool ResearchTools::generateComparisonVisualization(const std::string& outputFilepath) const {
    NLM_LOG_INFO("ResearchTools: Generated comparison visualization at " + outputFilepath);
    // In a real implementation, this would create comparison plots
    return true;
}

void ResearchTools::setDefaultParameters() {
    pImpl->defaultParameters.clear();
    pImpl->defaultParameters["neuron_count"] = 1000.0;
    pImpl->defaultParameters["max_steps"] = 10000.0;
    pImpl->defaultParameters["learning_rate"] = 0.01;
    pImpl->defaultParameters["plasticity_rate"] = 0.1;
    pImpl->defaultParameters["attention_threshold"] = 0.5;
    pImpl->defaultParameters["memory_decay_rate"] = 0.01;
    pImpl->defaultParameters["novelty_threshold"] = 0.7;
    
    NLM_LOG_INFO("ResearchTools: Set default parameters");
}

void ResearchTools::loadParametersFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (file.is_open()) {
            nlohmann::json j;
            file >> j;
            file.close();
            
            pImpl->defaultParameters = j["parameters"].get<std::map<std::string, double>>();
            NLM_LOG_INFO("ResearchTools: Loaded parameters from " + filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to load parameters from file: " + std::string(e.what()));
    }
}

void ResearchTools::saveParametersToFile(const std::string& filepath) const {
    try {
        nlohmann::json j;
        j["parameters"] = pImpl->defaultParameters;
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            NLM_LOG_INFO("ResearchTools: Saved parameters to " + filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("ResearchTools: Failed to save parameters to file: " + std::string(e.what()));
    }
}

bool ResearchTools::isRunning() const {
    return pImpl->running;
}

std::chrono::system_clock::time_point ResearchTools::getSessionStartTime() const {
    return pImpl->sessionStart;
}

 double ResearchTools::getSessionDuration() const {
    return std::chrono::duration<double>(std::chrono::system_clock::now() - pImpl->sessionStart).count();
}

std::vector<std::string> ResearchTools::getActiveSystems() const {
    return pImpl->activeSystems;
}

std::map<std::string, double> ResearchTools::getCurrentParameters() const {
    return pImpl->defaultParameters;
}

void ResearchTools::reset() {
    pImpl->experimentalConditions.clear();
    pImpl->currentCondition.clear();
    pImpl->baselineCondition.clear();
    pImpl->parameterTunes.clear();
    pImpl->parameterTuningResults.clear();
    pImpl->comparisonMetrics.clear();
    pImpl->comparisonResearchMetrics.clear();
    pImpl->statisticalTestResults.clear();
    pImpl->ablationConfigs.clear();
    pImpl->systemsPaused = false;
    pImpl->sessionStart = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("ResearchTools: Reset all tools and data");
}

void ResearchTools::stop() {
    pImpl->running = false;
    pImpl->systemsPaused = true;
    
    NLM_LOG_INFO("ResearchTools: Stopped");
}

} // namespace nlm
