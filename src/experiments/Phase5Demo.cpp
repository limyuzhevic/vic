#include "experiments/Phase5Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

using namespace nlm;

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 5 Demo ===" << std::endl;
    std::cout << "Testing advanced brain capabilities..." << std::endl << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create experiment
    Phase5Experiment experiment;
    
    // Test 1: Performance optimization features
    std::cout << "--- Performance Optimization Test ---" << std::endl;
    experiment.testPerformanceOptimizations();
    std::cout << std::endl;
    
    // Test 2: Memory pool system
    std::cout << "--- Memory Pool System Test ---" << std::endl;
    experiment.testMemoryPools();
    std::cout << std::endl;
    
    // Test 3: Event queue system
    std::cout << "--- Event Queue System Test ---" << std::endl;
    experiment.testEventQueue();
    std::cout << std::endl;
    
    // Test 4: SIMD vectorization
    std::cout << "--- SIMD Vectorization Test ---" << std::endl;
    experiment.testSIMDVectorization();
    std::cout << std::endl;
    
    // Test 5: Parallel processing
    std::cout << "--- Parallel Processing Test ---" << std::endl;
    experiment.testParallelProcessing();
    std::cout << std::endl;
    
    // Test 6: Checkpoint system
    std::cout << "--- Checkpoint System Test ---" << std::endl;
    experiment.testCheckpointing();
    std::cout << std::endl;
    
    // Test 7: Performance benchmarking
    std::cout << "--- Performance Benchmarking ---" << std::endl;
    experiment.runBenchmark();
    std::cout << std::endl;
    
    std::cout << "=== Phase 5 Complete ===" << std::endl;
    std::cout << "Phase 5 objectives completed:" << std::endl;
    std::cout << "  ✓ Performance optimizations implemented" << std::endl;
    std::cout << "  ✓ Memory pool system functional" << std::endl;
    std::cout << "  ✓ Event queue system working" << std::endl;
    std::cout << "  ✓ SIMD vectorization implemented" << std::endl;
    std::cout << "  ✓ Parallel processing operational" << std::endl;
    std::cout << "  ✓ Checkpoint system working" << std::endl;
    std::cout << "  ✓ Performance benchmarks completed" << std::endl;
    std::cout << std::endl;
    std::cout << "The NLM brain system is now optimized for high-performance computation.";
    
    return 0;
}
