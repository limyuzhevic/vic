#pragma once

#include "Config.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {
namespace ConfigPresets {

// Preset names for common configurations
enum class PresetName {
    Default,
    FastTest,
    HighAccuracy,
    LowPower,
    Custom
};

// Get a configuration preset by name
std::shared_ptr<Config> getPreset(const std::string& presetName);

// Get a configuration preset by enum
std::shared_ptr<Config> getPreset(PresetName presetName);

// List available preset names
std::vector<std::string> listPresets();

// Apply a preset to an existing config
void applyPreset(Config& config, const std::string& presetName);

// Apply a preset to an existing config by enum
void applyPreset(Config& config, PresetName presetName);

// Create a custom preset with the given name
std::shared_ptr<Config> createCustomPreset(const std::string& presetName);

// Update an existing custom preset
void updatePreset(const std::string& presetName, const Config& config);

// Remove a custom preset
void removePreset(const std::string& presetName);

// Check if a preset is a built-in preset
bool isBuiltInPreset(const std::string& presetName);

// Get the default configuration
std::shared_ptr<Config> getDefaultConfig();

// Get a test configuration (optimized for fast testing)
std::shared_ptr<Config> getTestConfig();

// Get a high accuracy configuration (optimized for precision)
std::shared_ptr<Config> getHighAccuracyConfig();

// Get a low power configuration (optimized for energy efficiency)
std::shared_ptr<Config> getLowPowerConfig();

} // namespace ConfigPresets
} // namespace nlm
