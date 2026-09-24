#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 8: Implement proper JSON/YAML parser with schema validation
    // PLACEHOLDER - Phase 1 uses a simple key=value format with basic validation
    
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
            
            set(key, value, ConfigSource::File);
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

// Implementation of ConfigValidation class
namespace {

// Helper function to get type index from ConfigValue
std::type_index getTypeIndex(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::type_index {
        using T = std::decay_t<decltype(arg)>;
        return typeid(T);
    }, value);
}

// Helper function to convert string to value
ConfigValue stringToValue(const std::string& str) {
    // Try int first
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            return int64Val;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Try bool
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    if (lowerStr == "true") return true;
    if (lowerStr == "false") return false;
    
    // Return as string
    return str;
}

} // anonymous namespace

// ConfigValidation implementation
bool ConfigValidation::validateKey(const std::string& key, const ConfigValue& value,
                                   std::vector<ConfigValidationError>& errors) {
    auto schema = Config::Impl::schema;
    
    // Find schema entry for this key
    auto schemaIt = std::find_if(schema.begin(), schema.end(),
                                [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == schema.end()) {
        // Key not found in schema - this is a warning, not an error
        ConfigValidationError err;
        err.key = key;
        err.message = "Configuration key not found in schema";
        err.type = ConfigValidationError::ErrorType::Type;
        errors.push_back(err);
        return false; // Still invalid
    }
    
    const auto& configSchema = *schemaIt;
    
    // Check if value type matches schema
    std::type_index valueType = getTypeIndex(value);
    if (valueType != configSchema.type) {
        ConfigValidationError err;
        err.key = key;
        err.message = "Type mismatch: expected " + std::string(typeid(configSchema.type).name()) +
                     ", got " + std::string(valueType.name());
        err.type = ConfigValidationError::ErrorType::Type;
        errors.push_back(err);
        return false;
    }
    
    // Check range constraints
    if (configSchema.minValue != std::numeric_limits<double>::max() ||
        configSchema.maxValue != std::numeric_limits<double>::lowest()) {
        
        double doubleVal = 0.0;
        if (valueType == typeid(int)) {
            doubleVal = std::get<int>(value);
        } else if (valueType == typeid(int64_t)) {
            doubleVal = static_cast<double>(std::get<int64_t>(value));
        } else if (valueType == typeid(double)) {
            doubleVal = std::get<double>(value);
        }
        
        if (!withinRange(value, configSchema.minValue, configSchema.maxValue)) {
            ConfigValidationError err;
            err.key = key;
            err.message = "Value " + std::to_string(doubleVal) +
                         " is outside allowed range [" + std::to_string(configSchema.minValue) +
                         ", " + std::to_string(configSchema.maxValue) + "]";
            err.type = ConfigValidationError::ErrorType::Range;
            errors.push_back(err);
            return false;
        }
    }
    
    // Check value constraints
    if (!configSchema.description.empty() && configSchema.description != "unbounded") {
        // Additional validation can be added here based on specific key constraints
        if (key == "checkpoint_compression") {
            int comp = std::get<int>(value);
            if (comp < 0 || comp > 3) {
                ConfigValidationError err;
                err.key = key;
                err.message = "Compression level must be between 0 and 3";
                err.type = ConfigValidationError::ErrorType::Range;
                errors.push_back(err);
                return false;
            }
        } else if (key == "log_level") {
            int level = std::get<int>(value);
            if (level < 0 || level > 4) {
                ConfigValidationError err;
                err.key = key;
                err.message = "Log level must be between 0 and 4";
                err.type = ConfigValidationError::ErrorType::Range;
                errors.push_back(err);
                return false;
            }
        } else if (key == "developmental_stage") {
            int stage = std::get<int>(value);
            if (stage < 0 || stage > 3) {
                ConfigValidationError err;
                err.key = key;
                err.message = "Developmental stage must be between 0 and 3";
                err.type = ConfigValidationError::ErrorType::Range;
                errors.push_back(err);
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigValidation::validateAll(const Config& config, std::vector<ConfigValidationError>& errors) {
    bool allValid = true;
    
    for (const auto& entry : config.getKeys()) {
        std::vector<ConfigValidationError> keyErrors;
        if (!validateKey(entry, config.get<std::string>(entry).value_or(""), keyErrors)) {
            allValid = false;
            errors.insert(errors.end(), keyErrors.begin(), keyErrors.end());
        }
    }
    
    return allValid;
}

bool ConfigValidation::requiresKey(const std::string& key, const std::type_index& type) {
    auto schema = Config::Impl::schema;
    auto schemaIt = std::find_if(schema.begin(), schema.end(),
                                [&key](const ConfigSchema& s) { return s.key == key; });
    
    return (schemaIt != schema.end() && schemaIt->type == type);
}

bool ConfigValidation::withinRange(const ConfigValue& value, double min, double max) {
    std::type_index type = getTypeIndex(value);
    
    if (type == typeid(int)) {
        int val = std::get<int>(value);
        return val >= min && val <= max;
    } else if (type == typeid(int64_t)) {
        int64_t val = std::get<int64_t>(value);
        return val >= min && val <= max;
    } else if (type == typeid(double)) {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    } else if (type == typeid(float)) {
        float val = std::get<float>(value);
        return val >= min && val <= max;
    }
    
    return true; // Non-numeric types don't have range constraints
}

std::vector<std::string> ConfigValidation::getValidKeys() {
    std::vector<std::string> keys;
    auto schema = Config::Impl::schema;
    
    for (const auto& entry : schema) {
        keys.push_back(entry.key);
    }
    
    return keys;
}

std::type_index ConfigValidation::getKeyType(const std::string& key) {
    auto schema = Config::Impl::schema;
    auto schemaIt = std::find_if(schema.begin(), schema.end(),
                                [&key](const ConfigSchema& s) { return s.key == key; });
    
    return (schemaIt != schema.end()) ? schemaIt->type : typeid(std::string);
}

bool ConfigValidation::hasRangeConstraint(const std::string& key, double& min, double& max) {
    auto schema = Config::Impl::schema;
    auto schemaIt = std::find_if(schema.begin(), schema.end(),
                                [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == schema.end()) return false;
    
    min = schemaIt->minValue;
    max = schemaIt->maxValue;
    
    // Check if this is a valid range (not default sentinel values)
    return (min != std::numeric_limits<double>::max() &&
            max != std::numeric_limits<double>::lowest());
}

bool ConfigValidation::hasValueConstraint(const std::string& key, const std::string& constraint) {
    auto schema = Config::Impl::schema;
    auto schemaIt = std::find_if(schema.begin(), schema.end(),
                                [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == schema.end()) return false;
    
    return schemaIt->description == constraint;
}

// Set method with validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the key-value pair
    std::vector<ConfigValidationError> errors;
    if (!ConfigValidation::validateKey(key, value, errors)) {
        // Log validation errors but still allow the operation
        for (const auto& err : errors) {
            NLM_LOG_WARNING("Configuration validation warning: " + err.key + " - " + err.message);
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

// Get keys with validation
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        // Only return keys that are in the schema
        auto schemaIt = std::find_if(Config::Impl::schema.begin(), Config::Impl::schema.end(),
                                    [&entry](const ConfigSchema& s) { return s.key == entry.key; });
        if (schemaIt != Config::Impl::schema.end()) {
            keys.push_back(entry.key);
        } else {
            // Include unknown keys but mark them as invalid
            NLM_LOG_WARNING("Unknown configuration key: " + entry.key);
        }
    }
    
    return keys;
}

// Summary with validation info
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    int validCount = 0;
    int invalidCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        // Check if key is in schema
        auto schemaIt = std::find_if(Config::Impl::schema.begin(), Config::Impl::schema.end(),
                                    [&entry](const ConfigSchema& s) { return s.key == entry.key; });
        
        if (schemaIt != Config::Impl::schema.end()) {
            // Check if value type matches schema
            std::type_index valueType = getTypeIndex(entry.value);
            if (valueType == schemaIt->type) {
                validCount++;
            } else {
                invalidCount++;
            }
        } else {
            invalidCount++;
        }
        
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (schemaIt != Config::Impl::schema.end()) {
            if (valueType != schemaIt->type) {
                oss << " *TYPE MISMATCH*";
            }
        } else {
            oss << " *UNKNOWN KEY*";
        }
        
        oss << "\n";
    }
    
    oss << "\nValidation Summary: " << validCount << " valid, " << invalidCount << " invalid entries\n";
    return oss.str();
}

// String to value conversion helper
ConfigValue Config::stringToValue(const std::string& str) {
    return ::stringToValue(str);
}

// Get schema helper
std::vector<ConfigSchema> Config::getSchema() {
    return Config::Impl::schema;
}

// Validate all configuration
bool Config::validateAll(std::vector<ConfigValidationError>& errors) {
    return ConfigValidation::validateAll(*this, errors);
}
