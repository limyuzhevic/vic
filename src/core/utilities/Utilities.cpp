#include "core/utilities/Utilities.hpp"
#include "core/Config/Config.hpp"
#include "agent/AgentBody.hpp"
#include "brain/NeuralRegion.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace nlm {

const char* motorCommandToString(MotorCommand cmd) {
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

const char* developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

std::shared_ptr<Config> createDefaultConfig(const std::string& experimentType) {
    auto config = std::make_shared<Config>();
    
    // Set phase 2/3/4/6 appropriate defaults
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Phase-specific settings
    if (experimentType == "phase3") {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    } else if (experimentType == "phase6") {
        config->set("environment_width", 16);
        config->set("environment_height", 16);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
        config->set("max_steps", 2000);
    }
    
    return config;
}

bool safeFileOperation(const std::string& path, std::function<void()> operation, const std::string& errorMsg) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open file " << path << ". " << errorMsg << std::endl;
        return false;
    }
    
    try {
        operation();
        file.close();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "ERROR during file operation " << path << ": " << e.what() << std::endl;
        file.close();
        return false;
    }
}

template<typename T>
bool safeInitialize(T& object, bool (T::*init)(), const std::string& errorMsg) {
    try {
        bool result = (object.*init)();
        if (!result) {
            std::cerr << "ERROR: " << errorMsg << std::endl;
        }
        return result;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << errorMsg << ": " << e.what() << std::endl;
        return false;
    }
}

bool validateConfig(const std::shared_ptr<Config>& config) {
    if (!config) {
        std::cerr << "ERROR: Config is null" << std::endl;
        return false;
    }
    
    // Validate essential parameters
    bool neuron_count_ok = config->getOr<int64_t>("neuron_count", 0) > 0;
    bool timestep_ok = config->getOr<double>("simulation_timestep", 0.0) > 0.0;
    bool seed_ok = config->getOr<int64_t>("random_seed", 0) != 0;
    
    if (!neuron_count_ok) {
        std::cerr << "ERROR: Invalid neuron_count" << std::endl;
    }
    if (!timestep_ok) {
        std::cerr << "ERROR: Invalid simulation_timestep" << std::endl;
    }
    if (!seed_ok) {
        std::cerr << "WARNING: Using default random_seed" << std::endl;
    }
    
    return neuron_count_ok && timestep_ok;
}

// Global debug flag
static bool debugOutputEnabled = false;
void enableDebugOutput(bool enabled) {
    debugOutputEnabled = enabled;
}

bool isDebugOutputEnabled() {
    return debugOutputEnabled;
}

} // namespace nlm
