#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>  // For faster lookup
#include <mutex>  // For thread safety
#include <iomanip>  // For precision control

namespace nlm {

// Helper function to escape JSON strings
static std::string escapeJsonString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '/': result += "\\/"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < ' ' || c == 0x7F) {
                    // Control characters
                    result += "\\u";
                    result += "000" + std::to_string(c & 0xFF);
                } else {
                    result += c;
                }
        }
    }
    return result;
}

// Helper function to unescape JSON strings
static std::string unescapeJsonString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '/': result += '/'; i++; break;
                case 'b': result += '\b'; i++; break;
                case 'f': result += '\f'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                case 'u':
                    if (i + 5 < str.size()) {
                        // Parse unicode escape sequence
                        std::string hex = str.substr(i + 2, 4);
                        try {
                            unsigned int code = std::stoul(hex, nullptr, 16);
                            result += static_cast<char>(code);
                            i += 5;
                        } catch (...) {
                            result += '?'; i += 5;
                        }
                    } else {
                        result += '?'; i += 5;
                    }
                    break;
                default: result += str[i + 1]; i++; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

// Improved parsing function
static bool parseJsonLine(const std::string& line, std::string& key, ConfigValue& value) {
    key.clear();
    
    size_t commentPos = line.find('#');
    if (commentPos != std::string::npos) {
        // Remove comment
        std::string withoutComment = line.substr(0, commentPos);
        
        // Trim whitespace
        size_t start = withoutComment.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return false;
        size_t end = withoutComment.find_last_not_of(" \t\r\n");
        if (start > end) return false;
        withoutComment = withoutComment.substr(start, end - start + 1);
        
        // Parse key=value
        size_t eqPos = withoutComment.find('=');
        if (eqPos == std::string::npos) return false;
        
        key = withoutComment.substr(0, eqPos);
        std::string valueStr = withoutComment.substr(eqPos + 1);
        
        // Trim key and value
        start = key.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return false;
        end = key.find_last_not_of(" \t\r\n");
        if (start > end) return false;
        key = key.substr(start, end - start + 1);
        
        start = valueStr.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            // Empty value - default to empty string
            value = std::string();
            return !key.empty();
        }
        end = valueStr.find_last_not_of(" \t\r\n\"'");
        if (start > end) return false;
        valueStr = valueStr.substr(start, end - start + 1);
        
        // Remove surrounding quotes
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
            valueStr = unescapeJsonString(valueStr);
        }
        
        // Parse boolean values
        if (valueStr == "true" || valueStr == "True" || valueStr == "TRUE") {
            value = true;
        } else if (valueStr == "false" || valueStr == "False" || valueStr == "FALSE") {
            value = false;
        } else if (valueStr == "null" || valueStr == "None") {
            // null/None treated as empty string for compatibility
            value = std::string();
        } else {
            // Try to parse as number
            size_t pos;
            // Try integer
            try {
                long long intVal = std::stoll(valueStr);
                value = intVal;
            } catch (...) {
                // Try double
                try {
                    double doubleVal = std::stod(valueStr);
                    value = doubleVal;
                } catch (...) {
                    // Default to string
                    value = valueStr;
                }
            }
        }
        
        return !key.empty();
    }
    return false;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable std::mutex mutex;  // For thread safety
    std::map<std::string, size_t> keyIndex;  // For faster lookup
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate filepath for security
    std::string normalizedPath = filepath;
    for (char& c : normalizedPath) {
        if (c == '\\') c = '/';
    }
    
    // Check for path traversal attempts
    if (normalizedPath.find("../") != std::string::npos || 
        normalizedPath.find("..\\") != std::string::npos) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath) || 
        !std::filesystem::is_regular_file(filepath)) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) continue;
        
        // Parse JSON-like key=value pairs
        std::string key;
        ConfigValue value;
        
        if (!parseJsonLine(line, key, value)) {
            // Try simple key=value format for backward compatibility
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                // Invalid line format
                continue;
            }
            
            std::string simpleKey = trim(line.substr(0, pos));
            std::string simpleValue = trim(line.substr(pos + 1));
            
            // Remove quotes from simple value
            if (simpleValue.size() >= 2 && 
                ((simpleValue.front() == '"' && simpleValue.back() == '"') ||
                 (simpleValue.front() == '\'' && simpleValue.back() == '\''))) {
                simpleValue = simpleValue.substr(1, simpleValue.size() - 2);
            }
            
            key = simpleKey;
            value = simpleValue;
            
            // Parse boolean values
            if (simpleValue == "true") value = true;
            else if (simpleValue == "false") value = false;
            else if (simpleValue == "null") value = std::string();
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!argv || argc <= 0) return false;
    
    std::string lastKey;
    bool hasError = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.find("=") != std::string::npos) {
            size_t pos = arg.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = trim(arg.substr(0, pos));
            std::string value = trim(arg.substr(pos + 1));
            
            // Handle --key value format (key without =)
            if (key.empty() || key == "=") {
                if (!lastKey.empty() && i + 1 < argc) {
                    value = argv[++i];
                    key = lastKey;
                } else {
                    hasError = true;
                    continue;
                }
            }
            
            lastKey = key;
            set(key, value, ConfigSource::CommandLine);
            
        } else if (arg[0] == '-') {
            // Handle -key value format
            if (i + 1 >= argc) {
                hasError = true;
                continue;
            }
            
            std::string key = trim(arg.substr(1));
            std::string value = trim(argv[++i]);
            
            if (key.empty()) {
                hasError = true;
                continue;
            }
            
            lastKey = key;
            set(key, value, ConfigSource::CommandLine);
            
        } else {
            // Standalone argument - treat as value for last key
            if (!lastKey.empty()) {
                set(lastKey, arg, ConfigSource::CommandLine);
            } else {
                hasError = true;
                continue;
            }
        }
    }
    
    return !hasError;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate filepath for security
    std::string normalizedPath = filepath;
    for (char& c : normalizedPath) {
        if (c == '\\') c = '/';
    }
    
    // Check for path traversal attempts
    if (normalizedPath.find("../") != std::string::npos || 
        normalizedPath.find("..\\") != std::string::npos) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create directory if it doesn't exist
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path.parent_path())) {
        std::filesystem::create_directories(path.parent_path());
    }
    
    for (const auto& entry : pImpl->entries) {
        // Write comment if present
        if (!entry.description.empty()) {
            file << "# " << entry.description << "\n";
        }
        
        // Write key=value pair
        file << entry.key << " = ";
        
        // Convert value to string representation
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJsonString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Use fixed precision to avoid scientific notation
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";  // Simple array representation
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeJsonString(arg[i]) << "\"";
                }
                file << "]";
            } else {
                file << "NULL";  // Fallback
            }
        }, entry.value);
        
        file << "\n\n";  // Empty line between entries for readability
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            return;
        }
    }
    
    // Add new entry
    pImpl->entries.emplace_back(key, value, source);
    pImpl->keyIndex[key] = pImpl->entries.size() - 1;
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    
    // Remove from keyIndex
    pImpl->keyIndex.erase(it);
    
    // Remove from entries vector
    if (index < pImpl->entries.size()) {
        // Update indices of all entries after this one
        for (auto& kv : pImpl->keyIndex) {
            if (kv.second > index) {
                kv.second--;
            }
        }
        
        pImpl->entries.erase(pImpl->entries.begin() + index);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->keyIndex.size());
    
    for (const auto& kv : pImpl->keyIndex) {
        keys.push_back(kv.first);
    }
    
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        
        oss << "\n";
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
    std::string result;
    result.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

// Explicit template instantiations
// Added support for all variant types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm