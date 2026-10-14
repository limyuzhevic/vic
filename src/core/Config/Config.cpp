#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include "Exceptions.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Validate file path
        if (filepath.empty()) {
            throw ConfigFileError("File path cannot be empty");
        }
        
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            throw ConfigFileError("File does not exist: '" + filepath + "'");
        }
        
        // Check if file is readable
        if (!std::filesystem::is_readable(filepath)) {
            throw ConfigFileError("File is not readable: '" + filepath + "'");
        }
        
        // Try JSON first, then fallback to simple key=value format
        bool success = false;
        
        // First try to parse as JSON
        success = loadFromJSON(filepath);
        if (!success) {
            // If JSON fails, try simple key=value format
            success = loadFromSimpleFormat(filepath);
        }
        
        return success;
    } catch (const ConfigFileError& e) {
        throw; // Re-throw ConfigFileError as-is
    } catch (const std::bad_alloc& e) {
        throw MemoryAllocationError("Memory allocation failed while loading config from file: '" + filepath + "': " + std::string(e.what()));
    } catch (const std::filesystem::filesystem_error& e) {
        throw ConfigFileError("Filesystem error loading file '" + filepath + "': " + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError("Failed to load config from file '" + filepath + "': " + e.what());
    } catch (...) {
        throw ConfigFileError("Unknown error loading config from file: '" + filepath + "'");
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parsing - looking for key-value pairs
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // Remove whitespace and newlines
        std::string cleaned;
        for (char c : content) {
            if (!std::isspace(c)) {
                cleaned += c;
            }
        }
        
        // Parse JSON-like format: {"key": "value", "key2": value2}
        if (cleaned.find('{') != 0 || cleaned.back() != '}') {
            return false;
        }
        
        // Remove braces
        std::string jsonContent = cleaned.substr(1, cleaned.size() - 2);
        
        // Parse key-value pairs
        size_t pos = 0;
        while (pos < jsonContent.size()) {
            // Find next key
            size_t keyStart = jsonContent.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonContent.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value after colon and comma
            size_t colonPos = jsonContent.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            size_t valueStart = jsonContent.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Check if value is string, number, boolean, or array
            char firstChar = jsonContent[valueStart];
            size_t valueEnd;
            
            if (firstChar == '"') {
                // String value
                valueEnd = jsonContent.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = jsonContent.substr(valueStart + 1, valueEnd - valueStart - 1);
                set(key, value, ConfigSource::File);
                pos = valueEnd + 1;
            } else if (firstChar == 't' || firstChar == 'f') {
                // Boolean value
                size_t nextComma = jsonContent.find(',', valueStart);
                if (nextComma == std::string::npos) {
                    nextComma = jsonContent.size();
                }
                std::string valueStr = jsonContent.substr(valueStart, nextComma - valueStart);
                bool value = (valueStr == "true");
                set(key, value, ConfigSource::File);
                pos = nextComma;
            } else if (firstChar == '-' || std::isdigit(firstChar)) {
                // Number value (integer or float)
                size_t nextComma = jsonContent.find(',', valueStart);
                if (nextComma == std::string::npos) {
                    nextComma = jsonContent.size();
                }
                std::string valueStr = jsonContent.substr(valueStart, nextComma - valueStart);
                
                // Try to parse as double first, then as int64 if no decimal
                try {
                    size_t decimalPos = valueStr.find('.');
                    if (decimalPos != std::string::npos) {
                        double value = std::stod(valueStr);
                        set(key, value, ConfigSource::File);
                    } else {
                        int64_t value = std::stoll(valueStr);
                        set(key, value, ConfigSource::File);
                    }
                    pos = nextComma;
                } catch (...) {
                    // Skip this value and continue
                    pos = nextComma;
                }
            } else {
                // Unknown type, skip
                pos = valueStart + 1;
            }
            
            // Skip comma and whitespace
            while (pos < jsonContent.size() && (jsonContent[pos] == ',' || jsonContent[pos] == ' ')) {
                pos++;
            }
        }
        
        return true;
    } catch (...) {
        // If JSON parsing fails, fallback to simple format
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool hasContent = false;
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
            
            // Try to infer type from value
            try {
                // Try boolean
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set(key, boolValue, ConfigSource::File);
                    hasContent = true;
                    continue;
                }
                
                // Try integer
                if (value.find('.') == std::string::npos) {
                    int64_t intValue = std::stoll(value);
                    set(key, intValue, ConfigSource::File);
                    hasContent = true;
                    continue;
                }
                
                // Try floating point
                double doubleValue = std::stod(value);
                set(key, doubleValue, ConfigSource::File);
                hasContent = true;
                continue;
                
            } catch (...) {
                // If all type conversions fail, treat as string
                set(key, value, ConfigSource::File);
                hasContent = true;
            }
        }
    }
    
    return hasContent;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasContent = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse value with type inference
                try {
                    // Try boolean
                    if (value == "true" || value == "false") {
                        bool boolValue = (value == "true");
                        set(key, boolValue, ConfigSource::CommandLine);
                        hasContent = true;
                        continue;
                    }
                    
                    // Try integer
                    if (value.find('.') == std::string::npos) {
                        int64_t intValue = std::stoll(value);
                        set(key, intValue, ConfigSource::CommandLine);
                        hasContent = true;
                        continue;
                    }
                    
                    // Try floating point
                    double doubleValue = std::stod(value);
                    set(key, doubleValue, ConfigSource::CommandLine);
                    hasContent = true;
                    continue;
                    
                } catch (...) {
                    // If all type conversions fail, treat as string
                    set(key, value, ConfigSource::CommandLine);
                    hasContent = true;
                }
            } else {
                // Handle --key value format
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    
                    try {
                        // Try boolean
                        if (value == "true" || value == "false") {
                            bool boolValue = (value == "true");
                            set(key, boolValue, ConfigSource::CommandLine);
                            hasContent = true;
                            continue;
                        }
                        
                        // Try integer
                        if (value.find('.') == std::string::npos) {
                            int64_t intValue = std::stoll(value);
                            set(key, intValue, ConfigSource::CommandLine);
                            hasContent = true;
                            continue;
                        }
                        
                        // Try floating point
                        double doubleValue = std::stod(value);
                        set(key, doubleValue, ConfigSource::CommandLine);
                        hasContent = true;
                        continue;
                        
                    } catch (...) {
                        // If all type conversions fail, treat as string
                        set(key, value, ConfigSource::CommandLine);
                        hasContent = true;
                    }
                }
            }
        }
    }
    
    return hasContent;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Validate file path
        if (filepath.empty()) {
            throw ConfigFileError("File path cannot be empty");
        }
        
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        std::filesystem::path parentDir = path.parent_path();
        if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
            if (!std::filesystem::create_directories(parentDir)) {
                throw ConfigFileError("Failed to create directory: '" + parentDir.string() + "'");
            }
        }
        
        // Check if file is writable (will be created or overwritten)
        std::ofstream testFile(filepath, std::ios::trunc);
        if (!testFile.is_open()) {
            throw ConfigFileError("File is not writable: '" + filepath + "'");
        }
        testFile.close();
        
        std::ofstream file(filepath, std::ios::trunc);
        if (!file.is_open()) {
            throw ConfigFileError("Failed to open file for writing: '" + filepath + "'");
        }
        
        // Save in simple key=value format (can be extended to JSON/YAML later)
        for (const auto& entry : pImpl->entries) {
            // Add description as comment if available
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            // Save key=value pair
            file << entry.key << " = ";
            
            // Convert ConfigValue to string based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                
                if constexpr (std::is_same_v<T, int>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << arg;
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
                        file << arg[i];
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
            
            file << "\n\n";  // Empty line between entries
        }
        
        if (!file.good()) {
            throw ConfigFileError("Error occurred while writing to file: '" + filepath + "'");
        }
        
        return true;
    } catch (const ConfigFileError& e) {
        throw; // Re-throw ConfigFileError as-is
    } catch (const std::bad_alloc& e) {
        throw MemoryAllocationError("Memory allocation failed while saving config to file: '" + filepath + "': " + std::string(e.what()));
    } catch (const std::filesystem::filesystem_error& e) {
        throw ConfigFileError("Filesystem error saving file '" + filepath + "': " + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError("Failed to save config to file '" + filepath + "': " + e.what());
    } catch (...) {
        throw ConfigFileError("Unknown error saving config to file: '" + filepath + "'");
    }
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

// Helper method to convert ConfigValue to string
std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
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
    }, value);
    
    return oss.str();
}

// Method to validate configuration
bool Config::validate() const {
    // Add configuration validation logic here
    // For example, check that required parameters are present
    // or that parameter values are within valid ranges
    return true;
}

// Method to get configuration statistics
std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    
    size_t entriesBySource = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: entriesBySource++; break;
            case ConfigSource::File: entriesBySource++; break;
            case ConfigSource::CommandLine: entriesBySource++; break;
            case ConfigSource::Runtime: entriesBySource++; break;
        }
    }
    
    oss << "  By source: Default=" << (entriesBySource / 4) 
        << ", File=" << (entriesBySource / 4) 
        << ", CommandLine=" << (entriesBySource / 4) 
        << ", Runtime=" << (entriesBySource / 4) << std::endl;
    
    return oss.str();
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
