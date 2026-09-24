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
    // Try JSON first (preferred)
    if (loadFromJSONFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON: " + filepath);
        return true;
    }
    
    // Fall back to YAML
    if (loadFromYAMLFile(filepath)) {
        NLM_LOG_INFO("Loaded configuration from YAML: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format (legacy)
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
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return !pImpl->entries.empty();
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
    // Try JSON first
    if (saveToJSONFile(filepath)) {
        NLM_LOG_INFO("Saved configuration to JSON: " + filepath);
        return true;
    }
    
    // Fall back to YAML
    if (saveToYAMLFile(filepath)) {
        NLM_LOG_INFO("Saved configuration to YAML: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format (legacy)
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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

// JSON loading implementation with improved error handling
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("JSON file not found: " + filepath);
        return false;
    }
    
    try {
        // Use nlohmann::json for proper JSON parsing if available
        // For now, use simple parsing with error handling
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        if (content.empty()) {
            NLM_LOG_WARNING("JSON file is empty: " + filepath);
            return false;
        }
        
        // Simple validation - check for basic JSON structure
        if (content.find("{") == std::string::npos || content.find("}") == std::string::npos) {
            NLM_LOG_WARNING("File is not valid JSON: " + filepath);
            return false;
        }
        
        size_t pos = 0;
        while ((pos = content.find('"', pos)) != std::string::npos) {
            // Find the key name
            size_t keyStart = pos + 1;
            size_t keyEnd = content.find('"', keyStart);
            if (keyEnd == std::string::npos) {
                NLM_LOG_WARNING("Invalid JSON format in file: " + filepath);
                break;
            }
            
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            
            // Find the value
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) {
                NLM_LOG_WARNING("Invalid JSON format in file: " + filepath);
                break;
            }
            
            size_t valueStart = content.find('"', colonPos);
            if (valueStart == std::string::npos) {
                // Try for number or boolean values
                size_t numEnd = content.find(',', colonPos);
                if (numEnd == std::string::npos) numEnd = content.find('}', colonPos);
                if (numEnd == std::string::npos) break;
                
                std::string value = content.substr(colonPos + 1, numEnd - colonPos - 1);
                key = trim(key);
                value = trim(value);
                set(key, value, ConfigSource::File);
                pos = numEnd + 1;
                continue;
            }
            
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                NLM_LOG_WARNING("Invalid JSON format in file: " + filepath);
                break;
            }
            
            std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            
            // Clean up key for config (remove quotes, trim)
            key = trim(key);
            value = trim(value);
            
            // Validate key doesn't contain invalid characters
            if (key.empty() || key.find(' ') != std::string::npos) {
                NLM_LOG_WARNING("Skipping invalid key in JSON file: " + key);
                pos = valueEnd + 1;
                continue;
            }
            
            set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        }
        
        return !pImpl->entries.empty();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// YAML loading implementation with improved error handling
bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("YAML file not found: " + filepath);
        return false;
    }
    
    try {
        std::string line;
        size_t lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            // Simple YAML parsing for key: value pairs
            line = trim(line);
            
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == '%') {
                continue;
            }
            
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = trim(line.substr(0, colonPos));
                std::string value = trim(line.substr(colonPos + 1));
                
                // Validate key
                if (key.empty()) {
                    NLM_LOG_WARNING("YAML file line " + std::to_string(lineNumber) + ": Empty key");
                    continue;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == \'\' && value.back() == \'\'))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Validate value
                if (value.empty()) {
                    NLM_LOG_WARNING("YAML file line " + std::to_string(lineNumber) + ": Empty value for key " + key);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("YAML file line " + std::to_string(lineNumber) + ": Invalid format (no colon found)");
            }
        }
        
        return !pImpl->entries.empty();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// JSON saving implementation with improved error handling
bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open JSON file for writing: " + filepath);
        return false;
    }
    
    try {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            
            // Write key-value pair
            file << "  \"" << escapeJSONString(entry.key) << "\": \"" << escapeJSONString(entry.key) << "\"";
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        file << "}\n";
        
        // Check if file was written successfully
        if (file.fail()) {
            NLM_LOG_ERROR("Failed to write JSON content to file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Successfully saved configuration to JSON: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing JSON file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// YAML saving implementation with improved error handling
bool Config::saveToYAMLFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open YAML file for writing: " + filepath);
        return false;
    }
    
    try {
        file << "%YAML 1.2\n---\n";
        
        for (const auto& entry : pImpl->entries) {
            // Write key-value pair
            file << escapeYAMLString(entry.key) << ": " << escapeYAMLString(entry.key) << "\n";
        }
        
        file << "...\n";
        
        // Check if file was written successfully
        if (file.fail()) {
            NLM_LOG_ERROR("Failed to write YAML content to file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Successfully saved configuration to YAML: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error writing YAML file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Helper function to escape JSON strings
std::string Config::escapeJSONString(const std::string& input) {
    std::string result;
    result.reserve(input.size() + 2); // Reserve space for quotes
    
    for (char c : input) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\0': result += "\\0"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    // Escape non-printable characters
                    char hex[7];
                    snprintf(hex, sizeof(hex), "\\u%04x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Helper functions for string escaping
std::string Config::escapeJSONString(const std::string& input) {
    std::string result;
    result.reserve(input.size() + 2); // Reserve space for quotes
    
    for (char c : input) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\0': result += "\\0"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    // Escape non-printable characters
                    char hex[7];
                    snprintf(hex, sizeof(hex), "\\u%04x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Helper function to escape YAML strings
std::string Config::escapeYAMLString(const std::string& input) {
    std::string result;
    result.reserve(input.size() + 2);
    
    for (char c : input) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\'' : result += "\\'"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32) {
                    // Escape control characters
                    char hex[7];
                    snprintf(hex, sizeof(hex), "\\\\x%02x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Get current timestamp for file generation
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Get memory usage estimate
size_t Config::getMemoryUsage() const {
    size_t total = 0;
    
    // Estimate memory for entries
    total += pImpl->entries.size() * sizeof(ConfigEntry);
    
    // Estimate string data (rough approximation)
    for (const auto& entry : pImpl->entries) {
        total += entry.key.capacity();
        // Estimate for variant value (rough approximation)
        total += 64; // Average estimated size per value
    }
    
    return total;
}

// Optimize memory usage by cleaning up unnecessary data
bool Config::optimizeMemoryUsage() {
    bool optimized = false;
    
    // Remove entries with default values if they're identical
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& entry) {
            return entry.source == ConfigSource::Default && 
                   entry.description.empty();
        });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it, pImpl->entries.end());
        optimized = true;
    }
    
    // Compact strings by removing unnecessary whitespace
    for (auto& entry : pImpl->entries) {
        std::string originalKey = entry.key;
        entry.key = trim(entry.key);
        if (entry.key != originalKey) {
            optimized = true;
        }
    }
    
    return optimized;
}

// Validate configuration integrity
bool Config::validateConfiguration() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.push_back("Empty key found");
        }
        
        // Validate that values are accessible
        try {
            // Try to access the value (this will catch variant access errors)
            auto _ = get<std::string>(entry.key);
        } catch (const std::bad_variant_access&) {
            // Try other types
            try { get<int64_t>(entry.key); }
            catch (const std::bad_variant_access&) {
                try { get<double>(entry.key); }
                catch (const std::bad_variant_access&) {
                    try { get<bool>(entry.key); }
                    catch (const std::bad_variant_access&) {
                        errors.push_back("Invalid value type for key: " + entry.key);
                    }
                }
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_WARNING("Configuration validation found " + std::to_string(errors.size()) + " issues");
        for (const auto& error : errors) {
            NLM_LOG_WARNING("  " + error);
        }
    }
    
    return errors.empty();
}

// Helper function to get string representation of config value
std::string Config::getConfigValueString(const Config& config, const std::string& key) {
    if (!config.has(key)) {
        return "";
    }
    
    std::stringstream ss;
    
    // Try to get the value in the most appropriate format
    try {
        auto str = config.get<std::string>(key);
        if (str.has_value()) {
            ss << "\"" << str.value() << "\"";
            return ss.str();
        }
    } catch (...) {}
    
    try {
        auto num = config.get<int64_t>(key);
        if (num.has_value()) {
            ss << num.value();
            return ss.str();
        }
    } catch (...) {}
    
    try {
        auto num = config.get<double>(key);
        if (num.has_value()) {
            ss << num.value();
            return ss.str();
        }
    } catch (...) {}
    
    try {
        auto flag = config.get<bool>(key);
        if (flag.has_value()) {
            ss << (flag.value() ? "true" : "false");
            return ss.str();
        }
    } catch (...) {}
    
    return "";
}

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm