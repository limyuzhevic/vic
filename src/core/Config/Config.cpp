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
    // Enhanced configuration file parser supporting multiple formats
    // Supports: key=value (legacy), JSON, and YAML formats
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                         std::istreambuf_iterator<char>());
    
    if (content.find('{') != std::string::npos) {
        // JSON format
        return loadFromJSON(content);
    } else if (content.find("---") != std::string::npos || 
               content.find(":") != std::string::npos && content.find(" ") != std::string::npos) {
        // Likely YAML format
        return loadFromYAML(content);
    } else {
        // Legacy key=value format
        return loadFromLegacyFormat(content);
    }
}

bool Config::loadFromJSON(const std::string& jsonContent) {
    // Enhanced JSON parser for configuration files
    // Simplified implementation supporting basic JSON types
    
    // Find opening brace
    size_t start = jsonContent.find('{');
    if (start == std::string::npos) return false;
    
    // Simple JSON parsing - extract key-value pairs
    // This is a simplified implementation for demonstration
    
    std::string::size_type pos = start + 1;
    while (pos < jsonContent.size()) {
        // Skip whitespace
        while (pos < jsonContent.size() && 
               std::isspace(jsonContent[pos])) pos++;
        
        if (pos >= jsonContent.size()) break;
        
        // Find key
        if (jsonContent[pos] != '\"') break; // Expected quoted key
        
        size_t keyEnd = jsonContent.find('\"', pos + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = jsonContent.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace and colon
        while (pos < jsonContent.size() && 
               std::isspace(jsonContent[pos])) pos++;
        
        if (pos >= jsonContent.size() || jsonContent[pos] != ':') break;
        pos++;
        
        // Skip whitespace after colon
        while (pos < jsonContent.size() && 
               std::isspace(jsonContent[pos])) pos++;
        
        // Parse value based on type
        if (pos < jsonContent.size() && jsonContent[pos] == '\"') {
            // String value
            size_t valueEnd = jsonContent.find('\"', pos + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = jsonContent.substr(pos + 1, valueEnd - pos - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (pos < jsonContent.size() && jsonContent[pos] == 't') {
            // Boolean true
            if (jsonContent.compare(pos, 4, "true") == 0) {
                set(key, true, ConfigSource::File);
                pos += 4;
            }
        } else if (pos < jsonContent.size() && jsonContent[pos] == 'f') {
            // Boolean false
            if (jsonContent.compare(pos, 5, "false") == 0) {
                set(key, false, ConfigSource::File);
                pos += 5;
            }
        } else {
            // Numeric value (float or integer)
            size_t valueEnd = jsonContent.find_first_of(",}", pos);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = jsonContent.substr(pos, valueEnd - pos);
            
            // Try to parse as integer first, then float
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double value = std::stod(valueStr);
                    set(key, value, ConfigSource::File);
                } else {
                    int64_t value = std::stoll(valueStr);
                    set(key, value, ConfigSource::File);
                }
            } catch (...) {
                // Skip unparsable values
            }
            
            pos = valueEnd;
        }
        
        // Skip whitespace and comma
        while (pos < jsonContent.size() && 
               std::isspace(jsonContent[pos])) pos++;
        
        if (pos < jsonContent.size() && jsonContent[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& yamlContent) {
    // Enhanced YAML parser for configuration files
    // Simplified implementation supporting basic YAML structure
    
    std::istringstream stream(yamlContent);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line == "---" || line.find("section:") != std::string::npos) {
            // Section header
            currentSection = "";
            continue;
        }
        
        // Parse key: value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Add section prefix if needed
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromLegacyFormat(const std::string& content) {
    // Original legacy key=value format parser
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
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}
    
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
