#include "test/TestUtils.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_examples {

/**
 * @brief Example test demonstrating BrainFactory usage
 * 
 * This test shows how to use the BrainFactory utilities to create
 * brains with different configurations for testing purposes.
 */
void testBrainFactoryExamples() {
    std::cout << "Testing BrainFactory examples..." << std::endl;
    
    // Example 1: Create a minimal brain for fast unit tests
    auto minimalBrain = nlm::test::BrainFactory::createAndInitializeBrain(50, 1);
    assert(minimalBrain->getTotalNeuronCount() == 50);
    std::cout << "  - Minimal brain: 50 neurons, " << minimalBrain->getTotalSynapseCount() << " synapses" << std::endl;
    
    // Example 2: Create a basic brain with realistic parameters
    auto basicBrain = nlm::test::BrainFactory::createAndInitializeBrain(100, 2);
    assert(basicBrain->getTotalNeuronCount() == 100);
    assert(basicBrain->getRegionCount() == 2);
    std::cout << "  - Basic brain: 100 neurons, " << basicBrain->getRegionCount() << " regions" << std::endl;
    
    // Example 3: Create a realistic brain configuration
    auto realisticBrain = nlm::test::BrainFactory::createRealisticBrain(5, 100);
    assert(realisticBrain->getTotalNeuronCount() == 500);
    std::cout << "  - Realistic brain: 500 neurons" << std::endl;
    
    // Example 4: Brain operations test
    realisticBrain->step(0);
    realisticBrain->reset();
    realisticBrain->initialize();
    assert(realisticBrain->getTotalNeuronCount() == 500);
    std::cout << "  - Brain reset and re-initialize test passed" << std::endl;
    
    std::cout << "BrainFactory examples test passed!" << std::endl;
}

/**
 * @brief Example test demonstrating TestUtils assertions
 * 
 * This test shows how to use the TestUtils assertion helpers
 * for better error reporting and testing.
 */
void testTestUtilsAssertions() {
    std::cout << "Testing TestUtils assertions..." << std::endl;
    
    // Create a brain for testing assertions
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(200, 3);
    
    // Example 1: Assert brain is valid
    nlm::test::TestAssertions::assertBrainValid(*brain);
    std::cout << "  - Brain validity assertion passed" << std::endl;
    
    // Example 2: Assert random sequence reproducibility
    auto sequence = nlm::test::RandomSeedManager::getDeterministicSequence(100, 42);
    nlm::RandomGenerator rng(42);
    nlm::test::TestAssertions::assertRandomSequence(rng, sequence);
    std::cout << "  - Random sequence reproducibility assertion passed" << std::endl;
    
    // Example 3: Assert approximate equality
    nlm::test::TestAssertions::assertApproxEqual(3.14159f, 3.1416f, 0.00001f);
    std::cout << "  - Approximate equality assertion passed" << std::endl;
    
    // Example 4: Assert value is within bounds
    nlm::test::TestAssertions::assertWithinBounds<float>(0.5f, 0.0f, 1.0f);
    std::cout << "  - Bounds assertion passed" << std::endl;
    
    std::cout << "TestUtils assertions test passed!" << std::endl;
}

/**
 * @brief Example test demonstrating integration testing patterns
 * 
 * This test shows how to use the new test infrastructure for
 * integration testing of complex brain systems.
 */
void testIntegrationTestingExample() {
    std::cout << "Testing integration patterns..." << std::endl;
    
    // Integration Test 1: Basic brain lifecycle
    std::cout << "  Integration Test 1: Basic brain lifecycle" << std::endl;
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(150, 2);
    
    // Test 1.1: Initial state
    assert(brain->getTotalNeuronCount() == 150);
    assert(brain->getRegionCount() == 2);
    
    // Test 1.2: Simulation steps
    for (nlm::SimulationStep step = 0; step < 10; ++step) {
        brain->step(step);
    }
    assert(brain->getTotalSpikeCount() >= 0);
    
    // Test 1.3: Reset and re-initialize
    brain->reset();
    brain->initialize();
    assert(brain->getTotalNeuronCount() == 150);
    
    std::cout << "    - Basic brain lifecycle test passed" << std::endl;
    
    // Integration Test 2: Brain with different plasticity mechanisms
    std::cout << "  Integration Test 2: Plasticity mechanisms" << std::endl;
    auto plasticityBrain = nlm::test::BrainFactory::createRealisticBrain(3, 100);
    
    // Enable all plasticity mechanisms
    auto* stdp = plasticityBrain->getSTDP();
    auto* hebbian = plasticityBrain->getHebbian();
    stdp->configure(0.025f, 0.027f, 30.0f);
    hebbian->configure(0.018f, 0.022f);
    
    // Run learning simulation
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        plasticityBrain->step(step);
        plasticityBrain->updatePlasticity();
        
        if (step % 50 == 0) {
            // Simulate some external input
            plasticityBrain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 3.0f);
        }
    }
    
    assert(plasticityBrain->getTotalSpikeCount() > 0);
    std::cout << "    - Plasticity mechanisms test passed" << std::endl;
    
    // Integration Test 3: Neuromodulation and learning
    std::cout << "  Integration Test 3: Neuromodulation and learning" << std::endl;
    auto neuromodBrain = nlm::test::BrainFactory::createRealisticBrain(2, 80);
    
    // Apply neuromodulatory signals
    for (nlm::SimulationStep step = 0; step < 300; ++step) {
        neuromodBrain->step(step);
        
        if (step % 100 == 0) {
            nlm::DopamineSignal rewardSignal(0.6f, nlm::RewardType::Positive);
            neuromodBrain->applyNeuromodulation(rewardSignal);
        } else if (step % 150 == 0) {
            nlm::NoveltySignal noveltySignal(0.4f);
            neuromodBrain->applyNeuromodulation(noveltySignal);
        }
    }
    
    assert(neuromodBrain->getTotalSpikeCount() > 100);
    std::cout << "    - Neuromodulation and learning test passed" << std::endl;
    
    std::cout << "Integration testing example passed!" << std::endl;
}

/**
 * @brief Example test demonstrating benchmarking utilities
 * 
 * This test shows how to use the BenchmarkUtils for
 * performance measurement and benchmarking.
 */
void testBenchmarkingExample() {
    std::cout << "Testing benchmarking examples..." << std::endl;
    
    // Benchmark 1: Brain initialization
    std::cout << "  Benchmark 1: Brain initialization" << std::endl;
    auto initTime1 = nlm::test::BenchmarkUtils::timeBrainInitialization(500, 5);
    auto initTime2 = nlm::test::BenchmarkUtils::timeBrainInitialization(1000, 10);
    auto initTime3 = nlm::test::BenchmarkUtils::timeBrainInitialization(2000, 15);
    
    std::cout << "    - 500 neurons, 5 regions: " << initTime1 << " seconds" << std::endl;
    std::cout << "    - 1000 neurons, 10 regions: " << initTime2 << " seconds" << std::endl;
    std::cout << "    - 2000 neurons, 15 regions: " << initTime3 << " seconds" << std::endl;
    
    // Benchmark 2: Simulation steps
    std::cout << "  Benchmark 2: Simulation steps" << std::endl;
    auto brain = nlm::test::BrainFactory::createAndInitializeBrain(200, 3);
    auto stepTime1 = nlm::test::BenchmarkUtils::timeSimulationSteps(brain, 50);
    auto stepTime2 = nlm::test::BenchmarkUtils::timeSimulationSteps(brain, 100);
    
    std::cout << "    - 50 steps: " << stepTime1 << " seconds" << std::endl;
    std::cout << "    - 100 steps: " << stepTime2 << " seconds" << std::endl;
    
    std::cout << "Benchmarking example passed!" << std::endl;
}

/**
 * @brief Example test demonstrating stress testing
 * 
 * This test shows how to use the new test infrastructure for
 * stress testing and edge case testing.
 */
void testStressTestingExample() {
    std::cout << "Testing stress testing examples..." << std::endl;
    
    // Stress Test 1: Large network
    std::cout << "  Stress Test 1: Large network" << std::endl;
    auto largeBrain = nlm::test::BrainFactory::createRealisticBrain(10, 500);
    
    // Run large simulation
    for (nlm::SimulationStep step = 0; step < 1000; ++step) {
        largeBrain->step(step);
        if (step % 200 == 0) {
            // Periodically check for issues
            assert(largeBrain->getTotalNeuronCount() == 5000);
        }
    }
    std::cout << "    - Large network test passed" << std::endl;
    
    // Stress Test 2: Rapid state changes
    std::cout << "  Stress Test 2: Rapid state changes" << std::endl;
    auto rapidBrain = nlm::test::BrainFactory::createAndInitializeBrain(100, 2);
    
    // Rapid state changes
    for (nlm::SimulationStep step = 0; step < 500; ++step) {
        rapidBrain->step(step);
        rapidBrain->reset();
        rapidBrain->initialize();
    }
    std::cout << "    - Rapid state changes test passed" << std::endl;
    
    // Stress Test 3: Random seed reproducibility stress test
    std::cout << "  Stress Test 3: Random seed reproducibility" << std::endl;
    bool reproducible = nlm::test::RandomSeedManager::verifyReproducibility(12345);
    assert(reproducible);
    std::cout << "    - Random seed reproducibility test passed" << std::endl;
    
    std::cout << "Stress testing example passed!" << std::endl;
}

/**
 * @brief Main function for example test suite
 * 
 * This function runs all example tests to demonstrate
 * the capabilities of the new test infrastructure.
 */
int main() {
    std::cout << "NLM Test Infrastructure Examples" << std::endl;
    std::cout << "================================" << std::endl;
    
    // Run all example tests
    testBrainFactoryExamples();
    std::cout << std::endl;
    
    testTestUtilsAssertions();
    std::cout << std::endl;
    
    testIntegrationTestingExample();
    std::cout << std::endl;
    
    testBenchmarkingExample();
    std::cout << std::endl;
    
    testStressTestingExample();
    std::cout << std::endl;
    
    std::cout << "All example tests completed successfully!" << std::endl;
    
    return 0;
}

} // namespace test_examples

// Example test using the testing macros
TEST_BRAIN_CREATION(testBrainFactoryDemo, 100, 3)
TEST_BRAIN_STEP(testSimulationStepDemo, 200, 50)

// Example benchmark test
BENCHMARK_SIMPLE(testBenchmarkDemo, std::cout << "Running benchmark demo" << std::endl;)

// Simple test runner
class DemoTestRunner {
public:
    static void runAllTests() {
        std::cout << "Demo Test Runner" << std::endl;
        std::cout << "================" << std::endl;
        
        // Run macro-based tests
        testBrainFactoryDemo();
        testSimulationStepDemo();
        
        // Run example tests
        test_examples::testBrainFactoryExamples();
        test_examples::testTestUtilsAssertions();
        
        std::cout << std::endl;
        std::cout << "All demo tests completed successfully!" << std::endl;
    }
};

int main() {
    DemoTestRunner::runAllTests();
    return 0;
}

} // namespace nlm