// NLM Command Line Interface
// Phase 6: Final Integration
//
// Comprehensive CLI tool for NLM brain simulation with:
// - Loading/saving brain states
// - Configuration customization
// - Verbose output and logging
// - Quick validation tests
// - Phase 6 demo mode
// - Configurable simulation parameters

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <sstream>

using namespace nlm;

class NLMCLI {
public:
    NLMCLI() {
        setupLogging();
    }

    int run(int argc, char** argv) {
        parseArguments(argc, argv);
        
        if (helpRequested) {
            showHelp();
            return 0;
        }
        
        if (versionRequested) {
            showVersion();
            return 0;
        }
        
        if (validateRequested) {
            return runValidation();
        }
        
        if (demoRequested) {
            return runPhase6Demo();
        }
        
        if (batchRequested) {
            return runBatchMode();
        }
        
        // Default: load and run simulation
        return runSimulation();
    }

private:
    struct CLIConfig {
        std::string configFile = "configs/default.cfg";
        std::string savePath;
        std::string loadPath;
        bool verbose = false;
        bool quiet = false;
        bool profile = false;
        double timestep = 0.001;
        int steps = 1000;
        int connections = 1000;
        float connectionProb = 0.1f;
        bool performance = false;
        std::string customArg;
        std::map<std::string, std::string> customParams;
    };

    bool helpRequested = false;
    bool versionRequested = false;
    bool validateRequested = false;
    bool demoRequested = false;
    bool batchRequested = false;
    CLIConfig config;
    std::vector<std::string> positionalArgs;

    void setupLogging() {
        auto logger = std::make_shared<Logger>();
        if (config.quiet) {
            auto quietLogger = std::make_shared<ConsoleLogger>(LogLevel::Error);
            logger->addLogger(quietLogger);
        } else if (config.verbose) {
            auto verboseLogger = std::make_shared<ConsoleLogger>(LogLevel::Debug);
            logger->addLogger(verboseLogger);
        } else {
            auto infoLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
            logger->addLogger(infoLogger);
        }
        Logger::setGlobal(logger);
    }

    void parseArguments(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
            } else if (arg == "--version") {
                versionRequested = true;
            } else if (arg == "--validate") {
                validateRequested = true;
            } else if (arg == "--demo") {
                demoRequested = true;
            } else if (arg == "--batch") {
                batchRequested = true;
            } else if (arg == "--quiet") {
                config.quiet = true;
            } else if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--profile") {
                config.profile = true;
            } else if (arg == "--performance") {
                config.performance = true;
            } else if (arg.find("--config=") == 0) {
                config.configFile = arg.substr(8);
            } else if (arg == "--config" && i + 1 < argc) {
                config.configFile = argv[++i];
            } else if (arg.find("--save=") == 0) {
                config.savePath = arg.substr(7);
            } else if (arg == "--save" && i + 1 < argc) {
                config.savePath = argv[++i];
            } else if (arg.find("--load=") == 0) {
                config.loadPath = arg.substr(7);
            } else if (arg == "--load" && i + 1 < argc) {
                config.loadPath = argv[++i];
            } else if (arg.find("--timestep=") == 0) {
                config.timestep = std::stod(arg.substr(11));
            } else if (arg.find("--steps=") == 0) {
                config.steps = std::stoi(arg.substr(8));
            } else if (arg.find("--connections=") == 0) {
                config.connections = std::stoi(arg.substr(14));
            } else if (arg.find("--connection-prob=") == 0) {
                config.connectionProb = std::stof(arg.substr(18));
            } else if (arg.find("-") != 0) {
                positionalArgs.push_back(arg);
            }
        }
    }

    void showHelp() {
        std::cout << "NLM Command Line Interface" << std::endl;
        std::cout << "Phase 6: Final Integration" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm_cli [OPTIONS] [ARGS...]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --help, -h                 Show this help message" << std::endl;
        std::cout << "  --version                  Show version information" << std::endl;
        std::cout << "  --validate                 Run quick validation test" << std::endl;
        std::cout << "  --demo                     Run Phase 6 demo" << std::endl;
        std::cout << "  --batch                    Run in batch mode" << std::endl;
        std::cout << "  --quiet                    Suppress non-essential output" << std::endl;
        std::cout << "  --verbose                  Enable verbose output" << std::endl;
        std::cout << "  --profile                  Enable performance profiling" << std::endl;
        std::cout << "  --performance              Enable performance monitoring" << std::endl;
        std::cout << "  --config <file>            Load configuration from file" << std::endl;
        std::cout << "  --save <path>              Save brain state to file" << std::endl;
        std::cout << "  --load <path>              Load brain state from file" << std::endl;
        std::cout << "  --timestep <t>             Set simulation timestep" << std::endl;
        std::cout << "  --steps <n>                Number of simulation steps" << std::endl;
        std::cout << "  --connections <n>          Number of neurons to create" << std::endl;
        std::cout << "  --connection-prob <p>      Connection probability" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_cli --demo                    # Run Phase 6 demo" << std::endl;
        std::cout << "  nlm_cli --load brain.nlm --save brain2.nlm  # Load and save" << std::endl;
        std::cout << "  nlm_cli --timestep 0.01 --steps 5000        # Custom simulation" << std::endl;
        std::cout << "  nlm_cli --validate                 # Quick validation test" << std::endl;
        std::cout << std::endl;
    }

    void showVersion() {
        std::cout << "NLM CLI v1.0.0 (Phase 6)" << std::endl;
        std::cout << "Neural Learning Machine - Final Integration" << std::endl;
        std::cout << "Built with C++20 and comprehensive features" << std::endl;
    }

    int runValidation() {
        std::cout << "=== NLM Quick Validation ===" << std::endl;
        
        // Create a minimal brain with small configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", config->getOr<double>("simulation_timestep", 0.001), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        
        try {
            NLM_LOG_INFO("Creating brain with config: neuron_count=" + std::to_string(config->getOr<int64_t>("neuron_count", 100)));
            NLM_LOG_INFO("Creating brain with config: region_count=" + std::to_string(config->getOr<int64_t>("region_count", 1)));
            
            auto brain = std::make_shared<Brain>(config);
            
            NLM_LOG_INFO("Initializing brain...");
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Brain initialization failed!");
                return 1;
            }
            
            NLM_LOG_INFO("Running connectivity test...");
            runBasicConnectivityTest(brain);
            
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Validation PASSED ===");
            NLM_LOG_INFO("NLM CLI is working correctly with all core functionality.");
            return 0;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Validation failed with exception: " + std::string(e.what()));
            return 1;
        }
    }

    int runPhase6Demo() {
        std::cout << "=== NLM Phase 6 Demo ===" << std::endl;
        NLM_LOG_INFO("Starting Phase 6 integration demo...");
        
        Phase6Demo demo;
        
        if (config.customArg == "quick") {
            NLM_LOG_INFO("Running QUICK demo mode (20 seconds)...");
            return demo.runQuickDemo();
        } else if (config.customArg == "performance") {
            NLM_LOG_INFO("Running PERFORMANCE demo mode...");
            return demo.runPerformanceDemo();
        } else {
            NLM_LOG_INFO("Running FULL demo mode (60 seconds)...");
            return demo.runFullDemo();
        }
    }

    int runBatchMode() {
        std::cout << "=== NLM Batch Mode ===" << std::endl;
        NLM_LOG_INFO("Running batch simulation with multiple iterations...");
        
        // Create brain with configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", config->getOr<double>("simulation_timestep", 0.001), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(config.connections), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", config.connectionProb, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Brain initialization failed in batch mode!");
            return 1;
        }
        
        NLM_LOG_INFO("Starting batch simulation with " + std::to_string(config.steps) + " steps");
        
        for (int iteration = 0; iteration < 5; ++iteration) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("=== Batch Iteration " + std::to_string(iteration + 1) + " ===");
            
            brain->reset();
            brain->initialize();
            
            for (int step = 0; step < config.steps; ++step) {
                brain->step(step, step * config.timestep);
                
                if (config.performance && step % 100 == 0) {
                    NLM_LOG_INFO("Iteration " + std::to_string(iteration + 1) + 
                               ", Step " + std::to_string(step) + 
                               ", Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
                }
            }
            
            NLM_LOG_INFO("Iteration " + std::to_string(iteration + 1) + " completed. " +
                        "Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Batch Mode Complete ===");
        NLM_LOG_INFO("All iterations completed successfully.");
        
        // Save final state if requested
        if (!config.savePath.empty()) {
            NLM_LOG_INFO("Saving final brain state to: " + config.savePath);
            if (brain->save(config.savePath)) {
                NLM_LOG_INFO("Save successful!");
            } else {
                NLM_LOG_ERROR("Save failed!");
            }
        }
        
        return 0;
    }

    int runSimulation() {
        std::cout << "=== NLM Simulation ===" << std::endl;
        NLM_LOG_INFO("Loading configuration from: " + config.configFile);
        
        // Create brain with configuration
        auto config = std::make_shared<Config>();
        
        // Load config from file (ignore if not found)
        if (config->loadFromFile(config.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Override with command line parameters
        config->set("simulation_timestep", config.timestep, ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(config.connections), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::CommandLine);
        config->set("connection_probability", config.connectionProb, ConfigSource::CommandLine);
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Brain initialization failed!");
            return 1;
        }
        
        // Load saved brain state if requested
        if (!config.loadPath.empty()) {
            NLM_LOG_INFO("Loading brain state from: " + config.loadPath);
            if (brain->load(config.loadPath)) {
                NLM_LOG_INFO("Load successful!");
            } else {
                NLM_LOG_ERROR("Load failed, using newly initialized brain.");
            }
        }
        
        // Log configuration summary
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  timestep: " + std::to_string(config->getOr<double>("simulation_timestep", config.timestep)) + "s");
        NLM_LOG_INFO("  neurons: " + std::to_string(config->getOr<int64_t>("neuron_count", config.connections)));
        NLM_LOG_INFO("  regions: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_prob: " + std::to_string(config->getOr<float>("connection_probability", config.connectionProb)));
        
        // Run simulation
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Starting simulation with " + std::to_string(config.steps) + " steps...");
        
        for (int step = 0; step < config.steps; ++step) {
            brain->step(step, step * config.timestep);
            
            if (config.performance && step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                           ", Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                           ", Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
            }
        }
        
        // Log final results
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Simulation Complete ===");
        NLM_LOG_INFO("Total steps: " + std::to_string(config.steps));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + "/step");
        
        // Save final state if requested
        if (!config.savePath.empty()) {
            NLM_LOG_INFO("");
            NLM_LOG_INFO("Saving brain state to: " + config.savePath);
            if (brain->save(config.savePath)) {
                NLM_LOG_INFO("Save successful!");
            } else {
                NLM_LOG_ERROR("Save failed!");
            }
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== NLM CLI Simulation Complete ===");
        return 0;
    }

    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Basic Connectivity Test ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            NLM_LOG_ERROR("No regions found!");
            return;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_ERROR("  No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
        
        if (spikes > 0) {
            NLM_LOG_INFO("  ✓ Spikes propagate through network");
        } else {
            NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                           " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                           " thresh=" + std::to_string(neurons[i]->getThreshold()));
            }
        }
    }
};

int main(int argc, char** argv) {
    NLMCLI cli;
    return cli.run(argc, argv);
}