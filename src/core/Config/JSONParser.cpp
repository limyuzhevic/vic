#include "JSONParser.hpp"
#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

// Tokenizer implementation
JSONParser::Tokenizer::Tokenizer(const std::string& input)
    : input(input), position(0), line(1), column(0), currentToken_(JSONTokenType::END_OF_FILE), currentValue_("") {}

void JSONParser::Tokenizer::skipWhitespace() {
    while (position < input.size() && isWhitespace(input[position])) {
        if (input[position] == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
        position++;
    }
}

void JSONParser::Tokenizer::parseString() {
    column++;
    position++;
    std::string result;
    
    while (position < input.size() && input[position] != '\"') {
        if (input[position] == '\\') {
            position++;
            column++;
            if (position >= input.size()) {
                throw JSONParseError("Unterminated escape sequence", line, column);
            }
            switch (input[position]) {
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: throw JSONParseError("Invalid escape sequence", line, column);
            }
        } else if (input[position] == '\n') {
            line++;
            column = 0;
        } else {
            result += input[position];
        }
        position++;
        column++;
    }
    
    if (position >= input.size() || input[position] != '\"') {
        throw JSONParseError("Unterminated string", line, column);
    }
    
    position++;
    column++;
    currentValue_ = unescapeString(result);
}

void JSONParser::Tokenizer::parseNumber() {
    size_t start = position;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    if (input[position] == '-') {
        position++;
        column++;
    }
    
    while (position < input.size()) {
        char c = input[position];
        if (isDigit(c)) {
            // Do nothing
        } else if (c == '.' && !hasDecimal) {
            hasDecimal = true;
        } else if ((c == 'e' || c == 'E') && !hasExponent) {
            hasExponent = true;
            position++;
            column++;
            if (position < input.size() && (input[position] == '+' || input[position] == '-')) {
                position++;
                column++;
            }
            continue;
        } else {
            break;
        }
        position++;
        column++;
    }
    
    currentValue_ = input.substr(start, position - start);
}

void JSONParser::Tokenizer::parseLiteral(const std::string& literal) {
    if (!startsWith(input.substr(position), literal)) {
        throw JSONParseError("Expected literal '" + literal + "'", line, column);
    }
    currentValue_ = literal;
    position += literal.size();
    column += literal.size();
}

JSONTokenType JSONParser::Tokenizer::nextToken() {
    skipWhitespace();
    
    if (position >= input.size()) {
        currentToken_ = JSONTokenType::END_OF_FILE;
        return currentToken_;
    }
    
    char c = input[position];
    
    switch (c) {
        case '{':
            currentToken_ = JSONTokenType::OBJECT_START;
            currentValue_ = "";
            position++;
            column++;
            break;
        case '}':
            currentToken_ = JSONTokenType::OBJECT_END;
            currentValue_ = "";
            position++;
            column++;
            break;
        case '[':
            currentToken_ = JSONTokenType::ARRAY_START;
            currentValue_ = "";
            position++;
            column++;
            break;
        case ']':
            currentToken_ = JSONTokenType::ARRAY_END;
            currentValue_ = "";
            position++;
            column++;
            break;
        case ':':
            currentToken_ = JSONTokenType::COLON;
            currentValue_ = "";
            position++;
            column++;
            break;
        case ',':
            currentToken_ = JSONTokenType::COMMA;
            currentValue_ = "";
            position++;
            column++;
            break;
        case '\"':
            currentToken_ = JSONTokenType::STRING;
            parseString();
            break;
        case 't': case 'f':
            currentToken_ = JSONTokenType::BOOLEAN;
            parseLiteral("true");
            break;
        case 'n':
            currentToken_ = JSONTokenType::NULL_VAL;
            parseLiteral("null");
            break;
        default:
            if (isDigit(c) || (c == '-' && position + 1 < input.size() && isDigit(input[position + 1]))) {
                currentToken_ = JSONTokenType::NUMBER;
                parseNumber();
            } else {
                throw JSONParseError("Unexpected character", line, column);
            }
    }
    
    return currentToken_;
}

// Parser implementation
JSONParser::Parser::Parser(Tokenizer& tokenizer) : tokenizer(tokenizer) {}

JSONValue JSONParser::Parser::parseValue() {
    skipWhitespace();
    JSONTokenType token = tokenizer.nextToken();
    
    switch (token) {
        case JSONTokenType::OBJECT_START:
            return parseObject();
        case JSONTokenType::ARRAY_START:
            return parseArray();
        case JSONTokenType::STRING:
            return parseStringValue();
        case JSONTokenType::NUMBER:
            return parseNumberValue(tokenizer.currentValue());
        case JSONTokenType::BOOLEAN:
            return parseBoolValue(tokenizer.currentValue());
        case JSONTokenType::NULL_VAL:
            return nullptr;
        case JSONTokenType::END_OF_FILE:
            throw JSONParseError("Unexpected end of file", tokenizer.getLine(), tokenizer.getColumn());
        default:
            throw JSONParseError("Unexpected token", tokenizer.getLine(), tokenizer.getColumn());
    }
}

JSONValue JSONParser::Parser::parseObject() {
    std::unordered_map<std::string, JSONValue> obj;
    
    // Expect object end first for empty object
    if (tokenizer.nextToken() == JSONTokenType::OBJECT_END) {
        return obj;
    }
    
    while (true) {
        // Back up to parse key
        if (tokenizer.currentToken() != JSONTokenType::OBJECT_END) {
            if (tokenizer.currentToken() != JSONTokenType::STRING) {
                throw JSONParseError("Expected string key in object", tokenizer.getLine(), tokenizer.getColumn());
            }
            
            std::string key = tokenizer.currentValue();
            
            // Expect colon
            if (tokenizer.nextToken() != JSONTokenType::COLON) {
                throw JSONParseError("Expected colon after key", tokenizer.getLine(), tokenizer.getColumn());
            }
            
            // Parse value
            JSONValue value = parseValue();
            obj[key] = value;
            
            // Next token should be comma or object end
            JSONTokenType nextToken = tokenizer.nextToken();
            if (nextToken == JSONTokenType::OBJECT_END) {
                break;
            } else if (nextToken != JSONTokenType::COMMA) {
                throw JSONParseError("Expected comma or end of object", tokenizer.getLine(), tokenizer.getColumn());
            }
        }
    }
    
    return obj;
}

JSONValue JSONParser::Parser::parseArray() {
    std::vector<JSONValue> arr;
    
    // Expect array end first for empty array
    if (tokenizer.nextToken() == JSONTokenType::ARRAY_END) {
        return arr;
    }
    
    while (true) {
        // Back up to parse element
        if (tokenizer.currentToken() != JSONTokenType::ARRAY_END) {
            JSONValue element = parseValue();
            arr.push_back(element);
            
            // Next token should be comma or array end
            JSONTokenType nextToken = tokenizer.nextToken();
            if (nextToken == JSONTokenType::ARRAY_END) {
                break;
            } else if (nextToken != JSONTokenType::COMMA) {
                throw JSONParseError("Expected comma or end of array", tokenizer.getLine(), tokenizer.getColumn());
            }
        }
    }
    
    return arr;
}

JSONValue JSONParser::Parser::parseStringValue() {
    return tokenizer.currentValue();
}

JSONValue JSONParser::Parser::parseNumberValue(const std::string& numStr) {
    // Try to parse as int first, then double
    size_t pos = 0;
    try {
        long long intVal = std::stoll(numStr, &pos);
        if (pos == numStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        double doubleVal = std::stod(numStr, &pos);
        if (pos == numStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // If all parsing fails, return as string
    return numStr;
}

JSONValue JSONParser::Parser::parseBoolValue(const std::string& boolStr) {
    return (boolStr == "true");
}

// Helper functions implementation
bool JSONParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool JSONParser::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool JSONParser::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool JSONParser::isAlphanumeric(char c) {
    return isDigit(c) || isAlpha(c);
}

std::string JSONParser::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && isWhitespace(str[start])) {
        start++;
    }
    
    while (end > start && isWhitespace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string JSONParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool JSONParser::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::string JSONParser::parseNumber(const std::string& str, size_t& pos) {
    size_t start = pos;
    if (str[pos] == '-') {
        pos++;
    }
    
    while (pos < str.size() && isDigit(str[pos])) {
        pos++;
    }
    
    if (pos < str.size() && str[pos] == '.') {
        pos++;
        while (pos < str.size() && isDigit(str[pos])) {
            pos++;
        }
    }
    
    if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
        pos++;
        if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
            pos++;
        }
        while (pos < str.size() && isDigit(str[pos])) {
            pos++;
        }
    }
    
    return str.substr(start, pos - start);
}

std::string JSONParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; i++; break;
                case '\"': result += '\"'; i++; break;
                case '/': result += '/'; i++; break;
                case 'b': result += '\b'; i++; break;
                case 'f': result += '\f'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

// Public API implementation
JSONValue JSONParser::parseFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw JSONParseError("Could not open file: " + filepath, 1, 0);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseString(buffer.str());
}

JSONValue JSONParser::parseString(const std::string& jsonStr) {
    Tokenizer tokenizer(jsonStr);
    Parser parser(tokenizer);
    return parser.parseValue();
}

JSONValue JSONParser::parseYAMLFile(const std::string& filepath) {
    // Simple YAML parser for configuration
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw JSONParseError("Could not open YAML file: " + filepath, 1, 0);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    std::string yamlStr = buffer.str();
    // Convert simple YAML to JSON for parsing
    // This is a simplified implementation for configuration files
    std::string jsonStr = convertSimpleYAMLToJSON(yamlStr);
    
    return parseString(jsonStr);
}

void JSONParser::convertToConfig(const JSONValue& jsonValue, std::vector<ConfigEntry>& entries) {
    if (std::holds_alternative<std::unordered_map<std::string, JSONValue>>(jsonValue)) {
        const auto& obj = std::get<std::unordered_map<std::string, JSONValue>>(jsonValue);
        
        for (const auto& pair : obj) {
            const std::string& key = pair.first;
            const JSONValue& value = pair.second;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (std::holds_alternative<int>(value)) {
                configValue = std::get<int>(value);
            } else if (std::holds_alternative<int64_t>(value)) {
                configValue = std::get<int64_t>(value);
            } else if (std::holds_alternative<double>(value)) {
                configValue = std::get<double>(value);
            } else if (std::holds_alternative<bool>(value)) {
                configValue = std::get<bool>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                configValue = std::get<std::string>(value);
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                configValue = std::get<std::vector<int>>(value);
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                configValue = std::get<std::vector<double>>(value);
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                configValue = std::get<std::vector<std::string>>(value);
            }
            
            entries.emplace_back(key, configValue, ConfigSource::File, "");
        }
    } else {
        throw JSONValidationError("JSON root must be an object for configuration");
    }
}

std::string JSONParser::getStringValue(const JSONValue& value, const std::string& path) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else {
        throw JSONValidationError("Expected string at path: " + path);
    }
}

int JSONParser::getIntValue(const JSONValue& value, const std::string& path) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<double>(value)) {
        double d = std::get<double>(value);
        return static_cast<int>(d);
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<int>(std::get<int64_t>(value));
    } else {
        throw JSONValidationError("Expected integer at path: " + path);
    }
}

double JSONParser::getDoubleValue(const JSONValue& value, const std::string& path) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else {
        throw JSONValidationError("Expected number at path: " + path);
    }
}

bool JSONParser::getBoolValue(const JSONValue& value, const std::string& path) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else {
        throw JSONValidationError("Expected boolean at path: " + path);
    }
}

void JSONParser::validateConfig(const std::vector<ConfigEntry>& entries) {
    // Check for required keys
    auto requiredKeys = getRequiredKeys();
    auto optionalKeys = getOptionalKeys();
    
    for (const auto& key : requiredKeys) {
        bool found = false;
        for (const auto& entry : entries) {
            if (entry.key == key) {
                found = true;
                break;
            }
        }
        if (!found) {
            throw JSONValidationError("Missing required configuration key: " + key);
        }
    }
}

std::vector<std::string> JSONParser::getRequiredKeys() {
    return {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability",
        "plasticity_learning_rate"
    };
}

std::vector<std::string> JSONParser::getOptionalKeys() {
    return {
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate",
        "replay_interval",
        "consolidation_interval",
        "checkpoint_dir"
    };
}

std::string JSONParser::convertSimpleYAMLToJSON(const std::string& yamlStr) {
    // Simple YAML to JSON conversion for configuration files
    // This handles key-value pairs with simple formatting
    std::stringstream jsonStr;
    jsonStr << "{\n";
    
    std::vector<std::string> lines;
    std::stringstream lineStream(yamlStr);
    std::string line;
    
    while (std::getline(lineStream, line)) {
        lines.push_back(trim(line));
    }
    
    bool firstLine = true;
    for (const auto& line : lines) {
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (!firstLine) {
                jsonStr << ",\n";
            }
            firstLine = false;
            
            jsonStr << "  \"" << key << "\": \"" << value << "\"";
        }
    }
    
    jsonStr << "\n}";
    return jsonStr.str();
}

} // namespace nlm