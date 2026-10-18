#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <functional>
#include <regex>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iomanip>

// Basic JSON support (since nlohmann/json is not available)
// JSON parsing implementation
struct JSONValue {
    virtual ~JSONValue() = default;
};

struct JSONString : JSONValue {
    std::string value;
    JSONString(const std::string& v) : value(v) {}
};

struct JSONNumber : JSONValue {
    double value;
    bool isInteger;
    JSONNumber(double v, bool integer) : value(v), isInteger(integer) {}
};

struct JSONBoolean : JSONValue {
    bool value;
    JSONBoolean(bool v) : value(v) {}
};

struct JSONArray : JSONValue {
    std::vector<std::shared_ptr<JSONValue>> elements;
    void add(const std::shared_ptr<JSONValue>& elem) { elements.push_back(elem); }
};

struct JSONObject : JSONValue {
    std::unordered_map<std::string, std::shared_ptr<JSONValue>> members;
    void add(const std::string& key, const std::shared_ptr<JSONValue>& value) { members[key] = value; }
};

class JSONParser {
public:
    std::shared_ptr<JSONValue> parse(const std::string& jsonStr);
    std::shared_ptr<JSONValue> parseFile(const std::string& filepath);

private:
    static bool parseString(const std::string& jsonStr, size_t& pos, std::string& out);
    static bool parseNumber(const std::string& jsonStr, size_t& pos, double& out, bool& isInteger);
    static bool parseBoolean(const std::string& jsonStr, size_t& pos, bool& out);
    static bool parseNull(const std::string& jsonStr, size_t& pos, bool& out);
    static bool parseArray(const std::string& jsonStr, size_t& pos, std::vector<std::shared_ptr<JSONValue>>& out);
    static bool parseObject(const std::string& jsonStr, size_t& pos, std::unordered_map<std::string, std::shared_ptr<JSONValue>>& out);
};

namespace basic_json {
    std::shared_ptr<JSONValue> JSONParser::parse(const std::string& jsonStr) {
        size_t pos = 0;
        std::shared_ptr<JSONValue> result;
        bool success = false;
        
        // Skip whitespace
        while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
        
        if (jsonStr[pos] == '"') {
            std::string str;
            success = parseString(jsonStr, pos, str);
            result = std::make_shared<JSONString>(str);
        } else if (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-') {
            double num;
            bool isInteger;
            success = parseNumber(jsonStr, pos, num, isInteger);
            result = std::make_shared<JSONNumber>(num, isInteger);
        } else if (jsonStr.compare(pos, 4, "true") == 0) {
            bool val;
            success = parseBoolean(jsonStr, pos, val);
            result = std::make_shared<JSONBoolean>(val);
        } else if (jsonStr.compare(pos, 5, "false") == 0) {
            bool val;
            success = parseBoolean(jsonStr, pos, val);
            result = std::make_shared<JSONBoolean>(val);
        } else if (jsonStr.compare(pos, 4, "null") == 0) {
            bool val;
            success = parseNull(jsonStr, pos, val);
            result = std::make_shared<JSONBoolean>(false); // null as false for now
        } else if (jsonStr[pos] == '[') {
            std::vector<std::shared_ptr<JSONValue>> arr;
            success = parseArray(jsonStr, pos, arr);
            auto array = std::make_shared<JSONArray>();
            for (const auto& elem : arr) {
                array->add(elem);
            }
            result = array;
        } else if (jsonStr[pos] == '{') {
            std::unordered_map<std::string, std::shared_ptr<JSONValue>> obj;
            success = parseObject(jsonStr, pos, obj);
            auto object = std::make_shared<JSONObject>();
            for (const auto& pair : obj) {
                object->add(pair.first, pair.second);
            }
            result = object;
        }
        
        if (!success) throw std::runtime_error("Failed to parse JSON");
        return result;
    }
    
    std::shared_ptr<JSONValue> JSONParser::parseFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parse(buffer.str());
    }
    
    // Implementation of parsing helper functions
    bool JSONParser::parseString(const std::string& jsonStr, size_t& pos, std::string& out) {
        size_t start = ++pos; // Skip opening quote
        while (pos < jsonStr.size()) {
            if (jsonStr[pos] == '\\') {
                pos += 2; // Skip escaped character
                continue;
            } else if (jsonStr[pos] == '"') {
                break;
            }
            pos++;
        }
        if (pos >= jsonStr.size()) return false;
        out = jsonStr.substr(start, pos - start);
        pos++; // Skip closing quote
        return true;
    }
    
    bool JSONParser::parseNumber(const std::string& jsonStr, size_t& pos, double& out, bool& isInteger) {
        size_t start = pos;
        bool negative = false;
        if (jsonStr[pos] == '-') {
            negative = true;
            pos++;
        }
        while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) pos++;
        if (pos < jsonStr.size() && jsonStr[pos] == '.') {
            pos++;
            while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) pos++;
            isInteger = false;
        } else {
            isInteger = true;
        }
        if (pos >= jsonStr.size()) return false;
        out = std::stod(jsonStr.substr(start, pos - start));
        return true;
    }
    
    bool JSONParser::parseBoolean(const std::string& jsonStr, size_t& pos, bool& out) {
        if (jsonStr.compare(pos, 4, "true") == 0) {
            out = true;
            pos += 4;
            return true;
        } else if (jsonStr.compare(pos, 5, "false") == 0) {
            out = false;
            pos += 5;
            return true;
        }
        return false;
    }
    
    bool JSONParser::parseNull(const std::string& jsonStr, size_t& pos, bool& out) {
        if (jsonStr.compare(pos, 4, "null") == 0) {
            out = false; // null as false for now
            pos += 4;
            return true;
        }
        return false;
    }
    
    bool JSONParser::parseArray(const std::string& jsonStr, size_t& pos, std::vector<std::shared_ptr<JSONValue>>& out) {
        if (jsonStr[pos] != '[') return false;
        pos++; // Skip '['
        
        bool first = true;
        while (pos < jsonStr.size()) {
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
            
            if (jsonStr[pos] == ']') {
                pos++;
                return true;
            }
            
            if (!first) {
                if (jsonStr[pos] != ',') return false;
                pos++;
            }
            
            // Parse element
            std::shared_ptr<JSONValue> elem;
            size_t startPos = pos;
            
            if (jsonStr[pos] == '"') {
                std::string str;
                if (parseString(jsonStr, pos, str)) {
                    elem = std::make_shared<JSONString>(str);
                }
            } else if (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-') {
                double num;
                bool isInt;
                if (parseNumber(jsonStr, pos, num, isInt)) {
                    elem = std::make_shared<JSONNumber>(num, isInt);
                }
            } else if (jsonStr.compare(pos, 4, "true") == 0) {
                bool val;
                if (parseBoolean(jsonStr, pos, val)) {
                    elem = std::make_shared<JSONBoolean>(val);
                }
            } else if (jsonStr.compare(pos, 5, "false") == 0) {
                bool val;
                if (parseBoolean(jsonStr, pos, val)) {
                    elem = std::make_shared<JSONBoolean>(val);
                }
            } else if (jsonStr[pos] == '[') {
                std::vector<std::shared_ptr<JSONValue>> nestedArr;
                size_t tempPos = pos;
                if (parseArray(jsonStr, tempPos, nestedArr)) {
                    elem = std::make_shared<JSONArray>();
                    static_cast<JSONArray*>(elem.get())->elements = nestedArr;
                    pos = tempPos;
                }
            } else if (jsonStr[pos] == '{') {
                std::unordered_map<std::string, std::shared_ptr<JSONValue>> nestedObj;
                size_t tempPos = pos;
                if (parseObject(jsonStr, tempPos, nestedObj)) {
                    elem = std::make_shared<JSONObject>();
                    static_cast<JSONObject*>(elem.get())->members = nestedObj;
                    pos = tempPos;
                }
            }
            
            if (!elem) return false;
            out.push_back(elem);
            first = false;
            
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
        }
        return false;
    }
    
    bool JSONParser::parseObject(const std::string& jsonStr, size_t& pos, std::unordered_map<std::string, std::shared_ptr<JSONValue>>& out) {
        if (jsonStr[pos] != '{') return false;
        pos++; // Skip '{'
        
        bool first = true;
        while (pos < jsonStr.size()) {
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
            
            if (jsonStr[pos] == '}') {
                pos++;
                return true;
            }
            
            if (!first) {
                if (jsonStr[pos] != ',') return false;
                pos++;
            }
            
            // Parse key
            if (jsonStr[pos] != '"') return false;
            std::string key;
            if (!parseString(jsonStr, pos, key)) return false;
            
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') return false;
            pos++; // Skip ':'
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
            
            // Parse value
            std::shared_ptr<JSONValue> value;
            size_t startPos = pos;
            
            if (jsonStr[pos] == '"') {
                std::string str;
                if (parseString(jsonStr, pos, str)) {
                    value = std::make_shared<JSONString>(str);
                }
            } else if (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-') {
                double num;
                bool isInt;
                if (parseNumber(jsonStr, pos, num, isInt)) {
                    value = std::make_shared<JSONNumber>(num, isInt);
                }
            } else if (jsonStr.compare(pos, 4, "true") == 0) {
                bool val;
                if (parseBoolean(jsonStr, pos, val)) {
                    value = std::make_shared<JSONBoolean>(val);
                }
            } else if (jsonStr.compare(pos, 5, "false") == 0) {
                bool val;
                if (parseBoolean(jsonStr, pos, val)) {
                    value = std::make_shared<JSONBoolean>(val);
                }
            } else if (jsonStr[pos] == '[') {
                std::vector<std::shared_ptr<JSONValue>> arr;
                if (parseArray(jsonStr, pos, arr)) {
                    auto jsonArray = std::make_shared<JSONArray>();
                    for (const auto& elem : arr) {
                        jsonArray->add(elem);
                    }
                    value = jsonArray;
                }
            } else if (jsonStr[pos] == '{') {
                std::unordered_map<std::string, std::shared_ptr<JSONValue>> obj;
                if (parseObject(jsonStr, pos, obj)) {
                    auto jsonObject = std::make_shared<JSONObject>();
                    for (const auto& pair : obj) {
                        jsonObject->add(pair.first, pair.second);
                    }
                    value = jsonObject;
                }
            }
            
            if (!value) return false;
            out[key] = value;
            first = false;
            
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
        }
        return false;
    }

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::shared_ptr<ConfigGroup>> groups;
    std::unordered_map<std::string, std::string> inheritanceMap;
    std::vector<ConfigSnapshot> snapshots;
};

Config::Config() : schema(), schemaDefined(false), groups(), inheritanceMap(), snapshots() {
    pImpl = std::make_unique<Impl>();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

} // namespace json

// Config helper implementations
std::string nlm::Config::validateValue(const std::string& key, const ConfigValue& value) const {
    if (hasSchema() && schema.hasRule(key)) {
        if (!schema.validate(key, value)) {
            return "Validation failed for key: " + key;
        }
    }
    return "";
}

nlm::ConfigPath nlm::Config::parsePath(const std::string& dotNotation) {
    return ConfigPath(dotNotation);
}

std::vector<std::string> nlm::Config::findMatchingKeys(const std::regex& pattern) const {
    std::vector<std::string> matches;
    for (const auto& entry : pImpl->entries) {
        if (std::regex_match(entry.key, pattern)) {
            matches.push_back(entry.key);
        }
    }
    return matches;
}

std::vector<std::string> nlm::Config::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Helper implementations for JSON conversion
std::string nlm::Config::jsonToConfigValueString(const json& j) {
    if (j.is_string()) return j.get<std::string>();
    if (j.is_number_integer()) return std::to_string(j.get<int64_t>());
    if (j.is_number()) return std::to_string(j.get<double>());
    if (j.is_boolean()) return j.get<bool>() ? "true" : "false";
    if (j.is_null()) return "";
    return j.dump();
}

json nlm::Config::configValueToJSON(const ConfigValue& value) {
    json j;
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            j = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = val;
        } else if constexpr (std::is_same_v<T, double>) {
            j = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = val;
        }
    }, value);
    return j;
}

std::string nlm::Config::generateChecksum(const std::string& data) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(data));
}

// Compression helpers
std::string nlm::Config::compress(const std::string& data) {
    // Simplified compression - just return the data for now
    // In a real implementation, you would use gzip, deflate, etc.
    return data;
}

std::string nlm::Config::decompress(const std::string& data) {
    // Simplified decompression
    return data;
}

// Continue with more implementations...
bool nlm::Config::loadFromFile(const std::string& filepath) {
    // Check if it's JSON/YAML format based on file extension
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return importFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return importFromYAML(filepath);
    } else {
        // Default to simple key=value format
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
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool nlm::Config::importFromJSON(const std::string& jsonStr) {
    try {
        auto j = json::parse(jsonStr);
        if (!j.is_object()) {
            return false;
        }
        
        clear();
        
        for (const auto& [key, value] : j.items()) {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Try to infer array type
                std::vector<std::string> stringArray;
                bool allStrings = true;
                for (const auto& item : value) {
                    if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    } else {
                        allStrings = false;
                        break;
                    }
                }
                
                if (allStrings) {
                    configValue = stringArray;
                } else {
                    // For simplicity, use vector<double> for numeric arrays
                    std::vector<double> doubleArray;
                    bool allNumbers = true;
                    for (const auto& item : value) {
                        if (item.is_number()) {
                            doubleArray.push_back(item.get<double>());
                        } else {
                            allNumbers = false;
                            break;
                        }
                    }
                    
                    if (allNumbers) {
                        configValue = doubleArray;
                    } else {
                        // Create a JSON string representation for complex values
                        configValue = value.dump();
                    }
                }
            } else {
                // Object or other complex type - convert to string
                configValue = value.dump();
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool nlm::Config::importFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return importFromJSON(buffer.str());
}

bool nlm::Config::importFromYAML(const std::string& yamlStr) {
    // For now, treat YAML as JSON (simplified)
    // In a real implementation, you would use a YAML parser
    return importFromJSON(yamlStr);
}

bool nlm::Config::importFromYAMLFile(const std::string& filepath) {
    return importFromYAML(filepath);
}

std::string nlm::Config::exportToJSON() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJSON(entry.value);
    }
    
    return j.dump(2); // Pretty print with 2-space indentation
}

std::string nlm::Config::exportToYAML() const {
    // For now, return JSON representation
    return exportToJSON();
}

std::string nlm::Config::exportToCompressed(const std::string& format) const {
    std::string jsonData = exportToJSON();
    return compress(jsonData);
}

bool nlm::Config::importFromCompressed(const std::string& data, const std::string& format) {
    std::string decompressed = decompress(data);
    return importFromJSON(decompressed);
}

// Rest of the implementation...
void nlm::Config::setNested(const std::string& dotNotationKey, const ConfigValue& value, ConfigSource source) {
    ConfigPath path(dotNotationKey);
    std::string errorMsg = validateValue(dotNotationKey, value);
    if (!errorMsg.empty()) {
        throw std::runtime_error(errorMsg);
    }
    
    // For now, just treat as regular key (nested support to be enhanced)
    set(dotNotationKey, value, source);
}

std::optional<int> nlm::Config::getNested<int>(const std::string& dotNotationKey) const {
    return get<int>(dotNotationKey);
}

std::optional<int64_t> nlm::Config::getNested<int64_t>(const std::string& dotNotationKey) const {
    return get<int64_t>(dotNotationKey);
}

std::optional<double> nlm::Config::getNested<double>(const std::string& dotNotationKey) const {
    return get<double>(dotNotationKey);
}

std::optional<bool> nlm::Config::getNested<bool>(const std::string& dotNotationKey) const {
    return get<bool>(dotNotationKey);
}

std::optional<std::string> nlm::Config::getNested<std::string>(const std::string& dotNotationKey) const {
    return get<std::string>(dotNotationKey);
}

int nlm::Config::getNestedOr<int>(const std::string& dotNotationKey, const int& defaultValue) const {
    auto val = getNested<int>(dotNotationKey);
    return val.has_value() ? val.value() : defaultValue;
}

int64_t nlm::Config::getNestedOr<int64_t>(const std::string& dotNotationKey, const int64_t& defaultValue) const {
    auto val = getNested<int64_t>(dotNotationKey);
    return val.has_value() ? val.value() : defaultValue;
}

double nlm::Config::getNestedOr<double>(const std::string& dotNotationKey, const double& defaultValue) const {
    auto val = getNested<double>(dotNotationKey);
    return val.has_value() ? val.value() : defaultValue;
}

bool nlm::Config::getNestedOr<bool>(const std::string& dotNotationKey, const bool& defaultValue) const {
    auto val = getNested<bool>(dotNotationKey);
    return val.has_value() ? val.value() : defaultValue;
}

std::string nlm::Config::getNestedOr<std::string>(const std::string& dotNotationKey, const std::string& defaultValue) const {
    auto val = getNested<std::string>(dotNotationKey);
    return val.has_value() ? val.value() : defaultValue;
}

bool nlm::Config::hasNested(const std::string& dotNotationKey) const {
    return has(dotNotationKey);
}

void nlm::Config::removeNested(const std::string& dotNotationKey) {
    remove(dotNotationKey);
}

bool nlm::Config::addGroup(const std::string& groupName, const std::string& description) {
    if (pImpl->groups.find(groupName) != pImpl->groups.end()) {
        return false; // Group already exists
    }
    
    auto group = std::make_shared<ConfigGroup>(groupName, description);
    pImpl->groups[groupName] = group;
    return true;
}

bool nlm::Config::removeGroup(const std::string& groupName) {
    auto it = pImpl->groups.find(groupName);
    if (it == pImpl->groups.end()) {
        return false; // Group not found
    }
    
    // Remove group and its configs from config map
    const auto& group = it->second;
    for (const auto& configKey : group->getConfigs()) {
        remove(configKey);
    }
    
    pImpl->groups.erase(it);
    return true;
}

const std::shared_ptr<nlm::ConfigGroup>& nlm::Config::getGroup(const std::string& groupName) const {
    static const std::shared_ptr<ConfigGroup> nullGroup;
    auto it = pImpl->groups.find(groupName);
    return it != pImpl->groups.end() ? it->second : nullGroup;
}

std::vector<std::string> nlm::Config::getGroupNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->groups.size());
    for (const auto& pair : pImpl->groups) {
        names.push_back(pair.first);
    }
    return names;
}

bool nlm::Config::addConfigToGroup(const std::string& groupName, const std::string& key) {
    auto groupIt = pImpl->groups.find(groupName);
    if (groupIt == pImpl->groups.end()) {
        return false; // Group not found
    }
    
    if (!has(key)) {
        return false; // Config key not found
    }
    
    groupIt->second->addConfig(key);
    return true;
}

bool nlm::Config::removeConfigFromGroup(const std::string& groupName, const std::string& key) {
    auto groupIt = pImpl->groups.find(groupName);
    if (groupIt == pImpl->groups.end()) {
        return false; // Group not found
    }
    
    groupIt->second->removeConfig(key);
    return true;
}

std::vector<std::string> nlm::Config::getConfigsInGroup(const std::string& groupName) const {
    auto groupIt = pImpl->groups.find(groupName);
    if (groupIt == pImpl->groups.end()) {
        return std::vector<std::string>(); // Group not found
    }
    
    return groupIt->second->getConfigs();
}

void nlm::Config::setSchema(const ConfigSchema& schema) {
    this->schema = schema;
    schemaDefined = true;
}

bool nlm::Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!schema.validate(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

bool nlm::Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false; // Key not found
    }
    
    return schema.validate(key, it->value);
}

bool nlm::Config::inheritFrom(const Config& baseConfig) {
    // For simplicity, inherit all entries from base config
    // In a real implementation, you would merge with override precedence
    for (const auto& baseEntry : baseConfig.pImpl->entries) {
        // Check if key already exists
        bool exists = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == baseEntry.key) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            pImpl->entries.push_back(baseEntry);
        }
    }
    
    return true;
}

bool nlm::Config::removeInheritance(const std::string& groupName) {
    auto it = pImpl->inheritanceMap.find(groupName);
    if (it == pImpl->inheritanceMap.end()) {
        return false; // No inheritance for this group
    }
    
    pImpl->inheritanceMap.erase(it);
    return true;
}

bool nlm::Config::hasInheritance(const std::string& groupName) const {
    return pImpl->inheritanceMap.find(groupName) != pImpl->inheritanceMap.end();
}

std::string nlm::Config::createSnapshot(const std::string& description) {
    std::string snapshotId = "snapshot_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    ConfigSnapshot snapshot;
    snapshot.id = snapshotId;
    snapshot.description = description;
    
    // Copy all entries
    for (const auto& entry : pImpl->entries) {
        snapshot.values[entry.key] = entry.value;
    }
    
    // Generate checksum
    std::ostringstream oss;
    for (const auto& pair : snapshot.values) {
        oss << pair.first << "=" << std::visit([](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return v;
            } else if constexpr (std::is_same_v<T, int>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(v);
            } else if constexpr (std::is_same_v<T, bool>) {
                return v ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::string result = "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) result += ",";
                    result += std::to_string(v[i]);
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::string result = "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) result += ",";
                    result += std::to_string(v[i]);
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::string result = "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) result += ",";
                    result += "\"" + v[i] + "\"";
                }
                result += "]";
                return result;
            }
            return "";
        }, pair.second));
    }
    
    snapshot.checksum = generateChecksum(oss.str());
    
    pImpl->snapshots.push_back(snapshot);
    return snapshotId;
}

bool nlm::Config::restoreSnapshot(const std::string& snapshotId) {
    for (const auto& snapshot : pImpl->snapshots) {
        if (snapshot.id == snapshotId) {
            clear();
            
            for (const auto& pair : snapshot.values) {
                ConfigValue value;
                // Try to infer type from string representation
                // This is simplified - a real implementation would preserve types
                if (pair.second.index() == 5) { // std::string
                    value = std::get<5>(pair.second);
                } else if (pair.second.index() == 0) { // int
                    value = std::get<0>(pair.second);
                } else if (pair.second.index() == 1) { // int64_t
                    value = std::get<1>(pair.second);
                } else if (pair.second.index() == 2) { // double
                    value = std::get<2>(pair.second);
                } else if (pair.second.index() == 3) { // bool
                    value = std::get<3>(pair.second);
                }
                
                set(pair.first, value, ConfigSource::File);
            }
            
            return true;
        }
    }
    
    return false; // Snapshot not found
}

bool nlm::Config::removeSnapshot(const std::string& snapshotId) {
    auto it = std::remove_if(pImpl->snapshots.begin(), pImpl->snapshots.end(),
        [&snapshotId](const ConfigSnapshot& snapshot) { return snapshot.id == snapshotId; });
    
    if (it != pImpl->snapshots.end()) {
        pImpl->snapshots.erase(it, pImpl->snapshots.end());
        return true;
    }
    
    return false;
}

std::vector<std::string> nlm::Config::getSnapshotIds() const {
    std::vector<std::string> ids;
    ids.reserve(pImpl->snapshots.size());
    for (const auto& snapshot : pImpl->snapshots) {
        ids.push_back(snapshot.id);
    }
    return ids;
}

nlm::ConfigSnapshot nlm::Config::getSnapshot(const std::string& snapshotId) const {
    for (const auto& snapshot : pImpl->snapshots) {
        if (snapshot.id == snapshotId) {
            return snapshot;
        }
    }
    
    return ConfigSnapshot(); // Return empty snapshot if not found
}

std::string nlm::Config::getDelta(const Config& otherConfig) const {
    json deltaJson = json::object();
    
    // Find added and modified keys
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : otherConfig.pImpl->entries) {
            if (otherEntry.key == entry.key) {
                found = true;
                // Check if values differ
                auto otherValue = otherEntry.value;
                if (!std::holds_alternative<std::nullptr_t>(otherValue) && 
                    entry.value.index() == otherValue.index() &&
                    std::visit([](const auto& v1, const auto& v2) { return v1 == v2; },
                              entry.value, otherValue)) {
                    // Values are the same, skip
                    continue;
                }
                
                // Value changed or type differs
                deltaJson["modified"][entry.key] = configValueToJSON(entry.value);
                break;
            }
        }
        
        if (!found) {
            deltaJson["added"][entry.key] = configValueToJSON(entry.value);
        }
    }
    
    // Find removed keys
    for (const auto& otherEntry : otherConfig.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            deltaJson["removed"].push_back(otherEntry.key);
        }
    }
    
    return deltaJson.dump();
}

bool nlm::Config::applyDelta(const std::string& deltaJson) {
    try {
        auto delta = json::parse(deltaJson);
        
        // Apply added/removed/modified changes
        if (delta.contains("added")) {
            for (const auto& [key, value] : delta["added"].items()) {
                ConfigValue configValue;
                // Convert JSON value to ConfigValue
                // This is simplified - a real implementation would handle all types
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                }
                set(key, configValue, ConfigSource::Runtime);
            }
        }
        
        if (delta.contains("removed")) {
            for (const auto& key : delta["removed"]) {
                remove(key);
            }
        }
        
        if (delta.contains("modified")) {
            for (const auto& [key, value] : delta["modified"].items()) {
                ConfigValue configValue;
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                }
                set(key, configValue, ConfigSource::Runtime);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void nlm::Config::clear() {
    pImpl->entries.clear();
    pImpl->groups.clear();
    pImpl->inheritanceMap.clear();
    pImpl->snapshots.clear();
}

std::string nlm::Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "  Entries: " << pImpl->entries.size() << "\n";
    oss << "  Groups: " << pImpl->groups.size() << "\n";
    oss << "  Snapshots: " << pImpl->snapshots.size() << "\n";
    oss << "  Schema Defined: " << (schemaDefined ? "Yes" : "No") << "\n";
    oss << "  Inheritance Map: " << pImpl->inheritanceMap.size() << "\n";
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!schemaDefined) {
        oss << "\nSchema Validation: Not defined\n";
    } else {
        oss << "\nSchema Validation: Defined\n";
        oss << "  Rules: " << schema.getKeys().size() << "\n";
    }
    
    return oss.str();
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
