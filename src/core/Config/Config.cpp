// Config.cpp - Enhanced configuration system with JSON support

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
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
    // Try JSON first, then fallback to simple format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    if (content.empty()) {
        return false;
    }
    
    // Try to parse as JSON
    if (content.find('{') != std::string::npos && parseFromJSON(content)) {
        return true;
    }
    
    // Fallback to simple key=value format
    std::istringstream stream(content);
    std::string line;
    bool success = false;
    
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            success = true;
        }
    }
    
    return success;
}

bool Config::parseFromJSON(const std::string& jsonContent) {
    try {
        size_t pos = 0;
        
        auto skipWhitespace = [&]() {
            while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) {
                pos++;
            }
        };
        
        auto parseString = [&]() -> std::string {
            skipWhitespace();
            if (pos >= jsonContent.size() || jsonContent[pos] != '"') {
                throw std::runtime_error("Expected string starting with '\"'");
            }
            pos++;
            size_t start = pos;
            while (pos < jsonContent.size() && jsonContent[pos] != '"') {
                if (jsonContent[pos] == '\\' && pos + 1 < jsonContent.size()) {
                    pos++; // Skip escaped character
                }
                pos++;
            }
            if (pos >= jsonContent.size()) {
                throw std::runtime_error("Unterminated string");
            }
            std::string result = jsonContent.substr(start, pos - start);
            pos++; // Skip closing quote
            return result;
        };
        
        auto parseValue = [&]() -> ConfigValue {
            skipWhitespace();
            
            if (pos >= jsonContent.size()) {
                throw std::runtime_error("Unexpected end of JSON");
            }
            
            char c = jsonContent[pos];
            
            if (c == '"') {
                // String
                return parseString();
            } else if (c == 't' || c == 'f') {
                // Boolean
                if (jsonContent.substr(pos, 4) == "true") {
                    pos += 4;
                    return true;
                } else if (jsonContent.substr(pos, 5) == "false") {
                    pos += 5;
                    return false;
                } else {
                    throw std::runtime_error("Expected 'true' or 'false'");
                }
            } else if (c == '-' || (c >= '0' && c <= '9')) {
                // Number
                size_t start = pos;
                while (pos < jsonContent.size() && (jsonContent[pos] == '.' || 
                                                   jsonContent[pos] == '-' ||
                                                   (jsonContent[pos] >= '0' && jsonContent[pos] <= '9'))) {
                    pos++;
                }
                std::string numStr = jsonContent.substr(start, pos - start);
                
                // Try double first
                try {
                    size_t idx;
                    double val = std::stod(numStr, idx);
                    if (idx == numStr.size()) {
                        return val;
                    }
                } catch (...) {}
                
                // Try int64_t
                try {
                    size_t idx;
                    int64_t val = std::stoll(numStr, idx);
                    if (idx == numStr.size()) {
                        return val;
                    }
                } catch (...) {}
                
                // Try int
                try {
                    size_t idx;
                    int val = std::stoi(numStr, idx);
                    if (idx == numStr.size()) {
                        return val;
                    }
                } catch (...) {}
                
                throw std::runtime_error("Invalid number format: " + numStr);
            } else if (c == '[') {
                // Array
                pos++;
                std::vector<ConfigValue> array;
                
                skipWhitespace();
                if (jsonContent[pos] == ']') {
                    pos++;
                    return array;
                }
                
                while (true) {
                    array.push_back(parseValue());
                    
                    skipWhitespace();
                    if (pos >= jsonContent.size()) {
                        throw std::runtime_error("Unterminated array");
                    }
                    
                    if (jsonContent[pos] == ']') {
                        pos++;
                        break;
                    } else if (jsonContent[pos] == ',') {
                        pos++;
                        skipWhitespace();
                        continue;
                    } else {
                        throw std::runtime_error("Expected ',' or ']' in array");
                    }
                }
                
                return array;
            } else if (c == '{') {
                // Object (treated as nested key-value pairs)
                pos++;
                while (true) {
                    skipWhitespace();
                    
                    if (pos >= jsonContent.size() || jsonContent[pos] == '}') {
                        if (pos < jsonContent.size()) pos++;
                        break;
                    }
                    
                    // Parse key
                    std::string key = parseString();
                    skipWhitespace();
                    
                    if (pos >= jsonContent.size() || jsonContent[pos] != ':') {
                        throw std::runtime_error("Expected ':' after key in object");
                    }
                    pos++;
                    
                    // Parse value
                    ConfigValue value = parseValue();
                    
                    // Handle nested objects with dot notation
                    if (key.find('.') != std::string::npos) {
                        // For nested objects, we could implement recursive parsing
                        // For now, treat as a string value
                        set(key, std::get<std::string>(value), ConfigSource::File);
                    } else {
                        set(key, value, ConfigSource::File);
                    }
                    
                    skipWhitespace();
                    if (pos < jsonContent.size() && jsonContent[pos] == '}') {
                        pos++;
                        break;
                    } else if (pos < jsonContent.size() && jsonContent[pos] == ',') {
                        pos++;
                        continue;
                    } else {
                        throw std::runtime_error("Expected ',' or '}' in object");
                    }
                }
                
                return ConfigValue(); // Empty variant
            } else {
                throw std::runtime_error("Unexpected character in JSON: " + std::string(1, c));
            }
        };
        
        skipWhitespace();
        if (pos >= jsonContent.size() || jsonContent[pos] != '{') {
            return false; // Not a JSON object
        }
        
        pos++; // Skip opening brace
        
        while (true) {
            skipWhitespace();
            
            if (pos >= jsonContent.size() || jsonContent[pos] == '}') {
                break;
            }
            
            // Parse key
            std::string key = parseString();
            skipWhitespace();
            
            if (pos >= jsonContent.size() || jsonContent[pos] != ':') {
                throw std::runtime_error("Expected ':' after key in JSON object");
            }
            pos++;
            
            // Parse value
            ConfigValue value = parseValue();
            set(key, value, ConfigSource::File);
            
            skipWhitespace();
            if (pos >= jsonContent.size()) {
                throw std::runtime_error("Unterminated JSON object");
            }
            
            if (jsonContent[pos] == '}') {
                pos++;
                break;
            } else if (jsonContent[pos] == ',') {
                pos++;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or '}' in JSON object");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    return loadFromArgsEnhanced(argc, argv);
}

bool Config::loadFromArgsEnhanced(int argc, char** argv) {
    bool success = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --help flags
        if (arg == "--help" || arg == "-h") {
            // Print usage information
            printUsage();
            return false;
        }
        
        // Handle --config=file.json format
        if (arg.substr(0, 8) == "--config") {
            std::string configFile;
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
            if (!configFile.empty()) {
                if (loadFromFile(configFile)) {
                    success = true;
                }
            }
            continue;
        }
        
        // Handle --set=key=value format (preferred)
        if (arg.substr(0, 6) == "--set=") {
            size_t pos = arg.find('=', 6);
            if (pos != std::string::npos) {
                std::string key = arg.substr(6, pos - 6);
                std::string value = arg.substr(pos + 1);
                if (setFromString(key, value)) {
                    success = true;
                }
            }
            continue;
        }
        
        // Handle --set key value format
        if (arg.substr(0, 6) == "--set" && i + 1 < argc) {
            std::string key = arg.substr(6);
            if (!key.empty()) {
                std::string value = argv[++i];
                if (setFromString(key, value)) {
                    success = true;
                }
            }
            continue;
        }
        
        // Handle --key=value format (traditional)
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (setFromString(key, value)) {
                    success = true;
                }
            }
            continue;
        }
        
        // Handle -key value format (traditional)
        if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                if (setFromString(key, value)) {
                    success = true;
                }
            }
            continue;
        }
        
        // Handle boolean flags (e.g., --enable_feature)
        if (arg[0] == '-' && arg.size() > 2) {
            std::string key = arg.substr(1);
            if (key.find('=') != std::string::npos) {
                // Handle --enable_feature=true
                size_t pos = key.find('=');
                std::string actualKey = key.substr(0, pos);
                std::string actualValue = key.substr(pos + 1);
                if (setFromString(actualKey, actualValue)) {
                    success = true;
                }
            } else {
                // Handle --enable_feature (sets to true)
                setFromString(key, "true");
                success = true;
            }
            continue;
        }
    }
    
    return success;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr) {
    // Attempt to determine type from value string
    // Try bool first (true/false)
    if (valueStr == "true") {
        set(key, true, ConfigSource::CommandLine);
        return true;
    } else if (valueStr == "false") {
        set(key, false, ConfigSource::CommandLine);
        return true;
    }
    
    // Try integer parsing
    try {
        size_t pos;
        int64_t intVal = std::stoll(valueStr, pos);
        if (pos == valueStr.size()) {
            set(key, intVal, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {}
    
    // Try double parsing
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, pos);
        if (pos == valueStr.size()) {
            set(key, doubleVal, ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {}
    
    // Try integer parsing (non-negative)
    try {
        size_t pos;
        unsigned long long uintVal = std::stoull(valueStr, pos);
        if (pos == valueStr.size()) {
            set(key, static_cast<int64_t>(uintVal), ConfigSource::CommandLine);
            return true;
        }
    } catch (...) {}
    
    // Default to string
    set(key, valueStr, ConfigSource::CommandLine);
    return true;
}

void Config::printUsage() const {
    std::cout << "NLM Configuration Options:" << std::endl;
    std::cout << "  --help, -h                    Show this help message" << std::endl;
    std::cout << "  --config=file.json           Load configuration from JSON file" << std::endl;
    std::cout << "  --set=key=value              Set configuration value" << std::endl;
    std::cout << "  --key=value                  Traditional key=value format" << std::endl;
    std::cout << "  --enable_feature             Set boolean flag to true" << std::endl;
    std::cout << "  -k value                     Short form for --key" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  --set=neuron_count=1000" << std::endl;
    std::cout << "  --set neuron_count 2000" << std::endl;
    std::cout << "  --config=my_config.json" << std::endl;
    std::cout << "  --enable_plasticity --debug" << std::endl;
}

bool Config::validate() const {
    std::string errors = validateErrors();
    return errors.empty();
}

std::string Config::validateErrors() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::string error = validateEntry(entry);
        if (!error.empty()) {
            if (!oss.str().empty()) oss << "; ";
            oss << key << ": " << error;
        }
    }
    
    return oss.str();
}

std::string Config::validateEntry(const ConfigEntry& entry) const {
    const std::string& key = entry.key;
    
    // Required validation for specific keys
    if (key == "neuron_count") {
        auto value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            return "must be positive";
        }
        if (value > 1000000) {
            return "too large (max 1,000,000)";
        }
    } else if (key == "region_count") {
        auto value = std::get<int64_t>(entry.value);
        if (value <= 0) {
            return "must be positive";
        }
    } else if (key == "simulation_timestep") {
        auto value = std::get<double>(entry.value);
        if (value <= 0.0) {
            return "must be positive";
        }
        if (value > 1.0) {
            return "too large (max 1.0)";
        }
    } else if (key == "connection_probability") {
        auto value = std::get<double>(entry.value);
        if (value < 0.0 || value > 1.0) {
            return "must be between 0.0 and 1.0";
        }
    } else if (key == "random_seed") {
        auto value = std::get<int64_t>(entry.value);
        if (value < 0) {
            return "must be non-negative";
        }
    }
    
    // Validate string values
    if (std::holds_alternative<std::string>(entry.value)) {
        const std::string& strValue = std::get<std::string>(entry.value);
        if (strValue.empty()) {
            return "cannot be empty";
        }
        
        // Check for potentially problematic characters
        if (containsProblematicCharacters(strValue)) {
            return "contains invalid characters";
        }
    }
    
    return "";
}

bool Config::containsProblematicCharacters(const std::string& str) const {
    // Check for control characters (except tab, newline, carriage return)
    for (char c : str) {
        if (std::iscntrl(c) && c != '\t' && c != '\n' && c != '\r') {
            return true;
        }
    }
    return false;
}

void Config::mergeFrom(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Only overwrite if the source is higher priority
        // CommandLine > File > Default > Runtime
        bool shouldOverwrite = false;
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == pImpl->entries.end()) {
            shouldOverwrite = true; // Key doesn't exist, add it
        } else {
            // Compare source priorities
            shouldOverwrite = sourcePriority(entry.source) > sourcePriority(it->source);
        }
        
        if (shouldOverwrite) {
            it->value = entry.value;
            it->source = entry.source;
            it->description = entry.description;
        }
    }
}

void Config::applyOverrides(const Config& overrides) {
    // Apply overrides with the highest priority
    for (const auto& entry : overrides.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = entry.source; // Preserve override priority
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

int Config::sourcePriority(ConfigSource source) {
    switch (source) {
        case ConfigSource::CommandLine:
            return 4;
        case ConfigSource::File:
            return 3;
        case ConfigSource::Default:
            return 2;
        case ConfigSource::Runtime:
            return 1;
        default:
            return 0;
    }
}

std::string Config::getSourceSummary() const {
    std::ostringstream oss;
    oss << "Configuration sources:" << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << ": " << static_cast<int>(entry.source);
        if (!entry.description.empty()) {
            oss << " (" << entry.description << ")";
        }
        oss << std::endl;
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
