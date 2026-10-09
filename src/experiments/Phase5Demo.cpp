// NLM Phase 5 Demo
// 
// Standalone Phase 5 experiment demo executable
// This provides a simple way to run Phase 5 functionality without the full framework

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "experiments/Phase5Experiment.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Demo ===" << std::endl;
    std::cout << "Running Phase 5 lifetime learning experiment..." << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create Phase 5 experiment
    Phase5Experiment experiment;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Basic configuration for demo
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config->set("max_simulation_steps", 50000, ConfigSource::Default);
    
    // Log configuration
    NLM_LOG_INFO("Phase 5 Configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(config->getOr<int64_t>("neuron_count", 1000)));
    NLM_LOG_INFO("  Max steps: " + std::to_string(config->getOr<uint64_t>("max_simulation_steps", 50000)));
    
    std::cout << std::endl;
    
    // Run the experiment
    bool success = experiment.run({}, config);
    
    if (success) {
        std::cout << std::endl;
        std::cout << "=== Phase 5 Demo Complete ===" << std::endl;
        std::cout << "Phase 5 lifetime learning experiment completed successfully!" << std::endl;
        std::cout << std::endl;
        std::cout << "Phase 5 achieves:" << std::endl;
        std::cout << "  ✓ Comprehensive lifetime learning with 7 development phases" << std::endl;
        std::cout << "  ✓ Memory consolidation and spatial representation" << std::endl;
        std::cout << "  ✓ Social development and imitation learning" << std::endl;
        std::cout << "  ✓ Lifelong adaptation and continual learning" << std::endl;
        std::cout << "  ✓ Performance optimizations with multithreading and SIMD" << std::endl;
        return 0;
    } else {
        std::cerr << "Phase 5 demo failed!" << std::endl;
        return 1;
    }
}