#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "Brain.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include "Observation.hpp"
#include "../core/Config/Config.hpp"

void printHelp() {
    std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    std::cout << "An Experimental Artificial Developmental Brain" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./nlm <command> [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  run [seconds]           Run simulation for specified time" << std::endl;
    std::cout << "  run-episode              Run a single episode with environment" << std::endl;
    std::cout << "  checkpoint-save [file]  Save current brain state to file" << std::endl;
    std::cout << "  checkpoint-load [file]  Load brain state from file" << std::endl;
    std::cout << "  status                   Print current system status" << std::endl;
    std::cout << "  help                    Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --neurons N             Number of neurons (default: 1000)" << std::endl;
    std::cout << "  --regions R             Number of regions (default: 1)" << std::endl;
    std::cout << "  --config FILE           Load configuration from file" << std::endl;
    std::cout << "  --verbose               Enable verbose logging" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  ./nlm run 10 --neurons 2000 --regions 2" << std::endl;
    std::cout << "  ./nlm run-episode --config config.txt" << std::endl;
    std::cout << "  ./nlm checkpoint-save brain_state.bin" << std::endl;
}

void runSimulation(int seconds, size_t neuronCount, size_t regionCount, bool verbose) {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", neuronCount);
    config->set("region_count", regionCount);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 123456);
    
    std::cout << "Initializing NLM brain (Phase 6 - Integrated Artificial Brain)..." << std::endl;
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Starting simulation..." << std::endl;
    std::cout << "Press Ctrl+C to stop early" << std::endl;
    
    try {
        auto startTime = std::chrono::steady_clock::now();
        auto lastStatusTime = startTime;
        
        while (std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - startTime).count() < seconds) {
            
            brain->step(0);
            
            // Print status every 5 seconds
            auto currentTime = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(
                currentTime - lastStatusTime).count() >= 5) {
                brain->logStatus();
                lastStatusTime = currentTime;
            }
            
            if (verbose) {
                std::cout << "." << std::flush;
            }
        }
    } catch (const std::exception& e) {
        std::cout << std::endl << "Simulation interrupted: " << e.what() << std::endl;
    }
    
    std::cout << std::endl << "Simulation completed!" << std::endl;
}

void runEpisode(size_t neuronCount, size_t regionCount) {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", neuronCount);
    config->set("region_count", regionCount);
    config->set("simulation_timestep", 0.001);
    config->set("random_seed", 123456);
    
    // Initialize brain and agent
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
    auto world = std::make_shared<nlm::SimpleWorld>();
    agentBrain->initialize(*world);
    
    std::cout << "Starting episode..." << std::endl;
    
    // Run episode loop
    nlm::SimulationStep step = 0;
    bool running = true;
    nlm::Timestamp startTime = 0.0;
    
    while (running && step < 1000) {  // Limit episode length
        // Get sensory input from world
        auto percept = world->observe(*agentBrain);
        
        // Process sensory input through agent brain
        agentBrain->processSensoryInput(percept);
        
        // Step the brain
        brain->step(step);
        
        // Decode motor command and apply action
        auto action = agentBrain->decodeMotorCommand();
        world->applyAction(*agentBrain, action);
        
        // Compute reward
        float reward = world->computeReward(*agentBrain);
        
        // Apply reward modulation
        float predictedReward = 0.5f;  // Placeholder
        agentBrain->applyRewardModulation(reward, predictedReward);
        
        // Update development
        agentBrain->updateDevelopment(0.001);
        
        step++;
        
        if (step % 100 == 0) {
            std::cout << "Step " << step << ", Reward: " << reward << std::endl;
        }
        
        // Check if episode should end
        if (world->shouldTerminate(*agentBrain)) {
            running = false;
        }
    }
    
    std::cout << std::endl << "Episode completed after " << step << " steps!" << std::endl;
}

void saveCheckpoint(const std::string& filename, std::shared_ptr<nlm::Brain> brain) {
    std::cout << "Saving checkpoint to " << filename << "..." << std::endl;
    if (brain->save(filename)) {
        std::cout << "Checkpoint saved successfully!" << std::endl;
    } else {
        std::cerr << "Failed to save checkpoint!" << std::endl;
    }
}

void loadCheckpoint(const std::string& filename, std::shared_ptr<nlm::Brain> brain) {
    std::cout << "Loading checkpoint from " << filename << "..." << std::endl;
    if (brain->load(filename)) {
        std::cout << "Checkpoint loaded successfully!" << std::endl;
    } else {
        std::cerr << "Failed to load checkpoint!" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }
    
    std::string command = argv[1];
    size_t neuronCount = 1000;
    size_t regionCount = 1;
    std::string configFile = "";
    std::string extraArg = "";
    bool verbose = false;
    
    // Parse options
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose") {
            verbose = true;
        } else if (arg == "--neurons" && i + 1 < argc) {
            neuronCount = std::stoul(argv[++i]);
        } else if (arg == "--regions" && i + 1 < argc) {
            regionCount = std::stoul(argv[++i]);
        } else if (arg == "--config" && i + 1 < argc) {
            configFile = argv[++i];
            // TODO: Load configuration from file
        } else {
            extraArg = arg;
        }
    }
    
    try {
        if (command == "help" || command == "--help") {
            printHelp();
        } else if (command == "run") {
            int seconds = 10;  // Default 10 seconds
            if (!extraArg.empty()) {
                try {
                    seconds = std::stoi(extraArg);
                } catch (...) {
                    std::cerr << "Invalid duration: " << extraArg << std::endl;
                    printHelp();
                    return 1;
                }
            }
            runSimulation(seconds, neuronCount, regionCount, verbose);
        } else if (command == "run-episode") {
            runEpisode(neuronCount, regionCount);
        } else if (command == "checkpoint-save") {
            if (extraArg.empty()) {
                std::cerr << "Please provide filename for checkpoint!" << std::endl;
                printHelp();
                return 1;
            }
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", neuronCount);
            config->set("region_count", regionCount);
            auto brain = std::make_shared<nlm::Brain>(config);
            if (brain->initialize()) {
                saveCheckpoint(extraArg, brain);
            }
        } else if (command == "checkpoint-load") {
            if (extraArg.empty()) {
                std::cerr << "Please provide filename for checkpoint!" << std::endl;
                printHelp();
                return 1;
            }
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", neuronCount);
            config->set("region_count", regionCount);
            auto brain = std::make_shared<nlm::Brain>(config);
            if (brain->initialize()) {
                loadCheckpoint(extraArg, brain);
            }
        } else if (command == "status") {
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", neuronCount);
            config->set("region_count", regionCount);
            auto brain = std::make_shared<nlm::Brain>(config);
            if (brain->initialize()) {
                brain->logStatus();
            }
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            printHelp();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
