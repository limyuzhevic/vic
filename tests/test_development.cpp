// NLM Development Test Suite
// Phase 6: Development Integration Tests

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/core/Config/Config.hpp"

namespace test_development {
    
    void test_development_system() {
        std::cout << "Testing Development System..." << std::endl;
        
        auto config = std::make_shared<Config>();
        DevelopmentSystem developmentSystem(config);
        
        assert(developmentSystem.initialize() == true);
        assert(developmentSystem.getCurrentStage() != nullptr);
        
        // Test development system operations
        developmentSystem.updateDevelopment(1.0f);
        assert(developmentSystem.getCurrentStage() != nullptr);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_synaptogenesis() {
        std::cout << "Testing Synaptogenesis system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Synaptogenesis synaptogenesis(config);
        
        assert(synaptogenesis.initialize() == true);
        assert(synaptogenesis.getSynaptogenesisRate() >= 0.0f);
        
        // Test synaptogenesis operations
        synaptogenesis.updateSynaptogenesis(1.0f);
        assert(synaptogenesis.getSynaptogenesisRate() >= 0.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_pruning() {
        std::cout << "Testing Pruning system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Pruning pruning(config);
        
        assert(pruning.initialize() == true);
        assert(pruning.getPruningRate() >= 0.0f);
        
        // Test pruning operations
        pruning.updatePruning(1.0f);
        assert(pruning.getPruningRate() >= 0.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_maturation() {
        std::cout << "Testing Maturation system..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Maturation maturation(config);
        
        assert(maturation.initialize() == true);
        assert(maturation.getMaturationRate() >= 0.0f);
        
        // Test maturation operations
        maturation.updateMaturation(1.0f);
        assert(maturation.getMaturationRate() >= 0.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_development_system_integration() {
        std::cout << "Testing Development System integration with plasticity..." << std::endl;
        
        auto config = std::make_shared<Config>();
        DevelopmentSystem developmentSystem(config);
        
        assert(developmentSystem.initialize() == true);
        
        // Test development system effects
        developmentSystem.setDevelopmentStage(DevelopmentalStage::CriticalPeriod);
        assert(developmentSystem.getCurrentStage() != nullptr);
        
        developmentSystem.updateDevelopment(1.0f);
        assert(developmentSystem.getCurrentStage() != nullptr);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_synaptogenesis_integration() {
        std::cout << "Testing Synaptogenesis integration with brain development..." << std::endl;
        
        auto config = std::make_shared<Config>();
        Synaptogenesis synaptogenesis(config);
        
        assert(synaptogenesis.initialize() == true);
        
        // Test synaptogenesis effects
        synaptogenesis.setSynaptogenesisRate(0.01f);
        assert(synaptogenesis.getSynaptogenesisRate() == 0.01f);
        
        synaptogenesis.updateSynaptogenesis(1.0f);
        assert(synaptogenesis.getSynaptogenesisRate() == 0.01f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void test_development_integration() {
        std::cout << "Testing Development integration with neuromodulation..." << std::endl;
        
        auto config = std::make_shared<Config>();
        DevelopmentSystem developmentSystem(config);
        Neuromodulator neuromodulator(config);
        
        assert(developmentSystem.initialize() == true);
        assert(neuromodulator.initialize() == true);
        
        // Test development-neuromodulation interaction
        developmentSystem.setDevelopmentStage(DevelopmentalStage::Maturation);
        neuromodulator.setNeuromodulationLevel(0.7f);
        
        assert(developmentSystem.getCurrentStage() != nullptr);
        assert(neuromodulator.getNeuromodulationLevel() == 0.7f);
        
        developmentSystem.updateDevelopment(1.0f);
        neuromodulator.update(1.0f);
        
        std::cout << "  PASSED" << std::endl;
    }
    
    void runAll() {
        std::cout << "=== NLM Development Tests ===" << std::endl;
        
        test_development_system();
        test_synaptogenesis();
        test_pruning();
        test_maturation();
        test_development_system_integration();
        test_synaptogenesis_integration();
        test_development_integration();
        
        std::cout << std::endl;
        std::cout << "=== All Development Tests PASSED ===" << std::endl;
    }
}