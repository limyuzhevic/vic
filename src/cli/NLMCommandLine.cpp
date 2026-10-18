#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/experiments/Phase6IntegratedExperiment.hpp>
#include <nlm/experiments/Phase6IntegratedExperiment.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

using namespace nlm;
namespace nl = nlohmann;

class NLMCli {
public:
    NLMCli() {
        Logger::getInstance().setLevel(Logger::Level::Info);
    }
    
    int run(int argc, char* argv[]) {
        if (argc < 2) {
            printHelp();
            return 0;
        }
        
        std::string command = argv[1];
        
        if (command == "help" || command == "--help") {
            printHelp();
        } else if (command == "version") {
            printVersion();
        } else if (command == "simulate") {
            return runSimulation(argc, argv);
        } else if (command == "experiment") {
            return runExperiment(argc, argv);
        } else if (command == "save") {
            return saveBrain(argc, argv);
        } else if (command == "load") {
            return loadBrain(argc, argv);
        } else if (command == "config") {
            return manageConfig(argc, argv);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printHelp();
            return 1;
        }
        
        return 0;
    }
    
private:
    void printHelp() {
        std::cout << "NLM (Neural Learning Machine) Command Line Interface" << std::endl;
        std::cout << "====================================================" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: nlm_cli <command> [options]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  help, --help          Show this help message" << std::endl;
        std::cout << "  version               Show version information" << std::endl;
        std::cout << "  simulate [options]    Run a simple neural simulation" << std::endl;
        std::cout << "  experiment [options]  Run Phase 6 integration experiment" << std::endl;
        std::cout << "  save <file>           Save brain state to file" << std::endl;
        std::cout << "  load <file>           Load brain state from file" << std::endl;
        std::cout << "  config [action]       Manage configuration" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  nlm_cli simulate --steps 1000 --neurons 500" << std::endl;
        std::cout << "  nlm_cli experiment --output results.json" << std::endl;
        std::cout << "  nlm_cli save my_brain.bin" << std::endl;
        std::cout << "  nlm_cli load my_brain.bin" << std::endl;
        std::cout << std::endl;
    }
    
    void printVersion() {
        std::cout << "NLM Phase 6 - Final Integration" << std::endl;
        std::cout << "Version: 0.1.0 (Phase 6)" << std::endl;
        std::cout << "Description: Experimental artificial developmental brain" << std::endl;
        std::cout << std::endl;
        std::cout << "Build Configuration:" << std::endl;
        std::cout << "  C++ Standard: C++20" << std::endl;
        std::cout << "  Python Bindings: Available (pynlm)" << std::endl;
        std::cout << "  Memory Systems: Working, Episodic, Associative" << std::endl;
        std::cout << "  Neuromodulation: Dopamine, Curiosity, Novelty, Prediction Error" << std::endl;
    }
    
    int runSimulation(int argc, char* argv[]) {
        std::string configFile = "configs/default.cfg";
        size_t neurons = 100;
        size_t steps = 1000;
        bool verbose = false;
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--config" && i + 1 < argc) {
                configFile = argv[++i];
            } else if (arg == "--neurons" && i + 1 < argc) {
                neurons = std::stoul(argv[++i]);
            } else if (arg == "--steps" && i + 1 < argc) {
                steps = std::stoul(argv[++i]);
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--help") {
                printSimulationHelp();
                return 0;
            }
        }
        
        // Run simulation
        return simulate(neurons, steps, configFile, verbose);
    }
    
    void printSimulationHelp() {
        std::cout << "Usage: nlm_cli simulate [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --config <file>      Configuration file (default: configs/default.cfg)" << std::endl;
        std::cout << "  --neurons <count>    Number of neurons (default: 100)" << std::endl;
        std::cout << "  --steps <count>      Number of simulation steps (default: 1000)" << std::endl;
        std::cout << "  --verbose, -v         Enable verbose output" << std::endl;
        std::cout << "  --help               Show this help" << std::endl;
    }
    
    int simulate(size_t neuronCount, size_t steps, const std::string& configFile, bool verbose) {
        std::cout << "=== NLM Simulation ===" << std::endl;
        std::cout << "Neurons: " << neuronCount << std::endl;
        std::cout << "Steps: " << steps << std::endl;
        std::cout << "Config: " << configFile << std::endl;
        std::cout << std::endl;
        
        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Try to load configuration from file
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration." << std::endl;
        }
        
        // Override with simulation parameters
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", 1, ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create and initialize brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        // Create world and agent
        SimpleWorld world;
        world.initialize(10, 10);
        AgentBrain agent(brain);
        agent.initialize(world);
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        
        std::cout << "Brain initialized successfully." << std::endl;
        
        // Run simulation
        float totalReward = 0.0f;
        float totalFiringRate = 0.0f;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = world.observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get action
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(brain->getRegions()[0].get(), cmd);
            
            // Compute reward
            float reward = world.computeReward(brain->getRegions()[0].get());
            totalReward += reward;
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, 0.0f);
            
            // Update development
            agent.updateDevelopment(0.001);
            
            // Collect metrics
            totalFiringRate += brain->getAverageFiringRate();
            
            // Periodic status
            if (verbose && step % 100 == 0) {
                std::cout << "Step " << step << ": Reward=" << totalReward / (step + 1) 
                          << " FiringRate=" << brain->getAverageFiringRate() << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate final metrics
        float avgReward = totalReward / steps;
        float avgFiringRate = totalFiringRate / steps;
        
        std::cout << std::endl;
        std::cout << "=== Simulation Results ===" << std::endl;
        std::cout << "Total reward: " << avgReward << std::endl;
        std::cout << "Average firing rate: " << avgFiringRate << " Hz" << std::endl;
        std::cout << "Wall clock time: " << duration << "s" << std::endl;
        std::cout << "Final developmental stage: " 
                  << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "Final curiosity level: " << agent.getCuriosityLevel() << std::endl;
        std::cout << "Final dopamine level: " << agent.getNeuromodulationLevel() << std::endl;
        
        return 0;
    }
    
    int runExperiment(int argc, char* argv[]) {
        std::string outputFile;
        
        // Parse arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--output" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg == "--help") {
                printExperimentHelp();
                return 0;
            }
        }
        
        return runIntegrationExperiment(outputFile);
    }
    
    void printExperimentHelp() {
        std::cout << "Usage: nlm_cli experiment [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --output <file>       Save results to JSON file" << std::endl;
        std::cout << "  --help                Show this help" << std::endl;
    }
    
    int runIntegrationExperiment(const std::string& outputFile) {
        std::cout << "=== Phase 6 Integration Experiment ===" << std::endl;
        
        // Create Phase 6 experiment
        Phase6IntegratedExperiment experiment;
        
        // Create experiment configuration
        Phase6Config config;
        config.neuronCount = 500;
        config.maxSteps = 2000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        
        // Run the experiment
        auto result = experiment.run(config);
        
        // Display results
        std::cout << std::endl;
        std::cout << "=== Final Results ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << std::endl;
        
        // Display integration status
        std::cout << "=== Integration Status ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        
        // Save results to JSON if requested
        if (!outputFile.empty()) {
            saveResultsToJson(result, outputFile);
            std::cout << "Results saved to: " << outputFile << std::endl;
        }
        
        return 0;
    }
    
    void saveResultsToJson(const Phase6IntegrationResult& result, const std::string& filepath) {
        nl::json jsonResult;
        
        // Add integration status
        jsonResult["integration_status"]["working_memory"] = result.memoryWorkingMemoryIntegrated;
        jsonResult["integration_status"]["episodic_memory"] = result.memoryEpisodicMemoryIntegrated;
        jsonResult["integration_status"]["neuromodulation"] = result.neuromodulationIntegrated;
        jsonResult["integration_status"]["prediction"] = result.predictionIntegrated;
        jsonResult["integration_status"]["development"] = result.developmentIntegrated;
        
        // Add metrics
        jsonResult["metrics"]["total_reward"] = result.totalReward;
        jsonResult["metrics"]["average_firing_rate"] = result.avgFiringRate;
        jsonResult["metrics"]["memory_episodes_stored"] = result.memoryEpisodesStored;
        jsonResult["metrics"]["novelty_level"] = result.noveltyLevel;
        jsonResult["metrics"]["curiosity_level"] = result.curiosityLevel;
        jsonResult["metrics"]["dopamine_level"] = result.dopamineLevel;
        jsonResult["metrics"]["average_synaptic_weight"] = result.avgSynapticWeight;
        
        // Add timing
        jsonResult["timing"]["wall_clock_time"] = result.totalWallClockTime;
        
        // Write to file
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << jsonResult.dump(2);
            file.close();
        } else {
            std::cerr << "Warning: Could not save results to " << filepath << std::endl;
        }
    }
    
    int saveBrain(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Error: Missing output file for save command" << std::endl;
            return 1;
        }
        
        std::string outputFile = argv[2];
        
        // For now, just show a message about brain saving
        std::cout << "Brain saving would be implemented here." << std::endl;
        std::cout << "Feature: Save brain state to file " << outputFile << std::endl;
        std::cout << "(Note: Full save/load implementation needs checkpoint system)" << std::endl;
        
        return 0;
    }
    
    int loadBrain(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Error: Missing input file for load command" << std::endl;
            return 1;
        }
        
        std::string inputFile = argv[2];
        
        // For now, just show a message about brain loading
        std::cout << "Brain loading would be implemented here." << std::endl;
        std::cout << "Feature: Load brain state from file " << inputFile << std::endl;
        std::cout << "(Note: Full save/load implementation needs checkpoint system)" << std::endl;
        
        return 0;
    }
    
    int manageConfig(int argc, char* argv[]) {
        if (argc < 3) {
            printConfigHelp();
            return 0;
        }
        
        std::string action = argv[2];
        
        if (action == "show") {
            showConfig();
        } else if (action == "create") {
            createConfig(argc, argv);
        } else if (action == "help") {
            printConfigHelp();
        } else {
            std::cerr << "Unknown config action: " << action << std::endl;
            printConfigHelp();
            return 1;
        }
        
        return 0;
    }
    
    void printConfigHelp() {
        std::cout << "Usage: nlm_cli config <action>" << std::endl;
        std::cout << "Actions:" << std::endl;
        std::cout << "  show                    Display current configuration" << std::endl;
        std::cout << "  create [file]           Create default configuration file" << std::endl;
        std::cout << "  help                    Show this help" << std::endl;
    }
    
    void showConfig() {
        std::cout << "=== Current Configuration ===" << std::endl;
        std::cout << "Configuration system would be displayed here." << std::endl;
        std::cout << "(Note: Full configuration management needs implementation)" << std::endl;
    }
    
    void createConfig(int argc, char* argv[]) {
        std::string configFile = "configs/default.cfg";
        if (argc >= 4) {
            configFile = argv[3];
        }
        
        std::cout << "Creating default configuration file: " << configFile << std::endl;
        
        // Check if file already exists
        std::ifstream existingFile(configFile);
        if (existingFile.good()) {
            std::cout << "Warning: File already exists!" << std::endl;
            std::cout << "Overwriting..." << std::endl;
        }
        
        // For now, just show that we would create the file
        std::cout << "Would write default configuration to file." << std::endl;
        std::cout << "Default config includes:" << std::endl;
        std::cout << "  - neuron_count = 1000" << std::endl;
        std::cout << "  - region_count = 2" << std::endl;
        std::cout << "  - connection_probability = 0.1" << std::endl;
        std::cout << "  - dopamine_baseline = 0.1" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    NLMCli cli;
    return cli.run(argc, argv);
}
