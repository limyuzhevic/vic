// JSON Parser Implementation for NLM Configuration System
// Provides JSON and YAML support with validation and error handling

#ifndef SRC_CORE_CONFIG_JSON_PARSER_H
#define SRC_CORE_CONFIG_JSON_PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "../ErrorHandling.hpp"

namespace nlm {

// JSON Token Types
enum class JSONTokenType {
    OBJECT_START,
    OBJECT_END,
    ARRAY_START,
    ARRAY_END,
    STRING,
    NUMBER,
    BOOLEAN,
    NULL_VAL,
    COLON,
    COMMA,
    END_OF_FILE
};

// JSON Value Types (matching ConfigValue)
using JSONValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

class JSONParser {
public:
    // Parse JSON from file
    static JSONValue parseFile(const std::string& filepath);
    
    // Parse JSON from string
    static JSONValue parseString(const std::string& jsonStr);
    
    // Parse YAML from file (simple YAML support)
    static JSONValue parseYAMLFile(const std::string& filepath);
    
    // Convert JSON to Config format
    static void convertToConfig(const JSONValue& jsonValue, std::vector<ConfigEntry>& entries);
    
    // Get required configuration values with validation
    static std::string getStringValue(const JSONValue& value, const std::string& path);
    static int getIntValue(const JSONValue& value, const std::string& path);
    static double getDoubleValue(const JSONValue& value, const std::string& path);
    static bool getBoolValue(const JSONValue& value, const std::string& path);
    
    // Configuration schema validation
    static void validateConfig(const std::vector<ConfigEntry>& entries);
    
    // Default configuration schema
    static std::vector<std::string> getRequiredKeys();
    static std::vector<std::string> getOptionalKeys();
    
private:
    // Tokenizer implementation
    class Tokenizer {
    public:
        Tokenizer(const std::string& input);
        JSONTokenType nextToken();
        JSONTokenType currentToken() const { return currentToken_; }
        const std::string& currentValue() const { return currentValue_; }
        size_t getLine() const { return line; }
        size_t getColumn() const { return column; }
    
    private:
        void skipWhitespace();
        void parseString();
        void parseNumber();
        void parseLiteral(const std::string& literal);
    
        std::string input;
        size_t position;
        size_t line;
        size_t column;
        JSONTokenType currentToken_;
        std::string currentValue_;
    };
    
    // Parser implementation
    class Parser {
    public:
        Parser(Tokenizer& tokenizer);
        JSONValue parseValue();
    
    private:
        JSONValue parseObject();
        JSONValue parseArray();
        JSONValue parseLiteral();
        JSONValue parseStringValue();
        JSONValue parseNumberValue(const std::string& numStr);
        JSONValue parseBoolValue(const std::string& boolStr);
        
        Tokenizer& tokenizer;
    };
    
    // Helper functions
    static bool isWhitespace(char c);
    static bool isDigit(char c);
    static bool isAlpha(char c);
    static bool isAlphanumeric(char c);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::string parseNumber(const std::string& str, size_t& pos);
    static std::string unescapeString(const std::string& str);
};

} // namespace nlm

#endif // SRC_CORE_CONFIG_JSON_PARSER_H