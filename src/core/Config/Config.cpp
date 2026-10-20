#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

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
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Remove surrounding quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value as different types
            bool parsed = false;
            
            // Try boolean
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
                parsed = true;
            }
            // Try integer
            else {
                try {
                    size_t posEnd;
                    long intValue = std::stol(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intValue, ConfigSource::File);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double if not parsed yet
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleValue = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, doubleValue, ConfigSource::File);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Fall back to string if not parsed as other types
            if (!parsed) {
                set(key, value, ConfigSource::File);
            }
            
        } else {
            // Log warning about malformed line
            NLM_LOG_WARNING("Malformed config line " + std::to_string(lineNumber) + ": '" + line + "'");
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
        
        // Write the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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

} // namespace nlm

// Configuration validation implementation
namespace nlm {

namespace ConfigValidation {
    
    // Valid configuration keys
    const std::set<std::string> VALID_KEYS = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate",
        "replay_interval",
        "consolidation_interval",
        "brain.neuron_count",
        "brain.synapse_density",
        "brain.connection_probability",
        "brain.initial_weight_mean",
        "brain.initial_weight_std",
        "brain.v_thresh",
        "brain.v_rest",
        "brain.v_reset",
        "brain.tau_mem",
        "brain.tau_ref",
        "plasticity.stdp.enable",
        "plasticity.stdp.learning_rate",
        "plasticity.stdp.tau_plus",
        "plasticity.stdp.tau_minus",
        "plasticity.hebbian.enable",
        "plasticity.structural.enable",
        "neuromod.dopamine.scale",
        "neuromod.curiosity.enable",
        "neuromod.novelty.enable"
    };
    
    // Configuration parameter validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    // Create validation rules
    std::vector<ValidationRule> createValidationRules() {
        std::vector<ValidationRule> rules;
        
        // random_seed validation
        rules.push_back({
            "random_seed",
            [](const ConfigValue& val) {
                auto* p = std::get_if<int64_t>(&val);
                return p && *p >= 0;
            },
            "random_seed must be a non-negative integer"
        });
        
        // simulation_timestep validation
        rules.push_back({
            "simulation_timestep",
            [](const ConfigValue& val) {
                auto* p = std::get_if<double>(&val);
                return p && *p > 0.0 && *p < 1.0;
            },
            "simulation_timestep must be a positive number less than 1.0"
        });
        
        // neuron_count validation
        rules.push_back({
            "neuron_count",
            [](const ConfigValue& val) {
                auto* p = std::get_if<size_t>(&val);
                return p && *p > 0 && *p <= 100000;
            },
            "neuron_count must be a positive number not exceeding 100000"
        });
        
        // region_count validation
        rules.push_back({
            "region_count",
            [](const ConfigValue& val) {
                auto* p = std::get_if<size_t>(&val);
                return p && *p > 0 && *p <= 1000;
            },
            "region_count must be a positive number not exceeding 1000"
        });
        
        // connection_probability validation
        rules.push_back({
            "connection_probability",
            [](const ConfigValue& val) {
                auto* p = std::get_if<float>(&val);
                return p && *p >= 0.0f && *p <= 1.0f;
            },
            "connection_probability must be between 0.0 and 1.0"
        });
        
        // Learning rate validation
        rules.push_back({
            "plasticity_learning_rate",
            [](const ConfigValue& val) {
                auto* p = std::get_if<float>(&val);
                return p && *p >= 0.0f && *p <= 1.0f;
            },
            "plasticity_learning_rate must be between 0.0 and 1.0"
        });
        
        // Boolean parameter validation
        rules.push_back({
            "log_to_file",
            [](const ConfigValue& val) {
                auto* p = std::get_if<bool>(&val);
                return p;
            },
            "log_to_file must be a boolean value"
        });
        
        return rules;
    }
    
    // Validate a configuration value
    ValidationResult validate(const std::string& key, const ConfigValue& value) {
        ValidationResult result;
        
        // Check if key is valid
        if (VALID_KEYS.find(key) == VALID_KEYS.end()) {
            result.isValid = false;
            result.errorMessage = "Unknown configuration key: '" + key + "'";
            result.suggestedAlternatives = suggestAlternatives(key);
            return result;
        }
        
        // Apply specific validation rules
        auto rules = createValidationRules();
        for (const auto& rule : rules) {
            if (rule.key == key) {
                if (!rule.validator(value)) {
                    result.isValid = false;
                    result.errorMessage = rule.errorMessage;
                }
                break;
            }
        }
        
        // If no rules applied, check basic type consistency
        if (result.isValid) {
            result.isValid = checkTypeConsistency(key, value);
            if (!result.isValid) {
                result.errorMessage = "Invalid value type for configuration key: '" + key + "'";
            }
        }
        
        return result;
    }
    
    // Suggest alternatives for unknown keys
    std::vector<std::string> suggestAlternatives(const std::string& key) {
        std::vector<std::string> suggestions;
        
        // Simple fuzzy matching for suggestions
        for (const auto& validKey : VALID_KEYS) {
            // Check for prefix match
            if (validKey.find(key) != std::string::npos || key.find(validKey) != std::string::npos) {
                suggestions.push_back(validKey);
                if (suggestions.size() >= 3) break; // Limit suggestions
            }
        }
        
        return suggestions;
    }
    
    // Check type consistency for a key
    bool checkTypeConsistency(const std::string& key, const ConfigValue& value) {
        // This is a simplified check - in a real implementation, we might have
        // more sophisticated type checking based on key patterns
        
        // Keys ending with "_rate" should be float
        if (key.find("_rate") != std::string::npos) {
            return std::holds_alternative<float>(value) || std::holds_alternative<double>(value);
        }
        
        // Keys containing "probability" should be float
        if (key.find("probability") != std::string::npos) {
            return std::holds_alternative<float>(value) || std::holds_alternative<double>(value);
        }
        
        // Keys with "count" or "number" should be integer
        if (key.find("count") != std::string::npos || key.find("number") != std::string::npos) {
            return std::holds_alternative<size_t>(value) || std::holds_alternative<int>(value) ||
                   std::holds_alternative<int64_t>(value);
        }
        
        // Keys with "enabled", "enable", "active" should be boolean
        if (key.find("enabled") != std::string::npos || key.find("enable") != std::string::npos ||
            key.find("active") != std::string::npos) {
            return std::holds_alternative<bool>(value);
        }
        
        return true; // Pass through for unknown patterns
    }
    
    // Get validation rules for documentation
    std::string getValidationDocumentation() {
        std::ostringstream oss;
        oss << "Configuration Validation Rules:\n";
        oss << "===============================\n\n";
        
        auto rules = createValidationRules();
        for (const auto& rule : rules) {
            oss << "Key: " << rule.key << "\n";
            oss << "  Error: " << rule.errorMessage << "\n\n";
        }
        
        oss << "Valid Keys:\n";
        for (const auto& key : VALID_KEYS) {
            oss << "  - " << key << "\n";
        }
        
        return oss.str();
    }
} // namespace ConfigValidation

bool Config::validate(const std::string& key, const std::string& value, std::string& errorMessage) {
    ConfigValue configValue(value);
    auto result = ConfigValidation::validate(key, configValue);
    
    if (!result.isValid) {
        errorMessage = result.errorMessage;
        if (!result.suggestedAlternatives.empty()) {
            errorMessage += ". Did you mean: ";
            for (size_t i = 0; i < result.suggestedAlternatives.size(); ++i) {
                if (i > 0) errorMessage += ", ";
                errorMessage += "'" + result.suggestedAlternatives[i] + "'";
            }
        }
        return false;
    }
    
    return true;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        auto result = ConfigValidation::validate(entry.key, entry.value);
        if (!result.isValid) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + entry.key + "': " + result.errorMessage);
            return false;
        }
    }
    return true;
}

std::string Config::getValidationDocumentation() const {
    return ConfigValidation::getValidationDocumentation();
}

std::vector<std::string> Config::getValidKeys() const {
    return std::vector<std::string>(ConfigValidation::VALID_KEYS.begin(), ConfigValidation::VALID_KEYS.end());
}

std::string Config::getKeyDocumentation(const std::string& key) const {
    // Return documentation for specific configuration keys
    std::map<std::string, std::string> keyDocs = {
        {"random_seed", "Random seed for reproducible experiments (non-negative integer)"},
        {"simulation_timestep", "Simulation timestep in seconds (positive, < 1.0)"},
        {"neuron_count", "Total number of neurons in the brain (positive, <= 100000)"},
        {"region_count", "Number of brain regions (positive, <= 1000)"},
        {"connection_probability", "Probability of connection between neurons (0.0-1.0)"},
        {"plasticity_learning_rate", "General plasticity learning rate (0.0-1.0)"},
        {"stdp_ltp_weight", "STDP Long-Term Potentiation weight"},
        {"stdp_ltd_weight", "STDP Long-Term Depression weight"},
        {"stdp_time_constant", "STDP time constant in milliseconds"},
        {"development_synaptogenesis_rate", "Rate of new synapse formation"},
        {"development_pruning_rate", "Rate of synapse elimination"},
        {"dopamine_baseline", "Baseline dopamine level"},
        {"reward_discount_factor", "Reward discount factor for temporal difference learning"},
        {"environment_name", "Name of the simulated environment"},
        {"environment_width", "Width of the environment (grid units)"},
        {"environment_height", "Height of the environment (grid units)"},
        {"log_level", "Logging level (Debug, Info, Warning, Error, Critical)"},
        {"log_to_file", "Whether to log to file instead of console"},
        {"log_filename", "Filename for log output"},
        {"max_simulation_steps", "Maximum number of simulation steps"},
        {"simulation_time_limit", "Maximum simulation time in seconds"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate in Hz"}
    };
    
    auto it = keyDocs.find(key);
    if (it != keyDocs.end()) {
        return it->second;
    }
    
    return "Configuration parameter: " + key;
}

Config::ValidationResult Config::getValidationResult(const std::string& key) const {
    ValidationResult result;
    result.isValid = true; // Assume valid if not found
    
    // Check if key exists in configuration
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Key exists, validate it
        result = ConfigValidation::validate(key, it->value);
        if (result.isValid) {
            result.errorMessage = "Configuration key '" + key + "' is valid";
        }
    } else {
        result.isValid = false;
        result.errorMessage = "Configuration key '" + key + "' not found";
    }
    
    return result;
}

bool Config::remove(const std::string& key) {
    auto originalSize = pImpl->entries.size();
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    return pImpl->entries.size() < originalSize;
}

std::string Config::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, size_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format as JSON number with appropriate precision
                std::ostringstream numOss;
                numOss << std::fixed << std::setprecision(6) << arg;
                std::string numStr = numOss.str();
                // Remove trailing zeros and decimal point if integer
                if (numStr.find('.') != std::string::npos) {
                    numStr.erase(numStr.find_last_not_of('0') + 1, std::string::npos);
                    if (numStr.back() == '.') {
                        numStr.pop_back();
                    }
                }
                oss << numStr;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

bool Config::fromJson(const std::string& jsonStr) {
    // Simple JSON parsing for basic structures
    // In a real implementation, we would use a proper JSON parser
    
    size_t pos = 0;
    bool inObject = false;
    bool inString = false;
    std::string currentKey;
    std::string currentValue;
    bool expectingValue = false;
    
    // Clear existing entries
    clear();
    
    for (char c : jsonStr) {
        switch (c) {
            case '{':
                if (!inObject) inObject = true;
                break;
            case '}':
                if (inObject) {
                    // Process key-value pair
                    if (!currentKey.empty() && !currentValue.empty()) {
                        set(currentKey, currentValue, ConfigSource::File);
                    }
                    currentKey.clear();
                    currentValue.clear();
                    expectingValue = false;
                }
                inObject = false;
                break;
            case '"':
                if (!expectingValue) {
                    currentKey += c;
                } else {
                    currentValue += c;
                }
                inString = !inString;
                break;
            case ':':
                if (!expectingValue) expectingValue = true;
                break;
            case ',':
                if (inObject && !inString) {
                    if (!currentKey.empty() && !currentValue.empty()) {
                        set(currentKey, currentValue, ConfigSource::File);
                    }
                    currentKey.clear();
                    currentValue.clear();
                    expectingValue = false;
                }
                break;
            default:
                if (inString) {
                    if (expectingValue) {
                        currentValue += c;
                    } else {
                        currentKey += c;
                    }
                }
                break;
        }
    }
    
    return true;
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

Config::~Config() = default;

} // namespace nlm

