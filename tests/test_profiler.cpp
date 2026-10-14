// Performance Profiler Tests
#include "core/Performance/Profiler.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>

namespace test_profiler {

void testProfilerCreation() {
    std::cout << "    testProfilerCreation starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    
    // Test singleton pattern
    assert(&profiler == &nlm::PerformanceProfiler::getInstance());
    
    // Test initial state
    auto results = profiler.getResults();
    assert(results.empty());
    
    std::cout << "    testProfilerCreation passed" << std::endl;
}

void testBasicProfiling() {
    std::cout << "    testBasicProfiling starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.clear();
    profiler.setLevel(nlm::PerformanceLevel::Basic);
    
    // Start profiling
    profiler.startProfile("test_operation");
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // End profiling
    profiler.endProfile("test_operation");
    
    // Check results
    auto results = profiler.getResults();
    assert(results.size() == 1);
    assert(results[0].operationName == "test_operation");
    assert(results[0].callCount == 1);
    assert(results[0].duration.count() > 0.01); // Should be > 10ms
    
    std::cout << "    testBasicProfiling passed" << std::endl;
}

void testMultipleOperations() {
    std::cout << "    testMultipleOperations starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.clear();
    profiler.setLevel(nlm::PerformanceLevel::Basic);
    
    // Profile multiple operations
    profiler.startProfile("operation1");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    profiler.endProfile("operation1");
    
    profiler.startProfile("operation2");
    std::this_thread::sleep_for(std::chrono::milliseconds(15));
    profiler.endProfile("operation2");
    
    profiler.startProfile("operation1"); // Call same operation twice
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    profiler.endProfile("operation1");
    
    // Check results
    auto results = profiler.getResults();
    assert(results.size() == 2); // operation1 and operation2
    
    // Find operation1
    auto op1 = std::find_if(results.begin(), results.end(), 
                           [](const nlm::PerformanceResult& r) { return r.operationName == "operation1"; });
    assert(op1 != results.end());
    assert(op1->callCount == 2);
    
    // Find operation2
    auto op2 = std::find_if(results.begin(), results.end(), 
                           [](const nlm::PerformanceResult& r) { return r.operationName == "operation2"; });
    assert(op2 != results.end());
    assert(op2->callCount == 1);
    
    // Check that operation2 took longer
    assert(op2->averageDuration.count() > op1->averageDuration.count());
    
    std::cout << "    testMultipleOperations passed" << std::endl;
}

void testTimerClass() {
    std::cout << "    testTimerClass starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.clear();
    profiler.setLevel(nlm::PerformanceLevel::Basic);
    
    {
        // Timer goes out of scope
        nlm::Timer timer("timer_test");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Check that timer was recorded
    auto results = profiler.getResults();
    assert(results.size() == 1);
    assert(results[0].operationName == "timer_test");
    assert(results[0].callCount == 1);
    
    std::cout << "    testTimerClass passed" << std::endl;
}

void testProfileScope() {
    std::cout << "    testProfileScope starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.clear();
    profiler.setLevel(nlm::PerformanceLevel::Basic);
    
    {
        // ProfileScope goes out of scope
        PROFILE_SCOPE("scope_test");
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    // Check that profile was recorded
    auto results = profiler.getResults();
    assert(results.size() == 1);
    assert(results[0].operationName == "scope_test");
    assert(results[0].callCount == 1);
    
    std::cout << "    testProfileScope passed" << std::endl;
}

void testSaveToFile() {
    std::cout << "    testSaveToFile starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.clear();
    profiler.setLevel(nlm::PerformanceLevel::Basic);
    
    // Profile some operations
    profiler.startProfile("file_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    profiler.endProfile("file_test");
    
    // Save to file
    profiler.saveToFile("/tmp/profiler_results.txt");
    
    // Note: We can't easily verify file contents without reading it,
    // but we can verify the function doesn't crash
    std::cout << "    testSaveToFile passed" << std::endl;
}

void testProfilerLevel() {
    std::cout << "    testProfilerLevel starting..." << std::endl;
    
    nlm::PerformanceProfiler& profiler = nlm::PerformanceProfiler::getInstance();
    profiler.clear();
    
    // Set to Off level
    profiler.setLevel(nlm::PerformanceLevel::Off);
    
    // This should not be recorded
    profiler.startProfile("off_level_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    profiler.endProfile("off_level_test");
    
    auto results = profiler.getResults();
    assert(results.empty()); // No results because level is Off
    
    // Set to Basic level
    profiler.setLevel(nlm::PerformanceLevel::Basic);
    
    profiler.startProfile("basic_level_test");
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    profiler.endProfile("basic_level_test");
    
    results = profiler.getResults();
    assert(results.size() == 1); // Now results should be recorded
    assert(results[0].operationName == "basic_level_test");
    
    std::cout << "    testProfilerLevel passed" << std::endl;
}

void runAll() {
    testProfilerCreation();
    testBasicProfiling();
    testMultipleOperations();
    testTimerClass();
    testProfileScope();
    testSaveToFile();
    testProfilerLevel();
}

} // namespace test_profiler
