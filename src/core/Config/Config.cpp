#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Core simulation parameters
    pImpl->schema = {
        // Core simulation parameters
        ConfigSchema{"random_seed", "int64_t", "Random seed for reproducible experiments", "42", false, {}, {}},
        ConfigSchema{"simulation_timestep", "double", "Simulation timestep in seconds", "0.001", false, {"0.0001", "10.0"}, {}},
        ConfigSchema{"neuron_count", "int64_t", "Number of neurons in the brain", "1000", false, {"1", "100000"}, {}},
        ConfigSchema{"region_count", "int64_t", "Number of brain regions", "2", false, {"1", "100"}, {}},
        ConfigSchema{"connection_probability", "double", "Connection probability for random connectivity", "0.1", false, {"0.0", "1.0"}, {}},
        
        // STDP parameters
        ConfigSchema{"stdp_ltp_weight", "float", "STDP Long-Term Potentiation weight", "0.01", false, {"0.0", "10.0"}, {}},
        ConfigSchema{"stdp_ltd_weight", "float", "STDP Long-Term Depression weight", "0.012", false, {"0.0", "10.0"}, {}},
        ConfigSchema{"stdp_tau", "float", "STDP time constant in milliseconds", "20.0", false, {"1.0", "1000.0"}, {}},
        
        // Structural plasticity
        ConfigSchema{"synaptogenesis_rate", "float", "Rate of new synapse formation", "0.001", false, {"0.0", "0.1"}, {}},
        ConfigSchema{"pruning_rate", "float", "Rate of synapse pruning", "0.0001", false, {"0.0", "0.01"}, {}},
        
        // Neuromodulation
        ConfigSchema{"dopamine_baseline", "double", "Baseline dopamine level", "0.1", false, {"0.0", "1.0"}, {}},
        ConfigSchema{"reward_discount_factor", "double", "Reward discount factor for learning", "0.99", false, {"0.0", "1.0"}, {}},
        
        // Environment
        ConfigSchema{"environment_name", "string", "Name of the environment", "GridWorld", true, {}, {}},
        ConfigSchema{"environment_width", "int64_t", "Width of environment grid", "10", true, {"1", "1000"}, {}},
        ConfigSchema{"environment_height", "int64_t", "Height of environment grid", "10", true, {"1", "1000"}, {}},
        
        // Logging
        ConfigSchema{"log_level", "string", "Logging verbosity level", "INFO", false, {"DEBUG", "INFO", "WARNING", "ERROR"}, {}},
        ConfigSchema{"log_to_file", "bool", "Whether to log to file", "false", false, {}, {}},
        ConfigSchema{"log_filename", "string", "Log file name", "nlm.log", false, {}, {}},
        
        // Performance
        ConfigSchema{"max_simulation_steps", "int64_t", "Maximum simulation steps", "10000", false, {"1", "1000000"}, {}},
        ConfigSchema{"simulation_time_limit", "double", "Simulation time limit in seconds", "0.0", false, {"0.0", "1000.0"}, {}},
        ConfigSchema{"visualization_enabled", "bool", "Whether visualization is enabled", "false", false, {}, {}},
        ConfigSchema{"visualization_update_rate", "double", "Visualization update rate in Hz", "30.0", false, {"1.0", "1000.0"}, {}},
        
        // Memory and performance
        ConfigSchema{"replay_interval", "int64_t", "Episodic memory replay interval", "100", false, {"1", "10000"}, {}},
        ConfigSchema{"consolidation_interval", "int64_t", "Memory consolidation interval", "1000", false, {"1", "10000"}, {}},
        ConfigSchema{"checkpoint_dir", "string", "Directory for checkpoint files", "./checkpoints", false, {}, {}},
    };
}

bool Config::validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Find schema entry
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->schema.end()) {
        // No schema entry found, warn but allow it
        std::cerr << "WARNING: Configuration key \"" << key << "\" is not in the known schema" << std::endl;
        return true;
    }
    
    const auto& schema = *schemaIt;
    
    // Check if deprecated
    if (schema.isDeprecated) {
        std::cerr << "WARNING: Configuration key \"" << key << "\" is deprecated" << std::endl;
    }
    
    // Validate type
    bool typeValid = false;
    if (schema.type == "int64_t" && std::holds_alternative<int64_t>(value)) {
        typeValid = true;
    } else if (schema.type == "double" && std::holds_alternative<double>(value)) {
        typeValid = true;
    } else if (schema.type == "float" && std::holds_alternative<double>(value)) {
        typeValid = true;
    } else if (schema.type == "bool" && std::holds_alternative<bool>(value)) {
        typeValid = true;
    } else if (schema.type == "string" && std::holds_alternative<std::string>(value)) {
        typeValid = true;
    } else if (schema.type == "int" && std::holds_alternative<int>(value)) {
        typeValid = true;
    }
    
    if (!typeValid) {
        std::cerr << "ERROR: Configuration key \"" << key << "\" has wrong type. Expected " 
                  << schema.type << ", got " << getValueTypeString(value) << std::endl;
        return false;
    }
    
    // Validate value ranges
    if (!schema.allowedValues.empty()) {
        std::string valueStr;
        if (std::holds_alternative<int>(value)) {
            valueStr = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            valueStr = std::to_string(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            valueStr = std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            valueStr = std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            valueStr = std::get<std::string>(value);
        }
        
        bool valueInAllowed = std::find(schema.allowedValues.begin(), schema.allowedValues.end(), valueStr) != schema.allowedValues.end();
        if (!valueInAllowed) {
            std::cerr << "ERROR: Configuration key \"" << key << "\" value \"" << valueStr 
                      << "\" is not in the allowed list" << std::endl;
            return false;
        }
    }
    
    if (!schema.numericRange.first && !schema.numericRange.second) {
        // Numeric range validation for numeric types
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            int64_t numVal;
            if (std::holds_alternative<int>(value)) {
                numVal = std::get<int>(value);
            } else {
                numVal = std::get<int64_t>(value);
            }
            
            if (schema.numericRange.first && numVal < schema.numericRange.first) {
                std::cerr << "ERROR: Configuration key \"" << key << "\" value " << numVal 
                          << " is too small. Minimum allowed value is " << schema.numericRange.first << std::endl;
                return false;
            }
            if (schema.numericRange.second && numVal > schema.numericRange.second) {
                std::cerr << "ERROR: Configuration key \"" << key << "\" value " << numVal 
                          << " is too large. Maximum allowed value is " << schema.numericRange.second << std::endl;
                return false;
            }
        } else if (std::holds_alternative<double>(value)) {
            double numVal = std::get<double>(value);
            if (schema.numericRange.first && numVal < schema.numericRange.first) {
                std::cerr << "ERROR: Configuration key \"" << key << "\" value " << numVal 
                          << " is too small. Minimum allowed value is " << schema.numericRange.first << std::endl;
                return false;
            }
            if (schema.numericRange.second && numVal > schema.numericRange.second) {
                std::cerr << "ERROR: Configuration key \"" << key << "\" value " << numVal 
                          << " is too large. Maximum allowed value is " << schema.numericRange.second << std::endl;
                return false;
            }
        }
    }
    
    return true;
}

std::string Config::getValueTypeString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
    return "unknown";
}

bool Config::validateAgainstSchema(const ConfigEntry& entry) const {
    auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
        [&entry](const ConfigSchema& s) { return s.key == entry.key; });
    
    if (schemaIt == pImpl->schema.end()) {
        return true; // Unknown key, allow it
    }
    
    const auto& schema = *schemaIt;
    
    // Check if required but missing
    if (schema.required && !has(entry.key)) {
        std::cerr << "ERROR: Required configuration key \"" << entry.key << "\" is missing" << std::endl;
        return false;
    }
    
    return true;
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    bool isValid = true;
    for (const auto& entry : pImpl->entries) {
        if (!validateAgainstSchema(entry)) {
            isValid = false;
        }
        
        if (!validateValue(entry.key, entry.value, entry.source)) {
            isValid = false;
        }
    }
    
    // Check for required keys
    for (const auto& schema : pImpl->schema) {
        if (schema.required && !has(schema.key)) {
            std::cerr << "ERROR: Required configuration key \"" << schema.key << "\" is missing" << std::endl;
            pImpl->validationErrors.push_back("Missing required key: " + schema.key);
            isValid = false;
        }
    }
    
    return isValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::setSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schema = schema;
}

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
    
    // Validate after loading
    if (!validate()) {
        std::cerr << "WARNING: Configuration file \"" << filepath << "\" has validation errors:" << std::endl;
        for (const auto& error : getValidationErrors()) {
            std::cerr << "  " << error << std::endl;
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
    
    // Validate after loading
    if (!validate()) {
        std::cerr << "WARNING: Command line arguments have validation errors:" << std::endl;
        for (const auto& error : getValidationErrors()) {
            std::cerr << "  " << error << std::endl;
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
        
        // Convert value to string based on type
        if (std::holds_alternative<int>(entry.value)) {
            file << entry.key << " = " << std::get<int>(entry.value) << "\n";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << entry.key << " = " << std::get<int64_t>(entry.value) << "\n";
        } else if (std::holds_alternative<double>(entry.value)) {
            file << entry.key << " = " << std::get<double>(entry.value) << "\n";
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << entry.key << " = " << (std::get<bool>(entry.value) ? "true" : "false") << "\n";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"\n";
        } else {
            file << entry.key << " = \"UNKNOWN_TYPE\"\n";
        }
        
        file << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    if (!validateValue(key, value, source)) {
        throw ConfigValidationError("Invalid configuration value for key: " + key);
    }
    
    // Check schema
    if (!validateAgainstSchema(ConfigEntry{key, value, source, ""})) {
        throw ConfigValidationError("Configuration validation failed for key: " + key);
    }
    
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

// Type-safe set methods
void Config::setRandomSeed(int64_t seed) {
    set(ConfigKeys::RANDOM_SEED, seed, ConfigSource::Runtime);
}

void Config::setSimulationTimestep(double timestep) {
    set(ConfigKeys::SIMULATION_TIMESTEP, timestep, ConfigSource::Runtime);
}

void Config::setNeuronCount(int64_t count) {
    set(ConfigKeys::NEURON_COUNT, count, ConfigSource::Runtime);
}

void Config::setRegionCount(int64_t count) {
    set(ConfigKeys::REGION_COUNT, count, ConfigSource::Runtime);
}

void Config::setConnectionProbability(float prob) {
    set(ConfigKeys::CONNECTION_PROBABILITY, prob, ConfigSource::Runtime);
}

void Config::setStdpParameters(float ltpWeight, float ltdWeight, float tau) {
    set(ConfigKeys::STDP_LTP_WEIGHT, ltpWeight, ConfigSource::Runtime);
    set(ConfigKeys::STDP_LTD_WEIGHT, ltdWeight, ConfigSource::Runtime);
    set(ConfigKeys::STDP_TAU, tau, ConfigSource::Runtime);
}

void Config::setSynaptogenesisRate(float rate) {
    set(ConfigKeys::SYNAPTOGENESIS_RATE, rate, ConfigSource::Runtime);
}

void Config::setPruningRate(float rate) {
    set(ConfigKeys::PRUNING_RATE, rate, ConfigSource::Runtime);
}

void Config::setDopamineBaseline(double baseline) {
    set(ConfigKeys::DOPAMINE_BASELINE, baseline, ConfigSource::Runtime);
}

void Config::setRewardDiscountFactor(double factor) {
    set(ConfigKeys::REWARD_DISCOUNT_FACTOR, factor, ConfigSource::Runtime);
}

void Config::setEnvironmentParameters(const std::string& name, int64_t width, int64_t height) {
    set(ConfigKeys::ENVIRONMENT_NAME, name, ConfigSource::Runtime);
    set(ConfigKeys::ENVIRONMENT_WIDTH, width, ConfigSource::Runtime);
    set(ConfigKeys::ENVIRONMENT_HEIGHT, height, ConfigSource::Runtime);
}

void Config::setLogLevel(const std::string& level) {
    set(ConfigKeys::LOG_LEVEL, level, ConfigSource::Runtime);
}

void Config::setLogToFile(bool enabled) {
    set(ConfigKeys::LOG_TO_FILE, enabled, ConfigSource::Runtime);
}

void Config::setLogFilename(const std::string& filename) {
    set(ConfigKeys::LOG_FILENAME, filename, ConfigSource::Runtime);
}

void Config::setMaxSimulationSteps(size_t steps) {
    set(ConfigKeys::MAX_SIMULATION_STEPS, static_cast<int64_t>(steps), ConfigSource::Runtime);
}

void Config::setSimulationTimeLimit(double limit) {
    set(ConfigKeys::SIMULATION_TIME_LIMIT, limit, ConfigSource::Runtime);
}

void Config::setVisualizationEnabled(bool enabled) {
    set(ConfigKeys::VISUALIZATION_ENABLED, enabled, ConfigSource::Runtime);
}

void Config::setVisualizationUpdateRate(double rate) {
    set(ConfigKeys::VISUALIZATION_UPDATE_RATE, rate, ConfigSource::Runtime);
}

void Config::setReplayInterval(size_t interval) {
    set(ConfigKeys::REPLAY_INTERVAL, static_cast<int64_t>(interval), ConfigSource::Runtime);
}

void Config::setConsolidationInterval(size_t interval) {
    set(ConfigKeys::CONSOLIDATION_INTERVAL, static_cast<int64_t>(interval), ConfigSource::Runtime);
}

void Config::setCheckpointDir(const std::string& dir) {
    set(ConfigKeys::CHECKPOINT_DIR, dir, ConfigSource::Runtime);
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm