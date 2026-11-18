#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchemaEntry> schema;
    std::unordered_map<std::string, size_t> usageStats;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    applyDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            setInternal(key, value, ConfigSource::File);
        }
    }

    if (validate) {
        ValidationResult result = validate();
        if (!result.valid) {
            std::cerr << "Configuration validation failed:" << std::endl;
            for (const auto& error : result.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
            return false;
        }
        if (!result.warnings.empty()) {
            std::cout << "Configuration warnings:" << std::endl;
            for (const auto& warning : result.warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
    }

    return true;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setInternal(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setInternal(key, value, ConfigSource::CommandLine);
        }
    }
    
    if (validate) {
        ValidationResult result = validate();
        if (!result.valid) {
            std::cerr << "Command line configuration validation failed:" << std::endl;
            for (const auto& error : result.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromMap(const std::unordered_map<std::string, std::string>& map, bool validate) {
    for (const auto& [key, value] : map) {
        setInternal(key, value, ConfigSource::Runtime);
    }
    
    if (validate) {
        ValidationResult result = validate();
        if (!result.valid) {
            std::cerr << "Map configuration validation failed:" << std::endl;
            for (const auto& error : result.errors) {
                std::cerr << "  ERROR: " << error << std::endl;
            }
            return false;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool includeExperimental) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on " << getCurrentTime() << std::endl;
    file << "# Format: key = value" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        if (!includeExperimental && entry.experimental) {
            continue;
        }
        
        file << "# " << entry.description << std::endl;
        file << entry.key << " = " << toString(entry.value) << "\n";
    }
    
    return true;
}

std::optional<int> Config::getInt(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<int> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<int>(v);
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<int64_t> Config::getInt64(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<int64_t> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int64_t>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<int64_t>(v);
        } else if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<int64_t>(v);
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<double> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<double>(v);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<double>(v);
        } else if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<double>(v);
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<bool> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<bool>(v);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<bool>(v);
        } else if constexpr (std::is_same_v<T, double>) {
            return static_cast<bool>(v);
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<std::string> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<std::vector<int>> Config::getIntVector(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<std::vector<int>> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::vector<int>>) {
            return v;
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<std::vector<double>> Config::getDoubleVector(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<std::vector<double>> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::vector<double>>) {
            return v;
        } else {
            return std::nullopt;
        }
    }, val.value());
}

std::optional<std::vector<std::string>> Config::getStringVector(const std::string& key) const {
    auto val = getInternal(key);
    if (!val.has_value()) return std::nullopt;
    
    return std::visit([](auto&& v) -> std::optional<std::vector<std::string>> {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return v;
        } else {
            return std::nullopt;
        }
    }, val.value());
}

int Config::getIntOr(const std::string& key, int defaultValue) const {
    auto val = getInt(key);
    return val.has_value() ? val.value() : defaultValue;
}

int64_t Config::getInt64Or(const std::string& key, int64_t defaultValue) const {
    auto val = getInt64(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getDoubleOr(const std::string& key, double defaultValue) const {
    auto val = getDouble(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getBoolOr(const std::string& key, bool defaultValue) const {
    auto val = getBool(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::string Config::getStringOr(const std::string& key, const std::string& defaultValue) const {
    auto val = getString(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, int64_t value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

bool Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    bool success = setInternal(key, value, source);
    if (success) {
        recordUsage(key);
    }
    return success;
}

void Config::setInternal(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key === key; });

    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    timestamp.pop_back();  // Remove newline

    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->lastModified = timestamp;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        it->lastModified = timestamp;
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key === key; };
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key === key; }),
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
    pImpl->usageStats.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (entry.experimental) {
            oss << " [EXPERIMENTAL]";
        }
        oss << "\n";
    }
    
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        // Check schema if available
        auto schemaIt = pImpl->schema.find(entry.key);
        if (schemaIt != pImpl->schema.end()) {
            const auto& schema = schemaIt->second;
            
            // Validate type
            if (!matchesType(entry.value, schema.type)) {
                result.addError("Key \"" + entry.key + "\" expects type " + schema.type + ", got " + getTypeName(entry.value));
            }
            
            // Run validator if available
            if (schema.validator) {
                ValidationResult validation = schema.validator(entry.value);
                if (!validation.valid) {
                    for (const auto& error : validation.errors) {
                        result.addError("Key \"" + entry.key + "\": " + error);
                    }
                }
                for (const auto& warning : validation.warnings) {
                    result.addWarning("Key \"" + entry.key + "\": " + warning);
                }
            }
            
            // Check if required
            if (schema.required && !has(entry.key)) {
                result.addError("Key \"" + entry.key + "\" is required");
            }
        }
    }
    
    return result;
}

void Config::applyDefaults() {
    for (const auto& [key, schema] : pImpl->schema) {
        if (!has(key)) {
            setInternal(key, schema.defaultValue, ConfigSource::Default);
        }
    }
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    pImpl->schema[entry.key] = entry;
}

const ConfigSchemaEntry* Config::getSchemaEntry(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    return it != pImpl->schema.end() ? &it->second : nullptr;
}

bool Config::isExperimental(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key === key; });
    return it != pImpl->entries.end() && it->experimental;
}

void Config::recordUsage(const std::string& key) {
    pImpl->usageStats[key]++;
}

std::string Config::getUsageStats() const {
    std::ostringstream oss;
    oss << "Configuration Usage Statistics:" << std::endl;
    
    for (const auto& [key, count] : pImpl->usageStats) {
        oss << "  " << key << ": " << count << " times" << std::endl;
    }
    
    return oss.str();
}

std::string Config::getMetadata() const {
    std::ostringstream oss;
    oss << "Configuration Metadata:" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "Schema entries: " << pImpl->schema.size() << std::endl;
    oss << "Unique keys used: " << pImpl->usageStats.size() << std::endl;
    oss << "Experimental entries: " << countExperimental() << std::endl;
    
    return oss.str();
}

// Template implementation details
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key === key; });
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

// Helper functions
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

std::string Config::toString(const ConfigValue& value) {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + v + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                            std::is_same_v<T, std::vector<double>> ||
                            std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < v.size(); ++i) {
                if (i) oss << ", ";
                oss << v[i];
            }
            oss << "]";
            return oss.str();
        } else {
            return std::to_string(v);
        }
    }, value);
}

bool Config::matchesType(const ConfigValue& value, const std::string& typeName) {
    std::string actualType = getTypeName(value);
    return actualType == typeName;
}

int Config::convertToInt(const ConfigValue& value) {
    return std::visit([](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) return v;
        else if constexpr (std::is_arithmetic_v<T>) return static_cast<int>(v);
        else return 0;
    }, value);
}

int64_t Config::convertToInt64(const ConfigValue& value) {
    return std::visit([](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int64_t>) return v;
        else if constexpr (std::is_same_v<T, int>) return static_cast<int64_t>(v);
        else if constexpr (std::is_arithmetic_v<T>) return static_cast<int64_t>(v);
        else return 0;
    }, value);
}

double Config::convertToDouble(const ConfigValue& value) {
    return std::visit([](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, double>) return v;
        else if constexpr (std::is_arithmetic_v<T>) return static_cast<double>(v);
        else return 0.0;
    }, value);
}

bool Config::convertToBool(const ConfigValue& value) {
    return std::visit([](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, bool>) return v;
        else if constexpr (std::is_same_v<T, int>) return static_cast<bool>(v);
        else if constexpr (std::is_same_v<T, int64_t>) return static_cast<bool>(v);
        else if constexpr (std::is_same_v<T, double>) return static_cast<bool>(v);
        else return false;
    }, value);
}

std::string Config::getTypeName(const ConfigValue& value) {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "std::vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "std::vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "std::vector<string>";
        else return "unknown";
    }, value);
}

std::string Config::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::string timestamp = std::ctime(&now);
    timestamp.pop_back();  // Remove newline
    return timestamp;
}

ValidationResult validatePositiveInt(const ConfigValue& value) {
    ValidationResult result;
    auto val = std::get_if<int>(&value);
    if (val && *val <= 0) {
        result.addError("Value must be positive, got " + std::to_string(*val));
    }
    return result;
}

ValidationResult validateNonNegativeInt(const ConfigValue& value) {
    ValidationResult result;
    auto val = std::get_if<int>(&value);
    if (val && *val < 0) {
        result.addError("Value must be non-negative, got " + std::to_string(*val));
    }
    return result;
}

ValidationResult validatePositiveDouble(const ConfigValue& value) {
    ValidationResult result;
    auto val = std::get_if<double>(&value);
    if (val && *val <= 0.0) {
        result.addError("Value must be positive, got " + std::to_string(*val));
    }
    return result;
}

ValidationResult validateRangeDouble(const ConfigValue& value, double min, double max) {
    ValidationResult result;
    auto val = std::get_if<double>(&value);
    if (val && (*val < min || *val > max)) {
        result.addError("Value must be in range [" + std::to_string(min) + ", " + std::to_string(max) + "], got " + std::to_string(*val));
    }
    return result;
}

ValidationResult validateStringLength(const ConfigValue& value, size_t minLength, size_t maxLength) {
    ValidationResult result;
    auto val = std::get_if<std::string>(&value);
    if (val && (val->length() < minLength || val->length() > maxLength)) {
        result.addError("String length must be between " + std::to_string(minLength) + " and " + std::to_string(maxLength) + ", got length " + std::to_string(val->length()));
    }
    return result;
}

ValidationResult validateEnum(const ConfigValue& value, const std::vector<std::string>& validValues) {
    ValidationResult result;
    auto val = std::get_if<std::string>(&value);
    if (val) {
        bool valid = false;
        for (const auto& validVal : validValues) {
            if (*val == validVal) {
                valid = true;
                break;
            }
        }
        if (!valid) {
            result.addError("Value must be one of: " + join(validValues, ", "));
        }
    }
    return result;
}

ValidationResult validateConfigFileExists(const ConfigValue& value) {
    ValidationResult result;
    auto val = std::get_if<std::string>(&value);
    if (val) {
        std::ifstream file(*val);
        if (!file.good()) {
            result.addError("Configuration file does not exist or cannot be read: " + *val);
        }
    }
    return result;
}

std::vector<ConfigSchemaEntry> Config::initializeSchema() {
    std::vector<ConfigSchemaEntry> schema;
    
    // Core configuration
    schema.emplace_back("neuron_count", "int", 1000,
                       "Total number of neurons in the brain", true, false);
    
    schema.emplace_back("region_count", "int", 1,
                       "Number of neural regions", true, false);
    
    schema.emplace_back("connection_probability", "double", 0.1,
                       "Probability of connection between neurons", true, false);
    
    schema.emplace_back("simulation_timestep", "double", 0.001,
                       "Simulation timestep in seconds", true, false);
    
    schema.emplace_back("random_seed", "int64_t", 42,
                       "Random seed for reproducible simulations", true, false);
    
    // Plasticity configuration
    schema.emplace_back("stdp_ltp_weight", "double", 0.01,
                       "STDP long-term potentiation weight", false, false);
    
    schema.emplace_back("stdp_ltd_weight", "double", 0.012,
                       "STDP long-term depression weight", false, false);
    
    schema.emplace_back("stdp_tau", "double", 20.0,
                       "STDP time constant in milliseconds", false, false);
    
    schema.emplace_back("hebbian_learning_rate", "double", 0.001,
                       "Hebbian learning rate", false, false);
    
    // Structural plasticity
    schema.emplace_back("synaptogenesis_rate", "double", 0.0001,
                       "Rate of new synapse formation", false, false);
    
    schema.emplace_back("pruning_rate", "double", 0.00001,
                       "Rate of synapse elimination", false, false);
    
    // Neuromodulation
    schema.emplace_back("dopamine_scale", "double", 1.0,
                       "Dopamine modulation scale factor", false, false);
    
    schema.emplace_back("curiosity_decay", "double", 0.01,
                       "Curiosity decay rate", false, false);
    
    schema.emplace_back("novelty_threshold", "double", 0.1,
                       "Novelty detection threshold", false, false);
    
    // Development
    schema.emplace_back("critical_period_start", "double", 0.1,
                       "Start of critical development period", false, false);
    
    schema.emplace_back("critical_period_end", "double", 1.0,
                       "End of critical development period", false, false);
    
    // Memory systems
    schema.emplace_back("working_memory_capacity", "int", 100,
                       "Capacity of working memory in units", false, false);
    
    schema.emplace_back("episodic_memory_duration", "double", 10.0,
                       "Duration of episodic memory traces in seconds", false, false);
    
    // Experimental features
    schema.emplace_back("experimental_plasticity_rules", "bool", false,
                       "Enable experimental plasticity rules", false, true);
    
    schema.emplace_back("experimental_neuromodulation", "bool", false,
                       "Enable experimental neuromodulation", false, true);
    
    return schema;
}

std::unordered_map<std::string, ConfigSchemaEntry> Config::createBuiltInSchema() {
    std::unordered_map<std::string, ConfigSchemaEntry> schema;
    
    for (const auto& entry : initializeSchema()) {
        schema[entry.key] = entry;
    }
    
    return schema;
}

} // namespace nlm
