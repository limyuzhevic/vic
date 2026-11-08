#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <random>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file type by extension or content
    std::string extension = std::filesystem::path(filepath).extension().string();
    bool success = false;
    
    if (extension == ".json" || extension == ".JSON") {
        success = loadFromJSON(file);
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".YAML" || extension == ".YML") {
        success = loadFromYAML(file);
    } else {
        // Try to auto-detect format from content
        file.clear();
        file.seekg(0, std::ios::beg);
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            firstLine = trim(firstLine);
            if (firstLine.find('{') != std::string::npos || firstLine.find('"') != std::string::npos) {
                success = loadFromJSON(file);
            } else if (firstLine.find('-') != std::string::npos || firstLine.find(':') != std::string::npos && firstLine.find(' ') != std::string::npos) {
                success = loadFromYAML(file);
            } else {
                // Fall back to simple key=value format
                success = loadFromSimpleFormat(file);
            }
        }
    }
    
    return success;
}

bool Config::loadFromJSON(std::ifstream& file) {
    try {
        // Simple JSON parser implementation for nested objects
        // Note: For production, use nlohmann/json library
        nlohmann_json::json jsonData;
        
        // Read entire file content
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Simple JSON parsing (would use nlohmann/json in production)
        if (parseSimpleJSON(content)) {
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYAML(std::ifstream& file) {
    try {
        // Simple YAML parser implementation
        // Note: For production, use yaml-cpp library
        
        // Read entire file content
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Simple YAML parsing (would use yaml-cpp in production)
        if (parseSimpleYAML(content)) {
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing YAML file: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Error parsing simple format file: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with error checking
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key.empty() || value.empty()) {
                    std::cerr << "Error: Invalid command line argument format '--" 
                              << key << "=\"" << value << "\"'. Both key and value must be non-empty." << std::endl;
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                std::cerr << "Error: Invalid command line argument format '--" 
                          << arg.substr(2) << "'. Expected '--key=value' format." << std::endl;
                success = false;
            }
        } 
        // Handle -key value format with error checking
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty() || value.empty()) {
                std::cerr << "Error: Invalid command line argument format '-" 
                          << key << " \"" << value << "\". Both key and value must be non-empty." << std::endl;
                success = false;
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            std::cerr << "Error: Invalid command line argument '--" << arg 
                      << "'. Expected '--key=value' or '-key value' format." << std::endl;
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

// Helper functions
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Config::parseSimpleJSON(const std::string& content) {
    // Simplified JSON parser for basic key-value pairs
    // In production, use nlohmann/json library
    size_t pos = content.find('{');
    if (pos == std::string::npos) return false;
    
    // Remove whitespace and parse simple JSON
    std::string cleaned = content;
    cleaned.erase(std::remove_if(cleaned.begin(), cleaned.end(), ::isspace), cleaned.end());
    
    if (cleaned.front() != '{' || cleaned.back() != '}') return false;
    
    cleaned = cleaned.substr(1, cleaned.size() - 2);
    
    size_t start = 0;
    while (start < cleaned.size()) {
        size_t colonPos = cleaned.find(':', start);
        if (colonPos == std::string::npos) break;
        
        size_t commaPos = cleaned.find(',', start);
        if (commaPos == std::string::npos) commaPos = cleaned.size();
        
        std::string key = cleaned.substr(start, colonPos - start);
        std::string value = cleaned.substr(colonPos + 1, commaPos - colonPos - 1);
        
        // Remove quotes from key and value
        key = trim(key);
        value = trim(value);
        
        if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
            key = key.substr(1, key.size() - 2);
        }
        
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
        
        start = commaPos + 1;
    }
    
    return true;
}

bool Config::parseSimpleYAML(const std::string& content) {
    // Simplified YAML parser for basic key-value pairs
    // In production, use yaml-cpp library
    std::string line;
    std::istringstream stream(content);
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        
        // Remove quotes from key and value
        if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
            key = key.substr(1, key.size() - 2);
        }
        
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.size() - 2);
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

std::string Config::valueToString(const ConfigValue& value) const {
    return std::visit([](auto&& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + v + "\"";
        } else {
            return std::to_string(v);
        }
    }, value);
}

ConfigSchema Config::createDefaultSchema() {
    ConfigSchema schema;
    
    // Add common configuration entries
    schema.addEntry("random_seed", ConfigType::Integer, "Random seed for reproducible experiments", 42, false);
    schema.addEntry("simulation_timestep", ConfigType::Double, "Simulation timestep in seconds", 0.001, false);
    schema.addEntry("neuron_count", ConfigType::Integer, "Number of neurons in the brain", 1000, false);
    schema.addEntry("region_count", ConfigType::Integer, "Number of brain regions", 2, false);
    schema.addEntry("connection_probability", ConfigType::Double, "Connection probability for random connectivity", 0.1, false);
    schema.addEntry("plasticity_learning_rate", ConfigType::Double, "Plasticity learning rate", 0.01, false);
    schema.addEntry("stdp_ltp_weight", ConfigType::Double, "STDP LTP weight", 0.01, false);
    schema.addEntry("stdp_ltd_weight", ConfigType::Double, "STDP LTD weight", 0.012, false);
    schema.addEntry("stdp_time_constant", ConfigType::Double, "STDP time constant", 20.0, false);
    schema.addEntry("development_synaptogenesis_rate", ConfigType::Double, "Development synaptogenesis rate", 0.001, false);
    schema.addEntry("development_pruning_rate", ConfigType::Double, "Development pruning rate", 0.0001, false);
    schema.addEntry("dopamine_baseline", ConfigType::Double, "Dopamine baseline value", 0.1, false);
    schema.addEntry("reward_discount_factor", ConfigType::Double, "Reward discount factor", 0.99, false);
    schema.addEntry("environment_name", ConfigType::String, "Environment name", std::string("GridWorld"), false);
    schema.addEntry("environment_width", ConfigType::Integer, "Environment width", 10, false);
    schema.addEntry("environment_height", ConfigType::Integer, "Environment height", 10, false);
    schema.addEntry("log_level", ConfigType::String, "Log level", std::string("INFO"), false);
    schema.addEntry("log_to_file", ConfigType::Boolean, "Log to file", false, false);
    schema.addEntry("log_filename", ConfigType::String, "Log filename", std::string("nlm.log"), false);
    schema.addEntry("max_simulation_steps", ConfigType::Integer, "Max simulation steps", 10000, false);
    schema.addEntry("simulation_time_limit", ConfigType::Double, "Simulation time limit", 0.0, false);
    schema.addEntry("visualization_enabled", ConfigType::Boolean, "Visualization enabled", false, false);
    schema.addEntry("visualization_update_rate", ConfigType::Double, "Visualization update rate", 30.0, false);
    
    return schema;
}

bool Config::validateAgainstSchema(const ConfigSchema& schema) const {
    std::vector<ValidationError> errors = pImpl->schema.validate(*this);
    
    if (!errors.empty()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  Key: \"" << error.key << "\" - " << error.message << std::endl;
        }
        return false;
    }
    
    return true;
}

// ConfigSchema implementation
ConfigSchema::ConfigSchema() {}

void ConfigSchema::addEntry(const std::string& key, ConfigType type, 
                           const std::string& description,
                           const std::any& defaultValue,
                           bool required,
                           const std::vector<std::any>& allowedValues,
                           const std::vector<std::pair<std::any, std::any>>& range,
                           const std::string& validationRegex) {
    ConfigSchemaEntry entry;
    entry.type = type;
    entry.description = description;
    entry.defaultValue = defaultValue;
    entry.required = required;
    entry.allowedValues = allowedValues;
    entry.range = range;
    entry.validationRegex = validationRegex;
    
    schema_[key] = entry;
}

std::vector<ValidationError> ConfigSchema::validate(const Config& config) const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : schema_) {
        const std::string& key = entry.first;
        const ConfigSchemaEntry& schemaEntry = entry.second;
        
        if (schemaEntry.required && !config.has(key)) {
            ValidationError error;
            error.key = key;
            error.message = "Required configuration key is missing";
            error.result = ValidationResult::Missing;
            errors.push_back(error);
            continue;
        }
        
        if (!config.has(key)) {
            continue; // Skip non-required missing keys
        }
        
        // Get the configuration value
        auto configValue = config.get<std::any>(key);
        if (!configValue.has_value()) {
            ValidationError error;
            error.key = key;
            error.message = "Configuration value cannot be retrieved";
            error.result = ValidationResult::Invalid;
            errors.push_back(error);
            continue;
        }
        
        // Type validation
        ValidationResult typeResult = validateType(key, schemaEntry.type, *configValue);
        if (typeResult != ValidationResult::Valid) {
            ValidationError error;
            error.key = key;
            error.message = "Type mismatch";
            error.result = typeResult;
            errors.push_back(error);
            continue;
        }
        
        // Range validation
        ValidationResult rangeResult = validateRange(key, schemaEntry.range, *configValue);
        if (rangeResult != ValidationResult::Valid) {
            ValidationError error;
            error.key = key;
            error.message = "Value out of range";
            error.result = rangeResult;
            errors.push_back(error);
            continue;
        }
        
        // Allowed values validation
        ValidationResult allowedResult = validateAllowedValues(key, schemaEntry.allowedValues, *configValue);
        if (allowedResult != ValidationResult::Valid) {
            ValidationError error;
            error.key = key;
            error.message = "Value not in allowed list";
            error.result = allowedResult;
            errors.push_back(error);
            continue;
        }
    }
    
    return errors;
}

ValidationResult ConfigSchema::validateType(const std::string& key, ConfigType expectedType, const std::any& value) const {
    ConfigType actualType = inferTypeFromAny(value);
    
    if (actualType == expectedType) {
        return ValidationResult::Valid;
    }
    
    // Allow some type conversions (e.g., int to double)
    if ((expectedType == ConfigType::Double && (actualType == ConfigType::Integer || actualType == ConfigType::Integer64)) ||
        (expectedType == ConfigType::String && (actualType == ConfigType::Integer || actualType == ConfigType::Integer64 || actualType == ConfigType::Double)) ||
        (expectedType == ConfigType::Boolean && actualType == ConfigType::Integer)) {
        return ValidationResult::Valid;
    }
    
    return ValidationResult::TypeMismatch;
}

ValidationResult ConfigSchema::validateRange(const std::string& key, const std::vector<std::pair<std::any, std::any>>& range, const std::any& value) const {
    if (range.empty()) return ValidationResult::Valid;
    
    double val = std::any_cast<double>(value);
    
    for (const auto& r : range) {
        double min = std::any_cast<double>(r.first);
        double max = std::any_cast<double>(r.second);
        
        if (val < min || val > max) {
            return ValidationResult::OutOfRange;
        }
    }
    
    return ValidationResult::Valid;
}

ValidationResult ConfigSchema::validateAllowedValues(const std::string& key, const std::vector<std::any>& allowedValues, const std::any& value) const {
    if (allowedValues.empty()) return ValidationResult::Valid;
    
    for (const auto& allowed : allowedValues) {
        if (value == allowed) {
            return ValidationResult::Valid;
        }
    }
    
    return ValidationResult::Invalid;
}

ConfigType ConfigSchema::inferTypeFromAny(const std::any& value) const {
    if (value.type() == typeid(int)) {
        return ConfigType::Integer;
    } else if (value.type() == typeid(int64_t)) {
        return ConfigType::Integer64;
    } else if (value.type() == typeid(double)) {
        return ConfigType::Double;
    } else if (value.type() == typeid(bool)) {
        return ConfigType::Boolean;
    } else if (value.type() == typeid(std::string)) {
        return ConfigType::String;
    } else if (value.type() == typeid(std::vector<int>)) {
        return ConfigType::IntVector;
    } else if (value.type() == typeid(std::vector<double>)) {
        return ConfigType::DoubleVector;
    } else if (value.type() == typeid(std::vector<std::string>)) {
        return ConfigType::StringVector;
    }
    
    return ConfigType::String; // Default to string
}

bool ConfigSchema::hasKey(const std::string& key) const {
    return schema_.find(key) != schema_.end();
}

ConfigType ConfigSchema::getType(const std::string& key) const {
    auto it = schema_.find(key);
    if (it != schema_.end()) {
        return it->second.type;
    }
    return ConfigType::String; // Default to string
}

std::any ConfigSchema::getDefaultValue(const std::string& key) const {
    auto it = schema_.find(key);
    if (it != schema_.end()) {
        return it->second.defaultValue;
    }
    return std::any();
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(schema_.size());
    for (const auto& entry : schema_) {
        keys.push_back(entry.first);
    }
    return keys;
}

// Additional Config methods
ConfigType Config::inferType(const std::string& value) {
    // Try to infer type from string
    if (value.empty()) return ConfigType::String;
    
    // Check if it's a boolean
    if (value == "true" || value == "false") return ConfigType::Boolean;
    
    // Check if it's an integer
    bool isInteger = true;
    for (char c : value) {
        if (!std::isdigit(c)) {
            isInteger = false;
            break;
        }
    }
    if (isInteger) return ConfigType::Integer;
    
    // Check if it's a floating point number
    bool isFloat = true;
    size_t dotPos = value.find('.');
    if (dotPos != std::string::npos) {
        for (size_t i = 0; i < value.size(); ++i) {
            if (i != dotPos && !std::isdigit(value[i])) {
                isFloat = false;
                break;
            }
        }
    } else {
        isFloat = false;
    }
    if (isFloat) return ConfigType::Double;
    
    return ConfigType::String;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Include nlohmann/json header for production use
// #include <nlohmann/json.hpp>

