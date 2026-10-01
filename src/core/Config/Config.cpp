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
    // Enhanced file loading with automatic format detection
    // Supports both simple key=value format (backward compatibility) and JSON/YAML formats
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file for format detection
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    if (content.empty()) {
        return false;
    }
    
    // Try JSON first, then YAML, then fallback to key=value format
    if (tryLoadJSON(content)) {
        return true;
    } else if (tryLoadYAML(content)) {
        return true;
    } else {
        return loadKeyValueFormat(content);
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

bool Config::tryLoadJSON(const std::string& content) {
    // Try to parse as JSON
    // Simple JSON parser - in production, would use a proper JSON library
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next object or array start
        size_t objStart = content.find('{', pos);
        size_t arrStart = content.find('[', pos);
        
        if (objStart == std::string::npos && arrStart == std::string::npos) {
            // No JSON structures found, fallback
            return false;
        }
        
        // Use the earlier of objStart and arrStart
        size_t structureStart = objStart == std::string::npos ? arrStart : 
                               (arrStart == std::string::npos ? objStart : std::min(objStart, arrStart));
        
        if (parseJSONObject(content, structureStart)) {
            return true;
        }
        
        pos = structureStart + 1;
    }
    return false;
}

bool Config::tryLoadYAML(const std::string& content) {
    // Try to parse as YAML
    // Simple YAML parser - in production, would use a proper YAML library
    
    std::istringstream stream(content);
    std::string line;
    size_t indentLevel = 0;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        
        size_t end = line.find_last_not_of(" \t\r\n");
        line = line.substr(start, end - start + 1);
        
        // Skip comments
        if (line.empty() || line[0] == '#') continue;
        
        // Calculate indentation
        size_t currentIndent = line.find_first_not_of(" \t");
        
        // Check if line looks like a key-value pair
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos < line.size() - 1) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Trim key and value
            key = trim(key);
            value = trim(value);
            
            // Remove quotes if present
            if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                     (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadKeyValueFormat(const std::string& content) {
    // Original simple key=value format for backward compatibility
    
    std::istringstream stream(content);
    std::string line;
    
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
            if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                     (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::parseJSONObject(const std::string& content, size_t startPos) {
    // Simple JSON object parser - very basic implementation
    size_t endPos = content.find('}', startPos);
    if (endPos == std::string::npos) {
        return false;
    }
    
    std::string objContent = content.substr(startPos + 1, endPos - startPos - 1);
    parseSimpleKeyValuePairs(objContent);
    return true;
}

void Config::parseSimpleKeyValuePairs(const std::string& content) {
    // Helper to parse simple key=value pairs from JSON-like content
    size_t pos = 0;
    while (pos < content.size()) {
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = content.find_first_not_of(' \t', colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Handle different value types
        if (content[valueStart] == '"') {
            // String value
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (content[valueStart] == '{' || content[valueStart] == '[') {
            // Nested object/array - skip for now
            size_t bracketEnd = content.find(content[valueStart], valueStart + 1);
            if (bracketEnd == std::string::npos) break;
            pos = bracketEnd + 1;
        } else {
            // Number or boolean
            size_t valueEnd = content.find_first_of(", }", valueStart);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valueStart, valueEnd - valueStart);
            set(key, value, ConfigSource::File);
            pos = valueEnd;
        }
    }
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
