// Phase 5 Continual Learning Demo Header
// Shows: experience across multiple tasks and domains

#ifndef NLM_PHASE5DEMO_HPP
#define NLM_PHASE5DEMO_HPP

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "Phase5Experiment.hpp"
#include <vector>
#include <functional>
#include <string>

namespace nlm {

// Forward declarations
class Phase5Demo;

// Metrics for Phase 5 experiments
struct ExperimentMetrics {
    uint64_t episode = 0;
    uint64_t steps = 0;
    float totalReward = 0.0f;
    float finalNovelty = 0.0f;
    float finalCuriosity = 0.0f;
    float finalEnergy = 0.0f;
    float predictionError = 0.0f;
    uint64_t developmentalAge = 0;
    std::string developmentalStage = "";
    uint64_t totalSynapses = 0;
    float averageFiringRate = 0.0f;
    
    // Action counts
    uint64_t forwardCount = 0;
    uint64_t backwardCount = 0;
    uint64_t turnLeftCount = 0;
    uint64_t turnRightCount = 0;
    uint64_t interactCount = 0;
    uint64_t waitCount = 0;
};

// Continual learning task definition
struct ContinualTask {
    std::string name;
    uint64_t stepsBetweenTasks;
    std::function<float(Brain* brain)> evaluateFunction;
};

// Phase 5 demo class
class Phase5Demo {
public:
    Phase5Demo();
    ~Phase5Demo();
    
    void run(int numTasks = 3, int cyclesPerTask = 5, uint64_t stepsPerCycle = 1000);
    
private:
    void configureConfig(std::shared_ptr<Config> config);
    void setupValidationRules(std::shared_ptr<Config> config);
    void createTasks(std::vector<ContinualTask>& tasks);
    void displayResults(const LifetimeExperimentResult& result,
                       const std::vector<ContinualTask>& tasks,
                       double wallClockTime);
    std::string generateJSONReport(const LifetimeExperimentResult& result,
                                   const std::vector<ContinualTask>& tasks);
};

// Global demo instance for callback compatibility
Phase5Demo* g_phase5Demo;

} // namespace nlm

#endif // NLM_PHASE5DEMO_HPP
