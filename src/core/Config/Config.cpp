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
    // Enhanced error handling with detailed error reporting
    if (filepath.empty()) {
        // Log error: empty file path
        return false;
    }
    
    // Check if file exists and is readable
    std::filesystem::path path(filepath);
    if (!std::filesystem::exists(path)) {
        // Log error: file not found
        return false;
    }
    
    if (!std::filesystem::is_regular_file(path)) {
        // Log error: not a regular file
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Log error: cannot open file (permission denied, etc.)
        return false;
    }
    
    // Set exception mask for stream
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs with better validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                // Log warning: malformed line, no '=' found
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (key.empty()) {
                // Log warning: empty key
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value (basic check for null bytes)
            if (value.find('\0') != std::string::npos) {
                // Log warning: value contains null byte
                continue;
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
        
    } catch (const std::ifstream::failure& e) {
        // Log error: file read error
        return false;
    } catch (const std::exception& e) {
        // Log error: unexpected exception
        return false;
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
    // Enhanced error handling with detailed validation
    if (filepath.empty()) {
        // Log error: empty file path
        return false;
    }
    
    // Validate directory exists and is writable
    std::filesystem::path path(filepath);
    std::filesystem::path parentDir = path.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        // Log error: parent directory doesn't exist
        return false;
    }
    
    if (!parentDir.empty() && std::filesystem::exists(parentDir) && 
        !std::filesystem::is_directory(parentDir)) {
        // Log error: parent path exists but is not a directory
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        // Log error: cannot open file for writing
        return false;
    }
    
    // Set exception mask for stream
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Write key=value pair with proper escaping
            file << entry.key << " = ";
            
            // Handle different value types
            std::visit([&file](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << escapeString(val) << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else {
                    file << val;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        // Ensure all data is written to disk
        file.flush();
        if (!file.good()) {
            // Log error: write error
            return false;
        }
        
        return true;
        
    } catch (const std::ofstream::failure& e) {
        // Log error: file write error
        return false;
    } catch (const std::exception& e) {
        // Log error: unexpected exception
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Enhanced validation for set operation
    if (key.empty()) {
        // Log error: empty key
        return;
    }
    
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

// Escape special characters in strings for file output
    static std::string escapeString(const std::string& input) {
        std::string output;
        output.reserve(input.size());
        for (char c : input) {
            switch (c) {
                case '\\': output += "\\\\"; break;
                case '"': output += "\\\""; break;
                case '\n': output += "\\n"; break;
                case '\r': output += "\\r"; break;
                case '\t': output += "\\t"; break;
                case '\0': output += "\\\\0"; break;
                default:
                    if (c < 32 || c == 127) {
                        // Control character - use escape sequence
                        output += "\\\\x";
                        output += "0123456789abcdef"[c >> 4];
                        output += "0123456789abcdef"[c & 0xF];
                    } else {
                        output += c;
                    }
            }
        }
        return output;
    }

    // Unescape string from file input
    static std::string unescapeString(const std::string& input) {
        std::string output;
        output.reserve(input.size());
        bool escapeNext = false;
        
        for (char c : input) {
            if (escapeNext) {
                switch (c) {
                    case 'n': output += '\n'; break;
                    case 'r': output += '\r'; break;
                    case 't': output += '\t'; break;
                    case '0': output += '\0'; break;
                    case 'x':
                        // Handle hex escape \xXX
                        // Would need more complex implementation
                        output += c;
                        break;
                    case '\\': output += '\\'; break;
                    case '\"': output += '\"'; break;
                    default: output += c; break; // Pass through
                }
                escapeNext = false;
            } else if (c == '\\') {
                escapeNext = true;
            } else {
                output += c;
            }
        }
        
        // Handle trailing escape
        if (escapeNext) {
            output += '\\';
        }
        
        return output;
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

// Escape special characters in strings for file output
std::string Config::escapeString(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    for (char c : input) {
        switch (c) {
            case '\\': output += "\\\\"; break;
            case '"': output += "\\\""; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            case '\0': output += "\\\\0"; break;
            default:
                if (c < 32 || c == 127) {
                    // Control character - use escape sequence
                    output += "\\\\x";
                    output += "0123456789abcdef"[c >> 4];
                    output += "0123456789abcdef"[c & 0xF];
                } else {
                    output += c;
                }
        }
    }
    return output;
}

// Unescape string from file input
std::string Config::unescapeString(const std::string& input) {
    std::string output;
    output.reserve(input.size());
    bool escapeNext = false;
    
    for (char c : input) {
        if (escapeNext) {
            switch (c) {
                case 'n': output += '\n'; break;
                case 'r': output += '\r'; break;
                case 't': output += '\t'; break;
                case '0': output += '\0'; break;
                case 'x':
                    // Handle hex escape \xXX - simplified implementation
                    output += c;
                    break;
                case '\\': output += '\\'; break;
                case '\"': output += '\"'; break;
                default: output += c; break; // Pass through
            }
            escapeNext = false;
        } else if (c == '\\') {
            escapeNext = true;
        } else {
            output += c;
        }
    }
    
    // Handle trailing escape
    if (escapeNext) {
        output += '\\';
    }
    
    return output;
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

} // namespace nlm
