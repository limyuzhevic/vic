// Example: Advanced NLM Features Demonstration
// This example demonstrates the new advanced features added to NLM
// Shows how to use AdvancedBrain, PerformanceProfiler, and custom features

#include <iostream>
#include <vector>
#include <memory>
#include "agent/AdvancedBrain.hpp"
#include "agent/PerformanceProfiler.hpp"
#include "plasticity/CustomPlasticityRule.hpp"

void demonstrateAdvancedBrain() {
    std::cout << "=== AdvancedBrain Demonstration ===" << std::endl;
    
    // Create advanced configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.005f);
    config->set("stdp_ltd_weight", 0.012f);
    
    // Create advanced brain
    nlm::AdvancedBrain advancedBrain(config);
    advancedBrain.initialize();
    
    // Configure advanced features
    advancedBrain.enableMonitoring(100);
    advancedBrain.enableOptimization(true);
    
    std::cout << "AdvancedBrain created and initialized." << std::endl;
    std::cout << "Neuron count: " << advancedBrain.getAdvancedStats()["neuron_count"] << std::endl;
    std::cout << "Firing rate: " << advancedBrain.getAdvancedStats()["firing_rate"] << " Hz" << std::endl;
    
    // Run simulation with monitoring
    std::cout << "Running simulation with monitoring..." << std::endl;
    for (int step = 0; step < 500; ++step) {
        advancedBrain.step(step);
        
        if (step % 100 == 0) {
            auto stats = advancedBrain.getAdvancedStats();
            std::cout << "Step " << step << ": " << stats["firing_rate"] << " Hz, "
                      << stats["total_spikes"] << " total spikes" << std::endl;
        }
    }
    
    // Analyze performance
    auto finalStats = advancedBrain.getAdvancedStats();
    std::cout << std::endl;
    std::cout << "Final statistics:" << std::endl;
    for (const auto& entry : finalStats) {
        std::cout << "  " << entry.first << ": " << entry.second << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstratePerformanceProfiler() {
    std::cout << "=== PerformanceProfiler Demonstration ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20.0f, 20.0f, 8, 8);
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(world);
    
    std::cout << "Running performance profile simulation..." << std::endl;
    
    // Profile the simulation
    auto profileResults = nlm::PerformanceProfiler::profileSimulation(brain, world, agent, 200);
    
    std::cout << "Performance profile results:" << std::endl;
    for (const auto& entry : profileResults) {
        std::cout << "  " << entry.first << ": " << entry.second << std::endl;
    }
    
    // Analyze bottlenecks
    auto bottlenecks = nlm::PerformanceProfiler::analyzePerformanceBottlenecks(brain, world, agent);
    std::cout << std::endl;
    std::cout << "Performance bottlenecks analysis:" << std::endl;
    for (const auto& entry : bottlenecks) {
        std::cout << "  " << entry.first << ": " << entry.second << std::endl;
    }
    
    std::cout << std::endl;
}

void demonstrateCustomPlasticity() {
    std::cout << "=== CustomPlasticityRule Demonstration ===" << std::endl;
    
    // Access the plasticity rule registry
    auto& registry = nlm::CustomPlasticityRuleRegistry::getInstance();
    
    std::cout << "Available plasticity rules: " << std::endl;
    auto availableRules = registry.listAvailableRules();
    if (availableRules.empty()) {
        std::cout << "  (No rules registered - this is expected in demonstration)" << std::endl;
    } else {
        for (const auto& rule : availableRules) {
            std::cout << "  - " << rule << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Note: In a real application, you would create custom plasticity rules" << std::endl;
    std::cout << "and register them with the registry for use in simulation." << std::endl;
    
    std::cout << std::endl;
}

void demonstrateAdvancedConfiguration() {
    std::cout << "=== Advanced Configuration Demonstration ===" << std::endl;
    
    // Create advanced configuration
    auto config = nlm::createDefaultConfig();
    
    // Apply advanced settings
    config->set("brain.neuron_count", 5000);
    config->set("brain.synapse_density", 0.15f);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("memory.episodic.capacity", 5000);
    config->set("memory.working.capacity", 1000);
    
    // Get configuration summary
    std::cout << "Advanced configuration created:" << std::endl;
    std::cout << "  - Neurons: " << config->getOr<int64_t>("brain.neuron_count", 1000) << std::endl;
    std::cout << "  - Synapse density: " << config->getOr<float>("brain.synapse_density", 0.1f) << std::endl;
    std::cout << "  - STDP enabled: " << (config->getOr<bool>("plasticity.stdp.enable", false) ? "Yes" : "No") << std::endl;
    std::cout << "  - Hebbian enabled: " << (config->getOr<bool>("plasticity.hebbian.enable", false) ? "Yes" : "No") << std::endl;
    std::cout << "  - Structural plasticity enabled: " << (config->getOr<bool>("plasticity.structural.enable", false) ? "Yes" : "No") << std::endl;
    std::cout << "  - Curiosity enabled: " << (config->getOr<bool>("neuromod.curiosity.enable", false) ? "Yes" : "No") << std::endl;
    std::cout << "  - Novelty detection enabled: " << (config->getOr<bool>("neuromod.novelty.enable", false) ? "Yes" : "No") << std::endl;
    std::cout << "  - Working memory capacity: " << config->getOr<int64_t>("memory.working.capacity", 100) << std::endl;
    std::cout << "  - Episodic memory capacity: " << config->getOr<int64_t>("memory.episodic.capacity", 1000) << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "NLM Advanced Features Demonstration" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << std::endl;
    
    demonstrateAdvancedBrain();
    demonstratePerformanceProfiler();
    demonstrateCustomPlasticity();
    demonstrateAdvancedConfiguration();
    
    std::cout << "=== All Demonstrations Completed ===" << std::endl;
    std::cout << std::endl;
    std::cout << "This demonstration shows the new advanced features available in NLM:" << std::endl;
    std::cout << "1. AdvancedBrain - Enhanced monitoring, analytics, and experiment capabilities" << std::endl;
    std::cout << "2. PerformanceProfiler - Performance monitoring and optimization tools" << std::endl;
    std::cout << "3. CustomPlasticityRule - Framework for custom synaptic learning rules" << std::endl;
    std::cout << "4. Advanced configuration options for enhanced simulation control" << std::endl;
    std::cout << std::endl;
    std::cout << "These features enable advanced users to create more sophisticated neural simulations," << std::endl;
    std::cout << "analyze performance, customize learning rules, and optimize simulations for specific tasks." << std::endl;
    
    return 0;
}
