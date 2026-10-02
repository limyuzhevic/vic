#ifndef NLM_JSON_PARSER_H
#define NLM_JSON_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <stdexcept>
#include <optional>

namespace nlm {

// Simple JSON parser for NLM configuration
class JSONParser {
public:
    // Parse JSON string to ConfigValue
    static bool parse(const std::string& json, ConfigValue& outValue);
    
    // Parse JSON file to ConfigValue  
    static bool parseFile(const std::string& filepath, ConfigValue& outValue);
    
    // Convert ConfigValue to JSON string
    static std::string toJSON(const ConfigValue& value);
    
    // Convert ConfigValue to pretty JSON string
    static std::string toJSONPretty(const ConfigValue& value);
    
    // Validate JSON syntax
    static bool validateJSON(const std::string& json);

private:
    // Helper classes for parsing
    class JSONTokenizer {
    public:
        JSONTokenizer(const std::string& json);
        
        enum TokenType {
            T_OBJECT_BEGIN,
            T_OBJECT_END,
            T_ARRAY_BEGIN,
            T_ARRAY_END,
            T_STRING,
            T_NUMBER,
            T_BOOL_TRUE,
            T_BOOL_FALSE,
            T_NULL,
            T_COMMA,
            T_COLON,
            T_EOF,
            T_ERROR
        };
        
        struct Token {
            TokenType type;
            std::string value;
            size_t line, column;
        };
        
        Token nextToken();
        void skipWhitespace();
        
    private:
        std::string json;
        size_t pos;
        size_t line, column;
        
        char peek() const;
        char advance();
        void skipLineComment();
        void skipBlockComment();
    };
    
    // Recursive descent parsing
    static bool parseValue(JSONTokenizer& tokenizer, ConfigValue& outValue);
    static bool parseObject(JSONTokenizer& tokenizer, ConfigValue& outValue);
    static bool parseArray(JSONTokenizer& tokenizer, ConfigValue& outValue);
    static bool parseString(JSONTokenizer& tokenizer, std::string& outString);
    static bool parseNumber(JSONTokenizer& tokenizer, ConfigValue& outValue);
    static bool parseBool(JSONTokenizer& tokenizer, ConfigValue& outValue);
    static bool parseNull(JSONTokenizer& tokenizer, ConfigValue& outValue);
    
    // Conversion utilities
    static std::string escapeString(const std::string& str);
    static bool isNumber(const std::string& str, double& outValue);
    static bool isInteger(const std::string& str, long long& outValue);
};

} // namespace nlm

#endif // NLM_JSON_PARSER_H