#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format
    std::string format = detectFileFormat(filepath);
    
    if (format == "JSON") {
        return ConfigParser::parseJSONFile(filepath, pImpl->entries);
    } else if (format == "YAML") {
        return ConfigParser::parseYAMLFile(filepath, pImpl->entries);
    } else {
        // Try to parse as simple key=value format (backward compatibility)
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
                
                bool success = false;
                ConfigValue parsedValue = ConfigParser::parseValue(value, success);
                if (success) {
                    pImpl->entries.emplace_back(key, parsedValue, ConfigSource::File);
                }
            }
        }
        
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Parse command line arguments using improved logic
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
        // Handle -k value format (short options)
        else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            // Handle boolean flags (-key)
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Boolean flag
                set(key, true, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine file format from extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (ext == "json") {
        std::string jsonContent = ConfigParser::serializeToJSON(pImpl->entries);
        file << jsonContent;
    } else if (ext == "yaml" || ext == "yml") {
        std::string yamlContent = ConfigParser::serializeToYAML(pImpl->entries);
        file << yamlContent;
    } else {
        // Default to simple key=value format for backward compatibility
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << ConfigParser::valueToString(entry.value) << "\n";
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

std::string Config::detectFileFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "UNKNOWN";
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    // Check for JSON format
    if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
        return "JSON";
    }
    
    // Check for YAML format
    if (firstLine.find("-") != std::string::npos || firstLine.find(":") != std::string::npos) {
        size_t colonPos = firstLine.find(':');
        if (colonPos != std::string::npos && colonPos < 50) {
            return "YAML";
        }
    }
    
    // Default to simple format for backward compatibility
    return "SIMPLE";
}

std::vector<std::string> Config::parseCommandLineArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--" || (arg[0] == '-' && arg.size() > 1)) {
            args.push_back(arg);
        }
    }
    return args;
}

bool Config::validateConfig(const std::vector<ConfigEntry>& entries) {
    // Basic validation: ensure keys are not empty and don't contain invalid characters
    for (const auto& entry : entries) {
        if (entry.key.empty()) {
            return false;
        }
        // Check for potentially problematic characters in keys
        for (char c : entry.key) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
    }
    return true;
}

// ConfigParser implementation
bool ConfigParser::parseJSON(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified JSON parsing for demonstration
    // In a real implementation, you'd use a proper JSON library like nlohmann/json
    std::stringstream ss(content);
    std::string line;
    bool inObject = false;
    bool inArray = false;
    
    while (std::getline(ss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '/') {
            continue;
        }
        
        // Basic JSON key-value extraction (simplified)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove trailing comma
            if (!value.empty() && value.back() == ',') {
                value.pop_back();
            }
            
            // Remove surrounding quotes
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            bool success = false;
            ConfigValue parsedValue = parseValue(value, success);
            if (success) {
                entries.emplace_back(key, parsedValue, ConfigSource::File);
            }
        }
    }
    return !entries.empty();
}

bool ConfigParser::parseYAML(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified YAML parsing for demonstration
    // In a real implementation, you'd use a proper YAML library
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Basic YAML key: value extraction (simplified)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from value
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            bool success = false;
            ConfigValue parsedValue = parseValue(value, success);
            if (success) {
                entries.emplace_back(key, parsedValue, ConfigSource::File);
            }
        }
    }
    return !entries.empty();
}

bool ConfigParser::parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return parseJSON(content, entries);
}

bool ConfigParser::parseYAMLFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return parseYAML(content, entries);
}

std::string ConfigParser::serializeToJSON(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        oss << "  \"" << entry.key << "\": " << valueToString(entry.value);
        if (i < entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "}\n";
    return oss.str();
}

std::string ConfigParser::serializeToYAML(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    oss << "# Configuration file\n";
    
    for (const auto& entry : entries) {
        oss << "# " << entry.description << "\n";
        oss << entry.key << ": " << valueToString(entry.value) << "\n";
        oss << "\n";
    }
    
    return oss.str();
}

ConfigValue ConfigParser::parseValue(const std::string& str, bool& success) {
    success = false;
    
    if (str.empty()) {
        return std::string();
    }
    
    // Try to parse as boolean
    if (str == "true" || str == "false") {
        success = true;
        return str == "true";
    }
    
    // Try to parse as integer
    if (isDigit(str[0]) || (str[0] == '-' && str.size() > 1 && isDigit(str[1]))) {
        try {
            // Try int64_t first (handles large integers)
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                success = true;
                return intVal;
            }
            
            // Try double if int fails
            double doubleVal = std::stod(str);
            success = true;
            return doubleVal;
        } catch (...) {
            // Fall through to string
        }
    }
    
    // Default to string
    success = true;
    return str;
}

std::string ConfigParser::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
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
        }
    }, value);
    return oss.str();
}

std::string ConfigParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigParser::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ConfigParser::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string ConfigParser::unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 't': result += '\t'; i++; break;
                case 'r': result += '\r'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

bool ConfigParser::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool ConfigParser::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Helper function to get string representation of ConfigValue type
static std::string getValueTypeName(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return std::string("int");
        else if constexpr (std::is_same_v<T, int64_t>) return std::string("int64_t");
        else if constexpr (std::is_same_v<T, double>) return std::string("double");
        else if constexpr (std::is_same_v<T, bool>) return std::string("bool");
        else if constexpr (std::is_same_v<T, std::string>) return std::string("string");
        else if constexpr (std::is_same_v<T, std::vector<int>>) return std::string("std::vector<int>");
        else if constexpr (std::is_same_v<T, std::vector<double>>) return std::string("std::vector<double>");
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return std::string("std::vector<std::string>");
        else return std::string("unknown");
    }, value);
}

// Configuration statistics
bool Config::validate() const {
    return Config::validateConfig(pImpl->entries);
}

bool Config::hasRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    return true;
}

// Configuration statistics
bool Config::validate() const {
    return Config::validateConfig(pImpl->entries);
}

bool Config::hasRequiredKeys(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    return true;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
                       [source](const ConfigEntry& entry) {
                           return entry.source == source;
                       });
}

float Config::getProgressPercentage(ConfigSource source) const {
    if (pImpl->entries.empty()) return 0.0f;
    
    size_t totalEntries = pImpl->entries.size();
    size_t sourceEntries = getEntryCount(source);
    return (static_cast<float>(sourceEntries) / static_cast<float>(totalEntries)) * 100.0f;
}

// Configuration import/export
bool Config::importFromJSON(const std::string& jsonContent) {
    std::vector<ConfigEntry> entries;
    if (ConfigParser::parseJSON(jsonContent, entries)) {
        pImpl->entries = std::move(entries);
        return true;
    }
    return false;
}

bool Config::importFromYAML(const std::string& yamlContent) {
    std::vector<ConfigEntry> entries;
    if (ConfigParser::parseYAML(yamlContent, entries)) {
        pImpl->entries = std::move(entries);
        return true;
    }
    return false;
}

bool Config::exportToJSONFile(const std::string& filepath) const {
    return saveToFile(filepath); // saveToFile now handles JSON format
}

bool Config::exportToYAMLFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string yamlContent = ConfigParser::serializeToYAML(pImpl->entries);
    file << yamlContent;
    return true;
}

// Configuration modification
void Config::renameKey(const std::string& oldKey, const std::string& newKey) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&oldKey](const ConfigEntry& e) { return e.key == oldKey; });
    if (it != pImpl->entries.end()) {
        it->key = newKey;
    }
}

void Config::batchSet(const std::unordered_map<std::string, ConfigValue>& values,
                      ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

void Config::removeBySource(ConfigSource source) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [source](const ConfigEntry& e) { return e.source == source; }),
        pImpl->entries.end()
    );
}

// Configuration comparison
int Config::compare(const Config& other, const std::string& key = "") const {
    if (key.empty()) {
        if (pImpl->entries.size() != other.pImpl->entries.size()) {
            return static_cast<int>(pImpl->entries.size()) - static_cast<int>(other.pImpl->entries.size());
        }
    }
    
    for (const auto& entry : pImpl->entries) {
        if (!key.empty() && entry.key != key) continue;
        
        auto otherEntry = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                      [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherEntry == other.pImpl->entries.end()) {
            return -1; // This entry doesn't exist in other
        }
        
        // Compare values (simplified - in real implementation would compare ConfigValue properly)
        if (entry.key != otherEntry->key) {
            return entry.key < otherEntry->key ? -1 : 1;
        }
    }
    
    return 0; // Equal
}

// Configuration summary and reporting
std::string Config::generateReport() const {
    std::ostringstream oss;
    oss << "=== NLM Configuration Report ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "\nEntries by source:\n";
    
    for (int source = 0; source < 4; ++source) {
        size_t count = getEntryCount(static_cast<ConfigSource>(source));
        if (count > 0) {
            oss << "  " << source << ": " << count << " entries\n";
        }
    }
    
    oss << "\nEntry details:\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  " << (i + 1) << ". " << entry.key << " = " 
            << ConfigParser::valueToString(entry.value) << "\n";
    }
    
    return oss.str();
}

std::string Config::generateDetailedReport() const {
    std::ostringstream oss;
    oss << "=== Detailed NLM Configuration Report ===\n\n";
    oss << "Configuration Summary:" << summary() << "\n\n";
    
    oss << "Configuration by Source:" << std::endl;
    for (int source = 0; source < 4; ++source) {
        oss << "  " << static_cast<int>(source) << ": " << getEntryCount(static_cast<ConfigSource>(source)) << " entries\n";
    }
    
    oss << "\nDetailed Entry Analysis:" << std::endl;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  Entry " << i << ":\n";
        oss << "    Key: " << entry.key << std::endl;
        oss << "    Source: " << static_cast<int>(entry.source) << std::endl;
        oss << "    Description: " << entry.description << std::endl;
        oss << "    Type: " << getValueTypeName(entry.value) << std::endl;
        oss << "    Value: " << ConfigParser::valueToString(entry.value) << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

// Configuration backup and restore
bool Config::backup(const std::string& backupPath) const {
    std::string jsonContent = ConfigParser::serializeToJSON(pImpl->entries);
    std::ofstream file(backupPath);
    if (!file.is_open()) {
        return false;
    }
    file << jsonContent;
    return true;
}

bool Config::restore(const std::string& backupPath) {
    // Clear current configuration
    pImpl->entries.clear();
    
    // Try to restore from JSON backup
    std::ifstream file(backupPath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    return importFromJSON(jsonContent);
}

// Configuration monitoring
std::string Config::getChangeSummary(const Config& previousConfig) const {
    std::ostringstream oss;
    oss << "=== Configuration Change Summary ===\n\n";
    
    std::vector<std::string> addedKeys;
    std::vector<std::string> removedKeys;
    std::vector<std::string> modifiedKeys;
    
    // Find added keys
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& prevEntry : previousConfig.pImpl->entries) {
            if (entry.key == prevEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            addedKeys.push_back(entry.key);
        }
    }
    
    // Find removed keys
    for (const auto& prevEntry : previousConfig.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (prevEntry.key == entry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            removedKeys.push_back(prevEntry.key);
        }
    }
    
    oss << "Added keys: " << (addedKeys.empty() ? "None" : "");
    for (const auto& key : addedKeys) {
        oss << "\n  - " << key;
    }
    oss << "\n\nRemoved keys: " << (removedKeys.empty() ? "None" : "");
    for (const auto& key : removedKeys) {
        oss << "\n  - " << key;
    }
    
    if (addedKeys.empty() && removedKeys.empty()) {
        oss << "\nNo changes detected.\n";
    }
    
    return oss.str();
}

bool Config::hasChanges(const Config& other) const {
    return compare(*this, other) != 0;
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