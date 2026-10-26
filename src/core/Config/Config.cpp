#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>
#include <iomanip>
#include <random>
#include <map>
#include <unordered_set>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigTemplate> templates;
    std::vector<std::string> groups;
    std::map<std::string, std::vector<std::string>> keyToGroups;
    std::map<std::string, std::vector<std::string>> keyToDependencies;
    std::vector<ValidationRule> validationRules;
    std::vector<ConfigChange> changeHistory;
    std::shared_ptr<Config> parent;
    double compatibilityScore;
    bool validateOnSet;
    bool trackChanges;
    size_t maxHistorySize;
    
    Impl() : compatibilityScore(0.0), validateOnSet(true), trackChanges(true), maxHistorySize(100) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    initializeDefaultConfig();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultConfig() {
    // Neural network configuration
    set("neuron_count", 500, ConfigSource::Default, "Total number of neurons in the network", "int");
    set("region_count", 1, ConfigSource::Default, "Number of neural regions", "int");
    set("connection_probability", 0.15f, ConfigSource::Default, "Probability of synaptic connections", "double");
    
    // Neuron dynamics
    set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation time step in seconds", "double");
    set("max_firing_rate", 100.0, ConfigSource::Default, "Maximum firing rate in Hz", "double");
    set("refractory_period", 0.002, ConfigSource::Default, "Neuron refractory period in seconds", "double");
    
    // Plasticity parameters
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP LTP learning rate", "double");
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP LTD learning rate", "double");
    set("stdp_tau", 20.0f, ConfigSource::Default, "STDP eligibility trace time constant", "double");
    set("stdp_theta", 0.5f, ConfigSource::Default, "STDP threshold for eligibility traces", "double");
    
    // Hebbian plasticity
    set("hebbian_learning_rate", 0.01f, ConfigSource::Default, "Hebbian learning rate", "double");
    set("hebbian_trace_decay", 0.995f, ConfigSource::Default, "Hebbian trace decay rate", "double");
    
    // Structural plasticity
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Rate of new synapse formation", "double");
    set("pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse elimination", "double");
    set("pruning_threshold", 0.01f, ConfigSource::Default, "Synaptic weight threshold for pruning", "double");
    
    // Neuromodulation
    set("dopamine_sensitivity", 0.1f, ConfigSource::Default, "Dopamine neuromodulation sensitivity", "double");
    set("curiosity_rate", 0.05f, ConfigSource::Default, "Curiosity-driven exploration rate", "double");
    set("novelty_sensitivity", 0.2f, ConfigSource::Default, "Novelty detection sensitivity", "double");
    
    // Development
    set("plasticity_decay_rate", 0.999f, ConfigSource::Default, "Plasticity decay rate over time", "double");
    set("critical_period_end", 1000, ConfigSource::Default, "Simulation step when critical period ends", "int");
    set("developmental_stage", 0, ConfigSource::Default, "Current developmental stage (0=initial, 1=critical, 2=adult)", "int");
    
    // Performance optimizations
    set("sparse_connectivity", true, ConfigSource::Default, "Use sparse connectivity pattern", "bool");
    set("vectorization_level", 3, ConfigSource::Default, "SIMD vectorization level (1-3)", "int");
    set("parallel_processing", true, ConfigSource::Default, "Enable parallel processing", "bool");
    set("memory_pool_size", 1024, ConfigSource::Default, "Size of memory pool in KB", "int");
    
    // Learning configuration
    set("target_stability", 0.95, ConfigSource::Default, "Target stability score", "double");
    set("learning_rate", 0.001, ConfigSource::Default, "General learning rate", "double");
    set("exploration_rate", 0.1, ConfigSource::Default, "Exploration vs exploitation balance", "double");
    
    // Output and monitoring
    set("log_level", 2, ConfigSource::Default, "Logging level (0=off, 1=error, 2=warning, 3=info, 4=debug)", "int");
    set("checkpoint_interval", 100, ConfigSource::Default, "Simulation steps between checkpoints", "int");
    set("enable_performance_profiling", true, ConfigSource::Default, "Enable performance profiling", "bool");
    
    // Advanced options
    set("enable_structural_plasticity", true, ConfigSource::Default, "Enable structural plasticity", "bool");
    set("enable_reward_modulation", true, ConfigSource::Default, "Enable reward-based learning", "bool");
    set("enable_curiosity", true, ConfigSource::Default, "Enable curiosity-driven learning", "bool");
    set("enable_novelty_detection", true, ConfigSource::Default, "Enable novelty detection", "bool");
    
    // Configuration groups
    addToGroup("neuron_count", "network");
    addToGroup("region_count", "network");
    addToGroup("connection_probability", "network");
    addToGroup("simulation_timestep", "dynamics");
    addToGroup("max_firing_rate", "dynamics");
    addToGroup("refractory_period", "dynamics");
    addToGroup("stdp_ltp_weight", "plasticity");
    addToGroup("stdp_ltd_weight", "plasticity");
    addToGroup("stdp_tau", "plasticity");
    addToGroup("stdp_theta", "plasticity");
    addToGroup("hebbian_learning_rate", "plasticity");
    addToGroup("hebbian_trace_decay", "plasticity");
    addToGroup("synaptogenesis_rate", "plasticity");
    addToGroup("pruning_rate", "plasticity");
    addToGroup("pruning_threshold", "plasticity");
    addToGroup("dopamine_sensitivity", "neuromodulation");
    addToGroup("curiosity_rate", "neuromodulation");
    addToGroup("novelty_sensitivity", "neuromodulation");
    addToGroup("plasticity_decay_rate", "development");
    addToGroup("critical_period_end", "development");
    addToGroup("developmental_stage", "development");
    addToGroup("sparse_connectivity", "performance");
    addToGroup("vectorization_level", "performance");
    addToGroup("parallel_processing", "performance");
    addToGroup("memory_pool_size", "performance");
    addToGroup("target_stability", "learning");
    addToGroup("learning_rate", "learning");
    addToGroup("exploration_rate", "learning");
    addToGroup("log_level", "output");
    addToGroup("checkpoint_interval", "output");
    addToGroup("enable_performance_profiling", "output");
    addToGroup("enable_structural_plasticity", "advanced");
    addToGroup("enable_reward_modulation", "advanced");
    addToGroup("enable_curiosity", "advanced");
    addToGroup("enable_novelty_detection", "advanced");
}

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON parsing first
    try {
        auto jsonData = nlohmann::json::parse(content);
        return loadFromJSON(jsonData.dump(), validate);
    } catch (const nlohmann::json::parse_error& e) {
        // Fall back to simple key=value format
        std::ifstream file2(filepath);
        if (!file2.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file2, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File, "Loaded from file");
            }
        }
        file2.close();
        
        if (validate) {
            validate();
        }
        return true;
    }
}

bool Config::loadFromJSON(const std::string& jsonString, bool validate) {
    try {
        auto jsonData = nlohmann::json::parse(jsonString);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            auto value = it.value();
            
            ConfigValue configValue;
            std::string type;
            
            if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
                type = "int64";
            } else if (value.is_number_unsigned()) {
                configValue = static_cast<int64_t>(value.get<uint64_t>());
                type = "int64";
            } else if (value.is_number()) {
                configValue = value.get<double>();
                type = "double";
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
                type = "bool";
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
                type = "string";
            } else if (value.is_array()) {
                if (!value.empty() && value[0].is_number_integer()) {
                    std::vector<int> intArray = value.get<std::vector<int>>();
                    configValue = intArray;
                    type = "int_array";
                } else if (!value.empty() && value[0].is_number()) {
                    std::vector<double> doubleArray = value.get<std::vector<double>>();
                    configValue = doubleArray;
                    type = "double_array";
                } else if (!value.empty() && value[0].is_string()) {
                    std::vector<std::string> stringArray = value.get<std::vector<std::string>>();
                    configValue = stringArray;
                    type = "string_array";
                } else {
                    configValue = std::vector<int>();
                    type = "int_array";
                }
            } else {
                configValue = std::string("null");
                type = "string";
            }
            
            set(key, configValue, ConfigSource::File, "Loaded from JSON", type);
        }
        
        if (validate) {
            validate();
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
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
    
    // Write as JSON for better interoperability
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJSON(entry.value);
    }
    
    file << jsonData.dump(2); // Pretty-printed JSON with 2-space indentation
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    std::string desc = description;
    std::string t = type;
    
    if (desc == "auto") {
        desc = "";
    }
    if (t == "auto") {
        t = detectType(value);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
    }
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!desc.empty()) it->description = desc;
        if (!t.empty()) it->type = t;
        
        // Validate if requested
        if (pImpl->validateOnSet && !validateKey(key)) {
            // Validation failed, revert
            it->value = oldValue;
            return;
        }
    } else {
        ConfigEntry newEntry(key, value, source, desc, t);
        pImpl->entries.push_back(newEntry);
    }
    
    // Track change if enabled
    if (pImpl->trackChanges) {
        ConfigChange change;
        change.key = key;
        change.oldValue = oldValue;
        change.newValue = value;
        change.source = source;
        change.timestamp = std::chrono::system_clock::now();
        change.changeId = generateChangeId();
        pImpl->changeHistory.push_back(change);
        
        // Limit history size
        if (pImpl->changeHistory.size() > pImpl->maxHistorySize) {
            pImpl->changeHistory.erase(pImpl->changeHistory.begin());
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source,
                 const std::string& description, const std::string& type) {
    set(key, ConfigValue(value), source, description, type);
}

template<typename T>
void Config::setAdvanced(const std::string& key, const T& value, ConfigSource source,
                         const std::string& description, bool validate, const std::string& type) {
    std::string t = type;
    if (t.empty()) {
        t = detectType(ConfigValue(value));
    }
    
    set(key, ConfigValue(value), source, description, t);
    
    if (validate && !validateKey(key)) {
        remove(key);
        throw std::runtime_error("Configuration validation failed for key: " + key);
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
    }
    
    // Also remove from groups and dependencies
    pImpl->keyToGroups.erase(key);
    pImpl->keyToDependencies.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByType(const std::string& type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.type == type) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& group) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->keyToGroups) {
        if (pair.second.find(group) != pair.second.end()) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

std::vector<std::string> Config::getGroups() const {
    return pImpl->groups;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyToGroups.clear();
    pImpl->keyToDependencies.clear();
    pImpl->templates.clear();
    pImpl->validationRules.clear();
    pImpl->changeHistory.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    // Count by source
    std::map<ConfigSource, int> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  Source distribution:";
    for (const auto& pair : sourceCount) {
        oss << " " << static_cast<int>(pair.first) << ":" << pair.second;
    }
    oss << "\n";
    
    // Count by type
    std::map<std::string, int> typeCount;
    for (const auto& entry : pImpl->entries) {
        typeCount[entry.type]++;
    }
    
    oss << "  Type distribution:";
    for (const auto& pair : typeCount) {
        oss << " " << pair.first << ":" << pair.second;
    }
    oss << "\n";
    
    // Show some key values
    oss << "  Key values:";
    int shown = 0;
    for (const auto& entry : pImpl->entries) {
        if (shown >= 5) break;
        oss << " " << entry.key << "=[";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ",";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "]";
        shown++;
    }
    oss << "\n";
    
    return oss.str();
}

// Additional implementation methods would continue here...

} // namespace nlm