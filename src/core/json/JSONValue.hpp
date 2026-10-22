// src/core/json/JSONValue.hpp
#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <stdexcept>

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

// JSON parsing exceptions
class JSONParseError : public std::runtime_error {
public:
    explicit JSONParseError(const std::string& msg) : std::runtime_error(msg) {}
};

// JSON helper functions
inline bool isNull(const JSONValue& value) {
    return std::holds_alternative<std::nullptr_t>(value);
}

inline bool isBoolean(const JSONValue& value) {
    return std::holds_alternative<bool>(value);
}

inline bool isNumber(const JSONValue& value) {
    return std::holds_alternative<double>(value);
}

inline bool isString(const JSONValue& value) {
    return std::holds_alternative<std::string>(value);
}

inline bool isArray(const JSONValue& value) {
    return std::holds_alternative<std::vector<JSONValue>>(value);
}

inline bool isObject(const JSONValue& value) {
    return std::holds_alternative<std::map<std::string, JSONValue>>(value);
}

// Type-safe accessors
inline const bool& asBoolean(const JSONValue& value) {
    return std::get<bool>(value);
}

inline const double& asNumber(const JSONValue& value) {
    return std::get<double>(value);
}

inline const std::string& asString(const JSONValue& value) {
    return std::get<std::string>(value);
}

inline const std::vector<JSONValue>& asArray(const JSONValue& value) {
    return std::get<std::vector<JSONValue>>(value);
}

inline const std::map<std::string, JSONValue>& asObject(const JSONValue& value) {
    return std::get<std::map<std::string, JSONValue>>(value);
}

// JSON parsing
JSONValue parseJSON(const std::string& input);

// JSON stringification
std::string toJSON(const JSONValue& value, int indent = 0);

} // namespace json
} // namespace nlm
