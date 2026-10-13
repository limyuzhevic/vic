# Advanced CLI Commands for NLM
# This file contains advanced command-line interface features for experienced users

#include "AdvancedCLI.hpp"
#include "../core/Config/Config.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct AdvancedCLI::Impl {
    std::shared_ptr<Config> config;
    std::shared_ptr<ExperimentRunner> experimentRunner;
    bool verbose;
    bool dryRun;
    std::map<std::string, std::vector<std::string>> parameterSweep;
    
    Impl() : verbose(false), dryRun(false) {}
};

AdvancedCLI::AdvancedCLI() : pImpl(std::make_unique<Impl>()) {}
AdvancedCLI::~AdvancedCLI() = default;

void AdvancedCLI::setConfig(std::shared_ptr<Config> config) {
    pImpl->config = config;
}

void AdvancedCLI::setExperimentRunner(std::shared_ptr<ExperimentRunner> runner) {
    pImpl->experimentRunner = runner;
}

void AdvancedCLI::setVerbose(bool verbose) {
    pImpl->verbose = verbose;
}

void AdvancedCLI::setDryRun(bool dryRun) {
    pImpl->dryRun = dryRun;
}

// Parameter sweep functionality
bool AdvancedCLI::runParameterSweep(const std::map<std::string, std::vector<std::string>>& params,
                                   const std::string& outputFile,
                                   SimulationStep maxSteps) {
    pImpl->parameterSweep = params;
    
    if (pImpl->verbose) {
        NLM_LOG_INFO("Starting parameter sweep with " + std::to_string(params.size()) + " parameters");
    }
    
    // Generate all parameter combinations
    std::vector<std::map<std::string, std::string>> combinations;
    generateParameterCombinations(params, combinations);
    
    NLM_LOG_INFO("Generated " + std::to_string(combinations.size()) + " parameter combinations");
    
    if (pImpl->dryRun) {
        NLM_LOG_INFO("[DRY RUN] Would run " + std::to_string(combinations.size()) + " parameter combinations");
        return true;
    }
    
    // TODO: Implement actual parameter sweep execution
    // This would create brain instances with different configurations
    // and run experiments for each parameter combination
    
    return true;
}

void AdvancedCLI::generateParameterCombinations(
    const std::map<std::string, std::vector<std::string>>& params,
    std::vector<std::map<std::string, std::string>>& combinations) {
    
    std::vector<std::string> keys;
    for (const auto& pair : params) {
        keys.push_back(pair.first);
    }
    
    generateParameterCombinationsRecursive(params, keys, 0, {}, combinations);
}

void AdvancedCLI::generateParameterCombinationsRecursive(
    const std::map<std::string, std::vector<std::string>>& params,
    const std::vector<std::string>& keys,
    size_t index,
    std::map<std::string, std::string> current,
    std::vector<std::map<std::string, std::string>>& combinations) {
    
    if (index == keys.size()) {
        combinations.push_back(current);
        return;
    }
    
    const std::string& key = keys[index];
    const std::vector<std::string>& values = params.at(key);
    
    for (const std::string& value : values) {
        current[key] = value;
        generateParameterCombinationsRecursive(params, keys, index + 1, current, combinations);
    }
}

// Custom configuration loading/saving
bool AdvancedCLI::loadConfiguration(const std::string& filepath, const std::string& profile) {
    if (pImpl->verbose) {
        NLM_LOG_INFO("Loading configuration from: " + filepath);
    }
    
    if (!pImpl->config->loadFromFile(filepath)) {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
        return false;
    }
    
    // Apply profile if specified
    if (!profile.empty()) {
        applyConfigurationProfile(profile);
    }
    
    NLM_LOG_INFO("Configuration loaded successfully");
    return true;
}

bool AdvancedCLI::saveConfiguration(const std::string& filepath, const std::string& format,
                                   bool includeDefaults, bool prettyPrint) {
    if (pImpl->verbose) {
        NLM_LOG_INFO("Saving configuration to: " + filepath);
    }
    
    // Filter configuration if needed
    Config filteredConfig;
    if (includeDefaults) {
        filteredConfig = *pImpl->config;
    } else {
        // Create config with only non-default values
        filterNonDefaultValues(filteredConfig);
    }
    
    bool success = false;
    std::string ext = std::filesystem::path(filepath).extension().string();
    
    if (ext == ".json" || ext == ".jsonc" || ext == ".json5") {
        success = filteredConfig.saveToFile(filepath, prettyPrint);
    } else {
        // Default to legacy format
        success = filteredConfig.saveToLegacyFile(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Configuration saved to: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to save configuration to: " + filepath);
    }
    
    return success;
}

void AdvancedCLI::applyConfigurationProfile(const std::string& profile) {
    // Apply predefined configuration profiles
    if (profile == "performance") {
        pImpl->config->set("neuron_count", static_cast<int64_t>(1000));
        pImpl->config->set("region_count", static_cast<int64_t>(2));
        pImpl->config->set("connection_probability", 0.3f);
        pImpl->config->set("simulation_timestep", 0.0005);
    } else if (profile == "accuracy") {
        pImpl->config->set("neuron_count", static_cast<int64_t>(5000));
        pImpl->config->set("region_count", static_cast<int64_t>(3));
        pImpl->config->set("connection_probability", 0.2f);
        pImpl->config->set("simulation_timestep", 0.0002);
    } else if (profile == "debug") {
        pImpl->config->set("neuron_count", static_cast<int64_t>(100));
        pImpl->config->set("region_count", static_cast<int64_t>(1));
        pImpl->config->set("connection_probability", 0.1f);
        pImpl->config->set("simulation_timestep", 0.001);
    }
}

void AdvancedCLI::filterNonDefaultValues(Config& config) {
    // Remove values that match defaults
    // This is a placeholder implementation
    // In practice, you'd need access to default values
}

// Experiment result comparison
bool AdvancedCLI::compareResults(const std::vector<std::string>& resultFiles,
                                const std::string& outputFile,
                                const std::vector<std::string>& metricsToCompare) {
    if (resultFiles.size() < 2) {
        NLM_LOG_ERROR("Need at least 2 result files to compare");
        return false;
    }
    
    NLM_LOG_INFO("Comparing " + std::to_string(resultFiles.size()) + " experiment results");
    
    // Load and analyze results
    std::vector<std::map<std::string, double>> resultData;
    for (const std::string& file : resultFiles) {
        auto result = loadResultFile(file);
        resultData.push_back(result);
    }
    
    // Generate comparison report
    generateComparisonReport(resultData, metricsToCompare, outputFile);
    
    return true;
}

std::map<std::string, double> AdvancedCLI::loadResultFile(const std::string& filepath) {
    std::map<std::string, double> result;
    
    // TODO: Implement result file loading
    // This would load experiment results from files
    // and extract key metrics
    
    return result;
}

void AdvancedCLI::generateComparisonReport(
    const std::vector<std::map<std::string, double>>& results,
    const std::vector<std::string>& metrics,
    const std::string& outputFile) {
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return;
    }
    
    file << "Experiment Result Comparison\n";
    file << "=============================\n\n";
    
    for (const std::string& metric : metrics) {
        file << "Metric: " << metric << "\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            auto it = result.find(metric);
            if (it != result.end()) {
                file << "  Experiment " << i << ": " << it->second << "\n";
            }
        }
        
        file << "  Statistics:\n";
        // Calculate statistics
        std::vector<double> values;
        for (const auto& result : results) {
            auto it = result.find(metric);
            if (it != result.end()) {
                values.push_back(it->second);
            }
        }
        
        if (!values.empty()) {
            double sum = std::accumulate(values.begin(), values.end(), 0.0);
            double mean = sum / values.size();
            file << "    Mean: " << mean << "\n";
            
            // Calculate standard deviation
            double variance = 0.0;
            for (double v : values) {
                variance += (v - mean) * (v - mean);
            }
            variance /= values.size();
            double stdDev = std::sqrt(variance);
            file << "    Std Dev: " << stdDev << "\n";
            
            file << "    Min: " << *std::min_element(values.begin(), values.end()) << "\n";
            file << "    Max: " << *std::max_element(values.begin(), values.end()) << "\n";
        }
        
        file << "\n";
    }
    
    file.close();
}

// Batch run management
bool AdvancedCLI::createBatchRun(const std::string& batchConfigFile,
                                const std::string& outputDir,
                                bool parallel) {
    if (pImpl->verbose) {
        NLM_LOG_INFO("Creating batch run from: " + batchConfigFile);
    }
    
    if (pImpl->dryRun) {
        NLM_LOG_INFO("[DRY RUN] Would create batch run from: " + batchConfigFile);
        return true;
    }
    
    // TODO: Implement batch run creation
    // This would parse batch configuration and create
    // multiple experiment runs
    
    return true;
}

bool AdvancedCLI::manageBatchRun(const std::string& operation,
                               const std::string& batchId,
                               const std::map<std::string, std::string>& options) {
    if (operation == "status") {
        return queryBatchStatus(batchId);
    } else if (operation == "resume") {
        return resumeBatchRun(batchId);
    } else if (operation == "cancel") {
        return cancelBatchRun(batchId);
    } else if (operation == "monitor") {
        return monitorBatchRun(batchId, options);
    }
    
    NLM_LOG_ERROR("Unknown batch operation: " + operation);
    return false;
}

bool AdvancedCLI::queryBatchStatus(const std::string& batchId) {
    NLM_LOG_INFO("Querying status for batch: " + batchId);
    // TODO: Implement batch status query
    return true;
}

bool AdvancedCLI::resumeBatchRun(const std::string& batchId) {
    NLM_LOG_INFO("Resuming batch: " + batchId);
    // TODO: Implement batch run resumption
    return true;
}

bool AdvancedCLI::cancelBatchRun(const std::string& batchId) {
    NLM_LOG_INFO("Cancelling batch: " + batchId);
    // TODO: Implement batch run cancellation
    return true;
}

bool AdvancedCLI::monitorBatchRun(const std::string& batchId,
                                 const std::map<std::string, std::string>& options) {
    NLM_LOG_INFO("Monitoring batch: " + batchId);
    // TODO: Implement batch run monitoring
    return true;
}

} // namespace nlm
