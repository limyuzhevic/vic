// Brain Management CLI Tools
// Tools for creating, managing, and analyzing NLM brains

#pragma once

#include "CLIUtils.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {
namespace cli {
namespace brain {

class BrainCreator {
public:
    static std::shared_ptr<Brain> createBrain(const std::string& configFile = "configs/default.cfg");
    static std::shared_ptr<Brain> createBrainWithParams(const std::string& params);
    static bool saveBrain(const Brain& brain, const std::string& filepath);
    static std::shared_ptr<Brain> loadBrain(const std::string& filepath);
    static void displayBrainInfo(const Brain& brain);
    static std::string brainStatistics(const Brain& brain);
};

class BrainManager {
public:
    static bool createBrain(const std::string& configFile = "configs/default.cfg", 
                           const std::string& brainName = "brain");
    static bool loadBrain(const std::string& filepath);
    static bool saveBrain(const std::string& filepath, bool includeSnapshots = false);
    static bool runSimulation(const std::string& configFile = "configs/default.cfg",
                             size_t steps = 1000, double timestep = 0.001);
    static bool analyzeBrain(const std::string& configFile = "configs/default.cfg", 
                       const std::string& analysisType = "comprehensive");
    static void listBrains(const std::string& directory = ".");
    static bool deleteBrain(const std::string& brainFile);
};

class BrainProfiler {
public:
    struct PerformanceMetrics {
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t spikeCount = 0;
        size_t activeNeurons = 0;
        float firingRate = 0.0f;
        float excitationInhibitionRatio = 0.0f;
        double simulationTime = 0.0;
        size_t memoryUsage = 0;
    };
    
    static PerformanceMetrics profileBrain(const Brain& brain);
    static void printDetailedStats(const Brain& brain);
    static void saveStats(const Brain& brain, const std::string& filepath);
    static std::vector<PerformanceMetrics> profileSimulation(const std::string& configFile, size_t steps);
};

class BrainCheckpoint {
public:
    static bool createCheckpoint(const Brain& brain, const std::string& label = "");
    static bool restoreCheckpoint(const std::string& label = "");
    static bool listCheckpoints(const std::string& directory = ".");
    static bool deleteCheckpoint(const std::string& label);
    static std::string getCheckpointPath(const std::string& label);
};

class BrainAnalyzer {
public:
    static void analyzeConnectivity(const Brain& brain);
    static void analyzePlasticity(const Brain& brain, SimulationStep steps);
    static void analyzeLearning(const Brain& brain, SimulationStep steps);
    static void analyzeDevelopment(const Brain& brain);
    static void saveAnalysisResults(const std::string& results, const std::string& filepath);
};

} // namespace brain
} // namespace cli
} // namespace nlm
