#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <string_view>
#include <optional>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension to determine format
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase for comparison
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
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

// Helper functions for JSON parsing
static std::string parseString(const std::string& line, size_t& pos);
static bool parseJSONValue(const std::string& line, size_t& pos, ConfigValue& value);
static bool parseJSONArray(const std::string& line, size_t& pos, ConfigValue& value);
static bool parseJSONObject(const std::string& line, size_t& pos, ConfigValue& value);

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Skip whitespace
    size_t pos = 0;
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    
    if (pos >= content.size() || content[pos] != '{') {
        NLM_LOG_ERROR("Invalid JSON format in file: " + filepath);
        return false;
    }
    
    try {
        ConfigValue rootValue;
        if (!parseJSONObject(content, pos, rootValue)) {
            NLM_LOG_ERROR("Failed to parse JSON from file: " + filepath);
            return false;
        }
        
        if (!std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(rootValue)) {
            NLM_LOG_ERROR("Invalid JSON root element in file: " + filepath);
            return false;
        }
        
        auto& jsonObject = std::get<std::unordered_map<std::string, ConfigValue>>(rootValue);
        
        for (const auto& [key, value] : jsonObject) {
            set(key, value, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception parsing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, fall back to simple format since full YAML parsing is complex
    // This maintains backward compatibility while providing the infrastructure
    NLM_LOG_WARNING("YAML parsing not yet implemented, falling back to simple format for: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid format in " + filepath + " line " + std::to_string(lineNumber) +
                          ": missing '='");
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove surrounding quotes
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                 (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing key-value pair in " + filepath + 
                         " line " + std::to_string(lineNumber) +
                         ": " + std::string(e.what()));
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple format file: " + filepath);
    return true;
}

// JSON parsing implementation

static std::string parseString(const std::string& line, size_t& pos) {
    std::string result;
    if (pos >= line.size() || line[pos] != '"') {
        throw std::runtime_error("Expected string starting with \"\"");
    }
    
    pos++; // Skip opening quote
    while (pos < line.size() && line[pos] != '\"') {
        if (line[pos] == '\\' && pos + 1 < line.size()) {
            pos++; // Skip escape character
            if (pos >= line.size()) break;
        }
        result += line[pos];
        pos++;
    }
    
    if (pos >= line.size() || line[pos] != '\"') {
        throw std::runtime_error("Unterminated string");
    }
    
    pos++; // Skip closing quote
    return result;
}

static bool parseJSONValue(const std::string& line, size_t& pos, ConfigValue& value) {
    skipWhitespace(line, pos);
    
    if (pos >= line.size()) {
        return false;
    }
    
    char ch = line[pos];
    
    if (ch == '"') {
        // String value
        std::string str = parseString(line, pos);
        value = str;
        return true;
    } else if (ch == '{') {
        // Object
        return parseJSONObject(line, pos, value);
    } else if (ch == '[') {
        // Array
        return parseJSONArray(line, pos, value);
    } else if (std::isdigit(ch) || ch == '-' || ch == '+') {
        // Number
        size_t start = pos;
        while (pos < line.size() && (std::isdigit(line[pos]) || 
                                     line[pos] == '.' || 
                                     line[pos] == 'e' || 
                                     line[pos] == 'E' ||
                                     line[pos] == '+' ||
                                     line[pos] == '-')) {
            pos++;
        }
        std::string numStr = line.substr(start, pos - start);
        
        try {
            if (numStr.find('.') != std::string::npos || 
                numStr.find('e') != std::string::npos || 
                numStr.find('E') != std::string::npos) {
                value = std::stod(numStr);
            } else {
                value = std::stoll(numStr);
            }
        } catch (...) {
            return false;
        }
        return true;
    } else if (ch == 't' || ch == 'f') {
        // Boolean
        if (line.substr(pos, 4) == "true") {
            value = true;
            pos += 4;
            return true;
        } else if (line.substr(pos, 5) == "false") {
            value = false;
            pos += 5;
            return true;
        }
    } else if (ch == 'n' && line.substr(pos, 4) == "null") {
        // null - not supported, skip
        pos += 4;
        return false;
    }
    
    return false;
}

static bool parseJSONObject(const std::string& line, size_t& pos, ConfigValue& value) {
    if (pos >= line.size() || line[pos] != '{') {
        return false;
    }
    
    pos++; // Skip {
    skipWhitespace(line, pos);
    
    std::unordered_map<std::string, ConfigValue> obj;
    
    while (pos < line.size() && line[pos] != '}') {
        skipWhitespace(line, pos);
        
        if (line[pos] == '"') {
            std::string key = parseString(line, pos);
            skipWhitespace(line, pos);
            
            if (pos >= line.size() || line[pos] != ':') {
                return false;
            }
            
            pos++; // Skip :
            
            ConfigValue val;
            if (!parseJSONValue(line, pos, val)) {
                return false;
            }
            
            obj[key] = val;
            
            skipWhitespace(line, pos);
            if (pos < line.size() && line[pos] == ',') {
                pos++; // Skip ,
            }
        } else {
            return false;
        }
    }
    
    if (pos >= line.size() || line[pos] != '}') {
        return false;
    }
    
    pos++; // Skip }
    value = obj;
    return true;
}

static bool parseJSONArray(const std::string& line, size_t& pos, ConfigValue& value) {
    if (pos >= line.size() || line[pos] != '[') {
        return false;
    }
    
    pos++; // Skip [
    skipWhitespace(line, pos);
    
    std::vector<ConfigValue> arr;
    
    while (pos < line.size() && line[pos] != ']') {
        ConfigValue element;
        if (!parseJSONValue(line, pos, element)) {
            return false;
        }
        
        arr.push_back(element);
        
        skipWhitespace(line, pos);
        if (pos < line.size() && line[pos] == ',') {
            pos++; // Skip ,
        }
    }
    
    if (pos >= line.size() || line[pos] != ']') {
        return false;
    }
    
    pos++; // Skip ]
    value = arr;
    return true;
}

static void skipWhitespace(const std::string& line, size_t& pos) {
    while (pos < line.size() && std::isspace(line[pos])) {
        pos++;
    }
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
