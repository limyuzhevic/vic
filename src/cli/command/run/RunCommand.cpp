#include "cli/command/run/RunCommand.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm::cli;

int RunCommand::execute(const std::vector<std::string>& args) {
    NLM_LOG_INFO("=== NLM Brain Simulation Run ===");
    
    // Parse arguments
    SimulationStep steps = 100;
    std::string logLevel = "info";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--steps" && i + 1 < args.size()) {
            steps = std::stoll(args[++i]);
        } else if (args[i] == "--log-level" && i + 1 < args.size()) {
            logLevel = args[++i];
        } else if (args[i] == "--help") {
            std::cout << "Usage: nlm run [steps] [--log-level level] [--help]" << std::endl;
            std::cout << "  steps        Number of simulation steps (default: 100)" << std::endl;
            std::cout << "  --log-level   Set logging level (debug, info, warn, error)" << std::endl;
            std::cout << "  --help        Show this help message" << std::endl;
            return 0;
        }
    }
    
    NLM_LOG_INFO("Running simulation for " + std::to_string(steps) + " steps");
    NLM_LOG_INFO("Log level: " + logLevel);
    
    // Run simulation
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        if (step % 10 == 0) {
            NLM_LOG_INFO("Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
    
    return 0;
}
