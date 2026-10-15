/**
 * Phase 6 Demo - Integration Test
 * 
 * This demo runs the Phase 6 integration experiment to verify
 * that all brain systems are properly connected.
 */

#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <chrono>
#include <cmath>
#include <sstream>
#include <map>
#include <string>
#include <vector>

using namespace nlm;

// Constants for NLM
namespace nlm_constants {
    const std::string VERSION = "6.0.0";
    const std::string PROJECT_NAME = "NLM — 熙然 (Neural Learning Machine)";
    const std::string PHASE_NAME = "Phase 6: Final Integration";
    const std::string PHASE_DESCRIPTION = "Final integration of all brain systems into a coherent artificial brain.";
}

// CLI parsing utilities
namespace cli_utils {
    
    struct CLIArgs {
        bool help = false;
        bool version = false;
        bool listConfigs = false;
        bool validateConfig = false;
        std::string configFile = "configs/default.cfg";
        int64_t neuronCount = 500;
        int64_t maxSteps = 2000;
        bool enableCheckpointing = true;
        bool enableReplay = true;
        bool enableDevelopment = true;
        bool enableDebug = false;
        bool enableVerbose = false;
        std::string outputFile = "";
        std::string checkpointFile = "";
        bool runQuickTests = false;
        bool runFullExperiment = true;
        int benchmarkMode = 0;  // 0=normal, 1=quick, 2=ultra-quick
        double timestep = 0.001;
        std::vector<std::string> customArgs;
    };
    
    CLIArgs parseCLIArgs(int argc, char* argv[]) {
        CLIArgs args;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--version" || arg == "-v") {
                args.version = true;
            } else if (arg == "--list-configs") {
                args.listConfigs = true;
            } else if (arg == "--validate-config") {
                args.validateConfig = true;
            } else if (arg == "--neuron-count" || arg == "-nc") {
                if (i + 1 < argc) args.neuronCount = std::stoll(argv[++i]);
            } else if (arg == "--max-steps" || arg == "-ms") {
                if (i + 1 < argc) args.maxSteps = std::stoll(argv[++i]);
            } else if (arg == "--checkpoint" || arg == "-ck") {
                args.enableCheckpointing = true;
                if (i + 1 < argc) args.checkpointFile = argv[++i];
            } else if (arg == "--no-checkpoint") {
                args.enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                args.enableReplay = false;
            } else if (arg == "--no-development") {
                args.enableDevelopment = false;
            } else if (arg == "--benchmark" || arg == "-b") {
                if (i + 1 < argc) {
                    std::string mode = argv[++i];
                    if (mode == "quick") args.benchmarkMode = 1;
                    else if (mode == "ultra") args.benchmarkMode = 2;
                    else args.benchmarkMode = 0;
                }
            } else if (arg == "--quick" || arg == "-q") {
                args.runQuickTests = true;
                args.runFullExperiment = false;
            } else if (arg == "--full" || arg == "-f") {
                args.runQuickTests = false;
                args.runFullExperiment = true;
            } else if (arg == "--enable-debug" || arg == "-d") {
                args.enableDebug = true;
            } else if (arg == "--enable-verbose" || arg == "-vv") {
                args.enableVerbose = true;
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) args.outputFile = argv[++i];
            } else if (arg == "--config") {
                if (i + 1 < argc) args.configFile = argv[++i];
            } else if (arg == "--timestep" || arg == "-t") {
                if (i + 1 < argc) args.timestep = std::stod(argv[++i]);
            } else if (arg == "--" || arg == "-+" || arg == "!") {
                i++;
                while (i < argc) {
                    args.customArgs.push_back(argv[i]);
                    i++;
                }
                break;
            } else {
                args.customArgs.push_back(arg);
            }
        }
        
        return args;
    }
    
    void printHelp(const std::string& programName) {
        std::cout << "USAGE: " << programName << " [OPTIONS]" << std::endl << std::endl;
        std::cout << "NLM (Neural Learning Machine) - " << nlm_constants::PROJECT_NAME << std::endl;
        std::cout << "Phase: " << nlm_constants::PHASE_NAME << std::endl << std::endl;
        
        std::cout << "BASIC OPTIONS:" << std::endl;
        std::cout << "  -h, --help              Show this help message" << std::endl;
        std::cout << "  -v, --version           Show version information" << std::endl;
        std::cout << "  -nc, --neuron-count N   Number of neurons (default: 500)" << std::endl;
        std::cout << "  -ms, --max-steps S      Maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "  -t, --timestep T        Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --config FILE            Configuration file" << std::endl;
        std::cout << "  --checkpoint FILE        Checkpoint file path" << std::endl;
        std::cout << "  --no-checkpoint          Disable checkpointing" << std::endl;
        std::cout << "  --no-replay              Disable replay functionality" << std::endl;
        std::cout << "  --no-development         Disable development system" << std::endl;
        std::cout << "  -o, --output FILE        Save output to file" << std::endl;
        
        std::cout << std::endl << "ADVANCED OPTIONS:" << std::endl;
        std::cout << "  -d, --enable-debug      Enable debug logging" << std::endl;
        std::cout << "  -vv, --enable-verbose   Enable verbose output" << std::endl;
        std::cout << "  -b, --benchmark MODE     Benchmark mode: quick, ultra, or normal" << std::endl;
        std::cout << "  -q, --quick              Run quick integration tests only" << std::endl;
        std::cout << "  -f, --full              Run full integration experiment (default)" << std::endl;
        
        std::cout << std::endl << "EXPERIMENTAL FEATURES:" << std::endl;
        std::cout << "  --                             Use to pass arguments directly to configuration" << std::endl;
        
        std::cout << std::endl << "PHASE " << nlm_constants::PHASE_NAME << ":" << std::endl;
        std::cout << nlm_constants::PHASE_DESCRIPTION << std::endl;
        
        std::cout << std::endl << "EXAMPLES:" << std::endl;
        std::cout << "  " << programName << " --help" << std::endl;
        std::cout << "  " << programName << " --neuron-count 1000 --benchmark quick" << std::endl;
        std::cout << "  " << programName << " --quick --no-checkpoint" << std::endl;
        std::cout << "  " << programName << " --full --output results.txt" << std::endl;
    }
    
    void printVersion() {
        std::cout << nlm_constants::PROJECT_NAME << std::endl;
        std::cout << "Version: " << nlm_constants::VERSION << std::endl;
        std::cout << "Phase: " << nlm_constants::PHASE_NAME << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    }
    
    double getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration<double>(duration).count();
    }
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    auto args = cli_utils::parseCLIArgs(argc, argv);
    
    // Handle help/version requests immediately
    if (args.help) {
        cli_utils::printHelp("nlm_phase6_demo");
        return 0;
    }
    
    if (args.version) {
        cli_utils::printVersion();
        return 0;
    }
    
    if (args.listConfigs) {
        std::cout << "=== Phase 6 Configuration Options ===" << std::endl;
        std::cout << "neuronCount: Number of neurons (default: 500)" << std::endl;
        std::cout << "maxSteps: Maximum simulation steps (default: 2000)" << std::endl;
        std::cout << "enableCheckpointing: Enable checkpointing (default: true)" << std::endl;
        std::cout << "enableReplay: Enable replay functionality (default: true)" << std::endl;
        std::cout << "enableDevelopment: Enable development system (default: true)" << std::endl;
        return 0;
    }
    
    if (args.validateConfig) {
        std::cout << "=== Configuration Validation ===" << std::endl;
        std::cout << "Configuration validation would be performed here" << std::endl;
        return 0;
    }
    
    std::cout << "=== NLM Phase 6 Integration Demo ===" << std::endl;
    std::cout << "Testing the integrated artificial brain..." << std::endl << std::endl;
    
    // Initialize logging levels
    if (args.enableDebug) {
        Logger::getInstance().setLevel(Logger::Level::Debug);
        std::cout << "Debug logging enabled" << std::endl << std::endl;
    }
    
    if (args.enableVerbose) {
        std::cout << "Verbose logging enabled" << std::endl << std::endl;
    }
    
    // Create experiment
    Phase6IntegratedExperiment experiment;
    
    // Adjust settings for benchmark modes
    if (args.benchmarkMode == 1) {  // Quick benchmark
        std::cout << "QUICK BENCHMARK MODE: Reducing settings for faster testing" << std::endl << std::endl;
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 500;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto startTime = cli_utils::getCurrentTimestamp();
        auto result = experiment.run(config);
        double elapsed = cli_utils::getCurrentTimestamp() - startTime;
        
        std::cout << std::endl << "=== QUICK BENCHMARK RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Wall clock time: " << elapsed << "s" << std::endl;
        return 0;
    } else if (args.benchmarkMode == 2) {  // Ultra-quick benchmark
        std::cout << "ULTRA-QUICK BENCHMARK MODE: Minimal settings" << std::endl << std::endl;
        Phase6Config config;
        config.neuronCount = 50;
        config.maxSteps = 100;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        
        auto startTime = cli_utils::getCurrentTimestamp();
        auto result = experiment.run(config);
        double elapsed = cli_utils::getCurrentTimestamp() - startTime;
        
        std::cout << std::endl << "=== ULTRA-QUICK BENCHMARK RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Wall clock time: " << elapsed << "s" << std::endl;
        return 0;
    }
    
    // First, run the quick integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "Integration verification passed!" << std::endl << std::endl;
    
    // Test individual systems if not in quick mode
    if (args.runQuickTests) {
        std::cout << "--- Quick Integration Tests ---" << std::endl;
        experiment.testMemoryIntegration();
        std::cout << std::endl;
        experiment.testNeuromodulationIntegration();
        std::cout << std::endl;
        experiment.testCheckpointing();
        std::cout << std::endl;
        experiment.testReplay();
        std::cout << std::endl;
    } else {
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
    }
    
    // Run full experiment if requested
    if (args.runFullExperiment) {
        std::cout << "--- Full Integration Experiment ---" << std::endl;
        Phase6Config config;
        config.neuronCount = args.neuronCount;
        config.maxSteps = args.maxSteps;
        config.enableCheckpointing = args.enableCheckpointing;
        config.enableReplay = args.enableReplay;
        config.enableDevelopment = args.enableDevelopment;
        
        auto startTime = cli_utils::getCurrentTimestamp();
        auto result = experiment.run(config);
        double elapsed = cli_utils::getCurrentTimestamp() - startTime;
        
        std::cout << std::endl;
        std::cout << "=== FINAL RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Avg firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall clock time: " << elapsed << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}