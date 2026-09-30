#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Logger/DebugUtils.hpp"
#include "../core/Logger/Monitor.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include <sstream>
#include <vector>

namespace nlm {

class LoggingTest {
public:
    LoggingTest() {
        std::cout << "=== Testing Enhanced Logging and Debugging Capabilities ===" << std::endl;
        testStructuredLogging();
        testPerformanceMonitoring();
        testDebuggingTools();
        testRealTimeMonitoring();
        testErrorRecovery();
        testVisualizationTools();
        testCorrelationTracking();
        std::cout << "=== All tests passed! ===" << std::endl;
    }
    
private:
    void testStructuredLogging() {
        std::cout << "\n--- Testing Structured Logging ---" << std::endl;
        
        // Test basic structured logging with correlation ID
        auto& logger = nlm::StructuredLogger::getInstance();
        std::string correlationId = logger.generateCorrelationId();
        
        LOG_CONTEXT("System", correlationId);
        LOG_TRACE("System initialization started", correlationId, {{ "component", "all_systems" }});
        LOG_DEBUG("Loading configuration files", correlationId, {{ "file_count", "5" }});
        LOG_INFO("Configuration loaded successfully", correlationId, {{ "neuron_count", "1000" }});
        LOG_WARNING("Some resources are near capacity", correlationId, {{ "resource", "memory" }});
        LOG_ERROR("Failed to load optional module", correlationId, {{ "module", "neural_enhancement" }});
        LOG_CRITICAL("System shutdown initiated", correlationId, {{ "reason", "critical_failure" }});
        
        // Test log categories
        LOG_TRACE("Neuronal activity detected", "Neural", correlationId);
        LOG_INFO("Simulation step completed", "Simulation", correlationId, {{ "step", "100" }});
        LOG_INFO("Memory allocation", "Memory", correlationId, {{ "size", "4096" }});
        LOG_INFO("Performance metric", "Performance", correlationId, {{ "fps", "60" }});
        
        // Verify logs were captured
        std::vector<nlm::StructuredLogEntry> logs = logger.getRecentLogs(20);
        std::cout << "Captured " << logs.size() << " structured log entries" << std::endl;
        
        assert(logs.size() >= 10); // At least 10 log entries should be captured
        
        // Verify correlation ID tracking
        std::vector<nlm::StructuredLogEntry> correlatedLogs;
        for (const auto& log : logs) {
            if (log.correlationId == correlationId) {
                correlatedLogs.push_back(log);
            }
        }
        
        assert(correlatedLogs.size() >= 6); // At least 6 logs should have our correlation ID
        
        std::cout << "✓ Structured logging test passed" << std::endl;
    }
    
    void testPerformanceMonitoring() {
        std::cout << "\n--- Testing Performance Monitoring ---" << std::endl;
        
        auto& logger = nlm::StructuredLogger::getInstance();
        
        // Test performance logging
        logger.logPerformance("model_inference", 150.5, "ms", "Performance", {{ "model", "deep_neural_net" }});
        logger.logPerformance("memory_allocation", 4096, "bytes", "Memory", {{ "allocation", "neuron_memory" }});
        logger.logPerformance("simulation_step", 0.016, "ms", "Simulation", {{ "step", "100" }});
        logger.logBenchmark("benchmark_test", 123.4, 50, "Benchmark");
        
        // Verify performance metrics were captured
        std::vector<nlm::PerformanceMetric> metrics = logger.getRecentMetrics(20);
        std::cout << "Captured " << metrics.size() << " performance metrics" << std::endl;
        
        assert(metrics.size() >= 4); // At least 4 performance metrics
        
        // Verify metric aggregation
        double totalTime = 0.0;
        for (const auto& metric : metrics) {
            if (metric.name.find("time") != std::string::npos || metric.name.find("step") != std::string::npos) {
                totalTime += metric.value;
            }
        }
        
        std::cout << "✓ Performance monitoring test passed" << std::endl;
    }
    
    void testDebuggingTools() {
        std::cout << "\n--- Testing Debugging Tools ---" << std::endl;
        
        auto& debugUtils = nlm::DebugUtils::getInstance();
        auto& logger = nlm::StructuredLogger::getInstance();
        
        // Test memory debugging
        std::string allocId = debugUtils.trackMemoryAllocation(0x1000, 1024, "neuron_creation");
        debugUtils.trackMemoryDeallocation(allocId);
        
        // Test CPU profiling
        debugUtils.debugProfileFunction([]() {
            // Simulate some work
            volatile double sum = 0.0;
            for (int i = 0; i < 1000000; ++i) {
                sum += std::sqrt(static_cast<double>(i));
            }
            return sum;
        }, "cpu_intensive_task", {{ "thread", "main" }});
        
        // Test network debugging
        debugUtils.debugLogNetwork("GET", "api.example.com", 45.2, 1024, 2048, {{ "user_id", "12345" }});
        
        // Test GUI debugging
        debugUtils.debugLogClick("button_submit", "submit_button", {{ "context", "form_validation" }});
        debugUtils.debugLogUpdate("neuron_visualization", "color_update", {{ "neuron_id", "neuron_42" }});
        
        // Test simulation debugging
        debugUtils.debugLogSimulationStep(100, 0.016, 450, 1000, {{ "active_rate", "0.45" }});
        
        // Verify debugging tools captured data
        std::vector<nlm::MemoryBlock> memoryBlocks = debugUtils.getActiveAllocations();
        assert(memoryBlocks.size() == 0); // All allocations should be deallocated
        
        std::vector<nlm::CPUProfileData> cpuProfiles = debugUtils.getRecentCPUProfiles(10);
        assert(!cpuProfiles.empty()); // CPU profiles should be captured
        
        std::cout << "✓ Debugging tools test passed" << std::endl;
    }
    
    void testRealTimeMonitoring() {
        std::cout << "\n--- Testing Real-Time Monitoring ---" << std::endl;
        
        auto& monitor = nlm::RealTimeMonitor::getInstance();
        
        // Start real-time monitoring
        monitor.startMonitoring(0.5);  // 0.5 second intervals
        
        // Wait for some data collection
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Get current system resources
        nlm::SystemResourceMetrics systemMetrics = monitor.getCurrentSystemResources();
        std::cout << "System resources - CPU: " << systemMetrics.cpuUsage << "%, Memory: " << systemMetrics.memoryUsage << "%" << std::endl;
        
        // Get simulation metrics (placeholder)
        nlm::SimulationMetrics simMetrics = monitor.getCurrentSimulationMetrics();
        std::cout << "Simulation metrics - Step: " << simMetrics.stepNumber << ", Neurons: " << simMetrics.totalNeurons << std::endl;
        
        // Get performance metrics
        nlm::PerformanceMetrics perfMetrics = monitor.getCurrentPerformance("test_metric");
        std::cout << "Performance metrics - Name: " << perfMetrics.name << ", Value: " << perfMetrics.value << " " << perfMetrics.unit << std::endl;
        
        // Stop monitoring
        monitor.stopMonitoring();
        
        std::cout << "✓ Real-time monitoring test passed" << std::endl;
    }
    
    void testErrorRecovery() {
        std::cout << "\n--- Testing Error Recovery ---" << std::endl;
        
        auto& debugUtils = nlm::DebugUtils::getInstance();
        
        // Test successful recovery
        auto result = debugUtils.attemptRecovery(
            []() { return 42; }, // This will succeed
            "test_operation",
            3, 1.0,
            {{ "operation", "test_success" }}
        );
        
        assert(result == 42);
        
        // Test failed recovery (will retry and fail)
        try {
            auto result2 = debugUtils.attemptRecovery(
                []() { throw std::runtime_error("Simulated failure"); },
                "test_failure",
                3, 0.1,
                {{ "operation", "test_failure" }}
            );
            assert(false); // Should not reach here
        } catch (const std::runtime_error& e) {
            assert(std::string(e.what()) == "Simulated failure");
        }
        
        // Log error context
        debugUtils.logErrorContext("database_connection", "Cannot connect to database",
                                   {{ "host", "localhost" }, { "port", "5432" }});
        
        std::cout << "✓ Error recovery test passed" << std::endl;
    }
    
    void testVisualizationTools() {
        std::cout << "\n--- Testing Visualization Tools ---" << std::endl;
        
        auto& debugUtils = nlm::DebugUtils::getInstance();
        
        // Create memory visualization
        std::string memoryViz = debugUtils.createMemoryVisualization("./debug_export/memory_report.html");
        std::cout << "Memory visualization saved to: " << memoryViz << std::endl;
        
        // Create performance visualization
        std::string perfViz = debugUtils.createPerformanceVisualization("./debug_export/performance_report.html");
        std::cout << "Performance visualization saved to: " << perfViz << std::endl;
        
        // Create trace visualization
        std::string traceViz = debugUtils.createTraceVisualization("./debug_export/trace_report.html");
        std::cout << "Trace visualization saved to: " << traceViz << std::endl;
        
        // Verify files were created
        assert(std::filesystem::exists("./debug_export/memory_report.html"));
        assert(std::filesystem::exists("./debug_export/performance_report.html"));
        assert(std::filesystem::exists("./debug_export/trace_report.html"));
        
        std::cout << "✓ Visualization tools test passed" << std::endl;
    }
    
    void testCorrelationTracking() {
        std::cout << "\n--- Testing Correlation Tracking ---" << std::endl;
        
        auto& logger = nlm::StructuredLogger::getInstance();
        
        // Start a correlation trace
        std::string correlationId = "test_correlation_123";
        logger.startCorrelationTrace(correlationId, "data_processing", {{ "source", "sensor_array_1" }});
        
        // Log several events with this correlation ID
        LOG_INFO("Data preprocessing started", "System", correlationId);
        LOG_INFO("Feature extraction completed", "Neural", correlationId, {{ "features", "128" }});
        LOG_INFO("Model inference completed", "Neural", correlationId, {{ "confidence", "0.95" }});
        LOG_INFO("Results saved", "System", correlationId, {{ "output_path", "/results/data.json" }});
        
        // End the correlation trace
        logger.endCorrelationTrace(correlationId, true, "All processing completed successfully");
        
        // Verify correlation tracking
        std::vector<nlm::StructuredLogEntry> correlatedLogs;
        for (const auto& log : logger.getRecentLogs(100)) {
            if (log.correlationId == correlationId) {
                correlatedLogs.push_back(log);
            }
        }
        
        assert(correlatedLogs.size() >= 4); // At least 4 logs with our correlation ID
        
        std::cout << "✓ Correlation tracking test passed" << std::endl;
    }
};

} // namespace nlm

int main() {
    try {
        nlm::LoggingTest test;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}