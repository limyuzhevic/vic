#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with JSON/YAML support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Try JSON format first (more common for modern configs)
    if (content.find('{') != std::string::npos || content.find('[') != std::string::npos) {
        // Simple JSON parsing - look for key-value pairs in object format
        size_t pos = content.find('"');
        while (pos != std::string::npos) {
            size_t keyStart = pos + 1;
            size_t keyEnd = content.find('"', keyStart);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            
            // Find the value after colon
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            size_t valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Handle different value types
            if (content[valueStart] == '"') {
                // String value
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                set(key, value, ConfigSource::File);
                pos = valueEnd;
            } else if (std::isdigit(content[valueStart]) || content[valueStart] == '-') {
                // Number value
                size_t valueEnd = content.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) valueEnd = content.size();
                std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
                try {
                    double numVal = std::stod(valueStr);
                    // Try to determine if it's int or double
                    if (numVal == static_cast<int64_t>(numVal)) {
                        set(key, static_cast<int64_t>(numVal), ConfigSource::File);
                    } else {
                        set(key, numVal, ConfigSource::File);
                    }
                } catch (...) {
                    // If parsing fails, treat as string
                    set(key, valueStr, ConfigSource::File);
                }
                pos = valueEnd;
            } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
                // Boolean value
                std::string boolStr = (content.substr(valueStart, 4) == "true" || 
                                      content.substr(valueStart, 5) == "false") ? 
                    content.substr(valueStart, content.find_first_of(",}", valueStart) - valueStart) : 
                    "true";
                bool boolVal = (boolStr == "true");
                set(key, boolVal, ConfigSource::File);
                pos = content.find_first_of(",}", valueStart);
            }
            
            pos = content.find('"', pos + 1);
        }
        
        if (!pImpl->entries.empty()) {
            return true;
        }
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::stringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
    
    return !pImpl->entries.empty();
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write in modern JSON-like format for better interoperability
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Convert value to JSON-like string representation
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Write with reasonable precision
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << arg[j] << "\"";
                }
                file << "]";
            } else {
                file << "null"; // Default for unknown types
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    
    return true;
}

// Helper function implementations
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