#include "nlm_cli_exec.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>

namespace nlm {

NLM_CLI_EXEC::NLM_CLI_EXEC() : currentBrain_(nullptr), configFile_(""), currentStep_(0) {
    Logger::getInstance().setLevel(Logger::Level::Info);
}

NLM_CLI_EXEC::~NLM_CLI_EXEC() {
    cleanup();
}

bool NLM_CLI_EXEC::createBrain(const std::vector<std::string>& args) {
    try {
        // Parse arguments
        int neuronCount = 500;
        int regionCount = 1;
        int randomSeed = 42;
        float connectionProbability = 0.1f;
        
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--neurons" && i + 1 < args.size()) {
                neuronCount = std::stoi(args[i + 1]);
            } else if (args[i] == "--regions" && i + 1 < args.size()) {
                regionCount = std::stoi(args[i + 1]);
            } else if (args[i] == "--random_seed" && i + 1 < args.size()) {
                randomSeed = std::stoi(args[i + 1]);
            } else if (args[i] == "--connection_probability" && i + 1 < args.size()) {
                connectionProbability = std::stof(args[i + 1]);
            }
        }
        
        // Validate parameters
        if (neuronCount < 10 || neuronCount > 100000) {
            std::cerr << "Error: Neuron count must be between 10 and 100000" << std::endl;
            return false;
        }
        if (regionCount < 1 || regionCount > 1000) {
            std::cerr << "Error: Region count must be between 1 and 1000" << std::endl;
            return false;
        }
        
        // Create configuration
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", neuronCount);
        cfg->set("region_count", regionCount);
        cfg->set("random_seed", randomSeed);
        cfg->set("connection_probability", connectionProbability);
        
        // Create brain
        currentBrain_ = std::make_shared<Brain>(cfg);
        if (!currentBrain_->initialize()) {
            std::cerr << "Error: Failed to initialize brain" << std::endl;
            currentBrain_.reset();
            return false;
        }
        
        currentStep_ = 0;
        std::cout << "Created brain with " << neuronCount << " neurons and " << regionCount 
                  << " regions" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool NLM_CLI_EXEC::runSimulation(const std::vector<std::string>& args) {
    try {
        if (!currentBrain_) {
            std::cerr << "Error: No brain created. Use 'new' command first." << std::endl;
            return false;
        }
        
        // Parse steps argument
        int steps = 100;
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i] == "--steps" && i + 1 < args.size()) {
                steps = std::stoi(args[i + 1]);
            }
        }
        
        if (steps < 1 || steps > 1000000) {
            std::cerr << "Error: Steps must be between 1 and 1000000" << std::endl;
            return false;
        }
        
        std::cout << "Running simulation for " << steps << " steps..." << std::endl;
        
        // Run simulation
        for (int step = 0; step < steps; ++step) {
            // Get observation if world exists
            if (configFile_.empty() && currentStep_ == 0) {
                // Create a simple world for demonstration
                SimpleWorld world;
                world.initialize(16, 16);
                
                // Create agent
                AgentBrain agent(currentBrain_);
                agent.initialize(world);
                agent.enableCuriosity(true);
                
                // Run steps
                for (int s = 0; s < steps; ++s) {
                    SensoryPercept percept = world.observe(currentBrain_->getRegions()[0].get());
                    agent.processSensoryInput(percept);
                    currentBrain_->step(s, s * 0.001);
                    MotorCommand cmd = agent.decodeMotorCommand();
                    world.applyAction(currentBrain_->getRegions()[0].get(), cmd);
                    float reward = world.computeReward(currentBrain_->getRegions()[0].get());
                    agent.applyRewardModulation(reward, 0.0f);
                    
                    // Show progress every 10%
                    if (s % (steps / 10) == 0) {
                        std::cout << "  Progress: " << (100.0 * s / steps) << "% complete" << std::endl;
                    }
                }
            } else {
                // Use configuration file
                currentBrain_->step(step, step * 0.001);
            }
            
            currentStep_++;
        }
        
        std::cout << "Simulation completed. Final state:" << std::endl;
        currentBrain_->logStatus();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool NLM_CLI_EXEC::showStatus() {
    if (!currentBrain_) {
        std::cout << "No brain created." << std::endl;
        return true;
    }
    
    currentBrain_->logStatus();
    return true;
}

bool NLM_CLI_EXEC::saveBrain(const std::vector<std::string>& args) {
    if (!currentBrain_) {
        std::cerr << "Error: No brain created. Use 'new' command first." << std::endl;
        return false;
    }
    
    std::string filename = "brain.bin";
    if (!args.empty()) {
        filename = args[0];
    }
    
    if (currentBrain_->save(filename)) {
        std::cout << "Brain saved to " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to save brain" << std::endl;
        return false;
    }
}

bool NLM_CLI_EXEC::loadBrain(const std::vector<std::string>& args) {
    if (!currentBrain_) {
        std::cerr << "Error: No brain created. Use 'new' command first." << std::endl;
        return false;
    }
    
    std::string filename = "brain.bin";
    if (!args.empty()) {
        filename = args[0];
    }
    
    // Create new brain and load
    auto cfg = std::make_shared<Config>();
    auto newBrain = std::make_shared<Brain>(cfg);
    newBrain->initialize();
    
    if (newBrain->load(filename)) {
        currentBrain_ = newBrain;
        std::cout << "Brain loaded from " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Error: Failed to load brain" << std::endl;
        return false;
    }
}

bool NLM_CLI_EXEC::exportData(const std::vector<std::string>& args) {
    if (!currentBrain_) {
        std::cerr << "Error: No brain created. Use 'new' command first." << std::endl;
        return false;
    }
    
    if (args.empty()) {
        std::cerr << "Error: Export format required (txt, json, csv)" << std::endl;
        return false;
    }
    
    std::string format = args[0];
    std::string filename = "export";
    if (args.size() > 1) {
        filename = args[1];
    }
    
    // For now, just create a simple text export
    std::ofstream file(filename + "." + format);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create export file " << filename << "." << format << std::endl;
        return false;
    }
    
    if (format == "txt") {
        file << "=== NLM Brain Export ===" << std::endl;
        file << "Neuron Count: " << currentBrain_->getTotalNeuronCount() << std::endl;
        file << "Synapse Count: " << currentBrain_->getTotalSynapseCount() << std::endl;
        file << "Region Count: " << currentBrain_->getRegionCount() << std::endl;
        file << "Total Spikes: " << currentBrain_->getTotalSpikeCount() << std::endl;
        file << "Average Firing Rate: " << currentBrain_->getAverageFiringRate() << std::endl;
        file << "Working Memory Traces: " << (currentBrain_->getWorkingMemory() ? 
            currentBrain_->getWorkingMemory()->getActiveTraces() : 0) << std::endl;
        file << "Episodic Memory Episodes: " << (currentBrain_->getEpisodicMemory() ? 
            currentBrain_->getEpisodicMemory()->getEpisodeCount() : 0) << std::endl;
        
        // Show regions
        file << std::endl << "Regions:" << std::endl;
        for (size_t i = 0; i < currentBrain_->getRegionCount(); ++i) {
            auto region = currentBrain_->getRegion(RegionId(i + 1));
            if (region) {
                file << "  Region " << i + 1 << ": " << region->getTotalNeuronCount() 
                    << " neurons, " << region->getSynapseCount() << " synapses" << std::endl;
            }
        }
        
    } else if (format == "json") {
        file << "{\n  \"neuron_count\": " << currentBrain_->getTotalNeuronCount() << ",\n"
             << "  \"synapse_count\": " << currentBrain_->getTotalSynapseCount() << ",\n"
             << "  \"region_count\": " << currentBrain_->getRegionCount() << ",\n"
             << "  \"total_spikes\": " << currentBrain_->getTotalSpikeCount() << ",\n"
             << "  \"average_firing_rate\": " << currentBrain_->getAverageFiringRate() << ",\n"
             << "  \"working_memory_traces\": " << (currentBrain_->getWorkingMemory() ? 
                currentBrain_->getWorkingMemory()->getActiveTraces() : 0) << ",\n"
             << "  \"episodic_memory_episodes\": " << (currentBrain_->getEpisodicMemory() ? 
                currentBrain_->getEpisodicMemory()->getEpisodeCount() : 0) << "\n"
             << "}" << std::endl;
    }
    
    file.close();
    
    std::cout << "Data exported to " << filename << "." << format << std::endl;
    return true;
}

void NLM_CLI_EXEC::cleanup() {
    currentBrain_.reset();
    configFile_.clear();
    currentStep_ = 0;
}

bool NLM_CLI_EXEC::parseCommand(const std::string& cmd, const std::vector<std::string>& args) {
    try {
        if (cmd == "new") {
            return createBrain(args);
        } else if (cmd == "run") {
            return runSimulation(args);
        } else if (cmd == "status") {
            return showStatus();
        } else if (cmd == "save") {
            return saveBrain(args);
        } else if (cmd == "load") {
            return loadBrain(args);
        } else if (cmd == "export") {
            return exportData(args);
        } else if (cmd == "exit" || cmd == "quit") {
            cleanup();
            return true;
        } else if (cmd == "help") {
            showHelp();
            return true;
        } else if (cmd == "version") {
            showVersion();
            return true;
        } else {
            std::cerr << "Error: Unknown command '" << cmd << "'" << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

void NLM_CLI_EXEC::showHelp() {
    std::cout << "=== NLM Command Line Interface ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  new --neurons <count> [--regions <count>] [--random_seed <seed>] [--connection_probability <prob>]" << std::endl;
    std::cout << "    Create a new brain" << std::endl;
    std::cout << "  run --steps <steps>" << std::endl;
    std::cout << "    Run simulation" << std::endl;
    std::cout << "  status" << std::endl;
    std::cout << "    Show brain status" << std::endl;
    std::cout << "  save [filename]" << std::endl;
    std::cout << "    Save brain state to file" << std::endl;
    std::cout << "  load [filename]" << std::endl;
    std::cout << "    Load brain state from file" << std::endl;
    std::cout << "  export <format> [filename]" << std::endl;
    std::cout << "    Export brain data (txt, json, csv)" << std::endl;
    std::cout << "  help" << std::endl;
    std::cout << "    Show this help" << std::endl;
    std::cout << "  version" << std::endl;
    std::cout << "    Show version information" << std::endl;
    std::cout << "  exit|quit" << std::endl;
    std::cout << "    Exit the CLI" << std::endl;
}

void NLM_CLI_EXEC::showVersion() {
    std::cout << "NLM Phase 6 Integration" << std::endl;
    std::cout << "Version 0.1.0" << std::endl;
    std::cout << "An experimental artificial developmental brain" << std::endl;
}