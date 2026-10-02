#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <stack>

namespace nlm {

namespace json_utils {
    // Simple JSON escape function for strings
    std::string escapeJsonString(const std::string& str) {
        std::string result;
        result.reserve(str.size() * 2); // Worst case size
        
        for (char c : str) {
            switch (c) {
                case '\\': result += "\\\\"; break;
                case '"': result += "\\\""; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < 0x20) {
                        result += "\\u";
                        static const char* hexDigits = "0123456789ABCDEF";
                        result += hexDigits[(c >> 12) & 0xF];
                        result += hexDigits[(c >> 8) & 0xF];
                        result += hexDigits[(c >> 4) & 0xF];
                        result += hexDigits[c & 0xF];
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        return result;
    }
    
    // Parse JSON value from string
    ConfigValue parseJsonValue(const std::string& json, size_t& pos);
    
    // Parse JSON object from string
    std::map<std::string, ConfigValue> parseJsonObject(const std::string& json);
    
    // Simple JSON value parser
    ConfigValue parseJsonValue(const std::string& json, size_t& pos) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) {
            throw std::runtime_error("Unexpected end of JSON");
        }
        
        char c = json[pos];
        
        if (c == '{') {
            // Object - not directly supported in ConfigValue
            throw std::runtime_error("JSON objects cannot be directly parsed into ConfigValue");
        } else if (c == '[') {
            // Array
            return parseJsonArray(json, pos);
        } else if (c == '"') {
            // String
            return parseJsonString(json, pos);
        } else if (c == 't' && json.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (c == 'f' && json.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        } else if (c == 'n' && json.substr(pos, 4) == "null") {
            pos += 4;
            return std::string(); // Empty string for null
        } else {
            // Number (including integers and floats)
            return parseJsonNumber(json, pos);
        }
    }
    
    // Parse JSON array from string
    std::vector<ConfigValue> parseJsonArray(const std::string& json, size_t& pos) {
        std::vector<ConfigValue> result;
        
        expectChar(json, pos, '[');
        skipWhitespace(json, pos);
        
        if (json[pos] == ']') {
            pos++; // Skip ']'
            return result;
        }
        
        while (pos < json.size()) {
            result.push_back(parseJsonValue(json, pos));
            
            skipWhitespace(json, pos);
            
            if (pos < json.size() && json[pos] == ']') {
                pos++; // Skip ']'
                break;
            } else if (pos < json.size() && json[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(json, pos);
            } else {
                throw std::runtime_error("Expected ',' or ']' in JSON array");
            }
        }
        
        return result;
    }
    
    // Parse JSON string from string
    std::string parseJsonString(const std::string& json, size_t& pos) {
        expectChar(json, pos, '"');
        
        std::string result;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos++; // Skip '\'
                switch (json[pos]) {
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u':
                        // Unicode escape - simplified implementation
                        if (pos + 4 < json.size()) {
                            // Parse 4 hex digits
                            std::string hex = json.substr(pos + 1, 4);
                            try {
                                char16_t code = static_cast<char16_t>(std::stoi(hex, nullptr, 16));
                                result += static_cast<char>(code);
                            } catch (...) {
                                result += '?' ;
                            }
                            pos += 4;
                        }
                        break;
                    default:
                        result += json[pos]; // Unknown escape, include as-is
                        break;
                }
            } else {
                result += json[pos];
            }
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != '"') {
            throw std::runtime_error("Unterminated JSON string");
        }
        
        pos++; // Skip closing '"'
        return result;
    }
    
    // Parse JSON number from string
    ConfigValue parseJsonNumber(const std::string& json, size_t& pos) {
        size_t start = pos;
        bool isNegative = false;
        
        if (json[pos] == '-') {
            isNegative = true;
            pos++;
        }
        
        // Parse integer part
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
        
        // Parse decimal part
        bool hasDecimal = false;
        if (pos < json.size() && json[pos] == '.') {
            hasDecimal = true;
            pos++;
            while (pos < json.size() && std::isdigit(json[pos])) {
                pos++;
            }
        }
        
        // Parse exponent part
        bool hasExponent = false;
        if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
            hasExponent = true;
            pos++;
            if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
                pos++;
            }
            while (pos < json.size() && std::isdigit(json[pos])) {
                pos++;
            }
        }
        
        std::string numberStr = json.substr(start, pos - start);
        
        // Try to parse as integer first
        try {
            if (numberStr.find('.') == std::string::npos && numberStr.find('e') == std::string::npos && numberStr.find('E') == std::string::npos) {
                // Integer without decimal or exponent
                if (isNegative) {
                    return std::stoll(numberStr);
                } else {
                    // Try unsigned long long first
                    try {
                        return std::stoull(numberStr);
                    } catch (...) {
                        return std::stoll(numberStr);
                    }
                }
            } else {
                // Float/double
                return std::stod(numberStr);
            }
        } catch (...) {
            throw std::runtime_error("Failed to parse number: " + numberStr);
        }
    }
    
    // Skip whitespace in JSON string
    void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    }
    
    // Expect specific character in JSON string
    void expectChar(const std::string& json, size_t& pos, char expected) {
        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != expected) {
            throw std::runtime_error("Expected character '" + std::string(1, expected) + "' in JSON");
        }
        pos++;
    }
    
    // Parse JSON file content
    std::string readJsonFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    // Parse JSON string to ConfigValue
    ConfigValue jsonStringToConfigValue(const std::string& jsonStr) {
        size_t pos = 0;
        skipWhitespace(jsonStr, pos);
        
        ConfigValue value = parseJsonValue(jsonStr, pos);
        
        // Check if there are trailing characters (except whitespace)
        skipWhitespace(jsonStr, pos);
        if (pos != jsonStr.size()) {
            throw std::runtime_error("Extra characters after JSON value");
        }
        
        return value;
    }
    
    // Parse JSON array of ConfigValue
    std::vector<ConfigValue> jsonStringToConfigValueArray(const std::string& jsonStr) {
        size_t pos = 0;
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size() || jsonStr[pos] != '[') {
            throw std::runtime_error("Expected '[' in JSON array");
        }
        
        std::vector<ConfigValue> result = parseJsonArray(jsonStr, pos);
        
        // Check if there are trailing characters (except whitespace)
        skipWhitespace(jsonStr, pos);
        if (pos != jsonStr.size()) {
            throw std::runtime_error("Extra characters after JSON array");
        }
        
        return result;
    }
    
    // Convert ConfigValue to JSON string
    std::string configValueToJson(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double to avoid scientific notation
                std::stringstream ss;
                ss << std::fixed << std::setprecision(10) << arg;
                std::string str = ss.str();
                // Remove trailing zeros after decimal point
                size_t decimalPos = str.find('.');
                if (decimalPos != std::string::npos) {
                    // Remove trailing zeros
                    size_t endPos = str.size() - 1;
                    while (endPos > decimalPos && str[endPos] == '0') {
                        endPos--;
                    }
                    if (str[endPos] == '.') {
                        endPos--; // Remove decimal point if all zeros
                    }
                    str.resize(endPos + 1);
                }
                oss << str;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJsonString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    std::stringstream ss;
                    ss << std::fixed << std::setprecision(10) << arg[i];
                    std::string str = ss.str();
                    // Remove trailing zeros
                    size_t decimalPos = str.find('.');
                    if (decimalPos != std::string::npos) {
                        size_t endPos = str.size() - 1;
                        while (endPos > decimalPos && str[endPos] == '0') {
                            endPos--;
                        }
                        if (str[endPos] == '.') {
                            endPos--;
                        }
                        str.resize(endPos + 1);
                    }
                    oss << str;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << escapeJsonString(arg[i]) << "\"";
                }
                oss << "]";
            }
        }, value);
        return oss.str();
    }
    
    // Write JSON object to stream
    void writeJsonObject(std::ostream& out, const std::map<std::string, ConfigValue>& config, bool pretty = false) {
        out << "{";
        
        if (pretty) out << "\n";
        
        bool first = true;
        for (const auto& entry : config) {
            if (!first) {
                out << ",";
                if (pretty) out << "\n";
            }
            first = false;
            
            if (pretty) out << "  ";
            
            out << "\"" << entry.first << "\": ";
            
            // Write value
            std::visit([&out, pretty](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    out << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    if (pretty) {
                        out << std::fixed << std::setprecision(10) << arg;
                    } else {
                        out << arg;
                    }
                } else if constexpr (std::is_same_v<T, bool>) {
                    out << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    out << "\"" << escapeJsonString(arg) << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    out << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) out << ", ";
                        out << arg[i];
                    }
                    out << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    out << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) out << ", ";
                        if (pretty) {
                            out << std::fixed << std::setprecision(10) << arg[i];
                        } else {
                            out << arg[i];
                        }
                    }
                    out << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    out << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) out << ", ";
                        out << "\"" << escapeJsonString(arg[i]) << "\"";
                    }
                    out << "]";
                }
            }, entry.second);
        }
        
        if (pretty) {
            out << "\n";
            if (!config.empty()) out << "  ";
        }
        
        out << "}";
    }
    
    // Convert ConfigValue to JSON string
    std::string configToJson(const ConfigValue& value, bool pretty = false) {
        std::ostringstream oss;
        
        std::visit([&oss, pretty](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                if (pretty) {
                    oss << std::fixed << std::setprecision(10) << arg;
                } else {
                    oss << arg;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJsonString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    if (pretty) {
                        oss << std::fixed << std::setprecision(10) << arg[i];
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << escapeJsonString(arg[i]) << "\"";
                }
                oss << "]";
            }
        }, value);
        
        return oss.str();
    }
}} // namespace json_utils

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try to parse as JSON first (modern format)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Check if it looks like JSON (starts with { or [)
        std::string trimmedContent = content;
        size_t start = trimmedContent.find_first_not_of(" \t\n\r");
        if (start != std::string::npos && (trimmedContent[start] == '{' || trimmedContent[start] == '[')) {
            // Parse as JSON
            return loadFromJson(trimmedContent);
        } else {
            // Try legacy key=value format
            return loadLegacyFormat(filepath);
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to load config from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save as JSON (modern format)
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        file << json_utils::configToJson(entry.value, true);
        
        if (!entry.description.empty()) {
            file << "  // " << entry.description << "\n";
        }
    }
    
    file << "\n}\n";
    return true;
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        // Parse JSON string
        auto configMap = json_utils::parseJsonObject(jsonString);
        
        // Clear existing entries and set from map
        clear();
        setFromMap(configMap);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Warning: Failed to load config from JSON: " << e.what() << std::endl;
        return false;
    }
}

std::string Config::saveToJson() const {
    return json_utils::configMapToJson(getAsMap());
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, "", source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, const ConfigValue& value, const std::string& description, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
            pImpl->descriptions[key] = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
        if (!description.empty()) {
            pImpl->descriptions[key] = description;
        }
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->descriptions.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->descriptions.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    return oss.str();
}

std::map<std::string, ConfigValue> Config::getAsMap() const {
    std::map<std::string, ConfigValue> result;
    for (const auto& entry : pImpl->entries) {
        result[entry.key] = entry.value;
    }
    return result;
}

void Config::setFromMap(const std::map<std::string, ConfigValue>& config) {
    for (const auto& entry : config) {
        // Use default source for bulk loading
        set(entry.first, entry.second, ConfigSource::File);
    }
}

bool Config::validate() const {
    // Basic validation - can be extended with type checking, range validation, etc.
    for (const auto& entry : pImpl->entries) {
        // Check if value can be converted to string (basic validation)
        try {
            std::visit([](auto&& arg) { 
                // Just test if we can access the value
                static_cast<void>(arg); 
            }, entry.value);
        } catch (const std::exception&) {
            std::cerr << "Warning: Invalid configuration value for key \"" << entry.key << "\"" << std::endl;
            return false;
        }
    }
    return true;
}

void Config::initializeDefaults() {
    // Set some reasonable defaults for NLM
    set("neuron_count", 1000LL, "Total number of neurons in the brain");
    set("region_count", 2, "Number of neural regions");
    set("connection_probability", 0.1, "Probability of connection between neurons");
    set("simulation_timestep", 0.001, "Simulation time step in seconds");
    set("random_seed", static_cast<int64_t>(42), "Random seed for reproducible simulations");
    set("stdp_ltp_weight", 0.01f, "STDP long-term potentiation weight");
    set("stdp_ltd_weight", 0.012f, "STDP long-term depression weight");
    set("stdp_tau", 20.0f, "STDP time constant");
    set("synaptogenesis_rate", 0.0001f, "Rate of new synapse formation");
    set("pruning_rate", 0.00001f, "Rate of synapse elimination");
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::loadLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    return true;
}

std::map<std::string, ConfigValue> json_utils::parseJsonObject(const std::string& json) {
    std::map<std::string, ConfigValue> result;
    
    size_t pos = 0;
    skipWhitespace(json, pos);
    
    if (json[pos] != '{') {
        throw std::runtime_error("Expected '{' for JSON object");
    }
    
    pos++; // Skip '{'
    skipWhitespace(json, pos);
    
    if (json[pos] == '}') {
        pos++; // Skip '}'
        return result;
    }
    
    while (pos < json.size()) {
        // Parse key
        skipWhitespace(json, pos);
        if (json[pos] != '"') {
            throw std::runtime_error("Expected quoted string for object key");
        }
        
        size_t keyStart = pos;
        std::string key = parseJsonString(json, pos);
        
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != ':') {
            throw std::runtime_error("Expected ':' after object key");
        }
        
        pos++; // Skip ':'
        skipWhitespace(json, pos);
        
        // Parse value
        ConfigValue value = parseJsonValue(json, pos);
        result[key] = value;
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == '}') {
            pos++; // Skip '}'
            break;
        } else if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip ','
            skipWhitespace(json, pos);
        } else {
            throw std::runtime_error("Expected ',' or '}' in JSON object");
        }
    }
    
    return result;
}

void json_utils::writeJsonObject(std::ostream& out, const std::map<std::string, ConfigValue>& config, bool pretty) {
    out << "{";
    
    if (pretty) out << "\n";
    
    bool first = true;
    for (const auto& entry : config) {
        if (!first) {
            out << ",";
            if (pretty) out << "\n";
        }
        first = false;
        
        if (pretty) out << "  ";
        
        out << "\"" << entry.first << "\": ";
        
        // Write value
        std::visit([&out, pretty](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                out << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                if (pretty) {
                    out << std::fixed << std::setprecision(10) << arg;
                } else {
                    out << arg;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                out << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                out << "\"" << escapeJsonString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                out << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) out << ", ";
                    out << arg[i];
                }
                out << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                out << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) out << ", ";
                    if (pretty) {
                        out << std::fixed << std::setprecision(10) << arg[i];
                    } else {
                        out << arg[i];
                    }
                }
                out << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                out << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) out << ", ";
                    out << "\"" << escapeJsonString(arg[i]) << "\"";
                }
                out << "]";
            }
        }, entry.second);
    }
    
    if (pretty) {
        out << "\n";
        if (!config.empty()) out << "  ";
    }
    
    out << "}";
}

std::string json_utils::configMapToJson(const std::map<std::string, ConfigValue>& config) {
    std::ostringstream oss;
    writeJsonObject(oss, config, true);
    return oss.str();
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
