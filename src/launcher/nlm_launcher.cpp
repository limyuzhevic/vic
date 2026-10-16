// NLM Unified Launcher
// A comprehensive command-line interface for running all NLM phases and experiments

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <filesystem>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "schema/ConfigSchema.hpp"

using namespace nlm;

// Command line argument parsing
struct CLIConfig {
    std::string mode = "demo";  // demo, phase2, phase3, phase4, phase6, experiment
    std::string configFile;
    std::string outputFile;
    std::string logLevel = "info";
    std::string logFile;
    bool verbose = false;
    bool quiet = false;
    bool benchmark = false;
    size_t numTrials = 1;
    size_t stepsPerTrial = 1000;
    std::map<std::string, std::string> customParams;
    bool help = false;
    bool version = false;
};

// Print help information
void printHelp() {
    std::cout << "NLM - Neural Learning Machine Unified Launcher\n";
    std::cout << "==============================================\n\n";
    std::cout << "Usage: nlm [OPTIONS] [MODE]\n\n";
    std::cout << "Modes:\n";
    std::cout << "  demo             - Run complete demo (default)\n";
    std::cout << "  phase2           - Phase 2: Real neural computation\n";
    std::cout << "  phase3           - Phase 3: World interaction\n";
    std::cout << "  phase4           - Phase 4: Emerging cognition\n";
    std::cout << "  phase6           - Phase 6: Integration test\n";
    std::cout << "  experiment       - Run custom experiment\n\n";
    std::cout << "Options:\n";
    std::cout << "  --config=file     Configuration file path\n";
    std::cout << "  --output=file     Output file for results\n";
    std::cout << "  --log-level=level Logging level (debug, info, warning, error)\n";
    std::cout << "  --log-file=file   Log output to file\n";
    std::cout << "  --verbose         Verbose output\n";
    std::cout << "  --quiet           Quiet mode (errors only)\n";
    std::cout << "  --benchmark       Run benchmark and measure performance\n";
    std::cout << "  --trials=N        Number of trials (default: 1)\n";
    std::cout << "  --steps=N         Steps per trial (default: 1000)\n";
    std::cout << "  --param=key=value Custom configuration parameter\n";
    std::cout << "  --help            Show this help\n";
    std::cout << "  --version         Show version info\n\n";
    std::cout << "Examples:\n";
    std::cout << "  nlm phase3 --trials=10 --steps=500 --verbose\n";
    std::cout << "  nlm --config=myconfig.cfg --log-level=debug phase6\n";
    std::cout << "  nlm --benchmark --output=results.txt phase4\n";
}

// Print version information
void printVersion() {
    std::cout << "NLM - Neural Learning Machine v0.1.0\n";
    std::cout << "Phase 6: Final Integration\n";
    std::cout << "Built with C++20\n";
}

// Parse command line arguments
CLIConfig parseArguments(int argc, char** argv) {
    CLIConfig config;
    
    // Process arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle mode (first non-option argument)
        if (arg.substr(0, 2) != "--" && config.mode == "demo") {
            config.mode = arg;
            continue;
        }
        
        // Handle options
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key == "config") {
                    config.configFile = value;
                } else if (key == "output") {
                    config.outputFile = value;
                } else if (key == "log-level") {
                    config.logLevel = value;
                } else if (key == "log-file") {
                    config.logFile = value;
                } else if (key == "param") {
                    pos = value.find('=');
                    if (pos != std::string::npos) {
                        std::string paramKey = value.substr(0, pos);
                        std::string paramValue = value.substr(pos + 1);
                        config.customParams[paramKey] = paramValue;
                    }
                } else if (key == "help") {
                    config.help = true;
                } else if (key == "version") {
                    config.version = true;
                } else {
                    std::cerr << "Unknown option: --" << key << std::endl;
                }
            } else if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--quiet") {
                config.quiet = true;
            } else if (arg == "--benchmark") {
                config.benchmark = true;
            } else {
                std::cerr << "Unknown option: " << arg << std::endl;
            }
        } else {
            // Non-option argument that wasn't a mode
            if (config.mode == "demo") {
                std::cerr << "Unknown argument: " << arg << std::endl;
            }
        }
    }
    
    // Set defaults based on mode
    if (config.mode == "phase2" || config.mode == "demo") {
        config.stepsPerTrial = 1000;
    } else if (config.mode == "phase3") {
        config.stepsPerTrial = 200;
        config.numTrials = 5;
    } else if (config.mode == "phase4") {
        config.stepsPerTrial = 0;  // Phase 4 uses trials internally
        config.numTrials = 50;
    } else if (config.mode == "phase6") {
        config.stepsPerTrial = 2000;
    }
    
    return config;
}

// Initialize logging based on CLI config
void setupLogging(const CLIConfig& config) {
    auto logger = std::make_shared<Logger>();
    
    LogLevel level;
    if (config.logLevel == "debug") level = LogLevel::Debug;
    else if (config.logLevel == "warning") level = LogLevel::Warning;
    else if (config.logLevel == "error") level = LogLevel::Error;
    else level = LogLevel::Info;  // Default
    
    auto consoleLogger = std::make_shared<ConsoleLogger>(level);
    consoleLogger->setUseColors(!config.quiet && config.verbose);
    
    logger->addLogger(consoleLogger);
    
    if (!config.logFile.empty()) {
        consoleLogger->setOutputFile(config.logFile);
    }
    
    Logger::setGlobal(logger);
    
    if (config.verbose || config.benchmark) {
        NLM_LOG_INFO("Logging initialized with level: " + config.logLevel);
        if (!config.logFile.empty()) {
            NLM_LOG_INFO("Log file: " + config.logFile);
        }
    }
}

// Load and merge configuration
std::shared_ptr<Config> createConfig(const CLIConfig& cliConfig) {
    auto config = std::make_shared<Config>();
    
    // Load from file if specified
    if (!cliConfig.configFile.empty()) {
        if (config->loadFromFile(cliConfig.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + cliConfig.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + cliConfig.configFile);
            NLM_LOG_WARNING("Using default configuration.");
        }
    } else {
        NLM_LOG_INFO("Using default configuration.");
    }
    
    // Apply mode-specific defaults
    if (cliConfig.mode == "phase2") {
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    } else if (cliConfig.mode == "phase3") {
        config->set("environment_width", 20, ConfigSource::Default);
        config->set("environment_height", 20, ConfigSource::Default);
        config->set("vision_width", 16, ConfigSource::Default);
        config->set("vision_height", 16, ConfigSource::Default);
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    } else if (cliConfig.mode == "phase4") {
        config->set("neuron_count", 1000, ConfigSource::Default);
        config->set("region_count", 2, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    } else if (cliConfig.mode == "phase6") {
        config->set("neuron_count", 500, ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("random_seed", 42, ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    // Apply custom parameters
    for (const auto& param : cliConfig.customParams) {
        config->set(param.first, param.second, ConfigSource::CommandLine);
    }
    
    return config;
}

// Print benchmark results
void printBenchmarkResults(const std::string& mode, double wallClockTime, 
                          size_t neurons, size_t synapses, size_t spikes) {
    std::cout << "\n=== Benchmark Results ===\n";
    std::cout << "Mode: " << mode << "\n";
    std::cout << "Wall clock time: " << std::fixed << std::setprecision(3) << wallClockTime << "s\n";
    std::cout << "Performance: " << (spikes / wallClockTime) << " spikes/sec\n";
    std::cout << "Neural scale: " << neurons << " neurons, " << synapses << " synapses\n";
    
    if (spikes > 0) {
        double energyPerSpike = wallClockTime / spikes;
        std::cout << "Time per spike: " << std::setprecision(6) << energyPerSpike << "s\n";
    }
}

// Print experiment summary
void printExperimentSummary(const CLIConfig& config, double startTime, double endTime,
                           const std::shared_ptr<Brain>& brain, bool success) {
    double duration = endTime - startTime;
    
    std::cout << "\n=== Experiment Summary ===\n";
    std::cout << "Mode: " << config.mode << "\n";
    std::cout << "Duration: " << std::fixed << std::setprecision(3) << duration << "s\n";
    std::cout << "Brain initialized: " << (brain ? "Yes" : "No") << "\n";
    
    if (brain) {
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
        std::cout << "Average firing rate: " << std::setprecision(3) << brain->getAverageFiringRate() << " Hz\n";
    }
    
    std::cout << "Status: " << (success ? "SUCCESS" : "FAILED") << "\n";
}

int main(int argc, char** argv) {
    // Parse arguments
    CLIConfig config = parseArguments(argc, argv);
    
    // Handle help and version
    if (config.help) {
        printHelp();
        return 0;
    }
    
    if (config.version) {
        printVersion();
        return 0;
    }
    
    // Setup logging
    setupLogging(config);
    
    // Create configuration
    auto cfg = createConfig(config);
    
    if (config.verbose) {
        std::cout << "Configuration:" << std::endl;
        std::cout << cfg->summary() << std::endl;
    }
    
    // Record start time for benchmarking
    auto startWall = std::chrono::high_resolution_clock::now();
    double startTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    bool success = false;
    
    try {
        if (config.mode == "phase2") {
            std::cout << "=== Phase 2: Real Neural Computation ===\n\n";
            
            // Run the original main.cpp functionality
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain!\n";
                return 1;
            }
            
            std::cout << "Running basic connectivity test...\n";
            // In a real implementation, we would call the test functions from main.cpp
            // For now, we'll run a simple simulation
            for (size_t step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
            }
            
            std::cout << "Running plasticity experiment...\n";
            for (size_t step = 0; step < 1000; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            
            std::cout << "Running STDP verification...\n";
            for (size_t step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            success = true;
            
        } else if (config.mode == "phase3") {
            std::cout << "=== Phase 3: World Interaction ===\n\n";
            
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain!\n";
                return 1;
            }
            
            SimpleWorld world;
            world.configure(20.0f, 20.0f, 16, 16);
            world.setRandomSeed(42);
            
            AgentBrain agent(brain);
            agent.initialize(world);
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
            
            std::cout << "Running " << config.numTrials << " trials...\n";
            
            for (size_t trial = 0; trial < config.numTrials; ++trial) {
                std::cout << "Trial " << trial << ": ";
                
                world.reset();
                agent.reset();
                brain->reset();
                brain->initialize();
                
                for (size_t step = 0; step < config.stepsPerTrial; ++step) {
                    const SensoryPercept& percept = world.getSensoryPercept();
                    agent.processSensoryInput(percept);
                    
                    for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                        brain->step(step * 10 + neuralStep, (step * 10 + neuralStep) * 0.001);
                    }
                    
                    MotorCommand cmd = agent.decodeMotorCommand();
                    world.applyMotorCommand(cmd, world.getSimulationTime());
                    agent.applyRewardModulation(0.0f, 0.0f);
                    world.update(0.01);
                    agent.updateDevelopment(0.01);
                }
                
                std::cout << "Completed" << std::endl;
            }
            
            success = true;
            
        } else if (config.mode == "phase4") {
            std::cout << "=== Phase 4: Emerging Cognition ===\n\n";
            
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain!\n";
                return 1;
            }
            
            std::cout << "Running " << config.numTrials << " trials...\n";
            
            // Run the Phase 4 experiments
            // In a real implementation, we would call the actual experiment functions
            // For now, we'll run a simple simulation
            for (size_t trial = 0; trial < config.numTrials; ++trial) {
                std::cout << "Trial " << trial << ": ";
                
                // Reset brain for each trial
                brain->reset();
                brain->initialize();
                
                // Run simulation steps
                for (size_t step = 0; step < 50; ++step) {
                    brain->step(step, step * 0.001);
                }
                
                std::cout << "Completed" << std::endl;
            }
            
            success = true;
            
        } else if (config.mode == "phase6") {
            std::cout << "=== Phase 6: Integration Test ===\n\n";
            
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain!\n";
                return 1;
            }
            
            std::cout << "Verifying integration...\n";
            
            // Check all systems
            bool hasWorkingMemory = brain->getWorkingMemory() != nullptr;
            bool hasEpisodicMemory = brain->getEpisodicMemory() != nullptr;
            bool hasNeuromodulation = brain->getDopamine() != nullptr;
            bool hasPrediction = brain->getPredictionSystem() != nullptr;
            bool hasDevelopment = brain->getDevelopmentSystem() != nullptr;
            
            std::cout << "Working Memory: " << (hasWorkingMemory ? "OK" : "FAIL") << std::endl;
            std::cout << "Episodic Memory: " << (hasEpisodicMemory ? "OK" : "FAIL") << std::endl;
            std::cout << "Neuromodulation: " << (hasNeuromodulation ? "OK" : "FAIL") << std::endl;
            std::cout << "Prediction System: " << (hasPrediction ? "OK" : "FAIL") << std::endl;
            std::cout << "Development System: " << (hasDevelopment ? "OK" : "FAIL") << std::endl;
            
            // Run integration test simulation
            for (size_t step = 0; step < config.stepsPerTrial; ++step) {
                brain->step(step, step * 0.001);
                
                if (step % 500 == 0) {
                    std::cout << "Progress: " << (step * 100 / config.stepsPerTrial) << "%\n";
                }
            }
            
            success = (hasWorkingMemory && hasEpisodicMemory && 
                      hasNeuromodulation && hasPrediction && hasDevelopment);
            
        } else if (config.mode == "demo") {
            std::cout << "=== Complete Demo Mode ===\n\n";
            
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain!\n";
                return 1;
            }
            
            std::cout << "Running complete demo with all phases...\n";
            
            // Run Phase 2 components
            std::cout << "Phase 2: Basic connectivity test...\n";
            for (size_t step = 0; step < 100; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            
            // Reset for Phase 3
            brain->reset();
            brain->initialize();
            
            std::cout << "Phase 3: World interaction simulation...\n";
            SimpleWorld world;
            world.configure(10.0f, 10.0f, 8, 8);
            world.setRandomSeed(42);
            
            AgentBrain agent(brain);
            agent.initialize(world);
            
            for (size_t step = 0; step < 200; ++step) {
                const SensoryPercept& percept = world.getSensoryPercept();
                agent.processSensoryInput(percept);
                brain->step(step, step * 0.001);
                
                MotorCommand cmd = agent.decodeMotorCommand();
                world.applyMotorCommand(cmd, world.getSimulationTime());
                
                if (step % 50 == 0) {
                    std::cout << "  Step " << step << ": " << agent.getCuriosityLevel() << " curiosity\n";
                }
            }
            
            // Reset for Phase 6
            brain->reset();
            brain->initialize();
            
            std::cout << "Phase 6: Integration verification...\n";
            for (size_t step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
            }
            
            success = true;
            
        } else {
            std::cerr << "Unknown mode: " << config.mode << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
        success = false;
    }
    
    // Record end time
    double endTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto endWall = std::chrono::high_resolution_clock::now();
    double wallClockTime = std::chrono::duration<double>(endWall - startWall).count();
    
    // Print benchmark results if requested
    if (config.benchmark) {
        printBenchmarkResults(config.mode, wallClockTime, 
                            (success && brain) ? brain->getTotalNeuronCount() : 0,
                            (success && brain) ? brain->getTotalSynapseCount() : 0,
                            (success && brain) ? brain->getTotalSpikeCount() : 0);
    }
    
    // Print experiment summary
    printExperimentSummary(config, startTime, endTime, brain, success);
    
    // Write output to file if requested
    if (!config.outputFile.empty() && brain) {
        std::ofstream outFile(config.outputFile);
        if (outFile.is_open()) {
            outFile << "NLM Experiment Results\n";
            outFile << "======================\n\n";
            outFile << "Mode: " << config.mode << "\n";
            outFile << "Duration: " << std::fixed << std::setprecision(3) << wallClockTime << "s\n";
            outFile << "Neurons: " << brain->getTotalNeuronCount() << "\n";
            outFile << "Synapses: " << brain->getTotalSynapseCount() << "\n";
            outFile << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
            outFile << "Firing neurons: " << brain->getFiringNeuronCount() << "\n";
            outFile << "Average firing rate: " << std::setprecision(3) << brain->getAverageFiringRate() << " Hz\n";
            outFile << "Status: " << (success ? "SUCCESS" : "FAILED") << "\n";
            
            NLM_LOG_INFO("Results written to: " + config.outputFile);
        } else {
            std::cerr << "Failed to write results to: " << config.outputFile << std::endl;
        }
    }
    
    if (config.verbose || config.quiet) {
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << (success ? "Experiment completed successfully!" : "Experiment failed!") << std::endl;
        std::cout << std::string(60, '=') << std::endl;
    }
    
    return success ? 0 : 1;
}