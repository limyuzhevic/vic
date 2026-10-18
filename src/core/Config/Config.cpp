#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

namespace nlm {

namespace {

// Helper to convert ConfigValue to JSON
json configValueToJson(const ConfigValue& value) {
    json j;
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    return j;
}

// Helper to convert JSON to ConfigValue
ConfigValue jsonToConfigValue(const json& j) {
    if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        // Check array element type
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                return j.get<std::vector<int64_t>>();
            } else if (j[0].is_number_float()) {
                return j.get<std::vector<double>>();
            } else if (j[0].is_string()) {
                return j.get<std::vector<std::string>>();
            }
        }
        return std::vector<int64_t>{};
    }
    return "";
}

// Helper to parse value from string
ConfigValue parseValueFromString(const std::string& valueStr) {
    // Try to detect type
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    // Check if it's a number
    bool isInteger = true;
    bool isFloat = true;
    size_t i = 0;
    bool hasMinus = false;
    
    if (!valueStr.empty() && valueStr[0] == '-') {
        hasMinus = true;
        i = 1;
    }
    
    for (; i < valueStr.size(); ++i) {
        if (!std::isdigit(valueStr[i])) {
            if (valueStr[i] == '.' && isInteger) {
                isFloat = true;
                isInteger = false;
            } else {
                isInteger = false;
                isFloat = false;
                break;
            }
        }
    }
    
    if (isInteger && !valueStr.empty()) {
        try {
            return std::stoll(valueStr);
        } catch (...) {
            // Fall through
        }
    } else if (isFloat && !valueStr.empty()) {
        try {
            return std::stod(valueStr);
        } catch (...) {
            // Fall through
        }
    }
    
    // Default to string
    return valueStr;
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    json configSchema;  // JSON schema for validation
};

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load JSON configuration file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Clear existing entries
        clear();
        
        // Process JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();
                const json& valueJson = it.value();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue = jsonToConfigValue(valueJson);
                
                // Add entry with file source
                pImpl->entries.emplace_back(key, configValue, ConfigSource::File, "Loaded from JSON");
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + e.what());
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR("JSON type error in " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from " + filepath + ": " + e.what());
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        
        // Convert all entries to JSON
        for (const auto& entry : pImpl->entries) {
            j[entry.key] = configValueToJson(entry.value);
        }
        
        // Write JSON to file with pretty formatting
        file << j.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving config to " + filepath + ": " + e.what());
        return false;
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

// Additional helper methods implementation

namespace {

ConfigValue parseValueFromString(const std::string& valueStr) {
    // Try to detect type
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    // Check if it's a number
    bool isInteger = true;
    bool isFloat = true;
    size_t i = 0;
    bool hasMinus = false;
    
    if (!valueStr.empty() && valueStr[0] == '-') {
        hasMinus = true;
        i = 1;
    }
    
    for (; i < valueStr.size(); ++i) {
        if (!std::isdigit(valueStr[i])) {
            if (valueStr[i] == '.' && isInteger) {
                isFloat = true;
                isInteger = false;
            } else {
                isInteger = false;
                isFloat = false;
                break;
            }
        }
    }
    
    if (isInteger && !valueStr.empty()) {
        try {
            return std::stoll(valueStr);
        } catch (...) {
            // Fall through
        }
    } else if (isFloat && !valueStr.empty()) {
        try {
            return std::stod(valueStr);
        } catch (...) {
            // Fall through
        }
    }
    
    // Default to string
    return valueStr;
}

} // namespace

// Method implementations

std::string Config::toJsonString() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    return j.dump(2);
}

bool Config::loadFromJsonString(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        // Clear existing entries
        clear();
        
        // Process JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();
                const json& valueJson = it.value();
                
                ConfigValue configValue = jsonToConfigValue(valueJson);
                pImpl->entries.emplace_back(key, configValue, ConfigSource::Runtime, "Loaded from JSON string");
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in string: " + std::string(e.what()));
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR("JSON type error in string: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from JSON string: " + std::string(e.what()));
        return false;
    }
}

bool Config::validateSchema(const JsonSchema& schema) const {
    if (!schema.is_object()) {
        NLM_LOG_ERROR("Invalid schema - must be a JSON object");
        return false;
    }
    
    // Simple schema validation
    for (const auto& item : schema.items()) {
        const std::string& key = item.key();
        const json& schemaItem = item.value();
        
        if (!has(key)) {
            NLM_LOG_ERROR("Missing required config key: " + key);
            return false;
        }
        
        // Check type if specified in schema
        if (schemaItem.contains("type")) {
            std::string type = schemaItem["type"].get<std::string>();
            
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });n            
            if (it != pImpl->entries.end()) {
                bool typeMatches = false;
                
                if (type == "integer" && std::holds_alternative<int64_t>(it->value)) {
                    typeMatches = true;
                } else if (type == "number" && std::holds_alternative<double>(it->value)) {
                    typeMatches = true;
                } else if (type == "boolean" && std::holds_alternative<bool>(it->value)) {
                    typeMatches = true;
                } else if (type == "string" && std::holds_alternative<std::string>(it->value)) {
                    typeMatches = true;
                } else if (type == "array" && (std::holds_alternative<std::vector<int>>(it->value) ||
                                               std::holds_alternative<std::vector<double>>(it->value) ||
                                               std::holds_alternative<std::vector<std::string>>(it->value))) {
                    typeMatches = true;
                }
                
                if (!typeMatches) {
                    NLM_LOG_ERROR("Config key \"" + key + "\" has incorrect type. Expected: " + type);
                    return false;
                }
            }
        }
    }
    
    return true;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Use other's source unless this entry already exists
        if (!has(entry.key)) {
            pImpl->entries.push_back(entry);
        }
    }
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

// Additional helper methods implementation

namespace {

ConfigValue parseValueFromString(const std::string& valueStr) {
    // Try to detect type
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    // Check if it's a number
    bool isInteger = true;
    bool isFloat = true;
    size_t i = 0;
    bool hasMinus = false;
    
    if (!valueStr.empty() && valueStr[0] == '-') {
        hasMinus = true;
        i = 1;
    }
    
    for (; i < valueStr.size(); ++i) {
        if (!std::isdigit(valueStr[i])) {
            if (valueStr[i] == '.' && isInteger) {
                isFloat = true;
                isInteger = false;
            } else {
                isInteger = false;
                isFloat = false;
                break;
            }
        }
    }
    
    if (isInteger && !valueStr.empty()) {
        try {
            return std::stoll(valueStr);
        } catch (...) {
            // Fall through
        }
    } else if (isFloat && !valueStr.empty()) {
        try {
            return std::stod(valueStr);
        } catch (...) {
            // Fall through
        }
    }
    
    // Default to string
    return valueStr;
}

} // namespace

// Method implementations

std::string Config::toJsonString() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    return j.dump(2);
}

bool Config::loadFromJsonString(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        // Clear existing entries
        clear();
        
        // Process JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();
                const json& valueJson = it.value();
                
                ConfigValue configValue = jsonToConfigValue(valueJson);
                pImpl->entries.emplace_back(key, configValue, ConfigSource::Runtime, "Loaded from JSON string");
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in string: " + std::string(e.what()));
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR("JSON type error in string: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from JSON string: " + std::string(e.what()));
        return false;
    }
}

bool Config::validateSchema(const JsonSchema& schema) const {
    if (!schema.is_object()) {
        NLM_LOG_ERROR("Invalid schema - must be a JSON object");
        return false;
    }
    
    // Simple schema validation
    for (const auto& item : schema.items()) {
        const std::string& key = item.key();
        const json& schemaItem = item.value();
        
        if (!has(key)) {
            NLM_LOG_ERROR("Missing required config key: " + key);
            return false;
        }
        
        // Check type if specified in schema
        if (schemaItem.contains("type")) {
            std::string type = schemaItem["type"].get<std::string>();
            
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });n            
            if (it != pImpl->entries.end()) {
                bool typeMatches = false;
                
                if (type == "integer" && std::holds_alternative<int64_t>(it->value)) {
                    typeMatches = true;
                } else if (type == "number" && std::holds_alternative<double>(it->value)) {
                    typeMatches = true;
                } else if (type == "boolean" && std::holds_alternative<bool>(it->value)) {
                    typeMatches = true;
                } else if (type == "string" && std::holds_alternative<std::string>(it->value)) {
                    typeMatches = true;
                } else if (type == "array" && (std::holds_alternative<std::vector<int>>(it->value) ||
                                               std::holds_alternative<std::vector<double>>(it->value) ||
                                               std::holds_alternative<std::vector<std::string>>(it->value))) {
                    typeMatches = true;
                }
                
                if (!typeMatches) {
                    NLM_LOG_ERROR("Config key \"" + key + "\" has incorrect type. Expected: " + type);
                    return false;
                }
            }
        }
    }
    
    return true;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Use other's source unless this entry already exists
        if (!has(entry.key)) {
            pImpl->entries.push_back(entry);
        }
    }
}

// Add missing helper methods

namespace {

ConfigValue parseValueFromString(const std::string& valueStr) {
    // Try to detect type
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    // Check if it's a number
    bool isInteger = true;
    bool isFloat = true;
    size_t i = 0;
    bool hasMinus = false;
    
    if (!valueStr.empty() && valueStr[0] == '-') {
        hasMinus = true;
        i = 1;
    }
    
    for (; i < valueStr.size(); ++i) {
        if (!std::isdigit(valueStr[i])) {
            if (valueStr[i] == '.' && isInteger) {
                isFloat = true;
                isInteger = false;
            } else {
                isInteger = false;
                isFloat = false;
                break;
            }
        }
    }
    
    if (isInteger && !valueStr.empty()) {
        try {
            return std::stoll(valueStr);
        } catch (...) {
            // Fall through
        }
    } else if (isFloat && !valueStr.empty()) {
        try {
            return std::stod(valueStr);
        } catch (...) {
            // Fall through
        }
    }
    
    // Default to string
    return valueStr;
}

} // namespace

// Method implementations

std::string Config::toJsonString() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    return j.dump(2);
}

bool Config::loadFromJsonString(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        
        // Clear existing entries
        clear();
        
        // Process JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();
                const json& valueJson = it.value();
                
                ConfigValue configValue = jsonToConfigValue(valueJson);
                pImpl->entries.emplace_back(key, configValue, ConfigSource::Runtime, "Loaded from JSON string");
            }
        }
        
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in string: " + std::string(e.what()));
        return false;
    } catch (const json::type_error& e) {
        NLM_LOG_ERROR("JSON type error in string: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading config from JSON string: " + std::string(e.what()));
        return false;
    }
}

bool Config::validateSchema(const JsonSchema& schema) const {
    if (!schema.is_object()) {
        NLM_LOG_ERROR("Invalid schema - must be a JSON object");
        return false;
    }
    
    // Simple schema validation
    for (const auto& item : schema.items()) {
        const std::string& key = item.key();
        const json& schemaItem = item.value();
        
        if (!has(key)) {
            NLM_LOG_ERROR("Missing required config key: " + key);
            return false;
        }
        
        // Check type if specified in schema
        if (schemaItem.contains("type")) {
            std::string type = schemaItem["type"].get<std::string>();
            
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });n            
            if (it != pImpl->entries.end()) {
                bool typeMatches = false;
                
                if (type == "integer" && std::holds_alternative<int64_t>(it->value)) {
                    typeMatches = true;
                } else if (type == "number" && std::holds_alternative<double>(it->value)) {
                    typeMatches = true;
                } else if (type == "boolean" && std::holds_alternative<bool>(it->value)) {
                    typeMatches = true;
                } else if (type == "string" && std::holds_alternative<std::string>(it->value)) {
                    typeMatches = true;
                } else if (type == "array" && (std::holds_alternative<std::vector<int>>(it->value) ||
                                               std::holds_alternative<std::vector<double>>(it->value) ||
                                               std::holds_alternative<std::vector<std::string>>(it->value))) {
                    typeMatches = true;
                }
                
                if (!typeMatches) {
                    NLM_LOG_ERROR("Config key \"" + key + "\" has incorrect type. Expected: " + type);
                    return false;
                }
            }
        }
    }
    
    return true;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Use other's source unless this entry already exists
        if (!has(entry.key)) {
            pImpl->entries.push_back(entry);
        }
    }
}

// Ensure all required methods are present
void Config::ensureMethods() {
    // This method ensures all required template methods are instantiated
    // The templates are already instantiated above
}
