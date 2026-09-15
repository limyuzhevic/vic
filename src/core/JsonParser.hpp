#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <memory>
#include <optional>
#include "../core/Config/Config.hpp"

namespace nlm {

// Simple JSON parser for NLM configuration
// TODO: Add full JSON parser implementation with proper error handling
class JsonParser {
public:
    static bool parseFile(const std::string& filepath, std::unordered_map<std::string, nlm::ConfigValue>& outValues);
    static bool parseString(const std::string& jsonStr, std::unordered_map<std::string, nlm::ConfigValue>& outValues);
    static bool validate(const std::unordered_map<std::string, nlm::ConfigValue>& values);
    static std::string toJson(const std::unordered_map<std::string, nlm::ConfigValue>& values);
    
private:
    static bool parseValue(const std::string& token, nlm::ConfigValue& outValue);
    static bool isWhitespace(char c);
    static std::string trim(const std::string& str);
    static std::string extractString(const std::string& str, size_t& pos);
    static double parseNumber(const std::string& str, size_t& pos);
    static bool parseArray(const std::string& str, size_t& pos, std::vector<nlm::ConfigValue>& outArray);
};

} // namespace nlm