// Comprehensive Integration Test
// Tests all the new advanced features and improvements

#include "src/core/AdvancedConfig.hpp"
#include "src/performance/PerformanceMonitor.hpp"
#include "src/tools/AdvancedTools.hpp"
#include "src/testing/AdvancedTestFramework.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>

namespace nlm {

class ComprehensiveIntegrationTest {
public:
    struct TestResult {
        std::string testName;
        bool passed;
        double executionTime;
        std::string errorMessage;
        
        TestResult(const std::string& name, bool pass, double time = 0.0, const std::string& error = "")
            : testName(name), passed(pass), executionTime(time), errorMessage(error) {}
    };

    ComprehensiveIntegrationTest() : advancedConfigTest("Advanced Config Test"),
                                    performanceMonitorTest("Performance Monitor Test"),
                                    visualizationTest("Visualization Test"),
                                    debuggerTest("Debugger Test") {}
    
    ~ComprehensiveIntegrationTest() = default;
    
    std::vector<TestResult> runAllTests() {
        std::vector<TestResult> results;
        
        std::cout << "=== NLM Comprehensive Integration Test ===" << std::endl;
        
        // Test Advanced Configuration System
        results.push_back(runAdvancedConfigTest());
        
        // Test Performance Monitoring System
        results.push_back(runPerformanceMonitorTest());
        
        // Test Visualization System
        results.push_back(runVisualizationTest());
        
        // Test Debugger System
        results.push_back(runDebuggerTest());
        
        // Test Plugin System
        results.push_back(runPluginTest());
        
        // Test Tool Integration
        results.push_back(runToolIntegrationTest());
        
        // Print summary
        printTestSummary(results);
        
        return results;
    }

private:
    // Test Components
    AdvancedConfig advancedConfigTest;
    PerformanceMonitor performanceMonitorTest;
    VisualizationSystem visualizationTest;
    AdvancedDebugger debuggerTest;
    
    // Test Results
    std::vector<TestResult> integrationResults;

    TestResult runAdvancedConfigTest() {
        auto start = std::chrono::steady_clock::now();
        
        bool success = true;
        std::string error;
        
        try {
            // Test schema creation
            advancedConfigTest.addSchema("test_key", "Test key", "default_value");
            
            // Test setting values
            if (!advancedConfigTest.set("test_key", "test_value")) {
                error = "Failed to set configuration value";
                success = false;
            }
            
            // Test getting values
            auto value = advancedConfigTest.get<std::string>("test_key");
            if (!value || *value != "test_value") {
                error = "Configuration value not retrieved correctly";
                success = false;
            }
            
            // Test validation
            if (!advancedConfigTest.isValid()) {
                error = "Configuration validation failed";
                success = false;
            }
            
            // Test summary generation
            std::string summary = advancedConfigTest.summary();
            if (summary.empty()) {
                error = "Failed to generate configuration summary";
                success = false;
            }
            
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
            success = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (success) {
            std::cout << "✓ Advanced Configuration Test PASSED (" << elapsed << "s)" << std::endl;
        } else {
            std::cout << "✗ Advanced Configuration Test FAILED: " << error << std::endl;
        }
        
        return TestResult("Advanced Configuration Test", success, elapsed, error);
    }

    TestResult runPerformanceMonitorTest() {
        auto start = std::chrono::steady_clock::now();
        
        bool success = true;
        std::string error;
        
        try {
            // Test performance monitor creation
            PerformanceMonitor monitor("TestMonitor");
            
            // Test sampling
            monitor.start();
            monitor.sample(PerformanceMonitor::MetricType::Time, 1.5, "test_sample");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            monitor.stop();
            
            // Test counter creation
            auto* counter = monitor.createCounter("test_counter", PerformanceMonitor::MetricType::Accuracy);
            if (!counter) {
                error = "Failed to create performance counter";
                success = false;
            }
            
            counter->increment(10);
            double value = counter->get();
            if (value != 10.0) {
                error = "Counter value incorrect: " + std::to_string(value);
                success = false;
            }
            
            // Test statistics
            auto stats = monitor.getStats(PerformanceMonitor::MetricType::Time);
            if (stats.sampleCount == 0) {
                error = "Failed to get performance statistics";
                success = false;
            }
            
            // Test report generation
            std::string report = monitor.getReport();
            if (report.empty()) {
                error = "Failed to generate performance report";
                success = false;
            }
            
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
            success = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (success) {
            std::cout << "✓ Performance Monitor Test PASSED (" << elapsed << "s)" << std::endl;
        } else {
            std::cout << "✗ Performance Monitor Test FAILED: " << error << std::endl;
        }
        
        return TestResult("Performance Monitor Test", success, elapsed, error);
    }

    TestResult runVisualizationTest() {
        auto start = std::chrono::steady_clock::now();
        
        bool success = true;
        std::string error;
        
        try {
            // Test visualization system creation
            VisualizationSystem visualizer;
            
            // Test visualization creation
            VisualizationSystem::VisualizationConfig config("test_vis", VisualizationSystem::ElementType::TimeSeries);
            std::string visId = visualizer.createVisualization(config);
            
            if (visId.empty()) {
                error = "Failed to create visualization";
                success = false;
            }
            
            // Test data addition
            VisualizationSystem::DataPoint dataPoint(1.0, 42.5, "test_point");
            visualizer.addData(visId, dataPoint);
            
            // Test data retrieval
            auto data = visualizer.getData(visId);
            if (data.empty()) {
                error = "Failed to retrieve visualization data";
                success = false;
            } else if (data[0].value != 42.5) {
                error = "Visualization data incorrect";
                success = false;
            }
            
            // Test report generation
            std::string report = visualizer.generateReport();
            if (report.empty()) {
                error = "Failed to generate visualization report";
                success = false;
            }
            
            // Test visualization removal
            if (!visualizer.removeVisualization(visId)) {
                error = "Failed to remove visualization";
                success = false;
            }
            
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
            success = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (success) {
            std::cout << "✓ Visualization Test PASSED (" << elapsed << "s)" << std::endl;
        } else {
            std::cout << "✗ Visualization Test FAILED: " << error << std::endl;
        }
        
        return TestResult("Visualization Test", success, elapsed, error);
    }

    TestResult runDebuggerTest() {
        auto start = std::chrono::steady_clock::now();
        
        bool success = true;
        std::string error;
        
        try {
            // Test debugger creation
            AdvancedDebugger debugger;
            
            // Test breakpoint creation
            AdvancedDebugger::Breakpoint bp("test_bp", AdvancedDebugger::BreakpointType::Step, "test_function");
            std::string bpId = debugger.addBreakpoint(bp);
            
            if (bpId.empty()) {
                error = "Failed to create breakpoint";
                success = false;
            }
            
            // Test breakpoint listing
            auto breakpoints = debugger.getBreakpoints();
            if (breakpoints.empty()) {
                error = "Failed to list breakpoints";
                success = false;
            }
            
            // Test breakpoint enabling
            if (!debugger.enableBreakpoint(bpId, true)) {
                error = "Failed to enable breakpoint";
                success = false;
            }
            
            // Test breakpoint removal
            if (!debugger.removeBreakpoint(bpId)) {
                error = "Failed to remove breakpoint";
                success = false;
            }
            
            // Test memory snapshot
            std::string memSnapId = debugger.takeMemorySnapshot();
            if (memSnapId.empty()) {
                error = "Failed to take memory snapshot";
                success = false;
            }
            
            // Test performance snapshot
            std::string perfSnapId = debugger.takePerformanceSnapshot();
            if (perfSnapId.empty()) {
                error = "Failed to take performance snapshot";
                success = false;
            }
            
            // Test debug report generation
            std::string debugReport = debugger.generateDebugReport();
            if (debugReport.empty()) {
                error = "Failed to generate debug report";
                success = false;
            }
            
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
            success = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (success) {
            std::cout << "✓ Debugger Test PASSED (" << elapsed << "s)" << std::endl;
        } else {
            std::cout << "✗ Debugger Test FAILED: " << error << std::endl;
        }
        
        return TestResult("Debugger Test", success, elapsed, error);
    }

    TestResult runPluginTest() {
        auto start = std::chrono::steady_clock::now();
        
        bool success = true;
        std::string error;
        
        try {
            // Test plugin manager creation
            PluginManager pluginManager;
            
            // Test plugin directory setup
            pluginManager.setPluginDirectory("./plugins");
            
            // Test scanning for plugins
            std::vector<std::string> plugins = pluginManager.scanPlugins("./plugins");
            if (plugins.empty()) {
                // This is okay - no plugins in the directory
            }
            
            // Test plugin loading (if any)
            for (const auto& pluginName : plugins) {
                if (!pluginManager.loadPlugin(pluginName)) {
                    error = "Failed to load plugin: " + pluginName;
                    success = false;
                    break;
                }
            }
            
            // Test plugin information
            std::string pluginInfo = pluginManager.getAllPluginsInfo();
            if (pluginInfo.empty()) {
                error = "Failed to get plugin information";
                success = false;
            }
            
            // Test initialization
            std::unordered_map<std::string, std::unordered_map<std::string, std::string>> pluginConfigs;
            if (!pluginManager.initializeAll(pluginConfigs)) {
                error = "Failed to initialize plugins";
                success = false;
            }
            
            // Test plugin actions
            for (const auto& pluginName : pluginManager.getLoadedPlugins()) {
                if (!pluginManager.executePluginAction(pluginName, "help", std::vector<std::string>{"help"})) {
                    error = "Failed to execute plugin action for: " + pluginName;
                    success = false;
                    break;
                }
            }
            
            // Test cleanup
            pluginManager.shutdownAll();
            
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
            success = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (success) {
            std::cout << "✓ Plugin Test PASSED (" << elapsed << "s)" << std::endl;
        } else {
            std::cout << "✗ Plugin Test FAILED: " << error << std::endl;
        }
        
        return TestResult("Plugin Test", success, elapsed, error);
    }

    TestResult runToolIntegrationTest() {
        auto start = std::chrono::steady_clock::now();
        
        bool success = true;
        std::string error;
        
        try {
            // Test integration of all tools
            VisualizationTool visualizerTool;
            DebuggerTool debuggerTool;
            PerformanceTestTool performanceTool;
            
            // Test tool command parsing
            std::vector<std::string> testArgs = {"generate", "test_input.txt", "output.html", "spike_rate"};
            if (!visualizerTool.parseArguments(testArgs.size(), &testArgs[0])) {
                error = "Failed to parse visualization tool arguments";
                success = false;
            }
            
            std::vector<std::string> debugArgs = {"breakpoint", "add", "bp1", "step", "test_function"};
            if (!debuggerTool.parseArguments(debugArgs.size(), &debugArgs[0])) {
                error = "Failed to parse debugger tool arguments";
                success = false;
            }
            
            // Test tool execution
            if (!visualizerTool.executeCommand()) {
                error = "Failed to execute visualization tool command";
                success = false;
            }
            
            if (!debuggerTool.executeCommand()) {
                error = "Failed to execute debugger tool command";
                success = false;
            }
            
            // Test performance tool execution
            if (!performanceTool.runAllTests()) {
                error = "Failed to run performance tool tests";
                success = false;
            }
            
        } catch (const std::exception& e) {
            error = std::string("Exception: ") + e.what();
            success = false;
        }
        
        auto end = std::chrono::steady_clock::now();
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (success) {
            std::cout << "✓ Tool Integration Test PASSED (" << elapsed << "s)" << std::endl;
        } else {
            std::cout << "✗ Tool Integration Test FAILED: " << error << std::endl;
        }
        
        return TestResult("Tool Integration Test", success, elapsed, error);
    }

    void printTestSummary(const std::vector<TestResult>& results) {
        std::cout << std::endl << "=== Test Summary ===" << std::endl;
        
        size_t totalTests = results.size();
        size_t passedTests = 0;
        double totalExecutionTime = 0.0;
        
        for (const auto& result : results) {
            if (result.passed) {
                passedTests++;
            }
            totalExecutionTime += result.executionTime;
            
            std::cout << result.testName << ": " 
                     << (result.passed ? "PASSED" : "FAILED")
                     << " (" << result.executionTime << "s)";
            if (!result.errorMessage.empty()) {
                std::cout << " - " << result.errorMessage;
            }
            std::cout << std::endl;
        }
        
        std::cout << std::endl << "=== Summary ===" << std::endl;
        std::cout << "Total Tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << "Success Rate: " << (passedTests * 100.0 / totalTests) << "%" << std::endl;
        std::cout << "Total Execution Time: " << totalExecutionTime << "s" << std::endl;
    }
};

} // namespace nlm

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Comprehensive Integration Test ===" << std::endl;
    std::cout << "Testing all advanced NLM features and improvements..." << std::endl;
    std::cout << std::endl;
    
    nlm::ComprehensiveIntegrationTest integrationTest;
    
    auto results = integrationTest.runAllTests();
    
    // Check if all tests passed
    bool allPassed = true;
    for (const auto& result : results) {
        if (!result.passed) {
            allPassed = false;
            break;
        }
    }
    
    if (allPassed) {
        std::cout << std::endl << "=== All Tests Passed! ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "=== Some Tests Failed ===" << std::endl;
        return 1;
    }
}
