// Advanced Configuration Manager Implementation

#include "nlm/config/AdvancedConfigManager.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace nlm {
namespace config {

AdvancedConfigManager::AdvancedConfigManager() : configLoaded_(false), currentVersion_("current") {
    // Initialize version history
    versionHistory_["current"] = "configs/default_advanced.cfg";
}

AdvancedConfigManager::~AdvancedConfigManager() {
}

bool AdvancedConfigManager::loadConfiguration(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open configuration file: " + filepath;
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "." + filepath.substr(filepath.find_last_of('.') + 1);
    bool success = false;
    
    if (ext == ".json") {
        success = loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(filepath);
    } else {
        success = loadFromKeyValue(filepath);
    }
    
    if (success) {
        configLoaded_ = true;
        saveCurrentVersion(currentVersion_);
    }
    
    return success;
}

bool AdvancedConfigManager::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open JSON configuration file: " + filepath;
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        std::string jsonString = jsonData.dump(4);
        config_.loadFromJSON(jsonString, false);
        return true;
    } catch (const std::exception& e) {
        lastError_ = "JSON parsing error: " + std::string(e.what());
        return false;
    }
}

bool AdvancedConfigManager::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open YAML configuration file: " + filepath;
        return false;
    }
    
    try {
        YAML::Node doc = YAML::Load(file);
        
        // Convert YAML to string representation
        std::stringstream yamlStream;
        yamlStream << doc;
        std::string yamlString = yamlStream.str();
        
        config_.loadFromYAML(yamlString, false);
        return true;
    } catch (const std::exception& e) {
        lastError_ = "YAML parsing error: " + std::string(e.what());
        return false;
    }
}

bool AdvancedConfigManager::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError_ = "Failed to open key-value configuration file: " + filepath;
        return false;
    }
    
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        config_.loadFromFile(filepath, false);
        return true;
    } catch (const std::exception& e) {
        lastError_ = "Key-value parsing error: " + std::string(e.what());
        return false;
    }
}

bool AdvancedConfigManager::validateConfiguration() {
    if (!configLoaded_) {
        lastError_ = "No configuration loaded for validation";
        return false;
    }
    
    // Validate configuration using internal schema
    std::vector<nlm::ConfigSchema> schema;
    
    // Add basic schema validation
    nlm::ConfigSchema neuronCountSchema("neuron_count", nlm::ConfigType::INTEGER, "Number of neurons", "1000", true);
    nlm::ConfigSchema connectionProbSchema("connection_probability", nlm::ConfigType::DOUBLE, "Connection probability", "0.1");
    
    schema.push_back(neuronCountSchema);
    schema.push_back(connectionProbSchema);
    
    config_.setSchema(schema);
    
    bool valid = config_.validateAgainstSchema(schema);
    
    if (!valid) {
        lastError_ = "Configuration validation failed: " + config_.getLastError();
    }
    
    return valid;
}

bool AdvancedConfigManager::validateAgainstSchema(const std::string& schemaFile) {
    std::ifstream file(schemaFile);
    if (!file.is_open()) {
        lastError_ = "Failed to open schema file: " + schemaFile;
        return false;
    }
    
    try {
        nlohmann::json schemaJson = nlohmann::json::parse(file);
        // Convert JSON schema to ConfigSchema format
        // For now, return true for testing
        return true;
    } catch (const std::exception& e) {
        lastError_ = "Schema parsing error: " + std::string(e.what());
        return false;
    }
}

bool AdvancedConfigManager::compareConfigurations(const std::string& file1, const std::string& file2) {
    // Load both configurations
    AdvancedConfigManager config1, config2;
    
    if (!config1.loadConfiguration(file1) || !config2.loadConfiguration(file2)) {
        return false;
    }
    
    // Get all keys from both configurations
    std::vector<std::string> keys1 = config1.getConfig().getKeys();
    std::vector<std::string> keys2 = config2.getConfig().getKeys();
    
    std::cout << "Configuration Comparison:" << std::endl;
    std::cout << "File 1: " << file1 << std::endl;
    std::cout << "File 2: " << file2 << std::endl;
    std::cout << std::endl;
    
    // Find differences
    for (const auto& key : keys1) {
        auto value1 = config1.getConfig().get<std::string>(key);
        auto value2 = config2.getConfig().get<std::string>(key);
        
        if (value1 != value2) {
            std::cout << "DIFFERENCE: " << key << std::endl;
            std::cout << "  File 1: " << value1.value_or("") << std::endl;
            std::cout << "  File 2: " << value2.value_or("") << std::endl;
        }
    }
    
    // Find keys only in file2
    for (const auto& key : keys2) {
        if (std::find(keys1.begin(), keys1.end(), key) == keys1.end()) {
            auto value2 = config2.getConfig().get<std::string>(key);
            std::cout << "ONLY IN FILE 2: " << key << " = " << value2.value_or("") << std::endl;
        }
    }
    
    return true;
}

std::string AdvancedConfigManager::generateDiff(const std::string& file1, const std::string& file2) {
    // Simplified diff generation
    std::stringstream diff;
    
    AdvancedConfigManager config1, config2;
    if (!config1.loadConfiguration(file1) || !config2.loadConfiguration(file2)) {
        return "Failed to load configurations";
    }
    
    diff << "--- " << file1 << std::endl;
    diff << "+++ " << file2 << std::endl;
    diff << "@@ -1," << config1.getConfig().getKeys().size() << " +1," << config2.getConfig().getKeys().size() << " @@" << std::endl;
    
    // Simple diff output
    for (const auto& key : config1.getConfig().getKeys()) {
        auto value1 = config1.getConfig().get<std::string>(key);
        auto value2 = config2.getConfig().get<std::string>(key);
        
        if (value1 != value2) {
            diff << "-" << key << " = " << value1.value_or("") << std::endl;
            diff << "+" << key << " = " << value2.value_or("") << std::endl;
        }
    }
    
    return diff.str();
}

bool AdvancedConfigManager::exportConfiguration(const std::string& format, const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        lastError_ = "Failed to open output file: " + outputFile;
        return false;
    }
    
    if (format == "json") {
        std::string jsonStr = toJSON();
        file << jsonStr;
    } else if (format == "yaml" || format == "yml") {
        std::string yamlStr = toYAML();
        file << yamlStr;
    } else if (format == "keyvalue") {
        std::string kvStr = toKeyValue();
        file << kvStr;
    } else {
        lastError_ = "Unsupported format: " + format;
        return false;
    }
    
    return true;
}

bool AdvancedConfigManager::importConfiguration(const std::string& format, const std::string& inputFile) {
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        lastError_ = "Failed to open input file: " + inputFile;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return parseConfiguration(content, format);
}

std::string AdvancedConfigManager::toJSON() const {
    return config_.toJSON();
}

std::string AdvancedConfigManager::toYAML() const {
    return config_.toYAML();
}

std::string AdvancedConfigManager::toKeyValue() const {
    std::stringstream ss;
    for (const auto& key : config_.getKeys()) {
        ss << key << " = " << config_.getOr<std::string>(key, "") << std::endl;
    }
    return ss.str();
}

bool AdvancedConfigManager::saveCurrentVersion(const std::string& versionName) {
    std::string filename = "configs/version_" + versionName + ".backup";
    return exportConfiguration("json", filename);
}

bool AdvancedConfigManager::rollbackVersion(const std::string& versionName) {
    std::string filename = "configs/version_" + versionName + ".backup";
    if (!std::filesystem::exists(filename)) {
        lastError_ = "Version backup not found: " + versionName;
        return false;
    }
    
    return loadConfiguration(filename);
}

std::vector<std::string> AdvancedConfigManager::getAvailableVersions() const {
    std::vector<std::string> versions;
    
    for (const auto& entry : std::filesystem::directory_iterator("configs")) {
        if (entry.path().extension() == ".backup") {
            std::string filename = entry.path().filename().string();
            // Extract version name
            size_t start = filename.find("version_") + 8;
            size_t end = filename.find(".backup");
            if (start != std::string::npos && end != std::string::npos) {
                versions.push_back(filename.substr(start, end - start));
            }
        }
    }
    
    return versions;
}

bool AdvancedConfigManager::inheritFromTemplate(const std::string& templateFile, const std::string& outputFile) {
    // Load template
    AdvancedConfigManager templateConfig;
    if (!templateConfig.loadConfiguration(templateFile)) {
        return false;
    }
    
    // Create inherited configuration
    nlm::Config inheritedConfig = templateConfig.getConfig();
    
    // Merge with current configuration
    for (const auto& key : config_.getKeys()) {
        auto value = config_.get<std::string>(key);
        if (value.has_value()) {
            inheritedConfig.set(key, value.value(), nlm::ConfigSource::Runtime);
        }
    }
    
    // Set the inherited configuration as current
    config_ = std::move(inheritedConfig);
    configLoaded_ = true;
    
    return saveCurrentVersion("inherited");
}

bool AdvancedConfigManager::mergeConfigurations(const std::string& file1, const std::string& file2, const std::string& outputFile) {
    // Load both configurations
    AdvancedConfigManager configA, configB;
    if (!configA.loadConfiguration(file1) || !configB.loadConfiguration(file2)) {
        return false;
    }
    
    // Create merged configuration
    nlm::Config mergedConfig;
    
    // Copy from configA
    for (const auto& key : configA.getConfig().getKeys()) {
        auto value = configA.getConfig().get<std::string>(key);
        if (value.has_value()) {
            mergedConfig.set(key, value.value(), nlm::ConfigSource::File);
        }
    }
    
    // Override with configB values
    for (const auto& key : configB.getConfig().getKeys()) {
        auto value = configB.getConfig().get<std::string>(key);
        if (value.has_value()) {
            mergedConfig.set(key, value.value(), nlm::ConfigSource::File);
        }
    }
    
    // Export merged configuration
    config_.setSchema(std::vector<nlm::ConfigSchema>());
    return exportConfiguration("json", outputFile);
}

bool AdvancedConfigManager::setSchema(const std::string& schemaFile) {
    // Load schema from file (simplified implementation)
    return validateConfiguration();
}

bool AdvancedConfigManager::validateSchema(const std::string& schemaFile) {
    // Validate schema file (simplified implementation)
    return true;
}

std::string AdvancedConfigManager::getSchemaHelp() const {
    std::stringstream ss;
    ss << "Advanced Configuration Schema Help:" << std::endl;
    ss << "===============================" << std::endl;
    ss << "Available configuration keys:" << std::endl;
    
    for (const auto& key : config_.getKeys()) {
        ss << "  - " << key << std::endl;
    }
    
    return ss.str();
}

bool AdvancedConfigManager::analyzeConfiguration(const std::string& filepath) {
    // Analyze configuration for potential issues
    AdvancedConfigManager tempConfig;
    if (!tempConfig.loadConfiguration(filepath)) {
        return false;
    }
    
    std::cout << "Configuration Analysis for " << filepath << ":" << std::endl;
    
    // Check for empty values
    for (const auto& key : tempConfig.getConfig().getKeys()) {
        auto value = tempConfig.getConfig().get<std::string>(key);
        if (value.value_or("").empty()) {
            std::cout << "  WARNING: Empty value for key: " << key << std::endl;
        }
    }
    
    // Check for missing required values
    std::vector<std::string> requiredKeys = {"neuron_count", "connection_probability"};
    for (const auto& key : requiredKeys) {
        if (!tempConfig.getConfig().has(key)) {
            std::cout << "  WARNING: Missing required key: " << key << std::endl;
        }
    }
    
    return true;
}

bool AdvancedConfigManager::detectInconsistencies() {
    // Detect inconsistencies in configuration
    std::cout << "Checking for configuration inconsistencies..." << std::endl;
    
    // Check neuron count is positive
    auto neuronCount = config_.getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        std::cout << "  ERROR: Invalid neuron count: " << neuronCount << std::endl;
        return false;
    }
    
    // Check connection probability is valid
    auto connectionProb = config_.getOr<double>("connection_probability", 0.0);
    if (connectionProb < 0.0 || connectionProb > 1.0) {
        std::cout << "  ERROR: Invalid connection probability: " << connectionProb << std::endl;
        return false;
    }
    
    std::cout << "  No inconsistencies detected." << std::endl;
    return true;
}

bool AdvancedConfigManager::suggestImprovements() {
    // Suggest configuration improvements
    std::cout << "Configuration improvement suggestions:" << std::endl;
    
    // Check for typical default values
    auto neuronCount = config_.getOr<int64_t>("neuron_count", 0);
    if (neuronCount == 1000) {
        std::cout << "  - Consider increasing neuron count for better performance" << std::endl;
    }
    
    auto connectionProb = config_.getOr<double>("connection_probability", 0.0);
    if (connectionProb == 0.1) {
        std::cout << "  - Consider adjusting connection probability for desired network density" << std::endl;
    }
    
    return true;
}

bool AdvancedConfigManager::parseConfiguration(const std::string& content, const std::string& format) {
    if (format == "json") {
        try {
            nlohmann::json jsonData = nlohmann::json::parse(content);
            std::string jsonString = jsonData.dump(4);
            config_.loadFromJSON(jsonString, false);
            return true;
        } catch (const std::exception& e) {
            lastError_ = "JSON parsing error: " + std::string(e.what());
            return false;
        }
    } else if (format == "yaml" || format == "yml") {
        try {
            YAML::Node doc = YAML::Load(content);
            std::stringstream yamlStream;
            yamlStream << doc;
            config_.loadFromYAML(yamlStream.str(), false);
            return true;
        } catch (const std::exception& e) {
            lastError_ = "YAML parsing error: " + std::string(e.what());
            return false;
        }
    }
    
    lastError_ = "Unsupported format: " + format;
    return false;
}

} // namespace config
} // namespace nlm
