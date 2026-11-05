// NLM Command Line Interface
// Phase 6: Improved User Interface for NLM

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <filesystem>

namespace nlm {

// NLM Command Line Interface
class NLMCLI {
public:
    NLMCLI() {
        setupDefaultConfig();
        initializeLogger();
    }
    
    void run(int argc, char* argv[]) {
        try {
            parseArguments(argc, argv);
            
            if (command == "build" || command == "run") {
                runSimulation();
            } else if (command == "save" || command == "load") {
                handleCheckpoint();
            } else if (command == "config") {
                showConfig();
            } else if (command == "test") {
                runTests();
            } else if (command == "help") {
                showHelp();
            } else {
                std::cerr << "Unknown command: " << command << std::endl;
                showHelp();
                exit(1);
            }
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            exit(1);
        } catch (...) {
            std::cerr << "ERROR: Unknown error occurred" << std::endl;
            exit(1);
        }
    }
    
private:
    std::string command;
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    size_t neuronCount = 500;
    size_t regionCount = 1;
    size_t maxSteps = 1000;
    bool verbose = false;
    bool enableCheckpoint = false;
    bool enableDevelopment = false;
    bool enableCuriosity = true;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> cfg;
    
    void setupDefaultConfig() {
        cfg = std::make_shared<Config>();
        cfg->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        cfg->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        cfg->set("connection_probability", 0.1f, ConfigSource::Default);
        cfg->set("simulation_timestep", 0.001, ConfigSource::Default);
        cfg->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        cfg->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        cfg->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        cfg->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        cfg->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        // Validate configuration
        if (!cfg->validate()) {
            std::cerr << "WARNING: Configuration validation failed, using defaults" << std::endl;
        }
    }
    
    void initializeLogger() {
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Log startup
        NLM_LOG_INFO("NLM CLI initialized");
        NLM_LOG_INFO("Version: Phase 6 - Final Integration");
    }
    
    void parseArguments(int argc, char* argv[]) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                if (arg == "--help" || arg == "-h") {
                    command = "help";
                } else if (arg == "--config" && i + 1 < argc) {
                    configFile = argv[++i];
                } else if (arg == "--neurons" && i + 1 < argc) {
                    neuronCount = std::stoul(argv[++i]);
                    cfg->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::CommandLine);
                } else if (arg == "--steps" && i + 1 < argc) {
                    maxSteps = std::stoul(argv[++i]);
                    cfg->set("max_steps", static_cast<int64_t>(maxSteps), ConfigSource::CommandLine);
                } else if (arg == "--regions" && i + 1 < argc) {
                    regionCount = std::stoul(argv[++i]);
                    cfg->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::CommandLine);
                } else if (arg == "--verbose" || arg == "-v") {
                    verbose = true;
                } else if (arg == "--checkpoint" || arg == "-c") {
                    enableCheckpoint = true;
                } else if (arg == "--development" || arg == "-d") {
                    enableDevelopment = true;
                    cfg->set("development_enabled", true, ConfigSource::CommandLine);
                } else if (arg == "--no-curiosity") {
                    enableCuriosity = false;
                    cfg->set("curiosity_enabled", false, ConfigSource::CommandLine);
                } else if (arg == "save" || arg == "load" || arg == "run" || arg == "build" || arg == "test") {
                    command = arg;
                    outputFile = "checkpoint_test.bin";
                    if (command == "save" || command == "load" && i + 1 < argc) {
                        outputFile = argv[++i];
                    }
                } else {
                    // Check if it's a key=value argument
                    size_t equalPos = arg.find('=');
                    if (equalPos != std::string::npos) {
                        std::string key = arg.substr(0, equalPos);
                        std::string value = arg.substr(equalPos + 1);
                        cfg->parseLine(key, value);
                    } else {
                        std::cerr << "Unknown argument: " << arg << std::endl;
                    }
                }
            }
            
            // Load config file if specified
            if (command != "help" && command != "test" && command != "config") {
                std::filesystem::path configPath(configFile);
                if (std::filesystem::exists(configPath)) {
                    if (!cfg->loadFromFile(configFile)) {
                        std::cerr << "WARNING: Could not load config file: " << configFile << ", using defaults" << std::endl;
                    } else {
                        NLM_LOG_INFO("Successfully loaded configuration from: " + configFile);
                    }
                } else {
                    std::cerr << "WARNING: Config file not found: " << configFile << ", using defaults" << std::endl;
                }
            }
            
            // Validate configuration
            if (command != "help" && command != "test" && command != "config") {
                if (!cfg->validate()) {
                    std::cerr << "ERROR: Configuration validation failed" << std::endl;
                    std::cerr << "Please check your configuration file and command line arguments" << std::endl;
                    exit(1);
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "ERROR: Argument parsing failed: " << e.what() << std::endl;
            throw;
        }
    }
    
    void showHelp() {
        std::cout << "=== NLM Command Line Interface ===" << std::endl;
        std::cout << "Usage: nlm [OPTIONS]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  run, build     Run simulation (default)" << std::endl;
        std::cout << "  save           Save brain state to checkpoint" << std::endl;
        std::cout << "  load           Load brain state from checkpoint" << std::endl;
        std::cout << "  test           Run built-in tests" << std::endl;
        std::cout << "  config         Show current configuration" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --config FILE          Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --neurons N            Number of neurons (default: 500)" << std::endl;
        std::cout << "  --steps N              Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --regions N            Number of brain regions (default: 1)" << std::endl;
        std::cout << "  --verbose, -v          Verbose output" << std::endl;
        std::cout << "  --checkpoint, -c        Enable checkpointing" << std::endl;
        std::cout << "  --development, -d       Enable developmental stages" << std::endl;
        std::cout << "  --no-curiosity          Disable curiosity-driven exploration" << std::endl;
        std::cout << "  --help, -h             Show this help" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm run --neurons 1000 --steps 5000 --verbose" << std::endl;
        std::cout << "  nlm save my_brain.bin" << std::endl;
        std::cout << "  nlm load saved_brain.bin" << std::endl;
        std::cout << "  nlm config" << std::endl;
    }
    
    void runSimulation() {
        std::cout << "=== NLM Simulation ===" << std::endl;
        std::cout << "Running simulation with " << neuronCount << " neurons, " 
                  << regionCount << " regions, " << maxSteps << " steps" << std::endl;
        
        // Create brain
        brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            exit(1);
        }
        
        double startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
        
        // Run simulation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        size_t firingSteps = 0;
        
        for (size_t step = 0; step < maxSteps; ++step) {
            // Step brain
            brain->step(step, step * cfg->getOr<double>("simulation_timestep", 0.001));
            
            // Collect statistics
            totalFiringRate += brain->getAverageFiringRate();
            if (brain->getFiringNeuronCount() > 0) firingSteps++;
            
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": "
                          << "Firing Rate: " << brain->getAverageFiringRate()
                          << " Hz, Total Spikes: " << brain->getTotalSpikeCount()
                          << " Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
            }
        }
        
        double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
        
        // Calculate final statistics
        float avgFiringRate = totalFiringRate / maxSteps;
        float avgRewardPerStep = totalReward / maxSteps;
        double wallClockTime = endTime - startTime;
        
        std::cout << std::endl;
        std::cout << "=== Simulation Results ===" << std::endl;
        std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(2) << wallClockTime << " seconds" << std::endl;
        std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(3) << avgFiringRate << " Hz" << std::endl;
        std::cout << "Steps per Second: " << std::fixed << std::setprecision(1) << maxSteps / wallClockTime << std::endl;
        std::cout << "Total Spike Events: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Active Steps: " << firingSteps << " out of " << maxSteps << std::endl;
        
        if (enableCheckpoint) {
            std::cout << std::endl;
            std::cout << "Saving checkpoint to: " << outputFile << std::endl;
            if (brain->save(outputFile)) {
                std::cout << "Checkpoint saved successfully!" << std::endl;
            } else {
                std::cerr << "Failed to save checkpoint!" << std::endl;
            }
        }
    }
    
    void handleCheckpoint() {
        if (command == "save") {
            std::cout << "Saving brain state to: " << outputFile << std::endl;
            brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain for saving!" << std::endl;
                exit(1);
            }
            brain->save(outputFile);
        } else if (command == "load") {
            std::cout << "Loading brain state from: " << outputFile << std::endl;
            auto brain = std::make_shared<Brain>(cfg);
            if (!brain->initialize()) {
                std::cerr << "Failed to initialize brain for loading!" << std::endl;
                exit(1);
            }
            if (brain->load(outputFile)) {
                std::cout << "Brain state loaded successfully!" << std::endl;
            } else {
                std::cerr << "Failed to load brain state!" << std::endl;
                exit(1);
            }
        }
    }
    
    void showConfig() {
        std::cout << "=== NLM Configuration ===" << std::endl;
        std::cout << "Config File: " << configFile << std::endl;
        std::cout << "Neuron Count: " << neuronCount << std::endl;
        std::cout << "Region Count: " << regionCount << std::endl;
        std::cout << "Max Steps: " << maxSteps << std::endl;
        std::cout << "Verbose: " << (verbose ? "Yes" : "No") << std::endl;
        std::cout << "Checkpointing: " << (enableCheckpoint ? "Yes" : "No") << std::endl;
        std::cout << "Development: " << (enableDevelopment ? "Yes" : "No") << std::endl;
        std::cout << "Curiosity: " << (enableCuriosity ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        std::cout << "Brain Configuration:" << std::endl;
        std::cout << "  connection_probability: " << cfg->getOr<float>("connection_probability", 0.1f) << std::endl;
        std::cout << "  simulation_timestep: " << cfg->getOr<double>("simulation_timestep", 0.001) << std::endl;
        std::cout << "  random_seed: " << cfg->getOr<int64_t>("random_seed", 42) << std::endl;
        std::cout << "  stdp_ltp_weight: " << cfg->getOr<float>("stdp_ltp_weight", 0.01f) << std::endl;
        std::cout << "  stdp_ltd_weight: " << cfg->getOr<float>("stdp_ltd_weight", 0.012f) << std::endl;
    }
    
    void runTests() {
        std::cout << "=== Running NLM Tests ===" << std::endl;
        
        // In a real implementation, this would run the existing test framework
        // For now, we'll just provide a simple test runner
        std::cout << "Test framework available - would run:" << std::endl;
        std::cout << "  - Test types" << std::endl;
        std::cout << "  - Test config" << std::endl;
        std::cout << "  - Test random" << std::endl;
        std::cout << "  - Test clock" << std::endl;
        std::cout << "  - Test neuron" << std::endl;
        std::cout << "  - Test synapse" << std::endl;
        std::cout << "  - Test stdp" << std::endl;
        std::cout << "  - Test brain" << std::endl;
        std::cout << "  - Phase 6 integration tests" << std::endl;
    }
};

} // namespace nlm

int main(int argc, char* argv[]) {
    nlm::NLMCLI cli;
    cli.run(argc, argv);
    return 0;
}
