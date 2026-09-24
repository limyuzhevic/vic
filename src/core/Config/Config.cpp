#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ValidationFunc> validators;
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
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromString(content);
}

bool Config::loadFromString(const std::string& content) {
    clear(); // Clear existing entries
    
    // Try to detect JSON format
    if (isJson(content)) {
        // For now, treat JSON as simple key=value pairs (simplified parsing)
        // In production, use proper JSON library
        return parseSimpleJson(content, std::unordered_map<std::string, std::string>());
    }
    
    // Parse as key=value format
    return parseKeyValue(content);
}

bool Config::isJson(const std::string& content) {
    size_t pos = content.find_first_not_of(" \t\n\r");
    if (pos == std::string::npos) return false;
    
    return content[pos] == '{' || content[pos] == '[';
}

bool Config::parseSimpleJson(const std::string& json, std::unordered_map<std::string, std::string>& out) {
    // Simplified JSON parsing for demonstration
    // In production, use proper JSON library like nlohmann::json, RapidJSON, etc.
    
    size_t pos = 0;
    while (pos < json.length()) {
        // Skip whitespace
        while (pos < json.length() && std::isspace(json[pos])) {
            pos++;
        }
        
        if (pos >= json.length()) break;
        
        // Check for key-value pair
        if (json[pos] == '"') {
            size_t keyEnd = json.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = json.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < json.length() && std::isspace(json[pos])) {
                pos++;
            }
            
            // Check for colon
            if (pos >= json.length() || json[pos] != ':') break;
            pos++;
            
            // Skip whitespace
            while (pos < json.length() && std::isspace(json[pos])) {
                pos++;
            }
            
            // Check for value
            if (json[pos] == '"') {
                size_t valueEnd = json.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                
                std::string value = json.substr(pos + 1, valueEnd - pos - 1);
                out[key] = value;
                pos = valueEnd + 1;
            } else if (std::isdigit(json[pos]) || json[pos] == '-') {
                // Parse number
                size_t valueEnd = pos;
                while (valueEnd < json.length() && (std::isdigit(json[valueEnd]) || json[valueEnd] == '.' || json[valueEnd] == 'e' || json[valueEnd] == 'E')) {
                    valueEnd++;
                }
                
                std::string value = json.substr(pos, valueEnd - pos);
                out[key] = value;
                pos = valueEnd;
            } else if (json[pos] == 't' && json.substr(pos, 4) == "true") {
                out[key] = "true";
                pos += 4;
            } else if (json[pos] == 'f' && json.substr(pos, 5) == "false") {
                out[key] = "false";
                pos += 5;
            }
        } else {
            // Skip to next key
            while (pos < json.length() && json[pos] != '"') {
                pos++;
            }
        }
        
        // Skip comma
        while (pos < json.length() && (std::isspace(json[pos]) || json[pos] == ',')) {
            if (json[pos] == ',') pos++;
            while (pos < json.length() && std::isspace(json[pos])) {
                pos++;
            }
        }
    }
    
    // Apply parsed values to config
    for (const auto& pair : out) {
        set(pair.first, pair.second, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseKeyValue(const std::string& content) {
    clear(); // Clear existing entries
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Parse key=value pair
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            continue; // Invalid line
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Remove quotes
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Try to determine value type
        ConfigValue value;
        
        // Try boolean
        if (valueStr == "true") {
            value = true;
        } else if (valueStr == "false") {
            value = false;
        } else {
            // Try integer
            try {
                size_t idx;
                int intValue = std::stoi(valueStr, &idx);
                if (idx == valueStr.length()) {
                    value = intValue;
                } else {
                    // Try int64
                    int64_t int64Value = std::stoll(valueStr, &idx);
                    if (idx == valueStr.length()) {
                        value = int64Value;
                    } else {
                        // Try double
                        double doubleValue = std::stod(valueStr, &idx);
                        if (idx == valueStr.length()) {
                            value = doubleValue;
                        } else {
                            // Keep as string
                            value = valueStr;
                        }
                    }
                }
            } catch (...) {
                // Keep as string
                value = valueStr;
            }
        }
        
        set(key, value, ConfigSource::File);
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
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Write value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << arg << '"';
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                               std::is_same_v<T, std::vector<double>> ||
                               std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                bool first = true;
                std::visit([&file, &first](auto&& innerArg) {
                    using InnerT = std::decay_t<decltype(innerArg)>;
                    if constexpr (std::is_same_v<InnerT, std::string>) {
                        if (!first) file << ", ";
                        file << '"' << innerArg << '"';
                        first = false;
                    } else {
                        if (!first) file << ", ";
                        file << innerArg;
                        first = false;
                    }
                }, arg);
                file << "]";
            } else {
                // For int, int64_t, double
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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
    
    // Run validator if exists
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end() && !validatorIt->second(it->value)) {
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
    // Run validator if exists and validation fails, don't set
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end() && !validatorIt->second(value)) {
        return; // Validation failed
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

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::addValidator(const std::string& key, const ValidationFunc& validator) {
    pImpl->validators[key] = validator;
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
    pImpl->validators.erase(key);
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
    pImpl->validators.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->validators.size() << " validators):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    if (!pImpl->validators.empty()) {
        oss << "  Validators:\n";
        for (const auto& validator : pImpl->validators) {
            oss << "    - " << validator.first << " (validation function)\n";
        }
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