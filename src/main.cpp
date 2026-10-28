// NLM (熙然) - Neural Learning Machine CLI
// Phase 6: Integration Test (Default)
//
// This is the main command-line interface for the Neural Learning Machine project.
// Supports multiple Phase 2 experiments and the Phase 6 integration test.

#include "basic_connectivity_test.hpp"
#include "plasticity_experiment.hpp"
#include "stdp_verification.hpp"
#include "phase6_integration_demo.hpp"

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>

namespace nlm {
    /**
     * Command-line arguments structure
     */
    struct CommandLineArgs {
        std::string configFile = "configs/default.cfg";
        std::string experiment = "phase6"; // Default to Phase 6
        bool help = false;
        bool verbose = false;
        bool quiet = false;
        int randomSeed = 42;
        double simulationTimestep = 0.001;
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.1f;
        
        // Phase 6 specific options
        bool phase6EnableCheckpointing = true;
        bool phase6EnableReplay = true;
        bool phase6EnableDevelopment = true;
        std::string phase6CheckpointPath = "./checkpoint_test.bin";
        size_t phase6MaxSteps = 10000;
    };
    
    /**
     * Print the application banner
     */
    void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Integration Test                                 ║
    ║                                                               ║
    ║     Advanced artificial developmental brain with all systems   ║
    ║     This version includes:                                    ║
    ║     - Memory systems (working & episodic)                   ║
    ║     - Neuromodulation (dopamine, curiosity, novelty)         ║
    ║     - Prediction system                                      ║
    ║     - Cognitive systems (planner, concept formation)         ║
    ║     - Development system                                     ║
    ║     - Checkpoint save/load                                  ║
    ║     - Replay system                                         ║
    ║     - All Phase 2 experiments                                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    /**
     * Parse command-line arguments
     */
    CommandLineArgs parseArguments(int argc, char** argv) {
        CommandLineArgs args;
        std::map<std::string, std::string> argMap;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--quiet" || arg == "-q") {
                args.quiet = true;
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg.substr(0, 8) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    args.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    args.configFile = argv[++i];
                }
            } else if (arg.substr(0, 7) == "--phase6" || arg.substr(0, 10) == "--phase6-" || arg == "--phase6") {
                args.experiment = "phase6";
            } else if (arg.substr(0, 10) == "--phase2-") {
                if (arg == "--phase2-basic") {
                    args.experiment = "phase2-basic";
                } else if (arg == "--phase2-plasticity") {
                    args.experiment = "phase2-plasticity";
                } else if (arg == "--phase2-stdp") {
                    args.experiment = "phase2-stdp";
                }
            } else if (arg == "--random-seed") {
                if (i + 1 < argc) {
                    args.randomSeed = std::stoi(argv[++i]);
                }
            } else if (arg == "--timestep") {
                if (i + 1 < argc) {
                    args.simulationTimestep = std::stod(argv[++i]);
                }
            } else if (arg == "--neurons") {
                if (i + 1 < argc) {
                    args.neuronCount = std::stoul(argv[++i]);
                }
            } else if (arg == "--regions") {
                if (i + 1 < argc) {
                    args.regionCount = std::stoul(argv[++i]);
                }
            } else if (arg == "--conn-prob") {
                if (i + 1 < argc) {
                    args.connectionProbability = std::stof(argv[++i]);
                }
            } else if (arg == "--phase6-checkpoint" || arg == "--phase6-checkpoint-path") {
                if (i + 1 < argc) {
                    args.phase6CheckpointPath = argv[++i];
                }
            } else if (arg == "--phase6-max-steps") {
                if (i + 1 < argc) {
                    args.phase6MaxSteps = std::stoul(argv[++i]);
                }
            } else {
                // Unknown argument
                std::cerr << "Warning: Unknown argument: " << arg << std::endl;
            }
        }
        
        return args;
    }
    
    /**
     * Display help information
     */
    void displayHelp() {
        printBanner();
        
        std::cout << "\n";
        std::cout << "Usage: nlm [OPTIONS]\n";
        std::cout << "\n";
        
        std::cout << "EXPERIMENTS:\n";
        std::cout << "  --phase6                    Run Phase 6 integration test (default)\n";
        std::cout << "  --phase2-basic             Run Phase 2: Basic Connectivity Test\n";
        std::cout << "  --phase2-plasticity        Run Phase 2: Plasticity Experiment\n";
        std::cout << "  --phase2-stdp              Run Phase 2: STDP Verification\n";
        std::cout << "\n";
        
        std::cout << "CONFIGURATION:\n";
        std::cout << "  --config <file>            Load configuration from file (default: configs/default.cfg)\n";
        std::cout << "  --random-seed <n>          Set random seed for reproducibility\n";
        std::cout << "  --timestep <t>             Set simulation timestep\n";
        std::cout << "  --neurons <n>              Set neuron count\n";
        std::cout << "  --regions <n>              Set brain region count\n";
        std::cout << "  --conn-prob <p>            Set connection probability (0.0-1.0)\n";
        std::cout << "\n";
        
        std::cout << "ADVANCED OPTIONS:\n";
        std::cout << "  --phase6-checkpoint-path <path>    Set Phase 6 checkpoint path\n";
        std::cout << "  --phase6-max-steps <n>             Set Phase 6 max simulation steps\n";
        std::cout << "  --quiet, -q                   Suppress informational messages\n";
        std::cout << "  --verbose, -v                  Enable verbose output\n";
        std::cout << "  --help, -h                     Display this help message\n";
        std::cout << "\n";
        
        std::cout << "EXAMPLES:\n";
        std::cout << "  nlm --help                              Show help\n";
        std::cout << "  nlm --phase2-basic                     Run basic connectivity test\n";
        std::cout << "  nlm --phase2-plasticity                Run plasticity experiment\n";
        std::cout << "  nlm --phase2-stdp                      Run STDP verification\n";
        std::cout << "  nlm --phase6 --neurons 2000            Run Phase 6 with 2000 neurons\n";
        std::cout << "\n";
        
        std::cout << "Phase 6 Integration Test\n";
        std::cout << "======================\n";
        std::cout << "The Phase 6 integration test verifies that all brain systems\n";
        std::cout << "are properly connected and functional:\n";
        std::cout << "  - Memory systems (working & episodic)\n";
        std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)\n";
        std::cout << "  - Prediction system\n";
        std::cout << "  - Cognitive systems (planner, concept formation, attention)\n";
        std::cout << "  - Development system\n";
        std::cout << "  - Checkpoint save/load\n";
        std::cout << "  - Replay system\n";
    }
    
    /**
     * Load configuration from file
     */
    void loadConfiguration(std::shared_ptr<Config> config, const std::string& configFile) {
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    /**
     * Apply command line overrides to configuration
     */
    void applyOverrides(std::shared_ptr<Config> config, const CommandLineArgs& args) {
        config->set("random_seed", static_cast<int64_t>(args.randomSeed), ConfigSource::Default);
        config->set("simulation_timestep", args.simulationTimestep, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(args.neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(args.regionCount), ConfigSource::Default);
        config->set("connection_probability", args.connectionProbability, ConfigSource::Default);
        
        // Phase 2 specific overrides
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    /**
     * Run basic connectivity test
     */
    int runBasicConnectivityTest() {
        std::cout << "=== Phase 2: Basic Connectivity Test ===\n";
        return nlm::runBasicConnectivityTestImpl();
    }
    
    /**
     * Run plasticity experiment
     */
    int runPlasticityExperiment() {
        std::cout << "=== Phase 2: Plasticity Experiment ===\n";
        return nlm::runPlasticityExperimentImpl();
    }
    
    /**
     * Run STDP verification
     */
    int runStdpVerification() {
        std::cout << "=== Phase 2: STDP Verification ===\n";
        return nlm::runStdpVerificationImpl();
    }
    
    /**
     * Run Phase 6 integration demo
     */
    int runPhase6IntegrationDemo(const CommandLineArgs& args) {
        std::cout << "=== NLM Phase 6 Integration Demo ===\n";
        
        Phase6Config config;
        config.neuronCount = args.neuronCount;
        config.maxSteps = args.phase6MaxSteps;
        config.enableCheckpointing = args.phase6EnableCheckpointing;
        config.enableReplay = args.phase6EnableReplay;
        config.enableDevelopment = args.phase6EnableDevelopment;
        config.checkpointPath = args.phase6CheckpointPath;
        
        Phase6IntegrationDemo demo;
        return demo.run(config);
    }
}

int main(int argc, char** argv) {
    try {
        nlm::printBanner();
        
        // Parse command line arguments
        nlm::CommandLineArgs args = nlm::parseArguments(argc, argv);
        
        // Display help and exit if requested
        if (args.help) {
            nlm::displayHelp();
            return 0;
        }
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(args.quiet ? LogLevel::Error : LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (args.verbose) {
            consoleLogger->setLevel(LogLevel::Debug);
        }
        
        // Initialize random number generator
        Random::initialize(static_cast<uint64_t>(args.randomSeed));
        NLM_LOG_INFO("Random seed initialized: " + std::to_string(args.randomSeed));
        
        // Create and configure brain
        auto config = std::make_shared<Config>();
        nlm::loadConfiguration(config, args.configFile);
        nlm::applyOverrides(config, args);
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration summary:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", args.randomSeed)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", args.simulationTimestep)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", static_cast<int64_t>(args.neuronCount))));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", static_cast<int64_t>(args.regionCount))));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", args.connectionProbability)));
        NLM_LOG_INFO("");
        
        // Run selected experiment
        int result;
        
        if (args.experiment == "phase2-basic") {
            result = nlm::runBasicConnectivityTest();
        } else if (args.experiment == "phase2-plasticity") {
            result = nlm::runPlasticityExperiment();
        } else if (args.experiment == "phase2-stdp") {
            result = nlm::runStdpVerification();
        } else {
            // Default to Phase 6 integration test
            result = nlm::runPhase6IntegrationDemo(args);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
}