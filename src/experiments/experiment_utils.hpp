#pragma once

#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <functional>
#include <vector>
#include <string>

namespace nlm {

/**
 * Create a brain from a configuration object
 * Reduces code duplication in brain creation patterns
 */
std::shared_ptr<Brain> createBrainFromConfig(const Config& config);

/**
 * Run a common simulation loop with reward computation
 * Standardizes simulation step-by-step processing
 */
float runSimulationLoop(
    Brain* brain,
    float (*getReward)(Brain*),
    uint64_t steps,
    std::function<void(uint64_t, float)> progressCallback = nullptr
);

/**
 * Get standardized sensory observation from the world
 * Centralizes observation creation patterns
 */
std::vector<float> getSensoryObservation(SimpleWorld& world, Brain* brain);

/**
 * Create default configuration for experiment setup
 * Provides common experiment configuration with customizable parameters
 */
Config createDefaultConfigForExperiment(
    size_t neuronCount = 1000,
    size_t regionCount = 1,
    float connectionProbability = 0.1f
);

/**
 * Memory system integration check utilities
 * Standardized validation of memory system access
 */
bool checkWorkingMemoryIntegration(Brain* brain);
bool checkEpisodicMemoryIntegration(Brain* brain);
bool checkNeuromodulationIntegration(Brain* brain);
bool checkPredictionIntegration(Brain* brain);
bool checkDevelopmentIntegration(Brain* brain);

/**
 * Common reward calculation utilities
 * Standardized reward computation approaches
 */
float computeStandardReward(Brain* brain, const std::vector<float>& observation);
float computeEfficiencyReward(Brain* brain, float baseReward);

/**
 * Common sensory input processing pattern
 * Standardized approach to sensory processing
 */
std::vector<float> processSensoryInput(SimpleWorld& world, Brain* brain, uint64_t step);

/**
 * Common brain step processing with metrics collection
 * Standardized brain stepping with performance metrics
 */
void processBrainStep(Brain* brain, uint64_t step, float deltaTime = 0.001f);

} // namespace nlm
