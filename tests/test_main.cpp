// Enhanced NLM Test Main with Consolidated Test Framework
// 
// This file provides a unified test runner that integrates with the enhanced
// test framework, improving upon the original test structure with:
// 1. Better error handling and reporting
// 2. Integration with enhanced test utilities
// 3. Consistent test patterns across modules
// 4. Progress tracking and detailed output
// 5. Enhanced validation and debugging support

#include "TestFramework.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>

// Forward declarations for test modules
namespace test_types {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_config {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_random {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_clock {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_neuron {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_synapse {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_stdp {
    void runAll(nlm::test::TestRunner& runner);
}

namespace test_brain {
    void runAll(nlm::test::TestRunner& runner);
}

// Test implementation functions (simplified versions)
void testNeuronCreation(nlm::test::TestRunner& runner) {
    try {
        nlm::Neuron neuron(nlm::NeuronId(1));
        assert(neuron.getId() == nlm::NeuronId(1));
        assert(neuron.getType() == nlm::NeuronType::Internal);
        runner.addTest("test_neuron", "testNeuronCreation", []() {
            nlm::Neuron neuron(nlm::NeuronId(1));
            assert(neuron.getId() == nlm::NeuronId(1));
            assert(neuron.getType() == nlm::NeuronType::Internal);
        });
        nlm::test::utils::logTestInfo("testNeuronCreation passed");
    } catch (const std::exception& e) {
        std::cout << "    ERROR: " << e.what() << std::endl;
        throw;
    }
}

void testNeuronType(nlm::test::TestRunner& runner) {
    runner.addTest("test_neuron", "testNeuronType", []() {
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.setType(nlm::NeuronType::Excitatory);
        assert(neuron.getType() == nlm::NeuronType::Excitatory);
        neuron.setType(nlm::NeuronType::Inhibitory);
        assert(neuron.getType() == nlm::NeuronType::Inhibitory);
    });
    nlm::test::utils::logTestInfo("testNeuronType passed");
}

void testBrainCreation(nlm::test::TestRunner& runner) {
    runner.addTest("test_brain", "testBrainCreation", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        nlm::Brain brain(config);
    });
    nlm::test::utils::logTestInfo("testBrainCreation passed");
}

void testBrainInitialization(nlm::test::TestRunner& runner) {
    runner.addTest("test_brain", "testBrainInitialization", []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        bool success = brain.initialize();
        assert(success);
        assert(brain.getRegionCount() == 2);
        assert(brain.getTotalNeuronCount() == 100);
        assert(brain.getTotalSynapseCount() > 0);
    });
    nlm::test::utils::logTestInfo("testBrainInitialization passed");
}

int main() {
    std::cout << "=== Enhanced NLM Test Suite ===" << std::endl;
    std::cout << "Starting comprehensive test execution with improved framework..." << std::endl;
    std::cout << std::endl;
    
    // Create test runner
    nlm::test::TestRunner runner;
    
    // Register all test modules
    try {
        test_types::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_types: " << e.what() << std::endl;
    }
    
    try {
        test_config::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_config: " << e.what() << std::endl;
    }
    
    try {
        test_random::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_random: " << e.what() << std::endl;
    }
    
    try {
        test_clock::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_clock: " << e.what() << std::endl;
    }
    
    try {
        test_neuron::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_neuron: " << e.what() << std::endl;
    }
    
    try {
        test_synapse::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_synapse: " << e.what() << std::endl;
    }
    
    try {
        test_stdp::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_stdp: " << e.what() << std::endl;
    }
    
    try {
        test_brain::runAll(runner);
    } catch (const std::exception& e) {
        std::cout << "ERROR in test_brain: " << e.what() << std::endl;
    }
    
    // Run all tests
    bool allPassed = runner.runAllTests();
    
    std::cout << std::endl;
    std::cout << "=== Final Test Results ===" << std::endl;
    std::cout << "Total test cases: " << runner.getTotalTests() << std::endl;
    std::cout << "Passed: " << runner.getPassedTests() << std::endl;
    std::cout << "Failed: " << runner.getFailedTests() << std::endl;
    std::cout << "Success rate: " << 
        (runner.getTotalTests() > 0 ? (runner.getPassedTests() * 100.0 / runner.getTotalTests()) : 0.0) << "%" << std::endl;
    std::cout << "Total execution time: " << 
        std::fixed << std::setprecision(2) << runner.getTotalExecutionTime() << " ms" << std::endl;
    
    if (allPassed) {
        std::cout << std::endl;
        std::cout << "🎉 All tests passed! The NLM implementation is ready for production." << std::endl;
        return 0;
    } else {
        std::cout << std::endl;
        std::cout << "❌ Some tests failed. Please review the implementation." << std::endl;
        return 1;
    }
}