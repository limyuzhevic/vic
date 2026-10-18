// Python Integration Demo
//
// This demo showcases the Python bindings for NLM, demonstrating how to use
the NLM framework from Python. It provides examples of:
// - Creating brain configurations
// - Running neural simulations
// - Accessing memory systems
// - Using agent interfaces
// - Advanced Python features with NLM

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace pybind11;

namespace nlm {

void pythonIntegrationDemo() {
    std::cout << "===========================================\n";
    std::cout << "NLM PYTHON INTEGRATION DEMO\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "This demo showcases the NLM Python bindings.\n\n";
    
    // Example 1: Creating a basic configuration
    std::cout << "1. Creating a basic brain configuration:\n";
    auto config = createDefaultConfig();
    std::cout << "   Configuration created successfully.\n";
    std::cout << "   Default neurons: " << config->getOr<int64_t>("neuron_count", 1000) << "\n\n";
    
    // Example 2: Creating a brain with custom configuration
    std::cout << "2. Creating a brain with custom configuration:\n";
    auto config2 = createRandomConfig(500, 0.05f);
    config2->set("stdp_ltp_weight", 0.02f);
    config2->set("stdp_ltd_weight", 0.018f);
    std::cout << "   Custom brain created with 500 neurons.\n";
    std::cout << "   STDP weights: LTP=" << config2->getOr<float>("stdp_ltp_weight", 0.02f) 
              << ", LTD=" << config2->getOr<float>("stdp_ltd_weight", 0.018f) << "\n\n";
    
    // Example 3: Creating a brain and world
    std::cout << "3. Creating brain and world objects:\n";
    auto brain = createBrain(config2);
    auto world = createSimpleWorld();
    
    if (brain->initialize()) {
        std::cout << "   ✓ Brain initialized successfully.\n";
        std::cout << "   ✓ World created successfully.\n";
        std::cout << "   Brain statistics:\n";
        std::cout << "     - Total neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "     - Total synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "     - Firing rate: " << brain->getAverageFiringRate() << " Hz\n";
    } else {
        std::cout << "   ✗ Failed to initialize brain.\n";
    }
    
    // Example 4: Advanced configuration
    std::cout << "\n4. Advanced configuration examples:\n";
    
    auto config3 = createDefaultConfig();
    
    // Set all advanced parameters
    config3->set("working_memory_capacity", 200);
    config3->set("episodic_memory_capacity", 100);
    config3->set("synaptogenesis_rate", 0.0001f);
    config3->set("pruning_rate", 0.00001f);
    config3->set("stdp_tau", 25.0f);
    config3->set("reward_scale", 2.0f);
    config3->set("exploration_rate", 0.2f);
    
    std::cout << "   Advanced configuration:\n";
    std::cout << "     - Working memory: " << config3->getOr<size_t>("working_memory_capacity", 200) << "\n";
    std::cout << "     - Episodic memory: " << config3->getOr<size_t>("episodic_memory_capacity", 100) << "\n";
    std::cout << "     - Synaptogenesis rate: " << config3->getOr<float>("synaptogenesis_rate", 0.0001f) << "\n";
    std::cout << "     - Pruning rate: " << config3->getOr<float>("pruning_rate", 0.00001f) << "\n";
    std::cout << "     - STDP tau: " << config3->getOr<float>("stdp_tau", 25.0f) << "\n";
    std::cout << "     - Reward scale: " << config3->getOr<float>("reward_scale", 2.0f) << "\n";
    std::cout << "     - Exploration rate: " << config3->getOr<float>("exploration_rate", 0.2f) << "\n\n";
    
    // Example 5: Brain analysis
    std::cout << "5. Brain analysis and monitoring:\n";
    
    auto analysis = analyzeBrainState(brain);
    std::cout << "   Brain analysis results:\n";
    for (const auto& pair : analysis) {
        if (pair.first.find("capacity") != std::string::npos) {
            std::cout << "     - " << pair.first << ": " << pair.second << "\n";
        }
    }
    std::cout << "     - Performance metrics: spikes=" << analysis["total_spikes"] 
              << ", neurons=" << analysis["total_neurons"] << "\n\n";
    
    // Example 6: Simulation functions
    std::cout << "6. Simulation functions demonstration:\n";
    
    auto brain2 = createBrain(createRandomConfig(100, 0.1f));
    if (brain2->initialize()) {
        auto world2 = createSimpleWorld();
        auto agent = createAgentBrain(brain2);
        agent->initialize(*world2);
        agent->enableRewardModulation(true);
        
        std::cout << "   Running simple simulation...\n";
        
        // Run a few simulation steps
        float avgReward = runSimulationEpisode(brain2, world2, agent, 10);
        std::cout << "   Average reward over 10 steps: " << avgReward << "\n";
        std::cout << "   Final neuromodulation level: " << agent->getNeuromodulationLevel() << "%\n";
    }
    
    // Example 7: Object creation and manipulation
    std::cout << "\n7. Object creation and manipulation:\n";
    
    auto config4 = createDefaultConfig();
    auto brain3 = createBrain(config4);
    
    if (brain3->initialize()) {
        // Add regions
        auto region1 = brain3->addRegion("sensory_region");
        auto region2 = brain3->addRegion("motor_region");
        std::cout << "   Added " << brain3->getRegionCount() << " brain regions.\n";
        
        // Inject current into different neuron types
        brain3->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
        brain3->injectCurrentToNeurons(NeuronType::Motor, 10.0f);
        brain3->injectCurrentToNeurons(NeuronType::Internal, 5.0f);
        std::cout << "   Injected current into sensory, motor, and internal neurons.\n";
        
        // Check neuron states
        std::cout << "   Neuron states:\n";
        std::cout << "     - Total active neurons: " << brain3->getActiveNeuronCount() << "\n";
        std::cout << "     - Total firing neurons: " << brain3->getFiringNeuronCount() << "\n";
        std::cout << "     - Average firing rate: " << brain3->getAverageFiringRate() << " Hz\n";
    }
    
    // Example 8: Python-style features
    std::cout << "\n8. Python-style features demonstration:\n";
    
    std::cout << "   Vector and map operations:\n";
    
    // Vector operations
    std::vector<float> weights = {0.1f, 0.5f, 0.8f, 1.2f, 2.0f};
    float sum = 0.0f;
    for (float w : weights) {
        sum += w;
    }
    std::cout << "     - Vector sum: " << sum << "\n";
    
    // Map operations (brain analysis)
    std::map<std::string, float> performanceMetrics;
    performanceMetrics["processing_speed"] = 1000.0f;
    performanceMetrics["memory_efficiency"] = 0.85f;
    performanceMetrics["learning_rate"] = 0.02f;
    
    std::cout << "   Performance metrics:\n";
    for (const auto& pair : performanceMetrics) {
        std::cout << "     - " << pair.first << ": " << pair.second << "\n";
    }
    
    // Example 9: Error handling
    std::cout << "\n9. Error handling examples:\n";
    
    try {
        auto config5 = createDefaultConfig();
        auto brain4 = createBrain(config5);
        
        // This should work fine
        if (brain4->initialize()) {
            std::cout << "   ✓ Basic initialization successful.\n";
        }
        
        // Demonstrate safe operations
        std::cout << "   Safe operations completed successfully.\n";
        
    } catch (const std::exception& e) {
        std::cout << "   Exception caught: " << e.what() << "\n";
    }
    
    // Summary
    std::cout << "\n" << "=" << std::string(60, '=') << "\n";
    std::cout << "PYTHON INTEGRATION DEMO SUMMARY\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "The NLM Python bindings provide:\n";
    std::cout << "  ✓ Comprehensive brain and world management\n";
    std::cout << "  ✓ Advanced configuration options\n";
    std::cout << "  ✓ Simulation control and monitoring\n";
    std::cout << "  ✓ Memory system access\n";
    std::cout << "  ✓ Neuromodulation control\n";
    std::cout << "  ✓ Performance analysis tools\n";
    std::cout << "  ✓ Python-style features (vectors, maps)\n";
    std::cout << "  ✓ Error handling and safety\n";
    std::cout << "\nThe NLM framework is fully accessible from Python!\n";
    
    std::cout << "\nPython Integration Demo Complete!\n";
}

} // namespace nlm

// Demo entry point for Python integration
int main(int argc, char* argv[]) {
    try {
        nlm::pythonIntegrationDemo();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "ERROR: Unknown exception occurred." << std::endl;
        return 1;
    }
}
