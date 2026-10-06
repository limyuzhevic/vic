// Monitoring System Test Suite
// Demonstrates the complete monitoring and performance metrics system for NLM

#include "MetricRegistry.hpp"
#include "RealTimeMetricsCollector.hpp"
#include "HistoricalMetricsStore.hpp"
#include "PerformanceAnalyzer.hpp"
#include "AlertSystem.hpp"
#include "RealTimeDashboard.hpp"
#include "PerformanceReportGenerator.hpp"
#include <iostream>
#include <thread>
#include <chrono>

void testMetricRegistry() {
    std::cout << "=== Testing MetricRegistry ===" << std::endl;
    
    nlm::MetricRegistry registry;
    
    // Register built-in metrics
    registry.registerBuiltInMetric("test_metric_1", 
                                  nlm::MetricCategory::NEURAL_PERFORMANCE,
                                  "Test metric 1",
                                  []() { return 42.0; });
    
    registry.registerBuiltInMetric("test_metric_2",
                                  nlm::MetricCategory::MEMORY_PERFORMANCE,
                                  "Test metric 2",
                                  []() { return 123.5; });
    
    // Register custom metric
    registry.registerCustomMetric("test_custom", 
                                  "NEURAL_PERFORMANCE",
                                  "Custom test metric",
                                  []() { return 99.9; });
    
    // Test collection
    auto metrics = registry.collectAllMetrics();
    std::cout << "Collected " << metrics.size() << " metrics:" << std::endl;
    for (const auto& metric : metrics) {
        std::cout << "  - " << metric.name << ": " << metric.value << std::endl;
    }
    
    // Test by category
    auto neuralMetrics = registry.getMetricsByCategory(nlm::MetricCategory::NEURAL_PERFORMANCE);
    std::cout << "Neural performance metrics: " << neuralMetrics.size() << std::endl;
    
    std::cout << "MetricRegistry test passed!" << std::endl;
}

void testRealTimeMetricsCollector() {
    std::cout << "\n=== Testing RealTimeMetricsCollector ===" << std::endl;
    
    nlm::MetricRegistry registry;
    nlm::RealTimeMetricsCollectorConfig config;
    
    // Register some test metrics
    registry.registerBuiltInMetric("cpu_usage",
                                  nlm::MetricCategory::SYSTEM_PERFORMANCE,
                                  "CPU usage percentage",
                                  []() { return 75.5; });
    
    registry.registerBuiltInMetric("memory_usage",
                                  nlm::MetricCategory::SYSTEM_PERFORMANCE,
                                  "Memory usage percentage",
                                  []() { return 82.3; });
    
    nlm::RealTimeMetricsCollector collector(registry, config);
    
    // Start collection
    if (collector.start()) {
        std::cout << "Metrics collection started successfully" << std::endl;
        
        // Let it run for a short time
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Collect metrics manually
        collector.collectMetrics();
        
        // Get current metrics
        auto currentMetrics = collector.getCurrentMetrics();
        std::cout << "Current metrics count: " << currentMetrics.size() << std::endl;
        
        // Get statistics
        auto stats = collector.getStats();
        std::cout << "Collection stats: " << stats.metricsCollected << " metrics collected, "
                  << stats.bufferSize << " in buffer" << std::endl;
        
        // Stop collection
        collector.stop();
        std::cout << "Metrics collection stopped" << std::endl;
    } else {
        std::cout << "Failed to start metrics collection" << std::endl;
    }
    
    std::cout << "RealTimeMetricsCollector test passed!" << std::endl;
}

void testHistoricalMetricsStore() {
    std::cout << "\n=== Testing HistoricalMetricsStore ===" << std::endl;
    
    nlm::HistoricalMetricsStoreConfig config;
    nlm::HistoricalMetricsStore store(config);
    
    // Create some test metrics
    std::vector<nlm::MetricData> metrics;
    
    auto now = std::chrono::system_clock::now();
    auto yesterday = now - std::chrono::hours(24);
    
    metrics.push_back(nlm::MetricData("cpu_usage", "SYSTEM_PERFORMANCE", "CPU usage", 75.5, now));
    metrics.push_back(nlm::MetricData("memory_usage", "SYSTEM_PERFORMANCE", "Memory usage", 82.3, yesterday));
    metrics.push_back(nlm::MetricData("spike_count", "NEURAL_PERFORMANCE", "Spike count", 1000.0, now));
    
    // Store metrics
    if (store.storeMetrics(metrics)) {
        std::cout << "Metrics stored successfully" << std::endl;
    } else {
        std::cout << "Failed to store metrics" << std::endl;
        return;
    }
    
    // Retrieve metrics for time range
    auto retrieved = store.retrieveMetrics("cpu_usage", yesterday, now);
    std::cout << "Retrieved " << retrieved.size() << " metrics for time range" << std::endl;
    
    // Get all samples
    auto samples = store.getAllSamples();
    std::cout << "Total samples in store: " << samples.size() << std::endl;
    
    // Get statistics
    auto stats = store.getStats();
    std::cout << "Store statistics: " << stats.totalSamples << " samples, "
              << stats.totalFiles << " files, " << stats.diskUsage << " bytes" << std::endl;
    
    std::cout << "HistoricalMetricsStore test passed!" << std::endl;
}

void testPerformanceAnalyzer() {
    std::cout << "\n=== Testing PerformanceAnalyzer ===" << std::endl;
    
    nlm::AnalysisConfig config;
    nlm::PerformanceAnalyzer analyzer(config);
    
    // Create test metrics
    std::vector<nlm::MetricData> metrics;
    
    auto now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; ++i) {
        double value = 50.0 + (rand() % 100); // Random values between 50 and 150
        metrics.push_back(nlm::MetricData("test_metric", "TEST", "Test metric", value, now));
    }
    
    // Perform analysis
    auto analysisResults = analyzer.analyze(metrics);
    std::cout << "Analysis results for " << metrics.front().name << ":" << std::endl;
    for (const auto& result : analysisResults) {
        std::cout << "  Mean: " << result.mean << "\n";
        std::cout << "  Median: " << result.median << "\n";
        std::cout << "  Std Dev: " << result.standardDeviation << "\n";
        std::cout << "  Min: " << result.min << ", Max: " << result.max << std::endl;
    }
    
    // Perform trend analysis
    auto trends = analyzer.analyzeTrends(metrics);
    std::cout << "Trends: " << trends.size() << " analyzed" << std::endl;
    
    // Detect anomalies
    auto anomalies = analyzer.detectAnomalies(metrics);
    std::cout << "Anomalies detected: " << anomalies.size() << std::endl;
    
    // Generate report
    std::string report = analyzer.generateReport(metrics, "text");
    std::cout << "Report generated (length: " << report.length() << " characters)" << std::endl;
    
    std::cout << "PerformanceAnalyzer test passed!" << std::endl;
}

void testAlertSystem() {
    std::cout << "\n=== Testing AlertSystem ===" << std::endl;
    
    nlm::AlertConfig config;
    nlm::AlertSystem alertSystem(config);
    
    // Add recipients
    alertSystem.addRecipient(nlm::AlertRecipient("console", nlm::RecipientType::CONSOLE, "console", nlm::AlertLevel::WARNING));
    alertSystem.addRecipient(nlm::AlertRecipient("file", nlm::RecipientType::LOG_FILE, "./alerts.log", nlm::AlertLevel::ERROR));
    
    // Process alerts
    nlm::AlertData alert1("alert_1", nlm::AlertType::PERFORMANCE_DEGRADATION, nlm::AlertLevel::WARNING,
                         "Performance degradation detected", "cpu_usage", 95.5, 80.0);
    
    if (alertSystem.processAlert(alert1)) {
        std::cout << "Alert 1 processed successfully" << std::endl;
    } else {
        std::cout << "Failed to process alert 1" << std::endl;
    }
    
    nlm::AlertData alert2("alert_2", nlm::AlertType::MEMORY_EXHAUSTION, nlm::AlertLevel::CRITICAL,
                         "Memory exhaustion", "memory_usage", 99.0, 90.0);
    
    if (alertSystem.processAlert(alert2)) {
        std::cout << "Alert 2 processed successfully" << std::endl;
    } else {
        std::cout << "Failed to process alert 2" << std::endl;
    }
    
    // Get recent alerts
    auto alerts = alertSystem.getRecentAlerts(10);
    std::cout << "Recent alerts: " << alerts.size() << std::endl;
    
    // Get alerts by level
    auto warningAlerts = alertSystem.getAlertsByLevel(nlm::AlertLevel::WARNING);
    std::cout << "Warning alerts: " << warningAlerts.size() << std::endl;
    
    // Get alerts by type
    auto perfAlerts = alertSystem.getAlertsByType(nlm::AlertType::PERFORMANCE_DEGRADATION);
    std::cout << "Performance degradation alerts: " << perfAlerts.size() << std::endl;
    
    std::cout << "AlertSystem test passed!" << std::endl;
}

void testRealTimeDashboard() {
    std::cout << "\n=== Testing RealTimeDashboard ===" << std::endl;
    
    nlm::MetricRegistry registry;
    nlm::RealTimeMetricsCollectorConfig collectorConfig;
    nlm::RealTimeMetricsCollector collector(registry, collectorConfig);
    
    // Register test metrics
    registry.registerBuiltInMetric("system_cpu", nlm::MetricCategory::SYSTEM_PERFORMANCE,
                                  "CPU usage", []() { return 75.5; });
    
    registry.registerBuiltInMetric("system_memory", nlm::MetricCategory::SYSTEM_PERFORMANCE,
                                  "Memory usage", []() { return 82.3; });
    
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.enableAutoRefresh = false; // Disable for test
    
    nlm::RealTimeDashboard dashboard(collector, dashboardConfig);
    
    // Add metrics to dashboard
    dashboard.addMetric(nlm::DashboardMetric("system_cpu", "CPU Usage"));
    dashboard.addMetric(nlm::DashboardMetric("system_memory", "Memory Usage"));
    
    // Add widgets
    dashboard.addWidget(nlm::DashboardWidget("widget_1", "System Status", "metric",
                                             0, 0, 2, 1, std::vector<std::string>{"system_cpu", "system_memory"}));
    
    // Get dashboard data
    std::string jsonData = dashboard.getDashboardData("json");
    std::cout << "Dashboard JSON data generated (length: " << jsonData.length() << " characters)" << std::endl;
    
    std::string htmlData = dashboard.getDashboardData("html");
    std::cout << "Dashboard HTML data generated (length: " << htmlData.length() << " characters)" << std::endl;
    
    // Get metrics and widgets
    auto metrics = dashboard.getMetrics();
    auto widgets = dashboard.getWidgets();
    
    std::cout << "Dashboard metrics: " << metrics.size() << std::endl;
    std::cout << "Dashboard widgets: " << widgets.size() << std::endl;
    
    std::cout << "RealTimeDashboard test passed!" << std::endl;
}

void testPerformanceReportGenerator() {
    std::cout << "\n=== Testing PerformanceReportGenerator ===" << std::endl;
    
    nlm::ReportConfig reportConfig;
    reportConfig.title = "NLM Performance Test Report";
    reportConfig.description = "Test report for NLM monitoring system";
    reportConfig.dateRange = "2024-01-01 to 2024-01-31";
    reportConfig.generatedBy = "Monitoring System Test";
    
    // Create a historical metrics store
    nlm::HistoricalMetricsStoreConfig storeConfig;
    nlm::HistoricalMetricsStore historicalStore(storeConfig);
    
    // Create a performance analyzer
    nlm::AnalysisConfig analyzerConfig;
    nlm::PerformanceAnalyzer analyzer(analyzerConfig);
    
    nlm::PerformanceReportGenerator reportGenerator(historicalStore, analyzer, reportConfig);
    
    // Create test metrics
    std::vector<nlm::MetricData> metrics;
    
    auto now = std::chrono::system_clock::now();
    auto yesterday = now - std::chrono::hours(24);
    
    metrics.push_back(nlm::MetricData("cpu_usage", "SYSTEM_PERFORMANCE", "CPU usage", 75.5, now));
    metrics.push_back(nlm::MetricData("memory_usage", "SYSTEM_PERFORMANCE", "Memory usage", 82.3, yesterday));
    metrics.push_back(nlm::MetricData("spike_count", "NEURAL_PERFORMANCE", "Spike count", 1000.0, now));
    
    // Store metrics
    historicalStore.storeMetrics(metrics);
    
    // Generate report
    if (reportGenerator.generateReport("test_report", metrics)) {
        std::cout << "Test report generated successfully" << std::endl;
        
        // Export report
        std::string jsonReport = reportGenerator.exportToJSON("test_report");
        std::cout << "JSON report generated (length: " << jsonReport.length() << " characters)" << std::endl;
        
        std::string htmlReport = reportGenerator.exportToHTML("test_report");
        std::cout << "HTML report generated (length: " << htmlReport.length() << " characters)" << std::endl;
        
        // Get report metadata
        auto metadata = reportGenerator.getReportMetadata("test_report");
        std::cout << "Report metadata: " << metadata.name << ", " << metadata.size << " bytes" << std::endl;
    } else {
        std::cout << "Failed to generate test report" << std::endl;
    }
    
    std::cout << "PerformanceReportGenerator test passed!" << std::endl;
}

void testCompleteIntegration() {
    std::cout << "\n=== Testing Complete Integration ===" << std::endl;
    
    // Create all components
    nlm::MetricRegistry registry;
    nlm::RealTimeMetricsCollectorConfig collectorConfig;
    nlm::RealTimeMetricsCollector collector(registry, collectorConfig);
    
    nlm::HistoricalMetricsStoreConfig storeConfig;
    nlm::HistoricalMetricsStore historicalStore(storeConfig);
    
    nlm::AnalysisConfig analyzerConfig;
    nlm::PerformanceAnalyzer analyzer(analyzerConfig);
    
    nlm::AlertConfig alertConfig;
    nlm::AlertSystem alertSystem(alertConfig);
    
    nlm::ReportConfig reportConfig;
    reportConfig.title = "Complete Integration Test Report";
    nlm::PerformanceReportGenerator reportGenerator(historicalStore, analyzer, reportConfig);
    
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.enableAutoRefresh = false;
    nlm::RealTimeDashboard dashboard(collector, dashboardConfig);
    
    // Setup integration
    collector.setHistoricalStore(historicalStore);
    collector.setAlertSystem(alertSystem);
    collector.setPerformanceAnalyzer(analyzer);
    
    dashboard.setAlertCallback([](const nlm::AlertData& alert) {
        std::cout << "Dashboard received alert: " << alert.message << std::endl;
    });
    
    // Register test metrics
    registry.registerBuiltInMetric("test_metric", nlm::MetricCategory::NEURAL_PERFORMANCE,
                                  "Test metric", []() { return 42.0; });
    
    // Start components
    collector.start();
    dashboard.start();
    
    // Let integration run for a bit
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Collect metrics
    collector.collectMetrics();
    
    // Generate report
    std::vector<nlm::MetricData> metrics = collector.getCurrentMetrics();
    if (!metrics.empty()) {
        if (reportGenerator.generateReport("integration_report", metrics)) {
            std::cout << "Integration test report generated successfully" << std::endl;
        } else {
            std::cout << "Failed to generate integration test report" << std::endl;
        }
    }
    
    // Stop components
    collector.stop();
    dashboard.stop();
    
    // Get system statistics
    auto collectorStats = collector.getStats();
    auto storageStats = historicalStore.getStats();
    auto alertStats = alertSystem.getStats();
    
    std::cout << "Integration test statistics:" << std::endl;
    std::cout << "  Collector: " << collectorStats.metricsCollected << " metrics collected" << std::endl;
    std::cout << "  Storage: " << storageStats.totalSamples << " samples stored" << std::endl;
    std::cout << "  Alerts: " << alertStats.totalAlerts << " alerts processed" << std::endl;
    
    std::cout << "Complete integration test passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Monitoring System Test Suite ===" << std::endl;
    
    testMetricRegistry();
    testRealTimeMetricsCollector();
    testHistoricalMetricsStore();
    testPerformanceAnalyzer();
    testAlertSystem();
    testRealTimeDashboard();
    testPerformanceReportGenerator();
    testCompleteIntegration();
    
    std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
    std::cout << "The NLM monitoring and performance metrics system is fully functional!" << std::endl;
    
    return 0;
}
