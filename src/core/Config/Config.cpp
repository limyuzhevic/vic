#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <cctype>
#include <chrono>
#include <ctime>

namespace nlm {

// Environment variable getter utilities
namespace env {
    static std::string get(const std::string& key, const std::string& defaultValue = "") {
        const char* value = std::getenv(key.c_str());
        return value ? value : defaultValue;
    }
    
    static bool has(const std::string& key) {
        return std::getenv(key.c_str()) != nullptr;
    }
    
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
}

// JSON utilities (simplified implementation)
namespace json {
    struct JsonValue {
        enum class Type { Null, String, Number, Boolean, Object, Array };
        
        Type type;
        std::string stringValue;
        double numberValue;
        bool booleanValue;
        std::unordered_map<std::string, JsonValue> objectValue;
        std::vector<JsonValue> arrayValue;
        
        JsonValue() : type(Type::Null) {}
        JsonValue(const std::string& s) : type(Type::String), stringValue(s) {}
        JsonValue(double n) : type(Type::Number), numberValue(n) {}
        JsonValue(bool b) : type(Type::Boolean), booleanValue(b) {}
        JsonValue(const std::unordered_map<std::string, JsonValue>& obj) : type(Type::Object), objectValue(obj) {}
        JsonValue(const std::vector<JsonValue>& arr) : type(Type::Array), arrayValue(arr) {}
    };
    
    class JsonParser {
    public:
        static std::optional<JsonValue> parse(const std::string& jsonStr) {
            // Simplified JSON parser - handles basic JSON
            std::string trimmed = trim(jsonStr);
            if (trimmed.empty() || (trimmed.front() != '{' && trimmed.front() != '[')) {
                return std::nullopt;
            }
            
            // For now, return nullopt for complex JSON parsing
            // In a real implementation, this would use a proper JSON parser
            return std::nullopt;
        }
        
        static std::string toString(const JsonValue& value) {
            switch (value.type) {
                case JsonValue::Type::Null: return "null";
                case JsonValue::Type::String: return "\"" + value.stringValue + "\"";
                case JsonValue::Type::Number: return std::to_string(value.numberValue);
                case JsonValue::Type::Boolean: return value.booleanValue ? "true" : "false";
                case JsonValue::Type::Object: {
                    std::string result = "{";
                    bool first = true;
                    for (const auto& pair : value.objectValue) {
                        if (!first) result += ", ";
                        result += "\"" + pair.first + "\": " + toString(pair.second);
                        first = false;
                    }
                    result += "}";
                    return result;
                }
                case JsonValue::Type::Array: {
                    std::string result = "[";
                    bool first = true;
                    for (const auto& item : value.arrayValue) {
                        if (!first) result += ", ";
                        result += toString(item);
                        first = false;
                    }
                    result += "]";
                    return result;
                }
            }
            return "";
        }
        
    private:
        static std::string trim(const std::string& str) {
            size_t start = str.find_first_not_of(" \t\r\n");
            size_t end = str.find_last_not_of(" \t\r\n");
            if (start == std::string::npos) return "";
            return str.substr(start, end - start + 1);
        }
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigEntry*> entriesByKey;
    
    // Configuration schema for validation
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    // Default values
    std::unordered_map<std::string, ConfigValue> defaults;
    
    // Configuration flags
    bool strictMode;
    bool environmentSupport;
    bool jsonSupport;
    
    Impl() : strictMode(false), environmentSupport(true), jsonSupport(true) {
        // Register default validators
        registerDefaultValidators();
    }
    
    void registerDefaultValidators() {
        // Basic type validators
        validators["int"] = [](const ConfigValue& v) {
            return std::holds_alternative<int>(v);
        };
        validators["int64"] = [](const ConfigValue& v) {
            return std::holds_alternative<int64_t>(v);
        };
        validators["double"] = [](const ConfigValue& v) {
            return std::holds_alternative<double>(v);
        };
        validators["bool"] = [](const ConfigValue& v) {
            return std::holds_alternative<bool>(v);
        };
        validators["string"] = [](const ConfigValue& v) {
            return std::holds_alternative<std::string>(v);
        };
        validators["float"] = [](const ConfigValue& v) {
            return std::holds_alternative<double>(v); // Treat float as double
        };
    }
    
    ~Impl() {
        for (auto& pair : entriesByKey) {
            delete pair.second;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
            continue;
        }
        
        // Try to parse as simple key=value
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
            continue;
        }
        
        // Try to parse as JSON
        if (pImpl->jsonSupport) {
            auto jsonValue = json::JsonParser::parse(line);
            if (jsonValue) {
                // For now, skip JSON parsing for complex cases
                // In a real implementation, this would use a proper JSON parser
                NLM_LOG_DEBUG("JSON parsing not yet implemented for: " + line.substr(0, 50));
                continue;
            }
        }
        
        // Try to parse environment variable reference
        if (value.find("${") != std::string::npos && value.find("}") != std::string::npos) {
            // Environment variable expansion
            size_t start = value.find("${");
            size_t end = value.find("}", start);
            if (start != std::string::npos && end != std::string::npos) {
                std::string envKey = value.substr(start + 2, end - start - 2);
                std::string envValue = env::get(envKey);
                if (!envValue.empty()) {
                    value.replace(start, end - start + 1, envValue);
                }
            }
        }
        
        // Parse the value
        parseValue(key, value, ConfigSource::File);
    }
    
    if (!success && pImpl->strictMode) {
        NLM_LOG_ERROR("Some lines in config file failed to parse: " + filepath);
        return false;
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
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# " << std::ctime(&std::time(nullptr)) << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Convert value to string based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << std::endl;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->entriesByKey.find(key);
    if (it == pImpl->entriesByKey.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second->value);
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
    // Check if this is a known key with type validation
    if (pImpl->validators.find(key) != pImpl->validators.end()) {
        // Validate against schema
        if (!pImpl->validators[key](value)) {
            NLM_LOG_WARNING("Invalid value for key " + key + ": " + valueToString(value));
            if (pImpl->strictMode) {
                return;
            }
        }
    }
    
    auto it = pImpl->entriesByKey.find(key);
    if (it != pImpl->entriesByKey.end()) {
        delete it->second;
        it->second = new ConfigEntry(key, value, source, getDescription(key));
    } else {
        pImpl->entries.emplace_back(key, value, source, getDescription(key));
        pImpl->entriesByKey[key] = &pImpl->entries.back();
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Parse the string value into appropriate type
    parseValue(key, value, source);
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

bool Config::has(const std::string& key) const {
    return pImpl->entriesByKey.find(key) != pImpl->entriesByKey.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->entriesByKey.find(key);
    if (it != pImpl->entriesByKey.end()) {
        // Remove from vector
        auto& entries = pImpl->entries;
        entries.erase(std::remove_if(entries.begin(), entries.end(),
                                     [&key](const ConfigEntry& e) { return e.key == key; }),
                      entries.end());
        
        // Remove from map
        delete it->second;
        pImpl->entriesByKey.erase(it);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entriesByKey.size());
    for (const auto& pair : pImpl->entriesByKey) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    for (auto& pair : pImpl->entriesByKey) {
        delete pair.second;
    }
    pImpl->entries.clear();
    pImpl->entriesByKey.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "  Strict mode: " << (pImpl->strictMode ? "enabled" : "disabled") << "\n";
    oss << "  Environment support: " << (pImpl->environmentSupport ? "enabled" : "disabled") << "\n";
    oss << "  JSON support: " << (pImpl->jsonSupport ? "enabled" : "disabled") << "\n";
    oss << "\n  Key : Value : Source : Description\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "    " << entry.key << " : ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " : " << static_cast<int>(entry.source) << " : " << entry.description << "\n";
    }
    
    return oss.str();
}

void Config::parseValue(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to determine type based on content
    std::string lowerValue = toLower(valueStr);
    
    // Check for boolean
    if (lowerValue == "true" || lowerValue == "false") {
        bool boolVal = (lowerValue == "true");
        set(key, boolVal, source);
        return;
    }
    
    // Check for number
    bool isNumber = true;
    double numVal = 0.0;
    size_t pos = 0;
    if (valueStr[0] == '-') pos = 1;
    
    for (; pos < valueStr.size(); ++pos) {
        if (!std::isdigit(valueStr[pos]) && valueStr[pos] != '.') {
            isNumber = false;
            break;
        }
    }
    
    if (isNumber) {
        numVal = std::stod(valueStr);
        set(key, numVal, source);
        return;
    }
    
    // Check for integer
    bool isInteger = true;
    for (size_t i = 0; i < valueStr.size(); ++i) {
        if (valueStr[i] == '-') continue;
        if (!std::isdigit(valueStr[i])) {
            isInteger = false;
            break;
        }
    }
    
    if (isInteger && !valueStr.empty() && valueStr[0] != '.') {
        try {
            int intVal = std::stoi(valueStr);
            set(key, intVal, source);
            return;
        } catch (...) {
            // Fall through to string
        }
    }
    
    // Default to string
    set(key, valueStr, source);
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

std::string Config::getDescription(const std::string& key) const {
    // Default descriptions for common keys
    static const std::unordered_map<std::string, std::string> defaultDescriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"plasticity_learning_rate", "Plasticity learning rate"},
        {"stdp_ltp_weight", "STDP LTP weight"},
        {"stdp_ltd_weight", "STDP LTD weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"development_synaptogenesis_rate", "Development synaptogenesis rate"},
        {"development_pruning_rate", "Development pruning rate"},
        {"dopamine_baseline", "Dopamine baseline level"},
        {"reward_discount_factor", "Reward discount factor"},
        {"environment_name", "Environment name"},
        {"environment_width", "Environment width"},
        {"environment_height", "Environment height"},
        {"log_level", "Log level"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Log filename"},
        {"max_simulation_steps", "Maximum simulation steps"},
        {"simulation_time_limit", "Simulation time limit"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate"}
    };
    
    auto it = defaultDescriptions.find(key);
    if (it != defaultDescriptions.end()) {
        return it->second;
    }
    return "Configuration parameter: " + key;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int6464_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm