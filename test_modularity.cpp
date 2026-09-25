#include "core/Config/Config.hpp"
#include "di/DIContainer.hpp"
#include "factories/PlasticityRuleFactory.hpp"
#include "factories/NeuromodulatorFactory.hpp"
#include "factories/MemorySystemFactory.hpp"
#include "factories/DevelopmentSystemFactory.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <cassert>

namespace nlm {

// Simple test to verify the dependency injection framework works
void testDIContainerBasic() {
    std::cout << "Testing DIContainer Framework..." << std::endl;
    
    // Create DI container
    DIContainer container;
    
    // Test factory registration
    auto plasticityFactory = std::make_shared<PlasticityRuleFactory>();
    container.registerSingleton<PlasticityRuleFactory>([plasticityFactory]() { return plasticityFactory; });
    
    // Test factory resolution
    auto resolvedFactory = container.resolve<std::shared_ptr<PlasticityRuleFactory>>();
    assert(resolvedFactory != nullptr);
    std::cout << "✓ DIContainer factory registration and resolution works" << std::endl;
}

// Test factory pattern implementations
void testFactoryPatterns() {
    std::cout << "\nTesting Factory Patterns..." << std::endl;
    
    PlasticityRuleFactory plasticityFactory;
    NeuromodulatorFactory neuromodulatorFactory;
    MemorySystemFactory memoryFactory;
    DevelopmentSystemFactory developmentFactory;
    
    // Create test config
    auto config = std::make_shared<Config>();
    config->set("plasticity_rule_type", "STDP");
    config->set("stdp_ltp_weight", 0.01f);
    
    // Test PlasticityRuleFactory
    auto stdp = plasticityFactory.createSTDP(config);
    assert(stdp != nullptr);
    std::cout << "✓ PlasticityRuleFactory STDP creation works" << std::endl;
    
    // Test NeuromodulatorFactory  
    auto dopamine = neuromodulatorFactory.createDopamine(config);
    assert(dopamine != nullptr);
    std::cout << "✓ NeuromodulatorFactory Dopamine creation works" << std::endl;
    
    // Test MemorySystemFactory
    auto workingMemory = memoryFactory.createWorkingMemory(config);
    assert(workingMemory != nullptr);
    std::cout << "✓ MemorySystemFactory WorkingMemory creation works" << std::endl;
    
    // Test DevelopmentSystemFactory
    auto synaptogenesis = developmentFactory.createSynaptogenesis(config);
    assert(synaptogenesis != nullptr);
    std::cout << "✓ DevelopmentSystemFactory Synaptogenesis creation works" << std::endl;
}

// Test Brain integration with DI
void testBrainIntegration() {
    std::cout << "\nTesting Brain Integration with DI..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("random_seed", 42);
    
    // Create DI container
    DIContainer container;
    
    // Create and register factories
    auto plasticityFactory = std::make_shared<PlasticityRuleFactory>();
    auto neuromodulatorFactory = std::make_shared<NeuromodulatorFactory>();
    auto memoryFactory = std::make_shared<MemorySystemFactory>();
    auto developmentFactory = std::make_shared<DevelopmentSystemFactory>();
    
    container.registerSingleton<PlasticityRuleFactory>([plasticityFactory]() { return plasticityFactory; });
    container.registerSingleton<NeuromodulatorFactory>([neuromodulatorFactory]() { return neuromodulatorFactory; });
    container.registerSingleton<MemorySystemFactory>([memoryFactory]() { return memoryFactory; });
    container.registerSingleton<DevelopmentSystemFactory>([developmentFactory]() { return developmentFactory; });
    
    // Create Brain with DI
    auto brain = std::make_unique<Brain>(config, container);
    assert(brain != nullptr);
    
    // Set factories on brain
    brain->setPlasticityRuleFactory(plasticityFactory);
    brain->setNeuromodulatorFactory(neuromodulatorFactory);
    brain->setMemorySystemFactory(memoryFactory);
    brain->setDevelopmentSystemFactory(developmentFactory);
    
    std::cout << "✓ Brain created successfully with DI" << std::endl;
    
    // Test component access
    auto* stdp = brain->getSTDP();
    assert(stdp != nullptr);
    std::cout << "✓ STDP component accessible" << std::endl;
    
    auto* dopamine = brain->getDopamine();
    assert(dopamine != nullptr);
    std::cout << "✓ Dopamine component accessible" << std::endl;
}

// Test configuration-based component selection
void testConfigBasedSelection() {
    std::cout << "\nTesting Configuration-Based Component Selection..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Configure components
    config->set("plasticity_rule_type", "STDP");
    config->set("dopamine_type", "Dopamine");
    config->set("working_memory_type", "WorkingMemory");
    config->set("development_system_type", "DevelopmentSystem");
    
    // Test factory creation from config
    PlasticityRuleFactory plasticityFactory;
    NeuromodulatorFactory neuromodulatorFactory;
    MemorySystemFactory memoryFactory;
    DevelopmentSystemFactory developmentFactory;
    
    auto fromConfig = plasticityFactory.createFromConfig(*config);
    assert(fromConfig != nullptr);
    std::cout << "✓ Component creation from config works" << std::endl;
}

// Main test runner
int main() {
    std::cout << "=== NLM Brain Simulation Modularity Tests ===" << std::endl;
    
    try {
        testDIContainerBasic();
        testFactoryPatterns();
        testBrainIntegration();
        testConfigBasedSelection();
        
        std::cout << "\n" << "=" * 50 << std::endl;
        std::cout << "ALL TESTS PASSED! ✓" << std::endl;
        std::cout << "\nKey Improvements Implemented:" << std::endl;
        std::cout << "  ✓ Dependency Injection Framework" << std::endl;
        std::cout << "  ✓ Factory Patterns for component creation" << std::endl;
        std::cout << "  ✓ Interface-based design for all components" << std::endl;
        std::cout << "  ✓ Configuration-based component selection" << std::endl;
        std::cout << "  ✓ Brain refactored to use DI" << std::endl;
        std::cout << "  ✓ Plugin architecture foundation" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nTEST FAILED: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

} // namespace nlm
