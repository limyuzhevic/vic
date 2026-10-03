#include "ConfigEnhanced.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cmath>
#include <cctype>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex;
    std::chrono::system_clock::time_point lastModified;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastModified = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry entry(key, value, source);
        pImpl->entries.push_back(std::move(entry));
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
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

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->lastModified = std::chrono::system_clock::now();
}

bool Config::loadFromFile(const std::string& filepath, const ConfigSchema* schema) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
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
    
    if (schema) {
        applySchema(*schema);
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
        file << entry.key << " = " << entry.toString() << "\n\n";
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, const ConfigSchema* schema) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    if (schema) {
        applySchema(*schema);
    }
    
    return true;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << entry.toString() << "] (" 
            << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::vector<std::string> Config::validate(const ConfigSchema* schema) const {
    std::vector<std::string> errors;
    
    if (schema) {
        std::map<std::string, ConfigValue> configMap;
        for (const auto& entry : pImpl->entries) {
            configMap[entry.key] = entry.value;
        }
        return schema->validateAll(configMap);
    }
    
    return errors;
}

bool Config::validateEntry(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    return it != pImpl->entries.end() && it->validate();
}

bool Config::applySchema(const ConfigSchema& schema) {
    bool changed = false;
    
    for (const auto& schemaEntry : schema.entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&schemaEntry](const ConfigEntry& e) { return e.key == schemaEntry.key; });
        
        if (it != pImpl->entries.end()) {
            if (it->value != schemaEntry.value) {
                it->value = schemaEntry.value;
                changed = true;
            }
        } else if (schemaEntry.required) {
            ConfigEntry entry = schemaEntry;
            entry.source = ConfigSource::Default;
            pImpl->entries.push_back(entry);
            changed = true;
        }
    }
    
    if (changed) {
        pImpl->lastModified = std::chrono::system_clock::now();
    }
    
    return changed;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

void Config::override(const Config& other) {
    merge(other, ConfigSource::Override);
}

std::string Config::exportToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"config\": {\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "    \"" << entry.key << "\": " << escapeJSONString(entry.toString());
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  }\n";
    oss << "}\n";
    return oss.str();
}

bool Config::importFromJSON(const std::string& json, const ConfigSchema* schema) {
    // Simplified JSON parsing - in real implementation would use a proper JSON parser
    clear();
    
    // For now, just use simple key=value parsing
    size_t pos = 0;
    while (pos < json.length()) {
        size_t keyStart = json.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = json.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = json.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        size_t valueStart = json.find('"', keyEnd + 1);
        if (valueStart == std::string::npos) break;
        
        size_t valueEnd = json.find('"', valueStart + 1);
        if (valueEnd == std::string::npos) break;
        
        std::string value = json.substr(valueStart + 1, valueEnd - valueStart - 1);
        
        set(key, value, ConfigSource::File);
        
        pos = valueEnd + 1;
    }
    
    if (schema) {
        applySchema(*schema);
    }
    
    return true;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                   [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

size_t Config::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

std::chrono::system_clock::time_point Config::getLastModified() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->lastModified;
}

ConfigSource Config::getSource(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    return it != pImpl->entries.end() ? it->source : ConfigSource::Default;
}

template<typename T>
std::optional<T> Config::convertTo(const std::string& str) {
    // Implementation would be specialized for different types
    return std::nullopt;
}

namespace ConfigUtils {
    bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    bool directoryExists(const std::string& path) {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    }
    
    bool ensureDirectoryExists(const std::string& path) {
        try {
            if (!std::filesystem::exists(path)) {
                return std::filesystem::create_directories(path);
            }
            return true;
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
    std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.rfind('.');
        return pos != std::string::npos ? filename.substr(pos) : "";
    }
    
    std::string getFileNameWithoutExtension(const std::string& filename) {
        size_t pos = filename.rfind('.');
        return pos != std::string::npos ? filename.substr(0, pos) : filename;
    }
    
    std::string getAbsolutePath(const std::string& path) {
        return std::filesystem::absolute(path).string();
    }
    
    std::string escapeString(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default: oss << c; break;
            }
        }
        return oss.str();
    }
    
    std::string unescapeString(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[i + 1]) {
                    case '\"': result += '\"'; i++; break;
                    case '\\': result += '\\'; i++; break;
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
    
    bool isWhitespace(char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }
    
    std::string removeWhitespace(const std::string& str) {
        std::string result;
        result.reserve(str.length());
        for (char c : str) {
            if (!isWhitespace(c)) {
                result += c;
            }
        }
        return result;
    }
    
    std::string toString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                oss << val;
            } else if constexpr (std::is_same_v<decltype(val), int>) {
                oss << val;
            } else if constexpr (std::is_same_v<decltype(val), int64_t>) {
                oss << val;
            } else if constexpr (std::is_same_v<decltype(val), double>) {
                oss << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                oss << (val ? "true" : "false");
            }
        }, value);
        return oss.str();
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    bool parseBoolean(const std::string& str) {
        std::string lower = toLower(str);
        return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
    }
    
    int parseInt(const std::string& str) {
        return std::stoi(str);
    }
    
    double parseDouble(const std::string& str) {
        return std::stod(str);
    }
    
    bool isValidKey(const std::string& key) {
        if (key.empty()) return false;
        return std::all_of(key.begin(), key.end(),
                          [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
    }
    
    bool isValidValue(const std::string& value, const std::string& type) {
        return true; // Simplified validation
    }
    
    void mergeConfigs(Config& target, const Config& source, ConfigSource sourceType) {
        target.merge(source, sourceType);
    }
    
    std::string escapeJSON(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (static_cast<unsigned char>(c) < 32) {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                    } else {
                        oss << c;
                    }
                    break;
            }
        }
        return oss.str();
    }
    
    std::string unescapeJSON(const std::string& str) {
        std::string result;
        for (size_t i = 0; i < str.length(); ++i) {
            if (str[i] == '\\' && i + 1 < str.length()) {
                switch (str[i + 1]) {
                    case '\"': result += '\"'; i++; break;
                    case '\\': result += '\\'; i++; break;
                    case '/': result += '/'; i++; break;
                    case 'b': result += '\b'; i++; break;
                    case 'f': result += '\f'; i++; break;
                    case 'n': result += '\n'; i++; break;
                    case 'r': result += '\r'; i++; break;
                    case 't': result += '\t'; i++; break;
                    case 'u': {
                        if (i + 5 < str.length()) {
                            std::string hex = str.substr(i + 2, 4);
                            try {
                                char32_t codePoint = static_cast<char32_t>(std::stoul(hex, nullptr, 16));
                                result += static_cast<char>(codePoint);
                                i += 5;
                            } catch (...) {
                                result += str[i];
                            }
                        } else {
                            result += str[i];
                        }
                        break;
                    }
                    default: result += str[i]; break;
                }
            } else {
                result += str[i];
            }
        }
        return result;
    }
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
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