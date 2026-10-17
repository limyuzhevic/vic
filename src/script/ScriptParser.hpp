// NLM Script Parser
// Handles parsing of various script formats (JSON, YAML, simple)

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Script AST node types
struct ScriptNode {
    virtual ~ScriptNode() {}
    virtual std::string toString() const = 0;
};

struct ScriptObjectNode : public ScriptNode {
    std::map<std::string, std::unique_ptr<ScriptNode>> properties;
    
    std::string toString() const override {
        std::string result = "{\n";
        for (const auto& pair : properties) {
            result += "  \"" + pair.first + "\": " + pair.second->toString() + ",\n";
        }
        result += "}";
        return result;
    }
};

struct ScriptArrayNode : public ScriptNode {
    std::vector<std::unique_ptr<ScriptNode>> elements;
    
    std::string toString() const override {
        std::string result = "[\n";
        for (const auto& element : elements) {
            result += "  " + element->toString() + ",\n";
        }
        result += "]";
        return result;
    }
};

struct ScriptStringNode : public ScriptNode {
    std::string value;
    
    ScriptStringNode(const std::string& v) : value(v) {}
    
    std::string toString() const override {
        return "\"" + value + "\"";
    }
};

struct ScriptNumberNode : public ScriptNode {
    double value;
    
    ScriptNumberNode(double v) : value(v) {}
    
    std::string toString() const override {
        return std::to_string(value);
    }
};

struct ScriptBooleanNode : public ScriptNode {
    bool value;
    
    ScriptBooleanNode(bool v) : value(v) {}
    
    std::string toString() const override {
        return value ? "true" : "false";
    }
};

struct ScriptNullNode : public ScriptNode {
    std::string toString() const override {
        return "null";
    }
};

struct ScriptArrayIndexNode : public ScriptNode {
    std::string arrayName;
    size_t index;
    
    ScriptArrayIndexNode(const std::string& name, size_t idx) : arrayName(name), index(idx) {}
    
    std::string toString() const override {
        return arrayName + "[" + std::to_string(index) + "]";
    }
};

struct ScriptPropertyAccessNode : public ScriptNode {
    std::string objectName;
    std::string propertyName;
    
    ScriptPropertyAccessNode(const std::string& obj, const std::string& prop) : objectName(obj), propertyName(prop) {}
    
    std::string toString() const override {
        return objectName + "." + propertyName;
    }
};

// Parse result containing AST and metadata
struct ParseResult {
    std::unique_ptr<ScriptNode> ast;
    std::string format;  // json, yaml, simple
    bool valid = false;
    std::string errorMessage;
    std::map<std::string, std::string> metadata;
    
    ParseResult() {}
    ParseResult(std::unique_ptr<ScriptNode> node, const std::string& fmt) 
        : ast(std::move(node)), format(fmt), valid(true) {}
};

// Main script parser class
class ScriptParser {
public:
    ScriptParser();
    ~ScriptParser();
    
    // Parse script from file
    ParseResult parseFile(const std::string& filename);
    
    // Parse script from string content
    ParseResult parseString(const std::string& content, const std::string& format = "auto");
    
    // Parse JSON format
    ParseResult parseJSON(const std::string& jsonStr);
    
    // Parse YAML format
    ParseResult parseYAML(const std::string& yamlStr);
    
    // Parse simple key-value format
    ParseResult parseSimple(const std::string& simpleStr);
    
    // Format detection
    std::string detectFormat(const std::string& content);
    
    // Validation
    bool validateParseResult(const ParseResult& result);
    
    // Get parser statistics
    struct ParserStats {
        size_t nodesParsed = 0;
        size_t objectsParsed = 0;
        size_t arraysParsed = 0;
        double parseTime = 0.0;
    };
    
    ParserStats getStats() const { return stats; }
    
    // Clear statistics
    void resetStats() { stats = ParserStats(); }
    
    // Static parser instance
    static ScriptParser& getInstance() {
        static ScriptParser instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ParserStats stats;
    
    // Internal parsing methods
    std::unique_ptr<ScriptObjectNode> parseJSONObject(const std::string& jsonStr);
    std::unique_ptr<ScriptArrayNode> parseJSONArray(const std::string& jsonStr);
    std::unique_ptr<ScriptNode> parseJSONValue(const std::string& jsonStr, size_t& pos);
    std::string parseJSONString(const std::string& str, size_t& pos);
    double parseJSONNumber(const std::string& str, size_t& pos);
    bool parseJSONBoolean(const std::string& str, size_t& pos);
    void skipWhitespace(const std::string& str, size_t& pos);
    
    // YAML parsing
    std::unique_ptr<ScriptNode> parseYAMLNode(const std::string& yamlStr, size_t& pos);
    std::unique_ptr<ScriptObjectNode> parseYAMLObject(const std::string& yamlStr, size_t& pos);
    std::unique_ptr<ScriptArrayNode> parseYAMLArray(const std::string& yamlStr, size_t& pos);
    std::string parseYAMLString(const std::string& str, size_t& pos);
    
    // Error handling
    void reportError(size_t line, size_t column, const std::string& message);
    
    // JSON helper functions
    static bool isDigit(char c) { return c >= '0' && c <= '9'; }
    static bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
    static bool isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
    static bool isJSONChar(char c) { return isAlpha(c) || isDigit(c) || c == '_' || c == '-' || c == '.'; }
};

} // namespace nlm