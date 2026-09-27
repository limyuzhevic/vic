#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::map<std::string, ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file: " + filepath);
            return false;
        }
        
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Convert JSON to our config entries
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            try {
                if (it->is_boolean()) {
                    value = it->get<bool>();
                } else if (it->is_number_integer()) {
                    value = it->get<int64_t>();
                } else if (it->is_number_float()) {
                    value = it->get<double>();
                } else if (it->is_string()) {
                    value = it->get<std::string>();
                } else if (it->is_array()) {
                    // Handle arrays
                    nlohmann::json array = *it;
                    if (!array.empty()) {
                        if (array[0].is_boolean()) {
                            std::vector<bool> boolArray = array.get<std::vector<bool>>();
                            value = boolArray;
                        } else if (array[0].is_number_integer()) {
                            std::vector<int64_t> intArray = array.get<std::vector<int64_t>>();
                            value = intArray;
                        } else if (array[0].is_number_float()) {
                            std::vector<double> doubleArray = array.get<std::vector<double>>();
                            value = doubleArray;
                        } else if (array[0].is_string()) {
                            std::vector<std::string> stringArray = array.get<std::vector<std::string>>();
                            value = stringArray;
                        }
                    }
                }
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse config value for key '" + key + "': " + std::string(e.what()));
                continue;
            }
            
            if (value.index() != std::variant_npos) {
                set(key, value, ConfigSource::File);
            }
        }
        
        // Validate configuration
        ValidationResult validation = schema_.validate(pImpl->entries);
        if (!validation.valid) {
            NLM_LOG_WARNING("Configuration validation failed:\n" + validation.toString());
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load config from file " + filepath + ": " + std::string(e.what()));
        return false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    NLM_LOG_INFO("Config file loaded from " + filepath + " (" + std::to_string(duration) + "ms)");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setFromString(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Invalid command line argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                setFromString(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Missing value for command line argument: " + arg);
                success = false;
            }
        }
        // Handle --help flag
        else if (arg == "--help" || arg == "-h") {
            printHelp();
            return false; // Exit normally after showing help
        }
    }
    
    // Validate configuration
    ValidationResult validation = schema_.validate(pImpl->entries);
    if (!validation.valid) {
        NLM_LOG_ERROR("Configuration validation failed:\n" + validation.toString());
        success = false;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    if (success) {
        NLM_LOG_INFO("Command line arguments processed (" + std::to_string(duration) + "ms)");
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
        
        file << jsonConfig.dump(2); // Pretty print with 2-space indentation
        
        NLM_LOG_INFO("Config file saved to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to save config to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second.value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for config key '" + key + "' (expected " + 
                      getTypeName<T>() + ")");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source) {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        it->second.value = value;
        it->second.source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate this entry
    auto entryIt = pImpl->entries.find(key);
    if (entryIt != pImpl->entries.end()) {
        ValidationResult validation = entryIt->second.validate(entryIt->second.value);
        if (!validation.valid) {
            for (const auto& error : validation.errors) {
                NLM_LOG_ERROR("Config validation error for key '" + key + "': " + error);
            }
        }
    }
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "NLM Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Sort by source for better readability
    std::vector<std::pair<std::string, ConfigEntry>> sortedEntries;
    for (const auto& entry : pImpl->entries) {
        sortedEntries.emplace_back(entry.key, entry.value);
    }
    
    // Sort by source (CommandLine > File > Default > Runtime)
    std::sort(sortedEntries.begin(), sortedEntries.end(),
        [](const auto& a, const auto& b) {
            int sourceA = static_cast<int>(a.second.source);
            int sourceB = static_cast<int>(b.second.source);
            return sourceA > sourceB; // Higher source priority first
        });
    
    for (const auto& entry : sortedEntries) {
        oss << "  " << entry.first << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.second.value);
        oss << " (" << static_cast<int>(entry.second.source) << ")\n";
    }
    
    return oss.str();
}

void Config::printHelp() const {
    std::cout << "NLM Configuration Options\n";
    std::cout << "========================\n\n";
    std::cout << "Usage:\n";
    std::cout << "  --key=value    Set configuration key to value\n";
    std::cout << "  -k value       Short form (requires separate value)\n";
    std::cout << "\n";        
    
    std::cout << "Available configuration keys:\n";
    for (const auto& entry : schema_.getEntries()) {
        std::cout << "  --" << entry.first << " <value>  ";
        std::cout << entry.second.description << "\n";
        std::cout << "      Type: " << entry.second.typeName << "\n";
        std::cout << "      Required: " << (entry.second.required ? "Yes" : "No") << "\n\n";
    }
}

ValidationResult Config::validate() const {
    return schema_.validate(pImpl->entries);
}

const ConfigSchema& Config::getSchema() const { return schema_; }

void Config::loadFromEnvironment(const std::string& prefix) {
    for (const auto& entry : schema_.getEntries()) {
        std::string envKey = prefix + entry.first;
        char* envValue = std::getenv(envKey.c_str());
        if (envValue) {
            std::string value = envValue;
            setFromString(entry.first, value, ConfigSource::Environment);
            NLM_LOG_INFO("Loaded config from environment: " + envKey);
        }
    }
}

void Config::loadDefaults() {
    for (const auto& entry : schema_.getEntries()) {
        if (std::holds_alternative<int>(entry.second.value)) {
            set(entry.first, std::get<int>(entry.second.value), entry.second.source);
        } else if (std::holds_alternative<int64_t>(entry.second.value)) {
            set(entry.first, std::get<int64_t>(entry.second.value), entry.second.source);
        } else if (std::holds_alternative<double>(entry.second.value)) {
            set(entry.first, std::get<double>(entry.second.value), entry.second.source);
        } else if (std::holds_alternative<bool>(entry.second.value)) {
            set(entry.first, std::get<bool>(entry.second.value), entry.second.source);
        } else if (std::holds_alternative<std::string>(entry.second.value)) {
            set(entry.first, std::get<std::string>(entry.second.value), entry.second.source);
        }
    }
}

Config::ConfigEntryHelper Config::set() {
    return ConfigEntryHelper(*this);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    return Config::get<T>(key);
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json json;
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json = arg;
        }
    }, value);
    return json;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to detect type and convert
    if (valueStr == "true" || valueStr == "false") {
        bool boolValue = (valueStr == "true");
        set(key, boolValue, source);
        return;
    }
    
    try {
        // Try integer
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, intValue, source);
            return;
        }
    } catch (...) {}
    
    try {
        // Try double
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, doubleValue, source);
            return;
        }
    } catch (...) {}
    
    // Default to string
    set(key, valueStr, source);
}

template<typename T>
std::string Config::getTypeName() const {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, std::string>) return "string";
    else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
    else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
    else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
    else return "unknown";
}

// Explicit template instantiations
namespace {
    struct ConfigInstantiator {
        ConfigInstantiator() {
            // Config
            Config config;
            config.get<int>("test");
            config.get<double>("test");
            config.get<std::string>("test");
            config.get<bool>("test");
            config.getOr<int>("test", 0);
            
            // ConfigSchema
            ConfigSchema schema;
            schema.addValidatedEntry<int>("test", 0, ConfigSource::Default, "Test", nullptr);
        }
    } instantiator;
}

// Initialize default schema
void Config::initializeDefaultSchema() {
    // Core simulation parameters
    addEntry("random_seed", 42, ConfigSource::Default,
             "Random seed for reproducible experiments");
    
    addEntry("simulation_timestep", 0.001, ConfigSource::Default,
             "Simulation timestep in seconds");
    
    addEntry("neuron_count", 1000, ConfigSource::Default,
             "Number of neurons in the brain");
    
    addEntry("region_count", 2, ConfigSource::Default,
             "Number of brain regions");
    
    addEntry("connection_probability", 0.1, ConfigSource::Default,
             "Connection probability for random connectivity");
    
    // Plasticity settings
    addEntry("stdp_ltp_weight", 0.01, ConfigSource::Default,
             "STDP long-term potentiation weight");
    
    addEntry("stdp_ltd_weight", 0.012, ConfigSource::Default,
             "STDP long-term depression weight");
    
    addEntry("stdp_tau", 20.0, ConfigSource::Default,
             "STDP time constant for plasticity");
    
    // Development settings
    addEntry("development_synaptogenesis_rate", 0.001, ConfigSource::Default,
             "Rate of synapse formation during development");
    
    addEntry("development_pruning_rate", 0.0001, ConfigSource::Default,
             "Rate of synapse pruning during development");
    
    // Neuromodulation settings
    addEntry("dopamine_baseline", 0.1, ConfigSource::Default,
             "Baseline dopamine level for reward modulation");
    
    // Reward settings
    addEntry("reward_discount_factor", 0.99, ConfigSource::Default,
             "Discount factor for reward prediction");
    
    // Environment settings
    addEntry("environment_name", std::string("GridWorld"), ConfigSource::Default,
             "Name of the environment");
    
    addEntry("environment_width", 10, ConfigSource::Default,
             "Width of the environment in grid units");
    
    addEntry("environment_height", 10, ConfigSource::Default,
             "Height of the environment in grid units");
    
    // Logging settings
    addEntry("log_level", std::string("INFO"), ConfigSource::Default,
             "Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)");
    
    addEntry("log_to_file", false, ConfigSource::Default,
             "Whether to log to file in addition to console");
    
    addEntry("log_filename", std::string("nlm.log"), ConfigSource::Default,
             "Filename for log output");
    
    // Simulation settings
    addEntry("max_simulation_steps", 10000, ConfigSource::Default,
             "Maximum number of simulation steps");
    
    addEntry("simulation_time_limit", 0.0, ConfigSource::Default,
             "Simulation time limit in seconds (0 = unlimited)");
    
    // Visualization settings
    addEntry("visualization_enabled", false, ConfigSource::Default,
             "Whether to enable visualization");
    
    addEntry("visualization_update_rate", 30.0, ConfigSource::Default,
             "Visualization update rate in Hz");
    
    // Advanced neural parameters
    addEntry("neuron_resting_potential", -70.0, ConfigSource::Default,
             "Resting potential for neurons in mV");
    
    addEntry("neuron_threshold", -55.0, ConfigSource::Default,
             "Firing threshold in mV");
    
    addEntry("neuron_reset_potential", -75.0, ConfigSource::Default,
             "Reset potential after firing in mV");
    
    addEntry("neuron_leak_conductance", 0.1, ConfigSource::Default,
             "Membrane leak conductance");
    
    addEntry("neuron_refractory_period", 5.0, ConfigSource::Default,
             "Refractory period in ms");
    
    // Memory system settings
    addEntry("working_memory_capacity", 100, ConfigSource::Default,
             "Capacity of working memory in traces");
    
    addEntry("episodic_memory_max_episodes", 1000, ConfigSource::Default,
             "Maximum number of episodes in episodic memory");
    
    // Development system settings
    addEntry("developmental_stage_initial", 0.0, ConfigSource::Default,
             "Duration of initial developmental stage");
    
    addEntry("developmental_stage_critical_period", 60.0, ConfigSource::Default,
             "Duration of critical period");
    
    addEntry("developmental_stage_maturation", 240.0, ConfigSource::Default,
             "Duration of maturation stage");
    
    // Checkpoint settings
    addEntry("checkpoint_enabled", true, ConfigSource::Default,
             "Whether to enable checkpoint saving");
    
    addEntry("checkpoint_interval", 1000, ConfigSource::Default,
             "Checkpoint save interval in steps");
    
    addEntry("checkpoint_dir", std::string("./checkpoints"), ConfigSource::Default,
             "Directory for checkpoint files");
    
    // Advanced plasticity settings
    addEntry("plasticity_learning_rate", 0.01, ConfigSource::Default,
             "Base learning rate for plasticity");
    
    addEntry("plasticity_modulation_strength", 1.0, ConfigSource::Default,
             "Strength of neuromodulation on plasticity");
    
    // Sensory system settings
    addEntry("sensory_noise_level", 0.01, ConfigSource::Default,
             "Level of noise in sensory processing");
    
    addEntry("sensory_filter_strength", 0.5, ConfigSource::Default,
             "Strength of sensory filtering");
    
    // Motor system settings
    addEntry("motor_command_history_length", 10, ConfigSource::Default,
             "Length of motor command history for action selection");
    
    addEntry("motor_noise_level", 0.05, ConfigSource::Default,
             "Level of noise in motor output");
    
    // Prediction system settings
    addEntry("prediction_horizon", 10, ConfigSource::Default,
             "Prediction horizon in steps");
    
    addEntry("prediction_error_threshold", 0.1, ConfigSource::Default,
             "Threshold for prediction error detection");
    
    // Attention system settings
    addEntry("attention_inhibition_strength", 0.5, ConfigSource::Default,
             "Strength of competitive inhibition in attention");
    
    addEntry("attention_excitation_strength", 1.5, ConfigSource::Default,
             "Strength of excitation in attention");
    
    // Concept formation settings
    addEntry("concept_threshold", 0.7, ConfigSource::Default,
             "Activation threshold for concept formation");
    
    addEntry("concept_association_strength", 1.0, ConfigSource::Default,
             "Strength of concept associations");
    
    // World simulation settings
    addEntry("world_time_step", 0.1, ConfigSource::Default,
             "World simulation time step");
    
    addEntry("world_max_energy", 100.0, ConfigSource::Default,
             "Maximum energy for agents");
    
    addEntry("world_energy_decay_rate", 0.01, ConfigSource::Default,
             "Energy decay rate per time step");
    
    // Agent brain settings
    addEntry("agent_curiosity_threshold", 0.3, ConfigSource::Default,
             "Curiosity threshold for exploration");
    
    addEntry("agent_novelty_decay_rate", 0.99, ConfigSource::Default,
             "Rate at which novelty decays");
    
    addEntry("agent_reward_modulation", true, ConfigSource::Default,
             "Whether to enable reward modulation");
    
    // Development settings
    addEntry("development_enabled", true, ConfigSource::Default,
             "Whether to enable developmental processes");
    
    addEntry("development_plasticity_modifier", 1.0, ConfigSource::Default,
             "Base plasticity modifier during development");
    
    // Curiosity settings
    addEntry("curiosity_enabled", true, ConfigSource::Default,
             "Whether to enable curiosity-driven exploration");
    
    addEntry("curiosity_sensitivity", 1.0, ConfigSource::Default,
             "Sensitivity to novel information");
    
    // Novelty settings
    addEntry("novelty_enabled", true, ConfigSource::Default,
             "Whether to enable novelty detection");
    
    addEntry("novelty_sensitivity", 1.0, ConfigSource::Default,
             "Sensitivity to novel information");
}

} // namespace nlm

// Add nlohmann/json as a dependency
toolchain_overrides = []
toolchain_prefix = ""
python_package_overrides = {}
packages = []
dependencies = [
    { "dependency": "nlohmann_json", "version": "3.11.2" }
]

standard = "c++17"
