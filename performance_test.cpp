#include <iostream>
#include <nlm_core/Brain.hpp>
#include <nlm_core/Config/Config.hpp>

int main() {
    // Test if performance infrastructure is enabled
    std::cout << "NLM Performance Infrastructure Test" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Test MemoryPool availability
    std::cout << "Testing MemoryPool instantiation..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (brain->initialize()) {
        std::cout << "✓ Brain initialized successfully" << std::endl;
        
        // Check if performance features are enabled
        std::cout << "\nPerformance Features Status:" << std::endl;
        std::cout << "  NLM_PERFORMANCE_ENABLED: " << (nlm::Brain::getPerformanceEnabled() ? "YES" : "NO") << std::endl;
        std::cout << "  NLM_USE_MEMORY_POOL: " << (nlm::Brain::getMemoryPoolEnabled() ? "YES" : "NO") << std::endl;
        std::cout << "  NLM_USE_SOA: " << (nlm::Brain::getSOAEnabled() ? "YES" : "NO") << std::endl;
        std::cout << "  NLM_USE_SIMD: " << (nlm::Brain::getSIMDEnabled() ? "YES" : "NO") << std::endl;
        
        // Run a simple simulation
        std::cout << "\nRunning basic connectivity test..." << std::endl;
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain->step(step, step * 0.001);
        }
        
        std::cout << "✓ Simulation completed successfully" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        
        brain->logStatus();
        return 0;
    }
    
    std::cerr << "✗ Brain initialization failed" << std::endl;
    return 1;
}
