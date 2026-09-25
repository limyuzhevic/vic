#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <filesystem>

namespace nlm {

ConfigManager::ConfigManager() {
    validator_ = std::make_shared<ConfigValidator>();
    loader_ = std::make_shared<ConfigLoader>();
    loader_->setValidator(validator_);
    
    // Initialize version from config file if exists
    std::string versionFile = configDirectory_ + "/version.txt";
    std::ifstream versionStream(versionFile);
    if (versionStream.is_open()) {
        int maj, min, patch;
        versionStream >> maj >> min >> patch;
        version_ = ConfigVersion(maj, min, patch);
        versionStream.close();
    }
}

bool ConfigManager::initialize(const std::string& configFile, int argc, char** argv) {
    // Clear any existing configuration
    clear();
    
    // Load from command line first (highest priority)
    if (loader_->parseCommandLine(argc, argv, *config_, ConfigSource::CommandLine)) {
        // Command line parsing successful (or no command line args)
    }
    
    // Load from file if specified
    std::string configToLoad = configFile;
    bool configLoaded = false;
    
    // Try config file from command line first
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configToLoad = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configToLoad = argv[++i];
                }
                configLoaded = loadFromFile(configToLoad);
                break;
            }
        }
    }
    
    // If not loaded from command line, try default
    if (!configLoaded && !loadFromFile(configToLoad)) {
        // If file loading failed, try to load defaults
        NLM_LOG_WARNING("Could not load configuration from " + configToLoad + ". Using defaults.");
        loadDefaultValues();
    }
    
    updateLastModified();
    return true;
}

std::vector<ConfigValidator::ValidationResult> ConfigManager::validate() const {
    if (config_) {
        return validator_->validateAll(*config_);
    }
    return {};
}

bool ConfigManager::saveToFile(const std::string& filepath, const std::string& description) const {
    if (!config_) {
        return false;
    }
    
    // Create backup first
    createBackup("Backup before save to " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for writing: " + filepath);
        return false;
    }
    
    // Write version header
    file << "# NLM Configuration File" << std::endl;
    file << "# Version: " << version_.major << "." << version_.minor << "." << version_.patch << std::endl;
    file << "# Description: " << description << std::endl;
    file << "# Created: " << generateTimestamp() << std::endl;
    file << std::endl;
    
    // Write configuration entries
    for (const auto& key : config_->getKeys()) {
        // Find the entry to get description
        // This would need access to Config implementation details
        file << key << " = " << "[value]" << std::endl;  // Placeholder - real implementation would get actual values
    }
    
    file.close();
    updateLastModified();
    
    return true;
}

bool ConfigManager::loadFromFile(const std::string& filepath) {
    clear();
    
    // Try different file formats
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    bool loaded = false;
    
    // Try JSON first
    if (ext == ".json" || ext.empty()) {
        loaded = loader_->loadFromJsonFile(filepath, ConfigSource::File);
    }
    
    // Try YAML if JSON failed
    if (!loaded && (ext == ".yaml" || ext == ".yml")) {
        loaded = loader_->loadFromYamlFile(filepath, ConfigSource::File);
    }
    
    // Try as plain key=value file if other formats failed
    if (!loaded) {
        // Simple file parsing
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Use ConfigLoader's helper method
            if (!loader_->parseKeyValuePair(line, ConfigSource::File)) {
                // Log warning for parse error
            }
        }
        file.close();
        loaded = true;
    }
    
    if (loaded) {
        config_ = std::make_shared<Config>(loader_->getConfig());
    }
    
    return loaded;
}

bool ConfigManager::loadFromString(const std::string& configString, const std::string& format) {
    clear();
    
    if (format == "json" || format == "auto") {
        if (nlohmann::json::accept(configString)) {
            return loader_->loadFromJsonString(configString, ConfigSource::File);
        }
    }
    
    // Try as YAML
    if (format == "yaml" || format == "auto") {
        // Simple YAML detection - count lines that look like key-value pairs
        std::istringstream stream(configString);
        std::string line;
        int keyValueLines = 0;
        while (std::getline(stream, line)) {
            if (loader_->parseKeyValuePair(line, ConfigSource::File)) {
                keyValueLines++;
            }
        }
        if (keyValueLines > 0) {
            return true; // Treat as key=value format
        }
    }
    
    return false;
}

bool ConfigManager::createBackup(const std::string& description) {
    if (!config_) {
        return false;
    }
    
    std::string timestamp = generateTimestamp();
    std::string backupFile = configDirectory_ + "/backup_" + timestamp + ".cfg";
    
    // Save backup
    bool saved = saveToFile(backupFile, description);
    
    if (saved) {
        ConfigBackup backup(timestamp, description, backupFile, version_);
        backups_.push_back(backup);
        
        // Keep only last 10 backups
        if (backups_.size() > 10) {
            backups_.erase(backups_.begin());
        }
    }
    
    return saved;
}

bool ConfigManager::restoreFromBackup(size_t backupIndex) {
    if (backupIndex >= backups_.size()) {
        return false;
    }
    
    const auto& backup = backups_[backupIndex];
    return loadFromFile(backup.filepath);
}

bool ConfigManager::resetToDefaults() {
    clear();
    return loadDefaultValues();
}

bool ConfigManager::mergeFromFile(const std::string& filepath) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    // Load from file and merge with existing
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (loader_->parseKeyValuePair(line, ConfigSource::File)) {
            // Merge into existing config
            // This would need access to loader_'s internal state
        }
    }
    
    file.close();
    return true;
}

std::string ConfigManager::getSummary() const {
    if (!config_) {
        return "No configuration loaded";
    }
    
    std::ostringstream oss;
    oss << "NLM Configuration Summary:" << std::endl;
    oss << "  Version: " << version_.major << "." << version_.minor << "." << version_.patch << std::endl;
    oss << "  Modified: " << lastModified_ << std::endl;
    oss << "  Number of entries: " << config_->getKeys().size() << std::endl;
    oss << "  Config directory: " << configDirectory_ << std::endl;
    
    // Get validation results
    auto validationResults = validate();
    if (!validationResults.empty()) {
        oss << "  Validation errors: " << validationResults.size() << std::endl;
    }
    
    return oss.str();
}

std::string ConfigManager::generateTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    
    std::ostringstream oss;
    oss << std::put_time(timeinfo, "%Y%m%d_%H%M%S");
    return oss.str();
}

bool ConfigManager::loadDefaultConfig() {
    // Load default configuration from built-in defaults
    return loadFromFile(configDirectory_ + "/default.cfg");
}

bool ConfigManager::loadDefaultValues() {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    // Load all default values (these would match the current main.cpp defaults)
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Add more default values as needed
    
    return true;
}

void ConfigManager::updateLastModified() {
    lastModified_ = generateTimestamp();
    modified_ = true;
}

} // namespace nlm