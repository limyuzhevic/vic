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
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

// Save to file - with improved error handling and formatting
    bool saveToFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        // Write configuration in INI-like format for readability
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Format based on value type
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << val << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (val ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << val[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << val[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << val[i] << "\"";
                    }
                    file << "]\n";
                } else {
                    file << entry.key << " = " << val << "\n";
                }
            }, entry.value);
        }
        
        file << "\n"; // Add blank line at end
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

// Helper function to parse simple values from strings
static ConfigValue parseValue(const std::string& str) {
    // Try different types
    try {
        if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
            return str.substr(1, str.size() - 2); // String with quotes
        }
        if (str == "true") {
            return true;
        }
        if (str == "false") {
            return false;
        }
        if (str.find('.') != std::string::npos) {
            return std::stod(str);
        }
        try {
            return std::stoll(str);
        } catch (...) {
            // If too large for int64_t, treat as double
            return std::stod(str);
        }
    } catch (...) {
        return str; // Return as string if all parsing fails
    }
}

// Helper function to generate JSON from config entries
static std::string configToJSON(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << val[j];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << val[j];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) oss << ", ";
                    oss << "\"" << val[j] << "\"";
                }
                oss << "]";
            } else {
                oss << val;
            }
        }, entry.value);
        
        if (i < entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}\n";
    return oss.str();
}

// Helper function to generate INI format from config entries
static std::string configToINI(const std::vector<ConfigEntry>& entries) {
    std::ostringstream oss;
    for (const auto& entry : entries) {
        oss << "# " << entry.description << "\n";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << entry.key << " = \"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << entry.key << " = " << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << entry.key << " = [";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << val[i] << "\"";
                }
                oss << "]\n";
            } else {
                oss << entry.key << " = " << val << "\n";
            }
        }, entry.value);
        oss << "\n";
    }
    return oss.str();
}

// Configuration validation
bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return false; // Key doesn't exist
    }
    
    // Basic validation based on key patterns and value types
    // This can be extended with more specific validation rules
    return true;
}

bool Config::validateAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for required keys (can be extended)
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Check for valid ranges (can be extended)
    auto neuronCount = getOr<int64_t>("neuron_count", 0);
    if (neuronCount <= 0) {
        errors.push_back("neuron_count must be positive");
    }
    
    auto timestep = getOr<double>("simulation_timestep", 0.0);
    if (timestep <= 0.0 || timestep > 1.0) {
        errors.push_back("simulation_timestep must be between 0 and 1.0 seconds");
    }
    
    return errors;
}

// Configuration export/import
std::string Config::exportToJSON() const {
    return configToJSON(pImpl->entries);
}

bool Config::importFromJSON(const std::string& jsonStr) {
    // Simple JSON parser implementation
    // This is a basic implementation; a real JSON parser would be more robust
    clear();
    
    try {
        // Remove braces and whitespace
        std::string content = jsonStr;
        content.erase(std::remove(content.begin(), content.end(), '{'), content.end());
        content.erase(std::remove(content.begin(), content.end(), '}'), content.end());
        content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
        content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());
        content.erase(std::remove(content.begin(), content.end(), ' '), content.end());
        
        // Simple key-value parsing (expects "key": value format)
        size_t pos = 0;
        while (pos < content.size()) {
            // Find key (up to " : ")
            size_t keyEnd = content.find('"', pos);
            if (keyEnd == std::string::npos) break;
            
            size_t keyStart = pos;
            size_t colonPos = content.find('":', keyEnd);
            if (colonPos == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Parse value
            size_t valuePos = colonPos + 2;
            if (valuePos >= content.size()) break;
            
            // Find end of value (next , or })
            size_t valueEnd = content.find(',', valuePos);
            if (valueEnd == std::string::npos) {
                valueEnd = content.find('}', valuePos);
            }
            
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = content.substr(valuePos, valueEnd - valuePos);
            
            // Parse value string
            ConfigValue value = parseValue(valueStr);
            
            // Add to config
            set(key, value, ConfigSource::File);
            
            pos = valueEnd + 1;
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON: " + std::string(e.what()));
        return false;
    }
}

std::string Config::exportToINI() const {
    return configToINI(pImpl->entries);
}

bool Config::importFromINI(const std::string& iniStr) {
    // Parse INI-style format
    clear();
    
    try {
        std::istringstream stream(iniStr);
        std::string line;
        std::string currentDescription;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty()) continue;
            
            // Comment line - store as description for next entry
            if (line[0] == '#' || line[0] == '/') {
                currentDescription = line.substr(1);
                trim(currentDescription);
                continue;
            }
            
            // Parse key = value
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove surrounding quotes
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Try to parse value as different types
                ConfigValue parsedValue = parseValue(value);
                
                set(key, parsedValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing INI: " + std::string(e.what()));
        return false;
    }
}

// Configuration comparison and merging
bool Config::hasSameValues(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    // Compare entries
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& otherEntry) { return otherEntry.key == entry.key; });
        
        if (it == other.pImpl->entries.end()) {
            return false;
        }
        
        // Compare values (simplified comparison)
        if (entry.value != it->value) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Use the source from the other config unless explicitly overridden
        ConfigSource entrySource = (source == ConfigSource::Runtime) ? entry.source : source;
        set(entry.key, entry.value, entrySource);
    }
}

// Enhanced API methods
void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, static_cast<double>(value), source);
}

void Config::set(const std::string& key, uint64_t value, ConfigSource source) {
    set(key, static_cast<int64_t>(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, value, source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, value, source);
}

// Configuration statistics and debugging
 size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<ConfigSource> Config::getSources(const std::string& key) const {
    std::vector<ConfigSource> sources;
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            sources.push_back(entry.source);
        }
    }
    return sources;
}

} // namespace nlm
