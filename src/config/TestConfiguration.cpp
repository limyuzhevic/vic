// NLM (熙然) - Neural Learning Machine
// Implementation: Configuration Management
// Provides configuration presets and validation for NLM tests

#include "config/TestConfiguration.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <map>
#include <string>

using namespace nlm::config;

namespace {
    const std::map<std::string, TestConfiguration::Parameters> PRESETS = {
        {"simple", {
            42, 0.001, 100, 1, 0.1f, 0.01f, 0.015f, 20.0f, 0.00001f, 0.000001f
        }},
        {"medium", {
            123, 0.001, 500, 2, 0.15f, 0.02f, 0.015f, 20.0f, 0.0001f, 0.00001f
        }},
        {"advanced", {
            456, 0.0005, 1000, 3, 0.2f, 0.03f, 0.025f, 15.0f, 0.0005f, 0.0001f
        }},
        {"debug", {
            0, 0.01, 50, 1, 0.05f, 0.005f, 0.003f, 10.0f, 0.00001f, 0.000001f
        }}
    };
}

TestConfiguration::Parameters TestConfiguration::loadDefaults() {
    Parameters params;
    // Phase 2 defaults
    params.randomSeed = 42;
    params.simulationTimestep = 0.001;
    params.neuronCount = 500;
    params.regionCount = 1;
    params.connectionProbability = 0.15f;
    params.stdpLtpWeight = 0.02f;
    params.stdpLtdWeight = 0.015f;
    params.stdpTau = 20.0f;
    params.synaptogenesisRate = 0.0001f;
    params.pruningRate = 0.00001f;
    return params;
}

TestConfiguration::Parameters TestConfiguration::loadFromConfig(const std::shared_ptr<Config>& config) {
    Parameters params = loadDefaults();
    
    if (!config) return params;
    
    // Load configuration values with validation
    if (config->has("random_seed")) {
        params.randomSeed = config->getOr<int64_t>("random_seed", params.randomSeed);
    }
    if (config->has("simulation_timestep")) {
        params.simulationTimestep = config->getOr<double>("simulation_timestep", params.simulationTimestep);
        // Validate timestep
        if (params.simulationTimestep <= 0.0 || params.simulationTimestep > 1.0) {
            std::cerr << "Warning: Invalid simulation_timestep " << params.simulationTimestep 
                     << ". Using default value 0.001." << std::endl;
            params.simulationTimestep = 0.001;
        }
    }
    if (config->has("neuron_count")) {
        int64_t count = config->getOr<int64_t>("neuron_count", params.neuronCount);
        params.neuronCount = std::max(int64_t(50), std::min(count, int64_t(10000)));
    }
    if (config->has("region_count")) {
        int64_t regions = config->getOr<int64_t>("region_count", params.regionCount);
        params.regionCount = std::max(int64_t(1), std::min(regions, int64_t(10)));
    }
    if (config->has("connection_probability")) {
        float prob = config->getOr<float>("connection_probability", params.connectionProbability);
        params.connectionProbability = std::max(0.01f, std::min(prob, 1.0f));
    }
    if (config->has("stdp_ltp_weight")) {
        params.stdpLtpWeight = config->getOr<float>("stdp_ltp_weight", params.stdpLtpWeight);
    }
    if (config->has("stdp_ltd_weight")) {
        params.stdpLtdWeight = config->getOr<float>("stdp_ltd_weight", params.stdpLtdWeight);
    }
    if (config->has("stdp_tau")) {
        params.stdpTau = config->getOr<float>("stdp_tau", params.stdpTau);
        // Validate tau
        if (params.stdpTau <= 0.0) {
            std::cerr << "Warning: Invalid stdp_tau " << params.stdpTau 
                     << ". Using default value 20.0." << std::endl;
            params.stdpTau = 20.0f;
        }
    }
    if (config->has("synaptogenesis_rate")) {
        params.synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", params.synaptogenesisRate);
    }
    if (config->has("pruning_rate")) {
        params.pruningRate = config->getOr<float>("pruning_rate", params.pruningRate);
    }
    
    return params;
}

TestConfiguration::Parameters TestConfiguration::loadFromCommandLine(const std::shared_ptr<Config>& config, int argc, char** argv) {
    // Parse command line arguments for configuration
    Parameters params = loadFromConfig(config);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle configuration overrides from command line
        if (arg.find("--config=") == 0) {
            std::string filePath = arg.substr(9);
            // Load from file if it exists (would be implemented in config)
        } else if (arg == "--config" && i + 1 < argc) {
            std::string filePath = argv[++i];
            // Load from file if it exists
        } else if (arg.find("--seed=") == 0) {
            uint64_t seed = std::stoull(arg.substr(7));
            params.randomSeed = seed;
        } else if (arg.find("--neurons=") == 0) {
            int64_t neurons = std::stoll(arg.substr(10));
            params.neuronCount = std::max(int64_t(50), std::min(neurons, int64_t(10000)));
        } else if (arg.find("--regions=") == 0) {
            int64_t regions = std::stoll(arg.substr(10));
            params.regionCount = std::max(int64_t(1), std::min(regions, int64_t(10)));
        } else if (arg.find("--timestep=") == 0) {
            double timestep = std::stod(arg.substr(11));
            if (timestep > 0.0 && timestep <= 1.0) {
                params.simulationTimestep = timestep;
            }
        }
    }
    
    return params;
}
