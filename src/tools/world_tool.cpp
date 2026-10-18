// World tool implementation
//
// This file provides the implementation of world CLI tools

#include "world_tool.hpp"
#include <algorithm>

namespace nlm {

// World tool implementations
bool WorldTool::saveWorldState(const std::string& outputFile, const Config& config) const {
    return saveWorldStateImpl(outputFile, config);
}

bool WorldTool::loadWorldState(const std::string& inputFile, Config& config) const {
    return loadWorldStateImpl(inputFile, config);
}

bool WorldTool::testWorldConfiguration(const std::string& configFile) const {
    return testWorldConfigurationImpl(configFile);
}

bool WorldTool::exploreWorldState(const std::string& worldFile) const {
    return exploreWorldStateImpl(worldFile);
}

// World-specific implementations
bool WorldTool::saveWorldStateImpl(const std::string& outputFile, const Config& config) const {
    printInfo("Saving world state to: " + outputFile);
    
    nlohmann::json worldState;
    worldState["timestamp"] = getCurrentTimestamp();
    worldState["simulation_steps"] = config.getOr<int64_t>("max_simulation_steps", 10000);
    worldState["timestep"] = config.getOr<double>("simulation_timestep", 0.001);
    worldState["environment_type"] = "simple_world";
    
    return writeJsonToFile(worldState, outputFile);
}

bool WorldTool::loadWorldStateImpl(const std::string& inputFile, Config& config) const {
    printInfo("Loading world state from: " + inputFile);
    
    if (!fileExists(inputFile)) {
        printError("World state file does not exist: " + inputFile);
        return false;
    }
    
    try {
        nlohmann::json worldState = parseJsonFromFile(inputFile);
        printInfo("Loaded world state with " + std::to_string(worldState["simulation_steps"].get<int64_t>()) + " steps");
    } catch (const std::exception& e) {
        printError("Failed to load world state: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

bool WorldTool::testWorldConfigurationImpl(const std::string& configFile) const {
    printInfo("Testing world configuration from: " + configFile);
    
    if (!fileExists(configFile)) {
        printError("Configuration file does not exist: " + configFile);
        return false;
    }
    
    try {
        nlohmann::json config = parseJsonFromFile(configFile);
        
        // Validate configuration
        bool valid = true;
        
        if (config.contains("neuron_count") && config["neuron_count"] < 0) {
            printError("Invalid neuron_count: " + config["neuron_count"].dump());
            valid = false;
        }
        
        if (config.contains("simulation_timestep") && config["simulation_timestep"] <= 0) {
            printError("Invalid simulation_timestep: " + config["simulation_timestep"].dump());
            valid = false;
        }
        
        if (valid) {
            printSuccess("World configuration is valid");
        } else {
            printError("World configuration has validation errors");
        }
        
    } catch (const std::exception& e) {
        printError("Failed to test world configuration: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

bool WorldTool::exploreWorldStateImpl(const std::string& worldFile) const {
    printInfo("Exploring world state: " + worldFile);
    
    if (!fileExists(worldFile)) {
        printError("World state file does not exist: " + worldFile);
        return false;
    }
    
    try {
        nlohmann::json worldState = parseJsonFromFile(worldFile);
        
        printInfo("World State Overview:");
        printInfo("  Environment Type: " + worldState["environment_type"].get<std::string>());
        printInfo("  Simulation Steps: " + std::to_string(worldState["simulation_steps"].get<int64_t>()));
        printInfo("  Time Step: " + std::to_string(worldState["timestep"].get<double>()));
        printInfo("  Loaded At: " + worldState["timestamp"].get<std::string>());
        
    } catch (const std::exception& e) {
        printError("Failed to explore world state: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

} // namespace nlm