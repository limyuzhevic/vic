#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <unordered_set>

namespace nlm {

// Add forward declarations for helper classes
struct JSONValue;
struct JSONObject;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Statistics for configuration tracking
    size_t totalEntries;
    size_t fromFile;
    size_t fromCommandLine;
    size_t fromDefault;
    size_t fromRuntime;
    
    // Configuration groups
    std::vector<ConfigGroup> groups;
    
    // Schema validation
    std::vector<ConfigSchemaEntry> schema;
    
    Impl() 
        : totalEntries(0), fromFile(0), fromCommandLine(0), fromDefault(0), fromRuntime(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize statistics
    pImpl->totalEntries = 0;
    pImpl->fromFile = 0;
    pImpl->fromCommandLine = 0;
    pImpl->fromDefault = 0;
    pImpl->fromRuntime = 0;
    
    // Initialize default configuration groups
    pImpl->groups = {
        {"core", {"random_seed", "simulation_timestep", "neuron_count", "region_count", "connection_probability"}, "Core simulation parameters"},
        {"plasticity", {"plasticity_learning_rate", "stdp_ltp_weight", "stdp_ltd_weight", "stdp_time_constant"}, "Plasticity learning parameters"},
        {"development", {"development_synaptogenesis_rate", "development_pruning_rate"}, "Development system parameters"},
        {"neuromodulation", {"dopamine_baseline", "curiosity_baseline", "novelty_baseline"}, "Neuromodulation system parameters"},
        {"reward", {"reward_discount_factor"}, "Reward system parameters"},
        {"environment", {"environment_name", "environment_width", "environment_height"}, "Environment parameters"},
        {"logging", {"log_level", "log_to_file", "log_filename"}, "Logging system parameters"},
        {"simulation", {"max_simulation_steps", "simulation_time_limit"}, "Simulation control parameters"},
        {"visualization", {"visualization_enabled", "visualization_update_rate"}, "Visualization parameters"}
    };
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    bool isNewEntry = (it == pImpl->entries.end());
    
    if (isNewEntry) {
        pImpl->entries.emplace_back(key, value, source);
        // Update statistics
        ++pImpl->totalEntries;
        switch (source) {
            case ConfigSource::File:
                ++pImpl->fromFile;
                break;
            case ConfigSource::CommandLine:
                ++pImpl->fromCommandLine;
                break;
            case ConfigSource::Default:
                ++pImpl->fromDefault;
                break;
            case ConfigSource::Runtime:
                ++pImpl->fromRuntime;
                break;
        }
    } else {
        it->value = value;
        it->source = source;
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
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
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

// Statistics methods
    ConfigStats getStats() const {
        ConfigStats stats;
        stats.totalEntries = pImpl->totalEntries;
        stats.fromFile = pImpl->fromFile;
        stats.fromCommandLine = pImpl->fromCommandLine;
        stats.fromDefault = pImpl->fromDefault;
        stats.fromRuntime = pImpl->fromRuntime;
        return stats;
    }

    // Schema validation
    bool validateSchema(const std::vector<ConfigSchemaEntry>& schema) const {
        bool isValid = true;
        
        // Check for duplicate keys in schema
        std::unordered_set<std::string> schemaKeys;
        for (const auto& entry : schema) {
            if (schemaKeys.find(entry.key) != schemaKeys.end()) {
                NLM_LOG_WARNING("Duplicate schema key: " + entry.key);
                isValid = false;
            } else {
                schemaKeys.insert(entry.key);
            }
        }
        
        // Validate each schema entry against current config
        for (const auto& schemaEntry : schema) {
            if (schemaEntry.required && !has(schemaEntry.key)) {
                NLM_LOG_WARNING("Missing required config key: " + schemaEntry.key);
                isValid = false;
            } else if (has(schemaEntry.key)) {
                auto currentValue = getValue(schemaEntry.key);
                if (!schemaEntry.validator(currentValue)) {
                    NLM_LOG_WARNING("Validation failed for config key: " + schemaEntry.key);
                    isValid = false;
                }
            }
        }
        
        return isValid;
    }

    // Get configuration groups
    std::vector<ConfigGroup> getGroups() const {
        return pImpl->groups;
    }

    // Get configuration by source
    std::vector<ConfigEntry> getBySource(ConfigSource source) const {
        std::vector<ConfigEntry> filtered;
        filtered.reserve(pImpl->entries.size());
        
        for (const auto& entry : pImpl->entries) {
            if (entry.source == source) {
                filtered.push_back(entry);
            }
        }
        
        return filtered;
    }

    // Merge configurations
    void merge(const Config& other) {
        for (const auto& entry : other.pImpl->entries) {
            // Don't overwrite existing entries from higher priority sources
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (it == pImpl->entries.end() || it->source == ConfigSource::Runtime) {
                set(entry.key, entry.value, entry.source);
            }
        }
    }

    // Export to JSON
    bool exportToJSON(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << std::setw(2) << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            writeJSONValue(file, entry.value);
        }
        file << "\n}\n";
        
        return true;
    }

    // Import from JSON
    bool importFromJSON(const std::string& filepath) {
        // Simple JSON parser implementation
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        return parseJSON(content);
    }

    // Template helper for validation
    template<typename T>
    bool setWithValidation(const std::string& key, const T& value, const std::vector<ConfigSchemaEntry>& schema) {
        // Check if key exists in schema
        auto schemaIt = std::find_if(schema.begin(), schema.end(),
            [&key](const ConfigSchemaEntry& e) { return e.key == key; });
        
        if (schemaIt == schema.end()) {
            // Not in schema, allow setting anyway
            set(key, value);
            return true;
        }
        
        // Convert to ConfigValue for validation
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
        } else {
            return false; // Unsupported type
        }
        
        // Validate
        if (!schemaIt->validator(configValue)) {
            NLM_LOG_WARNING("Configuration validation failed for key: " + key);
            return false;
        }
        
        // Set if valid
        set(key, value);
        return true;
    }

    // Configuration equality
    bool operator==(const Config& other) const {
        if (pImpl->entries.size() != other.pImpl->entries.size()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
                return false;
            }
        }
        
        return true;
    }

    bool operator!=(const Config& other) const {
        return !(*this == other);
    }

    // Helper functions
    ConfigValue getValue(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            return it->value;
        }
        
        return ConfigValue(); // Empty variant
    }

    void writeJSONValue(std::ofstream& file, const ConfigValue& value) {
        std::visit([&file](auto&& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, int> ||
                         std::is_same_v<T, int64_t> ||
                         std::is_same_v<T, double>) {
                file << v;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (v ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(v) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << v[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < v.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeString(v[i]) << "\"";
                }
                file << "]";
            } else {
                file << "null";
            }
        }, value);
    }

    std::string escapeString(const std::string& str) {
        std::string escaped;
        escaped.reserve(str.size() * 2);
        
        for (char c : str) {
            switch (c) {
                case '\"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (c < ' ' || c > '~') {
                        escaped += "\\u";
                        escaped += "00" + std::to_string((int)c);
                    } else {
                        escaped += c;
                    }
            }
        }
        
        return escaped;
    }

    bool parseJSON(const std::string& json) {
        // Simple JSON parser for key-value pairs
        size_t pos = 0;
        size_t braceCount = 0;
        std::string key;
        ConfigValue value;
        
        while (pos < json.size()) {
            char c = json[pos];
            
            if (c == '\"') {
                // Find key name
                size_t endQuote = json.find('\"', pos + 1);
                if (endQuote == std::string::npos) {
                    return false;
                }
                key = json.substr(pos + 1, endQuote - pos - 1);
                pos = endQuote + 1;
                
                // Skip whitespace
                while (pos < json.size() && std::isspace(json[pos])) pos++;
                
                if (pos >= json.size() || json[pos] != ':') {
                    return false;
                }
                pos++;
                
                // Skip whitespace
                while (pos < json.size() && std::isspace(json[pos])) pos++;
                
                // Parse value
                if (!parseJSONValue(json, pos, value)) {
                    return false;
                }
                
                set(key, value);
                
                // Skip whitespace and comma
                while (pos < json.size() && std::isspace(json[pos])) pos++;
                if (pos < json.size() && json[pos] == ',') {
                    pos++;
                }
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
            }
            
            pos++;
        }
        
        return braceCount == 0;
    }

    bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& value) {
        if (pos >= json.size()) return false;
        
        char c = json[pos];
        
        if (c == '\"') {
            // String value
            size_t endQuote = json.find('\"', pos + 1);
            if (endQuote == std::string::npos) return false;
            
            std::string str = json.substr(pos + 1, endQuote - pos - 1);
            value = str;
            pos = endQuote + 1;
            return true;
        } else if (c == 't' || c == 'f') {
            // Boolean value
            if (json.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
                return true;
            } else if (json.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
                return true;
            }
        } else if (c == 'n') {
            // Null value
            if (json.substr(pos, 4) == "null") {
                value = std::string(); // Use string as null placeholder
                pos += 4;
                return true;
            }
        } else if (c == '-' || (c >= '0' && c <= '9')) {
            // Number value
            size_t end = pos;
            bool hasDecimal = false;
            bool hasExponent = false;
            
            while (end < json.size()) {
                char ch = json[end];
                if (ch == '.' && !hasDecimal) {
                    hasDecimal = true;
                } else if ((ch == 'e' || ch == 'E') && !hasExponent) {
                    hasExponent = true;
                } else if (!std::isdigit(ch) && ch != '-' && ch != '+' && ch != 'e' && ch != 'E') {
                    break;
                }
                end++;
            }
            
            if (end == pos) return false;
            
            std::string numStr = json.substr(pos, end - pos);
            
            try {
                if (hasDecimal || hasExponent) {
                    value = std::stod(numStr);
                } else {
                    value = std::stoll(numStr);
                }
                pos = end;
                return true;
            } catch (...) {
                return false;
            }
        } else if (c == '[') {
            // Array value
            return parseJSONArray(json, pos, value);
        }
        
        return false;
    }

    bool parseJSONArray(const std::string& json, size_t& pos, ConfigValue& value) {
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        pos++; // Skip '['
        bool first = true;
        
        while (pos < json.size() && json[pos] != ']') {
            if (!first && json[pos] == ',') {
                pos++;
                first = true;
                continue;
            }
            
            ConfigValue item;
            if (parseJSONValue(json, pos, item)) {
                // Determine array type
                if (std::holds_alternative<int>(item)) {
                    intArray.push_back(std::get<int>(item));
                } else if (std::holds_alternative<double>(item)) {
                    doubleArray.push_back(std::get<double>(item));
                } else if (std::holds_alternative<std::string>(item)) {
                    stringArray.push_back(std::get<std::string>(item));
                } else {
                    return false;
                }
                first = false;
            } else {
                return false;
            }
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
        }
        
        if (pos >= json.size() || json[pos] != ']') {
            return false;
        }
        
        pos++; // Skip ']'
        
        // Choose array type based on content
        if (!intArray.empty()) {
            value = intArray;
        } else if (!doubleArray.empty()) {
            value = doubleArray;
        } else if (!stringArray.empty()) {
            value = stringArray;
        }
        
        return true;
    }
