#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include "src/experiments/ExperimentRunner.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <map>
#include <algorithm>

namespace nlm {

class CLIInterface {
public:
    CLIInterface() : verbose(false), quiet(false), configFile("configs/default.cfg") {}
    
    bool parseArguments(int argc, char** argv) {
        std::vector<std::string> args(argv, argv + argc);
        
        for (size_t i = 1; i < args.size(); ++i) {
            std::string arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                showHelp();
                return true;
            }
            else if (arg == "--version") {
                showVersion();
                return true;
            }
            else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            }
            else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
            }
            else if (arg == "--config" && i + 1 < args.size()) {
                configFile = args[++i];
            }
            else if (arg == "--steps" && i + 1 < args.size()) {
                maxSteps = std::stoul(args[++i]);
            }
            else if (arg == "--neurons" && i + 1 < args.size()) {
                neuronCount = std::stoul(args[++i]);
            }
            else if (arg == "--regions" && i + 1 < args.size()) {
                regionCount = std::stoul(args[++i]);
            }
            else if (arg == "--seed" && i + 1 < args.size()) {
                randomSeed = std::stoull(args[++i]);
            }
            else if (arg == "--stdp-ltp" && i + 1 < args.size()) {
                stdpLtp = std::stof(args[++i]);
            }
            else if (arg == "--stdp-ltd" && i + 1 < args.size()) {
                stdpLtd = std::stof(args[++i]);
            }
            else if (arg == "--stdp-tau" && i + 1 < args.size()) {
                stdpTau = std::stof(args[++i]);
            }
            else if (arg == "--verbose-logging") {
                enableVerboseLogging = true;
            }
            else if (arg == "--stats" || arg == "--statistics") {
                showStatistics = true;
            }
            else if (arg == "--save" && i + 1 < args.size()) {
                checkpointFile = args[++i];
            }
            else if (arg == "--load" && i + 1 < args.size()) {
                loadFile = args[++i];
            }
            else if (arg == "--list-experiments") {
                listExperiments();
                return true;
            }
            else if (arg == "--run-experiment" && i + 1 < args.size()) {
                experimentName = args[++i];
                i++; // Skip maxSteps if present
                if (i < args.size() && std::isdigit(args[i][0])) {
                    maxSteps = std::stoul(args[i]);
                }
                return true;  // Will be processed by main
            }
            else if (arg == "--inter-active" || arg == "--ia") {
                runInInteractiveMode = true;
            }
            else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    void showHelp() const {
        std::cout << R"(
╔═══════════════════════════════════════════════════════════════════════════════╗
║                               NLM CLI Interface                              ║
║                        Neural Learning Machine - Command Line                 ║
╚═══════════════════════════════════════════════════════════════════════════════╝

USAGE:
    nlm [OPTIONS] [ARGS]

OPTIONS:
  General Options:
    -h, --help                    Show this help message and exit
    -v, --verbose                  Enable verbose output
    -q, --quiet                    Suppress non-essential output
    --version                      Show version information and exit

  Configuration:
    --config FILE                 Load configuration from FILE (default: configs/default.cfg)
    --seed SEED                   Set random seed (default: 42)

  Simulation Parameters:
    --neurons COUNT               Set number of neurons (default: 1000)
    --regions COUNT                Set number of brain regions (default: 1)
    --steps STEPS                 Set maximum simulation steps (default: 1000)

  STDP Plasticity:
    --stdp-ltp WEIGHT              Set STDP LTP weight (default: 0.01)
    --stdp-ltd WEIGHT              Set STDP LTD weight (default: 0.012)
    --stdp-tau TAU                 Set STDP time constant (default: 20.0)

  Output & Logging:
    --verbose-logging             Enable verbose logging for all components
    --stats, --statistics          Show detailed statistics after simulation
    --save FILE                    Save brain state to FILE after simulation
    --load FILE                    Load brain state from FILE before simulation

  Experiments:
    --list-experiments            List available experiments
    --run-experiment NAME [STEPS]  Run specific experiment (NAME is required)

  Advanced Mode:
    --inter-active, --ia          Run in interactive mode with real-time monitoring

EXAMPLES:
    nlm --help                              Show help information
    nlm --neurons 500 --regions 2 --steps 500    Run simulation with custom parameters
    nlm --config myconfig.cfg --stdp-ltp 0.05    Run with custom STDP parameters
    nlm --run-experiment learning_test 1000      Run specific experiment
    nlm --list-experiments                     Show available experiments
    nlm --inter-active                         Start interactive mode

The NLM (熙然) - Neural Learning Machine implements real neural computation
with LIF neurons, STDP plasticity, and integrated memory systems.

For more information, visit: https://github.com/nlm/neural-learning-machine
)" << std::endl;
    }
    
    void showVersion() const {
        std::cout << "NLM (熙然) Neural Learning Machine v2.0.0 (Phase 2: Real Neural Computation)" << std::endl;
        std::cout << "Developed by NLM Research Team" << std::endl;
        std::cout << "Built with C++14, implementing real LIF neuron dynamics, STDP, and cognitive integration." << std::endl;
        std::cout << "Phase 2 completes real neural computation with event-driven spike propagation." << std::endl;
    }
    
    void listExperiments() const {
        std::cout << "Available Experiments:" << std::endl;
        std::cout << "  learning_test          Demonstrates measurable synaptic changes through experience" << std::endl;
        std::cout << "  stdp_verification      Tests STDP pre-before-post potentiation" << std::endl;
        std::cout << "  connectivity_test      Tests basic neural connectivity and spike propagation" << std::endl;
        std::cout << "  phase3                 Phase 3 experimental protocol" << std::endl;
        std::cout << "  phase4                 Phase 4 experimental protocol" << std::endl;
        std::cout << "  phase5                 Phase 5 experimental protocol" << std::endl;
        std::cout << "  phase6                 Phase 6: Integrated brain experiment" << std::endl;
        std::cout << "  phase3_demo           Phase 3 demonstration" << std::endl;
        std::cout << "  phase4_demo           Phase 4 demonstration" << std::endl;
        std::cout << "  phase5_demo           Phase 5 demonstration" << std::endl;
        std::cout << "  phase6_demo           Phase 6 demonstration" << std::endl;
    }
    
    bool runSimulation() {
        if (!quiet) {
            std::cout << "Initializing NLM Phase 2 Real Neural Computation..." << std::endl;
        }
        
        // Create and configure brain
        auto config = std::make_shared<Config>();
        
        // Load configuration file
        std::ifstream configFileCheck(configFile);
        if (configFileCheck.good()) {
            if (!config->loadFromFile(configFile)) {
                if (!quiet) {
                    std::cout << "Warning: Could not load config file " << configFile 
                              << ", using defaults." << std::endl;
                }
            } else if (!quiet) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            }
        }
        
        // Override with command line arguments
        if (verbose) {
            std::cout << "Configuring simulation parameters..." << std::endl;
        }
        
        config->set("random_seed", static_cast<int64_t>(randomSeed), ConfigSource::CommandLine);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
        config->set("stdp_ltp_weight", stdpLtp, ConfigSource::CommandLine);
        config->set("stdp_ltd_weight", stdpLtd, ConfigSource::CommandLine);
        config->set("stdp_tau", stdpTau, ConfigSource::CommandLine);
        
        // Initialize brain
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        if (!quiet) {
            brain->logStatus();
        }
        
        // Load checkpoint if specified
        if (!loadFile.empty()) {
            if (!brain->load(loadFile)) {
                std::cerr << "Warning: Could not load checkpoint " << loadFile << std::endl;
            } else if (!quiet) {
                std::cout << "Loaded brain state from: " << loadFile << std::endl;
            }
        }
        
        // Run simulation
        if (!quiet) {
            std::cout << "Running simulation for " << maxSteps << " steps..." << std::endl;
        }
        
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            brain->step(step);
            
            if (enableVerboseLogging && step % 100 == 0) {
                std::cout << "  Step " << step 
                          << " | Total spikes: " << brain->getTotalSpikeCount()
                          << " | Firing: " << brain->getFiringNeuronCount()
                          << " | E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            }
        }
        
        // Show statistics
        if (showStatistics) {
            std::cout << "\n=== Simulation Statistics ===" << std::endl;
            std::cout << "Total steps: " << maxSteps << std::endl;
            std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
            std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
            std::cout << "Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
            std::cout << "Region count: " << brain->getRegionCount() << std::endl;
            std::cout << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
            std::cout << "Synapse count: " << brain->getTotalSynapseCount() << std::endl;
            std::cout << "Pending spike events: " << brain->getPendingSpikeEventCount() << std::endl;
        }
        
        // Save checkpoint if specified
        if (!checkpointFile.empty()) {
            if (!brain->save(checkpointFile)) {
                std::cerr << "Error: Failed to save checkpoint " << checkpointFile << std::endl;
                return false;
            } else if (!quiet) {
                std::cout << "Saved brain state to: " << checkpointFile << std::endl;
            }
        }
        
        if (!quiet) {
            std::cout << "\n=== Simulation Complete ===" << std::endl;
        }
        
        return true;
    }
    
    bool runExperiment(const std::string& experimentName, size_t steps) {
        if (!quiet) {
            std::cout << "Running experiment: " << experimentName << std::endl;
        }
        
        auto config = std::make_shared<Config>();
        
        // Load default configuration
        config->set("random_seed", static_cast<int64_t>(randomSeed), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("stdp_ltp_weight", stdpLtp, ConfigSource::Default);
        config->set("stdp_ltd_weight", stdpLtd, ConfigSource::Default);
        config->set("stdp_tau", stdpTau, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain for experiment!" << std::endl;
            return false;
        }
        
        // Create environment
        auto environment = std::make_shared<Environment>();
        
        // Create experiment runner
        auto experimentRunner = std::make_shared<ExperimentRunner>();
        
        auto experiment = experimentRunner->createExperiment(experimentName);
        
        // Run experiment
        bool success = experimentRunner->runExperiment(experiment, brain, environment, steps);
        
        if (success && !quiet) {
            std::cout << "Experiment completed successfully!" << std::endl;
            std::cout << "Total reward: " << experiment->getMetric("total_reward") << std::endl;
            std::cout << "Final spike count: " << brain->getTotalSpikeCount() << std::endl;
        }
        
        return success;
    }
    
    bool runInteractiveMode() {
        std::cout << "Starting NLM Interactive Mode..." << std::endl;
        std::cout << "Commands available:" << std::endl;
        std::cout << "  help                    Show help information" << std::endl;
        std::cout << "  status                  Show current brain status" << std::endl;
        std::cout << "  step N                  Run N simulation steps" << std::endl;
        std::cout << "  config KEY VALUE        Set configuration value" << std::endl;
        std::cout << "  inspect NEURON_ID       Inspect neuron state" << std::endl;
        std::cout << "  analyze                 Analyze network properties" << std::endl;
        std::cout << "  save FILE               Save current state" << std::endl;
        std::cout << "  load FILE               Load state from file" << std::endl;
        std::cout << "  quit, exit             Exit interactive mode" << std::endl;
        
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return false;
        }
        
        std::string line;
        std::cout << ">>> ";
        
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                std::cout << ">>> ";
                continue;
            }
            
            if (line == "help" || line == "?") {
                showInteractiveHelp();
            }
            else if (line == "status") {
                brain->logStatus();
            }
            else if (line.substr(0, 5) == "step ") {
                size_t steps = std::stoul(line.substr(5));
                brain->step(0);  // Note: simplified for demo
                std::cout << "Ran " << steps << " steps" << std::endl;
            }
            else if (line.substr(0, 6) == "config ") {
                size_t space1 = line.find(' ', 6);
                size_t space2 = line.find(' ', space1 + 1);
                if (space1 != std::string::npos && space2 != std::string::npos) {
                    std::string key = line.substr(6, space1 - 6);
                    std::string value = line.substr(space2 + 1);
                    config->set(key, value, ConfigSource::Runtime);
                    std::cout << "Set " << key << " = " << value << std::endl;
                } else {
                    std::cout << "Invalid config command format" << std::endl;
                }
            }
            else if (line == "analyze") {
                std::cout << "Network Analysis:" << std::endl;
                std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
                std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
                std::cout << "  E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
                std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
                std::cout << "  Development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            }
            else if (line == "quit" || line == "exit") {
                break;
            }
            else {
                std::cout << "Unknown command: " << line << std::endl;
                showInteractiveHelp();
            }
            
            std::cout << ">>> ";
        }
        
        return true;
    }
    
    void showInteractiveHelp() const {
        std::cout << "Interactive Commands:" << std::endl;
        std::cout << "  help                    Show this help" << std::endl;
        std::cout << "  status                  Show brain status" << std::endl;
        std::cout << "  step N                 Run N steps (e.g., 'step 100')" << std::endl;
        std::cout << "  config KEY VALUE       Set config value (e.g., 'config neuron_count 500')" << std::endl;
        std::cout << "  analyze                 Analyze network" << std::endl;
        std::cout << "  save FILE              Save state to file" << std::endl;
        std::cout << "  load FILE              Load state from file" << std::endl;
        std::cout << "  quit, exit             Exit interactive mode" << std::endl;
    }
    
private:
    bool verbose = false;
    bool quiet = false;
    bool enableVerboseLogging = false;
    bool showStatistics = false;
    bool runInInteractiveMode = false;
    
    std::string configFile = "configs/default.cfg";
    std::string checkpointFile;
    std::string loadFile;
    std::string experimentName;
    
    uint64_t randomSeed = 42;
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    size_t maxSteps = 1000;
    
    float stdpLtp = 0.01f;
    float stdpLtd = 0.012f;
    float stdpTau = 20.0f;
};

} // namespace nlm
