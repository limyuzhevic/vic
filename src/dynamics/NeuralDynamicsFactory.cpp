// NeuralDynamicsFactory implementation
#include "NeuralDynamics.hpp"
#include "IntegrateAndFireDynamics.hpp"
#include "HodgkinHuxleyDynamics.hpp"
#include "FitzHughNagumoDynamics.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

std::unique_ptr<NeuralDynamics> NeuralDynamicsFactory::createDynamics(
    const std::string& dynamicsType,
    const std::unordered_map<std::string, std::string>& parameters) {
    
    if (dynamicsType == "integrate_and_fire" || dynamicsType == "iaf") {
        auto dynamics = std::make_unique<IntegrateAndFireDynamics>();
        
        // Apply parameters
        for (const auto& param : parameters) {
            if (param.first == "membrane_time_constant") {
                float value = std::stof(param.second);
                dynamics->setMembraneTimeConstant(value);
            } else if (param.first == "membrane_resistance") {
                // Note: Membrane resistance would need to be added to IntegrateAndFireDynamics
                // For now, we'll ignore this parameter
            }
        }
        
        return dynamics;
    }
    
    else if (dynamicsType == "hodgkin_huxley" || dynamicsType == "hh") {
        return std::make_unique<HodgkinHuxleyDynamics>();
    }
    
    else if (dynamicsType == "fitzhugh_nagumo" || dynamicsType == "fhn") {
        return std::make_unique<FitzHughNagumoDynamics>();
    }
    
    else {
        throw std::runtime_error("Unknown dynamics type: " + dynamicsType);
    }
}

std::vector<std::string> NeuralDynamicsFactory::getAvailableDynamicsTypes() {
    return {
        "integrate_and_fire", "iaf",
        "hodgkin_huxley", "hh",
        "fitzhugh_nagumo", "fhn"
    };
}

std::unique_ptr<NeuralDynamics> NeuralDynamicsFactory::createFromConfig(
    const std::unordered_map<std::string, std::string>& config) {
    
    // Extract dynamics type from config
    std::string dynamicsType = "integrate_and_fire";  // Default
    auto it = config.find("neural_dynamics_type");
    if (it != config.end()) {
        dynamicsType = it->second;
    }
    
    // Extract parameters
    std::unordered_map<std::string, std::string> parameters;
    
    // Copy relevant parameters
    for (const auto& configItem : config) {
        if (configItem.first == "membrane_time_constant" ||
            configItem.first == "membrane_resistance") {
            parameters[configItem.first] = configItem.second;
        }
    }
    
    return createDynamics(dynamicsType, parameters);
}

} // namespace nlm
