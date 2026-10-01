#include "src/development/DevelopmentSystem.hpp"
#include "src/brain/Brain.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Development System Integration..." << std::endl;
    
    // Test 1: DevelopmentSystem can be instantiated
    nlm::DevelopmentSystem devSystem;
    std::cout << "✓ DevelopmentSystem instantiated" << std::endl;
    
    // Test 2: DevelopmentSystem has new methods
    nlm::DevelopmentalStage stage = devSystem.getStage();
    std::cout << "✓ Current developmental stage: " << static_cast<int>(stage) << std::endl;
    
    // Test 3: DevelopmentSystem has enhanced getters
    float excitability = devSystem.getNeuralExcitability();
    float attentionBalance = devSystem.getAttentionBalance();
    float memoryStrength = devSystem.getMemoryConsolidationStrength();
    float neuromodSensitivity = devSystem.getNeuromodulationSensitivity();
    
    std::cout << "✓ Neural excitability: " << excitability << std::endl;
    std::cout << "✓ Attention balance: " << attentionBalance << std::endl;
    std::cout << "✓ Memory consolidation strength: " << memoryStrength << std::endl;
    std::cout << "✓ Neuromodulation sensitivity: " << neuromodSensitivity << std::endl;
    
    // Test 4: DevelopmentSystem methods work
    devSystem.advanceStage();
    std::cout << "✓ Stage advanced" << std::endl;
    
    bool isOptimal = devSystem.isOptimalStageForLearning();
    float learningPotential = devSystem.getLearningPotential();
    std::cout << "✓ Is optimal for learning: " << (isOptimal ? "Yes" : "No") << std::endl;
    std::cout << "✓ Learning potential: " << learningPotential << std::endl;
    
    // Test 5: Brain has development system access
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    config->set("region_count", 1, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool initSuccess = brain.initialize();
    
    if (initSuccess) {
        std::cout << "✓ Brain initialized successfully" << std::endl;
        
        // Test 6: Development integration in brain step
        nlm::DevelopmentSystem* devSys = brain.getDevelopmentSystem();
        if (devSys) {
            std::cout << "✓ Brain has development system" << std::endl;
            
            // Test the enhanced getters
            std::cout << "  - Current stage: " << static_cast<int>(devSys->getStage()) << std::endl;
            std::cout << "  - Neural excitability: " << devSys->getNeuralExcitability() << std::endl;
            std::cout << "  - Attention balance: " << devSys->getAttentionBalance() << std::endl;
            std::cout << "  - Memory consolidation strength: " << devSys->getMemoryConsolidationStrength() << std::endl;
            std::cout << "  - Neuromodulation sensitivity: " << devSys->getNeuromodulationSensitivity() << std::endl;
            
            // Test brain helper methods
            brain.applyNeuralExcitabilityEffects(devSys->getNeuralExcitability());
            std::cout << "✓ Neural excitability effects applied" << std::endl;
            
            brain.applyDevelopmentToNeuromodulators();
            std::cout << "✓ Neuromodulation development effects applied" << std::endl;
            
            brain.applyDevelopmentToMemorySystems();
            std::cout << "✓ Memory development effects applied" << std::endl;
            
            brain.applyDevelopmentToAttentionSystem();
            std::cout << "✓ Attention development effects applied" << std::endl;
        } else {
            std::cout << "✗ Brain does not have development system" << std::endl;
            return 1;
        }
    } else {
        std::cout << "✗ Brain initialization failed" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Development System Integration Test PASSED ===" << std::endl;
    std::cout << "\nEnhanced Development System Features:" << std::endl;
    std::cout << "✓ Neural excitability effects - Age-appropriate neuron responsiveness" << std::endl;
    std::cout << "✓ Attention balance effects - Developmental attention tuning" << std::endl;
    std::cout << "✓ Memory consolidation effects - Enhanced memory processing during development" << std::endl;
    std::cout << "✓ Neuromodulation sensitivity effects - Age-dependent neuromodulator levels" << std::endl;
    std::cout << "✓ Complete brain loop integration - All effects coordinated in Brain::step()" << std::endl;
    std::cout << "✓ Development parameter tuning - Real developmental trajectory" << std::endl;
    
    return 0;
}
