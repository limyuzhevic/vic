#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filename;  // Track last loaded filename for save operations
};

// Simple JSON parser implementation
class SimpleJsonParser {
public:
    static bool parse(const std::string& json, std::vector<ConfigEntry>& entries) {
        // Remove whitespace and newlines
        std::string cleaned;
        for (char c : json) {
            if (!std::isspace(c)) {
                cleaned += c;
            }
        }
        
        if (cleaned.empty() || cleaned[0] != '{') {
            return false;
        }
        
        // Try to parse as JSON object
        size_t pos = 1;  // Skip opening brace
        size_t end = cleaned.size() - 1;  // Skip closing brace
        
        while (pos < end) {
            // Find next key
            size_t keyStart = cleaned.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = cleaned.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon after key
            size_t colonPos = cleaned.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value
            size_t valueStart = cleaned.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            char valueType = cleaned[valueStart];
            
            ConfigValue value;
            bool parseSuccess = false;
            
            // Parse different value types
            if (valueType == '"') {
                // String value
                size_t valueEnd = cleaned.find('"', valueStart + 1);
                if (valueEnd != std::string::npos) {
                    std::string strValue = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
                    value = strValue;
                    parseSuccess = true;
                    pos = valueEnd + 1;
                }
            }
            else if (valueType == 't' || valueType == 'f') {
                // Boolean value
                size_t valueEnd = cleaned.find_first_of(",}", valueStart);
                if (valueEnd != std::string::npos) {
                    std::string boolStr = cleaned.substr(valueStart, valueEnd - valueStart);
                    bool boolValue = (boolStr == "true" || boolStr == "t");
                    value = boolValue;
                    parseSuccess = true;
                    pos = valueEnd;
                }
            }
            else if (valueType == '-' || std::isdigit(valueType)) {
                // Number (int or float)
                size_t valueEnd = cleaned.find_first_of(",}", valueStart);
                if (valueEnd != std::string::npos) {
                    std::string numStr = cleaned.substr(valueStart, valueEnd - valueStart);
                    
                    // Try to parse as double first
                    try {
                        size_t posAfterNum;
                        double doubleVal = std::stod(numStr, &posAfterNum);
                        if (posAfterNum == numStr.size()) {
                            // Check if it could be an integer
                            bool isInteger = true;
                            for (char c : numStr) {
                                if (!std::isdigit(c) && c != '-') {
                                    isInteger = false;
                                    break;
                                }
                            }
                            if (isInteger && std::abs(doubleVal) <= std::numeric_limits<int64_t>::max()) {
                                value = static_cast<int64_t>(doubleVal);
                            } else {
                                value = doubleVal;
                            }
                            parseSuccess = true;
                            pos = valueEnd;
                        }
                    } catch (...) {
                        // Parsing failed
                    }
                }
            }
            
            if (parseSuccess) {
                entries.emplace_back(key, value, ConfigSource::File);
            }
            
            // Find next comma or closing brace
            size_t nextComma = cleaned.find(',', pos);
            size_t nextBrace = cleaned.find('}', pos);
            
            if (nextComma == std::string::npos && nextBrace == std::string::npos) break;
            
            if (nextBrace != std::string::npos && (nextComma == std::string::npos || nextBrace < nextComma)) {
                pos = nextBrace;  // Skip closing brace
            } else {
                pos = nextComma + 1;  // Skip comma and any following spaces
            }
        }
        
        return !entries.empty();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->filename = filepath;  // Track for later saves
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON first
    pImpl->entries.clear();
    if (SimpleJsonParser::parse(content, pImpl->entries)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    pImpl->entries.clear();
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
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
            
            // Try to infer type based on content
            if (!value.empty()) {
                // Check if it's a boolean
                if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::File);
                }
                // Check if it's a number
                else {
                    try {
                        size_t posAfterNum;
                        double doubleVal = std::stod(value, &posAfterNum);
                        if (posAfterNum == value.size()) {
                            // Check if it could be an integer
                            bool isInteger = true;
                            for (char c : value) {
                                if (!std::isdigit(c) && c != '-' && c != '+') {
                                    isInteger = false;
                                    break;
                                }
                            }
                            if (isInteger) {
                                set(key, static_cast<int64_t>(doubleVal), ConfigSource::File);
                            } else {
                                set(key, doubleVal, ConfigSource::File);
                            }
                        }
                    } catch (...) {
                        // Not a number, keep as string
                    }
                }
            }
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Invalid command line argument (missing =): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-') {
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Missing value for argument: " + arg);
                success = false;
            }
        } else {
            NLM_LOG_ERROR("Invalid command line argument: " + arg);
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Format based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double with reasonable precision
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
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    file.flush();
    
    NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
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
        NLM_LOG_WARNING("Type mismatch for config key: " + key);
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
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new config key: " + key);
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
    NLM_LOG_DEBUG("Removed config key: " + key);
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
    size_t count = pImpl->entries.size();
    pImpl->entries.clear();
    NLM_LOG_DEBUG("Cleared " + std::to_string(count) + " config entries");
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
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
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

// Add missing include for numeric_limits
#include <limits>

// Fix missing include for iomanip
#include <iomanip>
