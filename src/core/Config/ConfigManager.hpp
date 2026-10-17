#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {

// Configuration value types
enum class ConfigValueType {
    Int,
    Float,
    String,
    Bool,
    VectorInt,
    VectorFloat,
    VectorString
};

// Base configuration value
class ConfigValue {
public:
    virtual ~ConfigValue() = default;
    virtual ConfigValueType getType() const = 0;
    virtual std::string toString() const = 0;
    virtual void fromString(const std::string& str) = 0;
    virtual ConfigValue* clone() const = 0;
};

// Template-based configuration values
template<typename T>
class TypedConfigValue : public ConfigValue {
public:
    TypedConfigValue(const T& value) : value_(value) {}
    
    ConfigValueType getType() const override {
        return getConfigValueType(value_);
    }
    
    std::string toString() const override {
        std::stringstream ss;
        ss << value_;
        return ss.str();
    }
    
    void fromString(const std::string& str) override {
        std::stringstream ss(str);
        ss >> value_;
    }
    
    T get() const { return value_; }
    void set(const T& value) { value_ = value; }
    
    ConfigValue* clone() const override {
        return new TypedConfigValue<T>(value_);
    }
    
private:
    T value_;
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string name;
    std::shared_ptr<ConfigValue> value;
    std::string description;
    bool required;
    std::string validationPattern;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigEntry(const std::string& n, std::shared_ptr<ConfigValue> v, const std::string& desc = "",
                bool req = false, const std::string& pattern = "")
        : name(n), value(v), description(desc), required(req), validationPattern(pattern) {}
};

// Main configuration manager
class ConfigManager {
public:
    ConfigManager(const std::string& name = "NLM_Config")
        : name_(name), version_(1), loadCount_(0), saveCount_(0),
          autoSave_(false), autoSaveInterval_(300.0), threadSafe_(true) {
        startTime_ = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    ~ConfigManager() = default;
    
    // Disable copying, enable moving
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    ConfigManager(ConfigManager&&) noexcept = default;
    ConfigManager& operator=(ConfigManager&&) noexcept = default;
    
    // Configuration management
    bool set(const std::string& key, const std::string& value);
    bool set(const std::string& key, int value);
    bool set(const std::string& key, float value);
    bool set(const std::string& key, bool value);
    bool set(const std::string& key, const std::vector<int>& value);
    bool set(const std::string& key, const std::vector<float>& value);
    
    template<typename T>
    bool set(const std::string& key, const T& value) {
        auto entryIt = findEntry(key);
        if (entryIt != entries_.end()) {
            // Update existing entry
            auto newValue = createConfigValue(value);
            entryIt->value = newValue;
            return true;
        }
        // Create new entry
        auto newValue = createConfigValue(value);
        entries_.push_back(ConfigEntry(key, newValue, "Generated entry"));
        return true;
    }
    
    // Get configuration values
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    float getFloat(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::vector<int> getVectorInt(const std::string& key) const;
    std::vector<float> getVectorFloat(const std::string& key) const;
    
    template<typename T>
    T get(const std::string& key) const {
        auto entryIt = findEntry(key);
        if (entryIt != entries_.end()) {
            auto typedValue = std::dynamic_pointer_cast<TypedConfigValue<T>>(entryIt->value);
            if (typedValue) {
                return typedValue->get();
            }
        }
        throw std::runtime_error("Configuration key not found: " + key);
    }
    
    // Configuration metadata
    bool hasKey(const std::string& key) const;
    std::vector<std::string> getAllKeys() const;
    std::string getDescription(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    
    // Add configuration entries
    bool addEntry(const std::string& key, const std::string& value, 
                  const std::string& description = "",
                  bool required = false,
                  const std::string& validationPattern = "",
                  std::function<bool(const ConfigValue&)> validator = nullptr);
    
    bool addIntEntry(const std::string& key, int value,
                     const std::string& description = "",
                     bool required = false,
                     int minValue = INT_MIN,
                     int maxValue = INT_MAX);
    
    bool addFloatEntry(const std::string& key, float value,
                       const std::string& description = "",
                       bool required = false,
                       float minValue = -std::numeric_limits<float>::max(),
                       float maxValue = std::numeric_limits<float>::max());
    
    bool addBoolEntry(const std::string& key, bool value,
                      const std::string& description = "",
                      bool required = false);
    
    // Configuration file operations
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromString(const std::string& configString);
    std::string saveToString() const;
    
    // Command line arguments
    bool loadFromCommandLine(int argc, char* argv[]);
    void printUsage(const std::string& programName) const;
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration groups
    bool createGroup(const std::string& groupName);
    bool addToGroup(const std::string& key, const std::string& groupName);
    std::vector<std::string> getKeysInGroup(const std::string& groupName) const;
    std::vector<std::string> getAllGroups() const;
    
    // Configuration statistics and monitoring
    std::string getStats() const;
    double getUptime() const;
    size_t getLoadCount() const { return loadCount_; }
    size_t getSaveCount() const { return saveCount_; }
    
    // Configuration backup and recovery
    bool createBackup(const std::string& backupPath = "");
    bool restoreFromBackup(const std::string& backupPath = "");
    std::string getBackupList() const;
    
    // Configuration comparison
    bool equals(const ConfigManager& other) const;
    std::string diff(const ConfigManager& other) const;
    
    // Configuration merging
    bool merge(const ConfigManager& other, bool overrideExisting = true);
    
    // Configuration reset
    void reset();
    void resetGroup(const std::string& groupName);
    
    // Configuration templates
    bool saveTemplate(const std::string& templateName);
    bool loadTemplate(const std::string& templateName);
    std::string getTemplateList() const;
    
    // Configuration monitoring
    void enableMonitoring(bool enable) { monitoringEnabled_ = enable; }
    bool isMonitoringEnabled() const { return monitoringEnabled_; }
    void setMonitoringCallback(std::function<void(const std::string&, const ConfigValue&)> callback) {
        monitoringCallback_ = callback;
    }
    
    // Configuration encryption (basic)
    bool enableEncryption(const std::string& keyFile = "");
    bool disableEncryption();
    bool isEncrypted() const { return encryptionEnabled_; }
    
    // Configuration compression
    bool enableCompression(bool enable) { compressionEnabled_ = enable; return true; }
    bool isCompressed() const { return compressionEnabled_; }
    
private:
    std::string name_;
    int version_;
    size_t loadCount_;
    size_t saveCount_;
    double startTime_;
    double autoSaveInterval_;
    bool autoSave_;
    bool threadSafe_;
    bool monitoringEnabled_;
    bool encryptionEnabled_;
    bool compressionEnabled_;
    
    std::vector<ConfigEntry> entries_;
    std::unordered_map<std::string, std::vector<std::string>> groups_;
    mutable std::mutex mutex_;
    
    std::function<void(const std::string&, const ConfigValue&)> monitoringCallback_;
    
    // Helper methods
    ConfigEntry* findEntry(const std::string& key);
    const ConfigEntry* findEntry(const std::string& key) const;
    
    template<typename T>
    std::shared_ptr<ConfigValue> createConfigValue(const T& value) {
        return std::make_shared<TypedConfigValue<T>>(value);
    }
    
    static ConfigValueType getConfigValueType(const std::string&) { return ConfigValueType::String; }
    static ConfigValueType getConfigValueType(int) { return ConfigValueType::Int; }
    static ConfigValueType getConfigValueType(float) { return ConfigValueType::Float; }
    static ConfigValueType getConfigValueType(bool) { return ConfigValueType::Bool; }
    static ConfigValueType getConfigValueType(const std::vector<int>&) { return ConfigValueType::VectorInt; }
    static ConfigValueType getConfigValueType(const std::vector<float>&) { return ConfigValueType::VectorFloat; }
    static ConfigValueType getConfigValueType(const std::vector<std::string>&) { return ConfigValueType::VectorString; }
    
    std::string serializeValue(const ConfigValue& value) const;
    bool deserializeValue(const std::string& str, ConfigValue& value);
    bool validateEntry(const ConfigEntry& entry) const;
    
    void notifyMonitoring(const std::string& key, const ConfigValue& value) const;
};

// Specialized configuration managers for different components
class AgentConfigManager : public ConfigManager {
public:
    AgentConfigManager() : ConfigManager("Agent_Config") {
        initializeDefaultValues();
    }
    
    void initializeDefaultValues() {
        // Sensory configuration
        addIntEntry("vision_width", 16, "Vision sensor width in pixels", true, 1, 1024);
        addIntEntry("vision_height", 16, "Vision sensor height in pixels", true, 1, 1024);
        addIntEntry("touch_sensors", 8, "Number of touch sensors", true, 1, 50);
        addIntEntry("internal_sensors", 4, "Number of internal sensors", true, 1, 20);
        addIntEntry("proprioception_sensors", 6, "Number of proprioception sensors", true, 1, 20);
        
        // Motor configuration
        addIntEntry("motor_groups", 6, "Number of motor neuron groups", true, 1, 50);
        addFloatEntry("motor_scaling", 1.0f, "Motor neuron scaling factor", true, 0.1f, 10.0f);
        
        // Neuromodulation configuration
        addFloatEntry("curiosity_threshold", 0.3f, "Curiosity threshold for exploration", true, 0.0f, 1.0f);
        addFloatEntry("exploration_threshold", 0.5f, "Exploration threshold for random actions", true, 0.0f, 1.0f);
        addFloatEntry("novelty_decay", 0.99f, "Novelty decay rate per timestep", true, 0.0f, 1.0f);
        
        // Development configuration
        addFloatEntry("developmental_timescale", 1.0, "Developmental process timescale", true, 0.1f, 10.0f);
        addFloatEntry("plasticity_modifier", 1.0f, "Plasticity modifier factor", true, 0.1f, 5.0f);
        
        // Performance configuration
        addBoolEntry("enable_sensory_buffering", true, "Enable sensory input buffering", true);
        addBoolEntry("enable_motor_optimization", true, "Enable motor output optimization", true);
        addIntEntry("max_neuron_groups", 100, "Maximum number of neuron groups", true, 10, 1000);
        
        // Debug configuration
        addBoolEntry("enable_debug_logging", false, "Enable debug logging", true);
        addBoolEntry("enable_performance_metrics", false, "Enable performance metrics collection", true);
    }
    
    // Agent-specific configuration methods
    size_t getVisionWidth() const { return getInt("vision_width"); }
    size_t getVisionHeight() const { return getInt("vision_height"); }
    size_t getTouchSensors() const { return getInt("touch_sensors"); }
    size_t getInternalSensors() const { return getInt("internal_sensors"); }
    size_t getProprioceptionSensors() const { return getInt("proprioception_sensors"); }
    
    size_t getMotorGroups() const { return getInt("motor_groups"); }
    float getMotorScaling() const { return getFloat("motor_scaling"); }
    
    float getCuriosityThreshold() const { return getFloat("curiosity_threshold"); }
    float getExplorationThreshold() const { return getFloat("exploration_threshold"); }
    float getNoveltyDecay() const { return getFloat("novelty_decay"); }
    
    float getDevelopmentalTimescale() const { return getFloat("developmental_timescale"); }
    float getPlasticityModifier() const { return getFloat("plasticity_modifier"); }
    
    bool getEnableSensoryBuffering() const { return getBool("enable_sensory_buffering"); }
    bool getEnableMotorOptimization() const { return getBool("enable_motor_optimization"); }
    size_t getMaxNeuronGroups() const { return getInt("max_neuron_groups"); }
    
    bool getEnableDebugLogging() const { return getBool("enable_debug_logging"); }
    bool getEnablePerformanceMetrics() const { return getBool("enable_performance_metrics"); }
};

class WorldConfigManager : public ConfigManager {
public:
    WorldConfigManager() : ConfigManager("World_Config") {
        initializeDefaultValues();
    }
    
    void initializeDefaultValues() {
        // World geometry
        addFloatEntry("world_width", 20.0f, "World width in meters", true, 1.0f, 1000.0f);
        addFloatEntry("world_height", 20.0f, "World height in meters", true, 1.0f, 1000.0f);
        
        // Agent properties
        addFloatEntry("max_energy", 100.0f, "Maximum agent energy", true, 0.0f, 1000.0f);
        addFloatEntry("energy_decay_rate", 0.01f, "Energy decay rate per second", true, 0.0f, 1.0f);
        addFloatEntry("resource_energy_gain", 30.0f, "Energy gained from resources", true, 0.0f, 1000.0f);
        
        // Object properties
        addIntEntry("initial_resources", 5, "Number of initial resource objects", true, 0, 50);
        addIntEntry("initial_hazards", 3, "Number of initial hazard objects", true, 0, 50);
        addFloatEntry("resource_value", 10.0f, "Value of resource objects", true, 0.0f, 1000.0f);
        addFloatEntry("hazard_damage", -15.0f, "Damage from hazard objects", true, -1000.0f, 0.0f);
        
        // Vision configuration
        addIntEntry("vision_ray_count", 16, "Number of vision rays", true, 1, 64);
        addFloatEntry("vision_field_of_view", 1.5708f, "Vision field of view in radians", true, 0.0f, 3.1416f);
        addFloatEntry("vision_max_range", 8.0f, "Maximum vision range", true, 1.0f, 100.0f);
        
        // Physics
        addFloatEntry("move_speed", 3.0f, "Agent movement speed", true, 0.1f, 20.0f);
        addFloatEntry("turn_speed", 2.0f, "Agent turning speed", true, 0.1f, 10.0f);
        addFloatEntry("action_cooldown", 0.1f, "Minimum time between actions", true, 0.0f, 10.0f);
    }
    
    // World-specific configuration methods
    float getWorldWidth() const { return getFloat("world_width"); }
    float getWorldHeight() const { return getFloat("world_height"); }
    
    float getMaxEnergy() const { return getFloat("max_energy"); }
    float getEnergyDecayRate() const { return getFloat("energy_decay_rate"); }
    float getResourceEnergyGain() const { return getFloat("resource_energy_gain"); }
    
    int getInitialResources() const { return getInt("initial_resources"); }
    int getInitialHazards() const { return getInt("initial_hazards"); }
    float getResourceValue() const { return getFloat("resource_value"); }
    float getHazardDamage() const { return getFloat("hazard_damage"); }
    
    size_t getVisionRayCount() const { return getInt("vision_ray_count"); }
    float getVisionFieldOfView() const { return getFloat("vision_field_of_view"); }
    float getVisionMaxRange() const { return getFloat("vision_max_range"); }
    
    float getMoveSpeed() const { return getFloat("move_speed"); }
    float getTurnSpeed() const { return getFloat("turn_speed"); }
    float getActionCooldown() const { return getFloat("action_cooldown"); }
};

class ConfigFactory {
public:
    static std::shared_ptr<ConfigManager> createConfigManager(const std::string& type) {
        if (type == "agent") {
            return std::make_shared<AgentConfigManager>();
        } else if (type == "world") {
            return std::make_shared<WorldConfigManager>();
        } else {
            return std::make_shared<ConfigManager>(type);
        }
    }
    
    static std::vector<std::string> getAvailableTypes() {
        return {"agent", "world", "global"};
    }
};

} // namespace nlm