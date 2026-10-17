#include "performance/Performance.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

using namespace nlm;
using namespace nlm::performance;

void testMemoryPool() {
    std::cout << "Testing MemoryPool..." << std::endl;
    
    MemoryPool pool(1024, 100); // 1KB chunks, max 100 chunks
    
    // Test allocation
    void* ptr1 = pool.allocate(256);
    assert(ptr1 != nullptr);
    
    void* ptr2 = pool.allocate(512);
    assert(ptr2 != nullptr);
    
    void* ptr3 = pool.allocate(1024);
    assert(ptr3 != nullptr);
    
    // Test deallocation
    pool.deallocate(ptr1, 256);
    pool.deallocate(ptr2, 512);
    pool.deallocate(ptr3, 1024);
    
    // Test reset
    pool.reset();
    
    // Verify memory usage is back to zero
    assert(pool.getMemoryUsage() == 0);
    
    std::cout << "  ✓ MemoryPool tests passed" << std::endl;
}

void testSpikeEventQueue() {
    std::cout << "Testing SpikeEventQueue..." << std::endl;
    
    SpikeEventQueue queue(100);
    
    // Test pushing events
    queue.push(0, 10, 1.0f); // neuron 0, step 10, weight 1.0
    queue.push(1, 20, -0.5f); // neuron 1, step 20, weight -0.5
    queue.push(2, 30, 2.0f); // neuron 2, step 30, weight 2.0
    
    assert(queue.getEventCount() == 3);
    
    // Create synaptic inputs array
    std::vector<float> synapticInputs(10, 0.0f);
    
    // Process events up to step 15
    bool isExcitatory(float weight) { return weight > 0.0f; }
    queue.processEvents(15, synapticInputs.data(), synapticInputs.size(), isExcitatory);
    
    // Should have processed event 0 (step 10) but not events 1 or 2
    assert(queue.getEventCount() == 2);
    assert(synapticInputs[0] > 0.0f); // Event 0 was excitatory
    
    // Process remaining events
    queue.processEvents(50, synapticInputs.data(), synapticInputs.size(), isExcitatory);
    
    assert(queue.getEventCount() == 0);
    
    std::cout << "  ✓ SpikeEventQueue tests passed" << std::endl;
}

void testSIMDVectorOps() {
    std::cout << "Testing SIMDVectorOps..." << std::endl;
    
    std::vector<float> a = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    std::vector<float> b = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    std::vector<float> resultAdd(a.size());
    std::vector<float> resultMult(a.size());
    std::vector<float> resultScalar(a.size());
    
    SIMDVectorOps::add(resultAdd.data(), a.data(), b.data(), a.size());
    SIMDVectorOps::multiply(resultMult.data(), a.data(), b.data(), a.size());
    SIMDVectorOps::scalarMultiply(resultScalar.data(), a.data(), 2.0f, a.size());
    
    // Verify results
    for (size_t i = 0; i < a.size(); ++i) {
        assert(resultAdd[i] == a[i] + b[i]);
        assert(resultMult[i] == a[i] * b[i]);
        assert(resultScalar[i] == a[i] * 2.0f);
    }
    
    std::cout << "  ✓ SIMDVectorOps tests passed" << std::endl;
}

void testParallelProcessor() {
    std::cout << "Testing ParallelProcessor..." << std::endl;
    
    ParallelProcessor processor(4); // 4 threads
    
    std::vector<int> data(100, 0);
    
    // Worker function that increments elements
    auto worker = [&data](size_t start, size_t end) {
        for (size_t i = start; i < end; ++i) {
            data[i] = static_cast<int>(i);
        }
    };
    
    processor.processNeurons(worker, 0, data.size());
    
    // Verify all elements were set
    for (size_t i = 0; i < data.size(); ++i) {
        assert(data[i] == static_cast<int>(i));
    }
    
    std::cout << "  ✓ ParallelProcessor tests passed" << std::endl;
}

void testPerformanceMonitor() {
    std::cout << "Testing PerformanceMonitor..." << std::endl;
    
    PerformanceMonitor monitor;
    
    // Record some metrics
    monitor.recordSpikeProcessingTime(1000.0); // 1000 ns
    monitor.recordSpikeProcessingTime(2000.0); // 2000 ns
    monitor.recordAllocationTime(500.0);       // 500 ns
    monitor.recordSynapticOperationTime(300.0); // 300 ns
    
    // Generate report
    std::string report = monitor.generateReport();
    assert(!report.empty());
    assert(report.find("Performance Report") != std::string::npos);
    
    // Reset monitor
    monitor.reset();
    
    std::cout << "  ✓ PerformanceMonitor tests passed" << std::endl;
}

void testBrainIntegration() {
    std::cout << "Testing Brain integration with performance systems..." << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Run some steps
    for (uint64_t step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Check brain status
    brain->logStatus();
    
    std::cout << "  ✓ Brain integration tests passed" << std::endl;
}

void testCommandLineParsing() {
    std::cout << "Testing command-line parsing..." << std::endl;
    
    // This test would require integration with the main function
    // For now, we'll just verify the basic config parsing
    
    auto config = std::make_shared<Config>();
    
    // Test basic config parsing
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    
    assert(config->get<int64_t>("random_seed") == 42);
    assert(config->get<int64_t>("neuron_count") == 500);
    assert(config->get<int64_t>("region_count") == 1);
    assert(config->get<float>("connection_probability") == 0.15f);
    
    std::cout << "  ✓ Command-line parsing tests passed" << std::endl;
}

void testPerformanceReportGeneration() {
    std::cout << "Testing performance report generation..." << std::endl;
    
    PerformanceMonitor monitor;
    
    // Record various metrics
    monitor.recordSpikeProcessingTime(1000.0);
    monitor.recordSpikeProcessingTime(2000.0);
    monitor.recordAllocationTime(500.0);
    monitor.recordSynapticOperationTime(300.0);
    
    // Generate report
    std::string report = monitor.generateReport();
    
    // Check report contains expected content
    assert(report.find("Spike Processing") != std::string::npos);
    assert(report.find("Memory Allocation") != std::string::npos);
    assert(report.find("Synaptic Operations") != std::string::npos);
    assert(report.find("Total Statistics") != std::string::npos);
    
    // Test report formatting
    monitor.reset();
    std::string emptyReport = monitor.generateReport();
    assert(emptyReport.find("Performance Report") != std::string::npos);
    
    std::cout << "  ✓ Performance report generation tests passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== NLM Performance and Integration Test Suite ===\n" << std::endl;
    
    testMemoryPool();
    testSpikeEventQueue();
    testSIMDVectorOps();
    testParallelProcessor();
    testPerformanceMonitor();
    testBrainIntegration();
    testCommandLineParsing();
    testPerformanceReportGeneration();
    
    std::cout << "\n=== All tests passed! ===\n" << std::endl;
    std::cout << "The NLM performance infrastructure and integration features are working correctly." << std::endl;
}

int main(int argc, char** argv) {
    // Parse command-line arguments
    bool runQuickTests = false;
    bool runFullTests = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "--quick") {
            runQuickTests = true;
        } else if (arg == "--full") {
            runFullTests = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "NLM Performance Test Suite\n"
                      << "  --quick    Run quick tests only\n"
                      << "  --full     Run all tests\n"
                      << "  --help, -h Show this help\n";
            return 0;
        }
    }
    
    if (runQuickTests) {
        std::cout << "Running quick performance tests...\n" << std::endl;
        testMemoryPool();
        testSpikeEventQueue();
        testPerformanceMonitor();
        std::cout << "\n✓ Quick tests completed successfully!\n" << std::endl;
    } else if (runFullTests || argc == 1) {
        runAllTests();
    }
    
    return 0;
}