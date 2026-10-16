// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This phase implements the complete integrated artificial brain with:
// - Working memory with persistent activity and competition
// - Episodic memory with experience encoding and replay
// - Associative memory with Hebbian pattern associations
// - All memory systems connected to neural processing
// - Neuromodulation integration (dopamine, curiosity, prediction error, novelty)
// - Prediction system integrated into brain loop
// - Neural planning with action sequence evaluation
// - Concept formation from experience patterns
// - Attention with competitive selection dynamics
// - Developmental stages affecting plasticity rates
// - Structural plasticity modulated by age
// - Neural excitability changes with development
// - Checkpoint save/load implementation
// - Brain state serialization
// - Episodic memory replay during simulation
// - Memory consolidation for important episodes
// - Integration with sleep/rest cycle

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <cctype>
#include <algorithm>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     Complete integrated artificial brain.                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printHelp() {
    std::cout << "NLM Phase 6 Demo - Integrated Artificial Brain" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
    std::cout << "Run the Phase 6 integration experiment to verify that all" << std::endl;
    std::cout << "brain systems are properly connected and working together." << std::endl << std::endl;
    
    std::cout << "Usage: nlm_phase6_demo [OPTIONS]" << std::endl << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help, -h              Show this help message" << std::endl;
    std::cout << "  --config <file>         Load configuration from file" << std::endl;
    std::cout << "  --neurons <count>       Set number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <count>         Set max simulation steps (default: 2000)" << std::endl;
    std::cout << "  --no-checkpoint         Disable checkpointing" << std::endl;
    std::cout << "  --no-replay             Disable replay system" << std::endl;
    std::cout << "  --no-development        Disable development system" << std::endl;
    std::cout << "  --checkpoint-path <path> Set checkpoint file path" << std::endl;
    std::cout << "  --log-level <level>     Set log level (DEBUG, INFO, WARN, ERROR)" << std::endl;
    std::cout << "  --visualization, --viz Enable visualization output" << std::endl;
    std::cout << "  --visualization-file <file> Set visualization output file" << std::endl;
    std::cout << "  --visualization-update-rate <rate> Set visualization update rate (Hz)" << std::endl << std::endl;
    
    std::cout << "The demo runs:" << std::endl;
    std::cout << "1. Quick integration verification" << std::endl;
    std::cout << "2. Individual system integration tests" << std::endl;
    std::cout << "3. Full integration experiment with replay" << std::endl << std::endl;
}

// Simple argument parsing helper functions
std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) start++;
    while (end > start && std::isspace(str[end - 1])) end--;
    
    return str.substr(start, end - start);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void parseArguments(int argc, char** argv, 
                   std::string& configFile,
                   size_t& neurons,
                   size_t& steps,
                   bool& enableCheckpointing,
                   bool& enableReplay,
                   bool& enableDevelopment,
                   std::string& checkpointPath,
                   std::string& logLevel,
                   bool& showHelp,
                   bool& enableVisualization,
                   std::string& visualizationFile,
                   int& visualizationUpdateRate) {
    // Set defaults
    configFile = "configs/default.cfg";
    neurons = 500;
    steps = 2000;
    enableCheckpointing = true;
    enableReplay = true;
    enableDevelopment = true;
    checkpointPath = "./checkpoint_test.bin";
    logLevel = "INFO";
    showHelp = false;
    enableVisualization = false;
    visualizationFile = "";
    visualizationUpdateRate = 30;
    
    // Parse arguments manually
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::string argLower = toLower(arg);
        
        if (argLower == "--help" || argLower == "-h") {
            showHelp = true;
        } else if (argLower.find("--config") == 0) {
            if (argLower.find("=") != std::string::npos) {
                configFile = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (argLower.find("--neurons") == 0) {
            if (argLower.find("=") != std::string::npos) {
                try {
                    neurons = std::stoul(arg.substr(arg.find("=") + 1));
                } catch (...) {
                    // Use default if invalid
                }
            } else if (i + 1 < argc) {
                try {
                    neurons = std::stoul(argv[++i]);
                } catch (...) {
                    // Use default if invalid
                }
            }
        } else if (argLower.find("--steps") == 0) {
            if (argLower.find("=") != std::string::npos) {
                try {
                    steps = std::stoul(arg.substr(arg.find("=") + 1));
                } catch (...) {
                    // Use default if invalid
                }
            } else if (i + 1 < argc) {
                try {
                    steps = std::stoul(argv[++i]);
                } catch (...) {
                    // Use default if invalid
                }
            }
        } else if (argLower == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (argLower == "--no-replay") {
            enableReplay = false;
        } else if (argLower == "--no-development") {
            enableDevelopment = false;
        } else if (argLower.find("--checkpoint-path") == 0) {
            if (argLower.find("=") != std::string::npos) {
                checkpointPath = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        } else if (argLower.find("--log-level") == 0) {
            if (argLower.find("=") != std::string::npos) {
                logLevel = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                logLevel = argv[++i];
            }
        } else if (argLower == "--visualization" || argLower == "--viz") {
            enableVisualization = true;
        } else if (argLower.find("--visualization-file") == 0 || argLower.find("--viz-file") == 0) {
            if (argLower.find("=") != std::string::npos) {
                visualizationFile = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                visualizationFile = argv[++i];
            }
        } else if (argLower.find("--visualization-update-rate") == 0 || argLower.find("--viz-update-rate") == 0) {
            if (argLower.find("=") != std::string::npos) {
                try {
                    visualizationUpdateRate = std::stoi(arg.substr(arg.find("=") + 1));
                } catch (...) {
                    // Use default if invalid
                }
            } else if (i + 1 < argc) {
                try {
                    visualizationUpdateRate = std::stoi(argv[++i]);
                } catch (...) {
                    // Use default if invalid
                }
            }
        }
    }
}

void logArguments(const Phase6Config& config, bool enableVisualization,
                  const std::string& visualizationFile, int visualizationUpdateRate) {
    NLM_LOG_INFO("Phase 6 Configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config.neuronCount));
    NLM_LOG_INFO("  Max Steps: " + std::to_string(config.maxSteps));
    NLM_LOG_INFO("  Checkpointing: " + std::string(config.enableCheckpointing ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Replay: " + std::string(config.enableReplay ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Development: " + std::string(config.enableDevelopment ? "Enabled" : "Disabled"));
    NLM_LOG_INFO("  Checkpoint Path: " + config.checkpointPath);
    
    // Log visualization configuration if enabled
    if (enableVisualization) {
        NLM_LOG_INFO("  Visualization: Enabled");
        if (!visualizationFile.empty()) {
            NLM_LOG_INFO("  Visualization File: " + visualizationFile);
        }
        NLM_LOG_INFO("  Visualization Update Rate: " + std::to_string(visualizationUpdateRate) + " Hz");
    } else {
        NLM_LOG_INFO("  Visualization: Disabled");
    }
}

int main(int argc, char** argv) {
    // Parse command line arguments using manual parsing (no third-party dependencies)
    std::string configFile = "configs/default.cfg";
    size_t neurons = 500;
    size_t steps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    std::string logLevel = "INFO";
    bool showHelp = false;
    
    // Manual argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::string argLower = arg;
        std::transform(argLower.begin(), argLower.end(), argLower.begin(), ::tolower);
        
        if (argLower == "--help" || argLower == "-h") {
            showHelp = true;
        } else if (argLower.find("--config") == 0) {
            if (argLower.find("=") != std::string::npos) {
                configFile = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (argLower.find("--neurons") == 0) {
            if (argLower.find("=") != std::string::npos) {
                try {
                    neurons = std::stoul(arg.substr(arg.find("=") + 1));
                } catch (...) {
                    // Use default if invalid
                }
            } else if (i + 1 < argc) {
                try {
                    neurons = std::stoul(argv[++i]);
                } catch (...) {
                    // Use default if invalid
                }
            }
        } else if (argLower.find("--steps") == 0) {
            if (argLower.find("=") != std::string::npos) {
                try {
                    steps = std::stoul(arg.substr(arg.find("=") + 1));
                } catch (...) {
                    // Use default if invalid
                }
            } else if (i + 1 < argc) {
                try {
                    steps = std::stoul(argv[++i]);
                } catch (...) {
                    // Use default if invalid
                }
            }
        } else if (argLower == "--no-checkpoint") {
            enableCheckpointing = false;
        } else if (argLower == "--no-replay") {
            enableReplay = false;
        } else if (argLower == "--no-development") {
            enableDevelopment = false;
        } else if (argLower.find("--checkpoint-path") == 0) {
            if (argLower.find("=") != std::string::npos) {
                checkpointPath = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                checkpointPath = argv[++i];
            }
        } else if (argLower.find("--log-level") == 0) {
            if (argLower.find("=") != std::string::npos) {
                logLevel = arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc) {
                logLevel = argv[++i];
            }
        }
    }

    // Show help if requested
    if (showHelp) {
        printHelp();
        return 0;
    }

    printBanner();
    
    std::cout << "NLM Phase 6 Integration Demo" << std::endl;
    std::cout << "Testing the complete integrated artificial brain..." << std::endl << std::endl;

    // Initialize logger with appropriate level
    auto logger = std::make_shared<Logger>();
    LogLevel level = LogLevel::Info;
    std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::toupper);
    if (logLevel == "DEBUG") level = LogLevel::Debug;
    else if (logLevel == "WARN") level = LogLevel::Warn;
    else if (logLevel == "ERROR") level = LogLevel::Error;
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    NLM_LOG_INFO("=== NLM Phase 6: Final Integration Demo ===");
    NLM_LOG_INFO("Running comprehensive integration test of all brain systems:");
    NLM_LOG_INFO("  - Working memory integration");
    NLM_LOG_INFO("  - Episodic memory integration");
    NLM_LOG_INFO("  - Neuromodulation integration");
    NLM_LOG_INFO("  - Prediction system integration");
    NLM_LOG_INFO("  - Development system integration");
    NLM_LOG_INFO("  - Checkpoint system functionality");
    NLM_LOG_INFO("  - Replay and consolidation system");
    NLM_LOG_INFO("");

    // Load configuration
    auto config = std::make_shared<Config>();
    
    // Try to load from file
    if (config->loadFromFile(configFile)) {
        NLM_LOG_INFO("Loaded configuration from: " + configFile);
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Override with command line args (simplified)
    config->loadFromArgs(argc, argv);

    // Set default values for Phase 6
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);

    // Create Phase 6 experiment
    Phase6IntegratedExperiment experiment;

    // Configure the experiment
    Phase6Config phase6Config;
    phase6Config.neuronCount = neurons;
    phase6Config.maxSteps = steps;
    phase6Config.enableCheckpointing = enableCheckpointing;
    phase6Config.enableReplay = enableReplay;
    phase6Config.enableDevelopment = enableDevelopment;
    phase6Config.checkpointPath = checkpointPath;

    // Log configuration
    logArguments(phase6Config);

    NLM_LOG_INFO("");

    // Run quick integration verification first
    std::cout << "=== Quick Integration Verification ===" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    std::cout << std::endl;
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Integration verification passed!" << std::endl << std::endl;

    // Test individual systems
    std::cout << "=== Individual System Integration Tests ===" << std::endl;
    
    std::cout << "--- Memory Integration Test ---" << std::endl;
    bool memoryOK = experiment.testMemoryIntegration();
    std::cout << (memoryOK ? "✓ Memory systems integrated" : "✗ Memory systems failed") << std::endl << std::endl;
    
    std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
    bool neuromodulationOK = experiment.testNeuromodulationIntegration();
    std::cout << (neuromodulationOK ? "✓ Neuromodulation systems integrated" : "✗ Neuromodulation systems failed") << std::endl << std::endl;
    
    std::cout << "--- Checkpoint System Test ---" << std::endl;
    bool checkpointOK = experiment.testCheckpointing();
    std::cout << (checkpointOK ? "✓ Checkpoint system working" : "✗ Checkpoint system failed") << std::endl << std::endl;
    
    std::cout << "--- Replay System Test ---" << std::endl;
    bool replayOK = experiment.testReplay();
    std::cout << (replayOK ? "✓ Replay system working" : "✗ Replay system failed") << std::endl << std::endl;

    // Run full integration experiment
    std::cout << "=== Full Integration Experiment ===" << std::endl;
    std::cout << "Running complete integration test with replay and development..." << std::endl << std::endl;
    
    auto result = experiment.run(phase6Config);

    // Display final results
    std::cout << std::endl;
    std::cout << "=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Average synaptic weight: " << result.avgSynapticWeight << std::endl;
    std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Novelty level: " << result.noveltyLevel << std::endl;
    std::cout << "Curiosity level: " << result.curiosityLevel << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "";
    
    std::cout << "=== INTEGRATION STATUS ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "✓ CONNECTED" : "✗ DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "Replay System: " << (result.replayWorks ? "✓ WORKING" : "✗ NOT WORKING") << std::endl;
    std::cout << "";

    std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;

    // Summary
    std::cout << std::endl;
    std::cout << "=== PHASE 6 INTEGRATION STATUS ===" << std::endl;
    
    bool allTestsPassed = integrationOK && memoryOK && neuromodulationOK && checkpointOK && replayOK;
    bool allSystemsConnected = result.memoryWorkingMemoryIntegrated && result.memoryEpisodicMemoryIntegrated && 
                               result.neuromodulationIntegrated && result.predictionIntegrated && result.developmentIntegrated;
    
    if (allTestsPassed && allSystemsConnected && result.checkpointingWorks && result.replayWorks) {
        std::cout << "✓✓✓ ALL TESTS PASSED ✓✓✓" << std::endl;
        std::cout << "✓ Phase 6 integration successful!" << std::endl;
        std::cout << "✓ All brain systems are properly connected and working together." << std::endl;
        std::cout << "✓ The NLM brain is a functioning artificial neural substrate." << std::endl;
        std::cout << "✓ The system can learn and adapt through experience." << std::endl;
    } else {
        std::cout << "✗ SOME TESTS FAILED ✗" << std::endl;
        std::cout << "  Integration verification: " << (integrationOK ? "PASS" : "FAIL") << std::endl;
        std::cout << "  System integration: " << (allSystemsConnected ? "PASS" : "FAIL") << std::endl;
        std::cout << "  Checkpoint system: " << (result.checkpointingWorks ? "PASS" : "FAIL") << std::endl;
        std::cout << "  Replay system: " << (result.replayWorks ? "PASS" : "FAIL") << std::endl;
        return 1;
    }

    return 0;
}