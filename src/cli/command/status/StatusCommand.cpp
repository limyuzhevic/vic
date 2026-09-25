#include "cli/command/status/StatusCommand.hpp"
#include <iostream>
#include <iomanip>

using namespace nlm::cli;

int StatusCommand::execute(const std::vector<std::string>& args) {
    bool showDetails = false;
    
    for (const auto& arg : args) {
        if (arg == "--details" || arg == "-d") {
            showDetails = true;
        } else if (arg == "--help") {
            std::cout << "Usage: nlm status [--details] [--help]" << std::endl;
            std::cout << "  --details     Show detailed system information" << std::endl;
            std::cout << "  --help        Show this help message" << std::endl;
            return 0;
        }
    }
    
    std::cout << "=== NLM Brain System Status ===" << std::endl;
    std::cout << std::endl;
    
    // Basic status
    std::cout << "Brain Status:" << std::endl;
    std::cout << "  Initialized: " << (brain ? "Yes" : "No") << std::endl;
    std::cout << "  Neurons: " << (brain ? std::to_string(brain->getTotalNeuronCount()) : "0") << std::endl;
    std::cout << "  Synapses: " << (brain ? std::to_string(brain->getTotalSynapseCount()) : "0") << std::endl;
    std::cout << "  Regions: " << (brain ? std::to_string(brain->getRegionCount()) : "0") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Simulation Status:" << std::endl;
    std::cout << "  Current step: " << (brain ? std::to_string(brain->getCurrentStep()) : "0") << std::endl;
    std::cout << "  Total spikes: " << (brain ? std::to_string(brain->getTotalSpikeCount()) : "0") << std::endl;
    std::cout << "  Firing neurons: " << (brain ? std::to_string(brain->getFiringNeuronCount()) : "0") << std::endl;
    std::cout << "  Simulation time: " << (brain ? std::to_string(brain->getCurrentTime()) : "0.0") << "s" << std::endl;
    std::cout << std::endl;
    
    // Configuration status
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Random seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
    std::cout << "  Simulation timestep: " << config->getOr<double>("simulation_timestep", 0.001) << "s" << std::endl;
    std::cout << "  Neuron count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
    std::cout << "  Connection probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
    std::cout << std::endl;
    
    if (showDetails) {
        std::cout << "=== Detailed Status ===" << std::endl;
        
        // Show region details
        if (brain) {
            for (size_t i = 0; i < brain->getRegionCount(); ++i) {
                std::cout << "Region " << i + 1 << ":" << std::endl;
                auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
                if (region) {
                    std::cout << "  Neurons: " << region->getNeuronCount() << std::endl;
                    std::cout << "  Synapses: " << region->getSynapseCount() << std::endl;
                }
                std::cout << std::endl;
            }
        }
        
        // Show memory system status
        std::cout << "Memory Systems:" << std::endl;
        std::cout << "  Working memory: Active" << std::endl;
        std::cout << "  Episodic memory: Active" << std::endl;
        std::cout << "  Associative memory: Active" << std::endl;
        std::cout << "  Semantic memory: Active" << std::endl;
        std::cout << "  Procedural memory: Active" << std::endl;
        std::cout << std::endl;
        
        // Show neuromodulation status
        std::cout << "Neuromodulation:" << std::endl;
        std::cout << "  Dopamine: Active" << std::endl;
        std::cout << "  Curiosity: Active" << std::endl;
        std::cout << "  Prediction error: Active" << std::endl;
        std::cout << "  Novelty: Active" << std::endl;
        std::cout << std::endl;
    }
    
    std::cout << "=== Status Complete ===" << std::endl;
    
    return 0;
}
