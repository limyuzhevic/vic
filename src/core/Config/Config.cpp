#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigVersion version;
    std::unordered_map<std::string, std::string> descriptions;
};

// ConfigSchema implementation
void ConfigSchema::addEntry(const SchemaEntry& entry) {
    entries[entry.key] = entry;
}

const ConfigSchema::SchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = entries.find(key);
    return it != entries.end() ? &it->second : nullptr;
}

bool ConfigSchema::hasEntry(const std::string& key) const {
    return entries.find(key) != entries.end();
}

std::vector<ConfigValidationError> ConfigSchema::validate(const Config& config) const {
    std::vector<ConfigValidationError> errors;
    
    for (const auto& pair : entries) {
        const auto& entry = pair.second;
        
        if (entry.required && !config.has(entry.key)) {
            errors.emplace_back(
                ConfigValidationError::ErrorType::MissingRequired,
                entry.key,
                "Required configuration parameter '" + entry.key + "' is missing"
            );
        }
        
        if (config.has(entry.key)) {
            auto value = config.getRaw(entry.key);
            if (!value.has_value()) {
                errors.emplace_back(
                    ConfigValidationError::ErrorType::TypeMismatch,
                    entry.key,
                    "Cannot parse configuration value for '" + entry.key + "'"
                );
                continue;
            }
            
            if (!validateType(entry.key, value.value(), entry.type)) {
                errors.emplace_back(
                    ConfigValidationError::ErrorType::TypeMismatch,
                    entry.key,
                    "Type mismatch for configuration parameter '" + entry.key + "'"
                );
            }
            
            if (!validateConstraints(entry.key, value.value(), *this)) {
                errors.emplace_back(
                    ConfigValidationError::ErrorType::SchemaMismatch,
                    entry.key,
                    "Schema validation failed for configuration parameter '" + entry.key + "'"
                );
            }
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> required;
    for (const auto& pair : entries) {
        if (pair.second.required) {
            required.push_back(pair.first);
        }
    }
    return required;
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries.size());
    for (const auto& pair : entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigSchema::clear() {
    entries.clear();
}

std::string ConfigSchema::exportAsJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& pair : entries) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << escapeJSON(pair.first) << "\": {\n";
        const auto& entry = pair.second;
        oss << "    \"type\": \"";
        
        switch (entry.type) {
            case SchemaType::Integer: oss << "integer"; break;
            case SchemaType::Double: oss << "double"; break;
            case SchemaType::Boolean: oss << "boolean"; break;
            case SchemaType::String: oss << "string"; break;
            case SchemaType::IntegerArray: oss << "integer_array"; break;
            case SchemaType::DoubleArray: oss << "double_array"; break;
            case SchemaType::StringArray: oss << "string_array"; break;
            case SchemaType::Object: oss << "object"; break;
        }
        
        oss << "\",\n";
        oss << "    \"required\": " << (entry.required ? "true" : "false") << ",\n";
        if (!entry.description.empty()) {
            oss << "    \"description\": \"" << escapeJSON(entry.description) << "\"\n";
        }
        if (!entry.defaultValue.empty()) {
            if (!entry.description.empty()) oss << ",\n";
            oss << "    \"default\": \"" << escapeJSON(entry.defaultValue) << "\"\n";
        }
        oss << "  }";
    }
    
    oss << "\n}\n";
    return oss.str();
}

bool ConfigSchema::importFromJSON(const std::string& json) {
    // Simple JSON parsing for schema import
    // In production, use a proper JSON library
    
    size_t pos = 0;
    std::string key;
    bool inKey = false;
    bool inValue = false;
    bool inBraces = false;
    bool inQuotes = false;
    std::string currentKey;
    std::string currentValue;
    
    for (char c : json) {
        if (c == '{' && !inBraces) {
            inBraces = true;
            continue;
        }
        if (c == '}' && inBraces) {
            inBraces = false;
            continue;
        }
        if (c == '\"') {
            inQuotes = !inQuotes;
            if (!inQuotes && !currentKey.empty()) {
                key = currentKey;
                currentKey.clear();
            }
            continue;
        }
        if (!inQuotes && c == ':') {
            inValue = true;
            continue;
        }
        if (!inQuotes && c == ',') {
            inValue = false;
            // Parse entry based on key
            if (key == "type") {
                if (currentValue == "integer") entries[key].type = SchemaType::Integer;
                else if (currentValue == "double") entries[key].type = SchemaType::Double;
                else if (currentValue == "boolean") entries[key].type = SchemaType::Boolean;
                else if (currentValue == "string") entries[key].type = SchemaType::String;
                else if (currentValue == "integer_array") entries[key].type = SchemaType::IntegerArray;
                else if (currentValue == "double_array") entries[key].type = SchemaType::DoubleArray;
                else if (currentValue == "string_array") entries[key].type = SchemaType::StringArray;
                else if (currentValue == "object") entries[key].type = SchemaType::Object;
            } else if (key == "required") {
                entries[key].required = (currentValue == "true");
            }
            currentValue.clear();
            continue;
        }
        
        if (inQuotes) {
            if (inValue) currentValue += c;
            else currentKey += c;
        }
    }
    
    return true;
}

// Config implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    std::ifstream jsonFile(filepath);
    if (jsonFile.is_open()) {
        std::stringstream buffer;
        buffer << jsonFile.rdbuf();
        std::string json = buffer.str();
        jsonFile.close();
        
        return loadFromJSON(json);
    }
    
    // Try YAML
    std::ifstream yamlFile(filepath);
    if (yamlFile.is_open()) {
        std::stringstream buffer;
        buffer << yamlFile.rdbuf();
        std::string yaml = buffer.str();
        yamlFile.close();
        
        return loadFromYAML(yaml);
    }
    
    // Fall back to simple key=value format for backward compatibility
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
            
            auto configValue = stringToConfigValue(value);
            if (configValue.has_value()) {
                set(key, configValue.value(), ConfigSource::File);
                
                // Store description if present
                if (line.find('#') != std::string::npos) {
                    size_t commentPos = line.find('#');
                    if (commentPos != std::string::npos) {
                        std::string comment = trim(line.substr(commentPos + 1));
                        pImpl->descriptions[key] = comment;
                    }
                }
            }
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& json) {
    std::unordered_map<std::string, ConfigValue> data;
    auto result = parseJSON(json, data);
    
    if (!result.success()) {
        return false;
    }
    
    clear();
    
    for (const auto& pair : data) {
        set(pair.first, pair.second, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& yaml) {
    std::unordered_map<std::string, ConfigValue> data;
    auto result = parseYAML(yaml, data);
    
    if (!result.success()) {
        return false;
    }
    
    clear();
    
    for (const auto& pair : data) {
        set(pair.first, pair.second, ConfigSource::File);
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
                auto configValue = stringToConfigValue(value);
                if (configValue.has_value()) {
                    set(key, configValue.value(), ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            auto configValue = stringToConfigValue(value);
            if (configValue.has_value()) {
                set(key, configValue.value(), ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine format based on file extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext == ".json" || ext == ".JSON") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << saveToJSON();
        return true;
    }
    else if (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML") {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << saveToYAML();
        return true;
    }
    else {
        // Default to INI format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << saveToINI();
        return true;
    }
}

std::string Config::saveToJSON() const {
    return serializeToJSON(convertToMap());
}

std::string Config::saveToYAML() const {
    return serializeToYAML(convertToMap());
}

std::string Config::saveToINI() const {
    return serializeINI(pImpl->entries);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto value = getRaw(key);
    if (!value.has_value()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(value.value());
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::optional<ConfigValue> Config::getRaw(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
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
    auto configValue = stringToConfigValue(value);
    if (configValue.has_value()) {
        set(key, configValue.value(), source);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
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
    pImpl->descriptions.erase(key);
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
    pImpl->descriptions.clear();
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
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, double> || 
                                 std::is_same_v<T, bool>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
                }
                oss << "]";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::vector<ConfigValidationError> Config::validateAgainstSchema(const ConfigSchema& schema) const {
    return schema.validate(*this);
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& value) {
    // Try to parse as boolean
    std::string lowerValue = toLower(value);
    if (lowerValue == "true" || lowerValue == "yes" || lowerValue == "1" || lowerValue == "on") {
        return true;
    }
    if (lowerValue == "false" || lowerValue == "no" || lowerValue == "0" || lowerValue == "off") {
        return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(value, &pos);
        if (pos == value.size()) {
            return intValue;
        }
    } catch (...) {}
    
    // Try to parse as int64
    try {
        size_t pos;
        int64_t int64Value = std::stoll(value, &pos);
        if (pos == value.size()) {
            return int64Value;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(value, &pos);
        if (pos == value.size()) {
            return doubleValue;
        }
    } catch (...) {}
    
    // Check if it's an array (starts with [ and ends with ])
    if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
        std::string inner = value.substr(1, value.size() - 2);
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        // Try to parse as integer array
        std::stringstream ss(inner);
        std::string token;
        bool allInts = true;
        bool allStrings = true;
        
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            
            // Check if it's an integer
            try {
                size_t pos;
                int intVal = std::stoi(token, &pos);
                if (pos != token.size()) allInts = false;
                else intArray.push_back(intVal);
            } catch (...) {
                allInts = false;
            }
            
            // Check if it's a string
            if (!token.empty() && 
                ((token.front() == '\"' && token.back() == '\"') ||
                 (token.front() == '\'' && token.back() == '\''))) {
                token = token.substr(1, token.size() - 2);
            }
            
            stringArray.push_back(token);
        }
        
        if (allInts && !intArray.empty()) {
            return intArray;
        }
        
        // Try to parse as double array
        ss.clear();
        ss.str(inner);
        allInts = true;
        allStrings = true;
        doubleArray.clear();
        
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            
            try {
                size_t pos;
                double doubleVal = std::stod(token, &pos);
                if (pos != token.size()) allInts = false;
                else doubleArray.push_back(doubleVal);
            } catch (...) {
                allInts = false;
            }
            
            if (!token.empty() && 
                ((token.front() == '\"' && token.back() == '\"') ||
                 (token.front() == '\'' && token.back() == '\''))) {
                token = token.substr(1, token.size() - 2);
            }
            
            stringArray.push_back(token);
        }
        
        if (!doubleArray.empty()) {
            return doubleArray;
        }
        
        if (!stringArray.empty()) {
            return stringArray;
        }
    }
    
    // Default to string
    return value;
}

template<typename T>
std::string Config::configValueToString(const ConfigValue& value) {
    try {
        const T& val = std::get<T>(value);
        std::ostringstream oss;
        oss << val;
        return oss.str();
    } catch (const std::bad_variant_access&) {
        return "";
    }
}

ParseResult Config::parseJSON(const std::string& json, std::unordered_map<std::string, ConfigValue>& out) {
    size_t pos = 0;
    return parseJSONInternal(json, out, pos);
}

ParseResult Config::parseYAML(const std::string& yaml, std::unordered_map<std::string, ConfigValue>& out) {
    size_t pos = 0;
    return parseYAMLInternal(yaml, out, pos);
}

std::string Config::serializeToJSON(const std::unordered_map<std::string, ConfigValue>& data) const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << escapeJSON(pair.first) << "\": " << valueToJSON(pair.second);
    }
    
    oss << "\n}\n";
    return oss.str();
}

std::string Config::serializeToYAML(const std::unordered_map<std::string, ConfigValue>& data) const {
    std::ostringstream oss;
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << "\n";
        first = false;
        
        oss << pair.first << ": " << valueToYAML(pair.second);
    }
    
    return oss.str();
}

std::string Config::serializeINI(const std::vector<ConfigEntry>& entries) const {
    std::ostringstream oss;
    for (const auto& entry : entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << " = " << configValueToString(entry.value) << "\n\n";
    }
    return oss.str();
}

void Config::setVersion(const ConfigVersion& version) {
    pImpl->version = version;
}

ConfigVersion Config::getVersion() const {
    return pImpl->version;
}

std::string Config::getVersionString() const {
    return pImpl->version.toString();
}

bool Config::isValidType(const std::string& key, const ConfigValue& value) const {
    // Basic type validation - can be extended based on schema
    return true;
}

bool Config::isValidRange(const std::string& key, const ConfigValue& value) const {
    // Range validation - can be extended based on schema
    return true;
}

bool Config::isValidLength(const std::string& key, const ConfigValue& value) const {
    // Length validation for arrays and strings
    return true;
}

bool Config::isValidPattern(const std::string& key, const ConfigValue& value) const {
    // Pattern validation - can be extended based on schema
    return true;
}

std::string Config::exportAsJSON() const {
    return saveToJSON();
}

std::string Config::exportAsYAML() const {
    return saveToYAML();
}

std::string Config::exportAsINI() const {
    return saveToINI();
}

bool Config::importFromJSON(const std::string& json) {
    return loadFromJSON(json);
}

bool Config::importFromYAML(const std::string& yaml) {
    return loadFromYAML(yaml);
}

bool Config::importFromINI(const std::string& ini) {
    // Parse INI format
    std::unordered_map<std::string, std::string> lines;
    size_t start = 0;
    size_t end;
    
    while ((end = ini.find('\n', start)) != std::string::npos) {
        std::string line = trim(ini.substr(start, end - start));
        start = end + 1;
        
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            lines[key] = value;
        }
    }
    
    clear();
    
    for (const auto& pair : lines) {
        auto configValue = stringToConfigValue(pair.second);
        if (configValue.has_value()) {
            set(pair.first, configValue.value(), ConfigSource::File);
        }
    }
    
    return true;
}

void Config::mergeFrom(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Override existing entries
        set(entry.key, entry.value, source);
    }
}

void Config::inheritFrom(const Config& parent, const std::vector<std::string>& path) {
    std::string prefix = path.empty() ? "" : path[0] + ".";
    
    for (const auto& entry : parent.pImpl->entries) {
        std::string key = prefix + entry.key;
        
        // Check if we already have this key
        if (!has(key)) {
            set(key, entry.value, entry.source);
            
            // Also copy the description
            auto descIt = parent.pImpl->descriptions.find(entry.key);
            if (descIt != parent.pImpl->descriptions.end()) {
                pImpl->descriptions[key] = descIt->second;
            }
        }
    }
}

std::string Config::generateDocumentation() const {
    std::ostringstream oss;
    oss << "# Configuration Documentation\n\n";
    oss << "Version: " << getVersionString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "## " << entry.key << "\n\n";
        oss << "**Type:** " << getTypeName(entry.value) << "\n";
        oss << "**Source:** " << static_cast<int>(entry.source) << "\n";
        
        auto descIt = pImpl->descriptions.find(entry.key);
        if (descIt != pImpl->descriptions.end()) {
            oss << "**Description:** " << descIt->second << "\n";
        }
        
        oss << "**Value:** " << configValueToString(entry.value) << "\n\n";
    }
    
    return oss.str();
}

std::string Config::getDocumentation(const std::string& key) const {
    std::ostringstream oss;
    
    if (!has(key)) {
        return "Key not found: " + key;
    }
    
    auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry == pImpl->entries.end()) {
        return "Key not found: " + key;
    }
    
    oss << "# " << key << "\n\n";
    oss << "**Type:** " << getTypeName(entry->value) << "\n";
    oss << "**Source:** " << static_cast<int>(entry->source) << "\n";
    
    auto descIt = pImpl->descriptions.find(key);
    if (descIt != pImpl->descriptions.end()) {
        oss << "**Description:** " << descIt->second << "\n\n";
    }
    
    oss << "**Value:** " << configValueToString(entry->value) << "\n";
    
    return oss.str();
}

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    std::unordered_map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "  By source:\n";
    for (const auto& pair : sourceCounts) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    pImpl->descriptions[key] = description;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->descriptions.find(key);
    if (it != pImpl->descriptions.end()) {
        return it->second;
    }
    return "";
}

// Private implementation methods
std::unordered_map<std::string, ConfigValue> Config::convertToMap() const {
    std::unordered_map<std::string, ConfigValue> result;
    for (const auto& entry : pImpl->entries) {
        result[entry.key] = entry.value;
    }
    return result;
}

std::string Config::getTypeName(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "int[]";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "double[]";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "string[]";
    }, value);
    return typeName;
}

std::string Config::valueToJSON(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeJSON(arg) << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

std::string Config::valueToYAML(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeYAML(arg) << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                             std::is_same_v<T, std::vector<double>> || 
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; // YAML array
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                first = false;
                
                if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << item;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << item;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "\"" << escapeYAML(item) << "\"";
                }
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

ParseResult Config::parseJSONInternal(const std::string& json, std::unordered_map<std::string, ConfigValue>& out, size_t& pos) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return ParseResult("Empty JSON");
    }
    
    if (json[pos] == '{') {
        return parseJSONObject(json, out, ++pos);
    } else if (json[pos] == '[') {
        std::vector<ConfigValue> array;
        if (parseJSONArray(json, array, ++pos).success()) {
            // For now, treat arrays as single values
            if (!array.empty()) {
                out["array"] = array[0];
            }
            return ParseResult();
        } else {
            return ParseResult("Failed to parse JSON array");
        }
    } else {
        ConfigValue value;
        ParseResult result = parseJSONValue(json, value, pos);
        if (result.success()) {
            // For primitive values, create a default key
            out["value"] = value;
        }
        return result;
    }
}

ParseResult Config::parseJSONObject(const std::string& json, std::unordered_map<std::string, ConfigValue>& out, size_t& pos) {
    while (pos < json.size() && json[pos] != '}') {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) break;
        
        if (json[pos] == '}') break;
        
        // Parse key
        std::string key;
        ParseResult keyResult = parseJSONString(json, key, ++pos);
        if (!keyResult.success()) {
            return keyResult;
        }
        
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != ':') {
            return ParseResult("Expected ':' after key");
        }
        
        ++pos;
        skipWhitespace(json, pos);
        
        // Parse value
        ConfigValue value;
        ParseResult valueResult = parseJSONValue(json, value, pos);
        if (!valueResult.success()) {
            return valueResult;
        }
        
        out[key] = value;
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.size() && json[pos] == '}') {
            break;
        }
    }
    
    if (pos < json.size() && json[pos] == '}') {
        ++pos;
    }
    
    return ParseResult();
}

ParseResult Config::parseJSONArray(const std::string& json, std::vector<ConfigValue>& out, size_t& pos) {
    while (pos < json.size() && json[pos] != ']') {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) break;
        
        if (json[pos] == ']') break;
        
        ConfigValue value;
        ParseResult result = parseJSONValue(json, value, pos);
        if (!result.success()) {
            return result;
        }
        
        out.push_back(value);
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.size() && json[pos] == ']') {
            break;
        }
    }
    
    if (pos < json.size() && json[pos] == ']') {
        ++pos;
    }
    
    return ParseResult();
}

ParseResult Config::parseJSONString(const std::string& json, std::string& out, size_t& pos) {
    if (pos >= json.size() || json[pos] != '\"') {
        return ParseResult("Expected string quote");
    }
    
    ++pos;
    size_t start = pos;
    
    while (pos < json.size() && json[pos] != '\"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            ++pos; // Skip escaped character
        }
        ++pos;
    }
    
    if (pos >= json.size()) {
        return ParseResult("Unterminated string");
    }
    
    out = unescapeJSON(json.substr(start, pos - start));
    ++pos; // Skip closing quote
    return ParseResult();
}

ParseResult Config::parseJSONNumber(const std::string& json, ConfigValue& out, size_t& pos) {
    size_t start = pos;
    
    // Check for negative
    if (json[pos] == '-') {
        ++pos;
    }
    
    // Parse integer part
    while (pos < json.size() && std::isdigit(json[pos])) {
        ++pos;
    }
    
    if (pos > start) {
        // Try to parse as int
        try {
            int intVal = std::stoi(json.substr(start, pos - start));
            out = intVal;
            return ParseResult();
        } catch (...) {}
        
        // Try to parse as double
        try {
            double doubleVal = std::stod(json.substr(start, pos - start));
            out = doubleVal;
            return ParseResult();
        } catch (...) {}
    }
    
    return ParseResult("Invalid number");
}

ParseResult Config::parseJSONBoolean(const std::string& json, ConfigValue& out, size_t& pos) {
    if (json.compare(pos, 4, "true") == 0) {
        out = true;
        pos += 4;
        return ParseResult();
    } else if (json.compare(pos, 5, "false") == 0) {
        out = false;
        pos += 5;
        return ParseResult();
    }
    
    return ParseResult("Invalid boolean");
}

ParseResult Config::parseJSONNull(const std::string& json, ConfigValue& out, size_t& pos) {
    if (json.compare(pos, 4, "null") == 0) {
        // Set to empty string as default
        out = std::string();
        pos += 4;
        return ParseResult();
    }
    
    return ParseResult("Invalid null");
}

ParseResult Config::parseJSONValue(const std::string& json, ConfigValue& out, size_t& pos) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return ParseResult("Unexpected end of JSON");
    }
    
    switch (json[pos]) {
        case '\"': {
            std::string str;
            ParseResult result = parseJSONString(json, str, pos);
            if (result.success()) {
                out = str;
            }
            return result;
        }
        case '{': {
            std::unordered_map<std::string, ConfigValue> obj;
            ParseResult result = parseJSONObject(json, obj, ++pos);
            if (result.success()) {
                // For object, use first key-value pair
                if (!obj.empty()) {
                    auto it = obj.begin();
                    out = it->second;
                }
            }
            return result;
        }
        case '[': {
            std::vector<ConfigValue> arr;
            ParseResult result = parseJSONArray(json, arr, ++pos);
            if (result.success()) {
                // For array, use first element
                if (!arr.empty()) {
                    out = arr[0];
                }
            }
            return result;
        }
        case 't': case 'f': case 'n': {
            if (json.compare(pos, 4, "true") == 0) {
                return parseJSONBoolean(json, out, pos);
            } else if (json.compare(pos, 5, "false") == 0) {
                return parseJSONBoolean(json, out, pos);
            } else if (json.compare(pos, 4, "null") == 0) {
                return parseJSONNull(json, out, pos);
            }
            break;
        }
        default: {
            if (std::isdigit(json[pos]) || json[pos] == '-') {
                return parseJSONNumber(json, out, pos);
            }
            break;
        }
    }
    
    return ParseResult("Invalid JSON value");
}

ParseResult Config::parseYAMLInternal(const std::string& yaml, std::unordered_map<std::string, ConfigValue>& out, size_t& pos) {
    skipWhitespace(yaml, pos);
    
    if (pos >= yaml.size()) {
        return ParseResult("Empty YAML");
    }
    
    if (yaml[pos] == '{') {
        return parseYAMLKeyValue(yaml, std::make_pair("root", ConfigValue()), ++pos);
    } else {
        // Try to parse as key-value pair
        std::pair<std::string, ConfigValue> kv;
        ParseResult result = parseYAMLKeyValue(yaml, kv, pos);
        if (result.success()) {
            out[kv.first] = kv.second;
        }
        return result;
    }
}

ParseResult Config::parseYAMLDocument(const std::string& yaml, std::unordered_map<std::string, ConfigValue>& out, size_t& pos) {
    while (pos < yaml.size() && yaml[pos] != '%' && yaml[pos] != '---') {
        skipWhitespace(yaml, pos);
        
        if (pos >= yaml.size()) break;
        
        std::pair<std::string, ConfigValue> kv;
        ParseResult result = parseYAMLKeyValue(yaml, kv, pos);
        if (!result.success()) {
            break;
        }
        
        out[kv.first] = kv.second;
        
        skipWhitespace(yaml, pos);
        
        if (pos < yaml.size() && (yaml[pos] == '\n' || yaml[pos] == '\r')) {
            while (pos < yaml.size() && (yaml[pos] == '\n' || yaml[pos] == '\r')) {
                ++pos;
            }
        }
    }
    
    return ParseResult();
}

ParseResult Config::parseYAMLKeyValue(const std::string& yaml, std::pair<std::string, ConfigValue>& out, size_t& pos) {
    skipWhitespace(yaml, pos);
    
    if (pos >= yaml.size()) {
        return ParseResult("Empty YAML key-value");
    }
    
    // Parse key
    std::string key;
    ParseResult keyResult = parseYAMLString(yaml, key, pos);
    if (!keyResult.success()) {
        return keyResult;
    }
    
    skipWhitespace(yaml, pos);
    
    if (pos >= yaml.size() || yaml[pos] != ':') {
        return ParseResult("Expected ':' after YAML key");
    }
    
    ++pos;
    skipWhitespace(yaml, pos);
    
    // Parse value
    ParseResult valueResult = parseYAMLScalar(yaml, out.second, pos);
    if (!valueResult.success()) {
        return valueResult;
    }
    
    out.first = key;
    return ParseResult();
}

ParseResult Config::parseYAMLString(const std::string& yaml, std::string& out, size_t& pos) {
    if (pos >= yaml.size()) {
        return ParseResult("Empty YAML string");
    }
    
    if (yaml[pos] == '\"' || yaml[pos] == '\'') {
        char quote = yaml[pos];
        ++pos;
        size_t start = pos;
        
        while (pos < yaml.size() && yaml[pos] != quote) {
            if (yaml[pos] == '\\' && pos + 1 < yaml.size()) {
                ++pos; // Skip escaped character
            }
            ++pos;
        }
        
        if (pos >= yaml.size()) {
            return ParseResult("Unterminated YAML string");
        }
        
        out = unescapeYAML(yaml.substr(start, pos - start));
        ++pos; // Skip closing quote
        return ParseResult();
    }
    
    // Plain scalar (until whitespace or colon)
    size_t start = pos;
    while (pos < yaml.size() && yaml[pos] != '\n' && yaml[pos] != '\r' && yaml[pos] != ':' && !std::isspace(yaml[pos])) {
        ++pos;
    }
    
    if (pos > start) {
        out = yaml.substr(start, pos - start);
        return ParseResult();
    }
    
    return ParseResult("Empty YAML string");
}

ParseResult Config::parseYAMLNumber(const std::string& yaml, ConfigValue& out, size_t& pos) {
    size_t start = pos;
    
    // Check for negative
    if (yaml[pos] == '-') {
        ++pos;
    }
    
    // Parse integer part
    while (pos < yaml.size() && std::isdigit(yaml[pos])) {
        ++pos;
    }
    
    if (pos > start) {
        // Try to parse as int
        try {
            int intVal = std::stoi(yaml.substr(start, pos - start));
            out = intVal;
            return ParseResult();
        } catch (...) {}
        
        // Try to parse as double
        try {
            double doubleVal = std::stod(yaml.substr(start, pos - start));
            out = doubleVal;
            return ParseResult();
        } catch (...) {}
    }
    
    return ParseResult("Invalid YAML number");
}

ParseResult Config::parseYAMLScalar(const std::string& yaml, ConfigValue& out, size_t& pos) {
    skipWhitespace(yaml, pos);
    
    if (pos >= yaml.size()) {
        return ParseResult("Empty YAML scalar");
    }
    
    switch (yaml[pos]) {
        case '\"': case '\'': {
            std::string str;
            ParseResult result = parseYAMLString(yaml, str, pos);
            if (result.success()) {
                out = str;
            }
            return result;
        }
        default: {
            if (std::isdigit(yaml[pos]) || yaml[pos] == '-') {
                return parseYAMLNumber(yaml, out, pos);
            } else if (yaml.compare(pos, 4, "true") == 0) {
                out = true;
                pos += 4;
                return ParseResult();
            } else if (yaml.compare(pos, 5, "false") == 0) {
                out = false;
                pos += 5;
                return ParseResult();
            } else {
                // Plain scalar
                size_t start = pos;
                while (pos < yaml.size() && yaml[pos] != '\n' && yaml[pos] != '\r' && yaml[pos] != ':' && !std::isspace(yaml[pos])) {
                    ++pos;
                }
                
                if (pos > start) {
                    std::string scalar = yaml.substr(start, pos - start);
                    // Try to parse as number
                    try {
                        size_t pos2;
                        int intVal = std::stoi(scalar, &pos2);
                        if (pos2 == scalar.size()) {
                            out = intVal;
                            return ParseResult();
                        }
                        
                        double doubleVal = std::stod(scalar, &pos2);
                        if (pos2 == scalar.size()) {
                            out = doubleVal;
                            return ParseResult();
                        }
                    } catch (...) {}
                    
                    // Default to string
                    out = scalar;
                    return ParseResult();
                }
                
                return ParseResult("Empty YAML scalar");
            }
        }
    }
}

bool Config::validateType(const std::string& key, const ConfigValue& value, SchemaType type) {
    switch (type) {
        case SchemaType::Integer:
            return std::holds_alternative<int>(value);
        case SchemaType::Double:
            return std::holds_alternative<int>(value) || std::holds_alternative<double>(value);
        case SchemaType::Boolean:
            return std::holds_alternative<bool>(value);
        case SchemaType::String:
            return std::holds_alternative<std::string>(value);
        case SchemaType::IntegerArray:
            return std::holds_alternative<std::vector<int>>(value);
        case SchemaType::DoubleArray:
            return std::holds_alternative<std::vector<double>>(value);
        case SchemaType::StringArray:
            return std::holds_alternative<std::vector<std::string>>(value);
        case SchemaType::Object:
            return false; // Objects not directly supported in ConfigValue
    }
    return false;
}

bool Config::validateConstraints(const std::string& key, const ConfigValue& value, const ConfigSchema& schema) {
    // Get schema entry
    const ConfigSchema::SchemaEntry* entry = schema.getEntry(key);
    if (!entry) {
        return true; // No constraints for keys not in schema
    }
    
    // Check constraints
    for (const auto& rule : entry->rules) {
        switch (rule.type) {
            case SchemaValidationRule::ConstraintType::MinValue: {
                if (auto val = std::get_if<int>(&value)) {
                    try {
                        int minVal = std::stoi(rule.value);
                        if (*val < minVal) return false;
                    } catch (...) {}
                }
                break;
            }
            case SchemaValidationRule::ConstraintType::MaxValue: {
                if (auto val = std::get_if<int>(&value)) {
                    try {
                        int maxVal = std::stoi(rule.value);
                        if (*val > maxVal) return false;
                    } catch (...) {}
                }
                break;
            }
            case SchemaValidationRule::ConstraintType::MinLength: {
                if (auto val = std::get_if<std::string>(&value)) {
                    try {
                        int minLen = std::stoi(rule.value);
                        if (static_cast<int>(val->size()) < minLen) return false;
                    } catch (...) {}
                } else if (auto val = std::get_if<std::vector<int>>(&value)) {
                    try {
                        int minLen = std::stoi(rule.value);
                        if (static_cast<int>(val->size()) < minLen) return false;
                    } catch (...) {}
                }
                break;
            }
            case SchemaValidationRule::ConstraintType::MaxLength: {
                if (auto val = std::get_if<std::string>(&value)) {
                    try {
                        int maxLen = std::stoi(rule.value);
                        if (static_cast<int>(val->size()) > maxLen) return false;
                    } catch (...) {}
                } else if (auto val = std::get_if<std::vector<int>>(&value)) {
                    try {
                        int maxLen = std::stoi(rule.value);
                        if (static_cast<int>(val->size()) > maxLen) return false;
                    } catch (...) {}
                }
                break;
            }
            case SchemaValidationRule::ConstraintType::Pattern: {
                if (auto val = std::get_if<std::string>(&value)) {
                    try {
                        std::regex pattern(rule.value);
                        if (!std::regex_match(*val, pattern)) return false;
                    } catch (...) {}
                }
                break;
            }
            case SchemaValidationRule::ConstraintType::Enum: {
                // Check if value matches any enum value
                std::string enumStr = rule.value;
                std::vector<std::string> enumValues;
                size_t pos = 0;
                std::string token;
                while ((pos = enumStr.find(',')) != std::string::npos) {
                    token = enumStr.substr(0, pos);
                    enumValues.push_back(trim(token));
                    enumStr.erase(0, pos + 1);
                }
                enumValues.push_back(trim(enumStr));
                
                if (auto val = std::get_if<std::string>(&value)) {
                    if (std::find(enumValues.begin(), enumValues.end(), *val) == enumValues.end()) {
                        return false;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    
    return true;
}

std::string Config::escapeJSON(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string Config::unescapeJSON(const std::string& str) {
    std::ostringstream oss;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': oss << '\\'; break;
                case '\"': oss << '\"'; break;
                case 'n': oss << '\n'; break;
                case 'r': oss << '\r'; break;
                case 't': oss << '\t'; break;
                default: oss << str[i + 1]; break;
            }
            ++i;
        } else {
            oss << str[i];
        }
    }
    return oss.str();
}

std::string Config::escapeYAML(const std::string& str) {
    // YAML escaping is similar to JSON for basic cases
    return escapeJSON(str);
}

std::string Config::unescapeYAML(const std::string& str) {
    // YAML unescaping is similar to JSON for basic cases
    return unescapeJSON(str);
}

void Config::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && isWhitespace(str[pos])) {
        ++pos;
    }
}

bool Config::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Explicit template instantiations
namespace nlm {
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
}

