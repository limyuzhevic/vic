#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// NLM includes
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"

using namespace nlm;

void printHeader() {
    std::cout << "========================================" << std::endl;
    std::cout << "  NLM (Neural Learning Machine) - C++ Demo" << std::endl;
    std::cout << "  Phase 6: Final Integration" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
}

void printBrainInfo(const Brain& brain, const AgentBrain& agent) {
    std::cout << "--- Brain Information ---" << std::endl;
    std::cout << "  Total neurons:  " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses:  " << brain.getTotalSynapseCount() << std::endl;
    std::cout << "  Active neurons:  " << brain.getActiveNeuronCount() << std::endl;
    std::cout << "  Firing neurons:  " << brain.getFiringNeuronCount() << std::endl;
    std::cout << "  Total spikes:    " << brain.getTotalSpikeCount() << std::endl;
    std::cout << "  Avg firing rate: " << brain.getAverageFiringRate() << std::endl;
    std::cout << "  E/I ratio:       " << brain.getExcitationInhibitionRatio() << std::endl;
    std::cout << "  Development stage:" << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
    std::cout << "  Curiosity level: " << agent.getCuriosityLevel() << std::endl;
    std::cout << "  Novelty level:   " << agent.getNoveltyLevel() << std::endl;
    std::cout << "  Neuromod level:  " << agent.getNeuromodulationLevel() << std::endl;
    std::cout << "  Prediction error: " << agent.getPredictionError() << std::endl;
    std::cout << std::endl;
}

void runDemo() {
    std::cout << "Initializing NLM brain (Phase 6: Integrated Artificial Brain)..." << std::endl;
    
    // Create default configuration
    auto config = std::make_shared<Config>();
    
    // Configure for demo
    config->set("brain.neuron_count", 500);
    config->set("brain.synapse_density", 0.15);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "Error: Failed to initialize brain" << std::endl;
        return;
    }
    
    // Create world
    auto world = std::make_shared<SimpleWorld>();
    world->configure(15.0f, 15.0f, 8, 8);
    world->reset();
    
    // Create agent brain interface
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Demo started! Press Ctrl+C to stop." << std::endl;
    std::cout << std::endl;
    
    // Run demo loop
    for (int step = 0; ; ++step) {
        system("clear"); // Clear screen (Unix)
        
        printHeader();
        std::cout << "Step " << step << " | Press Ctrl+C to stop" << std::endl;
        std::cout << std::endl;
        
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(step);
        
        // Decode motor command
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Print current state every 10 steps
        if (step % 10 == 0) {
            printBrainInfo(*brain, *agent);
            std::cout << "World state:" << std::endl;
            std::cout << "  Agent position: (" << world->getAgentBody().x << ", " 
                      << world->getAgentBody().y << ")" << std::endl;
            std::cout << "  Agent energy: " << world->getAgentBody().energy << "/" 
                      << world->getMaxEnergy() << std::endl;
            std::cout << "  Agent health: " << world->getAgentBody().health << std::endl;
        }
        
        // Check for quit condition
        if (step % 100 == 0) {
            std::cout << "\n" << "="*50 << std::endl;
            std::cout << "Step " << step << " | Press Ctrl+C to exit" << std::endl;
            std::cout << "="*50 << std::endl;
        }
        
        // Small delay for visualization
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    try {
        runDemo();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}
