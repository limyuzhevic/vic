#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <numeric>

namespace nlm {

struct ConfigManager::Impl {
    std::unordered_map<std::string, ConfigParameter> parameterDefinitions;
};

ConfigManager::ConfigManager() : config_(std::make_shared<Config>()), 
                                 lastLoadTime(std::chrono::steady_clock::now()),
                                 configChanged(false), validationErrors(""), initialized(false) {
    pImpl = std::make_unique<Impl>();
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::initialize() {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    if (initialized) return;
    
    // Register all configuration parameters
    // Simulation parameters
    registerParameter<int64_t>("simulation_timestep", 1, 
        "Base simulation timestep in seconds", ConfigCategory::Simulation);
    registerParameter<size_t>("max_simulation_steps", 10000,
        "Maximum number of simulation steps to run", ConfigCategory::Simulation);
    registerParameter<double>("simulation_time_limit", 0.0,
        "Maximum simulation time in seconds (0 = unlimited)", ConfigCategory::Simulation);
    registerParameter<size_t>("replay_interval", 100,
        "Replay important memories every N steps", ConfigCategory::Simulation);
    registerParameter<size_t>("consolidation_interval", 1000,
        "Consolidate episodic memories every N steps", ConfigCategory::Simulation);
    
    // Network parameters
    registerParameter<size_t>("neuron_count", 1000,
        "Total number of neurons in the brain", ConfigCategory::Network);
    registerParameter<size_t>("region_count", 2,
        "Number of brain regions", ConfigCategory::Network);
    registerParameter<float>("connection_probability", 0.1f,
        "Probability of connection between neurons", ConfigCategory::Network);
    registerParameter<float>("synaptic_weight_mean", 0.5f,
        "Mean synaptic weight", ConfigCategory::Network);
    registerParameter<float>("synaptic_weight_std", 0.1f,
        "Standard deviation of synaptic weights", ConfigCategory::Network);
    
    // Plasticity parameters
    registerParameter<float>("stdp_ltp_weight", 0.01f,
        "STDP Long-Term Potentiation weight", ConfigCategory::Plasticity);
    registerParameter<float>("stdp_ltd_weight", 0.012f,
        "STDP Long-Term Depression weight", ConfigCategory::Plasticity);
    registerParameter<float>("stdp_time_constant", 20.0f,
        "STDP time constant in ms", ConfigCategory::Plasticity);
    registerParameter<float>("plasticity_learning_rate", 0.01f,
        "General plasticity learning rate", ConfigCategory::Plasticity);
    
    // Structural plasticity
    registerParameter<float>("synaptogenesis_rate", 0.001f,
        "Rate of new synapse formation", ConfigCategory::Plasticity);
    registerParameter<float>("pruning_rate", 0.0001f,
        "Rate of synapse elimination", ConfigCategory::Plasticity);
    
    // Development parameters
    registerParameter<float>("development_synaptogenesis_rate", 0.001f,
        "Development phase synaptogenesis rate", ConfigCategory::Development);
    registerParameter<float>("development_pruning_rate", 0.0001f,
        "Development phase pruning rate", ConfigCategory::Development);
    
    // Neuromodulation parameters
    registerParameter<float>("dopamine_baseline", 0.1f,
        "Baseline dopamine level", ConfigCategory::Neuromodulation);
    registerParameter<float>("reward_discount_factor", 0.99f,
        "Reward discount factor for RL", ConfigCategory::Neuromodulation);
    
    // Memory parameters
    registerParameter<size_t>("working_memory_capacity", 100,
        "Capacity of working memory in neurons", ConfigCategory::Memory);
    registerParameter<size_t>("episodic_memory_max_episodes", 1000,
        "Maximum number of episodic memory episodes", ConfigCategory::Memory);
    registerParameter<float>("working_memory_decay_rate", 0.01f,
        "Decay rate of working memory traces", ConfigCategory::Memory);
    
    // Cognition parameters
    registerParameter<size_t>("planning_depth", 5,
        "Depth of action planning", ConfigCategory::Cognition);
    registerParameter<float>("attention_inhibition_strength", 0.5f,
        "Strength of attentional inhibition", ConfigCategory::Cognition);
    registerParameter<float>("attention_excitation_strength", 1.5f,
        "Strength of attentional excitation", ConfigCategory::Cognition);
    
    // Environment parameters
    registerParameter<std::string>("environment_name", "GridWorld",
        "Name of the environment", ConfigCategory::Environment);
    registerParameter<size_t>("environment_width", 10,
        "Width of the environment grid", ConfigCategory::Environment);
    registerParameter<size_t>("environment_height", 10,
        "Height of the environment grid", ConfigCategory::Environment);
    
    // Logging parameters
    registerParameter<std::string>("log_level", "INFO",
        "Logging level (DEBUG, INFO, WARNING, ERROR)", ConfigCategory::Logging);
    registerParameter<bool>("log_to_file", false,
        "Whether to log to file", ConfigCategory::Logging);
    registerParameter<std::string>("log_filename", "nlm.log",
        "Log file name", ConfigCategory::Logging);
    
    // Performance parameters
    registerParameter<std::string>("checkpoint_dir", "./checkpoints",
        "Directory for saving brain checkpoints", ConfigCategory::Performance);
    registerParameter<size_t>("checkpoint_interval", 10000,
        "Interval between checkpoints", ConfigCategory::Performance);
    registerParameter<size_t>("checkpoint_memory", 5,
        "Number of checkpoints to keep in memory", ConfigCategory::Performance);
    registerParameter<bool>("enable_checkpoint_compression", true,
        "Enable checkpoint compression", ConfigCategory::Performance);
    
    // Testing parameters
    registerParameter<bool>("enable_test_mode", false,
        "Enable fast test mode with fewer neurons", ConfigCategory::Testing);
    registerParameter<size_t>("test_neuron_count", 500,
        "Number of neurons in test mode", ConfigCategory::Testing);
    registerParameter<size_t>("test_repetition_count", 1,
        "Number of times to repeat tests", ConfigCategory::Testing);
    
    // Advanced parameters
    registerParameter<uint64_t>("random_seed", 42,
        "Random seed for reproducible experiments", ConfigCategory::Advanced);
    registerParameter<float>("neuron_threshold", 0.0f,
        "Neuron membrane potential threshold for firing", ConfigCategory::Advanced);
    registerParameter<float>("neuron_leak_conductance", 10.0f,
        "Neuron membrane leak conductance", ConfigCategory::Advanced);
    registerParameter<float>("neuron_refractory_period", 0.002f,
        "Neuron refractory period in seconds", ConfigCategory::Advanced);
    registerParameter<float>("max_simulation_duration", 3600.0f,
        "Maximum simulation duration in seconds", ConfigCategory::Advanced);
    
    // Test experiment parameters
    registerParameter<size_t>("test_current_neuron_count", 10,
        "Number of neurons to inject current during basic connectivity test", ConfigCategory::Testing);
    registerParameter<float>("test_current_strength", 50.0f,
        "Current strength injected during basic connectivity test", ConfigCategory::Testing);
    registerParameter<size_t>("test_plasticity_steps", 1000,
        "Number of steps for plasticity experiment", ConfigCategory::Testing);
    registerParameter<size_t>("test_stdp_trials", 50,
        "Number of STDP trials for verification", ConfigCategory::Testing);
    registerParameter<size_t>("test_most_active_neurons", 10,
        "Number of most active neurons to track", ConfigCategory::Testing);
    registerParameter<float>("test_weight_change_threshold", 0.01f,
        "Threshold for considering weight change significant", ConfigCategory::Testing);
    
    initialized = true;
}

bool ConfigManager::loadConfig(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    try {
        // Load config file
        if (!config_->loadFromFile(configFile)) {
            NLM_LOG_WARNING("Failed to load config file: " + configFile + ". Using defaults.");
        }
        
        // Load environment variables
        loadFromEnvironment();
        
        // Load from command line (if provided)
        // Note: This would need to be set by main()
        
        lastLoadTime = std::chrono::steady_clock::now();
        configChanged = true;
        
        NLM_LOG_INFO("Configuration loaded from: " + configFile);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration: " + std::string(e.what()));
        return false;
    }
}

void ConfigManager::loadFromEnvironment(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    for (const auto& paramDef : parameterDefinitions_) {
        std::string envKey = prefix + paramDef.first;
        if (std::getenv(envKey.c_str()) != nullptr) {
            std::string envValue = std::getenv(envKey.c_str());
            config_->set(paramDef.first, envValue, ConfigSource::File);
        }
    }
}

void ConfigManager::loadFromCommandLine(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    config_->loadFromArgs(argc, argv);
}

bool ConfigManager::validateConfig() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    validationErrors.clear();
    
    for (const auto& paramDef : parameterDefinitions_) {
        std::string key = normalizeKey(paramDef.first);
        
        // Check if parameter exists
        if (!config_->has(key)) {
            if (paramDef.second.required) {
                validationErrors += "Required parameter missing: " + key + "\n";
            }
            continue;
        }
        
        // Validate parameter type
        ConfigValue value = getConfigValue(key);
        if (!validateParameterType(paramDef.first, value)) {
            validationErrors += "Invalid type for parameter " + key + "\n";
        }
        
        // Validate parameter range
        if (!validateParameterRange(paramDef.first, value)) {
            validationErrors += "Parameter " + key + " out of valid range\n";
        }
    }
    
    return validationErrors.empty();
}

std::string ConfigManager::getValidationErrors() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    return validationErrors;
}

bool ConfigManager::reloadConfig() {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    if (!config_->has("config_file")) {
        NLM_LOG_WARNING("No config file path specified for reload");
        return false;
    }
    
    std::string configFile = config_->getOr<std::string>("config_file", "configs/default.cfg");
    return loadConfig(configFile);
}

bool ConfigManager::hasConfigChanged() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    auto now = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - lastLoadTime).count();
    
    if (diff > 60) { // Check every 60 seconds
        bool changed = checkConfigFileChanged();
        if (changed) configChanged = true;
    }
    
    bool result = configChanged;
    configChanged = false; // Reset after checking
    return result;
}

bool ConfigManager::saveConfig(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    // Get all config entries
    auto keys = config_->getKeys();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save config to: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated at: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& key : keys) {
        ConfigValue value = getConfigValue(key);
        std::string description = "";
        
        // Find parameter description
        auto paramIt = parameterDefinitions_.find(key);
        if (paramIt != parameterDefinitions_.end()) {
            description = paramIt->second.description;
        }
        
        if (!description.empty()) {
            file << "# " << description << "\n";
        }
        
        file << key << " = " << valueToString(value) << "\n\n";
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

bool ConfigManager::generateConfigTemplate(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to generate config template: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration Template\n";
    file << "# Copy this file to configs/custom.cfg and modify as needed\n\n";
    
    for (const auto& paramDef : parameterDefinitions_) {
        file << "# " << paramDef.second.description << "\n";
        file << "# Default value: " << valueToString(paramDef.second.defaultValue) << "\n";
        file << paramDef.first << " = " << valueToString(paramDef.second.defaultValue) << "\n\n";
        
        // Add examples for numeric parameters
        if (std::holds_alternative<double>(paramDef.second.defaultValue)) {
            double val = std::get<double>(paramDef.second.defaultValue);
            if (val > 0 && val < 1) {
                file << "# Example: " << val * 2 << " or " << val / 2 << "\n";
            }
        } else if (std::holds_alternative<float>(paramDef.second.defaultValue)) {
            float val = std::get<float>(paramDef.second.defaultValue);
            if (val > 0 && val < 1) {
                file << "# Example: " << val * 2 << " or " << val / 2 << "\n";
            }
        }
        
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Configuration template generated: " + filepath);
    return true;
}

std::string ConfigManager::getConfigSummary() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    std::ostringstream oss;
    oss << "NLM Configuration Summary (" << parameterDefinitions_.size() << " parameters defined)\n";
    oss << "===============================================================\n\n";
    
    // Group by category
    std::unordered_map<ConfigCategory, std::vector<std::string>> categoryGroups;
    for (const auto& param : parameterDefinitions_) {
        // In a real implementation, we'd need a mapping from param name to category
        // For now, using a simple placeholder
        categoryGroups[ConfigCategory::Simulation].push_back(param.first);
    }
    
    for (const auto& [category, params] : categoryGroups) {
        oss << categoryToString(category) << " Parameters:\n";
        for (const auto& param : params) {
            if (config_->has(param)) {
                ConfigValue value = getConfigValue(param);
                oss << "  " << param << " = " << valueToString(value) << "\n";
            } else {
                oss << "  " << param << " = [NOT SET]\n";
            }
        }
        oss << "\n";
    }
    
    oss << "Total parameters loaded: " << config_->getKeys().size() << "\n";
    return oss.str();
}

std::string ConfigManager::getConfigDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Documentation\n";
    oss << "================================\n\n";
    
    for (const auto& paramDef : parameterDefinitions_) {
        oss << "Parameter: " << paramDef.first << "\n";
        oss << "Description: " << paramDef.second.description << "\n";
        oss << "Default Value: " << valueToString(paramDef.second.defaultValue) << "\n";
        oss << "Required: " << (paramDef.second.required ? "Yes" : "No") << "\n";
        oss << "Categories: ";
        for (size_t i = 0; i < paramDef.second.categories.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << configCategoryToString(static_cast<ConfigCategory>(i));
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

std::vector<std::string> ConfigManager::getCategories() const {
    std::set<std::string> categories;
    for (const auto& paramDef : parameterDefinitions_) {
        for (const auto& catStr : paramDef.second.categories) {
            categories.insert(catStr);
        }
    }
    return std::vector<std::string>(categories.begin(), categories.end());
}

std::vector<std::string> ConfigManager::getParametersInCategory(ConfigCategory category) const {
    std::vector<std::string> params;
    for (const auto& paramDef : parameterDefinitions_) {
        for (const auto& catStr : paramDef.second.categories) {
            // In a real implementation, we'd need proper category matching
            params.push_back(paramDef.first);
        }
    }
    return params;
}

std::string ConfigManager::normalizeKey(const std::string& key) const {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool ConfigManager::validateParameterType(const std::string& key, const ConfigValue& value) const {
    // In a real implementation, we'd check parameter type based on definition
    return true;
}

bool ConfigManager::validateParameterRange(const std::string& key, const ConfigValue& value) const {
    // In a real implementation, we'd check parameter range based on definition
    return true;
}

std::string ConfigManager::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string ConfigManager::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(t, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

ConfigValue ConfigManager::getConfigValue(const std::string& key) const {
    // This would need to extract the ConfigValue from the Config
    // For now, returning an empty string as a placeholder
    return std::string("");
}

bool ConfigManager::checkConfigFileChanged() const {
    // Check if config files have changed
    // In a real implementation, we'd track file modification times
    return false;
}

std::string ConfigManager::categoryToString(ConfigCategory category) const {
    switch (category) {
        case ConfigCategory::Simulation: return "Simulation";
        case ConfigCategory::Network: return "Network";
        case ConfigCategory::Plasticity: return "Plasticity";
        case ConfigCategory::Development: return "Development";
        case ConfigCategory::Neuromodulation: return "Neuromodulation";
        case ConfigCategory::Memory: return "Memory";
        case ConfigCategory::Cognition: return "Cognition";
        case ConfigCategory::Environment: return "Environment";
        case ConfigCategory::Logging: return "Logging";
        case ConfigCategory::Performance: return "Performance";
        case ConfigCategory::Testing: return "Testing";
        case ConfigCategory::Advanced: return "Advanced";
        default: return "Unknown";
    }
}

std::string ConfigManager::configCategoryToString(ConfigCategory category) const {
    return categoryToString(category);
}

} // namespace nlm