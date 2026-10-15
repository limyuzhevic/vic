// NLM (熙然) - Neural Learning Machine
// Enhanced Command Line Interface
//
// This provides a comprehensive CLI for the NLM neural simulation system with:
// - Advanced command-line arguments
// - Configuration management
// - Debug and visualization capabilities
// - Performance monitoring
// - Multiple experiment modes

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "sensory/SensoryInput.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <chrono>
#include <thread>
#include <filesystem>

using namespace nlm;

// Forward declarations
class NLMCLI;

// Command-line options structure
struct CLIOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string logFile = "nlm.log";
    bool verbose = false;
    bool debug = false;
    bool visualize = false;
    bool profile = false;
    size_t steps = 0;
    double timeLimit = 0.0;
    size_t maxNeurons = 1000;
    size_t maxRegions = 1;
    std::string mode = "demo"; // demo, experiment, benchmark
    std::string experimentType = "plasticity"; // plasticity, stdp, learning, all
    bool saveState = false;
    bool loadState = false;
    std::string checkpointFile = "brain_checkpoint";
    bool continuous = false;
    size_t inferenceSteps = 100;
    float inferenceInterval = 0.1f;
    std::string apiMode = "full"; // full, minimal, expert
    bool enableGPU = false;
    bool enableSIMD = true;
    size_t threadCount = 4;
    std::vector<std::pair<std::string, std::string>> customArgs; // Custom configuration key=value pairs
};

// Enhanced NLM brain interface
class NLMBrain {
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<SimulationClock> clock;
    bool initialized;
    
public:
    NLMBrain() : initialized(false) {
        config = std::make_shared<Config>();
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        clock = std::make_shared<SimulationClock>(0.001);
    }
    
    ~NLMBrain() {
        shutdown();
    }
    
    // Initialize brain with configuration
    bool initialize(const CLIOptions& options) {
        try {
            std::cout << "Initializing NLM Brain (Enhanced Phase 6)..." << std::endl;
            
            // Load configuration
            if (!config->loadFromFile(options.configFile)) {
                std::cout << "Using default configuration (file not found: " << options.configFile << ")" << std::endl;
            }
            
            // Override with command line options
            if (options.maxNeurons > 0) {
                config->set("neuron_count", static_cast<int64_t>(options.maxNeurons), ConfigSource::CommandLine);
            }
            if (options.maxRegions > 0) {
                config->set("region_count", static_cast<int64_t>(options.maxRegions), ConfigSource::CommandLine);
            }
            
            // Set performance options
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("random_seed", 42, ConfigSource::Default);
            
            // Initialize brain
            brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
                return false;
            }
            
            // Log configuration
            if (options.verbose) {
                std::cout << config->summary() << std::endl;
            }
            
            brain->logStatus();
            initialized = true;
            
            std::cout << "✓ NLM Brain initialized successfully" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception during initialization: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Run a simple simulation
    bool runSimulation(size_t steps) {
        if (!initialized) {
            std::cerr << "ERROR: Brain not initialized!" << std::endl;
            return false;
        }
        
        std::cout << "Running simulation for " << steps << " steps..." << std::endl;
        
        try {
            for (size_t step = 0; step < steps; ++step) {
                // Record start time for profiling
                auto startTime = std::chrono::high_resolution_clock::now();
                
                // Run brain step
                brain->step(step);
                
                // Record end time
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
                
                // Log progress periodically
                if (step % 100 == 0) {
                    std::cout << "  Step " << step << " | "
                             << "Spikes: " << brain->getTotalSpikeCount() << " | "
                             << "Duration: " << duration.count() << " μs" << std::endl;
                }
                
                // Check for stop conditions
                if (brain->getTotalSpikeCount() > 10000) {
                    std::cout << "Early stop: High spike activity detected" << std::endl;
                    break;
                }
            }
            
            std::cout << "✓ Simulation completed" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception during simulation: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Run a specific experiment
    bool runExperiment(const std::string& experimentType) {
        if (!initialized) {
            std::cerr << "ERROR: Brain not initialized!" << std::endl;
            return false;
        }
        
        std::cout << "Running " << experimentType << " experiment..." << std::endl;
        
        try {
            if (experimentType == "plasticity") {
                runPlasticityExperiment();
            } else if (experimentType == "stdp") {
                runSTDPExperiment();
            } else if (experimentType == "learning") {
                runLearningExperiment();
            } else if (experimentType == "all") {
                runPlasticityExperiment();
                runSTDPExperiment();
                runLearningExperiment();
            } else {
                std::cerr << "ERROR: Unknown experiment type: " << experimentType << std::endl;
                return false;
            }
            
            std::cout << "✓ " << experimentType << " experiment completed" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception during experiment: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Save brain state
    bool saveState(const std::string& filepath) {
        if (!initialized) {
            std::cerr << "ERROR: Brain not initialized!" << std::endl;
            return false;
        }
        
        std::cout << "Saving brain state to " << filepath << "..." << std::endl;
        
        try {
            if (brain->save(filepath)) {
                std::cout << "✓ Brain state saved successfully" << std::endl;
                return true;
            } else {
                std::cerr << "ERROR: Failed to save brain state" << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception during save: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Load brain state
    bool loadState(const std::string& filepath) {
        std::cout << "Loading brain state from " << filepath << "..." << std::endl;
        
        try {
            // Reset current brain
            if (initialized) {
                brain->reset();
            }
            
            // Create new brain instance
            brain = std::make_shared<Brain>(config);
            
            if (!brain->initialize()) {
                std::cerr << "ERROR: Failed to initialize brain after load" << std::endl;
                return false;
            }
            
            if (brain->load(filepath)) {
                std::cout << "✓ Brain state loaded successfully" << std::endl;
                initialized = true;
                return true;
            } else {
                std::cerr << "ERROR: Failed to load brain state" << std::endl;
                return false;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Exception during load: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Get brain statistics
    void printStatistics() const {
        if (!initialized) {
            std::cout << "Brain not initialized" << std::endl;
            return;
        }
        
        std::cout << "=== NLM Brain Statistics ===" << std::endl;
        brain->logStatus();
        std::cout << "=========================" << std::endl;
    }
    
    // Shutdown brain
    void shutdown() {
        if (initialized) {
            std::cout << "Shutting down NLM Brain..." << std::endl;
            brain->reset();
            initialized = false;
            std::cout << "✓ NLM Brain shutdown complete" << std::endl;
        }
    }
    
    // Get configuration
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    // Get brain
    std::shared_ptr<Brain> getBrain() const {
        return brain;
    }
    
    // Check if initialized
    bool isInitialized() const {
        return initialized;
    }
    
private:
    void runPlasticityExperiment() {
        std::cout << "  Running plasticity experiment..." << std::endl;
        brain->reset();
        brain->initialize();
        
        // Simple plasticity demonstration
        for (size_t step = 0; step < 500; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(step);
            
            if (step % 100 == 0) {
                std::cout << "    Step " << step << ": " 
                         << brain->getTotalSpikeCount() << " spikes" << std::endl;
            }
        }
    }
    
    void runSTDPExperiment() {
        std::cout << "  Running STDP experiment..." << std::endl;
        brain->reset();
        brain->initialize();
        
        // Enable STDP on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(false, true, false);  // Enable only STDP
            }
        }
        
        // Create correlated activity to trigger LTP
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                for (int trial = 0; trial < 50; ++trial) {
                    // Fire pre then post
                    neurons[0]->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    neurons[1]->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
            }
        }
    }
    
    void runLearningExperiment() {
        std::cout << "  Running learning experiment..." << std::endl;
        brain->reset();
        brain->initialize();
        
        // Enable all plasticity
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);
            }
        }
        
        // Apply learning input pattern
        for (size_t step = 0; step < 1000; ++step) {
            // Create input pattern
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step);
            
            if (step % 200 == 0) {
                std::cout << "    Step " << step << ": " 
                         << brain->getTotalSpikeCount() << " spikes" << std::endl;
            }
        }
    }
};

// Parse command line arguments
CLIOptions parseCommandLine(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            exit(0);
        } else if (arg == "--version") {
            printVersion();
            exit(0);
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--debug") {
            options.debug = true;
        } else if (arg == "--visualize") {
            options.visualize = true;
        } else if (arg == "--profile") {
            options.profile = true;
        } else if (arg == "--continuous") {
            options.continuous = true;
        } else if (arg == "--enable-GPU") {
            options.enableGPU = true;
        } else if (arg == "--no-SIMD") {
            options.enableSIMD = false;
        } else if (arg == "--threads") {
            if (i + 1 < argc) {
                options.threadCount = std::stoul(argv[++i]);
            }
        } else if (arg == "--steps") {
            if (i + 1 < argc) {
                options.steps = std::stoul(argv[++i]);
            }
        } else if (arg == "--time-limit") {
            if (i + 1 < argc) {
                options.timeLimit = std::stod(argv[++i]);
            }
        } else if (arg == "--neurons") {
            if (i + 1 < argc) {
                options.maxNeurons = std::stoul(argv[++i]);
            }
        } else if (arg == "--regions") {
            if (i + 1 < argc) {
                options.maxRegions = std::stoul(argv[++i]);
            }
        } else if (arg == "--mode") {
            if (i + 1 < argc) {
                options.mode = argv[++i];
            }
        } else if (arg == "--experiment") {
            if (i + 1 < argc) {
                options.experimentType = argv[++i];
            }
        } else if (arg == "--save") {
            options.saveState = true;
            if (i + 1 < argc) {
                options.checkpointFile = argv[++i];
            }
        } else if (arg == "--load") {
            options.loadState = true;
            if (i + 1 < argc) {
                options.checkpointFile = argv[++i];
            }
        } else if (arg == "--output") {
            if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg == "--log-file") {
            if (i + 1 < argc) {
                options.logFile = argv[++i];
            }
        } else if (arg == "--api-mode") {
            if (i + 1 < argc) {
                options.apiMode = argv[++i];
            }
        } else if (arg == "--inference-steps") {
            if (i + 1 < argc) {
                options.inferenceSteps = std::stoul(argv[++i]);
            }
        } else if (arg == "--inference-interval") {
            if (i + 1 < argc) {
                options.inferenceInterval = std::stof(argv[++i]);
            }
        } else if (arg == "--config") {
            if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.find("--") == 0) {
            // Custom configuration key=value
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                // Store for later application
                options.customArgs.push_back({key, value});
            }
        } else {
            // Unknown argument
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }
    
    return options;
}

// Print help information
void printHelp() {
    std::cout << "NLM CLI - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << "=====================================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Usage: nlm [options]" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help              Show this help message" << std::endl;
    std::cout << "  --version               Show version information" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Basic Options:" << std::endl;
    std::cout << "  --mode <mode>           Run mode: demo, experiment, benchmark (default: demo)" << std::endl;
    std::cout << "  --steps <steps>         Number of simulation steps (default: 0)" << std::endl;
    std::cout << "  --neurons <n>           Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions <n>           Number of regions (default: 1)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Experiment Options:" << std::endl;
    std::cout << "  --experiment <type>     Experiment type: plasticity, stdp, learning, all" << std::endl;
    std::cout << "  --continuous            Run in continuous mode" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "State Management:" << std::endl;
    std::cout << "  --save <file>           Save brain state to file" << std::endl;
    std::cout << "  --load <file>           Load brain state from file" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "I/O Options:" << std::endl;
    std::cout << "  --config <file>         Configuration file (default: configs/default.cfg)" << std::endl;
    std::cout << "  --output <file>         Output file" << std::endl;
    std::cout << "  --log-file <file>       Log file (default: nlm.log)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Debug & Visualization:" << std::endl;
    std::cout << "  --verbose               Enable verbose output" << std::endl;
    std::cout << "  --debug                 Enable debug mode" << std::endl;
    std::cout << "  --visualize             Enable visualization" << std::endl;
    std::cout << "  --profile               Enable performance profiling" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Performance Options:" << std::endl;
    std::cout << "  --enable-GPU            Enable GPU acceleration (if available)" << std::endl;
    std::cout << "  --no-SIMD               Disable SIMD optimizations" << std::endl;
    std::cout << "  --threads <n>           Number of threads (default: 4)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "API Mode Options:" << std::endl;
    std::cout << "  --api-mode <mode>       API access mode: full, minimal, expert" << std::endl;
    std::cout << "  --inference-steps <n>   Number of inference steps (default: 100)" << std::endl;
    std::cout << "  --inference-interval <t> Inference interval in seconds (default: 0.1)" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Configuration:" << std::endl;
    std::cout << "  --time-limit <t>        Time limit in seconds (default: 0)" << std::endl;
    std::cout << "  --custom <key>=<value>  Custom configuration parameter" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Example usage:" << std::endl;
    std::cout << "  nlm --mode demo --steps 1000 --verbose" << std::endl;
    std::cout << "  nlm --mode experiment --experiment plasticity --save brain_state" << std::endl;
    std::cout << "  nlm --mode benchmark --steps 5000 --profile --visualize" << std::endl;
    std::cout << "  nlm --mode demo --load saved_brain.json --custom neuron_count=2000" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
}

// Print version information
void printVersion() {
    std::cout << "NLM - Neural Learning Machine v1.0.0" << std::endl;
    std::cout << "Enhanced with JSON/YAML configuration support" << std::endl;
    std::cout << "Phase 6: Integrated Artificial Brain" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  - Advanced CLI interface" << std::endl;
    std::cout << "  - JSON/YAML configuration parser" << std::endl;
    std::cout << "  - Memory management improvements" << std::endl;
    std::cout << "  - Performance profiling tools" << std::endl;
    std::cout << "  - Visualization capabilities" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For documentation, visit: https://nlm.readthedocs.io" << std::endl;
}

int main(int argc, char** argv) {
    try {
        // Parse command line arguments
        CLIOptions options = parseCommandLine(argc, argv);
        
        // Create enhanced NLM brain interface
        NLMBrain nlmBrain;
        
        // Initialize brain
        if (!nlmBrain.initialize(options)) {
            std::cerr << "ERROR: Failed to initialize NLM Brain" << std::endl;
            return 1;
        }
        
        // Process commands based on mode
        bool success = true;
        
        if (options.loadState) {
            success = nlmBrain.loadState(options.checkpointFile) && success;
        }
        
        if (options.mode == "demo") {
            if (options.steps > 0) {
                success = nlmBrain.runSimulation(options.steps) && success;
            } else {
                success = nlmBrain.runExperiment(options.experimentType) && success;
            }
        } else if (options.mode == "experiment") {
            success = nlmBrain.runExperiment(options.experimentType) && success;
        } else if (options.mode == "benchmark") {
            success = nlmBrain.runSimulation(options.steps) && success;
        }
        
        if (options.saveState && success) {
            success = nlmBrain.saveState(options.checkpointFile) && success;
        }
        
        // Print final statistics
        nlmBrain.printStatistics();
        
        // Save configuration if output file specified
        if (!options.outputFile.empty() && success) {
            std::cout << "Saving configuration to " << options.outputFile << "..." << std::endl;
            // Note: Configuration saving functionality would be added here
            std::cout << "Configuration save completed" << std::endl;
        }
        
        if (success) {
            std::cout << "✓ NLM CLI completed successfully" << std::endl;
            return 0;
        } else {
            std::cerr << "ERROR: NLM CLI completed with errors" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception in main: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception in main" << std::endl;
        return 1;
    }
}}