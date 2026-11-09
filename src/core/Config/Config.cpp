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

// Configuration validation and helper methods
bool Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Validate key
        if (entry.key.empty()) {
            errors.push_back("Empty configuration key found");
        }
        
        if (entry.key.find('=') != std::string::npos) {
            errors.push_back("Configuration key cannot contain '=' character: '" + entry.key + "'");
        }
        
        if (entry.key.find('\n') != std::string::npos || entry.key.find('\r') != std::string::npos) {
            errors.push_back("Configuration key cannot contain newline characters: '" + entry.key + "'");
        }
        
        // Validate value based on type
        validateValue(entry, errors);
    }
    
    // Check for duplicate keys (should be impossible with current implementation, but good to check)
    std::unordered_set<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (seenKeys.count(entry.key) > 0) {
            errors.push_back("Duplicate configuration key: '" + entry.key + "'");
        }
        seenKeys.insert(entry.key);
    }
    
    // Check configuration consistency
    checkConfigurationConsistency(errors);
    
    // Print validation errors
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  - " + error);
        }
        return false;
    }
    
    return true;
}

void Config::validateValue(const ConfigEntry& entry, std::vector<std::string>& errors) const {
    std::visit([&errors, &entry](auto&& v) {
        using T = std::decay_t<decltype(v)>;
        
        if constexpr (std::is_same<T, int> || std::is_same<T, int64_t> || 
                      std::is_same<T, float> || std::is_same<T, double>) {
            validateNumericValue(entry.key, v, errors);
        } else if constexpr (std::is_same<T, bool>) {
            validateBooleanValue(entry.key, v, errors);
        } else if constexpr (std::is_same<T, std::string>) {
            validateStringValue(entry.key, v, errors);
        } else if constexpr (std::is_same<T, std::vector<int>> ||
                           std::is_same<T, std::vector<double>> ||
                           std::is_same<T, std::vector<std::string>>) {
            validateArrayValue(entry.key, v, errors);
        }
    }, entry.value);
}

void Config::validateNumericValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const {
    if constexpr (std::is_same<decltype(value), int>) {
        if (value < -1000000 || value > 1000000) {
            errors.push_back("Value out of range for " + key + ": " + std::to_string(value));
        }
    } else if constexpr (std::is_same<decltype(value), int64_t>) {
        if (value < -1000000000000 || value > 1000000000000) {
            errors.push_back("Value out of range for " + key + ": " + std::to_string(value));
        }
    } else if constexpr (std::is_same<decltype(value), float> ||
                       std::is_same<decltype(value), double>) {
        if (std::isinf(value) || std::isnan(value)) {
            errors.push_back("Invalid value (NaN or infinity) for " + key + ": " + std::to_string(value));
        }
        if (value < -1e9 || value > 1e9) {
            errors.push_back("Value out of range for " + key + ": " + std::to_string(value));
        }
    }
}

void Config::validateBooleanValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const {
    // Boolean values are always valid, but we could add constraints here
    // For example, certain keys might be expected to have specific boolean values
}

void Config::validateStringValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const {
    // Check string length
    if (value.length() > 1000) {
        errors.push_back("String value too long for " + key + ": " + std::to_string(value.length()) + " characters (max 1000)");
    }
    
    // Check for problematic characters
    if (value.find('\x00') != std::string::npos) {
        errors.push_back("String value contains null character for " + key);
    }
}

void Config::validateArrayValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const {
    if constexpr (std::is_same<decltype(value), std::vector<int>> ||
                  std::is_same<decltype(value), std::vector<double>>) {
        if (value.size() > 10000) {
            errors.push_back("Array value too large for " + key + ": " + std::to_string(value.size()) + " elements (max 10000)");
        }
        
        for (size_t i = 0; i < value.size(); ++i) {
            if constexpr (std::is_same<decltype(value), std::vector<int>>) {
                if (value[i] < -1000000 || value[i] > 1000000) {
                    errors.push_back("Array element " + std::to_string(i) + " out of range for " + key + ": " + std::to_string(value[i]));
                }
            } else {
                if (std::isinf(value[i]) || std::isnan(value[i])) {
                    errors.push_back("Array element " + std::to_string(i) + " invalid (NaN or infinity) for " + key);
                }
            }
        }
    } else if constexpr (std::is_same<decltype(value), std::vector<std::string>>) {
        for (size_t i = 0; i < value.size(); ++i) {
            if (value[i].length() > 100) {
                errors.push_back("Array element " + std::to_string(i) + " too long for " + key + ": " + std::to_string(value[i].length()) + " characters (max 100)");
            }
        }
    }
}

void Config::checkConfigurationConsistency(std::vector<std::string>& errors) const {
    // Check for required configuration keys
    std::unordered_set<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const auto& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            errors.push_back("Missing required configuration key: '" + requiredKey + "'");
        }
    }
    
    // Check consistency between related keys
    if (has("neuron_count") && has("region_count")) {
        auto neuronCountOpt = get<int64_t>("neuron_count");
        auto regionCountOpt = get<int64_t>("region_count");
        
        if (neuronCountOpt && regionCountOpt) {
            size_t neuronCount = static_cast<size_t>(neuronCountOpt.value());
            size_t regionCount = static_cast<size_t>(regionCountOpt.value());
            
            if (neuronCount > 0 && regionCount > neuronCount) {
                errors.push_back("region_count (" + std::to_string(regionCount) + ") cannot be greater than neuron_count (" + std::to_string(neuronCount) + ")");
            }
            
            if (neuronCount < 100 && regionCount > 10) {
                errors.push_back("region_count too high for small network: " + std::to_string(regionCount) + ". Consider increasing neuron_count or decreasing region_count");
            }
        }
    }
    
    // Check plasticity configuration consistency
    if (has("plasticity.stdp.enable") && has("plasticity.hebbian.enable")) {
        auto stdpOpt = get<bool>("plasticity.stdp.enable");
        auto hebbianOpt = get<bool>("plasticity.hebbian.enable");
        
        if (stdpOpt && hebbianOpt) {
            if (!stdpOpt.value() && !hebbianOpt.value()) {
                errors.push_back("Both STDP and Hebbian plasticity are disabled. At least one plasticity rule should be enabled.");
            }
        }
    }
    
    // Check memory configuration consistency
    if (has("performance.memory_pools.enable") && has("neuron_count")) {
        auto memoryPoolsOpt = get<bool>("performance.memory_pools.enable");
        auto neuronCountOpt = get<int64_t>("neuron_count");
        
        if (memoryPoolsOpt && neuronCountOpt) {
            if (memoryPoolsOpt.value() && neuronCountOpt.value() > 10000) {
                errors.push_back("Memory pools should be disabled for large networks (>10,000 neurons) due to memory constraints.");
            }
        }
    }
}

// Exception handling for configuration errors
void Config::handleConfigurationError(const std::string& error) {
    // This method would be called when configuration operations fail
    // It could log errors, throw exceptions, or take other appropriate actions
    NLM_LOG_ERROR("Configuration error: " + error);
    
    // In a more complete implementation, we could:
    // - Store error state
    // - Attempt recovery
    // - Notify error handlers
    // - Retry operations
    // - Auto-correction of invalid values
}

// Configuration schema validation
bool Config::validateSchema(const std::string& schemaFile) const {
    // This would validate the configuration against a schema file
    // For now, we'll return true (schema validation is a future enhancement)
    NLM_LOG_INFO("Schema validation not yet implemented (future enhancement)");
    return true;
}

// Safe configuration loading with validation
safe Config::loadFromFile(const std::string& filepath) {
    // Load configuration from file with validation
    if (!Config::validateFilePath(filepath)) {
        NLM_LOG_ERROR("Invalid configuration file path: " + filepath);
        return false;
    }
    
    bool success = Config::loadFromFileImpl(filepath);
    
    if (success) {
        if (!validate()) {
            NLM_LOG_ERROR("Configuration validation failed after loading from file: " + filepath);
            // Attempt to revert to previous state or use default configuration
            return false;
        }
    }
    
    return success;
}

// Safe configuration loading with backup and validation
safe bool Config::loadFromFileWithBackup(const std::string& filepath, const std::string& backupDir) {
    // Load configuration with backup and validation
    std::string backupPath = backupDir + "/backup_" + getTimestamp() + ".cfg";
    
    // Create backup of current configuration
    if (!saveToFile(backupPath)) {
        NLM_LOG_WARNING("Failed to create backup of current configuration");
    }
    
    bool success = loadFromFile(filepath);
    
ánoreturn success;
}

std::string Config::getTimestamp() const {
    // Generate timestamp for backup files
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

bool Config::validateFilePath(const std::string& filepath) const {
    // Validate file path
    if (filepath.empty() || filepath.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_./-") != std::string::npos) {
        return false;
    }
    
    // Check if file exists (read-only check)
    std::ifstream file(filepath);
    return file.good();
}

// Configuration recovery and rollback
bool Config::recoverConfiguration(const std::string& backupPath) {
    // Attempt to recover configuration from backup
    if (!validateFilePath(backupPath)) {
        return false;
    }
    
    // Create backup of current configuration
    std::string currentBackup = "backup_before_recovery_" + getTimestamp() + ".cfg";
    if (!saveToFile(currentBackup)) {
        NLM_LOG_ERROR("Failed to create backup before recovery");
    }
    
    // Load backup configuration
    bool success = loadFromFile(backupPath);
    
    if (success && !validate()) {
        NLM_LOG_ERROR("Recovered configuration is invalid. Attempting to restore defaults.");
        clear();
        // Restore to a safe default state
        setDefaultConfiguration();
        return true;
    }
    
    return success;
}

void Config::setDefaultConfiguration() {
    // Set configuration to safe defaults
    clear();
    set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    
    // Enable basic plasticity
    set("plasticity.stdp.enable", true, ConfigSource::Default);
    set("plasticity.hebbian.enable", true, ConfigSource::Default);
    
    // Enable basic performance optimizations
    set("performance.memory_pools.enable", true, ConfigSource::Default);
    set("performance.simd.enable", true, ConfigSource::Default);
}

std::vector<std::string> Config::getKeysByPrefix(const std::string& prefix) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.size() >= prefix.size() && 
            entry.key.compare(0, prefix.size(), prefix) == 0) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getKeysBySuffix(const std::string& suffix) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.size() >= suffix.size() &&
            entry.key.compare(entry.key.size() - suffix.size(), suffix.size(), suffix) == 0) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::unordered_map<std::string, ConfigValue> Config::getSection(const std::string& prefix) const {
    std::unordered_map<std::string, ConfigValue> section;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.size() >= prefix.size() &&
            entry.key.compare(0, prefix.size(), prefix) == 0) {
            // Remove prefix from key
            std::string sectionKey = entry.key.substr(prefix.size());
            if (!sectionKey.empty() && sectionKey[0] == '.') {
                sectionKey = sectionKey.substr(1); // Remove leading dot
            }
            section[sectionKey] = entry.value;
        }
    }
    return section;
}

bool Config::hasSection(const std::string& prefix) const {
    return !getSection(prefix).empty();
}

void Config::removeSection(const std::string& prefix) {
    auto section = getSection(prefix);
    for (const auto& key : section) {
        remove(key.first);
    }
}

std::string Config::toJSON() const {
    std::ostringstream json;
    json << "{";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) json << ",\n";
        first = false;
        json << "\"" << entry.key << "\": ";
        
        // Convert value to JSON
        std::visit([&json](auto&& v) {
            if constexpr (std::is_same_v<decltype(v), int>) {
                json << v;
            } else if constexpr (std::is_same_v<decltype(v), int64_t>) {
                json << v;
            } else if constexpr (std::is_same_v<decltype(v), double>) {
                json << v;
            } else if constexpr (std::is_same_v<decltype(v), bool>) {
                json << (v ? "true" : "false");
            } else if constexpr (std::is_same_v<decltype(v), std::string>) {
                json << "\"" << escapeJSONString(v) << "\"";
            } else if constexpr (std::is_same_v<decltype(v), std::vector<int>>) {
                json << "[";
                bool vecFirst = true;
                for (const auto& item : v) {
                    if (!vecFirst) json << ", ";
                    vecFirst = false;
                    json << item;
                }
                json << "]";
            } else if constexpr (std::is_same_v<decltype(v), std::vector<double>>) {
                json << "[";
                bool vecFirst = true;
                for (const auto& item : v) {
                    if (!vecFirst) json << ", ";
                    vecFirst = false;
                    json << item;
                }
                json << "]";
            } else if constexpr (std::is_same_v<decltype(v), std::vector<std::string>>) {
                json << "[";
                bool vecFirst = true;
                for (const auto& item : v) {
                    if (!vecFirst) json << ", ";
                    vecFirst = false;
                    json << "\"" << escapeJSONString(item) << "\"";
                }
                json << "]";
            } else {
                json << "null";
            }
        }, entry.value);
    }
    json << "\n}";
    return json.str();
}

std::string Config::escapeJSONString(const std::string& str) const {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string Config::getSummary() const {
    std::ostringstream summary;
    summary << "Configuration Summary:" << std::endl;
    summary << "Total entries: " << pImpl->entries.size() << std::endl;
    
    // Group by source
    std::unordered_map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    summary << "By source:" << std::endl;
    for (const auto& pair : sourceCount) {
        summary << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Sample of key values
    summary << "Sample entries:" << std::endl;
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (count >= 5) break;
        summary << "  " << entry.key << " = ";
        
        std::visit([&summary](auto&& v) {
            if constexpr (std::is_same_v<decltype(v), std::string>) {
                summary << "\"" << v << "\"";
            } else {
                summary << v;
            }
        }, entry.value);
        summary << " (" << static_cast<int>(entry.source) << ")" << std::endl;
        count++;
    }
    
    return summary.str();
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
