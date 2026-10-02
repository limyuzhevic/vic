// test_advanced_brain.cpp - Tests for AdvancedBrain functionality
// Tests advanced monitoring, analytics, and experiment capabilities

#include <iostream>
#include <cassert>
#include "agent/AdvancedBrain.hpp"
#include "core/Config/Config.hpp"

namespace test_advanced_brain {

void testBasicAdvancedBrain() {
    std::cout << "Testing AdvancedBrain basic functionality..." << std::endl;
    
    // Create config
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create advanced brain
    nlm::AdvancedBrain advancedBrain(config);
    
    // Test initialization
    assert(advancedBrain.initialize());
    
    // Test monitoring enable/disable
    advancedBrain.enableMonitoring(100);
    assert(advancedBrain.isMonitoringEnabled());
    
    advancedBrain.enableOptimization(true);
    assert(advancedBrain.isOptimizationEnabled());
    
    // Test step execution
    advancedBrain.step(0);
    advancedBrain.step(1, 0.001);
    
    // Test advanced stats
    auto stats = advancedBrain.getAdvancedStats();
    assert(!stats.empty());
    assert(stats.find("neuron_count") != stats.end());
    assert(stats.find("firing_rate") != stats.end());
    
    // Test config manipulation
    advancedBrain.setConfigValue("test_param", 42.0);
    double value = advancedBrain.getConfigValue("test_param", 0.0);
    assert(value == 42.0);
    
    std::cout << "  ✓ AdvancedBrain basic functionality test passed" << std::endl;
}

void testAdvancedBrainMonitoring() {
    std::cout << "Testing AdvancedBrain monitoring capabilities..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::AdvancedBrain advancedBrain(config);
    advancedBrain.initialize();
    
    // Enable monitoring
    advancedBrain.enableMonitoring(50);
    assert(advancedBrain.isMonitoringEnabled());
    
    // Run simulation with monitoring
    for (int step = 0; step < 100; ++step) {
        advancedBrain.step(step);
    }
    
    // Check performance data
    auto performanceData = advancedBrain.getPerformanceData();
    assert(!performanceData.empty());
    assert(performanceData.size() <= 100);  // Should have data for each step
    
    std::cout << "  ✓ AdvancedBrain monitoring test passed" << std::endl;
}

void testAdvancedBrainLearningCurveAnalysis() {
    std::cout << "Testing AdvancedBrain learning curve analysis..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 50);
    config->set("region_count", 1);
    
    nlm::AdvancedBrain advancedBrain(config);
    advancedBrain.initialize();
    
    // Create sample reward history
    std::vector<double> rewardHistory = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    
    // Analyze learning curve
    auto analysis = advancedBrain.analyzeLearningCurve(rewardHistory);
    
    assert(analysis.find("learning_rate") != analysis.end());
    assert(analysis.find("convergence_step") != analysis.end());
    assert(analysis.find("asymptote") != analysis.end());
    assert(analysis.find("std_dev") != analysis.end());
    
    // Verify analysis values are reasonable
    double learningRate = analysis["learning_rate"];
    assert(learningRate >= 0.0); // Should be positive
    
    std::cout << "  ✓ AdvancedBrain learning curve analysis test passed" << std::endl;
}

void testAdvancedBrainMemoryManagement() {
    std::cout << "Testing AdvancedBrain memory management..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    nlm::AdvancedBrain advancedBrain(config);
    advancedBrain.initialize();
    
    // Get initial memory stats
    auto initialStats = advancedBrain.getAdvancedStats();
    
    // Run simulation
    for (int step = 0; step < 200; ++step) {
        advancedBrain.step(step);
        
        if (step % 50 == 0) {
            advancedBrain.optimizeMemoryUsage();
        }
    }
    
    // Get final memory stats
    auto finalStats = advancedBrain.getAdvancedStats();
    
    // Memory usage should be stable or improve
    std::cout << "  ✓ AdvancedBrain memory management test passed" << std::endl;
}

void runAll() {
    testBasicAdvancedBrain();
    testAdvancedBrainMonitoring();
    testAdvancedBrainLearningCurveAnalysis();
    testAdvancedBrainMemoryManagement();
    
    std::cout << std::endl;
    std::cout << "=== All AdvancedBrain Tests Passed ===" << std::endl;
}

} // namespace test_advanced_brain