// Common configuration utilities for NLM demos
#ifndef NLM_DEMO_CONFIG_H
#define NLM_DEMO_CONFIG_H

#include "core/Config/Config.hpp"
#include <memory>
#include <string>

namespace nlm {
namespace demo {

/**
 * Create and configure a default brain configuration
 * Used by all demo executables to ensure consistent settings
 */
std::shared_ptr<Config> createDefaultBrainConfig() {
    auto config = std::make_shared<Config>();
    
    // Core brain parameters
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    return config;
}

/**
 * Parse command line arguments for demo configuration
 * Supports --neuron_count, --region_count, --random_seed, --config
 */
void parseDemoArgs(int argc, char* argv[], std::shared_ptr<Config> config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.find("--neuron_count=") == 0) {
            config->set("neuron_count", std::stoll(arg.substr(14)));
        } else if (arg.find("--region_count=") == 0) {
            config->set("region_count", std::stoll(arg.substr(14)));
        } else if (arg.find("--random_seed=") == 0) {
            config->set("random_seed", std::stoll(arg.substr(13)));
        } else if (arg.find("--config=") == 0) {
            // Config file will be loaded by the main brain initialization
        } else if (arg == "--help" || arg == "-h") {
            printDemoHelp();
            exit(0);
        }
    }
}

/**
 * Print help message for demo executables
 */
void printDemoHelp() {
    std::cout << "NLM Demo Executables Usage:\n"
              << "  --neuron_count=N      Number of neurons (default: 500)\n"
              << "  --region_count=N      Number of brain regions (default: 1)\n"
              << "  --random_seed=N       Random seed for reproducibility (default: 42)\n"
              << "  --config=FILE         Load configuration from file\n"
              << "  --help, -h            Show this help message\n"
              << std::endl;
}

/**
 * Validate configuration parameters
 */
bool validateConfig(std::shared_ptr<Config> config) {
    // Ensure we have reasonable values
    if (config->getOr<int64_t>("neuron_count", 500) <= 0) {
        std::cerr << "Error: neuron_count must be positive" << std::endl;
        return false;
    }
    
    if (config->getOr<int64_t>("region_count", 1) <= 0) {
        std::cerr << "Error: region_count must be positive" << std::endl;
        return false;
    }
    
    float connectionProb = config->getOr<float>("connection_probability", 0.1f);
    if (connectionProb < 0.0f || connectionProb > 1.0f) {
        std::cerr << "Error: connection_probability must be between 0.0 and 1.0" << std::endl;
        return false;
    }
    
    return true;
}

} // namespace demo
} // namespace nlm

#endif // NLM_DEMO_CONFIG_H