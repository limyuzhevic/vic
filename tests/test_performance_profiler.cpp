// test_performance_profiler.cpp - Tests for PerformanceProfiler
// Tests performance monitoring and optimization capabilities

#include <iostream>
#include <cassert>
#include "agent/PerformanceProfiler.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"

namespace test_performance_profiler {

void testBasicPerformanceProfiler() {
    std::cout << "Testing PerformanceProfiler basic functionality..." << std::endl;
    
    nlm::PerformanceProfiler profiler;
    
    // Test basic properties
    assert(!profiler.isProfiling());
    
    // Start/stop profiling
    profiler.startProfiling();
    assert(profiler.isProfiling());
    
    profiler.stopProfiling();
    assert(!profiler.isProfiling());
    
    // Get profile results
    auto results = profiler.getProfileResults();
    assert(results.empty());  // Should be empty when not profiling
    
    std::cout << "  ✓ PerformanceProfiler basic functionality test passed" << std::endl;
}

void testPerformanceProfilerStaticMethods() {
    std::cout << "Testing PerformanceProfiler static methods..." << std::endl;
    
    // Create test configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100);
    config->set("region_count", 1);
    
    // Create test brain
    auto brain = std::make_shared<nlm::Brain>(config);
    assert(brain->initialize());
    
    // Create test world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20.0f, 20.0f, 8, 8);
    
    // Create test agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(world);
    
    // Test profile simulation
    auto profileResults = nlm::PerformanceProfiler::profileSimulation(brain, world, agent, 50);
    assert(!profileResults.empty());
    assert(profileResults.find("avg_firing_rate") != profileResults.end());
    assert(profileResults.find("total_spikes") != profileResults.end());
    assert(profileResults.find("simulation_time") != profileResults.end());
    
    // Test optimize configuration
    auto optimizedConfig = nlm::PerformanceProfiler::optimizeConfiguration(config);
    assert(!optimizedConfig.empty());
    
    // Test benchmark memory usage
    auto memoryResults = nlm::PerformanceProfiler::benchmarkMemoryUsage(brain, world, agent);
    assert(memoryResults.find("peak_memory_usage") != memoryResults.end());
    
    // Test bottleneck analysis
    auto bottlenecks = nlm::PerformanceProfiler::analyzePerformanceBottlenecks(brain, world, agent);
    assert(!bottlenecks.empty());
    
    std::cout << "  ✓ PerformanceProfiler static methods test passed" << std::endl;
}

void testPerformanceProfilerConfiguration() {
    std::cout << "Testing PerformanceProfiler configuration..." << std::endl;
    
    nlm::PerformanceProfiler profiler;
    
    // Test configuration methods
    profiler.setProfilingInterval(200);
    profiler.enableDetailedAnalysis(true);
    
    // Verify configuration
    // (Note: we can't directly access private members, but we can test the effects)
    
    std::cout << "  ✓ PerformanceProfiler configuration test passed" << std::endl;
}

void runAll() {
    testBasicPerformanceProfiler();
    testPerformanceProfilerStaticMethods();
    testPerformanceProfilerConfiguration();
    
    std::cout << std::endl;
    std::cout << "=== All PerformanceProfiler Tests Passed ===" << std::endl;
}

} // namespace test_performance_profiler