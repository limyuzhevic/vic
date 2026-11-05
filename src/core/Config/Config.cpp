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
    // Enhanced file loading with support for multiple formats
    // 1. JSON format (preferred): {"key": "value", "number": 42, "bool": true}
    // 2. YAML format (simplified): key: value\nnumber: 42\nbool: true
    // 3. Simple key=value format (backward compatible)
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    if (content.empty()) {
        return false;
    }
    
    // Try JSON parser first
    bool jsonSuccess = parseJSON(content);
    if (jsonSuccess) {
        return true;
    }
    
    // Try YAML parser second
    bool yamlSuccess = parseYAML(content);
    if (yamlSuccess) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    file.clear();
    file.seekg(0);
    return parseKeyValueFormat(file);
}

bool Config::parseJSON(const std::string& content) {
    // Simple JSON parser for configuration files
    // Supports basic objects: {"key": "value", "num": 42, "bool": true}
    try {
        size_t pos = 0;
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            if (pos >= content.size()) break;
            
            // Parse key
            if (content[pos] == '"') {
                size_t keyEnd = content.find('"', pos + 1);
                if (keyEnd == std::string::npos) return false;
                
                std::string key = content.substr(pos + 1, keyEnd - pos - 1);
                pos = keyEnd + 1;
                
                // Skip whitespace
                while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                    pos++;
                }
                
                // Parse colon
                if (pos >= content.size() || content[pos] != ':') return false;
                pos++;
                
                // Skip whitespace
                while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                    pos++;
                }
                
                // Parse value
                std::string value;
                if (content[pos] == '"') {
                    // String value
                    size_t valueEnd = content.find('"', pos + 1);
                    if (valueEnd == std::string::npos) return false;
                    value = content.substr(pos + 1, valueEnd - pos - 1);
                    pos = valueEnd + 1;
                } else if (content[pos] == 't' || content[pos] == 'f') {
                    // Boolean value
                    size_t valueEnd = content.find_first_of(",}", pos);
                    if (valueEnd == std::string::npos) return false;
                    std::string boolStr = content.substr(pos, valueEnd - pos);
                    if (boolStr == "true") {
                        value = "true";
                    } else if (boolStr == "false") {
                        value = "false";
                    } else {
                        return false;
                    }
                    pos = valueEnd;
                } else if (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9')) {
                    // Number value
                    size_t valueEnd = content.find_first_of(",}", pos);
                    if (valueEnd == std::string::npos) return false;
                    value = content.substr(pos, valueEnd - pos);
                    pos = valueEnd;
                } else {
                    return false;
                }
                
                // Remove quotes from key and value if present
                key = trim(key);
                if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                          (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                } else {
                    value = trim(value);
                }
                
                set(key, value, ConfigSource::File);
                
                // Skip comma if present
                while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                    pos++;
                }
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                }
            } else {
                // Not a JSON object, skip to YAML parser
                return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseYAML(const std::string& content) {
    // Simple YAML parser for configuration files
    // Supports key: value format, one per line
    try {
        size_t pos = 0;
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == '\r')) {
                pos++;
            }
            
            if (pos >= content.size()) break;
            
            // Skip comments
            if (content[pos] == '#') {
                size_t newline = content.find('\n', pos);
                if (newline == std::string::npos) break;
                pos = newline + 1;
                continue;
            }
            
            // Parse key
            size_t keyEnd = content.find(':', pos);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(pos, keyEnd - pos);
            pos = keyEnd + 1;
            
            // Skip whitespace after colon
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t')) {
                pos++;
            }
            
            // Parse value
            size_t valueEnd = content.find('\n', pos);
            if (valueEnd == std::string::npos) {
                valueEnd = content.size();
            }
            
            std::string value = content.substr(pos, valueEnd - pos);
            
            // Trim whitespace from key and value
            key = trim(key);
            value = trim(value);
            
            if (!key.empty()) {
                set(key, value, ConfigSource::File);
            }
            
            pos = valueEnd + 1;
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseKeyValueFormat(std::ifstream& file) {
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
