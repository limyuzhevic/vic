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

// Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            error_ = "cannot open configuration file: " + filepath;
            return false;
        }

        // Try JSON first, then fall back to simple format
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Simple JSON-like parser (basic implementation)
        // In production, use a proper JSON library like nlohmann::json
        if (parseSimpleConfig(content)) {
            return true;
        }
        
        // Fall back to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line.find('#') == 0 || line.find('/') == 0) {
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
                
                // Set configuration value with validation
                if (!setWithValidation(key, value)) {
                    error_ = "invalid configuration at line " + std::to_string(lineNumber) + ": " + key;
                    return false;
                }
            } else {
                error_ = "invalid configuration format at line " + std::to_string(lineNumber) + ": " + line;
                return false;
            }
        }
        
        return true;
    }

    bool parseSimpleConfig(const std::string& content) {
        // Basic JSON-like parsing: {"key": value, "key2": value2}
        // This is a very simple implementation - in production use a proper JSON library
        
        // Look for opening and closing braces
        size_t start = content.find('{');
        size_t end = content.rfind('}');
        
        if (start == std::string::npos || end == std::string::npos || end < start) {
            return false; // Not a valid JSON-like format
        }
        
        std::string jsonContent = content.substr(start + 1, end - start - 1);
        
        // Parse key-value pairs
        size_t pos = 0;
        while (pos < jsonContent.size()) {
            // Find next key (quoted string)
            size_t keyStart = jsonContent.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = jsonContent.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            key = trim(key);
            
            // Find colon after key
            size_t colonPos = jsonContent.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value (simple, could be quoted string, number, or boolean)
            size_t valuePos = jsonContent.find_first_not_of(' ', colonPos + 1);
            if (valuePos == std::string::npos) break;
            
            size_t valueEnd = valuePos;
            char quoteChar = 0;
            
            if (jsonContent[valuePos] == '"' || jsonContent[valuePos] == '\'') {
                quoteChar = jsonContent[valuePos];
                valueEnd = jsonContent.find(quoteChar, valuePos + 1);
                if (valueEnd == std::string::npos) break;
            } else {
                // Find end of number or boolean
                while (valueEnd < jsonContent.size() && 
                       (std::isalnum(jsonContent[valueEnd]) || 
                        jsonContent[valueEnd] == '.' || 
                        jsonContent[valueEnd] == '-' ||
                        jsonContent[valueEnd] == 'e' ||
                        jsonContent[valueEnd] == 'E')) {
                    valueEnd++;
                }
            }
            
            std::string value = jsonContent.substr(valuePos, valueEnd - valuePos);
            if (quoteChar) {
                value = value.substr(1, value.size() - 2); // Remove quotes
            }
            
            value = trim(value);
            
            // Set configuration value with validation
            if (!setWithValidation(key, value)) {
                return false;
            }
            
            // Move past this pair
            pos = valueEnd + 1;
            
            // Skip comma if present
            while (pos < jsonContent.size() && (jsonContent[pos] == ',' || jsonContent[pos] == ' ' || jsonContent[pos] == '\n')) {
                pos++;
            }
        }
        
        return true;
    }

    bool setWithValidation(const std::string& key, const std::string& valueStr) {
        // Basic validation of key
        if (key.empty() || key.find_first_of(" ={}();\\\"'[]") != std::string::npos) {
            return false;
        }
        
        // Try to infer type from value
        bool success = false;
        
        // Try boolean
        if (valueStr == "true" || valueStr == "false") {
            set(key, valueStr == "true", ConfigSource::File);
            success = true;
        }
        // Try integer
        else {
            try {
                size_t pos;
                int intVal = std::stoi(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, intVal, ConfigSource::File);
                    success = true;
                }
            } catch (...) {}
        }
        
        // Try int64
        if (!success) {
            try {
                size_t pos;
                int64_t int64Val = std::stoll(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, int64Val, ConfigSource::File);
                    success = true;
                }
            } catch (...) {}
        }
        
        // Try float
        if (!success) {
            try {
                size_t pos;
                float floatVal = std::stof(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, floatVal, ConfigSource::File);
                    success = true;
                }
            } catch (...) {}
        }
        
        // Try double
        if (!success) {
            try {
                size_t pos;
                double doubleVal = std::stod(valueStr, &pos);
                if (pos == valueStr.size()) {
                    set(key, doubleVal, ConfigSource::File);
                    success = true;
                }
            } catch (...) {}
        }
        
        // Default to string
        if (!success) {
            set(key, valueStr, ConfigSource::File);
            success = true;
        }
        
        return success;
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
            error_ = "cannot open configuration file for writing: " + filepath;
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated at: " << getCurrentTimestamp() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n";
            
            // Format value appropriately based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [\"";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << "\", \"";
                        file << arg[i];
                    }
                    file << "\"]\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                    std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else {
                    file << entry.key << " = " << arg << "\n";
                }
            }, entry.value);
            
            file << "\n";
        }
        
        // Write section for comments
        file << "\n# Comments:\n";
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty() && 
                entry.description.find("---") == std::string::npos) {
                file << "# " << entry.key << ": " << entry.description << "\n";
            }
        }
        
        return true;
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
