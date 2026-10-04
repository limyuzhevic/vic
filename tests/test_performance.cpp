// NLM Performance Monitoring Tests
// Tests for PerformanceMonitor, NeuralProfiler, MemoryProfiler, AlgorithmProfiler, BenchmarkRunner, and PerformanceAnalyzer

#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include "performance/PerformanceMonitoring.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

namespace test_performance_monitor {
    void runAll() {
        std::cout << "Running Performance Monitor tests..." << std::endl;
        
        // Create performance monitor
        auto monitor = nlm::createPerformanceMonitor();
        
        // Test initial state
        assert(!monitor->isRunning());
        assert(monitor->getStepCount() == 0);
        
        // Start monitoring
        monitor->start();
        assert(monitor->isRunning());
        
        // Record some steps
        monitor->recordStep(1.5, 100, 50);
        monitor->recordStep(2.3, 120, 60);
        monitor->setCurrentStep(10);
        
        // Test metrics collection
        auto metrics = monitor->getCurrentMetrics();
        assert(metrics.simulationTime > 0.0);
        assert(metrics.spikesPerSecond > 0);
        assert(metrics.activeNeurons > 0);
        
        // Test snapshot
        auto snapshot = monitor->getSnapshot();
        assert(snapshot.avgStepTimeMs > 0.0);
        
        // Test stability check
        assert(!monitor->isStable(10, 0.1)); // Not enough data for stable check
        
        // Reset
        monitor->reset();
        assert(monitor->getStepCount() == 0);
        
        // Stop monitoring
        monitor->stop();
        assert(!monitor->isRunning());
        
        std::cout << "  PASSED" << std::endl;
    }
}

namespace test_neural_profiler {
    void runAll() {
        std::cout << "Running Neural Profiler tests..." << std::endl;
        
        // Create neural profiler
        auto profiler = nlm::createNeuralProfiler();
        
        // Test initial state
        assert(!profiler->isRunning());
        
        // Start profiling
        profiler->startProfiling();
        assert(profiler->isRunning());
        
        // Create a mock brain for testing (simplified)
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        
        if (brain->initialize()) {
            // Record some neural activity
            for (int step = 0; step < 10; ++step) {
                // This would normally require actual neurons to test properly
                // For now, we'll just test the interface
                profiler->recordNeuralActivity(brain.get(), step);
            }
            
            // Get profile data
            auto profile = profiler->getProfileData();
            assert(profile.steps.size() > 0);
        }
        
        // Stop profiling
        profiler->stopProfiling();
        assert(!profiler->isRunning());
        
        std::cout << "  PASSED" << std::endl;
    }
}

namespace test_memory_profiler {
    void runAll() {
        std::cout << "Running Memory Profiler tests..." << std::endl;
        
        // Create memory profiler
        auto profiler = nlm::createMemoryProfiler();
        
        // Test initial state
        assert(!profiler->isRunning());
        assert(profiler->getPeakMemoryMB() == 0);
        
        // Start profiling
        profiler->startProfiling();
        assert(profiler->isRunning());
        
        // Record some memory samples
        for (int i = 0; i < 5; ++i) {
            profiler->record();
        }
        
        // Get memory profile
        auto profile = profiler->getMemoryProfile();
        assert(profile.peakMemoryMB > 0);
        assert(profile.averageMemoryMB > 0);
        
        // Stop profiling
        profiler->stopProfiling();
        assert(!profiler->isRunning());
        
        std::cout << "  PASSED" << std::endl;
    }
}

namespace test_algorithm_profiler {
    void runAll() {
        std::cout << "Running Algorithm Profiler tests..." << std::endl;
        
        // Create algorithm profiler
        auto profiler = nlm::createAlgorithmProfiler();
        
        // Test initial state
        assert(!profiler->isRunning());
        assert(profiler->getStepCount() == 0);
        
        // Start profiling
        profiler->start();
        assert(profiler->isRunning());
        
        // Record some algorithm performance
        for (int step = 0; step < 5; ++step) {
            profiler->recordStep(1.5 + step * 0.5, 100, 50, 120, 60);
        }
        
        // Get profile
        auto profile = profiler->getProfile();
        assert(profile.stepData.size() > 0);
        assert(profile.avgStepTime > 0.0);
        
        // Stop profiling
        profiler->stop();
        assert(!profiler->isRunning());
        
        std::cout << "  PASSED" << std::endl;
    }
}

namespace test_benchmark_runner {
    void runAll() {
        std::cout << "Running Benchmark Runner tests..." << std::endl;
        
        // Create benchmark runner
        auto runner = nlm::createBenchmarkRunner();
        
        // Test initial state
        assert(!runner->isRunning());
        
        // Start benchmarking
        runner->start();
        assert(runner->isRunning());
        
        // Create a simple brain for testing
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(10));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("simulation_timestep", 0.001);
        
        auto brain = std::make_shared<nlm::Brain>(config);
        
        if (brain->initialize()) {
            // Create benchmark steps
            std::vector<uint64_t> steps = {10, 20, 30};
            
            // Run benchmarks (simplified - just test interface)
            auto results = runner->runBenchmarks(brain, steps, "");
            // Results will be empty for this test, but interface is tested
        }
        
        // Stop benchmarking
        runner->stop();
        assert(!runner->isRunning());
        
        std::cout << "  PASSED" << std::endl;
    }
}

namespace test_performance_analyzer {
    void runAll() {
        std::cout << "Running Performance Analyzer tests..." << std::endl;
        
        // Create performance analyzer
        auto analyzer = nlm::createPerformanceAnalyzer();
        
        // Create mock performance metrics
        std::vector<nlm::PerformanceMetrics> metrics;
        
        // Add some test metrics
        nlm::PerformanceMetrics metric1;
        metric1.avgStepTimeMs = 1.5;
        metric1.spikesPerSecond = 100;
        metric1.totalMemoryMB = 500;
        metrics.push_back(metric1);
        
        nlm::PerformanceMetrics metric2;
        metric2.avgStepTimeMs = 1.6;
        metric2.spikesPerSecond = 110;
        metric2.totalMemoryMB = 520;
        metrics.push_back(metric2);
        
        // Analyze metrics
        auto analysis = analyzer->analyze(metrics);
        
        // Check analysis results
        assert(analysis.performanceTrend != 0.0); // Should have some trend
        assert(analysis.stabilityScore >= 0.0 && analysis.stabilityScore <= 1.0);
        
        // Create mock neural profiles for neural analysis
        std::vector<nlm::NeuralProfiler::NeuralProfile> neuralProfiles;
        
        // Add some test neural profiles
        nlm::NeuralProfiler::NeuralProfile neuralProfile1;
        neuralProfile1.totalSpikes = 100;
        neuralProfile1.uniqueActiveNeurons = 50;
        neuralProfile1.averageFiringRate = 0.5;
        neuralProfiles.push_back(neuralProfile1);
        
        // Analyze neural activity
        auto neuralAnalysis = analyzer->analyzeNeural(neuralProfiles);
        
        // Check neural analysis results
        assert(neuralAnalysis.avgFiringRate > 0.0);
        
        // Create mock memory profiles for memory analysis
        std::vector<nlm::MemoryProfiler::MemoryProfile> memoryProfiles;
        
        // Add some test memory profiles
        nlm::MemoryProfiler::MemoryProfile memoryProfile1;
        memoryProfile1.peakMemoryMB = 500;
        memoryProfile1.averageMemoryMB = 450;
        memoryProfile1.memoryGrowthRate = 10.0;
        memoryProfiles.push_back(memoryProfile1);
        
        // Analyze memory usage
        auto memoryAnalysis = analyzer->analyzeMemory(memoryProfiles);
        
        // Check memory analysis results
        assert(memoryAnalysis.peakMemoryMB > 0);
        assert(memoryAnalysis.averageMemoryMB > 0);
        
        std::cout << "  PASSED" << std::endl;
    }
}

int main() {
    std::cout << "=== NLM Performance Monitoring Tests ===" << std::endl;
    std::cout << "Testing Performance Monitoring and Profiling System:" << std::endl;
    std::cout << "  - Performance Monitoring" << std::endl;
    std::cout << "  - Neural Profiling" << std::endl;
    std::cout << "  - Memory Profiling" << std::endl;
    std::cout << "  - Algorithm Profiling" << std::endl;
    std::cout << "  - Benchmarking" << std::endl;
    std::cout << "  - Performance Analysis" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    try {
        test_performance_monitor::runAll();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        test_neural_profiler::runAll();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        test_memory_profiler::runAll();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        test_algorithm_profiler::runAll();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        test_benchmark_runner::runAll();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        test_performance_analyzer::runAll();
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Performance Monitoring Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Performance Monitoring Tests FAILED ===" << std::endl;
        return 1;
    }
}
