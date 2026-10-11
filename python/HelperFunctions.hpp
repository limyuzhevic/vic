#pragma once

#include <string>
#include <vector>

namespace nlm {

class PythonHelperFunctions {
public:
    // Enhanced factory methods for common patterns
    static std::string runSimulation(std::shared_ptr<Brain> brain, size_t steps, bool verbose = false);
    static std::shared_ptr<Config> createPhase6Config();
    static std::shared_ptr<AgentBrain> createLearningAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        bool enableRewardModulation = true,
        bool enableStructuralPlasticity = true,
        bool enableDevelopment = true,
        bool enableCuriosity = true);
    static std::vector<std::string> simulateEpisode(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t maxSteps = 1000,
        bool recordMetrics = true);
};

} // namespace nlm
