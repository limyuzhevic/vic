#pragma once

#include "../agent/AgentBody.hpp"
#include "ExperimentBase.hpp"
#include "ExperimentResults.hpp"
#include <string>
#include <vector>

namespace nlm {

// Simple text logger for experiments
class ExperimentLogger {
public:
    ExperimentLogger(const std::string& filename);
    ~ExperimentLogger();
    
    void log(const std::string& message);
    void logEpisodeStart(int episode);
    void logEpisodeEnd(int episode, float totalReward, int steps, float finalNovelty);
    void logStep(int step, float reward, float neuromod, float curiosity, 
                 float energy, const std::string& action);
    void logDevelopment(float age, const std::string& stage);
    void logSynapticStats(size_t totalSynapses, float avgWeight, float maxWeight);
    void logPhase3Header();
    
    void flush();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Phase 3 experiment class using new infrastructure
class Phase3Experiment : public ExperimentBase {
public:
    Phase3Experiment(const std::string& name = "Phase3Experiment");
    ~Phase3Experiment() = default;
    
    // Lifecycle management
    bool initialize() override;
    bool run() override;
    void cleanup() override;
    bool isRunning() const override;
    
    // Phase 3 specific methods
    Phase3Results runExperiment(Brain* brain, size_t numEpisodes = 5, size_t stepsPerEpisode = 200);
    
    // Get accumulated results
    const Phase3Results& getResults() const { return phase3Results_; }
    
private:
    Phase3Results phase3Results_;
    std::unique_ptr<ExperimentLogger> logger_;
};

} // namespace nlm
