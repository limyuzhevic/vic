#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>

namespace nlm {

// Simple JSON parser for configuration files
class JsonParser {
public:
    struct JsonValue {
        std::variant<int, int64_t, double, bool, std::string, std::vector<JsonValue>> value;
        std::string toString() const;
    };
    
    static bool parseFile(const std::string& filepath, std::map<std::string, JsonValue>& jsonData);
    static std::optional<JsonValue> parseValue(const std::string& str);
    static std::string trim(const std::string& str);
    static bool isWhitespace(char c);
};

} // namespace nlm