#include "AdvancedConfigurationManager.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <unordered_set>

namespace nlm {

// ConfigPath implementation
ConfigPath::ConfigPath(const std::string& path) {
    fullPath = path;
    if (path.empty()) return;
    
    std::stringstream ss(path);
    std::string component;
    while (std::getline(ss, component, '.')) {
        if (!component.empty()) {
            components.push_back(component);
        }
    }
}

ConfigPath ConfigPath::parent() const {
    if (components.size() <= 1) {
        return ConfigPath();
    }
    
    std::string parentPath;
    for (size_t i = 0; i < components.size() - 1; ++i) {
        if (i > 0) parentPath += ".";
        parentPath += components[i];
    }
    
    return ConfigPath(parentPath);
}

std::string ConfigPath::lastComponent() const {
    if (components.empty()) return "";
    return components.back();
}

bool ConfigPath::isEmpty() const {
    return components.empty();
}

bool ConfigPath::operator==(const ConfigPath& other) const {
    return fullPath == other.fullPath;
}

bool ConfigPath::operator!=(const ConfigPath& other) const {
    return fullPath != other.fullPath;
}

std::string ConfigPath::toString() const {
    return fullPath;
}

// ConfigParameterSchema implementation
bool ConfigParameterSchema::isValid(const ConfigValue& value) const {
    // Check type
    if (!type.empty()) {
        if (type == "int64_t" && !std::holds_alternative<int64_t>(value)) return false;
        if (type == "double" && !std::holds_alternative<double>(value)) return false;
        if (type == "float" && !std::holds_alternative<float>(value)) return false;
        if (type == "bool" && !std::holds_alternative<bool>(value)) return false;
        if (type == "string" && !std::holds_alternative<std::string>(value)) return false;
        if (type == "array" && !std::holds_alternative<std::vector<ConfigValue>>(value)) return false;
        if (type == "object" && !std::holds_alternative<std::map<std::string, ConfigValue>>(value)) return false;
    }
    
    // Check allowed values
    if (!allowedValues.empty()) {
        bool found = false;
        for (const auto& allowed : allowedValues) {
            if (value == allowed) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    
    // Check range
    if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        if (val < range.first || val > range.second) return false;
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < range.first || val > range.second) return false;
    }
    
    // Check pattern
    if (!pattern.empty() && std::holds_alternative<std::string>(value)) {
        std::string val = std::get<std::string>(value);
        std::regex regex(pattern);
        if (!std::regex_match(val, regex)) return false;
    }
    
    return true;
}

std::string ConfigParameterSchema::getTypeName() const {
    if (type.empty()) return "unknown";
    
    if (type == "int64_t") return "integer";
    if (type == "double") return "floating point";
    if (type == "float") return "single precision float";
    if (type == "bool") return "boolean";
    if (type == "string") return "string";
    if (type == "array") return "array";
    if (type == "object") return "object";
    
    return type;
}

// ConfigSchema implementation
bool ConfigSchema::validate(const ConfigEntry& entry) const {
    auto paramIt = parameters.find(entry.key);
    if (paramIt == parameters.end()) {
        return false;
    }
    
    return paramIt->second.isValid(entry.value);
}

const ConfigParameterSchema* ConfigSchema::getParameter(const std::string& key) const {
    auto it = parameters.find(key);
    if (it == parameters.end()) {
        return nullptr;
    }
    return &it->second;
}

bool ConfigSchema::hasParameter(const std::string& key) const {
    return parameters.find(key) != parameters.end();
}

std::vector<std::string> ConfigSchema::getAllParameterKeys() const {
    std::vector<std::string> keys;
    keys.reserve(parameters.size());
    for (const auto& pair : parameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

// ConfigurationManager implementation
struct ConfigurationManager::Impl {
    std::map<std::string, ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, ConfigSnapshot> snapshots;
    std::map<std::string, std::string> categories;
    
    // For dependency tracking
    std::map<std::string, std::vector<std::string>> dependencies;
    std::unordered_set<std::string> initializedSchemas;
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}

ConfigurationManager::~ConfigurationManager() = default;

void ConfigurationManager::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigMetadata metadata;
    metadata.source = source;
    metadata.timestamp = getCurrentTimestamp();
    
    // Validate against schema if available
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt != pImpl->schemas.end()) {
        ConfigEntry entry(key, value, metadata);
        if (!schemaIt->second.validate(entry)) {
            throw std::runtime_error("Configuration validation failed for key: " + key);
        }
    }
    
    pImpl->entries[key] = ConfigEntry(key, value, metadata);
}

void ConfigurationManager::set(const std::string& key, const ConfigValue& value, const ConfigMetadata& metadata) {
    pImpl->entries[key] = ConfigEntry(key, value, metadata);
}

ConfigValue ConfigurationManager::get(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        throw std::runtime_error("Configuration key not found: " + key);
    }
    return it->second.value;
}

ConfigValue ConfigurationManager::get(const std::string& key, const ConfigValue& defaultValue) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return defaultValue;
    }
    return it->second.value;
}

bool ConfigurationManager::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

bool ConfigurationManager::remove(const std::string& key) {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return false;
    }
    pImpl->entries.erase(it);
    return true;
}

ConfigValue ConfigurationManager::get(const ConfigPath& path) const {
    ConfigValue result = std::string(""); // Default empty string
    
    const ConfigManager* current = this;
    for (const auto& component : path.getComponents()) {
        std::string currentKey = result.empty() ? component : resultToString(result) + "." + component;
        
        auto it = pImpl->entries.find(currentKey);
        if (it == pImpl->entries.end()) {
            return std::string(""); // Path not found
        }
        
        // For nested access, we'd need to implement proper nested structure handling
        // For now, this is a simplified version
        result = it->second.value;
    }
    
    return result;
}

void ConfigurationManager::set(const ConfigPath& path, const ConfigValue& value, ConfigSource source) {
    // Simplified nested setting - in a full implementation, you'd need to handle nested structures
    set(path.toString(), value, source);
}

bool ConfigurationManager::has(const ConfigPath& path) const {
    // Simplified nested check - in a full implementation, you'd need to handle nested structures
    return has(path.toString());
}

void ConfigurationManager::registerSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.name] = schema;
    
    // Initialize dependencies
    for (const auto& paramName : schema.requiredParameters) {
        if (schema.parameters.find(paramName) != schema.parameters.end()) {
            pImpl->dependencies[paramName].push_back(schema.name);
        }
    }
    
    pImpl->initializedSchemas.insert(schema.name);
}

bool ConfigurationManager::hasSchema(const std::string& name) const {
    return pImpl->schemas.find(name) != pImpl->schemas.end();
}

const ConfigSchema* ConfigurationManager::getSchema(const std::string& name) const {
    auto it = pImpl->schemas.find(name);
    if (it == pImpl->schemas.end()) {
        return nullptr;
    }
    return &it->second;
}

void ConfigurationManager::loadFromFile(const std::string& filepath) {
    // Detect format and load
    std::string extension = std::filesystem::path(filepath).extension().string();
    ConfigFileFormat::Format format = ConfigFileFormat::detectFormat(filepath);
    
    bool success = ConfigFileFormat::loadFromFile(*this, filepath, format);
    if (!success) {
        throw std::runtime_error("Failed to load configuration from file: " + filepath);
    }
}

void ConfigurationManager::loadFromEnvironment() {
    // Load from environment variables
    // In a real implementation, you would iterate over environment variables
    // and parse them into configuration values
    // For now, this is a placeholder
}

void ConfigurationManager::loadFromCommandLine(int argc, char** argv) {
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.substr(0, 2) == "--") {
            size_t eqPos = arg.find("=");
            if (eqPos != std::string::npos) {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                
                ConfigValue configValue = ConfigValidation::stringToValue(value);
                set(key, configValue, ConfigSource::COMMAND_LINE);
            }
        }
    }
}

std::vector<std::string> ConfigurationManager::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        auto schemaIt = pImpl->schemas.find(entry.first);
        if (schemaIt != pImpl->schemas.end()) {
            if (!schemaIt->second.validate(entry.second)) {
                errors.push_back("Validation failed for key: " + entry.first);
            }
        }
    }
    
    return errors;
}

bool ConfigurationManager::validate(const std::string& key) const {
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt == pImpl->schemas.end()) {
        return true; // No schema, so valid
    }
    
    auto entryIt = pImpl->entries.find(key);
    if (entryIt == pImpl->entries.end()) {
        return false; // Missing required parameter
    }
    
    return schemaIt->second.validate(entryIt->second);
}

ConfigSnapshot ConfigurationManager::createSnapshot(const std::string& name, const std::string& description) const {
    ConfigSnapshot snapshot;
    snapshot.name = name;
    snapshot.description = description;
    snapshot.timestamp = getCurrentTimestamp();
    
    snapshot.entries = pImpl->entries;
    
    return snapshot;
}

void ConfigurationManager::restoreSnapshot(const ConfigSnapshot& snapshot) {
    pImpl->entries = snapshot.entries;
}

bool ConfigurationManager::hasSnapshot(const std::string& name) const {
    return pImpl->snapshots.find(name) != pImpl->snapshots.end();
}

std::vector<std::string> ConfigurationManager::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> ConfigurationManager::getKeys(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->entries) {
        if (pImpl->categories.find(pair.first) != pImpl->categories.end() &&
            pImpl->categories.at(pair.first) == category) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

std::vector<std::string> ConfigurationManager::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->entries) {
        if (pair.second.metadata.source == source) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

std::string ConfigurationManager::toJSON() const {
    json jsonData;
    
    for (const auto& pair : pImpl->entries) {
        jsonData[pair.first] = ConfigValidation::valueToString(pair.second.value);
    }
    
    return jsonData.dump(2);
}

std::string ConfigurationManager::toYAML() const {
    // Simple YAML serialization - in production, you'd use a proper YAML library
    std::ostringstream oss;
    oss << "# Configuration" << std::endl;
    
    for (const auto& pair : pImpl->entries) {
        oss << pair.first << ": " << ConfigValidation::valueToString(pair.second.value) << std::endl;
    }
    
    return oss.str();
}

std::string ConfigurationManager::toINI() const {
    std::ostringstream oss;
    oss << "# Configuration" << std::endl;
    
    for (const auto& pair : pImpl->entries) {
        oss << pair.first << " = " << ConfigValidation::valueToString(pair.second.value) << std::endl;
    }
    
    return oss.str();
}

bool ConfigurationManager::equals(const ConfigurationManager& other) const {
    return pImpl->entries == other.pImpl->entries;
}

std::map<std::string, ConfigValue> ConfigurationManager::diff(const ConfigurationManager& other) const {
    std::map<std::string, ConfigValue> diffs;
    
    for (const auto& pair : pImpl->entries) {
        auto it = other.pImpl->entries.find(pair.first);
        if (it == other.pImpl->entries.end() || pair.second.value != it->second.value) {
            diffs[pair.first] = pair.second.value;
        }
    }
    
    return diffs;
}

ConfigMetadata ConfigurationManager::getMetadata(const std::string& key) const {
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return ConfigMetadata();
    }
    return it->second.metadata;
}

void ConfigurationManager::setMetadata(const std::string& key, const ConfigMetadata& metadata) {
    auto it = pImpl->entries.find(key);
    if (it != pImpl->entries.end()) {
        it->second.metadata = metadata;
    }
}

void ConfigurationManager::setCategory(const std::string& key, const std::string& category) {
    pImpl->categories[key] = category;
}

std::string ConfigurationManager::getCategory(const std::string& key) const {
    auto it = pImpl->categories.find(key);
    if (it == pImpl->categories.end()) {
        return "";
    }
    return it->second;
}

std::vector<std::string> ConfigurationManager::getCategories() const {
    std::vector<std::string> categories;
    categories.reserve(pImpl->categories.size());
    for (const auto& pair : pImpl->categories) {
        categories.push_back(pair.second);
    }
    return categories;
}

// ConfigurationBuilder implementation
ConfigurationBuilder::ConfigurationBuilder() : pImpl(std::make_unique<Impl>()) {}

ConfigurationBuilder::~ConfigurationBuilder() = default;

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, const ConfigValue& value) {
    pImpl->entries[key] = ConfigEntry(key, value);
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, int64_t value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, double value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, float value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, bool value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, const std::string& value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, const std::vector<ConfigValue>& value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::set(const std::string& key, const std::map<std::string, ConfigValue>& value) {
    return set(key, value);
}

ConfigurationBuilder& ConfigurationBuilder::withSource(ConfigSource source) {
    // Update source for all entries
    for (auto& pair : pImpl->entries) {
        pair.second.metadata.source = source;
    }
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::withMetadata(const ConfigMetadata& metadata) {
    for (auto& pair : pImpl->entries) {
        pair.second.metadata = metadata;
    }
    return *this;
}

ConfigurationBuilder& ConfigurationBuilder::withCategory(const std::string& category) {
    for (auto& pair : pImpl->entries) {
        pImpl->categories[pair.first] = category;
    }
    return *this;
}

std::map<std::string, ConfigEntry> ConfigurationBuilder::build() const {
    return pImpl->entries;
}

std::map<std::string, ConfigEntry> ConfigurationBuilder::createDefaultConfig() {
    ConfigurationBuilder builder;
    
    builder.set("simulation.timestep", 0.001)
          .set("simulation.max_steps", 10000)
          .set("neuron.count", 1000)
          .set("neuron.region_count", 1)
          .set("plasticity.enable", true)
          .set("memory.working_memory_capacity", 100)
          .set("memory.episodic_memory_max_episodes", 1000)
          .set("neuromodulation.dopamine_enabled", true)
          .set("neuromodulation.curiosity_enabled", true)
          .set("debug.logging_level", "INFO")
          .set("debug.enable_profiling", false)
          .withSource(ConfigSource::DEFAULT)
          .withCategory("simulation");
    
    return builder.build();
}

std::map<std::string, ConfigEntry> ConfigurationBuilder::createDevelopmentConfig() {
    ConfigurationBuilder builder;
    
    builder.set("simulation.timestep", 0.0001)
          .set("simulation.max_steps", 50000)
          .set("neuron.count", 5000)
          .set("neuron.region_count", 2)
          .set("plasticity.enable", true)
          .set("plasticity.stdp_enabled", true)
          .set("plasticity.hebbian_enabled", true)
          .set("plasticity.structural_enabled", true)
          .set("memory.working_memory_capacity", 500)
          .set("memory.episodic_memory_max_episodes", 5000)
          .set("memory.associative_memory_enabled", true)
          .set("neuromodulation.dopamine_enabled", true)
          .set("neuromodulation.curiosity_enabled", true)
          .set("neuromodulation.novelty_enabled", true)
          .set("debug.logging_level", "DEBUG")
          .set("debug.enable_profiling", true)
          .set("debug.save_checkpoints", true)
          .set("optimization.enable_multithreading", true)
          .set("optimization.enable_vectorization", true)
          .withSource(ConfigSource::DEFAULT)
          .withCategory("development");
    
    return builder.build();
}

std::map<std::string, ConfigEntry> ConfigurationBuilder::createProductionConfig() {
    ConfigurationBuilder builder;
    
    builder.set("simulation.timestep", 0.001)
          .set("simulation.max_steps", 100000)
          .set("neuron.count", 10000)
          .set("neuron.region_count", 4)
          .set("plasticity.enable", true)
          .set("plasticity.stdp_enabled", true)
          .set("plasticity.hebbian_enabled", true)
          .set("plasticity.structural_enabled", false)  // Disable for production
          .set("memory.working_memory_capacity", 1000)
          .set("memory.episodic_memory_max_episodes", 10000)
          .set("memory.associative_memory_enabled", false)  // Disable for production
          .set("neuromodulation.dopamine_enabled", true)
          .set("neuromodulation.curiosity_enabled", false)  // Disable for production
          .set("neuromodulation.novelty_enabled", false)   // Disable for production
          .set("debug.logging_level", "WARNING")
          .set("debug.enable_profiling", false)
          .set("debug.save_checkpoints", false)  // Disable for production
          .set("optimization.enable_multithreading", true)
          .set("optimization.enable_vectorization", true)
          .set("optimization.sparse_connectivity", true)
          .withSource(ConfigSource::DEFAULT)
          .withCategory("production");
    
    return builder.build();
}

// Helper functions
std::string ConfigurationManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ConfigValidation::valueToString(const ConfigValue& value) {
    std::string result;
    
    std::visit([&result](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << val;
            result = oss.str();
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = val;
        } else if constexpr (std::is_same_v<T, std::vector<ConfigValue>>) {
            result = "[";
            bool first = true;
            for (const auto& item : val) {
                if (!first) result += ", ";
                result += valueToString(item);
                first = false;
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, ConfigValue>>) {
            result = "{";
            bool first = true;
            for (const auto& pair : val) {
                if (!first) result += ", ";
                result += pair.first + "=" + valueToString(pair.second);
                first = false;
            }
            result += "}";
        } else {
            result = "unknown";
        }
    }, value);
    
    return result;
}

ConfigValue ConfigValidation::stringToValue(const std::string& str) {
    // Try to parse as boolean
    if (str == "true" || str == "TRUE" || str == "1") {
        return true;
    }
    if (str == "false" || str == "FALSE" || str == "0") {
        return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.length() && intVal >= std::numeric_limits<int64_t>::min() && 
            intVal <= std::numeric_limits<int64_t>::max()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as floating point
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.length()) {
            // Check if it looks like an integer
            if (std::find(str.begin(), str.end(), '.') == std::string::npos && 
                std::find(str.begin(), str.end(), 'e') == std::string::npos &&
                std::find(str.begin(), str.end(), 'E') == std::string::npos) {
                return static_cast<int64_t>(doubleVal);
            }
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string
    return str;
}

std::string ConfigValidation::valueToString(const ConfigValue& value) {
    return ConfigValidation::valueToString(value);
}

std::string ConfigValidation::valueToString(const ConfigValue& value);

bool ConfigValidation::isInteger(const ConfigValue& value) {
    return std::holds_alternative<int64_t>(value);
}

bool ConfigValidation::isDouble(const ConfigValue& value) {
    return std::holds_alternative<double>(value);
}

bool ConfigValidation::isFloat(const ConfigValue& value) {
    return std::holds_alternative<float>(value);
}

bool ConfigValidation::isBool(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool ConfigValidation::isString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool ConfigValidation::isArray(const ConfigValue& value) {
    return std::holds_alternative<std::vector<ConfigValue>>(value);
}

bool ConfigValidation::isObject(const ConfigValue& value) {
    return std::holds_alternative<std::map<std::string, ConfigValue>>(value);
}

bool ConfigValidation::isInRange(const ConfigValue& value, double min, double max) {
    if (std::holds_alternative<int64_t>(value)) {
        int64_t val = std::get<int64_t>(value);
        return static_cast<double>(val) >= min && static_cast<double>(val) <= max;
    } else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } else if (std::holds_alternative<float>(value)) {
        float val = std::get<float>(value);
        return static_cast<double>(val) >= min && static_cast<double>(val) <= max;
    }
    return false;
}

bool ConfigValidation::isInAllowedValues(const ConfigValue& value, const std::vector<ConfigValue>& allowed) {
    for (const auto& allowedValue : allowed) {
        if (value == allowedValue) {
            return true;
        }
    }
    return false;
}

bool ConfigValidation::matchesPattern(const std::string& value, const std::string& pattern) {
    std::regex regex(pattern);
    return std::regex_match(value, regex);
}

ConfigValue ConfigValidation::convertToType(const ConfigValue& value, ConfigValue targetType) {
    // Simplified conversion - in production, you'd need more sophisticated conversion logic
    if (std::holds_alternative<int64_t>(value) && std::holds_alternative<double>(targetType)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<int64_t>(value) && std::holds_alternative<std::string>(targetType)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value) && std::holds_alternative<int64_t>(targetType)) {
        return static_cast<int64_t>(std::get<double>(value));
    }
    
    return value;
}

std::string ConfigValidation::valueToString(const ConfigValue& value);

// Forward declarations
ConfigValue ConfigValidation::valueToString(const ConfigValue& value);
std::string ConfigValidation::valueToString(const ConfigValue& value);

// Template definitions
namespace {
    template<typename T>
    ConfigValue convertToConfigValue(const T& value) {
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return static_cast<ConfigValue>(value);
        } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
            return static_cast<ConfigValue>(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return static_cast<ConfigValue>(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return static_cast<ConfigValue>(value);
        } else {
            return ConfigValue(); // Default
        }
    }
}

} // namespace nlm
