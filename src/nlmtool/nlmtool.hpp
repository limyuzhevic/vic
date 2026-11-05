#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase5Demo.hpp"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace nlm {

// Command line tool for running NLM simulations
class NLMCommandLineTool {
public:
    NLMCommandLineTool() {}
    ~NLMCommandLineTool() {}
    
    int run(int argc, char** argv);
    
private:
    // Command line options
    std::string configFile = "configs/default.cfg";
    std::string mode = "demo"; // demo, benchmark, validate, interactive
    std::string demoType = "phase3"; // phase3, phase5, phase6
    int episodes = 5;
    int steps = 200;
    bool showHelp = false;
    bool validateOnly = false;
    std::string outputFile;
    bool enableVerbose = false;
    std::string checkpointFile;
    bool loadCheckpoint = false;
    bool saveCheckpoint = false;
    bool enablePerformanceMetrics = false;
    
    // Help text
    void printHelp();
    void printVersion();
    
    // Parsing
    bool parseArguments(int argc, char** argv);
    void validateArguments();
    
    // Mode handlers
    int runDemoMode();
    int runBenchmarkMode();
    int runValidationMode();
    int runInteractiveMode();
    
    // Helper functions
    std::shared_ptr<Config> loadConfig();
    void setupLogging(const std::shared_ptr<Config>& config);
    void runPhase3Demo(const std::shared_ptr<Config>& config);
    void runPhase5Demo(const std::shared_ptr<Config>& config);
    void runPhase6Demo(const std::shared_ptr<Config>& config);
    void validateConfig(const std::shared_ptr<Config>& config);
    void printConfigSummary(const std::shared_ptr<Config>& config);
    void saveCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& filename);
    bool loadCheckpoint(const std::shared_ptr<Brain>& brain, const std::string& filename);
    void printPerformanceMetrics(const std::shared_ptr<Brain>& brain, double startTime, double endTime);
    
    // File operations
    bool fileExists(const std::string& path);
    
    // Config validation
    bool validateConfigValues(const std::shared_ptr<Config>& config);
    
    // Demo shortcuts
    void runQuickDemo(const std::shared_ptr<Config>& config);
    void runDevelopmentDemo(const std::shared_ptr<Config>& config);
    void runBenchmarkDemo(const std::shared_ptr<Config>& config);
};

} // namespace nlm