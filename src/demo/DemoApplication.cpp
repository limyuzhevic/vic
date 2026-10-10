#pragma once

#include "DebugInfrastructure.hpp"
#include "DiagnosticTools.hpp"
#include "MonitoringInfrastructure.hpp"
#include "VisualizationInfrastructure.hpp"
#include "Logger.hpp"
#include <thread>
#include <chrono>
#include <random>

namespace nlm {

// Demo class to showcase debugging and profiling features
class DemoApplication {
public:
    DemoApplication() {
        // Initialize debug logging
        DebugLogger::getGlobal().setLevel(DebugLevel::DEBUG);
        DebugLogger::getGlobal().enable(true);
        
        // Initialize diagnostic tools
        SystemDiagnoser::getGlobal().enableLogging(true);
        
        // Initialize monitoring
        RealTimeMonitor::getGlobal();
        HistoricalMonitor::getGlobal();
        AlertManager::getGlobal();
        ReportGenerator::getGlobal();
        TrendAnalyzer::getGlobal();
        
        // Initialize visualizations
        DebugVisualizer::getGlobal();
        PerformanceVisualizer::getGlobal();
        MemoryVisualizer::getGlobal();
        ThreadVisualizer::getGlobal();
        NetworkVisualizer::getGlobal();
        
        // Set up alert rules
        AlertManager& alertManager = AlertManager::getGlobal();
        alertManager.addAlertRule("high_cpu", "system", "cpu_usage > 80.0", "high", "High CPU usage detected");
        alertManager.addAlertRule("low_memory", "system", "memory_usage > 90.0", "critical", "Low memory available");
        
        // Add monitoring targets
        RealTimeMonitor& monitor = RealTimeMonitor::getGlobal();
        monitor.addMonitoringTarget("system", "system", {});
        monitor.addMonitoringTarget("performance", "performance", {});
        monitor.addMonitoringTarget("memory", "memory", {});
        
        std::cout << "Demo Application initialized successfully" << std::endl;
    }
    
    void runDemo() {
        std::cout << "\n=== NLM Debugging and Profiling Demo ===\n" << std::endl;
        
        // Start monitoring
        RealTimeMonitor::getGlobal().start();
        
        // Run various demo scenarios
        runDebugDemo();
        runProfilingDemo();
        runDiagnosticDemo();
        runMonitoringDemo();
        runVisualizationDemo();
        
        // Stop monitoring
        RealTimeMonitor::getGlobal().stop();
        
        std::cout << "\n=== Demo Complete ===\n" << std::endl;
    }
    
private:
    void runDebugDemo() {
        std::cout << "1. Debug Features Demo" << std::endl;
        std::cout << "------------------------" << std::endl;
        
        DebugLogger& debugLogger = DebugLogger::getGlobal();
        DebugSession& debugSession = DebugSession::getGlobal();
        
        // Log messages with different levels
        debugLogger.debug("Starting debug demo");
        debugLogger.info("Debug session initialized");
        debugLogger.warning("This is a warning message");
        debugLogger.error("This is an error message");
        
        // Add some metadata
        debugLogger.addMetadata("demo", "true");
        debugLogger.addMetadata("session_id", "demo_123");
        
        // Log with metadata
        debugLogger.info("Debug message with metadata");
        
        // Set up a breakpoint
        BreakpointManager& bpManager = BreakpointManager::getGlobal();
        int bpId = bpManager.addBreakpoint("DemoApp.cpp", 42, "cpu_usage > 80.0", DebugLevel::ERROR);
        std::cout << "Added breakpoint " << bpId << " at DemoApp.cpp:42" << std::endl;
        
        // Inspect variables
        VariableInspector& varInspector = VariableInspector::getGlobal();
        varInspector.registerVariable("cpu_usage", "float", "85.5", "0x7fff12345678");
        varInspector.registerVariable("memory_usage", "float", "92.3", "0x7fff12345670");
        varInspector.registerVariable("thread_count", "int", "4", "0x7fff12345660");
        
        std::cout << "Registered variables for inspection" << std::endl;
        
        // Capture call stack
        debugSession.captureCallStack();
        std::cout << "Captured call stack" << std::endl;
        
        // Start/stop memory tracking
        debugSession.startMemoryTracking();
        std::cout << "Memory tracking started" << std::endl;
        
        // Simulate some memory allocations
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            debugLogger.info("Processing item " + std::to_string(i));
        }
        
        debugSession.stopMemoryTracking();
        std::cout << "Memory tracking stopped" << std::endl;
        
        // Generate debug report
        DebugVisualizationData debugData;
        debugData.sessionId = "debug_demo";
        debugData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(5);
        debugData.endTime = std::chrono::system_clock::now();
        
        // Add some sample debug entries
        DebugEntry entry;
        entry.level = DebugLevel::INFO;
        entry.message = "Sample debug entry";
        entry.file = "DemoApp.cpp";
        entry.line = 100;
        entry.function = "demoFunction";
        entry.timestamp = std::chrono::system_clock::now();
        entry.threadId = "thread_1";
        entry.metadata["demo"] = "true";
        
        debugData.debugEntries.push_back(entry);
        
        std::string debugReport = DebugVisualizer::getGlobal().generateDebugReport(debugData);
        std::cout << "Debug report generated (first 500 chars): " << debugReport.substr(0, 500) << "..." << std::endl;
        
        std::cout << std::endl;
    }
    
    void runProfilingDemo() {
        std::cout << "2. Profiling Features Demo" << std::endl;
        std::cout << "--------------------------" << std::endl;
        
        PerformanceProfiler& perfProfiler = PerformanceProfiler::getGlobal();
        MemoryProfiler& memProfiler = MemoryProfiler::getGlobal();n        CPUProfiler& cpuProfiler = CPUProfiler::getGlobal();
        I/OProfiler& ioProfiler = I/OProfiler::getGlobal();
        NetworkProfiler& netProfiler = NetworkProfiler::getGlobal();
        AlgorithmProfiler& algProfiler = AlgorithmProfiler::getGlobal();
        
        // Start performance profiling
        perfProfiler.startProfiling("database_query", "database");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        double queryTime = perfProfiler.stopProfiling("database_query");
        std::cout << "Database query took " << queryTime << " microseconds" << std::endl;
        
        // Start algorithm profiling
        algProfiler.startAlgorithmProfiling("sort_algorithm", "algorithms");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        double sortTime = algProfiler.stopAlgorithmProfiling("sort_algorithm");
        std::cout << "Sort algorithm took " << sortTime << " seconds" << std::endl;
        
        // Start memory tracking
        memProfiler.enableTracking(true);
        void* ptr1 = malloc(1024); // Allocate 1KB
        void* ptr2 = malloc(2048); // Allocate 2KB
        memProfiler.trackAllocation(ptr1, 1024, "buffer", "DemoApp.cpp", 100);
        memProfiler.trackAllocation(ptr2, 2048, "buffer", "DemoApp.cpp", 105);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        memProfiler.trackDeallocation(ptr1);
        free(ptr1);
        memProfiler.trackDeallocation(ptr2);
        free(ptr2);
        
        // Check for memory leaks
        std::vector<void*> leaks = memProfiler.getMemoryLeaks();
        std::cout << "Memory leaks detected: " << leaks.size() << std::endl;
        
        // Get memory stats
        auto stats = memProfiler.getMemoryStats();
        std::cout << "Memory stats - Total allocated: " << stats.totalAllocated 
                  << " bytes, Current usage: " << stats.currentUsage << " bytes" << std::endl;
        
        // I/O profiling
        ioProfiler.trackRead("data.txt", 4096, 0.005);
        ioProfiler.trackWrite("output.txt", 2048, 0.003);
        ioProfiler.trackOpen("data.txt", "r");
        ioProfiler.trackClose("output.txt");
        
        // Network profiling
        netProfiler.trackConnect("api.example.com", 443, 0.1);
        netProfiler.trackSend("api.example.com", 443, 1024, 0.05);
        netProfiler.trackReceive("api.example.com", 443, 2048, 0.08);
        
        // Get network stats
        auto netStats = netProfiler.getNetworkStats();
        std::cout << "Network stats - Bytes sent: " << netStats.totalBytesSent 
                  << ", Bytes received: " << netStats.totalBytesReceived << std::endl;
        
        std::cout << std::endl;
    }
    
    void runDiagnosticDemo() {
        std::cout << "3. Diagnostic Features Demo" << std::endl;
        std::cout << "--------------------------" << std::endl;
        
        SystemDiagnoser& sysDiagnoser = SystemDiagnoser::getGlobal();
        ComponentDiagnoser& compDiagnoser = ComponentDiagnoser::getGlobal();
        ConfigurationDiagnoser& configDiagnoser = ConfigurationDiagnoser::getGlobal();
        DependencyDiagnoser& depDiagnoser = DependencyDiagnoser::getGlobal();
        ErrorDiagnoser& errDiagnoser = ErrorDiagnoser::getGlobal();
        PerformanceDiagnoser& perfDiagnoser = PerformanceDiagnoser::getGlobal();
        
        // Register components
        compDiagnoser.registerComponent("brain", "1.0.0", {"neuromodulation", "memory"});
        compDiagnoser.registerComponent("memory_system", "1.0.0", {"brain"});
        compDiagnoser.registerComponent("neuromodulation", "1.0.0", {});
        
        // Add configuration sources
        configDiagnoser.addConfigurationSource("default", {
            {"neuron_count", "100"},
            {"region_count", "10"},
            {"connection_probability", "0.5"}
        });
        
        // Add dependencies
        depDiagnoser.addDependency("external_lib", "2.1.0", "/usr/lib/libexternal.so");
        depDiagnoser.addDependency("graphics_lib", "3.0.0", "/usr/lib/libgraphics.so");
        
        // Log some errors for analysis
        errDiagnoser.logError("MEMORY_LEAK", "Memory leak detected at address 0x12345678");
        errDiagnoser.logError("CONNECTION_ERROR", "Failed to connect to database");
        errDiagnoser.logError("MEMORY_LEAK", "Memory leak detected at address 0x87654321");
        
        // Log performance metrics
        PerformanceInfo perf1;
        perf1.operationName = "brain_step";
        perf1.duration = 0.5;
        perf1.memoryUsed = 1024;
        perf1.cpuUsage = 45.0;
        perf1.threadId = "thread_1";
        perf1.timestamp = std::chrono::system_clock::now();
        perf1.metrics["success_rate"] = 0.95;
        
        PerformanceInfo perf2;
        perf2.operationName = "brain_step";
        perf2.duration = 1.2;
        perf2.memoryUsed = 2048;
        perf2.cpuUsage = 85.0;
        perf2.threadId = "thread_2";
        perf2.timestamp = std::chrono::system_clock::now();
        perf2.metrics["success_rate"] = 0.80;
        
        perfDiagnoser.logPerformance(perf1);
        perfDiagnoser.logPerformance(perf2);
        
        // Check for performance issues
        bool hasBottleneck = perfDiagnoser.detectPerformanceBottleneck("brain_step", 1.0);
        std::cout << "Performance bottleneck detected: " << (hasBottleneck ? "Yes" : "No") << std::endl;
        
        std::vector<std::string> issues = perfDiagnoser.getPerformanceIssues();
        std::cout << "Performance issues:" << std::endl;
        for (const auto& issue : issues) {
            std::cout << "  - " << issue << std::endl;
        }
        
        // Run system diagnostics
        bool sysHealthy = sysDiagnoser.checkSystemHealth();
        std::cout << "System health: " << (sysHealthy ? "Healthy" : "Unhealthy") << std::endl;
        
        std::vector<std::string> sysIssues = sysDiagnoser.getSystemHealthIssues();
        std::cout << "System issues:" << std::endl;
        for (const auto& issue : sysIssues) {
            std::cout << "  - " << issue << std::endl;
        }
        
        // Run component diagnostics
        bool compHealthy = compDiagnoser.checkComponentHealth("brain");
        std::cout << "Brain component health: " << (compHealthy ? "Healthy" : "Unhealthy") << std::endl;
        
        // Run configuration validation
        bool configValid = configDiagnoser.validateConfiguration("default");
        std::cout << "Configuration valid: " << (configValid ? "Yes" : "No") << std::endl;
        
        std::cout << std::endl;
    }
    
    void runMonitoringDemo() {
        std::cout << "4. Monitoring Features Demo" << std::endl;
        std::cout << "---------------------------" << std::endl;
        
        HistoricalMonitor& histMonitor = HistoricalMonitor::getGlobal();
        AlertManager& alertManager = AlertManager::getGlobal();
        ReportGenerator& reportGen = ReportGenerator::getGlobal();
        TrendAnalyzer& trendAnalyzer = TrendAnalyzer::getGlobal();
        
        // Generate some monitoring data
        MonitoringData sysData;
        sysData.id = "sys_1";
        sysData.type = "system";
        sysData.name = "system_monitor";
        sysData.timestamp = std::chrono::system_clock::now();
        sysData.metrics["cpu_usage"] = 25.0;
        sysData.metrics["memory_usage"] = 45.0;
        sysData.metrics["response_time"] = 50.0;
        sysData.status = "normal";
        sysData.severity = 0.1;
        sysData.description = "System monitoring data";
        
        MonitoringData perfData;
        perfData.id = "perf_1";
        perfData.type = "performance";
        perfData.name = "performance_monitor";
        perfData.timestamp = std::chrono::system_clock::now();
        perfData.metrics["db_query_time"] = 200.0;
        perfData.metrics["cpu_usage"] = 75.0;
        perfData.metrics["memory_usage"] = 85.0;
        perfData.status = "warning";
        perfData.severity = 0.7;
        perfData.description = "Performance monitoring data - HIGH CPU";
        
        // Store data in both real-time and historical monitors
        histMonitor.storeMonitoringData(sysData);
        histMonitor.storeMonitoringData(perfData);
        
        // Evaluate alerts based on monitoring data
        alertManager.evaluateAlerts(perfData);
        
        // Get active alerts
        std::vector<Alert> alerts = alertManager.getActiveAlerts();
        std::cout << "Active alerts: " << alerts.size() << std::endl;
        for (const auto& alert : alerts) {
            std::cout << "  - " << alert.message << " (Severity: " << alert.severity << ")" << std::endl;
        }
        
        // Generate reports
        Report sysReport = reportGen.generateSystemReport("System Health Report");
        Report perfReport = reportGen.generatePerformanceReport("Performance Analysis Report");
        
        std::cout << "Reports generated:" << std::endl;
        std::cout << "  - System Report ID: " << sysReport.id << std::endl;
        std::cout << "  - Performance Report ID: " << perfReport.id << std::endl;
        
        // Add trend data
        TrendDataPoint trend1;
        trend1.timestamp = std::chrono::system_clock::now() - std::chrono::hours(3);
        trend1.metricName = "cpu_usage";
        trend1.value = 20.0;
        trend1.category = "system";
        trend1.unit = "%";
        
        TrendDataPoint trend2;
        trend2.timestamp = std::chrono::system_clock::now() - std::chrono::hours(2);
        trend2.metricName = "cpu_usage";
        trend2.value = 45.0;
        trend2.category = "system";
        trend2.unit = "%";
        
        TrendDataPoint trend3;
        trend3.timestamp = std::chrono::system_clock::now() - std::chrono::hours(1);
        trend3.metricName = "cpu_usage";
        trend3.value = 75.0;
        trend3.category = "system";
        trend3.unit = "%";
        
        trendAnalyzer.addTrendData(trend1);
        trendAnalyzer.addTrendData(trend2);
        trendAnalyzer.addTrendData(trend3);
        
        // Analyze trends
        std::map<std::string, double> trendAnalysis = trendAnalyzer.analyzeTrend("cpu_usage");
        std::cout << "CPU Usage Trend Analysis:" << std::endl;
        for (const auto& pair : trendAnalysis) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        // Detect anomalies
        std::vector<TrendDataPoint> anomalies = trendAnalyzer.detectAnomalies("cpu_usage", 2.0);
        std::cout << "Anomalies detected: " << anomalies.size() << std::endl;
        
        std::cout << std::endl;
    }
    
    void runVisualizationDemo() {
        std::cout << "5. Visualization Features Demo" << std::endl;
        std::cout << "-----------------------------" << std::endl;
        
        DebugVisualizationData debugData;
        debugData.sessionId = "viz_debug_demo";
        debugData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(10);
        debugData.endTime = std::chrono::system_clock::now();
        debugData.fileFilter = "DemoApp.cpp";
        debugData.levelFilter = "DEBUG";
        
        // Add sample debug entries
        for (int i = 0; i < 5; ++i) {
            DebugEntry entry;
            entry.level = (i % 3 == 0) ? DebugLevel::DEBUG : 
                         (i % 3 == 1) ? DebugLevel::INFO : 
                                       DebugLevel::WARNING;
            entry.message = "Debug message " + std::to_string(i);
            entry.file = "DemoApp.cpp";
            entry.line = 100 + i;
            entry.function = "demoFunction";
            entry.timestamp = debugData.startTime + std::chrono::seconds(i * 2);
            entry.threadId = "thread_1";
            debugData.debugEntries.push_back(entry);
        }
        
        // Add sample variables
        debugData.variableValues["cpu_usage"] = "75.5";
        debugData.variableValues["memory_usage"] = "82.3";
        debugData.variableValues["thread_count"] = "4";
        
        // Generate debug visualization
        std::string debugViz = DebugVisualizer::getGlobal().generateDebugReport(debugData);
        std::cout << "Debug visualization generated (first 300 chars): " 
                  << debugViz.substr(0, 300) << "..." << std::endl;
        
        // Performance visualization
        PerformanceVisualizationData perfData;
        perfData.sessionId = "viz_perf_demo";
        perfData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(10);
        perfData.endTime = std::chrono::system_clock::now();
        perfData.operationFilter = "brain_step";
        perfData.categoryFilter = "cognition";
        
        // Add sample performance data
        for (int i = 0; i < 5; ++i) {
            PerformanceInfo perf;
            perf.operationName = "brain_step";
            perf.duration = 0.5 + (i * 0.2);
            perf.memoryUsed = 1024 + (i * 512);
            perf.cpuUsage = 50.0 + (i * 15.0);
            perf.threadId = "thread_1";
            perf.timestamp = perfData.startTime + std::chrono::seconds(i * 2);
            perf.metrics["success_rate"] = 0.90 + (i * 0.02);
            perfData.performanceData.push_back(perf);
        }
        
        // Generate performance visualization
        std::string perfViz = PerformanceVisualizer::getGlobal().generatePerformanceReport(perfData);
        std::cout << "Performance visualization generated (first 300 chars): " 
                  << perfViz.substr(0, 300) << "..." << std::endl;
        
        // Memory visualization
        MemoryVisualizationData memData;
        memData.sessionId = "viz_mem_demo";
        memData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(10);
        memData.endTime = std::chrono::system_clock::now();
        memData.allocationTypeFilter = "buffer";
        memData.showLeaks = true;
        
        // Add sample memory data
        uintptr_t addr1 = 0x1000;
        uintptr_t addr2 = 0x2000;
        uintptr_t addr3 = 0x3000;
        
        memData.allocations[addr1] = "buffer";
        memData.allocations[addr2] = "heap";
        memData.allocations[addr3] = "stack";
        
        memData.allocationSizes[addr1] = 1024;
        memData.allocationSizes[addr2] = 2048;
        memData.allocationSizes[addr3] = 512;
        
        memData.totalMemory = 10240;
        memData.usedMemory = 3584;
        
        // Add memory blocks
        for (const auto& pair : memData.allocations) {
            memData.memoryBlocks.push_back({pair.first, memData.allocationSizes.at(pair.first)});
        }
        
        // Generate memory visualization
        std::string memViz = MemoryVisualizer::getGlobal().generateMemoryReport(memData);
        std::cout << "Memory visualization generated (first 300 chars): " 
                  << memViz.substr(0, 300) << "..." << std::endl;
        
        // Thread visualization
        ThreadVisualizationData threadData;
        threadData.sessionId = "viz_thread_demo";
        threadData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(10);
        threadData.endTime = std::chrono::system_clock::now();
        threadData.threadFilter = "thread_1";
        threadData.showStackTraces = true;
        
        // Add sample thread data
        threadData.activeThreads.push_back("thread_1");
        threadData.activeThreads.push_back("thread_2");
        threadData.activeThreads.push_back("thread_3");
        
        threadData.threadCpuUsage["thread_1"] = 45.0;
        threadData.threadCpuUsage["thread_2"] = 65.0;
        threadData.threadCpuUsage["thread_3"] = 25.0;
        
        threadData.threadActivities["thread_1"] = {"step 1", "step 2", "step 3"};
        threadData.threadActivities["thread_2"] = {"processing", "waiting"};
        
        // Generate thread visualization
        std::string threadViz = ThreadVisualizer::getGlobal().generateThreadReport(threadData);
        std::cout << "Thread visualization generated (first 300 chars): " 
                  << threadViz.substr(0, 300) << "..." << std::endl;
        
        // Network visualization
        NetworkVisualizationData netData;
        netData.sessionId = "viz_net_demo";
        netData.startTime = std::chrono::system_clock::now() - std::chrono::seconds(10);
        netData.endTime = std::chrono::system_clock::now();
        netData.hostFilter = "api.example.com";
        netData.showProtocolBreakdown = true;
        
        // Add sample network data
        netData.connectionsByHost["api.example.com"] = 5;
        netData.connectionsByHost["db.example.com"] = 2;
        
        netData.bytesTransferred["api.example.com"] = 1024 * 1024; // 1MB
        netData.bytesTransferred["db.example.com"] = 512 * 1024; // 512KB
        
        netData.connectionDurations["api.example.com"] = 10.0;
        netData.connectionDurations["db.example.com"] = 5.0;
        
        // Generate network visualization
        std::string netViz = NetworkVisualizer::getGlobal().generateNetworkReport(netData);
        std::cout << "Network visualization generated (first 300 chars): " 
                  << netViz.substr(0, 300) << "..." << std::endl;
        
        std::cout << std::endl;
    }
}; // namespace nlm

// Main function to run the demo
int main() {
    DemoApplication app;
    app.runDemo();
    
    std::cout << "\nDemo application completed successfully!" << std::endl;
    std::cout << "\nThe NLM debugging and profiling infrastructure includes:" << std::endl;
    std::cout << "  - Debug infrastructure (DebugLogger, DebugSession, BreakpointManager, VariableInspector)" << std::endl;
    std::cout << "  - Profiling infrastructure (PerformanceProfiler, MemoryProfiler, CPUProfiler, etc.)" << std::endl;
    std::cout << "  - Diagnostic tools (SystemDiagnoser, ComponentDiagnoser, ConfigurationDiagnoser, etc.)" << std::endl;
    std::cout << "  - Monitoring infrastructure (RealTimeMonitor, HistoricalMonitor, AlertManager, etc.)" << std::endl;
    std::cout << "  - Visualization tools (DebugVisualizer, PerformanceVisualizer, MemoryVisualizer, etc.)" << std::endl;
    
    return 0;
}
