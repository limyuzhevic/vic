#ifndef NLM_JSON_PARSER_H
#define NLM_JSON_PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Simple JSON value types
using JsonValue = std::variant<std::string, int, double, bool, std::nullptr_t, std::vector<JsonValue>, std::unordered_map<std::string, JsonValue>>;

class JsonParser {
public:
    static std::optional<JsonValue> parse(const std::string& json);
    static std::string serialize(const JsonValue& json);
    
private:
    static void skipWhitespace(const std::string& json, size_t& pos);
    static char getNextChar(const std::string& json, size_t& pos);
    static std::optional<JsonValue> parseValue(const std::string& json, size_t& pos);
    static std::optional<std::string> parseString(const std::string& json, size_t& pos);
    static std::optional<double> parseNumber(const std::string& json, size_t& pos);
    static std::optional<bool> parseBool(const std::string& json, size_t& pos);
    static std::optional<JsonValue> parseArray(const std::string& json, size_t& pos);
    static std::optional<JsonValue> parseObject(const std::string& json, size_t& pos);
};

} // namespace nlm

#endif // NLM_JSON_PARSER_H