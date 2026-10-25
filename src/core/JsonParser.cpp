#include "JsonParser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

std::string JsonParser::JsonValue::toString() const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<JsonValue>>) {
            oss << "[";
            bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << item.toString();
                first = false;
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool JsonParser::parseFile(const std::string& filepath, std::map<std::string, JsonValue>& jsonData) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Parse key-value pair
        size_t colonPos = line.find(":");
        if (colonPos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string valueStr = trim(line.substr(colonPos + 1));
        
        // Remove surrounding quotes
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '\"' && valueStr.back() == '\"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        auto value = parseValue(valueStr);
        if (value.has_value()) {
            jsonData[key] = value.value();
        }
    }
    
    return true;
}

std::optional<JsonParser::JsonValue> JsonParser::parseValue(const std::string& str) {
    // Try to parse as boolean
    if (str == "true") {
        return JsonValue{true};
    } else if (str == "false") {
        return JsonValue{false};
    }
    
    // Try to parse as number
    if (!str.empty() && (isdigit(str[0]) || (str[0] == '-' && str.size() > 1 && isdigit(str[1])))) {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            // Check if it fits in integer range
            try {
                int64_t intVal = std::stoll(str);
                return JsonValue{intVal};
            } catch (...) {
                return JsonValue{doubleVal};
            }
        }
    }
    
    // Try to parse as array
    if (!str.empty() && str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<JsonValue> array;
        size_t start = 0;
        size_t bracketDepth = 0;
        
        for (size_t i = 0; i < inner.size(); ++i) {
            if (inner[i] == '[') bracketDepth++;
            else if (inner[i] == ']') bracketDepth--;
            else if (inner[i] == ',' && bracketDepth == 0) {
                if (i > start) {
                    std::string item = inner.substr(start, i - start);
                    auto value = parseValue(trim(item));
                    if (value.has_value()) {
                        array.push_back(value.value());
                    }
                }
                start = i + 1;
            }
        }
        
        if (start < inner.size()) {
            std::string item = inner.substr(start);
            auto value = parseValue(trim(item));
            if (value.has_value()) {
                array.push_back(value.value());
            }
        }
        
        return JsonValue{array};
    }
    
    // Default to string
    return JsonValue{str};
}

std::string JsonParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool JsonParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

} // namespace nlm