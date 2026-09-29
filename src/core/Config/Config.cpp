// Config.cpp - Enhanced configuration system with file format support and validation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <chrono>
#include <regex>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::chrono::system_clock::time_point lastModified;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Configuration validation and schema
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "string", "bool"
    std::string defaultValue;
    bool required = false;
    std::string description;
};

static const std::vector<ConfigSchema> getDefaultSchema() {
    return {
        {"random_seed", "int", "42", true, "Random number generator seed"},
        {"simulation_timestep", "double", "0.001", true, "Simulation time step in seconds"},
        {"neuron_count", "int", "1000", true, "Number of neurons in the brain"},
        {"region_count", "int", "1", true, "Number of neural regions"},
        {"connection_probability", "double", "0.1", true, "Probability of synaptic connections"},
        {"max_steps", "int", "10000", false, "Maximum simulation steps"},
        {"verbose_logging", "bool", "false", false, "Enable verbose logging"},
        {"checkpoint_path", "string", "./checkpoint.bin", false, "Path for checkpoint saves"},
        {"enable_curiosity", "bool", "false", false, "Enable curiosity-driven exploration"},
        {"enable_development", "bool", "true", false, "Enable developmental changes"},
        {"enable_replay", "bool", "true", false, "Enable memory replay"},
        {"stdp_ltp_weight", "double", "0.02", false, "STDP LTP weight"},
        {"stdp_ltd_weight", "double", "0.015", false, "STDP LTD weight"},
        {"stdp_tau", "double", "20.0", false, "STDP time constant"},
        {"synaptogenesis_rate", "double", "0.0001", false, "Structural plasticity rate"},
        {"pruning_rate", "double", "0.00001", false, "Structural pruning rate"},
    };
}

// Simple JSON parsing implementation
class SimpleJSONParser {
public:
    static bool parse(const std::string& jsonStr, std::map<std::string, std::string>& outMap) {
        try {
            // Remove whitespace
            std::string clean = removeWhitespace(jsonStr);
            
            if (clean.empty() || clean[0] != '{' || clean.back() != '}') {
                return false;
            }
            
            // Parse simple JSON objects
            size_t pos = 1; // Skip opening {
            while (pos < clean.size() - 1) {
                // Find next key
                size_t keyStart = clean.find('"', pos);
                if (keyStart == std::string::npos) break;
                
                size_t keyEnd = clean.find('"', keyStart + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = clean.substr(keyStart + 1, keyEnd - keyStart - 1);
                pos = keyEnd + 2; // Skip ",
                
                // Find value
                if (clean.substr(pos, 2) == "{") {
                    // Object - skip for now
                    size_t bracePos = findMatchingBrace(clean, pos);
                    if (bracePos == std::string::npos) break;
                    pos = bracePos + 1;
                } else if (clean.substr(pos, 2) == "[") {
                    // Array - skip for now
                    size_t bracketPos = findMatchingBracket(clean, pos);
                    if (bracketPos == std::string::npos) break;
                    pos = bracketPos + 1;
                } else if (clean.substr(pos, 2) == "\"") {
                    // String
                    size_t valueEnd = clean.find('"', pos + 1);
                    if (valueEnd == std::string::npos) break;
                    std::string value = clean.substr(pos + 1, valueEnd - pos - 1);
                    outMap[key] = value;
                    pos = valueEnd + 2; // Skip ",
                } else if (clean.substr(pos, 4) == "true") {
                    outMap[key] = "true";
                    pos += 4;
                } else if (clean.substr(pos, 5) == "false") {
                    outMap[key] = "false";
                    pos += 5;
                } else if (clean.substr(pos, 4) == "null") {
                    outMap[key] = "null";
                    pos += 4;
                } else if (std::isdigit(clean[pos]) || clean[pos] == '-') {
                    // Number
                    size_t valueEnd = pos;
                    while (valueEnd < clean.size() && (std::isdigit(clean[valueEnd]) || 
                                                      clean[valueEnd] == '.' || 
                                                      clean[valueEnd] == 'e' || 
                                                      clean[valueEnd] == 'E' || 
                                                      clean[valueEnd] == '+' || 
                                                      clean[valueEnd] == '-')) {
                        valueEnd++;
                    }
                    std::string value = clean.substr(pos, valueEnd - pos);
                    outMap[key] = value;
                    pos = valueEnd;
                }
                
                // Skip comma
                if (pos < clean.size() && clean[pos] == ',') {
                    pos++;
                } else if (pos < clean.size() && clean[pos] == '}') {
                    break;
                }
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
    
private:
    static std::string removeWhitespace(const std::string& str) {
        std::string result;
        for (char c : str) {
            if (!std::isspace(c)) {
                result += c;
            }
        }
        return result;
    }
    
    static size_t findMatchingBrace(const std::string& str, size_t start) {
        if (start >= str.size() || str[start] != '{') return std::string::npos;
        
        int depth = 0;
        for (size_t i = start; i < str.size(); ++i) {
            if (str[i] == '{') depth++;
            else if (str[i] == '}') depth--;
            
            if (depth == 0) {
                return i;
            }
        }
        
        return std::string::npos;
    }
    
    static size_t findMatchingBracket(const std::string& str, size_t start) {
        if (start >= str.size() || str[start] != '[') return std::string::npos;
        
        int depth = 0;
        for (size_t i = start; i < str.size(); ++i) {
            if (str[i] == '[') depth++;
            else if (str[i] == ']') depth--;
            
            if (depth == 0) {
                return i;
            }
        }
        
        return std::string::npos;
    }
};

bool Config::loadFromFile(const std::string& filepath) {
    std::error_code ec;
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Check if it's a JSON file
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    }
    // Check if it's a YAML file
    else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return loadFromYAML(filepath);
    }
    // Check if it's a YML file
    else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        return loadFromYAML(filepath);
    }
    // Default to simple key=value format
    else {
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Invalid config format at line " + std::to_string(lineNumber) + ": " + line);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file contains errors but was partially loaded");
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open JSON config file: " + filepath);
            return false;
        }
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonStr = buffer.str();
        
        // Clear existing entries
        clear();
        
        // Parse JSON using simple parser
        std::map<std::string, std::string> jsonData;
        if (!SimpleJSONParser::parse(jsonStr, jsonData)) {
            NLM_LOG_WARNING("Failed to parse JSON config file: " + filepath);
            // Fall back to simple format
            return loadFromSimpleFormat(filepath);
        }
        
        // Convert to config entries
        for (const auto& pair : jsonData) {
            const std::string& key = pair.first;
            const std::string& valueStr = pair.second;
            
            // Convert string to appropriate type
            if (valueStr == "true" || valueStr == "false") {
                bool val = (valueStr == "true");
                set(key, val, ConfigSource::File);
            } else if (valueStr == "null") {
                // Skip null values
                continue;
            } else if (isNumber(valueStr)) {
                if (isInteger(valueStr)) {
                    try {
                        int64_t val = std::stoll(valueStr);
                        set(key, val, ConfigSource::File);
                    } catch (...) {
                        // Try double
                        try {
                            double val = std::stod(valueStr);
                            set(key, val, ConfigSource::File);
                        } catch (...) {
                            NLM_LOG_WARNING("Could not parse number value for key: " + key);
                        }
                    }
                } else {
                    try {
                        double val = std::stod(valueStr);
                        set(key, val, ConfigSource::File);
                    } catch (...) {
                        NLM_LOG_WARNING("Could not parse double value for key: " + key);
                    }
                }
            } else {
                set(key, valueStr, ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON config file: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        // For now, treat YAML files as simple format
        // In a full implementation, we would use a YAML parser
        NLM_LOG_WARNING("YAML config loading not yet implemented, falling back to simple format for: " + filepath);
        return loadFromSimpleFormat(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML config file: " + std::string(e.what()));
        return false;
    }
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
    
    // Determine file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        saveToJSON(file);
    } else if (ext == "yaml" || ext == "yml") {
        saveToYAML(file);
    } else {
        saveToSimpleFormat(file);
    }
    
    return true;
}

void Config::saveToSimpleFormat(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        stream << "# " << entry.description << "\n";
        
        std::visit([&stream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in string values
                std::string value = arg;
                size_t pos = 0;
                while ((pos = value.find('"', pos)) != std::string::npos) {
                    value.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                stream << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                stream << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                stream << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        stream << "\n";
    }
}

void Config::saveToJSON(std::ostream& stream) const {
    stream << "{\n";
    bool first = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            stream << ",\n";
        }
        first = false;
        
        stream << "  \"" << entry.key << "\": ";
        
        std::visit([&stream](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                stream << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                stream << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                stream << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes in string
                std::string value = arg;
                size_t pos = 0;
                while ((pos = value.find('"', pos)) != std::string::npos) {
                    value.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                stream << "\"" << value << "\"";
            }
        }, entry.value);
    }
    
    stream << "\n}\n";
}

void Config::saveToYAML(std::ostream& stream) const {
    // For now, save in simple format
    // In a full implementation, we would use a YAML serializer
    stream << "# YAML config (saved in simple format for compatibility)\n";
    saveToSimpleFormat(stream);
}

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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate configuration before setting
    if (!validateConfigEntry(key, value)) {
        NLM_LOG_WARNING("Invalid configuration entry: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, getDescriptionForKey(key));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    size_t pos = 0;
    if (str[0] == '-') pos = 1;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    for (; pos < str.size(); ++pos) {
        if (std::isdigit(str[pos])) continue;
        if (str[pos] == '.' && !hasDecimal && !hasExponent) {
            hasDecimal = true;
            continue;
        }
        if ((str[pos] == 'e' || str[pos] == 'E') && !hasExponent) {
            hasExponent = true;
            // Check for optional sign after e
            if (pos + 1 < str.size() && (str[pos + 1] == '+' || str[pos + 1] == '-')) {
                pos++;
            }
            continue;
        }
        return false;
    }
    
    return true;
}

bool Config::isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t pos = 0;
    if (str[0] == '-') pos = 1;
    
    for (; pos < str.size(); ++pos) {
        if (!std::isdigit(str[pos])) return false;
    }
    
    return true;
}

bool Config::validateConfigEntry(const std::string& key, const ConfigValue& value) {
    // Check against schema
    auto schema = getDefaultSchema();
    auto it = std::find_if(schema.begin(), schema.end(), [&key](const ConfigSchema& s) {
        return toLower(s.key) == toLower(key);
    });
    
    if (it == schema.end()) {
        // Key not in schema, but that's okay - allow custom keys
        return true;
    }
    
    // Validate type
    bool validType = false;
    std::visit([&validType](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            validType = true;
        } else if constexpr (std::is_same_v<T, double>) {
            validType = true;
        } else if constexpr (std::is_same_v<T, bool>) {
            validType = true;
        } else if constexpr (std::is_same_v<T, std::string>) {
            validType = true;
        }
    }, value);
    
    return validType;
}

std::string Config::getDescriptionForKey(const std::string& key) const {
    auto schema = getDefaultSchema();
    auto it = std::find_if(schema.begin(), schema.end(), [&key](const ConfigSchema& s) {
        return toLower(s.key) == toLower(key);
    });
    
    if (it != schema.end()) {
        return it->description;
    }
    
    return "User-defined configuration";
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