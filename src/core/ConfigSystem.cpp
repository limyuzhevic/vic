#include "ConfigSystem.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iomanip>
#include <chrono>

namespace nlm {

struct ConfigSystem::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    std::vector<std::string> validationErrors;
    bool isLoaded;
    std::string lastLoadedFile;
    
    Impl() : isLoaded(false), lastLoadedFile() {}
};

ConfigSystem::ConfigSystem() : pImpl(std::make_unique<Impl>()) {}

ConfigSystem::~ConfigSystem() = default;

ConfigSystem::ConfigSystem(ConfigSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

ConfigSystem& ConfigSystem::operator=(ConfigSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool ConfigSystem::loadFromFile(const std::string& filepath, bool isYAML) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    bool success = isYAML ? loadFromString(buffer.str(), true) : loadFromString(buffer.str(), false);
    if (success) {
        pImpl->lastLoadedFile = filepath;
        pImpl->isLoaded = true;
    }
    
    return success;
}

bool ConfigSystem::loadFromString(const std::string& content, bool isYAML) {
    try {
        ConfigValue root;
        bool success = isYAML ? parseYAML(content, root) : parseJSON(content, root);
        if (!success) {
            return false;
        }
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Process the root object (assuming top-level object)
        if (std::holds_alternative<std::map<std::string, ConfigValue>>(root)) {
            auto& obj = std::get<std::map<std::string, ConfigValue>>(root);
            for (const auto& pair : obj) {
                ConfigValueToEntry(pair.first, pair.second, ConfigSource::File);
            }
        }
        
        pImpl->isLoaded = true;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigSystem::parseJSON(const std::string& content, ConfigValue& root) {
    try {
        // Simple JSON parsing (placeholder for nlohmann/json or similar)
        // For now, use basic parsing - in production, integrate with a proper JSON library
        size_t pos = 0;
        return parseJSONValue(content, pos, root);
    } catch (const std::exception&) {
        return false;
    }
}

bool ConfigSystem::parseYAML(const std::string& content, ConfigValue& root) {
    try {
        // Simple YAML parsing (placeholder for YAML::Parser or similar)
        // For now, use basic parsing - in production, integrate with a proper YAML library
        size_t pos = 0;
        return parseYAMLValue(content, pos, root);
    } catch (const std::exception&) {
        return false;
    }
}

template<typename T>
std::optional<T> ConfigSystem::get(const std::string& key) const {
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
T ConfigSystem::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void ConfigSystem::set(const std::string& key, const ConfigValue& value, 
                       ConfigSource source, const std::string& description) {
    addEntry(ConfigEntry(key, value, source, description));
}

void ConfigSystem::set(const std::string& key, int value, ConfigSource source,
                       const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void ConfigSystem::set(const std::string& key, double value, ConfigSource source,
                       const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void ConfigSystem::set(const std::string& key, bool value, ConfigSource source,
                       const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void ConfigSystem::set(const std::string& key, const std::string& value, 
                       ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

bool ConfigSystem::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void ConfigSystem::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> ConfigSystem::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void ConfigSystem::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
}

void ConfigSystem::addSchema(const std::string& key, const ConfigSchema& schema) {
    pImpl->schemas[key] = schema;
}

bool ConfigSystem::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        if (!validateSchema(entry.value, pImpl->schemas[entry.key])) {
            pImpl->validationErrors.push_back("Validation failed for key: " + entry.key);
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> ConfigSystem::getValidationErrors() const {
    return pImpl->validationErrors;
}

void ConfigSystem::merge(const ConfigSystem& other, bool overwriteExisting) {
    for (const auto& entry : other.pImpl->entries) {
        if (!overwriteExisting && has(entry.key)) {
            continue;
        }
        addEntry(entry);
    }
}

bool ConfigSystem::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool ConfigSystem::loadFromEnvironment(const std::string& prefix) {
    // Simple implementation - in production, use proper environment variable parsing
    return true;
}

std::string ConfigSystem::summary() const {
    std::ostringstream oss;
    oss << "Configuration System (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << sourceToString(entry.source) << ")\n";
    }
    return oss.str();
}

std::optional<ConfigEntry> ConfigSystem::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return *it;
}

ConfigSource ConfigSystem::getSource(const std::string& key) const {
    auto entry = getEntry(key);
    return entry ? entry->source : ConfigSource::Default;
}

// Implementation helpers
bool ConfigSystem::parseJSONValue(const std::string& content, size_t& pos, ConfigValue& value) {
    // Simple JSON parser (placeholder - should use nlohmann/json)
    // Skip whitespace
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
    
    if (pos >= content.size()) return false;
    
    char ch = content[pos];
    
    if (ch == '{') {
        std::map<std::string, ConfigValue> obj;
        pos++; // skip {
        while (pos < content.size() && content[pos] != '}') {
            std::string key;
            size_t keyEnd = content.find('"', pos);
            if (keyEnd == std::string::npos) return false;
            key = content.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size() || content[pos] != ':') return false;
            pos++; // skip :
            
            ConfigValue val;
            if (parseJSONValue(content, pos, val)) {
                obj[key] = val;
            }
            
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos < content.size() && content[pos] == ',') {
                pos++; // skip ,
            }
        }
        if (pos < content.size()) pos++; // skip }
        value = obj;
        return true;
    }
    else if (ch == '[') {
        std::vector<ConfigValue> arr;
        pos++; // skip [
        while (pos < content.size() && content[pos] != ']') {
            ConfigValue val;
            if (parseJSONValue(content, pos, val)) {
                arr.push_back(val);
            }
            
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos < content.size() && content[pos] == ',') {
                pos++; // skip ,
            }
        }
        if (pos < content.size()) pos++; // skip ]
        value = arr;
        return true;
    }
    else if (ch == '"') {
        size_t end = content.find('"', pos + 1);
        if (end == std::string::npos) return false;
        value = content.substr(pos + 1, end - pos - 1);
        pos = end + 1;
        return true;
    }
    else if (ch == 't' || ch == 'f') {
        // Handle true/false
        if (content.substr(pos, 4) == "true") {
            value = true;
            pos += 4;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            value = false;
            pos += 5;
            return true;
        }
    }
    else if (ch == 'n') {
        // Handle null
        if (content.substr(pos, 4) == "null") {
            pos += 4;
            return false; // null not supported
        }
    }
    else if (std::isdigit(ch) || ch == '-') {
        // Handle numbers
        size_t end = content.find_first_of(",] }", pos);
        if (end == std::string::npos) end = content.size();
        std::string numStr = content.substr(pos, end - pos);
        
        try {
            if (numStr.find('.') != std::string::npos) {
                value = std::stod(numStr);
            } else {
                // Check if it fits in int64_t
                try {
                    value = std::stoll(numStr);
                } catch (...) {
                    value = std::stod(numStr);
                }
            }
            pos = end;
            return true;
        } catch (...) {}
    }
    
    return false;
}

bool ConfigSystem::parseYAMLValue(const std::string& content, size_t& pos, ConfigValue& value) {
    // Simple YAML parser (placeholder - should use YAML::Parser)
    // For now, delegate to JSON parser for common cases
    return parseJSONValue(content, pos, value);
}

std::string ConfigSystem::sourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "DEFAULT";
        case ConfigSource::File: return "FILE";
        case ConfigSource::CommandLine: return "COMMAND_LINE";
        case ConfigSource::Runtime: return "RUNTIME";
        case ConfigSource::Environment: return "ENVIRONMENT";
        default: return "UNKNOWN";
    }
}

void ConfigSystem::addEntry(const ConfigEntry& entry) {
    remove(entry.key); // Replace if exists
    pImpl->entries.push_back(entry);
}

bool ConfigSystem::hasEntry(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

const ConfigEntry& ConfigSystem::getEntryInternal(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        throw std::runtime_error("Key not found: " + key);
    }
    
    return *it;
}

bool ConfigSystem::validateSchema(const ConfigValue& value, const ConfigSchema& schema) const {
    if (schema.required && value.index() == 0) return false;  // Invalid/empty value
    
    // Implementation depends on schema type and value type
    // For now, basic validation
    return true;
}

bool ConfigSystem::validateAgainstSchema(const std::string& key) const {
    auto entryOpt = getEntry(key);
    if (!entryOpt) return false;
    
    const auto& entry = *entryOpt;
    if (pImpl->schemas.find(key) == pImpl->schemas.end()) return true;  // No schema to validate against
    
    return validateSchema(entry.value, pImpl->schemas[key]);
}

} // namespace nlm