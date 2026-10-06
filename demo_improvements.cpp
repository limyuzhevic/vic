#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "sensory/Vision.hpp"
#include "motor/Action.hpp"

void demonstrateEnhancedConfiguration() {
    std::cout << "\n=== Enhanced Configuration Demo ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    
    // Demonstrate type-safe configuration
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(2));
    config->set("connection_probability", 0.15f);
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    
    // Get values with type safety
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    int64_t neurons = config->getOr<int64_t>("neuron_count", 100);
    float connProb = config->getOr<float>("connection_probability", 0.1f);
    
    std::cout << "Configuration loaded successfully:" << std::endl;
    std::cout << "  Simulation timestep: " << timestep << "s" << std::endl;
    std::cout << "  Neuron count: " << neurons << std::endl;
    std::cout << "  Connection probability: " << connProb << std::endl;
}

void demonstrateBrainConvenience() {
    std::cout << "\n=== Brain Convenience Methods Demo ===" << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.2f);
    
    nlm::Brain brain(config);
    if (!brain.initialize()) {
        std::cout << "Failed to initialize brain" << std::endl;
        return;
    }
    
    std::cout << "Brain initialized successfully" << std::endl;
    
    // Demonstrate getNeuronStats
    auto stats = brain.getNeuronStats();
    std::cout << "\nBrain Statistics:" << std::endl;
    std::cout << "  Total neurons: " << stats["total_neurons"] << std::endl;
    std::cout << "  Total synapses: " << stats["total_synapses"] << std::endl;
    std::cout << "  Active neurons: " << stats["active_neurons"] << std::endl;
    std::cout << "  Firing neurons: " << stats["firing_neurons"] << std::endl;
    std::cout << "  Firing rate: " << stats["firing_rate"] << std::endl;
    std::cout << "  Total spikes: " << stats["total_spikes"] << std::endl;
    
    // Demonstrate runExperiment
    std::cout << "\nRunning experiment..." << std::endl;
    brain.runExperiment(20, 0.001);
    
    // Demonstrate getSystemInfo
    auto systemInfo = brain.getSystemInfo();
    std::cout << "\nSystem Information:" << std::endl;
    std::cout << "  Version: " << systemInfo.at("version") << std::endl;
    std::cout << "  Neuron model: " << systemInfo.at("neuron_model") << std::endl;
    std::cout << "  Architecture: " << systemInfo.at("architecture") << std::endl;
    std::cout << "  Plasticity rules: " << systemInfo.at("plasticity_rules") << std::endl;
}

void demonstratePythonBindings() {
    std::cout << "\n=== Python Bindings Demo (Conceptual) ===" << std::endl;
    
    std::cout << "The enhanced Python bindings provide a user-friendly interface:" << std::endl;
    std::cout << """
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
brain = pynlm.createBrain(config)
brain.initialize()

# Run experiment with progress tracking
brain.runExperiment(50, 0.001)

# Get comprehensive statistics
stats = brain.getNeuronStats()
print(f"Total neurons: {stats['total_neurons']}")
print(f"Active neurons: {stats['active_neurons']}")
print(f"Firing rate: {stats['firing_rate']:.2f}")

# Get system information
system_info = brain.getSystemInfo()
print(f"Neuron model: {system_info['neuron_model']}")
print(f"Architecture: {system_info['architecture']}")

# Create agent for world interaction
world = pynlm.createSimpleWorld(10, 10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run training episode
agent.trainEpisode(world, initialPercept)
    """ << std::endl;
}

void demonstrateAdvancedFeatures() {
    std::cout << "\n=== Advanced Features Demo ===" << std::endl;
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 7, 7);
    world->setAgentStart(10, 10);
    
    // Setup experiment world
    pynlm::setupExperimentWorld(world); // Note: This is a pybind11 concept
    
    std::cout << "World configured and populated with resources and hazards" << std::endl;
    
    // Create agent
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(*world);
    
    std::cout << "Agent created and initialized with world" << std::endl;
    
    // Enable learning systems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Learning systems enabled:" << std::endl;
    std::cout << "  Reward modulation: " << (agent->isRewardModulationEnabled() ? "ON" : "OFF") << std::endl;
    std::cout << "  Curiosity: " << (agent->isCuriosityEnabled() ? "ON" : "OFF") << std::endl;
    std::cout << "  Development: " << (agent->isDevelopmentEnabled() ? "ON" : "OFF") << std::endl;
    
    // Get agent performance metrics
    auto metrics = agent->getPerformanceMetrics();
    std::cout << "\nPerformance Metrics:" << std::endl;
    for (const auto& pair : metrics) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

int main() {
    std::cout << "=== NLM (熙然) - Neural Learning Machine - Improvements Demonstration ===" << std::endl;
    std::cout << "Demonstrating the enhanced features and improvements made to the NLM project" << std::endl;
    
    // Run all demonstrations
    demonstrateEnhancedConfiguration();
    demonstrateBrainConvenience();
    demonstratePythonBindings();
    demonstrateAdvancedFeatures();
    
    std::cout << "\n=== Demonstration Complete ===" << std::endl;
    std::cout << "\nThe NLM project has been significantly enhanced with:" << std::endl;
    std::cout << "✓ Enhanced configuration system with type-safe methods" << std::endl;
    std::cout << "✓ Advanced brain class convenience methods" << std::endl;
    std::cout << "✓ Comprehensive Python bindings with user-friendly API" << std::endl;
    std::cout << "✓ Advanced agent brain interfaces and training utilities" << std::endl;
    std::cout << "✓ Improved error handling and user feedback" << std::endl;
    std::cout << "✓ Better documentation and examples" << std::endl;
    
    return 0;
}
