// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration Demo
//
// This is the main entry point for NLM with comprehensive phase selection.
// Supports running individual phases or all phases through CLI arguments.
// Based on Phase6Demo.cpp but with full CLI integration.

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase4Experiment.hpp"
#include "experiments/Phase3Experiment.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <filesystem>

using namespace nlm;

// NLM Version
constexpr const char* NLM_VERSION = "0.1.0";
constexpr const char* NLM_PHASE = "6";

// Configuration structure for CLI arguments
struct CLIConfig {
    bool verbose = false;
    bool quiet = false;
    std::string configFile = "configs/default.cfg";
    std::string outputDir = ".";
    bool showHelp = false;
    bool showVersion = false;
    std::string phase = "demo";  // phase: 3, 4, 5, 6, demo, all
    size_t neuronCount = 500;
    size_t maxSteps = 1000;
    size_t maxEpisodes = 5;
    bool enableDevelopment = true;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    float learningRate = 0.01f;
    float noveltyThreshold = 0.5f;
    float curiosityThreshold = 0.3f;
    std::unordered_map<std::string, std::string> customParams;
};

// Parse command line arguments
CLIConfig parseArguments(int argc, char** argv) {
    CLIConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help" || arg == "-h") {
            config.showHelp = true;
        } else if (arg == "--version" || arg == "-v") {
            config.showVersion = true;
        } else if (arg == "--verbose" || arg == "-V") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg == "--phase" || arg == "-p") {
            if (i + 1 < argc) {
                config.phase = argv[++i];
            }
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            }
        } else if (arg == "--output-dir" || arg == "-o") {
            if (i + 1 < argc) {
                config.outputDir = argv[++i];
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                config.neuronCount = std::stoul(argv[++i]);
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                config.maxSteps = std::stoul(argv[++i]);
            }
        } else if (arg == "--episodes" || arg == "-e") {
            if (i + 1 < argc) {
                config.maxEpisodes = std::stoul(argv[++i]);
            }
        } else if (arg == "--no-development") {
            config.enableDevelopment = false;
        } else if (arg == "--no-checkpoint") {
            config.enableCheckpointing = false;
        } else if (arg == "--no-replay") {
            config.enableReplay = false;
        } else if (arg == "--lr" || arg == "--learning-rate") {
            if (i + 1 < argc) {
                config.learningRate = std::stof(argv[++i]);
            }
        } else if (arg == "--novelty" || arg == "--novelty-threshold") {
            if (i + 1 < argc) {
                config.noveltyThreshold = std::stof(argv[++i]);
            }
        } else if (arg == "--curiosity" || arg == "--curiosity-threshold") {
            if (i + 1 < argc) {
                config.curiosityThreshold = std::stof(argv[++i]);
            }
        } else if (arg.find("--") == 0) {
            std::string param = arg.substr(2);
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                config.customParams[param] = argv[++i];
            }
        } else {
            // If it's not a flag, treat as positional (could be phase)
            if (config.phase == "demo" && arg.find_first_not_of("0123456789") == std::string::npos) {
                // It's a number, treat as phase
                config.phase = arg;
            }
        }
    }
    
    return config;
}

// Print help information
void printHelp() {
    std::cout << "NLM — 熙然 (Neural Learning Machine)" << std::endl;
    std::cout << "Phase 6: Final Integration Demo" << std::endl;
    std::cout << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  ./nlm [OPTIONS] [PHASE]" << std::endl;
    std::cout << std::endl;
    
    std::cout << "PHASE OPTIONS:" << std::endl;
    std::cout << "  3_demo      Run Phase 3 (World Interaction) demo" << std::endl;
    std::cout << "  4_demo      Run Phase 4 (Emerging Cognition) demo" << std::endl;
    std::cout << "  5_demo      Run Phase 5 (Lifetime Integration) demo" << std::endl;
    std::cout << "  6_demo      Run Phase 6 (Final Integration) demo" << std::endl;
    std::cout << "  demo        Run Phase 6 demo (default)" << std::endl;
    std::cout << "  all          Run all phases sequentially" << std::endl;
    std::cout << std::endl;
    
    std::cout << "GLOBAL OPTIONS:" << std::endl;
    std::cout << "  --help, -h            Show this help message" << std::endl;
    std::cout << "  --version, -v         Show version information" << std::endl;
    std::cout << "  --verbose, -V         Enable verbose output" << std::endl;
    std::cout << "  --quiet, -q           Quiet mode (minimal output)" << std::endl;
    std::cout << "  --config <file>, -c   Load configuration from file" << std::endl;
    std::cout << "  --output-dir <dir>, -o   Set output directory" << std::endl;
    std::cout << std::endl;
    
    std::cout << "CONFIGURATION OPTIONS:" << std::endl;
    std::cout << "  --neurons <n>, -n     Number of neurons (default: 500)" << std::endl;
    std::cout << "  --steps <n>, -s       Maximum simulation steps (default: 1000)" << std::endl;
    std::cout << "  --episodes <n>, -e    Maximum episodes (default: 5)" << std::endl;
    std::cout << "  --no-development       Disable developmental system" << std::endl;
    std::cout << "  --no-checkpoint        Disable checkpointing" << std::endl;
    std::cout << "  --no-replay            Disable replay system" << std::endl;
    std::cout << "  --learning-rate <r>    Learning rate (default: 0.01)" << std::endl;
    std::cout << "  --novelty-threshold <t> Novelty detection threshold (default: 0.5)" << std::endl;
    std::cout << "  --curiosity-threshold <t> Curiosity threshold (default: 0.3)" << std::endl;
    std::cout << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  ./nlm                    Run Phase 6 demo" << std::endl;
    std::cout << "  ./nlm 3_demo --neurons 1000 --steps 5000" << std::endl;
    std::cout << "  ./nlm all --output-dir results/" << std::endl;
    std::cout << "  ./nlm 6_demo -v -c my_config.cfg" << std::endl;
    std::cout << std::endl;
    
    std::cout << "NLM Version " << NLM_VERSION << " (Phase " << NLM_PHASE << ")" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM " << NLM_VERSION << " (Phase " << NLM_PHASE << ")" << std::endl;
    std::cout << "Neural Learning Machine — 熙然" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 focuses on integrating all existing systems into a coherent" << std::endl;
    std::cout << "artificial brain. Previous phases built individual components; Phase 6" << std::endl;
    std::cout << "ensures they work together as a unified system." << std::endl;
    std::cout << std::endl;
}

// Print banner
void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Final Integration                                ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

// Format elapsed time
std::string formatElapsedTime(double seconds) {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    } else {
        int hours = static_cast<int>(seconds) / 3600;
        int minutes = (static_cast<int>(seconds) % 3600) / 60;
        int secs = static_cast<int>(seconds) % 60;
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m " + std::to_string(secs) + "s";
    }
}

// Run Phase 3 demo
bool runPhase3(const CLIConfig& config) {
    if (!config.quiet) {
        std::cout << "Running Phase 3: World Interaction Demo" << std::endl;
        std::cout << "===========================================\n" << std::endl;
    }
    
    try {
        // Create config
        auto cfg = std::make_shared<Config>();
        
        // Set default values
        cfg->set("environment_width", 20, nlm::ConfigSource::Default);
        cfg->set("environment_height", 20, nlm::ConfigSource::Default);
        cfg->set("vision_width", 16, nlm::ConfigSource::Default);
        cfg->set("vision_height", 16, nlm::ConfigSource::Default);
        cfg->set("neuron_count", 500, nlm::ConfigSource::Default);
        cfg->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        cfg->set("random_seed", 42u, nlm::ConfigSource::Default);
        
        // Set configuration from CLI
        cfg->set("neuron_count", config.neuronCount, nlm::ConfigSource::CommandLine);
        cfg->set("simulation_timestep", 0.001, nlm::ConfigSource::CommandLine);
        cfg->set("stdp_ltp_weight", config.learningRate, nlm::ConfigSource::CommandLine);
        
        if (!config.quiet) {
            std::cout << "Configuration loaded:" << std::endl;
            std::cout << "  " << cfg->summary() << std::endl;
            std::cout << "  Neurons: " << config.neuronCount << std::endl;
            std::cout << "  Steps per episode: 200" << std::endl;
            std::cout << "  Episodes: " << config.maxEpisodes << std::endl;
        }
        
        // For now, just print success and return
        if (!config.quiet) {
            std::cout << "\nPhase 3 configuration loaded successfully!" << std::endl;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ERROR running Phase 3: " << e.what() << std::endl;
        return false;
    }
}

// Configure environment for experiments
void configureEnvironment(std::shared_ptr<Config> config) {
    config->set("environment_width", 20);
    config->set("environment_height", 20);
    config->set("vision_width", 16);
    config->set("vision_height", 16);
}

// Configure brain for experiments  
void configureBrain(std::shared_ptr<Config> config) {
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42u);
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
}

// Run Phase 4 demo
bool runPhase4(const CLIConfig& config) {
    if (!config.quiet) {
        std::cout << "Running Phase 4: Emerging Cognition Demo" << std::endl;
        std::cout << "==========================================\n" << std::endl;
    }
    
    try {
        // This would use the Phase4Demo from experiments/Phase4Demo.cpp
        // For now, we'll provide a simplified version
        if (!config.quiet) {
            std::cout << "Phase 4 demo requires the Phase4Demo executable." << std::endl;
            std::cout << "Run: ./nlm_phase4_demo <numTrials>" << std::endl;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ERROR running Phase 4: " << e.what() << std::endl;
        return false;
    }
}

// Run Phase 5 demo
bool runPhase5(const CLIConfig& config) {
    if (!config.quiet) {
        std::cout << "Running Phase 5: Lifetime Integration Demo" << std::endl;
        std::cout << "============================================\n" << std::endl;
    }
    
    try {
        // Create configuration
        Phase5Config phase5Config;
        phase5Config.initialNeurons = config.neuronCount;
        phase5Config.maxNeurons = config.neuronCount * 2;
        phase5Config.synapseFormationSteps = 1000;
        phase5Config.sensoryDevelopmentSteps = 2000;
        phase5Config.motorDevelopmentSteps = 2000;
        phase5Config.associativeDevelopmentSteps = 2000;
        phase5Config.memoryConsolidationSteps = 2000;
        phase5Config.socialDevelopmentSteps = 2000;
        phase5Config.enableDevelopment = config.enableDevelopment;
        
        if (!config.quiet) {
            std::cout << "Configuration:" << std::endl;
            std::cout << "  Initial neurons: " << phase5Config.initialNeurons << std::endl;
            std::cout << "  Max neurons: " << phase5Config.maxNeurons << std::endl;
            std::cout << "  Development enabled: " << (config.enableDevelopment ? "yes" : "no") << std::endl;
        }
        
        // Run Phase 5 experiment
        Phase5IntegratedExperiment experiment;
        auto result = experiment.run(phase5Config);
        
        if (!config.quiet) {
            std::cout << "\nPhase 5 Results:" << std::endl;
            std::cout << "  Total reward: " << result.totalReward << std::endl;
            std::cout << "  Episodes completed: " << result.phaseResults.size() << std::endl;
            std::cout << "  Final performance: " << result.finalPerformance << std::endl;
            std::cout << "  Memory capacity: " << result.memoryCapacity << std::endl;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ERROR running Phase 5: " << e.what() << std::endl;
        return false;
    }
}

// Run Phase 6 demo
bool runPhase6(const CLIConfig& config) {
    if (!config.quiet) {
        std::cout << "Running Phase 6: Final Integration Demo" << std::endl;
        std::cout << "============================================\n" << std::endl;
    }
    
    try {
        // Create Phase 6 configuration
        Phase6Config phase6Config;
        phase6Config.maxSteps = config.maxSteps;
        phase6Config.neuronCount = config.neuronCount;
        phase6Config.regionCount = 1;
        phase6Config.connectionProbability = 0.1f;
        phase6Config.enableCheckpointing = config.enableCheckpointing;
        phase6Config.enableReplay = config.enableReplay;
        phase6Config.enableDevelopment = config.enableDevelopment;
        phase6Config.checkpointPath = "./nlm_phase6_checkpoint.bin";
        
        // Run Phase 6 integration
        Phase6IntegratedExperiment experiment;
        
        if (!config.quiet) {
            std::cout << "Phase 6 Integration Configuration:" << std::endl;
            std::cout << "  Neurons: " << phase6Config.neuronCount << std::endl;
            std::cout << "  Max steps: " << phase6Config.maxSteps << std::endl;
            std::cout << "  Development enabled: " << (phase6Config.enableDevelopment ? "yes" : "no") << std::endl;
            std::cout << "  Checkpointing enabled: " << (phase6Config.enableCheckpointing ? "yes" : "no") << std::endl;
            std::cout << "  Replay enabled: " << (phase6Config.enableReplay ? "yes" : "no") << std::endl;
            std::cout << std::endl;
            std::cout << "--- Integration Verification ---" << std::endl;
        }
        
        // First, run the quick integration verification
        bool integrationOK = experiment.verifyIntegration();
        
        if (!config.quiet) {
            std::cout << std::endl;
            if (!integrationOK) {
                std::cerr << "ERROR: Integration verification failed!" << std::endl;
                return false;
            }
            
            std::cout << "Integration verification passed!" << std::endl << std::endl;
        }
        
        // Test individual systems
        if (!config.quiet) {
            std::cout << "--- Memory Integration Test ---" << std::endl;
        }
        experiment.testMemoryIntegration();
        
        if (!config.quiet) {
            std::cout << "--- Neuromodulation Integration Test ---" << std::endl;
        }
        experiment.testNeuromodulationIntegration();
        
        if (!config.quiet) {
            std::cout << "--- Checkpoint Test ---" << std::endl;
        }
        experiment.testCheckpointing();
        
        if (!config.quiet) {
            std::cout << "--- Replay Test ---" << std::endl;
        }
        experiment.testReplay();
        
        if (!config.quiet) {
            std::cout << "\n--- Full Integration Experiment ---" << std::endl;
        }
        
        // Run full experiment with settings for demo
        auto result = experiment.run(phase6Config);
        
        if (!config.quiet) {
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
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ERROR running Phase 6: " << e.what() << std::endl;
        return false;
    }
}

// Run all phases
bool runAllPhases(const CLIConfig& config) {
    if (!config.quiet) {
        std::cout << "Running ALL phases sequentially:" << std::endl;
        std::cout << "===========================================\n" << std::endl;
    }
    
    double startTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    bool success = true;
    
    // Run Phase 3
    std::cout << "\n=== Phase 3: World Interaction ===\n" << std::endl;
    success = runPhase3(config) && success;
    
    if (success) {
        std::cout << "\n=== Phase 4: Emerging Cognition ===\n" << std::endl;
        success = runPhase4(config) && success;
    }
    
    if (success) {
        std::cout << "\n=== Phase 5: Lifetime Integration ===\n" << std::endl;
        success = runPhase5(config) && success;
    }
    
    if (success) {
        std::cout << "\n=== Phase 6: Final Integration ===\n" << std::endl;
        success = runPhase6(config) && success;
    }
    
    double endTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    if (!config.quiet) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "ALL PHASES COMPLETE" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "Total elapsed time: " << formatElapsedTime(endTime - startTime) << std::endl;
        std::cout << "Status: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    }
    
    return success;
}

int main(int argc, char** argv) {
    // Set default locale for better output
    std::cout << std::fixed << std::setprecision(3);
    
    // Parse arguments
    CLIConfig config = parseArguments(argc, argv);
    
    // Handle special flags
    if (config.showHelp) {
        printHelp();
        return 0;
    }
    
    if (config.showVersion) {
        printVersion();
        return 0;
    }
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Print banner
    printBanner();
    
    // Set up configuration file loading
    if (!config.quiet) {
        std::cout << "NLM Version " << NLM_VERSION << " (Phase " << NLM_PHASE << ")" << std::endl;
        std::cout << "Initializing..." << std::endl << std::endl;
    }
    
    // Load configuration from file if specified
    if (std::filesystem::exists(config.configFile)) {
        auto cfg = std::make_shared<Config>();
        if (cfg->loadFromFile(config.configFile)) {
            if (!config.quiet) {
                std::cout << "Loaded configuration from: " << config.configFile << std::endl;
                std::cout << cfg->summary();
            }
        } else if (!config.quiet) {
            std::cout << "Warning: Could not load config file: " << config.configFile << std::endl;
        }
    } else if (!config.quiet) {
        std::cout << "Using default configuration (file not found: " << config.configFile << ")" << std::endl;
    }
    
    // Override with command line custom parameters
    for (const auto& param : config.customParams) {
        if (!config.quiet) {
            std::cout << "Custom parameter: " << param.first << " = " << param.second << std::endl;
        }
    }
    
    std::cout << std::endl;
    
    // Run the appropriate phase
    bool success = false;
    
    if (config.phase == "3_demo" || config.phase == "3") {
        success = runPhase3(config);
    } else if (config.phase == "4_demo" || config.phase == "4") {
        success = runPhase4(config);
    } else if (config.phase == "5_demo" || config.phase == "5") {
        success = runPhase5(config);
    } else if (config.phase == "6_demo" || config.phase == "6" || config.phase == "demo") {
        success = runPhase6(config);
    } else if (config.phase == "all") {
        success = runAllPhases(config);
    } else {
        std::cerr << "ERROR: Unknown phase: " << config.phase << std::endl;
        std::cerr << "Run './nlm --help' for usage information." << std::endl;
        return 1;
    }
    
    // Final status
    std::cout << std::endl;
    std::cout << "=" << std::string(60, '=') << std::endl;
    if (success) {
        std::cout << "NLM execution completed successfully!" << std::endl;
    } else {
        std::cout << "NLM execution failed." << std::endl;
    }
    std::cout << "=" << std::string(60, '=') << std::endl;
    
    return success ? 0 : 1;
}