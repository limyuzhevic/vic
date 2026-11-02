#include "JsonParser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace nlm {

bool JsonParser::parseJson(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (jsonStr[pos] != '{') {
        return false;
    }
    
    pos++; // Skip {
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        // Parse key
        skipWhitespace(jsonStr, pos);
        if (jsonStr[pos] != '"') {
            return false;
        }
        
        size_t keyStart = ++pos;
        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
            pos++;
        }
        if (pos >= jsonStr.size()) {
            return false;
        }
        
        std::string key = jsonStr.substr(keyStart, pos - keyStart);
        pos++; // Skip closing "
        
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            return false;
        }
        
        pos++; // Skip :
        skipWhitespace(jsonStr, pos);
        
        // Parse value
        std::optional<ConfigValue> value;
        
        if (jsonStr[pos] == '"') {
            value = extractStringValue(jsonStr, pos);
        } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
            value = extractBoolValue(jsonStr, pos);
        } else if (jsonStr[pos] == '-' || std::isdigit(jsonStr[pos])) {
            if (jsonStr.find('.', pos) != std::string::npos) {
                value = extractDoubleValue(jsonStr, pos);
            } else {
                // Check if it's int64
                size_t nextPos = pos;
                while (nextPos < jsonStr.size() && std::isdigit(jsonStr[nextPos])) {
                    nextPos++;
                }
                if (nextPos - pos > 19) { // Larger than 64-bit int
                    value = extractDoubleValue(jsonStr, pos);
                } else {
                    value = extractInt64Value(jsonStr, pos);
                }
            }
        } else if (jsonStr[pos] == '[') {
            // Try to detect array type
            size_t arrayStart = pos;
            auto intArray = extractArrayValue(jsonStr, pos);
            if (intArray.has_value()) {
                value = intArray;
            } else {
                pos = arrayStart;
                auto doubleArray = extractArrayValue(jsonStr, pos);
                if (doubleArray.has_value()) {
                    value = doubleArray;
                } else {
                    pos = arrayStart;
                    auto stringArray = extractStringArrayValue(jsonStr, pos);
                    if (stringArray.has_value()) {
                        value = *stringArray;
                    }
                }
            }
        } else {
            return false;
        }
        
        if (!value.has_value()) {
            return false;
        }
        
        entries.emplace_back(key, value.value(), ConfigSource::File);
        
        skipWhitespace(jsonStr, pos);
        
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip ,
            skipWhitespace(jsonStr, pos);
        }
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '}') {
        return false;
    }
    
    return true;
}

bool JsonParser::parseJsonFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseJson(buffer.str(), entries);
}

std::string JsonParser::entriesToJson(const std::vector<ConfigEntry>& entries) {
    std::ostringstream json;
    json << "{\n";
    
    for (size_t i = 0; i < entries.size(); ++i) {
        json << "  \"" << entries[i].key << "\": " << jsonValueToString(entries[i].value);
        
        if (i < entries.size() - 1) {
            json << ",\n";
        } else {
            json << "\n";
        }
    }
    
    json << "}\n";
    return json.str();
}

bool JsonParser::validateJson(const std::string& jsonStr) {
    try {
        std::vector<ConfigEntry> entries;
        return parseJson(jsonStr, entries);
    } catch (const std::exception&) {
        return false;
    }
}

std::optional<std::string> JsonParser::extractStringValue(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '"') {
        return std::nullopt;
    }
    
    pos++; // Skip opening "
    size_t start = pos;
    
    while (pos < jsonStr.size() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
            pos += 2; // Skip escaped character
        }
        pos++;
    }
    
    if (pos >= jsonStr.size()) {
        return std::nullopt;
    }
    
    std::string value = jsonStr.substr(start, pos - start);
    pos++; // Skip closing "
    
    return value;
}

std::optional<int> JsonParser::extractIntValue(const std::string& jsonStr, size_t& pos) {
    size_t start = pos;
    while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
        pos++;
    }
    
    if (start == pos) {
        return std::nullopt;
    }
    
    try {
        return std::stoi(jsonStr.substr(start, pos - start));
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<int64_t> JsonParser::extractInt64Value(const std::string& jsonStr, size_t& pos) {
    size_t start = pos;
    while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
        pos++;
    }
    
    if (start == pos) {
        return std::nullopt;
    }
    
    try {
        return std::stoll(jsonStr.substr(start, pos - start));
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<double> JsonParser::extractDoubleValue(const std::string& jsonStr, size_t& pos) {
    size_t start = pos;
    
    if (jsonStr[pos] == '-') {
        pos++;
    }
    
    while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
        pos++;
    }
    
    if (pos < jsonStr.size() && jsonStr[pos] == '.') {
        pos++; // Skip decimal point
        while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
            pos++;
        }
    }
    
    if (start == pos) {
        return std::nullopt;
    }
    
    try {
        return std::stod(jsonStr.substr(start, pos - start));
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<bool> JsonParser::extractBoolValue(const std::string& jsonStr, size_t& pos) {
    size_t start = pos;
    
    if (jsonStr.compare(pos, 4, "true") == 0) {
        pos += 4;
        return true;
    } else if (jsonStr.compare(pos, 5, "false") == 0) {
        pos += 5;
        return false;
    }
    
    return std::nullopt;
}

std::optional<std::vector<double>> JsonParser::extractArrayValue(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '[') {
        return std::nullopt;
    }
    
    pos++; // Skip opening [
    std::vector<double> result;
    
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != ']') {
        auto value = extractDoubleValue(jsonStr, pos);
        if (!value.has_value()) {
            return std::nullopt;
        }
        
        result.push_back(value.value());
        
        skipWhitespace(jsonStr, pos);
        
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip ,
            skipWhitespace(jsonStr, pos);
        }
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != ']') {
        return std::nullopt;
    }
    
    pos++; // Skip closing ]
    
    return result;
}

std::optional<std::vector<std::string>> JsonParser::extractStringArrayValue(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '[') {
        return std::nullopt;
    }
    
    pos++; // Skip opening [
    std::vector<std::string> result;
    
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != ']') {
        auto value = extractStringValue(jsonStr, pos);
        if (!value.has_value()) {
            return std::nullopt;
        }
        
        result.push_back(value.value());
        
        skipWhitespace(jsonStr, pos);
        
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip ,
            skipWhitespace(jsonStr, pos);
        }
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != ']') {
        return std::nullopt;
    }
    
    pos++; // Skip closing ]
    
    return result;
}

void JsonParser::skipWhitespace(const std::string& jsonStr, size_t& pos) {
    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
        pos++;
    }
}

char JsonParser::peekChar(const std::string& jsonStr, size_t pos) {
    if (pos < jsonStr.size()) {
        return jsonStr[pos];
    }
    return '\0';
}

char JsonParser::getChar(const std::string& jsonStr, size_t& pos) {
    if (pos < jsonStr.size()) {
        return jsonStr[pos++];
    }
    return '\0';
}

std::string JsonParser::jsonValueToString(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::string escaped = arg;
            // Simple JSON escaping
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\"");
                pos += 2;
            }
            pos = 0;
            while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\\");
                pos += 2;
            }
            return "\"" + escaped + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                result += std::to_string(arg[i]);
                if (i < arg.size() - 1) {
                    result += ", ";
                }
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                result += std::to_string(arg[i]);
                if (i < arg.size() - 1) {
                    result += ", ";
                }
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                std::string escaped = arg[i];
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                pos = 0;
                while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\\");
                    pos += 2;
                }
                result += "\"" + escaped + "\"";
                if (i < arg.size() - 1) {
                    result += ", ";
                }
            }
            result += "]";
            return result;
        } else {
            return "null";
        }
    }, value);
}

} // namespace nlm