#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool jsonMode = false;  // Whether file was parsed as JSON
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to check if string is valid JSON
static bool isValidJSON(const std::string& str) {
    if (str.empty() || str.find('{') == std::string::npos) return false;
    // Simple check for basic JSON structure
    return str.find('"') != std::string::npos;
}

// Parse JSON-like format (simplified JSON parser for compatibility)
bool parseSimpleJSON(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
    try {
        // Simple JSON parsing - handles basic format: {"key": value, "key2": "value2"}
        size_t pos = 0;
        std::string content = jsonStr;
        
        // Remove whitespace
        content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());
        
        if (content.empty() || content[0] != '{' || content.back() != '}') {
            return false;
        }
        
        content = content.substr(1, content.size() - 2);  // Remove braces
        
        size_t keyStart = std::string::npos;
        size_t keyEnd = std::string::npos;
        size_t valueStart = std::string::npos;
        
        while (pos < content.size()) {
            // Find next key
            keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            valueStart = content.find_first_not_of(' ', colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Determine value type and extract
            ConfigValue value;
            
            if (content[valueStart] == '"') {
                // String value
                size_t stringEnd = content.find('"', valueStart + 1);
                if (stringEnd == std::string::npos) break;
                std::string strValue = content.substr(valueStart + 1, stringEnd - valueStart - 1);
                value = strValue;
                pos = stringEnd + 1;
            } else if (content[valueStart] == 't' || content[valueStart] == 'f') {
                // Boolean value
                std::string boolStr = content.substr(valueStart, 5);
                if (boolStr == "true" || boolStr == "false") {
                    value = (boolStr == "true");
                    pos = valueStart + boolStr.size();
                } else {
                    break;
                }
            } else if (content[valueStart] == '-' || std::isdigit(content[valueStart])) {
                // Number value
                size_t numEnd = content.find_first_not_of("0123456789.-+", valueStart);
                std::string numStr = content.substr(valueStart, numEnd - valueStart);
                
                // Try to parse as integer first
                try {
                    size_t posEnd;
                    int64_t intVal = std::stoll(numStr, &posEnd);
                    if (posEnd == numStr.size()) {
                        value = intVal;
                    } else {
                        // Try as double
                        double doubleVal = std::stod(numStr);
                        value = doubleVal;
                    }
                    pos = numEnd;
                } catch (...) {
                    break;
                }
            } else {
                // Skip unknown value type
                break;
            }
            
            // Add entry
            entries.emplace_back(key, value, ConfigSource::File);
            
            // Find comma separator or end
            while (pos < content.size() && content[pos] != ',' && !std::isalnum(content[pos])) {
                pos++;
            }
            if (pos < content.size() && content[pos] == ',') {
                pos++;  // Skip comma
            }
            
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) {
                pos++;
            }
        }
        
        return !entries.empty();
    } catch (...) {
        return false;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (content.empty()) {
        return false;
    }
    
    // Clear existing entries
    pImpl->entries.clear();
    
    // Try to parse as JSON first
    pImpl->jsonMode = false;
    if (isValidJSON(content)) {
        if (parseSimpleJSON(content, pImpl->entries)) {
            pImpl->jsonMode = true;
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        }
    }
    
    // Fall back to simple key=value format
    std::string line;
    std::istringstream stream(content);
    while (std::getline(stream, line)) {
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
    
    if (!pImpl->entries.empty()) {
        NLM_LOG_INFO("Loaded configuration from simple format file: " + filepath);
        return true;
    }
    
    return false;
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
    
    // Save in JSON format if entries are in JSON mode
    if (pImpl->jsonMode) {
        file << "{" << std::endl;
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int> || 
                                     std::is_same_v<T, double>) {
                    file << arg;
                } else {
                    file << "\"unknown\"";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << "," << std::endl;
            }
        }
        file << std::endl << "}" << std::endl;
    } else {
        // Save in simple key=value format
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
    pImpl->jsonMode = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << (pImpl->jsonMode ? "JSON" : "simple format") << "):" << std::endl;
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
