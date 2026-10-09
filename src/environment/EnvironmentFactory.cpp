#include "EnvironmentFactory.hpp"
#include <stdexcept>
#include <iostream>

namespace nlm {

std::unordered_map<std::string, EnvironmentFactory::EnvironmentCreator>& EnvironmentFactory::getRegistry() {
    static std::unordered_map<std::string, EnvironmentCreator> registry;
    return registry;
}

std::unique_ptr<Environment> EnvironmentFactory::createEnvironment(const std::string& name, 
                                                                   const std::unordered_map<std::string, std::string>& params) {
    auto& registry = getRegistry();
    
    auto it = registry.find(name);
    if (it == registry.end()) {
        std::string error = "Environment type not found: " + name + ". Available: ";
        for (const auto& pair : registry) {
            error += pair.first + " ";
        }
        throw std::runtime_error(error);
    }
    
    return it->second(params);
}

bool EnvironmentFactory::registerEnvironment(const std::string& name, 
                                             EnvironmentCreator creator) {
    getRegistry()[name] = creator;
    return true;
}

std::vector<std::string> EnvironmentFactory::getAvailableEnvironments() {
    std::vector<std::string> names;
    for (const auto& pair : getRegistry()) {
        names.push_back(pair.first);
    }
    return names;
}

// GridWorldEnvironment factory registration
namespace {
    bool gridWorldRegistered = []() {
        EnvironmentFactory::registerEnvironment("GridWorld", 
            [](const std::unordered_map<std::string, std::string>& params) {
                int width = 10;
                int height = 10;
                
                if (params.find("width") != params.end()) {
                    width = std::stoi(params.at("width"));
                }
                if (params.find("height") != params.end()) {
                    height = std::stoi(params.at("height"));
                }
                
                return std::make_unique<GridWorldEnvironment>(width, height);
            });
        return true;
    }();
}

// MazeEnvironment factory registration
namespace {
    bool mazeRegistered = []() {
        EnvironmentFactory::registerEnvironment("Maze", 
            [](const std::unordered_map<std::string, std::string>& params) {
                int width = 20;
                int height = 20;
                float obstacleDensity = 0.2f;
                
                if (params.find("width") != params.end()) {
                    width = std::stoi(params.at("width"));
                }
                if (params.find("height") != params.end()) {
                    height = std::stoi(params.at("height"));
                }
                if (params.find("obstacle_density") != params.end()) {
                    obstacleDensity = std::stof(params.at("obstacle_density"));
                }
                
                return std::make_unique<MazeEnvironment>(width, height, obstacleDensity);
            });
        return true;
    }();
}

// GoalMazeEnvironment factory registration
namespace {
    bool goalMazeRegistered = []() {
        EnvironmentFactory::registerEnvironment("GoalMaze", 
            [](const std::unordered_map<std::string, std::string>& params) {
                int width = 15;
                int height = 15;
                int numGoals = 5;
                
                if (params.find("width") != params.end()) {
                    width = std::stoi(params.at("width"));
                }
                if (params.find("height") != params.end()) {
                    height = std::stoi(params.at("height"));
                }
                if (params.find("num_goals") != params.end()) {
                    numGoals = std::stoi(params.at("num_goals"));
                }
                
                return std::make_unique<GoalMazeEnvironment>(width, height, numGoals);
            });
        return true;
    }();
}

void registerGridWorldEnvironment() {
    // Already registered via lambda
}

void registerMazeEnvironment() {
    // Already registered via lambda
}

void registerGoalMazeEnvironment() {
    // Already registered via lambda
}

void registerAllEnvironments() {
    registerGridWorldEnvironment();
    registerMazeEnvironment();
    registerGoalMazeEnvironment();
    std::cout << "Registered " << EnvironmentFactory::getAvailableEnvironments().size() 
              << " environment types." << std::endl;
}

} // namespace nlm
