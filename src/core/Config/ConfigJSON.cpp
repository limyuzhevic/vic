#include "ConfigJSON.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct ConfigJSON::Impl {
    std::string filepath;
    bool prettyPrint;
    bool validateSchema;
    std::string schemaFile;
    
    Impl() : prettyPrint(false), validateSchema(false) {}
};

ConfigJSON::ConfigJSON() : pImpl(std::make_unique<Impl>()) {}

ConfigJSON::~ConfigJSON() = default;

bool ConfigJSON::loadFromFile(const std::string& filepath) {
    pImpl->filepath = filepath;
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        
        // Parse JSON content
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Try to parse as JSON
        if (!parseJSON(content)) {
            throw std::runtime_error("Invalid JSON format in file: " + filepath);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Log error but don't crash
        return false;
    }
}

bool ConfigJSON::loadFromString(const std::string& jsonString) {
    try {
        if (!parseJSON(jsonString)) {
            throw std::runtime_error("Invalid JSON format in string");
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigJSON::saveToFile(const std::string& filepath) const {
    pImpl->filepath = filepath;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filepath);
        }
        
        // Generate JSON
        std::string json = generateJSON();
        
        file << json;
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::string ConfigJSON::getFilepath() const {
    return pImpl->filepath;
}

void ConfigJSON::setFilepath(const std::string& filepath) {
    pImpl->filepath = filepath;
}

bool ConfigJSON::setPrettyPrint(bool enable) {
    pImpl->prettyPrint = enable;
    return true;
}

bool ConfigJSON::setValidateSchema(bool enable, const std::string& schemaFile) {
    pImpl->validateSchema = enable;
    pImpl->schemaFile = schemaFile;
    return true;
}

std::string ConfigJSON::toString() const {
    return generateJSON();
}

std::string ConfigJSON::generateJSON() const {
    std::ostringstream oss;
    
    if (pImpl->prettyPrint) {
        oss << "{\n";
        bool first = true;
        for (const auto& entry : getKeys()) {
            if (!first) oss << ",\n";
            first = false;
            oss << "  \"" << entry << "\": ";
            
            auto value = getValue(entry);
            if (std::holds_alternative<int>(value)) {
                oss << std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                oss << std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                oss << std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                oss << (std::get<bool>(value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(value)) {
                oss << "\"" << escapeJSON(std::get<std::string>(value)) << "\"";
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                oss << "[";
                auto& vec = std::get<std::vector<int>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                oss << "[";
                auto& vec = std::get<std::vector<double>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                oss << "[";
                auto& vec = std::get<std::vector<std::string>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << escapeJSON(vec[i]) << "\"";
                }
                oss << "]";
            }
        }
        oss << "\n}";
    } else {
        oss << "{";
        bool first = true;
        for (const auto& entry : getKeys()) {
            if (!first) oss << ", ";
            first = false;
            oss << "\"" << entry << "\": ";
            
            auto value = getValue(entry);
            if (std::holds_alternative<int>(value)) {
                oss << std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                oss << std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                oss << std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                oss << (std::get<bool>(value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(value)) {
                oss << "\"" << escapeJSON(std::get<std::string>(value)) << "\"";
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                oss << "[";
                auto& vec = std::get<std::vector<int>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                oss << "[";
                auto& vec = std::get<std::vector<double>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                oss << "[";
                auto& vec = std::get<std::vector<std::string>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << escapeJSON(vec[i]) << "\"";
                }
                oss << "]";
            }
        }
        oss << "}";
    }
    
    return oss.str();
}

bool ConfigJSON::parseJSON(const std::string& json) {
    // Simple JSON parsing - in a real implementation, you'd use a proper JSON library
    // For now, we'll extract key-value pairs from a simple JSON-like format
    
    size_t pos = 0;
    bool inObject = false;
    bool inString = false;
    char quoteChar = 0;
    
    std::string currentKey;
    std::string currentValue;
    bool parsingKey = false;
    bool parsingValue = false;
    
    while (pos < json.size()) {
        char c = json[pos];
        
        switch (c) {
            case '{':
                if (!inString) {
                    inObject = true;
                } else {
                    currentValue += c;
                }
                break;
                
            case '}':
                if (!inString) {
                    inObject = false;
                    if (!currentKey.empty()) {
                        // Store the last key-value pair
                        storeParsedValue(currentKey, currentValue);
                    }
                } else {
                    currentValue += c;
                }
                break;
                
            case '"':
                if (!inString) {
                    inString = true;
                    quoteChar = c;
                } else if (quoteChar == c) {
                    inString = false;
                    quoteChar = 0;
                    
                    if (!parsingKey) {
                        parsingKey = true;
                        parsingValue = false;
                    } else {
                        // We've finished parsing a value
                        storeParsedValue(currentKey, currentValue);
                        currentKey.clear();
                        currentValue.clear();
                        parsingKey = false;
                        parsingValue = false;
                    }
                } else {
                    currentValue += c;
                }
                break;
                
            case ':':
                if (!inString && parsingKey) {
                    parsingKey = false;
                    parsingValue = true;
                } else {
                    currentValue += c;
                }
                break;
                
            case ',':
                if (!inString && parsingValue) {
                    storeParsedValue(currentKey, currentValue);
                    currentKey.clear();
                    currentValue.clear();
                    parsingKey = false;
                    parsingValue = false;
                } else {
                    currentValue += c;
                }
                break;
                
            default:
                if (inString) {
                    if (parsingKey) {
                        currentKey += c;
                    } else if (parsingValue) {
                        currentValue += c;
                    }
                } else {
                    currentValue += c;
                }
                break;
        }
        
        pos++;
    }
    
    return true;
}

void ConfigJSON::storeParsedValue(const std::string& key, const std::string& valueStr) {
    // Convert string value to appropriate type
    if (valueStr == "true" || valueStr == "false") {
        bool boolVal = (valueStr == "true");
        set(key, boolVal, ConfigSource::File);
    } else {
        // Try to parse as number
        try {
            // Try integer first
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intVal, ConfigSource::File);
                return;
            }
            
            // Try double
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleVal, ConfigSource::File);
                return;
            }
            
            // Fall back to string
            set(key, valueStr, ConfigSource::File);
            
        } catch (const std::exception&) {
            // Fall back to string
            set(key, valueStr, ConfigSource::File);
        }
    }
}

std::string ConfigJSON::escapeJSON(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    oss << c;
                }
                break;
        }
    }
    return oss.str();
}

} // namespace nlm
