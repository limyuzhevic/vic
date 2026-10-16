// Enhanced Configuration System with JSON/YAML support, validation, inheritance, and advanced features

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <numeric>
#include <variant>
#include <regex>
#include <iostream>
#include <unordered_map>
#include <deque>
#include <mutex>
#include <thread>
#include <optional>
#include <cmath>
#include <limits>
#include <unordered_set>

namespace nlm {

// Forward declarations for internal classes
class ConfigValidator;
class ConfigHistory;

// Enhanced configuration value types with type information
struct ConfigValueWithType {
    std::variant<
        int,
        int64_t,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>
    > value;
    std::string typeName;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::string unit;
    
    ConfigValueWithType() : value(), typeName(), minValue(), maxValue(), unit() {}
};

// Configuration categories
enum class ConfigCategory {
    Core,
    Neural,
    Plasticity,
    Memory,
    Development,
    Neuromodulation,
    Reward,
    Environment,
    Simulation,
    Visualization,
    Logging,
    Performance,
    Custom
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    std::string author;
    std::chrono::system_clock::time_point created;
    
    ConfigPreset(const std::string& n, const std::string& desc, const std::string& author = "system")
        : name(n), description(desc), author(author), created(std::chrono::system_clock::now()) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    ConfigCategory category;
    double relevanceScore;
    
    ConfigTemplate() : category(ConfigCategory::Custom), relevanceScore(0.0) {}
    ConfigTemplate(const std::string& n, const std::string& desc)
        : name(n), description(desc), category(ConfigCategory::Custom), relevanceScore(0.0) {}
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, ConfigValue> values;
    std::string action;
    std::string user;
    std::string reason;
    
    ConfigHistoryEntry(const std::string& act, const std::string& usr = "")
        : timestamp(std::chrono::system_clock::now()), action(act), user(usr), reason() {}
};

// Configuration validator implementation
class ConfigValidator {
public:
    bool validate(const ConfigEntry& entry) const {
        const auto& valueWithType = entry.sources.at(entry.key).type;
        return validateValue(entry.value, valueWithType);
    }
    
    bool validateValue(const ConfigValue& value, const ConfigValueWithType& expected) const {
        if (expected.typeName.empty()) {
            return true; // No type expected, skip validation
        }
        
        try {
            if (expected.typeName == "int" && std::holds_alternative<int>(value)) {
                return validateRange(std::get<int>(value), expected);
            } else if (expected.typeName == "int64_t" && std::holds_alternative<int64_t>(value)) {
                return validateRange(std::get<int64_t>(value), expected);
            } else if (expected.typeName == "double" && std::holds_alternative<double>(value)) {
                return validateRange(std::get<double>(value), expected);
            } else if (expected.typeName == "bool" && std::holds_alternative<bool>(value)) {
                return true;
            } else if (expected.typeName == "string" && std::holds_alternative<std::string>(value)) {
                return validateStringRange(std::get<std::string>(value), expected);
            } else if (expected.typeName == "vector<int>" && std::holds_alternative<std::vector<int>>(value)) {
                return validateVectorRange(std::get<std::vector<int>>(value), expected);
            } else if (expected.typeName == "vector<double>" && std::holds_alternative<std::vector<double>>(value)) {
                return validateVectorRange(std::get<std::vector<double>>(value), expected);
            } else if (expected.typeName == "vector<string>" && std::holds_alternative<std::vector<std::string>>(value)) {
                return validateVectorRange(std::get<std::vector<std::string>>(value), expected);
            }
        } catch (const std::bad_variant_access&) {
            return false;
        }
        return false;
    }
    
private:
    template<typename T>
    bool validateRange(const T& value, const ConfigValueWithType& expected) const {
        if (expected.minValue.has_value() && static_cast<double>(value) < expected.minValue.value()) {
            return false;
        }
        if (expected.maxValue.has_value() && static_cast<double>(value) > expected.maxValue.value()) {
            return false;
        }
        return true;
    }
    
    bool validateStringRange(const std::string& value, const ConfigValueWithType& expected) const {
        if (expected.minValue.has_value() && value.length() < expected.minValue.value()) {
            return false;
        }
        if (expected.maxValue.has_value() && value.length() > expected.maxValue.value()) {
            return false;
        }
        return true;
    }
    
    template<typename T>
    bool validateVectorRange(const std::vector<T>& value, const ConfigValueWithType& expected) const {
        for (const auto& item : value) {
            if (expected.minValue.has_value() && static_cast<double>(item) < expected.minValue.value()) {
                return false;
            }
            if (expected.maxValue.has_value() && static_cast<double>(item) > expected.maxValue.value()) {
                return false;
            }
        }
        return true;
    }
};

// Configuration history implementation
class ConfigHistory {
public:
    void addEntry(const ConfigHistoryEntry& entry) {
        history.push_back(entry);
        if (history.size() > maxHistorySize) {
            history.erase(history.begin());
        }
    }
    
    std::vector<ConfigHistoryEntry> getHistory(size_t limit = 0) const {
        if (limit == 0) {
            return history;
        }
        size_t start = history.size() > limit ? history.size() - limit : 0;
        return std::vector<ConfigHistoryEntry>(history.begin() + start, history.end());
    }
    
    std::vector<std::string> getActions(const std::string& key) const {
        std::vector<std::string> actions;
        for (const auto& entry : history) {
            if (entry.values.find(key) != entry.values.end()) {
                actions.push_back(entry.action + " by " + entry.user + " (" + entry.reason + ")");
            }
        }
        return actions;
    }
    
    std::string getChangeSummary() const {
        std::ostringstream oss;
        oss << "Configuration History (" << history.size() << " entries):\n";
        for (size_t i = 0; i < history.size(); ++i) {
            const auto& entry = history[i];
            oss << "  " << i << ". " << entry.timestamp.time_since_epoch().count() << 
                " - " << entry.action << " by " << entry.user << ": " << entry.reason << "\n";
        }
        return oss.str();
    }
    
private:
    static const size_t maxHistorySize = 1000;
    std::vector<ConfigHistoryEntry> history;
};

// Enhanced configuration class implementation
struct Config::Impl {
    // Core configuration data
    std::map<std::string, ConfigEntry> entries;
    std::map<std::string, ConfigCategory> entryCategories;
    std::map<std::string, ConfigValueWithType> entrySchemas;
    
    // Configuration management
    std::vector<std::string> baseConfigs;
    std::map<std::string, std::string> overrides;
    std::vector<ConfigTemplate> templates;
    std::map<std::string, ConfigPreset> presets;
    std::vector<ConfigHistoryEntry> history;
    
    // File watching and hot-reload
    std::map<std::string, std::chrono::system_clock::time_point> fileWatchers;
    bool hotReloadEnabled;
    std::chrono::milliseconds hotReloadInterval;
    mutable std::mutex reloadMutex;
    
    // Validation and parsing
    std::unique_ptr<ConfigValidator> validator;
    
    // Suggestions and help
    std::map<std::string, std::vector<std::string>> suggestions;
    std::map<std::string, std::string> documentation;
    
    // Performance and caching
    mutable bool cacheDirty;
    mutable std::unordered_map<std::string, std::optional<ConfigValue>> valueCache;
    
    Impl() 
        : hotReloadEnabled(false), hotReloadInterval(std::chrono::milliseconds(100)),
          cacheDirty(true), validator(std::make_unique<ConfigValidator>()) {}
};

// Constructor
Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeDefaultTemplates();
    initializeSuggestions();
    initializeDocumentation();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

// Helper functions
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

// Initialize default configuration templates
void Config::initializeDefaultTemplates() {
    // Create templates for common configurations
    ConfigTemplate basicConfig("Basic", "Basic neural network configuration for testing");
    basicConfig.values = {
        {"neuron_count", 100},
        {"connection_probability", 0.2},
        {"random_seed", 42},
        {"simulation_timestep", 0.001}
    };
    basicConfig.category = ConfigCategory::Core;
    basicConfig.relevanceScore = 0.9;
    pImpl->templates.push_back(basicConfig);
    
    ConfigTemplate highPerformanceConfig("High Performance", "Optimized configuration for high-performance neural simulations");
    highPerformanceConfig.values = {
        {"neuron_count", 5000},
        {"connection_probability", 0.15},
        {"plasticity_learning_rate", 0.005},
        {"simulation_timestep", 0.0005},
        {"max_simulation_steps", 100000}
    };
    highPerformanceConfig.category = ConfigCategory::Performance;
    highPerformanceConfig.relevanceScore = 0.8;
    pImpl->templates.push_back(highPerformanceConfig);
    
    ConfigTemplate plasticityFocusedConfig("Plasticity Focus", "Configuration optimized for learning and plasticity experiments");
    plasticityFocusedConfig.values = {
        {"plasticity_learning_rate", 0.1},
        {"stdp_ltp_weight", 0.05},
        {"stdp_ltd_weight", 0.055},
        {"development_synaptogenesis_rate", 0.01},
        {"development_pruning_rate", 0.001}
    };
    plasticityFocusedConfig.category = ConfigCategory::Plasticity;
    plasticityFocusedConfig.relevanceScore = 0.7;
    pImpl->templates.push_back(plasticityFocusedConfig);
}

// Initialize configuration suggestions
void Config::initializeSuggestions() {
    pImpl->suggestions = {
        {"neuron_count", {"Choose based on experiment complexity", "1000-10000 for most tests"}},
        {"connection_probability", {"0.1-0.3 for sparse, 0.5+ for dense networks"}},
        {"random_seed", {"Use deterministic seeds for reproducible experiments"}},
        {"plasticity_learning_rate", {"0.001-0.1 based on learning speed requirements"}},
        {"max_simulation_steps", {"Balance simulation duration with performance"}}
    };
}

// Initialize configuration documentation
void Config::initializeDocumentation() {
    pImpl->documentation = {
        {"neuron_count", "Number of neurons in the brain (default: 1000)"},
        {"connection_probability", "Probability of forming random connections between neurons (0.0-1.0)"},
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Time step for simulation updates in seconds"},
        {"plasticity_learning_rate", "Learning rate for plasticity mechanisms"},
        {"max_simulation_steps", "Maximum number of simulation steps to execute"},
        {"region_count", "Number of brain regions (default: 2)"},
        {"environment_width", "Width of the simulated environment"},
        {"environment_height", "Height of the simulated environment"},
        {"log_level", "Logging level: debug, info, warning, error, critical"},
        {"visualization_enabled", "Enable/disable visualization output"}
    };
}

// Load from simple key=value file (backward compatibility)
bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Warning: Invalid line " << lineNumber << ": missing '='" << std::endl;
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
        
        // Parse value
        ConfigValue configValue;
        ConfigValueWithType valueWithType;
        
        // Try to parse boolean
        if (value == "true" || value == "false") {
            bool boolVal = (value == "true");
            configValue = boolVal;
            valueWithType = ConfigValueWithType(boolVal, "bool");
        }
        // Try to parse integer
        else if (std::regex_match(value, std::regex("^-?\d+$"))) {
            int intVal = std::stoi(value);
            configValue = intVal;
            valueWithType = ConfigValueWithType(intVal, "int", -std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
        }
        // Try to parse double
        else if (std::regex_match(value, std::regex("^-?\d+\.\d*$") || std::regex_match(value, std::regex("^-?\d*\.\d+$")))) {
            double doubleVal = std::stod(value);
            configValue = doubleVal;
            valueWithType = ConfigValueWithType(doubleVal, "double", -std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
        }
        // Handle vector types
        else if (value.find('[') != std::string::npos && value.find(']') != std::string::npos) {
            // Simple vector parsing: [1,2,3] or [a,b,c]
            std::string inner = value.substr(1, value.size() - 2);
            std::vector<std::string> tokens;
            std::stringstream ss(inner);
            std::string token;
            while (std::getline(ss, token, ',')) {
                token = trim(token);
                tokens.push_back(token);
            }
            
            if (!tokens.empty()) {
                // Try to determine vector type
                bool allInts = true;
                bool allDoubles = true;
                bool allStrings = true;
                
                for (const auto& token : tokens) {
                    if (!std::regex_match(token, std::regex("^-?\d+$"))) allInts = false;
                    if (!std::regex_match(token, std::regex("^-?\d+\.\d*$") || std::regex_match(token, std::regex("^-?\d*\.\d+$")))) allDoubles = false;
                    allStrings = false; // Assume strings if not numeric
                }
                
                if (allInts) {
                    std::vector<int> intVec;
                    for (const auto& token : tokens) {
                        intVec.push_back(std::stoi(token));
                    }
                    configValue = intVec;
                    valueWithType = ConfigValueWithType(intVec, "vector<int>");
                } else if (allDoubles) {
                    std::vector<double> doubleVec;
                    for (const auto& token : tokens) {
                        doubleVec.push_back(std::stod(token));
                    }
                    configValue = doubleVec;
                    valueWithType = ConfigValueWithType(doubleVec, "vector<double>");
                } else {
                    configValue = tokens;
                    valueWithType = ConfigValueWithType(tokens, "vector<string>");
                }
            }
        }
        // Default to string
        else {
            configValue = value;
            valueWithType = ConfigValueWithType(value, "string");
        }
        
        // Validate and set
        if (pImpl->validator->validateValue(configValue, valueWithType)) {
            set(key, configValue, ConfigSource::File, ConfigCategory::Custom, "Loaded from simple config");
            pImpl->entrySchemas[key] = valueWithType;
        } else {
            std::cerr << "Warning: Invalid value for key " << key << " in simple config file" << std::endl;
        }
    }
    
    return true;
}

// Load from command line arguments (enhanced)
bool Config::loadFromArgs(int argc, char** argv) {
    bool loaded = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.find('=') != std::string::npos && arg.find('=') < 3) { // --key=value format
            size_t pos = arg.find('=');
            if (pos > 2) { // Ensure it's --key=value
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse value
                ConfigValue configValue;
                ConfigValueWithType valueWithType;
                
                // Try to parse boolean
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    configValue = boolVal;
                    valueWithType = ConfigValueWithType(boolVal, "bool");
                }
                // Try to parse integer
                else if (std::regex_match(value, std::regex("^-?\d+$"))) {
                    int intVal = std::stoi(value);
                    configValue = intVal;
                    valueWithType = ConfigValueWithType(intVal, "int");
                }
                // Try to parse double
                else if (std::regex_match(value, std::regex("^-?\d+\.\d*$") || std::regex_match(value, std::regex("^-?\d*\.\d+$")))) {
                    double doubleVal = std::stod(value);
                    configValue = doubleVal;
                    valueWithType = ConfigValueWithType(doubleVal, "double");
                }
                else {
                    configValue = value;
                    valueWithType = ConfigValueWithType(value, "string");
                }
                
                // Validate and set
                if (pImpl->validator->validateValue(configValue, valueWithType)) {
                    set(key, configValue, ConfigSource::CommandLine, ConfigCategory::Custom, "From command line");
                    pImpl->entrySchemas[key] = valueWithType;
                    loaded = true;
                } else {
                    std::cerr << "Error: Invalid value for key " << key << ": " << value << std::endl;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Parse value
            ConfigValue configValue;
            ConfigValueWithType valueWithType;
            
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                configValue = boolVal;
                valueWithType = ConfigValueWithType(boolVal, "bool");
            } else if (std::regex_match(value, std::regex("^-?\d+$"))) {
                int intVal = std::stoi(value);
                configValue = intVal;
                valueWithType = ConfigValueWithType(intVal, "int");
            } else if (std::regex_match(value, std::regex("^-?\d+\.\d*$") || std::regex_match(value, std::regex("^-?\d*\.\d+$")))) {
                double doubleVal = std::stod(value);
                configValue = doubleVal;
                valueWithType = ConfigValueWithType(doubleVal, "double");
            } else {
                configValue = value;
                valueWithType = ConfigValueWithType(value, "string");
            }
            
            // Validate and set
            if (pImpl->validator->validateValue(configValue, valueWithType)) {
                set(key, configValue, ConfigSource::CommandLine, ConfigCategory::Custom, "From command line");
                pImpl->entrySchemas[key] = valueWithType;
                loaded = true;
            } else {
                std::cerr << "Error: Invalid value for key " << key << ": " << value << std::endl;
            }
        }
    }
    
    return loaded;
}

// Load from environment variables
bool Config::loadFromEnvironment() {
    bool loaded = false;
    
    // Common environment variables
    const char* envVars[] = {
        "NLM_RANDOM_SEED",
        "NLM_NEURON_COUNT", 
        "NLM_CONNECTION_PROBABILITY",
        "NLM_SIMULATION_STEPS",
        "NLM_LOG_LEVEL",
        "NLM_ENVIRONMENT_NAME"
    };
    
    for (const char* envVar : envVars) {
        const char* value = std::getenv(envVar);
        if (value != nullptr) {
            std::string key = toLower(envVar);
            // Remove NLM_ prefix
            if (key.find("nlm_") == 0) {
                key = key.substr(4);
            }
            
            // Parse value
            ConfigValue configValue;
            ConfigValueWithType valueWithType;
            
            if (std::string(value) == "true" || std::string(value) == "false") {
                bool boolVal = (std::string(value) == "true");
                configValue = boolVal;
                valueWithType = ConfigValueWithType(boolVal, "bool");
            } else if (std::regex_match(std::string(value), std::regex("^-?\d+$"))) {
                int intVal = std::stoi(std::string(value));
                configValue = intVal;
                valueWithType = ConfigValueWithType(intVal, "int");
            } else if (std::regex_match(std::string(value), std::regex("^-?\d+\.\d*$") || std::regex_match(std::string(value), std::regex("^-?\d*\.\d+$")))) {
                double doubleVal = std::stod(std::string(value));
                configValue = doubleVal;
                valueWithType = ConfigValueWithType(doubleVal, "double");
            } else {
                configValue = std::string(value);
                valueWithType = ConfigValueWithType(std::string(value), "string");
            }
            
            // Validate and set
            if (pImpl->validator->validateValue(configValue, valueWithType)) {
                set(key, configValue, ConfigSource::Environment, ConfigCategory::Custom, "From environment");
                pImpl->entrySchemas[key] = valueWithType;
                loaded = true;
            }
        }
    }
    
    return loaded;
}

// Set value with category
void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, ConfigCategory category, 
                 const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->reloadMutex);
    
    // Validate the value using schema if available
    if (pImpl->entrySchemas.find(key) != pImpl->entrySchemas.end()) {
        ConfigValueWithType expected = pImpl->entrySchemas[key];
        if (!pImpl->validator->validateValue(value, expected)) {
            std::cerr << "Validation error for key " << key << ": Invalid value" << std::endl;
            return;
        }
    }
    
    // Update cache
    pImpl->valueCache.erase(key);
    pImpl->cacheDirty = true;
    
    // Set the value
    pImpl->entries[key] = ConfigEntry(key, value, source);
    pImpl->entryCategories[key] = category;
    
    // Record schema
    ConfigValueWithType valueWithType;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            valueWithType = ConfigValueWithType(arg, "int");
        } else if constexpr (std::is_same_v<T, int64_t>) {
            valueWithType = ConfigValueWithType(arg, "int64_t");
        } else if constexpr (std::is_same_v<T, double>) {
            valueWithType = ConfigValueWithType(arg, "double");
        } else if constexpr (std::is_same_v<T, bool>) {
            valueWithType = ConfigValueWithType(arg, "bool");
        } else if constexpr (std::is_same_v<T, std::string>) {
            valueWithType = ConfigValueWithType(arg, "string");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            valueWithType = ConfigValueWithType(arg, "vector<int>");
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            valueWithType = ConfigValueWithType(arg, "vector<double>");
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            valueWithType = ConfigValueWithType(arg, "vector<string>");
        }
    }, value);
    
    pImpl->entrySchemas[key] = valueWithType;
}

// Convenience overloads for backward compatibility
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source, ConfigCategory::Custom, "");
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

// Get value with type checking
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Check cache first
    auto cacheIt = pImpl->valueCache.find(key);
    if (cacheIt != pImpl->valueCache.end()) {
        return cacheIt->second;
    }
    
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T result = std::get<T>(it->second.value);
        pImpl->valueCache[key] = result;
        return result;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

// Get or default
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Check if key exists
bool Config::has(const std::string& key) const {
    return pImpl->entries.find(key) != pImpl->entries.end();
}

// Remove key
void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->reloadMutex);
    
    if (pImpl->entries.find(key) != pImpl->entries.end()) {
        pImpl->entries.erase(key);
        pImpl->entryCategories.erase(key);
        pImpl->entrySchemas.erase(key);
        
        pImpl->valueCache.erase(key);
        pImpl->cacheDirty = true;
    }
}

// Get all keys
std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

// Get keys by category
std::vector<std::string> Config::getKeysByCategory(ConfigCategory category) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->entries) {
        if (pImpl->entryCategories.find(pair.first) != pImpl->entryCategories.end() &&
            pImpl->entryCategories.at(pair.first) == category) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

// Get category for key
ConfigCategory Config::getCategory(const std::string& key) const {
    auto it = pImpl->entryCategories.find(key);
    if (it != pImpl->entryCategories.end()) {
        return it->second;
    }
    return ConfigCategory::Custom;
}

// Get categories
std::vector<ConfigCategory> Config::getCategories() const {
    std::vector<ConfigCategory> categories;
    std::set<ConfigCategory> uniqueCategories;
    
    for (const auto& pair : pImpl->entryCategories) {
        uniqueCategories.insert(pair.second);
    }
    
    categories.assign(uniqueCategories.begin(), uniqueCategories.end());
    return categories;
}

// Clear all configuration
void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->reloadMutex);
    
    pImpl->entries.clear();
    pImpl->entryCategories.clear();
    pImpl->entrySchemas.clear();
    
    pImpl->valueCache.clear();
    pImpl->cacheDirty = true;
}

// Get configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary\n";
    oss << "===============================\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    
    // Group by category
    std::map<ConfigCategory, std::vector<std::pair<std::string, std::string>>> categories;
    for (const auto& entry : pImpl->entries) {
        ConfigCategory category = getCategory(entry.first);
        std::string valueStr;
        
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.second.value);
        
        categories[category].push_back({entry.first, valueStr});
    }
    
    // Print categories
    oss << "\nCategories:\n";
    for (const auto& categoryPair : categories) {
        oss << "  " << categoryToString(categoryPair.first) << " (" << categoryPair.second.size() << "):\n";
        for (const auto& keyValue : categoryPair.second) {
            oss << "    " << keyValue.first << " = " << keyValue.second << "\n";
        }
    }
    
    return oss.str();
}

// Convert category to string
std::string Config::categoryToString(ConfigCategory category) {
    switch (category) {
        case ConfigCategory::Core: return "Core";
        case ConfigCategory::Neural: return "Neural";
        case ConfigCategory::Plasticity: return "Plasticity";
        case ConfigCategory::Memory: return "Memory";
        case ConfigCategory::Development: return "Development";
        case ConfigCategory::Neuromodulation: return "Neuromodulation";
        case ConfigCategory::Reward: return "Reward";
        case ConfigCategory::Environment: return "Environment";
        case ConfigCategory::Simulation: return "Simulation";
        case ConfigCategory::Visualization: return "Visualization";
        case ConfigCategory::Logging: return "Logging";
        case ConfigCategory::Performance: return "Performance";
        case ConfigCategory::Custom: return "Custom";
        default: return "Unknown";
    }
}

// Initialize default templates, suggestions, and documentation
void Config::initializeDefaultTemplates() {
    // Create templates for common configurations
    ConfigTemplate basicConfig("Basic", "Basic neural network configuration for testing");
    basicConfig.values = {
        {"neuron_count", 100},
        {"connection_probability", 0.2},
        {"random_seed", 42},
        {"simulation_timestep", 0.001}
    };
    basicConfig.category = ConfigCategory::Core;
    basicConfig.relevanceScore = 0.9;
    pImpl->templates.push_back(basicConfig);
    
    ConfigTemplate highPerformanceConfig("High Performance", "Optimized configuration for high-performance neural simulations");
    highPerformanceConfig.values = {
        {"neuron_count", 5000},
        {"connection_probability", 0.15},
        {"plasticity_learning_rate", 0.005},
        {"simulation_timestep", 0.0005},
        {"max_simulation_steps", 100000}
    };
    highPerformanceConfig.category = ConfigCategory::Performance;
    highPerformanceConfig.relevanceScore = 0.8;
    pImpl->templates.push_back(highPerformanceConfig);
    
    ConfigTemplate plasticityFocusedConfig("Plasticity Focus", "Configuration optimized for learning and plasticity experiments");
    plasticityFocusedConfig.values = {
        {"plasticity_learning_rate", 0.1},
        {"stdp_ltp_weight", 0.05},
        {"stdp_ltd_weight", 0.055},
        {"development_synaptogenesis_rate", 0.01},
        {"development_pruning_rate", 0.001}
    };
    plasticityFocusedConfig.category = ConfigCategory::Plasticity;
    plasticityFocusedConfig.relevanceScore = 0.7;
    pImpl->templates.push_back(plasticityFocusedConfig);
}

// Initialize configuration suggestions
void Config::initializeSuggestions() {
    pImpl->suggestions = {
        {"neuron_count", {"Choose based on experiment complexity", "1000-10000 for most tests"}},
        {"connection_probability", {"0.1-0.3 for sparse, 0.5+ for dense networks"}},
        {"random_seed", {"Use deterministic seeds for reproducible experiments"}},
        {"plasticity_learning_rate", {"0.001-0.1 based on learning speed requirements"}},
        {"max_simulation_steps", {"Balance simulation duration with performance"}}
    };
}

// Initialize configuration documentation
void Config::initializeDocumentation() {
    pImpl->documentation = {
        {"neuron_count", "Number of neurons in the brain (default: 1000)"},
        {"connection_probability", "Probability of forming random connections between neurons (0.0-1.0)"},
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Time step for simulation updates in seconds"},
        {"plasticity_learning_rate", "Learning rate for plasticity mechanisms"},
        {"max_simulation_steps", "Maximum number of simulation steps to execute"},
        {"region_count", "Number of brain regions (default: 2)"},
        {"environment_width", "Width of the simulated environment"},
        {"environment_height", "Height of the simulated environment"},
        {"log_level", "Logging level: debug, info, warning, error, critical"},
        {"visualization_enabled", "Enable/disable visualization output"}
    };
}

// Save to file
bool Config::saveToFile(const std::string& filepath) const {
    return saveToJSONFile(filepath, true);
}

// Export to file
bool Config::exportToFile(const std::string& filepath, bool includeHistory) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration Export\n";
    file << "# Exported at: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.second.description << "\n";
        file << entry.first << " = ";
        
        // Export value
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.second.value);
        
        file << "\n\n";
    }
    
    if (includeHistory) {
        file << "\n# Configuration History\n";
        file << "# (History not implemented in enhanced version)\n";
    }
    
    return true;
}

// Import from file
bool Config::importFromFile(const std::string& filepath) {
    clear();
    
    if (!loadFromFile(filepath)) {
        return false;
    }
    
    return true;
}

// Get history
std::string Config::getHistory(size_t limit) const {
    std::ostringstream oss;
    oss << "Configuration History (showing " << limit << " entries):\n";
    oss << "(History not implemented in enhanced version)\n";
    return oss.str();
}

// Get history actions
std::string Config::getHistoryActions(const std::string& key) const {
    std::ostringstream oss;
    oss << "History for key \"" << key << "\":\n";
    oss << "(History not implemented in enhanced version)\n";
    return oss.str();
}

// Enable hot-reload
void Config::enableHotReload(bool enabled, std::chrono::milliseconds interval) {
    pImpl->hotReloadEnabled = enabled;
    pImpl->hotReloadInterval = interval;
}

// Get help
std::string Config::getHelp() const {
    std::ostringstream oss;
    oss << "NLM Configuration Help\n";
    oss << "======================\n\n";
    
    oss << "Available Configuration Categories:\n";
    std::vector<ConfigCategory> categories = getCategories();
    for (ConfigCategory category : categories) {
        oss << "  " << categoryToString(category) << "\n";
    }
    
    oss << "\n\nConfiguration Templates:\n";
    for (const auto& template : pImpl->templates) {
        oss << "  " << template.name << ": " << template.description << "\n";
        oss << "    Relevance: " << (template.relevanceScore * 100) << "%\n";
    }
    
    oss << "\n\nAvailable Presets:\n";
    for (const auto& preset : pImpl->presets) {
        oss << "  " << preset.first << ": " << preset.second.description << "\n";
    }
    
    oss << "\n\nConfiguration Suggestions:\n";
    for (const auto& suggestion : pImpl->suggestions) {
        oss << "  " << suggestion.first << ":\n";
        for (const auto& tip : suggestion.second) {
            oss << "    - " << tip << "\n";
        }
    }
    
    return oss.str();
}

// Get statistics
std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics\n";
    oss << "======================\n";
    
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Categories: " << getCategories().size() << "\n";
    oss << "Templates: " << pImpl->templates.size() << "\n";
    oss << "Presets: " << pImpl->presets.size() << "\n";
    oss << "Hot-reload: " << (pImpl->hotReloadEnabled ? "enabled" : "disabled") << "\n";
    
    return oss.str();
}

// Get usage statistics
std::string Config::getUsageStatistics() const {
    std::ostringstream oss;
    oss << "Usage Statistics\n";
    oss << "==============\n\n";
    
    // Count keys by category
    std::map<ConfigCategory, int> categoryCounts;
    for (const auto& pair : pImpl->entryCategories) {
        categoryCounts[pair.second]++;
    }
    
    oss << "Configuration by category:\n";
    for (const auto& pair : categoryCounts) {
        oss << "  " << categoryToString(pair.first) << ": " << pair.second << " keys\n";
    }
    
    return oss.str();
}

// Validate configuration
bool Config::validateConfiguration() const {
    for (const auto& entry : pImpl->entries) {
        if (!pImpl->validator->validate(entry.second)) {
            std::cerr << "Validation error for key " << entry.first << std::endl;
            return false;
        }
    }
    return true;
}

// Fix configuration
bool Config::fixConfiguration() {
    bool fixed = false;
    
    for (auto& pair : pImpl->entries) {
        const auto& valueWithType = pImpl->entrySchemas.find(pair.first);
        if (valueWithType != pImpl->entrySchemas.end()) {
            ConfigValue converted = tryConvertValue(pair.second.value, valueWithType->second.typeName);
            if (converted != pair.second.value) {
                set(pair.first, converted, pair.second.source);
                fixed = true;
            }
        }
    }
    
    return fixed;
}

// Try to convert value
ConfigValue Config::tryConvertValue(const ConfigValue& value, const std::string& targetType) const {
    try {
        if (targetType == "int" && std::holds_alternative<double>(value)) {
            double d = std::get<double>(value);
            return static_cast<int>(d);
        } else if (targetType == "double" && std::holds_alternative<int>(value)) {
            int i = std::get<int>(value);
            return static_cast<double>(i);
        } else if (targetType == "string" && std::holds_alternative<bool>(value)) {
            bool b = std::get<bool>(value);
            return b ? "true" : "false";
        } else if (targetType == "string" && std::holds_alternative<int>(value)) {
            int i = std::get<int>(value);
            return std::to_string(i);
        } else if (targetType == "string" && std::holds_alternative<double>(value)) {
            double d = std::get<double>(value);
            return std::to_string(d);
        }
    } catch (const std::bad_variant_access&) {
    }
    
    return value;
}

// Get recommendations
std::vector<std::string> Config::getRecommendations() const {
    std::vector<std::string> recommendations;
    
    if (has("neuron_count") && get<int>("neuron_count").value_or(0) > 10000) {
        recommendations.push_back("Large neuron count may impact performance. Consider reducing to 10000 or less for better performance.");
    }
    
    if (has("connection_probability") && get<double>("connection_probability").value_or(0.0) > 0.5) {
        recommendations.push_back("High connection probability may lead to overfitting. Consider reducing to 0.3 or less.");
    }
    
    if (has("plasticity_learning_rate") && get<double>("plasticity_learning_rate").value_or(0.0) > 0.1) {
        recommendations.push_back("High plasticity learning rate may cause instability. Consider reducing to 0.05 or less.");
    }
    
    return recommendations;
}

// Export value to stream
void Config::exportValue(std::ostream& os, const ConfigValue& value) const {
    std::visit([&os](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            os << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            os << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            os << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            os << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            os << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            os << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) os << ", ";
                os << arg[i];
            }
            os << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            os << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) os << ", ";
                os << std::setprecision(6) << arg[i];
            }
            os << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            os << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) os << ", ";
                os << "\"" << arg[i] << "\"";
            }
            os << "]";
        }
    }, value);
}

// Escape string
std::string Config::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2);
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
        }
    }
    
    return result;
}

// Parse string value
ConfigValue Config::parseValue(const std::string& str) {
    if (str == "true" || str == "false") {
        return str == "true";
    }
    
    if (std::regex_match(str, std::regex("^-?\d+$"))) {
        try {
            return std::stoi(str);
        } catch (...) {
            return str;
        }
    }
    
    if (std::regex_match(str, std::regex("^-?\d+\.\d*$") || std::regex_match(str, std::regex("^-?\d*\.\d+$")))) {
        try {
            return std::stod(str);
        } catch (...) {
            return str;
        }
    }
    
    if (str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<std::string> tokens;
        std::stringstream ss(inner);
        std::string token;
        while (std::getline(ss, token, ',')) {
            token = trim(token);
            tokens.push_back(token);
        }
        
        if (!tokens.empty()) {
            bool allInts = true;
            bool allDoubles = true;
            
            for (const auto& token : tokens) {
                if (!std::regex_match(token, std::regex("^-?\d+$"))) allInts = false;
                if (!std::regex_match(token, std::regex("^-?\d+\.\d*$") || std::regex_match(token, std::regex("^-?\d*\.\d+$")))) allDoubles = false;
            }
            
            if (allInts) {
                std::vector<int> intVec;
                for (const auto& token : tokens) {
                    intVec.push_back(std::stoi(token));
                }
                return intVec;
            } else if (allDoubles) {
                std::vector<double> doubleVec;
                for (const auto& token : tokens) {
                    doubleVec.push_back(std::stod(token));
                }
                return doubleVec;
            } else {
                return tokens;
            }
        }
    }
    
    return str;
}

// Get current timestamp
std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

// Get timestamp string
std::string Config::getTimestampString(std::chrono::system_clock::time_point timestamp) {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

// Category to string
std::string Config::categoryToString(ConfigCategory category) {
    switch (category) {
        case ConfigCategory::Core: return "Core";
        case ConfigCategory::Neural: return "Neural";
        case ConfigCategory::Plasticity: return "Plasticity";
        case ConfigCategory::Memory: return "Memory";
        case ConfigCategory::Development: return "Development";
        case ConfigCategory::Neuromodulation: return "Neuromodulation";
        case ConfigCategory::Reward: return "Reward";
        case ConfigCategory::Environment: return "Environment";
        case ConfigCategory::Simulation: return "Simulation";
        case ConfigCategory::Visualization: return "Visualization";
        case ConfigCategory::Logging: return "Logging";
        case ConfigCategory::Performance: return "Performance";
        case ConfigCategory::Custom: return "Custom";
        default: return "Unknown";
    }
}

// JSON file loading (simplified implementation)
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            ConfigValue configValue;
            ConfigValueWithType valueWithType;
            
            if (it->is_number_integer()) {
                int val = it->get<int>();
                configValue = val;
                valueWithType = ConfigValueWithType(val, "int");
            } else if (it->is_number_unsigned()) {
                uint64_t val = it->get<uint64_t>();
                configValue = static_cast<int64_t>(val);
                valueWithType = ConfigValueWithType(static_cast<int64_t>(val), "int64_t");
            } else if (it->is_number_float()) {
                double val = it->get<double>();
                configValue = val;
                valueWithType = ConfigValueWithType(val, "double");
            } else if (it->is_boolean()) {
                bool val = it->get<bool>();
                configValue = val;
                valueWithType = ConfigValueWithType(val, "bool");
            } else if (it->is_string()) {
                std::string val = it->get<std::string>();
                configValue = val;
                valueWithType = ConfigValueWithType(val, "string");
            } else if (it->is_array()) {
                const auto& arr = *it;
                if (!arr.empty()) {
                    if (arr[0].is_number_integer()) {
                        std::vector<int> val;
                        for (const auto& item : arr) {
                            val.push_back(item.get<int>());
                        }
                        configValue = val;
                        valueWithType = ConfigValueWithType(val, "vector<int>");
                    } else if (arr[0].is_number_float()) {
                        std::vector<double> val;
                        for (const auto& item : arr) {
                            val.push_back(item.get<double>());
                        }
                        configValue = val;
                        valueWithType = ConfigValueWithType(val, "vector<double>");
                    } else if (arr[0].is_string()) {
                        std::vector<std::string> val;
                        for (const auto& item : arr) {
                            val.push_back(item.get<std::string>());
                        }
                        configValue = val;
                        valueWithType = ConfigValueWithType(val, "vector<string>");
                    }
                }
            }
            
            if (pImpl->validator->validateValue(configValue, valueWithType)) {
                set(key, configValue, ConfigSource::File, ConfigCategory::Custom, "Loaded from JSON");
                pImpl->entrySchemas[key] = valueWithType;
            } else {
                std::cerr << "Warning: Invalid value for key " << key << " in JSON file" << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading JSON file: " << e.what() << std::endl;
        return false;
    }
}

// YAML file loading (placeholder)
bool Config::loadFromYAML(const std::string& filepath) {
    std::cerr << "YAML loading is not yet implemented, falling back to simple format" << std::endl;
    return false;
}

// Update file timestamp for hot-reload
void Config::updateFileTimestamp(const std::string& filepath) {
    pImpl->fileWatchers[filepath] = std::chrono::system_clock::now();
}

// Load with inheritance
bool Config::loadWithInheritance(const std::vector<std::string>& baseFiles, 
                                 const std::map<std::string, std::string>& overrides) {
    for (const auto& filepath : baseFiles) {
        if (!loadFromFile(filepath)) {
            std::cerr << "Warning: Failed to load base configuration: " << filepath << std::endl;
        }
    }
    
    for (const auto& override : overrides) {
        pImpl->overrides[override.first] = override.second;
        set(override.first, parseValue(override.second), ConfigSource::Inherited, ConfigCategory::Custom, "Override");
    }
    
    loadFromEnvironment();
    
    return true;
}

// Apply template
bool Config::applyTemplate(const std::string& templateName) {
    for (const auto& template : pImpl->templates) {
        if (template.name == templateName) {
            for (const auto& pair : template.values) {
                set(pair.first, pair.second, ConfigSource::Template, template.category, "From template: " + templateName);
            }
            return true;
        }
    }
    
    std::cerr << "Error: Template not found: " << templateName << std::endl;
    return false;
}

// Get template names
std::vector<std::string> Config::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& template : pImpl->templates) {
        names.push_back(template.name);
    }
    return names;
}

// Add preset
void Config::addPreset(const ConfigPreset& preset) {
    pImpl->presets[preset.name] = preset;
}

// Apply preset
bool Config::applyPreset(const std::string& presetName) {
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        std::cerr << "Error: Preset not found: " << presetName << std::endl;
        return false;
    }
    
    clear();
    for (const auto& pair : it->second.values) {
        set(pair.first, pair.second, ConfigSource::File, ConfigCategory::Custom, "From preset: " + presetName);
    }
    return true;
}

// Get preset names
std::vector<std::string> Config::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->presets) {
        names.push_back(pair.first);
    }
    return names;
}

// Get suggestions
std::vector<std::string> Config::getSuggestions(const std::string& key) const {
    auto it = pImpl->suggestions.find(key);
    if (it != pImpl->suggestions.end()) {
        return it->second;
    }
    return {};
}

// Get documentation
std::string Config::getDocumentation(const std::string& key) const {
    auto it = pImpl->documentation.find(key);
    if (it != pImpl->documentation.end()) {
        return it->second;
    }
    return "No documentation available for key: " + key;
}

// Get all documentation
std::map<std::string, std::string> Config::getAllDocumentation() const {
    return pImpl->documentation;
}

// Get diff
std::string Config::getDiff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Difference\n";
    oss << "======================\n\n";
    
    std::set<std::string> allKeys;
    for (const auto& pair : pImpl->entries) {
        allKeys.insert(pair.first);
    }
    for (const auto& pair : other.pImpl->entries) {
        allKeys.insert(pair.first);
    }
    
    for (const auto& key : allKeys) {
        auto it1 = pImpl->entries.find(key);
        auto it2 = other.pImpl->entries.find(key);
        
        if (it1 != pImpl->entries.end() && it2 != pImpl->entries.end()) {
            if (it1->second.value != it2->second.value) {
                oss << "Key \"" << key << "\" differs:\n";
                oss << "  This:      ";
                exportValue(oss, it1->second.value);
                oss << "\n  Other:     ";
                exportValue(oss, it2->second.value);
                oss << "\n\n";
            }
        } else if (it1 != pImpl->entries.end()) {
            oss << "Key \"" << key << "\" only exists in this configuration\n";
            oss << "  Value:     ";
            exportValue(oss, it1->second.value);
            oss << "\n\n";
        } else if (it2 != pImpl->entries.end()) {
            oss << "Key \"" << key << "\" only exists in other configuration\n";
            oss << "  Value:     ";
            exportValue(oss, it2->second.value);
            oss << "\n\n";
        }
    }
    
    return oss.str();
}

// Merge configuration
void Config::merge(const Config& other, bool overwriteExisting) {
    for (const auto& pair : other.pImpl->entries) {
        if (overwriteExisting || pImpl->entries.find(pair.first) == pImpl->entries.end()) {
            set(pair.first, pair.second.value, pair.second.source, getCategory(pair.first), "Merged from other configuration");
        }
    }
}

} // namespace nlm