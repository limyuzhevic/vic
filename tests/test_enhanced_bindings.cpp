// TestEnhancedBindings.cpp
// Comprehensive tests for enhanced Python bindings functionality
// Tests the new classes and methods added in the enhancement phase

#include "EnhancedBrain.hpp"
#include "EnhancedAgentBrain.hpp"
#include "SimulationRunner.hpp"
#include "MemoryManager.hpp"
#include "PerformanceProfiler.hpp"
#include "NeuralNetworkUtils.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

namespace test_enhanced_bindings {

void testEnhancedBrainClass() {
    std::cout << "    Testing EnhancedBrain class..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    
    EnhancedBrain brain(config);
    
    assert(brain.initialize());
    assert(brain.getTotalNeuronCount() == 500);
    assert(brain.getRegionCount() == 2);
    
    // Test memory system access
    assert(brain.getWorkingMemory() != nullptr);
    assert(brain.getEpisodicMemory() != nullptr);
    assert(brain.getAssociativeMemory() != nullptr);
    
    // Test enhanced statistics
    auto stats = brain.getFiringStatistics();
    assert(!stats.empty());
    
    // Test connectivity analysis
    std::string analysis = brain.analyzeConnectivity();
    assert(analysis.find("Total synapses") != std::string::npos);
    assert(analysis.find("Connectivity Analysis") != std::string::npos);
    
    std::cout << "    EnhancedBrain tests passed" << std::endl;
}

void testEnhancedAgentBrainClass() {
    std::cout << "    Testing EnhancedAgentBrain class..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    
    EnhancedBrain brain(config);
    EnhancedAgentBrain agent(&brain);
    
    // Test enabling all features
    agent.enableAllFeatures(true);
    
    // Test advanced stats
    auto stats = agent.getAdvancedStats();
    assert(stats.find("neuromodulation") != nullptr);
    assert(stats.find("curiosity") != nullptr);
    assert(stats.find("enabled_features") != nullptr);
    
    // Test developmental stage
    auto stage = agent.getDevelopmentalStage();
    assert(stage == DevelopmentalStage::Initial || 
           stage == DevelopmentalStage::CriticalPeriod ||
           stage == DevelopmentalStage::Maturation ||
           stage == DevelopmentalStage::Adult ||
           stage == DevelopmentalStage::Aging);
    
    std::cout << "    EnhancedAgentBrain tests passed" << std::endl;
}

void testSimulationRunnerClass() {
    std::cout << "    Testing SimulationRunner class..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    
    EnhancedBrain brain(config);
    SimpleWorld world;
    EnhancedAgentBrain agent(&brain);
    
    // Test single episode
    SimulationRunner::SimulationMetrics metrics;
    metrics = SimulationRunner::runEpisode(brain, world, agent, 100, false);
    assert(metrics.steps == 100);
    assert(!metrics.firingRateHistory.empty());
    assert(!metrics.rewardHistory.empty());
    assert(!metrics.neuromodulationHistory.empty());
    
    // Test multiple episodes
    auto allMetrics = SimulationRunner::runMultipleEpisodes(
        brain, world, agent, 3, 50, false);
    assert(allMetrics.size() == 3);
    
    std::cout << "    SimulationRunner tests passed" << std::endl;
}

void testMemoryManagerClass() {
    std::cout << "    Testing MemoryManager class..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 400);
    
    EnhancedBrain brain(config);
    
    // Test memory optimization
    MemoryManager::optimizeMemory(&brain, 0.5f);
    
    // Test memory usage
    size_t usage = MemoryManager::getMemoryUsage(&brain);
    assert(usage >= 0);
    
    // Test clearing memory
    MemoryManager::clearAllMemory(&brain);
    
    std::cout << "    MemoryManager tests passed" << std::endl;
}

void testPerformanceProfilerClass() {
    std::cout << "    Testing PerformanceProfiler class..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 250);
    
    EnhancedBrain brain(config);
    
    // Start profiling
    PerformanceProfiler::PerformanceStats stats;
    stats = PerformanceProfiler::startProfiling();
    
    // Run some steps
    for (int i = 0; i < 10; ++i) {
        brain.step(i);
        PerformanceProfiler::recordStep(stats, &brain);
    }
    
    // Generate report
    std::string report = PerformanceProfiler::generateReport(stats);
    assert(report.find("NLM Performance Report") != std::string::npos);
    assert(report.find("Steps completed: 10") != std::string::npos);
    assert(report.find("Average step time") != std::string::npos);
    
    std::cout << "    PerformanceProfiler tests passed" << std::endl;
}

void testNeuralNetworkUtilsClass() {
    std::cout << "    Testing NeuralNetworkUtils class..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 350);
    config->set("region_count", 4);
    
    EnhancedBrain brain(config);
    
    // Test firing statistics
    auto stats = NeuralNetworkUtils::computeFiringStatistics(&brain);
    assert(!stats.empty());
    for (float stat : stats) {
        assert(!std::isnan(stat));
        assert(std::isfinite(stat));
    }
    
    // Test connectivity analysis
    std::string analysis = NeuralNetworkUtils::analyzeConnectivity(&brain);
    assert(analysis.find("Connectivity Analysis") != std::string::npos);
    
    std::cout << "    NeuralNetworkUtils tests passed" << std::endl;
}

void testContextManager() {
    std::cout << "    Testing context manager..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 150);
    
    EnhancedBrain brain(config);
    
    // Test brain context manager
    {
        EnhancedBrainContext context(&brain);
        assert(context.getBrain() == &brain);
        context.step(0);
        // Context should automatically reset brain when exiting
    }
    
    std::cout << "    Context manager tests passed" << std::endl;
}

void testConfigurationBuilder() {
    std::cout << "    Testing configuration builder..." << std::endl;
    
    // Test creating standard config
    auto standardConfig = ConfigBuilder::createBrainConfig();
    assert(standardConfig != nullptr);
    assert(standardConfig->getOr<int64_t>("neuron_count", 0) == 1000);
    
    // Test creating advanced config
    auto advancedConfig = ConfigBuilder::createAdvancedConfig();
    assert(advancedConfig != nullptr);
    
    // Test custom parameters
    std::map<std::string, std::string> customParams = {
        {"neuron_count", "2000"},
        {"float_synaptic_rate", "0.05"},
        {"bool_enable_plasticity", "true"},
        {"weight_initial_mean", "0.6"}
    };
    
    auto customConfig = ConfigBuilder::createAdvancedConfig("CustomBrain", customParams);
    assert(customConfig != nullptr);
    
    std::cout << "    Configuration builder tests passed" << std::endl;
}

void testLegacyCompatibility() {
    std::cout << "    Testing legacy compatibility..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 100);
    
    // Test legacy functions
    EnhancedBrain brain(config);
    assert(brain.initialize());
    
    EnhancedAgentBrain agent(&brain);
    assert(agent.getBrain() == &brain);
    
    SimpleWorld world;
    assert(world.getWidth() > 0);
    assert(world.getHeight() > 0);
    
    std::cout << "    Legacy compatibility tests passed" << std::endl;
}

void testThreadSafety() {
    std::cout << "    Testing thread safety..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 200);
    
    EnhancedBrain brain(config);
    
    // Create multiple threads accessing the brain
    std::vector<std::thread> threads;
    std::atomic<int> stepsCompleted{0};
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 20; ++j) {
                brain.step(j);
                stepsCompleted.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    assert(stepsCompleted.load() == 100); // 5 threads * 20 steps
    
    std::cout << "    Thread safety tests passed" << std::endl;
}

void testPerformanceBenchmarks() {
    std::cout << "    Testing performance benchmarks..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    config->set("region_count", 6);
    
    EnhancedBrain brain(config);
    
    // Performance test - simulate realistic workload
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int step = 0; step < 200; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Should complete within reasonable time
    double elapsedSeconds = elapsed.count() / 1000.0;
    assert(elapsedSeconds < 30.0, "Performance test too slow: " + std::to_string(elapsedSeconds) + "s");
    
    std::cout << "    Performance benchmarks passed (" << std::fixed << std::setprecision(2) 
              << elapsedSeconds << "s)" << std::endl;
}

void runAllEnhancedBindingTests() {
    std::cout << "=== Enhanced Python Bindings Tests ===" << std::endl;
    
    bool allPassed = true;
    
    try {
        testEnhancedBrainClass();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testEnhancedAgentBrainClass();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testSimulationRunnerClass();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testMemoryManagerClass();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPerformanceProfilerClass();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testNeuralNetworkUtilsClass();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testContextManager();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testConfigurationBuilder();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testLegacyCompatibility();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testThreadSafety();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        testPerformanceBenchmarks();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << std::endl;
        std::cout << "All enhanced binding tests PASSED!" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "Some enhanced binding tests FAILED!" << std::endl;
        throw std::runtime_error("Enhanced binding tests failed");
    }
}

} // namespace test_enhanced_bindings
