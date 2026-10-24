#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first
    if (filepath.size() > 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath);
    } else if (filepath.size() > 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromSimpleFile(filepath);
    }
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
}

// Helper functions for JSON/YAML parsing
namespace nlm {
    bool isJSONFile(const std::string& filepath) {
        if (filepath.size() < 5) return false;
        std::string ext = filepath.substr(filepath.size() - 5);
        return ext == ".json" || ext == ".JSON";
    }

    bool isYAMLFile(const std::string& filepath) {
        if (filepath.size() < 5) return false;
        std::string ext = filepath.substr(filepath.size() - 5);
        return ext == ".yaml" || ext == ".yml";
    }

    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }

    std::string extractString(const std::string& str, size_t& pos) {
        if (pos >= str.size()) return "";
        char quote = str[pos];
        if (quote != '"' && quote != '\'') return "";
        size_t endPos = str.find(quote, pos + 1);
        if (endPos == std::string::npos) return "";
        pos = endPos + 1;
        return str.substr(pos - 1, endPos - pos + 2);
    }

    double parseNumber(const std::string& str, size_t& pos) {
        size_t endPos = str.find_first_of(" ,}\]\",", pos);
        std::string numStr = str.substr(pos, endPos - pos);
        pos = endPos;
        try {
            return std::stod(numStr);
        } catch (...) {
            return 0.0;
        }
    }

    bool parseBool(const std::string& str, size_t& pos) {
        if (startsWith(str.substr(pos), "true") || startsWith(str.substr(pos), "True") || startsWith(str.substr(pos), "TRUE")) {
            pos += 4;
            return true;
        } else if (startsWith(str.substr(pos), "false") || startsWith(str.substr(pos), "False") || startsWith(str.substr(pos), "FALSE")) {
            pos += 5;
            return false;
        }
        return false;
    }

    std::string extractIdentifier(const std::string& str, size_t& pos) {
        size_t endPos = str.find_first_of(" :\",[}\]\", pos);
        std::string ident = str.substr(pos, endPos - pos);
        pos = endPos;
        return trim(ident);
    }

    bool skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
        return pos < str.size();
    }

    std::string extractKey(const std::string& str, size_t& pos) {
        skipWhitespace(str, pos);
        if (pos >= str.size() || str[pos] != '"') return "";
        size_t quotePos = str.find('"', pos + 1);
        if (quotePos == std::string::npos) return "";
        std::string key = str.substr(pos + 1, quotePos - pos - 1);
        pos = quotePos + 1;
        return key;
    }

    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c; break;
            }
        }
        return result;
    }

    std::string unescapeString(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[i + 1]) {
                    case '\"': result += '\"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    case 'n': result += '\n'; i++; break;
                    case 'r': result += '\r'; i++; break;
                    case 't': result += '\t'; i++; break;
                    default: result += str[i + 1]; i++; break;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }

    bool isValidIdentifier(const std::string& str) {
        if (str.empty()) return false;
        if (!std::isalpha(str[0]) && str[0] != '_') return false;
        for (char c : str) {
            if (!std::isalnum(c) && c != '_') return false;
        }
        return true;
    }

    std::vector<std::string> splitString(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            result.push_back(trim(token));
        }
        return result;
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S")
           << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }
// Add the implementation methods before the namespace closing

    bool Config::loadFromSimpleFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }

            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));

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

    bool Config::loadFromJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        return parseFromJSON(content);
    }

    bool Config::loadFromYAML(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        return parseFromYAML(content);
    }

    bool Config::parseFromJSON(const std::string& jsonStr) {
        if (jsonStr.empty()) return false;

        size_t pos = 0;
        skipWhitespace(jsonStr, pos);

        if (pos >= jsonStr.size() || jsonStr[pos] != '{') {
            return false;
        }

        pos++; // Skip opening brace
        while (pos < jsonStr.size()) {
            skipWhitespace(jsonStr, pos);

            if (pos >= jsonStr.size()) break;

            if (jsonStr[pos] == '}') {
                pos++;
                break;
            }

            // Parse key
            std::string key = extractKey(jsonStr, pos);
            if (key.empty()) break;

            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                return false;
            }
            pos++;

            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size()) break;

            // Parse value
            ConfigValue value = parseJSONValue(jsonStr, pos);
            set(key, value, ConfigSource::File);

            skipWhitespace(jsonStr, pos);
            if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                pos++;
            }
        }

        return true;
    }

    bool Config::parseFromYAML(const std::string& yamlStr) {
        std::istringstream stream(yamlStr);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) continue;

            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));

            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }

            set(key, value, ConfigSource::File);
        }

        return true;
    }

    ConfigValue Config::parseJSONValue(const std::string& jsonStr, size_t& pos) {
        skipWhitespace(jsonStr, pos);

        if (pos >= jsonStr.size()) {
            return "";
        }

        char c = jsonStr[pos];
        if (c == '"' || c == '\'') {
            std::string result = extractString(jsonStr, pos);
            if (!result.empty() && result.front() == '\"') {
                result = unescapeString(result.substr(1, result.size() - 2));
            }
            return result;
        } else if (std::isdigit(c) || c == '-') {
            double result = parseNumber(jsonStr, pos);
            if (result == static_cast<int>(result)) {
                return static_cast<int>(result);
            }
            return result;
        } else if (c == 't' || c == 'f') {
            bool result = parseBool(jsonStr, pos);
            return result;
        } else if (c == 'n') {
            std::string word = extractIdentifier(jsonStr, pos);
            if (word == "null") {
                return "null";
            }
            return "";
        } else if (c == '[') {
            pos++; // Skip opening bracket
            std::vector<ConfigValue> array;

            skipWhitespace(jsonStr, pos);
            if (pos < jsonStr.size() && jsonStr[pos] == ']') {
                pos++; // Skip closing bracket
                return array;
            }

            while (pos < jsonStr.size()) {
                ConfigValue element = parseJSONValue(jsonStr, pos);
                array.push_back(element);

                skipWhitespace(jsonStr, pos);
                if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                    pos++;
                }

                skipWhitespace(jsonStr, pos);
                if (pos < jsonStr.size() && jsonStr[pos] == ']') {
                    pos++;
                    break;
                }
            }

            return array;
        } else {
            std::string result = extractIdentifier(jsonStr, pos);
            return result;
        }
    }

    std::string Config::serializeToJSON() const {
        std::ostringstream json;
        json << "{\n";

        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) json << ",\n";
            first = false;

            json << "  \"" << escapeString(entry.key) << "\": ";

            std::visit([&json](const auto& value) {
                using T = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    json << value;
                } else if constexpr (std::is_same_v<T, double>) {
                    json << std::fixed << std::setprecision(6) << value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    json << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    json << "\"" << escapeString(value) << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    json << "[";
                    bool firstElement = true;
                    for (int val : value) {
                        if (!firstElement) json << ", ";
                        firstElement = false;
                        json << val;
                    }
                    json << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    json << "[";
                    bool firstElement = true;
                    for (double val : value) {
                        if (!firstElement) json << ", ";
                        firstElement = false;
                        json << std::fixed << std::setprecision(6) << val;
                    }
                    json << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    json << "[";
                    bool firstElement = true;
                    for (const std::string& val : value) {
                        if (!firstElement) json << ", ";
                        firstElement = false;
                        json << "\"" << escapeString(val) << "\"";
                    }
                    json << "]";
                } else {
                    json << "null";
                }
            }, entry.value);
        }

        json << "\n}";
        return json.str();
    }

    std::string Config::serializeToYAML() const {
        std::ostringstream yaml;

        for (const auto& entry : pImpl->entries) {
            yaml << entry.key << ": ";

            std::visit([&yaml](const auto& value) {
                using T = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    yaml << value;
                } else if constexpr (std::is_same_v<T, double>) {
                    yaml << std::fixed << std::setprecision(6) << value;
                } else if constexpr (std::is_same_v<T, bool>) {
                    yaml << (value ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (value.find(' ') != std::string::npos || value.find('\t') != std::string::npos) {
                        yaml << "\"" << escapeString(value) << "\"";
                    } else {
                        yaml << value;
                    }
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    yaml << "[";
                    bool first = true;
                    for (int val : value) {
                        if (!first) yaml << ", ";
                        first = false;
                        yaml << val;
                    }
                    yaml << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    yaml << "[";
                    bool first = true;
                    for (double val : value) {
                        if (!first) yaml << ", ";
                        first = false;
                        yaml << std::fixed << std::setprecision(6) << val;
                    }
                    yaml << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    yaml << "[";
                    bool first = true;
                    for (const std::string& val : value) {
                        if (!first) yaml << ", ";
                        first = false;
                        if (val.find(' ') != std::string::npos || val.find('\t') != std::string::npos) {
                            yaml << "\"" << escapeString(val) << "\"";
                        } else {
                            yaml << val;
                        }
                    }
                    yaml << "]";
                } else {
                    yaml << "null";
                }
            }, entry.value);

            yaml << "\n";
        }

        return yaml.str();
    }

    bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
        if (key.empty()) return false;

        if (!isValidIdentifier(key)) return false;

        if (key.find("password") != std::string::npos || 
            key.find("secret") != std::string::npos) {
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    if (val.size() < 8) {
                        throw std::runtime_error("Password too short: minimum 8 characters");
                    }
                }
            }, value);
        }

        return true;
    }

    std::string Config::getValueTypeName(const ConfigValue& value) const {
        std::string typeName;
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                typeName = "integer";
            } else if constexpr (std::is_same_v<T, double>) {
                typeName = "float";
            } else if constexpr (std::is_same_v<T, bool>) {
                typeName = "boolean";
            } else if constexpr (std::is_same_v<T, std::string>) {
                typeName = "string";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                typeName = "integer_array";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                typeName = "float_array";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                typeName = "string_array";
            } else {
                typeName = "unknown";
            }
        }, value);
        return typeName;
    }
}

// Implementation functions that were declared as friend

ConfigValue parseJSONValue(const std::string& jsonStr, size_t& pos) {
    nlm::Config tempConfig;
    return tempConfig.parseJSONValue(jsonStr, pos);
}

std::string serializeToJSON(const nlm::Config& config) {
    return config.serializeToJSON();
}

std::string serializeToYAML(const nlm::Config& config) {
    return config.serializeToYAML();
}

bool validateValue(const nlm::Config& config, const std::string& key, const nlm::ConfigValue& value) {
    return config.validateValue(key, value);
}

std::string getValueTypeName(const nlm::Config& config, const nlm::ConfigValue& value) {
    return config.getValueTypeName(value);
}
