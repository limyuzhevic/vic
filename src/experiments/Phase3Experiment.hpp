#pragma once

#include "../agent/AgentBody.hpp"
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

// Metrics collector for Phase 3 experiments
struct ExperimentMetrics {
    // Episode metrics
    int episode;
    int steps;
    float totalReward;
    float finalEnergy;
    float finalNovelty;
    float finalCuriosity;
    float predictionError;
    
    // Development
    float developmentalAge;
    std::string developmentalStage;
    
    // Network state
    size_t totalSynapses;
    float averageWeight;
    float maxWeight;
    size_t totalSpikes;
    float averageFiringRate;
    
    // Motor behavior
    int forwardCount;
    int backwardCount;
    int turnLeftCount;
    int turnRightCount;
    int interactCount;
    int waitCount;
    
    ExperimentMetrics();
    void reset();
    std::string toCSV() const;
};

} // namespace nlm
