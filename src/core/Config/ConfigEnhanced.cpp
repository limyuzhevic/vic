// Enhanced Configuration Implementation for NLM
// This implementation extends the basic Config class with comprehensive features

#include "ConfigEnhanced.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>
#include <cctype>

namespace nlm {

// Config implementation details
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::map<std::string, std::string> descriptions;
    bool autoSaveEnabled = false;
    std::string autoSavePath;
    double autoSaveInterval = 0.0;
    std::chrono::steady_clock::time_point lastSaveTime;
    bool validationEnabled = true;
    bool strictMode = false;
    
    // Statistics
    size_t totalLoadCount = 0;
    size_t totalSaveCount = 0;
    size_t validationErrorCount = 0;
    std::chrono::steady_clock::time_point creationTime;
    
    Impl() : creationTime(std::chrono::steady_clock::now()) {}
};

// Config constructor/destructor
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from JSON file
bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Import from JSON
        bool success = importFromJSON(j, ConfigSource::File);
        
        if (success) {
            pImpl->totalLoadCount++;
            // Enable auto-save if configured
            if (pImpl->autoSaveEnabled && !pImpl->autoSavePath.empty()) {
                if (std::filesystem::exists(pImpl->autoSavePath)) {
                    pImpl->lastSaveTime = std::chrono::steady_clock::now();
                }
            }
        }
        
        return success;
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

// Load from environment variables
bool Config::loadFromEnvironment(const std::string& prefix) {
    bool loaded = false;
    
    // Common environment variable names for NLM
    const std::vector<std::string> envVars = {
        "NLM_RANDOM_SEED",
        "NLM_SIMULATION_TIMESTEP",
        "NLM_NEURON_COUNT",
        "NLM_REGION_COUNT",
        "NLM_CONNECTION_PROBABILITY",
        "NLM_STDP_LTP_WEIGHT",
        "NLM_STDP_LTD_WEIGHT",
        "NLM_STDP_TAU",
        "NLM_SYNAPTAGENESIS_RATE",
        "NLM_PRUNING_RATE",
        "NLM_CONFIG_FILE",
        "NLM_MODE",
        "NLM_MAX_STEPS",
        "NLM_CHECKPOINT_INTERVAL"
    };
    
    for (const auto& envVar : envVars) {
        const char* value = std::getenv(envVar.c_str());
        if (value) {
            std::string key = envVar;
            if (startsWith(key, prefix)) {
                key = key.substr(prefix.length());
            }
            set(key, std::string(value), ConfigSource::Environment, "Environment variable " + envVar);
            loaded = true;
        }
    }
    
    return loaded;
}

// Load from command line arguments
bool Config::loadFromArgs(int argc, char** argv) {
    bool loaded = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Normalize key
                key = normalizeKey(key);
                
                // Try to parse value based on key name
                bool parsed = false;
                
                if (key == "random_seed" || key == "seed") {
                    try {
                        int64_t seedVal = std::stoll(value);
                        setInt(key, seedVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "simulation_timestep" || key == "timestep") {
                    try {
                        double timestepVal = std::stod(value);
                        setDouble(key, timestepVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "neuron_count") {
                    try {
                        int64_t neuronCountVal = std::stoll(value);
                        setInt(key, neuronCountVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "region_count") {
                    try {
                        int64_t regionCountVal = std::stoll(value);
                        setInt(key, regionCountVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "connection_probability") {
                    try {
                        float probVal = std::stof(value);
                        setDouble(key, probVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "stdp_ltp_weight") {
                    try {
                        float weightVal = std::stof(value);
                        setDouble(key, weightVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "stdp_ltd_weight") {
                    try {
                        float weightVal = std::stof(value);
                        setDouble(key, weightVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "stdp_tau") {
                    try {
                        float tauVal = std::stof(value);
                        setDouble(key, tauVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "synaptogenesis_rate") {
                    try {
                        float rateVal = std::stof(value);
                        setDouble(key, rateVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "pruning_rate") {
                    try {
                        float rateVal = std::stof(value);
                        setDouble(key, rateVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "max_steps" || key == "steps") {
                    try {
                        int stepsVal = std::stoi(value);
                        setInt(key, stepsVal, ConfigSource::CommandLine, "Command line argument");
                        parsed = true;
                    } catch (...) {}
                } else if (key == "bool") {
                    // Handle boolean values
                    bool boolVal = (value == "true" || value == "1" || value == "yes");
                    setBool(key, boolVal, ConfigSource::CommandLine, "Command line argument");
                    parsed = true;
                } else {
                    // Treat as string value
                    setString(key, value, ConfigSource::CommandLine, "Command line argument");
                    parsed = true;
                }
                
                if (parsed) {
                    loaded = true;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Normalize key
            key = normalizeKey(key);
            setString(key, value, ConfigSource::CommandLine, "Command line argument");
            loaded = true;
        }
    }
    
    return loaded;
}

// Save to JSON file
bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j = exportToJSON();
        
        if (pretty) {
            file << j.dump(2);
        } else {
            file << j.dump();
        }
        
        file.close();
        
        if (pImpl) {
            pImpl->totalSaveCount++;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config: " << e.what() << std::endl;
        return false;
    }
}

// Merge with another configuration
void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Create new entry with merged source (other source has higher priority)
        auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (existingIt != pImpl->entries.end()) {
            // Update existing entry but keep higher priority source
            if (source == ConfigSource::CommandLine || source == ConfigSource::File) {
                existingIt->value = entry.value;
                existingIt->source = entry.source; // Preserve original source priority
            }
            // If merging with lower priority, keep existing value
        } else {
            // Add new entry with merged source
            pImpl->entries.push_back(entry);
        }
    }
}

// Get values template implementation
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto normalizedKey = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Get or template implementation
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Set value with description
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    std::string normalizedKey = normalizeKey(key);
    
    // Validate if schema exists
    if (pImpl->validationEnabled) {
        auto schemaIt = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
            [&normalizedKey](const ConfigSchema& s) { return s.key == normalizedKey; });
        
        if (schemaIt != pImpl->schemas.end()) {
            ValidationResult validation = validateValue(*schemaIt, value, normalizedKey);
            if (!validation.valid && pImpl->strictMode) {
                std::cerr << "Configuration validation error for key '" << normalizedKey << "': " << validation.errorMessage << std::endl;
                return;
            }
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        ConfigEntry newEntry;
        newEntry.key = normalizedKey;
        newEntry.value = value;
        newEntry.source = source;
        newEntry.description = description.empty() ? "" : description;
        
        pImpl->entries.push_back(newEntry);
    }
}

// Enhanced set methods
void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::setList(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    // Convert vector<string> to ConfigValue
    ConfigValue listValue;
    try {
        listValue = std::vector<std::string>(value);
        set(key, listValue, source, "");
    } catch (...) {
        // Fallback to string representation
        std::string strValue = "";
        for (size_t i = 0; i < value.size(); ++i) {
            if (i > 0) strValue += ",";
            strValue += value[i];
        }
        set(key, ConfigValue(strValue), source, "");
    }
}

// Other methods
bool Config::has(const std::string& key) const {
    auto normalizedKey = normalizeKey(key);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
}

void Config::remove(const std::string& key) {
    auto normalizedKey = normalizeKey(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; }),
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
    pImpl->schemas.clear();
    pImpl->descriptions.clear();
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

void Config::addDefaultSchema(const std::vector<ConfigSchema>& schemas) {
    pImpl->schemas.insert(pImpl->schemas.end(), schemas.begin(), schemas.end());
}

bool Config::validateSchema() const {
    std::vector<ValidationResult> results = validateAllSchemas();
    bool valid = true;
    
    for (const auto& result : results) {
        if (!result.valid) {
            valid = false;
            if (pImpl->validationEnabled) {
                std::cerr << "Schema validation error for key '" << result.path << "': " << result.errorMessage << std::endl;
            }
        }
    }
    
    return valid;
}

std::vector<ValidationResult> Config::validate() const {
    return validateAllSchemas();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary (" << pImpl->entries.size() << " entries, " << pImpl->schemas.size() << " schemas):" << std::endl;
    oss << std::string(50, '-') << std::endl;
    
    // Show configuration entries
    oss << "Configuration Entries:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << std::left << std::setw(20) << entry.key << ": ";
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
        oss << " (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << std::endl;
    }
    
    // Show schema information
    if (!pImpl->schemas.empty()) {
        oss << std::endl << "Defined Schemas:" << std::endl;
        for (const auto& schema : pImpl->schemas) {
            oss << "  " << std::left << std::setw(20) << schema.key << ": " << schema.type;
            if (schema.required) oss << " (required)";
            if (!schema.description.empty()) oss << " - " << schema.description;
            oss << std::endl;
        }
    }
    
    oss << std::string(50, '-') << std::endl;
    
    return oss.str();
}

std::string Config::toJSON() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Convert ConfigValue to JSON
        std::visit([&j, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[key] = arg;
            }
        }, entry.value);
    }
    
    return j.dump(2);
}

void Config::printValidationErrors() const {
    auto results = validateAllSchemas();
    bool hasErrors = false;
    
    for (const auto& result : results) {
        if (!result.valid) {
            hasErrors = true;
            std::cerr << "Validation error: " << result.errorMessage << std::endl;
            if (!result.path.empty()) {
                std::cerr << "  Path: " << result.path << std::endl;
            }
        }
    }
    
    if (!hasErrors && pImpl->validationEnabled) {
        std::cout << "All configurations are valid!" << std::endl;
    }
}

bool Config::importFromJSON(const json& j, ConfigSource source) {
    bool success = true;
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        json value = *it;
        
        try {
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Determine array type
                if (!value.empty()) {
                    if (value[0].is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : value) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        configValue = intArray;
                    } else if (value[0].is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : value) {
                            doubleArray.push_back(item.get<double>());
                        }
                        configValue = doubleArray;
                    } else if (value[0].is_string()) {
                        std::vector<std::string> stringArray;
                        for (const auto& item : value) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        configValue = stringArray;
                    } else {
                        // Fallback to string representation
                        configValue = value.dump();
                    }
                } else {
                    configValue = std::vector<std::string>();
                }
            } else {
                // Fallback to string representation
                configValue = value.dump();
            }
            
            set(key, configValue, source, "Imported from JSON");
            
        } catch (const std::exception& e) {
            std::cerr << "Error importing JSON value for key '" << key << "': " << e.what() << std::endl;
            success = false;
        }
    }
    
    return success;
}

json Config::exportToJSON() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Convert ConfigValue to JSON
        std::visit([&j, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[key] = arg;
            }
        }, entry.value);
    }
    
    return j;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto normalizedKey = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

std::string Config::getDescription(const std::string& key) const {
    auto normalizedKey = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "";
}

std::string Config::normalizeKey(const std::string& key) {
    std::string result = key;
    
    // Convert to lowercase
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    // Remove leading dashes
    if (result.substr(0, 2) == "--") {
        result = result.substr(2);
    } else if (result.substr(0, 1) == "-") {
        result = result.substr(1);
    }
    
    // Replace hyphens with underscores
    std::replace(result.begin(), result.end(), '-', '_');
    
    // Trim whitespace
    result = trim(result);
    
    return result;
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.find(' ') != std::string::npos) {
        return false;
    }
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

std::string Config::getConfigFilePath(const std::string& filename) {
    // First check for environment variable
    const char* envPath = std::getenv("NLM_CONFIG_PATH");
    if (envPath) {
        std::string configDir = envPath;
        return configDir + "/" + filename;
    }
    
    // Check for user config directory
    const char* home = std::getenv("HOME");
    if (home) {
        std::string configDir = std::string(home) + "/.config/nlm";
        if (std::filesystem::exists(configDir)) {
            return configDir + "/" + filename;
        }
    }
    
    // Default to current directory
    return filename;
}

Config Config::getDefaultConfig() {
    Config config;
    
    // Set default values
    config.setInt("random_seed", 42);
    config.setDouble("simulation_timestep", 0.001);
    config.setInt64_t("neuron_count", 1000);
    config.setInt64_t("region_count", 1);
    config.setDouble("connection_probability", 0.15);
    config.setDouble("stdp_ltp_weight", 0.02);
    config.setDouble("stdp_ltd_weight", 0.015);
    config.setDouble("stdp_tau", 20.0);
    config.setDouble("synaptogenesis_rate", 0.0001);
    config.setDouble("pruning_rate", 0.00001);
    config.setString("mode", "phase6");
    config.setInt("max_steps", 1000);
    
    return config;
}

Config Config::getPhase3Config() {
    Config config = getDefaultConfig();
    
    config.setInt64_t("neuron_count", 500);
    config.setInt64_t("region_count", 2);
    config.setString("mode", "phase3");
    
    return config;
}

Config Config::getPhase4Config() {
    Config config = getDefaultConfig();
    
    config.setInt64_t("neuron_count", 800);
    config.setString("mode", "phase4");
    
    return config;
}

Config Config::getPhase5Config() {
    Config config = getDefaultConfig();
    
    config.setInt64_t("neuron_count", 1500);
    config.setString("mode", "phase5");
    
    return config;
}

Config Config::getPhase6Config() {
    Config config = getDefaultConfig();
    
    config.setInt64_t("neuron_count", 1000);
    config.setString("mode", "phase6");
    
    return config;
}

// ConfigManager implementation
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() {
    initialize();
}

ConfigManager::~ConfigManager() = default;

void ConfigManager::initialize() {
    globalConfig = std::make_unique<Config>();
    
    // Add default schemas
    std::vector<ConfigSchema> defaultSchemas = ConfigPresets::getDefaultSchema();
    globalConfig->addDefaultSchema(defaultSchemas);
}

Config& ConfigManager::getGlobalConfig() {
    return *globalConfig;
}

void ConfigManager::loadFromFile(const std::string& filepath) {
    getGlobalConfig().loadFromFile(filepath);
}

void ConfigManager::loadFromCommandLine(int argc, char** argv) {
    getGlobalConfig().loadFromArgs(argc, argv);
}

void ConfigManager::loadFromEnvironment(const std::string& prefix) {
    getGlobalConfig().loadFromEnvironment(prefix);
}

void ConfigManager::saveToFile(const std::string& filepath, bool pretty) {
    getGlobalConfig().saveToFile(filepath, pretty);
}

std::string ConfigManager::getConfigPath(const std::string& filename) {
    return Config::getConfigFilePath(filename);
}

bool ConfigManager::validateConfig(const Config& config) {
    return config.validateSchema();
}

// ConfigPresets implementation
std::vector<ConfigSchema> ConfigPresets::getPhase3Schema() {
    return {
        ConfigSchema("random_seed", 42, "Random seed for reproducible results"),
        ConfigSchema("simulation_timestep", 0.001, "Simulation timestep in seconds"),
        ConfigSchema("neuron_count", 500, "Number of neurons in the network"),
        ConfigSchema("region_count", 2, "Number of neural regions"),
        ConfigSchema("connection_probability", 0.15, "Probability of synaptic connections"),
        ConfigSchema("mode", std::string("phase3"), "Operating mode"),
        ConfigSchema("max_steps", 2000, "Maximum simulation steps"),
    };
}

std::vector<ConfigSchema> ConfigPresets::getPhase4Schema() {
    return {
        ConfigSchema("random_seed", 42, "Random seed for reproducible results"),
        ConfigSchema("simulation_timestep", 0.001, "Simulation timestep in seconds"),
        ConfigSchema("neuron_count", 800, "Number of neurons in the network"),
        ConfigSchema("region_count", 1, "Number of neural regions"),
        ConfigSchema("connection_probability", 0.15, "Probability of synaptic connections"),
        ConfigSchema("mode", std::string("phase4"), "Operating mode"),
        ConfigSchema("max_steps", 3000, "Maximum simulation steps"),
    };
}

std::vector<ConfigSchema> ConfigPresets::getPhase5Schema() {
    return {
        ConfigSchema("random_seed", 42, "Random seed for reproducible results"),
        ConfigSchema("simulation_timestep", 0.001, "Simulation timestep in seconds"),
        ConfigSchema("neuron_count", 1500, "Number of neurons in the network"),
        ConfigSchema("region_count", 3, "Number of neural regions"),
        ConfigSchema("connection_probability", 0.12, "Probability of synaptic connections"),
        ConfigSchema("mode", std::string("phase5"), "Operating mode"),
        ConfigSchema("max_steps", 5000, "Maximum simulation steps"),
    };
}

std::vector<ConfigSchema> ConfigPresets::getPhase6Schema() {
    return {
        ConfigSchema("random_seed", 42, "Random seed for reproducible results"),
        ConfigSchema("simulation_timestep", 0.001, "Simulation timestep in seconds"),
        ConfigSchema("neuron_count", 1000, "Number of neurons in the network"),
        ConfigSchema("region_count", 1, "Number of neural regions"),
        ConfigSchema("connection_probability", 0.15, "Probability of synaptic connections"),
        ConfigSchema("mode", std::string("phase6"), "Operating mode"),
        ConfigSchema("max_steps", 10000, "Maximum simulation steps"),
    };
}

std::vector<ConfigSchema> ConfigPresets::getDefaultSchema() {
    return {
        ConfigSchema("random_seed", 42, "Random seed for reproducible results"),
        ConfigSchema("simulation_timestep", 0.001, "Simulation timestep in seconds"),
        ConfigSchema("neuron_count", 1000, "Number of neurons in the network"),
        ConfigSchema("region_count", 1, "Number of neural regions"),
        ConfigSchema("connection_probability", 0.15, "Probability of synaptic connections"),
        ConfigSchema("stdp_ltp_weight", 0.02, "STDP LTP weight"),
        ConfigSchema("stdp_ltd_weight", 0.015, "STDP LTD weight"),
        ConfigSchema("stdp_tau", 20.0, "STDP time constant"),
        ConfigSchema("synaptogenesis_rate", 0.0001, "Structural plasticity synaptogenesis rate"),
        ConfigSchema("pruning_rate", 0.00001, "Structural plasticity pruning rate"),
        ConfigSchema("mode", std::string("phase6"), "Operating mode"),
        ConfigSchema("max_steps", 1000, "Maximum simulation steps"),
        ConfigSchema("quiet", false, "Suppress progress output"),
        ConfigSchema("verbose", false, "Enable verbose logging"),
        ConfigSchema("monitor", false, "Enable performance monitoring"),
        ConfigSchema("benchmark", false, "Run performance benchmarks"),
        ConfigSchema("validate_only", false, "Only validate configuration and exit"),
        ConfigSchema("print_config_only", false, "Only print configuration and exit"),
        ConfigSchema("json_output", std::string(""), "Output results to JSON file"),
        ConfigSchema("csv_output", std::string(""), "Output metrics to CSV file"),
        ConfigSchema("checkpoint", std::string(""), "Save checkpoint to file"),
        ConfigSchema("restore", std::string(""), "Restore from checkpoint"),
        ConfigSchema("auto_checkpoint", 0.0, "Auto-save checkpoint every N seconds"),
    };
}

// ConfigUtils implementation
template<typename T>
T ConfigUtils::toValue(const ConfigValue& value) {
    if constexpr (std::is_same_v<T, int>) {
        return std::get<int>(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        return std::get<int64_t>(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::get<double>(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return std::get<bool>(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return std::get<std::string>(value);
    } else {
        throw std::runtime_error("Unsupported type conversion");
    }
}

template<typename T>
bool ConfigUtils::isType(const ConfigValue& value) {
    return std::holds_alternative<T>(value);
}

bool ConfigUtils::isInRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool ConfigUtils::isInRange(int value, int min, int max) {
    return value >= min && value <= max;
}

bool ConfigUtils::isInRange(int64_t value, int64_t min, int64_t max) {
    return value >= min && value <= max;
}

std::string ConfigUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string ConfigUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string ConfigUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool ConfigUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool ConfigUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::vector<std::string> ConfigUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

ValidationResult Config::validateValue(const ConfigSchema& schema, const ConfigValue& value, const std::string& path) const {
    // Check if key exists in schemas
    if (schema.key.empty()) {
        return ValidationResult::error("Empty key in schema", path);
    }
    
    // Check required fields
    if (schema.required && !has(schema.key)) {
        return ValidationResult::error("Required configuration key missing: " + schema.key, path + "." + schema.key);
    }
    
    // Check type compatibility
    if (!validateType(value, schema.type)) {
        return ValidationResult::error("Type mismatch for key '" + schema.key + "'. Expected " + schema.type + ", got " + getTypeName(value), path + "." + schema.key);
    }
    
    // Check allowed values
    if (!schema.allowedValues.empty()) {
        std::string valueStr;
        if (std::holds_alternative<std::string>(value)) {
            valueStr = std::get<std::string>(value);
        } else {
            // Convert to string representation
            std::ostringstream oss;
            std::visit([&oss](auto&& arg) { oss << arg; }, value);
            valueStr = oss.str();
        }
        
        bool allowed = false;
        for (const auto& allowed : schema.allowedValues) {
            if (valueStr == allowed) {
                allowed = true;
                break;
            }
        }
        
        if (!allowed) {
            return ValidationResult::error("Value '" + valueStr + "' for key '" + schema.key + "' is not in allowed list: " + 
                std::accumulate(schema.allowedValues.begin(), schema.allowedValues.end(), std::string(),
                    [](const std::string& acc, const std::string& val) { return acc.empty() ? val : acc + ", " + val; }), 
                path + "." + schema.key);
        }
    }
    
    // Run custom validator if provided
    if (schema.validator) {
        ValidationResult customResult = schema.validator(value);
        if (!customResult.valid) {
            customResult.path = path.empty() ? schema.key : path + "." + schema.key;
            return customResult;
        }
    }
    
    return ValidationResult::success();
}

bool Config::validateType(const ConfigValue& value, const std::string& expectedType) const {
    std::string actualType = getTypeName(value);
    
    // Normalize type names for comparison
    std::string normalizedExpected = toLower(expectedType);
    std::string normalizedActual = toLower(actualType);
    
    if (normalizedExpected == normalizedActual) {
        return true;
    }
    
    // Handle type conversions
    if (normalizedExpected == "double" && (normalizedActual == "int" || normalizedActual == "int64_t")) {
        return true;
    }
    
    if (normalizedExpected == "int" && normalizedActual == "int64_t") {
        return true;
    }
    
    if (normalizedExpected == "string" && (normalizedActual == "int" || normalizedActual == "int64_t" || normalizedActual == "double" || normalizedActual == "bool")) {
        return true;
    }
    
    return false;
}

std::vector<ValidationResult> Config::validateAllSchemas() const {
    std::vector<ValidationResult> results;
    
    for (const auto& schema : pImpl->schemas) {
        std::string path = schema.key;
        
        // Get value if exists
        auto val = get<std::string>(schema.key);  // Get as string first
        if (!val.has_value()) {
            // Try other types
            if (schema.type == "int" || schema.type == "int64_t") {
                if (schema.required) {
                    results.push_back(ValidationResult::error("Required configuration key missing: " + schema.key, path));
                }
            } else {
                if (schema.required) {
                    results.push_back(ValidationResult::error("Required configuration key missing: " + schema.key, path));
                }
            }
            continue;
        }
        
        // Convert back to ConfigValue for validation (simplified)
        // In practice, we would need to properly extract the value based on its actual type
        ConfigValue configValue = ConfigValue(val.value());
        
        ValidationResult result = validateValue(schema, configValue, path);
        results.push_back(result);
        
        if (!result.valid) {
            pImpl->validationErrorCount++;
        }
    }
    
    return results;
}

std::string Config::getTypeName(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "list<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "list<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "list<string>";
    return "unknown";
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

std::string Config::sanitizeJSON(const std::string& input) {
    std::string result = input;
    // Remove control characters except newline and tab
    result.erase(std::remove_if(result.begin(), result.end(),
        [](char c) { return c < 32 && c != '\t' && c != '\n'; }), result.end());
    return result;
}

} // namespace nlm

// Implementation file ends here
// Note: The ConfigEnhanced.cpp file has been created to include the enhanced implementation
// This provides comprehensive configuration management with validation, schema, and multiple input sources

// The enhanced configuration system should be compiled with nlohmann/json library
// Add to CMakeLists.txt: find_package(nlohmann_json REQUIRED)