#include "ConfigAdvanced.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <ctime>
#include <iomanip>
#include <regex>
#include <chrono>
#include <cstring>

namespace nlm {

// Helper functions
namespace ConfigHelpers {
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }
    
    bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            return false;
        }
        
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        
        // Remove quotes
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return !key.empty();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.rfind('.');
        if (pos == std::string::npos) return "";
        return filename.substr(pos + 1);
    }
    
    FileFormat detectFormat(const std::string& content) {
        if (content.empty()) return FileFormat::KeyValue;
        
        // Check for JSON - look for object or array
        if ((content.find('{') != std::string::npos && content.find('}') != std::string::npos) ||
            (content.find('[') != std::string::npos && content.find(']') != std::string::npos)) {
            return FileFormat::JSON;
        }
        
        // Check for YAML - look for key: value patterns with indentation
        size_t colonPos = content.find(": ");
        if (colonPos != std::string::npos && content.find("\n  ") != std::string::npos) {
            return FileFormat::YAML;
        }
        
        // Check for XML - look for tags
        if (content.find('<') != std::string::npos && content.find('>') != std::string::npos) {
            return FileFormat::XML;
        }
        
        // Check for TOML - look for key = value patterns
        size_t eqPos = content.find(" = ");
        if (eqPos != std::string::npos && content.find('"') != std::string::npos) {
            return FileFormat::TOML;
        }
        
        // Check for INI - look for [section] patterns
        if (content.find('[') != std::string::npos && content.find(']') != std::string::npos) {
            return FileFormat::INI;
        }
        
        return FileFormat::KeyValue;
    }
    
    std::string escapeXML(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '&': result += "&amp;"; break;
                case '<': result += "&lt;"; break;
                case '>': result += "&gt;"; break;
                case '"': result += "&quot;"; break;
                case '\'': result += "&apos;"; break;
                default: result += c; break;
            }
        }
        return result;
    }
    
    std::string escapeYAML(const std::string& str) {
        if (str.find(':') != std::string::npos || str.find('-') != std::string::npos ||
            str.find('"') != std::string::npos || str.find("'") != std::string::npos) {
            // Quote if contains special characters
            std::string result = "\"";
            for (char c : str) {
                if (c == '\\' || c == '\"') result += "\\";
                result += c;
            }
            result += "\"";
            return result;
        }
        return str;
    }
    
    std::string escapeTOML(const std::string& str) {
        if (str.find('"') != std::string::npos || str.find("'") != std::string::npos) {
            // Use single quotes for TOML
            std::string result = "'";
            for (char c : str) {
                if (c == '\'') result += "''";
                else result += c;
            }
            result += "'";
            return result;
        }
        return str;
    }
    
    std::string escapeINI(const std::string& str) {
        // For INI, values might need escaping
        return str;
    }
    
    std::string getTypeString(const ConfigValue& value) {
        std::string typeName;
        std::visit([&typeName](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) typeName = "int";
            else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
            else if constexpr (std::is_same_v<T, double>) typeName = "double";
            else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
            else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "int[]";
            else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "double[]";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "string[]";
            else if constexpr (std::is_same_v<T, std::vector<bool>>) typeName = "bool[]";
            else typeName = "unknown";
        }, value);
        return typeName;
    }
    
    std::string toString(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << arg[i] << "\"";
                }
                ss << "]";
            } else {
                ss << arg;
            }
        }, value);
        return ss.str();
    }
}

// JSON serialization
namespace JsonSerializer {
    std::string serialize(const Config& config) {
        std::stringstream ss;
        ss << "{\n";
        
        for (size_t i = 0; i < config.pImpl->entries.size(); ++i) {
            const auto& entry = config.pImpl->entries[i];
            ss << "  \"" << ConfigHelpers::escapeJSON(entry.key) << "\": " << JsonSerializer::serializeValue(entry.value);
            if (i < config.pImpl->entries.size() - 1) ss << ",\n";
        }
        ss << "\n}\n";
        
        return ss.str();
    }
    
    std::string serializeValue(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << ConfigHelpers::escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << ConfigHelpers::escapeJSON(arg[i]) << "\"";
                }
                ss << "]";
            } else {
                ss << arg;
            }
        }, value);
        return ss.str();
    }
    
    bool deserialize(const std::string& content, Config& config) {
        if (content.empty() || content[0] != '{') {
            return false;
        }
        
        // Simple JSON parsing - would use a proper JSON library in production
        config.clear();
        
        // Skip opening brace
        size_t pos = content.find('{') + 1;
        while (pos < content.size()) {
            size_t keyEnd = content.find('"', pos);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(pos, keyEnd - pos);
            pos = content.find('"', keyEnd + 1);
            if (pos == std::string::npos) break;
            
            pos = content.find(':', pos);
            if (pos == std::string::npos) break;
            
            pos = content.find_first_not_of(' ', pos + 1);
            if (pos >= content.size()) break;
            
            size_t valueEnd;
            if (content[pos] == '"') {
                // String value
                valueEnd = content.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                
                std::string value = content.substr(pos + 1, valueEnd - pos - 1);
                config.set(key, value, ConfigSource::File);
                pos = content.find(',', valueEnd);
            } else {
                // Number or boolean value
                size_t nextComma = content.find(',', pos);
                size_t nextBrace = content.find('}', pos);
                
                if (nextComma == std::string::npos && nextBrace == std::string::npos) break;
                
                size_t end = std::min(nextComma, nextBrace);
                std::string valueStr = content.substr(pos, end - pos);
                
                // Try to parse as different types
                try {
                    size_t parsedPos;
                    long long_val = std::stoll(valueStr, &parsedPos);
                    if (parsedPos == valueStr.size()) {
                        config.set(key, static_cast<int>(long_val), ConfigSource::File);
                    } else {
                        double double_val = std::stod(valueStr, &parsedPos);
                        if (parsedPos == valueStr.size()) {
                            config.set(key, double_val, ConfigSource::File);
                        } else {
                            bool bool_val = (valueStr == "true" || valueStr == "True" || valueStr == "1");
                            config.set(key, bool_val, ConfigSource::File);
                        }
                    }
                } catch (...) {
                    config.set(key, valueStr, ConfigSource::File);
                }
                
                pos = nextComma != std::string::npos ? nextComma : nextBrace;
            }
            
            if (pos < content.size() && content[pos] == ',') {
                pos++;
            }
        }
        
        return true;
    }
}

// YAML serialization
namespace YamlSerializer {
    std::string serialize(const Config& config) {
        std::stringstream ss;
        ss << "# Configuration file\n\n";
        
        for (const auto& entry : config.pImpl->entries) {
            if (entry.key == "config_version") continue; // Skip version for YAML
            
            ss << """  """ << ConfigHelpers::escapeYAML(entry.key) << ": " << YamlSerializer::serializeValue(entry.value) << "\n";
        }
        
        return ss.str();
    }
    
    std::string serializeValue(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << ConfigHelpers::escapeYAML(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << ConfigHelpers::escapeYAML(arg[i]);
                }
                ss << "]";
            } else {
                ss << arg;
            }
        }, value);
        return ss.str();
    }
    
    bool deserialize(const std::string& content, Config& config) {
        if (content.empty()) return false;
        
        config.clear();
        
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = ConfigHelpers::trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            // Parse key: value
            size_t colonPos = line.find(": ");
            if (colonPos == std::string::npos) continue;
            
            std::string key = ConfigHelpers::trim(line.substr(0, colonPos));
            std::string value = ConfigHelpers::trim(line.substr(colonPos + 2));
            
            // Remove quotes
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to detect type and set
            try {
                if (value == "true" || value == "false") {
                    config.set(key, value == "true", ConfigSource::File);
                } else {
                    size_t pos;
                    long long_val = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        config.set(key, static_cast<int>(long_val), ConfigSource::File);
                    } else {
                        double double_val = std::stod(value, &pos);
                        if (pos == value.size()) {
                            config.set(key, double_val, ConfigSource::File);
                        } else {
                            config.set(key, value, ConfigSource::File);
                        }
                    }
                }
            } catch (...) {
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
}

// XML serialization
namespace XmlSerializer {
    std::string serialize(const Config& config) {
        std::stringstream ss;
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        ss << "<configuration>\n";
        
        for (const auto& entry : config.pImpl->entries) {
            ss << "  <entry key=\"" << ConfigHelpers::escapeXML(entry.key) << "\" type=\"" << ConfigHelpers::getTypeString(entry.value) << "\">\n";
            ss << "    <value>" << ConfigHelpers::escapeXML(ConfigHelpers::toString(entry.value)) << "</value>\n";
            ss << "    <source>" << static_cast<int>(entry.source) << "</source>\n";
            if (!entry.description.empty()) {
                ss << "    <description>" << ConfigHelpers::escapeXML(entry.description) << "</description>\n";
            }
            ss << "  </entry>\n";
        }
        
        ss << "</configuration>\n";
        return ss.str();
    }
    
    bool deserialize(const std::string& content, Config& config) {
        if (content.empty() || content.find("<configuration>") == std::string::npos) {
            return false;
        }
        
        config.clear();
        
        // Simple XML parsing - would use a proper XML library in production
        size_t pos = content.find("<entry ");
        while (pos != std::string::npos && pos < content.size()) {
            // Extract key attribute
            size_t keyStart = content.find('"', pos + 6);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Extract value
            size_t valueStart = content.find("<value>", pos);
            size_t valueEnd = content.find("</value>", pos);
            if (valueStart == std::string::npos || valueEnd == std::string::npos) break;
            
            std::string valueStr = content.substr(valueStart + 7, valueEnd - valueStart - 7);
            
            // Parse value back to appropriate type
            try {
                if (valueStr == "true" || valueStr == "false") {
                    config.set(key, valueStr == "true", ConfigSource::File);
                } else {
                    size_t parsedPos;
                    long long_val = std::stoll(valueStr, &parsedPos);
                    if (parsedPos == valueStr.size()) {
                        config.set(key, static_cast<int>(long_val), ConfigSource::File);
                    } else {
                        double double_val = std::stod(valueStr, &parsedPos);
                        if (parsedPos == valueStr.size()) {
                            config.set(key, double_val, ConfigSource::File);
                        } else {
                            config.set(key, valueStr, ConfigSource::File);
                        }
                    }
                }
            } catch (...) {
                config.set(key, valueStr, ConfigSource::File);
            }
            
            pos = content.find("<entry ", valueEnd);
        }
        
        return true;
    }
}

// TOML serialization
namespace TomlSerializer {
    std::string serialize(const Config& config) {
        std::stringstream ss;
        ss << "# Configuration file\n\n";
        
        for (const auto& entry : config.pImpl->entries) {
            ss << """  """ << ConfigHelpers::escapeTOML(entry.key) << " = " << TomlSerializer::serializeValue(entry.value) << "\n";
        }
        
        return ss.str();
    }
    
    std::string serializeValue(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << ConfigHelpers::escapeTOML(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << ConfigHelpers::escapeTOML(arg[i]);
                }
                ss << "]";
            } else {
                ss << arg;
            }
        }, value);
        return ss.str();
    }
    
    bool deserialize(const std::string& content, Config& config) {
        if (content.empty()) return false;
        
        config.clear();
        
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = ConfigHelpers::trim(line);
            if (line.empty() || line[0] == '#') continue;
            
            // Parse key = value
            size_t eqPos = line.find(" = ");
            if (eqPos == std::string::npos) continue;
            
            std::string key = ConfigHelpers::trim(line.substr(0, eqPos));
            std::string value = ConfigHelpers::trim(line.substr(eqPos + 3));
            
            // Remove quotes
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value
            try {
                if (value == "true" || value == "false") {
                    config.set(key, value == "true", ConfigSource::File);
                } else {
                    size_t pos;
                    long long_val = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        config.set(key, static_cast<int>(long_val), ConfigSource::File);
                    } else {
                        double double_val = std::stod(value, &pos);
                        if (pos == value.size()) {
                            config.set(key, double_val, ConfigSource::File);
                        } else {
                            config.set(key, value, ConfigSource::File);
                        }
                    }
                }
            } catch (...) {
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
}

// INI serialization
namespace IniSerializer {
    std::string serialize(const Config& config) {
        std::stringstream ss;
        ss << "# Configuration file\n\n";
        
        for (const auto& entry : config.pImpl->entries) {
            // Group by category if present
            if (!entry.category.empty()) {
                ss << "[" << entry.category << "]\n";
            }
            
            ss << """  """ << entry.key << " = " << IniSerializer::serializeValue(entry.value) << "\n";
            
            if (!entry.description.empty()) {
                ss << "  ; " << entry.description << "\n";
            }
            
            ss << "\n";
        }
        
        return ss.str();
    }
    
    std::string serializeValue(const ConfigValue& value) {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << ConfigHelpers::escapeINI(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << ConfigHelpers::escapeINI(arg[i]);
                }
                ss << "]";
            } else {
                ss << arg;
            }
        }, value);
        return ss.str();
    }
    
    bool deserialize(const std::string& content, Config& config) {
        if (content.empty()) return false;
        
        config.clear();
        
        std::istringstream stream(content);
        std::string line;
        std::string currentCategory;
        
        while (std::getline(stream, line)) {
            line = ConfigHelpers::trim(line);
            if (line.empty() || line[0] == ';' || line[0] == '#') continue;
            
            // Check for section
            if (line[0] == '[' && line.back() == ']') {
                currentCategory = line.substr(1, line.size() - 2);
                continue;
            }
            
            // Parse key = value
            size_t eqPos = line.find(" = ");
            if (eqPos == std::string::npos) continue;
            
            std::string key = ConfigHelpers::trim(line.substr(0, eqPos));
            std::string value = ConfigHelpers::trim(line.substr(eqPos + 3));
            
            // Remove quotes
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse value
            try {
                if (value == "true" || value == "false") {
                    config.set(key, value == "true", ConfigSource::File, "", currentCategory);
                } else {
                    size_t pos;
                    long long_val = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        config.set(key, static_cast<int>(long_val), ConfigSource::File, "", currentCategory);
                    } else {
                        double double_val = std::stod(value, &pos);
                        if (pos == value.size()) {
                            config.set(key, double_val, ConfigSource::File, "", currentCategory);
                        } else {
                            config.set(key, value, ConfigSource::File, "", currentCategory);
                        }
                    }
                }
            } catch (...) {
                config.set(key, value, ConfigSource::File, "", currentCategory);
            }
        }
        
        return true;
    }
}

ConfigAdvanced::ConfigAdvanced() : pImpl(std::make_unique<Impl>()) {
    // Initialize with advanced defaults
    pImpl->historyEnabled = true;
    pImpl->environmentVariablesEnabled = true;
    pImpl->environmentPrefix = "NLM_";
    pImpl->hotReloadingEnabled = false;
    pImpl->reloadInterval = 5;
    pImpl->encrypted = false;
    pImpl->backupDir = "configs_backups";
    
    // Create backup directory
    std::filesystem::create_directories(pImpl->backupDir);
}

ConfigAdvanced::~ConfigAdvanced() = default;

ConfigAdvanced::ConfigAdvanced(ConfigAdvanced&& other) noexcept = default;
ConfigAdvanced& ConfigAdvanced::operator=(ConfigAdvanced&& other) noexcept = default;

bool ConfigAdvanced::loadFromFile(const std::string& filepath, FileFormat format) {
    return pImpl->parentConfig.loadFromFile(filepath, format);
}

bool ConfigAdvanced::loadFromString(const std::string& content, FileFormat format) {
    return pImpl->parentConfig.loadFromString(content, format);
}

bool ConfigAdvanced::saveToFile(const std::string& filepath, FileFormat format) const {
    return pImpl->parentConfig.saveToFile(filepath, format);
}

std::string ConfigAdvanced::saveToString(FileFormat format) const {
    return pImpl->parentConfig.saveToString(format);
}

bool ConfigAdvanced::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> ConfigAdvanced::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Basic validation for enhanced config
    for (const auto& entry : pImpl->parentConfig.pImpl->entries) {
        if (entry.key == "config_version") continue;
        
        // Type validation
        if (!validateValueType(entry)) {
            errors.push_back("Invalid type for key '" + entry.key + "'");
        }
        
        // Range validation for numeric values
        if (!validateValueRange(entry)) {
            errors.push_back("Invalid range for key '" + entry.key + "'");
        }
        
        // Dependency validation
        if (!validateDependencies(entry)) {
            errors.push_back("Dependency validation failed for key '" + entry.key + "'");
        }
    }
    
    return errors;
}

// Additional implementations for the advanced config...

std::string ConfigAdvanced::generateDocumentation() const {
    return pImpl->parentConfig.generateDocumentation();
}

std::string ConfigAdvanced::getAutocompleteSuggestions(const std::string& partialKey) const {
    return pImpl->parentConfig.getAutocompleteSuggestions(partialKey);
}

bool ConfigAdvanced::exportConfig(const std::string& outputPath, const std::vector<FileFormat>& formats) {
    bool success = true;
    
    for (const auto& format : formats) {
        std::string formatName;
        switch (format) {
            case FileFormat::JSON: formatName = ".json"; break;
            case FileFormat::YAML: formatName = ".yaml"; break;
            case FileFormat::XML: formatName = ".xml"; break;
            case FileFormat::TOML: formatName = ".toml"; break;
            case FileFormat::INI: formatName = ".ini"; break;
            default: formatName = ".txt"; break;
        }
        
        std::string filepath = outputPath + formatName;
        if (!saveToFile(filepath, format)) {
            success = false;
        }
    }
    
    return success;
}

bool ConfigAdvanced::importConfig(const std::string& inputPath, FileFormat format) {
    return loadFromFile(inputPath, format);
}

bool ConfigAdvanced::encryptConfig(const std::string& key) {
    pImpl->encryptionKey = key;
    pImpl->encrypted = true;
    return true;
}

bool ConfigAdvanced::decryptConfig(const std::string& key) {
    if (key != pImpl->encryptionKey) {
        return false;
    }
    pImpl->encryptionKey.clear();
    pImpl->encrypted = false;
    return true;
}

bool ConfigAdvanced::enableHotReloading(const std::string& filepath, FileFormat format, int interval) {
    if (!pImpl->parentConfig.loadFromFile(filepath, format)) {
        return false;
    }
    
    pImpl->hotReloadingEnabled = true;
    pImpl->autoReloadPath = filepath;
    pImpl->autoReloadFormat = format;
    pImpl->reloadInterval = interval;
    
    return true;
}

std::string ConfigAdvanced::getCurrentTimestamp() {
    return ConfigHelpers::getCurrentTimestamp();
}

bool ConfigAdvanced::backupConfig(const std::string& name) {
    std::string timestamp = ConfigHelpers::getCurrentTimestamp();
    std::string backupFile = pImpl->backupDir + "/" + name + "_" + timestamp + ".cfg";
    return saveToFile(backupFile, FileFormat::KeyValue);
}

bool ConfigAdvanced::restoreConfig(const std::string& backupFile) {
    return loadFromFile(backupFile);
}

std::vector<std::string> ConfigAdvanced::getBackupFiles() const {
    std::vector<std::string> backups;
    
    if (!std::filesystem::exists(pImpl->backupDir)) {
        return backups;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(pImpl->backupDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
            backups.push_back(entry.path().string());
        }
    }
    
    return backups;
}

bool ConfigAdvanced::validateValueType(const ConfigEntry& entry) const {
    // Implement type validation based on schema
    return true;
}

bool ConfigAdvanced::validateValueRange(const ConfigEntry& entry) const {
    // Implement range validation for numeric values
    return true;
}

bool ConfigAdvanced::validateDependencies(const ConfigEntry& entry) const {
    // Implement dependency validation
    return true;
}

} // namespace nlm