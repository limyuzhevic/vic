// Simple JSON parser for NLM configuration
// This provides basic JSON parsing for configuration files without requiring external dependencies

#pragma once

#include <string>
#include <map>
#include <vector>
#include <cstdint>

namespace nlm {

// Simple JSON value type for parsing configuration files
struct JsonValue {
    enum Type { OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NULLVAL };
    Type type;
    
    // Union for values
    std::string stringValue;
    double numberValue;
    bool boolValue;
    std::map<std::string, JsonValue> objectValue;
    std::vector<JsonValue> arrayValue;
    
    JsonValue() : type(NULLVAL) {}
    JsonValue(const std::string& s) : type(STRING), stringValue(s) {}
    JsonValue(double n) : type(NUMBER), numberValue(n) {}
    JsonValue(bool b) : type(BOOLEAN), boolValue(b) {}
    JsonValue(const std::map<std::string, JsonValue>& o) : type(OBJECT), objectValue(o) {}
    JsonValue(const std::vector<JsonValue>& a) : type(ARRAY), arrayValue(a) {}
};

// Simple JSON parser
class JsonParser {
public:
    static bool parse(const std::string& json, JsonValue& result);
    static std::string unescapeString(const std::string& str);
    static std::string trim(const std::string& str);
};

} // namespace nlm