#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <variant>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace nlm {

// JSON value types
using JSONValue = std::variant<
    std::monostate, // null
    bool,
    double,
    int64_t,
    std::string,
    std::vector<JSONValue>,     // array
    std::map<std::string, JSONValue>  // object
>;

class JSONParser {
public:
    static bool parse(const std::string& input, JSONValue& output);
    static std::string serialize(const JSONValue& value, int indent = 0);
    static std::optional<JSONValue> parseFile(const std::string& filepath);
    
private:
    static bool parseValue(const std::string& input, size_t& pos, JSONValue& output);
    static bool parseObject(const std::string& input, size_t& pos, std::map<std::string, JSONValue>& output);
    static bool parseArray(const std::string& input, size_t& pos, std::vector<JSONValue>& output);
    static bool parseString(const std::string& input, size_t& pos, std::string& output);
    static bool parseNumber(const std::string& input, size_t& pos, double& output);
    static bool parseBoolean(const std::string& input, size_t& pos, bool& output);
    static bool parseNull(const std::string& input, size_t& pos);
    static void skipWhitespace(const std::string& input, size_t& pos);
    static char getNextChar(const std::string& input, size_t& pos);
};

// Helper functions for string manipulation
inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

} // namespace nlm