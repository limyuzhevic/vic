// Simple JSON implementation for NLM
// This is a minimal JSON implementation to avoid external dependencies

#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace json {

// JSON value types
using JSONValue = std::variant<
    std::nullptr_t,
    bool,
    double,
    std::string,
    std::vector<JSONValue>,
    std::map<std::string, JSONValue>
>;

class JSON {
public:
    JSON() : data(nullptr) {}
    JSON(bool value) : data(value) {}
    JSON(double value) : data(value) {}
    JSON(const std::string& value) : data(value) {}
    JSON(const char* value) : data(std::string(value)) {}
    JSON(const std::vector<JSONValue>& value) : data(value) {}
    JSON(const std::map<std::string, JSONValue>& value) : data(value) {}
    
    // Parse JSON string
    static JSON parse(const std::string& jsonStr);
    
    // Convert to string
    std::string dump(int indent = 2) const;
    
    // Type checking
    bool isNull() const { return std::holds_alternative<std::nullptr_t>(data); }
    bool isBool() const { return std::holds_alternative<bool>(data); }
    bool isNumber() const { return std::holds_alternative<double>(data); }
    bool isString() const { return std::holds_alternative<std::string>(data); }
    bool isArray() const { return std::holds_alternative<std::vector<JSONValue>>(data); }
    bool isObject() const { return std::holds_alternative<std::map<std::string, JSONValue>>(data); }
    
    // Getters
    bool getBool() const { return std::get<bool>(data); }
    double getNumber() const { return std::get<double>(data); }
    const std::string& getString() const { return std::get<std::string>(data); }
    const std::vector<JSONValue>& getArray() const { return std::get<std::vector<JSONValue>>(data); }
    const std::map<std::string, JSONValue>& getObject() const { return std::get<std::map<std::string, JSONValue>>(data); }
    
    // Subscript operators
    JSONValue& operator[](const std::string& key);
    const JSONValue& operator[](const std::string& key) const;
    
    JSONValue& operator[](size_t index);
    const JSONValue& operator[](size_t index) const;
    
    // Check if key exists in object
    bool contains(const std::string& key) const;
    
    // Size
    size_t size() const;
    
    // Iterators
    std::map<std::string, JSONValue>::iterator begin();
    std::map<std::string, JSONValue>::iterator end();
    std::map<std::string, JSONValue>::const_iterator begin() const;
    std::map<std::string, JSONValue>::const_iterator end() const;

private:
    JSONValue data;
    
    // Helper functions for parsing and dumping
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string trim(const std::string& str);
    static bool isWhitespace(char c);
    static std::string parseString(const std::string& jsonStr, size_t& pos);
    static double parseNumber(const std::string& jsonStr, size_t& pos);
    static std::vector<JSONValue> parseArray(const std::string& jsonStr, size_t& pos);
    static std::map<std::string, JSONValue> parseObject(const std::string& jsonStr, size_t& pos);
    
    // Dumping helpers
    static void dumpValue(const JSONValue& value, std::ostringstream& oss, int indent, bool pretty);
    static void dumpString(const std::string& str, std::ostringstream& oss);
    static void dumpArray(const std::vector<JSONValue>& array, std::ostringstream& oss, int indent, bool pretty);
    static void dumpObject(const std::map<std::string, JSONValue>& object, std::ostringstream& oss, int indent, bool pretty);
};

// Global JSON null value
const JSON JSON_NULL;

// Helper functions for easy access
inline JSON parseJSON(const std::string& jsonStr) {
    return JSON::parse(jsonStr);
}

inline std::string toJSON(const std::string& str) {
    return "\"" + JSON::escapeString(str) + "\"";
}

} // namespace json
} // namespace nlm