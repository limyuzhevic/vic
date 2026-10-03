#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <cctype>
#include <iostream>

namespace nlm {

// Static schema member
std::map<std::string, ConfigSchemaEntry> Config::schema;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Initialize the default configuration schema
void Config::initializeSchema() {
    if (!schema.empty()) return;  // Already initialized
    
    // Core simulation parameters
    schema[CoreSimulationParameters::timestep] = ConfigSchemaEntry(
        CoreSimulationParameters::timestep, 0.001, "double",
        "Simulation timestep in seconds (smaller = more accurate but slower)",
        false, std::make_pair(1e-6, 1.0)
    );
    
    schema[CoreSimulationParameters::neuron_count] = ConfigSchemaEntry(
        CoreSimulationParameters::neuron_count, static_cast<int64_t>(1000), "int64",
        "Number of neurons in the brain", false, std::make_pair(1, 1000000)
    );
    
    schema[CoreSimulationParameters::region_count] = ConfigSchemaEntry(
        CoreSimulationParameters::region_count, static_cast<int64_t>(1), "int64",
        "Number of brain regions", false, std::make_pair(1, 100)
    );
    
    // Connectivity parameters
    schema[ConnectivityParameters::connection_probability] = ConfigSchemaEntry(
        ConnectivityParameters::connection_probability, 0.1, "double",
        "Probability of forming a connection between neurons (0.0 to 1.0)",
        false, std::make_pair(0.0, 1.0)
    );
    
    schema[ConnectivityParameters::connection_strength] = ConfigSchemaEntry(
        ConnectivityParameters::connection_strength, 0.2, "double",
        "Average synaptic weight for new connections", false, std::make_pair(0.0, 10.0)
    );
    
    // Plasticity parameters
    schema[PlasticityParameters::stdp_ltp_weight] = ConfigSchemaEntry(
        PlasticityParameters::stdp_ltp_weight, 0.01, "double",
        "STDP long-term potentiation weight (positive Δw for pre->post)",
        false, std::make_pair(0.0, 1.0)
    );
    
    schema[PlasticityParameters::stdp_ltd_weight] = ConfigSchemaEntry(
        PlasticityParameters::stdp_ltd_weight, 0.012, "double",
        "STDP long-term depression weight (negative Δw for post->pre)",
        false, std::make_pair(0.0, 1.0)
    );
    
    schema[PlasticityParameters::stdp_tau] = ConfigSchemaEntry(
        PlasticityParameters::stdp_tau, 20.0, "double",
        "STDP time constant (in milliseconds) for trace decay", false, std::make_pair(1.0, 1000.0)
    );
    
    schema[PlasticityParameters::hebbian_learning_rate] = ConfigSchemaEntry(
        PlasticityParameters::hebbian_learning_rate, 0.01, "double",
        "Hebbian learning rate for simultaneous neuron activation", false, std::make_pair(0.0, 1.0)
    );
    
    // Structural plasticity parameters
    schema[StructuralPlasticityParameters::synaptogenesis_rate] = ConfigSchemaEntry(
        StructuralPlasticityParameters::synaptogenesis_rate, 0.0001, "double",
        "Rate of new synapse formation per neuron per step", false, std::make_pair(1e-9, 0.1)
    );
    
    schema[StructuralPlasticityParameters::pruning_rate] = ConfigSchemaEntry(
        StructuralPlasticityParameters::pruning_rate, 0.00001, "double",
        "Rate of synapse pruning per synapse per step", false, std::make_pair(1e-9, 0.01)
    );
    
    schema[StructuralPlasticityParameters::max_synapses] = ConfigSchemaEntry(
        StructuralPlasticityParameters::max_synapses, 100000, "int64",
        "Maximum number of synapses the brain can hold", false, std::make_pair(100, 1000000)
    );
    
    // Memory parameters
    schema[MemoryParameters::working_memory_capacity] = ConfigSchemaEntry(
        MemoryParameters::working_memory_capacity, 100, "int64",
        "Capacity of working memory (number of active memories)", false, std::make_pair(1, 10000)
    );
    
    schema[MemoryParameters::episodic_memory_max_episodes] = ConfigSchemaEntry(
        MemoryParameters::episodic_memory_max_episodes, 1000, "int64",
        "Maximum number of episodes stored in episodic memory", false, std::make_pair(10, 100000)
    );
    
    schema[MemoryParameters::associative_memory_capacity] = ConfigSchemaEntry(
        MemoryParameters::associative_memory_capacity, 5000, "int64",
        "Capacity of associative memory for pattern associations", false, std::make_pair(100, 100000)
    );
    
    // Neuromodulation parameters
    schema[NeuromodulationParameters::dopamine_baseline] = ConfigSchemaEntry(
        NeuromodulationParameters::dopamine_baseline, 0.0, "double",
        "Baseline dopamine level for reward prediction", false, std::make_pair(0.0, 10.0)
    );
    
    schema[NeuromodulationParameters::curiosity_strength] = ConfigSchemaEntry(
        NeuromodulationParameters::curiosity_strength, 1.0, "double",
        "Strength of curiosity-driven exploration", false, std::make_pair(0.0, 10.0)
    );
    
    schema[NeuromodulationParameters::novelty_threshold] = ConfigSchemaEntry(
        NeuromodulationParameters::novelty_threshold, 0.5, "double",
        "Threshold for novelty detection (0-1, higher = more sensitive)", false, std::make_pair(0.0, 1.0)
    );
    
    schema[NeuromodulationParameters::prediction_error_decay] = ConfigSchemaEntry(
        NeuromodulationParameters::prediction_error_decay, 0.1, "double",
        "Decay rate for prediction error signals", false, std::make_pair(0.01, 1.0)
    );
    
    // Development parameters
    schema[DevelopmentParameters::developmental_stage_duration] = ConfigSchemaEntry(
        DevelopmentParameters::developmental_stage_duration, 1000, "int64",
        "Number of simulation steps per developmental stage", false, std::make_pair(10, 10000)
    );
    
    schema[DevelopmentParameters::plasticity_decay] = ConfigSchemaEntry(
        DevelopmentParameters::plasticity_decay, 0.01, "double",
        "Rate at which plasticity decreases with development", false, std::make_pair(0.001, 0.1)
    );
    
    // Performance tuning
    schema[PerformanceTuning::max_steps] = ConfigSchemaEntry(
        PerformanceTuning::max_steps, 10000, "int64",
        "Maximum number of simulation steps to run", false, std::make_pair(1, 1000000)
    );
    
    schema[PerformanceTuning::checkpoint_interval] = ConfigSchemaEntry(
        PerformanceTuning::checkpoint_interval, 1000, "int64",
        "Interval (in steps) for saving brain state checkpoints", false, std::make_pair(1, 100000)
    );
    
    schema[PerformanceTuning::memory_limit] = ConfigSchemaEntry(
        PerformanceTuning::memory_limit, 1024, "int64",
        "Memory limit in MB for the simulation", false, std::make_pair(128, 8192)
    );
    
    schema[PerformanceTuning::max_memory_mb] = ConfigSchemaEntry(
        PerformanceTuning::max_memory_mb, 2048, "int64",
        "Maximum memory usage in MB before cleanup", false, std::make_pair(256, 16384)
    );
    
    schema[PerformanceTuning::thread_count] = ConfigSchemaEntry(
        PerformanceTuning::thread_count, 1, "int64",
        "Number of threads for parallel computation", false, std::make_pair(1, 64)
    );
    
    // Advanced debugging
    schema[AdvancedDebugging::enable_debug_logging] = ConfigSchemaEntry(
        AdvancedDebugging::enable_debug_logging, false, "bool",
        "Enable detailed debug logging for troubleshooting", false
    );
    
    schema[AdvancedDebugging::enable_spike_tracing] = ConfigSchemaEntry(
        AdvancedDebugging::enable_spike_tracing, false, "bool",
        "Enable detailed spike tracing for analysis", false
    );
    
    schema[AdvancedDebugging::enable_visualization] = ConfigSchemaEntry(
        AdvancedDebugging::enable_visualization, false, "bool",
        "Enable visualization output for debugging", false
    );
    
    schema[AdvancedDebugging::trace_spikes] = ConfigSchemaEntry(
        AdvancedDebugging::trace_spikes, false, "bool",
        "Trace spike times for analysis", false
    );
    
    schema[AdvancedDebugging::show_synaptic_graphics] = ConfigSchemaEntry(
        AdvancedDebugging::show_synaptic_graphics, false, "bool",
        "Show synaptic graphics in visualization", false
    );
    
    // Random seed and reproducibility options
    schema[RandomSeedOptions::random_seed] = ConfigSchemaEntry(
        RandomSeedOptions::random_seed, static_cast<int64_t>(42), "int64",
        "Random seed for reproducible experiments", false, std::make_pair(0, std::numeric_limits<int64_t>::max())
    );
    
    schema[RandomSeedOptions::deterministic] = ConfigSchemaEntry(
        RandomSeedOptions::deterministic, false, "bool",
        "Run in deterministic mode (no random variations)", false
    );
    
    // File options
    schema[FileOptions::config_file] = ConfigSchemaEntry(
        FileOptions::config_file, std::string("configs/default.cfg"), "string",
        "Path to configuration file", false
    );
    
    schema[FileOptions::output_file] = ConfigSchemaEntry(
        FileOptions::output_file, std::string(""), "string",
        "Path to output file for results", false
    );
    
    schema[FileOptions::checkpoint_dir] = ConfigSchemaEntry(
        FileOptions::checkpoint_dir, std::string("./checkpoints"), "string",
        "Directory for checkpoint files", false
    );
    
    // Logging options
    schema[LoggingOptions::log_level] = ConfigSchemaEntry(
        LoggingOptions::log_level, std::string("INFO"), "string",
        "Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", false
    );
    
    schema[LoggingOptions::log_file] = ConfigSchemaEntry(
        LoggingOptions::log_file, std::string("nlm.log"), "string",
        "File for log output", false
    );
    
    schema[LoggingOptions::verbose] = ConfigSchemaEntry(
        LoggingOptions::verbose, false, "bool",
        "Enable verbose output", false
    );
    
    // Experiment options
    schema[ExperimentOptions::experiment_mode] = ConfigSchemaEntry(
        ExperimentOptions::experiment_mode, std::string("auto"), "string",
        "Experiment mode (auto, test, learning, development)", false
    );
    
    schema[ExperimentOptions::test_number] = ConfigSchemaEntry(
        ExperimentOptions::test_number, 0, "int64",
        "Test number for experimental runs", false, std::make_pair(0, 1000)
    );
    
    schema[ExperimentOptions::profile] = ConfigSchemaEntry(
        ExperimentOptions::profile, false, "bool",
        "Enable profiling mode", false
    );
    
    schema[ExperimentOptions::profile_interval] = ConfigSchemaEntry(
        ExperimentOptions::profile_interval, 100, "int64",
        "Interval (in steps) for profiling measurements", false, std::make_pair(1, 10000)
    );
    
    schema[ExperimentOptions::fast_mode] = ConfigSchemaEntry(
        ExperimentOptions::fast_mode, false, "bool",
        "Enable fast mode (reduced accuracy, faster execution)", false
    );
    
    std::cout << "Config schema initialized with " << schema.size() << " parameters" << std::endl;
}

// Get schema for a specific key
std::optional<ConfigSchemaEntry> Config::getSchema(const std::string& key) {
    auto it = schema.find(key);
    if (it == schema.end()) return std::nullopt;
    return it->second;
}

// Get all schema keys
std::vector<std::string> Config::getSchemaKeys() {
    std::vector<std::string> keys;
    keys.reserve(schema.size());
    for (const auto& pair : schema) {
        keys.push_back(pair.first);
    }
    return keys;
}

// Get configuration schema summary
std::string Config::getSchemaSummary() {
    std::ostringstream oss;
    oss << "Configuration Schema (" << schema.size() << " parameters):\n\n";
    
    for (const auto& pair : schema) {
        const auto& entry = pair.second;
        oss << "  " << pair.first << " [";
        
        // Show type
        oss << entry.type;
        
        // Show range if applicable
        if (entry.range.has_value()) {
            oss << " = [" << entry.range.value().first << ", " << entry.range.value().second << "]";
        }
        
        // Show minLength if applicable
        if (entry.minLength.has_value()) {
            oss << " minLen=" << entry.minLength.value();
        }
        
        oss << "] " << entry.description << "\n";
    }
    
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect format by extension
    std::string ext = detectFileFormat(filepath);
    if (ext == "json") {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromJson(buffer.str());
    } else if (ext == "yaml" || ext == "yml") {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromYaml(buffer.str());
    } else {
        // Default to simple key=value format
        clear();
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << "\", \";
                }
                file << "\"]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setValidated(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setWithDescription(const std::string& key, const ConfigValue& value, 
                                const std::string& description, ConfigSource source) {
    setValidated(key, value, source);
    
    // Find existing entry and update description
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[\"";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "\"]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& json) {
    try {
        if (json.is_string()) {
            return json.get<std::string>();
        } else if (json.is_boolean()) {
            return json.get<bool>();
        } else if (json.is_number_integer()) {
            // Try int64 first
            try {
                return json.get<int64_t>();
            } catch (...) {
                // Try int
                return json.get<int>();
            }
        } else if (json.is_number_float()) {
            return json.get<double>();
        } else if (json.is_array()) {
            // Check if it's an array of numbers or strings
            if (!json.empty()) {
                if (json[0].is_number_integer()) {
                    std::vector<int> vec = json.get<std::vector<int>>();
                    return ConfigValue(vec);
                } else if (json[0].is_number_float()) {
                    std::vector<double> vec = json.get<std::vector<double>>();
                    return ConfigValue(vec);
                } else if (json[0].is_string()) {
                    std::vector<std::string> vec = json.get<std::vector<std::string>>();
                    return ConfigValue(vec);
                }
            }
        }
    } catch (const std::exception& e) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::string Config::detectFileFormat(const std::string& filepath) {
    std::string ext = filepath;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext.size() >= 5 && ext.substr(ext.size() - 5) == ".json") {
        return "json";
    } else if (ext.size() >= 4 && (ext.substr(ext.size() - 4) == ".yaml" || ext.substr(ext.size() - 4) == ".yml")) {
        return "yaml";
    }
    return "unknown";
}

bool Config::validateKey(const std::string& key) {
    return !key.empty() && key.find_first_of(" \t\r\n" ) == std::string::npos &&
           (key.find('=') == std::string::npos && key.find('*') == std::string::npos);
}

bool Config::validateValue(const ConfigValue& value, const ConfigSchemaEntry& schemaEntry) {
    if (schemaEntry.range.has_value()) {
        if (!validateRange(value, schemaEntry.range)) return false;
    }
    
    if (schemaEntry.minLength.has_value()) {
        if (!validateStringArrayLength(value, schemaEntry.minLength)) return false;
    }
    
    return true;
}

bool Config::validateRange(const ConfigValue& value, const std::optional<std::pair<double, double>>& range) {
    if (!range.has_value()) return true;
    
    const auto& r = range.value();
    
    bool isValid = false;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || 
                      std::is_same_v<T, int> ||
                      std::is_same_v<T, double>) {
            double val = (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) ? 
                         static_cast<double>(arg) : arg;
            isValid = (val >= r.first - 1e-9) && (val <= r.second + 1e-9);
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean values are always valid
            isValid = true;
        }
    }, value);
    
    return isValid;
}

bool Config::validateStringArrayLength(const ConfigValue& value, const std::optional<size_t>& minLength) {
    if (!minLength.has_value()) return true;
    
    bool isValid = false;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            isValid = arg.size() >= minLength.value();
        }
    }, value);
    
    return isValid;
}

void Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateKey(key)) {
        throw ConfigValidationError("Invalid configuration key: " + key);
    }
    
    // Check if schema exists
    auto schemaEntry = getSchema(key);
    if (schemaEntry.has_value() && !validateValue(value, schemaEntry.value())) {
        throw ConfigValidationError("Invalid value for configuration parameter: " + key);
    }
    
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Update description from schema if available
        if (schemaEntry.has_value()) {
            it->description = schemaEntry.value().description;
        }
    } else {
        std::string description = schemaEntry.has_value() ? schemaEntry.value().description : "";
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

// Explicit template instantiations
namespace {
    struct ConfigTypeIniter {
        ConfigTypeIniter() {
            initializeDefaultConfigSchema();
        }
    };
    
    ConfigTypeIniter configTypeIniter;
}

// Initialize default config schema
void initializeDefaultConfigSchema() {
    Config::initializeSchema();
}

// Implementation of constant definitions
const std::string CoreSimulationParameters::timestep = "simulation_timestep";
const std::string CoreSimulationParameters::neuron_count = "neuron_count";
const std::string CoreSimulationParameters::region_count = "region_count";

const std::string ConnectivityParameters::connection_probability = "connection_probability";
const std::string ConnectivityParameters::connection_strength = "connection_strength";

const std::string PlasticityParameters::stdp_ltp_weight = "stdp_ltp_weight";
const std::string PlasticityParameters::stdp_ltd_weight = "stdp_ltd_weight";
const std::string PlasticityParameters::stdp_tau = "stdp_tau";
const std::string PlasticityParameters::hebbian_learning_rate = "hebbian_learning_rate";

const std::string StructuralPlasticityParameters::synaptogenesis_rate = "synaptogenesis_rate";
const std::string StructuralPlasticityParameters::pruning_rate = "pruning_rate";
const std::string StructuralPlasticityParameters::max_synapses = "max_synapses";

const std::string MemoryParameters::working_memory_capacity = "working_memory_capacity";
const std::string MemoryParameters::episodic_memory_max_episodes = "episodic_memory_max_episodes";
const std::string MemoryParameters::associative_memory_capacity = "associative_memory_capacity";

const std::string NeuromodulationParameters::dopamine_baseline = "dopamine_baseline";
const std::string NeuromodulationParameters::curiosity_strength = "curiosity_strength";
const std::string NeuromodulationParameters::novelty_threshold = "novelty_threshold";
const std::string NeuromodulationParameters::prediction_error_decay = "prediction_error_decay";

const std::string DevelopmentParameters::developmental_stage_duration = "developmental_stage_duration";
const std::string DevelopmentParameters::plasticity_decay = "plasticity_decay";

const std::string PerformanceTuning::max_steps = "max_steps";
const std::string PerformanceTuning::checkpoint_interval = "checkpoint_interval";
const std::string PerformanceTuning::memory_limit = "memory_limit";
const std::string PerformanceTuning::max_memory_mb = "max_memory_mb";
const std::string PerformanceTuning::thread_count = "thread_count";

const std::string AdvancedDebugging::enable_debug_logging = "enable_debug_logging";
const std::string AdvancedDebugging::enable_spike_tracing = "enable_spike_tracing";
const std::string AdvancedDebugging::enable_visualization = "enable_visualization";
const std::string AdvancedDebugging::trace_spikes = "trace_spikes";
const std::string AdvancedDebugging::show_synaptic_graphics = "show_synaptic_graphics";

const std::string RandomSeedOptions::random_seed = "random_seed";
const std::string RandomSeedOptions::deterministic = "deterministic";

const std::string FileOptions::config_file = "config_file";
const std::string FileOptions::output_file = "output_file";
const std::string FileOptions::checkpoint_dir = "checkpoint_dir";

const std::string LoggingOptions::log_level = "log_level";
const std::string LoggingOptions::log_file = "log_file";
const std::string LoggingOptions::verbose = "verbose";

const std::string ExperimentOptions::experiment_mode = "experiment_mode";
const std::string ExperimentOptions::test_number = "test_number";
const std::string ExperimentOptions::profile = "enable_profiling";
const std::string ExperimentOptions::profile_interval = "profile_interval";
const std::string ExperimentOptions::fast_mode = "fast_mode";

} // namespace nlm
