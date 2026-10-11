#include "ConfigValidator.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>

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
        // Write the actual value with proper formatting
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings that contain spaces or special characters
                if (arg.find(' ') != std::string::npos || 
                    arg.find('=') != std::string::npos ||
                    arg.find('"') != std::string::npos) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
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

// Add configuration validation methods
bool Config::validate(const ConfigValidator& validator) const {
    return validator.validate(*this).empty();
}

std::vector<std::string> Config::validate(const ConfigValidator& validator) const {
    return validator.validate(*this);
}

bool Config::validate() const {
    ConfigValidator validator;
    addDefaultValidationRules(validator);
    return validator.validate(*this).empty();
}

std::vector<std::string> Config::validate() const {
    ConfigValidator validator;
    addDefaultValidationRules(validator);
    return validator.validate(*this);
}

bool Config::validateWithFile(const std::string& schemaFile) {
    ConfigValidator validator;
    
    // Load schema from file
    if (!validator.loadSchema(schemaFile)) {
        throw std::runtime_error("Failed to load validation schema from: " + schemaFile);
    }
    
    // Validate current config against schema
    auto errors = validator.validate(*this);
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed with " << errors.size() << " error(s):\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
        throw std::runtime_error(oss.str());
    }
    
    return true;
}

void Config::addDefaultValidationRules(ConfigValidator& validator) {
    // Add common validation rules for typical NLM configuration
    
    // Validate neuron count
    validator.addRule("brain.neuron_count",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg > 0 && arg <= 100000;  // Reasonable limits
                }
                return false;
            }, value);
        }, "Neuron count must be between 1 and 100000");
    
    // Validate connection probability
    validator.addRule("brain.connection_probability",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= 0.0 && arg <= 1.0;
                }
                return false;
            }, value);
        }, "Connection probability must be between 0.0 and 1.0");
    
    // Validate simulation timestep
    validator.addRule("simulation_timestep",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg > 0.0 && arg <= 0.1;  // Should be small for stability
                }
                return false;
            }, value);
        }, "Simulation timestep must be positive and reasonable (<= 0.1s)");
    
    // Validate plasticity parameters
    validator.addRule("plasticity.stdp.learning_rate",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= 0.0 && arg <= 1.0;
                }
                return false;
            }, value);
        }, "STDP learning rate must be between 0.0 and 1.0");
    
    // Validate neuromodulation scale factors
    validator.addRule("neuromod.dopamine.scale",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= 0.0 && arg <= 10.0;
                }
                return false;
            }, value);
        }, "Dopamine scale factor must be between 0.0 and 10.0");
    
    // Validate developmental stage bounds
    validator.addRule("development.initial_plasticity_rate",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= 0.0 && arg <= 2.0;
                }
                return false;
            }, value);
        }, "Initial plasticity rate must be between 0.0 and 2.0");
    
    // Validate checkpoint intervals
    validator.addRule("checkpoint.interval",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg > 0 && arg <= 100000;
                }
                return false;
            }, value);
        }, "Checkpoint interval must be positive");
    
    // Validate memory system capacities
    validator.addRule("memory.working.capacity",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg > 0 && arg <= 10000;
                }
                return false;
            }, value);
        }, "Working memory capacity must be positive and reasonable");
    
    // Validate synaptic weight bounds
    validator.addRule("brain.initial_weight_mean",
        [](const ConfigValue& value) -> bool {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= -1.0 && arg <= 1.0;
                }
                return false;
            }, value);
        }, "Initial weight mean must be between -1.0 and 1.0");
}

// Helper method to get validation errors as a formatted string
std::string Config::getValidationErrors() const {
    ConfigValidator validator;
    addDefaultValidationRules(validator);
    auto errors = validator.validate(*this);
    
    if (errors.empty()) {
        return "Configuration is valid";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation errors:";
    for (size_t i = 0; i < errors.size(); ++i) {
        oss << "\n  " << (i + 1) << ". " << errors[i];
    }
    return oss.str();
}

// Helper method to check if a key has a valid type
bool Config::validateKeyType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;  // Key doesn't exist
    }
    
    // Basic type validation based on key prefix
    if (key.find("brain.") == 0 || key.find("plasticity.") == 0 || 
        key.find("neuromod.") == 0 || key.find("development.") == 0) {
        // These keys should have numeric values
        return std::holds_alternative<int64_t>(it->value) ||
               std::holds_alternative<double>(it->value) ||
               std::holds_alternative<bool>(it->value);
    } else if (key.find("memory.") == 0) {
        // Memory keys should have integer values
        return std::holds_alternative<int64_t>(it->value);
    } else if (key.find("checkpoint.") == 0) {
        // Checkpoint keys should have integer values
        return std::holds_alternative<int64_t>(it->value);
    } else if (key.find(".enable") != std::string::npos || 
               key.find(".enabled") != std::string::npos) {
        // Enable flags should be boolean
        return std::holds_alternative<bool>(it->value);
    }
    
    // Default to string for unknown key types
    return std::holds_alternative<std::string>(it->value);
}

// Validate that all entries have consistent types
std::vector<std::string> Config::validateTypeConsistency() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKeyType(entry.key)) {
            errors.push_back("Key \"" + entry.key + "\" has invalid or missing type");
        }
    }
    
    return errors;
}

// Export configuration as JSON for external validation
std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::to_string(arg);
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

std::string Config::escapeJSONString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            case '\\': oss << "\\\\"; break;
            default:
                if (c < 32 || c > 126) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}
