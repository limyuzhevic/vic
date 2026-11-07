#include "demo_utils.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <algorithm>

namespace nlm {
namespace demo_utils {

bool CLIOptions::parse(int argc, char* argv[]) {
    // Reset to defaults
    numEpisodes = 5;
    stepsPerEpisode = 200;
    numTrials = 50;
    checkpointSteps = 100;
    verbose = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (key == "episodes") {
                    numEpisodes = std::atoi(value.c_str());
                } else if (key == "steps") {
                    stepsPerEpisode = std::atoi(value.c_str());
                } else if (key == "trials") {
                    numTrials = static_cast<size_t>(std::atoi(value.c_str()));
                } else if (key == "checkpoint") {
                    checkpointSteps = static_cast<size_t>(std::atoi(value.c_str()));
                } else if (key == "verbose" || key == "v") {
                    verbose = (value == "true" || value == "1" || value == "yes");
                } else if (key == "help" || key == "h") {
                    printUsage();
                    return false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key == "e") {
                numEpisodes = std::atoi(value.c_str());
            } else if (key == "s") {
                stepsPerEpisode = std::atoi(value.c_str());
            } else if (key == "t") {
                numTrials = static_cast<size_t>(std::atoi(value.c_str()));
            } else if (key == "c") {
                checkpointSteps = static_cast<size_t>(std::atoi(value.c_str()));
            } else if (key == "v") {
                verbose = (value == "true" || value == "1" || value == "yes");
            }
        } else if (arg == "--help" || arg == "-h") {
            printUsage();
            return false;
        }
    }
    
    return true;
}

void CLIOptions::printUsage() const {
    std::cout << "NLM Demo Usage:\n";
    std::cout << "  --episodes N    Number of episodes (default: 5)\n";
    std::cout << "  --steps N       Steps per episode (default: 200)\n";
    std::cout << "  --trials N      Number of trials (default: 50)\n";
    std::cout << "  --checkpoint N  Checkpoint interval (default: 100)\n";
    std::cout << "  --verbose/-v    Enable verbose output (default: true)\n";
    std::cout << "  --help/-h       Show this help message\n";
    std::cout << "\nExample:\n";
    std::cout << "  ./nlm_phase4_demo --episodes 10 --steps 500 --verbose false\n";
}

const char* CLIOptions::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

const char* CLIOptions::developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

std::shared_ptr<Config> CLIOptions::makeDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic simulation parameters
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    
    // Plasticity parameters (Phase 2)
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    config->set("stdp_tau", 20.0f);
    
    // Hebbian plasticity
    config->set("hebbian_learning_rate", 0.01f);
    config->set("hebbian_max_weight", 1.0f);
    config->set("hebbian_min_weight", 0.0f);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Neuromodulation
    config->set("dopamine_baseline", 0.1f);
    config->set("dopamine_decay_rate", 0.95f);
    
    // Reward
    config->set("reward_discount_factor", 0.99f);
    
    return config;
}

SensoryPercept CLIOptions::makeStandardPercept(const std::vector<float>& vision,
                                              const std::vector<float>& touch,
                                              const std::string& text) {
    SensoryPercept p;
    p.setVision(vision);
    p.setTouch(touch);
    p.setInternal({1.0f, 1.0f, 0.0f, 0.0f});
    p.setProprioception({0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f});
    p.setAudio({});
    p.setText(text);
    return p;
}

void CLIOptions::printHeader(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void CLIOptions::printResult(const std::string& name, float value, float max) {
    std::cout << "  " << std::left << std::setw(30) << name << ": ";
    std::cout << std::fixed << std::setprecision(3) << value;
    if (max > 0) {
        std::cout << " (" << std::setprecision(1) << (value/max*100) << "%)";
    }
    std::cout << "\n";
}

void CLIOptions::printSection(const std::string& title) {
    std::cout << "\n--- " << title << " ---\n";
}

bool CLIOptions::validateConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    
    // Check for required parameters
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "simulation_timestep",
        "connection_probability"
    };
    
    for (const auto& key : requiredKeys) {
        if (!config->has(key)) {
            std::cerr << "Error: Missing required configuration key: " << key << std::endl;
            return false;
        }
    }
    
    // Validate parameter ranges
    auto validateRange = [config](const std::string& key, double min, double max) {
        auto val = config->get<double>(key);
        if (!val.has_value()) {
            std::cerr << "Error: Cannot validate parameter " << key << ": not found" << std::endl;
            return false;
        }
        if (val.value() < min || val.value() > max) {
            std::cerr << "Error: Parameter " << key << " has invalid value: " 
                      << val.value() << ". Must be between " << min << " and " << max << std::endl;
            return false;
        }
        return true;
    };
    
    // Validate ranges for important parameters
    if (!validateRange("neuron_count", 1.0, 10000.0)) return false;
    if (!validateRange("region_count", 1.0, 100.0)) return false;
    if (!validateRange("simulation_timestep", 0.0001, 1.0)) return false;
    if (!validateRange("connection_probability", 0.0, 1.0)) return false;
    
    return true;
}

std::string CLIOptions::getConfigParameterInfo(const std::string& key) {
    // Parameter documentation with descriptions and units
    static const std::unordered_map<std::string, std::string> parameterInfo = {
        {"neuron_count", "Number of neurons in the brain (int)"},
        {"region_count", "Number of brain regions (int)"},
        {"simulation_timestep", "Time step size in seconds (double, seconds)"},
        {"connection_probability", "Probability of forming random connections (float, 0-1)"},
        {"random_seed", "Random seed for reproducible experiments (int64)"},
        {"stdp_ltp_weight", "STDP long-term potentiation weight (float)"},
        {"stdp_ltd_weight", "STDP long-term depression weight (float)"},
        {"stdp_tau", "STDP time constant (float, ms)"},
        {"hebbian_learning_rate", "Hebbian plasticity learning rate (float)"},
        {"hebbian_max_weight", "Maximum synaptic weight for Hebbian (float)"},
        {"hebbian_min_weight", "Minimum synaptic weight for Hebbian (float)"},
        {"synaptogenesis_rate", "Structural plasticity synaptogenesis rate (float)"},
        {"pruning_rate", "Structural plasticity pruning rate (float)"},
        {"dopamine_baseline", "Neuromodulation dopamine baseline level (float)"},
        {"dopamine_decay_rate", "Neuromodulation dopamine decay rate (float, 0-1)"},
        {"reward_discount_factor", "Reward discount factor for future rewards (float, 0-1)"}
    };
    
    auto it = parameterInfo.find(key);
    if (it != parameterInfo.end()) {
        return it->second;
    }
    return "Unknown parameter";
}

} // namespace demo_utils
} // namespace nlm
