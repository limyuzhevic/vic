// Enhanced Configuration System for NLM
// Supports JSON files, validation, and schema documentation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <chrono>
#include <random>

// JSON parsing support (simplified for now, could be enhanced with proper JSON library)
namespace json {
    class JsonValue {
    public:
        enum class Type {
            Null,
            Bool,
            Number,
            String,
            Array,
            Object
        };
        
        JsonValue() : type_(Type::Null) {}
        JsonValue(bool b) : type_(Type::Bool), bool_value_(b) {}
        JsonValue(double n) : type_(Type::Number), number_value_(n) {}
        JsonValue(int n) : type_(Type::Number), number_value_(static_cast<double>(n)) {}
        JsonValue(const std::string& s) : type_(Type::String), string_value_(s) {}
        JsonValue(const std::vector<double>& arr) : type_(Type::Array), array_value_(arr) {}
        JsonValue(const std::vector<std::string>& arr) : type_(Type::Array), string_array_value_(arr) {}
        
        Type getType() const { return type_; }
        
        bool isNull() const { return type_ == Type::Null; }
        bool isBool() const { return type_ == Type::Bool; }
        bool isNumber() const { return type_ == Type::Number; }
        bool isString() const { return type_ == Type::String; }
        bool isArray() const { return type_ == Type::Array; }
        bool isObject() const { return type_ == Type::Object; }
        
        bool getBool() const { return bool_value_; }
        double getNumber() const { return number_value_; }
        const std::string& getString() const { return string_value_; }
        const std::vector<double>& getNumberArray() const { return array_value_; }
        const std::vector<std::string>& getStringArray() const { return string_array_value_; }
        
    private:
        Type type_;
        union {
            bool bool_value_;
            double number_value_;
            std::string string_value_;
            std::vector<double> array_value_;
            std::vector<std::string> string_array_value_;
        };
    };
    
    // Simple JSON parser (could be enhanced with proper JSON library)
    class JsonParser {
    public:
        static JsonValue parse(const std::string& json);
        static bool isValidJson(const std::string& json);
    };
}

// Configuration schema definitions
struct ConfigSchema {
    std::string key;
    std::string description;
    std::string type;  // "int", "double", "bool", "string", "array"
    std::string defaultValue;
    bool required;
    double minValue;
    double maxValue;
    std::vector<std::string> allowedValues;
    std::string validationRegex;
    
    ConfigSchema(const std::string& k, const std::string& desc, const std::string& t = "string",
                 const std::string& def = "", bool req = false, double min = -1e9, double max = 1e9)
        : key(k), description(desc), type(t), defaultValue(def), required(req), 
          minValue(min), maxValue(max) {}
};

// Enhanced Config implementation
namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schema;
    bool fileFormat;  // true = JSON, false = simple key=value
};

// Configuration schema initialization
static std::vector<ConfigSchema> initializeConfigSchema() {
    return {
        // Brain configuration
        {"brain.neuron_count", "Number of neurons in the brain", "int", "1000", true, 1, 10000},
        {"brain.region_count", "Number of brain regions", "int", "2", true, 1, 10},
        {"brain.synapse_density", "Connection probability for random connectivity", "double", "0.1", true, 0.0, 1.0},
        {"brain.initial_weight_mean", "Mean initial synaptic weight", "double", "0.5", false, -1.0, 1.0},
        {"brain.initial_weight_std", "Standard deviation of initial weights", "double", "0.1", false, 0.0, 1.0},
        {"brain.v_thresh", "Neuron threshold potential (mV)", "double", "-50.0", false, -100.0, 0.0},
        {"brain.v_rest", "Resting potential (mV)", "double", "-70.0", false, -100.0, 0.0},
        {"brain.v_reset", "Reset potential after spike (mV)", "double", "-75.0", false, -100.0, 0.0},
        {"brain.tau_mem", "Membrane time constant (ms)", "double", "20.0", false, 1.0, 100.0},
        {"brain.tau_ref", "Refractory period (ms)", "double", "2.0", false, 0.0, 20.0},
        
        // Plasticity configuration
        {"plasticity.stdp.enable", "Enable STDP", "bool", "true", false},
        {"plasticity.stdp.learning_rate", "STDP learning rate", "double", "0.001", false, 0.0, 0.1},
        {"plasticity.stdp.tau_plus", "STDP LTP time constant (ms)", "double", "20.0", false, 1.0, 100.0},
        {"plasticity.stdp.tau_minus", "STDP LTD time constant (ms)", "double", "20.0", false, 1.0, 100.0},
        {"plasticity.hebbian.enable", "Enable Hebbian learning", "bool", "true", false},
        {"plasticity.structural.enable", "Enable structural plasticity", "bool", "true", false},
        {"plasticity.synaptogenesis_rate", "Rate of new synapse formation", "double", "0.0001", false, 0.0, 1.0},
        {"plasticity.pruning_rate", "Rate of synapse elimination", "double", "0.00001", false, 0.0, 1.0},
        
        // Neuromodulation configuration
        {"neuromod.dopamine.scale", "Dopamine modulation scale", "double", "1.0", false, 0.0, 10.0},
        {"neuromod.curiosity.enable", "Enable curiosity-driven exploration", "bool", "true", false},
        {"neuromod.novelty.enable", "Enable novelty detection", "bool", "true", false},
        
        // Development configuration
        {"development.synaptogenesis_rate", "Developmental synaptogenesis rate", "double", "0.001", false, 0.0, 1.0},
        {"development.pruning_rate", "Developmental pruning rate", "double", "0.0001", false, 0.0, 1.0},
        {"development.maturation_rate", "Neural maturation rate", "double", "0.05", false, 0.0, 1.0},
        
        // Memory configuration
        {"memory.working_memory_size", "Working memory capacity", "int", "100", false, 10, 1000},
        {"memory.episodic_capacity", "Episodic memory capacity", "int", "1000", false, 100, 10000},
        
        // Simulation configuration
        {"simulation.timestep", "Simulation timestep (s)", "double", "0.001", false, 0.0001, 1.0},
        {"simulation.max_steps", "Maximum simulation steps", "int", "10000", false, 100, 100000},
        {"simulation.time_limit", "Simulation time limit (s)", "double", "0.0", false, 0.0, 1000.0},
        
        // Logging configuration
        {"log.level", "Log level (Debug=0, Info=1, Warning=2, Error=3, Critical=4)", "int", "1", false, 0, 4},
        {"log.to_file", "Enable logging to file", "bool", "false", false},
        {"log.filename", "Log filename", "string", "nlm.log", false},
        
        // Environment configuration
        {"environment.name", "Environment name", "string", "GridWorld", false},
        {"environment.width", "Environment width", "int", "10", false, 1, 100},
        {"environment.height", "Environment height", "int", "10", false, 1, 100},
        {"environment.vision_width", "Agent vision width", "int", "8", false, 1, 50},
        {"environment.vision_height", "Agent vision height", "int", "8", false, 1, 50},
        
        // Reward configuration
        {"reward.discount_factor", "Reward discount factor", "double", "0.99", false, 0.0, 1.0},
        {"reward.exploration_bonus", "Exploration bonus", "double", "0.1", false, 0.0, 10.0},
        {"reward.exploitation_bonus", "Exploitation bonus", "double", "1.0", false, 0.0, 10.0},
    };
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = initializeConfigSchema();
    pImpl->fileFormat = false;  // Default to simple format
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON first, then fallback to simple format
    if (json::JsonParser::isValidJson(content)) {
        return loadFromJson(content);
    } else {
        return loadFromSimpleFormat(content);
    }
}

// Load from JSON format
bool Config::loadFromJson(const std::string& jsonContent) {
    pImpl->fileFormat = true;
    
    // For now, use simple parsing - in Phase 2 this would use proper JSON library
    // Simplified JSON parsing: extract key=value pairs from JSON-like structure
    std::istringstream stream(jsonContent);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line = trim(line);
        
        // Skip comments and empty lines
        if (line.empty() || line.substr(0, 2) == "//" || line.substr(0, 2) == "#") {
            continue;
        }
        
        // Parse JSON key-value pairs (simplified)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes and trailing comma
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            if (value.back() == ',') {
                value.pop_back();
            }
            
            // Convert value to appropriate type based on key
            ConfigValue configValue = parseJsonValue(value);
            
            // Apply validation before setting
            if (validateConfigEntry(key, configValue)) {
                set(key, configValue, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Invalid configuration value for key: " + key);
            }
        }
    }
    
    return true;
}

// Load from simple key=value format (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& content) {
    pImpl->fileFormat = false;
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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
    
    return true;
}

// Parse JSON value to appropriate type
ConfigValue Config::parseJsonValue(const std::string& value) {
    // Try to determine type
    if (value == "true" || value == "false") {
        return value == "true";
    }
    
    // Check if it's a number (integer or float)
    bool isNumber = true;
    bool isInteger = true;
    size_t pos = 0;
    
    if (value[0] == '-' || value[0] == '+') {
        pos = 1;
    }
    
    for (; pos < value.size(); ++pos) {
        if (value[pos] == '.') {
            isInteger = false;
        } else if (!std::isdigit(value[pos])) {
            isNumber = false;
            break;
        }
    }
    
    if (isNumber && pos > 0) {
        if (isInteger) {
            try {
                return std::stoll(value);
            } catch (...) {
                // Fall through to double
            }
        }
        
        try {
            return std::stod(value);
        } catch (...) {
            // Fall through to string
        }
    }
    
    // Check for array format [val1, val2, ...]
    if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
        std::string inner = value.substr(1, value.size() - 2);
        std::vector<double> numbers;
        
        std::stringstream arrayStream(inner);
        std::string item;
        
        while (std::getline(arrayStream, item, ',')) {
            item = trim(item);
            try {
                numbers.push_back(std::stod(item));
            } catch (...) {
                // Skip invalid items
            }
        }
        
        if (!numbers.empty()) {
            return numbers;
        }
    }
    
    // Default to string
    return value;
}

// Enhanced configuration validation
bool Config::validateConfigEntry(const std::string& key, const ConfigValue& value) const {
    // Check if key exists in schema
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt == pImpl->schema.end()) {
        // Unknown key, still accept it (backward compatibility)
        return true;
    }
    
    const ConfigSchema& schema = schemaIt->second;
    
    // Validate type
    if (!validateType(key, value, schema.type)) {
        return false;
    }
    
    // Validate range
    if (!validateRange(key, value, schema)) {
        return false;
    }
    
    return true;
}

// Type validation
bool Config::validateType(const std::string& key, const ConfigValue& value, const std::string& expectedType) const {
    switch (value.index()) {
        case 0: // int
            return expectedType == "int" || expectedType == "number";
            
        case 1: // int64_t
            return expectedType == "int" || expectedType == "number";
            
        case 2: // double
            return expectedType == "double" || expectedType == "number";
            
        case 3: // bool
            return expectedType == "bool";
            
        case 4: // string
            return expectedType == "string";
            
        case 5: // vector<int>
            return expectedType == "array" || expectedType == "vector<int>";
            
        case 6: // vector<double>
            return expectedType == "array" || expectedType == "vector<double>";
            
        case 7: // vector<string>
            return expectedType == "array" || expectedType == "vector<string>";
            
        default:
            return false;
    }
}

// Range validation
bool Config::validateRange(const std::string& key, const ConfigValue& value, const ConfigSchema& schema) const {
    try {
        double numericValue = 0.0;
        
        switch (value.index()) {
            case 0: // int
                numericValue = static_cast<double>(std::get<int>(value));
                break;
            case 1: // int64_t
                numericValue = static_cast<double>(std::get<int64_t>(value));
                break;
            case 2: // double
                numericValue = std::get<double>(value);
                break;
            case 3: // bool
                numericValue = std::get<bool>(value) ? 1.0 : 0.0;
                break;
            case 4: // string
                // String validation based on content
                if (schema.validationRegex.empty()) {
                    return true;  // No validation regex specified
                }
                // Simple regex check (could be enhanced with proper regex library)
                return validateStringWithRegex(std::get<std::string>(value), schema.validationRegex);
            default:
                return true;  // Array types - basic validation
        }
        
        if (numericValue < schema.minValue) {
            NLM_LOG_WARNING("Configuration " + key + " value " + std::to_string(numericValue) + 
                           " is below minimum " + std::to_string(schema.minValue));
            return false;
        }
        
        if (numericValue > schema.maxValue) {
            NLM_LOG_WARNING("Configuration " + key + " value " + std::to_string(numericValue) + 
                           " exceeds maximum " + std::to_string(schema.maxValue));
            return false;
        }
        
        return true;
        
    } catch (...) {
        NLM_LOG_WARNING("Error validating configuration " + key);
        return false;
    }
}

// String validation with regex (simplified)
bool Config::validateStringWithRegex(const std::string& value, const std::string& regex) const {
    // Simple validation - could be enhanced with proper regex library
    if (regex == "^alpha.*$") {
        return value.find("alpha") == 0;
    } else if (regex == "^[a-zA-Z0-9_]+$") {
        return std::all_of(value.begin(), value.end(), 
                          [](char c) { return std::isalnum(c) || c == '_'; });
    }
    return true;  // No specific validation
}

// Enhanced save to file with JSON format support
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pImpl->fileFormat) {
        // Save in JSON format
        file << "{" << std::endl;
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < arg.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << arg[j];
                    }
                    file << "]";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << "," << std::endl;
            }
        }
        file << std::endl << "}" << std::endl;
    } else {
        // Save in simple key=value format (backward compatibility)
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "# " << entry.description << std::endl;
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << std::endl << std::endl;
        }
    }
    
    return true;
}

// Enhanced summary with validation information
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "] (Source: " << static_cast<int>(entry.source) << ")";
        
        // Add validation info if available
        auto schemaIt = pImpl->schema.find(entry.key);
        if (schemaIt != pImpl->schema.end()) {
            oss << " [Validated]";
        }
        
        oss << " - " << entry.description << std::endl;
    }
    
    if (!pImpl->schema.empty()) {
        oss << std::endl << "Schema Information:" << std::endl;
        oss << "  Total schema entries: " << pImpl->schema.size() << std::endl;
        oss << "  File format: " << (pImpl->fileFormat ? "JSON" : "Simple key=value") << std::endl;
        
        // Count validated entries
        int validatedCount = 0;
        for (const auto& entry : pImpl->entries) {
            if (pImpl->schema.find(entry.key) != pImpl->schema.end()) {
                validatedCount++;
            }
        }
        oss << "  Validated entries: " << validatedCount << "/" << pImpl->entries.size() << std::endl;
    }
    
    return oss.str();
}

// Additional helper methods
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

// JSON parser implementation (simplified)
namespace json {
    JsonValue JsonParser::parse(const std::string& json) {
        // Simplified JSON parser - would use proper library in production
        if (json.empty() || json[0] != '{' || json.back() != '}') {
            return JsonValue();  // Return null for invalid JSON
        }
        
        // For now, return null - this would be enhanced with proper JSON parsing
        return JsonValue();
    }
    
    bool JsonParser::isValidJson(const std::string& json) {
        if (json.empty()) return false;
        
        // Simple JSON validation
        int braceCount = 0;
        bool inString = false;
        bool escaped = false;
        
        for (char c : json) {
            if (!inString) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
                else if (c == '"') inString = true;
            } else {
                if (escaped) {
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == '"') {
                    inString = false;
                }
            }
            
            if (braceCount < 0) return false;
        }
        
        return braceCount == 0 && !inString;
    }
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