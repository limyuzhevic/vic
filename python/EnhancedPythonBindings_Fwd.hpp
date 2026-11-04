// Forward declarations for Enhanced Python Bindings
#pragma once

#include "EnhancedPythonBindings.hpp"

namespace nlm {

// Forward declarations for types used in the bindings
class Brain;
class AgentBrain;
class SimpleWorld;
class SensoryPercept;
class MotorCommand;
class Config;

// Helper functions for creating enhanced wrappers
std::shared_ptr<PythonBrainWrapper> createEnhancedBrainWrapper(size_t neuronCount = Constants::BRAIN_DEFAULT_NEURON_COUNT,
                                                             float connectionProbability = Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
                                                             float simulationTimestep = Constants::DEFAULT_TIMESTEP,
                                                             int randomSeed = Constants::SIMULATION_DEFAULT_RANDOM_SEED,
                                                             const std::string& preset = "default");

std::shared_ptr<PythonAgentWrapper> createEnhancedAgentWrapper(std::shared_ptr<Brain> brain);

std::shared_ptr<PythonWorldWrapper> createEnhancedWorldWrapper();

// Convenience functions for backward compatibility
std::shared_ptr<Brain> createBrain(size_t neuronCount = Constants::BRAIN_DEFAULT_NEURON_COUNT,
                                 float connectionProbability = Constants::BRAIN_DEFAULT_CONNECTION_PROBABILITY,
                                 float simulationTimestep = Constants::DEFAULT_TIMESTEP,
                                 int randomSeed = Constants::SIMULATION_DEFAULT_RANDOM_SEED);

std::shared_ptr<SimpleWorld> createWorld();

std::shared_ptr<AgentBrain> createAgent(std::shared_ptr<Brain> brain);

void applyPreset(std::shared_ptr<Config> config, const std::string& presetName);

std::vector<std::string> getAvailablePresets();

std::pair<bool, std::vector<std::string>> validateConfig(std::shared_ptr<Config> config);

} // namespace nlm