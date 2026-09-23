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

// Simple JSON parser implementation
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Parse JSON value
    ConfigValue parseJsonValue(const std::string& jsonStr) {
        std::string value = trim(jsonStr);
        
        if (value.empty() || value == "null") {
            throw ConfigException("Empty JSON value");
        }
        
        // Check for string (quoted)
        if ((value.front() == '"' && value.back() == '"') ||
            (value.front() == '\'' && value.back() == '\'')) {
            return value.substr(1, value.size() - 2);
        }
        
        // Check for boolean
        if (value == "true") {
            return true;
        }
        if (value == "false") {
            return false;
        }
        
        // Check for number
        size_t pos = 0;
        if (value[0] == '-') {
            pos++;
        }
        bool hasDecimal = false;
        for (size_t i = pos; i < value.size(); ++i) {
            if (value[i] == '.') {
                if (hasDecimal) throw ConfigException("Invalid number format: " + value);
                hasDecimal = true;
            } else if (value[i] == 'e' || value[i] == 'E') {
                if (i == value.size() - 1) throw ConfigException("Invalid number format: " + value);
                if (value[i + 1] == '-' || value[i + 1] == '+') {
                    i++;
                }
            } else if (!std::isdigit(static_cast<unsigned char>(value[i]))) {
                throw ConfigException("Invalid number format: " + value);
            }
        }
        
        try {
            if (hasDecimal || value.find('.') != std::string::npos) {
                return std::stod(value);
            }
            return std::stoll(value);
        } catch (const std::exception&) {
            throw ConfigException("Failed to parse number: " + value);
        }
    }
    
    // Parse JSON array
    std::vector<ConfigValue> parseJsonArray(const std::string& jsonStr) {
        std::vector<ConfigValue> result;
        std::string arrayContent = jsonStr.substr(1, jsonStr.size() - 2);  // Remove []
        
        size_t pos = 0;
        size_t start = 0;
        int bracketDepth = 0;
        
        for (; pos < arrayContent.size(); ++pos) {
            char c = arrayContent[pos];
            
            if (c == '[') bracketDepth++;
            else if (c == ']') bracketDepth--;
            else if (c == ',' && bracketDepth == 0) {
                std::string element = trim(arrayContent.substr(start, pos - start));
                if (!element.empty()) {
                    result.push_back(parseJsonValue(element));
                }
                start = pos + 1;
            }
        }
        
        if (bracketDepth != 0) {
            throw ConfigException("Mismatched brackets in JSON array");
        }
        
        std::string lastElement = trim(arrayContent.substr(start));
        if (!lastElement.empty()) {
            result.push_back(parseJsonValue(lastElement));
        }
        
        return result;
    }
    
    // Parse JSON object value
    ConfigValue parseJsonValueInternal(const std::string& jsonStr) {
        std::string value = trim(jsonStr);
        
        if (value.empty()) {
            throw ConfigException("Empty JSON value");
        }
        
        if (value[0] == '"') {
            // String
            size_t endQuote = value.find('"', 1);
            if (endQuote == std::string::npos) {
                throw ConfigException("Unterminated string in JSON");
            }
            return value.substr(1, endQuote - 1);
        } else if (value[0] == '[') {
            // Array
            if (value.back() != ']') {
                throw ConfigException("Unterminated JSON array");
            }
            return parseJsonArray(value);
        } else {
            // Number or boolean
            return parseJsonValue(value);
        }
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigException("Cannot open file: " + filepath);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    try {
        clear();
        
        // Simple JSON parsing for key-value pairs
        size_t pos = 0;
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            if (pos >= content.size()) break;
            
            // Find key (quoted string)
            if (content[pos] != '"') {
                throw ConfigException("Expected quoted key in JSON");
            }
            
            size_t keyStart = ++pos;
            while (pos < content.size() && content[pos] != '"') pos++;
            
            if (pos >= content.size()) {
                throw ConfigException("Unterminated key in JSON");
            }
            
            std::string key = content.substr(keyStart, pos - keyStart);
            pos++;  // Skip closing quote
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            if (pos >= content.size() || content[pos] != ':') {
                throw ConfigException("Expected colon after key in JSON");
            }
            
            pos++;  // Skip colon
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Find value
            if (content[pos] != '"' && content[pos] != '[' && 
                content[pos] != 't' && content[pos] != 'f' &&
                !std::isdigit(static_cast<unsigned char>(content[pos])) &&
                content[pos] != '-' && content[pos] != 'n') {
                throw ConfigException("Invalid JSON value");
            }
            
            size_t valueStart = pos;
            if (content[pos] == '"') {
                // String value
                while (pos < content.size() && content[pos] != '"') pos++;
                if (pos >= content.size()) {
                    throw ConfigException("Unterminated string value");
                }
            } else if (content[pos] == '[') {
                // Array value
                int bracketDepth = 1;
                pos++;
                while (pos < content.size() && bracketDepth > 0) {
                    if (content[pos] == '[') bracketDepth++;
                    else if (content[pos] == ']') bracketDepth--;
                    pos++;
                }
                if (bracketDepth > 0) {
                    throw ConfigException("Unterminated JSON array");
                }
            } else {
                // Number or boolean
                while (pos < content.size() && 
                       (std::isdigit(static_cast<unsigned char>(content[pos])) ||
                        content[pos] == '.' || content[pos] == '-' ||
                        content[pos] == '+' || content[pos] == 'e' || content[pos] == 'E' ||
                        content[pos] == 't' || content[pos] == 'f' ||
                        content[pos] == 'n')) {
                    pos++;
                }
            }
            
            std::string valueStr = content.substr(valueStart, pos - valueStart);
            
            // Parse JSON value
            ConfigValue jsonValue;
            try {
                jsonValue = parseJsonValueInternal(valueStr);
            } catch (const ConfigException& e) {
                throw ConfigException("Failed to parse JSON value for key '" + key + "': " + e.what());
            }
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            // Parse optional comma
            if (pos < content.size() && content[pos] == ',') {
                pos++;
                // Skip whitespace
                while (pos < content.size() && std::isspace(content[pos])) pos++;
            }
            
            set(key, jsonValue, ConfigSource::File);
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        std::cerr << "Error loading config from " << filepath << ": " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error loading config from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from command line: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigException("Cannot open file for writing: " + filepath);
    }
    
    // Write as simple key=value format for now
    // TODO: Implement proper JSON/YAML serialization
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Write key
        file << '"' << entry.key << '"' << " = ";
        
        // Write value
        try {
            if (std::holds_alternative<int>(entry.value)) {
                file << std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                file << std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                file << std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                file << (std::get<bool>(entry.value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(entry.value)) {
                file << '"' << std::get<std::string>(entry.value) << '"';
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                file << "["; 
                const auto& arr = std::get<std::vector<int>>(entry.value);
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arr[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                file << "["; 
                const auto& arr = std::get<std::vector<double>>(entry.value);
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arr[i];
                }
                file << "]";
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                file << "["; 
                const auto& arr = std::get<std::vector<std::string>>(entry.value);
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << '"' << arr[i] << '"';
                }
                file << "]";
            }
            file << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Warning: Failed to save config entry " << entry.key << ": " << e.what() << std::endl;
        }
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
    if (key.empty()) {
        throw ConfigException("Cannot set empty key");
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
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
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
