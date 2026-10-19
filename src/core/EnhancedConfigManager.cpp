// EnhancedConfigManager implementation
#include "EnhancedConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

EnhancedConfigManager::EnhancedConfigManager() {
    config_ = std::make_shared<Config>();
    initializeDefaults();
}

bool EnhancedConfigManager::loadConfig(const std::string& configFile) {
    std::string fileToLoad = configFile.empty() ? "configs/default.cfg" : configFile;
    
    if (config_->loadFromFile(fileToLoad)) {
        NLM_LOG_INFO("Loaded configuration from: " + fileToLoad);
        return true;
    } else {
        NLM_LOG_WARNING("Failed to load configuration from: " + fileToLoad + ", using defaults");
        initializeDefaults();
        return false;
    }
}

bool EnhancedConfigManager::loadFromCLI(const CLIParser& cli) {
    bool modified = false;
    
    // Load configuration from CLI options
    if (cli.hasOption("--config")) {
        std::string configFile = cli.getString("--config");
        if (loadConfig(configFile)) {
            modified = true;
        }
    }
    
    if (cli.hasOption("--random-seed")) {
        int64_t seed = cli.getInt("--random-seed");
        config_->set("random_seed", seed, ConfigSource::CommandLine);
        modified = true;
    }
    
    if (cli.hasOption("--log-level")) {
        std::string level = cli.getString("--log-level");
        // Convert string to LogLevel enum
        if (level == "Debug") {
            config_->set("log_level", 0, ConfigSource::CommandLine);
        } else if (level == "Info") {
            config_->set("log_level", 1, ConfigSource::CommandLine);
        } else if (level == "Warning") {
            config_->set("log_level", 2, ConfigSource::CommandLine);
        } else if (level == "Error") {
            config_->set("log_level", 3, ConfigSource::CommandLine);
        } else if (level == "Critical") {
            config_->set("log_level", 4, ConfigSource::CommandLine);
        }
        modified = true;
    }
    
    if (cli.hasOption("--steps")) {
        int steps = cli.getInt("--steps");
        config_->set("max_simulation_steps", steps, ConfigSource::CommandLine);
        modified = true;
    }
    
    if (cli.hasOption("--experiments")) {
        std::vector<std::string> experiments = cli.getStringList("--experiments");
        std::string experimentsStr;
        for (size_t i = 0; i < experiments.size(); ++i) {
            if (i > 0) experimentsStr += ",";
            experimentsStr += experiments[i];
        }
        config_->set("experiments_to_run", experimentsStr, ConfigSource::CommandLine);
        modified = true;
    }
    
    return modified;
}

bool EnhancedConfigManager::loadFromDefaults() {
    initializeDefaults();
    return true;
}

bool EnhancedConfigManager::saveConfig(const std::string& configFile) const {
    // Save current configuration to file
    // This is a simplified implementation
    std::ofstream file(configFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration" << std::endl;
    file << "# Generated automatically" << std::endl;
    file << std::endl;
    
    auto keys = config_->getKeys();
    for (const auto& key : keys) {
        auto value = config_->get<std::string>(key);
        if (value.has_value()) {
            file << key << " = " << value.value() << std::endl;
        }
    }
    
    file.close();
    return true;
}

bool EnhancedConfigManager::exportConfig(const std::string& outputFile, const std::string& format) const {
    if (format == "json") {
        // JSON export would go here
        return saveConfig(outputFile);
    } else if (format == "ini") {
        // INI export would go here
        return saveConfig(outputFile);
    }
    return false;
}

template<typename T>
T EnhancedConfigManager::getValue(const std::string& key, const T& defaultValue) const {
    return config_->getOr<T>(key, defaultValue);
}

void EnhancedConfigManager::setValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    config_->set(key, value, source);
}

bool EnhancedConfigManager::validateConfig() const {
    validationErrors_.clear();
    
    if (!validateNeuronConfig()) return false;
    if (!validatePlasticityConfig()) return false;
    if (!validateNeuromodulationConfig()) return false;
    if (!validateDevelopmentConfig()) return false;
    
    return validationErrors_.empty();
}

std::string EnhancedConfigManager::getValidationErrors() const {
    std::stringstream ss;
    for (const auto& error : validationErrors_) {
        ss << error << std::endl;
    }
    return ss.str();
}

std::string EnhancedConfigManager::getSummary() const {
    std::stringstream ss;
    ss << "=== Configuration Summary ===" << std::endl;
    ss << "Random seed: " << getValue<int64_t>("random_seed", 42) << std::endl;
    ss << "Simulation timestep: " << getValue<double>("simulation_timestep", 0.001) << "s" << std::endl;
    ss << "Neuron count: " << getValue<int64_t>("neuron_count", 500) << std::endl;
    ss << "Region count: " << getValue<int64_t>("region_count", 1) << std::endl;
    ss << "Connection probability: " << getValue<float>("connection_probability", 0.15f) << std::endl;
    ss << "STDP LTP weight: " << getValue<float>("stdp_ltp_weight", 0.02f) << std::endl;
    ss << "STDP LTD weight: " << getValue<float>("stdp_ltd_weight", 0.015f) << std::endl;
    ss << "Synaptogenesis rate: " << getValue<float>("synaptogenesis_rate", 0.0001f) << std::endl;
    ss << "Pruning rate: " << getValue<float>("pruning_rate", 0.00001f) << std::endl;
    ss << "=== End Summary ===" << std::endl;
    return ss.str();
}

void EnhancedConfigManager::initializeDefaults() {
    config_->clear();
    
    // Basic settings
    config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config_->set("simulation_timestep", 0.001, ConfigSource::Default);
    config_->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config_->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Plasticity settings
    config_->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config_->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config_->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity settings
    config_->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config_->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Logging settings
    config_->set("log_level", static_cast<int>(LogLevel::Info), ConfigSource::Default);
    config_->set("log_to_file", false, ConfigSource::Default);
    config_->set("log_filename", "nlm.log", ConfigSource::Default);
    
    // Simulation settings
    config_->set("max_simulation_steps", 1000, ConfigSource::Default);
    config_->set("simulation_time_limit", 0.0, ConfigSource::Default);
    
    // Experiments to run
    config_->set("experiments_to_run", "all", ConfigSource::Default);
}

bool EnhancedConfigManager::validateNeuronConfig() const {
    auto neuronCount = getValue<int64_t>("neuron_count", 500);
    if (neuronCount <= 0 || neuronCount > 1000000) {
        validationErrors_.push_back("Invalid neuron_count: must be between 1 and 1,000,000");
        return false;
    }
    
    auto connectionProb = getValue<float>("connection_probability", 0.15f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        validationErrors_.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
        return false;
    }
    
    return true;
}

bool EnhancedConfigManager::validatePlasticityConfig() const {
    auto stdpLtpWeight = getValue<float>("stdp_ltp_weight", 0.02f);
    if (stdpLtpWeight <= 0.0f) {
        validationErrors_.push_back("Invalid stdp_ltp_weight: must be positive");
        return false;
    }
    
    auto stdpLtdWeight = getValue<float>("stdp_ltd_weight", 0.015f);
    if (stdpLtdWeight <= 0.0f) {
        validationErrors_.push_back("Invalid stdp_ltd_weight: must be positive");
        return false;
    }
    
    auto stdpTau = getValue<float>("stdp_tau", 20.0f);
    if (stdpTau <= 0.0f) {
        validationErrors_.push_back("Invalid stdp_tau: must be positive");
        return false;
    }
    
    auto synaptogenesisRate = getValue<float>("synaptogenesis_rate", 0.0001f);
    if (synaptogenesisRate < 0.0f || synaptogenesisRate > 1.0f) {
        validationErrors_.push_back("Invalid synaptogenesis_rate: must be between 0.0 and 1.0");
        return false;
    }
    
    return true;
}

bool EnhancedConfigManager::validateNeuromodulationConfig() const {
    // Neuromodulation configuration validation
    return true;
}

bool EnhancedConfigManager::validateDevelopmentConfig() const {
    // Development configuration validation
    return true;
}

// Template implementation

template<typename T>
T EnhancedConfigManager::getValue(const std::string& key, const T& defaultValue) const {
    return config_->getOr<T>(key, defaultValue);
}

} // namespace nlm