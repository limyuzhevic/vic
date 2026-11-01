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
// File-level error handling utilities
namespace {
    // Thread-safe error logging
    void logError(const std::string& operation, const std::string& filepath, 
                  const std::string& details = "") {
        std::cerr << "[Config Error] " << operation << ": " << filepath;
        if (!details.empty()) {
            std::cerr << " - " << details;
        }
        std::cerr << std::endl;
    }
    
    // Safe file opening with timeout protection
    bool safeOpenFile(std::ifstream& file, const std::string& filepath, 
                     size_t timeout_ms = 1000) {
        auto start = std::chrono::steady_clock::now();
        while (true) {
            file.open(filepath, std::ios::in);
            if (file.is_open()) {
                return true;
            }
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            if (elapsed.count() > timeout_ms) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return false;
    }
    
    // Safe file writing with timeout protection
    bool safeWriteFile(std::ofstream& file, const std::string& filepath,
                      size_t timeout_ms = 1000) {
        auto start = std::chrono::steady_clock::now();
        while (true) {
            file.open(filepath, std::ios::out | std::ios::trunc);
            if (file.is_open()) {
                return true;
            }
            
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            if (elapsed.count() > timeout_ms) {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser
    // Phase 2: Support multiple config file formats (JSON, YAML, key=value)
    
    try {
        // Check if file exists and is readable
        std::ifstream testFile(filepath, std::ios::in);
        if (!testFile.good()) {
            logError("loadFromFile", filepath, "File does not exist or is not readable");
            return false;
        }
        testFile.close();
        
        // Try JSON format first (more robust)
        if (isJsonFile(filepath)) {
            return loadFromJson(filepath);
        }
        
        // Try YAML format second
        if (isYamlFile(filepath)) {
            return loadFromYaml(filepath);
        }
        
        // Fallback to Phase 1 simple key=value format for backward compatibility
        std::ifstream file;
        if (!safeOpenFile(file, filepath)) {
            logError("loadFromFile", filepath, "Failed to open file within timeout");
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            try {
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
                    
                    if (key.empty()) {
                        logError("loadFromFile", filepath, "Empty key at line " + 
                                std::to_string(lineNumber));
                        continue;
                    }
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '\"' && value.back() == '\"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                } else {
                    // Log warning for malformed line (missing =)
                    if (!line.empty()) {
                        logError("loadFromFile", filepath, "Malformed key=value pair at line " + 
                                std::to_string(lineNumber) + ": " + line);
                    }
                }
            } catch (const std::exception& e) {
                logError("loadFromFile", filepath, 
                        "Exception at line " + std::to_string(lineNumber) + ": " + e.what());
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        logError("loadFromFile", filepath, std::string("Unexpected exception: ") + e.what());
        return false;
    }
}

bool Config::isJsonFile(const std::string& filepath) const {
    try {
        // Check if file has .json extension or starts with { or [
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            return true;
        }
        
        std::ifstream file;
        if (!safeOpenFile(file, filepath)) {
            return false;
        }
        
        char firstChar;
        file >> firstChar;
        file.close();
        
        return firstChar == '{' || firstChar == '[';
    } catch (const std::exception& e) {
        logError("isJsonFile", filepath, std::string("Exception: ") + e.what());
        return false;
    }
}

bool Config::isYamlFile(const std::string& filepath) const {
    try {
        if (filepath.size() >= 5 && (filepath.substr(filepath.size() - 5) == ".yaml" || 
                                     filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml")) {
            return true;
        }
        
        // Simple heuristic: check for YAML-like patterns
        std::ifstream file;
        if (!safeOpenFile(file, filepath)) {
            return false;
        }
        
        std::string line;
        bool hasYamlIndicators = false;
        size_t lineNumber = 0;
        while (std::getline(file, line) && !hasYamlIndicators) {
            lineNumber++;
            line = trim(line);
            // YAML indicators: indentation, key: value, list items (-)
            if (!line.empty() && (line[0] == '-' || line.find(": ") != std::string::npos)) {
                hasYamlIndicators = true;
            }
        }
        
        return hasYamlIndicators;
    } catch (const std::exception& e) {
        logError("isYamlFile", filepath, std::string("Exception: ") + e.what());
        return false;
    }
}

bool Config::loadFromJson(const std::string& filepath) const {
    try {
        std::ifstream file;
        if (!safeOpenFile(file, filepath)) {
            logError("loadFromJson", filepath, "Failed to open file within timeout");
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), 
                            std::istreambuf_iterator<char>());
        file.close();
        
        if (content.empty()) {
            logError("loadFromJson", filepath, "File is empty");
            return false;
        }
        
        // Remove whitespace for simple parsing (not production-ready, but functional)
        std::string cleaned;
        for (char c : content) {
            if (!std::isspace(c)) {
                cleaned += c;
            }
        }
        
        if (cleaned.empty()) {
            logError("loadFromJson", filepath, "File contains only whitespace");
            return false;
        }
        
        // Simple parsing for key-value pairs in objects
        size_t pos = 0;
        size_t entryCount = 0;
        
        while (pos < cleaned.size()) {
            // Find next "key"::
            size_t keyStart = cleaned.find('\"', pos);
            if (keyStart == std::string::npos) {
                // Try to find a colon as fallback
                size_t colonPos = cleaned.find(':', pos);
                if (colonPos == std::string::npos) break;
                
                std::string key = trim(cleaned.substr(pos, colonPos - pos));
                if (key.empty()) break;
                
                size_t valueStart = cleaned.find_first_not_of(' ', colonPos + 1);
                if (valueStart == std::string::npos) break;
                
                // Parse as simple string value
                size_t valueEnd = cleaned.find_first_of(",}", valueStart);
                if (valueEnd == std::string::npos) {
                    valueEnd = cleaned.size();
                }
                
                std::string valueStr = cleaned.substr(valueStart, valueEnd - valueStart);
                set(key, valueStr, ConfigSource::File);
                entryCount++;
                pos = valueEnd + 1;
                continue;
            }
            
            size_t keyEnd = cleaned.find('\"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find next ":"
            size_t colonPos = cleaned.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value
            size_t valueStart = cleaned.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Handle different value types
            ConfigValue value;
            bool parsed = false;
            
            if (valueStart < cleaned.size()) {
                char firstChar = cleaned[valueStart];
                
                if (firstChar == '\"') {
                    // String value
                    size_t valueEnd = cleaned.find('\"', valueStart + 1);
                    if (valueEnd != std::string::npos) {
                        std::string strValue = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
                        set(key, strValue, ConfigSource::File);
                        parsed = true;
                        entryCount++;
                    }
                } else if (firstChar == 't' || firstChar == 'f') {
                    // Boolean value (true/false)
                    if (cleaned.substr(valueStart, 5) == "true") {
                        set(key, true, ConfigSource::File);
                        parsed = true;
                        entryCount++;
                    } else if (cleaned.substr(valueStart, 4) == "false") {
                        set(key, false, ConfigSource::File);
                        parsed = true;
                        entryCount++;
                    }
                } else if (firstChar == '-' || (firstChar >= '0' && firstChar <= '9')) {
                    // Number or array
                    // Find the end of this value (next ", " or "}")
                    size_t valueEnd = cleaned.find_first_of(",}", valueStart);
                    if (valueEnd == std::string::npos) {
                        valueEnd = cleaned.size();
                    }
                    
                    std::string numStr = cleaned.substr(valueStart, valueEnd - valueStart);
                    
                    // Try to parse as double first
                    try {
                        double doubleVal = std::stod(numStr);
                        set(key, doubleVal, ConfigSource::File);
                        parsed = true;
                        entryCount++;
                    } catch (...) {
                        // If not a number, try as array [1, 2, 3]
                        if (numStr[0] == '[' && numStr.back() == ']') {
                            std::vector<double> arrayValue;
                            std::string inner = numStr.substr(1, numStr.size() - 2);
                            // Simple array parsing (comma-separated numbers)
                            size_t arrayPos = 0;
                            while (arrayPos < inner.size()) {
                                size_t nextComma = inner.find(',', arrayPos);
                                std::string element;
                                if (nextComma == std::string::npos) {
                                    element = inner.substr(arrayPos);
                                    arrayPos = inner.size();
                                } else {
                                    element = inner.substr(arrayPos, nextComma - arrayPos);
                                    arrayPos = nextComma + 1;
                                }
                                try {
                                    double elemVal = std::stod(element);
                                    arrayValue.push_back(elemVal);
                                } catch (...) {
                                    // Skip invalid elements
                                }
                            }
                            set(key, arrayValue, ConfigSource::File);
                            parsed = true;
                            entryCount++;
                        }
                    }
                }
            }
            
            if (!parsed) {
                logError("loadFromJson", filepath, 
                        "Failed to parse value for key '" + key + "'");
            }
            
            // Move to next key
            pos = cleaned.find('"', keyStart) + 1;
            if (pos < 2) break;
            pos = cleaned.find('"', pos);
            if (pos == std::string::npos) break;
        }
        
        return entryCount > 0;
    } catch (const std::exception& e) {
        logError("loadFromJson", filepath, std::string("Exception: ") + e.what());
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) const {
    try {
        std::ifstream file;
        if (!safeOpenFile(file, filepath)) {
            logError("loadFromYaml", filepath, "Failed to open file within timeout");
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        size_t entryCount = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            try {
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '%') {
                    continue;
                }
                
                // Parse YAML key: value pairs
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = trim(line.substr(0, colonPos));
                    std::string value = trim(line.substr(colonPos + 1));
                    
                    if (key.empty()) {
                        logError("loadFromYaml", filepath, "Empty key at line " + 
                                std::to_string(lineNumber));
                        continue;
                    }
                    
                    // Remove quotes if present
                    if (value.size() >= 2 && 
                        ((value.front() == '\"' && value.back() == '\"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                    entryCount++;
                } else if (!line.empty()) {
                    // Log warning for malformed line (missing :)
                    logError("loadFromYaml", filepath, "Malformed key:value pair at line " + 
                            std::to_string(lineNumber) + ": " + line);
                }
            } catch (const std::exception& e) {
                logError("loadFromYaml", filepath, 
                        "Exception at line " + std::to_string(lineNumber) + ": " + e.what());
            }
        }
        
        return entryCount > 0;
    } catch (const std::exception& e) {
        logError("loadFromYaml", filepath, std::string("Exception: ") + e.what());
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        std::visit([&file, entry](const ConfigValue& value) {
            if (std::holds_alternative<std::string>(value)) {
                file << entry.key << " = \"" << std::get<std::string>(value) << "\"\n";
            } else if (std::holds_alternative<bool>(value)) {
                file << entry.key << " = " << (std::get<bool>(value) ? "true" : "false") << "\n";
            } else if (std::holds_alternative<double>(value)) {
                file << entry.key << " = " << std::get<double>(value) << "\n";
            } else if (std::holds_alternative<int>(value)) {
                file << entry.key << " = " << std::get<int>(value) << "\n";
            } else if (std::holds_alternative<int64_t>(value)) {
                file << entry.key << " = " << std::get<int64_t>(value) << "\n";
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                file << entry.key << " = [";
                const auto& vec = std::get<std::vector<int>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i) file << ", ";
                    file << vec[i];
                }
                file << "]\n";
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                file << entry.key << " = [";
                const auto& vec = std::get<std::vector<double>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i) file << ", ";
                    file << vec[i];
                }
                file << "]\n";
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                file << entry.key << " = [";
                const auto& vec = std::get<std::vector<std::string>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i) file << ", ";
                    file << vec[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << value << "\n";
            }
        }, entry.value);
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
