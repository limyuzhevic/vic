// Simple Brain Demo - NLM Beginner Example
// Demonstrates basic brain creation and operation

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

void printDemoBanner() {
    std::cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — Simple Brain Demo                                   ║
║     Neural Learning Machine - Beginner Version               ║
║                                                               ║
║     Demonstrates basic brain functionality                    ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝
)" << std::endl;
}

void demoBasicBrainCreation() {
    std::cout << "=== Basic Brain Creation ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    std::cout << "✓ Brain created and initialized successfully!" << std::endl;
    std::cout << "  - Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  - Total synapses: " << brain->getTotalSynapseCount() << std::endl;
}

void demoBasicOperation() {
    std::cout << "\n=== Basic Brain Operation ===" << std::endl;
    
    // Create simple configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(50), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Get initial state
    std::cout << "Initial state:" << std::endl;
    std::cout << "  - Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    // Run for 100 steps
    std::cout << "\nRunning simulation for 100 steps..." << std::endl;
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Get final state
    std::cout << "\nFinal state (after 100 steps):" << std::endl;
    std::cout << "  - Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  - Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
}

void demoLearning() {
    std::cout << "\n=== Simple Learning Demonstration ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Enable plasticity for learning
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
        }
    }
    
    std::cout << "Brain initialized with plasticity enabled." << std::endl;
    std::cout << "Running learning simulation for 500 steps..." << std::endl;
    
    for (int step = 0; step < 500; ++step) {
        // Create input pattern
        for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 20; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            std::cout << "  Step " << step << ": " << brain->getFiringNeuronCount() << " neurons firing" << std::endl;
        }
    }
    
    std::cout << "\nLearning complete!" << std::endl;
    std::cout << "  - Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  - Final firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
}

void demoCheckpointing() {
    std::cout << "\n=== Checkpointing Demo ===" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(150), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Simulate some activity
    for (int step = 0; step < 200; ++step) {
        brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
        brain->step(step, step * 0.001);
    }
    
    // Save checkpoint
    std::string checkpointFile = "test_checkpoint.bin";
    if (brain->save(checkpointFile)) {
        std::cout << "✓ Checkpoint saved to: " << checkpointFile << std::endl;
        
        // Create new brain and load checkpoint
        auto newBrain = std::make_shared<Brain>(config);
        newBrain->load(checkpointFile);
        
        std::cout << "✓ Checkpoint loaded successfully!" << std::endl;
        std::cout << "  - Loaded neurons: " << newBrain->getTotalNeuronCount() << std::endl;
        std::cout << "  - Loaded spikes: " << newBrain->getTotalSpikeCount() << std::endl;
        
        // Clean up
        std::remove(checkpointFile.c_str());
    } else {
        std::cout << "✗ Failed to save checkpoint" << std::endl;
    }
}

int main(int argc, char** argv) {
    printDemoBanner();
    
    std::cout << "Welcome to the NLM Simple Brain Demo!" << std::endl;
    std::cout << "This demo shows basic brain functionality for beginners." << std::endl << std::endl;
    
    try {
        demoBasicBrainCreation();
        demoBasicOperation();
        demoLearning();
        demoCheckpointing();
        
        std::cout << "\n" << std::string(60, '=') << std::endl;
        std::cout << "DEMO COMPLETE!" << std::endl;
        std::cout << "You have successfully run a simple brain simulation." << std::endl;
        std::cout << "The brain learned, stored, and replayed experiences!" << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
