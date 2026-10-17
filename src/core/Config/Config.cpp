#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Add JSON parsing function before loadFromFile
bool Config::parseJSON(const std::string& jsonStr) {
    try {
        if (jsonStr.empty() || jsonStr[0] != '{') {
            return false;
        }
        
        size_t pos = 1;
        while (pos < jsonStr.size() && jsonStr[pos] != '}') {
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] == '}') {
                break;
            }
            
            size_t keyStart = jsonStr.find('"', pos);
            if (keyStart == std::string::npos) {
                return false;
            }
            
            size_t keyEnd = jsonStr.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) {
                return false;
            }
            
            std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                return false;
            }
            
            pos++;
            
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size()) {
                return false;
            }
            
            if (jsonStr[pos] == '"') {
                size_t valueEnd = jsonStr.find('"', pos + 1);
                if (valueEnd == std::string::npos) {
                    return false;
                }
                std::string value = jsonStr.substr(pos + 1, valueEnd - pos - 1);
                set(key, value, ConfigSource::File);
                pos = valueEnd + 2;
            } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                if (jsonStr.substr(pos, 5) == "true") {
                    set(key, true, ConfigSource::File);
                    pos += 5;
                } else if (jsonStr.substr(pos, 4) == "false") {
                    set(key, false, ConfigSource::File);
                    pos += 4;
                } else {
                    return false;
                }
            } else if (jsonStr[pos] == '-' || (jsonStr[pos] >= '0' && jsonStr[pos] <= '9')) {
                size_t numEnd = pos;
                while (numEnd < jsonStr.size() && (jsonStr[numEnd] == '-' || jsonStr[numEnd] == '.' || 
                    (jsonStr[numEnd] >= '0' && jsonStr[numEnd] <= '9') || jsonStr[numEnd] == 'e' || jsonStr[numEnd] == 'E')) {
                    numEnd++;
                }
                std::string numStr = jsonStr.substr(pos, numEnd - pos);
                
                try {
                    double doubleVal = std::stod(numStr);
                    if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && numStr.find('E') == std::string::npos) {
                        try {
                            int64_t intVal = std::stoll(numStr);
                            set(key, intVal, ConfigSource::File);
                        } catch (...) {
                            set(key, doubleVal, ConfigSource::File);
                        }
                    } else {
                        set(key, doubleVal, ConfigSource::File);
                    }
                } catch (...) {
                    return false;
                }
                
                pos = numEnd;
            } else if (jsonStr[pos] == '[') {
                pos = jsonStr.find(']', pos);
                if (pos == std::string::npos) {
                    return false;
                }
                pos++;
            } else {
                return false;
            }
            
            while (pos < jsonStr.size() && (std::isspace(jsonStr[pos]) || jsonStr[pos] == ',')) {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Initialize the global logger
inline Logger& getLogger() {
    static Logger logger;
    static bool initialized = false;
    if (!initialized) {
        logger.setLevel(LogLevel::Info);
        initialized = true;
    }
    return logger;
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
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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

namespace nlm {

// Helper function to trim whitespace from both ends of a string
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert string to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Parse JSON string (simplified implementation)
bool Config::parseJSON(const std::string& jsonStr) {
    try {
        // Simple JSON parser for basic key-value pairs
        // This handles objects like: {"key": "value", "number": 42}
        if (jsonStr.empty() || jsonStr[0] != '{') {
            return false;
        }
        
        size_t pos = 1;
        while (pos < jsonStr.size() && jsonStr[pos] != '}') {
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] == '}') {
                break;
            }
            
            // Find key (string in quotes)
            size_t keyStart = jsonStr.find('"', pos);
            if (keyStart == std::string::npos) {
                return false;
            }
            
            size_t keyEnd = jsonStr.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) {
                return false;
            }
            
            std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace and colon
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                return false;
            }
            
            pos++; // Skip colon
            
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size()) {
                return false;
            }
            
            // Parse value based on its type
            if (jsonStr[pos] == '"') {
                // String value
                size_t valueEnd = jsonStr.find('"', pos + 1);
                if (valueEnd == std::string::npos) {
                    return false;
                }
                std::string value = jsonStr.substr(pos + 1, valueEnd - pos - 1);
                set(key, value, ConfigSource::File);
                pos = valueEnd + 2; // +1 for quote, +1 for comma/space
            } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                // Boolean value (true/false)
                if (jsonStr.substr(pos, 5) == "true") {
                    set(key, true, ConfigSource::File);
                    pos += 5;
                } else if (jsonStr.substr(pos, 4) == "false") {
                    set(key, false, ConfigSource::File);
                    pos += 4;
                } else {
                    return false;
                }
            } else if (jsonStr[pos] == '-' || (jsonStr[pos] >= '0' && jsonStr[pos] <= '9')) {
                // Number value (integer or float)
                size_t numEnd = pos;
                while (numEnd < jsonStr.size() && (jsonStr[numEnd] == '-' || jsonStr[numEnd] == '.' || 
                    (jsonStr[numEnd] >= '0' && jsonStr[numEnd] <= '9') || jsonStr[numEnd] == 'e' || jsonStr[numEnd] == 'E')) {
                    numEnd++;
                }
                std::string numStr = jsonStr.substr(pos, numEnd - pos);
                
                // Try to parse as double first
                try {
                    double doubleVal = std::stod(numStr);
                    // Check if it looks like an integer
                    if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && numStr.find('E') == std::string::npos) {
                        // Try to parse as int64_t to preserve integer type
                        try {
                            int64_t intVal = std::stoll(numStr);
                            set(key, intVal, ConfigSource::File);
                        } catch (...) {
                            set(key, doubleVal, ConfigSource::File);
                        }
                    } else {
                        set(key, doubleVal, ConfigSource::File);
                    }
                } catch (...) {
                    return false;
                }
                
                pos = numEnd;
            } else if (jsonStr[pos] == '[') {
                // Array value - for now, skip and store as string
                // TODO: Implement proper array parsing
                pos = jsonStr.find(']', pos);
                if (pos == std::string::npos) {
                    return false;
                }
                pos++; // Skip closing bracket
            } else {
                return false; // Unsupported value type
            }
            
            // Skip comma or whitespace
            while (pos < jsonStr.size() && (std::isspace(jsonStr[pos]) || jsonStr[pos] == ',')) {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

// Initialize the global logger
inline Logger& getLogger() {
    static Logger logger;
    static bool initialized = false;
    if (!initialized) {
        logger.setLevel(LogLevel::Info);
        initialized = true;
    }
    return logger;
}
