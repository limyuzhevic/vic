#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

namespace {
    // JSON parsing helpers
    bool isWhitespace(char c) { return std::isspace(static_cast<unsigned char>(c)); }
    
    std::string trimString(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        while (start < end && isWhitespace(str[start])) start++;
        while (end > start && isWhitespace(str[end - 1])) end--;
        
        return str.substr(start, end - start);
    }
    
    // Safe string conversion with error handling
    template<typename T>
    std::optional<T> safeConvert(const std::string& str) {
        std::istringstream iss(str);
        T result;
        iss >> result;
        
        if (iss.fail() || !iss.eof()) {
            return std::nullopt;
        }
        
        return result;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Validation helper to ensure data integrity
    void validateEntry(const ConfigEntry& entry) const {
        if (entry.key.empty()) {
            throw std::invalid_argument("Config key cannot be empty");
        }
        
        // Check for duplicate keys (last one wins)
        auto it = std::find_if(entries.begin(), entries.end(),
            [&entry](const ConfigEntry& existing) { return existing.key == entry.key; });n
        if (it != entries.end()) {
            // Duplicate key - replace with new entry
            *it = entry;
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Simple JSON parsing (robust, no external dependencies)
    if (!content.empty() && (content.front() == '{' || content.front() == '[')) {
        // Parse JSON-like format: {"key": "value", "key2": 123}
        size_t pos = 0;
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size()) break;
            
            if (content[pos] == '}') break; // End of object
            
            // Parse key (expecting "key": value)
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break; // Malformed JSON
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break; // Malformed JSON
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace after key
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size() || content[pos] != ':') break; // Malformed JSON
            
            // Skip colon and whitespace
            pos++;
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos >= content.size()) break;
            
            // Parse value
            ConfigValue value;
            bool success = false;
            
            if (content[pos] == '"') {
                // String value
                size_t valueEnd = content.find('"', pos + 1);
                if (valueEnd != std::string::npos) {
                    value = content.substr(pos + 1, valueEnd - pos - 1);
                    pos = valueEnd + 1;
                    success = true;
                }
            } else if (content[pos] == 't' || content[pos] == 'f') {
                // Boolean value
                std::string boolStr = "true";
                bool isTrue = (content.compare(pos, 4, boolStr) == 0);
                if (isTrue) {
                    value = true;
                    pos += 4;
                    success = true;
                } else {
                    boolStr = "false";
                    if (content.compare(pos, 5, boolStr) == 0) {
                        value = false;
                        pos += 5;
                        success = true;
                    }
                }
            } else if (content[pos] == 'n') {
                // Null value (not in our type system, skip)
                while (pos < content.size() && content[pos] != ',' && content[pos] != '}') pos++;
                continue;
            } else if (content[pos] == '[') {
                // Array (try to parse simple int or string arrays)
                size_t arrayEnd = content.find(']', pos);
                if (arrayEnd != std::string::npos) {
                    std::string arrayContent = content.substr(pos + 1, arrayEnd - pos - 1);
                    // For now, just skip arrays - could be enhanced later
                    value = std::vector<std::string>(); // Empty placeholder
                    pos = arrayEnd + 1;
                    success = true;
                }
            } else {
                // Number (int or double)
                size_t numEnd = pos;
                while (numEnd < content.size() && (std::isdigit(content[numEnd]) || content[numEnd] == '.' || content[numEnd] == '-')) {
                    numEnd++;
                }
                if (numEnd > pos) {
                    std::string numStr = content.substr(pos, numEnd - pos);
                    try {
                        size_t dotPos = numStr.find('.');
                        if (dotPos != std::string::npos) {
                            value = std::stod(numStr);
                        } else {
                            value = std::stoll(numStr);
                        }
                        pos = numEnd;
                        success = true;
                    } catch (...) {
                        // Failed to parse number
                    }
                }
            }
            
            if (!success) {
                // Skip to next comma or brace
                while (pos < content.size() && content[pos] != ',' && content[pos] != '}') pos++;
                continue;
            }
            
            // Store the parsed value
            set(key, value, ConfigSource::File);
            
            // Skip to next comma or brace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos < content.size() && content[pos] == ',') pos++;
            while (pos < content.size() && std::isspace(content[pos])) pos++;
        }
    } else {
        // Fallback to simple key=value format for backward compatibility
        std::ifstream file2(filepath);
        if (!file2.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file2, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
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
    
    // Write in JSON format for better interoperability
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        // Write key
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double without trailing zeros for cleaner output
                file.precision(6);
                file << std::fixed << arg;
                std::streamsize oldPrecision = file.precision();
                file.unsetf(std::ios_base::floatfield);
                file.precision(oldPrecision);
            } else {
                // For unknown types, write as string representation
                file << "\"" << arg << "\"";
            }
        }, entry.value);
        
        // Add comment with source
        if (!entry.description.empty()) {
            file << "  // " << entry.description << " (source: " << static_cast<int>(entry.source) << ")";
        }
    }
    
    file << "\n}\n";
    
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
