// NLM Batch Processor implementation
// Handles batch execution of multiple experiments with different configurations

#include "BatchProcessor.hpp"
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <sstream>

namespace nlm {

struct BatchProcessor::Impl {
    std::map<std::string, std::shared_ptr<BatchExperiment>> experiments;
    std::vector<BatchResult> results;
    std::shared_ptr<Config> globalConfig;
    std::shared_ptr<Logger> logger;
    int nextExperimentId;
    
    Impl() : nextExperimentId(0) {
        globalConfig = std::make_shared<Config>();
        logger = std::make_shared<Logger>();
        logger->setLevel(LogLevel::INFO);
    }
    
    std::string generateUniqueName(const std::string& baseName) {
        std::string uniqueName = baseName;
        int counter = 0;
        while (experiments.find(uniqueName) != experiments.end()) {
            counter++;
            uniqueName = baseName + "_" + std::to_string(counter);
        }
        return uniqueName;
    }
};

// BatchProcessor implementation

BatchProcessor::BatchProcessor() : pImpl(std::make_unique<Impl>()) {}

BatchProcessor::~BatchProcessor() = default;

void BatchProcessor::addExperiment(const std::string& name,
                                  const std::shared_ptr<Config>& config,
                                  const std::string& description,
                                  int priority) {
    std::string uniqueName = pImpl->generateUniqueName(name);
    
    auto experiment = std::make_shared<BatchExperiment>();
    experiment->name = uniqueName;
    experiment->config = config;
    experiment->description = description;
    experiment->priority = priority;
    experiment->enabled = true;
    
    pImpl->experiments[uniqueName] = experiment;
    
    NLM_LOG_INFO("Added experiment: " + uniqueName + " - " + description);
}

bool BatchProcessor::addExperimentFromFile(const std::string& name,
                                          const std::string& configFile,
                                          const std::string& description,
                                          int priority) {
    auto config = std::make_shared<Config>();
    if (!config->loadFromFile(configFile)) {
        NLM_LOG_ERROR("Failed to load config file: " + configFile);
        return false;
    }
    
    addExperiment(name, config, description, priority);
    return true;
}

bool BatchProcessor::removeExperiment(const std::string& name) {
    auto it = pImpl->experiments.find(name);
    if (it == pImpl->experiments.end()) {
        return false;
    }
    
    NLM_LOG_INFO("Removed experiment: " + name);
    pImpl->experiments.erase(it);
    return true;
}

void BatchProcessor::clearExperiments() {
    NLM_LOG_INFO("Cleared all experiments");
    pImpl->experiments.clear();
}

std::vector<std::string> BatchProcessor::getExperimentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->experiments) {
        names.push_back(pair.first);
    }
    return names;
}

std::shared_ptr<BatchExperiment> BatchProcessor::getExperiment(const std::string& name) const {
    auto it = pImpl->experiments.find(name);
    if (it != pImpl->experiments.end()) {
        return it->second;
    }
    return nullptr;
}

void BatchProcessor::setExperimentEnabled(const std::string& name, bool enabled) {
    auto experiment = getExperiment(name);
    if (experiment) {
        experiment->enabled = enabled;
        NLM_LOG_INFO((enabled ? "Enabled" : "Disabled") + " experiment: " + name);
    }
}

void BatchProcessor::setExperimentParameter(const std::string& name,
                                           const std::string& key,
                                           const std::string& value) {
    auto experiment = getExperiment(name);
    if (experiment) {
        experiment->parameters[key] = value;
        experiment->config->set(key, value);
        NLM_LOG_INFO("Set parameter for " + name + ": " + key + " = " + value);
    }
}

std::string BatchProcessor::getExperimentParameter(const std::string& name,
                                                   const std::string& key) const {
    auto experiment = getExperiment(name);
    if (experiment) {
        auto it = experiment->parameters.find(key);
        if (it != experiment->parameters.end()) {
            return it->second;
        }
    }
    return "";
}

std::vector<std::shared_ptr<BatchExperiment>> BatchProcessor::generateVariations(
    const std::string& baseName,
    const std::map<std::string, std::vector<std::string>>& parameterSets) {
    
    auto baseExperiment = getExperiment(baseName);
    if (!baseExperiment) {
        NLM_LOG_ERROR("Base experiment not found: " + baseName);
        return {};
    }
    
    std::vector<std::shared_ptr<BatchExperiment>> variations;
    
    // Generate parameter combinations
    std::vector<std::map<std::string, std::string>> parameterCombinations;
    generateParameterCombinations(parameterSets, 0, std::map<std::string, std::string>(), parameterCombinations);
    
    for (size_t i = 0; i < parameterCombinations.size(); ++i) {
        auto variation = std::make_shared<BatchExperiment>(*baseExperiment);
        variation->name = baseName + "_var_" + std::to_string(i + 1);
        
        // Apply parameters
        for (const auto& pair : parameterCombinations[i]) {
            variation->parameters[pair.first] = pair.second;
            variation->config->set(pair.first, pair.second);
        }
        
        variations.push_back(variation);
        pImpl->experiments[variation->name] = variation;
    }
    
    NLM_LOG_INFO("Generated " + std::to_string(variations.size()) + 
                 " variations of experiment: " + baseName);
    
    return variations;
}

void BatchProcessor::generateParameterCombinations(
    const std::map<std::string, std::vector<std::string>>& parameterSets,
    size_t index,
    std::map<std::string, std::string>& current,
    std::vector<std::map<std::string, std::string>>& combinations) {
    
    if (index >= parameterSets.size()) {
        combinations.push_back(current);
        return;
    }
    
    auto it = parameterSets.begin();
    std::advance(it, index);
    
    for (const auto& value : it->second) {
        current[it->first] = value;
        generateParameterCombinations(parameterSets, index + 1, current, combinations);
        current.erase(it->first);
    }
}

BatchResult BatchProcessor::runExperiment(const std::string& name) {
    BatchResult result;
    result.experimentName = name;
    result.success = false;
    
    auto experiment = getExperiment(name);
    if (!experiment || !experiment->enabled) {
        result.errorMessage = "Experiment not found or disabled: " + name;
        NLM_LOG_ERROR(result.errorMessage);
        return result;
    }
    
    // Record start time
    result.startTime = std::chrono::high_resolution_clock::now();
    
    try {
        NLM_LOG_INFO("Running experiment: " + name);
        
        // Create brain from experiment config
        auto brain = std::make_shared<Brain>(experiment->config);
        brain->initialize();
        
        // Run simulation
        for (int step = 0; step < 100; ++step) {
            brain->step(step);
        }
        
        // Record results
        result.endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(result.endTime - result.startTime);
        result.executionTime = duration.count() / 1000.0; // Convert to seconds
        
        result.success = true;
        result.outputFile = name + "_result.checkpoint";
        
        // Save brain state
        brain->save(result.outputFile);
        
        // Record metrics
        result.metrics["neuron_count"] = static_cast<double>(brain->getTotalNeuronCount());
        result.metrics["synapse_count"] = static_cast<double>(brain->getTotalSynapseCount());
        result.metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        result.metrics["average_firing_rate"] = brain->getAverageFiringRate();
        
        result.logMessages.push_back("Experiment completed successfully");
        result.logMessages.push_back("Neuron count: " + std::to_string(brain->getTotalNeuronCount()));
        result.logMessages.push_back("Synapse count: " + std::to_string(brain->getTotalSynapseCount()));
        result.logMessages.push_back("Execution time: " + std::to_string(result.executionTime) + " seconds");
        
        NLM_LOG_INFO("Experiment " + name + " completed in " + std::to_string(result.executionTime) + " seconds");
        
    } catch (const std::exception& e) {
        result.endTime = std::chrono::high_resolution_clock::now();
        result.errorMessage = std::string("Exception: ") + e.what();
        NLM_LOG_ERROR("Experiment " + name + " failed: " + result.errorMessage);
    } catch (...) {
        result.endTime = std::chrono::high_resolution_clock::now();
        result.errorMessage = "Unknown exception";
        NLM_LOG_ERROR("Experiment " + name + " failed with unknown exception");
    }
    
    pImpl->results.push_back(result);
    return result;
}

std::vector<BatchResult> BatchProcessor::runBatch(const std::vector<std::string>& names) {
    std::vector<BatchResult> results;
    
    NLM_LOG_INFO("Running batch of " + std::to_string(names.size()) + " experiments");
    
    for (const auto& name : names) {
        BatchResult result = runExperiment(name);
        results.push_back(result);
        
        // Small delay between experiments
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return results;
}

std::vector<BatchResult> BatchProcessor::runAllEnabled() {
    std::vector<std::string> enabledNames;
    for (const auto& pair : pImpl->experiments) {
        if (pair.second->enabled) {
            enabledNames.push_back(pair.first);
        }
    }
    
    return runBatch(enabledNames);
}

bool BatchProcessor::saveConfiguration(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Batch Configuration" << std::endl;
    file << "# Generated on " << std::chrono::system_clock::now() << std::endl;
    file << std::endl;
    
    file << "[Experiments]" << std::endl;
    for (const auto& pair : pImpl->experiments) {
        file << pair.first << " = " << pair.second->description << std::endl;
        file << pair.first << "_enabled = " << (pair.second->enabled ? "true" : "false") << std::endl;
        file << pair.first << "_priority = " << pair.second->priority << std::endl;
        file << std::endl;
    }
    
    file << "[GlobalConfig]" << std::endl;
    file << "# Global configuration settings" << std::endl;
    
    return true;
}

bool BatchProcessor::loadConfiguration(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple parser for batch config format
    std::string line;
    std::string currentExperiment;
    
    while (std::getline(file, line)) {
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        // Check for section headers
        if (line.find("[") == 0 && line.find("]") != std::string::npos) {
            currentExperiment = "";
            continue;
        }
        
        // Parse key=value pairs
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);
            
            // Trim whitespace from key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (currentExperiment.empty()) {
                // Global config
                if (key == "experiment_name") {
                    currentExperiment = value;
                    // Create new experiment
                    addExperiment(value, std::make_shared<Config>(), "Loaded from batch config");
                }
            } else {
                // Experiment-specific config
                auto experiment = getExperiment(currentExperiment);
                if (experiment) {
                    if (key == "enabled") {
                        experiment->enabled = (value == "true" || value == "1");
                    } else if (key == "priority") {
                        experiment->priority = std::stoi(value);
                    } else if (key == "description") {
                        experiment->description = value;
                    } else {
                        experiment->parameters[key] = value;
                        experiment->config->set(key, value);
                    }
                }
            }
        }
    }
    
    NLM_LOG_INFO("Loaded batch configuration from: " + filepath);
    return true;
}

const std::vector<BatchResult>& BatchProcessor::getResults() const {
    return pImpl->results;
}

void BatchProcessor::clearResults() {
    pImpl->results.clear();
}

std::string BatchProcessor::getSummary() const {
    std::stringstream ss;
    
    ss << "Batch Processing Summary:" << std::endl;
    ss << "========================" << std::endl;
    ss << "Total experiments: " << pImpl->experiments.size() << std::endl;
    
    int enabledCount = 0;
    for (const auto& pair : pImpl->experiments) {
        if (pair.second->enabled) {
            enabledCount++;
        }
    }
    ss << "Enabled experiments: " << enabledCount << std::endl;
    ss << "Total results: " << pImpl->results.size() << std::endl;
    
    if (!pImpl->results.empty()) {
        double totalTime = 0.0;
        int successCount = 0;
        
        for (const auto& result : pImpl->results) {
            if (result.success) {
                successCount++;
                totalTime += result.executionTime;
            }
        }
        
        ss << "Successful runs: " << successCount << std::endl;
        ss << "Average execution time: " << (successCount > 0 ? totalTime / successCount : 0) << " seconds" << std::endl;
        ss << "Success rate: " << (pImpl->results.size() > 0 ? (successCount * 100.0 / pImpl->results.size()) : 0) << "%" << std::endl;
    }
    
    return ss.str();
}

} // namespace nlm
