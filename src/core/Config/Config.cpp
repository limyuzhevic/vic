#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include <map>

// Try to include JSON and YAML libraries if available
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
namespace nlohmann
{
    template<typename T>
    ConfigValue jsonToConfigValue(const T& value);
    
    template<typename T>
    T configValueToJson(const ConfigValue& configVal);
}
#endif

#if __has_include(<yaml-cpp/yaml.h>)
#include <yaml-cpp/yaml.h>
namespace YAML
{
    template<typename T>
    ConfigValue yamlToConfigValue(const T& value);
    
    template<typename T>
    T configValueToYaml(const ConfigValue& configVal);
}
#endif

// Implementation of JSON to ConfigValue conversion
namespace nlm {

// JSON to ConfigValue conversion helpers
namespace json_helpers {
    template<typename T>
    ConfigValue jsonToConfigValue(const T& value);
    
    template<typename T>
    T configValueToJson(const ConfigValue& configVal);
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return ConfigValue(j.get<std::string>());
    } else if (j.is_number_integer()) {
        return ConfigValue(j.get<int64_t>());
    } else if (j.is_number_unsigned()) {
        return ConfigValue(static_cast<int64_t>(j.get<uint64_t>()));
    } else if (j.is_number_float()) {
        return ConfigValue(j.get<double>());
    } else if (j.is_boolean()) {
        return ConfigValue(j.get<bool>());
    } else if (j.is_array()) {
        // Try to determine array type
        const auto& arr = j;
        if (!arr.empty()) {
            // Check if all elements are integers
            bool allInts = std::all_of(arr.begin(), arr.end(), [](const auto& elem) { return elem.is_number_integer(); });
            if (allInts) {
                std::vector<int> intVec;
                intVec.reserve(arr.size());
                for (const auto& elem : arr) {
                    intVec.push_back(elem.get<int>());
                }
                return ConfigValue(intVec);
            }
            
            // Check if all elements are doubles
            bool allDoubles = std::all_of(arr.begin(), arr.end(), [](const auto& elem) { return elem.is_number_float(); });
            if (allDoubles) {
                std::vector<double> doubleVec;
                doubleVec.reserve(arr.size());
                for (const auto& elem : arr) {
                    doubleVec.push_back(elem.get<double>());
                }
                return ConfigValue(doubleVec);
            }
            
            // Check if all elements are strings
            bool allStrings = std::all_of(arr.begin(), arr.end(), [](const auto& elem) { return elem.is_string(); });
            if (allStrings) {
                std::vector<std::string> stringVec;
                stringVec.reserve(arr.size());
                for (const auto& elem : arr) {
                    stringVec.push_back(elem.get<std::string>());
                }
                return ConfigValue(stringVec);
            }
        }
        // Default to int vector if array type cannot be determined
        std::vector<int> defaultVec;
        defaultVec.reserve(arr.size());
        for (const auto& elem : arr) {
            defaultVec.push_back(elem.get<int>());
        }
        return ConfigValue(defaultVec);
    } else if (j.is_null()) {
        return ConfigValue(std::string(""));  // Default to empty string
    }
    
    return ConfigValue(std::string("")); // Default fallback
}

// Convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& configVal) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(arg);
        }
        return nlohmann::json(); // Default empty JSON
    }, configVal);
}

// Helper function to convert ConfigValue to string representation
std::string configValueToString(const ConfigValue& configVal) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else {
            return std::to_string(arg);
        }
    }, configVal);
}

// Helper function to check if string ends with suffix
bool endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// Helper function to resolve source priority
ConfigSource resolveSource(ConfigSource source) const {
    // Default implementation - can be overridden by subclasses
    return source;
}

// Helper function to convert ConfigValue to string for summary
std::string valueToString(const ConfigValue& value) {
    return configValueToString(value);
}

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

// JSON parsing implementation
static bool isValidJson(const std::string& jsonString) {
    if (jsonString.empty()) {
        return false;
    }
    
    try {
        nlohmann::json::parse(jsonString);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

// YAML parsing implementation  
static bool isValidYaml(const std::string& yamlString) {
    if (yamlString.empty()) {
        return false;
    }
    
    try {
        YAML::Load(yamlString);
        return true;
    } catch (const YAML::ParserException& e) {
        return false;
    }
}

// Convert YAML value to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        const std::string value = node.as<std::string>();
        // Try to detect type
        if (value == "true" || value == "false") {
            return ConfigValue(value == "true");
        }
        
        // Try integer parsing
        try {
            int64_t intVal = std::stoll(value);
            return ConfigValue(intVal);
        } catch (...) {
            // Not an integer
        }
        
        // Try double parsing
        try {
            double doubleVal = std::stod(value);
            return ConfigValue(doubleVal);
        } catch (...) {
            // Not a double
        }
        
        return ConfigValue(value);
    } else if (node.IsSequence()) {
        std::vector<ConfigValue> values;
        for (const auto& element : node) {
            values.push_back(yamlToConfigValue(element));
        }
        
        // Determine sequence type
        bool allInts = std::all_of(values.begin(), values.end(), [](const ConfigValue& val) {
            return std::holds_alternative<int>(val) || std::holds_alternative<int64_t>(val);
        });
        
        if (allInts) {
            std::vector<int> intVec;
            for (const auto& val : values) {
                if (std::holds_alternative<int>(val)) {
                    intVec.push_back(std::get<int>(val));
                } else if (std::holds_alternative<int64_t>(val)) {
                    intVec.push_back(static_cast<int>(std::get<int64_t>(val)));
                }
            }
            return ConfigValue(intVec);
        }
        
        bool allDoubles = std::all_of(values.begin(), values.end(), [](const ConfigValue& val) {
            return std::holds_alternative<double>(val);
        });
        
        if (allDoubles) {
            std::vector<double> doubleVec;
            for (const auto& val : values) {
                if (std::holds_alternative<double>(val)) {
                    doubleVec.push_back(std::get<double>(val));
                }
            }
            return ConfigValue(doubleVec);
        }
        
        bool allStrings = std::all_of(values.begin(), values.end(), [](const ConfigValue& val) {
            return std::holds_alternative<std::string>(val);
        });
        
        if (allStrings) {
            std::vector<std::string> stringVec;
            for (const auto& val : values) {
                if (std::holds_alternative<std::string>(val)) {
                    stringVec.push_back(std::get<std::string>(val));
                }
            }
            return ConfigValue(stringVec);
        }
        
        // Default to int vector
        std::vector<int> defaultVec;
        for (const auto& val : values) {
            if (std::holds_alternative<int>(val)) {
                defaultVec.push_back(std::get<int>(val));
            } else if (std::holds_alternative<int64_t>(val)) {
                defaultVec.push_back(static_cast<int>(std::get<int64_t>(val)));
            }
        }
        return ConfigValue(defaultVec);
    }
    
    return ConfigValue(std::string(""));
}

std::map<std::string, ConfigValue> Config::parseYamlString(const std::string& yamlString) {
    std::map<std::string, ConfigValue> result;
    
    if (!isValidYaml(yamlString)) {
        return result;
    }
    
    try {
        YAML::Node doc = YAML::Load(yamlString);
        
        if (doc.IsMap()) {
            for (const auto& pair : doc) {
                if (pair.first.IsScalar()) {
                    std::string key = pair.first.as<std::string>();
                    result[key] = yamlToConfigValue(pair.second);
                }
            }
        }
    } catch (const YAML::ParserException& e) {
        // Log error
    }
    
    return result;
}

bool Config::importFromYamlString(const std::string& yamlString, ConfigSource source) {
    auto parsedData = parseYamlString(yamlString);
    
    for (const auto& pair : parsedData) {
        // Convert ConfigValue back to appropriate type for setting
        set(pair.first, pair.second, source);
    }
    
    return !parsedData.empty();
}

bool Config::importFromJsonString(const std::string& jsonString, ConfigSource source) {
    auto parsedData = parseJsonString(jsonString);
    
    for (const auto& pair : parsedData) {
        set(pair.first, pair.second, source);
    }
    
    return !parsedData.empty();
}

bool Config::validateConfigValue(const ConfigValue& value, const std::string& expectedType) {
    std::string expectedLower = toLower(expectedType);
    
    if (expectedLower == "int" || expectedLower == "integer") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (expectedLower == "double" || expectedLower == "float") {
        return std::holds_alternative<double>(value);
    } else if (expectedLower == "bool" || expectedLower == "boolean") {
        return std::holds_alternative<bool>(value);
    } else if (expectedLower == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (expectedLower == "int[]" || expectedLower == "int array") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (expectedLower == "double[]" || expectedLower == "double array") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (expectedLower == "string[]" || expectedLower == "string array") {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return false; // Unknown type
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Basic validation - check for empty keys
        if (entry.key.empty()) {
            errors.push_back("Empty key found in configuration");
        }
        
        // Add more specific validation based on config key
        // For example, specific keys might have expected types
    }
    
    return errors;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Create a new entry with the source parameter
        pImpl->entries.push_back(ConfigEntry(entry.key, entry.value, source, entry.description));
    }
}

std::string Config::exportToJsonString() const {
    nlohmann::json jsonObj;
    
    for (const auto& entry : pImpl->entries) {
        jsonObj[entry.key] = configValueToJson(entry.value);
    }
    
    return jsonObj.dump(2); // Pretty print with 2-space indentation
}

// Parse JSON string to config values
std::map<std::string, ConfigValue> Config::parseJsonString(const std::string& jsonString) {
    std::map<std::string, ConfigValue> result;
    
    if (!isValidJson(jsonString)) {
        return result;
    }
    
    try {
        nlohmann::json jsonObj = nlohmann::json::parse(jsonString);
        
        if (jsonObj.is_object()) {
            for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
                result[it.key()] = jsonToConfigValue(it.value());
            }
        }
    } catch (const nlohmann::json::parse_error& e) {
        // Log error
    }
    
    return result;
}

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
