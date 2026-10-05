// Config.cpp implementation
#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <codecvt>
#include <locale>
#include <sstream>

namespace nlm {

using json = nlohmann::json;

// Private implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> variables; // For variable substitution
    std::map<std::string, ConfigSchema> schemas;
    std::string version;
    std::map<std::string, json> templates;
    
    // Default constructor
    Impl() : version("1.0.0") {}
};

// Helper functions for JSON serialization/deserialization
namespace ConfigUtils {
    // Convert ConfigValue to JSON
    json configValueToJSON(const ConfigValue& value) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j = arg;
            } else if constexpr (std::is_same_v<T, std::map<std::string, std::any>>) {
                j = json();
                for (const auto& pair : arg) {
                    j[pair.first] = "[...]"; // Simplified nested object representation
                }
            }
        }, value);
        return j;
    }
    
    // Convert JSON to ConfigValue
    ConfigValue jsonToConfigValue(const json& j) {
        if (j.is_number_integer()) {
            return j.get<int>();
        } else if (j.is_number_unsigned()) {
            return j.get<int64_t>();
        } else if (j.is_number()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            // Determine array type
            if (j.empty()) {
                return std::vector<int>(); // Default to int vector
            }
            
            // Check if all elements are integers
            bool allInts = true;
            for (const auto& elem : j) {
                if (!elem.is_number_integer()) {
                    allInts = false;
                    break;
                }
            }
            if (allInts) {
                return j.get<std::vector<int>>();
            }
            
            // Check if all elements are doubles
            bool allDoubles = true;
            for (const auto& elem : j) {
                if (!elem.is_number()) {
                    allDoubles = false;
                    break;
                }
            }
            if (allDoubles) {
                return j.get<std::vector<double>>();
            }
            
            // Check if all elements are booleans
            bool allBools = true;
            for (const auto& elem : j) {
                if (!elem.is_boolean()) {
                    allBools = false;
                    break;
                }
            }
            if (allBools) {
                return j.get<std::vector<bool>>();
            }
            
            // Default to string vector
            return j.get<std::vector<std::string>>();
        } else if (j.is_object()) {
            std::map<std::string, std::any> result;
            for (const auto& [key, value] : j.items()) {
                result[key] = "[...]"; // Simplified nested object representation
            }
            return result;
        }
        
        // Default case
        return std::string("");
    }
    
    // Convert vector of ConfigEntry to JSON
    json entriesToJSON(const std::vector<ConfigEntry>& entries) {
        json j = json::array();
        for (const auto& entry : entries) {
            json entryJson;
            entryJson["key"] = entry.key;
            entryJson["value"] = configValueToJSON(entry.value);
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            j.push_back(entryJson);
        }
        return j;
    }
    
    // Convert JSON to vector of ConfigEntry
    std::vector<ConfigEntry> jsonToEntries(const json& j, ConfigSource source) {
        std::vector<ConfigEntry> entries;
        for (const auto& entryJson : j) {
            std::string key = entryJson["key"];
            ConfigValue value = jsonToConfigValue(entryJson["value"]);
            std::string description = entryJson.value("description", "");
            ConfigEntry entry(key, value, source, description);
            entries.push_back(entry);
        }
        return entries;
    }
    
    // Check if a JSON value can be converted to a specific type
    template<typename T>
    bool jsonCanConvertToType(const json& j) {
        if constexpr (std::is_same_v<T, int>) {
            return j.is_number_integer();
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return j.is_number_integer();
        } else if constexpr (std::is_same_v<T, double>) {
            return j.is_number();
        } else if constexpr (std::is_same_v<T, bool>) {
            return j.is_boolean();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return j.is_string();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return j.is_array();
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return j.is_array();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return j.is_array();
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return j.is_array();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return j.is_array();
        } else if constexpr (std::is_same_v<T, std::map<std::string, std::any>>) {
            return j.is_object();
        }
        return false;
    }
    
    // Convert JSON to specific type
    template<typename T>
    T jsonToValue(const json& j) {
        if constexpr (std::is_same_v<T, int>) {
            return j.get<int>();
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return j.get<int64_t>();
        } else if constexpr (std::is_same_v<T, double>) {
            return j.get<double>();
        } else if constexpr (std::is_same_v<T, bool>) {
            return j.get<bool>();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return j.get<std::string>();
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return j.get<std::vector<int>>();
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return j.get<std::vector<int64_t>>();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return j.get<std::vector<double>>();
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return j.get<std::vector<bool>>();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return j.get<std::vector<std::string>>();
        } else if constexpr (std::is_same_v<T, std::map<std::string, std::any>>) {
            std::map<std::string, std::any> result;
            for (const auto& item : j.items()) {
                result[item.key()] = "[...]"; // Simplified nested object representation
            }
            return result;
        }
    }
    
    // Check if a string matches a pattern
    bool stringMatchesPattern(const std::string& str, const std::regex& pattern) {
        return std::regex_match(str, pattern);
    }
    
    // Check if a value is within min/max bounds
    template<typename T>
    bool valueWithinBounds(const T& value, double min, double max) {
        if constexpr (std::is_same_v<T, double>) {
            return value >= min && value <= max;
        } else if constexpr (std::is_same_v<T, int>) {
            return static_cast<double>(value) >= min && static_cast<double>(value) <= max;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return static_cast<double>(value) >= min && static_cast<double>(value) <= max;
        } else if constexpr (std::is_same_v<T, bool>) {
            return true; // Boolean values don't have numeric bounds
        }
        return true;
    }
    
    // Check if a value is in allowed values
    bool valueInAllowedValues(const std::any& value, const std::vector<std::any>& allowedValues) {
        for (const auto& allowed : allowedValues) {
            // Compare values based on their types
            if (value.type() == typeid(int) && allowed.type() == typeid(int)) {
                if (std::any_cast<int>(value) == std::any_cast<int>(allowed)) {
                    return true;
                }
            } else if (value.type() == typeid(int64_t) && allowed.type() == typeid(int64_t)) {
                if (std::any_cast<int64_t>(value) == std::any_cast<int64_t>(allowed)) {
                    return true;
                }
            } else if (value.type() == typeid(double) && allowed.type() == typeid(double)) {
                if (std::any_cast<double>(value) == std::any_cast<double>(allowed)) {
                    return true;
                }
            } else if (value.type() == typeid(bool) && allowed.type() == typeid(bool)) {
                if (std::any_cast<bool>(value) == std::any_cast<bool>(allowed)) {
                    return true;
                }
            } else if (value.type() == typeid(std::string) && allowed.type() == typeid(std::string)) {
                if (std::any_cast<std::string>(value) == std::any_cast<std::string>(allowed)) {
                    return true;
                }
            }
            // Add more type comparisons as needed
        }
        return false;
    }
    
    // Convert any type to string
    template<typename T>
    std::string anyToString(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
    
    // Convert string to any type
    template<typename T>
    std::any stringToAny(const std::string& str) {
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(str);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::stoll(str);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(str);
        } else if constexpr (std::is_same_v<T, bool>) {
            return (str == "true" || str == "1");
        } else if constexpr (std::is_same_v<T, std::string>) {
            return str;
        }
        return std::any();
    }
    
    // Parse nested path
    std::vector<std::string> parseNestedPath(const std::string& path) {
        std::vector<std::string> result;
        std::string token;
        std::istringstream stream(path);
        
        while (std::getline(stream, token, '.')) {
            if (!token.empty()) {
                result.push_back(token);
            }
        }
        return result;
    }
    
    // Get nested value from JSON
    json getNestedJSON(const json& j, const std::vector<std::string>& path) {
        json result = j;
        for (const auto& key : path) {
            if (result.is_object() && result.contains(key)) {
                result = result[key];
            } else {
                return json(nullptr); // Key not found
            }
        }
        return result;
    }
    
    // Set nested value in JSON
    void setNestedJSON(json& j, const std::vector<std::string>& path, const json& value) {
        if (path.empty()) return;
        
        json* current = &j;
        for (size_t i = 0; i < path.size() - 1; ++i) {
            if (!current->is_object()) {
                current->clear();
                current->push_back(json::object());
            }
            if (!current->contains(path[i])) {
                (*current)[path[i]] = json::object();
            }
            current = &((*current)[path[i]]);
        }
        
        if (!current->is_object()) {
            current->clear();
            current->push_back(json::object());
        }
        (*current)[path.back()] = value;
    }
    
    // Convert ConfigEntry to JSON
    json entryToJSON(const ConfigEntry& entry) {
        json j;
        j["key"] = entry.key;
        j["value"] = configValueToJSON(entry.value);
        j["source"] = static_cast<int>(entry.source);
        j["description"] = entry.description;
        return j;
    }
    
    // Convert JSON to ConfigEntry
    ConfigEntry jsonToEntry(const json& j) {
        std::string key = j.value("key", "");
        ConfigValue value = jsonToConfigValue(j.value("value", json(nullptr)));
        ConfigSource source = static_cast<ConfigSource>(j.value("source", static_cast<int>(ConfigSource::File)));
        std::string description = j.value("description", "");
        return ConfigEntry(key, value, source, description);
    }
}

// ConfigSchema implementation
bool ConfigSchema::validate(const ConfigValue& value) const {
    // Implementation for schema validation
    return true;
}

std::string ConfigSchema::getTypeName(const std::type_info* type) const {
    if (type == &typeid(int)) return "int";
    if (type == &typeid(int64_t)) return "int64_t";
    if (type == &typeid(double)) return "double";
    if (type == &typeid(bool)) return "bool";
    if (type == &typeid(std::string)) return "string";
    if (type == &typeid(std::vector<int>)) return "vector<int>";
    if (type == &typeid(std::vector<int64_t>)) return "vector<int64_t>";
    if (type == &typeid(std::vector<double>)) return "vector<double>";
    if (type == &typeid(std::vector<bool>)) return "vector<bool>";
    if (type == &typeid(std::vector<std::string>)) return "vector<string>";
    if (type == &typeid(std::map<std::string, std::any>)) return "map<string, any>";
    return "unknown";
}

bool ConfigSchema::isTypeCompatible(const std::type_info* type, const std::any& value) const {
    // Implementation for type compatibility check
    return true;
}

// Config implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    // Check if it's a config file
    if (!isConfigFile(filepath)) {
        return false;
    }
    
    // Determine file source
    ConfigSource fileSource = detectFileSource(filepath);
    
    // Try to parse based on file extension
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonStr = buffer.str();
        
        return loadFromJSON(jsonStr, fileSource);
    } else if (ext == "toml" || ext == "cfg") {
        // Simple key=value parser for backward compatibility
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
                
                // Resolve variables in value
                value = resolveVariables(value);
                
                set(key, value, fileSource);
            }
        }
        
        return true;
    }
    
    return false;
}

bool Config::loadFromJSON(const std::string& jsonStr, ConfigSource source) {
    try {
        json j = json::parse(jsonStr);
        
        // Clear existing entries
        clear();
        
        // Convert JSON to Config entries
        auto entries = ConfigUtils::jsonToEntries(j, source);
        
        // Add all entries
        for (const auto& entry : entries) {
            pImpl->entries.push_back(entry);
        }
        
        return true;
    } catch (const json::parse_error& e) {
        // Log error
        return false;
    }
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    // Implementation for loading from environment variables
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
                
                // Resolve variables in value
                value = resolveVariables(value);
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Resolve variables in value
            value = resolveVariables(value);
            
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
    
    // Convert entries to JSON
    json j = ConfigUtils::entriesToJSON(pImpl->entries);
    
    // Write JSON to file
    file << j.dump(4);
    
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

template<typename T>
std::optional<T> Config::getNested(const std::vector<std::string>& path) const {
    // Implementation for getting nested values
    return std::nullopt;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
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

void Config::setNested(const std::vector<std::string>& path, const ConfigValue& value, ConfigSource source) {
    // Implementation for setting nested values
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

void Config::removeNested(const std::vector<std::string>& path) {
    // Implementation for removing nested values
}

bool Config::merge(const Config& other, bool overrideExisting) {
    // Implementation for merging configurations
    return true;
}

bool Config::validate(const ConfigSchema& schema) const {
    // Implementation for schema validation
    return true;
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.name] = schema;
}

const std::vector<ConfigSchema>& Config::getSchemas() const {
    // Implementation for getting schemas
    static std::vector<ConfigSchema> empty;
    return empty;
}

void Config::setVariable(const std::string& name, const std::string& value) {
    pImpl->variables[name] = value;
}

std::string Config::resolveVariables(const std::string& str) const {
    return resolveVariable(str, pImpl->variables);
}

bool Config::reloadFromFile(const std::string& filepath) {
    // Implementation for reloading from file
    return true;
}

std::chrono::system_clock::time_point Config::getLastModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->lastModified;
    }
    
    return std::chrono::system_clock::time_point();
}

bool Config::applyTemplate(const std::string& templateName, const std::map<std::string, std::string>& variables) {
    // Implementation for applying templates
    return true;
}

bool Config::saveTemplate(const std::string& templateName, const std::string& filepath) const {
    // Implementation for saving templates
    return true;
}

std::string Config::getVersion() const {
    return pImpl->version;
}

bool Config::upgrade(const std::string& fromVersion, const std::string& toVersion) {
    // Implementation for version upgrade
    return true;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->variables.clear();
    pImpl->schemas.clear();
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
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<int64_t>> || 
                                 std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<bool>> || 
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << item << "\"";
                    } else {
                        oss << item;
                    }
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::map<std::string, std::any>>) {
                oss << "{";
                bool first = true;
                for (const auto& pair : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << pair.first << "\": ";
                    // Simplified handling for nested objects
                    oss << "[...]";
                    first = false;
                }
                oss << "}";
            } else {
                oss << "unknown_type";
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

std::string Config::extractKeyFromArg(const std::string& arg) {
    // Implementation for extracting key from argument
    return "";
}

std::string Config::extractValueFromArg(const std::string& arg) {
    // Implementation for extracting value from argument
    return "";
}

bool Config::isConfigFile(const std::string& filepath) {
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path)) {
        return false;
    }
    
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == ".json" || ext == ".toml" || ext == ".cfg";
}

ConfigSource Config::detectFileSource(const std::string& filepath) {
    // Implementation for detecting file source
    return ConfigSource::File;
}

std::string Config::resolveVariable(const std::string& str, const std::map<std::string, std::string>& variables) {
    std::string result = str;
    
    // Simple variable substitution: ${VAR}
    size_t pos = result.find("${");
    while (pos != std::string::npos) {
        size_t endPos = result.find('}', pos);
        if (endPos == std::string::npos) {
            break;
        }
        
        std::string varName = result.substr(pos + 2, endPos - pos - 2);
        auto it = variables.find(varName);
        if (it != variables.end()) {
            result.replace(pos, endPos - pos + 1, it->second);
        } else {
            // Variable not found, leave as is
            result.replace(pos, endPos - pos + 1, "");
        }
        
        pos = result.find("${", pos);
    }
    
    return result;
}

// Explicit template instantiations
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm