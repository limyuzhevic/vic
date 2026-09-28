#include "Config.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigValue> nestedValues;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension
    FileFormat format = detectFileFormat(filepath);
    
    Logger::getGlobal().debug("Loading configuration from: " + filepath + " (format: " + 
                             (format == FileFormat::JSON ? "JSON" : 
                              format == FileFormat::YAML ? "YAML" : 
                              format == FileFormat::KeyValue ? "key=value" : 
                              "unknown") + ")");
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::getGlobal().error("Failed to open configuration file: " + filepath);
        throw ConfigException(ConfigErrorType::FileNotFound, 
                            "Could not open configuration file: " + filepath, 
                            -1, filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    try {
        switch (format) {
            case FileFormat::JSON:
                return loadFromJSON(content);
            case FileFormat::YAML:
                return loadFromYAML(content);
            case FileFormat::KeyValue:
                return loadFromKeyValue(content);
            default:
                throw ConfigException(ConfigErrorType::InvalidFormat,
                                    "Unknown file format for: " + filepath,
                                    -1, filepath);
        }
    } catch (const ConfigException& e) {
        Logger::getGlobal().error("Configuration parsing failed: " + std::string(e.what()));
        throw;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Unexpected error parsing configuration: " + std::string(e.what()));
        throw ConfigException(ConfigErrorType::ParseError,
                            "Unexpected error parsing configuration: " + std::string(e.what()),
                            -1, filepath);
    }
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        // Simple JSON parser implementation
        // This is a basic implementation - in production you'd use a proper JSON library
        clear();
        
        // Remove whitespace
        std::string content = trim(jsonString);
        
        // Check if it's an object (starts with {)
        if (content.empty() || content[0] != '{') {
            throw ConfigException(ConfigErrorType::InvalidFormat,
                                "JSON must start with '{' for object format",
                                -1, "JSON input");
        }
        
        // Parse JSON object
        size_t pos = 1; // Skip opening {
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Check for closing }
            if (pos >= content.size() || content[pos] == '}') {
                break;
            }
            
            // Parse key
            std::string key;
            while (pos < content.size() && content[pos] != '"' && !std::isspace(content[pos])) {
                key += content[pos];
                pos++;
            }
            
            if (pos >= content.size() || content[pos] != '"') {
                throw ConfigException(ConfigErrorType::ParseError,
                                    "Expected key name in double quotes",
                                    -1, "JSON input");
            }
            
            // Skip closing quote
            pos++;
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Expect colon
            if (pos >= content.size() || content[pos] != ':') {
                throw ConfigException(ConfigErrorType::ParseError,
                                    "Expected ':' after key",
                                    -1, "JSON input");
            }
            
            // Skip colon
            pos++;
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Parse value
            ConfigValue value;
            size_t valuePos;
            
            if (pos < content.size() && content[pos] == '"') {
                // String value
                std::string strValue;
                pos++; // Skip opening quote
                while (pos < content.size() && content[pos] != '\"') {
                    if (content[pos] == '\\' && pos + 1 < content.size()) {
                        pos++; // Skip escape character
                    }
                    strValue += content[pos];
                    pos++;
                }
                value = strValue;
            }
            else if (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '-')) {
                // Number value
                size_t startPos = pos;
                while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                    pos++;
                }
                std::string numStr = content.substr(startPos, pos - startPos);
                
                // Try to parse as integer
                try {
                    size_t endPos;
                    long long intVal = std::stoll(numStr, &endPos);
                    if (endPos == numStr.size()) {
                        value = intVal;
                    } else {
                        double doubleVal = std::stod(numStr, &endPos);
                        if (endPos == numStr.size()) {
                            value = doubleVal;
                        } else {
                            throw ConfigException(ConfigErrorType::ParseError,
                                                "Invalid number format: " + numStr,
                                                -1, "JSON input");
                        }
                    }
                } catch (...) {
                    throw ConfigException(ConfigErrorType::ParseError,
                                        "Invalid number format: " + numStr,
                                        -1, "JSON input");
                }
            }
            else if (pos < content.size() && content[pos] == 't' && content.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            }
            else if (pos < content.size() && content[pos] == 'f' && content.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            }
            else if (pos < content.size() && content[pos] == 'n' && content.substr(pos, 4) == "null") {
                value = std::string("null");
                pos += 4;
            }
            else {
                throw ConfigException(ConfigErrorType::ParseError,
                                    "Unsupported JSON value type at position " + std::to_string(pos),
                                    -1, "JSON input");
            }
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Parse comma or end
            if (pos < content.size() && content[pos] == ',') {
                pos++; // Skip comma
            } else if (pos < content.size() && content[pos] != '}') {
                throw ConfigException(ConfigErrorType::ParseError,
                                    "Expected ',' or '}' after value",
                                    -1, "JSON input");
            }
            
            // Add to configuration
            set(key, value, ConfigSource::File);
        }
        
        Logger::getGlobal().debug("JSON configuration loaded successfully");
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException(ConfigErrorType::ParseError,
                            "Error parsing JSON: " + std::string(e.what()),
                            -1, "JSON input");
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    try {
        // Simple YAML parser implementation
        // This is a basic implementation - in production you'd use a proper YAML library
        clear();
        
        std::string content = trim(yamlString);
        
        // Basic YAML parsing for key=value pairs and simple structures
        size_t pos = 0;
        int lineNum = 1;
        int validEntries = 0;
        int invalidEntries = 0;
        
        while (pos < content.size()) {
            // Find next line
            size_t lineEnd = content.find('\n', pos);
            if (lineEnd == std::string::npos) {
                lineEnd = content.size();
            }
            
            std::string line = content.substr(pos, lineEnd - pos);
            pos = lineEnd + 1;
            lineNum++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line.find("#") == 0 || line.find("-") == 0) {
                continue;
            }
            
            // Parse YAML key-value pairs
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string valueStr = trim(line.substr(colonPos + 1));
                
                // Remove quotes from value
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '"' && valueStr.back() == '"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                // Parse value based on content
                ConfigValue value;
                bool isBool = false;
                
                if (valueStr == "true" || valueStr == "false") {
                    isBool = true;
                }
                
                // Try to parse as boolean
                if (valueStr == "true") {
                    value = true;
                } else if (valueStr == "false") {
                    value = false;
                } else if (isNumber(valueStr)) {
                    // Try integer first
                    try {
                        size_t endPos;
                        long long intVal = std::stoll(valueStr, &endPos);
                        if (endPos == valueStr.size()) {
                            value = intVal;
                        } else {
                            double doubleVal = std::stod(valueStr, &endPos);
                            if (endPos == valueStr.size()) {
                                value = doubleVal;
                            } else {
                                value = valueStr;
                            }
                        }
                    } catch (...) {
                        value = valueStr;
                    }
                } else {
                    value = valueStr;
                }
                
                set(key, value, ConfigSource::File);
                validEntries++;
            } else {
                Logger::getGlobal().warning("Invalid YAML format at line " + std::to_string(lineNum) + ": " + line);
                invalidEntries++;
            }
        }
        
        if (invalidEntries > 0) {
            Logger::getGlobal().warning("YAML configuration loaded: " + std::to_string(validEntries) + 
                                       " entries, " + std::to_string(invalidEntries) + " invalid lines (ignored)");
        } else {
            Logger::getGlobal().debug("YAML configuration loaded: " + std::to_string(validEntries) + " entries (all valid)");
        }
        
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException(ConfigErrorType::ParseError,
                            "Error parsing YAML: " + std::string(e.what()),
                            -1, "YAML input");
    }
}

bool Config::loadFromKeyValue(const std::string& keyValueString) {
    return parseKeyValueInternal(keyValueString);
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

bool Config::loadFromKeyValue(const std::string& keyValueString) {
    return parseKeyValueInternal(keyValueString);
}

bool Config::saveToFile(const std::string& filepath) const {
    // Detect file format based on extension
    FileFormat format = detectFileFormat(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        switch (format) {
            case FileFormat::JSON:
                file << saveToJSON();
                break;
            case FileFormat::YAML:
                file << saveToYAML();
                break;
            case FileFormat::KeyValue:
                serializeToKeyValue(file);
                break;
            default:
                // Default to key-value format for backward compatibility
                serializeToKeyValue(file);
                break;
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Failed to save configuration: " + std::string(e.what()));
        return false;
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
    pImpl->nestedValues.clear();
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

FileFormat Config::detectFileFormat(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") return FileFormat::JSON;
    if (ext == "yaml" || ext == "yml") return FileFormat::YAML;
    if (ext == "cfg" || ext == "ini" || ext == "properties") return FileFormat::KeyValue;
    
    return FileFormat::Unknown;
}

bool Config::validate() const {
    // Basic validation - check for required keys or value constraints
    // This is a placeholder for schema validation
    for (const auto& entry : pImpl->entries) {
        // Example validation: check string length for certain keys
        if (entry.key.find("password") != std::string::npos) {
            if (std::holds_alternative<std::string>(entry.value)) {
                const std::string& pass = std::get<std::string>(entry.value);
                if (pass.size() < 8) {
                    Logger::getGlobal().warning("Configuration validation warning: Password key '" + entry.key + 
                                               "' has weak length: " + std::to_string(pass.size()) + " characters");
                }
            }
        }
    }
    return true;
}

bool Config::validateWithSchema(const std::string& schemaFile) const {
    // Placeholder for schema validation
    // In a real implementation, this would load and validate against a JSON Schema
    Logger::getGlobal().warning("Schema validation not yet implemented for: " + schemaFile);
    return true;
}

bool Config::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-') start = 1;
    
    bool hasDecimal = false;
    for (size_t i = start; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal) return false; // Multiple decimals
            hasDecimal = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> Config::splitKey(const std::string& key) {
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = key.find('.');
    
    while (end != std::string::npos) {
        parts.push_back(key.substr(start, end - start));
        start = end + 1;
        end = key.find('.', start);
    }
    
    parts.push_back(key.substr(start));
    return parts;
}

std::string Config::joinKey(const std::vector<std::string>& parts) {
    if (parts.empty()) return "";
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += "." + parts[i];
    }
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
