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
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot load config from empty filepath");
        return false;
    }
    
    // Detect file format based on extension or content
    bool isJSON = isJSONFile(filepath);
    bool isYAML = false;
    
    if (!isJSON) {
        // Check for YAML extension
        std::string lowerPath = toLower(filepath);
        if (lowerPath.size() >= 5 && 
            (lowerPath.substr(lowerPath.size() - 5) == ".yaml" || 
             lowerPath.substr(lowerPath.size() - 2) == ".yml")) {
            isYAML = true;
        }
    }
    
    // Parse based on format
    if (isJSON) {
        return parseJSONFile(filepath);
    } else if (isYAML) {
        return parseYAMLFile(filepath); // TODO: Implement YAML parsing
    } else {
        return parseSimpleKeyValueFile(filepath);
    }
}

// Helper function to detect file format
static bool isJSONFile(const std::string& filepath) {
    // Simple extension check
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return true;
    }
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".JSON") {
        return true;
    }
    
    // For other files, check content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Check if it starts with "{" or "[" (basic JSON detection)
        if (firstLine.empty() || (firstLine.find('{') == 0) || 
            (firstLine.find('[') == 0)) {
            return true;
        }
    }
    
    return false;
}

// Simple JSON parser for basic types
bool parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON config file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    // Simple JSON parsing - expects format like {"key": value, "key2": value2}
    size_t pos = 0;
    skipWhitespace(jsonContent, pos);
    
    if (pos >= jsonContent.size() || jsonContent[pos] != '{') {
        NLM_LOG_ERROR("Invalid JSON format in config file: " + filepath);
        return false;
    }
    
    pos++; // Skip opening brace
    skipWhitespace(jsonContent, pos);
    
    while (pos < jsonContent.size() && jsonContent[pos] != '}') {
        // Parse key
        skipWhitespace(jsonContent, pos);
        if (!parseJSONString(jsonContent, pos, std::ignore)) {
            break;
        }
        std::string key = std::get<std::string>(std::ignore);
        
        skipWhitespace(jsonContent, pos);
        
        if (pos >= jsonContent.size() || jsonContent[pos] != ':') {
            NLM_LOG_ERROR("Missing colon in JSON key-value pair in config file: " + filepath);
            return false;
        }
        
        pos++; // Skip colon
        skipWhitespace(jsonContent, pos);
        
        // Parse value
        ConfigValue value;
        if (!parseJSONValue(jsonContent, pos, value)) {
            NLM_LOG_ERROR("Invalid value for key '" + key + "' in JSON config file: " + filepath);
            return false;
        }
        
        // Set the value
        set(key, value, ConfigSource::File);
        
        skipWhitespace(jsonContent, pos);
        
        if (pos < jsonContent.size() && jsonContent[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(jsonContent, pos);
        } else if (pos < jsonContent.size() && jsonContent[pos] != '}') {
            break;
        }
    }
    
    if (pos >= jsonContent.size() || jsonContent[pos] != '}') {
        NLM_LOG_ERROR("Unterminated JSON object in config file: " + filepath);
        return false;
    }
    
    return true;
}

bool parseYAMLFile(const std::string& filepath) {
    // TODO: Implement YAML parsing
    // For now, just log a warning and use simple parsing
    NLM_LOG_WARNING("YAML config parsing not yet implemented. Using simple key=value format for: " + filepath);
    return parseSimpleKeyValueFile(filepath);
}

bool parseSimpleKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
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
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key in config file " + filepath + " at line " + std::to_string(lineNumber));
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to infer value type
            bool parsed = false;
            
            // Try integer
            try {
                size_t posEnd;
                int intVal = std::stoi(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    parsed = true;
                    continue;
                }
            } catch (...) {}
            
            // Try double
            try {
                size_t posEnd;
                double doubleVal = std::stod(value, &posEnd);
                if (posEnd == value.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    parsed = true;
                    continue;
                }
            } catch (...) {}
            
            // Try boolean
            if (!parsed) {
                if (value == "true") {
                    set(key, true, ConfigSource::File);
                    parsed = true;
                } else if (value == "false") {
                    set(key, false, ConfigSource::File);
                    parsed = true;
                }
            }
            
            // Fallback to string
            if (!parsed) {
                set(key, value, ConfigSource::File);
            }
        } else {
            NLM_LOG_WARNING("Invalid line format in config file " + filepath + " at line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

// Helper functions for JSON parsing
bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& outValue) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return false;
    }
    
    char c = json[pos];
    
    if (c == '"') {
        // String
        return parseJSONString(json, pos, outValue);
    } else if (c == 't' || c == 'f') {
        // Boolean
        return parseJSONBoolean(json, pos, outValue);
    } else if (c == 'n') {
        // Null - not supported, return empty string
        return false;
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number
        return parseJSONNumber(json, pos, outValue);
    } else {
        return false;
    }
}

// Helper function to skip whitespace
void skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        pos++;
    }
}

// Helper function to parse JSON string
bool parseJSONString(const std::string& json, size_t& pos, ConfigValue& outValue) {
    if (pos >= json.size() || json[pos] != '"') {
        return false;
    }
    
    pos++; // Skip opening quote
    std::string strValue;
    
    while (pos < json.size()) {
        if (json[pos] == '\\') {
            pos++; // Skip escaped character
            if (pos < json.size()) {
                strValue += json[pos];
                pos++;
            }
        } else if (json[pos] == '"') {
            pos++; // Skip closing quote
            outValue = strValue;
            return true;
        } else {
            strValue += json[pos];
            pos++;
        }
    }
    
    return false; // Unterminated string
}

// Helper function to parse JSON boolean
bool parseJSONBoolean(const std::string& json, size_t& pos, ConfigValue& outValue) {
    std::string boolStr;
    
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
        boolStr = "true";
        pos += 4;
    } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
        boolStr = "false";
        pos += 5;
    } else {
        return false;
    }
    
    outValue = boolStr == "true";
    return true;
}

// Helper function to parse JSON number
bool parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& outValue) {
    size_t start = pos;
    
    // Handle negative sign
    if (json[pos] == '-') {
        pos++;
    }
    
    // Parse integer part
    while (pos < json.size() && std::isdigit(json[pos])) {
        pos++;
    }
    
    // Parse decimal part
    if (pos < json.size() && json[pos] == '.') {
        pos++; // Skip decimal point
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    // Parse exponent
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        pos++; // Skip exponent character
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            pos++; // Skip sign
        }
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    if (pos == start) {
        return false;
    }
    
    std::string numStr = json.substr(start, pos - start);
    
    // Try to parse as integer first
    try {
        size_t posEnd;
        int intVal = std::stoi(numStr, &posEnd);
        if (posEnd == numStr.size()) {
            outValue = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t posEnd;
        double doubleVal = std::stod(numStr, &posEnd);
        if (posEnd == numStr.size()) {
            outValue = doubleVal;
            return true;
        }
    } catch (...) {}
    
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments to process
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.empty()) {
            continue; // Skip empty arguments
        }
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key in command line argument: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Malformed command line argument (missing '='): " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            if (i + 1 >= argc) {
                NLM_LOG_WARNING("Missing value for argument: " + arg);
                continue;
            }
            
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key in command line argument: " + arg);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Unknown command line argument format: " + arg);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (filepath.empty()) {
        NLM_LOG_ERROR("Cannot save config to empty filepath");
        return false;
    }
    
    // Check if file exists and is writable
    std::ifstream existingFile(filepath);
    if (existingFile.good()) {
        existingFile.close();
        // File exists, check if it's writable
        std::ofstream testWrite(filepath, std::ios::app);
        if (!testWrite.is_open()) {
            NLM_LOG_ERROR("Cannot write to existing config file: " + filepath);
            return false;
        }
        testWrite.close();
    }
    
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    file.exceptions(std::ofstream::badbit | std::ofstream::failbit);
    
    try {
        for (const auto& entry : pImpl->entries) {
            // Skip empty keys
            if (entry.key.empty()) {
                continue;
            }
            
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Handle different value types
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || 
                                     std::is_same_v<T, int64_t> ||
                                     std::is_same_v<T, double>) {
                    file << arg;
                } else {
                    file << "\"" << arg << "\""; // Fallback for other types
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        file.flush();
        if (!file.good()) {
            NLM_LOG_ERROR("Error occurred while writing config file: " + filepath);
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception while writing config file " + filepath + ": " + std::string(e.what()));
        return false;
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
    // Validate key
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot set config with empty key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new config key: " + key);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot set config with empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot set config with empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot set config with empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot set config with empty key");
        return;
    }
    
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_WARNING("Cannot remove config with empty key");
        return;
    }
    
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
