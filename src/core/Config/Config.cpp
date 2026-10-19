#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to trim whitespace from both ends of a string
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Simple JSON value parser
class JSONValueParser {
public:
    static ConfigValue parseJSONValue(const std::string& jsonValue) {
        std::string value = trim(jsonValue);
        
        // Handle null
        if (value == "null") {
            return ConfigValue(); // Default initialized
        }
        
        // Handle boolean
        if (value == "true") {
            return ConfigValue(true);
        }
        if (value == "false") {
            return ConfigValue(false);
        }
        
        // Handle numbers
        if (!value.empty() && (isdigit(value[0]) || value[0] == '-')) {
            size_t pos = 0;
            // Try to parse as int
            try {
                long intVal = std::stol(value, &pos);
                if (pos == value.length()) {
                    return ConfigValue(intVal);
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                double doubleVal = std::stod(value, &pos);
                if (pos == value.length()) {
                    return ConfigValue(doubleVal);
                }
            } catch (...) {}
        }
        
        // Handle string (with quotes)
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                  (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
            return ConfigValue(value);
        }
        
        // Handle arrays
        if (!value.empty() && value.front() == '[' && value.back() == ']') {
            std::vector<double> arrayValues;
            std::string inner = value.substr(1, value.size() - 2);
            size_t pos = 0;
            
            while (pos < inner.length()) {
                size_t elementEnd = inner.find(',', pos);
                if (elementEnd == std::string::npos) {
                    elementEnd = inner.length();
                }
                
                std::string element = trim(inner.substr(pos, elementEnd - pos));
                if (!element.empty()) {
                    try {
                        if (element.find('.') != std::string::npos) {
                            arrayValues.push_back(std::stod(element));
                        } else {
                            arrayValues.push_back(static_cast<double>(std::stol(element)));
                        }
                    } catch (...) {}
                }
                
                pos = elementEnd + 1;
                // Skip comma and whitespace
                while (pos < inner.length() && (inner[pos] == ',' || isspace(inner[pos]))) {
                    pos++;
                }
            }
            
            return ConfigValue(arrayValues);
        }
        
        // Handle string (unquoted)
        return ConfigValue(value);
    }
};

// Simple YAML value parser
class YAMLValueParser {
public:
    static ConfigValue parseYAMLValue(const std::string& yamlValue) {
        std::string value = trim(yamlValue);
        
        // Handle null
        if (value == "~" || value == "null") {
            return ConfigValue(); // Default initialized
        }
        
        // Handle boolean
        if (value == "true" || value == "yes" || value == "on") {
            return ConfigValue(true);
        }
        if (value == "false" || value == "no" || value == "off") {
            return ConfigValue(false);
        }
        
        // Handle numbers
        if (!value.empty() && (isdigit(value[0]) || value[0] == '-')) {
            size_t pos = 0;
            // Try to parse as int
            try {
                long intVal = std::stol(value, &pos);
                if (pos == value.length()) {
                    return ConfigValue(intVal);
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                double doubleVal = std::stod(value, &pos);
                if (pos == value.length()) {
                    return ConfigValue(doubleVal);
                }
            } catch (...) {}
        }
        
        // Handle string (with quotes)
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                  (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
            return ConfigValue(value);
        }
        
        // Handle arrays
        if (!value.empty() && value.front() == '[' && value.back() == ']') {
            std::vector<double> arrayValues;
            std::string inner = value.substr(1, value.size() - 2);
            size_t pos = 0;
            
            while (pos < inner.length()) {
                size_t elementEnd = inner.find(',', pos);
                if (elementEnd == std::string::npos) {
                    elementEnd = inner.length();
                }
                
                std::string element = trim(inner.substr(pos, elementEnd - pos));
                if (!element.empty()) {
                    try {
                        if (element.find('.') != std::string::npos) {
                            arrayValues.push_back(std::stod(element));
                        } else {
                            arrayValues.push_back(static_cast<double>(std::stol(element)));
                        }
                    } catch (...) {}
                }
                
                pos = elementEnd + 1;
                // Skip comma and whitespace
                while (pos < inner.length() && (inner[pos] == ',' || isspace(inner[pos]))) {
                    pos++;
                }
            }
            
            return ConfigValue(arrayValues);
        }
        
        // Handle string (unquoted)
        return ConfigValue(value);
    }
};

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determine file type based on extension or content
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.length() - 1) {
        extension = filepath.substr(dotPos);
    }
    
    // Try to parse as JSON first
    if (extension == ".json" || isJSON(content)) {
        return parseJSON(content);
    }
    // Try to parse as YAML
    else if (extension == ".yaml" || extension == ".yml" || isYAML(content)) {
        return parseYAML(content);
    }
    // Fallback to simple key=value format
    else {
        return parseSimpleFormat(content);
    }
}

bool Config::isJSON(const std::string& content) const {
    // Simple heuristic: check for curly braces and quotes
    size_t bracePos = content.find('{');
    size_t bracketPos = content.find('[');
    size_t quotePos = content.find('"');
    
    return (bracePos != std::string::npos) || 
           (bracketPos != std::string::npos) || 
           (quotePos != std::string::npos);
}

bool Config::isYAML(const std::string& content) const {
    // Simple heuristic: check for dashes (YAML lists), colons (YAML key-value)
    size_t dashPos = content.find('-');
    size_t colonPos = content.find(':');
    
    return (dashPos != std::string::npos) && (colonPos != std::string::npos);
}

bool Config::parseJSON(const std::string& content) {
    clear();
    
    std::string json = content;
    // Remove comments
    size_t commentPos = json.find("//");
    while (commentPos != std::string::npos) {
        size_t newLinePos = json.find('\n', commentPos);
        if (newLinePos != std::string::npos) {
            json.erase(commentPos, newLinePos - commentPos);
        } else {
            json.erase(commentPos);
            break;
        }
        commentPos = json.find("//");
    }
    
    // Parse key-value pairs
    std::string currentKey;
    std::string currentValue;
    bool inString = false;
    bool escapeNext = false;
    bool parsingKey = true;
    
    for (size_t i = 0; i < json.length(); ++i) {
        char c = json[i];
        
        if (escapeNext) {
            currentValue += c;
            escapeNext = false;
            continue;
        }
        
        if (inString) {
            if (c == '\\') {
                escapeNext = true;
            } else if (c == '"') {
                inString = false;
            } else {
                currentValue += c;
            }
        } else {
            if (c == '"') {
                inString = true;
                if (parsingKey) {
                    currentKey = currentValue;
                    currentValue.clear();
                    parsingKey = false;
                }
            } else if (c == ':') {
                parsingKey = true;
            } else if (c == ',') {
                if (!parsingKey) {
                    // Parse the current key-value pair
                    if (!currentKey.empty()) {
                        ConfigValue value = JSONValueParser::parseJSONValue(trim(currentValue));
                        set(currentKey, value, ConfigSource::File);
                    }
                }
                currentKey.clear();
                currentValue.clear();
                parsingKey = true;
            } else {
                if (parsingKey) {
                    currentKey += c;
                } else {
                    currentValue += c;
                }
            }
        }
    }
    
    // Handle last key-value pair
    if (!currentKey.empty()) {
        ConfigValue value = JSONValueParser::parseJSONValue(trim(currentValue));
        set(currentKey, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseYAML(const std::string& content) {
    clear();
    
    std::string yaml = content;
    // Remove comments
    size_t commentPos = yaml.find("#");
    while (commentPos != std::string::npos) {
        size_t newLinePos = yaml.find('\n', commentPos);
        if (newLinePos != std::string::npos) {
            yaml.erase(commentPos, newLinePos - commentPos);
        } else {
            yaml.erase(commentPos);
            break;
        }
        commentPos = yaml.find("#");
    }
    
    // Parse key-value pairs
    std::string currentKey;
    std::string currentValue;
    bool inString = false;
    bool escapeNext = false;
    
    for (size_t i = 0; i < yaml.length(); ++i) {
        char c = yaml[i];
        
        if (escapeNext) {
            currentValue += c;
            escapeNext = false;
            continue;
        }
        
        if (inString) {
            if (c == '\\') {
                escapeNext = true;
            } else if (c == '"' || c == '\'') {
                inString = false;
            } else {
                currentValue += c;
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                currentValue += c;
            } else if (c == ':') {
                // Key-value pair found
                std::string key = trim(currentKey);
                if (!key.empty()) {
                    ConfigValue value = YAMLValueParser::parseYAMLValue(trim(currentValue));
                    set(key, value, ConfigSource::File);
                }
                currentKey.clear();
                currentValue.clear();
            } else if (c == '\n') {
                // New line, check if it's a key-value pair
                if (!currentValue.empty() && currentValue.find(':') != std::string::npos) {
                    // This is a multi-line value
                    // For simplicity, just store the line
                    std::string key = trim(currentKey);
                    if (!key.empty()) {
                        set(key, ConfigValue(trim(currentValue)), ConfigSource::File);
                    }
                }
                currentKey.clear();
                currentValue.clear();
            } else {
                if (currentValue.empty() && !isspace(c)) {
                    currentKey += c;
                } else {
                    currentValue += c;
                }
            }
        }
    }
    
    return true;
}

bool Config::parseSimpleFormat(const std::string& content) {
    clear();
    
    std::string line;
    std::stringstream ss(content);
    
    while (std::getline(ss, line)) {
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
    
    // Save in JSON format
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << arg[j];
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << std::to_string(arg[j]);
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    file << "\"" << arg[j] << "\"";
                    if (j < arg.size() - 1) file << ", ";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        }
    }
    file << "}\n";
    
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
