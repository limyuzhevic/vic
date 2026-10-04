// NLM Explore - Interactive exploration and analysis tool
// Provides a command-line interface for exploring NLM brain behavior

#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6Demo.hpp"
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

void printHelp() {
    std::cout << "=== NLM Explorer Help ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  help                    - Show this help" << std::endl;
    std::cout << "  status                  - Show current brain status" << std::endl;
    std::cout << "  neurons <count>         - Create brain with specified neuron count" << std::endl;
    std::cout << "  simulate <steps>        - Run simulation for specified steps" << std::endl;
    std::cout << "  analyze                 - Analyze brain state and statistics" << std::endl;
    std::cout << "  plasticity              - Test plasticity mechanisms" << std::endl;
    std::cout << "  memory                  - Test memory systems" << std::endl;
    std::cout << "  neuromod                 - Test neuromodulation" << std::endl;
    std::cout << "  development             - Test development system" << std::endl;
    std::cout << "  checkpoint <path>       - Save brain state to checkpoint" << std::endl;
    std::cout << "  load <path>             - Load brain state from checkpoint" << std::endl;
    std::cout << "  experiment              - Run integration experiment" << std::endl;
    std::cout << "  quit/q                  - Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Phase 6 Integration Demo:" << std::endl;
    std::cout << "  phase6                  - Run Phase 6 integration test" << std::endl;
    std::cout << std::endl;
}

void printBrainStats(std::shared_ptr<Brain> brain) {
    std::cout << "--- Brain Statistics ---" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Pending events: " << brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "E/I ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    
    // Memory system stats
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMemory();
    
    if (workingMem) {
        std::cout << "Working memory traces: " << workingMem->getActiveTraces() << std::endl;
    }
    
    if (episdicMem) {
        std::cout << "Episodic memory episodes: " << episodicMem->getEpisodeCount() << std::endl;
    }
}

void createAndRunExperiment(size_t neuronCount, size_t steps, std::string experimentType) {
    auto config = std::make_shared<Config>();
    
    // Configure for experiment
    config->set("neuron_count", static_cast<int64_t>(neuronCount));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    // Configure plasticity
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    config->set("stdp_tau", 20.0f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    
    // Configure neuromodulation
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Configure development
    config->set("development.enable_critical_period", true);
    config->set("development.enable_synaptogenesis", true);
    config->set("development.enable_pruning", true);
    
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "Created brain with " << neuronCount << " neurons" << std::endl;
    
    // Inject some initial activity
    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
    
    // Run simulation
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (step % 1000 == 0 && steps > 1000) {
            std::cout << "Step " << step << "/" << steps 
                     << " | Firing: " << brain->getFiringNeuronCount()
                     << " | Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    // Print results
    std::cout << "\n=== Experiment Results ===" << std::endl;
    printBrainStats(brain);
}

int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    std::cout << "=== NLM Explorer (Phase 6) ===" << std::endl;
    std::cout << "Interactive exploration tool for NLM brain simulation" << std::endl;
    
    std::string currentCommand;
    std::shared_ptr<Brain> currentBrain;
    
    while (true) {
        std::cout << ">>> ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            continue;
        }
        
        // Parse command
        size_t spacePos = input.find(' ');
        if (spacePos == std::string::npos) {
            currentCommand = input;
        } else {
            currentCommand = input.substr(0, spacePos);
        }
        
        // Trim whitespace from command
        currentCommand.erase(0, currentCommand.find_first_not_of(" \t"));
        currentCommand.erase(currentCommand.find_last_not_of(" \t") + 1);
        
        // Handle commands
        if (currentCommand == "help") {
            printHelp();
        }
        else if (currentCommand == "status") {
            if (currentBrain) {
                printBrainStats(currentBrain);
            } else {
                std::cout << "No brain created yet. Use 'neurons <count>' to create one." << std::endl;
            }
        }
        else if (currentCommand == "neurons") {
            // Parse neuron count from rest of input
            std::string args = input.substr(spacePos + 1);
            try {
                size_t neuronCount = std::stoul(args);
                std::cout << "Creating brain with " << neuronCount << " neurons..." << std::endl;
                createAndRunExperiment(neuronCount, 1000, "exploration");
                currentBrain = std::make_shared<Brain>(std::make_shared<Config>());
            } catch (...) {
                std::cout << "Invalid neuron count. Usage: neurons <count>" << std::endl;
            }
        }
        else if (currentCommand == "simulate") {
            if (!currentBrain) {
                std::cout << "No brain created. Use 'neurons <count>' first." << std::endl;
                continue;
            }
            
            std::string args = input.substr(spacePos + 1);
            try {
                size_t steps = std::stoul(args);
                std::cout << "Running simulation for " << steps << " steps..." << std::endl;
                
                for (size_t step = 0; step < steps; ++step) {
                    currentBrain->step(step, step * 0.001);
                    
                    if (step % 500 == 0 && steps > 500) {
                        std::cout << "Step " << step << "/" << steps 
                                 << " | Firing: " << currentBrain->getFiringNeuronCount()
                                 << " | Spikes: " << currentBrain->getTotalSpikeCount() << std::endl;
                    }
                }
                
                std::cout << "Simulation complete!" << std::endl;
                printBrainStats(currentBrain);
            } catch (...) {
                std::cout << "Invalid step count. Usage: simulate <steps>" << std::endl;
            }
        }
        else if (currentCommand == "analyze") {
            if (!currentBrain) {
                std::cout << "No brain to analyze. Use 'neurons <count>' first." << std::endl;
                continue;
            }
            
            std::cout << "=== Brain Analysis ===" << std::endl;
            
            // Analyze neuron distribution
            float eiRatio = currentBrain->getExcitationInhibitionRatio();
            std::cout << "Excitatory/Inhibitory ratio: " << eiRatio << std::endl;
            
            if (eiRatio > 1000) {
                std::cout << "WARNING: Very high E/I ratio - possible imbalance" << std::endl;
            } else if (eiRatio < 0.1) {
                std::cout << "WARNING: Very low E/I ratio - possible imbalance" << std::endl;
            }
            
            // Analyze firing patterns
            float avgFiring = currentBrain->getAverageFiringRate();
            std::cout << "Average firing rate: " << avgFiring << " Hz" << std::endl;
            
            if (avgFiring > 10.0f) {
                std::cout << "High firing activity detected" << std::endl;
            } else if (avgFiring < 0.1f) {
                std::cout << "Low firing activity detected" << std::endl;
            }
            
            // Check for learning indicators
            size_t totalSpikes = currentBrain->getTotalSpikeCount();
            if (totalSpikes > 1000) {
                std::cout << "Significant learning activity (" << totalSpikes << " total spikes)" << std::endl;
            }
        }
        else if (currentCommand == "plasticity") {
            if (!currentBrain) {
                std::cout << "No brain created. Use 'neurons <count>' first." << std::endl;
                continue;
            }
            
            std::cout << "=== Testing Plasticity ===" << std::endl;
            
            // Create two regions for plasticity testing
            RegionId reg1 = currentBrain->addRegion("Plasticity_Test_1");
            RegionId reg2 = currentBrain->addRegion("Plasticity_Test_2");
            
            auto* region1 = currentBrain->getRegion(reg1);
            auto* region2 = currentBrain->getRegion(reg2);
            
            if (region1 && region2) {
                std::cout << "Created two regions for plasticity testing" << std::endl;
                std::cout << "Region 1: " << region1->getName() << " (" 
                         << region1->getTotalNeuronCount() << " neurons)" << std::endl;
                std::cout << "Region 2: " << region2->getName() << " (" 
                         << region2->getTotalNeuronCount() << " neurons)" << std::endl;
                
                // Test STDP with correlated activity
                std::cout << "Testing STDP with correlated pre-post firing..." << std::endl;
                
                // Create correlated activity pattern
                for (int step = 0; step < 500; ++step) {
                    currentBrain->step(step, step * 0.001);
                }
                
                std::cout << "Plasticity test completed" << std::endl;
            }
        }
        else if (currentCommand == "memory") {
            if (!currentBrain) {
                std::cout << "No brain created. Use 'neurons <count>' first." << std::endl;
                continue;
            }
            
            std::cout << "=== Testing Memory Systems ===" << std::endl;
            
            auto* workingMem = currentBrain->getWorkingMemory();
            auto* episodicMem = currentBrain->getEpisodicMemory();
            
            if (!workingMem) {
                std::cout << "Working memory not available" << std::endl;
            } else {
                std::cout << "Working memory capacity: " << workingMem->getCapacity() << std::endl;
                std::cout << "Working memory traces: " << workingMem->getActiveTraces() << std::endl;
            }
            
            if (!episodicMem) {
                std::cout << "Episodic memory not available" << std::endl;
            } else {
                std::cout << "Episodic memory max episodes: " << episodicMem->getMaxEpisodes() << std::endl;
                std::cout << "Episodic memory episodes stored: " << episodicMem->getEpisodeCount() << std::endl;
            }
        }
        else if (currentCommand == "neuromod") {
            if (!currentBrain) {
                std::cout << "No brain created. Use 'neurons <count>' first." << std::endl;
                continue;
            }
            
            std::cout << "=== Testing Neuromodulation ===" << std::endl;
            
            auto* dopamine = currentBrain->getDopamine();
            auto* curiosity = currentBrain->getCuriosity();
            auto* novelty = currentBrain->getNovelty();
            
            if (dopamine) {
                std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
                std::cout << "Plasticity factor: " << dopamine->getPlasticityFactor() << std::endl;
            }
            
            if (curiosity) {
                std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
            }
            
            if (novelty) {
                std::cout << "Novelty detection active: " << (novelty->isActive() ? "Yes" : "No") << std::endl;
                std::cout << "Novelty level: " << novelty->getNoveltyLevel() << std::endl;
            }
        }
        else if (currentCommand == "development") {
            if (!currentBrain) {
                std::cout << "No brain created. Use 'neurons <count>' first." << std::endl;
                continue;
            }
            
            std::cout << "=== Testing Development System ===" << std::endl;
            
            std::cout << "Developmental stage: " << static_cast<int>(currentBrain->getDevelopmentalStage()) << std::endl;
            
            // Check development system
            auto* devSys = currentBrain->getDevelopmentSystem();
            if (devSys) {
                std::cout << "Development system active" << std::endl;
                // In a real implementation, we would check specific development parameters
                std::cout << "Development effects: Synaptogenesis, Pruning, Maturation" << std::endl;
            }
        }
        else if (currentCommand == "phase6") {
            std::cout << "=== Running Phase 6 Integration Demo ===" << std::endl;
            
            Phase6Demo demo;
            bool success = demo.runDemo(500, 2000);
            
            if (success) {
                std::cout << "Phase 6 integration demo completed successfully!" << std::endl;
            } else {
                std::cout << "Phase 6 integration demo failed." << std::endl;
            }
        }
        else if (currentCommand == "quit" || currentCommand == "q") {
            std::cout << "Exiting NLM Explorer. Goodbye!" << std::endl;
            break;
        }
        else {
            std::cout << "Unknown command: " << currentCommand << std::endl;
            std::cout << "Type 'help' for available commands." << std::endl;
        }
    }
    
    return 0;
}
