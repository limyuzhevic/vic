#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"
#include "src/sensory/SensoryInput.hpp"

namespace nlm {

class BrainManager {
public:
    BrainManager() = default;
    ~BrainManager() = default;

    // Configuration management
    static std::shared_ptr<Config> createConfig(
        size_t neuronCount = 1000,
        size_t regionCount = 1,
        float connectionProbability = 0.1f,
        double timestep = 0.001,
        uint64_t randomSeed = 42
    );

    static std::shared_ptr<Config> loadConfigFromFile(const std::string& filepath);
    static bool saveConfigToFile(const std::shared_ptr<Config>& config, const std::string& filepath);

    // Brain lifecycle management
    static std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config);
    static bool initializeBrain(std::shared_ptr<Brain>& brain);
    static void resetBrain(std::shared_ptr<Brain>& brain);
    static void cleanupBrain(std::shared_ptr<Brain>& brain);

    // Simplified simulation interface
    static void runSimpleSimulation(
        std::shared_ptr<Brain>& brain,
        size_t steps,
        const std::vector<float>& sensoryInput = {}
    );

    static void runSimulationWithWorld(
        std::shared_ptr<Brain>& brain,
        std::shared_ptr<SimpleWorld>& world,
        std::shared_ptr<AgentBrain>& agent,
        size_t steps
    );

    // Convenience methods for common operations
    static size_t getNeuronCount(const std::shared_ptr<Brain>& brain);
    static size_t getSynapseCount(const std::shared_ptr<Brain>& brain);
    static size_t getActiveNeuronCount(const std::shared_ptr<Brain>& brain);
    static float getAverageFiringRate(const std::shared_ptr<Brain>& brain);
    static float getExcitationInhibitionRatio(const std::shared_ptr<Brain>& brain);

    // Development and learning
    static void enableDevelopment(std::shared_ptr<Brain>& brain, bool enable = true);
    static void enablePlasticity(std::shared_ptr<Brain>& brain, bool enable = true);
    static void enableRewardModulation(std::shared_ptr<Brain>& brain, bool enable = true);
    static void enableCuriosity(std::shared_ptr<Brain>& brain, bool enable = true);

    // State management
    static bool saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath);
    static bool loadBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath);

    // Monitoring and statistics
    static std::string getBrainStatus(const std::shared_ptr<Brain>& brain);
    static std::unordered_map<std::string, double> getSystemStats(const std::shared_ptr<Brain>& brain);

    // Simulation utilities
    static void injectRandomSensoryInput(std::shared_ptr<Brain>& brain, size_t count = 10);
    static std::vector<float> getSensoryNeuronsState(std::shared_ptr<Brain>& brain);
    static void stepBrain(std::shared_ptr<Brain>& brain, size_t step);

private:
    static std::unordered_map<std::string, std::function<void()>> initializationCallbacks_;
    static std::unordered_map<std::string, std::function<void()>> cleanupCallbacks_;
};

} // namespace nlm
