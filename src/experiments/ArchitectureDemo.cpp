//!/usr/bin/env // Simple demonstration of Phase 6 Integration Architecture Refactoring

#include "experiments/Phase6ConfigManager.hpp"
#include "experiments/ExperimentFactory.hpp"
#include "experiments/ExperimentRunnerImpl.hpp"
#include <iostream>

int main() {
    std::cout << "=== Phase 6 Integration Architecture Refactoring Demo ===" << std::endl;
    std::cout << std::endl;
    
    // 1. Configuration Management
    std::cout << "1. Configuration Management:" << std::endl;
    auto& configManager = nlm::Phase6ConfigManager::getInstance();
    std::cout << "   Max steps: " << configManager.getMaxSteps() << std::endl;
    std::cout << "   Neurons: " << configManager.getNeuronCount() << std::endl;
    std::cout << "   Valid: " << (configManager.isValid() ? "Yes" : "No") << std::endl;
    
    // 2. Factory Pattern
    std::cout << std::endl << "2. Factory Pattern:" << std::endl;
    auto& factoryRegistry = nlm::ExperimentFactoryRegistry::getInstance();
    auto factories = factoryRegistry.getRegisteredFactories();
    std::cout << "   Registered factories: ";
    for (size_t i = 0; i < factories.size(); ++i) {
        std::cout << factories[i];
        if (i < factories.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    // 3. System Creators
    std::cout << std::endl << "3. System Creators:" << std::endl;
    auto& creatorRegistry = nlm::SystemCreatorRegistry::getInstance();
    auto brainCreators = creatorRegistry.getBrainCreators();
    auto worldCreators = creatorRegistry.getWorldCreators();
    std::cout << "   Brain creators: " << brainCreators.size() << std::endl;
    std::cout << "   World creators: " << worldCreators.size() << std::endl;
    
    // 4. Experiment Runner
    std::cout << std::endl << "4. Experiment Runner:" << std::endl;
    auto& runnerRegistry = nlm::ExperimentRunnerRegistry::getInstance();
    auto runners = runnerRegistry.getRegisteredRunners();
    std::cout << "   Registered runners: ";
    for (size_t i = 0; i < runners.size(); ++i) {
        std::cout << runners[i];
        if (i < runners.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;
    
    std::cout << std::endl << "=== Architecture Refactoring Complete ===" << std::endl;
    return 0;
}
