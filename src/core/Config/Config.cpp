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

} // namespace nlm

// ConfigSchema implementation
namespace nlm {

ValidationResult ConfigSchema::validate(const Config& config) const {
    ValidationResult result;
    
    for (const auto& rulePair : rules) {
        const std::string& key = rulePair.first;
        const ValidationRule& rule = rulePair.second;
        
        if (config.has(key)) {
            const ConfigValue& value = config.get(key);
            
            // Check if value passes the validator
            if (!rule.validator(value)) {
                result.addError(rule.errorMessage + " (key: '" + key + "')");
                continue;
            }
        } else {
            // Check if default should be applied
            if (rule.defaultGenerator) {
                result.addWarning("Default value should be applied for key: '" + key + "'");
            } else if (!rule.warningMessage.empty()) {
                result.addWarning("Missing required configuration key: '" + key + "'");
            }
        }
    }
    
    return result;
}

ConfigSchema ConfigSchema::createDefaultSchema() {
    ConfigSchema schema;
    
    // Define configuration schema rules
    // Random seed
    schema.addRule("random_seed",
                   [](const ConfigValue& v) { return std::get_if<int64_t>(&v) != nullptr; },
                   "Random seed must be an integer");
    
    // Simulation timestep
    schema.addRule("simulation_timestep",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Simulation timestep must be a double");
    
    // Neuron count
    schema.addRule("neuron_count",
                   [](const ConfigValue& v) { return std::get_if<int64_t>(&v) != nullptr; },
                   "Neuron count must be an integer");
    
    // Region count
    schema.addRule("region_count",
                   [](const ConfigValue& v) { return std::get_if<int64_t>(&v) != nullptr; },
                   "Region count must be an integer");
    
    // Connection probability
    schema.addRule("connection_probability",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Connection probability must be a double");
    
    // STDP LTP weight
    schema.addRule("stdp_ltp_weight",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "STDP LTP weight must be a double");
    
    // STDP LTD weight
    schema.addRule("stdp_ltd_weight",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "STDP LTD weight must be a double");
    
    // STDP tau
    schema.addRule("stdp_tau",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "STDP tau must be a double");
    
    // Synaptogenesis rate
    schema.addRule("synaptogenesis_rate",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Synaptogenesis rate must be a double");
    
    // Pruning rate
    schema.addRule("pruning_rate",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Pruning rate must be a double");
    
    // Hebbian learning rate
    schema.addRule("hebbian_rate",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Hebbian rate must be a double");
    
    // Developmental stage
    schema.addRule("developmental_stage",
                   [](const ConfigValue& v) { return std::get_if<std::string>(&v) != nullptr; },
                   "Developmental stage must be a string",
                   "Valid stages: Initial, CriticalPeriod, Maturation, Adult, Aging",
                   nullptr,
                   {-1e6, 1e6},
                   {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"});
    
    // Replay interval
    schema.addRule("replay_interval",
                   [](const ConfigValue& v) { return std::get_if<int64_t>(&v) != nullptr; },
                   "Replay interval must be an integer");
    
    // Consolidation interval
    schema.addRule("consolidation_interval",
                   [](const ConfigValue& v) { return std::get_if<int64_t>(&v) != nullptr; },
                   "Consolidation interval must be an integer");
    
    // Dopamine level
    schema.addRule("dopamine_level",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Dopamine level must be a double");
    
    // Curiosity level
    schema.addRule("curiosity_level",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Curiosity level must be a double");
    
    // Novelty level
    schema.addRule("novelty_level",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Novelty level must be a double");
    
    // Prediction error
    schema.addRule("prediction_error",
                   [](const ConfigValue& v) { return std::get_if<double>(&v) != nullptr; },
                   "Prediction error must be a double");
    
    return schema;
}

std::string ConfigSchema::getSchemaDocumentation() {
    std::string doc = "=== NLM Configuration Schema ===\n\n";
    doc += "This schema defines the validation rules for all NLM configuration parameters.\n\n";
    
    for (const auto& rulePair : rules) {
        const std::string& key = rulePair.first;
        const ValidationRule& rule = rulePair.second;
        
        doc += "Key: '" + key + "'\n";
        doc += "  Type: Automatically inferred\n";
        doc += "  Required: " + (rule.validator ? "Yes" : "No") + "\n";
        doc += "  Error Message: " + rule.errorMessage + "\n";
        if (!rule.warningMessage.empty()) {
            doc += "  Warning: " + rule.warningMessage + "\n";
        }
        if (!rule.allowedValues.empty()) {
            doc += "  Allowed Values: ";
            for (size_t i = 0; i < rule.allowedValues.size(); ++i) {
                if (i > 0) doc += ", ";
                doc += "'" + rule.allowedValues[i] + "'";
            }
            doc += "\n";
        }
        doc += "\n";
    }
    
    return doc;
}

} // namespace nlm

// Implement the helper functions in the Config.cpp namespace
namespace nlm {

bool getBool(const ConfigValue& value) {
    if (auto b = std::get_if<bool>(&value)) {
        return *b;
    }
    return false;
}

int64_t getInt(const ConfigValue& value) {
    if (auto i = std::get_if<int>(&value)) {
        return *i;
    } else if (auto i64 = std::get_if<int64_t>(&value)) {
        return *i64;
    }
    return 0;
}

double getDouble(const ConfigValue& value) {
    if (auto d = std::get_if<double>(&value)) {
        return *d;
    } else if (auto i = std::get_if<int>(&value)) {
        return *i;
    } else if (auto i64 = std::get_if<int64_t>(&value)) {
        return *i64;
    }
    return 0.0;
}

std::string getString(const ConfigValue& value) {
    if (auto s = std::get_if<std::string>(&value)) {
        return *s;
    }
    return "";
}

} // namespace nlm
