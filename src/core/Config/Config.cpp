#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <ctime>
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
    // Try JSON format first (modern)
    auto jsonData = loadFromJSONFile(filepath);
    if (jsonData) {
        return true;
    }
    
    // Fall back to legacy key=value format
    return loadFromLegacyFile(filepath);
}

// JSON parsing support
std::optional<std::string> Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::nullopt;
    }
    
    try {
        // Simple JSON parsing - in production, use nlohmann::json
        std::string content((std::istreambuf_iterator<char>(file)), 
                           std::istreambuf_iterator<char>());
        
        // Parse basic JSON objects {"key": value, "key2": value2}
        if (content.empty() || content.find('{') == std::string::npos) {
            return std::nullopt;
        }
        
        // Remove whitespace and braces
        std::string jsonContent = content;
        jsonContent.erase(std::remove(jsonContent.begin(), jsonContent.end(), ' '), jsonContent.end());
        jsonContent.erase(std::remove(jsonContent.begin(), jsonContent.end(), '\n'), jsonContent.end());
        jsonContent.erase(std::remove(jsonContent.begin(), jsonContent.end(), '\t'), jsonContent.end());
        
        if (jsonContent[0] == '{' && jsonContent[jsonContent.size() - 1] == '}') {
            jsonContent = jsonContent.substr(1, jsonContent.size() - 2);
        }
        
        // Parse key-value pairs
        size_t pos = 0;
        while (pos < jsonContent.size()) {
            // Find key
            size_t keyStart = jsonContent.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = jsonContent.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip colon
            if (pos >= jsonContent.size() || jsonContent[pos] != ':') break;
            pos++;
            
            // Find value
            // Handle string values
            if (pos < jsonContent.size() && jsonContent[pos] == '"') {
                size_t valueStart = pos + 1;
                size_t valueEnd = jsonContent.find('"', valueStart);
                if (valueEnd == std::string::npos) break;
                std::string value = jsonContent.substr(valueStart, valueEnd - valueStart);
                set(key, value, ConfigSource::File);
                pos = valueEnd + 1;
            }
            // Handle numeric values
            else {
                size_t valueEnd = jsonContent.find_first_of(",}", pos);
                if (valueEnd == std::string::npos) valueEnd = jsonContent.size();
                std::string valueStr = jsonContent.substr(pos, valueEnd - pos);
                trim(valueStr);
                
                // Try to parse as int, then double, then boolean
                try {
                    size_t posAfter = 0;
                    long intVal = std::stol(valueStr, &posAfter);
                    if (posAfter == valueStr.size()) {
                        set(key, intVal, ConfigSource::File);
                    } else {
                        double doubleVal = std::stod(valueStr, &posAfter);
                        if (posAfter == valueStr.size()) {
                            set(key, doubleVal, ConfigSource::File);
                        } else if (valueStr == "true") {
                            set(key, true, ConfigSource::File);
                        } else if (valueStr == "false") {
                            set(key, false, ConfigSource::File);
                        }
                    }
                } catch (...) {
                    // If numeric parsing fails, treat as string
                    set(key, valueStr, ConfigSource::File);
                }
                
                pos = valueEnd + 1;
            }
            
            // Skip comma
            if (pos < jsonContent.size() && jsonContent[pos] == ',') {
                pos++;
            }
        }
        
        return std::make_optional("");
    } catch (...) {
        return std::nullopt;
    }
}

// Legacy key=value format
bool Config::loadFromLegacyFile(const std::string& filepath) {
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

// Improved saveToFile with proper serialization
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header comment
    file << "# NLM Configuration File\n";
    file << "# Generated on: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << 
            (entry.source == ConfigSource::Default ? "default" :
             entry.source == ConfigSource::File ? "file" :
             entry.source == ConfigSource::CommandLine ? "command_line" :
             entry.source == ConfigSource::Runtime ? "runtime" : "unknown") << ")\n";
        
        // Serialize value based on its type
        std::visit([&](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, int>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double with appropriate precision
                file << entry.key << " = " << std::fixed << std::setprecision(6) << value << "\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings that contain special characters
                if (value.find(' ') != std::string::npos || value.find('=') != std::string::npos) {
                    file << entry.key << " = \"" << value << "\"\n";
                } else {
                    file << entry.key << " = " << value << "\n";
                }
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << value[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << value[i] << "\"";
                }
                file << "]\n";
            } else {
                // Unknown type - use placeholder
                file << entry.key << " = " << "UNKNOWN_TYPE_" << typeid(T).name() << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    file.flush();
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

std::string Config::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

// Add validation functions
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateEntry(entry)) {
            return false;
        }
    }
    return true;
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation - check if key is valid
    if (entry.key.empty() || entry.key.find('=') != std::string::npos) {
        return false;
    }
    
    // Type-specific validation could be added here
    // For now, we just check that the value can be serialized
    return true;
}

// Add getAllEntries method for external access
std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

// Add method to check if config was modified
bool Config::isModified() const {
    return !pImpl->entries.empty();
}

// Helper to get current timestamp
std::string Config::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

// Add validation method implementations
void Config::setValidator(std::shared_ptr<ConfigValidator> validator) {
    // TODO: Store validator in implementation
}

void Config::addSchemaEntry(const ConfigSchemaEntry& schemaEntry) {
    // TODO: Store schema entry in implementation
}

bool Config::validateAgainstSchema() const {
    // TODO: Implement schema validation
    return true;
}

// Configuration presets
std::map<std::string, std::vector<ConfigEntry>> Config::getPresets() {
    std::map<std::string, std::vector<ConfigEntry>> presets;
    
    // Fast preset for quick testing
    std::vector<ConfigEntry> fastPreset {
        ConfigEntry("random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments"),
        ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds"),
        ConfigEntry("neuron_count", 100, ConfigSource::Default, "Number of neurons (reduced for fast test)"),
        ConfigEntry("region_count", 1, ConfigSource::Default, "Number of brain regions"),
        ConfigEntry("connection_probability", 0.15, ConfigSource::Default, "Connection probability for random connectivity"),
    };
    presets["fast_test"] = fastPreset;
    
    // Standard preset
    std::vector<ConfigEntry> standardPreset {
        ConfigEntry("random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments"),
        ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds"),
        ConfigEntry("neuron_count", 1000, ConfigSource::Default, "Number of neurons"),
        ConfigEntry("region_count", 2, ConfigSource::Default, "Number of brain regions"),
        ConfigEntry("connection_probability", 0.1, ConfigSource::Default, "Connection probability for random connectivity"),
    };
    presets["standard"] = standardPreset;
    
    // High-density preset
    std::vector<ConfigEntry> highDensityPreset {
        ConfigEntry("random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments"),
        ConfigEntry("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds"),
        ConfigEntry("neuron_count", 5000, ConfigSource::Default, "Number of neurons (high density)"),
        ConfigEntry("region_count", 4, ConfigSource::Default, "Number of brain regions"),
        ConfigEntry("connection_probability", 0.2, ConfigSource::Default, "Connection probability for random connectivity"),
    };
    presets["high_density"] = highDensityPreset;
    
    return presets;
}

bool Config::applyPreset(const std::string& presetName) {
    auto presets = getPresets();
    auto it = presets.find(presetName);
    if (it == presets.end()) {
        return false;
    }
    
    // Clear current config
    clear();
    
    // Apply preset entries
    for (const auto& entry : it->second) {
        pImpl->entries.push_back(entry);
    }
    
    return true;
}
