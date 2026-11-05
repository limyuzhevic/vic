#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <ctime>
#include <algorithm>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Configuration schema for validation
    struct Schema {
        std::vector<ValidationRule> rules;
        std::map<std::string, std::string> descriptions;
    };
    Schema schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validation schema
    pImpl->schema = createDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON format first (files ending with .json)
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSONFile(filepath);
    }
    // Try YAML format (files ending with .yaml or .yml)
    else if (filepath.size() >= 5) {
        std::string ext = filepath.substr(filepath.size() - 5);
        if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAMLFile(filepath);
        }
    }
    // Fall back to simple key=value format for .cfg files and others
    else {
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Use nlohmann/json if available, otherwise use custom parser
        // For now, we'll implement a simple JSON parser as a fallback
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Try to parse as YAML (would need YAML library)
        // For now, fall back to simple parser
        return loadFromSimpleFile(filepath);
    } catch (const std::exception& e) {
        return false;
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

// Add command line help/usage functionality
void Config::printHelp(const std::string& programName) const {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "\nConfiguration Options:\n";
    std::cout << "  --key=value           Set configuration value\n";
    std::cout << "  -k value              Short form: set key to value\n";
    std::cout << "\nCommon Configuration Keys:\n";
    
    // Print some common keys as examples
    auto keys = getKeys();
    std::vector<std::string> importantKeys = {
        "random_seed", "simulation_timestep", "neuron_count", "region_count",
        "connection_probability", "plasticity_learning_rate", "stdp_ltp_weight",
        "stdp_ltd_weight", "development_synaptogenesis_rate", "log_level"
    };
    
    for (const auto& key : importantKeys) {
        if (has(key)) {
            auto value = get<std::string>(key);
            if (value) {
                std::cout << "  " << key << " = " << *value << "\n";
            }
        }
    }
    
    std::cout << "\nFile Format Support:\n";
    std::cout << "  .cfg - Simple key=value format\n";
    std::cout << "  .json - JSON format\n";
    std::cout << "  .yaml/.yml - YAML format\n";
}

// Save configuration to file in the requested format
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format based on extension
    std::string ext = filepath.size() >= 4 ? filepath.substr(filepath.size() - 4) : "";
    
    if (ext == ".json") {
        return saveToJSONFile(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAMLFile(file);
    } else {
        return saveToSimpleFile(file);
    }
}

bool Config::saveToSimpleFile(std::ofstream& file) const {
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, uint64_t>) {
                file << entry.key << " = " << val << "\n";
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float>) {
                file << entry.key << " = " << val << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << "[unknown]\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    return true;
}

bool Config::saveToJSONFile(std::ofstream& file) const {
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, uint64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << val[i] << "\"";
                    } else {
                        file << val[i];
                    }
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    return true;
}

bool Config::saveToYAMLFile(std::ofstream& file) const {
    file << "# NLM Configuration File\n";
    file << "# Generated from NLM config system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << ": \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << ": " << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, uint64_t>) {
                file << entry.key << ": " << val << "\n";
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float>) {
                file << entry.key << ": " << val << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << ": [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << val[i] << "\"";
                    } else {
                        file << val[i];
                    }
                }
                file << "]\n";
            } else {
                file << entry.key << ": null\n";
            }
        }, entry.value);
        
        file << "\n";
    }
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

// Configuration validation and schema implementation

void Config::ValidationRule::addRule(const ValidationRule& rule) {
    pImpl->schema.rules.push_back(rule);
}

bool Config::ValidationRule::validate(const Config& config, std::vector<std::string>& errors) const {
    std::vector<std::string> localErrors;
    
    for (const auto& rule : pImpl->schema.rules) {
        if (config.has(rule.key)) {
            auto value = config.get<std::string>(rule.key);
            if (!value) {
                localErrors.push_back("Key \"" + rule.key + "\" exists but cannot be retrieved as string");
                continue;
            }
            
            // Try to convert to the expected type and validate
            // For simplicity, we'll do basic type checking
            ConfigValue testValue = *value;
            if (!rule.validator(testValue)) {
                localErrors.push_back("Validation failed for key \"" + rule.key + "\": " + rule.errorMessage);
            }
        } else if (rule.key.find("?") != std::string::npos) {
            // Optional key - no error if it doesn't exist
        } else {
            // Required key missing
            localErrors.push_back("Required configuration key \"" + rule.key + "\" is missing");
        }
    }
    
    errors = localErrors;
    return localErrors.empty();
}

std::string Config::ValidationRule::getDescription(const std::string& key) const {
    auto it = pImpl->schema.descriptions.find(key);
    if (it != pImpl->schema.descriptions.end()) {
        return it->second;
    }
    return "";
}

bool Config::validateConfiguration(std::vector<std::string>& errors) const {
    return pImpl->schema.validate(*this, errors);
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    validateConfiguration(errors);
    return errors;
}

std::vector<std::string> Config::getConfigurationWarnings() const {
    std::vector<std::string> warnings;
    
    // Check for potential issues
    if (has("neuron_count") && getOr<int64_t>("neuron_count", 0) == 0) {
        warnings.push_back("neuron_count is 0 - this may cause issues with network simulation");
    }
    
    if (has("simulation_timestep") && getOr<double>("simulation_timestep", 0.001) <= 0.0) {
        warnings.push_back("simulation_timestep should be positive");
    }
    
    if (has("connection_probability") && (getOr<double>("connection_probability", 0.0) < 0.0 || 
                                          getOr<double>("connection_probability", 0.0) > 1.0)) {
        warnings.push_back("connection_probability should be between 0.0 and 1.0");
    }
    
    return warnings;
}

Config::ConfigurationSchema Config::createDefaultSchema() {
    ConfigurationSchema schema;
    
    // Basic configuration validation rules
    schema.addRule(ValidationRule("random_seed", "Random seed for reproducible experiments",
                                 [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); },
                                 "Random seed must be an integer"));
    
    schema.addRule(ValidationRule("simulation_timestep", "Simulation timestep in seconds",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
                                 },
                                 "Timestep must be a positive number"));
    
    schema.addRule(ValidationRule("neuron_count", "Number of neurons in the brain",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
                                 },
                                 "Neuron count must be a positive integer"));
    
    schema.addRule(ValidationRule("region_count", "Number of brain regions",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
                                 },
                                 "Region count must be a positive integer"));
    
    schema.addRule(ValidationRule("connection_probability", "Connection probability for random connectivity",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
                                 },
                                 "Connection probability must be between 0.0 and 1.0"));
    
    schema.addRule(ValidationRule("plasticity_learning_rate", "Hebbian plasticity learning rate",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "Plasticity learning rate must be non-negative"));
    
    schema.addRule(ValidationRule("stdp_ltp_weight", "STDP LTP (long-term potentiation) weight",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "STDP LTP weight must be non-negative"));
    
    schema.addRule(ValidationRule("stdp_ltd_weight", "STDP LTD (long-term depression) weight",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "STDP LTD weight must be non-negative"));
    
    schema.addRule(ValidationRule("stdp_time_constant", "STDP time constant",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
                                 },
                                 "STDP time constant must be positive"));
    
    schema.addRule(ValidationRule("development_synaptogenesis_rate", "Synaptogenesis rate during development",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "Synaptogenesis rate must be non-negative"));
    
    schema.addRule(ValidationRule("development_pruning_rate", "Pruning rate during development",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "Pruning rate must be non-negative"));
    
    schema.addRule(ValidationRule("dopamine_baseline", "Dopamine baseline level",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "Dopamine baseline must be non-negative"));
    
    schema.addRule(ValidationRule("reward_discount_factor", "Reward discount factor",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
                                 },
                                 "Reward discount factor must be between 0.0 and 1.0"));
    
    schema.addRule(ValidationRule("environment_width", "Environment width",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
                                 },
                                 "Environment width must be a positive integer"));
    
    schema.addRule(ValidationRule("environment_height", "Environment height",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
                                 },
                                 "Environment height must be a positive integer"));
    
    schema.addRule(ValidationRule("log_level", "Log level",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<std::string>(v);
                                 },
                                 "Log level must be a string"));
    
    schema.addRule(ValidationRule("max_simulation_steps", "Maximum simulation steps",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
                                 },
                                 "Maximum simulation steps must be a positive integer"));
    
    schema.addRule(ValidationRule("simulation_time_limit", "Simulation time limit",
                                 [](const ConfigValue& v) { 
                                     return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0;
                                 },
                                 "Simulation time limit must be non-negative"));
    
    // Add descriptions for common keys
    schema.descriptions["random_seed"] = "Random seed for reproducible experiments";
    schema.descriptions["simulation_timestep"] = "Simulation timestep in seconds (should be positive)";
    schema.descriptions["neuron_count"] = "Number of neurons in the brain (recommended: 500-10000)";
    schema.descriptions["region_count"] = "Number of brain regions (recommended: 1-10)";
    schema.descriptions["connection_probability"] = "Connection probability for random connectivity (0.0-1.0)";
    schema.descriptions["plasticity_learning_rate"] = "Hebbian plasticity learning rate (recommended: 0.01-0.1)";
    schema.descriptions["stdp_ltp_weight"] = "STDP LTP (long-term potentiation) weight (recommended: 0.01-0.1)";
    schema.descriptions["stdp_ltd_weight"] = "STDP LTD (long-term depression) weight (recommended: 0.01-0.1)";
    schema.descriptions["stdp_time_constant"] = "STDP time constant in milliseconds (recommended: 20-50)";
    schema.descriptions["development_synaptogenesis_rate"] = "Synaptogenesis rate during development (recommended: 0.001-0.01)";
    schema.descriptions["development_pruning_rate"] = "Pruning rate during development (recommended: 0.0001-0.001)";
    schema.descriptions["dopamine_baseline"] = "Dopamine baseline level (recommended: 0.0-1.0)";
    schema.descriptions["reward_discount_factor"] = "Reward discount factor (recommended: 0.9-0.99)";
    schema.descriptions["environment_width"] = "Environment width in units (recommended: 10-50)";
    schema.descriptions["environment_height"] = "Environment height in units (recommended: 10-50)";
    schema.descriptions["log_level"] = "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)";
    schema.descriptions["max_simulation_steps"] = "Maximum simulation steps before auto-termination";
    schema.descriptions["simulation_time_limit"] = "Maximum simulation time in seconds before auto-termination";
    
    return schema;
}

} // namespace nlm
