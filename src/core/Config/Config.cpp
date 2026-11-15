#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// JSON parsing methods
nlohmann::json Config::parseSimpleConfigLine(const std::string& line) {
    nlohmann::json result;
    
    // Try to parse as simple key=value pair first
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Try to determine value type
        try {
            // Try boolean
            if (valueStr == "true" || valueStr == "false") {
                bool boolVal = (valueStr == "true");
                result[key] = boolVal;
                return result;
            }
            
            // Try integer
            size_t posInt;
            int intVal = std::stoi(valueStr, &posInt);
            if (posInt == valueStr.size()) {
                result[key] = intVal;
                return result;
            }
            
            // Try int64
            int64_t int64Val = std::stoll(valueStr, &posInt);
            if (posInt == valueStr.size()) {
                result[key] = int64Val;
                return result;
            }
            
            // Try double
            size_t posDouble;
            double doubleVal = std::stod(valueStr, &posDouble);
            if (posDouble == valueStr.size()) {
                result[key] = doubleVal;
                return result;
            }
            
            // Default to string
            result[key] = valueStr;
        } catch (...) {
            // If parsing fails, treat as string
            result[key] = valueStr;
        }
    }
    
    return result;
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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // First, try to parse as JSON
    try {
        auto jsonConfig = nlohmann::json::parse(content);
        clear();
        
        for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(*it);
            set(key, value, ConfigSource::File);
            
            // Try to get description from comments or set default
            if (key == "random_seed") {
                set(key, value, ConfigSource::File); // Re-set with description
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Not JSON, try simple key=value format
        clear();
        
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            auto jsonConfig = parseSimpleConfigLine(line);
            if (!jsonConfig.empty()) {
                for (auto it = jsonConfig.begin(); it != jsonConfig.end(); ++it) {
                    std::string key = it.key();
                    ConfigValue value = jsonToConfigValue(*it);
                    set(key, value, ConfigSource::File);
                }
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
        return true;
    }
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

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || 
                      std::is_same_v<T, int64_t> ||
                      std::is_same_v<T, double> ||
                      std::is_same_v<T, bool>) {
            return nlohmann::json(val);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json(val);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : val) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : val) {
                arr.push_back(item);
            }
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json arr = nlohmann::json::array();
            for (const auto& item : val) {
                arr.push_back(item);
            }
            return arr;
        } else {
            return nlohmann::json(nullptr);
        }
    }, value);
}

ConfigValue Config::jsonToConfigValue(const nlohmann::json& json) {
    if (json.is_null()) {
        return std::string("");
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_number_integer()) {
        // Check if it fits in int64_t
        if (json.get<int64_t>() <= std::numeric_limits<int>::max() &&
            json.get<int64_t>() >= std::numeric_limits<int>::min()) {
            return json.get<int>();
        }
        return json.get<int64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        // Determine array type
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& item : json) {
            if (!item.is_number_integer()) allInts = false;
            if (!item.is_number_float()) allDoubles = false;
            if (!item.is_string()) allStrings = false;
        }
        
        if (allInts) {
            std::vector<int> intVec;
            for (const auto& item : json) {
                intVec.push_back(item.get<int>());
            }
            return intVec;
        } else if (allDoubles) {
            std::vector<double> doubleVec;
            for (const auto& item : json) {
                doubleVec.push_back(item.get<double>());
            }
            return doubleVec;
        } else if (allStrings) {
            std::vector<std::string> stringVec;
            for (const auto& item : json) {
                stringVec.push_back(item.get<std::string>());
            }
            return stringVec;
        }
        
        // Default to double vector
        std::vector<double> doubleVec;
        for (const auto& item : json) {
            if (item.is_number()) {
                doubleVec.push_back(item.get<double>());
            } else {
                doubleVec.push_back(std::stod(item.get<std::string>()));
            }
        }
        return doubleVec;
    }
    
    return std::string("");
}

nlohmann::json Config::toJson() const {
    nlohmann::json jsonConfig;
    
    for (const auto& entry : pImpl->entries) {
        jsonConfig[entry.key] = configValueToJson(entry.value);
    }
    
    return jsonConfig;
}

std::shared_ptr<Config> Config::fromJson(const nlohmann::json& json) {
    auto config = std::make_shared<Config>();
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = jsonToConfigValue(*it);
        config->set(key, value, ConfigSource::File);
    }
    
    return config;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write in JSON format for better readability and parsing
    auto jsonConfig = toJson();
    
    // Pretty print JSON with indentation
    file << jsonConfig.dump(2);
    
    NLM_LOG_INFO("Configuration saved to JSON file: " + filepath);
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Configuration validation implementation

bool Config::validateKey(const std::string& key, std::string& errorMessage) {
    // List of valid configuration keys
    const std::vector<std::string> validKeys = {
        // Core parameters
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability",
        
        // Plasticity parameters
        "plasticity_learning_rate", "stdp_ltp_weight", "stdp_ltd_weight",
        "stdp_tau", "stdp_threshold",
        
        // Development parameters
        "synaptogenesis_rate", "pruning_rate", "maturation_rate",
        "developmental_stage",
        
        // Neuromodulation parameters
        "dopamine_baseline", "curiosity_rate", "novelty_rate",
        "reward_prediction_error_rate",
        
        // Memory parameters
        "working_memory_capacity", "episodic_memory_capacity",
        "associative_memory_strength",
        
        // Prediction parameters
        "prediction_horizon", "prediction_error_threshold",
        "confidence_threshold",
        
        // Environment parameters
        "environment_width", "environment_height", "vision_width",
        "vision_height", "energy_decay_rate", "hazard_damage",
        
        // Reward parameters
        "reward_discount_factor", "reward_scaling_factor",
        "resource_reward_value", "hazard_penalty_value",
        
        // Simulation parameters
        "max_simulation_steps", "simulation_time_limit",
        "checkpoint_interval", "replay_interval",
        
        // Logging parameters
        "log_level", "log_to_file", "log_filename",
        "log_rotation_size", "log_retention_days",
        
        // Visualization parameters
        "visualization_enabled", "visualization_update_rate",
        "visualization_width", "visualization_height",
        
        // Performance parameters
        "memory_pool_size", "event_queue_size", "thread_count",
        "simd_enabled", "parallel_processing_enabled",
        
        // Development parameters
        "enable_development", "enable_structural_plasticity",
        "enable_reward_modulation", "enable_curiosity",
        "enable_novelty", "enable_prediction",
        
        // Special parameters
        "random_seed_initial", "debug_mode", "validation_level"
    };
    
    // Check if key is empty
    if (key.empty()) {
        errorMessage = "Configuration key cannot be empty";
        return false;
    }
    
    // Check if key is valid
    bool isValid = std::find(validKeys.begin(), validKeys.end(), key) != validKeys.end();
    
    if (!isValid) {
        errorMessage = "Invalid configuration key: '" + key + "'. Valid keys: " + 
                      "random_seed, simulation_timestep, neuron_count, region_count, " +
                      "connection_probability, plasticity_learning_rate, stdp_ltp_weight, " +
                      "stdp_ltd_weight, stdp_tau, synaptogenesis_rate, pruning_rate, " +
                      "dopamine_baseline, curiosity_rate, novelty_rate, " +
                      "working_memory_capacity, episodic_memory_capacity, " +
                      "prediction_horizon, environment_width, environment_height, " +
                      "reward_discount_factor, max_simulation_steps, log_level, " +
                      "enable_development, enable_structural_plasticity, etc.";
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, std::string& errorMessage) {
    // Check if key is valid first
    std::string tempError;
    if (!validateKey(key, tempError)) {
        errorMessage = "Invalid key: " + tempError;
        return false;
    }
    
    // Validate based on key type
    if (key == "random_seed" || key == "seed" || key == "random_seed_initial") {
        // Integer validation
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val < 0) {
                errorMessage = "random_seed must be non-negative (got " + std::to_string(val) + ")";
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (val < 0) {
                errorMessage = "random_seed must be non-negative (got " + std::to_string(val) + ")";
                return false;
            }
        }
    } 
    else if (key == "simulation_timestep" || key == "dt" || key == "timestep") {
        // Float validation - should be positive
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val <= 0.0) {
                errorMessage = "simulation_timestep must be positive (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "neuron_count") {
        // Integer validation - should be reasonable
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0 || val > 10000) {
                errorMessage = "neuron_count must be between 1 and 10000 (got " + std::to_string(val) + ")";
                return false;
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t val = std::get<int64_t>(value);
            if (val <= 0 || val > 10000) {
                errorMessage = "neuron_count must be between 1 and 10000 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "region_count") {
        // Integer validation - should be reasonable
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0 || val > 100) {
                errorMessage = "region_count must be between 1 and 100 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "connection_probability") {
        // Float validation - should be between 0 and 1
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                errorMessage = "connection_probability must be between 0.0 and 1.0 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || key == "plasticity_learning_rate") {
        // Float validation - should be reasonable
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < -1.0 || val > 1.0) {
                errorMessage = key + " should be between -1.0 and 1.0 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "stdp_tau") {
        // Float validation - should be positive
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val <= 0.0) {
                errorMessage = "stdp_tau must be positive (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "synaptogenesis_rate" || key == "pruning_rate" || key == "maturation_rate") {
        // Float validation - should be non-negative and reasonable
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                errorMessage = key + " should be between 0.0 and 1.0 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "dopamine_baseline" || key == "curiosity_rate" || key == "novelty_rate") {
        // Float validation - should be reasonable
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < -1.0 || val > 1.0) {
                errorMessage = key + " should be between -1.0 and 1.0 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "environment_width" || key == "environment_height") {
        // Integer validation - should be positive
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0) {
                errorMessage = key + " must be positive (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "vision_width" || key == "vision_height") {
        // Integer validation - should be reasonable
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0 || val > 100) {
                errorMessage = key + " should be between 1 and 100 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "reward_discount_factor") {
        // Float validation - should be between 0 and 1
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                errorMessage = "reward_discount_factor must be between 0.0 and 1.0 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    else if (key == "max_simulation_steps") {
        // Integer validation - should be reasonable
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (val <= 0 || val > 1000000) {
                errorMessage = "max_simulation_steps should be between 1 and 1,000,000 (got " + std::to_string(val) + ")";
                return false;
            }
        }
    }
    
    return true;
}

bool Config::isValidRange(const std::string& key, const ConfigValue& value) {
    // Quick check for range validity
    if (!validateKey(key, std::string())) {
        return false;
    }
    
    std::string tempError;
    return validateValue(key, value, tempError);
}

std::string Config::getValidationWarning(const std::string& key, const ConfigValue& value) {
    std::string warning = "";
    
    // Check for suspicious values
    if (key == "neuron_count" && std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val > 5000) {
            warning = "WARNING: High neuron count (" + std::to_string(val) + ") may cause performance issues. Consider using 1000-2000 neurons for simulations.";
        }
    }
    else if (key == "connection_probability" && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val > 0.5) {
            warning = "WARNING: High connection probability (" + std::to_string(val) + ") may cause dense connectivity. Consider 0.1-0.3 for sparser connections.";
        }
    }
    
    else if (key == "stdp_ltp_weight" && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < 0.001) {
            warning = "WARNING: Very low STDP LTP weight (" + std::to_string(val) + ") may not cause significant learning.";
        } else if (val > 0.1) {
            warning = "WARNING: Very high STDP LTP weight (" + std::to_string(val) + ") may cause unstable learning.";
        }
    }
    
    return warning;
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string errorMessage;
        if (!validateValue(entry.key, entry.value, errorMessage)) {
            errors.push_back("Key '" + entry.key + "': " + errorMessage);
        }
        
        std::string warning = getValidationWarning(entry.key, entry.value);
        if (!warning.empty()) {
            errors.push_back("WARNING: Key '" + entry.key + "': " + warning);
        }
    }
    
    return errors;
}

std::string Config::getParameterDescription(const std::string& key) {
    // Map of parameter descriptions
    static const std::map<std::string, std::string> parameterDescriptions = {
        {"random_seed", "Random seed for reproducible simulations. Use a positive integer."},
        {"simulation_timestep", "Duration of each simulation step in seconds. Should be small (e.g., 0.001)."},
        {"neuron_count", "Number of neurons in the brain. Typical range: 100-5000."},
        {"region_count", "Number of brain regions. Typical range: 1-10."},
        {"connection_probability", "Probability of connections between neurons. Typical range: 0.05-0.2."},
        {"stdp_ltp_weight", "STDP long-term potentiation weight. Typical range: 0.01-0.05."},
        {"stdp_ltd_weight", "STDP long-term depression weight. Typical range: 0.01-0.05."},
        {"stdp_tau", "STDP time constant in simulation steps. Typical range: 10-100."},
        {"synaptogenesis_rate", "Rate of new synapse formation. Typical range: 0.0001-0.001."},
        {"pruning_rate", "Rate of synapse elimination. Typical range: 0.00001-0.0001."},
        {"dopamine_baseline", "Baseline dopamine level. Typical range: 0.0-0.5."},
        {"curiosity_rate", "Curiosity modulation rate. Typical range: 0.0-0.2."},
        {"novelty_rate", "Novelty detection rate. Typical range: 0.0-0.2."},
        {"working_memory_capacity", "Working memory capacity in number of items. Typical range: 5-20."},
        {"episodic_memory_capacity", "Episodic memory capacity in number of episodes. Typical range: 10-100."},
        {"prediction_horizon", "Time horizon for prediction in simulation steps. Typical range: 10-50."},
        {"environment_width", "Width of the simulation environment. Positive value required."},
        {"environment_height", "Height of the simulation environment. Positive value required."},
        {"vision_width", "Width of agent's visual field. Typical range: 5-20."},
        {"vision_height", "Height of agent's visual field. Typical range: 5-20."},
        {"reward_discount_factor", "Factor for future reward discounting. Typical range: 0.9-0.99."},
        {"max_simulation_steps", "Maximum number of simulation steps. Typical range: 100-10000."},
        {"log_level", "Logging level: DEBUG, INFO, WARN, or ERROR."},
        {"enable_development", "Enable/disable developmental processes."},
        {"enable_structural_plasticity", "Enable/disable structural plasticity."},
        {"enable_reward_modulation", "Enable/disable reward modulation."},
        {"enable_curiosity", "Enable/disable curiosity-driven exploration."}
    };
    
    auto it = parameterDescriptions.find(key);
    if (it != parameterDescriptions.end()) {
        return it->second;
    }
    
    return "Unknown parameter. Check documentation for valid parameter names.";
}

std::vector<std::string> Config::getValidParameterKeys() {
    static const std::vector<std::string> validKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau",
        "synaptogenesis_rate", "pruning_rate", "maturation_rate",
        "dopamine_baseline", "curiosity_rate", "novelty_rate",
        "working_memory_capacity", "episodic_memory_capacity",
        "prediction_horizon", "prediction_error_threshold",
        "confidence_threshold", "environment_width", "environment_height",
        "vision_width", "vision_height", "energy_decay_rate", "hazard_damage",
        "reward_discount_factor", "reward_scaling_factor",
        "resource_reward_value", "hazard_penalty_value",
        "max_simulation_steps", "simulation_time_limit",
        "checkpoint_interval", "replay_interval",
        "log_level", "log_to_file", "log_filename",
        "visualization_enabled", "visualization_update_rate",
        "enable_development", "enable_structural_plity",
        "enable_reward_modulation", "enable_curiosity", "enable_novelty",
        "enable_prediction", "debug_mode", "validation_level"
    };
    
    return validKeys;
}

bool Config::isValidParameterKey(const std::string& key) {
    const std::vector<std::string>& validKeys = getValidParameterKeys();
    return std::find(validKeys.begin(), validKeys.end(), key) != validKeys.end();
}

std::string Config::getParameterType(const std::string& key) {
    // Map of parameter types
    static const std::map<std::string, std::string> parameterTypes = {
        {"random_seed", "integer"},
        {"simulation_timestep", "float"},
        {"neuron_count", "integer"},
        {"region_count", "integer"},
        {"connection_probability", "float"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_tau", "float"},
        {"synaptogenesis_rate", "float"},
        {"pruning_rate", "float"},
        {"dopamine_baseline", "float"},
        {"curiosity_rate", "float"},
        {"novelty_rate", "float"},
        {"working_memory_capacity", "integer"},
        {"episodic_memory_capacity", "integer"},
        {"prediction_horizon", "integer"},
        {"environment_width", "integer"},
        {"environment_height", "integer"},
        {"vision_width", "integer"},
        {"vision_height", "integer"},
        {"reward_discount_factor", "float"},
        {"max_simulation_steps", "integer"},
        {"log_level", "string"},
        {"enable_development", "boolean"},
        {"enable_structural_plity", "boolean"},
        {"enable_reward_modulation", "boolean"},
        {"enable_curiosity", "boolean"},
        {"enable_novelty", "boolean"},
        {"enable_prediction", "boolean"},
        {"debug_mode", "boolean"},
        {"validation_level", "integer"}
    };
    
    auto it = parameterTypes.find(key);
    if (it != parameterTypes.end()) {
        return it->second;
    }
    
    return "unknown";
}

bool Config::validateParameterConstraints(const std::string& key, const ConfigValue& value) {
    // Parameter-specific constraint validation
    if (key == "neuron_count") {
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            int64_t val = std::holds_alternative<int>(value) ? std::get<int>(value) : std::get<int64_t>(value);
            if (val > 5000) {
                return false; // Too many neurons, may cause performance issues
            }
        }
    }
    
    else if (key == "connection_probability") {
        if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (val > 0.5) {
                return false; // Too dense connectivity
            }
        }
    }
    
    else if (key == "stdp_ltp_weight" && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < 0.001 || val > 0.1) {
            return false; // Extreme values may cause instability
        }
    }
    
    return true;
}

std::string Config::getSuspicionWarning(const std::string& key, const ConfigValue& value) {
    std::string warning = "";
    
    if (key == "neuron_count" && std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val > 5000) {
            warning = "WARNING: Simulation may run slow with this many neurons.";
        }
    }
    
    else if (key == "max_simulation_steps" && std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val > 1000000) {
            warning = "WARNING: Very long simulations may consume significant resources.";
        }
    }
    
    else if (key == "log_level" && std::holds_alternative<std::string>(value)) {
        std::string val = std::get<std::string>(value);
        if (val == "DEBUG" || val == "INFO" || val == "WARN" || val == "ERROR") {
            // Valid log levels
        } else {
            warning = "WARNING: Invalid log level. Using INFO instead.";
        }
    }
    
    return warning;
}

void Config::validateAndWarn() const {
    std::vector<std::string> errors = validateAll();
    
    for (const auto& error : errors) {
        if (error.find("WARNING:") == 0) {
            NLM_LOG_WARN(error.substr(8));
        } else {
            NLM_LOG_ERROR(error);
        }
    }
}

void Config::clearValidationWarnings() {
    // Clear validation warnings - implementation would depend on logging system
    // For now, this is a placeholder for future enhancement
}

std::vector<std::string> Config::getAllValidationWarnings() const {
    return validateAll();
}