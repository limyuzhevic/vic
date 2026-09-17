#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return loadFromJSON(buffer.str());
    }
    
    // Otherwise try key=value format for backward compatibility
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
            
            auto configValue = stringToConfigValue(value, key);
            if (configValue) {
                set(key, *configValue, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        clear();
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            auto configValue = jsonToConfigValue(*it, it.key());
            if (configValue) {
                set(it.key(), *configValue, ConfigSource::File);
            } else {
                // Try to fix invalid values
                ConfigValue fixedValue;
                if (fixInvalidValue(fixedValue, it.key())) {
                    configValue = jsonToConfigValue(fixedValue, it.key());
                    if (configValue) {
                        set(it.key(), *configValue, ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if it's a JSON file
    bool isJSON = filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json";
    
    if (isJSON) {
        // Save as JSON
        nlohmann::json json;
        
        for (const auto& entry : pImpl->entries) {
            json[entry.key] = configValueToJSON(entry.value);
        }
        
        file << std::setw(2) << json << std::endl;
    } else {
        // Save as key=value format (backward compatibility)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
        }
    }
    
    return true;
}

void Config::loadFromArgs(int argc, char** argv) {
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
    oss << "Configuration Summary:\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Type: " << getTypeName(entry.value) << "\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Value: " << configValueToString(entry.value) << "\n";
        oss << "  Description: " << entry.description << "\n\n";
    }
    
    return oss.str();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source, "");
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // First try to convert to appropriate type
    auto configValue = stringToConfigValue(value, key);
    if (configValue) {
        set(key, *configValue, source);
    } else {
        // If conversion fails, store as string
        set(key, ConfigValue(value), source);
    }
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

std::vector<ValidationError> Config::validate() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        auto schemaIt = schema.find(entry.key);
        if (schemaIt != schema.end()) {
            if (!schemaIt->second.validator(entry.value)) {
                errors.push_back(ValidationError(entry.key, 
                    "Invalid value for key '" + entry.key + "'. Expected type " + schemaIt->second.typeName));
            }
        }
    }
    
    return errors;
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    schema[entry.key] = entry;
}

bool Config::loadDefaults(const std::string& filepath) {
    clear();
    return loadFromFile(filepath);
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    nlohmann::json json;
    
    std::visit([&json](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            json = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                json.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                json.push_back(item);
            }
        }
    }, value);
    
    return json;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& json, const std::string& key) {
    try {
        if (json.is_string()) {
            return stringToConfigValue(json.get<std::string>(), key);
        } else if (json.is_number_integer()) {
            return ConfigValue(json.get<int64_t>());
        } else if (json.is_number_unsigned()) {
            return ConfigValue(static_cast<int64_t>(json.get<unsigned long long>()));
        } else if (json.is_number()) {
            return ConfigValue(json.get<double>());
        } else if (json.is_boolean()) {
            return ConfigValue(json.get<bool>());
        } else if (json.is_array()) {
            std::vector<std::string> items;
            for (const auto& item : json) {
                items.push_back(item.get<std::string>());
            }
            
            // Determine array type based on content
            bool allInts = true;
            bool allDoubles = true;
            for (const auto& item : json) {
                if (!item.is_number_integer()) allInts = false;
                if (!item.is_number()) allDoubles = false;
            }
            
            if (allInts) {
                std::vector<int> intVec;
                for (const auto& item : json) {
                    intVec.push_back(item.get<int>());
                }
                return ConfigValue(intVec);
            } else if (allDoubles) {
                std::vector<double> doubleVec;
                for (const auto& item : json) {
                    doubleVec.push_back(item.get<double>());
                }
                return ConfigValue(doubleVec);
            } else {
                return ConfigValue(items);
            }
        }
    } catch (...) {
        // Handle conversion errors
    }
    
    return std::nullopt;
}

bool Config::fixInvalidValue(ConfigValue& value, const std::string& key) {
    try {
        std::string valStr;
        if (std::holds_alternative<std::string>(value)) {
            valStr = std::get<std::string>(value);
        }
        
        if (!valStr.empty()) {
            auto newValue = stringToConfigValue(valStr, key);
            if (newValue) {
                value = *newValue;
                return true;
            }
        }
    } catch (...) {
        // Handle errors
    }
    
    return false;
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str, const std::string& key) {
    if (str.empty()) {
        return ConfigValue(std::string(""));
    }
    
    // Try boolean
    if (str == "true" || str == "1" || str == "yes" || str == "on") {
        return ConfigValue(true);
    } else if (str == "false" || str == "0" || str == "no" || str == "off") {
        return ConfigValue(false);
    }
    
    // Try integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(intVal);
        }
    } catch (...) {
        // Ignore
    }
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return ConfigValue(doubleVal);
        }
    } catch (...) {
        // Ignore
    }
    
    // Return as string
    return ConfigValue(str);
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ",";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ",";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ",";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::validateInt(const ConfigValue& value, const ConfigSchemaEntry& schema) {
    if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) {
        return false;
    }
    
    int64_t val;
    if (std::holds_alternative<int>(value)) {
        val = std::get<int>(value);
    } else {
        val = std::get<int64_t>(value);
    }
    
    return val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max();
}

bool Config::validateDouble(const ConfigValue& value, const ConfigSchemaEntry& schema) {
    if (!std::holds_alternative<double>(value)) {
        return false;
    }
    
    double val = std::get<double>(value);
    
    // Check for NaN or infinity
    return !std::isnan(val) && !std::isinf(val);
}

bool Config::validateBool(const ConfigValue& value, const ConfigSchemaEntry& schema) {
    return std::holds_alternative<bool>(value);
}

bool Config::validateString(const ConfigValue& value, const ConfigSchemaEntry& schema) {
    return std::holds_alternative<std::string>(value);
}

bool Config::validateVector(const ConfigValue& value, const ConfigSchemaEntry& schema) {
    return std::holds_alternative<std::vector<int>>(value) ||
           std::holds_alternative<std::vector<double>>(value) ||
           std::holds_alternative<std::vector<std::string>>(value);
}

void Config::initializeDefaultSchema() {
    // Add default schema entries for common configuration keys
    
    // Integer fields
    addSchemaEntry(ConfigSchemaEntry("random_seed", "Random seed for reproducible experiments", ConfigValue(42),
        [this](const ConfigValue& v) { return validateInt(v, schema["random_seed"]); }, true, "int"));
    
    addSchemaEntry(ConfigSchemaEntry("neuron_count", "Number of neurons in the brain", ConfigValue(1000),
        [this](const ConfigValue& v) { return validateInt(v, schema["neuron_count"]); }, true, "int"));
    
    addSchemaEntry(ConfigSchemaEntry("region_count", "Number of brain regions", ConfigValue(2),
        [this](const ConfigValue& v) { return validateInt(v, schema["region_count"]); }, true, "int"));
    
    // Double fields
    addSchemaEntry(ConfigSchemaEntry("simulation_timestep", "Simulation timestep in seconds", ConfigValue(0.001),
        [this](const ConfigValue& v) { return validateDouble(v, schema["simulation_timestep"]); }, true, "double"));
    
    addSchemaEntry(ConfigSchemaEntry("connection_probability", "Connection probability for random connectivity", ConfigValue(0.1),
        [this](const ConfigValue& v) { return validateDouble(v, schema["connection_probability"]); }, true, "double"));
    
    addSchemaEntry(ConfigSchemaEntry("plasticity_learning_rate", "Plasticity learning rate", ConfigValue(0.01),
        [this](const ConfigValue& v) { return validateDouble(v, schema["plasticity_learning_rate"]); }, false, "double"));
    
    // Boolean fields
    addSchemaEntry(ConfigSchemaEntry("log_to_file", "Whether to log to file", ConfigValue(false),
        [this](const ConfigValue& v) { return validateBool(v, schema["log_to_file"]); }, false, "bool"));
    
    addSchemaEntry(ConfigSchemaEntry("visualization_enabled", "Whether visualization is enabled", ConfigValue(false),
        [this](const ConfigValue& v) { return validateBool(v, schema["visualization_enabled"]); }, false, "bool"));
    
    // String fields
    addSchemaEntry(ConfigSchemaEntry("environment_name", "Name of the environment", ConfigValue(std::string("GridWorld")),
        [this](const ConfigValue& v) { return validateString(v, schema["environment_name"]); }, true, "string"));
    
    addSchemaEntry(ConfigSchemaEntry("log_level", "Log level", ConfigValue(std::string("INFO")),
        [this](const ConfigValue& v) { return validateString(v, schema["log_level"]); }, false, "string"));
    
    addSchemaEntry(ConfigSchemaEntry("log_filename", "Log filename", ConfigValue(std::string("nlm.log")),
        [this](const ConfigValue& v) { return validateString(v, schema["log_filename"]); }, false, "string"));
}

std::string Config::getTypeName(const ConfigValue& value) {
    std::string type;
    
    if (std::holds_alternative<int>(value)) {
        type = "int";
    } else if (std::holds_alternative<int64_t>(value)) {
        type = "int64_t";
    } else if (std::holds_alternative<double>(value)) {
        type = "double";
    } else if (std::holds_alternative<bool>(value)) {
        type = "bool";
    } else if (std::holds_alternative<std::string>(value)) {
        type = "string";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        type = "vector<int>";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        type = "vector<double>";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        type = "vector<string>";
    }
    
    return type;
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

// Explicit template instantiations - only needed for templates used across modules
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

} // namespace nlm
