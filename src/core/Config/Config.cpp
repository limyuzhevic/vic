#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cctype>

namespace nlm {

// Forward declarations for internal parsing
static bool parseJsonValue(const std::string& content, JsonValue& value, size_t& pos);
static bool parseJsonObject(const std::string& content, std::map<std::string, JsonValue>& obj, size_t& pos);
static bool parseJsonArray(const std::string& content, std::vector<JsonValue>& arr, size_t& pos);
static bool skipWhitespace(const std::string& content, size_t& pos);
static bool parseString(const std::string& content, std::string& result, size_t& pos);
static bool parseNumber(const std::string& content, double& result, size_t& pos);
static bool parseBool(const std::string& content, bool& result, size_t& pos);
static bool parseNull(const std::string& content, size_t& pos);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, JsonValue> jsonData;
    bool jsonParsed;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonParsed = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON/YAML first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Check if it looks like JSON (starts with { or [)
    if (content.empty()) {
        NLM_LOG_ERROR("Configuration file is empty: " + filepath);
        return false;
    }
    
    char firstChar = content[0];
    if (firstChar == '{' || firstChar == '[') {
        // Try JSON parsing
        JsonValue parsed;
        size_t pos = 0;
        if (parseJsonValue(content, parsed, pos)) {
            pImpl->jsonData = convertToConfigMap(parsed);
            pImpl->jsonParsed = true;
            NLM_LOG_INFO("Configuration loaded as JSON from: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse JSON configuration file: " + filepath);
            return false;
        }
    }
    
    // Fall back to simple key=value parsing
    NLM_LOG_INFO("Parsing configuration file as key=value format: " + filepath);
    std::string line;
    size_t lineNum = 0;
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
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key found in configuration file (line " + std::to_string(lineNum) + ")");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid line format in configuration file (line " + std::to_string(lineNum) + ")");
        }
    }
    
    if (pImpl->entries.empty()) {
        NLM_LOG_WARNING("No valid configuration entries found in: " + filepath);
    }
    
    return true;
}

// Helper function to convert JsonValue to Config map
std::map<std::string, JsonValue> Config::convertToConfigMap(const JsonValue& value) {
    std::map<std::string, JsonValue> result;
    if (std::holds_alternative<std::map<std::string, JsonValue>>(value)) {
        result = std::get<std::map<std::string, JsonValue>>(value);
    }
    return result;
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

namespace nlm {

// Forward declarations for internal parsing
static bool parseJsonValue(const std::string& content, JsonValue& value, size_t& pos);
static bool parseJsonObject(const std::string& content, std::map<std::string, JsonValue>& obj, size_t& pos);
static bool parseJsonArray(const std::string& content, std::vector<JsonValue>& arr, size_t& pos);
static bool skipWhitespace(const std::string& content, size_t& pos);
static bool parseString(const std::string& content, std::string& result, size_t& pos);
static bool parseNumber(const std::string& content, double& result, size_t& pos);
static bool parseBool(const std::string& content, bool& result, size_t& pos);
static bool parseNull(const std::string& content, size_t& pos);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, JsonValue> jsonData;
    bool jsonParsed;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonParsed = false;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON/YAML first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Check if it looks like JSON (starts with { or [)
    if (content.empty()) {
        NLM_LOG_ERROR("Configuration file is empty: " + filepath);
        return false;
    }
    
    char firstChar = content[0];
    if (firstChar == '{' || firstChar == '[') {
        // Try JSON parsing
        JsonValue parsed;
        size_t pos = 0;
        if (parseJsonValue(content, parsed, pos)) {
            pImpl->jsonData = convertToConfigMap(parsed);
            pImpl->jsonParsed = true;
            NLM_LOG_INFO("Configuration loaded as JSON from: " + filepath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to parse JSON configuration file: " + filepath);
            return false;
        }
    }
    
    // Fall back to simple key=value parsing
    NLM_LOG_INFO("Parsing configuration file as key=value format: " + filepath);
    std::string line;
    size_t lineNum = 0;
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
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key found in configuration file (line " + std::to_string(lineNum) + ")");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid line format in configuration file (line " + std::to_string(lineNum) + ")");
        }
    }
    
    if (pImpl->entries.empty()) {
        NLM_LOG_WARNING("No valid configuration entries found in: " + filepath);
    }
    
    return true;
}

// Helper function to convert JsonValue to Config map
std::map<std::string, JsonValue> Config::convertToConfigMap(const JsonValue& value) {
    std::map<std::string, JsonValue> result;
    if (std::holds_alternative<std::map<std::string, JsonValue>>(value)) {
        result = std::get<std::map<std::string, JsonValue>>(value);
    }
    return result;
}

// JSON parsing helper functions
static bool skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
    return pos < content.size();
}

static bool parseString(const std::string& content, std::string& result, size_t& pos) {
    if (pos >= content.size() || content[pos] != '"') {
        return false;
    }
    
    pos++; // Skip opening quote
    size_t start = pos;
    
    while (pos < content.size()) {
        if (content[pos] == '\\') {
            pos += 2; // Skip escape sequence
            continue;
        } else if (content[pos] == '"') {
            break;
        }
        pos++;
    }
    
    if (pos >= content.size()) {
        return false;
    }
    
    result = content.substr(start, pos - start);
    pos++; // Skip closing quote
    return true;
}

static bool parseNumber(const std::string& content, double& result, size_t& pos) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExp = false;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    while (pos < content.size()) {
        if (std::isdigit(content[pos])) {
            pos++;
        } else if (content[pos] == '.' && !hasDecimal) {
            hasDecimal = true;
            pos++;
        } else if ((content[pos] == 'e' || content[pos] == 'E') && !hasExp) {
            hasExp = true;
            pos++;
            if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
                pos++;
            }
        } else {
            break;
        }
    }
    
    if (pos == start) {
        return false;
    }
    
    result = std::stod(content.substr(start, pos - start));
    return true;
}

static bool parseBool(const std::string& content, bool& result, size_t& pos) {
    if (pos + 4 <= content.size() && content.substr(pos, 4) == "true") {
        result = true;
        pos += 4;
        return true;
    } else if (pos + 5 <= content.size() && content.substr(pos, 5) == "false") {
        result = false;
        pos += 5;
        return true;
    }
    return false;
}

static bool parseNull(const std::string& content, size_t& pos) {
    if (pos + 4 <= content.size() && content.substr(pos, 4) == "null") {
        pos += 4;
        return true;
    }
    return false;
}

static bool parseJsonArray(const std::string& content, std::vector<JsonValue>& arr, size_t& pos) {
    if (pos >= content.size() || content[pos] != '[') {
        return false;
    }
    
    pos++; // Skip opening bracket
    skipWhitespace(content, pos);
    
    while (pos < content.size() && content[pos] != ']') {
        JsonValue value;
        if (!parseJsonValue(content, value, pos)) {
            return false;
        }
        
        arr.push_back(std::move(value));
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;
            skipWhitespace(content, pos);
        }
    }
    
    if (pos >= content.size() || content[pos] != ']') {
        return false;
    }
    
    pos++; // Skip closing bracket
    return true;
}

static bool parseJsonObject(const std::string& content, std::map<std::string, JsonValue>& obj, size_t& pos) {
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    
    pos++; // Skip opening brace
    skipWhitespace(content, pos);
    
    while (pos < content.size() && content[pos] != '}') {
        // Parse key
        std::string key;
        if (!parseString(content, key, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        
        pos++; // Skip colon
        skipWhitespace(content, pos);
        
        // Parse value
        JsonValue value;
        if (!parseJsonValue(content, value, pos)) {
            return false;
        }
        
        obj[key] = std::move(value);
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;
            skipWhitespace(content, pos);
        }
    }
    
    if (pos >= content.size() || content[pos] != '}') {
        return false;
    }
    
    pos++; // Skip closing brace
    return true;
}

static bool parseJsonValue(const std::string& content, JsonValue& value, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return false;
    }
    
    switch (content[pos]) {
        case '"': {
            std::string str;
            if (parseString(content, str, pos)) {
                value = str;
                return true;
            }
            return false;
        }
        case '{': {
            std::map<std::string, JsonValue> obj;
            if (parseJsonObject(content, obj, pos)) {
                value = std::move(obj);
                return true;
            }
            return false;
        }
        case '[': {
            std::vector<JsonValue> arr;
            if (parseJsonArray(content, arr, pos)) {
                value = std::move(arr);
                return true;
            }
            return false;
        }
        default:
            // Try number
            if (parseNumber(content, std::get<double>(value), pos)) {
                return true;
            }
            // Try boolean
            if (parseBool(content, std::get<bool>(value), pos)) {
                return true;
            }
            // Try null
            if (parseNull(content, pos)) {
                // Null is a special case
                return true;
            }
            return false;
    }
}

// ============================================================================
// Template instantiations
// ============================================================================

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
