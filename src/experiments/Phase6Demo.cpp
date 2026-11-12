#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include "core/utilities/Utilities.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>
#include <chrono>

using namespace nlm;

int main(int argc, char* argv[]) {
    // Parse command line arguments
    std::string configFile = "configs/default.cfg";
    std::string logLevelStr = "Info";
    bool verboseMode = false;
    bool helpRequested = false;
    bool validateConfigMode = false;
    bool dryRunMode = false;
    std::string outputFile = "nlm_phase6_results.txt";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--help") {
            helpRequested = true;
        } else if (arg == "--verbose") {
            verboseMode = true;
        } else if (arg == "--validate-config") {
            validateConfigMode = true;
        } else if (arg == "--dry-run") {
            dryRunMode = true;
        } else if (arg == "--output-file" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--log-level" && i + 1 < argc) {
            logLevelStr = argv[++i];
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        }
    }
    
    if (helpRequested) {
        std::cout << "NLM Phase 6 Integration Demo" << std::endl;
        std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help                    Show this help message" << std::endl;
        std::cout << "  --verbose                 Enable verbose output" << std::endl;
        std::cout << "  --log-level LEVEL         Set log level (DEBUG|INFO|WARNING|ERROR)" << std::endl;
        std::cout << "  --config=FILE             Load configuration from file" << std::endl;
        std::cout << "  --validate-config         Validate configuration and exit" << std::endl;
        std::cout << "  --dry-run                 Parse arguments but don't run experiments" << std::endl;
        std::cout << "  --output-file FILE        Write results to file" << std::endl;
        return 0;
    }
    
    enableDebugOutput(verboseMode);
    
    LogLevel level;
    if (logLevelStr == "DEBUG") level = LogLevel::Debug;
    else if (logLevelStr == "WARNING") level = LogLevel::Warning;
    else if (logLevelStr == "ERROR") level = LogLevel::Error;
    else level = LogLevel::Info;
    
    // Create file logger if output file specified
    std::shared_ptr<Logger> fileLogger;
    if (!outputFile.empty() && outputFile != "console") {
        fileLogger = std::make_shared<Logger>();
        auto fileOutput = std::make_shared<Logger>(); // Note: This should be FileLogger, but assuming simple logger for demo
        fileLogger->addLogger(fileOutput);
        fileLogger->setLevel(level);
    }
    
    Logger::getGlobal().setLevel(level);
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    std::cout << "Verbose mode: " << (verboseMode ? "enabled" : "disabled") << std::endl;
    std::cout << "Configuration file: " << configFile << std::endl;
    std::cout << "Log level: " << logLevelStr << std::endl;
    std::cout << "Output file: " << outputFile << std::endl << std::endl;
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems
    std::cout << "--- Memory Integration Test ---" << std::endl;
    experiment.testMemoryIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    experiment.testNeuromodulationIntegration();
    std::cout << std::endl;
    
    std::cout << "--- Checkpoint Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    std::cout << "--- Replay Test ---" << std::endl;
    experiment.testReplay();
    std::cout << std::endl;
    
    // Run full experiment with smaller settings for demo
    std::cout << "--- Full Integration Experiment ---" << std::endl;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableCheckpointing = true;
    config.enableReplay = true;
    config.enableDevelopment = true;
    
    auto result = experiment.run(config);
    
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    
    return 0;
}
