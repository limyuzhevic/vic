// AdvancedFeatures implementation
#include "AdvancedFeatures.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>

namespace nlm {

AdvancedFeatures::AdvancedFeatures(std::shared_ptr<CommandOutputFormatter> formatter,
                                   std::shared_ptr<Logger> logger)
    : formatter(formatter), logger(logger) {}

bool AdvancedFeatures::experimentCreate(const std::string& name) {
    if (!validateExperimentName(name)) {
        logger->log("Error: Invalid experiment name: " + name, LogLevel::Error);
        return false;
    }
    
    logger->log("Creating new experiment: " + name, LogLevel::Info);
    
    if (!createExperimentDirectory(name)) {
        return false;
    }
    
    // Initialize experiment status
    experimentStatusMap[name] = "created";
    
    logger->log("Experiment created successfully: " + name, LogLevel::Info);
    return true;
}

bool AdvancedFeatures::experimentRun(const std::string& name) {
    if (experimentStatusMap.find(name) == experimentStatusMap.end()) {
        logger->log("Error: Experiment not found: " + name, LogLevel::Error);
        return false;
    }
    
    logger->log("Running experiment: " + name, LogLevel::Info);
    
    if (!runExperiment(name)) {
        logger->log("Error: Failed to run experiment: " + name, LogLevel::Error);
        return false;
    }
    
    experimentStatusMap[name] = "completed";
    logger->log("Experiment completed: " + name, LogLevel::Info);
    
    return true;
}

bool AdvancedFeatures::experimentList() {
    logger->log("=== Available Experiments ===", LogLevel::Info);
    
    if (experimentStatusMap.empty()) {
        logger->log("No experiments found", LogLevel::Info);
    } else {
        for (const auto& entry : experimentStatusMap) {
            logger->log("  " + entry.first + ": " + entry.second, LogLevel::Info);
        }
    }
    
    return true;
}

bool AdvancedFeatures::experimentStatus(const std::string& name) {
    logger->log("=== Experiment Status: " + name + " ===", LogLevel::Info);
    
    auto it = experimentStatusMap.find(name);
    if (it == experimentStatusMap.end()) {
        logger->log("Error: Experiment not found: " + name, LogLevel::Error);
        return false;
    }
    
    logger->log("Status: " + it->second, LogLevel::Info);
    
    // Show experiment details (placeholder)
    logger->log("Created: Placeholder timestamp", LogLevel::Info);
    logger->log("Last run: Placeholder timestamp", LogLevel::Info);
    
    return true;
}

bool AdvancedFeatures::experimentResults(const std::string& name) {
    logger->log("=== Experiment Results: " + name + " ===", LogLevel::Info);
    
    auto it = experimentResultsMap.find(name);
    if (it == experimentResultsMap.end()) {
        logger->log("Error: No results found for experiment: " + name, LogLevel::Error);
        return false;
    }
    
    const auto& results = it->second;
    
    if (results.empty()) {
        logger->log("No results data available", LogLevel::Info);
    } else {
        logger->log("Results (" + std::to_string(results.size()) + " data points):", LogLevel::Info);
        for (size_t i = 0; i < std::min(size_t(10), results.size()); ++i) {
            logger->log("  " + std::to_string(results[i]), LogLevel::Info);
        }
        if (results.size() > 10) {
            logger->log("  ... and " + std::to_string(results.size() - 10) + " more points", LogLevel::Info);
        }
    }
    
    return true;
}

bool AdvancedFeatures::experimentExport(const std::string& name) {
    logger->log("Exporting experiment results: " + name, LogLevel::Info);
    
    std::string filename = name + "_results.json";
    if (exportExperimentResults(name, filename)) {
        logger->log("Experiment results exported to: " + filename, LogLevel::Info);
        return true;
    }
    
    return false;
}

bool AdvancedFeatures::experimentImport(const std::string& name) {
    logger->log("Importing experiment results: " + name, LogLevel::Info);
    
    std::string filename = name + "_results.json";
    if (importExperimentResults(name, filename)) {
        logger->log("Experiment results imported from: " + filename, LogLevel::Info);
        return true;
    }
    
    return false;
}

bool AdvancedFeatures::parameterSweep(const std::string& configFile) {
    if (!validateConfigFile(configFile)) {
        logger->log("Error: Invalid config file: " + configFile, LogLevel::Error);
        return false;
    }
    
    logger->log("Running parameter sweep with config: " + configFile, LogLevel::Info);
    
    if (!performParameterSweep(configFile)) {
        logger->log("Error: Parameter sweep failed", LogLevel::Error);
        return false;
    }
    
    logger->log("Parameter sweep completed", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::monteCarlo(int runs) {
    if (runs <= 0) {
        logger->log("Error: Number of Monte Carlo runs must be positive", LogLevel::Error);
        return false;
    }
    
    logger->log("Running Monte Carlo simulation with " + std::to_string(runs) + " runs", LogLevel::Info);
    
    if (!performMonteCarlo(runs)) {
        logger->log("Error: Monte Carlo simulation failed", LogLevel::Error);
        return false;
    }
    
    logger->log("Monte Carlo simulation completed", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::sensitivityAnalysis(const std::string& param) {
    if (!validateParamName(param)) {
        logger->log("Error: Invalid parameter name: " + param, LogLevel::Error);
        return false;
    }
    
    logger->log("Performing sensitivity analysis for parameter: " + param, LogLevel::Info);
    
    if (!performSensitivityAnalysis(param)) {
        logger->log("Error: Sensitivity analysis failed", LogLevel::Error);
        return false;
    }
    
    logger->log("Sensitivity analysis completed", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::onlineLearning() {
    logger->log("Enabling online learning mode", LogLevel::Info);
    
    if (!enableOnlineLearning()) {
        logger->log("Error: Failed to enable online learning", LogLevel::Error);
        return false;
    }
    
    logger->log("Online learning mode enabled", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::batchLearning() {
    logger->log("Enabling batch learning mode", LogLevel::Info);
    
    if (!enableBatchLearning()) {
        logger->log("Error: Failed to enable batch learning", LogLevel::Error);
        return false;
    }
    
    logger->log("Batch learning mode enabled", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::curriculumLearning() {
    logger->log("Enabling curriculum learning mode", LogLevel::Info);
    
    if (!enableCurriculumLearning()) {
        logger->log("Error: Failed to enable curriculum learning", LogLevel::Error);
        return false;
    }
    
    logger->log("Curriculum learning mode enabled", LogLevel::Info);
    return true;
}

// Experiment helper methods
bool AdvancedFeatures::createExperimentDirectory(const std::string& name) {
    std::string dirName = "experiments/" + name;
    
    if (std::filesystem::exists(dirName)) {
        logger->log("Warning: Experiment directory already exists: " + dirName, LogLevel::Warning);
        return true;
    }
    
    try {
        std::filesystem::create_directories(dirName);
        logger->log("Created experiment directory: " + dirName, LogLevel::Info);
        return true;
    } catch (const std::exception& e) {
        logger->log("Error: Could not create directory " + dirName + ": " + std::string(e.what()), LogLevel::Error);
        return false;
    }
}

bool AdvancedFeatures::saveExperimentConfig(const std::string& name, const std::string& config) {
    std::string configPath = "experiments/" + name + "/config.json";
    
    std::ofstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    file << config;
    file.close();
    return true;
}

bool AdvancedFeatures::loadExperimentConfig(const std::string& name, std::string& config) {
    std::string configPath = "experiments/" + name + "/config.json";
    
    std::ifstream file(configPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    config = buffer.str();
    
    file.close();
    return true;
}

bool AdvancedFeatures::runExperiment(const std::string& name) {
    logger->log("Running experiment: " + name, LogLevel::Info);
    
    // Simulate experiment run
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    std::vector<double> results;
    for (int i = 0; i < 100; ++i) {
        results.push_back(dis(gen));
    }
    
    experimentResultsMap[name] = results;
    
    return true;
}

bool AdvancedFeatures::exportExperimentResults(const std::string& name, const std::string& filename) {
    auto it = experimentResultsMap.find(name);
    if (it == experimentResultsMap.end()) {
        logger->log("Error: No results found for experiment: " + name, LogLevel::Error);
        return false;
    }
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"experiment\": \"" << name << "\",\n";
    file << "  \"timestamp\": \"" << std::time(nullptr) << "\",\n";
    file << "  \"results\": [\n";
    
    const auto& results = it->second;
    for (size_t i = 0; i < results.size(); ++i) {
        file << "    " << results[i];
        if (i < results.size() - 1) file << ",";
        file << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    file.close();
    return true;
}

bool AdvancedFeatures::importExperimentResults(const std::string& name, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Parse JSON (simplified)
    std::string line;
    while (std::getline(file, line)) {
        // Simple parsing (placeholder)
        if (line.find("\"results\": [") != std::string::npos) {
            // Start of results array
            std::vector<double> results;
            // Parse results (placeholder)
            experimentResultsMap[name] = results;
            break;
        }
    }
    
    file.close();
    return true;
}

// Parameter analysis helper methods
bool AdvancedFeatures::performParameterSweep(const std::string& configFile) {
    logger->log("Performing parameter sweep (placeholder implementation)", LogLevel::Info);
    
    // Simulate parameter sweep
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    std::vector<double> sweepResults;
    for (int i = 0; i < 50; ++i) {
        sweepResults.push_back(dis(gen));
    }
    
    // Store results
    experimentResultsMap["parameter_sweep"] = sweepResults;
    
    return true;
}

bool AdvancedFeatures::performMonteCarlo(int runs) {
    logger->log("Performing Monte Carlo simulation (placeholder implementation)", LogLevel::Info);
    
    // Simulate Monte Carlo runs
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    std::vector<double> mcResults;
    for (int i = 0; i < runs; ++i) {
        mcResults.push_back(dis(gen));
    }
    
    // Store results
    experimentResultsMap["monte_carlo"] = mcResults;
    
    return true;
}

bool AdvancedFeatures::performSensitivityAnalysis(const std::string& param) {
    logger->log("Performing sensitivity analysis for parameter: " + param + " (placeholder)", LogLevel::Info);
    
    // Simulate sensitivity analysis
    std::vector<double> sensitivityResults;
    for (int i = 0; i < 20; ++i) {
        sensitivityResults.push_back(0.1 * i);  // Linear sensitivity
    }
    
    // Store results
    experimentResultsMap["sensitivity_analysis"] = sensitivityResults;
    
    return true;
}

// Learning mode helper methods
bool AdvancedFeatures::enableOnlineLearning() {
    onlineLearningEnabled = true;
    batchLearningEnabled = false;
    curriculumLearningEnabled = false;
    
    logger->log("Online learning mode enabled", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::enableBatchLearning() {
    batchLearningEnabled = true;
    onlineLearningEnabled = false;
    curriculumLearningEnabled = false;
    
    logger->log("Batch learning mode enabled", LogLevel::Info);
    return true;
}

bool AdvancedFeatures::enableCurriculumLearning() {
    curriculumLearningEnabled = true;
    onlineLearningEnabled = false;
    batchLearningEnabled = false;
    
    logger->log("Curriculum learning mode enabled", LogLevel::Info);
    return true;
}

// Validation helpers
bool AdvancedFeatures::validateExperimentName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : name) {
        if (!isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool AdvancedFeatures::validateConfigFile(const std::string& configFile) const {
    if (configFile.empty()) {
        return false;
    }
    
    return std::filesystem::exists(configFile);
}

bool AdvancedFeatures::validateParamName(const std::string& param) const {
    if (param.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : param) {
        if (!isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}
