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
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    // Try to determine file format by extension
    std::string extension;
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    // Try JSON first
    if (extension == "json" || extension.empty()) {
        if (loadFromJSON(file, filepath)) {
            return true;
        }
        NLM_LOG_WARN("Failed to parse as JSON, trying YAML...");
    }
    
    // Try YAML
    if (extension == "yaml" || extension == "yml") {
        if (loadFromYAML(file, filepath)) {
            return true;
        }
        NLM_LOG_WARN("Failed to parse as YAML, trying key=value format...");
    }
    
    // Fallback to simple key=value format
    file.clear();
    file.seekg(0, std::ios::beg);
    NLM_LOG_WARN("Using key=value format for config file: " + filepath);
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARN("Invalid line in config file " + filepath + ":" + std::to_string(lineNum) + 
                        " - expected 'key=value' format: '" + line + "'");
        }
    }
    
    return true;
}

bool Config::loadFromJSON(std::ifstream& file, const std::string& filepath) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Simple JSON parsing - in a real implementation we'd use nlohmann/json
        // This is a minimal implementation for basic JSON support
        bool validJSON = true;
        
        // Remove whitespace
        content.erase(std::remove_if(content.begin(), content.end(), 
            [](unsigned char c) { return std::isspace(c); }), content.end());
        
        // Check if it's an object { ... }
        if (content.empty() || content.front() != '{' || content.back() != '}') {
            return false;
        }
        
        // Extract key-value pairs (very basic parsing)
        std::string inner = content.substr(1, content.size() - 2);
        size_t pos = 0;
        while (pos < inner.size()) {
            // Find next quote
            size_t keyStart = inner.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = inner.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = inner.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < inner.size() && (inner[pos] == ' ' || inner[pos] == '\t')) pos++;
            
            if (pos >= inner.size() || inner[pos] != ':') break;
            pos++;
            
            // Skip whitespace
            while (pos < inner.size() && (inner[pos] == ' ' || inner[pos] == '\t')) pos++;
            
            // Extract value (simplified - handles strings, numbers, booleans, null)
            if (pos < inner.size()) {
                size_t valueEnd = inner.find_first_of(",}", pos);
                if (valueEnd == std::string::npos) valueEnd = inner.size();
                
                std::string valueStr = inner.substr(pos, valueEnd - pos);
                
                // Remove surrounding quotes for strings
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '\"' && valueStr.back() == '\"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                // Try to determine type and set value
                try {
                    // Try boolean
                    if (valueStr == "true" || valueStr == "false") {
                        set(key, valueStr == "true", ConfigSource::File);
                    }
                    // Try null/None
                    else if (valueStr == "null" || valueStr == "None") {
                        set(key, std::string("null"), ConfigSource::File);
                    }
                    // Try integer
                    else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) ||
                            (valueStr.front() == '-' && std::all_of(valueStr.begin() + 1, valueStr.end(), ::isdigit))) {
                        int64_t intVal = std::stoll(valueStr);
                        set(key, intVal, ConfigSource::File);
                    }
                    // Try floating point
                    else if (isFloatString(valueStr)) {
                        float floatVal = std::stof(valueStr);
                        set(key, floatVal, ConfigSource::File);
                    }
                    // Default to string
                    else {
                        set(key, valueStr, ConfigSource::File);
                    }
                } catch (...) {
                    // If parsing fails, store as string
                    set(key, valueStr, ConfigSource::File);
                }
            }
            
            // Move to next pair
            if (pos < inner.size() && inner[pos] == ',') {
                pos++;
                while (pos < inner.size() && (inner[pos] == ' ' || inner[pos] == '\t')) pos++;
            } else {
                break;
            }
        }
        
        NLM_LOG_INFO("Successfully loaded config from JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(std::ifstream& file, const std::string& filepath) {
    try {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // For now, implement a very basic YAML parser that handles simple key-value pairs
        // In a real implementation, we'd use a proper YAML library like yaml-cpp
        
        // Remove comments
        std::string line;
        std::stringstream yamlContent;
        std::istringstream lineStream(content);
        
        while (std::getline(lineStream, line)) {
            // Remove inline comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Remove trailing whitespace
            line = trim(line);
            
            if (!line.empty()) {
                yamlContent << line << "\n";
            }
        }
        
        content = yamlContent.str();
        
        // Handle simple YAML format: key: value
        std::istringstream yamlStream(content);
        std::string currentKey;
        
        while (std::getline(yamlStream, line)) {
            line = trim(line);
            if (line.empty()) continue;
            
            // Check if line is a key-value pair (contains ':')
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                currentKey = trim(line.substr(0, colonPos));
                std::string valueStr = trim(line.substr(colonPos + 1));
                
                // Remove quotes from value
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '\"' && valueStr.back() == '\"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                // Try to determine type and set value
                try {
                    if (valueStr == "true" || valueStr == "false") {
                        set(currentKey, valueStr == "true", ConfigSource::File);
                    } else if (valueStr == "null" || valueStr == "None") {
                        set(currentKey, std::string("null"), ConfigSource::File);
                    } else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) ||
                              (valueStr.front() == '-' && std::all_of(valueStr.begin() + 1, valueStr.end(), ::isdigit))) {
                        int64_t intVal = std::stoll(valueStr);
                        set(currentKey, intVal, ConfigSource::File);
                    } else if (isFloatString(valueStr)) {
                        float floatVal = std::stof(valueStr);
                        set(currentKey, floatVal, ConfigSource::File);
                    } else {
                        set(currentKey, valueStr, ConfigSource::File);
                    }
                } catch (...) {
                    set(currentKey, valueStr, ConfigSource::File);
                }
            }
        }
        
        NLM_LOG_INFO("Successfully loaded config from YAML file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::isFloatString(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    
    bool hasDecimal = false;
    bool hasExponent = false;
    bool digitFound = false;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
        } else if (str[i] == 'e' || str[i] == 'E') {
            if (hasExponent) return false;
            hasExponent = true;
            digitFound = false; // Need at least one digit after exponent
        } else if (str[i] == '+' || str[i] == '-') {
            // Allow exponent sign
            if (i == 0 || (i > 0 && (str[i-1] == 'e' || str[i-1] == 'E'))) {
                // Valid exponent sign
            } else {
                return false;
            }
        } else if (std::isdigit(static_cast<unsigned char>(str[i]))) {
            digitFound = true;
        } else {
            return false;
        }
    }
    
    return digitFound;
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

bool Config::isFloatString(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '+' || str[0] == '-') start = 1;
    
    bool hasDecimal = false;
    bool hasExponent = false;
    bool digitFound = false;
    
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
        } else if (str[i] == 'e' || str[i] == 'E') {
            if (hasExponent) return false;
            hasExponent = true;
            digitFound = false; // Need at least one digit after exponent
        } else if (str[i] == '+' || str[i] == '-') {
            // Allow exponent sign
            if (i == 0 || (i > 0 && (str[i-1] == 'e' || str[i-1] == 'E'))) {
                // Valid exponent sign
            } else {
                return false;
            }
        } else if (std::isdigit(static_cast<unsigned char>(str[i]))) {
            digitFound = true;
        } else {
            return false;
        }
    }
    
    return digitFound;
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

} // namespace nlm
