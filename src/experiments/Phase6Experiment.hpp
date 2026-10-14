// Phase 6 Integration Experiment Header
// Final Integration Phase - connects all systems into a coherent artificial brain

#ifndef NLM_PHASE6EXPERIMENT_HPP
#define NLM_PHASE6EXPERIMENT_HPP

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <functional>

namespace nlm {

// ============================================================================
// Phase6Config - Configuration for Phase 6 integration experiments
// ============================================================================

struct Phase6Config {
    uint64_t neuronCount = 500;
    uint64_t regionCount = 1;
    float connectionProbability = 0.15f;
    uint64_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableDevelopment = true;
    bool enableReplay = true;
    std::string checkpointPath = "/tmp/nlm_phase6_checkpoint.bin";
    uint32_t environmentWidth = 16;
    uint32_t environmentHeight = 16;
};

// ============================================================================
// Phase6IntegrationResult - Results from Phase 6 integration experiments
// ============================================================================

struct Phase6IntegrationResult {
    time_t startTime = 0;
    time_t endTime = 0;
    
    // Integration verification results
    bool memoryWorkingMemoryIntegrated = false;
    bool memoryEpisodicMemoryIntegrated = false;
    bool neuromodulationIntegrated = false;
    bool predictionIntegrated = false;
    bool developmentIntegrated = false;
    bool checkpointingWorks = false;
    
    // Performance metrics
    float totalReward = 0.0f;
    float avgFiringRate = 0.0f;
    float memoryEpisodesStored = 0.0f;
    float noveltyLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float dopamineLevel = 0.0f;
    
    // Timing
    uint64_t totalSteps = 0;
    double totalWallClockTime = 0.0;
    
    // Convert to JSON format
    std::string toJSON() const;
    
    // Generate human-readable summary
    std::string summary() const;
};

// ============================================================================
// Phase6IntegratedExperiment - Main Phase 6 integration experiment runner
// ============================================================================

class Phase6IntegratedExperiment {
public:
    Phase6IntegratedExperiment();
    ~Phase6IntegratedExperiment();
    
    // Run a single integration experiment
    Phase6IntegrationResult run(
        const Phase6Config& config,
        const std::function<std::shared_ptr<Brain>(const Config& config)>& createBrain,
        const std::function<std::shared_ptr<SimpleWorld>(const Config& config)>& createWorld,
        const std::function<std::shared_ptr<AgentBrain>(std::shared_ptr<Brain> brain)>& createAgent
    );
    
    // Run multiple configurations for comprehensive testing
    std::vector<Phase6IntegrationResult> runMultipleConfigurations(
        const std::vector<Phase6Config>& configs,
        const std::function<std::shared_ptr<Brain>(const Config& config)>& createBrain,
        const std::function<std::shared_ptr<SimpleWorld>(const Config& config)>& createWorld,
        const std::function<std::shared_ptr<AgentBrain>(std::shared_ptr<Brain> brain)>& createAgent
    );
    
    // Generate comprehensive system report
    std::vector<std::string> generateSystemReport(std::shared_ptr<Brain> brain);
    
private:
    Phase6IntegrationResult initializeResult(const Phase6Config& config);
};

} // namespace nlm

#endif // NLM_PHASE6EXPERIMENT_HPP
