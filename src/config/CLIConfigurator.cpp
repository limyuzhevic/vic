#include "CLIConfigurator.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace nlm {

std::shared_ptr<Config> CLIConfigurator::createConfigFromArgs(int argc, char* argv[]) {
    NLM_LOG_INFO("Parsing command-line arguments for configuration...");
    
    auto config = getDefaultConfig();
    
    // Parse command-line arguments
    std::map<std::string, std::string> keyValues = parseKeyValueArgs(argc, argv);
    
    // Apply parsed key-value pairs to configuration
    for (const auto& pair : keyValues) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        // Try to set each possible configuration parameter
        if (key == "neuron_count" || key == "neurons") {
            int64_t neurons;
            if (convertValue(value, neurons)) {
                config->set<int64_t>("neuron_count", neurons);
                NLM_LOG_INFO("Set neuron_count: " + std::to_string(neurons));
            }
        } else if (key == "region_count" || key == "regions") {
            int64_t regions;
            if (convertValue(value, regions)) {
                config->set<int64_t>("region_count", regions);
                NLM_LOG_INFO("Set region_count: " + std::to_string(regions));
            }
        } else if (key == "connection_probability" || key == "conn_prob") {
            float prob;
            if (convertValue(value, prob)) {
                config->set<float>("connection_probability", prob);
                NLM_LOG_INFO("Set connection_probability: " + std::to_string(prob));
            }
        } else if (key == "simulation_timestep" || key == "timestep") {
            double timestep;
            if (convertValue(value, timestep)) {
                config->set<double>("simulation_timestep", timestep);
                NLM_LOG_INFO("Set simulation_timestep: " + std::to_string(timestep));
            }
        } else if (key == "random_seed" || key == "seed") {
            int64_t seed;
            if (convertValue(value, seed)) {
                config->set<int64_t>("random_seed", seed);
                NLM_LOG_INFO("Set random_seed: " + std::to_string(seed));
            }
        } else if (key == "replay_interval") {
            int64_t interval;
            if (convertValue(value, interval)) {
                config->set<int64_t>("replay_interval", interval);
                NLM_LOG_INFO("Set replay_interval: " + std::to_string(interval));
            }
        } else if (key == "consolidation_interval") {
            int64_t interval;
            if (convertValue(value, interval)) {
                config->set<int64_t>("consolidation_interval", interval);
                NLM_LOG_INFO("Set consolidation_interval: " + std::to_string(interval));
            }
        } else if (key == "checkpoint_dir" || key == "checkpoint_path") {
            config->set<std::string>("checkpoint_dir", value);
            NLM_LOG_INFO("Set checkpoint_dir: " + value);
        } else if (key == "log_level") {
            // Convert string log level to enum
            std::string level = value;
            std::transform(level.begin(), level.end(), level.begin(), ::tolower);
            if (level == "debug") {
                config->set<int64_t>("log_level", 0);
            } else if (level == "info") {
                config->set<int64_t>("log_level", 1);
            } else if (level == "warning") {
                config->set<int64_t>("log_level", 2);
            } else if (level == "error") {
                config->set<int64_t>("log_level", 3);
            } else if (level == "critical") {
                config->set<int64_t>("log_level", 4);
            }
            NLM_LOG_INFO("Set log_level: " + level);
        } else if (key == "stdp_ltp_weight") {
            float weight;
            if (convertValue(value, weight)) {
                config->set<float>("stdp_ltp_weight", weight);
                NLM_LOG_INFO("Set stdp_ltp_weight: " + std::to_string(weight));
            }
        } else if (key == "stdp_ltd_weight") {
            float weight;
            if (convertValue(value, weight)) {
                config->set<float>("stdp_ltd_weight", weight);
                NLM_LOG_INFO("Set stdp_ltd_weight: " + std::to_string(weight));
            }
        } else if (key == "stdp_tau") {
            float tau;
            if (convertValue(value, tau)) {
                config->set<float>("stdp_tau", tau);
                NLM_LOG_INFO("Set stdp_tau: " + std::to_string(tau));
            }
        } else if (key == "synaptogenesis_rate") {
            float rate;
            if (convertValue(value, rate)) {
                config->set<float>("synaptogenesis_rate", rate);
                NLM_LOG_INFO("Set synaptogenesis_rate: " + std::to_string(rate));
            }
        } else if (key == "pruning_rate") {
            float rate;
            if (convertValue(value, rate)) {
                config->set<float>("pruning_rate", rate);
                NLM_LOG_INFO("Set pruning_rate: " + std::to_string(rate));
            }
        } else {
            // Unknown key - could be logged as warning
            NLM_LOG_WARNING("Unknown configuration key: " + key);
        }
    }
    
    // Validate configuration
    std::string errorMessage;
    if (!validateConfig(config, errorMessage)) {
        NLM_LOG_ERROR("Configuration validation failed: " + errorMessage);
        return nullptr;
    }
    
    NLM_LOG_INFO("Configuration parsing complete");
    return config;
}

std::shared_ptr<Config> CLIConfigurator::loadConfigFromFile(const std::string& filename) {
    NLM_LOG_INFO("Loading configuration from file: " + filename);
    
    std::map<std::string, std::string> keyValues = parseFileConfig(filename);
    
    if (keyValues.empty()) {
        NLM_LOG_WARNING("No configuration found in file: " + filename);
        return getDefaultConfig();
    }
    
    auto config = getDefaultConfig();
    
    for (const auto& pair : keyValues) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        // Convert value based on key type (simplified)
        if (key == "neuron_count" || key == "regions" || key == "random_seed" || 
            key == "replay_interval" || key == "consolidation_interval") {
            int64_t intValue;
            if (convertValue(value, intValue)) {
                config->set<int64_t>(key, intValue);
            }
        } else if (key == "connection_probability" || key == "simulation_timestep" || 
                   key == "stdp_ltp_weight" || key == "stdp_ltd_weight" || 
                   key == "stdp_tau" || key == "synaptogenesis_rate" || 
                   key == "pruning_rate") {
            float floatValue;
            if (convertValue(value, floatValue)) {
                config->set<float>(key, floatValue);
            }
        } else if (key == "checkpoint_dir" || key == "log_file") {
            config->set<std::string>(key, value);
        }
    }
    
    NLM_LOG_INFO("Configuration loaded from file: " + filename);
    return config;
}

bool CLIConfigurator::saveConfigToFile(const std::shared_ptr<const Config>& config, const std::string& filename) {
    NLM_LOG_INFO("Saving configuration to file: " + filename);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    // Write configuration as key=value pairs
    file << "# NLM Brain Configuration File\n";
    file << "# Generated by CLIConfigurator\n\n";
    
    // Try to get some common configuration values
    auto neuronCount = config->get<int64_t>("neuron_count");
    if (neuronCount) {
        file << "neuron_count = " << *neuronCount << "\n";
    }
    
    auto regionCount = config->get<int64_t>("region_count");
    if (regionCount) {
        file << "region_count = " << *regionCount << "\n";
    }
    
    auto connProb = config->get<float>("connection_probability");
    if (connProb) {
        file << "connection_probability = " << *connProb << "\n";
    }
    
    auto timestep = config->get<double>("simulation_timestep");
    if (timestep) {
        file << "simulation_timestep = " << *timestep << "\n";
    }
    
    auto seed = config->get<int64_t>("random_seed");
    if (seed) {
        file << "random_seed = " << *seed << "\n";
    }
    
    auto replayInterval = config->get<int64_t>("replay_interval");
    if (replayInterval) {
        file << "replay_interval = " << *replayInterval << "\n";
    }
    
    auto consolidationInterval = config->get<int64_t>("consolidation_interval");
    if (consolidationInterval) {
        file << "consolidation_interval = " << *consolidationInterval << "\n";
    }
    
    auto checkpointDir = config->get<std::string>("checkpoint_dir");
    if (checkpointDir) {
        file << "checkpoint_dir = " << *checkpointDir << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Configuration saved to file: " + filename);
    return true;
}

void CLIConfigurator::printConfig(const std::shared_ptr<const Config>& config) {
    NLM_LOG_INFO("=== Current Configuration ===");
    
    auto neuronCount = config->get<int64_t>("neuron_count");
    if (neuronCount) {
        NLM_LOG_INFO("Neuron count: " + std::to_string(*neuronCount));
    }
    
    auto regionCount = config->get<int64_t>("region_count");
    if (regionCount) {
        NLM_LOG_INFO("Region count: " + std::to_string(*regionCount));
    }
    
    auto connProb = config->get<float>("connection_probability");
    if (connProb) {
        NLM_LOG_INFO("Connection probability: " + std::to_string(*connProb));
    }
    
    auto timestep = config->get<double>("simulation_timestep");
    if (timestep) {
        NLM_LOG_INFO("Simulation timestep: " + std::to_string(*timestep));
    }
    
    auto seed = config->get<int64_t>("random_seed");
    if (seed) {
        NLM_LOG_INFO("Random seed: " + std::to_string(*seed));
    }
    
    auto replayInterval = config->get<int64_t>("replay_interval");
    if (replayInterval) {
        NLM_LOG_INFO("Replay interval: " + std::to_string(*replayInterval));
    }
    
    auto consolidationInterval = config->get<int64_t>("consolidation_interval");
    if (consolidationInterval) {
        NLM_LOG_INFO("Consolidation interval: " + std::to_string(*consolidationInterval));
    }
    
    auto checkpointDir = config->get<std::string>("checkpoint_dir");
    if (checkpointDir) {
        NLM_LOG_INFO("Checkpoint directory: " + *checkpointDir);
    }
    
    NLM_LOG_INFO("===========================");
}

void CLIConfigurator::printHelp(const std::string& programName) {
    std::cout << "NLM Brain - Neural Learning Machine\n";
    std::cout << "Usage: " << programName << " [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --neuron-count N, -n N      Number of neurons (default: 1000)\n";
    std::cout << "  --region-count R, -r R      Number of regions (default: 1)\n";
    std::cout << "  --connection-prob P, -p P  Connection probability (default: 0.1)\n";
    std::cout << "  --timestep T, -t T          Simulation timestep (default: 0.001)\n";
    std::cout << "  --seed S, -s S              Random seed (default: 42)\n";
    std::cout << "  --replay-interval I         Replay interval in steps (default: 100)\n";
    std::cout << "  --consolidation-interval I   Consolidation interval in steps (default: 1000)\n";
    std::cout << "  --checkpoint-dir D, -c D    Checkpoint directory (default: ./checkpoints)\n";
    std::cout << "  --log-level L, -l L         Log level (debug, info, warning, error, critical)\n";
    std::cout << "  --load-config F, -f F       Load configuration from file\n";
    std::cout << "  --save-config F, -S F       Save configuration to file\n";
    std::cout << "  --help, -h                  Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " --neuron-count 500 --region-count 4\n";
    std::cout << "  " << programName << " --load-config config.txt --save-config custom_config.txt\n";
    std::cout << "  " << programName << " --neuron-count 2000 --seed 12345\n\n";
}

std::shared_ptr<Config> CLIConfigurator::getDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set<int64_t>("neuron_count", 1000);
    config->set<int64_t>("region_count", 1);
    config->set<float>("connection_probability", 0.1f);
    config->set<double>("simulation_timestep", 0.001);
    config->set<int64_t>("random_seed", 42);
    config->set<int64_t>("replay_interval", 100);
    config->set<int64_t>("consolidation_interval", 1000);
    config->set<std::string>("checkpoint_dir", "./checkpoints");
    config->set<int64_t>("log_level", 1);  // Info level
    
    // Set default plasticity parameters
    config->set<float>("stdp_ltp_weight", 0.01f);
    config->set<float>("stdp_ltd_weight", 0.012f);
    config->set<float>("stdp_tau", 20.0f);
    config->set<float>("synaptogenesis_rate", 0.0001f);
    config->set<float>("pruning_rate", 0.00001f);
    
    return config;
}

bool CLIConfigurator::validateConfig(const std::shared_ptr<const Config>& config, std::string& errorMessage) {
    auto neuronCount = config->get<int64_t>("neuron_count");
    if (!neuronCount || *neuronCount <= 0) {
        errorMessage = "Invalid neuron_count: must be positive";
        return false;
    }
    
    auto regionCount = config->get<int64_t>("region_count");
    if (!regionCount || *regionCount <= 0) {
        errorMessage = "Invalid region_count: must be positive";
        return false;
    }
    
    auto connProb = config->get<float>("connection_probability");
    if (!connProb || *connProb < 0.0f || *connProb > 1.0f) {
        errorMessage = "Invalid connection_probability: must be between 0.0 and 1.0";
        return false;
    }
    
    auto timestep = config->get<double>("simulation_timestep");
    if (!timestep || *timestep <= 0.0) {
        errorMessage = "Invalid simulation_timestep: must be positive";
        return false;
    }
    
    // Check that regions don't exceed neurons
    if (*regionCount > *neuronCount) {
        errorMessage = "Invalid configuration: region_count cannot exceed neuron_count";
        return false;
    }
    
    return true;
}

std::map<std::string, std::string> CLIConfigurator::parseKeyValueArgs(int argc, char* argv[]) {
    std::map<std::string, std::string> result;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
            size_t equalPos = arg.find("=");
            std::string key = arg.substr(2, equalPos - 2);
            std::string value = arg.substr(equalPos + 1);
            result[key] = value;
        }
        // Handle --key value format
        else if (arg.find("--") == 0 && i + 1 < argc) {
            std::string key = arg.substr(2);
            std::string value = argv[i + 1];
            result[key] = value;
            i++; // Skip next argument
        }
        // Handle short options -k value format
        else if (arg.find("-") == 0 && arg.size() == 2 && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[i + 1];
            result[key] = value;
            i++; // Skip next argument
        }
    }
    
    return result;
}

std::map<std::string, std::string> CLIConfigurator::parseFileConfig(const std::string& filename) {
    std::map<std::string, std::string> result;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filename);
        return result;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find("#") == 0) {
            continue;
        }
        
        // Parse key=value pairs
        size_t equalPos = line.find("=");
        if (equalPos != std::string::npos) {
            std::string key = line.substr(0, equalPos);
            std::string value = line.substr(equalPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            result[key] = value;
        }
    }
    
    file.close();
    return result;
}

bool CLIConfigurator::convertValue(const std::string& value, std::string& result) {
    result = value;
    return true;
}

bool CLIConfigurator::convertValue(const std::string& value, int64_t& result) {
    try {
        result = std::stoll(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIConfigurator::convertValue(const std::string& value, float& result) {
    try {
        result = std::stof(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIConfigurator::convertValue(const std::string& value, double& result) {
    try {
        result = std::stod(value);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIConfigurator::convertValue(const std::string& value, bool& result) {
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    
    if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") {
        result = true;
        return true;
    } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") {
        result = false;
        return true;
    }
    
    return false;
}

std::string CLIConfigurator::getEnvVar(const std::string& name, const std::string& defaultValue) {
    const char* value = std::getenv(name.c_str());
    return value ? value : defaultValue;
}

} // namespace nlm
