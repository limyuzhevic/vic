#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <regex>

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
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        NLM_LOG_ERROR("Error: " + std::string(std::strerror(errno)));
        return false;
    }
    
    // Check if file is readable and has content
    file.seekg(0, std::ios::end);
    if (file.tellg() == 0) {
        NLM_LOG_WARN("Configuration file is empty: " + filepath);
        return false;
    }
    file.seekg(0, std::ios::beg);
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    if (content.empty()) {
        NLM_LOG_WARN("Configuration file is empty: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    if (isJSON(content)) {
        NLM_LOG_INFO("Parsing JSON configuration from: " + filepath);
        std::vector<ConfigEntry> entries;
        if (parseJSONObject(content, entries)) {
            // Replace current entries with parsed ones
            pImpl->entries = std::move(entries);
            NLM_LOG_INFO("Successfully parsed JSON configuration with " + 
                        std::to_string(pImpl->entries.size()) + " entries");
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse JSON configuration from: " + filepath);
            NLM_LOG_ERROR("JSON parsing errors may include: missing braces, syntax errors, or invalid values");
        }
    }
    
    // Fallback to simple key=value format
    NLM_LOG_INFO("Parsing simple key=value configuration from: " + filepath);
    std::vector<ConfigEntry> entries;
    if (parseSimpleKeyValue(content, entries)) {
        // Replace current entries with parsed ones
        pImpl->entries = std::move(entries);
        NLM_LOG_INFO("Successfully parsed simple configuration with " + 
                    std::to_string(pImpl->entries.size()) + " entries");
        return true;
    } else {
        NLM_LOG_ERROR("Failed to parse configuration from: " + filepath);
        NLM_LOG_ERROR("The file may contain malformed key=value pairs or unsupported syntax");
        return false;
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

// JSON parsing helper functions
static bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return parseJSONObject(content, entries);
}

static bool parseSimpleKeyValue(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            if (!parseJSONValue(value, configValue)) {
                NLM_LOG_WARN("Invalid value format in config file at line " + std::to_string(lineNumber) +
                           ". Key: '" + key + "', Value: '" + value + "'. Using string default.");
                configValue = value; // fallback to string
            }
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        } else {
            NLM_LOG_WARN("Invalid format in config file at line " + std::to_string(lineNumber) +
                        ". Expected 'key = value' format. Line: '" + line + "'");
        }
    }
    
    return !entries.empty();
}

static bool parseJSONValue(const std::string& valueStr, ConfigValue& configValue) {
    std::string trimmed = trim(valueStr);
    
    // Check for boolean
    if (trimmed == "true" || trimmed == "false") {
        configValue = (trimmed == "true");
        return true;
    }
    
    // Check for numbers
    size_t pos;
    try {
        size_t intPos;
        int intVal = std::stoi(trimmed, &intPos);
        if (intPos == trimmed.size()) {
            configValue = intVal;
            return true;
        }
        
        int64_t int64Val;
        size_t int64Pos;
        try {
            int64Val = std::stoll(trimmed, &int64Pos);
            if (int64Pos == trimmed.size()) {
                configValue = int64Val;
                return true;
            }
        } catch (...) {}
        
        double doubleVal = parseNumber(trimmed);
        if (doubleVal != 0.0f || std::isdigit(trimmed[0])) {
            configValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Check for array
    if (!trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']') {
        std::string arrayStr = trimmed.substr(1, trimmed.size() - 2);
        std::vector<ConfigValue> configValues;
        if (parseJSONArray(arrayStr, configValues)) {
            configValue = configValues;
            return true;
        }
    }
    
    // Default to string
    configValue = trimmed;
    return true;
}

static bool parseJSONArray(const std::string& arrayStr, std::vector<ConfigValue>& configValues) {
    configValues.clear();
    if (arrayStr.empty()) {
        return true;
    }
    
    size_t pos = 0;
    bool inString = false;
    std::string currentValue;
    
    for (size_t i = 0; i < arrayStr.size(); ++i) {
        char c = arrayStr[i];
        
        if (c == '\"' && (i == 0 || arrayStr[i-1] != '\\')) {
            inString = !inString;
            if (!inString) {
                ConfigValue value;
                if (parseJSONValue(currentValue, value)) {
                    configValues.push_back(value);
                }
                currentValue.clear();
            }
        } else if (c == ',' && !inString) {
            ConfigValue value;
            if (parseJSONValue(currentValue, value)) {
                configValues.push_back(value);
            }
            currentValue.clear();
        } else {
            currentValue += c;
        }
    }
    
    if (!inString && !currentValue.empty()) {
        ConfigValue value;
        if (parseJSONValue(currentValue, value)) {
            configValues.push_back(value);
        }
    }
    
    return true;
}

static bool parseJSONObject(const std::string& objStr, std::vector<ConfigEntry>& entries) {
    entries.clear();
    if (objStr.empty() || objStr.front() != '{' || objStr.back() != '}') {
        return false;
    }
    
    std::string innerStr = objStr.substr(1, objStr.size() - 2);
    size_t pos = 0;
    
    while (pos < innerStr.size()) {
        // Skip whitespace
        while (pos < innerStr.size() && std::isspace(innerStr[pos])) {
            pos++;
        }
        
        if (pos >= innerStr.size()) {
            break;
        }
        
        // Extract key
        size_t keyEnd = innerStr.find('"', pos);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = innerStr.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace after key
        while (pos < innerStr.size() && std::isspace(innerStr[pos])) {
            pos++;
        }
        
        // Expect colon
        if (pos >= innerStr.size() || innerStr[pos] != ':') {
            break;
        }
        pos++;
        
        // Skip whitespace after colon
        while (pos < innerStr.size() && std::isspace(innerStr[pos])) {
            pos++;
        }
        
        // Extract value
        std::string valueStr;
        if (innerStr[pos] == '"') {
            // String value
            size_t valueEnd = innerStr.find('"', pos + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            valueStr = innerStr.substr(pos + 1, valueEnd - pos - 1);
            pos = valueEnd + 1;
        } else if (innerStr[pos] == '[') {
            // Array value
            int bracketCount = 1;
            size_t valueEnd = pos + 1;
            while (valueEnd < innerStr.size() && bracketCount > 0) {
                if (innerStr[valueEnd] == '[') bracketCount++;
                else if (innerStr[valueEnd] == ']') bracketCount--;
                valueEnd++;
            }
            valueStr = innerStr.substr(pos, valueEnd - pos);
            pos = valueEnd;
        } else {
            // Number or boolean value (find next comma or closing brace)
            size_t valueEnd = innerStr.find_first_of(",}", pos);
            if (valueEnd == std::string::npos) {
                valueEnd = innerStr.size();
            }
            valueStr = innerStr.substr(pos, valueEnd - pos);
            pos = valueEnd;
        }
        
        ConfigValue configValue;
        if (parseJSONValue(trim(valueStr), configValue)) {
            entries.emplace_back(key, configValue, ConfigSource::File);
        } else {
            NLM_LOG_WARN("Failed to parse JSON value for key '" + key + "': '" + valueStr + "'");
        }
        
        // Skip whitespace and comma
        while (pos < innerStr.size() && std::isspace(innerStr[pos])) {
            pos++;
        }
        if (pos < innerStr.size() && innerStr[pos] == ',') {
            pos++;
        }
    }
    
    return !entries.empty();
}

static bool isJSON(const std::string& content) {
    if (content.empty()) {
        return false;
    }
    
    std::string trimmed = trim(content);
    return (trimmed.front() == '{' && trimmed.back() == '}') ||
           (trimmed.front() == '[' && trimmed.back() == ']');
}

static double parseNumber(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

static bool parseBool(const std::string& str) {
    return str == "true" || str == "1";
}
}
// namespace nlm
