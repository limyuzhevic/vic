#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyToIndex;
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    std::vector<std::string> validationErrors;
    bool hotReloadEnabled;
    std::chrono::seconds hotReloadInterval;
    std::chrono::system_clock::time_point lastLoadTime;
    
    Impl() : hotReloadEnabled(false), hotReloadInterval(std::chrono::seconds(5)), lastLoadTime(std::chrono::system_clock::now()) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool isJSON(const std::string& content) {
        if (content.empty()) return false;
        auto trimContent = trim(content);
        return (trimContent.front() == '{' && trimContent.back() == '}') ||
               (trimContent.front() == '[' && trimContent.back() == ']');
    }
    
    bool isYAML(const std::string& content) {
        if (content.empty()) return false;
        auto trimContent = trim(content);
        // Simple YAML detection - contains : or list markers
        return trimContent.find(':') != std::string::npos ||
               trimContent.find("- ") != std::string::npos ||
               trimContent.find("  - ") != std::string::npos;
    }
    
    std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any> jsonToVariant(const nlohmann::json& jsonValue) {
        if (jsonValue.is_string()) {
            return jsonValue.get<std::string>();
        } else if (jsonValue.is_number_integer()) {
            return jsonValue.get<int>();
        } else if (jsonValue.is_number()) {
            return jsonValue.get<double>();
        } else if (jsonValue.is_boolean()) {
            return jsonValue.get<bool>();
        } else if (jsonValue.is_array()) {
            // Determine array type
            if (!jsonValue.empty()) {
                if (jsonValue[0].is_string()) {
                    std::vector<std::string> result;
                    for (const auto& item : jsonValue) {
                        result.push_back(item.get<std::string>());
                    }
                    return result;
                } else if (jsonValue[0].is_number_integer()) {
                    std::vector<int> result;
                    for (const auto& item : jsonValue) {
                        result.push_back(item.get<int>());
                    }
                    return result;
                } else if (jsonValue[0].is_number()) {
                    std::vector<double> result;
                    for (const auto& item : jsonValue) {
                        result.push_back(item.get<double>());
                    }
                    return result;
                }
            }
            // Default to empty vector
            return std::vector<std::string>{};
        }
        return "";
    }
    
    nlohmann::json variantToJson(const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any>& value) {
        nlohmann::json jsonValue;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonValue = arg;
            } else if constexpr (std::is_same_v<T, std::any>) {
                // For std::any, try to extract common types
                if (arg.type() == typeid(int)) {
                    jsonValue = std::any_cast<int>(arg);
                } else if (arg.type() == typeid(double)) {
                    jsonValue = std::any_cast<double>(arg);
                } else if (arg.type() == typeid(std::string)) {
                    jsonValue = std::any_cast<std::string>(arg);
                } else {
                    jsonValue = "unknown_type";
                }
            }
        }, value);
        return jsonValue;
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        result.reserve(str.size() * 2);
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    std::string unescapeString(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[++i]) {
                    case '\"': result += '\"'; break;
                    case '\\': result += '\\'; break;
                    case 'n': result += '\n'; break;
                    case '\r': result += '\r'; break;
                    case '\t': result += '\t'; break;
                    default: result += str[i]; break;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, ConfigSource::File);
}

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determine file format and parse accordingly
    if (isJSON(content)) {
        return loadFromJSON(content);
    } else if (isYAML(content)) {
        return loadFromYAML(content);
    } else {
        // Fallback to simple key=value parsing for backward compatibility
        return loadFromSimpleFormat(content, source);
    }
}

bool Config::loadFromJSON(const std::string& jsonContent) {
    try {
        auto jsonData = nlohmann::json::parse(jsonContent);
        
        // Clear existing entries
        clear();
        
        // Parse JSON object (assuming top-level object)
        if (jsonData.is_object()) {
            for (auto& [key, value] : jsonData.items()) {
                auto configValue = jsonToVariant(value);
                set(key, configValue, ConfigSource::File);
            }
        } else {
            return false;
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlContent) {
    // For now, treat YAML as JSON (fallback)
    // In a real implementation, you would include yaml-cpp and parse actual YAML
    return loadFromJSON(yamlContent);
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    clear();
    bool success = true;
    
    for (const auto& envEntry : std::unordered_map<std::string, std::string>()) {
        std::string key = envEntry.first;
        std::string value = envEntry.second;
        
        // Remove prefix from key
        if (key.rfind(prefix, 0) == 0) {
            key = key.substr(prefix.length());
            
            // Convert string value to appropriate type
            bool converted = false;
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::Environment);
                converted = true;
            } else {
                try {
                    size_t pos;
                    int intVal = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        set(key, intVal, ConfigSource::Environment);
                        converted = true;
                    }
                } catch (...) {}
                
                if (!converted) {
                    try {
                        size_t pos;
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.size()) {
                            set(key, doubleVal, ConfigSource::Environment);
                            converted = true;
                        }
                    } catch (...) {}
                }
            }
            
            if (!converted) {
                set(key, value, ConfigSource::Environment);
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as JSON
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        auto jsonValue = variantToJson(entry.value);
        jsonData[entry.key] = jsonValue;
    }
    
    file << jsonData.dump(2); // Pretty print with 2 spaces indentation
    return true;
}

bool Config::set(const std::string& key, 
                 const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::any>& value, 
                 ConfigSource source, bool validate) {
    std::string normalizedKey = trim(key);
    
    // Check if key already exists
    size_t index = 0;
    bool keyExists = false;
    if (pImpl->keyToIndex.find(normalizedKey) != pImpl->keyToIndex.end()) {
        index = pImpl->keyToIndex[normalizedKey];
        keyExists = true;
    }
    
    // Update existing entry or create new one
    if (keyExists) {
        // Update existing entry
        pImpl->entries[index].value = value;
        pImpl->entries[index].source = source;
        pImpl->entries[index].timestamp = std::chrono::system_clock::now();
        pImpl->entries[index].validated = false;
    } else {
        // Create new entry
        ConfigEntry newEntry(normalizedKey, value, source);
        pImpl->entries.push_back(newEntry);
        index = pImpl->entries.size() - 1;
        pImpl->keyToIndex[normalizedKey] = index;
    }
    
    // Validate if requested
    if (validate) {
        pImpl->validationErrors.clear();
        bool isValid = true;
        
        // Check dependencies
        if (pImpl->dependencies.find(normalizedKey) != pImpl->dependencies.end()) {
            for (const auto& depKey : pImpl->dependencies[normalizedKey]) {
                if (pImpl->keyToIndex.find(depKey) == pImpl->keyToIndex.end()) {
                    pImpl->validationErrors.push_back("Key depends on missing key: " + depKey);
                    isValid = false;
                }
            }
        }
        
        // Mark as validated
        pImpl->entries[index].validated = isValid;
    }
    
    return true;
}

bool Config::set(const std::string& key, int value, ConfigSource source, bool validate) {
    return set(key, value, source, validate);
}

bool Config::set(const std::string& key, double value, ConfigSource source, bool validate) {
    return set(key, value, source, validate);
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source, bool validate) {
    return set(key, value, source, validate);
}

bool Config::set(const std::string& key, bool value, ConfigSource source, bool validate) {
    return set(key, value, source, validate);
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        // Check if required
        if (entry.required && !has(entry.key)) {
            pImpl->validationErrors.push_back("Required key missing: " + entry.key);
            isValid = false;
        }
        
        // Check if validated
        if (!entry.validated && entry.required) {
            pImpl->validationErrors.push_back("Required key not validated: " + entry.key);
            isValid = false;
        }
    }
    
    return isValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

bool Config::has(const std::string& key) const {
    return pImpl->keyToIndex.find(trim(key)) != pImpl->keyToIndex.end();
}

bool Config::has(const std::string& key, ConfigSource source) const {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return false;
    }
    
    size_t index = pImpl->keyToIndex.at(normalizedKey);
    return pImpl->entries[index].source == source;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string normalizedKey = trim(key);
    
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = pImpl->keyToIndex.at(normalizedKey);
    const auto& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

std::optional<int> Config::getInt(const std::string& key) const {
    return get<int>(key);
}

std::optional<double> Config::getDouble(const std::string& key) const {
    return get<double>(key);
}

std::optional<std::string> Config::getString(const std::string& key) const {
    return get<std::string>(key);
}

std::optional<bool> Config::getBool(const std::string& key) const {
    return get<bool>(key);
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = trim(key);
    
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return;
    }
    
    size_t index = pImpl->keyToIndex[normalizedKey];
    
    // Remove from keyToIndex
    pImpl->keyToIndex.erase(normalizedKey);
    
    // Update indices in keyToIndex
    for (auto& pair : pImpl->keyToIndex) {
        if (pair.second > index) {
            pair.second--;
        }
    }
    
    // Remove from entries
    pImpl->entries.erase(pImpl->entries.begin() + index);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keyToIndex.clear();
    pImpl->dependencies.clear();
    pImpl->validationErrors.clear();
}

bool Config::reload() {
    // For now, just clear and reload from file if available
    // In a full implementation, this would reload based on hot reload settings
    return true;
}

void Config::setHotReload(bool enabled, std::chrono::seconds interval) {
    pImpl->hotReloadEnabled = enabled;
    pImpl->hotReloadInterval = interval;
}

bool Config::isHotReloadEnabled() const {
    return pImpl->hotReloadEnabled;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) != pImpl->keyToIndex.end()) {
        size_t index = pImpl->keyToIndex[normalizedKey];
        pImpl->entries[index].description = description;
    }
}

void Config::setType(const std::string& key, const std::string& type) {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) != pImpl->keyToIndex.end()) {
        size_t index = pImpl->keyToIndex[normalizedKey];
        pImpl->entries[index].type = type;
    }
}

void Config::setRequired(const std::string& key, bool required) {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) != pImpl->keyToIndex.end()) {
        size_t index = pImpl->keyToIndex[normalizedKey];
        pImpl->entries[index].required = required;
    }
}

bool Config::addDependency(const std::string& key, const std::string& dependsOnKey) {
    std::string normalizedKey = trim(key);
    std::string normalizedDepKey = trim(dependsOnKey);
    
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return false;
    }
    
    pImpl->dependencies[normalizedKey].push_back(normalizedDepKey);
    return true;
}

std::optional<std::string> Config::getDescription(const std::string& key) const {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = pImpl->keyToIndex.at(normalizedKey);
    return pImpl->entries[index].description;
}

std::optional<std::string> Config::getType(const std::string& key) const {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = pImpl->keyToIndex.at(normalizedKey);
    return pImpl->entries[index].type;
}

std::optional<bool> Config::getRequired(const std::string& key) const {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = pImpl->keyToIndex.at(normalizedKey);
    return pImpl->entries[index].required;
}

std::optional<ConfigSource> Config::getSource(const std::string& key) const {
    std::string normalizedKey = trim(key);
    if (pImpl->keyToIndex.find(normalizedKey) == pImpl->keyToIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = pImpl->keyToIndex.at(normalizedKey);
    return pImpl->entries[index].source;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (entry.required) oss << " [REQUIRED]";
        if (!entry.description.empty()) oss << " - " << entry.description;
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::toJSON() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        auto jsonValue = variantToJson(entry.value);
        jsonData[entry.key] = jsonValue;
    }
    
    return jsonData.dump(2);
}

std::string Config::toYAML() const {
    // For now, convert JSON to YAML format
    // In a real implementation, you would use a YAML library
    std::string jsonStr = toJSON();
    // Simple YAML conversion - replace { } with nothing and format
    std::string yamlStr;
    bool inObject = false;
    bool afterColon = false;
    
    for (char c : jsonStr) {
        if (c == '{' || c == '[') {
            if (!inObject) {
                inObject = true;
                afterColon = false;
            }
        } else if (c == '}' || c == ']') {
            if (inObject) {
                inObject = false;
                afterColon = false;
            }
        } else if (c == ':') {
            yamlStr += ": ";
            afterColon = true;
        } else if (c == ',' && inObject && !afterColon) {
            yamlStr += "\n";
            afterColon = false;
        } else if (c == '"') {
            // Skip quotes
            continue;
        } else {
            yamlStr += c;
            afterColon = false;
        }
    }
    
    return yamlStr;
}

// Backward compatibility implementation
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

bool Config::loadFromSimpleFormat(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
            
            set(key, value, source);
        }
    }
    return true;
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

// Include nlohmann::json for JSON parsing
#include <nlohmann/json.hpp>