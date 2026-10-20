#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <numeric>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
    std::unordered_map<std::string, ConfigValue> defaults;
    std::unordered_map<std::string, std::string> categories;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> types;
    std::unordered_map<std::string, std::pair<double, double>> validations;
    std::unordered_map<std::string, bool> requireds;
    std::unordered_map<std::string, std::vector<std::string>> allowedValues;
    std::unordered_map<std::string, std::string> metadata;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::Config(const ConfigSchema& schema) : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = schema;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool autoDetect) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    // Auto-detect file format
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (autoDetect) {
        if (extension == ".json" || extension == ".jsonc") {
            return loadFromJSON(filepath);
        } else {
            return loadFromSimple(filepath);
        }
    }
    
    // Try JSON first, then fallback to simple format
    if (loadFromJSON(filepath)) {
        return true;
    }
    return loadFromSimple(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing (not full implementation - placeholder for real JSON parser)
    // For now, we'll just parse a very simple subset
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Very basic JSON parsing for demonstration
    // In a real implementation, you would use a proper JSON parser
    size_t pos = 0;
    while (pos < content.size()) {
        // Skip whitespace
        while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= content.size()) break;
        
        // Check for object start
        if (content[pos] == '{') {
            // Parse object - simplified implementation
            pos++;
            while (pos < content.size() && content[pos] != '}') {
                // Skip key
                while (pos < content.size() && content[pos] != '"') pos++;
                if (pos >= content.size()) break;
                pos++; // Skip opening quote
                std::string key;
                while (pos < content.size() && content[pos] != '"') {
                    key += content[pos];
                    pos++;
                }
                if (pos < content.size()) pos++; // Skip closing quote
                
                // Skip whitespace and colon
                while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n')) pos++;
                if (pos < content.size() && content[pos] == ':') pos++;
                
                // Skip whitespace
                while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n')) pos++;
                
                // Parse value (simplified - just handle basic types)
                if (pos < content.size() && content[pos] == '"') {
                    // String
                    pos++;
                    std::string value;
                    while (pos < content.size() && content[pos] != '"') {
                        value += content[pos];
                        pos++;
                    }
                    if (pos < content.size()) pos++; // Skip closing quote
                    set(key, value, ConfigSource::JSON);
                } else if (pos < content.size() && (content[pos] == 't' || content[pos] == 'f')) {
                    // Boolean
                    std::string boolStr;
                    while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                        boolStr += content[pos];
                        pos++;
                    }
                    if (boolStr == "true") {
                        set(key, true, ConfigSource::JSON);
                    } else if (boolStr == "false") {
                        set(key, false, ConfigSource::JSON);
                    }
                } else if (pos < content.size() && (content[pos] >= '0' && content[pos] <= '9') || content[pos] == '-') {
                    // Number
                    std::string numStr;
                    while (pos < content.size() && (content[pos] >= '0' && content[pos] <= '9') || content[pos] == '.' || content[pos] == 'e' || content[pos] == 'E' || content[pos] == '+' || content[pos] == '-') {
                        numStr += content[pos];
                        pos++;
                    }
                    try {
                        double val = std::stod(numStr);
                        set(key, val, ConfigSource::JSON);
                    } catch (...) {
                        set(key, numStr, ConfigSource::JSON);
                    }
                } else {
                    // Skip value
                    while (pos < content.size() && content[pos] != ',' && content[pos] != '}') pos++;
                }
                
                // Skip comma and whitespace
                while (pos < content.size() && (content[pos] == ',' || content[pos] == ' ' || content[pos] == '\n')) pos++;
            }
            if (pos < content.size() && content[pos] == '}') pos++;
        } else {
            // Not a JSON object, try simple format
            file.clear();
            file.seekg(0);
            return loadFromSimple(filepath);
        }
    }
    
    return true;
}

bool Config::loadFromSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (isCommentOrEmpty(line)) {
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
            
            try {
                // Auto-detect type and convert
                set(key, fromString(value), ConfigSource::File);
            } catch (const std::exception& e) {
                throw ConfigParseError(std::string("Failed to parse simple config value for key '") + key + "': " + e.what(), filepath, lineNumber);
            }
        } else {
            throw ConfigParseError(std::string("Invalid config line (no '='): ") + line, filepath, lineNumber);
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
                set(key, fromString(value), ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, fromString(value), ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJSON(filepath, pretty);
    } else {
        return saveToSimple(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON serialization
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            file << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            file << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            file << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            file << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            file << "\"" << escapeString(std::get<std::string>(entry.value)) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<int>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << vec[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<double>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << vec[j];
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << escapeString(vec[j]) << "\"";
            }
            file << "]";
        } else if (std::holds_alternative<std::vector<bool>>(entry.value)) {
            file << "[";
            const auto& vec = std::get<std::vector<bool>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) file << ", ";
                file << (vec[j] ? "true" : "false");
            }
            file << "]";
        }
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    
    return true;
}

bool Config::saveToSimple(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Write description if available
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write key=value pair
        file << entry.key << " = " << toString(entry.value) << "\n\n";
    }
    
    return true;
}

std::optional<int> Config::getInt(const std::string& key) const {
    auto val = get<int>(key);
    return val;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto val = get<double>(key);
    return val;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto val = get<bool>(key);
    return val;
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto val = get<std::string>(key);
    return val;
}

std::optional<std::vector<int>> Config::getIntVector(const std::string& key) const {
    auto val = get<std::vector<int>>(key);
    return val;
}

std::optional<std::vector<double>> Config::getDoubleVector(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val;
}

std::optional<std::vector<std::string>> Config::getStringVector(const std::string& key) const {
    auto val = get<std::vector<std::string>>(key);
    return val;
}

std::optional<std::vector<bool>> Config::getBoolVector(const std::string& key) const {
    auto val = get<std::vector<bool>>(key);
    return val;
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
    // Validate before setting
    std::string type = "auto";
    if (pImpl->types.find(key) != pImpl->types.end()) {
        type = pImpl->types[key];
    } else {
        type = getTypeFromValue(value);
    }
    
    if (!validateValue(value, type, 
                      pImpl->validations.count(key) ? pImpl->validations[key].first : std::numeric_limits<double>::lowest(),
                      pImpl->validations.count(key) ? pImpl->validations[key].second : std::numeric_limits<double>::max())) {
        throw ConfigValidationError("Validation failed for key '" + key + "' with value '" + toString(value) + "'");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry entry;
    if (it != pImpl->entries.end()) {
        entry = *it;
        entry.value = value;
        entry.source = source;
        entry.timestamp = std::chrono::system_clock::now();
        *it = entry;
    } else {
        entry = ConfigEntry(key, value, source);
        entry.type = type;
        if (pImpl->categories.find(key) != pImpl->categories.end()) {
            entry.category = pImpl->categories[key];
        }
        if (pImpl->descriptions.find(key) != pImpl->descriptions.end()) {
            entry.description = pImpl->descriptions[key];
        }
        if (pImpl->requireds.find(key) != pImpl->requireds.end()) {
            entry.required = pImpl->requireds[key];
        }
        if (pImpl->validations.find(key) != pImpl->validations.end()) {
            entry.minValue = pImpl->validations[key].first;
            entry.maxValue = pImpl->validations[key].second;
        }
        if (pImpl->allowedValues.find(key) != pImpl->allowedValues.end()) {
            entry.allowedValues = pImpl->allowedValues[key];
        }
        if (pImpl->metadata.find(key) != pImpl->metadata.end()) {
            entry.metadata = pImpl->metadata[key];
        }
        pImpl->entries.push_back(entry);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, fromString(value), source);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setCategory(const std::string& key, const std::string& category) {
    if (has(key)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            it->category = category;
        }
    }
    pImpl->categories[key] = category;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    if (has(key)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            it->description = description;
        }
    }
    pImpl->descriptions[key] = description;
}

void Config::setType(const std::string& key, const std::string& type) {
    pImpl->types[key] = type;
}

void Config::setDefault(const std::string& key, const ConfigValue& value) {
    pImpl->defaults[key] = value;
}

void Config::setRequired(const std::string& key, bool required) {
    pImpl->requireds[key] = required;
}

void Config::setValidation(const std::string& key, const std::string& regex, double min, double max) {
    pImpl->validations[key] = std::make_pair(min, max);
    if (!regex.empty()) {
        pImpl->types[key] = "string";
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isRequired(const std::string& key) const {
    if (pImpl->requireds.find(key) != pImpl->requireds.end()) {
        return pImpl->requireds[key];
    }
    
    // Check in entries
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->required;
    }
    
    return false;
}

std::string Config::getCategory(const std::string& key) const {
    if (pImpl->categories.find(key) != pImpl->categories.end()) {
        return pImpl->categories[key];
    }
    
    // Check in entries
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->category;
    }
    
    return "general";
}

std::string Config::getDescription(const std::string& key) const {
    if (pImpl->descriptions.find(key) != pImpl->descriptions.end()) {
        return pImpl->descriptions[key];
    }
    
    // Check in entries
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "";
}

std::string Config::getType(const std::string& key) const {
    if (pImpl->types.find(key) != pImpl->types.end()) {
        return pImpl->types[key];
    }
    
    // Check in entries
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->type;
    }
    
    return "auto";
}

void Config::remove(const std::string& key) {
    // Remove from entries
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Remove from metadata
    pImpl->categories.erase(key);
    pImpl->descriptions.erase(key);
    pImpl->types.erase(key);
    pImpl->requireds.erase(key);
    pImpl->validations.erase(key);
    pImpl->allowedValues.erase(key);
    pImpl->metadata.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysInCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->defaults.clear();
    pImpl->categories.clear();
    pImpl->descriptions.clear();
    pImpl->types.clear();
    pImpl->requireds.clear();
    pImpl->validations.clear();
    pImpl->allowedValues.clear();
    pImpl->metadata.clear();
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
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << (arg[i] ? "true" : "false");
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.category.empty() && entry.category != "general") {
            oss << " [" << entry.category << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::summaryInCategory(const std::string& category) const {
    std::ostringstream oss;
    oss << "Configuration in category '" << category << "' (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
    }
    return oss.str();
}

bool Config::validate() const {
    std::string errors;
    for (const auto& entry : pImpl->entries) {
        std::string error = validateKey(entry.key);
        if (!error.empty()) {
            if (!errors.empty()) errors += "\n";
            errors += "Key '" + entry.key + "': " + error;
        }
    }
    
    if (!errors.empty()) {
        throw ConfigValidationError("Configuration validation failed:\n" + errors);
    }
    
    return true;
}

std::string Config::validateKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "Key not found";
    }
    
    // Validate type
    std::string type = getType(key);
    if (type != "auto") {
        ConfigValue value = it->value;
        if (!validateValue(value, type, it->minValue, it->maxValue)) {
            return "Type validation failed";
        }
    }
    
    // Validate against allowed values
    if (pImpl->allowedValues.find(key) != pImpl->allowedValues.end()) {
        const auto& allowed = pImpl->allowedValues[key];
        std::string strValue = toString(it->value);
        if (std::find(allowed.begin(), allowed.end(), strValue) == allowed.end()) {
            return "Value not in allowed list";
        }
    }
    
    // Validate required
    if (isRequired(key)) {
        if (!has(key)) {
            return "Required key is missing";
        }
    }
    
    return "";
}

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

void Config::applyDefaults() {
    for (const auto& defaultPair : pImpl->defaults) {
        if (!has(defaultPair.first)) {
            set(defaultPair.first, defaultPair.second, ConfigSource::Default);
        }
    }
}

bool Config::hasDefault(const std::string& key) const {
    return pImpl->defaults.find(key) != pImpl->defaults.end();
}

ConfigValue Config::getDefault(const std::string& key) const {
    if (pImpl->defaults.find(key) != pImpl->defaults.end()) {
        return pImpl->defaults.at(key);
    }
    throw ConfigError("No default value for key: " + key);
}

void Config::copyFrom(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

bool Config::mergeFromFile(const std::string& filepath, bool overwrite) {
    if (!loadFromFile(filepath, false)) {
        return false;
    }
    
    // Merge logic would go here
    return true;
}

void Config::setFromEnvironment(const std::string& prefix) {
    // Implementation for reading from environment variables
    for (const auto& entry : pImpl->entries) {
        std::string envKey = prefix + entry.key;
        const char* envValue = std::getenv(envKey.c_str());
        if (envValue != nullptr) {
            try {
                set(entry.key, fromString(std::string(envValue)), ConfigSource::Environment);
            } catch (const std::exception& e) {
                // Ignore invalid environment values
            }
        }
    }
}

bool Config::saveToEnv(const std::string& prefix) const {
    // Implementation for writing to environment variables
    for (const auto& entry : pImpl->entries) {
        std::string envKey = prefix + entry.key;
        const char* existing = std::getenv(envKey.c_str());
        if (existing != nullptr) {
            continue; // Skip if already set
        }
        
        std::string envValue = toString(entry.value);
        // Note: Can't actually set environment variables at runtime
        // This is a placeholder for configuration export
    }
    return true;
}

std::string Config::toJSON(bool pretty) const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        oss << "  \"" << entry.key << "\": ";
        
        if (std::holds_alternative<int>(entry.value)) {
            oss << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            oss << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            oss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            oss << (std::get<bool>(entry.value) ? "true" : "false");
        } else if (std::holds_alternative<std::string>(entry.value)) {
            oss << "\"" << escapeString(std::get<std::string>(entry.value)) << "\"";
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<int>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) oss << ", ";
                oss << vec[j];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<double>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) oss << ", ";
                oss << vec[j];
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) oss << ", ";
                oss << "\"" << escapeString(vec[j]) << "\"";
            }
            oss << "]";
        } else if (std::holds_alternative<std::vector<bool>>(entry.value)) {
            oss << "[";
            const auto& vec = std::get<std::vector<bool>>(entry.value);
            for (size_t j = 0; j < vec.size(); ++j) {
                if (j > 0) oss << ", ";
                oss << (vec[j] ? "true" : "false");
            }
            oss << "]";
        }
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    oss << "}\n";
    
    return oss.str();
}

bool Config::fromJSON(const std::string& jsonStr) {
    // Try to parse as a simple JSON object (for demonstration)
    // This is a very simplified JSON parser - real implementation would use a proper JSON library
    
    // Very basic parsing for demonstration only
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Skip whitespace
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n' || jsonStr[pos] == '\t' || jsonStr[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= jsonStr.size()) break;
        
        if (jsonStr[pos] == '"') {
            // Parse key
            pos++;
            std::string key;
            while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                key += jsonStr[pos];
                pos++;
            }
            if (pos < jsonStr.size() && jsonStr[pos] == '"') pos++;
            
            // Skip whitespace
            while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n')) pos++;
            
            if (pos < jsonStr.size() && jsonStr[pos] == ':') pos++;
            
            // Skip whitespace
            while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n')) pos++;
            
            // Parse value
            if (pos < jsonStr.size() && jsonStr[pos] == '"') {
                // String value
                pos++;
                std::string value;
                while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                    value += jsonStr[pos];
                    pos++;
                }
                if (pos < jsonStr.size() && jsonStr[pos] == '"') pos++;
                set(key, value, ConfigSource::JSON);
            } else if (pos < jsonStr.size() && (jsonStr[pos] == 't' || jsonStr[pos] == 'f')) {
                // Boolean value
                std::string boolStr;
                while (pos < jsonStr.size() && jsonStr[pos] != ',' && jsonStr[pos] != '}' && jsonStr[pos] != ' ') {
                    boolStr += jsonStr[pos];
                    pos++;
                }
                if (boolStr == "true") {
                    set(key, true, ConfigSource::JSON);
                } else if (boolStr == "false") {
                    set(key, false, ConfigSource::JSON);
                }
            } else if (pos < jsonStr.size() && (jsonStr[pos] >= '0' && jsonStr[pos] <= '9') || jsonStr[pos] == '-') {
                // Number value
                std::string numStr;
                while (pos < jsonStr.size() && (jsonStr[pos] >= '0' && jsonStr[pos] <= '9') || jsonStr[pos] == '.' || jsonStr[pos] == 'e' || jsonStr[pos] == 'E' || jsonStr[pos] == '+' || jsonStr[pos] == '-') {
                    numStr += jsonStr[pos];
                    pos++;
                }
                try {
                    double val = std::stod(numStr);
                    set(key, val, ConfigSource::JSON);
                } catch (...) {
                    set(key, numStr, ConfigSource::JSON);
                }
            } else {
                // Skip value
                while (pos < jsonStr.size() && jsonStr[pos] != ',' && jsonStr[pos] != '}') pos++;
            }
            
            // Skip comma and whitespace
            while (pos < jsonStr.size() && (jsonStr[pos] == ',' || jsonStr[pos] == ' ' || jsonStr[pos] == '\n')) pos++;
        } else {
            break;
        }
    }
    
    return true;
}

std::string Config::toSimple(bool comments) const {
    std::ostringstream oss;
    oss << "# NLM Configuration File\n";
    oss << "# Generated on " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        // Write description if available and comments enabled
        if (comments && !entry.description.empty()) {
            oss << "# " << entry.description << "\n";
        }
        
        // Write category if not general
        if (comments && entry.category != "general") {
            oss << "# Category: " << entry.category << "\n";
        }
        
        // Write key=value pair
        oss << entry.key << " = " << toString(entry.value) << "\n\n";
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

bool Config::isCommentOrEmpty(const std::string& line) {
    if (line.empty()) return true;
    return (line[0] == '#' || line[0] == ';');
}

std::string Config::escapeString(const std::string& str) {
    std::string result;
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

std::string Config::unescapeString(const std::string& str) {
    std::string result;
    bool escaped = false;
    
    for (char c : str) {
        if (escaped) {
            switch (c) {
                case '\"': result += '\"'; break;
                case '\\': result += '\\'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += c; break;
            }
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else {
            result += c;
        }
    }
    
    return result;
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue Config::fromString(const std::string& str, const std::string& type) {
    // Try to detect type from content if not specified
    if (type == "auto") {
        if (str.empty()) {
            return std::string();
        } else if (str == "true" || str == "false") {
            return str == "true";
        } else if (str.find('.') != std::string::npos || str.find('e') != std::string::npos || str.find('E') != std::string::npos) {
            try {
                return std::stod(str);
            } catch (...) {
                return str;
            }
        } else {
            try {
                // Try int first
                size_t pos;
                long val = std::stol(str, &pos);
                if (pos == str.size()) {
                    return val;
                }
                // Try int64_t
                long long val64 = std::stoll(str, &pos);
                if (pos == str.size()) {
                    return val64;
                }
            } catch (...) {
                // Fall through to string
            }
            return str;
        }
    } else if (type == "int") {
        return std::stoi(str);
    } else if (type == "double" || type == "float") {
        return std::stod(str);
    } else if (type == "bool") {
        std::string lower = toLower(str);
        if (lower == "true" || lower == "yes" || lower == "1") {
            return true;
        } else if (lower == "false" || lower == "no" || lower == "0") {
            return false;
        }
        throw ConfigError("Invalid boolean value: " + str);
    } else if (type == "string") {
        return str;
    } else if (type == "int_vector" || type == "vector<int>") {
        // Simple parsing for comma-separated integers
        std::vector<int> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            if (!token.empty()) {
                try {
                    result.push_back(std::stoi(token));
                } catch (...) {
                    throw ConfigError("Invalid integer in vector: " + token);
                }
            }
        }
        return result;
    }
    
    return str; // Default to string
}

bool Config::validateValue(const ConfigValue& value, const std::string& type, double min, double max) {
    // Type validation
    if (type == "int") {
        if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) {
            return false;
        }
        double val = std::visit([](auto&& arg) { return static_cast<double>(arg); }, value);
        return val >= min && val <= max;
    } else if (type == "double" || type == "float") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } else if (type == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "int_vector" || type == "vector<int>") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (type == "auto") {
        return true; // Basic validation only for auto type
    }
    
    return false;
}

std::string Config::getTypeFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return "int";
    } else if (std::holds_alternative<int64_t>(value)) {
        return "int64_t";
    } else if (std::holds_alternative<double>(value)) {
        return "double";
    } else if (std::holds_alternative<bool>(value)) {
        return "bool";
    } else if (std::holds_alternative<std::string>(value)) {
        return "string";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return "int_vector";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return "double_vector";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return "string_vector";
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        return "bool_vector";
    }
    
    return "auto";
}

} // namespace nlm