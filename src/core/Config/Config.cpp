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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file to detect format
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Determine file format
    bool isJSON = false;
    bool isYAML = false;
    
    // Check for JSON format (starts with { or [)
    if (!content.empty() && (content.front() == '{' || content.front() == '[')) {
        isJSON = true;
    }
    
    // Check for YAML format (has key: value patterns)
    size_t colonPos = content.find(':');
    if (colonPos != std::string::npos && colonPos > 0 && 
        content.find(':', colonPos + 1) != std::string::npos && 
        content.find('\n') != std::string::npos) {
        isYAML = true;
    }
    
    if (isJSON) {
        return loadFromJSON(content, ConfigSource::File);
    } else if (isYAML) {
        return loadFromYAML(content, ConfigSource::File);
    } else {
        // Fallback to simple key=value format
        std::istringstream stream(content);
        std::string line;
        size_t lineNum = 0;
        bool validationErrors = false;
        
        while (std::getline(stream, line)) {
            lineNum++;
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
                
                // Validate key format (should be alphanumeric with underscores)
                if (!key.empty() && !std::isalnum(key[0]) && key[0] != '_') {
                    NLM_LOG_WARNING("Config: Invalid key format at line " + std::to_string(lineNum) + ": " + key);
                    validationErrors = true;
                }
                
                // Validate key for known configuration parameters
                if (key == "random_seed" && !validateInt64(value)) {
                    NLM_LOG_ERROR("Config: Invalid random_seed value at line " + std::to_string(lineNum) + ": " + value);
                    validationErrors = true;
                } else if (key == "neuron_count" && !validatePositiveInt(value)) {
                    NLM_LOG_ERROR("Config: Invalid neuron_count value at line " + std::to_string(lineNum) + ": " + value);
                    validationErrors = true;
                } else if (key == "simulation_timestep" && !validatePositiveDouble(value)) {
                    NLM_LOG_ERROR("Config: Invalid simulation_timestep value at line " + std::to_string(lineNum) + ": " + value);
                    validationErrors = true;
                } else if (key == "connection_probability" && !validateFloatRange(value, 0.0f, 1.0f)) {
                    NLM_LOG_ERROR("Config: Invalid connection_probability value at line " + std::to_string(lineNum) + ": " + value);
                    validationErrors = true;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Config: Invalid line format at line " + std::to_string(lineNum) + ": " + line);
                validationErrors = true;
            }
        }
        
        return !validationErrors;
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

bool Config::loadFromJSON(const std::string& content, ConfigSource source) {
    // Simple JSON parser for common formats
    // This is a simplified implementation that handles basic JSON structures
    try {
        // Check if it's a JSON object (starts with {)
        if (content.front() != '{') {
            return false;
        }
        
        // Parse key-value pairs
        size_t pos = 1;  // Skip opening brace
        while (pos < content.size() && content[pos] != '}') {
            // Find next key (skip whitespace and "")
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            if (pos >= content.size() || content[pos] != '"') {
                break;
            }
            
            // Find key end
            size_t keyStart = pos + 1;
            pos = content.find('"', keyStart);
            if (pos == std::string::npos) {
                break;
            }
            
            std::string key = content.substr(keyStart, pos - keyStart);
            
            // Skip whitespace and colon
            pos++;
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            if (pos >= content.size() || content[pos] != ':') {
                break;
            }
            
            // Find value
            pos++;
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            // Parse value based on first character
            if (pos < content.size()) {
                if (content[pos] == '"') {
                    // String value
                    size_t valueEnd = content.find('"', pos + 1);
                    if (valueEnd != std::string::npos) {
                        std::string value = content.substr(pos + 1, valueEnd - pos - 1);
                        set(key, value, source);
                        pos = valueEnd + 1;
                    }
                } else if (content[pos] == 't' || content[pos] == 'f') {
                    // Boolean value
                    if (content.substr(pos, 5) == "true") {
                        set(key, true, source);
                        pos += 4;
                    } else if (content.substr(pos, 4) == "false") {
                        set(key, false, source);
                        pos += 5;
                    }
                } else if (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9')) {
                    // Number value
                    size_t valueEnd = content.find_first_of(",}", pos);
                    if (valueEnd != std::string::npos) {
                        std::string valueStr = content.substr(pos, valueEnd - pos);
                        
                        // Try to parse as int64
                        try {
                            size_t endPos;
                            int64_t intVal = std::stoll(valueStr, &endPos);
                            if (endPos == valueStr.size()) {
                                set(key, intVal, source);
                                pos = valueEnd;
                                continue;
                            }
                        } catch (...) {}
                        
                        // Try to parse as double
                        try {
                            size_t endPos;
                            double doubleVal = std::stod(valueStr, &endPos);
                            if (endPos == valueStr.size()) {
                                set(key, doubleVal, source);
                                pos = valueEnd;
                                continue;
                            }
                        } catch (...) {}
                        
                        // If both fail, treat as string
                        set(key, valueStr, source);
                        pos = valueEnd;
                    }
                } else if (content[pos] == '[') {
                    // Array value (simplified)
                    size_t arrayEnd = content.find(']', pos);
                    if (arrayEnd != std::string::npos) {
                        // For now, store as string representation
                        std::string arrayStr = content.substr(pos, arrayEnd - pos + 1);
                        set(key, arrayStr, source);
                        pos = arrayEnd + 1;
                    }
                }
            }
            
            // Skip to next key or closing brace
            while (pos < content.size() && content[pos] != ',' && content[pos] != '}') {
                pos++;
            }
            
            if (pos < content.size() && content[pos] == ',') {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& content, ConfigSource source) {
    // Simple YAML parser for basic key: value format
    try {
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            
            // Skip empty lines, comments, and YAML document markers
            if (line.empty() || line[0] == '#' || line == "---" || line == "...") {
                continue;
            }
            
            // Parse key: value pairs (YAML basic format)
            size_t pos = line.find(':');
            if (pos == std::string::npos) {
                continue;  // Not a key-value pair
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes from value
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value as different types
            bool parsed = false;
            
            // Try boolean
            if (value == "true") {
                set(key, true, source);
                parsed = true;
            } else if (value == "false") {
                set(key, false, source);
                parsed = true;
            }
            
            // Try integer
            if (!parsed) {
                try {
                    size_t endPos;
                    int64_t intVal = std::stoll(value, &endPos);
                    if (endPos == value.size()) {
                        set(key, intVal, source);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!parsed) {
                try {
                    size_t endPos;
                    double doubleVal = std::stod(value, &endPos);
                    if (endPos == value.size()) {
                        set(key, doubleVal, source);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // If not parsed as numeric, treat as string
            if (!parsed) {
                set(key, value, source);
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Helper validation functions
bool Config::validateInt64(const std::string& str) {
    try {
        size_t endPos;
        int64_t val = std::stoll(str, &endPos);
        return endPos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::validatePositiveInt(const std::string& str) {
    try {
        size_t endPos;
        int64_t val = std::stoll(str, &endPos);
        return endPos == str.size() && val > 0;
    } catch (...) {
        return false;
    }
}

bool Config::validatePositiveDouble(const std::string& str) {
    try {
        size_t endPos;
        double val = std::stod(str, &endPos);
        return endPos == str.size() && val > 0.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateFloatRange(const std::string& str, float min, float max) {
    try {
        size_t endPos;
        double val = std::stod(str, &endPos);
        if (endPos != str.size()) return false;
        return val >= min && val <= max;
    } catch (...) {
        return false;
    }
}

} // namespace nlm