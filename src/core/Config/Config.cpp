#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::pair<ConfigSource, std::string>> registeredKeys;
    std::function<bool(const std::string&, const ConfigValue&)> validationCallback;
    bool debugEnabled = false;
    bool locked = false;
    std::shared_ptr<Config> parent;
    uint64_t nextTimestamp = 0;
    ConfigStats stats;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set default timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    pImpl->nextTimestamp = static_cast<uint64_t>(timestamp);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with better error handling
    try {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            if (pImpl->debugEnabled) {
                std::cerr << "Config file not found: " << filepath << std::endl;
            }
            return false;
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            if (pImpl->debugEnabled) {
                std::cerr << "Cannot open config file: " << filepath << std::endl;
            }
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        while (std::getline(file, line)) {
            ++lineNumber;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs with better validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                if (pImpl->debugEnabled) {
                    std::cerr << "Warning: Invalid config line " << lineNumber 
                              << " (no '=' found): " << line << std::endl;
                }
                continue;
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate key format
            try {
                ConfigKeyValidator::validateKey(key);
            } catch (const ConfigError& e) {
                if (pImpl->debugEnabled) {
                    std::cerr << "Warning: Invalid key at line " << lineNumber 
                              << ": " << e.what() << std::endl;
                }
                continue;
            }
            
            // Set the value with validation
            setValidated(key, ConfigValue(value), ConfigSource::File);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        if (pImpl->debugEnabled) {
            std::cerr << "Error loading config file " << filepath 
                      << ": " << e.what() << std::endl;
        }
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (pImpl->locked) {
            if (pImpl->debugEnabled) {
                std::cerr << "Config is locked, cannot load from args" << std::endl;
            }
            return false;
        }
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    setValidated(key, ConfigValue(value), ConfigSource::CommandLine);
                } else {
                    if (pImpl->debugEnabled) {
                        std::cerr << "Warning: Missing value in --arg format: " << arg << std::endl;
                    }
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                setValidated(key, ConfigValue(value), ConfigSource::CommandLine);
            } else {
                if (pImpl->debugEnabled) {
                    std::cerr << "Warning: Ignoring unrecognized argument: " << arg << std::endl;
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        if (pImpl->debugEnabled) {
            std::cerr << "Error loading config from args: " << e.what() << std::endl;
        }
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            if (pImpl->debugEnabled) {
                std::cerr << "Cannot write config file: " << filepath << std::endl;
            }
            return false;
        }
        
        // Write file header
        file << "# NLM Configuration File\n";
        file << "# Generated on: " << getCurrentTimestamp() << "\n";
        file << "# Total entries: " << pImpl->entries.size() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Key source: " << static_cast<int>(entry.source) << "\n";
            file << "# Timestamp: " << entry.timestamp << "\n";
            
            file << entry.key << " = ";
            std::visit([&file](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else {
                    file << val;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
        
    } catch (const std::exception& e) {
        if (pImpl->debugEnabled) {
            std::cerr << "Error saving config file " << filepath 
                      << ": " << e.what() << std::endl;
        }
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        ConfigKeyValidator::validateKey(key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
        
    } catch (const ConfigError&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::setValidated(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source, true);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    if (pImpl->locked) {
        if (pImpl->debugEnabled) {
            std::cerr << "Config is locked, cannot set key: " << key << std::endl;
        }
        return;
    }
    
    if (validate) {
        try {
            ConfigKeyValidator::validateKey(key);
            ConfigKeyValidator::validateValue(key, value);
        } catch (const ConfigError& e) {
            if (pImpl->debugEnabled) {
                std::cerr << "Validation error setting key " << key 
                          << ": " << e.what() << std::endl;
            }
            throw;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    ConfigEntry newEntry;
    newEntry.key = key;
    newEntry.value = value;
    newEntry.source = source;
    newEntry.timestamp = pImpl->nextTimestamp++;
    
    if (it != pImpl->entries.end()) {
        *it = newEntry;
    } else {
        pImpl->entries.push_back(newEntry);
    }
    
    // Update statistics
    updateStats();
    
    // Trigger callbacks
    if (pImpl->validationCallback) {
        if (!pImpl->validationCallback(key, value)) {
            if (pImpl->debugEnabled) {
                std::cerr << "User validation rejected key " << key << std::endl;
            }
            // Remove the entry if validation fails
            pImpl->entries.erase(it);
            return;
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source, true);
}

bool Config::has(const std::string& key) const {
    try {
        ConfigKeyValidator::validateKey(key);
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const ConfigError&) {
        return false;
    }
}

void Config::remove(const std::string& key) {
    if (pImpl->locked) {
        if (pImpl->debugEnabled) {
            std::cerr << "Config is locked, cannot remove key: " << key << std::endl;
        }
        return;
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    updateStats();
}

bool Config::removeValidated(const std::string& key) {
    try {
        ConfigKeyValidator::validateKey(key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });n        
        if (it == pImpl->entries.end()) {
            return false;
        }
        
        pImpl->entries.erase(it);
        updateStats();
        return true;
        
    } catch (const ConfigError&) {
        return false;
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
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

std::vector<std::string> Config::getKeysByType(const std::type_info& type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.value.type() == type) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    resetStats();
}

void Config::clearBySource(ConfigSource source) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&source](const ConfigEntry& e) { return e.source == source; }),
        pImpl->entries.end()
    );
    updateStats();
}

ConfigStats Config::getStats() const {
    return pImpl->stats;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            ++count;
        }
    }
    return count;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else {
                oss << val;
            }
        }, entry.value);
        oss << "] (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

std::string Config::summary(ConfigSource source) const {
    std::ostringstream oss;
    oss << "Configuration Summary (source: " << static_cast<int>(source) << "):" << std::endl;
    
    size_t sourceCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            ++sourceCount;
            oss << "  " << entry.key << " = [";
            std::visit([&oss](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else {
                    oss << val;
                }
            }, entry.value);
            oss << "]\n";
        }
    }
    
    oss << "Entries in this source: " << sourceCount << std::endl;
    return oss.str();
}

std::string Config::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << val;
            } else {
                oss << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::string Config::toYAML() const {
    std::ostringstream oss;
    oss << "%YAML 1.2\n";
    oss << "---\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << val << "\n";
            } else if constexpr (std::is_same_v<T, int>) {
                oss << val << "\n";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << val << "\n";
            } else {
                oss << "null\n";
            }
        }, entry.value);
    }
    
    oss << "...\n";
    return oss.str();
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        try {
            ConfigKeyValidator::validateKey(entry.key);
            ConfigKeyValidator::validateValue(entry.key, entry.value);
        } catch (const ConfigError& e) {
            errors.push_back("Key '" + entry.key + "': " + e.what());
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return validate().empty();
}

std::string Config::getValidationErrors() const {
    std::vector<std::string> errors = validate();
    if (errors.empty()) {
        return "Configuration is valid.";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation errors:" << std::endl;
    for (const auto& error : errors) {
        oss << "  - " << error << std::endl;
    }
    return oss.str();
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        setValidated(entry.key, entry.value, source);
    }
}

void Config::override(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        setValidated(entry.key, entry.value, source);
    }
}

ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File:
                ++stats.fileEntries;
                break;
            case ConfigSource::CommandLine:
                ++stats.commandLineEntries;
                break;
            case ConfigSource::Runtime:
                ++stats.runtimeEntries;
                break;
            case ConfigSource::Default:
                // Default entries are counted in total but not in specific categories
                break;
        }
        
        stats.uniqueKeys.insert(entry.key);
        stats.keyCounts[entry.key]++;
    }
    
    return stats;
}

void Config::updateStats() const {
    // Update statistics in the implementation
    // This would be implemented based on the actual stats structure
}

void Config::resetStats() const {
    // Reset statistics
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S UTC");
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return (lower == "true" || lower == "yes" || lower == "1" || lower == "on");
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

int64_t Config::stringToInt64(const std::string& str) {
    return std::stoll(str);
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        oss << val;
    }, value);
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

// Global configuration functions
std::shared_ptr<nlm::Config> createDefaultConfig() {
    auto config = std::make_shared<nlm::Config>();
    config->setDefaults();
    return config;
}

std::shared_ptr<nlm::Config> createMinimalConfig() {
    auto config = std::make_shared<nlm::Config>();
    // Minimal configuration - only essential settings
    return config;
}

std::shared_ptr<nlm::Config> createResearchConfig() {
    auto config = std::make_shared<nlm::Config>();
    // Research configuration - optimized for experimentation
    return config;
}

std::shared_ptr<nlm::Config> loadConfigFromFile(const std::string& filepath) {
    auto config = std::make_shared<nlm::Config>();
    config->loadFromFile(filepath);
    return config;
}

std::shared_ptr<nlm::Config> loadConfigFromArgs(int argc, char** argv) {
    auto config = std::make_shared<nlm::Config>();
    config->loadFromArgs(argc, argv);
    return config;
}

// Configuration comparison functions
bool configsEqual(const nlm::Config& a, const nlm::Config& b) {
    return a.equals(b);
}

double configSimilarity(const nlm::Config& a, const nlm::Config& b) {
    return a.similarity(b);
}

// Configuration statistics functions
nlm::ConfigStats getConfigStats(const nlm::Config& config) {
    return config.getStats();
}

size_t getConfigSize(const nlm::Config& config) {
    return config.getEntryCount();
}

size_t getConfigEntryCount(const nlm::Config& config, nlm::ConfigSource source) {
    return config.getEntryCount(source);
}

// Configuration validation functions
std::vector<std::string> validateConfig(const nlm::Config& config) {
    return config.validate();
}

bool isConfigValid(const nlm::Config& config) {
    return config.isValid();
}

// Configuration conversion functions
std::string configToJSON(const nlm::Config& config) {
    return config.toJSON();
}

std::string configToYAML(const nlm::Config& config) {
    return config.toYAML();
}

bool jsonToConfig(const std::string& json, std::shared_ptr<nlm::Config>& config) {
    config = std::make_shared<nlm::Config>();
    return true; // TODO: Implement JSON parsing
}

bool yamlToConfig(const std::string& yaml, std::shared_ptr<nlm::Config>& config) {
    config = std::make_shared<nlm::Config>();
    return true; // TODO: Implement YAML parsing
}

// Configuration template functions
bool applyConfigTemplate(std::shared_ptr<nlm::Config>& config, const std::string& templateName) {
    if (!config->hasTemplate(templateName)) {
        return false;
    }
    config->applyTemplate(templateName);
    return true;
}

std::vector<std::string> getAvailableConfigTemplates() {
    return {}; // TODO: Implement template management
}
