// Include necessary headers
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

ConfigValidationResult Config::validate() const {
    ConfigValidationResult result;
    
    // Check for duplicate keys
    std::unordered_map<std::string, size_t> keyCount;
    for (const auto& entry : pImpl->entries) {
        keyCount[entry.key]++;
    }
    
    for (const auto& [key, count] : keyCount) {
        if (count > 1) {
            result.addWarning("Duplicate configuration key: " + key);
        }
    }
    
    // Basic type validation for known keys
    for (const auto& entry : pImpl->entries) {
        if (entry.key == "random_seed") {
            if (!std::holds_alternative<int64_t>(entry.value) && 
                !std::holds_alternative<int>(entry.value)) {
                result.addError("random_seed should be an integer");
            }
        } else if (entry.key == "simulation_timestep") {
            if (!std::holds_alternative<double>(entry.value)) {
                result.addError("simulation_timestep should be a floating point number");
            }
        } else if (entry.key == "neuron_count") {
            if (!std::holds_alternative<int64_t>(entry.value) && 
                !std::holds_alternative<size_t>(entry.value)) {
                result.addError("neuron_count should be an integer");
            }
        } else if (entry.key == "region_count") {
            if (!std::holds_alternative<int64_t>(entry.value) && 
                !std::holds_alternative<size_t>(entry.value)) {
                result.addError("region_count should be an integer");
            }
        } else if (entry.key == "connection_probability") {
            if (!std::holds_alternative<double>(entry.value) || 
                std::get<double>(entry.value) < 0.0 || 
                std::get<double>(entry.value) > 1.0) {
                result.addError("connection_probability should be a number between 0.0 and 1.0");
            }
        }
    }
    
    return result;
}

bool Config::validateAndFix(ConfigValidationResult& result) {
    // Try to fix validation issues automatically
    bool fixed = false;
    
    for (auto& entry : pImpl->entries) {
        if (entry.key == "connection_probability") {
            if (!std::holds_alternative<double>(entry.value)) {
                // Try to convert to double
                if (std::holds_alternative<int64_t>(entry.value)) {
                    entry.value = static_cast<double>(std::get<int64_t>(entry.value));
                    fixed = true;
                    result.addWarning("connection_probability converted to double: " + std::to_string(std::get<double>(entry.value)));
                }
            }
            // Clamp to valid range
            if (std::holds_alternative<double>(entry.value)) {
                double& value = std::get<double>(entry.value);
                if (value < 0.0) {
                    value = 0.0;
                    fixed = true;
                    result.addWarning("connection_probability clamped to minimum value (0.0)");
                } else if (value > 1.0) {
                    value = 1.0;
                    fixed = true;
                    result.addWarning("connection_probability clamped to maximum value (1.0)");
                }
            }
        }
    }
    
    return fixed;
}

ConfigValidationResult Config::validateWithSchema(const std::vector<ConfigSchema>& schema) const {
    ConfigValidationResult result;
    
    // Check required keys
    for (const auto& schemaEntry : schema) {
        bool keyFound = false;
        for (const auto& configEntry : pImpl->entries) {
            if (configEntry.key == schemaEntry.key) {
                keyFound = true;
                
                // Check type
                if (!checkType(schemaEntry.expectedType, configEntry.value)) {
                    result.addError("Key " + schemaEntry.key + ": expected type " + 
                                  typeToString(schemaEntry.expectedType) + 
                                  ", got " + typeToString(configEntry.value));
                }
                
                // Check range for numeric types
                if (std::holds_alternative<double>(schemaEntry.expectedType) ||
                    std::holds_alternative<float>(schemaEntry.expectedType) ||
                    std::holds_alternative<int64_t>(schemaEntry.expectedType) ||
                    std::holds_alternative<int>(schemaEntry.expectedType)) {
                    
                    double configValue = getNumericValue(configEntry.value);
                    if (!schemaEntry.validRange.empty() && 
                        (configValue < schemaEntry.validRange[0] || configValue > schemaEntry.validRange[1])) {
                        result.addError("Key " + schemaEntry.key + ": value " + std::to_string(configValue) + 
                                      " outside valid range [" + std::to_string(schemaEntry.validRange[0]) + 
                                      ", " + std::to_string(schemaEntry.validRange[1]) + "]");
                    }
                }
                
                break;
            }
        }
        
        if (schemaEntry.required && !keyFound) {
            result.addError("Missing required key: " + schemaEntry.key + ". " + schemaEntry.description);
        }
    }
    
    return result;
}

bool Config::checkType(const ConfigValue& expected, const ConfigValue& actual) const {
    auto expectedType = expected.index();
    auto actualType = actual.index();
    
    // Allow some type flexibility for numeric types
    if (expectedType == actualType) {
        return true;
    }
    
    // Allow int -> int64 conversion
    if (expectedType == typeid(int64_t) && actualType == typeid(int)) {
        return true;
    }
    if (expectedType == typeid(int) && actualType == typeid(int64_t)) {
        return true;
    }
    
    // Allow double -> float conversion
    if (expectedType == typeid(double) && actualType == typeid(float)) {
        return true;
    }
    if (expectedType == typeid(float) && actualType == typeid(double)) {
        return true;
    }
    
    // Don't allow string -> numeric or vice versa
    return false;
}

std::string Config::typeToString(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            typeName = "int";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            typeName = "int64_t";
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            typeName = "uint64_t";
        } else if constexpr (std::is_same_v<T, float>) {
            typeName = "float";
        } else if constexpr (std::is_same_v<T, double>) {
            typeName = "double";
        } else if constexpr (std::is_same_v<T, bool>) {
            typeName = "bool";
        } else if constexpr (std::is_same_v<T, std::string>) {
            typeName = "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            typeName = "std::vector<int>";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            typeName = "std::vector<double>";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            typeName = "std::vector<std::string>";
        }
    }, value);
    return typeName;
}

double Config::getNumericValue(const ConfigValue& value) const {
    double result = 0.0;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, float>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        }
    }, value);
    return result;
}

std::vector<ConfigSchema> Config::getDefaultSchema() const {
    std::vector<ConfigSchema> schema;
    
    // Add schema entries for commonly used configuration keys
    schema.emplace_back("random_seed", 0, true, "Random seed for reproducible experiments");
    schema.emplace_back("simulation_timestep", 0.001, true, "Simulation timestep in seconds");
    schema.emplace_back("neuron_count", 1000, true, "Number of neurons in the brain");
    schema.emplace_back("region_count", 1, true, "Number of brain regions");
    schema.emplace_back("connection_probability", 0.1f, true, "Connection probability for random connectivity", {0.0f, 1.0f});
    schema.emplace_back("plasticity_learning_rate", 0.01f, false, "Plasticity learning rate", {0.0f, 1.0f});
    schema.emplace_back("stdp_ltp_weight", 0.01f, false, "STDP long-term potentiation weight", {0.0f, 1.0f});
    schema.emplace_back("stdp_ltd_weight", 0.012f, false, "STDP long-term depression weight", {0.0f, 1.0f});
    
    return schema;
}