/**
 * NLM Sensory-Motor Loop Example
 * 
 * This example demonstrates the complete sensory-motor integration loop
 * in NLM. It shows how perception, decision making, and action are
 * connected in the neural processing pipeline.
 * 
 * Based on: src/agent/AgentBrain.cpp, src/world/SimpleWorld.hpp
 */

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

using namespace nlm;

void printProgress(int step, int totalSteps, float curiosity, float novelty, float development) {
    std::cout << "\rStep: " << std::setw(4) << step << "/" << totalSteps 
              << "  Curiosity: " << std::setw(6) << std::fixed << std::setprecision(3) << curiosity
              << "  Novelty: " << std::setw(6) << std::setprecision(3) << novelty
              << "  Dev: " << std::setw(6) << std::setprecision(3) << development
              << "  Neurs: " << std::setw(6) << brain.getFiringNeuronCount()
              << "   ";
    std::cout.flush();
}

int main() {
    std::cout << "NLM Sensory-Motor Integration Example" << std::endl;
    std::cout << "====================================\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Warning);
    
    printSection("Setup Complete");
    std::cout << "Running sensory-motor loop demonstration...\n";
    std::cout << std::string(80, '-') << "\n";
    
    printSection("Create Components");
    std::cout << "1. Creating brain configuration...\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(500));
    config->set("region_count", size_t(1));
    config->set("vision_width", 8);
    config->set("vision_height", 8);
    config->set("vision_range", 5.0f);
    
    printSection("Create Brain");
    std::cout << "2. Creating brain...\n";
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    std::cout << "   Brain initialized with " << brain->getTotalNeuronCount() << " neurons\n";
    
    printSection("Create World");
    std::cout << "3. Creating world environment...\n";
    
    auto world = std::make_shared<SimpleWorld>();
    world->configure(
        config->get<int>("width", 20),
        config->get<int>("height", 20),
        config->get<int>("vision_width", 8),
        config->get<int>("vision_height", 8)
    );
    world->reset();
    
    printSection("Create Agent");
    std::cout << "4. Creating agent interface...\n";
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(world.get());
    
    printSection("Enable Features");
    std::cout << "5. Enabling brain subsystems...\n";
    
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    agent->enableDevelopment(true);
    agent->enableCuriosity(true);
    agent->enableNoveltyDetection(true);
    
    printSection("Set Agent Start");
    std::cout << "6. Placing agent in world...\n";
    
    float startX = static_cast<float>(world->getWidth()) / 2.0f;
    float startY = static_cast<float>(world->getHeight()) / 2.0f;
    world->setAgentStart(startX, startY);
    
    printSection("Run Simulation");
    std::cout << "7. Running simulation...\n";
    std::cout << std::string(80, '-') << "\n";
    
    int totalSteps = 500;
    bool resourceFound = false;
    
    for (int step = 0; step < totalSteps; ++step) {
        // Update world
        world->update(0.1f);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode motor command
        auto action = agent->decodeMotorCommand();
        
        // Apply motor command
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        float reward = 0.0f;
        if (percept.getResources().size() > 0) {
            reward = 1.0f;  // Reward for resource proximity
            resourceFound = true;
        }
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1f);
        
        // Print progress every 50 steps
        if (step % 50 == 0 || step == totalSteps - 1) {
            float curiosityLevel = agent->getCuriosityLevel();
            float noveltyLevel = agent->getNoveltyLevel();
            float devStage = brain->getDevelopmentalStage();
            
            printProgress(step, totalSteps, curiosityLevel, noveltyLevel, devStage);
            
            if (step % 100 == 0) {
                std::cout << "\n   " << (resourceFound ? "Resource found!" : "Exploring...") << std::endl;
            }
        }
    }
    
    printSection("Final Results");
    std::cout << std::string(80, '-') << "\n";
    
    int totalSpikes = brain->getTotalSpikeCount();
    float avgFiringRate = brain->getAverageFiringRate();
    float finalCuriosity = agent->getCuriosityLevel();
    float finalNovelty = agent->getNoveltyLevel();
    
    std::cout << "Simulation completed!\n";
    std::cout << std::endl;
    std::cout << "Final Statistics:\n";
    std::cout << "  Total spikes: " << totalSpikes << "\n";
    std::cout << "  Average firing rate: " << std::fixed << std::setprecision(2) << avgFiringRate << " Hz\n";
    std::cout << "  Final curiosity level: " << finalCuriosity << "\n";
    std::cout << "  Final novelty level: " << finalNovelty << "\n";
    std::cout << "  Brain developmental stage: " << brain->getDevelopmentalStage() << "\n";
    
    printSection("Experience Summary");
    std::cout << "\nThe agent experienced:\n";
    std::cout << "  • Continuous sensory input from the world\n";
    std::cout << "  • Motor action selection based on neural processing\n";
    std::cout << "  • Reward modulation affecting learning\n";
    std::cout << "  • Developmental changes over time\n";
    std::cout << "  • Curiosity-driven exploration behavior\n";
    
    return 0;
}
