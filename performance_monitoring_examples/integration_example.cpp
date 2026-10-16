# Performance Monitoring Integration Example
#
# This example demonstrates how to integrate the NLM performance monitoring system
# into the core NLM components (Config, Brain, Neuron, Synapse)
#

/**
 * Example 1: Basic Performance Monitoring Integration
 */

#include "performance_monitoring/PerformanceMonitor.hpp"
#include "performance_monitoring/Profiler.hpp"
#include "performance_monitoring/PerformanceMetrics.hpp"
#include "performance_monitoring/PerformanceAnalysis.hpp"
#include "performance_monitoring/PerformanceDashboard.hpp"
#include "performance_monitoring/PerformanceReporter.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Brain.hpp"

void exampleBasicIntegration() {
    std::cout << "=== Basic Performance Monitoring Integration ===\n";
    
    // Initialize NLM components
    auto config = std::make_shared<Config>();
    config->set("neuron.count", 1000);
    config->set("synapse.count", 10000);
    config->set("performance.monitoring_level", "Detailed");
    config->set("performance.enable_real_time", true);
    config->set("performance.generate_flame_graphs", true);
    
    // Initialize performance monitoring system
    auto performanceMonitor = std::make_shared<nlm::PerformanceMonitor>(config);
    nlm::PerformanceMonitor::setInstance(performanceMonitor);
    
    auto profiler = std::make_shared<nlm::Profiler>(config);
    nlm::Profiler::setInstance(profiler);
    
    auto performanceMetrics = std::make_shared<nlm::PerformanceMetrics>();
    nlm::PerformanceMetrics::setInstance(performanceMetrics);
    
    auto performanceAnalysis = std::make_shared<nlm::PerformanceAnalysis>(performanceMetrics);
    nlm::PerformanceAnalysis::setInstance(performanceAnalysis);
    
    // Start monitoring
    performanceMonitor->startMonitoring();
    profiler->startCallStackProfiling();
    profiler->startMemoryProfiling();
    profiler->startEventProfiling();
    
    std::cout << "Performance monitoring system initialized and started\n";
    
    // Simulate some performance data
    for (int i = 0; i < 100; ++i) {
        // Record metrics
        performanceMetrics->recordMetric("neuron.step.time", nlm::PerformanceCategory::NeuralComputation,
                                        0.001 * (1 + rand() % 100) / 100.0, "seconds");
        performanceMetrics->recordMetric("memory.usage", nlm::PerformanceCategory::MemoryManagement,
                                        50.0 * 1024.0 * 1024.0, "bytes");  // 50 MB
        performanceMetrics->recordMetric("cpu.time", nlm::PerformanceCategory::Overall,
                                        0.1 * (1 + rand() % 10) / 10.0, "seconds");
        
        // Start and stop timers
        performanceMetrics->startTimer("synapse_update");
        // Simulate synapse update work
        performanceMetrics->stopTimer("synapse_update");
        
        // Record memory allocation
        performanceMetrics->recordMemorySnapshot();
        
        // Record CPU and I/O samples
        performanceMetrics->recordCPUSample(50.0 + rand() % 50);
        performanceMetrics->recordIOSample(1024 * (i + 1), std::chrono::microseconds(1000));
        
        // Record profiling data
        profiler->recordCallStack(std::vector<nlm::CallStackFrame>());  // Simplified
        profiler->recordAllocation((void*)0x1000, 1024, "Neuron");
        profiler->recordEvent("neuron_step", i * 1000);
    }
    
    // Update analysis
    auto analysis = performanceAnalysis->analyzePerformance();
    for (const auto& item : analysis) {
        std::cout << "Analysis: " << item << "\n";
    }
    
    // Generate report
    performanceMonitor->generateDetailedReport("/tmp/detailed_performance_report.txt");
    performanceMonitor->exportPerformanceData("/tmp/performance_data.json", nlm::OutputFormat::JSON);
    
    // Generate flame graph
    profiler->generateFlameGraph("/tmp/flame_graph.html");
    
    std::cout << "Performance data exported and reports generated\n";
    
    // Stop monitoring
    performanceMonitor->stopMonitoring();
    profiler->stopCallStackProfiling();
    profiler->stopMemoryProfiling();
    profiler->stopEventProfiling();
}

/**
 * Example 2: Performance Monitoring in Neuron Operations
 */

class NeuronPerformanceMonitor {
private:
    std::shared_ptr<nlm::PerformanceMonitor> monitor;
    std::chrono::high_resolution_clock::time_point stepStart;
    
public:
    NeuronPerformanceMonitor() {
        monitor = &nlm::PerformanceMonitor::getInstance();
    }
    
    void recordStepStart() {
        if (monitor->isMonitoring()) {
            stepStart = std::chrono::high_resolution_clock::now();
            profiler->startProfile("Neuron::step", __FILE__, __LINE__);
        }
    }
    
    void recordStepEnd() {
        if (monitor->isMonitoring()) {
            auto stepEnd = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart);
            
            // Record step performance
            monitor->recordMetric("neuron.step.time", nlm::PerformanceCategory::NeuralComputation,
                                 duration.count() / 1000000.0, "seconds");
            
            profiler->stopProfile("Neuron::step");
        }
    }
    
    void recordSpike(const nlm::Neuron& neuron) {
        if (monitor->isMonitoring()) {
            monitor->recordMetric("neuron.spike.count", nlm::PerformanceCategory::NeuralComputation, 1.0, "count");
            monitor->recordEvent("neuron_spike", 1);
        }
    }
};

/**
 * Example 3: Performance Monitoring in Synapse Operations
 */

class SynapsePerformanceMonitor {
private:
    std::shared_ptr<nlm::PerformanceMonitor> monitor;
    
public:
    SynapsePerformanceMonitor() {
        monitor = &nlm::PerformanceMonitor::getInstance();
    }
    
    void recordPlasticityUpdate(const nlm::Synapse& synapse) {
        if (monitor->isMonitoring()) {
            monitor->startTimer("synaptic_plasticity");
            // Simulate plasticity computation
            monitor->stopTimer("synaptic_plasticity");
            
            monitor->recordMetric("synapse.plasticity.time", nlm::PerformanceCategory::Connectivity,
                                 monitor->getTimerDuration("synaptic_plasticity").count() / 1000000.0, "seconds");
        }
    }
    
    void recordWeightUpdate(double deltaWeight) {
        if (monitor->isMonitoring()) {
            monitor->recordMetric("synapse.weight.change", nlm::PerformanceCategory::Connectivity,
                                 fabs(deltaWeight), "normalized");
        }
    }
};

/**
 * Example 4: Performance Dashboard Integration
 */

void exampleDashboardIntegration() {
    std::cout << "=== Performance Dashboard Integration ===\n";
    
    auto monitor = &nlm::PerformanceMonitor::getInstance();
    auto metrics = &nlm::PerformanceMetrics::getInstance();
    
    // Create dashboard
    auto dashboard = std::make_shared<nlm::PerformanceDashboard>(monitor, metrics);
    nlm::PerformanceDashboard::setInstance(dashboard);
    
    // Configure dashboard
    dashboard->setMaxDisplayItems(50);
    dashboard->setRefreshInterval(std::chrono::milliseconds(500));
    dashboard->setAutoRefresh(true);
    
    // Add widgets
    nlm::DashboardWidget memoryWidget;
    memoryWidget.id = "memory_usage";
    memoryWidget.title = "Memory Usage";
    memoryWidget.type = "metric";
    memoryWidget.category = nlm::PerformanceCategory::MemoryManagement;
    memoryWidget.metricName = "memory.usage";
    memoryWidget.unit = "MB";
    memoryWidget.showTrend = true;
    memoryWidget.width = 400;
    memoryWidget.height = 300;
    
    dashboard->addWidget(memoryWidget);
    
    nlm::DashboardWidget cpuWidget;
    cpuWidget.id = "cpu_usage";
    cpuWidget.title = "CPU Usage";
    cpuWidget.type = "metric";
    cpuWidget.category = nlm::PerformanceCategory::Overall;
    cpuWidget.metricName = "cpu.time";
    cpuWidget.unit = "seconds";
    cpuWidget.showTrend = true;
    cpuWidget.width = 400;
    cpuWidget.height = 300;
    
    dashboard->addWidget(cpuWidget);
    
    // Start dashboard
    dashboard->startDashboard();
    
    std::cout << "Performance dashboard started with " << dashboard->getAllWidgets().size() << " widgets\n";
    
    // Display dashboard
    dashboard->display();
    
    // Export dashboard
    dashboard->exportDashboard("/tmp/performance_dashboard.html", nlm::OutputFormat::HTML);
    
    // Stop dashboard
    dashboard->stopDashboard();
}

/**
 * Example 5: Performance Regression Detection
 */

void exampleRegressionDetection() {
    std::cout << "=== Performance Regression Detection ===\n";
    
    auto monitor = &nlm::PerformanceMonitor::getInstance();
    auto analysis = &nlm::PerformanceAnalysis::getInstance();
    
    // Establish baseline
    monitor->establishBaseline("baseline_v1.0");
    
    // Simulate some baseline data
    for (int i = 0; i < 100; ++i) {
        monitor->recordMetric("neuron.step.time", nlm::PerformanceCategory::NeuralComputation,
                             0.001 * (1 + rand() % 100) / 100.0, "seconds");
        monitor->recordMetric("memory.usage", nlm::PerformanceCategory::MemoryManagement,
                             50.0 * 1024.0 * 1024.0, "bytes");
    }
    
    monitor->addProfileToHistory();
    
    // Compare with baseline
    bool isRegressed = monitor->compareToBaseline("baseline_v1.0");
    
    std::cout << "Baseline comparison: " << (isRegressed ? "REGRESSION DETECTED" : "No regression") << "\n";
    
    // Detect regressions
    auto regressions = analysis->detectRegression("baseline_v1.0");
    for (const auto& regression : regressions) {
        std::cout << "Regression: " << regression.regressionDescription << "\n";
        std::cout << "  Impact: " << regression.impactScore << "\n";
        std::cout << "  Suggested Fix: " << regression.suggestedFix << "\n";
    }
    
    // Generate comparison report
    auto reporter = std::make_shared<nlm::PerformanceReporter>(monitor, &nlm::PerformanceMetrics::getInstance(), analysis);
    reporter->generateComparisonReport("/tmp/comparison_report.txt", "baseline_v1.0");
}

/**
 * Example 6: Performance Configuration System
 */

void exampleConfigurationSystem() {
    std::cout << "=== Performance Configuration System ===\n";
    
    // Create configuration file
    std::ofstream configFile("/tmp/performance_config.json");
    configFile << "{\n  \"performance\": {\n"
             << "    \"monitoring_level\": \"Comprehensive\",\n"
             << "    \"enable_real_time\": true,\n"
             << "    \"enable_call_stack_profiling\": true,\n"
             << "    \"enable_memory_profiling\": true,\n"
             << "    \"enable_event_profiling\": true,\n"
             << "    \"enable_bottleneck_detection\": true,\n"
             << "    \"max_profile_history\": 1000,\n"
             << "    \"update_interval_ms\": 100,\n"
             << "    \"generate_flame_graphs\": true,\n"
             << "    \"enable_regression_detection\": true,\n"
             << "    \"regression_detection_interval_ms\": 300000,\n"
             << "    \"enable_optimization_recommendations\": true,\n"
             << "    \"enable_performance_dashboard\": true,\n"
             << "    \"memory_thresholds\": {\"default\": 1073741824},\n"
             << "    \"event_thresholds\": {\"default\": 100000}\n"
             << "  }\n"
             << "}\n";
    configFile.close();
    
    std::cout << "Configuration file created at /tmp/performance_config.json\n";
    
    // Load configuration
    auto config = std::make_shared<Config>();
    config->loadFromFile("/tmp/performance_config.json");
    
    // Create performance monitor with configuration
    auto performanceMonitor = std::make_shared<nlm::PerformanceMonitor>(config);
    performanceMonitor->initialize();
    
    // Access configuration values
    std::string monitoringLevel = config->getOr<std::string>("performance.monitoring_level", "Detailed");
    bool enableRealTime = config->getOr<bool>("performance.enable_real_time", true);
    
    std::cout << "Monitoring Level: " << monitoringLevel << "\n";
    std::cout << "Real-time Monitoring: " << (enableRealTime ? "Enabled" : "Disabled") << "\n";
}

int main() {
    std::cout << "NLM Performance Monitoring Integration Examples\n";
    std::cout << "===============================================\n\n";
    
    // Run all examples
    exampleBasicIntegration();
    std::cout << "\n";
    
    exampleDashboardIntegration();
    std::cout << "\n";
    
    exampleRegressionDetection();
    std::cout << "\n";
    
    exampleConfigurationSystem();
    std::cout << "\n";
    
    std::cout << "All examples completed successfully!\n";
    return 0;
}
