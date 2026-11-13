// Simple standalone JSON parser for Config.cpp - doesn't depend on nlohmann_json
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace nlm {

struct JsonValue;
using JsonObject = std::unordered_map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

struct JsonValue {
    enum class Type { Null, Bool, Number, String, Object, Array };
    
    Type type;
    std::variant<bool, double, std::string, JsonObject, JsonArray> value;
    
    JsonValue() : type(Type::Null) {}
    JsonValue(bool b) : type(Type::Bool), value(b) {}
    JsonValue(double d) : type(Type::Number), value(d) {}
    JsonValue(const std::string& s) : type(Type::String), value(s) {}
    JsonValue(const JsonObject& o) : type(Type::Object), value(o) {}
    JsonValue(const JsonArray& a) : type(Type::Array), value(a) {}
};

// Simple JSON parser implementation
class SimpleJsonParser {
public:
    static bool parse(const std::string& jsonStr, JsonValue& out);
    static std::string toString(const JsonValue& val);
    static bool isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
};

// Config implementation using SimpleJsonParser
struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    try {
        JsonValue json;
        if (!SimpleJsonParser::parse(content, json)) {
            return false;
        }
        
        pImpl->entries.clear();
        
        if (json.type == JsonValue::Type::Object) {
            const auto& obj = std::get<JsonObject>(json.value);
            for (const auto& pair : obj) {
                std::string key = pair.first;
                const JsonValue& val = pair.second;
                ConfigValue configVal;
                
                switch (val.type) {
                    case JsonValue::Type::Bool:
                        configVal = std::get<bool>(val.value);
                        break;
                    case JsonValue::Type