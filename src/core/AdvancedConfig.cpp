#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigGroup> groups;
    std::shared_ptr<Config> parentConfig;
    std::unique_ptr<ConfigSchema> schema;
    std::map<std::string, std::string> keyToGroup;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Legacy support - try to detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Simple format detection
    if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
        // Try JSON first
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto jsonParser = ConfigParserFactory::createJSONParser();
        if (jsonParser && jsonParser->parse(content, *this)) {
            return true;
        }
    } else if (firstLine.find(':') != std::string::npos && (firstLine.find('-') != std::string::npos || firstLine.find('"') != std::string::npos)) {
        // Try YAML
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto yamlParser = ConfigParserFactory::createYAMLParser();
        if (yamlParser && yamlParser->parse(content, *this)) {
            return true;
        }
    }
    
    // Fall back to legacy format
    return legacyLoadFromFile(filepath);
}

bool Config::legacyLoadFromFile(const std::string& filepath) {
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

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
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

// Enhanced loading methods
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto jsonParser = ConfigParserFactory::createJSONParser();
    return jsonParser && jsonParser->parse(content, *this);
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    auto yamlParser = ConfigParserFactory::createYAMLParser();
    return yamlParser && yamlParser->parse(content, *this);
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    for (const auto& [key, value] : std::getenv()) {
        if (key.find(prefix) == 0) {
            std::string configKey = key.substr(prefix.length());
            set(configKey, value, ConfigSource::Environment);
        }
    }
    return true;
}

bool Config::loadFromMultipleFiles(const std::vector<std::string>& filepaths, bool override) {
    bool success = true;
    for (const auto& filepath : filepaths) {
        if (override) {
            // Save current state
            std::vector<ConfigEntry> savedEntries = pImpl->entries;
            pImpl->entries.clear();
            
            if (!loadFromFile(filepath)) {
                success = false;
            }
            
            // Restore non-overridden entries
            for (const auto& entry : savedEntries) {
                if (!has(entry.key) || entry.source == ConfigSource::File) {
                    pImpl->entries.push_back(entry);
                }
            }
        } else {
            if (!loadFromFile(filepath)) {
                success = false;
            }
        }
    }
    return success;
}

// Enhanced serialization
bool Config::saveToJSONFile(const std::string& filepath, bool pretty) const {
    auto jsonParser = ConfigParserFactory::createJSONParser();
    if (!jsonParser) return false;
    
    std::string jsonStr = jsonParser->serialize(*this, pretty);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << jsonStr;
    return true;
}

bool Config::saveToYAMLFile(const std::string& filepath, bool pretty) const {
    auto yamlParser = ConfigParserFactory::createYAMLParser();
    if (!yamlParser) return false;
    
    std::string yamlStr = yamlParser->serialize(*this, pretty);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    file << yamlStr;
    return true;
}

// Template get methods
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

// Group management
void Config::addGroup(const ConfigGroup& group) {
    pImpl->groups[group.name] = group;
    for (const auto& [key, entry] : group.entries) {
        pImpl->keyToGroup[key] = group.name;
    }
}

void Config::removeGroup(const std::string& name) {
    auto it = pImpl->groups.find(name);
    if (it != pImpl->groups.end()) {
        for (const auto& [key, entry] : it->second.entries) {
            pImpl->keyToGroup.erase(key);
        }
        pImpl->groups.erase(it);
    }
}

std::vector<std::string> Config::getGroupNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->groups.size());
    for (const auto& [name, group] : pImpl->groups) {
        names.push_back(name);
    }
    return names;
}

ConfigGroup* Config::getGroup(const std::string& name) {
    auto it = pImpl->groups.find(name);
    if (it != pImpl->groups.end()) {
        return &it->second;
    }
    return nullptr;
}

// Inheritance management
void Config::setParentConfig(std::shared_ptr<Config> parent) {
    pImpl->parentConfig = parent;
}

std::shared_ptr<Config> Config::getParentConfig() const {
    return pImpl->parentConfig;
}

// Schema management
void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = std::make_unique<ConfigSchema>(schema);
}

const ConfigSchema* Config::getSchema() const {
    return pImpl->schema.get();
}

// Configuration merging and overrides
void Config::merge(const Config& other, bool override) {
    for (const auto& entry : other.pImpl->entries) {
        if (!override && has(entry.key) && entry.source != ConfigSource::File) {
            continue;
        }
        set(entry.key, entry.value, entry.source);
    }
}

void Config::overrideWith(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

// Advanced queries
std::vector<ConfigEntry> Config::getEntriesInGroup(const std::string& groupName) const {
    std::vector<ConfigEntry> entries;
    auto groupIt = pImpl->groups.find(groupName);
    if (groupIt != pImpl->groups.end()) {
        for (const auto& [key, entry] : groupIt->second.entries) {
            entries.push_back(entry);
        }
    }
    return entries;
}

std::map<std::string, std::string> Config::getTagsForKey(const std::string& key) const {
    std::map<std::string, std::string> tags;
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (entryIt != pImpl->entries.end()) {
        for (const auto& tag : entryIt->tags) {
            tags[tag] = tag; // Simple implementation - tags could store key-value
        }
    }
    return tags;
}

std::string Config::getUnitForKey(const std::string& key) const {
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (entryIt != pImpl->entries.end()) {
        return entryIt->unit;
    }
    return "";
}

// Configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->groups.size() << " groups):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::any>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    std::visit([&oss](auto&& elem) {
                        using ET = std::decay_t<decltype(elem)>;
                        if constexpr (std::is_same_v<ET, std::string>) {
                            oss << "\"" << elem << "\"";
                        } else {
                            oss << elem;
                        }
                    }, arg[i]);
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") ";
        if (!entry.category.empty()) {
            oss << "[" << entry.category << "]";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::schemaSummary() const {
    if (!pImpl->schema) {
        return "No schema defined.\n";
    }
    
    std::ostringstream oss;
    oss << "Schema summary (" << pImpl->schema->constraints.size() << " constraints):\n";
    for (const auto& [key, constraint] : pImpl->schema->constraints) {
        oss << "  " << key << " : " << constraint.typeName;
        if (constraint.required) oss << " (required)";
        oss << "\n";
    }
    return oss.str();
}

// Configuration templates
void Config::saveAsTemplate(const std::string& filepath) const {
    saveToJSONFile(filepath, true);
}

bool Config::loadFromTemplate(const std::string& filepath) {
    return loadFromJSONFile(filepath);
}

// Configuration search
std::vector<std::string> Config::findKeysByTag(const std::string& tag) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (std::find(entry.tags.begin(), entry.tags.end(), tag) != entry.tags.end()) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::findKeysByPattern(const std::string& pattern) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (stringMatch(pattern, entry.key)) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Internal helpers
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

bool Config::stringMatch(const std::string& pattern, const std::string& str) {
    std::regex regex(pattern);
    return std::regex_match(str, regex);
}

// ConfigSchema implementation
void ConfigSchema::addConstraint(const std::string& key, const std::string& typeName,
                                 const std::vector<ValidationRule>& rules,
                                 const std::any& defaultValue,
                                 bool required) {
    TypeConstraint constraint;
    constraint.key = key;
    constraint.typeName = typeName;
    constraint.validationRules = rules;
    constraint.defaultValue = defaultValue;
    constraint.required = required;
    constraints[key] = constraint;
}

template<typename T>
void ConfigSchema::addTypeConstraint(const std::string& key, T defaultValue,
                                     bool required, const std::vector<ValidationRule>& rules) {
    addConstraint(key, getTypeName(std::any(defaultValue)), rules, defaultValue, required);
}

bool ConfigSchema::validate(const ConfigEntry& entry) const {
    auto it = constraints.find(entry.key);
    if (it == constraints.end()) {
        return true; // No constraint defined, pass validation
    }
    
    const auto& constraint = it->second;
    
    // Check type compatibility
    if (!isTypeCompatible(constraint.typeName, entry.value)) {
        return false;
    }
    
    // Check required field
    if (constraint.required && entry.value.index() == 0) { // std::string index 0
        return false;
    }
    
    // Run validation rules
    for (const auto& rule : constraint.validationRules) {
        if (!rule.validator(entry.value)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigSchema::validate(const Config& config) const {
    for (const auto& entry : config.pImpl->entries) {
        if (!validate(entry)) {
            return false;
        }
    }
    return true;
}

std::string ConfigSchema::getTypeName(const ConfigValue& value) const {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<int64_t>>) return "vector<int64_t>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<bool>>) return "vector<bool>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
        else if constexpr (std::is_same_v<T, std::vector<std::any>>) return "vector<any>";
        else return "unknown";
    }, value);
    return "unknown";
}

bool ConfigSchema::isTypeCompatible(const std::string& expectedType, const ConfigValue& value) const {
    std::string actualType = getTypeName(value);
    return actualType == expectedType || expectedType == "any" || expectedType == "auto";
}

// ConfigParserFactory implementation
std::unique_ptr<JSONConfigParser> ConfigParserFactory::createJSONParser() {
#ifdef HAS_JSON_SUPPORT
    return std::make_unique<JSONConfigParserImpl>();
#else
    return nullptr;
#endif
}

std::unique_ptr<YAMLConfigParser> ConfigParserFactory::createYAMLParser() {
#ifdef HAS_YAML_SUPPORT
    return std::make_unique<YAMLConfigParserImpl>();
#else
    return nullptr;
#endif
}

} // namespace nlm