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
        NLM_LOG_WARNING("Could not open config file: " + filepath);
        return false;
    }
    
    // Detect file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJSON(file);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYAML(file);
    } else {
        // Default to simple key=value format
        success = loadFromKeyValue(file);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath);
    } else {
        NLM_LOG_ERROR("Failed to parse configuration file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromKeyValue(std::istream& stream) {
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
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else if (!line.empty()) {
            NLM_LOG_WARNING("Ignoring malformed config line: " + line);
        }
    }
    return true;
}

bool Config::loadFromJSON(std::istream& stream) {
    // Simple JSON parser for key-value pairs
    // TODO: Replace with nlohmann::json library when available
    
    std::string line;
    std::string currentKey;
    bool inObject = false;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        // Remove trailing comma
        if (!line.empty() && line.back() == ',') {
            line.pop_back();
        }
        
        // Skip comments and empty lines
        size_t commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
            line = trim(line);
        }
        
        if (line.empty()) {
            continue;
        }
        
        if (line.front() == '{' && line.back() == '}') {
            inObject = true;
            // Parse inline object { "key": "value", ... }
            parseJSONObjectInline(line.substr(1, line.size() - 2));
        } else if (inObject) {
            if (line.front() == '}') {
                inObject = false;
            } else {
                parseJSONObjectLine(line);
            }
        } else {
            // Try to parse as simple key: value
            parseJSONObjectLine(line);
        }
    }
    
    return true;
}

bool Config::loadFromYAML(std::istream& stream) {
    // Simple YAML parser for key-value pairs
    // TODO: Replace with yaml-cpp library when available
    
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
            line = trim(line);
        }
        
        if (line.empty()) {
            continue;
        }
        
        // Parse simple key: value format
        size_t pos = line.find(':');
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
            NLM_LOG_WARNING("Ignoring malformed YAML line: " + line);
        }
    }
    return true;
}

void Config::parseJSONObjectInline(const std::string& content) {
    // Simple JSON object parsing for inline format: { "key": "value", "key2": 123 }
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        if (pos >= content.size() || content[pos] != ':') break;
        pos++;
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        
        if (pos >= content.size()) break;
        
        // Parse value
        if (content[pos] == '"') {
            // String value
            size_t valueStart = pos;
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (std::isdigit(content[pos]) || content[pos] == '-') {
            // Number value
            size_t valueEnd = pos;
            while (valueEnd < content.size() && (std::isdigit(content[valueEnd]) || content[valueEnd] == '.' || content[valueEnd] == 'e' || content[valueEnd] == 'E' || content[valueEnd] == '-')) {
                valueEnd++;
            }
            std::string valueStr = content.substr(pos, valueEnd - pos);
            try {
                if (valueStr.find('.') != std::string::npos) {
                    set(key, std::stod(valueStr), ConfigSource::File);
                } else {
                    set(key, std::stoll(valueStr), ConfigSource::File);
                }
            } catch (...) {
                NLM_LOG_WARNING("Failed to parse JSON number: " + valueStr);
            }
            pos = valueEnd;
        } else if (content[pos] == 't' || content[pos] == 'f') {
            // Boolean value
            if (content.substr(pos, 4) == "true") {
                set(key, true, ConfigSource::File);
                pos += 4;
            } else if (content.substr(pos, 5) == "false") {
                set(key, false, ConfigSource::File);
                pos += 5;
            } else {
                break;
            }
        } else {
            break;
        }
        
        // Skip whitespace and comma
        while (pos < content.size() && std::isspace(content[pos])) pos++;
        if (pos < content.size() && content[pos] == ',') pos++;
        while (pos < content.size() && std::isspace(content[pos])) pos++;
    }
}

void Config::parseJSONObjectLine(const std::string& line) {
    // Parse a single line from JSON object: "key": value
    size_t quoteStart = line.find('"');
    if (quoteStart == std::string::npos) {
        return;
    }
    
    size_t quoteEnd = line.find('"', quoteStart + 1);
    if (quoteEnd == std::string::npos) {
        return;
    }
    
    std::string key = line.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
    std::string rest = line.substr(quoteEnd + 1);
    
    // Skip whitespace
    size_t pos = 0;
    while (pos < rest.size() && std::isspace(rest[pos])) pos++;
    
    if (pos >= rest.size()) {
        return;
    }
    
    // Parse value based on type
    if (rest[pos] == '\"') {
        // String value
        size_t valueStart = pos;
        size_t valueEnd = rest.find('\"', valueStart + 1);
        if (valueEnd == std::string::npos) return;
        std::string value = rest.substr(valueStart + 1, valueEnd - valueStart - 1);
        set(key, value, ConfigSource::File);
    } else if (std::isdigit(rest[pos]) || rest[pos] == '-') {
        // Number value
        size_t valueEnd = pos;
        while (valueEnd < rest.size() && (std::isdigit(rest[valueEnd]) || rest[valueEnd] == '.' || rest[valueEnd] == 'e' || rest[valueEnd] == 'E' || rest[valueEnd] == '-')) {
            valueEnd++;
        }
        std::string valueStr = rest.substr(pos, valueEnd - pos);
        try {
            if (valueStr.find('.') != std::string::npos) {
                set(key, std::stod(valueStr), ConfigSource::File);
            } else {
                set(key, std::stoll(valueStr), ConfigSource::File);
            }
        } catch (...) {
            NLM_LOG_WARNING("Failed to parse JSON number: " + valueStr);
        }
    } else if (rest.substr(pos, 4) == "true") {
        // Boolean true
        set(key, true, ConfigSource::File);
    } else if (rest.substr(pos, 5) == "false") {
        // Boolean false
        set(key, false, ConfigSource::File);
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
        NLM_LOG_ERROR("Could not open config file for writing: " + filepath);
        return false;
    }
    
    // Write in key=value format with comments
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Format based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << arg;
            } else {
                // For other types (vectors, etc.)
                file << "[]";  // Simple placeholder
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
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
