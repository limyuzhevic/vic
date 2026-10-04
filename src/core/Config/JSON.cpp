#include "JSON.h"
#include <regex>
#include <algorithm>
#include <cctype>

namespace nlm {

std::vector<std::pair<std::string, std::string>> JSONParser::parseJSON(const std::string& json) {
    std::vector<std::pair<std::string, std::string>> result;
    
    if (!validateJSON(json)) {
        throw std::runtime_error("Invalid JSON format");
    }
    
    // Remove whitespace and newlines
    std::string clean;
    for (char c : json) {
        if (!std::isspace(c)) clean += c;
    }
    
    // Simple regex-based parsing for key=value pairs
    // This handles basic JSON objects like {"key": "value", "number": 42}
    std::regex pattern(R"("([^"\s]+)"\s*:\s*("[^"]*"|[^,}]+))");
    
    auto begin = std::sregex_iterator(clean.begin(), clean.end(), pattern);
    auto end = std::sregex_iterator();
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        if (match.size() >= 3) {
            std::string key = match[1].str();
            std::string value = match[2].str();
            
            // Remove quotes from value if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            result.emplace_back(key, value);
        }
    }
    
    return result;
}

std::vector<std::pair<std::string, std::string>> JSONParser::parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseJSONString(buffer.str());
}

std::vector<std::pair<std::string, std::string>> JSONParser::parseJSONString(const std::string& json) {
    // Handle both file and string inputs
    if (json.empty()) {
        return {};
    }
    
    // Remove whitespace and newlines
    std::string clean;
    for (char c : json) {
        if (!std::isspace(c)) clean += c;
    }
    
    // Handle different JSON formats
    std::vector<std::pair<std::string, std::string>> result;
    
    // Format 1: {"key": "value", "num": 123}
    if (clean.front() == '{' && clean.back() == '}') {
        std::string content = clean.substr(1, clean.size() - 2);
        return parseJSON(content);
    }
    
    // Format 2: simple key=value pairs
    std::regex pattern(R"("([^"\s]+)"\s*:\s*("[^"]*"|[^,}]+))");
    auto begin = std::sregex_iterator(content.begin(), content.end(), pattern);
    auto end = std::sregex_iterator();
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        if (match.size() >= 3) {
            std::string key = match[1].str();
            std::string value = match[2].str();
            
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            result.emplace_back(key, value);
        }
    }
    
    return result;
}

std::vector<std::pair<std::string, std::string>> JSONParser::parseYAML(const std::string& yaml) {
    std::vector<std::pair<std::string, std::string>> result;
    
    if (!validateYAML(yaml)) {
        throw std::runtime_error("Invalid YAML format");
    }
    
    // Simple YAML parsing for key: value format
    std::regex pattern(R"([^:\s]+\s*:\s*[^#]+)\s*(?:#.*)?");
    auto begin = std::sregex_iterator(yaml.begin(), yaml.end(), pattern);
    auto end = std::sregex_iterator();
    
    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        if (match.size() >= 1) {
            std::string line = match[0].str();
            
            // Remove leading/trailing whitespace
            line = trim(line);
            
            // Find the first colon separator
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                result.emplace_back(key, value);
            }
        }
    }
    
    return result;
}

std::vector<std::pair<std::string, std::string>> JSONParser::parseYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseYAML(buffer.str());
}

std::string JSONParser::inferType(const std::string& value) {
    // Determine if value should be int, float, bool, or string
    // For configuration, we'll keep it simple and return the original string
    // The Config system will handle type conversion
    return value;
}

bool JSONParser::validateJSON(const std::string& json) {
    // Simple JSON validation - check basic structure
    if (json.empty()) return true;
    
    // Check for matching braces
    int braceCount = 0;
    bool inString = false;
    bool escaped = false;
    
    for (char c : json) {
        if (!escaped && c == '\\') {
            escaped = true;
            continue;
        }
        
        if (!escaped) {
            if (c == '"') inString = !inString;
            else if (!inString) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
            }
        }
        
        escaped = false;
    }
    
    return braceCount == 0 && !inString;
}

bool JSONParser::validateYAML(const std::string& yaml) {
    // Simple YAML validation - check for basic key: value format
    if (yaml.empty()) return true;
    
    // Basic check for line-based structure
    std::istringstream stream(yaml);
    std::string line;
    bool hasValidPair = false;
    
    while (std::getline(stream, line)) {
        // Skip comments and empty lines
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        if (trim(line).empty()) continue;
        
        // Check for key: value format
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos || colonPos == 0) {
            return false;
        }
        
        hasValidPair = true;
    }
    
    return hasValidPair;
}

std::string JSONSerializer::toJSON(const std::vector<std::pair<std::string, std::string>>& entries) {
    std::stringstream json;
    json << "{\n";
    
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        json << "  \"" << entry.first << "\": \"" << escapeString(entry.second) << "\"";
        if (i < entries.size() - 1) {
            json << ",\n";
        } else {
            json << "\n";
        }
    }
    
    json << "}";
    return json.str();
}

std::string JSONSerializer::toYAML(const std::vector<std::pair<std::string, std::string>>& entries) {
    std::stringstream yaml;
    
    for (const auto& entry : entries) {
        yaml << entry.first << ": " << entry.second << "\n";
    }
    
    return yaml.str();
}

bool JSONSerializer::toJSONFile(const std::string& filepath, const std::vector<std::pair<std::string, std::string>>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJSON(entries);
    return true;
}

bool JSONSerializer::toYAMLFile(const std::string& filepath, const std::vector<std::pair<std::string, std::string>>& entries) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toYAML(entries);
    return true;
}

std::string JSONSerializer::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

} // namespace nlm
