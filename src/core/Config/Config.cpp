#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    registerDefaultSchema();
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
            
            // Determine expected type from schema
            ConfigType expectedType = ConfigType::String;
            if (defaultSchema.find(key) != defaultSchema.end()) {
                expectedType = defaultSchema.at(key).type;
            }
            
            set(key, parseValue(value, expectedType), ConfigSource::File);
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
                
                // Determine expected type from schema
                ConfigType expectedType = ConfigType::String;
                if (defaultSchema.find(key) != defaultSchema.end()) {
                    expectedType = defaultSchema.at(key).type;
                }
                
                set(key, parseValue(value, expectedType), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Determine expected type from schema
            ConfigType expectedType = ConfigType::String;
            if (defaultSchema.find(key) != defaultSchema.end()) {
                expectedType = defaultSchema.at(key).type;
            }
            
            set(key, parseValue(value, expectedType), ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromDefaults() {
    for (const auto& pair : defaultSchema) {
        const auto& entry = pair.second;
        set(entry.key, entry.value, entry.source);
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
        file << entry.key << " = " << toString(entry.value) << "\n";
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Check if key is in schema
        if (defaultSchema.find(key) != defaultSchema.end()) {
            const auto& schemaEntry = defaultSchema.at(key);
            pImpl->entries.emplace_back(key, value, source, schemaEntry.description, schemaEntry.type, schemaEntry.range);
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    }
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
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            result.addError("Invalid configuration key: " + entry.key);
        }
        
        if (!validateValue(entry.key, entry.value)) {
            result.addError("Invalid value for key '" + entry.key + "': " + toString(entry.value));
        }
    }
    
    return result;
}

bool Config::validateKey(const std::string& key) const {
    return defaultSchema.find(key) != defaultSchema.end();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    if (defaultSchema.find(key) == defaultSchema.end()) {
        return true; // No validation for custom keys
    }
    
    const auto& schemaEntry = defaultSchema.at(key);
    double numericValue = toDouble(value);
    return schemaEntry.range.isValid(numericValue);
}

std::string Config::getValidationErrors() const {
    ValidationResult result = validate();
    return result.getErrors();
}

void Config::registerDefaultSchema() {
    // Default schema entries
    defaultSchema = {
        {"random_seed", ConfigEntry("random_seed", 42, ConfigSource::Default, 
                                    "Random seed for reproducible experiments", ConfigType::Int, 
                                    ConfigRange(ConfigType::Int, 0, std::numeric_limits<int>::max(), "Non-negative integer seed"))},
        {"simulation_timestep", ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default,
                                            "Simulation timestep in seconds", ConfigType::Double,
                                            ConfigRange(ConfigType::Double, 0.000001, 1.0, "Must be positive and small"))},
        {"neuron_count", ConfigEntry("neuron_count", 1000, ConfigSource::Default,
                                     "Number of neurons in the brain", ConfigType::Int,
                                     ConfigRange(ConfigType::Int, 1, 1000000, "Must be between 1 and 1,000,000"))},
        {"region_count", ConfigEntry("region_count", 2, ConfigSource::Default,
                                     "Number of brain regions", ConfigType::Int,
                                     ConfigRange(ConfigType::Int, 1, 100, "Must be between 1 and 100"))},
        {"connection_probability", ConfigEntry("connection_probability", 0.1, ConfigSource::Default,
                                                "Connection probability for random connectivity", ConfigType::Double,
                                                ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"plasticity_learning_rate", ConfigEntry("plasticity_learning_rate", 0.01, ConfigSource::Default,
                                                  "Plasticity learning rate", ConfigType::Double,
                                                  ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"stdp_ltp_weight", ConfigEntry("stdp_ltp_weight", 0.01, ConfigSource::Default,
                                        "STDP LTP weight", ConfigType::Double,
                                        ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"stdp_ltd_weight", ConfigEntry("stdp_ltd_weight", 0.012, ConfigSource::Default,
                                        "STDP LTD weight", ConfigType::Double,
                                        ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"stdp_time_constant", ConfigEntry("stdp_time_constant", 20.0, ConfigSource::Default,
                                           "STDP time constant", ConfigType::Double,
                                           ConfigRange(ConfigType::Double, 1.0, 1000.0, "Must be between 1.0 and 1000.0"))},
        {"development_synaptogenesis_rate", ConfigEntry("development_synaptogenesis_rate", 0.001, ConfigSource::Default,
                                                         "Development synaptogenesis rate", ConfigType::Double,
                                                         ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"development_pruning_rate", ConfigEntry("development_pruning_rate", 0.0001, ConfigSource::Default,
                                                 "Development pruning rate", ConfigType::Double,
                                                 ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"dopamine_baseline", ConfigEntry("dopamine_baseline", 0.1, ConfigSource::Default,
                                           "Dopamine baseline level", ConfigType::Double,
                                           ConfigRange(ConfigType::Double, 0.0, 10.0, "Must be between 0.0 and 10.0"))},
        {"reward_discount_factor", ConfigEntry("reward_discount_factor", 0.99, ConfigSource::Default,
                                                "Reward discount factor", ConfigType::Double,
                                                ConfigRange(ConfigType::Double, 0.0, 1.0, "Must be between 0.0 and 1.0"))},
        {"environment_name", ConfigEntry("environment_name", std::string("GridWorld"), ConfigSource::Default,
                                         "Environment name", ConfigType::String,
                                         ConfigRange(ConfigType::String, {"GridWorld", "OpenWorld", "CustomWorld"}, "Must be one of: GridWorld, OpenWorld, CustomWorld"))},
        {"environment_width", ConfigEntry("environment_width", 10, ConfigSource::Default,
                                          "Environment width", ConfigType::Int,
                                          ConfigRange(ConfigType::Int, 1, 1000, "Must be between 1 and 1000"))},
        {"environment_height", ConfigEntry("environment_height", 10, ConfigSource::Default,
                                           "Environment height", ConfigType::Int,
                                           ConfigRange(ConfigType::Int, 1, 1000, "Must be between 1 and 1000"))},
        {"log_level", ConfigEntry("log_level", std::string("INFO"), ConfigSource::Default,
                                   "Log level", ConfigType::String,
                                   ConfigRange(ConfigType::String, {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"}, "Must be one of: DEBUG, INFO, WARNING, ERROR, CRITICAL"))},
        {"log_to_file", ConfigEntry("log_to_file", false, ConfigSource::Default,
                                    "Whether to log to file", ConfigType::Bool, ConfigRange())},
        {"log_filename", ConfigEntry("log_filename", std::string("nlm.log"), ConfigSource::Default,
                                      "Log filename", ConfigType::String, ConfigRange())},
        {"max_simulation_steps", ConfigEntry("max_simulation_steps", 10000, ConfigSource::Default,
                                              "Maximum simulation steps", ConfigType::Int64,
                                              ConfigRange(ConfigType::Int64, 1, std::numeric_limits<int64_t>::max(), "Must be positive"))},
        {"simulation_time_limit", ConfigEntry("simulation_time_limit", 0.0, ConfigSource::Default,
                                              "Simulation time limit in seconds", ConfigType::Double,
                                              ConfigRange(ConfigType::Double, 0.0, std::numeric_limits<double>::max(), "Must be non-negative"))},
        {"visualization_enabled", ConfigEntry("visualization_enabled", false, ConfigSource::Default,
                                              "Whether visualization is enabled", ConfigType::Bool, ConfigRange())},
        {"visualization_update_rate", ConfigEntry("visualization_update_rate", 30.0, ConfigSource::Default,
                                                 "Visualization update rate in Hz", ConfigType::Double,
                                                 ConfigRange(ConfigType::Double, 1.0, 1000.0, "Must be between 1.0 and 1000.0"))},
    };
}

ConfigEntry Config::createEntry(const std::string& key, const ConfigValue& defaultValue, 
                                 ConfigSource source, const std::string& description,
                                 ConfigType type, const ConfigRange& range) {
    return ConfigEntry(key, defaultValue, source, description, type, range);
}

std::unordered_map<std::string, ConfigEntry> Config::getDefaultSchema() {
    return defaultSchema;
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
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

ConfigValue Config::parseValue(const std::string& str, ConfigType expectedType) {
    std::string trimmed = trim(str);
    
    // Check for boolean values
    if (expectedType == ConfigType::Bool) {
        if (toLower(trimmed) == "true") return true;
        if (toLower(trimmed) == "false") return false;
        if (trimmed == "1") return true;
        if (trimmed == "0") return false;
    }
    
    // Try to parse as double first
    try {
        size_t pos;
        double doubleValue = std::stod(trimmed, &pos);
        if (pos == trimmed.length()) {
            // Successfully parsed as double
            if (expectedType == ConfigType::Double || expectedType == ConfigType::Int || 
                expectedType == ConfigType::Int64) {
                return doubleValue;
            }
        }
    } catch (...) {}
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intValue = std::stoll(trimmed, &pos);
        if (pos == trimmed.length()) {
            // Successfully parsed as integer
            if (expectedType == ConfigType::Int) {
                return intValue;
            } else if (expectedType == ConfigType::Int64) {
                return intValue;
            }
        }
    } catch (...) {}
    
    // Return as string for any other type
    return trimmed;
}

double Config::toDouble(const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return static_cast<double>(arg);
        }
    }, value);
    return 0.0; // Default case
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Schema storage
std::unordered_map<std::string, ConfigEntry> Config::defaultSchema;

} // namespace nlm
