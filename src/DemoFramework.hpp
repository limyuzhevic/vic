// Unified Demo Framework for NLM
// Provides common functionality for all Phase demos

#pragma once

#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * Common configuration for neural network demos
 */
struct DemoConfig {
    size_t neuronCount = 500;
    size_t regionCount = 2;
    float connectionProbability = 0.1f;
    int64_t randomSeed = 42;
    float simulationTimestep = 0.001f;
    float stdpLtpWeight = 0.01f;
    float stdpLtdWeight = 0.012f;
    float stdpTau = 20.0f;
    float synaptogenesisRate = 0.0001f;
    float pruningRate = 0.00001f;
    std::string worldSizeX = "20.0f";
    std::string worldSizeY = "20.0f";
    size_t visionWidth = 16;
    size_t visionHeight = 16;
    std::string environmentName = "GridWorld";
    bool enableRewardModulation = true;
    bool enableCuriosity = true;
    bool enableStructuralPlasticity = true;
    bool enableDevelopment = true;
    bool enableNeuromodulation = true;
    
    DemoConfig() = default;
};

/**
 * Utility functions for all demos
 */
namespace DemoUtils {
    // Convert motor command to string
    const char* motorCommandToString(MotorCommand cmd);
    
    // Create configuration from DemoConfig
    std::shared_ptr<Config> createConfig(const DemoConfig& config);
    
    // Setup world with common parameters
    void setupWorld(SimpleWorld& world, float width, float height, 
                   size_t visionWidth, size_t visionHeight, int64_t seed = 42);
    
    // Setup agent brain with world
    AgentBrain setupAgentBrain(std::shared_ptr<Brain> brain, SimpleWorld& world,
                             const DemoConfig& config);
    
    // Common main loop execution
    void runCommonLoop(AgentBrain& agentBrain, SimpleWorld& world,
                      std::shared_ptr<Brain> brain, 
                      int numSteps, int numEpisodes = 1,
                      bool enableInstrumentation = false,
                      const std::string& instrumentVar = "");
    
    // Get phase name from episode number
    std::string getPhaseName(int phaseNumber);
    
    // Parse command line arguments
    bool parseArgs(int argc, char* argv[], DemoConfig& config,
                   int& numEpisodes, int& stepsPerEpisode);
}

/**
 * Base demo class for extensible demo system
 */
class Demo {
protected:
    DemoConfig config;
    std::shared_ptr<Logger> logger;
    
public:
    Demo(const DemoConfig& cfg) : config(cfg) {
        logger = Logger::getGlobal();
        logger->setLevel(LogLevel::Info);
    }
    
    virtual ~Demo() = default;
    
    // Main demo entry point
    virtual void run(int numEpisodes, int stepsPerEpisode) = 0;
    
    // Get configuration
    const DemoConfig& getConfig() const { return config; }
    
    // Utility methods
    void printHeader(const std::string& title) const;
    void printSection(const std::string& title) const;
    void printResult(const std::string& name, float value, float max = 1.0f) const;
};

/**
 * Phase 3 specific demo implementation
 */
class Phase3Demo : public Demo {
public:
    Phase3Demo(const DemoConfig& cfg) : Demo(cfg) {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) override;
};

/**
 * Phase 4 specific demo implementation
 */
class Phase4Demo : public Demo {
public:
    Phase4Demo(const DemoConfig& cfg) : Demo(cfg) {}
    
    void run(int numTrials = 50) override;
};

/**
 * Phase 5 specific demo implementation
 */
class Phase5Demo : public Demo {
public:
    Phase5Demo(const DemoConfig& cfg) : Demo(cfg) {}
    
    void run() override;
};

/**
 * Phase 6 specific demo implementation
 */
class Phase6Demo : public Demo {
public:
    Phase6Demo(const DemoConfig& cfg) : Demo(cfg) {}
    
    void run() override;
};

/**
 * Unified demo runner that can execute any phase
 */
class DemoRunner {
public:
    static void runDemo(int phase, const std::string& configFile = "configs/default.cfg");
    static void runBenchmark(size_t neurons, size_t steps);
    static DemoConfig loadConfigFromFile(const std::string& filename);
    static void printAvailablePhases();
};

} // namespace nlm
