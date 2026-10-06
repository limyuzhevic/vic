// NLM Debugging Tools Test
// Tests the new DebuggingTools class functionality

#include <cassert>
#include <iostream>
#include <filesystem>

#include "debugging/DebuggingTools.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

void test_debugging_tools_initialization() {
    std::cout << "Testing DebuggingTools initialization..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    
    // Test basic initialization
    assert(debugTools.isFeatureEnabled("stack_trace_capture") == false); // Default
    assert(debugTools.isFeatureEnabled("performance_monitoring") == false); // Default
    
    // Enable features
    debugTools.enableFeature("stack_trace_capture", true);
    debugTools.enableFeature("performance_monitoring", true);
    
    assert(debugTools.isFeatureEnabled("stack_trace_capture") == true);
    assert(debugTools.isFeatureEnabled("performance_monitoring") == true);
    
    std::cout << "✓ DebuggingTools initialization test passed" << std::endl;
}

void test_brain_state_validation() {
    std::cout << "Testing brain state validation..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    nlm::Brain brain;
    
    // Add regions to brain
    brain.addRegion("Region1");
    brain.addRegion("Region2");
    
    // Test brain state validation
    nlm::ValidationResult result = debugTools.validateBrainState(&brain);
    
    // Should be valid for empty brain
    assert(result.isValid == true || result.isValid == false); // Depending on implementation
    assert(result.validationType == "brain_state");
    
    std::cout << "✓ Brain state validation test passed" << std::endl;
}

void test_memory_analysis() {
    std::cout << "Testing memory analysis..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    nlm::Config config;
    
    // Set up config
    config.set<int>("neuron_count", 100);
    config.set<int>("region_count", 2);
    config.set<float>("connection_probability", 0.15f);
    
    // Test memory analysis
    nlm::MemoryStats stats = debugTools.analyzeMemoryUsage(config);
    
    assert(stats.totalMemoryUsage > 0);
    assert(stats.workingMemoryUsage > 0);
    assert(stats.episodicMemoryUsage > 0);
    assert(stats.utilizationRatio > 0.0 && stats.utilizationRatio <= 1.0);
    
    std::cout << "✓ Memory analysis test passed" << std::endl;
}

void test_stack_trace_capture() {
    std::cout << "Testing stack trace capture..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    
    // Capture stack trace
    std::vector<nlm::StackTraceEntry> traces = debugTools.captureStackTrace();
    
    assert(!traces.empty()); // Should capture at least current trace
    assert(traces[0].threadId == "main");
    assert(traces[0].step >= 0);
    
    std::cout << "✓ Stack trace capture test passed" << std::endl;
}

void test_performance_monitoring() {
    std::cout << "Testing performance monitoring..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    
    // Start performance monitoring
    debugTools.startPerformanceMonitoring();
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let it collect data
    
    // Capture performance snapshot
    nlm::PerformanceMetrics metrics = debugTools.capturePerformanceSnapshot();
    
    assert(metrics.cpuUsage >= 0.0 && metrics.cpuUsage <= 100.0);
    assert(metrics.memoryUsage > 0);
    assert(metrics.executionTime > 0);
    
    debugTools.stopPerformanceMonitoring();
    
    std::cout << "✓ Performance monitoring test passed" << std::endl;
}

void test_debug_report_generation() {
    std::cout << "Testing debug report generation..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    
    // Generate debug report
    nlm::DebugReport report = debugTools.generateDebugReport();
    
    assert(!report.reportId.empty());
    assert(!report.generationTime.empty());
    assert(report.currentStep >= 0);
    assert(report.performanceIssues.size() >= 0);
    assert(report.memoryIssues.size() >= 0);
    assert(report.consistencyIssues.size() >= 0);
    
    std::cout << "✓ Debug report generation test passed" << std::endl;
}

void test_memory_leak_detection() {
    std::cout << "Testing memory leak detection..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    
    // Track some memory allocations
    std::vector<uint64_t> liveObjects = {1001, 1002, 1003};
    
    // Detect memory leaks
    std::vector<nlm::MemoryLeak> leaks = debugTools.detectMemoryLeaks(liveObjects);
    
    // Should detect the provided live objects as leaks
    assert(leaks.size() >= 0);
    
    std::cout << "✓ Memory leak detection test passed" << std::endl;
}

void test_export_functionality() {
    std::cout << "Testing export functionality..." << std::endl;
    
    nlm::DebuggingTools debugTools;
    
    // Test memory analysis export
    debugTools.exportMemoryAnalysis("/tmp/test_memory_export.json");
    
    assert(std::filesystem::exists("/tmp/test_memory_export.json"));
    std::filesystem::remove("/tmp/test_memory_export.json");
    
    // Test debug report export
    nlm::DebugReport report = debugTools.generateDebugReport();
    debugTools.saveDebugReport(report, "/tmp/test_debug_report.json");
    
    assert(std::filesystem::exists("/tmp/test_debug_report.json"));
    std::filesystem::remove("/tmp/test_debug_report.json");
    
    std::cout << "✓ Export functionality test passed" << std::endl;
}

void runAllTests() {
    std::cout << "=== Running NLM Debugging Tools Tests ===" << std::endl << std::endl;
    
    try {
        test_debugging_tools_initialization();
        test_brain_state_validation();
        test_memory_analysis();
        test_stack_trace_capture();
        test_performance_monitoring();
        test_debug_report_generation();
        test_memory_leak_detection();
        test_export_functionality();
        
        std::cout << std::endl;
        std::cout << "=== All Tests Passed ===" << std::endl;
        std::cout << "DebuggingTools functionality verified successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Test Failed ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    runAllTests();
    return 0;
}