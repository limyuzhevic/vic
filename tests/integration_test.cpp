#include "tests/test_brain.cpp"
#include "tests/test_config.cpp"
#include "tests/test_neuron.cpp"
#include "tests/test_stdp.cpp"
#include "tests/test_synapse.cpp"
#include "tests/test_clock.cpp"
#include "tests/test_random.cpp"
#include "tests/test_types.cpp"
#include "test/TestUtils.hpp"
#include <cassert>
#include <iostream>
#include <vector>

// Simple test that demonstrates the new test infrastructure
namespace test_integration {

/**
 * @brief Simple integration test showing how to use BrainFactory
 * 
 * This test demonstrates the new test infrastructure capabilities
 * for creating and testing brains with consistent configurations.
 */
void testBrainFactoryIntegration() {
    std::cout << "Testing BrainFactory integration..." << std::endl;
    
    // Test 1: Create brain using factory
    auto brain1 = nlm::test::BrainFactory::createAndInitializeBrain(100, 2);
    assert(brain1->getTotalNeuronCount() == 100);
    assert(brain1->getRegionCount() == 2);
    
    // Test 2: Create another brain with different configuration
    auto brain2 = nlm::test::BrainFactory::createRealisticBrain(5, 200);
    assert(brain2->getTotalNeuronCount() == 1000);
    
    // Test 3: Use TestUtils for assertions
    nlm::test::TestAssertions::assertBrainValid(*brain1);
    nlm::test::TestAssertions::assertBrainValid(*brain2);
    
    // Test 4: Demonstrate random seed management
    bool reproducible = nlm::test::RandomSeedManager::verifyReproducibility(12345);
    assert(reproducible);
    
    // Test 5: Generate deterministic random values
    auto randomValues = nlm::test::RandomSeedManager::getDeterministicSequence(50, 42);
    assert(randomValues.size() == 50);
    
    // Test 6: Benchmark brain initialization
    double initTime = nlm::test::BenchmarkUtils::timeBrainInitialization(500, 5);
    assert(initTime > 0.0);
    
    std::cout << "BrainFactory integration test passed!" << std::endl;
}

/**
 * @brief Test demonstrating TestUtils assertion helpers
 * 
 * This test shows how to use the enhanced assertion utilities
 * for better error reporting and testing.
 */
void testEnhancedAssertions() {
    std::cout << "Testing enhanced assertions..." << std::endl;
    
    // Create brain for testing assertions
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(200, 3);
    
    // Test brain validation assertion
    nlm::test::TestAssertions::assertBrainValid(*brain);
    
    // Test approximate equality assertion
    nlm::test::TestAssertions::assertApproxEqual(3.14159f, 3.1416f, 0.00001f);
    nlm::test::TestAssertions::assertApproxEqual(2.71828f, 2.7183f, 0.00001f);
    
    // Test bounds assertion
    nlm::test::TestAssertions::assertWithinBounds<float>(0.5f, 0.0f, 1.0f);
    nlm::test::TestAssertions::assertWithinBounds<int>(42, 0, 100);
    
    // Test random sequence assertion
    auto sequence = nlm::test::RandomSeedManager::getDeterministicSequence(100, 123);
    nlm::RandomGenerator rng(123);
    nlm::test::TestAssertions::assertRandomSequence(rng, sequence);
    
    std::cout << "Enhanced assertions test passed!" << std::endl;
}

/**
 * @brief Test demonstrating integration testing patterns
 * 
 * This test shows how to use the new test infrastructure for
 * integration testing of complex brain systems.
 */
void testIntegrationPatterns() {
    std::cout << "Testing integration patterns..." << std::endl;
    
    // Pattern 1: Brain lifecycle test
    std::cout << "  Pattern 1: Brain lifecycle" << std::endl;
    {
        auto brain = nlm::test::BrainFactory::createAndInitializeBrain(150, 2);
        
        // Initialize and simulate
        brain->step(0);
        brain->step(1);
        
        // Verify state
        assert(brain->getTotalNeuronCount() == 150);
        assert(brain->getTotalSpikeCount() >= 0);
        
        // Reset and re-initialize
        brain->reset();
        brain->initialize();
        assert(brain->getTotalNeuronCount() == 150);
    }
    
    // Pattern 2: Configuration testing
    std::cout << "  Pattern 2: Configuration testing" << std::endl;
    auto config = nlm::test::ConfigFactory::createMinimal();
    nlm::test::TestAssertions::assertConfigValid(config);
    
    // Pattern 3: Random seed reproducibility
    std::cout << "  Pattern 3: Random seed reproducibility" << std::endl;
    bool reproducible = nlm::test::RandomSeedManager::verifyReproducibility(999);
    assert(reproducible);
    
    // Pattern 4: Performance testing
    std::cout << "  Pattern 4: Performance testing" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(300, 5);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    assert(duration > 0);
    std::cout << "    Brain initialization time: " << duration << "ms" << std::endl;
    
    // Pattern 5: Stress testing
    std::cout << "  Pattern 5: Stress testing" << std::endl;
    for (int i = 0; i < 10; ++i) {
        auto stressBrain = nlm::test::BrainFactory::createAndInitializeBrain(50, 2);
        // Simulate some stress
        stressBrain->step(i);
        stressBrain->reset();
        stressBrain->initialize();
    }
    std::cout << "    Stress test pattern passed" << std::endl;
    
    std::cout << "Integration patterns test passed!" << std::endl;
}

/**
 * @brief Test demonstrating comprehensive test suite
 * 
 * This test shows how to run all available tests
 * using the new test infrastructure.
 */
void runComprehensiveTests() {
    std::cout << "Running comprehensive tests..." << std::endl;
    
    // Run all integration tests
    testBrainFactoryIntegration();
    std::cout << std::endl;
    
    testEnhancedAssertions();
    std::cout << std::endl;
    
    testIntegrationPatterns();
    std::cout << std::endl;
    
    std::cout << "All comprehensive tests passed!" << std::endl;
}

/**
 * @brief Example showing how to use the old tests with new infrastructure
 * 
 * This test demonstrates how to incorporate the existing test patterns
 * into the new test framework.
 */
void testCompatibilityWithExistingTests() {
    std::cout << "Testing compatibility with existing tests..." << std::endl;
    
    // The old test_brain.cpp test can now be enhanced with new utilities
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    // Verify brain initialized with expected properties
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    assert(brain.getTotalSynapseCount() > 0);
    
    // Use new test utilities for additional assertions
    nlm::test::TestAssertions::assertBrainValid(brain);
    
    std::cout << "Compatibility with existing tests passed!" << std::endl;
}

/**
 * @brief Main function demonstrating test infrastructure
 * 
 * This function shows how to use the new test infrastructure
 * for comprehensive testing of the NLM system.
 */
int main() {
    std::cout << "NLM Test Infrastructure Demonstration" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Run comprehensive tests
    runComprehensiveTests();
    
    // Run compatibility tests
    testCompatibilityWithExistingTests();
    
    std::cout << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    std::cout << std::endl;
    std::cout << "Key improvements demonstrated:" << std::endl;
    std::cout << "  - BrainFactory for consistent brain creation" << std::endl;
    std::cout << "  - TestUtils for enhanced assertions" << std::endl;
    std::cout << "  - RandomSeedManager for reproducibility" << std::endl;
    std::cout << "  - BenchmarkUtils for performance measurement" << std::endl;
    std::cout << "  - Integration testing patterns" << std::endl;
    std::cout << "  - Stress testing capabilities" << std::endl;
    
    return 0;
}

} // namespace test_integration

// Simple test runner for demonstration
class DemoTestRunner {
public:
    static void runAllTests() {
        std::cout << "NLM Test Infrastructure Demo" << std::endl;
        std::cout << "================================" << std::endl;
        
        // Run the new integration tests
        test_integration::runComprehensiveTests();
        
        std::cout << std::endl;
        std::cout << "Demo tests completed successfully!" << std::endl;
    }
};

int main() {
    DemoTestRunner::runAllTests();
    return 0;
}

} // namespace nlm

// Test macro example (can be included in test_brain.cpp)
TEST_BRAIN_CREATION(testNewExample, 250, 4)
TEST_BRAIN_STEP(testNewStepExample, 300, 100)