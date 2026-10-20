#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // Now implementing JSON parser for Phase 6
    return loadFromJSONFile(filepath);
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing - in production, use nlohmann::json
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Remove whitespace and comments
    std::string cleanContent;
    bool inString = false;
    char quoteChar = '\0';
    
    for (size_t i = 0; i < content.size(); ++i) {
        char c = content[i];
        
        if (!inString && (c == '/' && i + 1 < content.size() && content[i + 1] == '/')) {
            // Skip line comment
            while (i + 1 < content.size() && content[i + 1] != '\n') {
                i++;
            }
            continue;
        }
        
        if (!inString && c == '/' && i + 1 < content.size() && content[i + 1] == '*') {
            // Skip block comment
            i += 2;
            while (i + 1 < content.size() && !(content[i] == '*' && content[i + 1] == '/')) {
                i++;
            }
            continue;
        }
        
        if (c == '\"' || c == '\'') {
            inString = !inString;
            quoteChar = c;
            cleanContent += c;
        } else if (!inString && (c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
            // Keep whitespace inside objects
            cleanContent += c;
        } else {
            cleanContent += c;
        }
    }
    
    // Parse JSON objects
    return parseJSONObject(cleanContent);
}

bool Config::parseJSONObject(const std::string& json) {
    size_t pos = 0;
    while (pos < json.size()) {
        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t')) {
            pos++;
        }
        
        if (pos >= json.size()) break;
        
        // Parse key
        size_t keyStart = pos;
        while (pos < json.size() && json[pos] != ':' && json[pos] != '{' && json[pos] != '}') {
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != ':') {
            break; // Malformed JSON
        }
        
        std::string key = trim(json.substr(keyStart, pos - keyStart));
        pos++; // Skip colon
        
        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t')) {
            pos++;
        }
        
        // Parse value
        std::string value;
        bool isString = false;
        if (pos < json.size() && (json[pos] == '\"' || json[pos] == '\'')) {
            isString = true;
            char quoteChar = json[pos];
            pos++;
            size_t valueStart = pos;
            while (pos < json.size() && json[pos] != quoteChar) {
                if (json[pos] == '\\' && pos + 1 < json.size()) {
                    pos += 2; // Skip escaped character
                }
                pos++;
            }
            value = json.substr(valueStart, pos - valueStart);
            pos++; // Skip closing quote
        } else if (pos < json.size() && json[pos] == '{') {
            // Object - for now, treat as string
            value = json.substr(pos, 1);
            pos++;
        } else if (pos < json.size() && json[pos] == '[') {
            // Array - for now, treat as string
            size_t arrayEnd = json.find(']', pos);
            if (arrayEnd != std::string::npos) {
                value = json.substr(pos, arrayEnd - pos + 1);
                pos = arrayEnd + 1;
            }
        } else if (pos < json.size() && (json[pos] == '-' || isdigit(json[pos]))) {
            // Number
            size_t valueStart = pos;
            while (pos < json.size() && (isdigit(json[pos]) || json[pos] == '-' || json[pos] == '.' || json[pos] == 'e' || json[pos] == 'E' || json[pos] == '+')) {
                pos++;
            }
            value = json.substr(valueStart, pos - valueStart);
        } else if (pos < json.size() && (json[pos] == 't' || json[pos] == 'f')) {
            // Boolean
            if (json.substr(pos, 4) == "true" || json.substr(pos, 5) == "false") {
                value = json.substr(pos, json.substr(pos, 4) == "true" ? 4 : 5);
                pos += value.size();
            }
        } else {
            // Null or empty
            value = "null";
        }
        
        // Set configuration value
        set(key, value);
        
        // Skip whitespace
        while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r' || json[pos] == '\t' || json[pos] == ',' || json[pos] == ';')) {
            pos++;
        }
    }
    
    return true;
}

// Helper function to trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\v\f");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r\v\f");
    return str.substr(start, end - start + 1);
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
