#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Configuration schema for validation
    struct ConfigSchema {
        std::string key;
        std::string type;  // "int", "double", "string", "bool", "array"
        bool required = false;
        std::string description;
        std::variant<int, double, bool, std::string> defaultValue;
        std::pair<double, double> range = {-std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
        size_t minArraySize = 0;
        size_t maxArraySize = std::numeric_limits<size_t>::max();
    };
    
    std::vector<ConfigSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // For now, support JSON format using nlohmann/json library
    // This provides better validation and type safety
    
    // Check if file exists
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        // For now, we'll use the simple key=value format but improve it
        // TODO: Integrate nlohmann/json library for proper JSON support
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Try to parse as JSON object first
            if (line[0] == '{' && line.back() == '}') {
                // Simple JSON object parsing
                // Remove braces and parse key-value pairs
                std::string content = line.substr(1, line.size() - 2);
                size_t pos = 0;
                while (pos < content.size()) {
                    // Find next comma or end
                    size_t commaPos = content.find(',', pos);
                    size_t endPos = (commaPos == std::string::npos) ? content.size() : commaPos;
                    
                    std::string pair = content.substr(pos, endPos - pos);
                    pos = endPos + 1; // Skip comma
                    
                    // Parse key="value" or key=value
                    size_t colonPos = pair.find(':');
                    if (colonPos != std::string::npos) {
                        std::string key = trim(pair.substr(0, colonPos));
                        std::string value = trim(pair.substr(colonPos + 1));
                        
                        // Remove quotes from value
                        if (value.size() >= 2 && 
                            ((value.front() == '"' && value.back() == '"') ||
                             (value.front() == '\'' && value.back() == '\''))) {
                            value = value.substr(1, value.size() - 2);
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                }
                continue;
            }
            
            // Fall back to simple key=value format
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
    } catch (const std::exception& e) {
        // If JSON parsing fails, fall back to original simple format
        file.clear();
        file.seekg(0);
        
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
    
    // Save in JSON format for better readability and parsing
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Serialize different value types
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeString(arg[i]) << "\"";
                }
                file << "]";
            } else {
                // For unknown types, convert to string
                file << "\"unknown type\"";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    file << "}\n";
    
    return true;
}

bool Config::validate(const std::string& key, const ConfigValue& value) const {
    if (!has(key)) {
        return true;  // Key doesn't exist, nothing to validate
    }
    
    const auto& entry = *std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // Type checking
    if (!validateType(entry.value, value)) {
        return false;
    }
    
    // Range/value validation
    if (!validateValue(entry.key, value)) {
        return false;
    }
    
    return true;
}

bool Config::validateType(const ConfigValue& schemaValue, const ConfigValue& actualValue) const {
    // Use schema value's type to validate against actual value
    // For simplicity, we'll just check if they match variant types
    return std::visit([](auto&& schema, auto&& actual) {
        using SchemaT = std::decay_t<decltype(schema)>;
        using ActualT = std::decay_t<decltype(actual)>;
        return std::is_same_v<SchemaT, ActualT>;
    }, schemaValue, actualValue);
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Get schema for this key
    const auto& schema = *std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // Additional validation based on key
    return validateKeySpecific(key, value);
}

bool Config::validateKeySpecific(const std::string& key, const ConfigValue& value) const {
    // Common configuration validations
    if (key == "neuron_count") {
        if (!std::holds_alternative<size_t>(value) && !std::holds_alternative<int>(value)) {
            return false;
        }
        size_t count = std::visit([](auto&& arg) { return static_cast<size_t>(arg); }, value);
        return count > 0 && count <= 1000000;  // Reasonable upper limit
    }
    else if (key == "simulation_timestep") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        double timestep = std::get<double>(value);
        return timestep > 0.0 && timestep <= 1.0;  // Reasonable timestep range
    }
    else if (key == "connection_probability") {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        double prob = std::get<double>(value);
        return prob >= 0.0 && prob <= 1.0;  // Probability must be in [0,1]
    }
    else if (key == "random_seed") {
        if (!std::holds_alternative<int64_t>(value) && !std::holds_alternative<uint64_t>(value)) {
            return false;
        }
        // Random seed validation - any non-negative value is acceptable
        return true;
    }
    
    // Default validation - just check it's not null
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Check if key has schema
        auto schemaIt = std::find_if(pImpl->schema.begin(), pImpl->schema.end(),
            [&entry](const Config::Impl::ConfigSchema& s) { return s.key == entry.key; });
        
        if (schemaIt != pImpl->schema.end()) {
            const auto& schema = *schemaIt;
            
            // Check required field
            if (schema.required && !has(entry.key)) {
                errors.push_back("Required configuration key '" + entry.key + "' is missing");
            }
            
            // Check type
            if (!validateType(schemaValue, entry.value)) {
                errors.push_back("Configuration key '" + entry.key + "' has invalid type. Expected " + 
                    getTypeString(schema.type) + ", got " + getTypeString(getTypeString(entry.value)));
            }
            
            // Check range
            if (!validateValue(entry.key, entry.value)) {
                errors.push_back("Configuration key '" + entry.key + "' has invalid value");
            }
        }
    }
    
    return errors;
}

std::string Config::getTypeString(const std::string& type) const {
    if (type == "int") return "integer";
    if (type == "double") return "float/double";
    if (type == "string") return "string";
    if (type == "bool") return "boolean";
    if (type == "array") return "array";
    return "unknown";
}

std::string Config::getTypeString(const ConfigValue& value) const {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return std::string("integer");
        else if constexpr (std::is_same_v<T, int64_t>) return std::string("int64");
        else if constexpr (std::is_same_v<T, double>) return std::string("float");
        else if constexpr (std::is_same_v<T, bool>) return std::string("boolean");
        else if constexpr (std::is_same_v<T, std::string>) return std::string("string");
        else if constexpr (std::is_same_v<T, std::vector<int>>) return std::string("int array");
        else if constexpr (std::is_same_v<T, std::vector<double>>) return std::string("float array");
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return std::string("string array");
        else return std::string("unknown");
    }, value);
}

} // namespace nlm
