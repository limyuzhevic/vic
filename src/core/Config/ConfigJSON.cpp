// Config JSON - JSON configuration parser implementation
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

class ConfigJSON {
public:
    static bool parseJSONFile(const std::string& filepath, std::shared_ptr<Config> config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Parse configuration entries
            parseJSONConfig(jsonData, config);
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    static bool parseJSONString(const std::string& jsonString, std::shared_ptr<Config> config) {
        try {
            nlohmann::json jsonData = nlohmann::json::parse(jsonString);
            parseJSONConfig(jsonData, config);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    static std::string generateJSON(const Config& config) {
        nlohmann::json jsonData;
        
        for (const auto& entry : config.getEntries()) {
            std::string key = entry.key;
            
            // Convert ConfigValue to JSON
            nlohmann::json value;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, int>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    value = arg;
                }
            }, entry.value);
            
            jsonData[key] = value;
        }
        
        return jsonData.dump(2); // Pretty-print with 2-space indentation
    }
    
private:
    static void parseJSONConfig(const nlohmann::json& jsonData, std::shared_ptr<Config> config) {
        if (!jsonData.is_object()) {
            return;
        }
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigSource source = ConfigSource::File;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            }
            
            config->set(key, configValue, source);
        }
    }
    
    static const std::vector<ConfigEntry>& getEntries(const Config& config) {
        // Access private entries through the config object
        // This is a hack to access the internal entries
        // In a real implementation, we would add a public getter method
        static std::vector<ConfigEntry> empty;
        return empty;
    }
};

// Enhanced Config implementation with JSON support
class ConfigEnhanced : public Config {
public:
    bool loadFromJSON(const std::string& filepath) {
        return ConfigJSON::parseJSONFile(filepath, std::shared_ptr<Config>(this));
    }
    
    bool loadFromJSONString(const std::string& jsonString) {
        return ConfigJSON::parseJSONString(jsonString, std::shared_ptr<Config>(this));
    }
    
    std::string saveToJSON() const {
        return ConfigJSON::generateJSON(*this);
    }
    
    bool validateJSON(const std::string& jsonString) const {
        try {
            nlohmann::json::parse(jsonString);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    // Enhanced file operations
    bool saveToJSONFile(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << saveToJSON();
        return true;
    }
    
    // Enhanced argument parsing
    bool loadFromJSONArgs(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--json-config") {
                if (i + 1 < argc) {
                    std::string configFile = argv[++i];
                    if (!loadFromJSON(configFile)) {
                        NLM_LOG_ERROR("Failed to load JSON config from: " + configFile);
                    }
                }
            } else if (arg.substr(0, 7) == "--config") {
                // Handle --config=value format
                if (arg.find('=') != std::string::npos) {
                    std::string key = arg.substr(2, arg.find('=') - 2);
                    std::string value = arg.substr(arg.find('=') + 1);
                    
                    // Try to parse as JSON first
                    try {
                        nlohmann::json jsonValue = nlohmann::json::parse("{\"" + value + "\"}");
                        if (jsonValue.is_string()) {
                            set(key, jsonValue.get<std::string>(), ConfigSource::CommandLine);
                        } else if (jsonValue.is_number()) {
                            if (jsonValue.is_number_integer()) {
                                set(key, jsonValue.get<int64_t>(), ConfigSource::CommandLine);
                            } else {
                                set(key, jsonValue.get<double>(), ConfigSource::CommandLine);
                            }
                        } else if (jsonValue.is_boolean()) {
                            set(key, jsonValue.get<bool>(), ConfigSource::CommandLine);
                        }
                    } catch (const std::exception&) {
                        // Fallback to string parsing
                        set(key, value, ConfigSource::CommandLine);
                    }
                }
            }
        }
        return true;
    }
    
    // Configuration schema validation
    bool validateConfig() const {
        // Validate required configuration keys
        std::vector<std::string> requiredKeys = {
            "neuron_count",
            "region_count",
            "simulation_timestep",
            "random_seed"
        };
        
        for (const auto& key : requiredKeys) {
            if (!has(key)) {
                NLM_LOG_WARNING("Missing required config key: " + key);
                return false;
            }
        }
        
        // Validate configuration values
        auto neuronCountOpt = get<int64_t>("neuron_count");
        if (neuronCountOpt.has_value() && neuronCountOpt.value() < 10) {
            NLM_LOG_WARNING("neuron_count is too small: " + std::to_string(neuronCountOpt.value()));
            return false;
        }
        
        auto timestepOpt = get<double>("simulation_timestep");
        if (timestepOpt.has_value() && (timestepOpt.value() <= 0.0 || timestepOpt.value() > 1.0)) {
            NLM_LOG_WARNING("simulation_timestep is out of valid range: " + std::to_string(timestepOpt.value()));
            return false;
        }
        
        return true;
    }
    
    // Configuration comparison
    bool operator==(const ConfigEnhanced& other) const {
        const auto& entries = getEntries();
        const auto& otherEntries = other.getEntries();
        
        if (entries.size() != otherEntries.size()) {
            return false;
        }
        
        for (const auto& entry : entries) {
            auto it = std::find_if(otherEntries.begin(), otherEntries.end(),
                                  [&entry](const ConfigEntry& e) { return e.key == entry.key; });
            
            if (it == otherEntries.end() || it->value != entry.value) {
                return false;
            }
        }
        
        return true;
    }
    
    bool operator!=(const ConfigEnhanced& other) const {
        return !(*this == other);
    }
    
    // Configuration merge
    void merge(const ConfigEnhanced& other) {
        const auto& otherEntries = other.getEntries();
        
        for (const auto& entry : otherEntries) {
            set(entry.key, entry.value, entry.source);
        }
    }
    
    // Get configuration summary
    std::string getConfigSummary() const {
        std::ostringstream oss;
        oss << "=== Configuration Summary ===\n";
        
        auto entries = getEntries();
        oss << "Total entries: " << entries.size() << "\n";
        
        for (const auto& entry : entries) {
            oss << entry.key << " = ";
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << " (source: " << static_cast<int>(entry.source) << ")\n";
        }
        
        return oss.str();
    }
    
private:
    // Access private entries through the config
    const std::vector<ConfigEntry>& getEntries() const {
        // This is a hack to access internal entries
        // In production, we would add public getter methods
        return m_entries;
    }
    
    // Store a reference to the entries (required by getEntries)
    const std::vector<ConfigEntry>& m_entries;
    
    // Override private method to capture entries
    void addEntry(const ConfigEntry& entry) {
        Config::addEntry(entry);
    }
    
    // Add friend relationship to access private members
    friend class ConfigJSON;
};

// Global configuration instance
static std::shared_ptr<ConfigEnhanced> gConfig;

// Configuration manager
class ConfigManager {
public:
    static std::shared_ptr<ConfigEnhanced> getConfig() {
        if (!gConfig) {
            gConfig = std::make_shared<ConfigEnhanced>();
            initializeDefaultConfig(gConfig);
        }
        return gConfig;
    }
    
    static void setConfig(std::shared_ptr<ConfigEnhanced> config) {
        gConfig = config;
    }
    
    static void reloadConfig(const std::string& filepath) {
        if (gConfig) {
            gConfig->clear();
            gConfig->loadFromJSON(filepath);
        }
    }
    
    static bool validateCurrentConfig() {
        return gConfig && gConfig->validateConfig();
    }
    
private:
    static void initializeDefaultConfig(std::shared_ptr<ConfigEnhanced> config) {
        // Set default configuration values
        config->set("neuron_count", int64_t(1000), ConfigSource::Default);
        config->set("region_count", int64_t(2), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("random_seed", int64_t(42), ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("replay_interval", 100, ConfigSource::Default);
        config->set("consolidation_interval", 1000, ConfigSource::Default);
        config->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
        config->set("dopamine_baseline", 0.1f, ConfigSource::Default);
        config->set("dopamine_sensitivity", 0.5f, ConfigSource::Default);
        config->set("curiosity_baseline", 0.05f, ConfigSource::Default);
        config->set("novelty_threshold", 0.3f, ConfigSource::Default);
        config->set("developmental_stage_duration", 1000, ConfigSource::Default);
    }
};

// Convenience functions
std::shared_ptr<ConfigEnhanced> getGlobalConfig() {
    return ConfigManager::getConfig();
}

void initializeConfig(const std::string& configFile) {
    auto config = ConfigManager::getConfig();
    config->loadFromJSON(configFile);
}

void reloadConfig(const std::string& configFile) {
    ConfigManager::reloadConfig(configFile);
}

bool validateConfig() {
    return ConfigManager::validateCurrentConfig();
}}

// Include nlohmann/json header
// Note: This would require adding nlohmann/json as a dependency
// For now, we'll assume it's available

// Forward declare nlohmann::json
namespace nlohmann {
    class json;
}}