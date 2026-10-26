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
    // Try to detect file format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Peek at first character to determine format
    char peek;
    file.peek();
    peek = file.get();
    file.clear();
    file.seekg(0);
    
    if (peek == '{') {
        // JSON format
        return loadFromJSON(filepath);
    } else if (peek == '[') {
        // JSON array format
        return loadFromJSONArray(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Experimental JSON support
    // For now, we'll parse basic JSON objects with string values
    // This can be enhanced later with a proper JSON library
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    try {
        // Remove whitespace and comments for basic parsing
        std::string cleanContent;
        bool inString = false;
        char quoteChar = '\0';
        
        for (size_t i = 0; i < content.size(); ++i) {
            char c = content[i];
            
            // Skip whitespace and line comments
            if (!inString && (c == ' ' || c == '\n' || c == '\r' || c == '\t')) {
                continue;
            }
            
            // Handle string literals
            if (c == '\"' || c == '\'') {
                inString = !inString;
                quoteChar = c;
                cleanContent += c;
                continue;
            }
            
            if (inString) {
                // Escape quotes inside strings
                if (c == quoteChar) {
                    cleanContent += '\\';
                }
                cleanContent += c;
                continue;
            }
            
            // Skip block comments
            if (!inString && c == '/' && i + 1 < content.size() && content[i + 1] == '*') {
                i++; // Skip '*'
                while (i + 1 < content.size() && !(content[i] == '*' && content[i + 1] == '/')) {
                    i++;
                }
                if (i + 1 < content.size()) {
                    i++; // Skip '/'
                }
                continue;
            }
            
            cleanContent += c;
        }
        
        // Parse JSON object: { "key": "value", "number": 123 }
        if (!cleanContent.empty() && cleanContent.front() == '{' && cleanContent.back() == '}') {
            std::string inner = cleanContent.substr(1, cleanContent.size() - 2);
            
            // Simple key-value parsing (does not handle nested objects/arrays)
            size_t pos = 0;
            while (pos < inner.size()) {
                // Find next key (after opening quote)
                size_t keyStart = inner.find('"', pos);
                if (keyStart == std::string::npos) break;
                
                size_t keyEnd = inner.find('"', keyStart + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = inner.substr(keyStart + 1, keyEnd - keyStart - 1);
                
                // Find colon after key
                size_t colonPos = inner.find(':', keyEnd);
                if (colonPos == std::string::npos) break;
                
                // Find value
                size_t valueStart = inner.find_first_not_of(' ', colonPos + 1);
                if (valueStart == std::string::npos) break;
                
                char valueChar = inner[valueStart];
                if (valueChar == '\"') {
                    // String value
                    size_t valueEnd = inner.find('"', valueStart + 1);
                    if (valueEnd == std::string::npos) break;
                    std::string value = inner.substr(valueStart + 1, valueEnd - valueStart - 1);
                    pos = valueEnd + 1;
                    set(key, value, ConfigSource::File);
                } else if (std::isdigit(valueChar) || valueChar == '-') {
                    // Number value
                    size_t valueEnd = valueStart;
                    while (valueEnd < inner.size() && (std::isdigit(inner[valueEnd]) || inner[valueEnd] == '.' || inner[valueEnd] == '-')) {
                        valueEnd++;
                    }
                    std::string valueStr = inner.substr(valueStart, valueEnd - valueStart);
                    
                    // Try to parse as int, then double
                    try {
                        int64_t intVal = std::stoll(valueStr);
                        set(key, intVal, ConfigSource::File);
                    } catch (...) {
                        try {
                            double doubleVal = std::stod(valueStr);
                            set(key, doubleVal, ConfigSource::File);
                        } catch (...) {
                            set(key, valueStr, ConfigSource::File);
                        }
                    }
                    pos = valueEnd;
                } else if (valueChar == 't' || valueChar == 'f') {
                    // Boolean value
                    std::string boolStr;
                    size_t valueEnd = valueStart;
                    while (valueEnd < inner.size() && (inner[valueEnd] == 't' || inner[valueEnd] == 'r' || 
                                                    inner[valueEnd] == 'u' || inner[valueEnd] == 'e' ||
                                                    inner[valueEnd] == 'f' || inner[valueEnd] == 'a' ||
                                                    inner[valueEnd] == 'l' || inner[valueEnd] == 's')) {
                        boolStr += inner[valueEnd];
                        valueEnd++;
                    }
                    bool boolVal = (boolStr == "true");
                    set(key, boolVal, ConfigSource::File);
                    pos = valueEnd;
                } else {
                    // Unknown type, treat as string
                    pos = valueStart;
                }
                
                // Skip comma
                while (pos < inner.size() && (inner[pos] == ',' || inner[pos] == ' ')) {
                    pos++;
                }
            }
        } else {
            NLM_LOG_WARNING("Invalid JSON format in " + filepath + ". Using simple format.");
            return loadFromSimpleFormat(filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Error parsing JSON file " + filepath + ": " + std::string(e.what()) + ". Using simple format.");
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSONArray(const std::string& filepath) {
    // JSON array format (experimental)
    // For now, just log a warning and fall back to simple format
    NLM_LOG_WARNING("JSON array format in " + filepath + " not yet supported. Using simple format.");
    return loadFromSimpleFormat(filepath);
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
    
    // Write header comment
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM configuration system" << std::endl;
    file << std::endl;
    
    // Write configuration entries
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format doubles with reasonable precision
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
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
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        
        file << std::endl << std::endl;
    }
    
    file << "# End of configuration file" << std::endl;
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
