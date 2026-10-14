#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool useJsonFormat;  // New feature: support JSON/YAML formats
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->useJsonFormat = false; // Default to key=value format
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Improved JSON parser implementation
namespace {
    // Simple JSON tokenization
    enum class JsonTokenType {
        Unknown,
        String,
        Number,
        Boolean,
        Null,
        LeftBrace,
        RightBrace,
        LeftBracket,
        RightBracket,
        Colon,
        Comma,
        EndOfStream
    };
    
    struct JsonToken {
        JsonTokenType type;
        std::string value;
        size_t line;
        size_t column;
    };
    
    class JsonParser {
    public:
        JsonParser(const std::string& json) : json_(json), pos_(0), line_(1), column_(1) {}
        
        bool parseObject(std::vector<ConfigEntry>& entries) {
            if (!expect(JsonTokenType::LeftBrace)) return false;
            
            while (true) {
                JsonToken token = nextToken();
                if (token.type == JsonTokenType::RightBrace) {
                    break;
                }
                if (token.type != JsonTokenType::String) return false;
                
                std::string key = token.value;
                
                if (!expect(JsonTokenType::Colon)) return false;
                
                ConfigValue value = parseValue();
                if (!value.has_value()) return false;
                
                entries.emplace_back(key, value, ConfigSource::File);
                
                JsonToken comma = nextToken();
                if (comma.type == JsonTokenType::RightBrace) {
                    break;
                } else if (comma.type != JsonTokenType::Comma) {
                    return false;
                }
            }
            
            return true;
        }
        
        bool parseArray(std::vector<ConfigEntry>& entries) {
            if (!expect(JsonTokenType::LeftBracket)) return false;
            
            while (true) {
                JsonToken token = nextToken();
                if (token.type == JsonTokenType::RightBracket) {
                    break;
                }
                
                // For now, arrays are not directly supported as config values
                // but we can parse them as strings for backward compatibility
                std::string arrayStr;
                if (token.type == JsonTokenType::String) {
                    arrayStr = token.value;
                } else if (token.type == JsonTokenType::Number) {
                    arrayStr = token.value;
                } else {
                    return false;
                }
                
                // Treat arrays as strings for now
                entries.emplace_back("array", ConfigValue(arrayStr), ConfigSource::File);
                
                JsonToken comma = nextToken();
                if (comma.type == JsonTokenType::RightBracket) {
                    break;
                } else if (comma.type != JsonTokenType::Comma) {
                    return false;
                }
            }
            
            return true;
        }
        
        ConfigValue parseValue() {
            JsonToken token = nextToken();
            
            switch (token.type) {
                case JsonTokenType::String:
                    return token.value;
                case JsonTokenType::Number: {
                    // Try to parse as integer first
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(token.value, &pos);
                        if (pos == token.value.size()) {
                            return intVal;
                        }
                    } catch (...) {}
                    
                    try {
                        size_t pos;
                        double doubleVal = std::stod(token.value, &pos);
                        if (pos == token.value.size()) {
                            return doubleVal;
                        }
                    } catch (...) {}
                    
                    return token.value; // Fall back to string
                }
                case JsonTokenType::Boolean:
                    return token.value == "true";
                case JsonTokenType::Null:
                    return std::string("null");
                case JsonTokenType::LeftBrace: {
                    // Object - for now treat as string
                    std::string objStr = "{";
                    while (true) {
                        JsonToken t = nextToken();
                        objStr += " " + t.value + " ";
                        if (t.type == JsonTokenType::RightBrace) break;
                        if (t.type == JsonTokenType::Comma) objStr += ",";
                    }
                    objStr += "}";
                    return objStr;
                }
                case JsonTokenType::LeftBracket: {
                    // Array - treat as string
                    std::string arrStr = "[";
                    while (true) {
                        JsonToken t = nextToken();
                        if (t.type == JsonTokenType::RightBracket) break;
                        arrStr += " " + t.value + " ";
                        if (t.type == JsonTokenType::Comma) arrStr += ",";
                    }
                    arrStr += "]";
                    return arrStr;
                }
                default:
                    return std::string("");
            }
        }
        
    private:
        JsonToken nextToken() {
            skipWhitespace();
            
            if (pos_ >= json_.size()) {
                return {JsonTokenType::EndOfStream, "", line_, column_};
            }
            
            char c = json_[pos_];
            
            if (c == '{') {
                column_++;
                pos_++;
                return {JsonTokenType::LeftBrace, "{", line_, column_ - 1};
            } else if (c == '}') {
                column_++;
                pos_++;
                return {JsonTokenType::RightBrace, "}", line_, column_ - 1};
            } else if (c == '[') {
                column_++;
                pos_++;
                return {JsonTokenType::LeftBracket, "[", line_, column_ - 1};
            } else if (c == ']') {
                column_++;
                pos_++;
                return {JsonTokenType::RightBracket, "]", line_, column_ - 1};
            } else if (c == ':') {
                column_++;
                pos_++;
                return {JsonTokenType::Colon, ":", line_, column_ - 1};
            } else if (c == ',') {
                column_++;
                pos_++;
                return {JsonTokenType::Comma, ",", line_, column_ - 1};
            } else if (c == '\"') {
                return parseString();
            } else if (c == 't' || c == 'f') {
                return parseBoolean();
            } else if (c == 'n') {
                return parseNull();
            } else if (std::isdigit(c) || c == '-') {
                return parseNumber();
            } else {
                column_++;
                pos_++;
                return {JsonTokenType::Unknown, std::string(1, c), line_, column_ - 1};
            }
        }
        
        JsonToken parseString() {
            size_t start = pos_;
            pos_++; // Skip opening quote
            column_++;
            
            std::string value;
            while (pos_ < json_.size() && json_[pos_] != '\"') {
                if (json_[pos_] == '\\') {
                    pos_++; // Skip escape character
                    if (pos_ < json_.size()) {
                        value += json_[pos_];
                        pos_++;
                        column_++;
                    }
                } else {
                    value += json_[pos_];
                    pos_++;
                    column_++;
                }
            }
            
            if (pos_ >= json_.size() || json_[pos_] != '\"') {
                return {JsonTokenType::Unknown, "", line_, column_};
            }
            
            pos_++; // Skip closing quote
            column_++;
            
            return {JsonTokenType::String, value, line_, column_ - 1 - static_cast<int>(value.size())};
        }
        
        JsonToken parseBoolean() {
            size_t start = pos_;
            while (pos_ < json_.size() && (json_[pos_] == 't' || json_[pos_] == 'f' || 
                                           json_[pos_] == 'r' || json_[pos_] == 'u' || json_[pos_] == 'e')) {
                pos_++;
                column_++;
            }
            
            std::string tokenStr = json_.substr(start, pos_ - start);
            
            if (tokenStr == "true" || tokenStr == "false") {
                return {JsonTokenType::Boolean, tokenStr, line_, column_ - static_cast<int>(tokenStr.size())};
            } else {
                return {JsonTokenType::Unknown, tokenStr, line_, column_ - static_cast<int>(tokenStr.size())};
            }
        }
        
        JsonToken parseNull() {
            size_t start = pos_;
            while (pos_ < json_.size() && (json_[pos_] == 'n' || json_[pos_] == 'u' || json_[pos_] == 'l')) {
                pos_++;
                column_++;
            }
            
            std::string tokenStr = json_.substr(start, pos_ - start);
            
            if (tokenStr == "null") {
                return {JsonTokenType::Null, tokenStr, line_, column_ - static_cast<int>(tokenStr.size())};
            } else {
                return {JsonTokenType::Unknown, tokenStr, line_, column_ - static_cast<int>(tokenStr.size())};
            }
        }
        
        JsonToken parseNumber() {
            size_t start = pos_;
            bool hasDecimal = false;
            bool hasExponent = false;
            
            if (json_[pos_] == '-') {
                pos_++;
                column_++;
            }
            
            while (pos_ < json_.size() && (std::isdigit(json_[pos_]) || 
                                           json_[pos_] == '.' || 
                                           json_[pos_] == 'e' || json_[pos_] == 'E' ||
                                           json_[pos_] == '+' || json_[pos_] == '-')) {
                if (json_[pos_] == '.') hasDecimal = true;
                else if (json_[pos_] == 'e' || json_[pos_] == 'E') hasExponent = true;
                pos_++;
                column_++;
            }
            
            std::string tokenStr = json_.substr(start, pos_ - start);
            return {JsonTokenType::Number, tokenStr, line_, column_ - static_cast<int>(tokenStr.size())};
        }
        
        void skipWhitespace() {
            while (pos_ < json_.size() && (json_[pos_] == ' ' || json_[pos_] == '\t' || 
                                           json_[pos_] == '\n' || json_[pos_] == '\r')) {
                if (json_[pos_] == '\n') {
                    line_++;
                    column_ = 1;
                } else if (json_[pos_] == '\r') {
                    // Handle Windows line endings
                    if (pos_ + 1 < json_.size() && json_[pos_ + 1] == '\n') {
                        pos_++;
                    }
                    line_++;
                    column_ = 1;
                } else {
                    column_++;
                }
                pos_++;
            }
        }
        
        bool expect(JsonTokenType type) {
            JsonToken token = nextToken();
            return token.type == type;
        }
        
        std::string json_;
        size_t pos_;
        size_t line_;
        size_t column_;
    };
    
    // Simple YAML parser (very basic implementation)
    bool parseYamlObject(const std::string& yaml, std::vector<ConfigEntry>& entries) {
        std::istringstream stream(yaml);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip comments and empty lines
            line = Config::trim(line);
            if (line.empty() || line.substr(0, 1) == "#") {
                continue;
            }
            
            // Simple key=value format
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to infer type
                ConfigValue configValue;
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else {
                    try {
                        size_t pos_int;
                        int64_t intVal = std::stoll(value, &pos_int);
                        if (pos_int == value.size()) {
                            configValue = intVal;
                        } else {
                            try {
                                size_t pos_double;
                                double doubleVal = std::stod(value, &pos_double);
                                if (pos_double == value.size()) {
                                    configValue = doubleVal;
                                } else {
                                    configValue = value;
                                }
                            } catch (...) {
                                configValue = value;
                            }
                        }
                    } catch (...) {
                        configValue = value;
                    }
                }
                
                entries.emplace_back(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    // Reset entries for clean load
    pImpl->entries.clear();
    
    if (extension == ".json") {
        pImpl->useJsonFormat = true;
        // Read JSON file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonContent = buffer.str();
        
        // Parse JSON
        JsonParser parser(jsonContent);
        return parser.parseObject(pImpl->entries);
    } else if (extension == ".yaml" || extension == ".yml") {
        pImpl->useJsonFormat = true;
        // Read YAML file (simplified - treating as key=value)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open YAML config file: " + filepath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string yamlContent = buffer.str();
        
        // Parse YAML (simplified implementation)
        return parseYamlObject(yamlContent, pImpl->entries);
    } else {
        pImpl->useJsonFormat = false;
        // Use original simple key=value format
        return Config::loadFromFile(filepath);
    }
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
    
    if (pImpl->useJsonFormat) {
        // Save in JSON format
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        file << val[j];
                        if (j < val.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        file << val[j];
                        if (j < val.size() - 1) file << ", ";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        file << "\"" << val[j] << "\"";
                        if (j < val.size() - 1) file << ", ";
                    }
                    file << "]";
                } else {
                    file << "null";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
    } else {
        // Save in key=value format (original)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else {
                    file << "UNKNOWN_TYPE";
                }
            }, entry.value);
            
            file << "\n\n";
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
