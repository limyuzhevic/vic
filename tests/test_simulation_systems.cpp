#include <iostream>
#include <cassert>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "simulation/ISimulationSystem.hpp"
#include "simulation/SimulationSystemProxy.hpp"

int main() {
    std::cout << "NLM Simulation Systems Facade Test" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Test 1: Basic facade interface
    {
        std::cout << "Test 1: Basic facade interface..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 2: Simulation system proxy
    {
        std::cout << "Test 2: Simulation system proxy..." << std::endl;
        auto& proxy = nlm::SimulationSystemProxy::GetInstance();
        
        std::string status = proxy.GetStatus();
        assert(!status.empty());
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 3: System enable/disable functionality
    {
        std::cout << "Test 3: System enable/disable..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Check that systems are initially enabled
        auto* workingMemorySystem = proxy.GetSystem("NeuralWorkingMemorySystem");
        assert(workingMemorySystem != nullptr);
        assert(workingMemorySystem->IsEnabled());
        
        // Test toggle
        bool toggled = proxy.ToggleEnabled("NeuralWorkingMemorySystem");
        assert(toggled);
        
        // Test disable
        bool disabled = proxy.SetEnabled("NeuralWorkingMemorySystem", false);
        assert(disabled);
        assert(!workingMemorySystem->IsEnabled());
        
        // Test enable
        bool enabled = proxy.SetEnabled("NeuralWorkingMemorySystem", true);
        assert(enabled);
        assert(workingMemorySystem->IsEnabled());
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 4: Dependency injection
    {
        std::cout << "Test 4: Dependency injection..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run simulation steps
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 5: Backward compatibility
    {
        std::cout << "Test 5: Backward compatibility..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Verify direct memory system access still works
        auto* workingMemory = brain.getWorkingMemory();
        assert(workingMemory != nullptr);
        
        auto* episodicMemory = brain.getEpisodicMemory();
        assert(episodicMemory != nullptr);
        
        // Verify neuromodulation systems still work
        auto* dopamine = brain.getDopamine();
        assert(dopamine != nullptr);
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 6: System status tracking
    {
        std::cout << "Test 6: System status tracking..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Get combined status
        std::string combinedStatus = brain.pImpl->simulationProxy->GetCombinedStatus();
        assert(!combinedStatus.empty());
        
        // Get individual system statuses
        auto systems = brain.pImpl->simulationProxy->GetAllSystems();
        for (auto* system : systems) {
            if (system) {
                std::string status = system->GetStatus();
                assert(!status.empty());
                
                // Test system name
                const char* name = system->GetName();
                assert(name != nullptr && strlen(name) > 0);
            }
        }
        
        std::cout << "   PASSED" << std::endl;
    }
    
    // Test 7: Logging
    {
        std::cout << "Test 7: Logging integration..." << std::endl;
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run a step to generate logs
        brain.step(0);
        
        // Log status to verify logging works
        brain.logStatus();
        
        std::cout << "   PASSED" << std::endl;
    }
    
    std::cout << "====================================" << std::endl;
    std::cout << "All tests passed!" << std::endl;
    
    return 0;
}
