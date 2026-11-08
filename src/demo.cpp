#include "DemoFramework.hpp"
#include "DependencyInjection.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "NLM Demo Framework" << std::endl;
    std::cout << "=====================" << std::endl << std::endl;
    
    // Initialize dependency injection
    nlm::DependencyContainer& container = nlm::DependencyContainer::getInstance();
    nlm::DI::initializeGlobalContainer();
    
    // Check for phase argument
    if (argc > 1) {
        int phase = std::stoi(argv[1]);
        
        if (phase == 999) {
            // Benchmark mode
            nlm::DemoRunner::runBenchmark(1000, 500);
            nlm::DI::shutdownGlobalContainer();
            return 0;
        }
        
        if (phase >= 3 && phase <= 6) {
            nlm::DemoRunner::runDemo(phase, "configs/default.cfg");
            nlm::DI::shutdownGlobalContainer();
            return 0;
        }
    }
    
    // Show help if no arguments or invalid phase
    nlm::DemoRunner::printAvailablePhases();
    std::cout << "\nUsage: nlm [phase] [num_episodes] [steps_per_episode]" << std::endl;
    std::cout << "      nlm 3 5 200 - runs Phase 3 demo with 5 episodes, 200 steps each" << std::endl;
    std::cout << "      nlm 999       - runs performance benchmark" << std::endl;
    
    nlm::DI::shutdownGlobalContainer();
    return 0;
}
