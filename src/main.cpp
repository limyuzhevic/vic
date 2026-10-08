#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <csignal>
#include <cstring>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase3Demo.hpp"
#include "experiments/Phase4Demo.hpp"
#include "experiments/Phase5Experiment.hpp"
#include "experiments/Phase6Demo.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;

// Global flag for graceful shutdown
bool g_shutdown = false;

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", shutting down gracefully..." << std::endl;
    g_shutdown = true;
}

class CommandLineInterface {
public:
    CommandLineInterface() {
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
    }
    
    int run(int argc, char** argv) {
        parseArguments(argc, argv);
        
        if (helpRequested) {
            printHelp();
            return 0;
        }
        
        if (versionRequested) {
            printVersion();
            return 0;
        }
        
        if (demoMode.empty()) {
            std::cerr << "ERROR: No demo mode specified. Use --demo to select a mode." << std::endl;
            printHelp();
            return 1;
        }
        
        return runDemo();
    }
    
private:
    // Command line options
    bool helpRequested = false;
    bool versionRequested = false;
    std::string demoMode;
    std::string configFile;
    std::string outputFile;
    std::string checkpointDir;
    bool verbose = false;
    bool quiet = false;
    std::string logLevel = "info";
    int numSteps = 1000;
    int numEpisodes = 5;
    int numSeeds = 1;
    float timestep = 0.001f;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    bool enableNovelty = true;
    bool enableCuriosity = true;
    bool enableReward = true;
    float rewardMultiplier = 1.0f;
    std::string experimentType = "integrated";
    std::string outputFormat = "text";
    bool continuous = false;
    int updateInterval = 1000;
    
    void parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
            } else if (arg == "--version" || arg == "-v") {
                versionRequested = true;
            } else if (arg == "--demo" || arg == "-d") {
                if (i + 1 < argc) {
                    demoMode = argv[++i];
                } else {
                    std::cerr << "ERROR: --demo requires a mode argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--config" || arg == "-c") {
                if (i + 1 < argc) {
                    configFile = argv[++i];
                } else {
                    std::cerr << "ERROR: --config requires a file argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    outputFile = argv[++i];
                } else {
                    std::cerr << "ERROR: --output requires a file argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--checkpoint-dir") {
                if (i + 1 < argc) {
                    checkpointDir = argv[++i];
                } else {
                    std::cerr << "ERROR: --checkpoint-dir requires a directory argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--verbose" || arg == "-V") {
                verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
            } else if (arg == "--log-level") {
                if (i + 1 < argc) {
                    logLevel = argv[++i];
                } else {
                    std::cerr << "ERROR: --log-level requires a level argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--steps" || arg == "-s") {
                if (i + 1 < argc) {
                    numSteps = std::atoi(argv[++i]);
                } else {
                    std::cerr << "ERROR: --steps requires a number argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--episodes" || arg == "-e") {
                if (i + 1 < argc) {
                    numEpisodes = std::atoi(argv[++i]);
                } else {
                    std::cerr << "ERROR: --episodes requires a number argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--seeds" || arg == "-S") {
                if (i + 1 < argc) {
                    numSeeds = std::atoi(argv[++i]);
                } else {
                    std::cerr << "ERROR: --seeds requires a number argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    timestep = static_cast<float>(std::atof(argv[++i]));
                } else {
                    std::cerr << "ERROR: --timestep requires a number argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--no-checkpointing") {
                enableCheckpointing = false;
            } else if (arg == "--no-replay") {
                enableReplay = false;
            } else if (arg == "--no-development") {
                enableDevelopment = false;
            } else if (arg == "--no-novelty") {
                enableNovelty = false;
            } else if (arg == "--no-curiosity") {
                enableCuriosity = false;
            } else if (arg == "--no-reward") {
                enableReward = false;
            } else if (arg == "--reward-multiplier") {
                if (i + 1 < argc) {
                    rewardMultiplier = static_cast<float>(std::atof(argv[++i]));
                } else {
                    std::cerr << "ERROR: --reward-multiplier requires a number argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--demo-mode") {
                if (i + 1 < argc) {
                    experimentType = argv[++i];
                } else {
                    std::cerr << "ERROR: --demo-mode requires a mode argument." << std::endl;
                    exit(1);
                }
            } else if (arg == "--continuous" || arg == "-C") {
                continuous = true;
            } else if (arg == "--update-interval") {
                if (i + 1 < argc) {
                    updateInterval = std::atoi(argv[++i]);
                } else {
                    std::cerr << "ERROR: --update-interval requires a number argument." << std::endl;
                    exit(1);
                }
            } else if (arg.substr(0, 2) == "--") {
                std::cerr << "ERROR: Unknown option: " << arg << std::endl;
                exit(1);
            } else {
                std::cerr << "ERROR: Unknown argument: " << arg << std::endl;
                exit(1);
            }
        }
    }
    
    int runDemo() {
        // Initialize global logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(logLevelToEnum(logLevel));
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (!quiet) {
            printBanner();
        }
        
        // Load configuration
        auto config = std::make_shared<Config>();
        if (!configFile.empty() && config->loadFromFile(configFile)) {
            if (!quiet) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            }
        } else if (!quiet) {
            std::cout << "Using default configuration." << std::endl;
        }
        
        // Apply command-line overrides
        config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
        
        std::cout << "=== NLM Phase 6 Advanced CLI ===" << std::endl;
        std::cout << "Demo mode: " << demoMode << std::endl;
        std::cout << "Steps: " << numSteps << ", Episodes: " << numEpisodes << std::endl;
        std::cout << "Log level: " << logLevel << std::endl;
        
        // Run the selected demo
        int result = 0;
        
        if (demoMode == "phase3") {
            result = runPhase3Demo(config);
        } else if (demoMode == "phase4") {
            result = runPhase4Demo(config);
        } else if (demoMode == "phase5") {
            result = runPhase5Demo(config);
        } else if (demoMode == "phase6") {
            result = runPhase6Demo(config);
        } else if (demoMode == "integrated") {
            result = runIntegratedDemo(config);
        } else {
            std::cerr << "ERROR: Unknown demo mode: " << demoMode << std::endl;
            printHelp();
            return 1;
        }
        
        return result;
    }
    
    int runPhase3Demo(std::shared_ptr<Config> config) {
        std::cout << "\n--- Running Phase 3 Demo ---" << std::endl;
        
        Phase3Demo demo;
        
        if (continuous) {
            std::cout << "Continuous mode enabled. Press Ctrl+C to stop." << std::endl;
            while (!g_shutdown) {
                demo.run(1, numSteps);
                if (!quiet) {
                    std::cout << "\nEpisode complete. Running another..." << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } else {
            demo.run(numEpisodes, numSteps);
        }
        
        return 0;
    }
    
    int runPhase4Demo(std::shared_ptr<Config> config) {
        std::cout << "\n--- Running Phase 4 Demo ---" << std::endl;
        
        // Create and run Phase 4 demo
        // This is a simplified version - in a full implementation, you'd use
        // the actual Phase4Demo class
        
        std::cout << "Phase 4 demo would run here..." << std::endl;
        std::cout << "Features: Memory systems, Prediction, Cognition" << std::endl;
        
        return 0;
    }
    
    int runPhase5Demo(std::shared_ptr<Config> config) {
        std::cout << "\n--- Running Phase 5 Demo ---" << std::endl;
        
        // Create and run Phase 5 demo
        // This is a simplified version - in a full implementation, you'd use
        // the actual Phase5IntegratedExperiment class
        
        std::cout << "Phase 5 demo would run here..." << std::endl;
        std::cout << "Features: Lifelong learning, Performance optimizations, Multithreading" << std::endl;
        
        return 0;
    }
    
    int runPhase6Demo(std::shared_ptr<Config> config) {
        std::cout << "\n--- Running Phase 6 Demo ---" << std::endl;
        
        // Use the actual Phase 6 demo
        nlm::Phase6IntegratedExperiment experiment;
        
        Phase6Config phase6Config;
        phase6Config.neuronCount = config->getOr<int64_t>("neuron_count", 500);
        phase6Config.maxSteps = numSteps;
        phase6Config.enableCheckpointing = enableCheckpointing;
        phase6Config.enableReplay = enableReplay;
        phase6Config.enableDevelopment = enableDevelopment;
        phase6Config.checkpointPath = "./checkpoints";
        
        if (continuous) {
            std::cout << "Continuous mode enabled. Press Ctrl+C to stop." << std::endl;
            while (!g_shutdown) {
                auto result = experiment.run(phase6Config);
                printPhase6Results(result);
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } else {
            auto result = experiment.run(phase6Config);
            printPhase6Results(result);
        }
        
        return 0;
    }
    
    int runIntegratedDemo(std::shared_ptr<Config> config) {
        std::cout << "\n--- Running Integrated Demo ---" << std::endl;
        
        // Run all phases sequentially
        int result = 0;
        
        result |= runPhase3Demo(config);
        
        result |= runPhase4Demo(config);
        
        result |= runPhase5Demo(config);
        
        result |= runPhase6Demo(config);
        
        return result;
    }
    
    void printPhase6Results(const Phase6IntegrationResult& result) {
        std::cout << "\n=== Phase 6 Integration Results ===" << std::endl;
        std::cout << "System Integration Status:" << std::endl;
        std::cout << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "  Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << "  Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
        
        std::cout << "\nPerformance Metrics:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Average Firing Rate: " << result.avgFiringRate << std::endl;
        std::cout << "  Episodes Stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "  Dopamine Level: " << result.dopamineLevel << std::endl;
        std::cout << "  Novelty Level: " << result.noveltyLevel << std::endl;
        std::cout << "  Curiosity Level: " << result.curiosityLevel << std::endl;
        
        std::cout << "\nTiming:" << std::endl;
        std::cout << "  Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
    }
    
    LogLevel logLevelToEnum(const std::string& level) {
        std::string lowerLevel = level;
        std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), ::tolower);
        
        if (lowerLevel == "debug") return LogLevel::Debug;
        if (lowerLevel == "info") return LogLevel::Info;
        if (lowerLevel == "warning") return LogLevel::Warning;
        if (lowerLevel == "error") return LogLevel::Error;
        if (lowerLevel == "critical") return LogLevel::Critical;
        
        return LogLevel::Info; // Default
    }
    
    void printHelp() {
        std::cout << "=== NLM Phase 6 Advanced CLI Usage ===" << std::endl;
        std::cout << std::endl;
        std::cout << "DESCRIPTION:" << std::endl;
        std::cout << "  Advanced command-line interface for NLM Phase 6 experiments." << std::endl;
        std::cout << std::endl;
        std::cout << "USAGE:" << std::endl;
        std::cout << "  nlm_cli --demo <mode> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "DEMO MODES:" << std::endl;
        std::cout << "  phase3     Run Phase 3 world interaction demo" << std::endl;
        std::cout << "  phase4     Run Phase 4 cognitive demo" << std::endl;
        std::cout << "  phase5     Run Phase 5 lifetime learning demo" << std::endl;
        std::cout << "  phase6     Run Phase 6 integrated demo (default)" << std::endl;
        std::cout << "  integrated Run all phases sequentially" << std::endl;
        std::cout << std::endl;
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --help, -h                    Show this help message" << std::endl;
        std::cout << "  --version, -v                 Show version information" << std::endl;
        std::cout << "  --demo <mode>, -d <mode>      Select demo mode (required)" << std::endl;
        std::cout << "  --config <file>, -c <file>    Load configuration file" << std::endl;
        std::cout << "  --output <file>, -o <file>    Output results to file" << std::endl;
        std::cout << "  --checkpoint-dir <dir>         Set checkpoint directory" << std::endl;
        std::cout << "  --verbose, -V                  Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q                    Suppress output" << std::endl;
        std::cout << "  --log-level <level>            Set logging level (debug, info, warning, error, critical)" << std::endl;
        std::cout << "  --steps <num>, -s <num>        Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --episodes <num>, -e <num>     Number of episodes (default: 5)" << std::endl;
        std::cout << "  --seeds <num>, -S <num>        Number of random seeds (default: 1)" << std::endl;
        std::cout << "  --timestep <float>              Simulation timestep (default: 0.001)" << std::endl;
        std::cout << "  --no-checkpointing              Disable checkpointing" << std::endl;
        std::cout << "  --no-replay                     Disable replay functionality" << std::endl;
        std::cout << "  --no-development                Disable development system" << std::endl;
        std::cout << "  --no-novelty                    Disable novelty detection" << std::endl;
        std::cout << "  --no-curiosity                  Disable curiosity system" << std::endl;
        std::cout << "  --no-reward                     Disable reward system" << std::endl;
        std::cout << "  --reward-multiplier <float>     Set reward multiplier" << std::endl;
        std::cout << "  --continuous, -C                Run continuously until interrupted" << std::endl;
        std::cout << "  --update-interval <int>         Update interval in continuous mode" << std::endl;
        std::cout << std::endl;
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm_cli --demo phase6                    Run Phase 6 demo with default settings" << std::endl;
        std::cout << "  nlm_cli --demo phase6 --steps 5000         Run with 5000 steps" << std::endl;
        std::cout << "  nlm_cli --demo integrated --no-checkpointing Run all phases without checkpointing" << std::endl;
        std::cout << "  nlm_cli --demo phase5 --verbose           Run Phase 5 with verbose output" << std::endl;
        std::cout << std::endl;
        std::cout << "ADVANCED FEATURES:" << std::endl;
        std::cout << "  - Real-time monitoring and status updates" << std::endl;
        std::cout << "  - Multiple experiment modes and configurations" << std::endl;
        std::cout << "  - Configurable logging and output options" << std::endl;
        std::cout << "  - Graceful shutdown with Ctrl+C" << std::endl;
        std::cout << "  - Configurable experiment parameters" << std::endl;
        std::cout << std::endl;
        std::cout << "For more information, see the NLM documentation." << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM Phase 6 Advanced CLI" << std::endl;
        std::cout << "Version: 0.1.0" << std::endl;
        std::cout << "Build: Phase 6 Final Integration" << std::endl;
        std::cout << "Features: Advanced configuration, real-time monitoring, multiple demo modes" << std::endl;
    }
    
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Advanced Integration Demo                        ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements the complete integrated brain system.║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
};

int main(int argc, char** argv) {
    CommandLineInterface cli;
    return cli.run(argc, argv);
}
