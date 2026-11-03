#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include <stack>
#include <stdexcept>

// JSON/YAML Parsing Helper Functions
namespace nlm {

// Simple JSON/YAML file format detection
static bool isJSONFile(const std::string& content) {
    // Check if content starts with { or [ (JSON object or array)
    size_t startPos = 0;
    while (startPos < content.size() && std::isspace(content[startPos])) {
        startPos++;
    }
    return startPos < content.size() && (content[startPos] == '{' || content[startPos] == '[');
}

// Simple JSON parser (minimal implementation for config files)
static bool parseJSONConfig(const std::string& content, std::map<std::string, std::string>& entries) {
    try {
        // Very simple JSON parsing - only handles flat objects
        // This is a minimal implementation for config files
        size_t pos = 0;
        bool inString = false;
        char quoteChar = '\0';
        std::string currentKey;
        std::string currentValue;
        int braceLevel = 0;
        bool inValue = false;
        
        while (pos < content.size()) {
            char c = content[pos];
            
            if (!inString) {
                if (c == '{' || c == '[') {
                    braceLevel++;
                } else if (c == '}' || c == ']') {
                    braceLevel--;
                } else if (c == '"') {
                    inString = true;
                    quoteChar = c;
                } else if (c == ':') {
                    inValue = true;
                } else if (c == ',') {
                    if (!currentKey.empty() && inValue) {
                        // Store the current entry
                        entries[currentKey] = trim(currentValue);
                        currentKey.clear();
                        currentValue.clear();
                        inValue = false;
                    }
                }
            } else if (c == quoteChar && (pos == 0 || content[pos-1] != '\\')) {
                inString = false;
                quoteChar = '\0';
            }
            
            if (!inString && !inValue && braceLevel == 1 && c != '{' && c != '}') {
                // We're in a key (before colon)
                if (c == '"') {
                    size_t keyEnd = content.find('"', pos + 1);
                    if (keyEnd != std::string::npos) {
                        currentKey = content.substr(pos + 1, keyEnd - pos - 1);
                        pos = keyEnd;
                    }
                }
            } else if (inValue) {
                currentValue += c;
            }
            
            pos++;
        }
        
        // Handle the last entry
        if (!currentKey.empty() && !currentValue.empty()) {
            entries[currentKey] = trim(currentValue);
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Simple YAML parser (minimal implementation for config files)
static bool parseYAMLConfig(const std::string& content, std::map<std::string, std::string>& entries) {
    try {
        // Very simple YAML parsing - only handles key: value format
        size_t pos = 0;
        bool inString = false;
        char quoteChar = '\0';
        std::string currentKey;
        std::string currentValue;
        bool inKey = true;
        
        while (pos < content.size()) {
            char c = content[pos];
            
            if (!inString) {
                if (c == '"') {
                    inString = true;
                    quoteChar = c;
                } else if (c == ':' && !inKey) {
                    // Key ends, value starts
                    inKey = false;
                } else if ((c == '\n' || c == '\r') && !inString && !inKey && !currentKey.empty()) {
                    // End of line, store entry
                    entries[currentKey] = trim(currentValue);
                    currentKey.clear();
                    currentValue.clear();
                    inKey = true;
                    inString = false;
                }
            } else if (c == quoteChar && (pos == 0 || content[pos-1] != '\\')) {
                inString = false;
                quoteChar = '\0';
            }
            
            if (inKey) {
                currentKey += c;
            } else {
                currentValue += c;
            }
            
            pos++;
        }
        
        // Handle the last entry
        if (!currentKey.empty()) {
            entries[currentKey] = trim(currentValue);
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace nlm

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
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    // Detect file format and parse accordingly
    bool isJSON = isJSONFile(content);
    bool isYAML = false;
    
    // Simple YAML detection (key: value format on separate lines)
    if (!isJSON) {
        size_t pos = 0;
        bool hasYAMLPattern = false;
        while (pos < content.size() - 1) {
            char c1 = content[pos];
            char c2 = content[pos + 1];
            if ((std::isalpha(c1) || c1 == '_') && c2 == ':') {
                hasYAMLPattern = true;
                break;
            }
            pos++;
        }
        isYAML = hasYAMLPattern;
    }
    
    std::map<std::string, std::string> entries;
    
    bool success = false;
    if (isJSON) {
        success = parseJSONConfig(content, entries);
    } else if (isYAML) {
        success = parseYAMLConfig(content, entries);
    } else {
        // Fall back to simple key=value format
        std::string line;
        std::istringstream stream(content);
        while (std::getline(stream, line)) {
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
                
                entries[key] = value;
            }
        }
        success = true;
    }
    
    if (!success) {
        return false;
    }
    
    // Apply parsed entries
    for (const auto& entry : entries) {
        set(entry.first, entry.second, ConfigSource::File);
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

} // namespace nlm
