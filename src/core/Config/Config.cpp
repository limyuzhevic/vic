#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <set>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigValue> nestedValues;
    std::vector<ConfigEntry> flatEntries;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to parse as JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Try to parse as JSON first
    bool isJSON = false;
    if (!content.empty()) {
        char firstChar = content[0];
        char secondChar = content.size() > 1 ? content[1] : ' ';
        isJSON = (firstChar == '{' && secondChar != '-') ||
                 (firstChar == '[' && secondChar == ' ');
    }
    
    if (isJSON) {
        return loadFromJSON(content);
    }
    
    // Fall back to simple key=value format
    pImpl->flatEntries.clear();
    pImpl->nestedValues.clear();
    
    std::string line;
    std::ifstream fileStream(filepath);
    while (std::getline(fileStream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
    
    validate();
    return true;
}

bool Config::loadFromJSON(const std::string& jsonContent) {
    // Simple JSON parser for nested objects
    // This is a simplified implementation - for production, use a proper JSON library
    pImpl->flatEntries.clear();
    pImpl->nestedValues.clear();
    
    try {
        // Find the outermost object
        size_t start = jsonContent.find('{');
        if (start == std::string::npos) {
            return false;
        }
        
        // Count braces to find matching closing brace
        int braceCount = 1;
        size_t pos = start + 1;
        while (pos < jsonContent.size() && braceCount > 0) {
            if (jsonContent[pos] == '{') {
                braceCount++;
            } else if (jsonContent[pos] == '}') {
                braceCount--;
            }
            pos++;
        }
        
        if (braceCount != 0) {
            return false;
        }
        
        std::string jsonObject = jsonContent.substr(start, pos - start);
        parseJSONObject(jsonObject, "");
        validate();
        
        return true;
    } catch (...) {
        return false;
    }
}

void Config::parseJSONObject(const std::string& jsonObject, const std::string& prefix) {
    // Simplified JSON parsing
    std::regex pairRegex(R"((\w+):\s*"([^"]*)"|(\w+):\s*(\d+\.?\d*|true|false))");
    
    auto begin = std::sregex_iterator(jsonObject.begin(), jsonObject.end(), pairRegex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        
        std::string key;
        std::string valueStr;
        
        if (match[1].matched) {
            // String value
            key = match[1].str();
            valueStr = match[2].str();
            ConfigValue value = valueStr;
            std::string fullKey = prefix.empty() ? key : prefix + "." + key;
            set(fullKey, value, ConfigSource::File);
        } else if (match[3].matched) {
            // String value without quotes
            key = match[3].str();
            valueStr = match[4].str();
            
            // Try to determine type
            if (valueStr == "true" || valueStr == "false") {
                bool value = (valueStr == "true");
                std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                set(fullKey, value, ConfigSource::File);
            } else {
                try {
                    size_t pos;
                    int intVal = std::stoi(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                        set(fullKey, intVal, ConfigSource::File);
                        continue;
                    }
                    
                    double doubleVal = std::stod(valueStr, &pos);
                    if (pos == valueStr.size()) {
                        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                        set(fullKey, doubleVal, ConfigSource::File);
                    }
                } catch (...) {
                    // Keep as string
                    std::string fullKey = prefix.empty() ? key : prefix + "." + key;
                    set(fullKey, valueStr, ConfigSource::File);
                }
            }
        }
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
    
    // Save in INI format for backward compatibility
    for (const auto& entry : pImpl->flatEntries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << jsonValueToString(entry.value) << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    ensureNestedStructure(key);
    auto it = pImpl->nestedValues.find(key);
    if (it == pImpl->nestedValues.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second);
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
    std::string normalizedKey = normalizeKey(key);
    
    // For backward compatibility, also add to flat entries
    std::string description;
    if (!has(key) && pImpl->flatEntries.empty()) {
        description = "Configuration for " + key;
    }
    
    auto it = std::find_if(pImpl->flatEntries.begin(), pImpl->flatEntries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->flatEntries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->flatEntries.emplace_back(normalizedKey, value, source, description);
    }
    
    pImpl->nestedValues[normalizedKey] = value;
    pImpl->validationErrors.clear();
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
    std::string normalizedKey = normalizeKey(key);
    return pImpl->nestedValues.find(normalizedKey) != pImpl->nestedValues.end();
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = normalizeKey(key);
    
    // Remove from both flat and nested
    pImpl->nestedValues.erase(normalizedKey);
    
    pImpl->flatEntries.erase(
        std::remove_if(pImpl->flatEntries.begin(), pImpl->flatEntries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; }),
        pImpl->flatEntries.end()
    );
    
    // Also remove nested entries with this as a prefix
    std::vector<std::string> toRemove;
    for (const auto& pair : pImpl->nestedValues) {
        if (pair.first != normalizedKey && pair.first.find(normalizedKey + ".") == 0) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& keyToRemove : toRemove) {
        pImpl->nestedValues.erase(keyToRemove);
        pImpl->flatEntries.erase(
            std::remove_if(pImpl->flatEntries.begin(), pImpl->flatEntries.end(),
                [&keyToRemove](const ConfigEntry& e) { return e.key == keyToRemove; }),
            pImpl->flatEntries.end()
        );
    }
    
    pImpl->validationErrors.clear();
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->nestedValues.size());
    for (const auto& pair : pImpl->nestedValues) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    std::string normalizedPrefix = normalizeKey(prefix);
    
    for (const auto& pair : pImpl->nestedValues) {
        if (pair.first == normalizedPrefix || pair.first.find(normalizedPrefix + ".") == 0) {
            keys.push_back(pair.first);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->flatEntries.clear();
    pImpl->nestedValues.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->nestedValues.size() << " entries):\n";
    
    for (const auto& pair : pImpl->nestedValues) {
        oss << "  " << pair.first << " = [";
        oss << jsonValueToString(pair.second) << "] (" << static_cast<int>(ConfigSource::Runtime) << ")\n";
    }
    
    return oss.str();
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& pair : pImpl->nestedValues) {
        if (!isValidKey(pair.first)) {
            pImpl->validationErrors.push_back("Invalid key format: " + pair.first);
        }
        
        // Check for duplicate keys
        auto it = std::find_if(pImpl->flatEntries.begin(), pImpl->flatEntries.end(),
            [&pair](const ConfigEntry& e) { return e.key == pair.first; });
        if (it != pImpl->flatEntries.end() && std::next(it) != pImpl->flatEntries.end() &&
            std::next(it)->key == pair.first) {
            pImpl->validationErrors.push_back("Duplicate key: " + pair.first);
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

std::string Config::getParentKey(const std::string& key) {
    size_t lastDot = key.find_last_of('.');
    if (lastDot == std::string::npos) {
        return "";
    }
    return key.substr(0, lastDot);
}

std::string Config::getChildKey(const std::string& parentKey, const std::string& childKey) {
    if (parentKey.empty()) {
        return childKey;
    }
    return parentKey + "." + childKey;
}

bool Config::isNestedKey(const std::string& key) {
    return key.find('.') != std::string::npos;
}

void Config::ensureNestedStructure(const std::string& key) const {
    // Ensure parent keys exist
    size_t lastDot = key.find_last_of('.');
    if (lastDot != std::string::npos) {
        std::string parentKey = key.substr(0, lastDot);
        if (pImpl->nestedValues.find(parentKey) == pImpl->nestedValues.end()) {
            // Create a dummy parent entry
            pImpl->nestedValues[parentKey] = std::string("_");
        }
    }
}

ConfigValue& Config::getNestedValue(const std::string& key, bool createIfMissing) {
    if (createIfMissing && pImpl->nestedValues.find(key) == pImpl->nestedValues.end()) {
        pImpl->nestedValues[key] = std::string("_");
    }
    return pImpl->nestedValues[key];
}

std::optional<ConfigValue> Config::getNestedValue(const std::string& key) const {
    auto it = pImpl->nestedValues.find(key);
    if (it == pImpl->nestedValues.end()) {
        return std::nullopt;
    }
    return it->second;
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

std::string Config::jsonValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "\"unknown\"";
        }
    }, value);
    return oss.str();
}

ConfigValue Config::parseJSONValue(const std::string& valueStr) {
    // Try to parse as different types
    if (valueStr == "true") return true;
    if (valueStr == "false") return false;
    
    try {
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) return intVal;
        
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) return doubleVal;
    } catch (...) {}
    
    return valueStr;
}

template<typename T>
std::string Config::valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<typename T>
std::string Config::valueToString(const std::vector<T>& value) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < value.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << value[i];
    }
    oss << "]";
    return oss.str();
}

bool Config::isValidKey(const std::string& key) {
    // Basic validation: no spaces, allowed characters
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Check for consecutive dots
    if (key.find("..") != std::string::npos) {
        return false;
    }
    
    return true;
}

std::string Config::normalizeKey(const std::string& key) {
    std::string normalized = key;
    
    // Convert to lowercase
    for (char& c : normalized) {
        c = std::tolower(c);
    }
    
    // Replace spaces with underscores
    for (char& c : normalized) {
        if (std::isspace(c)) {
            c = '_';
        }
    }
    
    // Remove invalid characters except dots, underscores, and hyphens
    std::string result;
    for (char c : normalized) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-') {
            result += c;
        }
    }
    
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm