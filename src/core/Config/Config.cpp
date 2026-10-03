#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <regex>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_DEBUG("Loading configuration from: " + filepath);
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    // Determine file format based on extension
    std::string extension = "." + std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Parse based on file format
    if (extension == ".json") {
        return parseJson(content);
    } else if (extension == ".yaml" || extension == ".yml") {
        return parseYaml(content);
    } else {
        // Default to key=value format for backward compatibility
        return parseKeyValue(content);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_DEBUG("Loading configuration from command line arguments");
    
    bool success = true;
    
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
    
    return success;
}

bool Config::parseJson(const std::string& json) {
    try {
        // Simple JSON parser (minimal implementation)
        if (json.empty() || json[0] != '{' || json.back() != '}') {
            NLM_LOG_ERROR("Invalid JSON format: not an object");
            return false;
        }
        
        std::string content = json.substr(1, json.size() - 2);
        parseJsonObject(content);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse JSON: ") + e.what());
        return false;
    }
}

bool Config::parseYaml(const std::string& yaml) {
    try {
        // Simple YAML parser (minimal implementation)
        std::istringstream stream(yaml);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line.substr(0, 2) == "# ") {
                continue;
            }
            
            // Parse key: value format
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, colonPos));
            std::string valueStr = trim(line.substr(colonPos + 1));
            
            // Parse value
            ConfigValue value;
            if (!parseYamlValue(valueStr, value)) {
                NLM_LOG_WARNING(std::string("Failed to parse YAML value for key: ") + key);
                continue;
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse YAML: ") + e.what());
        return false;
    }
}

bool Config::parseKeyValue(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
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
            NLM_LOG_WARNING(std::string("Skipping invalid configuration line: ") + 
                          std::to_string(lineNumber) + " - " + line);
        }
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
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
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
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    
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
        oss << "] (" << static_cast<int>(entry.source) << ")" << std::endl;
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

// Parse JSON object
void Config::parseJsonObject(const std::string& content) {
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        key = trim(key);
        
        // Find colon
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value
        size_t valuePos = content.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valuePos == std::string::npos) break;
        
        ConfigValue value;
        if (parseJsonValue(content.substr(valuePos), value)) {
            set(key, value, ConfigSource::File);
        }
        
        // Find next comma or closing brace
        pos = content.find(',', valuePos);
        if (pos == std::string::npos) {
            break;
        }
        pos = content.find_first_not_of(" \t\n\r", pos + 1);
    }
}

bool Config::parseJsonValue(const std::string& valueStr, ConfigValue& value) {
    std::string trimmed = trim(valueStr);
    
    if (trimmed.empty()) {
        value = std::string();
        return true;
    }
    
    if (trimmed[0] == '"' || trimmed[0] == '\'') {
        // String value
        value = trimmed.substr(1, trimmed.size() - 2);
        return true;
    } else if (trimmed == "true" || trimmed == "false") {
        // Boolean value
        value = (trimmed == "true");
        return true;
    } else if (std::isdigit(trimmed[0]) || (trimmed[0] == '-' && trimmed.size() > 1 && std::isdigit(trimmed[1]))) {
        // Number value
        try {
            if (trimmed.find('.') != std::string::npos || trimmed.find('e') != std::string::npos || 
                trimmed.find('E') != std::string::npos) {
                // Float value
                float floatVal = std::stof(trimmed);
                value = floatVal;
            } else {
                // Integer value
                int intVal = std::stoi(trimmed);
                value = intVal;
            }
            return true;
        } catch (const std::exception&) {
            value = trimmed;
            return false;
        }
    } else if (trimmed == "null" || trimmed == "nil") {
        value = std::string(); // Empty string as null representation
        return true;
    } else if (trimmed.size() >= 2 && trimmed[0] == '[' && trimmed.back() == ']') {
        // Array value
        return parseJsonArray(trimmed.substr(1, trimmed.size() - 2), value);
    } else {
        value = trimmed;
        return false;
    }
}

bool Config::parseJsonArray(const std::string& arrayStr, ConfigValue& value) {
    std::vector<std::string> elements;
    size_t pos = 0;
    
    while (pos < arrayStr.size()) {
        size_t elementStart = arrayStr.find_first_not_of(" \t\n\r", pos);
        if (elementStart == std::string::npos) break;
        
        size_t elementEnd;
        if (arrayStr[elementStart] == '"' || arrayStr[elementStart] == '\'') {
            // String element
            elementEnd = arrayStr.find(arrayStr[elementStart], elementStart + 1);
            if (elementEnd == std::string::npos) break;
        } else if (arrayStr[elementStart] == '[') {
            // Nested array - find matching closing bracket
            int bracketDepth = 1;
            elementEnd = elementStart + 1;
            while (elementEnd < arrayStr.size() && bracketDepth > 0) {
                if (arrayStr[elementEnd] == '[') bracketDepth++;
                else if (arrayStr[elementEnd] == ']') bracketDepth--;
                elementEnd++;
            }
        } else {
            // Simple element (number, boolean, null)
            elementEnd = arrayStr.find_first_of(",]\", elementStart);
            if (elementEnd == std::string::npos) elementEnd = arrayStr.size();
        }
        
        std::string element = trim(arrayStr.substr(elementStart, elementEnd - elementStart));
        if (!element.empty()) {
            elements.push_back(element);
        }
        
        pos = arrayStr.find(',', elementEnd);
        if (pos == std::string::npos) {
            break;
        }
        pos = arrayStr.find_first_not_of(" \t\n\r", pos + 1);
    }
    
    // Convert elements to appropriate types
    std::vector<ConfigValue> configElements;
    for (const auto& element : elements) {
        ConfigValue elementValue;
        parseJsonValue(element, elementValue);
        configElements.push_back(std::move(elementValue));
    }
    
    value = configElements;
    return true;
}

bool Config::parseYamlValue(const std::string& valueStr, ConfigValue& value) {
    std::string trimmed = trim(valueStr);
    
    if (trimmed.empty()) {
        value = std::string();
        return true;
    }
    
    if (trimmed[0] == '"' || trimmed[0] == '\'') {
        // String value with quotes
        value = trimmed.substr(1, trimmed.size() - 2);
        return true;
    } else if (trimmed == "true" || trimmed == "false") {
        // Boolean value
        value = (trimmed == "true");
        return true;
    } else if (std::isdigit(trimmed[0]) || (trimmed[0] == '-' && trimmed.size() > 1 && std::isdigit(trimmed[1]))) {
        // Number value
        try {
            if (trimmed.find('.') != std::string::npos || trimmed.find('e') != std::string::npos || 
                trimmed.find('E') != std::string::npos) {
                // Float value
                float floatVal = std::stof(trimmed);
                value = floatVal;
            } else {
                // Integer value
                int intVal = std::stoi(trimmed);
                value = intVal;
            }
            return true;
        } catch (const std::exception&) {
            value = trimmed;
            return false;
        }
    } else if (trimmed == "null" || trimmed == "nil") {
        value = std::string(); // Empty string as null representation
        return true;
    } else if (trimmed.find('[') == 0 && trimmed.back() == ']') {
        // Array value
        return parseYamlArray(trimmed.substr(1, trimmed.size() - 2), value);
    } else {
        value = trimmed;
        return false;
    }
}

bool Config::parseYamlArray(const std::string& arrayStr, ConfigValue& value) {
    std::vector<std::string> elements;
    size_t pos = 0;
    
    while (pos < arrayStr.size()) {
        size_t elementStart = arrayStr.find_first_not_of(" \t\n\r", pos);
        if (elementStart == std::string::npos) break;
        
        size_t elementEnd;
        if (arrayStr[elementStart] == '"' || arrayStr[elementStart] == '\'') {
            // String element
            elementEnd = arrayStr.find(arrayStr[elementStart], elementStart + 1);
            if (elementEnd == std::string::npos) break;
        } else if (arrayStr[elementStart] == '[') {
            // Nested array - find matching closing bracket
            int bracketDepth = 1;
            elementEnd = elementStart + 1;
            while (elementEnd < arrayStr.size() && bracketDepth > 0) {
                if (arrayStr[elementEnd] == '[') bracketDepth++;
                else if (arrayStr[elementEnd] == ']') bracketDepth--;
                elementEnd++;
            }
        } else {
            // Simple element (number, boolean, null)
            elementEnd = arrayStr.find_first_of(",]\", elementStart);
            if (elementEnd == std::string::npos) elementEnd = arrayStr.size();
        }
        
        std::string element = trim(arrayStr.substr(elementStart, elementEnd - elementStart));
        if (!element.empty()) {
            elements.push_back(element);
        }
        
        pos = arrayStr.find(',', elementEnd);
        if (pos == std::string::npos) {
            break;
        }
        pos = arrayStr.find_first_not_of(" \t\n\r", pos + 1);
    }
    
    // Convert elements to appropriate types
    std::vector<ConfigValue> configElements;
    for (const auto& element : elements) {
        ConfigValue elementValue;
        parseYamlValue(element, elementValue);
        configElements.push_back(std::move(elementValue));
    }
    
    value = configElements;
    return true;
}

} // namespace nlm
