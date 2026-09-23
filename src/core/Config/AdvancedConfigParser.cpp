#include "AdvancedConfigParser.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace nlm {

AdvancedConfigParser::ParsedConfig AdvancedConfigParser::parseFile(const std::string& filepath) {
    ParsedConfig parsed;
    parsed.filename = filepath;
    parsed.isValid = false;
    
    // Read file content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        parsed.errors.push_back("Cannot open file: " + filepath);
        return parsed;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Detect format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        parsed.format = "json";
    } else if (content.find('[') != std::string::npos && content.find(']') != std::string::npos) {
        parsed.format = "ini";
    } else {
        parsed.format = "key-value";
    }
    
    // Parse based on format
    bool parseSuccess = false;
    if (parsed.format == "json") {
        parseSuccess = parseJsonFormat(content, parsed);
    } else if (parsed.format == "ini") {
        parseSuccess = parseIniFormat(content, parsed);
    } else {
        parseSuccess = parseKeyValueFormat(content, parsed);
    }
    
    parsed.isValid = parseSuccess;
    if (!parseSuccess) {
        parsed.errors.push_back("Failed to parse " + parsed.format + " format");
    }
    
    return parsed;
}

std::string AdvancedConfigParser::formatForDisplay(const ParsedConfig& parsed) const {
    std::stringstream ss;
    ss << "=== Parsed Configuration ===" << std::endl;
    ss << "File: " << parsed.filename << std::endl;
    ss << "Format: " << parsed.format << std::endl;
    ss << "Status: " << (parsed.isValid ? "VALID" : "INVALID") << std::endl;
    
    if (!parsed.isValid && !parsed.errors.empty()) {
        ss << "Errors:" << std::endl;
        for (const auto& error : parsed.errors) {
            ss << "  - " << error << std::endl;
        }
    }
    
    ss << std::endl << "Key-Value Pairs:" << std::endl;
    for (const auto& pair : parsed.keyValuePairs) {
        ss << "  " << pair.first << " = " << pair.second << std::endl;
    }
    
    if (!parsed.comments.empty()) {
        ss << std::endl << "Comments:" << std::endl;
        for (const auto& comment : parsed.comments) {
            ss << "  # " << comment << std::endl;
        }
    }
    
    return ss.str();
}

std::string AdvancedConfigParser::formatForOutput(const ParsedConfig& parsed, const std::string& outputFormat) const {
    if (outputFormat == "json") {
        return formatAsJson(parsed);
    } else if (outputFormat == "ini") {
        return formatAsIni(parsed);
    } else {
        return formatAsKeyValue(parsed);
    }
}

std::string AdvancedConfigParser::formatAsJson(const ParsedConfig& parsed) const {
    std::stringstream ss;
    ss << "{\n";
    for (size_t i = 0; i < parsed.keyValuePairs.size(); ++i) {
        const auto& pair = parsed.keyValuePairs[i];
        ss << "  \"" << pair.first << "\": \"" << pair.second << "\"";
        if (i < parsed.keyValuePairs.size() - 1) {
            ss << ",";
        }
        ss << "\n";
    }
    ss << "}\n";
    return ss.str();
}

std::string AdvancedConfigParser::formatAsIni(const ParsedConfig& parsed) const {
    std::stringstream ss;
    for (const auto& pair : parsed.keyValuePairs) {
        ss << pair.first << " = " << pair.second << "\n";
    }
    return ss.str();
}

std::string AdvancedConfigParser::formatAsKeyValue(const ParsedConfig& parsed) const {
    std::stringstream ss;
    for (const auto& pair : parsed.keyValuePairs) {
        ss << pair.first << " = " << pair.second << "\n";
    }
    return ss.str();
}

bool AdvancedConfigParser::parseKeyValueFormat(const std::string& content, ParsedConfig& parsed) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Skip comments
        if (line.find("#") == 0 || line.find("//") == 0) {
            parsed.comments.push_back(line);
            continue;
        }
        
        // Parse key = value
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            parsed.errors.push_back("Line " + std::to_string(lineNumber) + ": Missing '='
");
            return false;
        }
        
        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));
        
        if (key.empty()) {
            parsed.errors.push_back("Line " + std::to_string(lineNumber) + ": Empty key");
            return false;
        }
        
        parsed.keyValuePairs.push_back({key, value});
    }
    
    return true;
}

bool AdvancedConfigParser::parseJsonFormat(const std::string& content, ParsedConfig& parsed) {
    // Simple JSON parsing for key-value pairs
    // This is a minimal implementation - for production use, consider using a proper JSON library
    try {
        std::istringstream stream(content);
        std::string token;
        bool inKey = true;
        std::string currentKey;
        std::string currentValue;
        char ch;
        
        while (stream.get(ch)) {
            if (ch == '\"') {
                std::string string;
                std::getline(stream, string, '\"');
                if (inKey) {
                    currentKey = string;
                } else {
                    currentValue = string;
                    parsed.keyValuePairs.push_back({currentKey, currentValue});
                }
                inKey = !inKey;
            } else if (ch == '{' || ch == '}' || ch == '[' || ch == ']') {
                // Skip structural characters
                continue;
            } else if (ch == ',') {
                // Continue to next key-value pair
                continue;
            }
        }
        
        return true;
    } catch (...) {
        parsed.errors.push_back("JSON parsing error");
        return false;
    }
}

bool AdvancedConfigParser::parseIniFormat(const std::string& content, ParsedConfig& parsed) {
    std::istringstream stream(content);
    std::string line;
    size_t sectionLine = 0;
    
    while (std::getline(stream, line)) {
        ++sectionLine;
        line = trim(line);
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Skip comments
        if (line.find("#") == 0 || line.find("//") == 0) {
            parsed.comments.push_back(line);
            continue;
        }
        
        // Parse section header
        if (line.find('[') == 0 && line.find(']') != std::string::npos) {
            continue; // Skip section headers for now
        }
        
        // Parse key = value
        size_t equalsPos = line.find('=');
        if (equalsPos == std::string::npos) {
            parsed.errors.push_back("Line " + std::to_string(sectionLine) + ": Missing '='");
            return false;
        }
        
        std::string key = trim(line.substr(0, equalsPos));
        std::string value = trim(line.substr(equalsPos + 1));
        
        if (key.empty()) {
            parsed.errors.push_back("Line " + std::to_string(sectionLine) + ": Empty key");
            return false;
        }
        
        parsed.keyValuePairs.push_back({key, value});
    }
    
    return true;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

} // namespace nlm
