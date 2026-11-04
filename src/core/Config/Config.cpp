#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <string_view>
#include "../Logger/Logger.hpp"

namespace nlm {

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON parsing
    if (parseJSON(content)) {
        return true;
    }
    
    // Fall back to simple key=value format
    file.open(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to reopen config file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
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
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Write JSON header
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Serialize based on value type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(val) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, double> || 
                                 std::is_same_v<T, float>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << escapeString(val[i]) << "\"";
                    } else {
                        file << val[i];
                    }
                }
                file << "]";
            } else {
                file << "\"" << escapeString(std::to_string(val)) << "\"";
            }
        }, entry.value);
        
        file << " // Source: " << static_cast<int>(entry.source);
        if (!entry.description.empty()) {
            file << " (" << entry.description << ")";
        }
    }
    
    file << "\n}\n";
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

bool Config::parseJSON(const std::string& json) {
    // Simple JSON parser for basic key-value pairs
    // This is a simplified implementation - for production use, consider a proper JSON library
    
    try {
        size_t pos = 0;
        while (pos < json.size()) {
            // Find key
            pos = json.find('"', pos);
            if (pos == std::string::npos) break;
            
            size_t keyEnd = json.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = json.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Find value
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (pos >= json.size() || json[pos] != ':') break;
            pos++;
            
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (pos >= json.size()) break;
            
            // Parse value based on type
            ConfigValue value;
            bool parsed = false;
            
            if (json[pos] == '"') {
                // String value
                size_t valueEnd = json.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                std::string strValue = json.substr(pos + 1, valueEnd - pos - 1);
                unescapeString(strValue);
                set(key, strValue, ConfigSource::File);
                parsed = true;
                pos = valueEnd + 1;
            } else if (json.compare(pos, 4, "true") == 0) {
                // Boolean true
                set(key, true, ConfigSource::File);
                pos += 4;
                parsed = true;
            } else if (json.compare(pos, 5, "false") == 0) {
                // Boolean false
                set(key, false, ConfigSource::File);
                pos += 5;
                parsed = true;
            } else if (std::isdigit(json[pos]) || json[pos] == '-' || json[pos] == '+') {
                // Numeric value
                size_t numEnd = pos;
                while (numEnd < json.size() && (std::isdigit(json[numEnd]) || 
                                              json[numEnd] == '.' || json[numEnd] == 'e' || 
                                              json[numEnd] == 'E' || json[numEnd] == '-')) {
                    numEnd++;
                }
                std::string numStr = json.substr(pos, numEnd - pos);
                try {
                    // Try to parse as integer first
                    try {
                        set(key, std::stoll(numStr), ConfigSource::File);
                    } catch (...) {
                        // Fall back to double
                        set(key, std::stod(numStr), ConfigSource::File);
                    }
                    parsed = true;
                    pos = numEnd;
                } catch (...) {
                    // Invalid number, skip
                }
            } else if (json[pos] == '[') {
                // Array value (simplified)
                // For now, skip array values
                size_t arrayEnd = json.find(']', pos);
                if (arrayEnd == std::string::npos) break;
                pos = arrayEnd + 1;
                parsed = true;
            }
            
            if (!parsed) {
                // Skip to next comma or brace
                while (pos < json.size() && json[pos] != ',' && json[pos] != '}') pos++;
            }
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("JSON parsing error: ") + e.what());
        return false;
    }
}

std::string Config::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\"': result += '"'; i++; break;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
