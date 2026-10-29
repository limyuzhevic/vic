#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cmath>
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
        std::cerr << "Failed to open config file: " << filepath << std::endl;
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.clear();
    file.seekg(0, std::ios::beg);
    
    // Simple JSON parser (for Phase 2)
    bool is_json = false;
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        is_json = true;
    }
    
    if (is_json) {
        // Basic JSON parsing for simple key-value pairs
        return parseJson(content);
    } else {
        // Fallback to simple key=value format (Phase 1)
        return parseSimpleFormat(file);
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

bool Config::parseJson(const std::string& content) {
    // Simple JSON parser for Phase 2
    // This is a basic implementation that handles object notation
    try {
        size_t pos = 0;
        while (pos < content.size()) {
            // Skip whitespace and find next key
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            if (pos >= content.size() || content[pos] != '"') {
                break;
            }
            
            pos++; // Skip opening quote
            std::string key;
            while (pos < content.size() && content[pos] != '"') {
                key += content[pos];
                pos++;
            }
            
            if (pos >= content.size()) break;
            pos++; // Skip closing quote
            
            // Skip whitespace
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            if (pos >= content.size() || content[pos] != ':') {
                break;
            }
            
            pos++; // Skip colon
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t')) {
                pos++;
            }
            
            // Parse value
            if (pos >= content.size()) break;
            
            ConfigValue value;
            if (content[pos] == '"') {
                // String value
                pos++;
                std::string str_value;
                while (pos < content.size() && content[pos] != '"') {
                    str_value += content[pos];
                    pos++;
                }
                value = ConfigValue(str_value);
                if (pos < content.size()) pos++; // Skip closing quote
            } else if (content[pos] == 't' || content[pos] == 'f') {
                // Boolean value
                std::string bool_str;
                while (pos < content.size() && (content[pos] == 't' || content[pos] == 'f' || content[pos] == 'r' || content[pos] == 'u' || content[pos] == 'e')) {
                    bool_str += content[pos];
                    pos++;
                }
                bool bool_value = (bool_str == "true" || bool_str == "yes" || bool_str == "on");
                value = ConfigValue(bool_value);
            } else if (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9')) {
                // Number value
                std::string num_str;
                bool is_float = false;
                while (pos < content.size() && (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9') || content[pos] == '.' || content[pos] == 'e' || content[pos] == 'E')) {
                    num_str += content[pos];
                    if (content[pos] == '.') is_float = true;
                    pos++;
                }
                if (is_float) {
                    double double_value = std::stod(num_str);
                    value = ConfigValue(double_value);
                } else {
                    int int_value = std::stoi(num_str);
                    value = ConfigValue(int_value);
                }
            } else if (content[pos] == '[') {
                // Array value
                pos++; // Skip opening bracket
                std::vector<ConfigValue> array_values;
                while (pos < content.size() && content[pos] != ']') {
                    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\n' || content[pos] == '\t' || content[pos] == ',')) {
                        pos++;
                    }
                    
                    if (pos >= content.size() || content[pos] == ']') break;
                    
                    if (content[pos] == '"') {
                        pos++;
                        std::string str_value;
                        while (pos < content.size() && content[pos] != '"') {
                            str_value += content[pos];
                            pos++;
                        }
                        array_values.push_back(ConfigValue(str_value));
                        if (pos < content.size()) pos++; // Skip closing quote
                    } else if (content[pos] == 't' || content[pos] == 'f') {
                        std::string bool_str;
                        while (pos < content.size() && (content[pos] == 't' || content[pos] == 'f' || content[pos] == 'r' || content[pos] == 'u' || content[pos] == 'e')) {
                            bool_str += content[pos];
                            pos++;
                        }
                        bool bool_value = (bool_str == "true" || bool_str == "yes" || bool_str == "on");
                        array_values.push_back(ConfigValue(bool_value));
                    } else if (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9')) {
                        std::string num_str;
                        bool is_float = false;
                        while (pos < content.size() && (content[pos] == '-' || (content[pos] >= '0' && content[pos] <= '9') || content[pos] == '.' || content[pos] == 'e' || content[pos] == 'E')) {
                            num_str += content[pos];
                            if (content[pos] == '.') is_float = true;
                            pos++;
                        }
                        if (is_float) {
                            double double_value = std::stod(num_str);
                            array_values.push_back(ConfigValue(double_value));
                        } else {
                            int int_value = std::stoi(num_str);
                            array_values.push_back(ConfigValue(int_value));
                        }
                    }
                }
                value = ConfigValue(array_values);
                if (pos < content.size()) pos++; // Skip closing bracket
            }
            
            set(key, value, ConfigSource::File);
            
            // Skip to next key or end
            while (pos < content.size() && content[pos] != '"' && content[pos] != ',') {
                pos++;
            }
            
            if (pos < content.size() && content[pos] == ',') {
                pos++; // Skip comma
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON config: " << e.what() << std::endl;
        return parseSimpleFormat(content); // Fallback to simple format
    }
}

bool Config::parseSimpleFormat(std::ifstream& file) {
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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
            std::cerr << "Warning: Invalid config line " << line_number << ": " << line << std::endl;
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
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
                    file << "\"" << escapeString(arg[i]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
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
        std::cerr << "Warning: Config key \"" << key << "\" has type mismatch" << std::endl;
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
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
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

