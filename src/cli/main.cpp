// NLM Brain Simulation CLI
#include "nlm_cli.h"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"
#include "world/SimpleWorld.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <map>
#include <chrono>
#include <thread>

using namespace nlm;
using namespace std::chrono;

// Forward declarations for CLI commands
class SimulationController;
class ConfigManager;
class DataExporter;
class ExperimentRunner;
class ProfileMonitor;

// Configuration defaults
struct CLIConfig {
    std::string configFile = "configs/default.cfg";
    std::string outputDir = ".";
    bool verbose = false;
    std::string logLevel = "INFO";
    bool jsonOutput = false;
    std::string profileOutput = "profile.json";
    std::string experimentOutput = "experiment_results.json";
    int maxSteps = 1000;
    double timestep = 0.001;
};

class NLMCLI {
public:
    NLMCLI(int argc, char** argv) : argc(argc), argv(argv) {}
    
    int run() {
        parseArgs();
        
        if (helpRequested) {
            printHelp();
            return 0;
        }
        
        if (versionRequested) {
            printVersion();
            return 0;
        }
        
        // Initialize configuration
        config = std::make_shared<Config>();
        
        // Load configuration
        if (configFileProvided && !config->loadFromFile(configFile)) {
            std::cerr << "Warning: Failed to load config file: " << configFile << std::endl;
            std::cerr << "Using default configuration." << std::endl;
        }
        
        // Load config from command line args
        config->loadFromArgs(argc, argv);
        
        // Set defaults based on command line options
        if (neuronCount) config->set("neuron_count", *neuronCount, ConfigSource::CommandLine);
        if (timestep) config->set("simulation_timestep", *timestep, ConfigSource::CommandLine);
        if (maxSteps) config->set("max_simulation_steps", *maxSteps, ConfigSource::CommandLine);
        
        // Run commands based on subcommand
        if (subcommand == "run") {
            return runSimulation();
        } else if (subcommand == "experiment") {
            return runExperiment();
        } else if (subcommand == "config") {
            return handleConfigCommand();
        } else if (subcommand == "export") {
            return handleExportCommand();
        } else if (subcommand == "import") {
            return handleImportCommand();
        } else if (subcommand == "profile") {
            return runProfile();
        } else if (subcommand == "test") {
            return runTests();
        } else if (subcommand == "monitor") {
            return runMonitor();
        } else if (subcommand == "benchmark") {
            return runBenchmark();
        } else {
            std::cerr << "Error: Unknown subcommand: " << subcommand << std::endl;
            printHelp();
            return 1;
        }
    }
    
private:
    int argc;
    char** argv;
    std::shared_ptr<Config> config;
    
    // Command line options
    std::string subcommand = "run";
    bool helpRequested = false;
    bool versionRequested = false;
    bool configFileProvided = false;
    std::string configFile;
    std::optional<int64_t> neuronCount;
    std::optional<double> timestep;
    std::optional<int> maxSteps;
    bool verbose = false;
    std::string logLevel = "INFO";
    std::string outputFile;
    
    // Additional options
    std::string experimentName;
    std::string exportFormat;
    std::string importFile;
    std::shared_ptr<Brain> brain;
    
    void parseArgs() {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpRequested = true;
            } else if (arg == "--version") {
                versionRequested = true;
            } else if (arg == "--config" || arg == "-c") {
                configFileProvided = true;
                if (i + 1 < argc) {
                    configFile = argv[++i];
                }
            } else if (arg == "--neuron-count" || arg == "-n") {
                if (i + 1 < argc) {
                    neuronCount = std::stoll(argv[++i]);
                }
            } else if (arg == "--timestep" || arg == "-t") {
                if (i + 1 < argc) {
                    timestep = std::stod(argv[++i]);
                }
            } else if (arg == "--max-steps") {
                if (i + 1 < argc) {
                    maxSteps = std::stoi(argv[++i]);
                }
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--log-level") {
                if (i + 1 < argc) {
                    logLevel = argv[++i];
                }
            } else if (arg == "--output" || arg == "-o") {
                if (i + 1 < argc) {
                    outputFile = argv[++i];
                }
            } else if (arg == "--output-dir") {
                if (i + 1 < argc) {
                    config->set("output_dir", argv[++i], ConfigSource::CommandLine);
                }
            } else if (arg.find("--set=") == 0) {
                std::string setting = arg.substr(6);
                size_t pos = setting.find('=');
                if (pos != std::string::npos) {
                    std::string key = setting.substr(0, pos);
                    std::string value = setting.substr(pos + 1);
                    config->set(key, value, ConfigSource::CommandLine);
                }
            } else if (arg.find("--experiment=") == 0) {
                experimentName = arg.substr(13);
                subcommand = "experiment";
            } else if (arg.find("--export=") == 0) {
                exportFormat = arg.substr(8);
                subcommand = "export";
            } else if (arg.find("--import=") == 0) {
                importFile = arg.substr(9);
                subcommand = "import";
            } else if (arg == "run" || arg == "experiment" || arg == "config" || arg == "export" || 
                       arg == "import" || arg == "profile" || arg == "test" || arg == "monitor" || 
                       arg == "benchmark") {
                subcommand = arg;
            } else if (!arg.empty() && arg[0] != '-') {
                // Assume it's a config file if it looks like a filename
                if (configFile.empty() && (arg.find(".cfg") != std::string::npos || arg.find("config") != std::string::npos)) {
                    configFileProvided = true;
                    configFile = arg;
                }
            }
        }
        
        if (verbose && logLevel.empty()) {
            logLevel = "DEBUG";
        }
    }
    
    int runSimulation() {
        std::cout << "=== Running NLM Brain Simulation ===" << std::endl;
        
        // Initialize brain
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        brain->logStatus();
        
        std::cout << "Running simulation for " << maxSteps.value_or(1000) << " steps..." << std::endl;
        
        int steps = maxSteps.value_or(1000);
        double timeStep = timestep.value_or(config->getOr<double>("simulation_timestep", 0.001));
        
        for (int step = 0; step < steps; ++step) {
            brain->step(step, timeStep);
            
            if (verbose && step % 100 == 0) {
                std::cout << "  Step: " << step << ", Spikes: " << brain->getTotalSpikeCount() 
                          << ", Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        brain->logStatus();
        
        // Save final state if output file specified
        if (!outputFile.empty()) {
            saveBrainState(outputFile);
        }
        
        std::cout << "Simulation complete!" << std::endl;
        return 0;
    }
    
    int runExperiment() {
        std::cout << "=== Running NLM Experiment: " << experimentName << " ===" << std::endl;
        
        if (!brain) {
            brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                std::cerr << "Error: Failed to initialize brain for experiment!" << std::endl;
                return 1;
            }
        }
        
        // Run basic experiments
        if (experimentName == "connectivity") {
            std::cout << "Running connectivity test..." << std::endl;
            return runBasicConnectivityTest(brain);
        } else if (experimentName == "plasticity") {
            std::cout << "Running plasticity experiment..." << std::endl;
            return runPlasticityExperiment(brain);
        } else if (experimentName == "stdp") {
            std::cout << "Running STDP verification..." << std::endl;
            return runStdpVerification(brain);
        } else if (experimentName == "phase2") {
            std::cout << "Running complete Phase 2 demonstration..." << std::endl;
            return runPhase2Demonstration(brain);
        } else {
            std::cerr << "Error: Unknown experiment: " << experimentName << std::endl;
            return 1;
        }
    }
    
    int runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        std::cout << "" << std::endl;
        std::cout << "=== Test 1: Basic Neural Connectivity ===" << std::endl;
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            std::cout << "  No neurons found!" << std::endl;
            return 0;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        std::cout << "  Injecting current into 10 neurons..." << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        std::cout << "  Spikes generated: " << spikes << std::endl;
        
        if (spikes > 0) {
            std::cout << "  ✓ Spikes propagate through network" << std::endl;
        } else {
            std::cout << "  ! No spikes - checking neuron parameters..." << std::endl;
            for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
                std::cout << "    Neuron " << i 
                          << " V=" << neurons[i]->getMembranePotential() 
                          << " thresh=" << neurons[i]->getThreshold() << std::endl;
            }
        }
        
        return 0;
    }
    
    int runPlasticityExperiment(std::shared_ptr<Brain> brain) {
        std::cout << "" << std::endl;
        std::cout << "=== Test 2: Plasticity Learning Experiment ===" << std::endl;
        
        // Record initial state
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        std::cout << "  Applying repeated input patterns (1000 steps)..." << std::endl;
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                std::cout << "  Step " << step 
                          << " | Spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount() << std::endl;
            }
        }
        
        std::cout << "  Initial synapses: " << initialSynapseCount << std::endl;
        std::cout << "  Final synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        return 0;
    }
    
    int runStdpVerification(std::shared_ptr<Brain> brain) {
        std::cout << "" << std::endl;
        std::cout << "=== Test 3: STDP Verification ===" << std::endl;
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return 0;
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            std::cout << "  Not enough synapses for STDP test" << std::endl;
            return 0;
        }
        
        std::cout << "  Testing STDP on 5 synapses:" << std::endl;
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            std::cout << "    Synapse " << i 
                      << " before: " << beforeWeights[i] << std::endl;
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        std::cout << "" << std::endl;
        std::cout << "  Creating correlated pre->post activity (potentiation)..." << std::endl;
        
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        std::cout << "  After correlated activity:" << std::endl;
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            std::cout << "    Synapse " << i 
                      << " after: " << synapses[i]->getWeight()
                      << " (Δ=" << delta << ")" << std::endl;
        }
        
        // Check if weights increased (LTP)
        float totalDelta = 0.0f;
        for (size_t i = 0; i < 5; ++i) {
            totalDelta += synapses[i]->getWeight() - beforeWeights[i];
        }
        
        std::cout << "" << std::endl;
        if (totalDelta > 0.001f) {
            std::cout << "  ✓ STDP WORKING: Pre-before-post produced potentiation" << std::endl;
        } else if (totalDelta < -0.001f) {
            std::cout << "  ! STDP reversed: Check parameters" << std::endl;
        } else {
            std::cout << "  ! No change: STDP may not be triggering" << std::endl;
        }
        
        return 0;
    }
    
    int runPhase2Demonstration(std::shared_ptr<Brain> brain) {
        std::cout << "" << std::endl;
        std::cout << "=== Complete Phase 2 Demonstration ===" << std::endl;
        
        // Run all three tests in sequence
        runBasicConnectivityTest(brain);
        
        brain->reset();
        brain->initialize();
        runPlasticityExperiment(brain);
        
        brain->reset();
        brain->initialize();
        runStdpVerification(brain);
        
        return 0;
    }
    
    int handleConfigCommand() {
        std::cout << "=== NLM Configuration Management ===" << std::endl;
        
        std::string action = "list";
        if (!configFileProvided) {
            // Look for --generate, --validate, etc.
            for (int i = 1; i < argc; ++i) {
                if (std::string(argv[i]) == "--generate") {
                    action = "generate";
                    break;
                } else if (std::string(argv[i]) == "--validate") {
                    action = "validate";
                    break;
                }
            }
        }
        
        if (action == "generate") {
            return generateConfigFile();
        } else if (action == "validate") {
            return validateConfigFile();
        } else {
            // List current config
            std::cout << "Current Configuration:" << std::endl;
            std::cout << config->summary() << std::endl;
            return 0;
        }
    }
    
    int handleExportCommand() {
        std::cout << "=== Exporting NLM Data ===" << std::endl;
        
        if (exportFormat.empty()) {
            std::cerr << "Error: No export format specified. Use --export=<format>" << std::endl;
            return 1;
        }
        
        if (!brain) {
            std::cerr << "Error: No brain loaded for export" << std::endl;
            return 1;
        }
        
        return exportBrainData(exportFormat);
    }
    
    int handleImportCommand() {
        std::cout << "=== Importing NLM Data ===" << std::endl;
        
        if (importFile.empty()) {
            std::cerr << "Error: No import file specified. Use --import=<file>" << std::endl;
            return 1;
        }
        
        // TODO: Implement brain state import
        std::cerr << "Error: Brain state import not yet implemented!" << std::endl;
        return 1;
    }
    
    int runProfile() {
        std::cout << "=== Profiling NLM Simulation ===" << std::endl;
        
        auto startTime = high_resolution_clock::now();
        
        // TODO: Implement profiling
        std::cerr << "Error: Profiling not yet implemented!" << std::endl;
        
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(endTime - startTime);
        
        std::cout << "Profile duration: " << duration.count() << " ms" << std::endl;
        
        return 0;
    }
    
    int runTests() {
        std::cout << "=== Running NLM Tests ===" << std::endl;
        
        // TODO: Implement test running
        std::cerr << "Error: Test runner not yet implemented!" << std::endl;
        return 1;
    }
    
    int runMonitor() {
        std::cout << "=== Monitoring NLM Brain ===" << std::endl;
        
        // TODO: Implement monitoring
        std::cerr << "Error: Monitor not yet implemented!" << std::endl;
        return 1;
    }
    
    int runBenchmark() {
        std::cout << "=== Running NLM Benchmarks ===" << std::endl;
        
        // TODO: Implement benchmarking
        std::cerr << "Error: Benchmark not yet implemented!" << std::endl;
        return 1;
    }
    
    int generateConfigFile() {
        std::cout << "=== Generating Default Configuration ===" << std::endl;
        
        std::string filename = outputFile.empty() ? "configs/default.cfg" : outputFile;
        
        if (config->saveToFile(filename)) {
            std::cout << "Successfully generated configuration file: " << filename << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to save configuration file: " << filename << std::endl;
            return 1;
        }
    }
    
    int validateConfigFile() {
        std::cout << "=== Validating Configuration ===" << std::endl;
        
        if (!configFileProvided) {
            std::cerr << "Error: No configuration file to validate" << std::endl;
            return 1;
        }
        
        auto tempConfig = std::make_shared<Config>();
        if (tempConfig->loadFromFile(configFile)) {
            std::cout << "Configuration file validated successfully" << std::endl;
            std::cout << "Keys: ";
            auto keys = tempConfig->getKeys();
            for (size_t i = 0; i < keys.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << keys[i];
            }
            std::cout << std::endl;
            return 0;
        } else {
            std::cerr << "Error: Failed to validate configuration file: " << configFile << std::endl;
            return 1;
        }
    }
    
    void saveBrainState(const std::string& filename) {
        std::cout << "Saving brain state to: " << filename << std::endl;
        
        // TODO: Implement brain state serialization
        std::cerr << "Warning: Brain state saving not yet implemented!" << std::endl;
    }
    
    int exportBrainData(const std::string& format) {
        std::cout << "Exporting brain data in format: " << format << std::endl;
        
        // TODO: Implement brain data export in different formats
        std::cerr << "Error: Brain data export for format '" << format << "' not yet implemented!" << std::endl;
        return 1;
    }
    
    void printHelp() {
        std::cout << R"(
NLM Brain Simulation CLI
========================

USAGE:
    nlm_cli [OPTIONS] <COMMAND> [ARGS...]

COMMANDS:
    run                    Run a simple brain simulation
    experiment <name>      Run a predefined experiment
    config                  Configuration management
    export <format>        Export brain state to various formats
    import <file>           Import brain state from a file
    profile                 Profile performance metrics
    test                    Run unit/integration tests
    monitor                 Monitor brain status in real-time
    benchmark               Run performance benchmarks
    
CONFIGURATION OPTIONS:
    --config <file>         Configuration file (default: configs/default.cfg)
    --set <key>=<value>     Set configuration value via command line
    --neuron-count <n>      Override number of neurons
    --timestep <t>          Simulation timestep
    --max-steps <n>         Maximum simulation steps
    
OUTPUT OPTIONS:
    --output <file>         Output file for brain state or results
    --output-dir <dir>      Directory for output files
    --verbose, -v           Enable verbose output
    --log-level <level>     Set log level (DEBUG, INFO, WARN, ERROR)
    
GENERAL OPTIONS:
    --help, -h              Show this help message
    --version               Show version information

EXAMPLES:
    Basic simulation:
        nlm_cli run --neuron-count 100 --max-steps 500

    With custom config:
        nlm_cli run --config my_config.cfg

    Run experiment:
        nlm_cli experiment plasticity_experiment

    Export results:
        nlm_cli export json --output results.json

    Profile performance:
        nlm_cli profile --output profile.json
    )" << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM CLI Version 1.0.0" << std::endl;
        std::cout << "Built for NLM Phase 2: Real Neural Computation" << std::endl;
    }
};

int main(int argc, char** argv) {
    NLMCLI cli(argc, argv);
    return cli.run();
}

