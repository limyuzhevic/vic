// NLM Phase 6 Command Line Interface
// Enhanced command-line interface for NLM Phase 6 experiments

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <chrono>

using namespace nlm;

class NLMPhase6CLI {
public:
    NLMPhase6CLI() {
        experiment = std::make_unique<Phase6IntegratedExperiment>();
        setupCommands();
    }
    
    void run(int argc, char* argv[]) {
        parseArguments(argc, argv);
        
        if (showHelp || helpRequested) {
            printHelp();
            return;
        }
        
        if (versionRequested) {
            printVersion();
            return;
        }
        
        if (integrationOnly) {
            runIntegrationTest();
            return;
        }
        
        if (memoryOnly) {
            runMemoryTest();
            return;
        }
        
        if (neuromodulationOnly) {
            runNeuromodulationTest();
            return;
        }
        
        if (checkpointOnly) {
            runCheckpointTest();
            return;
        }
        
        if (replayOnly) {
            runReplayTest();
            return;
        }
        
        // Run full experiment
        runFullExperiment();
    }
    
private:
    std::unique_ptr<Phase6IntegratedExperiment> experiment;
    
    // Configuration
    bool showHelp = false;
    bool helpRequested = false;
    bool versionRequested = false;
    bool integrationOnly = false;
    bool memoryOnly = false;
    bool neuromodulationOnly = false;
    bool checkpointOnly = false;
    bool replayOnly = false;
    
    // Experiment configuration
    Phase6Config config;
    
    // Command registry
    std::map<std::string, std::string> commands;
    
    void setupCommands() {
        commands["--help"] = "Show help information";
        commands["--version"] = "Show version information";
        commands["--integration"] = "Run integration verification only";
        commands["--memory"] = "Run memory integration test only";
        commands["--neuromodulation"] = "Run neuromodulation integration test only";
        commands["--checkpoint"] = "Run checkpoint test only";
        commands["--replay"] = "Run replay test only";
        commands["--neurons"] = "Number of neurons (default: 500)";
        commands["--steps"] = "Number of simulation steps (default: 2000)";
        commands["--regions"] = "Number of neural regions (default: 2)";
        commands["--checkpoint-dir"] = "Checkpoint directory (default: ./checkpoints)";
        commands["--checkpoint-file"] = "Checkpoint file path";
        commands["--quiet"] = "Suppress informational messages";
        commands["--verbose"] = "Enable verbose output";
        commands["--config"] = "Load configuration from file";
    }
    
    void parseArguments(int argc, char* argv[]) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
            } else if (arg == "--version") {
                versionRequested = true;
            } else if (arg == "--integration") {
                integrationOnly = true;
            } else if (arg == "--memory") {
                memoryOnly = true;
            } else if (arg == "--neuromodulation") {
                neuromodulationOnly = true;
            } else if (arg == "--checkpoint") {
                checkpointOnly = true;
            } else if (arg == "--replay") {
                replayOnly = true;
            } else if (arg == "--quiet") {
                Logger::getGlobal().setLevel(LogLevel::Error);
            } else if (arg == "--verbose") {
                Logger::getGlobal().setLevel(LogLevel::Debug);
            } else if (arg.find("--neurons=") == 0) {
                config.neuronCount = std::stoi(arg.substr(10));
            } else if (arg.find("--steps=") == 0) {
                config.maxSteps = std::stoi(arg.substr(8));
            } else if (arg.find("--regions=") == 0) {
                config.regionCount = std::stoi(arg.substr(10));
            } else if (arg.find("--checkpoint-dir=") == 0) {
                config.checkpointPath = arg.substr(18);
            } else if (arg.find("--config=") == 0) {
                std::string configFile = arg.substr(9);
                loadConfigFile(configFile);
            } else {
                // Unknown argument, ignore but print warning in verbose mode
                if (Logger::getGlobal().isEnabled(LogLevel::Warning)) {
                    std::cerr << "Warning: Unknown argument: " << arg << std::endl;
                }
            }
        }
    }
    
    void loadConfigFile(const std::string& filename) {
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(filename)) {
            if (cfg->has("neuron_count")) {
                config.neuronCount = cfg->getOr<size_t>("neuron_count", config.neuronCount);
            }
            if (cfg->has("max_steps")) {
                config.maxSteps = cfg->getOr<size_t>("max_steps", config.maxSteps);
            }
            if (cfg->has("region_count")) {
                config.regionCount = cfg->getOr<size_t>("region_count", config.regionCount);
            }
            if (cfg->has("connection_probability")) {
                config.connectionProbability = cfg->getOr<float>("connection_probability", config.connectionProbability);
            }
            if (cfg->has("checkpoint_path")) {
                config.checkpointPath = cfg->getOr<std::string>("checkpoint_path", config.checkpointPath);
            }
        } else {
            std::cerr << "Warning: Could not load config file: " << filename << std::endl;
        }
    }
    
    void printHelp() {
        std::cout << "=== NLM Phase 6 Command Line Interface ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm_phase6 [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        for (const auto& pair : commands) {
            std::cout << "  " << pair.first << " - " << pair.second << std::endl;
        }
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_phase6 --integration" << std::endl;
        std::cout << "  nlm_phase6 --neurons=1000 --steps=5000" << std::endl;
        std::cout << "  nlm_phase6 --config=experiment.cfg --checkpoint-dir=./results" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    }
    
    void printVersion() {
        std::cout << "=== NLM Phase 6 ===" << std::endl;
        std::cout << "Version: 6.0.0 (Final Integration)" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 6 Achievements:" << std::endl;
        std::cout << "  ✓ All brain systems integrated (memory, neuromodulation, prediction, cognition)" << std::endl;
        std::cout << "  ✓ Phase 6 real neural computation with biological realism" << std::endl;
        std::cout << "  ✓ Checkpoint save/load functionality" << std::endl;
        std::cout << "  ✓ Memory replay and consolidation" << std::endl;
        std::cout << "  ✓ Developmental system with plasticity modulation" << std::endl;
    }
    
    void runIntegrationTest() {
        std::cout << "=== Phase 6 Integration Verification ===" << std::endl;
        
        bool result = experiment->verifyIntegration();
        
        if (result) {
            std::cout << "✓ Integration verification PASSED" << std::endl;
        } else {
            std::cout << "✗ Integration verification FAILED" << std::endl;
            exit(1);
        }
    }
    
    void runMemoryTest() {
        std::cout << "=== Memory Integration Test ===" << std::endl;
        
        bool result = experiment->testMemoryIntegration();
        
        if (result) {
            std::cout << "✓ Memory integration test PASSED" << std::endl;
        } else {
            std::cout << "✗ Memory integration test FAILED" << std::endl;
            exit(1);
        }
    }
    
    void runNeuromodulationTest() {
        std::cout << "=== Neuromodulation Integration Test ===" << std::endl;
        
        bool result = experiment->testNeuromodulationIntegration();
        
        if (result) {
            std::cout << "✓ Neuromodulation integration test PASSED" << std::endl;
        } else {
            std::cout << "✗ Neuromodulation integration test FAILED" << std::endl;
            exit(1);
        }
    }
    
    void runCheckpointTest() {
        std::cout << "=== Checkpoint Test ===" << std::endl;
        
        bool result = experiment->testCheckpointing();
        
        if (result) {
            std::cout << "✓ Checkpoint test PASSED" << std::endl;
        } else {
            std::cout << "✗ Checkpoint test FAILED" << std::endl;
            exit(1);
        }
    }
    
    void runReplayTest() {
        std::cout << "=== Replay Test ===" << std::endl;
        
        bool result = experiment->testReplay();
        
        if (result) {
            std::cout << "✓ Replay test PASSED" << std::endl;
        } else {
            std::cout << "✗ Replay test FAILED" << std::endl;
            exit(1);
        }
    }
    
    void runFullExperiment() {
        std::cout << "=== Phase 6 Full Integration Experiment ===" << std::endl;
        std::cout << "Configuration: " << config.neuronCount << " neurons, " 
                  << config.maxSteps << " steps, " << config.regionCount << " regions" << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Phase6IntegrationResult result = experiment->run(config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
        std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Execution time: " << duration.count() << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        if (!result.memoryWorkingMemoryIntegrated || !result.memoryEpisodicMemoryIntegrated || 
            !result.neuromodulationIntegrated || !result.predictionIntegrated || 
            !result.developmentIntegrated || !result.checkpointingWorks) {
            std::cout << "⚠️  Some systems are not properly integrated!" << std::endl;
            exit(1);
        }
        
        std::cout << "✓ All systems are properly integrated! Phase 6 complete!" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    NLMPhase6CLI cli;
    cli.run(argc, argv);
    return 0;
}