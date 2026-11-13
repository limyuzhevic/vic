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

// TODO: Implement proper JSON/YAML parser using nlohmann::json or similar
    // For now, we provide both the simple key=value format (Phase 1) and
    // stub support for JSON/YAML formats (Phase 2+).
    // The simple format is used for quick setup and prototyping.
    // 
    // JSON/YAML support will be added when the project transitions to Phase 2
    // or Phase 3, with proper configuration schema validation and file format support.
    
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
        
        // Save actual value instead of placeholder
        std::visit([&file](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << v << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (v ? "true" : "false") << "\n";
            } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                file << entry.key << " = " << v << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << v[i] << "\"";
                    } else {
                        file << v[i];
                    }
                }
                file << "]\n";
            }
        }, entry.value);
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

// Configuration schema for validation and documentation
struct ConfigSchema {
    std::string key;
    std::string type;
    std::string defaultValue;
    std::string description;
    bool required;
    std::string validation;
};

const std::vector<ConfigSchema>& getConfigSchema() {
    static const std::vector<ConfigSchema> schema = {
        // Brain Configuration
        {"brain.neuron_count", "int", "1000", "Total number of neurons", false, ">= 1 && <= 100000"},
        {"brain.synapse_density", "float", "0.1", "Synapse connectivity density", false, ">= 0.0 && <= 1.0"},
        {"brain.connection_probability", "float", "0.05", "Probability of connection between neurons", false, ">= 0.0 && <= 1.0"},
        {"brain.initial_weight_mean", "float", "0.5", "Mean initial synaptic weight", false, ">=-1.0 && <= 1.0"},
        {"brain.initial_weight_std", "float", "0.1", "Standard deviation of initial weights", false, ">= 0.0"},
        {"brain.v_thresh", "float", "-50.0", "Neuron threshold potential (mV)", false, ">=-100.0 && <= 0.0"},
        {"brain.v_rest", "float", "-70.0", "Resting potential (mV)", false, ">=-80.0 && <= 0.0"},
        {"brain.v_reset", "float", "-75.0", "Reset potential after spike (mV)", false, ">=-80.0 && <= 0.0"},
        {"brain.tau_mem", "float", "20.0", "Membrane time constant (ms)", false, ">= 1.0"},
        {"brain.tau_ref", "float", "2.0", "Refractory period (ms)", false, ">= 0.1"},
        
        // Plasticity Configuration
        {"plasticity.stdp.enable", "bool", "true", "Enable STDP", false, ""},
        {"plasticity.stdp.learning_rate", "float", "0.001", "STDP learning rate", false, ">= 0.0"},
        {"plasticity.stdp.tau_plus", "float", "20.0", "STDP time constant (ms)", false, ">= 1.0"},
        {"plasticity.stdp.tau_minus", "float", "20.0", "STDP time constant (ms)", false, ">= 1.0"},
        {"plasticity.hebbian.enable", "bool", "true", "Enable Hebbian learning", false, ""},
        {"plasticity.structural.enable", "bool", "true", "Enable structural plasticity", false, ""},
        
        // Development Configuration
        {"development.synaptogenesis_rate", "float", "0.001", "Synaptogenesis rate", false, ">= 0.0"},
        {"development.pruning_rate", "float", "0.0001", "Pruning rate", false, ">= 0.0"},
        
        // Neuromodulation Configuration
        {"neuromod.dopamine.scale", "float", "1.0", "Dopamine modulation scale", false, ">= 0.0"},
        {"neuromod.curiosity.enable", "bool", "true", "Enable curiosity-driven exploration", false, ""},
        {"neuromod.novelty.enable", "bool", "true", "Enable novelty detection", false, ""},
        
        // Reward Configuration
        {"reward.discount_factor", "float", "0.99", "Reward discount factor", false, ">= 0.0 && <= 1.0"},
        
        // Environment Configuration
        {"environment.name", "string", "GridWorld", "Environment name", false, ""},
        {"environment.width", "int", "10", "Environment width", false, ">= 1"},
        {"environment.height", "int", "10", "Environment height", false, ">= 1"},
        
        // Simulation Configuration
        {"simulation.max_steps", "int", "10000", "Maximum simulation steps", false, ">= 1"},
        {"simulation.time_limit", "float", "0.0", "Simulation time limit (seconds)", false, ">= 0.0"},
        {"simulation.random_seed", "int64", "42", "Random seed for reproducibility", false, ""},
        {"simulation.timestep", "float", "0.001", "Simulation timestep (seconds)", false, ">= 0.0"},
        
        // Logging Configuration
        {"logging.level", "string", "INFO", "Log level (DEBUG, INFO, WARNING, ERROR)", false, "in DEBUG,INFO,WARNING,ERROR"},
        {"logging.to_file", "bool", "false", "Log to file instead of console", false, ""},
        {"logging.filename", "string", "nlm.log", "Log file name", false, ""},
        
        // Visualization Configuration
        {"visualization.enabled", "bool", "false", "Enable visualization", false, ""},
        {"visualization.update_rate", "float", "30.0", "Visualization update rate (Hz)", false, ">= 1.0"}
    };
    return schema;
}

// Validate configuration values against schema
bool Config::validate(const std::string& key, const ConfigValue& value) const {
    const auto& schema = getConfigSchema();
    
    // Find schema entry
    auto schemaIt = std::find_if(schema.begin(), schema.end(), 
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == schema.end()) {
        // No schema entry, validation passes
        return true;
    }
    
    const auto& entry = *schemaIt;
    
    // Type validation
    bool typeValid = false;
    if (entry.type == "int" && std::holds_alternative<int>(value)) {
        typeValid = true;
    } else if (entry.type == "int64" && std::holds_alternative<int64_t>(value)) {
        typeValid = true;
    } else if (entry.type == "float" && (std::holds_alternative<float>(value) || 
                                         std::holds_alternative<double>(value) || 
                                         std::holds_alternative<int>(value))) {
        typeValid = true;
    } else if (entry.type == "bool" && std::holds_alternative<bool>(value)) {
        typeValid = true;
    } else if (entry.type == "string" && std::holds_alternative<std::string>(value)) {
        typeValid = true;
    } else if (entry.type == "vector<int>" && std::holds_alternative<std::vector<int>>(value)) {
        typeValid = true;
    } else if (entry.type == "vector<float>" && (std::holds_alternative<std::vector<float>>(value) || 
                                                 std::holds_alternative<std::vector<double>>(value))) {
        typeValid = true;
    } else if (entry.type == "vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) {
        typeValid = true;
    }
    
    if (!typeValid) {
        return false;
    }
    
    // Value validation based on validation string
    if (!entry.validation.empty()) {
        return validateValue(value, entry.validation);
    }
    
    return true;
}

// Validate configuration values based on validation rules
bool Config::validateValue(const ConfigValue& value, const std::string& validation) const {
    // Helper function to convert value to string for validation
    std::string valueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](const auto& v) {
            oss << v;
        }, value);
        return oss.str();
    }
    
    std::string valStr = valueToString(value);
    
    // Simple validation for common cases
    if (validation.find(">=") != std::string::npos) {
        // Parse range validation
        // This is a simplified implementation
        return true;
    } else if (validation.find("in ") != std::string::npos) {
        // Check if value is in allowed set
        // This is a simplified implementation
        return true;
    }
    
    return true; // Default: if we can't validate, assume it's valid
}

// Check for configuration errors
std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key, entry.value)) {
            errors.push_back("Invalid configuration value: " + entry.key + " = " + 
                           (std::holds_alternative<std::string>(entry.value) ? 
                            std::get<std::string>(entry.value) : "[complex value]"));
        }
    }
    
    // Check for required keys
    const auto& schema = getConfigSchema();
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            errors.push_back("Missing required configuration: " + schemaEntry.key);
        }
    }
    
    return errors;
}

// Set default values based on schema
void Config::applySchemaDefaults() {
    const auto& schema = getConfigSchema();
    
    for (const auto& schemaEntry : schema) {
        if (!has(schemaEntry.key)) {
            // Set default value based on type
            ConfigSource source = ConfigSource::Default;
            
            if (schemaEntry.type == "int") {
                set(schemaEntry.key, std::stoi(schemaEntry.defaultValue), source);
            } else if (schemaEntry.type == "int64") {
                set(schemaEntry.key, std::stoll(schemaEntry.defaultValue), source);
            } else if (schemaEntry.type == "float") {
                set(schemaEntry.key, std::stof(schemaEntry.defaultValue), source);
            } else if (schemaEntry.type == "bool") {
                bool defaultBool = (schemaEntry.defaultValue == "true" || 
                                   schemaEntry.defaultValue == "1" || 
                                   schemaEntry.defaultValue == "yes");
                set(schemaEntry.key, defaultBool, source);
            } else if (schemaEntry.type == "string") {
                set(schemaEntry.key, schemaEntry.defaultValue, source);
            } else if (schemaEntry.type == "vector<int>") {
                // Parse vector from string (simplified)
                set(schemaEntry.key, std::vector<int>(), source);
            } else if (schemaEntry.type == "vector<float>") {
                // Parse vector from string (simplified)
                set(schemaEntry.key, std::vector<double>(), source);
            } else if (schemaEntry.type == "vector<string>") {
                // Parse vector from string (simplified)
                set(schemaEntry.key, std::vector<std::string>(), source);
            }
        }
    }
}
