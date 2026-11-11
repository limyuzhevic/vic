#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Metrics.hpp"
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {

// Standard command-line options structure
struct DemoOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    bool verbose = true;
    bool recordMetrics = true;
    bool showHelp = false;
    int seed = 42;
    float timestep = 0.001f;
    
    // Common demo parameters
    int numEpisodes = 1;
    int stepsPerEpisode = 100;
    int maxNeurons = 500;
    int maxSynapses = 500;
    
    // Phase-specific parameters
    int param1 = 0;
    float param2 = 0.0f;
    std::string param3 = "";
};

// Standardized argument parser with help support
class CommandLineParser {
public:
    static DemoOptions parse(int argc, char* argv[], const char* programName);
    static void printHelp(const char* programName);
    static std::string getUsageString(const char* programName);
private:
    static void printBanner();
    static std::string trim(const std::string& str);
};

// Base demo framework class
class DemoFramework {
public:
    DemoFramework(const std::string& demoName, const std::string& demoDescription);
    virtual ~DemoFramework() = default;
    
    // Main entry point
    int run(int argc, char* argv[]);
    
    // Public interface for derived classes
    virtual bool initialize(const DemoOptions& options) = 0;
    virtual void cleanup() {}
    virtual bool runDemo(const DemoOptions& options) = 0;
    virtual void printResults(const DemoOptions& options) const = 0;
    
    // Common helper methods
    void setupLogging(bool verbose = true);
    void setupConfiguration(const DemoOptions& options);
    void setupBrain(std::shared_ptr<Config> config);
    void setupWorld(std::shared_ptr<SimpleWorld> world);
    void setupAgent(std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world);
    
    // Metrics and timing
    void recordMetric(const std::string& name, double value);
    void printMetricsSummary() const;
    double getElapsedTime() const;
    
    // Utility methods
    void printHeader(const std::string& title) const;
    void printSection(const std::string& title) const;
    void printResult(const std::string& name, double value, double max = 1.0) const;
    void printProgress(int step, int total, float progress) const;
    
    // Configuration helpers
    void applyStandardConfig(std::shared_ptr<Config> config, const DemoOptions& options);
    void applyPhaseConfig(std::shared_ptr<Config> config, int phase);
    
    // Validation
    bool validateOptions(const DemoOptions& options) const;
    
protected:
    std::string demoName_;
    std::string demoDescription_;
    
    // Common objects
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    
    // Metrics and timing
    Metrics metrics_;
    std::chrono::steady_clock::time_point startTime_;
    double elapsedTime_;
    
    // Runtime state
    bool initialized_ = false;
    bool running_ = false;
};

} // namespace nlm

// Include implementation
#include "DemoFramework.cpp"