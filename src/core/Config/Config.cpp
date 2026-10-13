#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <chrono>

// Optional: You can add nlohmann/json.hpp here if you want to support JSON files
// #include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_DEBUG("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found or cannot be opened: " + filepath);
        return false;
    }
    
    // Check file extension to determine format
    std::string ext = "";
    size_t dotPos = filepath.rfind('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    bool success = false;
    if (ext == "json" || ext == "JSON") {
        success = loadFromJSON(file);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML") {
        NLM_LOG_WARNING("YAML parsing not yet implemented, falling back to simple key=value format");
        // TODO: Implement YAML parsing
        // For now, fall back to simple format
        success = loadFromSimpleFormat(file);
    } else {
        // Default: try to detect format from content
        std::string firstLine;
        if (std::getline(file, firstLine)) {
            file.clear();
            file.seekg(0);
            
            // Check if it looks like JSON
            if (firstLine.trim().front() == '{') {
                success = loadFromJSON(file);
            } else {
                success = loadFromSimpleFormat(file);
            }
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from: " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
    }
    
    return success;
}

bool Config::loadFromSimpleFormat(std::ifstream& file) {
    std::string line;
    int lineNumber = 0;
    int successCount = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line in simple format: line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key found in configuration at line " + std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Type inference based on value content
        try {
            // Try to parse as integer
            size_t posEnd;
            int intValue = std::stoi(value, &posEnd);
            if (posEnd == value.length()) {
                set(key, intValue, ConfigSource::File);
                successCount++;
                continue;
            }
            
            // Try to parse as double
            double doubleValue = std::stod(value, &posEnd);
            if (posEnd == value.length()) {
                set(key, doubleValue, ConfigSource::File);
                successCount++;
                continue;
            }
            
            // Try to parse as boolean
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                set(key, boolValue, ConfigSource::File);
                successCount++;
                continue;
            }
            
            // Default to string
            set(key, value, ConfigSource::File);
            successCount++;
            
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to parse value for key '" + key + "' at line " + 
                           std::to_string(lineNumber) + ": " + e.what());
            // Fall back to storing as string
            set(key, value, ConfigSource::File);
            successCount++;
        }
    }
    
    NLM_LOG_DEBUG("Loaded " + std::to_string(successCount) + " entries from simple configuration format");
    return !pImpl->entries.empty();
}

bool Config::loadFromJSON(std::ifstream& file) {
    NLM_LOG_DEBUG("Attempting to parse JSON configuration");
    
    // For now, implement a simple JSON parser
    // This is a minimal implementation that can be replaced with nlohmann/json
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing - expects format: {"key": "value", "number": 42}
    if (content.empty() || content.find("{") == std::string::npos) {
        NLM_LOG_WARNING("Invalid JSON format: missing opening brace");
        return false;
    }
    
    // This is a very basic JSON parser - in production, use nlohmann/json
    // For now, fall back to simple format
    file.clear();
    file.seekg(0);
    return loadFromSimpleFormat(file);
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
        // Save actual value instead of placeholder
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
        file << "\n";
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

// Enhanced set method with input validation
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    NLM_LOG_DEBUG("Setting config key: '" + key + "' = '" + value + "' (source: " + std::to_string(static_cast<int>(source)) + ")");
    
    // Validate key
    if (key.empty()) {
        NLM_LOG_ERROR("Attempted to set config with empty key");
        return;
    }
    
    // Validate key format (alphanumeric and underscores)
    if (!std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    })) {
        NLM_LOG_WARNING("Config key contains non-alphanumeric characters: '" + key + "'");
    }
    
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    NLM_LOG_DEBUG("Setting config key: '" + key + "' = " + std::to_string(value) + " (source: " + std::to_string(static_cast<int>(source)) + ")");
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    NLM_LOG_DEBUG("Setting config key: '" + key + "' = " + std::to_string(value) + " (source: " + std::to_string(static_cast<int>(source)) + ")");
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    NLM_LOG_DEBUG("Setting config key: '" + key + "' = " + std::string(value ? "true" : "false") + " (source: " + std::to_string(static_cast<int>(source)) + ")");
    set(key, ConfigValue(value), source);
}

// Helper method to format variant values for logging
template<typename T>
std::string Config::formatValue(const ConfigValue& value) const {
    try {
        T val = std::get<T>(value);
        if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + val + "\"";
        } else {
            return std::to_string(val);
        }
    } catch (const std::bad_variant_access&) {
        return "[unknown type]";
    }
}

// Simple format loader (for backward compatibility and fallback)
bool Config::loadFromSimpleFormat(std::ifstream& file) {
    NLM_LOG_DEBUG("Loading configuration using simple key=value format");
    
    std::string line;
    int lineNumber = 0;
    int successCount = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid configuration line in simple format: line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key found in configuration at line " + std::to_string(lineNumber));
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Type inference based on value content
        try {
            // Try to parse as integer
            size_t posEnd;
            int intValue = std::stoi(value, &posEnd);
            if (posEnd == value.length()) {
                set(key, intValue, ConfigSource::File);
                successCount++;
                continue;
            }
            
            // Try to parse as double
            double doubleValue = std::stod(value, &posEnd);
            if (posEnd == value.length()) {
                set(key, doubleValue, ConfigSource::File);
                successCount++;
                continue;
            }
            
            // Try to parse as boolean
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                set(key, boolValue, ConfigSource::File);
                successCount++;
                continue;
            }
            
            // Default to string
            set(key, value, ConfigSource::File);
            successCount++;
            
        } catch (const std::exception& e) {
            NLM_LOG_WARNING("Failed to parse value for key '" + key + "' at line " + 
                           std::to_string(lineNumber) + ": " + e.what());
            // Fall back to storing as string
            set(key, value, ConfigSource::File);
            successCount++;
        }
    }
    
    NLM_LOG_DEBUG("Loaded " + std::to_string(successCount) + " entries from simple configuration format");
    return !pImpl->entries.empty();
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
