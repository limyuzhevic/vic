#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace nlm {

// Simple JSON value types
using JsonValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<JsonValue>,
    std::map<std::string, JsonValue>
>;

class SimpleJsonParser {
public:
    static bool parseJson(const std::string& jsonStr, JsonValue& outValue);
    static bool parseJsonFile(const std::string& filepath, JsonValue& outValue);
    static std::string toJsonString(const JsonValue& value, int indent = 0);
    static bool fromJsonValue(const JsonValue& value, const std::string& key, std::string& outValue);
    static bool fromJsonValue(const JsonValue& value, const std::string& key, int& outValue);
    static bool fromJsonValue(const JsonValue& value, const std::string& key, double& outValue);
    static bool fromJsonValue(const JsonValue& value, const std::string& key, bool& outValue);
    
private:
    static bool parseJsonInternal(const std::string& str, size_t& pos, JsonValue& outValue);
    static void skipWhitespace(const std::string& str, size_t& pos);
    static bool parseString(const std::string& str, size_t& pos, std::string& outValue);
    static bool parseNumber(const std::string& str, size_t& pos, JsonValue& outValue);
    static bool parseBool(const std::string& str, size_t& pos, JsonValue& outValue);
    static bool parseNull(const std::string& str, size_t& pos);
    static bool parseArray(const std::string& str, size_t& pos, JsonValue& outValue);
    static bool parseObject(const std::string& str, size_t& pos, JsonValue& outValue);
};

// Legacy ConfigValue for compatibility
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

} // namespace nlm