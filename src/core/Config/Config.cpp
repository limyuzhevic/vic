#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to load as JSON first
    bool success = tryLoadJSON(filepath);
    if (success) {
        NLM_LOG_DEBUG("Config loaded as JSON from " + filepath);
        return true;
    }
    
    // Fall back to key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config file not found: " + filepath);
        return false;
    }
    
    std::string line;
    size_t validEntries = 0;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced validation
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Enhanced key validation
            if (key.empty() || key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") != std::string::npos) {
                NLM_LOG_WARNING("Invalid config key: " + key);
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to infer type from value
            std::optional<ConfigValue> typedValue;
            
            // Check numeric types
            bool isNumeric = false;
            if (value.find('.') != std::string::npos) {
                // Try double
                bool valid = false;
                double dValue = 0.0;
                try {
                    dValue = std::stod(value);
                    typedValue = ConfigValue(dValue);
                    isNumeric = true;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse numeric value: " + value);
                    typedValue = ConfigValue(0.0); // 0.0 default
                }
            } else {
                // Try int64
                bool valid = false;
                int64_t iValue = 0;
                try {
                    iValue = std::stoll(value);
                    typedValue = ConfigValue(iValue);
                    isNumeric = true;
                } catch (const std::exception& e) {
                    NLM_LOG_WARNING("Failed to parse integer value: " + value);
                }
            }
            
            // Try boolean
            if (!isNumeric) {
                std::string lowerValue = toLower(value);
                if (lowerValue == "true" || lowerValue == "false") {
                    bool bValue = (lowerValue == "true");
                    typedValue = ConfigValue(bValue);
                    isNumeric = true;
                }
            }
            
            // If not numeric or boolean, treat as string
            if (!isNumeric) {
                typedValue = ConfigValue(value);
            }
            
            if (typedValue.has_value()) {
                set(key, typedValue.value(), ConfigSource::File);
                validEntries++;
            } else {
                NLM_LOG_WARNING("Failed to parse value for key: " + key + ", value: " + value);
            }
        } else {
            NLM_LOG_WARNING("Invalid config line (no '='): " + line);
        }
    }
    
    if (validEntries == 0) {
        NLM_LOG_WARNING("No valid configuration entries found in " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Config loaded with " + std::to_string(validEntries) + " entries from " + filepath);
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

// JSON/YAML parsing helpers (simplified implementations)
bool Config::tryLoadJSON(const std::string& filepath) {
    // Try to parse as JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Check if file is empty
    if (content.empty()) {
        NLM_LOG_WARNING("Empty JSON file: " + filepath);
        return false;
    }
    
    // Basic JSON parsing (supports simple key-value pairs)
    try {
        // Remove whitespace
        content.erase(std::remove_if(content.begin(), content.end(), ::isspace), content.end());
        
        // Check if it looks like JSON
        if (content[0] == '{') {
            // Simple JSON parsing - extract key-value pairs
            size_t pos = 1;
            while (pos < content.size() && content[pos] != '}') {
                // Find next key
                size_t keyStart = content.find('"', pos);
                if (keyStart == std::string::npos) break;
                size_t keyEnd = content.find('"', keyStart + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
                pos = keyEnd + 2; // Skip ",
                
                // Find value
                if (content[pos] == '{') {
                    // Object - skip for now
                    size_t braceCount = 1;
                    pos++;
                    while (pos < content.size() && braceCount > 0) {
                        if (content[pos] == '{') braceCount++;
                        else if (content[pos] == '}') braceCount--;
                        pos++;
                    }
                } else if (content[pos] == '[') {
                    // Array - skip for now
                    size_t bracketCount = 1;
                    pos++;
                    while (pos < content.size() && bracketCount > 0) {
                        if (content[pos] == '[') bracketCount++;
                        else if (content[pos] == ']') bracketCount--;
                        pos++;
                    }
                } else {
                    // Simple value
                    size_t valueEnd = content.find(',', pos);
                    if (valueEnd == std::string::npos) valueEnd = content.find('}', pos);
                    if (valueEnd == std::string::npos) valueEnd = content.size();
                    
                    std::string valueStr = content.substr(pos, valueEnd - pos);
                    
                    // Try to parse value as different types
                    set(key, valueStr, ConfigSource::File);
                    pos = valueEnd + 1;
                }
            }
            
            NLM_LOG_INFO("JSON config loaded from " + filepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to parse JSON config: " + std::string(e.what()) + " in " + filepath);
    }
    
    return false;
}

void Config::addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    // Add validation rule for key
    validationRules[key] = validator;
}

bool Config::validate() const {
    // Validate all configuration entries
    for (const auto& entry : pImpl->entries) {
        auto it = validationRules.find(entry.key);
        if (it != validationRules.end()) {
            if (!it->second(entry.value)) {
                NLM_LOG_ERROR("Validation failed for key: " + entry.key);
                return false;
            }
        }
    }
    return true;
}

ConfigValue Config::safeGet(const std::string& key, const ConfigValue& defaultValue) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    return defaultValue;
}

bool Config::hasAny(const std::vector<std::string>& keys) const {
    for (const auto& key : keys) {
        if (has(key)) return true;
    }
    return false;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}
