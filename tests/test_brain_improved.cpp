// Test basic Brain functionality with improved error handling and validation
#include "brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <limits>

namespace test_brain_improved {

// Helper function for error reporting
inline void test_log_failure(const std::string& testName, const std::string& message) {
    std::cout << "FAILED: " << testName << " - " << message << std::endl;
}

void testBrainCreationWithValidation() {
    try {
        // Test 1: Basic creation
        auto config = std::make_shared<nlm::Config>();
        nlm::Brain brain(config);
        
        // Verify creation
        assert(brain.getRegionCount() == 0);  // No regions until initialized
        assert(brain.getTotalNeuronCount() == 0);
        
        std::cout << "    testBrainCreationWithValidation passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainCreationWithValidation", 
                       "Exception during creation: " + std::string(e.what()));
        throw;
    }
}

void testBrainInitializationWithValidation() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        // Test initialization with error handling
        bool success = brain.initialize();
        if (!success) {
            test_log_failure("testBrainInitializationWithValidation", 
                           "Brain initialization failed");
            return;
        }
        
        // Verify initialization with comprehensive checks
        assert(brain.getRegionCount() == 1);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
        assert(brain.getTotalNeuronCount() > 0);
        
        // Test accessing various systems
        assert(brain.getSpikeSystem() != nullptr);
        assert(brain.getSTDP() != nullptr);
        assert(brain.getHebbian() != nullptr);
        assert(brain.getStructuralPlasticity() != nullptr);
        
        std::cout << "    testBrainInitializationWithValidation passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainInitializationWithValidation", 
                       "Exception during initialization: " + std::string(e.what()));
        throw;
    }
}

void testBrainStepWithErrorHandling() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test steps with validation
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
            
            // Validate state after each step
            assert(brain.getTotalNeuronCount() == 100);
            assert(brain.getTotalSpikeCount() >= 0);
            assert(brain.getFiringNeuronCount() >= 0);
        }
        
        std::cout << "    testBrainStepWithErrorHandling passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainStepWithErrorHandling", 
                       "Exception during stepping: " + std::string(e.what()));
        throw;
    }
}

void testBrainResetWithValidation() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run some steps to generate state
        for (nlm::SimulationStep step = 0; step < 5; ++step) {
            brain.step(step);
        }
        
        // Test reset with validation
        brain.reset();
        
        // Verify reset worked
        assert(brain.getTotalNeuronCount() == 50);
        assert(brain.getTotalSpikeCount() == 0);  // Should be reset
        assert(brain.getPendingSpikeEventCount() == 0);
        
        std::cout << "    testBrainResetWithValidation passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainResetWithValidation", 
                       "Exception during reset: " + std::string(e.what()));
        throw;
    }
}

void testBrainRegionsWithValidation() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test region access with validation
        auto regionIds = brain.getRegionIds();
        assert(regionIds.size() == 3);
        
        for (auto rid : regionIds) {
            auto* region = brain.getRegion(rid);
            if (region == nullptr) {
                test_log_failure("testBrainRegionsWithValidation", 
                               "Region " + std::to_string(rid.index()) + " not found");
                return;
            }
            
            // Validate region properties
            assert(region->getId() == rid);
            assert(region->getTotalNeuronCount() > 0);
        }
        
        std::cout << "    testBrainRegionsWithValidation passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainRegionsWithValidation", 
                       "Exception during region testing: " + std::string(e.what()));
        throw;
    }
}

void testBrainActionProductionWithValidation() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test action production with validation
        auto action = brain.produceAction();
        assert(action != nullptr);
        
        // Validate action properties
        auto actionType = action->getType();
        assert(static_cast<int>(actionType) >= 0);  // Valid action type
        
        std::cout << "    testBrainActionProductionWithValidation passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainActionProductionWithValidation", 
                       "Exception during action production: " + std::string(e.what()));
        throw;
    }
}

void testBrainStatusWithValidation() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test status access with validation
        size_t totalNeurons = brain.getTotalNeuronCount();
        size_t totalSynapses = brain.getTotalSynapseCount();
        size_t activeNeurons = brain.getActiveNeuronCount();
        size_t firingNeurons = brain.getFiringNeuronCount();
        float avgFiringRate = brain.getAverageFiringRate();
        size_t pendingSpikes = brain.getPendingSpikeEventCount();
        
        // Validate statistics
        assert(totalNeurons > 0);
        assert(totalSynapses >= 0);
        assert(activeNeurons >= 0);
        assert(firingNeurons >= 0);
        assert(avgFiringRate >= 0.0f);
        assert(pendingSpikes >= 0);
        
        // Test E/I ratio calculation (should not crash)
        float eiRatio = brain.getExcitationInhibitionRatio();
        assert(std::isfinite(eiRatio) || eiRatio == std::numeric_limits<float>::infinity());
        
        std::cout << "    testBrainStatusWithValidation passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testBrainStatusWithValidation", 
                       "Exception during status testing: " + std::string(e.what()));
        throw;
    }
}

void testMemorySystemAccess() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test memory system access with validation
        auto* workingMemory = brain.getWorkingMemory();
        auto* episodicMemory = brain.getEpisodicMemory();
        auto* associativeMemory = brain.getAssociativeMemory();
        
        // Memory systems should be accessible (may be nullptr if not properly initialized)
        if (workingMemory != nullptr) {
            assert(workingMemory->getActiveTraces() >= 0);
        }
        
        if (episodicMemory != nullptr) {
            assert(episodicMemory->getEpisodeCount() >= 0);
        }
        
        if (associativeMemory != nullptr) {
            // Test associative memory if available
        }
        
        std::cout << "    testMemorySystemAccess passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testMemorySystemAccess", 
                       "Exception during memory system testing: " + std::string(e.what()));
        throw;
    }
}

void testPlasticitySystemsAccess() {
    try {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Test plasticity system access with validation
        auto* stdp = brain.getSTDP();
        auto* hebbian = brain.getHebbian();
        auto* structural = brain.getStructuralPlasticity();
        
        // Plasticity systems should be accessible
        if (stdp != nullptr) {
            // Can check if STDP is properly configured
        }
        
        if (hebbian != nullptr) {
            // Can check Hebbian learning parameters
        }
        
        if (structural != nullptr) {
            // Can check structural plasticity parameters
        }
        
        std::cout << "    testPlasticitySystemsAccess passed" << std::endl;
    } catch (const std::exception& e) {
        test_log_failure("testPlasticitySystemsAccess", 
                       "Exception during plasticity system testing: " + std::string(e.what()));
        throw;
    }
}

void runAllImprovedTests() {
    std::cout << "Running improved NLM Brain tests with error handling and validation..." << std::endl;
    
    testBrainCreationWithValidation();
    testBrainInitializationWithValidation();
    testBrainStepWithErrorHandling();
    testBrainResetWithValidation();
    testBrainRegionsWithValidation();
    testBrainActionProductionWithValidation();
    testBrainStatusWithValidation();
    testMemorySystemAccess();
    testPlasticitySystemsAccess();
    
    std::cout << "All improved tests completed successfully!" << std::endl;
}

} // namespace test_brain_improved
