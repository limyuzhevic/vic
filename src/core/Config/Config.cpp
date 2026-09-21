#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <ctime>
#include <iomanip>

namespace nlm {

// Helper function to convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to validate configuration key format
static bool isValidKey(const std::string& key) {
    if (key.empty() || key.length() > 100) return false;
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-' || c == '.')) {
            return false;
        }
    }
    return true;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::function<std::string()>> defaultValueSuppliers;
};

// JSON serialization helper for ConfigValue
static void to_json(nlohmann::json& j, const ConfigValue& cv) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, cv);
}

static void from_json(const nlohmann::json& j, ConfigValue& cv) {
    try {
        if (j.is_number_integer()) {
            cv = j.get<int>();
        } else if (j.is_number_unsigned()) {
            cv = j.get<int64_t>();
        } else if (j.is_number()) {
            cv = j.get<double>();
        } else if (j.is_boolean()) {
            cv = j.get<bool>();
        } else if (j.is_string()) {
            cv = j.get<std::string>();
        } else if (j.is_array()) {
            // Check array element types
            auto element_types = j.element_count() > 0 ? 
                nlohmann::json(j[0]).type() : nlohmann::json::value_t::null;
            
            std::vector<int> int_vec;
            std::vector<double> double_vec;
            std::vector<std::string> string_vec;
            
            for (const auto& elem : j) {
                if (elem.is_number_integer()) {
                    int_vec.push_back(elem.get<int>());
                } else if (elem.is_number()) {
                    double_vec.push_back(elem.get<double>());
                } else if (elem.is_string()) {
                    string_vec.push_back(elem.get<std::string>());
                }
            }
            
            if (!int_vec.empty() && int_vec.size() == j.size()) {
                cv = int_vec;
            } else if (!double_vec.empty() && double_vec.size() == j.size()) {
                cv = double_vec;
            } else if (!string_vec.empty() && string_vec.size() == j.size()) {
                cv = string_vec;
            }
        }
    } catch (const std::exception&) {
        throw;
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

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

static void from_json(const nlohmann::json& j, ConfigValue& cv) {
    try {
        if (j.is_number_integer()) {
            cv = j.get<int>();
        } else if (j.is_number_unsigned()) {
            cv = j.get<int64_t>();
        } else if (j.is_number()) {
            cv = j.get<double>();
        } else if (j.is_boolean()) {
            cv = j.get<bool>();
        } else if (j.is_string()) {
            cv = j.get<std::string>();
        } else if (j.is_array()) {
            // Check array element types
            auto element_types = j.element_count() > 0 ? 
                nlohmann::json(j[0]).type() : nlohmann::json::value_t::null;
            
            std::vector<int> int_vec;
            std::vector<double> double_vec;
            std::vector<std::string> string_vec;
            
            for (const auto& elem : j) {
                if (elem.is_number_integer()) {
                    int_vec.push_back(elem.get<int>());
                } else if (elem.is_number()) {
                    double_vec.push_back(elem.get<double>());
                } else if (elem.is_string()) {
                    string_vec.push_back(elem.get<std::string>());
                }
            }
            
            if (!int_vec.empty() && int_vec.size() == j.size()) {
                cv = int_vec;
            } else if (!double_vec.empty() && double_vec.size() == j.size()) {
                cv = double_vec;
            } else if (!string_vec.empty() && string_vec.size() == j.size()) {
                cv = string_vec;
            }
        }
    } catch (const std::exception&) {
        throw;
    }
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default configuration values
    registerDefaultValues();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::registerDefaultValues() {
    // Register default configuration values for NLM Phase 2
    
    // Basic simulation parameters
    defaultValueSuppliers["random_seed"] = []() { return "42"; };
    defaultValueSuppliers["simulation_timestep"] = []() { return "0.001"; };
    defaultValueSuppliers["neuron_count"] = []() { return "500"; };
    defaultValueSuppliers["region_count"] = []() { return "1"; };
    defaultValueSuppliers["connection_probability"] = []() { return "0.15"; };
    
    // STDP parameters
    defaultValueSuppliers["stdp_ltp_weight"] = []() { return "0.02"; };
    defaultValueSuppliers["stdp_ltd_weight"] = []() { return "0.015"; };
    defaultValueSuppliers["stdp_tau"] = []() { return "20.0"; };
    
    // Structural plasticity parameters
    defaultValueSuppliers["synaptogenesis_rate"] = []() { return "0.0001"; };
    defaultValueSuppliers["pruning_rate"] = []() { return "0.00001"; };
    
    // Memory system parameters
    defaultValueSuppliers["working_memory_capacity"] = []() { return "50"; };
    defaultValueSuppliers["episodic_memory_max_episodes"] = []() { return "1000"; };
    defaultValueSuppliers["consolidation_interval"] = []() { return "1000"; };
    defaultValueSuppliers["replay_interval"] = []() { return "100"; };
    
    // Neuromodulation parameters
    defaultValueSuppliers["dopamine_sensitivity"] = []() { return "1.0"; };
    defaultValueSuppliers["curiosity_gain"] = []() { return "0.5"; };
    defaultValueSuppliers["novelty_threshold"] = []() { return "0.3"; };
    
    // Reward parameters
    defaultValueSuppliers["reward_scale"] = []() { return "1.0"; };
    defaultValueSuppliers["prediction_error_decay"] = []() { return "0.95"; };
    
    // Development parameters
    defaultValueSuppliers["initial_plasticity"] = []() { return "1.0"; };
    defaultValueSuppliers["critical_period_duration"] = []() { return "300.0"; };
    
    // Logging parameters
    defaultValueSuppliers["log_level"] = []() { return "Info"; };
    defaultValueSuppliers["enable_profiling"] = []() { return "false"; };
    defaultValueSuppliers["checkpoint_dir"] = []() { return "./checkpoints"; };
    defaultValueSuppliers["checkpoint_interval"] = []() { return "10000"; };
    defaultValueSuppliers["checkpoint_compression"] = []() { return "true"; };
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then fallback to simple format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON entries
        for (auto& [key, value] : jsonData.items()) {
            if (!isValidKey(key)) {
                NLM_LOG_WARNING("Invalid config key: " + key + " - skipping");
                continue;
            }
            
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            
            // Set description based on key
            entry.description = getDefaultDescription(key);
            
            try {
                entry.value = value.get<ConfigValue>();
                setInternal(entry);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse config value for key '" + key + "': " + e.what());
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    } catch (const nlohmann::json::parse_error&) {
        // JSON parsing failed, fallback to simple format
        file.clear();
        file.seekg(0);
    }
    
    // Simple format parser (original implementation)
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
            NLM_LOG_DEBUG("Parsed config line " + std::to_string(lineNumber) + ": " + key + " = " + value);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool loaded = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (setFromCommandLine(key, value)) {
                    loaded = true;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (setFromCommandLine(key, value)) {
                loaded = true;
            }
        }
        // Handle boolean flags
        else if (arg == "--enable_debug" || arg == "--disable_debug") {
            std::string key = "log_level";
            std::string value = (arg == "--enable_debug") ? "Debug" : "Info";
            if (setFromCommandLine(key, value)) {
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool Config::setFromCommandLine(const std::string& key, const std::string& valueStr) {
    if (!isValidKey(key)) {
        NLM_LOG_WARNING("Invalid config key from command line: " + key);
        return false;
    }
    
    // Try to determine type based on value string
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", ConfigSource::CommandLine);
        return true;
    }
    
    // Check if it's an integer
    bool isInteger = true;
    size_t pos = 0;
    long int_value = std::stol(valueStr, &pos);
    if (pos == valueStr.size() && int_value >= INT_MIN && int_value <= INT_MAX) {
        set(key, int_value, ConfigSource::CommandLine);
        return true;
    }
    
    // Check if it's a double
    bool isDouble = true;
    try {
        size_t decPos = valueStr.find('.');
        if (decPos != std::string::npos) {
            size_t expPos = valueStr.find('e') + valueStr.find('E');
            if (expPos == std::string::npos || expPos > decPos) {
                double double_value = std::stod(valueStr);
                set(key, double_value, ConfigSource::CommandLine);
                return true;
            }
        }
    } catch (...) {
        isDouble = false;
    }
    
    // Default to string
    set(key, valueStr, ConfigSource::CommandLine);
    return true;
}

std::string Config::getDefaultDescription(const std::string& key) const {
    const std::unordered_map<std::string, std::string> descriptions = {
        {"random_seed", "Random number generator seed for reproducible experiments"},
        {"simulation_timestep", "Time step size in seconds for neural simulation"},
        {"neuron_count", "Total number of neurons in the brain"},
        {"region_count", "Number of neural regions"},
        {"connection_probability", "Probability of synaptic connections between neurons"},
        {"stdp_ltp_weight", "Spike-Timing-Dependent Plasticity Long-Term Potentiation weight"},
        {"stdp_ltd_weight", "Spike-Timing-Dependent Plasticity Long-Term Depression weight"},
        {"stdp_tau", "STDP trace decay time constant (in simulation steps)"},
        {"synaptogenesis_rate", "Rate of new synapse formation (per step)"},
        {"pruning_rate", "Rate of synapse elimination (per step)"},
        {"working_memory_capacity", "Maximum number of items in working memory"},
        {"episodic_memory_max_episodes", "Maximum number of stored episodes"},
        {"consolidation_interval", "Steps between memory consolidation cycles"},
        {"replay_interval", "Steps between memory replay cycles"},
        {"dopamine_sensitivity", "Dopamine modulation sensitivity"},
        {"curiosity_gain", "Curiosity signal gain factor"},
        {"novelty_threshold", "Novelty threshold for triggering curiosity"},
        {"reward_scale", "Scale factor for reward signals"},
        {"prediction_error_decay", "Decay rate for prediction error tracking"},
        {"initial_plasticity", "Initial synaptic plasticity level"},
        {"critical_period_duration", "Duration of critical period (in simulation steps)"},
        {"log_level", "Logging verbosity level"},
        {"enable_profiling", "Enable performance profiling"},
        {"checkpoint_dir", "Directory for brain state checkpoints"},
        {"checkpoint_interval", "Steps between checkpoint saves"},
        {"checkpoint_compression", "Enable checkpoint compression"}
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "Configuration value for " + key;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Save in JSON format for better readability and future compatibility
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = entry.value;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write with indentation for readability
    file << jsonData.dump(2);
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it == pImpl->entries.end()) {
        // Try default value supplier
        auto itSupplier = pImpl->defaultValueSuppliers.find(key);
        if (itSupplier != pImpl->defaultValueSuppliers.end()) {
            try {
                std::string defaultValue = itSupplier->second();
                // Convert default value to type T
                return convertStringToT<T>(defaultValue);
            } catch (...) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Template function to convert string to type T
template<typename T>
static T convertStringToT(const std::string& str) {
    if constexpr (std::is_same_v<T, int>) {
        return std::stoi(str);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return std::stoll(str);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(str);
    } else if constexpr (std::is_same_v<T, bool>) {
        // Handle "true", "false", "1", "0"
        std::string lower = toLower(str);
        return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
    } else if constexpr (std::is_same_v<T, std::string>) {
        return str;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        // Parse CSV or space-separated list
        std::vector<int> result;
        std::string num;
        for (char c : str) {
            if (c == ',' || c == ' ' || c == ';') {
                if (!num.empty()) {
                    result.push_back(std::stoi(num));
                    num.clear();
                }
            } else {
                num += c;
            }
        }
        if (!num.empty()) {
            result.push_back(std::stoi(num));
        }
        return result;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        std::vector<double> result;
        std::string num;
        for (char c : str) {
            if (c == ',' || c == ' ' || c == ';') {
                if (!num.empty()) {
                    result.push_back(std::stod(num));
                    num.clear();
                }
            } else {
                num += c;
            }
        }
        if (!num.empty()) {
            result.push_back(std::stod(num));
        }
        return result;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        // For string vectors, split on comma or space
        std::vector<std::string> result;
        std::string token;
        for (char c : str) {
            if (c == ',' || c == ' ' || c == ';') {
                if (!token.empty()) {
                    result.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            result.push_back(token);
        }
        return result;
    }
    
    throw std::runtime_error("Unsupported type for string conversion");
}

void Config::setInternal(const ConfigEntry& entry) {
    // Ensure key is valid
    if (!isValidKey(entry.key)) {
        throw std::invalid_argument("Invalid configuration key: " + entry.key);
    }
    
    // Type validation based on key name
    validateConfigValue(entry.key, entry.value);
    
    // Add to entries list
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&entry](const ConfigEntry& e) { return e.key == entry.key; });
    
    if (it != pImpl->entries.end()) {
        it->value = entry.value;
        it->source = entry.source;
        it->description = entry.description;
    } else {
        pImpl->entries.push_back(entry);
    }
}

void Config::validateConfigValue(const std::string& key, const ConfigValue& value) {
    // Validate based on key name
    if (key == "random_seed") {
        auto val = std::get<int64_t>(value);
        if (val < 0 || val > 2147483647) {
            throw std::invalid_argument("random_seed must be between 0 and 2147483647");
        }
    } else if (key == "simulation_timestep") {
        auto val = std::get<double>(value);
        if (val <= 0.0 || val > 1.0) {
            throw std::invalid_argument("simulation_timestep must be between 0.001 and 1.0");
        }
    } else if (key == "neuron_count") {
        auto val = std::get<int64_t>(value);
        if (val <= 0 || val > 100000) {
            throw std::invalid_argument("neuron_count must be between 1 and 100000");
        }
    } else if (key == "region_count") {
        auto val = std::get<int64_t>(value);
        if (val <= 0 || val > 1000) {
            throw std::invalid_argument("region_count must be between 1 and 1000");
        }
    } else if (key == "connection_probability") {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            throw std::invalid_argument("connection_probability must be between 0.0 and 1.0");
        }
    } else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight") {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            throw std::invalid_argument(key + " must be between 0.0 and 1.0");
        }
    } else if (key == "stdp_tau") {
        auto val = std::get<double>(value);
        if (val <= 0.0 || val > 1000.0) {
            throw std::invalid_argument("stdp_tau must be between 0.1 and 1000.0");
        }
    } else if (key == "synaptogenesis_rate" || key == "pruning_rate") {
        auto val = std::get<double>(value);
        if (val < 0.0 || val > 1.0) {
            throw std::invalid_argument(key + " must be between 0.0 and 1.0");
        }
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    validateConfigValue(key, value);
    
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = getDefaultDescription(key);
    
    setInternal(entry);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    if (std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; })) {
        return true;
    }
    
    // Check if we have a default value supplier
    return pImpl->defaultValueSuppliers.find(key) != pImpl->defaultValueSuppliers.end();
}

void Config::remove(const std::string& key) {
    if (isValidKey(key)) {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size() + pImpl->defaultValueSuppliers.size());
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    // Add default keys
    for (const auto& supplier : pImpl->defaultValueSuppliers) {
        if (std::none_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&supplier](const ConfigEntry& e) { return e.key == supplier.first; })) {
            keys.push_back(supplier.first);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " explicit entries, " 
        << pImpl->defaultValueSuppliers.size() << " defaults):\n";
    
    // Show explicit entries
    oss << "\n=== Explicit Configuration ===\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(3) << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
        
        // Show description if not default
        if (entry.description.empty() || 
            entry.description.find("Configuration value for ") != std::string::npos) {
            oss << "      Note: " << entry.description << "\n";
        }
    }
    
    // Show default-only values
    oss << "\n=== Default Values ===\n";
    bool hasDefaults = false;
    for (const auto& supplier : pImpl->defaultValueSuppliers) {
        bool isExplicit = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&supplier](const ConfigEntry& e) { return e.key == supplier.first; });
        
        if (!isExplicit) {
            hasDefaults = true;
            oss << "  " << supplier.first << " = [";
            try {
                std::string defaultValue = supplier.second();
                // Try to format nicely
                if (supplier.first.find("flag") != std::string::npos || 
                    supplier.first.find("enable_") != std::string::npos) {
                    oss << (defaultValue == "true" ? "true" : "false");
                } else if (supplier.first.find("count") != std::string::npos || 
                           supplier.first.find("interval") != std::string::npos) {
                    oss << defaultValue;
                } else {
                    oss << "\"" << defaultValue << "\"";
                }
            } catch (...) {
                oss << "<default>";
            }
            oss << "] (default)\n";
        }
    }
    
    if (!hasDefaults) {
        oss << "  (No default values available)\n";
    }
    
    return oss.str();
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default configuration values
    registerDefaultValues();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::registerDefaultValues() {
    // Register default configuration values for NLM Phase 2
    
    // Basic simulation parameters
    defaultValueSuppliers["random_seed"] = []() { return "42"; };
    defaultValueSuppliers["simulation_timestep"] = []() { return "0.001"; };
    defaultValueSuppliers["neuron_count"] = []() { return "500"; };
    defaultValueSuppliers["region_count"] = []() { return "1"; };
    defaultValueSuppliers["connection_probability"] = []() { return "0.15"; };
    
    // STDP parameters
    defaultValueSuppliers["stdp_ltp_weight"] = []() { return "0.02"; };
    defaultValueSuppliers["stdp_ltd_weight"] = []() { return "0.015"; };
    defaultValueSuppliers["stdp_tau"] = []() { return "20.0"; };
    
    // Structural plasticity parameters
    defaultValueSuppliers["synaptogenesis_rate"] = []() { return "0.0001"; };
    defaultValueSuppliers["pruning_rate"] = []() { return "0.00001"; };
    
    // Memory system parameters
    defaultValueSuppliers["working_memory_capacity"] = []() { return "50"; };
    defaultValueSuppliers["episodic_memory_max_episodes"] = []() { return "1000"; };
    defaultValueSuppliers["consolidation_interval"] = []() { return "1000"; };
    defaultValueSuppliers["replay_interval"] = []() { return "100"; };
    
    // Neuromodulation parameters
    defaultValueSuppliers["dopamine_sensitivity"] = []() { return "1.0"; };
    defaultValueSuppliers["curiosity_gain"] = []() { return "0.5"; };
    defaultValueSuppliers["novelty_threshold"] = []() { return "0.3"; };
    
    // Reward parameters
    defaultValueSuppliers["reward_scale"] = []() { return "1.0"; };
    defaultValueSuppliers["prediction_error_decay"] = []() { return "0.95"; };
    
    // Development parameters
    defaultValueSuppliers["initial_plasticity"] = []() { return "1.0"; };
    defaultValueSuppliers["critical_period_duration"] = []() { return "300.0"; };
    
    // Logging parameters
    defaultValueSuppliers["log_level"] = []() { return "Info"; };
    defaultValueSuppliers["enable_profiling"] = []() { return "false"; };
    defaultValueSuppliers["checkpoint_dir"] = []() { return "./checkpoints"; };
    defaultValueSuppliers["checkpoint_interval"] = []() { return "10000"; };
    defaultValueSuppliers["checkpoint_compression"] = []() { return "true"; }
}
