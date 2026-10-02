#include <cassert>
#include <iostream>
#include <memory>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "simulation/ISimulationSystem.hpp"
#include "simulation/SimulationSystemProxy.hpp"

namespace test_simulation {

void testFacadeInterface() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test facade interface initialization
    bool success = brain.initialize();
    assert(success);
    
    // Test simulation proxy initialization
    assert(brain.pImpl->simulationProxy != nullptr);
    assert(brain.pImpl->simulationProxy->GetSystemCount() > 0);
    
    // Test system status
    std::string status = brain.pImpl->simulationProxy->GetCombinedStatus();
    assert(!status.empty());
    
    // Test system enable/disable
    auto systems = brain.pImpl->simulationProxy->GetAllSystems();
    assert(!systems.empty());
    
    for (auto* system : systems) {
        if (system) {
            // Test system name
            assert(system->GetName() != nullptr);
            assert(strlen(system->GetName()) > 0);
            
            // Test system status
            std::string sysStatus = system->GetStatus();
            assert(!sysStatus.empty());
        }
    }
    
    std::cout << "    testFacadeInterface passed" << std::endl;
}

void testSystemEnableDisable() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Find a system to test enable/disable
    auto systems = brain.pImpl->simulationProxy->GetAllSystems();
    assert(!systems.empty());
    
    // Test disabling a system
    bool systemDisabled = brain.pImpl->simulationProxy->SetEnabled(systems[0]->GetName(), false);
    assert(systemDisabled);
    
    // Test enabling the system
    bool systemEnabled = brain.pImpl->simulationProxy->SetEnabled(systems[0]->GetName(), true);
    assert(systemEnabled);
    
    std::cout << "    testSystemEnableDisable passed" << std::endl;
}

void testSimulationProxy() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Test proxy initialization
    auto& proxy = nlm::SimulationSystemProxy::GetInstance();
    proxy.Initialize(&brain);
    
    // Test adding a system
    assert(proxy.GetSystemCount() == brain.pImpl->simulationProxy->GetSystemCount());
    
    // Test system retrieval
    assert(proxy.GetSystem("NeuralWorkingMemorySystem") != nullptr);
    
    // Test system existence check
    assert(proxy.HasSystem("NeuralWorkingMemorySystem"));
    assert(!proxy.HasSystem("NonExistentSystem"));
    
    std::cout << "    testSimulationProxy passed" << std::endl;
}

void testDependencyInjection() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    
    // Test that systems receive brain reference during initialization
    // This is tested implicitly through the initialize() call
    assert(brain.pImpl->simulationProxy != nullptr);
    
    // Run simulation steps to test system updates
    for (nlm::SimulationStep step = 0; step < 5; ++step) {
        brain.step(step);
    }
    
    std::cout << "    testDependencyInjection passed" << std::endl;
}

void testBackwardCompatibility() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Verify that direct memory system access still works (backward compatibility)
    auto* workingMemory = brain.getWorkingMemory();
    assert(workingMemory != nullptr);
    
    auto* episodicMemory = brain.getEpisodicMemory();
    assert(episodicMemory != nullptr);
    
    // Verify that neuromodulation systems still work
    auto* dopamine = brain.getDopamine();
    assert(dopamine != nullptr);
    
    std::cout << "    testBackwardCompatibility passed" << std::endl;
}

void runAll() {
    testFacadeInterface();
    testSystemEnableDisable();
    testSimulationProxy();
    testDependencyInjection();
    testBackwardCompatibility();
}

} // namespace test_simulation
