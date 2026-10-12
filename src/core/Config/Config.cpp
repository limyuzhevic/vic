#include "Config.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <iomanip>
#include <sstream>
#include <regex>

using json = nlohmann::json;
using namespace std;

namespace nlm {

struct Config::Impl {
    // Configuration storage
    std::map<std::string, ConfigValue> flatEntries;
    std::map<std::vector<std::string>, ConfigValue> nestedEntries;
    std::map<std::string, ConfigSource> sources;
    std::map<std::string, std::string> descriptions;
    
    // Environment and interpolation
    std::string environment = "default";
    bool interpolationEnabled = false;
    std::map<std::string, std::string> interpolationReferences;
    
    // Validation
    std::map<std::string, SchemaValidator> validators;
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, ConfigTemplate> templates;
    
    // Logging
    std::function<void(const std::string&)> logger = [](const std::string& msg) {};
    
    // Error collection
    mutable std::vector<std::string> errors;
    
    // Environment configs
    std::map<std::string, EnvironmentConfig> environmentConfigs;
    
    // Mutex for thread safety
    mutable std::mutex mutex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// Static factory methods
Config Config::fromFile(const std::string& filepath) {
    Config config;
    if (config.loadFromFile(filepath)) {
        return config;
    }
    return Config();
}

Config Config::fromArgs(int argc, char** argv) {
    Config config;
    config.loadFromArgs(argc, argv);
    return config;
}

Config Config::fromEnvironment(const std::string& envName) {
    Config config;
    config.setEnvironment(envName);
    config.loadFromEnvironment("NLM_");
    return config;
}

Config Config::fromTemplate(const ConfigTemplate& template) {
    Config config;
    config.addTemplate(template);
    config.applyTemplate(template.name);
    return config;
}

// Enhanced file loading with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Check file extension for format detection
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return loadFromJsonFile(filepath, source);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYamlFile(filepath, source);
        } else {
            // Fallback to simple key=value format
            pImpl->logger("Unsupported file format: " + ext + ". Using simple format.")
            return loadFromSimpleFile(filepath, source);
        }
    } catch (const std::exception& e) {
        pImpl->logger("Error loading config from " + filepath + ": " + e.what());
        return false;
    }
}

// Helper methods for file I/O
bool Config::loadFromSimpleFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->logger("Could not open config file: " + filepath);
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
            
            set(key, value, source);
        }
    }
    return true;
}

bool Config::loadFromJsonFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->logger("Could not open JSON config file: " + filepath);
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Handle nested keys (e.g., "brain.neuron_count")
            if (key.find('.') != std::string::npos) {
                std::vector<std::string> keyPath = splitPath(key);
                set(keyPath, parseJsonValue(it.value()), source);
            } else {
                set(key, parseJsonValue(it.value()), source);
            }
        }
        return true;
    } catch (const json::exception& e) {
        pImpl->logger("JSON parsing error in " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath, ConfigSource source) {
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            if (key.find('.') != std::string::npos) {
                std::vector<std::string> keyPath = splitPath(key);
                set(keyPath, yamlToConfigValue(pair.second), source);
            } else {
                set(key, yamlToConfigValue(pair.second), source);
            }
        }
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->logger("YAML parsing error in " + filepath + ": " + e.what());
        return false;
    }
}

// Environment support
void Config::loadFromEnvironment(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& envVar : std::unordered_map<std::string, std::string>()) {
        if (envVar.first.rfind(prefix, 0) == 0) {
            std::string key = envVar.first.substr(prefix.length());
            std::string value = envVar.second;
            
            // Try to infer type
            try {
                if (std::regex_match(value, std::regex("^true$|^false$"))) {
                    set(key, value == "true", ConfigSource::Environment);
                } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
                    set(key, std::stoi(value), ConfigSource::Environment);
                } else if (std::regex_match(value, std::regex("^-?\d+\.\d+$"))) {
                    set(key, std::stod(value), ConfigSource::Environment);
                } else {
                    set(key, value, ConfigSource::Environment);
                }
            } catch (...) {
                set(key, value, ConfigSource::Environment);
            }
        }
    }
}

void Config::setEnvironment(const std::string& envName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->environment = envName;
    
    // Apply environment-specific overrides if defined
    auto it = pImpl->environmentConfigs.find(envName);
    if (it != pImpl->environmentConfigs.end()) {
        for (const auto& pair : it->second.overrides) {
            set(pair.first, pair.second, ConfigSource::Environment);
        }
    }
}

std::string Config::getEnvironment() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->environment;
}

bool Config::isDevelopment() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = pImpl->environmentConfigs.find(pImpl->environment);
    return it != pImpl->environmentConfigs.end() && it->second.isDevelopment;
}

// Enhanced get methods with type safety
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->flatEntries.find(key);
    if (it == pImpl->flatEntries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        pImpl->logger("Type mismatch for key " + key + ". Expected " + typeid(T).name() + " but got different type.");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Nested key support
template<typename T>
std::optional<T> Config::get(const std::vector<std::string>& keyPath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->nestedEntries.find(keyPath);
    if (it == pImpl->nestedEntries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        pImpl->logger("Type mismatch for nested key " + joinPath(keyPath));
        return std::nullopt;
    }
}

// Enhanced set methods
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flatEntries[key] = value;
    pImpl->sources[key] = source;
}

void Config::set(const std::vector<std::string>& keyPath, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->nestedEntries[keyPath] = value;
    std::string flatKey = joinPath(keyPath);
    pImpl->sources[flatKey] = source;
    pImpl->flatEntries[flatKey] = value;
}

// Existence checks
bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->flatEntries.find(key) != pImpl->flatEntries.end();
}

bool Config::has(const std::vector<std::string>& keyPath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->nestedEntries.find(keyPath) != pImpl->nestedEntries.end();
}

// Remove keys
void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flatEntries.erase(key);
    pImpl->sources.erase(key);
}

void Config::remove(const std::vector<std::string>& keyPath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->nestedEntries.erase(keyPath);
    std::string flatKey = joinPath(keyPath);
    pImpl->flatEntries.erase(flatKey);
    pImpl->sources.erase(flatKey);
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->flatEntries.size() + pImpl->nestedEntries.size());
    
    for (const auto& pair : pImpl->flatEntries) {
        keys.push_back(pair.first);
    }
    
    for (const auto& pair : pImpl->nestedEntries) {
        keys.push_back(joinPath(pair.first));
    }
    
    return keys;
}

std::vector<std::string> Config::getAllKeys() const {
    return getKeys();
}

std::vector<std::string> Config::getKeysWithPrefix(const std::string& prefix) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    
    for (const auto& pair : pImpl->flatEntries) {
        if (pair.first.rfind(prefix, 0) == 0) {
            keys.push_back(pair.first);
        }
    }
    
    for (const auto& pair : pImpl->nestedEntries) {
        std::string key = joinPath(pair.first);
        if (key.rfind(prefix, 0) == 0) {
            keys.push_back(key);
        }
    }
    
    return keys;
}

// Clear all
void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->flatEntries.clear();
    pImpl->nestedEntries.clear();
    pImpl->sources.clear();
    pImpl->descriptions.clear();
}

// Validation framework
void Config::addValidator(const std::string& key, SchemaValidator validator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->validators[key] = validator;
}

ValidationResult Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
    
    // Validate flat entries
    for (const auto& pair : pImpl->flatEntries) {
        ValidationResult result = validate(pair.first);
        if (!result.valid) {
            pImpl->errors.push_back(result.error);
        }
    }
    
    // Validate nested entries
    for (const auto& pair : pImpl->nestedEntries) {
        std::string key = joinPath(pair.first);
        ValidationResult result = validate(key);
        if (!result.valid) {
            pImpl->errors.push_back(result.error);
        }
    }
    
    return pImpl->errors.empty() ? ValidationResult::success() : 
           ValidationResult::error("Configuration validation failed: " + pImpl->errors[0]);
}

ValidationResult Config::validate(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find the actual value
    ConfigValue value;
    bool found = false;
    
    auto it = pImpl->flatEntries.find(key);
    if (it != pImpl->flatEntries.end()) {
        value = it->second;
        found = true;
    } else {
        // Check nested entries
        for (const auto& pair : pImpl->nestedEntries) {
            if (joinPath(pair.first) == key) {
                value = pair.second;
                found = true;
                break;
            }
        }
    }
    
    if (!found) {
        return ValidationResult::error("Key not found: " + key, key);
    }
    
    // Apply registered validators
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        return validatorIt->second(key, value);
    }
    
    return ValidationResult::success();
}

// Schema validation
def
void Config::setSchema(const ConfigSchema& schema) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schemas.clear();
    pImpl->schemas = schema;
}

ValidationResult Config::validateSchema() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errors.clear();
    
    for (const auto& pair : pImpl->schemas) {
        const std::string& key = pair.first;
        const ConfigSchema& schema = pair.second;
        
        // Find the value
        ConfigValue value;
        bool found = false;
        
        auto it = pImpl->flatEntries.find(key);
        if (it != pImpl->flatEntries.end()) {
            value = it->second;
            found = true;
        }
        
        if (!found && key.find('.') != std::string::npos) {
            std::vector<std::string> keyPath = splitPath(key);
            auto nestedIt = pImpl->nestedEntries.find(keyPath);
            if (nestedIt != pImpl->nestedEntries.end()) {
                value = nestedIt->second;
                found = true;
            }
        }
        
        if (!found) {
            // Check if required
            auto requiredIt = schema.find("required");
            if (requiredIt != schema.end() && std::get<bool>(requiredIt->second)) {
                pImpl->errors.push_back("Required key missing: " + key);
            }
            continue;
        }
        
        // Validate type
        auto typeIt = schema.find("type");
        if (typeIt != schema.end()) {
            ValidationResult result = validateType(key, value, typeIt->second);
            if (!result.valid) {
                pImpl->errors.push_back(result.error);
            }
        }
        
        // Validate range
        auto rangeIt = schema.find("range");
        if (rangeIt != schema.end()) {
            ValidationResult result = validateRange(key, value, rangeIt->second);
            if (!result.valid) {
                pImpl->errors.push_back(result.error);
            }
        }
        
        // Validate enum
        auto enumIt = schema.find("enum");
        if (enumIt != schema.end()) {
            ValidationResult result = validateEnum(key, value, enumIt->second);
            if (!result.valid) {
                pImpl->errors.push_back(result.error);
            }
        }
        
        // Validate pattern
        auto patternIt = schema.find("pattern");
        if (patternIt != schema.end()) {
            ValidationResult result = validatePattern(key, value, patternIt->second);
            if (!result.valid) {
                pImpl->errors.push_back(result.error);
            }
        }
    }
    
    return pImpl->errors.empty() ? ValidationResult::success() : 
           ValidationResult::error("Schema validation failed: " + pImpl->errors[0]);
}

// Interpolation support
void Config::enableInterpolation(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->interpolationEnabled = enable;
}

void Config::addInterpolationReference(const std::string& target, const std::string& source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->interpolationReferences[target] = source;
}

bool Config::resolveInterpolations() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->interpolationEnabled) {
        return true;
    }
    
    bool allResolved = true;
    
    for (const auto& pair : pImpl->interpolationReferences) {
        const std::string& target = pair.first;
        const std::string& source = pair.second;
        
        auto targetIt = pImpl->flatEntries.find(target);
        if (targetIt == pImpl->flatEntries.end()) {
            continue;
        }
        
        auto sourceIt = pImpl->flatEntries.find(source);
        if (sourceIt == pImpl->flatEntries.end()) {
            pImpl->logger("Interpolation source not found: " + source + " (target: " + target + ")");
            allResolved = false;
            continue;
        }
        
        // Try to convert source value to string
        std::string resolvedValue;
        bool conversionSuccess = false;
        
        try {
            if (std::holds_alternative<std::string>(sourceIt->second)) {
                resolvedValue = std::get<std::string>(sourceIt->second);
                conversionSuccess = true;
            } else if (std::holds_alternative<int>(sourceIt->second)) {
                resolvedValue = std::to_string(std::get<int>(sourceIt->second));
                conversionSuccess = true;
            } else if (std::holds_alternative<double>(sourceIt->second)) {
                resolvedValue = std::to_string(std::get<double>(sourceIt->second));
                conversionSuccess = true;
            } else if (std::holds_alternative<bool>(sourceIt->second)) {
                resolvedValue = std::get<bool>(sourceIt->second) ? "true" : "false";
                conversionSuccess = true;
            }
        } catch (...) {
        }
        
        if (conversionSuccess) {
            targetIt->second = ConfigValue(resolvedValue);
            pImpl->logger("Interpolated " + target + " from " + source + " -> " + resolvedValue);
        } else {
            pImpl->logger("Failed to interpolate " + target + " from " + source);
            allResolved = false;
        }
    }
    
    return allResolved;
}

// Template support
void Config::addTemplate(const ConfigTemplate& template) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->templates[template.name] = template;
}

bool Config::applyTemplate(const std::string& templateName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        pImpl->logger("Template not found: " + templateName);
        return false;
    }
    
    const ConfigTemplate& template = it->second;
    
    // Apply defaults
    for (const auto& pair : template.defaults) {
        set(pair.first, pair.second, template.source);
    }
    
    return true;
}

// Summary
std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->flatEntries.size() << " flat, " 
        << pImpl->nestedEntries.size() << " nested entries, "
        << "environment: " << pImpl->environment << "):" << std::endl;
    
    for (const auto& entry : pImpl->flatEntries) {
        oss << "  " << entry.first << " = [";
        try {
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                                   std::is_same_v<T, double> || std::is_same_v<T, bool>) {
                    oss << arg;
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
                }
            }, entry.second);
        } catch (...) {
            oss << "[type unknown]";
        }
        oss << "] (" << static_cast<int>(pImpl->sources[entry.first]) << ")" << std::endl;
    }
    
    // Show nested entries
    for (const auto& entry : pImpl->nestedEntries) {
        oss << "  " << joinPath(entry.first) << " = [";
        try {
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                                   std::is_same_v<T, double> || std::is_same_v<T, bool>) {
                    oss << arg;
                }
            }, entry.second);
        } catch (...) {
            oss << "[type unknown]";
        }
        oss << "] (" << static_cast<int>(pImpl->sources[joinPath(entry.first)]) << ")" << std::endl;
    }
    
    return oss.str();
}

// Error handling
void Config::setLogger(std::function<void(const std::string&)>& logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logger = logger;
}

// File I/O utilities
bool Config::validateFileFormat(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return ext == ".json" || ext == ".yaml" || ext == ".yml" || ext == ".cfg" || ext == ".ini";
}

std::string Config::getDefaultConfigFile() {
    return "config.json";
}

// Configuration comparison
bool Config::operator==(const Config& other) const {
    return pImpl->flatEntries == other.pImpl->flatEntries && 
           pImpl->nestedEntries == other.pImpl->nestedEntries;
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

// Configuration merge
void Config::merge(const Config& other, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& pair : other.pImpl->flatEntries) {
        set(pair.first, pair.second, source);
    }
    
    for (const auto& pair : other.pImpl->nestedEntries) {
        set(pair.first, pair.second, source);
    }
}

// Static member function implementations
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

std::string Config::replacePlaceholders(const std::string& str, const std::function<std::string(const std::string&)>& resolver) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find('}', pos);
        if (endPos == std::string::npos) {
            break;
        }
        
        std::string key = result.substr(pos + 2, endPos - pos - 2);
        std::string replacement = resolver(key);
        result.replace(pos, endPos - pos + 1, replacement);
        pos += replacement.length();
    }
    return result;
}

std::vector<std::string> Config::splitPath(const std::string& path) {
    std::vector<std::string> result;
    std::stringstream ss(path);
    std::string item;
    
    while (std::getline(ss, item, '.')) {
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

std::string Config::joinPath(const std::vector<std::string>& path) {
    std::string result;
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) {
            result += ".";
        }
        result += path[i];
    }
    return result;
}

// JSON/YAML conversion utilities
ConfigValue Config::parseJsonValue(const json& j) {
    if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        if (j.empty()) {
            return std::vector<int>{};
        }
        
        // Try to infer array type
        bool allInts = std::all_of(j.begin(), j.end(), [](const json& item) { return item.is_number_integer(); });
        bool allDoubles = std::all_of(j.begin(), j.end(), [](const json& item) { return item.is_number(); });
        bool allBools = std::all_of(j.begin(), j.end(), [](const json& item) { return item.is_boolean(); });
        
        if (allInts) {
            std::vector<int> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<int>());
            }
            return vec;
        } else if (allDoubles) {
            std::vector<double> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<double>());
            }
            return vec;
        } else if (allBools) {
            std::vector<bool> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<bool>());
            }
            return vec;
        } else {
            // Assume strings
            std::vector<std::string> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        }
    } else {
        return "";
    }
}

std::string Config::serializeJsonValue(const ConfigValue& value) {
    if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::ostringstream oss;
        oss << std::setprecision(6) << std::get<double>(value);
        return oss.str();
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        json j = std::get<std::vector<int>>(value);
        return j.dump();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        json j = std::get<std::vector<double>>(value);
        return j.dump();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        json j = std::get<std::vector<std::string>>(value);
        return j.dump();
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        json j = std::get<std::vector<bool>>(value);
        return j.dump();
    }
    return "null";
}

std::string Config::jsonToConfigFormat(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        std::string result;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it != j.begin()) {
                result += "\n";
            }
            
            std::string key = it.key();
            std::string valueStr;
            
            if (key.find('.') != std::string::npos) {
                // Nested key - keep as is
                valueStr = serializeJsonValue(parseJsonValue(it.value()));
                result += key + " = " + valueStr;
            } else {
                // Simple key
                if (it.value().is_string()) {
                    valueStr = "\"" + it.value().get<std::string>() + "\"";
                } else {
                    valueStr = it.value().dump();
                }
                result += key + " = " + valueStr;
            }
        }
        
        return result;
    } catch (const json::exception& e) {
        return "";
    }
}

std::string Config::configToJsonFormat(const std::string& configStr) {
    json j;
    std::istringstream iss(configStr);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Try to parse value
            try {
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                    j[key] = value;
                } else if (std::regex_match(value, std::regex("^true$|^false$"))) {
                    j[key] = value == "true";
                } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
                    j[key] = std::stoi(value);
                } else if (std::regex_match(value, std::regex("^-?\d+\.\d+$"))) {
                    j[key] = std::stod(value);
                } else {
                    j[key] = value;
                }
            } catch (...) {
                j[key] = value;
            }
        }
    }
    
    return j.dump(2);
}

// Schema validation utilities
ValidationResult Config::validateType(const std::string& key, const ConfigValue& value, const std::any& typeInfo) {
    try {
        std::string typeStr = std::any_cast<std::string>(typeInfo);
        
        if (typeStr == "int" || typeStr == "integer") {
            return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) ? 
                   ValidationResult::success() : 
                   ValidationResult::error("Value must be an integer", key);
        } else if (typeStr == "float" || typeStr == "double") {
            return std::holds_alternative<double>(value) || std::holds_alternative<int>(value) || 
                   std::holds_alternative<int64_t>(value) ? 
                   ValidationResult::success() : 
                   ValidationResult::error("Value must be a number", key);
        } else if (typeStr == "bool" || typeStr == "boolean") {
            return std::holds_alternative<bool>(value) ? 
                   ValidationResult::success() : 
                   ValidationResult::error("Value must be a boolean", key);
        } else if (typeStr == "string") {
            return std::holds_alternative<std::string>(value) ? 
                   ValidationResult::success() : 
                   ValidationResult::error("Value must be a string", key);
        } else if (typeStr == "array") {
            return std::holds_alternative<std::vector<int>>(value) || 
                   std::holds_alternative<std::vector<double>>(value) || 
                   std::holds_alternative<std::vector<std::string>>(value) || 
                   std::holds_alternative<std::vector<bool>>(value) ? 
                   ValidationResult::success() : 
                   ValidationResult::error("Value must be an array", key);
        }
    } catch (...) {
    }
    
    return ValidationResult::error("Unknown type specification", key);
}

ValidationResult Config::validateRange(const std::string& key, const ConfigValue& value, const std::any& rangeInfo) {
    try {
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            int64_t val = std::holds_alternative<int>(value) ? 
                          std::get<int>(value) : std::get<int64_t>(value);
            
            const json& range = std::any_cast<json>(rangeInfo);
            if (range.is_array() && range.size() == 2) {
                int64_t min = range[0].get<int64_t>();
                int64_t max = range[1].get<int64_t>();
                
                if (val >= min && val <= max) {
                    return ValidationResult::success();
                } else {
                    return ValidationResult::error("Value out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]", key);
                }
            }
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            
            const json& range = std::any_cast<json>(rangeInfo);
            if (range.is_array() && range.size() == 2) {
                double min = range[0].get<double>();
                double max = range[1].get<double>();
                
                if (val >= min && val <= max) {
                    return ValidationResult::success();
                } else {
                    return ValidationResult::error("Value out of range [" + std::to_string(min) + ", " + std::to_string(max) + "]", key);
                }
            }
        }
    } catch (...) {
    }
    
    return ValidationResult::error("Invalid range specification", key);
}

ValidationResult Config::validateEnum(const std::string& key, const ConfigValue& value, const std::any& enumInfo) {
    try {
        const json& enumValues = std::any_cast<json>(enumInfo);
        
        if (std::holds_alternative<std::string>(value)) {
            std::string val = std::get<std::string>(value);
            for (const auto& item : enumValues) {
                if (item.get<std::string>() == val) {
                    return ValidationResult::success();
                }
            }
            return ValidationResult::error("Value must be one of: " + enumValues.dump(), key);
        } else if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            for (const auto& item : enumValues) {
                if (item.get<int>() == val) {
                    return ValidationResult::success();
                }
            }
            return ValidationResult::error("Value must be one of: " + enumValues.dump(), key);
        }
    } catch (...) {
    }
    
    return ValidationResult::error("Invalid enum specification", key);
}

ValidationResult Config::validatePattern(const std::string& key, const ConfigValue& value, const std::any& patternInfo) {
    try {
        if (std::holds_alternative<std::string>(value)) {
            std::string val = std::get<std::string>(value);
            std::string pattern = std::any_cast<std::string>(patternInfo);
            
            std::regex regex(pattern);
            if (std::regex_match(val, regex)) {
                return ValidationResult::success();
            } else {
                return ValidationResult::error("Value does not match pattern: " + pattern, key);
            }
        }
    } catch (...) {
    }
    
    return ValidationResult::error("Pattern validation requires string value", key);
}

// YAML to ConfigValue conversion
ConfigValue Config::yamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.IsBoolean()) {
            return node.as<bool>();
        } else if (node.IsIntegral()) {
            return node.as<int64_t>();
        } else if (node.IsFloat()) {
            return node.as<double>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        // Determine array type
        bool allInts = true, allDoubles = true, allBools = true, allStrings = true;
        
        for (const auto& item : node) {
            if (item.IsIntegral()) {
                allDoubles = false;
                allBools = false;
                allStrings = false;
            } else if (item.IsFloat()) {
                allInts = false;
                allBools = false;
                allStrings = false;
            } else if (item.IsBoolean()) {
                allInts = false;
                allDoubles = false;
                allStrings = false;
            } else if (item.IsScalar()) {
                allInts = false;
                allDoubles = false;
                allBools = false;
            } else {
                allInts = allDoubles = allBools = allStrings = false;
            }
        }
        
        if (allInts) {
            std::vector<int> vec;
            for (const auto& item : node) {
                vec.push_back(item.as<int>());
            }
            return vec;
        } else if (allDoubles) {
            std::vector<double> vec;
            for (const auto& item : node) {
                vec.push_back(item.as<double>());
            }
            return vec;
        } else if (allBools) {
            std::vector<bool> vec;
            for (const auto& item : node) {
                vec.push_back(item.as<bool>());
            }
            return vec;
        } else if (allStrings) {
            std::vector<std::string> vec;
            for (const auto& item : node) {
                vec.push_back(item.as<std::string>());
            }
            return vec;
        }
        
        // Default to strings
        std::vector<std::string> vec;
        for (const auto& item : node) {
            vec.push_back(item.as<std::string>());
        }
        return vec;
    }
    
    return "";
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