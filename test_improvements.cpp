#!/usr/bin/env g++
// NLM Improvements Test Suite
// Tests all the major improvements made to the NLM brain simulation project

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// Include all the improved systems
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Acetylcholine.hpp"
#include "neuromodulation/Norepinephrine.hpp"
#include "neuromodulation/Serotonin.hpp"

using namespace nlm;

class NLMImprovementsTest {
public:
    NLMImprovementsTest() {
        std::cout << "=== NLM Improvements Test Suite ===" << std::endl;
        std::cout << "Testing all major improvements made to NLM" << std::endl;
    }
    
    bool runAllTests() {
        bool allPassed = true;
        
        std::cout << "\n1. Testing Configuration System..." << std::endl;
        if (!testConfigurationSystem()) {
            allPassed = false;
            std::cout << "FAILED: Configuration system test failed" << std::endl;
        } else {
            std::cout << "PASSED: Configuration system working correctly" << std::endl;
        }
        
        std::cout << "\n2. Testing Logging System..." << std::endl;
        if (!testLoggingSystem()) {
            allPassed = false;
            std::cout << "FAILED: Logging system test failed" << std::endl;
        } else {
            std::cout << "PASSED: Logging system working correctly" << std::endl;
        }
        
        std::cout << "\n3. Testing Random Number Generator..." << std::endl;
        if (!testRandomNumberGenerator()) {
            allPassed = false;
            std::cout << "FAILED: Random number generator test failed" << std::endl;
        } else {
            std::cout << "PASSED: Random number generator working correctly" << std::endl;
        }
        
        std::cout << "\n4. Testing Neuromodulation Systems..." << std::endl;
        if (!testNeuromodulationSystems()) {
            allPassed = false;
            std::cout << "FAILED: Neuromodulation systems test failed" << std::endl;
        } else {
            std::cout << "PASSED: Neuromodulation systems working correctly" << std::endl;
        }
        
        std::cout << "\n5. Testing Integration..." << std::endl;
        if (!testIntegration()) {
            allPassed = false;
            std::cout << "FAILED: Integration test failed" << std::endl;
        } else {
            std::cout << "PASSED: All systems integrated correctly" << std::endl;
        }
        
        return allPassed;
    }
    
private:
    bool testConfigurationSystem() {
        try {
            // Test configuration creation
            auto config = std::make_shared<Config>();
            
            // Test setting various configuration values
            config->set("neuron_count", ConfigValue(static_cast<int64_t>(1000)));
            config->set("simulation_timestep", ConfigValue(0.001));
            config->set("connection_probability", ConfigValue(0.15f));
            config->set("development_enabled", ConfigValue(true));
            config->set("stdp_ltp_weight", ConfigValue(0.02f));
            
            // Test value retrieval
            int64_t neuronCount;
            if (!config->get("neuron_count", neuronCount)) return false;
            if (neuronCount != 1000) return false;
            
            float timestep;
            if (!config->get("simulation_timestep", timestep)) return false;
            if (std::abs(timestep - 0.001f) > 0.0001f) return false;
            
            bool devEnabled;
            if (!config->get("development_enabled", devEnabled)) return false;
            if (!devEnabled) return false;
            
            // Test default values
            float unknownValue = config->getOr("unknown_setting", 0.5f);
            if (std::abs(unknownValue - 0.5f) > 0.0001f) return false;
            
            // Test configuration keys
            std::vector<std::string> keys = config->getKeys();
            if (keys.empty()) return false;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception in configuration test: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool testLoggingSystem() {
        try {
            // Test logger singleton
            Logger& logger = Logger::getInstance();
            logger.setLevel(LogLevel::Info);
            
            // Test log message output (this should be captured by console)
            NLM_LOG_INFO("Test info message");
            NLM_LOG_WARNING("Test warning message");
            NLM_LOG_ERROR("Test error message");
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception in logging test: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool testRandomNumberGenerator() {
        try {
            RandomGenerator rng(42);  // Fixed seed for reproducibility
            
            // Test uniform integer distribution
            int64_t value = rng.uniformInt(1, 100);
            if (value < 1 || value > 100) return false;
            
            // Test uniform real distribution
            float realValue = rng.uniformReal();
            if (realValue < 0.0f || realValue >= 1.0f) return false;
            
            float boundedValue = rng.uniformReal(10.0f, 20.0f);
            if (boundedValue < 10.0f || boundedValue > 20.0f) return false;
            
            // Test uniform double distribution
            double doubleValue = rng.uniformDouble();
            if (doubleValue < 0.0 || doubleValue >= 1.0) return false;
            
            // Test boolean distribution
            bool boolValue = rng.randomBool(0.7f);
            // 70% chance of true
            
            // Test normal distribution
            float normalValue = rng.normalRandom(0.0f, 1.0f);
            if (std::isnan(normalValue)) return false;
            
            // Test exponential distribution
            float exponentialValue = rng.exponentialRandom(1.0f);
            if (exponentialValue < 0.0f) return false;
            
            // Test seed management
            uint64_t originalSeed = rng.getSeed();
            rng.setSeed(123);
            if (rng.getSeed() != 123) return false;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception in random number generator test: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool testNeuromodulationSystems() {
        try {
            // Test neuromodulator implementations
            Dopamine dopamine;
            Acetylcholine acetylcholine;
            Norepinephrine norepinephrine;
            Serotonin serotonin;
            
            // Test basic neuromodulator interface
            const char* dopamineName = dopamine.getName();
            const char* achName = acetylcholine.getName();
            const char* neName = norepinephrine.getName();
            const char* fiveHtName = serotonin.getName();
            
            if (!dopamineName || !achName || !neName || !fiveHtName) return false;
            
            // Test level management
            dopamine.setLevel(0.8f);
            acetylcholine.setLevel(0.6f);
            norepinephrine.setLevel(0.9f);
            serotonin.setLevel(0.4f);
            
            if (dopamine.getLevel() != 0.8f) return false;
            if (acetylcholine.getLevel() != 0.6f) return false;
            if (norepinephrine.getLevel() != 0.9f) return false;
            if (serotonin.getLevel() != 0.4f) return false;
            
            // Test plasticity factors
            float dopaminePlasticity = dopamine.getPlasticityFactor();
            float achPlasticity = acetylcholine.getPlasticityFactor();
            float nePlasticity = norepinephrine.getPlasticityFactor();
            float fiveHtPlasticity = serotonin.getPlasticityFactor();
            
            if (std::isnan(dopaminePlasticity) || std::isnan(achPlasticity) ||
                std::isnan(nePlasticity) || std::isnan(fiveHtPlasticity)) return false;
            
            // Test update methods
            dopamine.update(0.001f);
            acetylcholine.update(0.001f);
            norepinephrine.update(0.001f);
            serotonin.update(0.001f);
            
            // Test reset methods
            dopamine.reset();
            acetylcholine.reset();
            norepinephrine.reset();
            serotonin.reset();
            
            if (dopamine.getLevel() != 0.0f) return false;
            if (acetylcholine.getLevel() != 0.0f) return false;
            if (norepinephrine.getLevel() != 0.0f) return false;
            if (serotonin.getLevel() != 0.0f) return false;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception in neuromodulation test: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool testIntegration() {
        try {
            // Test that all systems work together
            auto config = std::make_shared<Config>();
            config->set("neuron_count", ConfigValue(static_cast<int64_t>(100)));
            config->set("simulation_timestep", ConfigValue(0.001f));
            
            // Initialize logger
            Logger::getInstance().setLevel(LogLevel::Info);
            NLM_LOG_INFO("Integration test started");
            
            // Initialize random generator
            RandomGenerator rng(42);
            
            // Test neuromodulators
            Dopamine dopamine;
            Acetylcholine acetylcholine;
            Norepinephrine norepinephrine;
            Serotonin serotonin;
            
            // Set levels
            dopamine.setLevel(0.5f);
            acetylcholine.setLevel(0.3f);
            norepinephrine.setLevel(0.7f);
            serotonin.setLevel(0.4f);
            
            // Test configuration integration
            int64_t neuronCount;
            config->get("neuron_count", neuronCount);
            float timestep;
            config->get("simulation_timestep", timestep);
            
            // Verify integration
            if (neuronCount != 100) return false;
            if (std::abs(timestep - 0.001f) > 0.0001f) return false;
            if (dopamine.getLevel() != 0.5f) return false;
            
            NLM_LOG_INFO("Integration test completed successfully");
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Exception in integration test: " << e.what() << std::endl;
            return false;
        }
    }
};

int main(int argc, char* argv[]) {
    NLMImprovementsTest test;
    bool success = test.runAllTests();
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    if (success) {
        std::cout << "SUCCESS: All NLM improvements tests passed!" << std::endl;
        std::cout << "The enhanced NLM system is working correctly with:" << std::endl;
        std::cout << "  ✓ Advanced configuration management" << std::endl;
        std::cout << "  ✓ Comprehensive logging system" << std::endl;
        std::cout << "  ✓ Enhanced random number generation" << std::endl;
        std::cout << "  ✓ Real neuromodulation systems (Dopamine, ACh, NE, 5-HT)" << std::endl;
        std::cout << "  ✓ System integration" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain simulation has been successfully enhanced!" << std::endl;
        return 0;
    } else {
        std::cout << "FAILURE: Some NLM improvements tests failed!" << std::endl;
        std::cout << "Please check the implementation and try again." << std::endl;
        return 1;
    }
}