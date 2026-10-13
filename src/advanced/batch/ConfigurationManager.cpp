# Configuration Management Tools for NLM
# This file contains advanced configuration management utilities

#include "ConfigurationManager.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace nlm {

struct ConfigurationManager::Impl {
    std::shared_ptr<Config> config;
    std::map<std::string, std::string> validationSchemas;
    std::vector<std::string> validationErrors;
    
    Impl() {}
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}
ConfigurationManager::~ConfigurationManager() = default;

void ConfigurationManager::setConfig(std::shared_ptr<Config> config) {
    pImpl->config = config;
}

bool ConfigurationManager::analyzeConfiguration(const std::string& outputFile) {
    NLM_LOG_INFO("Analyzing configuration to: " + outputFile);
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return false;
    }
    
    file << "Configuration Analysis Report\n";
    file << "==========================\n\n";
    
    if (!pImpl->config) {
        file << "No configuration set for analysis.\n";
        file.close();
        return true;
    }
    
    // Generate analysis report
    generateConfigurationAnalysis(file);
    
    file.close();
    return true;
}

bool ConfigurationManager::validateConfiguration(
    const std::string& configFile,
    const std::string& schemaFile) {
    
    NLM_LOG_INFO("Validating configuration: " + configFile);
    
    Config config;
    if (!config.loadFromFile(configFile)) {
        NLM_LOG_ERROR("Failed to load configuration: " + configFile);
        return false;
    }
    
    // Load schema if provided
    if (!schemaFile.empty()) {
        loadSchemaFromFile(schemaFile);
    }
    
    // Validate configuration
    config.setValidationEnabled(true);
    bool isValid = config.validate();
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validated successfully");
    } else {
        NLM_LOG_ERROR("Configuration validation failed: " + config.getValidationErrors());
    }
    
    return isValid;
}

bool ConfigurationManager::generateConfigurationReport(const std::string& outputFile) {
    NLM_LOG_INFO("Generating configuration report: " + outputFile);
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return false;
    }
    
    file << "Configuration Report\n";
    file << "==================\n\n";
    
    if (!pImpl->config) {
        file << "No configuration available.\n";
        file.close();
        return true;
    }
    
    // Generate report
    generateConfigurationReport(file);
    
    file.close();
    return true;
}

std::vector<std::string> ConfigurationManager::getConfigurationIssues() const {
    return pImpl->validationErrors;
}

std::map<std::string, double> ConfigurationManager::getConfigurationStatistics() const {
    std::map<std::string, double> stats;
    
    if (!pImpl->config) {
        return stats;
    }
    
    auto configStats = pImpl->config->getStats();
    stats["total_entries"] = static_cast<double>(configStats.totalEntries);
    stats["file_entries"] = static_cast<double>(configStats.fileEntries);
    stats["command_line_entries"] = static_cast<double>(configStats.commandLineEntries);
    stats["runtime_entries"] = static_cast<double>(configStats.runtimeEntries);
    stats["inherited_entries"] = static_cast<double>(configStats.inheritedEntries);
    stats["validation_score"] = configStats.validationScore;
    
    return stats;
}

bool ConfigurationManager::compareConfigurations(
    const std::string& configFile1,
    const std::string& configFile2,
    const std::string& outputFile,
    const std::vector<std::string>& sectionsToCompare) {
    
    NLM_LOG_INFO("Comparing configurations: " + configFile1 + " vs " + configFile2);
    
    Config config1, config2;
    
    if (!config1.loadFromFile(configFile1)) {
        NLM_LOG_ERROR("Failed to load configuration 1: " + configFile1);
        return false;
    }
    
    if (!config2.loadFromFile(configFile2)) {
        NLM_LOG_ERROR("Failed to load configuration 2: " + configFile2);
        return false;
    }
    
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open output file: " + outputFile);
        return false;
    }
    
    file << "Configuration Comparison\n";
    file << "======================\n\n";
    
    compareConfigurations(file, config1, config2, sectionsToCompare);
    
    file.close();
    return true;
}

bool ConfigurationManager::analyzeConfigurationInheritance(const std::string& configFile) {
    NLM_LOG_INFO("Analyzing configuration inheritance: " + configFile);
    
    Config config;
    if (!config.loadFromFile(configFile)) {
        NLM_LOG_ERROR("Failed to load configuration: " + configFile);
        return false;
    }
    
    // Analyze inheritance
    analyzeInheritance(config);
    
    return true;
}

bool ConfigurationManager::validateInheritanceChain(const std::string& configFile) {
    NLM_LOG_INFO("Validating inheritance chain: " + configFile);
    
    Config config;
    if (!config.loadFromFile(configFile)) {
        NLM_LOG_ERROR("Failed to load configuration: " + configFile);
        return false;
    }
    
    // Validate inheritance chain
    validateInheritanceChain(config);
    
    return true;
}

// Helper methods
void ConfigurationManager::generateConfigurationAnalysis(std::ofstream& file) {
    if (!pImpl->config) {
        file << "No configuration available.\n";
        return;
    }
    
    auto stats = pImpl->config->getStats();
    file << "Configuration Statistics:\n";
    file << "  Total entries: " << stats.totalEntries << "\n";
    file << "  File entries: " << stats.fileEntries << "\n";
    file << "  Command-line entries: " << stats.commandLineEntries << "\n";
    file << "  Runtime entries: " << stats.runtimeEntries << "\n";
    file << "  Inherited entries: " << stats.inheritedEntries << "\n";
    file << "  Validation score: " << stats.validationScore << "%\n\n";
    
    file << "Validation Status:\n";
    bool isValid = pImpl->config->validate();
    file << "  Valid: " << (isValid ? "Yes" : "No") << "\n";
    
    if (!isValid) {
        file << "  Errors:\n" << pImpl->config->getValidationErrors() << "\n";
    }
}

void ConfigurationManager::generateConfigurationReport(std::ofstream& file) {
    if (!pImpl->config) {
        file << "No configuration available.\n";
        return;
    }
    
    // Generate full configuration report
    file << pImpl->config->summary() << "\n";
}

void ConfigurationManager::compareConfigurations(
    std::ofstream& file,
    const Config& config1,
    const Config& config2,
    const std::vector<std::string>& sectionsToCompare) {
    
    // Compare configurations
    file << "Comparison results would be displayed here.\n";
}

void ConfigurationManager::analyzeInheritance(const Config& config) {
    // Analyze inheritance relationships
    NLM_LOG_INFO("Analyzing configuration inheritance relationships");
}

void ConfigurationManager::validateInheritanceChain(const Config& config) {
    // Validate inheritance chain
    NLM_LOG_INFO("Validating configuration inheritance chain");
}

void ConfigurationManager::loadSchemaFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open schema file: " + filepath);
        return;
    }
    
    // TODO: Parse schema file
    // This would load validation schema from file
    
    file.close();
}

} // namespace nlm
