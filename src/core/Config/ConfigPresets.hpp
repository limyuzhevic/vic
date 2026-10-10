#pragma once

#include "Config.hpp"
#include <string>
#include <map>
#include <vector>

namespace nlm {

// Configuration preset definition
struct Preset {
    std::string name;
    std::string description;
    PresetType type;
    std::map<std::string, ConfigValue> values;
    
    Preset() : name(), description(), type(PresetType::Simple) {}
    
    Preset(const std::string& name, const std::string& description, PresetType type)
        : name(name), description(description), type(type) {}
    
    // Add a configuration value
    void add(const std::string& key, const ConfigValue& value) {
        values[key] = value;
    }
    
    // Add a configuration value with convenience methods
    void add(const std::string& key, int value) {
        values[key] = value;
    }
    
    void add(const std::string& key, double value) {
        values[key] = value;
    }
    
    void add(const std::string& key, bool value) {
        values[key] = value;
    }
    
    void add(const std::string& key, const std::string& value) {
        values[key] = value;
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(values.size());
        for (const auto& pair : values) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // Check if preset has a key
    bool has(const std::string& key) const {
        return values.find(key) != values.end();
    }
    
    // Get value
    ConfigValue get(const std::string& key) const {
        auto it = values.find(key);
        if (it != values.end()) {
            return it->second;
        }
        throw std::runtime_error("Preset key not found: " + key);
    }
    
    // Get value with type conversion
    template<typename T>
    T get(const std::string& key) const {
        auto value = get(key);
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Type mismatch for key: " + key);
        }
    }
};

// Configuration presets manager
class ConfigPresets {
public:
    ConfigPresets();
    ~ConfigPresets() = default;
    
    // Get preset by name
    Preset getPreset(const std::string& name) const;
    
    // Get all preset names
    std::vector<std::string> getPresetNames() const;
    
    // Check if preset exists
    bool hasPreset(const std::string& name) const;
    
    // Get preset type
    PresetType getPresetType(const std::string& name) const;
    
    // Create a new preset
    void addPreset(const Preset& preset);
    
    // Update an existing preset
    void updatePreset(const std::string& name, const Preset& preset);
    
    // Remove a preset
    void removePreset(const std::string& name);
    
    // Get default presets
    static std::vector<Preset> getDefaultPresets();
    
    // Get simple preset
    static Preset getSimplePreset();
    
    // Get medium preset
    static Preset getMediumPreset();
    
    // Get advanced preset
    static Preset getAdvancedPreset();
    
    // Get debug preset
    static Preset getDebugPreset();
    
    // Apply preset to configuration
    static void applyPreset(Config& config, const std::string& name, ConfigSource source = ConfigSource::Runtime);
    
    // Apply preset type to configuration
    static void applyPresetType(Config& config, PresetType type, ConfigSource source = ConfigSource::Runtime);
    
    // Merge configuration with preset
    static void mergePreset(Config& config, const std::string& name, ConfigSource source = ConfigSource::Runtime);
    
    // Get preset description
    static std::string getPresetDescription(const std::string& name);
    
    // Get all preset descriptions
    static std::map<std::string, std::string> getAllPresetDescriptions();
    
    // Create configuration from preset
    static std::map<std::string, ConfigValue> createConfigMap(const std::string& name);
    
    // Create configuration from preset type
    static std::map<std::string, ConfigValue> createConfigMap(PresetType type);
    
private:
    std::map<std::string, Preset> presets;
    
    // Initialize default presets
    void initializePresets();
};

} // namespace nlm