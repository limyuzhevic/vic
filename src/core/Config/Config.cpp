#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

// JSON Parser Implementation
namespace {

class JSONParser {
public:
    JSONParser(const std::string& content) : content_(content), pos_(0) {}
    
    ConfigValue parse() {
        skipWhitespaceAndComments();
        return parseValue();
    }
    
private:
    char peek() const {
        return pos_ < content_.size() ? content_[pos_] : '\0';
    }
    
    char advance() {
        return pos_ < content_.size() ? content_[pos_++] : '\0';
    }
    
    void skipWhitespaceAndComments() {
        while (true) {
            while (pos_ < content_.size() && std::isspace(content_[pos_])) {
                ++pos_;
            }
            if (pos_ + 1 < content_.size() && content_[pos_] == '/' && content_[pos_ + 1] == '/') {
                // Single-line comment
                while (pos_ < content_.size() && content_[pos_] != '\n') {
                    ++pos_;
                }
            } else if (pos_ + 1 < content_.size() && content_[pos_] == '/' && content_[pos_ + 1] == '*') {
                // Multi-line comment
                pos_ += 2;
                while (pos_ + 1 < content_.size() && !(content_[pos_] == '*' && content_[pos_ + 1] == '/')) {
                    ++pos_;
                }
                if (pos_ + 1 < content_.size()) {
                    pos_ += 2; // Skip */
                }
            } else {
                break;
            }
        }
    }
    
    ConfigValue parseValue() {
        skipWhitespaceAndComments();
        char c = peek();
        
        if (c == '{') {
            return parseObject();
        } else if (c == '[') {
            return parseArray();
        } else if (c == '"' || c == '\'') {
            return parseString();
        } else if (c == 't' || c == 'f') {
            return parseBool();
        } else if (c == 'n') {
            return parseNull();
        } else {
            return parseNumber();
        }
    }
    
    ConfigValue parseObject() {
        std::vector<std::pair<std::string, ConfigValue>> obj;
        
        char open = advance(); // Skip {
        skipWhitespaceAndComments();
        
        if (peek() == '}') {
            advance(); // Skip }
            return ConfigValue(obj);
        }
        
        while (true) {
            skipWhitespaceAndComments();
            
            // Parse key (must be a string)
            std::string key = parseString().as<std::string>();
            
            skipWhitespaceAndComments();
            if (peek() != ':') {
                throw std::runtime_error("Expected ':' after key in JSON object");
            }
            advance(); // Skip :
            
            skipWhitespaceAndComments();
            ConfigValue value = parseValue();
            
            obj.emplace_back(key, value);
            
            skipWhitespaceAndComments();
            char next = peek();
            if (next == '}') {
                advance(); // Skip }
                break;
            } else if (next == ',') {
                advance(); // Skip ,
            } else {
                throw std::runtime_error("Expected ',' or '}' in JSON object");
            }
        }
        
        return ConfigValue(obj);
    }
    
    ConfigValue parseArray() {
        std::vector<ConfigValue> arr;
        
        char open = advance(); // Skip [
        skipWhitespaceAndComments();
        
        if (peek() == ']') {
            advance(); // Skip ]
            return ConfigValue(arr);
        }
        
        while (true) {
            skipWhitespaceAndComments();
            arr.push_back(parseValue());
            
            skipWhitespaceAndComments();
            char next = peek();
            if (next == ']') {
                advance(); // Skip ]
                break;
            } else if (next == ',') {
                advance(); // Skip ,
            } else {
                throw std::runtime_error("Expected ',' or ']' in JSON array");
            }
        }
        
        return ConfigValue(arr);
    }
    
    ConfigValue parseString() {
        char quote = advance(); // Skip opening quote
        std::string result;
        
        while (pos_ < content_.size() && peek() != quote) {
            if (peek() == '\\') {
                advance(); // Skip backslash
                if (pos_ >= content_.size()) {
                    throw std::runtime_error("Invalid escape sequence in JSON string");
                }
                char escaped = advance();
                switch (escaped) {
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape - simplified implementation
                        if (pos_ + 4 > content_.size()) {
                            throw std::runtime_error("Invalid Unicode escape sequence");
                        }
                        std::string hex = content_.substr(pos_, 4);
                        char* endptr;
                        long code = std::strtol(hex.c_str(), &endptr, 16);
                        if (*endptr != '\0') {
                            throw std::runtime_error("Invalid Unicode escape sequence");
                        }
                        result += static_cast<char>(code);
                        pos_ += 4;
                        break;
                    }
                    default:
                        result += escaped; // Allow any other character
                }
            } else {
                result += advance();
            }
        }
        
        if (peek() == quote) {
            advance(); // Skip closing quote
        } else {
            throw std::runtime_error("Unterminated string in JSON");
        }
        
        return ConfigValue(result);
    }
    
    ConfigValue parseBool() {
        if (content_.substr(pos_, 4) == "true") {
            pos_ += 4;
            return ConfigValue(true);
        } else if (content_.substr(pos_, 5) == "false") {
            pos_ += 5;
            return ConfigValue(false);
        } else {
            throw std::runtime_error("Invalid boolean value in JSON");
        }
    }
    
    ConfigValue parseNull() {
        if (content_.substr(pos_, 4) == "null") {
            pos_ += 4;
            return ConfigValue(std::string{}); // Use empty string to represent null
        } else {
            throw std::runtime_error("Invalid null value in JSON");
        }
    }
    
    ConfigValue parseNumber() {
        size_t start = pos_;
        bool isDouble = false;
        
        if (peek() == '-') {
            ++pos_;
        }
        
        // Parse digits before decimal point
        while (pos_ < content_.size() && std::isdigit(content_[pos_])) {
            ++pos_;
        }
        
        // Parse decimal part
        if (peek() == '.') {
            isDouble = true;
            ++pos_;
            while (pos_ < content_.size() && std::isdigit(content_[pos_])) {
                ++pos_;
            }
        }
        
        // Parse exponent
        if (peek() == 'e' || peek() == 'E') {
            isDouble = true;
            ++pos_;
            if (peek() == '+' || peek() == '-') {
                ++pos_;
            }
            while (pos_ < content_.size() && std::isdigit(content_[pos_])) {
                ++pos_;
            }
        }
        
        std::string numStr = content_.substr(start, pos_ - start);
        
        if (isDouble) {
            double val;
            try {
                val = std::stod(numStr);
            } catch (...) {
                throw std::runtime_error("Invalid number in JSON: " + numStr);
            }
            return ConfigValue(val);
        } else {
            // Try to parse as integer
            int64_t val;
            try {
                val = std::stoll(numStr);
            } catch (...) {
                try {
                    val = std::stoul(numStr);
                } catch (...) {
                    throw std::runtime_error("Invalid number in JSON: " + numStr);
                }
            }
            return ConfigValue(val);
        }
    }
    
    std::string content_;
    size_t pos_;
};

// Helper function to detect file format
detected_file_format detectFileFormat(const std::string& content) {
    if (content.empty()) return detected_file_format::Simple;
    
    // Skip whitespace
    size_t start = 0;
    while (start < content.size() && std::isspace(content[start])) {
        ++start;
    }
    
    if (start >= content.size()) return detected_file_format::Simple;
    
    char firstChar = content[start];
    
    if (firstChar == '{') {
        return detected_file_format::JSON;
    } else if (firstChar == '[') {
        return detected_file_format::JSON;
    } else if (firstChar == '"' || firstChar == '\'') {
        return detected_file_format::JSON;
    } else if (firstChar == 't' || firstChar == 'f' || firstChar == 'n') {
        // Check for boolean or null
        if (content.substr(start, 4) == "true" || content.substr(start, 5) == "false" || 
            content.substr(start, 4) == "null") {
            return detected_file_format::JSON;
        }
    } else if (firstChar == '-' || std::isdigit(firstChar)) {
        // Number
        return detected_file_format::JSON;
    } else {
        return detected_file_format::Simple;
    }
}

// Function to flatten JSON object into key=value pairs
void flattenJSON(const ConfigValue& value, std::vector<std::pair<std::string, std::string>>& flat,
                const std::string& prefix = "", const std::string& delimiter = ".") {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            flat.emplace_back(prefix, std::to_string(arg));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            flat.emplace_back(prefix, std::to_string(arg));
        } else if constexpr (std::is_same_v<T, double>) {
            flat.emplace_back(prefix, std::to_string(arg));
        } else if constexpr (std::is_same_v<T, bool>) {
            flat.emplace_back(prefix, arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Escape quotes and backslashes in string values
            std::string escaped = arg;
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\"");
                pos += 2;
            }
            pos = 0;
            while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\\");
                pos += 2;
            }
            flat.emplace_back(prefix, escaped);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string arrayStr = "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) arrayStr += ", ";
                arrayStr += std::to_string(arg[i]);
            }
            arrayStr += "]";
            flat.emplace_back(prefix, arrayStr);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string arrayStr = "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) arrayStr += ", ";
                arrayStr += std::to_string(arg[i]);
            }
            arrayStr += "]";
            flat.emplace_back(prefix, arrayStr);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string arrayStr = "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) arrayStr += ", ";
                // Escape quotes and backslashes
                std::string escaped = arg[i];
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                pos = 0;
                while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                    escaped.replace(pos, 1, "\\\\");
                    pos += 2;
                }
                arrayStr += "\"" + escaped + "\"";
            }
            arrayStr += "]";
            flat.emplace_back(prefix, arrayStr);
        } else {
            // Handle nested objects and null
            std::string arrayStr = "{}";
            flat.emplace_back(prefix, arrayStr);
        }
    }, value);
}

}} // namespace nlm

namespace nlm {

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Detect file format
    detected_file_format format = detectFileFormat(content);
    
    if (format == detected_file_format::JSON) {
        return loadFromJSON(content);
    } else {
        // Fall back to simple key=value parser
        return loadFromSimple(content);
    }
}

bool Config::loadFromJSON(const std::string& content) {
    try {
        JSONParser parser(content);
        ConfigValue value = parser.parse();
        
        // If it's a simple object (key-value pairs), flatten it
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::vector<std::pair<std::string, ConfigValue>>>) {
                // JSON object
                for (const auto& entry : arg) {
                    // Convert value to string representation
                    std::string valueStr = valueToString(entry.second);
                    set(entry.first, valueStr, ConfigSource::File);
                }
            } else {
                // Single value JSON - convert to a single key
                set("value", valueToString(arg), ConfigSource::File);
            }
        }, value);
        
        return true;
    } catch (const std::exception& e) {
        // Fall back to simple parser if JSON parsing fails
        return loadFromSimple(content);
    }
}

std::string Config::valueToString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        } else {
            ss << "{}";
        }
    }, value);
    return ss.str();
}

bool Config::loadFromSimple(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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
