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

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON parser first (Phase 5 enhancement)
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fallback to key=value format (Phase 1)
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Simple JSON parsing (can be enhanced with nlohmann/json library)
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Remove whitespace for parsing
        std::string trimmed;
        for (char c : content) {
            if (!std::isspace(c)) {
                trimmed += c;
            }
        }
        
        // Basic JSON object detection
        if (trimmed.empty() || trimmed[0] != '{' || trimmed.back() != '}') {
            return false; // Not a JSON object
        }
        
        // Parse simple key:value pairs (JSON format)
        // This is a simplified parser - in production use nlohmann/json
        size_t pos = 1; // Skip opening {
        while (pos < trimmed.size() - 1) {
            // Find next key
            size_t keyStart = trimmed.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = trimmed.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = trimmed.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 2; // Skip ",
            
            // Find colon separator
            size_t colonPos = trimmed.find(':', pos);
            if (colonPos == std::string::npos) break;
            pos = colonPos + 1;
            
            // Find value (could be quoted string, number, or boolean)
            if (pos < trimmed.size()) {
                char firstChar = trimmed[pos];
                
                if (firstChar == '"') {
                    // String value
                    size_t valueEnd = trimmed.find('"', pos + 1);
                    if (valueEnd == std::string::npos) break;
                    std::string value = trimmed.substr(pos + 1, valueEnd - pos - 1);
                    set(key, value, ConfigSource::File);
                    pos = valueEnd + 2;
                } else if (firstChar == 't' || firstChar == 'f') {
                    // Boolean value
                    size_t valueEnd = trimmed.find_first_of(",}", pos);
                    if (valueEnd == std::string::npos) break;
                    std::string boolStr = trimmed.substr(pos, valueEnd - pos);
                    bool boolVal = (boolStr == "true");
                    set(key, boolVal, ConfigSource::File);
                    pos = valueEnd + 1;
                } else {
                    // Numeric value (int or double)
                    size_t valueEnd = trimmed.find_first_of(",}", pos);
                    if (valueEnd == std::string::npos) break;
                    std::string numStr = trimmed.substr(pos, valueEnd - pos);
                    
                    // Try to parse as double first
                    try {
                        size_t posAfter = 0;
                        double doubleVal = std::stod(numStr, &posAfter);
                        if (posAfter == numStr.size()) {
                            set(key, doubleVal, ConfigSource::File);
                        } else {
                            // Try as integer
                            size_t intPosAfter = 0;
                            int64_t intVal = std::stoll(numStr, &intPosAfter);
                            if (intPosAfter == numStr.size()) {
                                set(key, intVal, ConfigSource::File);
                            }
                        }
                    } catch (...) {
                        // Skip invalid number
                    }
                    pos = valueEnd + 1;
                }
                
                // Skip comma if present
                if (pos < trimmed.size() && trimmed[pos] == ',') {
                    pos++;
                }
            }
        }
        
        return true;
    } catch (...) {
        return false; // JSON parsing failed, fallback to simple format
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
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
    // Auto-detect format based on file extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos);
    }
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        return saveToJSON(filepath);
    } else {
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
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

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
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
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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
                file << arg;
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

// Validation methods
bool Config::validate() const {
    // Basic validation - can be extended with specific validation rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            return false;
        }
        
        // Validate that string values don't contain problematic characters
        if (std::holds_alternative<std::string>(entry.value)) {
            const std::string& str = std::get<std::string>(entry.value);
            if (str.find('\0') != std::string::npos) {
                return false; // Null bytes not allowed
            }
        }
    }
    return true;
}

bool Config::hasNonDefaultValues() const {
    for (const auto& entry : pImpl->entries) {
        if (entry.source != ConfigSource::Default) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> Config::getValidatedKeys() const {
    std::vector<std::string> validatedKeys;
    for (const auto& entry : pImpl->entries) {
        if (validateKey(entry.key)) {
            validatedKeys.push_back(entry.key);
        }
    }
    return validatedKeys;
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

// Config entry management
void Config::addEntry(const std::string& key, const ConfigValue& value, 
                     const std::string& description, ConfigSource source) {
    if (validateKey(key)) {
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

// Config statistics
ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<int>(entry.source)]++;
        
        std::visit([&stats](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                stats.valuesByType["int"]++;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                stats.valuesByType["int64"]++;
            } else if constexpr (std::is_same_v<T, double>) {
                stats.valuesByType["double"]++;
            } else if constexpr (std::is_same_v<T, bool>) {
                stats.valuesByType["bool"]++;
            } else if constexpr (std::is_same_v<T, std::string>) {
                stats.valuesByType["string"]++;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                stats.valuesByType["int_vector"]++;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                stats.valuesByType["double_vector"]++;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                stats.valuesByType["string_vector"]++;
            }
        }, entry.value);
    }
    
    return stats;
}

// Serialization
std::string Config::serialize(const std::string& format) const {
    if (format == "json") {
        return serializeToJSON();
    } else if (format == "yaml") {
        return serializeToYAML();
    } else if (format == "ini") {
        return serializeToINI();
    } else {
        return serializeToSimple();
    }
}

std::string Config::serializeToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            oss << ",\n";
        }
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        
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
    }
    
    oss << "\n}\n";
    return oss.str();
}

std::string Config::serializeToYAML() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        
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
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::serializeToINI() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << entry.key << " = \\\"" << arg << "\\\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                oss << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::serializeToSimple() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << "# " << entry.description << "\n";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << entry.key << " = \\\"" << arg << "\\\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                oss << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        oss << "\n";
    }
    
    return oss.str();
}

} // namespace nlm
