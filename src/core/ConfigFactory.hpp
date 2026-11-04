#pragma once

#include "core/Constants.hpp"
#include "core/ConfigDefaults.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

namespace nlm {

// Global configuration factory and convenience functions

// Create default configuration for basic usage
std::shared_ptr<Config> createDefaultConfig();

// Create configuration with specific parameters
std::shared_ptr<Config> createConfig(
    size_t neuronCount = Constants::BRAIN_DEFAULT_NEURON_COUNT,
    float connectionProbability = Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
    float simulationTimestep = Constants::DEFAULT_TIMESTEP,
    int randomSeed = Constants::SIMULATION_DEFAULT_RANDOM_SEED
);

// Create configuration from JSON file
std::shared_ptr<Config> createConfigFromFile(const std::string& filepath);

// Create configuration from command line arguments
std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv);

// Apply common configuration presets
void applyPreset(std::shared_ptr<Config> config, const std::string& presetName);

// Get list of available presets
std::vector<std::string> getAvailablePresets();

// Configuration validation
bool validateConfig(const Config& config, std::vector<std::string>& errors);

// Configuration summary for logging
std::string configSummary(const Config& config);

} // namespace nlm