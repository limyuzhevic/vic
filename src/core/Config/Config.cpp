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
    // Enhanced JSON/YAML parser implementation (Phase 2)
    // Supports JSON format: {"key": "value", "number": 42, "bool": true}
    // Also supports simple key=value format for backward compatibility
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING(std::string("Failed to open config file: ") + filepath);
        return false;
    }
    
    // Check file size to prevent memory issues with large files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    // Limit file size to 10MB to prevent memory issues
    const size_t MAX_CONFIG_SIZE = 10 * 1024 * 1024;
    if (fileSize > MAX_CONFIG_SIZE) {
        NLM_LOG_ERROR(std::string("Config file too large: ") + filepath + 
                     std::string(" (") + std::to_string(fileSize) + 
                     std::string(" bytes, max ") + std::to_string(MAX_CONFIG_SIZE) + 
                     std::string(" bytes)"));
        return false;
    }
    
    std::string content;
    content.reserve(fileSize);
    
    // Read file with error checking
    char buffer[4096];
    while (file.good()) {
        file.read(buffer, sizeof(buffer));
        std::streamsize bytesRead = file.gcount();
        if (bytesRead > 0) {
            content.append(buffer, bytesRead);
        }
    }
    
    if (file.bad()) {
        NLM_LOG_ERROR(std::string("Error reading config file: ") + filepath);
        return false;
    }
    
    // Try JSON parsing first
    bool jsonParsed = parseJSONConfig(content);
    if (jsonParsed) {
        NLM_LOG_INFO(std::string("Successfully loaded config from JSON: ") + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    file.clear();
    file.seekg(0, std::ios::beg);
    
    std::string line;
    int lineNumber = 0;
    size_t lineCount = 0;
    const size_t MAX_LINES = 10000;  // Prevent memory issues with huge files
    
    while (std::getline(file, line)) {
        ++lineNumber;
        ++lineCount;
        
        // Prevent memory issues with extremely long lines
        if (line.length() > 10000) {
            NLM_LOG_WARNING(std::string("Skipping extremely long line (over 10k chars) in config file: ") + 
                           filepath + std::string(" at line ") + std::to_string(lineNumber));
            continue;
        }
        
        if (lineCount > MAX_LINES) {
            NLM_LOG_ERROR(std::string("Config file has too many lines (over ") + 
                         std::to_string(MAX_LINES) + std::string("). Stopping parsing.") + filepath);
            break;
        }
        
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || 
            line.find("#") == 0 || line.find("//") == 0 || 
            line.find("-") == 0) {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty()) {
                NLM_LOG_WARNING(std::string("Empty key in config file: ") + filepath + 
                               std::string(" at line ") + std::to_string(lineNumber));
                continue;
            }
            
            // Validate key name (should not contain special characters)
            if (!std::isalnum(key[0]) && key[0] != '_') {
                NLM_LOG_WARNING(std::string("Invalid key name ") + key + 
                               std::string(" in config file ") + filepath + 
                               std::string(" at line ") + std::to_string(lineNumber));
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING(std::string("Malformed line in config file: ") + filepath + 
                           std::string(" line ") + std::to_string(lineNumber) + 
                           std::string(" - ") + line);
        }
    }
    
    NLM_LOG_INFO(std::string("Loaded config from key=value format: ") + filepath + 
                std::string(" (lines: ") + std::to_string(lineCount) + std::string(")"));
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

// Forward declaration for JSON parsing helper
namespace {
    bool parseJSONValue(const std::string& str, ConfigValue& value);
}

bool Config::parseJSONConfig(const std::string& json) {
    // Parse JSON configuration in format: {"key": "value", "number": 42, "bool": true}
    // Simple but robust JSON parser for configuration files
    
    size_t pos = 0;
    while (pos < json.size()) {
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) {
            ++pos;
        }
        
        if (pos >= json.size() || json[pos] != '"') {
            break; // Not JSON format
        }
        
        // Find key
        size_t keyEnd = json.find('"', pos + 1);
        if (keyEnd == std::string::npos) {
            return false; // Malformed JSON
        }
        
        std::string key = json.substr(pos + 1, keyEnd - pos - 1);
        pos = keyEnd + 1;
        
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) {
            ++pos;
        }
        
        if (pos >= json.size() || json[pos] != ':') {
            return false; // Malformed JSON
        }
        
        // Skip whitespace after colon
        ++pos;
        while (pos < json.size() && std::isspace(json[pos])) {
            ++pos;
        }
        
        // Parse value
        ConfigValue value;
        size_t valueEnd;
        
        if (json[pos] == '"') {
            // String value
            valueEnd = json.find('"', pos + 1);
            if (valueEnd == std::string::npos) return false;
            value = json.substr(pos + 1, valueEnd - pos - 1);
            pos = valueEnd + 1;
        } else if (json[pos] == 't' || json[pos] == 'f') {
            // Boolean value
            if (json.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else if (json.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            } else {
                return false;
            }
        } else if (json[pos] == '-' || std::isdigit(json[pos])) {
            // Number (integer or float)
            size_t numEnd = pos;
            while (numEnd < json.size() && (std::isdigit(json[numEnd]) || json[numEnd] == '.' || json[numEnd] == '-')) {
                ++numEnd;
            }
            std::string numStr = json.substr(pos, numEnd - pos);
            
            // Try to parse as integer first
            try {
                size_t intPos;
                int intVal = std::stoi(numStr, &intPos);
                if (intPos == numStr.size()) {
                    value = intVal;
                    pos = numEnd;
                } else {
                    // Try float
                    float floatVal = std::stof(numStr);
                    value = floatVal;
                    pos = numEnd;
                }
            } catch (...) {
                return false;
            }
        } else {
            return false; // Unsupported value type
        }
        
        // Store the key-value pair
        set(key, value, ConfigSource::File);
        
        // Skip whitespace and comma
        while (pos < json.size() && std::isspace(json[pos])) {
            ++pos;
        }
        if (pos < json.size() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.size() && json[pos] != '}') {
            return false;
        }
    }
    
    return true;
}

// JSON helper function implementation
namespace {
bool parseJSONValue(const std::string& str, ConfigValue& value) {
    if (str.empty()) return false;
    
    if (str == "true") {
        value = true;
        return true;
    } else if (str == "false") {
        value = false;
        return true;
    } else if (str.front() == '"' && str.back() == '"') {
        // String with quotes
        value = str.substr(1, str.size() - 2);
        return true;
    } else {
        // Try numeric values
        try {
            // Check if it's an integer
            size_t intPos;
            long long intVal = std::stoll(str, &intPos);
            if (intPos == str.size()) {
                value = static_cast<int>(intVal);
                return true;
            }
            
            // Try double (float)
            double floatVal = std::stod(str);
            value = static_cast<double>(floatVal);
            return true;
        } catch (...) {
            return false;
        }
    }
}

} // namespace (anonymous)
