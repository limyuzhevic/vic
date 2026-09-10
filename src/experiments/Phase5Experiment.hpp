#pragma once

#include "core/Config/Config.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Brain;
class AgentBrain;
class SimpleWorld;
class DevelopmentalStage;
class ConfigSource;

class Phase5Experiment {
public:
    Phase5Experiment(std::shared_ptr<Config> config);
    ~Phase5Experiment();
    
    bool initialize();
    void run();
    
private:
    void runDevelopmentPhase();
    void runLearningPhase();
    void runMemoryConsolidationPhase();
    void runCheckpointPhase();
    void runPerformanceAnalysisPhase();
    void runNeuromodulationPhase();
    void runIntegrationSummaryPhase();
    
    float calculateReward(const SimpleWorld& world) const;
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<SimpleWorld> world_;
};

} // namespace nlm