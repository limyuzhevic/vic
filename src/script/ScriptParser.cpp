// NLM Script Parser
// Handles parsing of various script formats (JSON, YAML, simple)

#include "ScriptParser.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

namespace nlm {

struct ScriptParser::Impl {
    std::string lastError;
    size_t currentLine = 1;
    size_t currentColumn = 0;
    double startTime;
    
    Impl() : startTime(std::chrono::system_clock::now().time_since_epoch().count() / 1000000.0) {}
};

ScriptParser::ScriptParser() : pImpl(std::make_unique<Impl>()) {}
ScriptParser::~ScriptParser() = default;

ParseResult ScriptParser::parseFile(const std::string& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        pImpl->lastError = "Cannot open file: " + filename;
        return ParseResult();
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    std::string content = buffer.str();
    return parseString(content, "auto");
}

ParseResult ScriptParser::parseString(const std::string& content, const std::string& format) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::string detectedFormat = (format == "auto") ? detectFormat(content) : format;
    
    ParseResult result;
    if (detectedFormat == "json") {
        result = parseJSON(content);
    } else if (detectedFormat == "yaml") {
        result = parseYAML(content);
    } else {
        result = parseSimple(content);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.parseTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

ParseResult ScriptParser::parseJSON(const std::string& jsonStr) {
    try {
        // Simplified JSON parsing
        std::string::size_type pos = 0;
        skipWhitespace(jsonStr, pos);
        
        auto node = parseJSONValue(jsonStr, pos);
        if (!node) {
            pImpl->lastError = "Invalid JSON format";
            return ParseResult();
        }
        
        // For now, only accept objects as root
        auto* objectNode = dynamic_cast<ScriptObjectNode*>(node.get());
        if (!objectNode) {
            pImpl->lastError = "JSON root must be an object";
            return ParseResult();
        }
        
        return ParseResult(std::move(node), "json");
        
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("JSON parsing error: ") + e.what();
        return ParseResult();
    }
}

ParseResult ScriptParser::parseYAML(const std::string& yamlStr) {
    try {
        // Simplified YAML parsing
        size_t pos = 0;
        auto node = parseYAMLNode(yamlStr, pos);
        if (!node) {
            pImpl->lastError = "Invalid YAML format";
            return ParseResult();
        }
        
        return ParseResult(std::move(node), "yaml");
        
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("YAML parsing error: ") + e.what();
        return ParseResult();
    }
}

ParseResult ScriptParser::parseSimple(const std::string& simpleStr) {
    try {
        // Simple key=value parsing
        auto node = std::make_unique<ScriptObjectNode>();
        
        std::istringstream iss(simpleStr);
        std::string line;
        while (std::getline(iss, line)) {
            // Remove comments
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            // Skip empty lines
            if (line.empty()) continue;
            
            // Parse key=value
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                
                // Try to parse as number
                bool isNumber = false;
                double numValue;
                try {
                    size_t pos;
                    numValue = std::stod(value, pos);
                    if (pos == value.length()) {
                        isNumber = true;
                    }
                } catch (...) {}
                
                if (isNumber) {
                    node->properties[key] = std::make_unique<ScriptNumberNode>(numValue);
                } else {
                    // Check for boolean
                    if (value == "true") {
                        node->properties[key] = std::make_unique<ScriptBooleanNode>(true);
                    } else if (value == "false") {
                        node->properties[key] = std::make_unique<ScriptBooleanNode>(false);
                    } else {
                        node->properties[key] = std::make_unique<ScriptStringNode>(value);
                    }
                }
                
                stats.objectsParsed++;
            }
        }
        
        return ParseResult(std::move(node), "simple");
        
    } catch (const std::exception& e) {
        pImpl->lastError = std::string("Simple parsing error: ") + e.what();
        return ParseResult();
    }
}

std::string ScriptParser::detectFormat(const std::string& content) {
    if (content.empty()) return "unknown";
    
    // Check for JSON indicators
    if (content.find("{[") != std::string::npos || content.find("{\n") != std::string::npos) {
        return "json";
    }
    
    // Check for YAML indicators
    if (content.find(": ") != std::string::npos || content.find("\n  ") != std::string::npos) {
        return "yaml";
    }
    
    // Default to simple
    return "simple";
}

bool ScriptParser::validateParseResult(const ParseResult& result) {
    // Basic validation - ensure AST is not empty
    return result.valid && result.ast != nullptr;
}

// Helper function implementations
void ScriptParser::skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.length() && isWhitespace(str[pos])) {
        if (str[pos] == '\n') {
            pImpl->currentLine++;
            pImpl->currentColumn = 0;
        } else {
            pImpl->currentColumn++;
        }
        pos++;
    }
}

void ScriptParser::reportError(size_t line, size_t column, const std::string& message) {
    pImpl->lastError = "Line " + std::to_string(line) + ", Column " + std::to_string(column) + ": " + message;
}

} // namespace nlm