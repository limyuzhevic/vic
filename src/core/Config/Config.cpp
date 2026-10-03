#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <unordered_set>
#include <json/json.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigType> typeHints;
    std::map<std::string, ValidationCallback> validators;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, std::string> categories;
    bool strictMode;
    bool jsonMode;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->strictMode = false;
    pImpl->jsonMode = false;
    
    // Register neural network specific configuration schemas
    registerNeuralNetworkSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

// Enhanced YAML parsing (simplified implementation)
bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node config = YAML::LoadFile(filepath);
        
        for (const auto& pair : config) {
            std::string key = pair.first.as<std::string>();
            
            // Determine type based on YAML node type
            if (pair.second.IsScalar()) {
                std::string value = pair.second.as<std::string>();
                set(key, value, ConfigSource::File, "YAML loaded " + key);
            } else if (pair.second.IsSequence()) {
                // Convert sequence to vector<double>
                std::vector<double> values;
                for (const auto& item : pair.second) {
                    values.push_back(item.as<double>());
                }
                set(key, values, ConfigSource::File, "YAML loaded " + key);
            } else if (pair.second.IsMap()) {
                // Convert map to map<string, double>
                std::map<std::string, double> values;
                for (const auto& pair2 : pair.second) {
                    values[pair2.first.as<std::string>()] = pair2.second.as<double>();
                }
                set(key, values, ConfigSource::File, "YAML loaded " + key);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

// Enhanced environment variable loading
bool Config::loadFromEnv(const std::string& prefix) {
    bool loaded = false;
    
    // Common neural network environment variables
    const std::vector<std::string> commonVars = {
        "NLM_NEURON_COUNT",
        "NLM_REGION_COUNT", 
        "NLM_CONNECTION_PROBABILITY",
        "NLM_RANDOM_SEED",
        "NLM_SIMULATION_TIMESTEP"
    };
    
    for (const auto& var : commonVars) {
        const char* value = std::getenv(var.c_str());
        if (value) {
            std::string key = var.substr(prefix.size()); // Remove prefix
            set(key, std::string(value), ConfigSource::Runtime);
            loaded = true;
        }
    }
    
    // Load all NLM_* environment variables
    for (const auto& env : std::vector<std::string>()) {
        // Would need to implement env iteration
    }
    
    return loaded;
}

// Enhanced YAML saving
bool Config::saveToYAML(const std::string& filepath) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::Key << entry.key;
            
            std::visit([&emitter](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    emitter << YAML::Value << static_cast<int>(arg);
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    emitter << YAML::Value << static_cast<double>(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<float>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
                    emitter << YAML::Value << arg;
                } else if constexpr (std::is_same_v<T, std::map<std::string, double>> ||
                                     std::is_same_v<T, std::map<std::string, std::string>>) {
                    emitter << YAML::Value << arg;
                }
            }, entry.value);
        }
        
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
        
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML writing error: " + std::string(e.what()));
        return false;
    }
}

// Advanced set methods implementation
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<float>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::vector<double>>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::map<std::string, double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::map<std::string, std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Advanced set methods with descriptions
void Config::setInt(const std::string& key, int64_t value, const std::string& description) {
    registerTypeHint(key, ConfigType::Int64);
    registerDescription(key, description.empty() ? ("Integer value: " + std::to_string(value)) : description);
    set(key, ConfigValue(value), ConfigSource::Runtime, description, "neural");
}

void Config::setFloat(const std::string& key, float value, const std::string& description) {
    registerTypeHint(key, ConfigType::Float);
    registerDescription(key, description.empty() ? ("Float value: " + std::to_string(value)) : description);
    set(key, ConfigValue(value), ConfigSource::Runtime, description, "neural");
}

void Config::setDouble(const std::string& key, double value, const std::string& description) {
    registerTypeHint(key, ConfigType::Double);
    registerDescription(key, description.empty() ? ("Double value: " + std::to_string(value)) : description);
    set(key, ConfigValue(value), ConfigSource::Runtime, description, "neural");
}

void Config::setString(const std::string& key, const std::string& value, const std::string& description) {
    registerTypeHint(key, ConfigType::String);
    registerDescription(key, description.empty() ? ("String value: " + value) : description);
    set(key, ConfigValue(value), ConfigSource::Runtime, description, "neural");
}

void Config::setBool(const std::string& key, bool value, const std::string& description) {
    registerTypeHint(key, ConfigType::Bool);
    registerDescription(key, description.empty() ? ("Boolean value: " + std::to_string(value)) : description);
    set(key, ConfigValue(value), ConfigSource::Runtime, description, "neural");
}

void Config::setVector(const std::string& key, const std::vector<double>& value, const std::string& description) {
    registerTypeHint(key, ConfigType::FloatList);
    registerDescription(key, description.empty() ? ("Vector of size " + std::to_string(value.size())) : description);
    set(key, ConfigValue(value), ConfigSource::Runtime, description, "neural");
}

// Get entry details
const ConfigEntry* Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return nullptr;
    }
    
    return &(*it);
}

std::optional<ConfigSource> Config::getSource(const std::string& key) const {
    auto entry = getEntry(key);
    if (!entry) {
        return std::nullopt;
    }
    return entry->source;
}

// Get all keys by category
std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        auto cat_it = pImpl->categories.find(entry.key);
        if (cat_it != pImpl->categories.end() && cat_it->second == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Configuration schema and validation
void Config::registerTypeHint(const std::string& key, ConfigType type) {
    pImpl->typeHints[key] = type;
}

void Config::registerValidator(const std::string& key, const ValidationCallback& validator) {
    if (validator) {
        pImpl->validators[key] = validator;
    }
}

void Config::registerDescription(const std::string& key, const std::string& description) {
    if (!description.empty()) {
        pImpl->descriptions[key] = description;
    }
}

void Config::registerCategory(const std::string& key, const std::string& category) {
    pImpl->categories[key] = category;
}

bool Config::validate() const {
    std::vector<ValidationError> errors = getValidationErrors();
    return errors.empty();
}

std::vector<ValidationError> Config::getValidationErrors() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Check type hints
        auto type_it = pImpl->typeHints.find(entry.key);
        if (type_it != pImpl->typeHints.end()) {
            bool type_valid = validateType(entry.key, entry.value, type_it->second);
            if (!type_valid && pImpl->strictMode) {
                ValidationError error;
                error.key = entry.key;
                error.message = "Type mismatch: expected " + configTypeToString(type_it->second) + ", got unknown type";
                error.source = entry.source;
                errors.push_back(error);
            }
        }
        
        // Run validators
        auto validator_it = pImpl->validators.find(entry.key);
        if (validator_it != pImpl->validators.end()) {
            if (!validator_it->second(entry.key, entry.value)) {
                ValidationError error;
                error.key = entry.key;
                error.message = "Validation failed for key: " + entry.key;
                error.source = entry.source;
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

bool Config::validateType(const std::string& key, const ConfigValue& value, ConfigType type) {
    return std::visit([&type](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return type == ConfigType::Int || type == ConfigType::Int64;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return type == ConfigType::Int || type == ConfigType::Int64;
        } else if constexpr (std::is_same_v<T, double>) {
            return type == ConfigType::Float || type == ConfigType::Double;
        } else if constexpr (std::is_same_v<T, float>) {
            return type == ConfigType::Float;
        } else if constexpr (std::is_same_v<T, bool>) {
            return type == ConfigType::Bool;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return type == ConfigType::String;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return type == ConfigType::IntList;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return type == ConfigType::FloatList;
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            return type == ConfigType::FloatList;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return type == ConfigType::StringList;
        } else if constexpr (std::is_same_v<T, std::vector<std::vector<double>>>) {
            return type == ConfigType::Matrix;
        } else if constexpr (std::is_same_v<T, std::map<std::string, double>>) {
            return type == ConfigType::DictDouble;
        } else if constexpr (std::is_same_v<T, std::map<std::string, std::string>>) {
            return type == ConfigType::DictString;
        }
        return false;
    }, value);
}

std::string Config::configTypeToString(ConfigType type) {
    switch (type) {
        case ConfigType::Int: return "int";
        case ConfigType::Int64: return "int64";
        case ConfigType::Float: return "float";
        case ConfigType::Double: return "double";
        case ConfigType::Bool: return "bool";
        case ConfigType::String: return "string";
        case ConfigType::IntList: return "vector<int>";
        case ConfigType::FloatList: return "vector<double>";
        case ConfigType::StringList: return "vector<string>";
        case ConfigType::Matrix: return "matrix<double>";
        case ConfigType::DictDouble: return "map<string,double>";
        case ConfigType::DictString: return "map<string,string>";
        default: return "unknown";
    }
}

// Configuration summary by category
std::string Config::summaryByCategory(const std::string& category) const {
    std::ostringstream oss;
    oss << "Configuration " << category << " (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        auto cat_it = pImpl->categories.find(entry.key);
        if (cat_it == pImpl->categories.end() || cat_it->second != category) {
            continue;
        }
        
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " << 
            (pImpl->descriptions.count(entry.key) ? pImpl->descriptions.at(entry.key) : "") << "\n";
    }
    
    return oss.str();
}

// Neural network configuration helpers
NeuralNetworkConfig Config::getNeuralNetworkConfig() const {
    NeuralNetworkConfig config;
    
    // Set defaults
    config.neuronCount = getOr<int64_t>("neuron_count", 500);
    config.regionCount = getOr<int64_t>("region_count", 5);
    config.connectionProbability = getOr<float>("connection_probability", 0.2f);
    config.randomSeed = getOr<int64_t>("random_seed", 42);
    config.simulationTimestep = getOr<double>("simulation_timestep", 0.001);
    config.stdpLtpWeight = getOr<float>("stdp_ltp_weight", 0.02f);
    config.stdpLtdWeight = getOr<float>("stdp_ltd_weight", 0.015f);
    config.synaptogenesisRate = getOr<float>("synaptogenesis_rate", 0.00005f);
    config.pruningRate = getOr<float>("pruning_rate", 0.000005f);
    config.enableRewardModulation = getOr<bool>("enable_reward_modulation", true);
    config.enableStructuralPlasticity = getOr<bool>("enable_structural_plasticity", true);
    config.enableDevelopment = getOr<bool>("enable_development", true);
    config.enableCuriosity = getOr<bool>("enable_curiosity", true);
    
    return config;
}

bool Config::configureNeuralNetwork(const NeuralNetworkConfig& config) {
    bool success = true;
    
    try {
        setInt("neuron_count", config.neuronCount, "Number of neurons in the brain");
        setInt("region_count", config.regionCount, "Number of brain regions");
        setFloat("connection_probability", config.connectionProbability, "Probability of connection between neurons");
        setInt("random_seed", config.randomSeed, "Random seed for reproducible simulations");
        setDouble("simulation_timestep", config.simulationTimestep, "Simulation time step in seconds");
        setFloat("stdp_ltp_weight", config.stdpLtpWeight, "STDP long-term potentiation weight");
        setFloat("stdp_ltd_weight", config.stdpLtdWeight, "STDP long-term depression weight");
        setFloat("synaptogenesis_rate", config.synaptogenesisRate, "Rate of new synapse formation");
        setFloat("pruning_rate", config.pruningRate, "Rate of synapse removal");
        setBool("enable_reward_modulation", config.enableRewardModulation, "Enable reward-based learning");
        setBool("enable_structural_plasticity", config.enableStructuralPlasticity, "Enable structural changes");
        setBool("enable_development", config.enableDevelopment, "Enable developmental processes");
        setBool("enable_curiosity", config.enableCuriosity, "Enable curiosity-driven exploration");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error configuring neural network: " + std::string(e.what()));
        success = false;
    }
    
    return success;
}

void Config::registerNeuralNetworkSchema() {
    // Register neural network specific configurations with descriptions and validation
    
    // Core neural parameters
    registerTypeHint("neuron_count", ConfigType::Int64);
    registerValidator("neuron_count", [](const std::string&, const ConfigValue& value) {
        return std::get<int64_t>(value) > 0 && std::get<int64_t>(value) <= 100000;
    });
    registerDescription("neuron_count", "Number of neurons in the brain network");
    registerCategory("neuron_count", "neural");
    
    registerTypeHint("region_count", ConfigType::Int64);
    registerValidator("region_count", [](const std::string&, const ConfigValue& value) {
        return std::get<int64_t>(value) > 0 && std::get<int64_t>(value) <= 100;
    });
    registerDescription("region_count", "Number of distinct brain regions");
    registerCategory("region_count", "neural");
    
    registerTypeHint("connection_probability", ConfigType::Float);
    registerValidator("connection_probability", [](const std::string&, const ConfigValue& value) {
        return std::get<float>(value) >= 0.0f && std::get<float>(value) <= 1.0f;
    });
    registerDescription("connection_probability", "Probability of connection between neurons (0.0 to 1.0)");
    registerCategory("connection_probability", "neural");
    
    registerTypeHint("random_seed", ConfigType::Int64);
    registerDescription("random_seed", "Random seed for reproducible simulations");
    registerCategory("random_seed", "neural");
    
    registerTypeHint("simulation_timestep", ConfigType::Double);
    registerValidator("simulation_timestep", [](const std::string&, const ConfigValue& value) {
        return std::get<double>(value) > 0.0 && std::get<double>(value) <= 1.0;
    });
    registerDescription("simulation_timestep", "Simulation time step in seconds");
    registerCategory("simulation_timestep", "neural");
    
    // STDP parameters
    registerTypeHint("stdp_ltp_weight", ConfigType::Float);
    registerValidator("stdp_ltp_weight", [](const std::string&, const ConfigValue& value) {
        return std::get<float>(value) >= 0.0f && std::get<float>(value) <= 1.0f;
    });
    registerDescription("stdp_ltp_weight", "STDP long-term potentiation weight");
    registerCategory("stdp_ltp_weight", "plasticity");
    
    registerTypeHint("stdp_ltd_weight", ConfigType::Float);
    registerValidator("stdp_ltd_weight", [](const std::string&, const ConfigValue& value) {
        return std::get<float>(value) >= 0.0f && std::get<float>(value) <= 1.0f;
    });
    registerDescription("stdp_ltd_weight", "STDP long-term depression weight");
    registerCategory("stdp_ltd_weight", "plasticity");
    
    // Structural plasticity parameters
    registerTypeHint("synaptogenesis_rate", ConfigType::Float);
    registerValidator("synaptogenesis_rate", [](const std::string&, const ConfigValue& value) {
        return std::get<float>(value) >= 0.0f && std::get<float>(value) <= 0.01f;
    });
    registerDescription("synaptogenesis_rate", "Rate of new synapse formation");
    registerCategory("synaptogenesis_rate", "plasticity");
    
    registerTypeHint("pruning_rate", ConfigType::Float);
    registerValidator("pruning_rate", [](const std::string&, const ConfigValue& value) {
        return std::get<float>(value) >= 0.0f && std::get<float>(value) <= 0.001f;
    });
    registerDescription("pruning_rate", "Rate of synapse removal");
    registerCategory("pruning_rate", "plasticity");
    
    // Feature flags
    registerTypeHint("enable_reward_modulation", ConfigType::Bool);
    registerDescription("enable_reward_modulation", "Enable reward-based learning and neuromodulation");
    registerCategory("enable_reward_modulation", "features");
    
    registerTypeHint("enable_structural_plasticity", ConfigType::Bool);
    registerDescription("enable_structural_plasticity", "Enable structural changes (synaptogenesis/pruning)");
    registerCategory("enable_structural_plasticity", "features");
    
    registerTypeHint("enable_development", ConfigType::Bool);
    registerDescription("enable_development", "Enable developmental processes and aging");
    registerCategory("enable_development", "features");
    
    registerTypeHint("enable_curiosity", ConfigType::Bool);
    registerDescription("enable_curiosity", "Enable curiosity-driven exploration and novelty detection");
    registerCategory("enable_curiosity", "features");
}

// Configuration comparison
bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto other_entry = other.getEntry(entry.key);
        if (!other_entry || entry.value != other_entry->value) {
            return false;
        }
    }
    
    return true;
}

std::map<std::string, std::pair<ConfigValue, ConfigValue>> Config::diff(const Config& other) const {
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> differences;
    
    // Check entries in this config
    for (const auto& entry : pImpl->entries) {
        auto other_entry = other.getEntry(entry.key);
        if (!other_entry) {
            differences[entry.key] = {entry.value, ConfigValue()};
        } else if (entry.value != other_entry->value) {
            differences[entry.key] = {entry.value, other_entry->value};
        }
    }
    
    // Check entries in other config
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key)) {
            differences[entry.key] = {ConfigValue(), entry.value};
        }
    }
    
    return differences;
}

// Configuration merging
void Config::merge(const Config& other, bool overwrite) {
    for (const auto& entry : other.pImpl->entries) {
        if (overwrite || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

// Configuration cloning
std::shared_ptr<Config> Config::clone() const {
    auto cloned = std::make_shared<Config>();
    cloned->pImpl->entries = pImpl->entries;
    cloned->pImpl->typeHints = pImpl->typeHints;
    cloned->pImpl->validators = pImpl->validators;
    cloned->pImpl->descriptions = pImpl->descriptions;
    cloned->pImpl->categories = pImpl->categories;
    cloned->pImpl->strictMode = pImpl->strictMode;
    cloned->pImpl->jsonMode = pImpl->jsonMode;
    return cloned;
}

// Configuration search
std::vector<std::string> Config::search(const std::string& pattern) const {
    std::vector<std::string> matches;
    for (const auto& entry : pImpl->entries) {
        if (stringMatchesPattern(entry.key, pattern)) {
            matches.push_back(entry.key);
        }
    }
    return matches;
}

// Helper implementations
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::stringMatchesPattern(const std::string& str, const std::string& pattern) {
    bool has_wildcard = false;
    for (char c : pattern) {
        if (c == '*' || c == '?') {
            has_wildcard = true;
            break;
        }
    }
    
    if (!has_wildcard) {
        return str == pattern;
    }
    
    // Convert pattern to regex (simplified)
    std::string regex_pattern = pattern;
    std::replace(regex_pattern.begin(), regex_pattern.end(), '*', ".*");
    std::replace(regex_pattern.begin(), regex_pattern.end(), '?', ".");
    
    std::regex re(regex_pattern, std::regex_constants::icase);
    return std::regex_match(str, re);
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<float>> Config::get<std::vector<float>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<std::vector<double>>> Config::get<std::vector<std::vector<double>>>(const std::string&) const;
template std::optional<std::map<std::string, double>> Config::get<std::map<std::string, double>>(const std::string&) const;
template std::optional<std::map<std::string, std::string>> Config::get<std::map<std::string, std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<float> Config::getOr<std::vector<float>>(const std::string&, const std::vector<float>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
