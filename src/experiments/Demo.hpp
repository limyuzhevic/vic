#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <iomanip>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "motor/Action.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"

namespace nlm {

/**
 * Metrics structure for tracking experiment results
 */
struct ExperimentMetrics {
    int episode = 0;
    size_t steps = 0;
    float totalReward = 0.0f;
    size_t totalSpikes = 0;
    
    // Action counts
    size_t forwardCount = 0;
    size_t backwardCount = 0;
    size_t turnLeftCount = 0;
    size_t turnRightCount = 0;
    size_t interactCount = 0;
    size_t waitCount = 0;
    
    // Final state
    float finalNovelty = 0.0f;
    float finalCuriosity = 0.0f;
    float finalEnergy = 0.0f;
    float predictionError = 0.0f;
    size_t developmentalAge = 0;
    std::string developmentalStage = "";
    size_t totalSynapses = 0;
    float averageFiringRate = 0.0f;
};

/**
 * Phase 3 Demo class - Encapsulates Phase 3 world interaction demonstration
 */
class Phase3Demo {
public:
    Phase3Demo();
    ~Phase3Demo();
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200);
    
private:
    void configureEnvironment(std::shared_ptr<Config> config);
    void configureBrain(std::shared_ptr<Config> config);
    
    const char* motorCommandToString(MotorCommand cmd);
    const char* stageToString(DevelopmentalStage stage);
    
    // Current implementation is in .cpp file
    // This header provides the interface
};

/**
 * Phase 3 Demo implementation
 * Extracted from Phase3Demo.cpp to follow proper C++ practices
 */

inline Phase3Demo::Phase3Demo() {}
inline Phase3Demo::~Phase3Demo() {}

/**
 * Phase 4 Demo utility functions
 */
void printHeader(const std::string& title);
void printResult(const std::string& name, float value, float max = 1.0f);
void printSection(const std::string& title);

/**
 * Benchmark utility functions
 */
struct BenchmarkResult {
    size_t neurons = 0;
    size_t synapses = 0;
    size_t steps = 0;
    double totalTime = 0.0;      // seconds
    double stepsPerSecond = 0.0;
    double spikesPerStep = 0.0;
    double synapsesPerNeuron = 0.0;
};

/**
 * Simple demo structure for Phase 6 integration demonstration
 */
struct Phase6Demo {
    bool runIntegrationDemo();
    void printResults(const Phase6Results& results);
};

/**
 * Common utility functions used across demos
 */
const char* getMotorCommandString(MotorCommand cmd);
const char* getDevelopmentalStageString(DevelopmentalStage stage);

} // namespace nlm
