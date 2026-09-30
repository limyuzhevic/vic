// ============================================================================
// Advanced Configuration Implementation
// ============================================================================

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <ctime>

namespace nlm {

// ============================================================================
// TypeInfo Implementation
// ============================================================================

std::string TypeInfo::typeToString() const {
    switch (type) {
        case Type::INT: return "int";
        case Type::INT64: return "int64";
        case Type::DOUBLE: return "double";
        case Type::BOOL: return "bool";
        case Type::STRING: return "string";
        case Type::INT_VECTOR: return "int_vector";
        case Type::DOUBLE_VECTOR: return "double_vector";
        case Type::STRING_VECTOR: return "string_vector";
        case Type::ANY: return "any";
        default: return "unknown";
    }
}

std::string TypeInfo::validateValue(const ConfigValue& value) const {
    try {
        if (std::holds_alternative<int>(value)) {
            if (range) {
                double val = static_cast<double>(std::get<int>(value));
                if (val < range->first || val > range->second) {
                    return "Value out of range";
                }
            }
            return "";
        } else if (std::holds_alternative<int64_t>(value)) {
            if (range) {
                double val = static_cast<double>(std::get<int64_t>(value));
                if (val < range->first || val > range->second) {
                    return "Value out of range";
                }
            }
            return "";
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (range) {
                if (val < range->first || val > range->second) {
                    return "Value out of range";
                }
            }
            return "";
        } else if (std::holds_alternative<bool>(value)) {
            return "";
        } else if (std::holds_alternative<std::string>(value)) {
            if (allowedValues) {
                const auto& str = std::get<std::string>(value);
                bool found = false;
                for (const auto& allowed : *allowedValues) {
                    if (str == allowed) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return "Value not in allowed list";
                }
            }
            return "";
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return "";
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return "";
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return "";
        }
    } catch (const std::bad_variant_access&) {}
    return "Invalid value type";
}

std::optional<ConfigValue> TypeInfo::convertToType(const std::string& strValue) const {
    try {
        if (type == Type::INT) {
            int val = std::stoi(strValue);
            return val;
        } else if (type == Type::INT64) {
            int64_t val = std::stoll(strValue);
            return val;
        } else if (type == Type::DOUBLE) {
            double val = std::stod(strValue);
            return val;
        } else if (type == Type::BOOL) {
            bool val = stringToBool(strValue);
            return val;
        } else if (type == Type::STRING) {
            return strValue;
        } else if (type == Type::STRING_VECTOR) {
            // Simple parsing of comma-separated values
            std::vector<std::string> vec;
            std::string token;
            std::istringstream iss(strValue);
            while (std::getline(iss, token, ',')) {
                token = trim(token);
                if (!token.empty()) {
                    vec.push_back(token);
                }
            }
            return vec;
        }
    } catch (const std::exception&) {}
    return std::nullopt;
}

bool TypeInfo::matchesType(const ConfigValue& value) const {
    if (type == Type::ANY) return true;
    
    switch (type) {
        case Type::INT: return std::holds_alternative<int>(value);
        case Type::INT64: return std::holds_alternative<int64_t>(value);
        case Type::DOUBLE: return std::holds_alternative<double>(value);
        case Type::BOOL: return std::holds_alternative<bool>(value);
        case Type::STRING: return std::holds_alternative<std::string>(value);
        case Type::INT_VECTOR: return std::holds_alternative<std::vector<int>>(value);
        case Type::DOUBLE_VECTOR: return std::holds_alternative<std::vector<double>>(value);
        case Type::STRING_VECTOR: return std::holds_alternative<std::vector<std::string>>(value);
        default: return false;
    }
}

// ============================================================================
// ConfigEntry Implementation
// ============================================================================

std::string ConfigEntry::getTypeName() const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int_vector";
    if (std::holds_alternative<std::vector<double>>(value)) return "double_vector";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string_vector";
    return "unknown";
}

std::string ConfigEntry::getValueString() const {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
}

// ============================================================================
// Config Implementation
// ============================================================================

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), frozen_(false) {
    lastChanged_ = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (frozen_) {
        return false;
    }
    
    // Check if schema is loaded and validate
    if (!schema_.getKeys().empty()) {
        // Try to parse as simple format first
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::unordered_map<std::string, ConfigValue> parsedConfig;
        
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Apply value converter if available
                ConfigValue convertedValue = applyValueConverter(key, value);
                parsedConfig[key] = convertedValue;
                
                // Add entry
                set(key, convertedValue, ConfigSource::File);
            }
        }
        
        // Validate if schema exists
        if (!schema_.getKeys().empty()) {
            ValidationResult validation = schema_.validate(parsedConfig);
            if (!validation.valid) {
                // Log validation errors but continue
                for (const auto& error : validation.errors) {
                    std::cerr << "WARNING: Configuration validation error: " << error << std::endl;
                }
            }
        }
        
        file.close();
        return !parsedConfig.empty();
    }
    
    // Fallback to simple loading without schema
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Apply value converter if available
            ConfigValue convertedValue = applyValueConverter(key, value);
            set(key, convertedValue, ConfigSource::File);
        }
    }
    
    file.close();
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    envPrefix_ = prefix;
    
    // For simplicity, we'll read specific known environment variables
    // In a real implementation, this would use actual environment variables
    const char* env = std::getenv((prefix + "MAX_STEPS").c_str());
    if (env) {
        set("max_steps", std::stoull(env), ConfigSource::Environment, 
            "Maximum simulation steps from environment", "simulation");
    }
    
    env = std::getenv((prefix + "NEURON_COUNT").c_str());
    if (env) {
        set("neuron_count", std::stoull(env), ConfigSource::Environment, 
            "Number of neurons from environment", "brain");
    }
    
    env = std::getenv((prefix + "DEBUG_MODE").c_str());
    if (env) {
        set("debug_mode", stringToBool(env), ConfigSource::Environment, 
            "Debug mode from environment", "debug");
    }
    
    env = std::getenv((prefix + "LOG_LEVEL").c_str());
    if (env) {
        set("log_level", std::string(env), ConfigSource::Environment, 
            "Log level from environment", "logging");
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
                set(key, value, ConfigSource::CommandLine, "Set from command line", "command_line");
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine, "Set from command line", "command_line");
        }
    }
    return true;
}

bool Config::loadFromAll(const std::vector<std::string>& files, const std::string& envPrefix) {
    bool success = true;
    
    // Load from environment variables first (highest priority for environment)
    if (!envPrefix.empty()) {
        success &= loadFromEnvironment(envPrefix);
    }
    
    // Load from files (medium priority)
    for (const auto& file : files) {
        success &= loadFromFile(file);
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "// NLM Configuration File\n";
    file << "// Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << value;
            } else {
                file << value;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(it->value)) {
                return std::get<int>(it->value);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(it->value)) {
                return std::get<int64_t>(it->value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(it->value)) {
                return std::get<double>(it->value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(it->value)) {
                return std::get<bool>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(it->value)) {
                return std::get<std::string>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(it->value)) {
                return std::get<std::vector<int>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(it->value)) {
                return std::get<std::vector<double>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(it->value)) {
                return std::get<std::vector<std::string>>(it->value);
            }
        }
    } catch (const std::bad_variant_access&) {}
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
bool Config::set(const std::string& key, const T& value, ConfigSource source,
                   const std::string& description, const std::string& category) {
    if (frozen_) {
        return false;
    }
    
    // Check if key exists to get old value for callback
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    ConfigValue oldValue;
    bool valueChanged = false;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        // For simplicity, assume value changed if it's a different type or different value
        // In real implementation, need proper comparison
        valueChanged = true;
    }
    
    // Convert to ConfigValue
    ConfigValue configValue;
    if constexpr (std::is_same_v<T, int>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, double>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::string>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        configValue = value;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        configValue = value;
    }
    
    // Add or update entry
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        it->value = configValue;
        it->source = source;
        it->description = description;
        it->category = category;
        it->timestamp = std::chrono::system_clock::now();
        valueChanged = true;
    } else {
        pImpl->entries.emplace_back(key, configValue, source, description, category);
        valueChanged = true;
    }
    
    if (valueChanged) {
        lastChanged_ = std::chrono::system_clock::now();
        addToHistory(key, configValue, source);
        notifyChange(key, oldValue, configValue, source);
    }
    
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    pImpl->entries.erase(it, pImpl->entries.end());
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getCategories() const {
    std::vector<std::string> categories;
    std::set<std::string> uniqueCategories;
    for (const auto& entry : pImpl->entries) {
        uniqueCategories.insert(entry.category);
    }
    for (const auto& category : uniqueCategories) {
        categories.push_back(category);
    }
    return categories;
}

void Config::clear() {
    pImpl->entries.clear();
}

void Config::overrideFrom(const Config& other, bool preserveSourcePriority) {
    for (const auto& otherEntry : other.pImpl->entries) {
        // Check if we should override (preserveSourcePriority means keep if source has higher priority)
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });n        
        if (it == pImpl->entries.end()) {
            pImpl->entries.push_back(otherEntry);
        } else if (!preserveSourcePriority || static_cast<int>(otherEntry.source) > static_cast<int>(it->source)) {
            *it = otherEntry;
        }
    }
}

void Config::merge(const Config& other) {
    for (const auto& otherEntry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });n        
        if (it == pImpl->entries.end()) {
            pImpl->entries.push_back(otherEntry);
        }
        // Note: In a real implementation, might want to merge values or prioritize based on source
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Categories: ";
    auto categories = getCategories();
    for (size_t i = 0; i < categories.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << categories[i];
    }
    oss << "\n";
    oss << "Last changed: " << std::chrono::system_clock::to_time_t(lastChanged_) << "\n";
    oss << "Frozen: " << (frozen_ ? "yes" : "no") << "\n";
    oss << "\nBy source:\n";
    auto bySource = getBySource();
    for (const auto& pair : bySource) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second.size() << " entries\n";
    }
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << entry.getValueString() 
            << " [" << entry.category << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::unordered_map<ConfigSource, std::vector<ConfigEntry>> Config::getBySource() const {
    std::unordered_map<ConfigSource, std::vector<ConfigEntry>> result;
    for (const auto& entry : pImpl->entries) {
        result[entry.source].push_back(entry);
    }
    return result;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Convert to map for schema validation
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    // Validate with schema if available
    if (!schema_.getKeys().empty()) {
        ValidationResult schemaResult = schema_.validate(configMap);
        for (const auto& error : schemaResult.errors) {
            result.addError(error);
        }
        for (const auto& warning : schemaResult.warnings) {
            result.addWarning(warning);
        }
    }
    
    return result;
}

ValidationResult Config::validate(const std::string& key) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        result.addError("Key not found: " + key);
        return result;
    }
    
    // Validate with schema if available
    if (!schema_.getKeys().empty()) {
        ValidationResult schemaResult = schema_.validate(key, it->value);
        for (const auto& error : schemaResult.errors) {
            result.addError(error);
        }
        for (const auto& warning : schemaResult.warnings) {
            result.addWarning(warning);
        }
    }
    
    return result;
}

bool Config::applyPreset(const std::string& presetName) {
    // For now, return false (would need preset implementation)
    return false;
}

std::vector<std::string> Config::getPresetNames() const {
    return std::vector<std::string>();//{"quick", "comprehensive", "debug", "minimal", "performance", "experimental"};
}

void Config::notifyChange(const std::string& key, const ConfigValue& oldValue,
                          const ConfigValue& newValue, ConfigSource source) {
    if (changeCallback_) {
        changeCallback_(key, newValue, source);
    }
}

void Config::addToHistory(const std::string& key, const ConfigValue& value, ConfigSource source) {
    changeHistory_.push_back(std::make_pair(key, std::make_pair(value, source)));
    
    // Keep only recent history
    if (changeHistory_.size() > 100) {
        changeHistory_.erase(changeHistory_.begin());
    }
}

ConfigValue Config::applyValueConverter(const std::string& key, const std::string& strValue) const {
    if (valueConverter_) {
        auto result = valueConverter_(key, strValue);
        if (result.has_value()) {
            return result.value();
        }
    }
    
    // Default conversion based on key prefix or content
    if (key.find("enabled") != std::string::npos || key.find("debug") != std::string::npos) {
        return stringToBool(strValue);
    } else if (key.find("count") != std::string::npos || key.find("steps") != std::string::npos ||
               key.find("neuron") != std::string::npos || key.find("region") != std::string::npos) {
        try {
            return std::stoull(strValue);
        } catch (...) {}
    } else if (key.find("probability") != std::string::npos || key.find("rate") != std::string::npos ||
               key.find("confidence") != std::string::npos || key.find("strength") != std::string::npos) {
        try {
            return stringToDouble(strValue);
        } catch (...) {}
    }
    
    // Default to string
    return strValue;
}

// Static helper functions
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

int Config::stringToInt(const std::string& str) {
    return std::stoi(str);
}

int64_t Config::stringToInt64(const std::string& str) {
    return std::stoll(str);
}

std::string Config::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string Config::doubleToString(double value) {
    std::ostringstream oss;
    oss << std::setprecision(6) << value;
    return oss.str();
}

std::string Config::intToString(int value) {
    return std::to_string(value);
}

std::string Config::int64ToString(int64_t value) {
    return std::to_string(value);
}

} // namespace nlm

// ============================================================================
// Schema Implementations
// ============================================================================

namespace nlm {
namespace Schemas {

ConfigSchema phase6Default() {
    ConfigSchema schema;
    
    // Add common Phase 6 configuration types
    schema.addType("experiment_name", TypeInfo::string("Experiment name", "Name of the experiment", {"default_experiment", "phase6_default", "quick_test"}, false));
    schema.addType("max_steps", TypeInfo::int("Maximum simulation steps", "Maximum number of simulation steps", {1000, 1000000}, false));
    schema.addType("neuron_count", TypeInfo::int("Number of neurons", "Number of neurons in the brain network", {10, 1000000}, false));
    schema.addType("region_count", TypeInfo::int("Number of brain regions", "Number of brain regions", {1, 10}, false));
    schema.addType("connection_probability", TypeInfo::double("Connection probability", "Probability of synaptic connections between neurons", {0.0, 1.0}, false));
    schema.addType("enable_checkpointing", TypeInfo::bool("Enable checkpointing", "Enable checkpoint saving and loading", false));
    schema.addType("enable_replay", TypeInfo::bool("Enable replay system", "Enable episodic memory replay and consolidation", false));
    schema.addType("enable_development", TypeInfo::bool("Enable development stages", "Enable brain development and maturation", false));
    schema.addType("checkpoint_path", TypeInfo::string("Checkpoint directory path", "Path where checkpoints are saved"));
    
    return schema;
}

ConfigSchema phase6Advanced() {
    ConfigSchema schema = phase6Default();
    
    // Add advanced configuration types
    schema.addType("enable_simd", TypeInfo::bool("Enable SIMD optimization", "Enable Single Instruction Multiple Data optimizations", false));
    schema.addType("enable_parallel", TypeInfo::bool("Enable parallel processing", "Enable parallel processing across multiple cores", false));
    schema.addType("enable_memory_pools", TypeInfo::bool("Enable memory pools", "Enable memory pool allocation for efficiency", false));
    schema.addType("prediction_confidence", TypeInfo::double("Prediction confidence", "Confidence level for prediction system", {0.0, 1.0}, false));
    schema.addType("attention_strength", TypeInfo::double("Attention strength", "Strength of attention mechanisms", {0.0, 5.0}, false));
    schema.addType("working_memory_capacity", TypeInfo::double("Working memory capacity", "Capacity of working memory system", {0.0, 10000.0}, false));
    schema.addType("episodic_memory_max_episodes", TypeInfo::double("Episodic memory capacity", "Maximum number of episodic memory episodes", {0.0, 10000.0}, false));
    schema.addType("development_stage_speed", TypeInfo::double("Development stage speed", "Speed of development stage transitions", {0.0, 10.0}, false));
    schema.addType("neuromodulation_gain", TypeInfo::double("Neuromodulation gain", "Gain factor for neuromodulation effects", {0.0, 5.0}, false));
    
    return schema;
}

ConfigSchema phase6Performance() {
    ConfigSchema schema = phase6Default();
    
    // Add performance benchmark types
    schema.addType("performance_benchmark_scales", TypeInfo::stringVector("Performance benchmark scales", "List of neuron counts for scaling tests", false));
    schema.addType("steps_per_scale", TypeInfo::int("Steps per scale", "Number of simulation steps for each scale", {1000, 100000}, false));
    schema.addType("num_seeds", TypeInfo::int("Number of random seeds", "Number of random seeds for multi-seed experiments", {1, 100}, false));
    schema.addType("enable_multithreading", TypeInfo::bool("Enable multithreading", "Enable multithreaded execution", false));
    schema.addType("thread_count", TypeInfo::int("Thread count", "Number of threads for parallel execution", {0, 100}, false));
    
    return schema;
}

ConfigSchema phase6Ablation() {
    ConfigSchema schema = phase6Default();
    
    // Add ablation study types
    schema.addType("target_types", TypeInfo::string("Ablation target types", "Types of neural mechanisms to ablate", {"all", "plasticity", "neuromodulation", "memory", "cognition"}, false));
    schema.addType("steps_per_trial", TypeInfo::int("Steps per ablation trial", "Number of simulation steps per ablation trial", {1000, 100000}, false));
    schema.addType("trials_per_condition", TypeInfo::int("Trials per ablation condition", "Number of trials for each ablation condition", {1, 20}, false));
    schema.addType("random_seed", TypeInfo::int("Random seed for ablation", "Random seed for ablation experiments", {0, 10000}, false));
    schema.addType("enable_statistical_comparison", TypeInfo::bool("Enable statistical comparison", "Enable statistical analysis of ablation results", false));
    
    return schema;
}

ConfigSchema phase6ContinualLearning() {
    ConfigSchema schema = phase6Default();
    
    // Add continual learning types
    schema.addType("num_cycles", TypeInfo::int("Number of continual learning cycles", "Number of cycles for continual learning experiments", {1, 10}, false));
    schema.addType("tasks_per_cycle", TypeInfo::int("Tasks per cycle", "Number of tasks per continual learning cycle", {1, 100}, false));
    schema.addType("steps_per_task", TypeInfo::int("Steps per task", "Number of simulation steps per continual learning task", {1000, 100000}, false));
    schema.addType("enable_task_switching", TypeInfo::bool("Enable task switching", "Enable switching between different tasks", false));
    schema.addType("enable_transfer_learning", TypeInfo::bool("Enable transfer learning", "Enable transfer learning between tasks", false));
    
    return schema;
}

ConfigSchema phase6StressTesting() {
    ConfigSchema schema = phase6Default();
    
    // Add stress testing types
    schema.addType("extreme_conditions", TypeInfo::bool("Extreme conditions", "Enable extreme stress testing conditions", false));
    schema.addType("memory_pressure", TypeInfo::bool("Memory pressure testing", "Enable memory pressure stress testing", false));
    schema.addType("cpu_overload", TypeInfo::bool("CPU overload testing", "Enable CPU overload stress testing", false));
    schema.addType("connection_extremes", TypeInfo::bool("Connection extremes", "Enable connection probability extremes", false));
    schema.addType("time_pressure", TypeInfo::bool("Time pressure", "Enable time pressure stress testing", false));
    schema.addType("memory_limit_gb", TypeInfo::double("Memory limit GB", "Memory limit for stress testing in GB", {0.1, 100.0}, false));
    schema.addType("cpu_limit_percent", TypeInfo::double("CPU limit percent", "CPU limit for stress testing in percent", {1.0, 100.0}, false));
    
    return schema;
}

ConfigSchema phase6MultiRun() {
    ConfigSchema schema = phase6Default();
    
    // Add multi-run experiment types
    schema.addType("num_runs", TypeInfo::int("Number of experimental runs", "Number of runs for multi-seed experiments", {1, 1000}, false));
    schema.addType("run_delay_ms", TypeInfo::int("Run delay milliseconds", "Delay between experimental runs in milliseconds", {0, 10000}, false));
    schema.addType("output_prefix", TypeInfo::string("Output prefix", "Prefix for output files", false));
    schema.addType("parallel_execution", TypeInfo::bool("Parallel execution", "Enable parallel execution of multiple runs", false));
    schema.addType("max_parallel_runs", TypeInfo::int("Maximum parallel runs", "Maximum number of parallel runs", {1, 32}, false));
    schema.addType("automatic_aggregation", TypeInfo::bool("Automatic aggregation", "Automatically aggregate results from multiple runs", false));
    
    return schema;
}

} // namespace Schemas

// ============================================================================
// Preset Implementations
// ============================================================================

namespace Presets {

bool applyQuick(Config& config) {
    bool success = true;
    
    success &= config.set("max_steps", 1000, ConfigSource::Programmatic, 
                          "Quick preset: Reduced simulation steps", "performance");
    success &= config.set("neuron_count", 500, ConfigSource::Programmatic,
                          "Quick preset: Fewer neurons", "brain");
    success &= config.set("region_count", 1, ConfigSource::Programmatic,
                          "Quick preset: Single region", "brain");
    success &= config.set("connection_probability", 0.05, ConfigSource::Programmatic,
                          "Quick preset: Lower connection probability", "connectivity");
    success &= config.set("enable_debug_mode", true, ConfigSource::Programmatic,
                          "Quick preset: Debug mode enabled", "debug");
    success &= config.set("enable_simd", true, ConfigSource::Programmatic,
                          "Quick preset: SIMD enabled", "optimization");
    
    return success;
}

bool applyComprehensive(Config& config) {
    bool success = true;
    
    success &= config.set("max_steps", 50000, ConfigSource::Programmatic,
                          "Comprehensive preset: Extended simulation", "performance");
    success &= config.set("neuron_count", 5000, ConfigSource::Programmatic,
                          "Comprehensive preset: Many neurons", "brain");
    success &= config.set("region_count", 3, ConfigSource::Programmatic,
                          "Comprehensive preset: Multiple regions", "brain");
    success &= config.set("connection_probability", 0.15, ConfigSource::Programmatic,
                          "Comprehensive preset: Higher connection probability", "connectivity");
    success &= config.set("enable_debug_mode", true, ConfigSource::Programmatic,
                          "Comprehensive preset: Debug mode enabled", "debug");
    success &= config.set("enable_simd", true, ConfigSource::Programmatic,
                          "Comprehensive preset: SIMD enabled", "optimization");
    success &= config.set("enable_parallel", true, ConfigSource::Programmatic,
                          "Comprehensive preset: Parallel enabled", "optimization");
    
    return success;
}

bool applyDebug(Config& config) {
    bool success = true;
    
    success &= config.set("max_steps", 100, ConfigSource::Programmatic,
                          "Debug preset: Very few simulation steps", "performance");
    success &= config.set("neuron_count", 100, ConfigSource::Programmatic,
                          "Debug preset: Few neurons", "brain");
    success &= config.set("region_count", 1, ConfigSource::Programmatic,
                          "Debug preset: Single region", "brain");
    success &= config.set("connection_probability", 0.01, ConfigSource::Programmatic,
                          "Debug preset: Very low connection probability", "connectivity");
    success &= config.set("enable_debug_mode", true, ConfigSource::Programmatic,
                          "Debug preset: Maximum debugging", "debug");
    success &= config.set("log_level", "DEBUG", ConfigSource::Programmatic,
                          "Debug preset: DEBUG log level", "logging");
    
    return success;
}

bool applyMinimal(Config& config) {
    bool success = true;
    
    success &= config.set("max_steps", 100, ConfigSource::Programmatic,
                          "Minimal preset: Very few simulation steps", "performance");
    success &= config.set("neuron_count", 10, ConfigSource::Programmatic,
                          "Minimal preset: Very few neurons", "brain");
    success &= config.set("region_count", 1, ConfigSource::Programmatic,
                          "Minimal preset: Single region", "brain");
    success &= config.set("connection_probability", 0.01, ConfigSource::Programmatic,
                          "Minimal preset: Minimal connectivity", "connectivity");
    success &= config.set("enable_debug_mode", false, ConfigSource::Programmatic,
                          "Minimal preset: No debug", "debug");
    
    return success;
}

bool applyPerformance(Config& config) {
    bool success = true;
    
    success &= config.set("max_steps", 10000, ConfigSource::Programmatic,
                          "Performance preset: Balanced steps", "performance");
    success &= config.set("neuron_count", 1000, ConfigSource::Programmatic,
                          "Performance preset: Standard neurons", "brain");
    success &= config.set("region_count", 2, ConfigSource::Programmatic,
                          "Performance preset: Standard regions", "brain");
    success &= config.set("connection_probability", 0.1, ConfigSource::Programmatic,
                          "Performance preset: Standard connectivity", "connectivity");
    success &= config.set("enable_simd", true, ConfigSource::Programmatic,
                          "Performance preset: SIMD enabled", "optimization");
    success &= config.set("enable_memory_pools", true, ConfigSource::Programmatic,
                          "Performance preset: Memory pools enabled", "optimization");
    
    return success;
}

bool applyExperimental(Config& config) {
    bool success = true;
    
    success &= config.set("max_steps", 100000, ConfigSource::Programmatic,
                          "Experimental preset: Extensive simulation", "performance");
    success &= config.set("neuron_count", 10000, ConfigSource::Programmatic,
                          "Experimental preset: Large network", "brain");
    success &= config.set("region_count", 5, ConfigSource::Programmatic,
                          "Experimental preset: Many regions", "brain");
    success &= config.set("connection_probability", 0.2, ConfigSource::Programmatic,
                          "Experimental preset: High connectivity", "connectivity");
    success &= config.set("enable_debug_mode", false, ConfigSource::Programmatic,
                          "Experimental preset: No debug", "debug");
    success &= config.set("enable_simd", true, ConfigSource::Programmatic,
                          "Experimental preset: SIMD enabled", "optimization");
    success &= config.set("enable_parallel", true, ConfigSource::Programmatic,
                          "Experimental preset: Parallel enabled", "optimization");
    
    return success;
}

} // namespace Presets

// ============================================================================
// Formatter Implementations
// ============================================================================

namespace Formatters {

std::string formatJSON(const Config& config) {
    std::ostringstream oss;
    oss << "{\n";
    
    auto keys = config.getKeys();n    for (size_t i = 0; i < keys.size(); ++i) {
        const auto& value = config.get(keys[i]);n        if (value.has_value()) {
            oss << "  \"" << keys[i] << "\": \";
            
            // Convert to JSON representation
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (val ? "true" : "false");
                } else {
                    oss << val;
                }
            }, value.value());
            
            if (i < keys.size() - 1) oss << ",\n";
        }
    }
    
    oss << "\n}\n";
    return oss.str();
}

std::string formatCSV(const Config& config) {
    std::ostringstream oss;
    oss << "key,value,type\n";
    
    auto keys = config.getKeys();n    for (const auto& key : keys) {
        auto value = config.get(key);n        if (value.has_value()) {
            oss << key << ",";
            
            // Convert to CSV representation
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    // Escape quotes in string
                    std::string strVal = val;
                    size_t pos = 0;
                    while ((pos = strVal.find('"', pos)) != std::string::npos) {
                        strVal.replace(pos, 1, "\"\"");
                        pos += 2;
                    }
                    oss << "\"" << strVal << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    oss << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    oss << std::setprecision(6) << val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    oss << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) oss << ",";
                        oss << val[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    oss << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) oss << ",";
                        oss << std::setprecision(6) << val[i];
                    }
                    oss << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) oss << ",";
                        std::string strVal = val[i];
                        size_t pos = 0;
                        while ((pos = strVal.find('"', pos)) != std::string::npos) {
                            strVal.replace(pos, 1, "\"\"");
                            pos += 2;
                        }
                        oss << "\"" << strVal << "\"";
                    }
                    oss << "]";
                }
            }, value.value());
            
            oss << "," << getTypeNameFromValue(value.value()) << "\n";
        }
    }
    
    return oss.str();
}

std::string formatHumanReadable(const Config& config) {
    std::ostringstream oss;
    oss << "=== NLM Configuration Summary ===\n\n";
    
    auto categories = config.getCategories();n    for (const auto& category : categories) {
        oss << "Category: " << category << "\n";
        auto keys = config.getKeysByCategory(category);n        for (const auto& key : keys) {
            auto value = config.get(key);n            if (value.has_value()) {
                oss << "  " << key << " = ";
                
                std::visit([&oss](const auto& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << val << "\"";
                    } else {
                        oss << val;
                    }
                }, value.value());
                
                oss << "\n";
            }
        }
        oss << "\n";
    }
    
    oss << "=== Configuration by Source ===\n";
    auto bySource = config.getBySource();n    for (const auto& pair : bySource) {
        oss << "Source " << static_cast<int>(pair.first) << ": " << pair.second.size() << " entries\n";
    }
    
    return oss.str();
}

std::string formatSummary(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration Summary\n";
    oss << "===================\n\n";
    
    oss << "Total entries: " << config.getKeys().size() << "\n";
    oss << "Categories: ";
    auto categories = config.getCategories();n    for (size_t i = 0; i < categories.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << categories[i];
    }
    oss << "\n";
    
    oss << "Last changed: " << std::chrono::system_clock::to_time_t(config.getAge()) << " seconds ago\n";
    oss << "Frozen: " << (config.isFrozen() ? "Yes" : "No") << "\n";
    
    return oss.str();
}

std::string formatDiff(const Config& config1, const Config& config2) {
    std::ostringstream oss;
    oss << "Configuration Difference Report\n";
    oss << "=============================\n\n";
    
    auto keys1 = config1.getKeys();n    auto keys2 = config2.getKeys();n    
    // Keys in config1 but not in config2
    std::set<std::string> set1(keys1.begin(), keys1.end());n    std::set<std::string> set2(keys2.begin(), keys2.end());n    
    std::set<std::string> onlyInConfig1, onlyInConfig2, inBoth;
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(inBoth, inBoth.begin()));n    std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(onlyInConfig1, onlyInConfig1.begin()));n    std::set_difference(set2.begin(), set2.end(), set1.begin(), set1.end(), std::inserter(onlyInConfig2, onlyInConfig2.begin()));n    
    if (!onlyInConfig1.empty()) {
        oss << "Only in Configuration 1:\n";
        for (const auto& key : onlyInConfig1) {
            auto value = config1.get(key);n            oss << "  " << key << " = ";
            if (value.has_value()) {
                std::visit([&oss](const auto& val) { oss << val; }, value.value());
            }
            oss << "\n";
        }
        oss << "\n";
    }
    
    if (!onlyInConfig2.empty()) {
        oss << "Only in Configuration 2:\n";
        for (const auto& key : onlyInConfig2) {
            auto value = config2.get(key);n            oss << "  " << key << " = ";
            if (value.has_value()) {
                std::visit([&oss](const auto& val) { oss << val; }, value.value());
            }
            oss << "\n";
        }
        oss << "\n";
    }
    
    if (!inBoth.empty()) {
        oss << "Differences between Configurations:\n";
        for (const auto& key : inBoth) {
            auto value1 = config1.get(key);n            auto value2 = config2.get(key);n            
            if (value1.has_value() && value2.has_value()) {
                // Compare values
                if (value1.value() != value2.value()) {
                    oss << "  " << key << ":\n";
                    oss << "    Config 1: ";
                    std::visit([&oss](const auto& val) { oss << val; }, value1.value());
                    oss << "\n";
                    oss << "    Config 2: ";
                    std::visit([&oss](const auto& val) { oss << val; }, value2.value());
                    oss << "\n";
                }
            }
        }
    }
    
    return oss.str();
}

// Helper function to get type name from ConfigValue
std::string getTypeNameFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int_vector";
    if (std::holds_alternative<std::vector<double>>(value)) return "double_vector";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string_vector";
    return "unknown";
}

} // namespace Formatters

} // namespace nlm