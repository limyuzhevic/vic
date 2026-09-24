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

// Simple key=value format (for .cfg files)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

// Basic JSON parser for configuration files
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Simple JSON parsing for configuration files
    // This is a basic implementation that handles:
    // - Simple key-value pairs
    // - Arrays of values
    // - Nested objects (flattened with dot notation)
    
    // Remove whitespace except inside strings
    bool in_string = false;
    bool escape_next = false;
    std::string clean_json;
    
    for (size_t i = 0; i < content.size(); ++i) {
        char c = content[i];
        
        if (escape_next) {
            clean_json += c;
            escape_next = false;
            continue;
        }
        
        if (c == '\\') {
            escape_next = true;
            clean_json += c;
        } else if (c == '"') {
            in_string = !in_string;
            clean_json += c;
        } else if (!in_string && (c == '\n' || c == '\r' || c == ' ' || c == '\t')) {
            // Skip whitespace outside strings
            continue;
        } else {
            clean_json += c;
        }
    }
    
    // Parse JSON object
    size_t pos = 0;
    if (clean_json.front() != '{') {
        NLM_LOG_ERROR("JSON file must start with '{'");
        return false;
    }
    
    return parseJSONObject(clean_json, pos, "");
}

bool Config::parseJSONObject(const std::string& json, size_t& pos, const std::string& prefix) {
    bool success = true;
    
    // Skip opening brace
    pos++;
    
    while (pos < json.size() && json[pos] != '}') {
        // Parse key
        size_t key_start = json.find('"', pos);
        if (key_start == std::string::npos) {
            break; // Malformed JSON
        }
        
        size_t key_end = json.find('"', key_start + 1);
        if (key_end == std::string::npos) {
            break; // Malformed JSON
        }
        
        std::string key = json.substr(key_start + 1, key_end - key_start - 1);
        pos = key_end + 1;
        
        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != ':') {
            break; // Malformed JSON
        }
        pos++;
        
        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
            pos++;
        }
        
        // Parse value based on first character
        if (pos < json.size()) {
            bool value_ok = false;
            
            if (json[pos] == '"') {
                // String value
                size_t value_start = pos++;
                while (pos < json.size()) {
                    if (json[pos] == '\\' && pos + 1 < json.size()) {
                        // Skip escaped character
                        pos += 2;
                    } else if (json[pos] == '"') {
                        break;
                    }
                    pos++;
                }
                if (pos < json.size()) {
                    std::string value = json.substr(value_start + 1, pos - value_start - 1);
                    set(prefix.empty() ? key : prefix + "." + key, value, ConfigSource::File);
                    value_ok = true;
                }
                pos++; // Skip closing quote
            } else if (json[pos] == 't' || json[pos] == 'f') {
                // Boolean value
                size_t value_end = json.find_first_of(", }", pos);
                std::string value_str = json.substr(pos, value_end - pos);
                bool value = (value_str == "true");
                set(prefix.empty() ? key : prefix + "." + key, value, ConfigSource::File);
                value_ok = true;
                pos = value_end;
            } else if (json[pos] == 'n') {
                // Null value (treated as empty string)
                size_t value_end = json.find_first_of(", }", pos);
                set(prefix.empty() ? key : prefix + "." + key, "", ConfigSource::File);
                value_ok = true;
                pos = value_end;
            } else if (json[pos] == '[') {
                // Array value - try to parse as simple types
                pos++; // Skip opening bracket
                std::vector<ConfigValue> array_values;
                
                while (pos < json.size() && json[pos] != ']') {
                    // Skip whitespace
                    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
                        pos++;
                    }
                    
                    if (pos >= json.size()) break;
                    
                    if (json[pos] == '"') {
                        // String element
                        size_t element_start = ++pos;
                        while (pos < json.size()) {
                            if (json[pos] == '\\' && pos + 1 < json.size()) {
                                pos += 2;
                            } else if (json[pos] == '"') {
                                break;
                            }
                            pos++;
                        }
                        if (pos < json.size()) {
                            std::string element = json.substr(element_start, pos - element_start);
                            array_values.push_back(element);
                        }
                        pos++; // Skip closing quote
                    } else {
                        // Numeric or boolean element
                        size_t element_end = json.find_first_of(", ]", pos);
                        std::string element_str = json.substr(pos, element_end - pos);
                        
                        // Try to parse as double first (handles integers too)
                        char* endptr;
                        double double_val = std::strtod(element_str.c_str(), &endptr);
                        if (*endptr == '\0') {
                            array_values.push_back(double_val);
                        } else {
                            // Try boolean
                            if (element_str == "true") {
                                array_values.push_back(true);
                            } else if (element_str == "false") {
                                array_values.push_back(false);
                            } else {
                                // Try integer
                                long long_val = std::strtoll(element_str.c_str(), &endptr, 10);
                                if (*endptr == '\0') {
                                    array_values.push_back(static_cast<int64_t>(ll_val));
                                } else {
                                    array_values.push_back(element_str); // Fallback to string
                                }
                            }
                        }
                        
                        pos = element_end;
                    }
                    
                    // Skip comma or closing bracket
                    while (pos < json.size() && (json[pos] == ',' || json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
                        pos++;
                    }
                }
                
                if (pos < json.size() && json[pos] == ']') {
                    pos++; // Skip closing bracket
                    set(prefix.empty() ? key : prefix + "." + key, array_values, ConfigSource::File);
                    value_ok = true;
                }
            } else {
                // Numeric value
                size_t value_end = json.find_first_of(", }", pos);
                std::string value_str = json.substr(pos, value_end - pos);
                
                // Try to parse as double first (handles integers too)
                char* endptr;
                double double_val = std::strtod(value_str.c_str(), &endptr);
                if (*endptr == '\0') {
                    set(prefix.empty() ? key : prefix + "." + key, double_val, ConfigSource::File);
                    value_ok = true;
                } else {
                    // Try integer
                    long long_val = std::strtoll(value_str.c_str(), &endptr, 10);
                    if (*endptr == '\0') {
                        set(prefix.empty() ? key : prefix + "." + key, static_cast<int64_t>(ll_val), ConfigSource::File);
                        value_ok = true;
                    } else {
                        // Try boolean
                        if (value_str == "true") {
                            set(prefix.empty() ? key : prefix + "." + key, true, ConfigSource::File);
                            value_ok = true;
                        } else if (value_str == "false") {
                            set(prefix.empty() ? key : prefix + "." + key, false, ConfigSource::File);
                            value_ok = true;
                        }
                    }
                }
                
                pos = value_end;
            }
            
            // Skip comma or closing brace
            while (pos < json.size() && (json[pos] == ',' || json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r')) {
                pos++;
            }
            
            if (!value_ok) {
                success = false;
            }
        }
    }
    
    if (pos < json.size() && json[pos] == '}') {
        pos++; // Skip closing brace
    }
    
    return success;
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

// Validation method implementations

bool Config::validate() const {
    return validateConfiguration(*this, validationRules);
}

std::string Config::validateWithDetails() const {
    std::ostringstream oss;
    bool isValid = true;
    
    // Check required keys
    if (validationRules.requireBrainNeurons && !has("brain.neuron_count")) {
        oss << "Missing required key: brain.neuron_count\n";
        isValid = false;
    }
    
    if (validationRules.requireRegionCount && !has("brain.region_count")) {
        oss << "Missing required key: brain.region_count\n";
        isValid = false;
    }
    
    if (validationRules.requireConnectionProbability && !has("brain.connection_probability")) {
        oss << "Missing required key: brain.connection_probability\n";
        isValid = false;
    }
    
    if (validationRules.requireSimulationTimestep && !has("simulation.timestep")) {
        oss << "Missing required key: simulation.timestep\n";
        isValid = false;
    }
    
    // Validate existing keys
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            oss << "Invalid key: " << entry.key << "\n";
            isValid = false;
        }
        
        if (!validateValue(entry.value)) {
            oss << "Invalid value for key " << entry.key << "\n";
            isValid = false;
        }
        
        // Check specific validation rules
        if (entry.key == "brain.neuron_count" && entry.value.index() == typeid(int64_t)) {
            int64_t neurons = std::get<int64_t>(entry.value);
            if (neurons < validationRules.minNeuronCount || neurons > validationRules.maxNeuronCount) {
                oss << "brain.neuron_count out of range: " << neurons << " (valid: " 
                    << validationRules.minNeuronCount << " - " << validationRules.maxNeuronCount << ")\n";
                isValid = false;
            }
        }
        else if (entry.key == "brain.region_count" && entry.value.index() == typeid(int64_t)) {
            int64_t regions = std::get<int64_t>(entry.value);
            if (regions < validationRules.minRegionCount || regions > validationRules.maxRegionCount) {
                oss << "brain.region_count out of range: " << regions << " (valid: " 
                    << validationRules.minRegionCount << " - " << validationRules.maxRegionCount << ")\n";
                isValid = false;
            }
        }
        else if (entry.key == "brain.synapse_density" && entry.value.index() == typeid(double)) {
            double density = std::get<double>(entry.value);
            if (density < validationRules.minSynapseDensity || density > validationRules.maxSynapseDensity) {
                oss << "brain.synapse_density out of range: " << density << " (valid: " 
                    << validationRules.minSynapseDensity << " - " << validationRules.maxSynapseDensity << ")\n";
                isValid = false;
            }
        }
        else if (entry.key == "simulation.timestep" && entry.value.index() == typeid(double)) {
            double timestep = std::get<double>(entry.value);
            if (timestep < validationRules.minSimulationTimestep || timestep > validationRules.maxSimulationTimestep) {
                oss << "simulation.timestep out of range: " << timestep << " (valid: " 
                    << validationRules.minSimulationTimestep << " - " << validationRules.maxSimulationTimestep << ")\n";
                isValid = false;
            }
        }
    }
    
    if (isValid) {
        oss << "Configuration is valid.\n";
    }
    
    return oss.str();
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '.' && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Check for valid structure
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        std::string prefix = key.substr(0, dotPos);
        std::string suffix = key.substr(dotPos + 1);
        
        if (prefix.empty() || suffix.empty()) {
            return false;
        }
    }
    
    return true;
}

bool Config::isValidValue(const ConfigValue& value) {
    switch (value.index()) {
        case 0: // int
            return true;
        case 1: // int64_t
            return true;
        case 2: // double
            return true;
        case 3: // bool
            return true;
        case 4: // std::string
            return true;
        case 5: // std::vector<int>
            return true;
        case 6: // std::vector<double>
            return true;
        case 7: // std::vector<std::string>
            return true;
        default:
            return false;
    }
}

std::string Config::getValidationError(const std::string& key, const ConfigValue& value) {
    if (!isValidKey(key)) {
        return "Invalid key format: " + key;
    }
    
    if (!isValidValue(value)) {
        return "Invalid value type for key: " + key;
    }
    
    return "";
}
