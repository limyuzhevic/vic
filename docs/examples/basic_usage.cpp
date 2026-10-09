/**
 * NLM Basic Usage Example
 * 
 * This example demonstrates the fundamental usage of the NLM brain simulation.
 * It shows how to create a brain, run a simulation, and analyze the results.
 * 
 * Based on: src/experiments/Phase6Demo.cpp
 */

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <iomanip>

using namespace nlm;

void printSection(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void printResult(const std::string& name, float value, float max = 1.0f) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "NLM Basic Usage Example" << std::endl;
    std::cout << "========================\n";
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    printSection("Configuration Setup");
    std::cout << "Creating default configuration...\n";
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    
    printSection("Brain Creation and Initialization");
    std::cout << "Creating brain with " << config->get<size_t>("neuron_count") << " neurons...\n";
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    printSection("Basic Simulation");
    std::cout << "Running " << config->get<size_t>("neuron_count") << " neurons for 100 simulation steps...\n";
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    printSection("Results Analysis");
    std::cout << "Analyzing simulation results...\n";
    
    // Collect statistics
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    float avgFiringRate = brain->getAverageFiringRate();
    float eiRatio = brain->getExcitationInhibitionRatio();
    
    printResult("Total Neurons", static_cast<float>(totalNeurons));
    printResult("Total Synapses", static_cast<float>(totalSynapses));
    printResult("Firing Neurons", static_cast<float>(firingNeurons));
    printResult("Average Firing Rate", avgFiringRate);
    printResult("Excitatory/Inhibitory Ratio", eiRatio, 1.0f);
    
    printSection("Brain State");
    std::cout << "Brain developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    std::cout << "Total spike count: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Last update time: " << brain->getLastUpdateTime() << " ms" << std::endl;
    
    printSection("Conclusion");
    std::cout << "This example demonstrates:\n";
    std::cout << "  • Creating and configuring a brain\n";
    std::cout << "  • Running a basic simulation\n";
    std::cout << "  • Analyzing the results\n";
    std::cout << "  • Understanding brain statistics\n";
    
    return 0;
}
