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
    // Try JSON format first (Phase 2 enhancement)
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
        file << entry.key << " = ";
        
        // Properly format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<int>> ||
                                  std::is_same_v<T, std::vector<double>>) {
                        file << item;
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << item << "\"";
                    }
                    first = false;
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Phase 2: JSON support
bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parser for basic types
    // Supports: strings, numbers, booleans, arrays of basic types
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Skip BOM if present
    if (content.size() >= 3 && content[0] == '\xEF' && content[1] == '\xBB' && content[2] == '\xBF') {
        content = content.substr(3);
    }
    
    // Parse JSON object
    size_t pos = 0;
    return parseJSONObject(content, pos);
}

bool Config::parseJSONObject(const std::string& json, size_t& pos) {
    if (json[pos] != '{') {
        return false;
    }
    ++pos;
    
    while (pos < json.size() && json[pos] != '}') {
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
        
        if (pos >= json.size()) break;
        
        // Parse key
        if (!parseJSONString(json, pos)) {
            return false;
        }
        
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
        
        if (pos >= json.size() || json[pos] != ':') {
            return false;
        }
        ++pos;
        
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
        
        // Parse value
        if (!parseJSONValue(json, pos)) {
            return false;
        }
        
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
        
        // Expect comma or closing brace
        if (pos < json.size() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.size() && json[pos] != '}') {
            return false;
        }
    }
    
    if (pos < json.size() && json[pos] == '}') {
        ++pos;
        return true;
    }
    
    return false;
}

bool Config::parseJSONString(const std::string& json, size_t& pos) {
    if (pos >= json.size() || json[pos] != '"') {
        return false;
    }
    ++pos;
    
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            ++pos; // Skip escaped character
        }
        ++pos;
    }
    
    if (pos < json.size() && json[pos] == '"') {
        ++pos;
        return true;
    }
    
    return false;
}

std::string Config::extractString(const std::string& json, size_t& pos) {
    size_t start = pos;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            pos += 2; // Skip escaped character
        } else {
            ++pos;
        }
    }
    
    std::string result = json.substr(start + 1, pos - start - 1);
    if (pos < json.size() && json[pos] == '"') {
        ++pos;
    }
    
    return result;
}

bool Config::parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& value) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    while (pos < json.size() && (std::isdigit(json[pos]) ||
                                 json[pos] == '.' || json[pos] == '-' ||
                                 json[pos] == 'e' || json[pos] == 'E')) {
        if (json[pos] == '.') hasDecimal = true;
        if (json[pos] == 'e' || json[pos] == 'E') hasExponent = true;
        ++pos;
    }
    
    if (pos == start) return false;
    
    std::string numStr = json.substr(start, pos - start);
    
    // Try to parse as different types
    if (numStr.find('.') != std::string::npos || hasExponent) {
        try {
            double d = std::stod(numStr);
            value = d;
            return true;
        } catch (...) {
            return false;
        }
    } else {
        try {
            // Check if it fits in int64_t
            size_t i = 0;
            bool positive = true;
            if (numStr[0] == '-') {
                positive = false;
                i = 1;
            }
            
            int64_t val = 0;
            for (; i < numStr.size(); ++i) {
                val = val * 10 + (numStr[i] - '0');
                if (!positive) val = -val;
            }
            
            value = val;
            return true;
        } catch (...) {
            return false;
        }
    }
}

bool Config::parseJSONBool(const std::string& json, size_t& pos, ConfigValue& value) {
    if (pos + 4 < json.size() && json.substr(pos, 4) == "true") {
        value = true;
        pos += 4;
        return true;
    } else if (pos + 5 < json.size() && json.substr(pos, 5) == "false") {
        value = false;
        pos += 5;
        return true;
    }
    return false;
}

bool Config::parseJSONArray(const std::string& json, size_t& pos, ConfigValue& value) {
    if (pos >= json.size() || json[pos] != '[') {
        return false;
    }
    ++pos;
    
    std::vector<ConfigValue> array;
    
    while (pos < json.size() && json[pos] != ']') {
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
        
        if (pos >= json.size()) return false;
        
        ConfigValue item;
        if (!parseJSONValue(json, pos, item)) {
            return false;
        }
        
        array.push_back(std::move(item));
        
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) ++pos;
        
        if (pos < json.size() && json[pos] == ',') {
            ++pos;
        } else if (pos < json.size() && json[pos] != ']') {
            return false;
        }
    }
    
    if (pos < json.size() && json[pos] == ']') {
        ++pos;
        value = std::move(array);
        return true;
    }
    
    return false;
}

bool Config::parseJSONValue(const std::string& json, size_t& pos, ConfigValue& value) {
    // Skip whitespace
    while (pos < json.size() && std::isspace(json[pos])) ++pos;
    
    if (pos >= json.size()) return false;
    
    char c = json[pos];
    
    if (c == '"') {
        // String
        std::string str = extractString(json, pos);
        value = str;
        return true;
    } else if (c == '{') {
        // Object (not directly supported for now, fall back to simple format)
        return false;
    } else if (c == '[') {
        return parseJSONArray(json, pos, value);
    } else if (c == 't' || c == 'f') {
        return parseJSONBool(json, pos, value);
    } else if (c == '-' || std::isdigit(c)) {
        return parseJSONNumber(json, pos, value);
    } else if (c == 'n' && pos + 1 < json.size() && json.substr(pos, 3) == "null") {
        pos += 3;
        return true; // null is treated as empty value
    }
    
    return false;
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
