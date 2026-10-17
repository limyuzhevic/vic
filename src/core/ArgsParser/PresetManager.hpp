#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <memory>

namespace nlm {

class PresetManager {
public:
    PresetManager();
    ~PresetManager();
    
    // Disable copying, enable moving
    PresetManager(const PresetManager&) = delete;
    PresetManager& operator=(const PresetManager&) = delete;
    PresetManager(PresetManager&&) noexcept;
    PresetManager& operator=(PresetManager&&) noexcept;
    
    // Add a preset
    void addPreset(const Preset& preset);
    
    // Get preset by name
    const Preset* getPreset(const std::string& name) const;
    
    // Remove preset by name
    bool removePreset(const std::string& name);
    
    // Get all preset names
    std::vector<std::string> getPresetNames() const;
    
    // Apply a preset to configuration
    bool applyPreset(const std::string& presetName, 
                     std::map<std::string, ArgsValue>& config) const;
    
    // Get configuration from preset
    std::map<std::string, ArgsValue> getPresetConfig(const std::string& presetName) const;
    
    // Merge configurations (higher priority overrides)
    static std::map<std::string, ArgsValue> mergeConfigs(
        const std::map<std::string, ArgsValue>& base,
        const std::map<std::string, ArgsValue>& override);
    
    // Find best matching preset based on criteria
    std::string findMatchingPreset(const std::map<std::string, ArgsValue>& config) const;
    
    // Clear all presets
    void clear() noexcept;
    
    // Get all presets
    const std::vector<Preset>& getPresets() const { return presets; }
    
    // Get preset count
    size_t getPresetCount() const { return presets.size(); }
    
    // Preset factory registration
    void registerPresetFactory(const std::string& name,
                               const std::function<std::map<std::string, ArgsValue>()>& factory);
    
    // Create preset from factory
    std::map<std::string, ArgsValue> createFromFactory(const std::string& name) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};