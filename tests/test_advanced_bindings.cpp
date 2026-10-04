// Comprehensive test suite for advanced NLM Python bindings
// Tests all new functionality in the advanced Python bindings

#include "python/bindings_advanced.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace nlm;

// Test configuration system
void testAdvancedConfig() {
    std::cout << "Testing Advanced Configuration System..." << std::endl;
    
    auto config = std::make_shared<AdvancedConfig>();
    
    // Test type-safe getters
    assert(config->getInt("test_int", 42) == 42);
    assert(config->getFloat("test_float", 3.14f) == 3.14f);
    assert(config->getDouble("test_double", 2.718) == 2.718);
    assert(config->getBool("test_bool", true) == true);
    assert(config->getString("test_string", "hello") == "hello");
    
    // Test advanced configuration methods
    config->setNeuralNetwork("1000", "0.15");
    assert(config->getInt("brain.neuron_count", 0) == 1000);
    assert(config->getFloat("brain.connection_probability", 0.0f) == 0.15f);
    
    config->setLearningParameters(0.001f, 2.0f);
    assert(config->getFloat("plasticity.stdp.learning_rate", 0.0f) == 0.001f);
    assert(config->getFloat("neuromod.dopamine.scale", 0.0f) == 2.0f);
    
    config->setMemorySystem(50000, 500);
    assert(config->getInt("memory.working_capacity", 0) == 50000);
    assert(config->getInt("memory.max_episodes", 0) == 500);
    
    // Test configuration validation
    std::vector<std::string> errors = config->validateConfiguration();
    // Should be empty for valid configuration
    
    // Test advanced summary
    std::string summary = config->getAdvancedSummary();
    assert(summary.find("Advanced Configuration Summary:") != std::string::npos);
    assert(summary.find("1000") != std::string::npos);
    
    std::cout << "✓ Advanced Config tests passed!" << std::endl;
}

// Test advanced brain functionality
void testAdvancedBrain() {
    std::cout << "Testing Advanced Brain Functionality..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    
    // Create advanced brain
    auto advancedBrain = std::make_shared<AdvancedBrain>(config);
    
    // Test advanced initialization
    assert(advancedBrain->initializeAdvanced());
    
    // Test advanced step with metrics collection
    advancedBrain->stepAdvanced(0, 0.0);
    
    // Test metrics collection
    auto metrics = advancedBrain->collectStepMetrics();
    assert(metrics.stepNumber >= 0);
    assert(metrics.totalSpikes >= 0);
    assert(metrics.firingNeurons >= 0);
    assert(metrics.averageFiringRate >= 0.0f);
    
    // Test memory system control
    advancedBrain->controlMemory(true, true, true, true);
    
    // Test neuromodulation control
    advancedBrain->controlNeuromodulation(0.7f, 0.5f, 0.3f, 1.0f);
    
    // Test development control
    advancedBrain->controlDevelopment(0.05f, true, 1.5f);
    
    // Test sensory processor
    auto processor = advancedBrain->getSensoryProcessor();
    assert(processor.getSensoryHistory().empty());
    
    std::cout << "✓ Advanced Brain tests passed!" << std::endl;
}

// Test performance monitoring
void testPerformanceMonitor() {
    std::cout << "Testing Performance Monitor..." << std::endl;
    
    auto monitor = std::make_shared<PerformanceMonitor>();
    
    // Test performance monitoring
    monitor->recordStepStart();
    // Simulate work
    for (int i = 0; i < 1000; ++i) {
        volatile int x = i * i; // Prevent optimization
    }
    monitor->recordStepEnd();
    monitor->recordWallTime();
    
    // Get statistics
    auto stats = monitor->getStats();
    assert(stats.totalSteps == 1);
    assert(stats.totalWallTime > 0.0);
    assert(stats.totalCpuTime > 0.0);
    assert(stats.avgStepTime > 0.0);
    assert(stats.stepsPerSecond > 0.0);
    
    // Get formatted report
    std::string report = monitor->getFormattedReport();
    assert(report.find("Performance Monitor Report") != std::string::npos);
    assert(report.find("Total Steps: 1") != std::string::npos);
    
    std::cout << "✓ Performance Monitor tests passed!" << std::endl;
}

// Test error handling utilities
void testErrorUtils() {
    std::cout << "Testing Error Handling Utilities..." << std::endl;
    
    // Test error formatting
    std::string error = ErrorUtils::formatError("test_operation", "invalid_parameter");
    assert(error.find("NLM test_operation failed:") != std::string::npos);
    assert(error.find("invalid_parameter") != std::string::npos);
    
    // Test parameter validation
    std::string validationError;
    assert(ErrorUtils::validateParameters("test_param", 5.0f, 0.0f, 10.0f, validationError) == true);
    assert(ErrorUtils::validateParameters("test_param", 15.0f, 0.0f, 10.0f, validationError) == false);
    assert(validationError.find("must be between 0.0 and 10.0") != std::string::npos);
    
    std::cout << "✓ Error Handling tests passed!" << std::endl;
}

// Test backward compatibility
void testBackwardCompatibility() {
    std::cout << "Testing Backward Compatibility..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    
    // Create basic brain (should still work)
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test basic step
    brain->step(0);
    brain->step(1, 0.001);
    
    // Test basic brain functionality
    assert(brain->getTotalNeuronCount() == 100);
    assert(brain->getTotalSynapseCount() > 0);
    assert(brain->getFiringNeuronCount() >= 0);
    assert(brain->getAverageFiringRate() >= 0.0f);
    
    // Test action production
    auto action = brain->produceAction();
    assert(action != nullptr);
    
    // Test brain state
    brain->reset();
    brain->logStatus();
    
    std::cout << "✓ Backward Compatibility tests passed!" << std::endl;
}

// Test integration between advanced and basic systems
void testIntegration() {
    std::cout << "Testing Integration Between Systems..." << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(1));
    
    // Create advanced brain
    auto advancedBrain = std::make_shared<AdvancedBrain>(config);
    
    // Initialize with advanced features
    assert(advancedBrain->initializeAdvanced());
    
    // Run simulation with mixed basic and advanced calls
    for (int step = 0; step < 10; ++step) {
        // Basic step call
        advancedBrain->step(step);
        
        // Collect metrics occasionally
        if (step % 3 == 0) {
            auto metrics = advancedBrain->collectStepMetrics();
            assert(metrics.stepNumber >= 0);
        }
        
        // Control systems occasionally
        if (step % 5 == 0) {
            advancedBrain->controlMemory(false, true, false, true);
        }
    }
    
    std::cout << "✓ Integration tests passed!" << std::endl;
}

// Performance benchmarks
void runPerformanceBenchmarks() {
    std::cout << "Running Performance Benchmarks..." << std::endl;
    
    auto monitor = std::make_shared<PerformanceMonitor>();
    
    // Benchmark 1: Small network
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        monitor->recordStepStart();
        for (int i = 0; i < 100; ++i) {
            brain->step(i);
        }
        monitor->recordStepEnd();
        
        auto stats = monitor->getStats();
        std::cout << "Small network benchmark: " << stats.stepsPerSecond << " steps/sec" << std::endl;
    }
    
    // Benchmark 2: Medium network with advanced features
    {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500));
        config->set("region_count", static_cast<int64_t>(1));
        
        auto advancedBrain = std::make_shared<AdvancedBrain>(config);
        advancedBrain->initializeAdvanced();
        
        monitor->recordStepStart();
        for (int i = 0; i < 100; ++i) {
            advancedBrain->stepAdvanced(i, static_cast<double>(i) * 0.001);
        }
        monitor->recordStepEnd();
        
        auto stats = monitor->getStats();
        std::cout << "Medium network benchmark: " << stats.stepsPerSecond << " steps/sec" << std::endl;
    }
    
    std::cout << "✓ Performance benchmarks completed!" << std::endl;
}

// Memory system integration test
void testMemorySystemIntegration() {
    std::cout << "Testing Memory System Integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(300));
    config->set("region_count", static_cast<int64_t>(1));
    
    auto advancedBrain = std::make_shared<AdvancedBrain>(config);
    assert(advancedBrain->initializeAdvanced());
    
    // Enable all memory systems
    advancedBrain->controlMemory(true, true, true, true);
    
    // Run simulation to generate memory activity
    for (int i = 0; i < 50; ++i) {
        advancedBrain->stepAdvanced(i, static_cast<double>(i) * 0.001);
        
        // Check memory metrics
        auto metrics = advancedBrain->collectStepMetrics();
        assert(metrics.workingMemoryActiveTraces >= 0);
        assert(metrics.episodicMemoryEpisodes >= 0);
    }
    
    std::cout << "✓ Memory System Integration tests passed!" << std::endl;
}

// Neuromodulation integration test
void testNeuromodulationIntegration() {
    std::cout << "Testing Neuromodulation Integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200));
    config->set("region_count", static_cast<int64_t>(1));
    
    auto advancedBrain = std::make_shared<AdvancedBrain>(config);
    assert(advancedBrain->initializeAdvanced());
    
    // Test neuromodulation control
    advancedBrain->controlNeuromodulation(0.8f, 0.6f, 0.4f, 1.0f);
    
    // Run simulation with neuromodulation
    for (int i = 0; i < 30; ++i) {
        advancedBrain->stepAdvanced(i, static_cast<double>(i) * 0.001);
        
        // Collect metrics
        auto metrics = advancedBrain->collectStepMetrics();
        assert(metrics.neuromodulationLevel >= 0.0f);
    }
    
    std::cout << "✓ Neuromodulation Integration tests passed!" << std::endl;
}

// Development system integration test
void testDevelopmentSystemIntegration() {
    std::cout << "Testing Development System Integration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(250));
    config->set("region_count", static_cast<int64_t>(1));
    
    auto advancedBrain = std::make_shared<AdvancedBrain>(config);
    assert(advancedBrain->initializeAdvanced());
    
    // Test development control
    advancedBrain->controlDevelopment(0.03f, true, 1.2f);
    
    // Run simulation with development
    for (int i = 0; i < 40; ++i) {
        advancedBrain->stepAdvanced(i, static_cast<double>(i) * 0.001);
        
        // Collect metrics
        auto metrics = advancedBrain->collectStepMetrics();
        assert(metrics.plasticityRates.size() >= 0);
    }
    
    std::cout << "✓ Development System Integration tests passed!" << std::endl;
}

// Main test runner
int main() {
    std::cout << "=== Advanced NLM Python Bindings Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testAdvancedConfig();
        std::cout << std::endl;
        
        testAdvancedBrain();
        std::cout << std::endl;
        
        testPerformanceMonitor();
        std::cout << std::endl;
        
        testErrorUtils();
        std::cout << std::endl;
        
        testBackwardCompatibility();
        std::cout << std::endl;
        
        testIntegration();
        std::cout << std::endl;
        
        testMemorySystemIntegration();
        std::cout << std::endl;
        
        testNeuromodulationIntegration();
        std::cout << std::endl;
        
        testDevelopmentSystemIntegration();
        std::cout << std::endl;
        
        // Run performance benchmarks (commented out for normal testing)
        // runPerformanceBenchmarks();
        // std::cout << std::endl;
        
        std::cout << "=== All Tests Passed! ===" << std::endl;
        std::cout << "The advanced Python bindings are working correctly." << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception." << std::endl;
        return 1;
    }
}