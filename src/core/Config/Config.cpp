#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>

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
        return false;
    }
    
    // Try to parse as JSON first
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parser for configuration
    // Expected format: { "key": "value", "number": 123, "boolean": true }
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        if (parseJSON(content)) {
            return true;
        }
    }
    
    // Fallback to simple key=value format for backward compatibility
    file.clear();
    file.seekg(0);
    
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

bool Config::parseJSON(const std::string& json) {
    // Simple JSON parser for configuration
    // Remove whitespace
    std::string cleaned;
    for (char c : json) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            cleaned += c;
        }
    }
    
    if (cleaned.empty() || cleaned[0] != '{' || cleaned.back() != '}') {
        return false;
    }
    
    // Extract content between braces
    std::string content = cleaned.substr(1, cleaned.size() - 2);
    
    size_t pos = 0;
    while (pos < content.size()) {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = keyEnd + 1;
        
        // Find colon
        size_t colonPos = content.find(':', pos);
        if (colonPos == std::string::npos) break;
        pos = colonPos + 1;
        
        // Skip whitespace
        while (pos < content.size() && std::isspace(static_cast<unsigned char>(content[pos]))) {
            pos++;
        }
        
        // Parse value
        ConfigValue value;
        if (pos < content.size()) {
            if (content[pos] == '"') {
                // String
                size_t valEnd = content.find('"', pos + 1);
                if (valEnd == std::string::npos) break;
                std::string val = content.substr(pos + 1, valEnd - pos - 1);
                value = val;
                pos = valEnd + 1;
            } else if (content[pos] == 't' || content[pos] == 'f') {
                // Boolean
                if (content.substr(pos, 4) == "true") {
                    value = true;
                    pos += 4;
                } else if (content.substr(pos, 5) == "false") {
                    value = false;
                    pos += 5;
                } else {
                    break;
                }
            } else if (content[pos] == 'n') {
                // null (skip)
                size_t nullEnd = content.find(',', pos);
                if (nullEnd == std::string::npos) {
                    nullEnd = content.find('}', pos);
                }
                if (nullEnd == std::string::npos) break;
                pos = nullEnd;
                continue;
            } else if (content[pos] == '-' || std::isdigit(static_cast<unsigned char>(content[pos]))) {
                // Number
                size_t valEnd = content.find(',', pos);
                if (valEnd == std::string::npos) {
                    valEnd = content.find('}', pos);
                }
                if (valEnd == std::string::npos) break;
                
                std::string numStr = content.substr(pos, valEnd - pos);
                // Try to parse as integer first
                try {
                    size_t endpos;
                    long long intVal = std::stoll(numStr, &endpos);
                    if (endpos == numStr.size()) {
                        value = intVal;
                        pos = valEnd;
                    } else {
                        // Try double
                        double doubleVal = std::stod(numStr, &endpos);
                        if (endpos == numStr.size()) {
                            value = doubleVal;
                            pos = valEnd;
                        } else {
                            break;
                        }
                    }
                } catch (...) {
                    break;
                }
            } else {
                break;
            }
        }
        
        // Set the value
        set(key, value, ConfigSource::File);
        
        // Skip comma and whitespace
        while (pos < content.size() && (content[pos] == ',' || std::isspace(static_cast<unsigned char>(content[pos])))) {
            pos++;
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
        
        // Write the value based on its type
        if (auto val = std::get_if<int>(&entry.value)) {
            file << entry.key << " = " << *val << "\n";
        } else if (auto val = std::get_if<int64_t>(&entry.value)) {
            file << entry.key << " = " << *val << "\n";
        } else if (auto val = std::get_if<double>(&entry.value)) {
            file << entry.key << " = " << std::fixed << std::setprecision(6) << *val << "\n";
        } else if (auto val = std::get_if<bool>(&entry.value)) {
            file << entry.key << " = " << (*val ? "true" : "false") << "\n";
        } else if (auto val = std::get_if<std::string>(&entry.value)) {
            file << entry.key << " = \"" << *val << "\"\n";
        } else if (auto val = std::get_if<std::vector<int>>(&entry.value)) {
            file << entry.key << " = ";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) file << " ";
                file << (*val)[i];
            }
            file << "\n";
        } else if (auto val = std::get_if<std::vector<double>>(&entry.value)) {
            file << entry.key << " = ";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) file << " ";
                file << std::fixed << std::setprecision(6) << (*val)[i];
            }
            file << "\n";
        } else if (auto val = std::get_if<std::vector<std::string>>(&entry.value)) {
            file << entry.key << " = ";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) file << " ";
                file << "\"" << (*val)[i] << "\"";
            }
            file << "\n";
        }
    }
    
    return true;
}

// Template function definitions
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

// Other Config methods implementation
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
        // Handle different value types
        if (auto val = std::get_if<int>(&entry.value)) {
            oss << *val;
        } else if (auto val = std::get_if<int64_t>(&entry.value)) {
            oss << *val;
        } else if (auto val = std::get_if<double>(&entry.value)) {
            oss << std::fixed << std::setprecision(6) << *val;
        } else if (auto val = std::get_if<bool>(&entry.value)) {
            oss << (*val ? "true" : "false");
        } else if (auto val = std::get_if<std::string>(&entry.value)) {
            oss << "\"" << *val << "\"";
        } else if (auto val = std::get_if<std::vector<int>>(&entry.value)) {
            oss << "[";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) oss << ", ";
                oss << (*val)[i];
            }
            oss << "]";
        } else if (auto val = std::get_if<std::vector<double>>(&entry.value)) {
            oss << "[";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << (*val)[i];
            }
            oss << "]";
        } else if (auto val = std::get_if<std::vector<std::string>>(&entry.value)) {
            oss << "[";
            for (size_t i = 0; i < val->size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << (*val)[i] << "\"";
            }
            oss << "]";
        }
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

} // namespace nlm