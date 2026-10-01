// NLM Command Line Interface
// AgentBrain.cpp - Command Line Interface Implementation

// This file provides a command line interface for the NLM brain simulation
// allowing users to run simulations, configure parameters, and manage checkpoints

// Usage examples:
//   nlm --help                    # Show help information
//   nlm --run --steps 1000         # Run simulation with 1000 steps
//   nlm --load brain.checkpoint     # Load a saved brain state
//   nlm --save brain.checkpoint     # Save current brain state
//   nlm --config config.cfg        # Load configuration from file
//   nlm --steps 500 --world-size 20 20  # Run with custom world size

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sstream>

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace cli {

class NLMCLI {
public:
    NLMCLI() : verbose_(false), quiet_(false) {}
    
    int run(int argc, char* argv[]) {
        // Parse command line arguments
        std::vector<std::string> args(argv + 1, argv + argc);
        
        // Process command line options
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--help" || args[i] == "-h") {
                showHelp();
                return 0;
            } else if (args[i] == "--version") {
                showVersion();
                return 0;
            } else if (args[i] == "--verbose" || args[i] == "-v") {
                verbose_ = true;
            } else if (args[i] == "--quiet" || args[i] == "-q") {
                quiet_ = true;
            } else if (args[i] == "--run" || args[i] == "-r") {
                return runSimulation(args, i + 1);
            } else if (args[i] == "--load" || args[i] == "-L") {
                return loadBrain(args, i + 1);
            } else if (args[i] == "--save" || args[i] == "-S") {
                return saveBrain(args, i + 1);
            } else if (args[i] == "--config" || args[i] == "-c") {
                loadConfiguration(args, i + 1);
            } else if (args[i] == "--steps") {
                if (i + 1 < args.size()) {
                    steps_ = std::stoul(args[++i]);
                }
            } else if (args[i] == "--world-size") {
                if (i + 1 < args.size()) {
                    parseWorldSize(args[++i]);
                }
            } else if (args[i] == "--world-width") {
                if (i + 1 < args.size()) {
                    worldWidth_ = std::stoul(args[++i]);
                }
            } else if (args[i] == "--world-height") {
                if (i + 1 < args.size()) {
                    worldHeight_ = std::stoul(args[++i]);
                }
            } else if (args[i] == "--world-vision-width") {
                if (i + 1 < args.size()) {
                    worldVisionWidth_ = std::stoul(args[++i]);
                }
            } else if (args[i] == "--world-vision-height") {
                if (i + 1 < args.size()) {
                    worldVisionHeight_ = std::stoul(args[++i]);
                }
            } else if (args[i] == "--checkpoint") {
                if (i + 1 < args.size()) {
                    checkpointFile_ = args[++i];
                }
            } else if (args[i] == "--enable-learning") {
                enableLearning_ = true;
            } else if (args[i] == "--enable-curiosity") {
                enableCuriosity_ = true;
            } else if (args[i] == "--enable-development") {
                enableDevelopment_ = true;
            } else if (!args[i].empty() && args[i][0] != '-') {
                // Unknown argument or configuration file
                if (std::filesystem::exists(args[i])) {
                    loadConfigurationFile(args[i]);
                } else {
                    std::cerr << "Error: Unknown argument or file: " << args[i] << std::endl;
                    showHelp();
                    return 1;
                }
            }
        }
        
        // If no command specified, show help
        if (steps_ == 0 && checkpointFile_.empty()) {
            showHelp();
            return 0;
        }
        
        return 0;
    }
    
private:
    void showHelp() {
        std::cout << "NLM - Neural Learning Machine CLI" << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << std::endl;
        
        std::cout << "USAGE:" << std::endl;
        std::cout << "  nlm [OPTIONS] [CONFIG_FILE]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "COMMANDS:" << std::endl;
        std::cout << "  --run, -r                  Run simulation" << std::endl;
        std::cout << "  --load, -L <file>          Load brain state" << std::endl;
        std::cout << "  --save, -S <file>          Save brain state" << std::endl;
        std::cout << "  --config, -c <file>        Load configuration file" << std::endl;
        std::cout << std::endl;
        
        std::cout << "OPTIONS:" << std::endl;
        std::cout << "  --steps N                  Number of simulation steps (default: 100)" << std::endl;
        std::cout << "  --world-size W H           Set world dimensions (width height)" << std::endl;
        std::cout << "  --world-width W            Set world width" << std::endl;
        std::cout << "  --world-height H           Set world height" << std::endl;
        std::cout << "  --world-vision-width W     Set world vision width" << std::endl;
        std::cout << "  --world-vision-height H    Set world vision height" << std::endl;
        std::cout << "  --checkpoint FILE          Checkpoint file path" << std::endl;
        std::cout << "  --enable-learning          Enable reward-based learning" << std::endl;
        std::cout << "  --enable-curiosity         Enable curiosity-based exploration" << std::endl;
        std::cout << "  --enable-development       Enable developmental stages" << std::endl;
        std::cout << "  --verbose, -v              Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q                Quiet mode (only errors)" << std::endl;
        std::cout << "  --help, -h                 Show this help message" << std::endl;
        std::cout << "  --version                  Show version information" << std::endl;
        std::cout << std::endl;
        
        std::cout << "EXAMPLES:" << std::endl;
        std::cout << "  nlm --run --steps 1000" << std::endl;
        std::cout << "  nlm --load brain.checkpoint --steps 500" << std::endl;
        std::cout << "  nlm --save brain.checkpoint" << std::endl;
        std::cout << "  nlm --run --world-size 20 20 --checkpoint demo.checkpoint" << std::endl;
        std::cout << "  nlm --run --enable-learning --enable-curiosity config.cfg" << std::endl;
        std::cout << std::endl;
    }
    
    void showVersion() {
        std::cout << "NLM Phase 6 - Final Integration" << std::endl;
        std::cout << "Version 0.1.0" << std::endl;
        std::cout << "Build date: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Phase 6: Final Integration of All Brain Systems" << std::endl;
        std::cout << "Memory, neuromodulation, prediction, cognition, development" << std::endl;
    }
    
    int runSimulation(const std::vector<std::string>& args, size_t startIndex) {
        // Create brain with default configuration
        auto config = std::make_shared<Config>();
        
        // Load configuration file if specified
        if (!configFile_.empty() && std::filesystem::exists(configFile_)) {
            config->loadFromFile(configFile_);
        }
        
        // Create brain
        std::cout << "Creating NLM brain..." << std::endl;
        auto brain = std::make_shared<Brain>(config);
        
        // Initialize brain
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            return 1;
        }
        
        // Create world
        std::cout << "Creating simulation world..." << std::endl;
        auto world = std::make_unique<SimpleWorld>();
        world->configure(worldWidth_, worldHeight_, worldVisionWidth_, worldVisionHeight_);
        world->reset();
        
        // Create agent brain
        std::cout << "Creating agent brain..." << std::endl;
        auto agent = std::make_unique<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Configure agent features
        if (enableLearning_) {
            agent->enableRewardModulation(true);
            std::cout << "Enabled reward-based learning" << std::endl;
        }
        
        if (enableCuriosity_) {
            agent->enableCuriosity(true);
            std::cout << "Enabled curiosity-based exploration" << std::endl;
        }
        
        if (enableDevelopment_) {
            agent->enableDevelopment(true);
            std::cout << "Enabled developmental stages" << std::endl;
        }
        
        // Load checkpoint if specified
        if (!checkpointFile_.empty() && std::filesystem::exists(checkpointFile_)) {
            std::cout << "Loading brain state from: " << checkpointFile_ << std::endl;
            if (!brain->load(checkpointFile_)) {
                std::cerr << "Warning: Failed to load checkpoint, starting fresh" << std::endl;
            }
        }
        
        // Run simulation
        std::cout << "Running simulation for " << steps_ << " steps..." << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps_; ++step) {
            // Update world
            world->update(0.1);  // Fixed timestep for CLI
            
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(step);
            
            // Get motor command
            auto action = agent->decodeMotorCommand();
            
            // Apply motor command
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Print progress periodically
            if (verbose_ || (step % 100 == 0)) {
                std::cout << "Step: " << step << "/" << steps_;
                
                // Print brain statistics
                std::cout << "  Neurons: " << brain->getTotalNeuronCount();
                std::cout << "  Spikes: " << brain->getTotalSpikeCount();
                std::cout << "  Firing: " << brain->getFiringNeuronCount();
                std::cout << "  Avg firing rate: " << brain->getAverageFiringRate();
                
                // Print agent state
                std::cout << "  Dopamine: " << agent->getNeuromodulationLevel();
                std::cout << "  Curiosity: " << agent->getCuriosityLevel();
                std::cout << "  Novelty: " << agent->getNoveltyLevel();
                std::cout << std::endl;
            }
            
            // Update checkpoint periodically
            if (step % 1000 == 0 && step > 0) {
                std::string checkpointName = "checkpoint_" + std::to_string(step);
                saveBrain(checkpointName);
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << std::endl;
        std::cout << "Simulation completed successfully!" << std::endl;
        std::cout << "Steps per second: " << (steps_ / (duration.count() + 1)) << std::endl;
        
        // Save final checkpoint
        if (!checkpointFile_.empty()) {
            std::cout << "Saving final brain state to: " << checkpointFile_ << std::endl;
            saveBrain(checkpointFile_);
        }
        
        // Print final statistics
        std::cout << std::endl;
        std::cout << "=== FINAL STATISTICS ===" << std::endl;
        std::cout << "Total simulation time: " << duration.count() << " seconds" << std::endl;
        std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        return 0;
    }
    
    int loadBrain(const std::vector<std::string>& args, size_t startIndex) {
        if (startIndex >= args.size()) {
            std::cerr << "Error: --load requires a file argument" << std::endl;
            return 1;
        }
        
        checkpointFile_ = args[startIndex];
        std::cout << "Load brain state command parsed (use --run to execute)" << std::endl;
        return 0;
    }
    
    int saveBrain(const std::vector<std::string>& args, size_t startIndex) {
        if (startIndex >= args.size()) {
            std::cerr << "Error: --save requires a file argument" << std::endl;
            return 1;
        }
        
        checkpointFile_ = args[startIndex];
        std::cout << "Save brain state command parsed (use --run to execute)" << std::endl;
        return 0;
    }
    
    void loadConfiguration(const std::vector<std::string>& args, size_t startIndex) {
        if (startIndex >= args.size()) {
            std::cerr << "Error: --config requires a file argument" << std::endl;
            return;
        }
        
        configFile_ = args[startIndex];
        if (!std::filesystem::exists(configFile_)) {
            std::cerr << "Error: Configuration file not found: " << configFile_ << std::endl;
        }
    }
    
    void parseWorldSize(const std::string& worldSize) {
        std::stringstream ss(worldSize);
        std::string token;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, token, ' ')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 2) {
            worldWidth_ = std::stoul(tokens[0]);
            worldHeight_ = std::stoul(tokens[1]);
        }
    }
    
    void loadConfigurationFile(const std::string& filename) {
        configFile_ = filename;
        std::cout << "Loading configuration from: " << filename << std::endl;
    }
    
    // CLI parameters
    size_t steps_ = 100;
    size_t worldWidth_ = 10;
    size_t worldHeight_ = 10;
    size_t worldVisionWidth_ = 8;
    size_t worldVisionHeight_ = 8;
    std::string configFile_;
    std::string checkpointFile_;
    bool enableLearning_ = false;
    bool enableCuriosity_ = false;
    bool enableDevelopment_ = false;
    bool verbose_ = false;
    bool quiet_ = false;
};

} // namespace cli

// Main function
int main(int argc, char* argv[]) {
    nlm::cli::NLMCLI cli;
    return cli.run(argc, argv);
}

// End of NLM CLI implementation
