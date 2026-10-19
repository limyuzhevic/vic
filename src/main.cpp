// NLM (熙然) - Neural Learning Machine
// Main command-line interface for the experimental artificial developmental brain
//
// This program provides a comprehensive command-line interface for running
// NLM simulations with various configurations, saving/loading checkpoints,
// and running different experiment modes.

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Logger/ConsoleLogger.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>

using namespace nlm;

// Forward declarations for command-line parsing
class CommandLineInterface;

// Command-line options structure
struct CLIConfig {
    std::string configFile;
    std::string checkpointFile;
    std::string outputFile;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    float connectionProbability = 0.1f;
    double simulationTimestep = 0.001;
    int64_t randomSeed = 42;
    bool enableCheckpointing = false;
    bool enableReplay = false;
    bool enableDevelopment = true;
    bool verbose = false;
    bool quiet = false;
    size_t simulationSteps = 10000;
    std::string mode = "brain";
    std::string experiment = "phase6";
    std::vector<std::string> additionalArgs;
};

// Usage information
void printUsage() {
    std::cout << R"(
=== NLM - Neural Learning Machine ===
Command-line interface for experimental artificial developmental brain

USAGE:
    nlm [OPTIONS]

MODES:
    brain        Run basic brain simulation (default)
    phase6       Run Phase 6 integration experiment
    demo         Run Phase 6 demo

OPTIONS:
    --help, -h              Show this help message
    --version, -v           Show version information
    --config, -c FILE       Load configuration from FILE
    --checkpoint, -k FILE   Save/load checkpoint from FILE
    --output, -o FILE       Save simulation results to FILE
    --neurons, -n COUNT     Number of neurons (default: 1000)
    --regions, -r COUNT      Number of brain regions (default: 1)
    --conn, -p PROBABILITY   Connection probability (default: 0.1)
    --timestep, -t TIME      Simulation timestep (default: 0.001)
    --seed, -s SEED         Random seed (default: 42)
    --steps, -S STEPS        Number of simulation steps (default: 10000)
    --checkpointing, -k      Enable checkpointing
    --replay, -y            Enable replay system
    --development, -d       Enable development system
    --verbose, -V           Verbose output
    --quiet, -q             Quiet output (minimal logging)

EXPERIMENT OPTIONS:
    --experiment, -e MODE    Experiment mode (brain, phase6, demo)

ADVANCED OPTIONS:
    --load FILE              Load brain state from checkpoint
    --save FILE              Save brain state to checkpoint
    --continue               Continue from checkpoint if available

EXAMPLES:
    # Basic brain simulation
    nlm --neurons 500 --steps 1000

    # Load configuration and save results
    nlm --config my_config.cfg --output results.bin --neurons 2000

    # Run Phase 6 integration experiment
    nlm --experiment phase6 --steps 5000 --checkpointing

    # Run with custom settings
    nlm --neurons 100 --regions 2 --conn 0.2 --seed 123

    # Continue from checkpoint
    nlm --checkpoint my_checkpoint.bin --continue

For more information, see the documentation files:
    - README.md
    - docs/ARCHITECTURE.md
    - docs/SCIENCE.md
    - easy_usage.md (Python interface)

For technical details, see:
    - docs/PHASE6_FINAL_AUDIT.md
    - docs/PHASE6_FINAL_RESULTS.md
    - docs/LIMITATIONS.md
)" << std::endl;
}

void printVersion() {
    std::cout << "NLM (熙然) - Neural Learning Machine version 0.1.0" << std::endl;
    std::cout << "Phase 6: Final Integration" << std::endl;
    std::cout << "Experimental artificial developmental brain system" << std::endl;
    std::cout << "Built with C++20 and Python bindings (pynlm)" << std::endl;
}

// Parse command line arguments
CLIConfig parseCommandLine(int argc, char** argv) {
    CLIConfig config;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            exit(0);
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            exit(0);
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                config.configFile = argv[++i];
            } else {
                std::cerr << "Error: --config requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--checkpoint" || arg == "-k") {
            if (i + 1 < argc) {
                config.checkpointFile = argv[++i];
            } else {
                std::cerr << "Error: --checkpoint requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--output" || arg == "-o") {
            if (i + 1 < argc) {
                config.outputFile = argv[++i];
            } else {
                std::cerr << "Error: --output requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--neurons" || arg == "-n") {
            if (i + 1 < argc) {
                config.neuronCount = std::stoul(argv[++i]);
            } else {
                std::cerr << "Error: --neurons requires a count argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--regions" || arg == "-r") {
            if (i + 1 < argc) {
                config.regionCount = std::stoul(argv[++i]);
            } else {
                std::cerr << "Error: --regions requires a count argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--conn" || arg == "-p") {
            if (i + 1 < argc) {
                config.connectionProbability = std::stof(argv[++i]);
            } else {
                std::cerr << "Error: --conn requires a probability argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--timestep" || arg == "-t") {
            if (i + 1 < argc) {
                config.simulationTimestep = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: --timestep requires a time value" << std::endl;
                exit(1);
            }
        } else if (arg == "--seed" || arg == "-s") {
            if (i + 1 < argc) {
                config.randomSeed = std::stoll(argv[++i]);
            } else {
                std::cerr << "Error: --seed requires a seed value" << std::endl;
                exit(1);
            }
        } else if (arg == "--steps" || arg == "-S") {
            if (i + 1 < argc) {
                config.simulationSteps = std::stoul(argv[++i]);
            } else {
                std::cerr << "Error: --steps requires a count argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--checkpointing") {
            config.enableCheckpointing = true;
        } else if (arg == "--replay") {
            config.enableReplay = true;
        } else if (arg == "--development") {
            config.enableDevelopment = true;
        } else if (arg == "--verbose" || arg == "-V") {
            config.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            config.quiet = true;
        } else if (arg == "--experiment" || arg == "-e") {
            if (i + 1 < argc) {
                config.experiment = argv[++i];
            } else {
                std::cerr << "Error: --experiment requires a mode argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--load") {
            if (i + 1 < argc) {
                config.checkpointFile = "load:" + std::string(argv[++i]);
            } else {
                std::cerr << "Error: --load requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--save") {
            if (i + 1 < argc) {
                config.checkpointFile = "save:" + std::string(argv[++i]);
            } else {
                std::cerr << "Error: --save requires a file argument" << std::endl;
                exit(1);
            }
        } else if (arg == "--continue") {
            config.checkpointFile = "continue";
        } else {
            // Additional arguments
            config.additionalArgs.push_back(arg);
        }
    }
    
    return config;
}

// Create default configuration from CLI options
std::shared_ptr<Config> createConfig(const CLIConfig& cliConfig) {
    auto config = std::make_shared<Config>();
    
    // Set basic configuration
    config->set("neuron_count", static_cast<int64_t>(cliConfig.neuronCount), ConfigSource::CommandLine);
    config->set("region_count", static_cast<int64_t>(cliConfig.regionCount), ConfigSource::CommandLine);
    config->set("connection_probability", cliConfig.connectionProbability, ConfigSource::CommandLine);
    config->set("simulation_timestep", cliConfig.simulationTimestep, ConfigSource::CommandLine);
    config->set("random_seed", cliConfig.randomSeed, ConfigSource::CommandLine);
    
    // Set plasticity parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Set integration intervals
    config->set("replay_interval", cliConfig.enableReplay ? 100 : 1000, ConfigSource::CommandLine);
    config->set("consolidation_interval", 1000, ConfigSource::Default);
    config->set("checkpoint_dir", "./checkpoints", ConfigSource::Default);
    
    return config;
}

// Setup logging based on verbosity level
void setupLogging(bool verbose, bool quiet) {
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(quiet ? LogLevel::Error : (verbose ? LogLevel::Debug : LogLevel::Info));
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
}

// Run basic brain simulation
bool runBrainSimulation(const CLIConfig& config) {
    auto cfg = createConfig(config);
    
    std::cout << "=== NLM Basic Brain Simulation ===" << std::endl;
    std::cout << "Running brain simulation with " << config.neuronCount << " neurons for " << config.simulationSteps << " steps" << std::endl;
    
    auto brain = std::make_shared<Brain>(cfg);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain!" << std::endl;
        return false;
    }
    
    brain->logStatus();
    
    // Run simulation
    for (SimulationStep step = 0; step < config.simulationSteps && !config.enableCheckpointing; ++step) {
        brain->step(step);
        
        if (step % 1000 == 0) {
            std::cout << "Step " << step << ": " << brain->getAverageFiringRate() << " avg firing rate" << std::endl;
        }
    }
    
    // Save checkpoint if requested
    if (config.enableCheckpointing && !config.checkpointFile.empty()) {
        if (config.checkpointFile.find("save:") == 0) {
            std::string filepath = config.checkpointFile.substr(5);
            if (!brain->save(filepath)) {
                std::cerr << "Error: Failed to save checkpoint!" << std::endl;
                return false;
            }
            std::cout << "Checkpoint saved to: " << filepath << std::endl;
        }
    }
    
    // Load checkpoint if requested
    if (!config.checkpointFile.empty() && config.checkpointFile.find("load:") == 0) {
        std::string filepath = config.checkpointFile.substr(5);
        if (!brain->load(filepath)) {
            std::cerr << "Error: Failed to load checkpoint!" << std::endl;
            return false;
        }
        std::cout << "Checkpoint loaded from: " << filepath << std::endl;
    }
    
    // Save results if requested
    if (!config.outputFile.empty()) {
        if (!brain->save(config.outputFile)) {
            std::cerr << "Error: Failed to save results!" << std::endl;
            return false;
        }
        std::cout << "Results saved to: " << config.outputFile << std::endl;
    }
    
    std::cout << "=== Simulation Complete ===" << std::endl;
    std::cout << "Final statistics:" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    
    return true;
}

// Run Phase 6 integration experiment
bool runPhase6Experiment(const CLIConfig& config) {
    std::cout << "=== NLM Phase 6 Integration Experiment ===" << std::endl;
    std::cout << "Running Phase 6 integration with " << config.neuronCount << " neurons for " << config.simulationSteps << " steps" << std::endl;
    
    // Create Phase 6 configuration
    Phase6Config phase6Config;
    phase6Config.neuronCount = config.neuronCount;
    phase6Config.maxSteps = config.simulationSteps;
    phase6Config.regionCount = config.regionCount;
    phase6Config.connectionProbability = config.connectionProbability;
    phase6Config.enableCheckpointing = config.enableCheckpointing;
    phase6Config.enableReplay = config.enableReplay;
    phase6Config.enableDevelopment = config.enableDevelopment;
    
    if (!config.checkpointFile.empty() && config.checkpointFile.find("load:") == 0) {
        phase6Config.checkpointPath = config.checkpointFile.substr(5);
    } else if (!config.checkpointFile.empty() && config.checkpointFile.find("save:") == 0) {
        phase6Config.checkpointPath = config.checkpointFile.substr(5);
    } else {
        phase6Config.checkpointPath = "./phase6_checkpoint.bin";
    }
    
    // Create and run experiment
    Phase6IntegratedExperiment experiment;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto result = experiment.run(phase6Config);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    
    // Print results
    std::cout << std::endl;
    std::cout << "=== Phase 6 Experiment Results ===" << std::endl;
    std::cout << "Total reward: " << result.totalReward << std::endl;
    std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
    std::cout << "Episodic memory episodes: " << result.memoryEpisodesStored << std::endl;
    std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
    std::cout << "Wall clock time: " << duration << "s" << std::endl;
    std::cout << std::endl;
    std::cout << "=== Integration Status ===" << std::endl;
    std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
    std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
    std::cout << "Replay: " << (result.replayWorks ? "WORKING" : "NOT WORKING") << std::endl;
    
    return result.checkpointingWorks && result.replayWorks;
}

// Run Phase 6 demo
bool runPhase6Demo() {
    std::cout << "=== NLM Phase 6 Demo ===" << std::endl;
    std::cout << "Running Phase 6 integration demo..." << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // First run integration verification
    std::cout << "--- Integration Verification ---" << std::endl;
    bool integrationOK = experiment.verifyIntegration();
    
    if (!integrationOK) {
        std::cerr << "ERROR: Integration verification failed!" << std::endl;
        return false;
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
    
    std::cout << "=== Phase 6 Demo Complete ===" << std::endl;
    std::cout << "All integration tests passed successfully!" << std::endl;
    
    return true;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Parse command line arguments
    CLIConfig config = parseCommandLine(argc, argv);
    
    // Setup logging
    setupLogging(config.verbose, config.quiet);
    
    // Run the requested mode
    bool success = false;
    
    if (config.experiment == "phase6") {
        success = runPhase6Experiment(config);
    } else if (config.experiment == "demo") {
        success = runPhase6Demo();
    } else {
        success = runBrainSimulation(config);
    }
    
    if (!success) {
        std::cerr << "Error: Command failed!" << std::endl;
        return 1;
    }
    
    std::cout << "=== Command completed successfully ===" << std::endl;
    return 0;
}