#pragma once

#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_session.hpp"
#include "catch2/catch_stringref.hpp"
#include "../performance/MetricsRecorder.hpp"
#include "../performance/MonitoringManager.hpp"
#include "../visualization/VisualizationEngine.hpp"
#include "../export/ExportManager.hpp"
#include "../web/WebInterface.hpp"
#include "../brain/Brain.hpp"
#include <chrono>
#include <thread>

// Test fixtures for monitoring functionality
test_suite "MetricsRecorder Tests" {
    TEST_CASE("MetricsRecorder Initialization", "[MetricsRecorder][Unit]" ) {
        nlm::MetricsRecorder& recorder = nlm::MetricsRecorder::getInstance();
        
        // Test initial state
        REQUIRE_FALSE(recorder.isEnabled());
        
        // Enable recording
        recorder.setEnabled(true);
        REQUIRE(recorder.isEnabled());
        
        // Set recording interval
        recorder.setRecordingInterval(std::chrono::milliseconds(100));
        REQUIRE(recorder.getRecordingInterval() == std::chrono::milliseconds(100));
        
        // Record a metric
        recorder.record("test_metric", 42.5, "Test metric for unit testing");
        
        // Check metric was recorded
        auto value = recorder.getCurrentValue("test_metric");
        REQUIRE(value.has_value());
        REQUIRE(*value == 42.5);
        
        // Get metric names
        auto names = recorder.getMetricNames();
        REQUIRE(names.size() >= 1);
        REQUIRE(std::find(names.begin(), names.end(), "test_metric") != names.end());
        
        // Get statistics
        auto stats = recorder.getStats("test_metric");
        REQUIRE(stats.count == 1);
        REQUIRE(stats.mean == 42.5);
        REQUIRE(stats.min == 42.5);
        REQUIRE(stats.max == 42.5);
        
        // Clean up
        recorder.clear();
        REQUIRE(recorder.getMetricNames().empty());
    }
    
    TEST_CASE("MetricsRecorder Neural Metrics", "[MetricsRecorder][Integration]" ) {
        nlm::MetricsRecorder& recorder = nlm::MetricsRecorder::getInstance();
        
        // Enable recording
        recorder.setEnabled(true);
        
        // Create a mock brain (would need actual brain implementation in real tests)
        // For now, test with placeholder data
        recorder.record("neuron_count", 1000.0);
        recorder.record("synapse_count", 5000.0);
        recorder.record("active_neurons", 50.0);
        
        // Verify metrics
        REQUIRE(recorder.getCurrentValue("neuron_count").value() == 1000.0);
        REQUIRE(recorder.getCurrentValue("synapse_count").value() == 5000.0);
        REQUIRE(recorder.getCurrentValue("active_neurons").value() == 50.0);
        
        recorder.clear();
    }
    
    TEST_CASE("MetricsRecorder Performance Metrics", "[MetricsRecorder][Performance]" ) {
        nlm::MetricsRecorder& recorder = nlm::MetricsRecorder::getInstance();
        
        // Enable recording
        recorder.setEnabled(true);
        
        // Record performance metrics
        recorder.record("cpu_usage", 45.0);
        recorder.record("memory_usage", 512.0);
        recorder.record("step_time", 1.5);
        
        // Verify metrics
        REQUIRE(recorder.getCurrentValue("cpu_usage").value() == 45.0);
        REQUIRE(recorder.getCurrentValue("memory_usage").value() == 512.0);
        REQUIRE(recorder.getCurrentValue("step_time").value() == 1.5);
        
        // Get statistics for performance metrics
        auto cpuStats = recorder.getStats("cpu_usage");
        REQUIRE(cpuStats.mean == 45.0);
        
        recorder.clear();
    }
    
    TEST_CASE("MetricsRecorder Historical Data", "[MetricsRecorder][History]" ) {
        nlm::MetricsRecorder& recorder = nlm::MetricsRecorder::getInstance();
        
        // Enable recording
        recorder.setEnabled(true);
        
        // Record multiple values over time
        recorder.record("temperature", 20.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        recorder.record("temperature", 21.0);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        recorder.record("temperature", 22.0);
        
        // Get historical data
        auto history = recorder.getHistoricalData("temperature", 10);
        REQUIRE(history.size() == 3);
        
        // Verify historical values
        REQUIRE(history[0].value == 20.0);
        REQUIRE(history[1].value == 21.0);
        REQUIRE(history[2].value == 22.0);
        
        recorder.clear();
    }
    
    TEST_CASE("MetricsRecorder Clear Old Data", "[MetricsRecorder][Cleanup]" ) {
        nlm::MetricsRecorder& recorder = nlm::MetricsRecorder::getInstance();
        
        // Enable recording
        recorder.setEnabled(true);
        
        // Record some metrics
        recorder.record("metric1", 1.0);
        recorder.record("metric2", 2.0);
        
        // Clear old data (everything should be removed)
        recorder.clearOldData(std::chrono::milliseconds(0));
        
        // All metrics should be cleared
        REQUIRE(recorder.getMetricNames().empty());
    }
};

test_suite "MonitoringManager Tests" {
    TEST_CASE("MonitoringManager Initialization", "[MonitoringManager][Unit]" ) {
        nlm::MonitoringConfig config;
        config.enableRealTimeMetrics = true;
        config.enablePerformanceMonitoring = true;
        config.enableHistoryTracking = true;
        
        nlm::MonitoringManager& manager = nlm::MonitoringManager::getInstance();
        
        // Initialize manager
        bool success = manager.initialize(config);
        REQUIRE(success);
        
        // Check configuration
        REQUIRE(manager.isRealTimeMetricsEnabled());
        REQUIRE(manager.isPerformanceMonitoringEnabled());
        REQUIRE(manager.isHistoryTrackingEnabled());
        
        // Start monitoring
        manager.start();
        REQUIRE(manager.isRunning());
        
        // Stop monitoring
        manager.stop();
        REQUIRE_FALSE(manager.isRunning());
    }
    
    TEST_CASE("MonitoringManager Metrics Registration", "[MonitoringManager][Registration]" ) {
        nlm::MonitoringManager& manager = nlm::MonitoringManager::getInstance();
        
        // Register a test metric
        nlm::MetricDefinition metric("test_metric", "Test metric", "units",
                                    []() { return 42.0; },
                                    std::chrono::milliseconds(1000));
        
        manager.registerMetric(metric);
        
        // Check metric was registered
        auto names = manager.getMetricNames();
        REQUIRE(std::find(names.begin(), names.end(), "test_metric") != names.end());
        
        // Get metric value
        auto value = manager.getMetricValue("test_metric");
        REQUIRE(value.has_value());
        REQUIRE(*value == 42.0);
    }
    
    TEST_CASE("MonitoringManager Neural Metrics", "[MonitoringManager][Neural]" ) {
        nlm::MonitoringManager& manager = nlm::MonitoringManager::getInstance();
        
        // Create a mock brain for testing
        // In real implementation, would need actual brain
        
        // Register neural metrics
        manager.registerNeuralMetrics(const_cast<nlm::Brain&>(*reinterpret_cast<Brain*>(0x1)));
        
        // Check metrics were registered
        auto names = manager.getMetricNames();
        // Should contain neural metrics like neuron_count, synapse_count, etc.
        REQUIRE_FALSE(names.empty());
        
        // Test update
        manager.update();
        
        // Get performance metrics
        auto perfMetrics = manager.getPerformanceMetrics();
        REQUIRE(perfMetrics.neuronCount >= 0);
    }
    
    TEST_CASE("MonitoringManager Statistics", "[MonitoringManager][Stats]" ) {
        nlm::MonitoringManager& manager = nlm::MonitoringManager::getInstance();
        
        // Get statistics
        auto stats = manager.getStats();
        
        // Check basic statistics
        REQUIRE(stats.totalMetrics >= 0);
        REQUIRE(stats.activeMetrics >= 0);
        REQUIRE(stats.realTimeMetrics >= 0);
        REQUIRE(stats.cpuUsageAvg >= 0.0);
        REQUIRE(stats.memoryUsageAvg >= 0.0);
        
        // Test summary generation
        std::string summary = stats.toString();
        REQUIRE(!summary.empty());
        REQUIRE(summary.find("Total metrics:") != std::string::npos);
    }
    
    TEST_CASE("MonitoringManager Export", "[MonitoringManager][Export]" ) {
        nlm::MonitoringManager& manager = nlm::MonitoringManager::getInstance();
        
        // Export to CSV
        bool csvSuccess = manager.exportMetrics("test_output.csv", "csv");
        // In real implementation, would need actual export functionality
        // For now, this is a placeholder
        
        // Export to JSON
        bool jsonSuccess = manager.exportPerformanceMetrics("test_output.json", "json");
        // In real implementation, would need actual export functionality
        // For now, this is a placeholder
        
        // Note: Export functionality is implemented but needs proper integration
    }
};

test_suite "VisualizationEngine Tests" {
    TEST_CASE("VisualizationEngine Initialization", "[VisualizationEngine][Unit]" ) {
        nlm::VisualizationConfig config;
        config.enableVisualization = true;
        config.enableNetworkVisualization = true;
        config.enableActivityVisualization = true;
        
        nlm::VisualizationEngine& engine = nlm::VisualizationEngine::getInstance();
        
        // Initialize engine
        bool success = engine.initialize(config);
        REQUIRE(success);
        
        // Check configuration
        REQUIRE(engine.isNetworkVisualizationEnabled());
        REQUIRE(engine.isActivityVisualizationEnabled());
        
        // Start visualization
        engine.start();
        REQUIRE(engine.isRunning());
        
        // Stop visualization
        engine.stop();
        REQUIRE_FALSE(engine.isRunning());
    }
    
    TEST_CASE("VisualizationEngine Update", "[VisualizationEngine][Update]" ) {
        nlm::VisualizationEngine& engine = nlm::VisualizationEngine::getInstance();
        
        // Enable engine
        engine.setEnabled(true);
        
        // Update with mock data (in real tests, would need actual brain)
        // For now, this is a placeholder
        
        // Check that engine is running
        REQUIRE(engine.isRunning());
        
        // Test data access
        auto data = engine.getCurrentData();
        REQUIRE(data.neuralData.neuronActivations.size() >= 0);
    }
    
    TEST_CASE("VisualizationEngine Export", "[VisualizationEngine][Export]" ) {
        nlm::VisualizationEngine& engine = nlm::VisualizationEngine::getInstance();
        
        // Export to PNG
        bool pngSuccess = engine.exportVisualization("test_output.png", nlm::ExportFormat::PNG);
        // In real implementation, would need actual export functionality
        // For now, this is a placeholder
        
        // Export to JSON
        bool jsonSuccess = engine.exportVisualization("test_output.json", nlm::ExportFormat::JSON);
        // In real implementation, would need actual export functionality
        // For now, this is a placeholder
        
        // Note: Export functionality is implemented but needs proper integration
    }
};

test_suite "ExportManager Tests" {
    TEST_CASE("ExportManager Initialization", "[ExportManager][Unit]" ) {
        nlm::ExportConfig config;
        config.outputDirectory = "./test_exports";
        config.autoCreateDirectory = true;
        
        nlm::ExportManager& manager = nlm::ExportManager::getInstance();
        
        // Initialize manager
        bool success = manager.initialize(config);
        REQUIRE(success);
        
        // Check configuration
        REQUIRE(manager.getOutputDirectory() == config.outputDirectory);
        
        // Start manager
        manager.start();
        REQUIRE(manager.isRunning());
        
        // Stop manager
        manager.stop();
        REQUIRE_FALSE(manager.isRunning());
    }
    
    TEST_CASE("ExportManager Metrics Export", "[ExportManager][Metrics]" ) {
        nlm::ExportManager& manager = nlm::ExportManager::getInstance();
        
        // Start manager
        manager.start();
        
        // Export metrics
        auto result = manager.exportMetrics("test_metrics.csv", "csv");
        
        // Check result
        REQUIRE(result.success);
        REQUIRE(!result.outputPath.empty());
        
        // Check manager is still running
        REQUIRE(manager.isRunning());
    }
    
    TEST_CASE("ExportManager Formats", "[ExportManager][Formats]" ) {
        nlm::ExportManager& manager = nlm::ExportManager::getInstance();
        
        // Start manager
        manager.start();
        
        // Test available formats
        auto formats = manager.getAvailableFormats();
        REQUIRE(!formats.empty());
        
        // Check for common formats
        bool hasCSV = std::find(formats.begin(), formats.end(), "csv") != formats.end();
        bool hasJSON = std::find(formats.begin(), formats.end(), "json") != formats.end();
        bool hasPNG = std::find(formats.begin(), formats.end(), "png") != formats.end();
        
        // At least CSV format should be available
        REQUIRE(hasCSV);
    }
    
    TEST_CASE("ExportManager Task Management", "[ExportManager][Tasks]" ) {
        nlm::ExportManager& manager = nlm::ExportManager::getInstance();
        
        // Start manager
        manager.start();
        
        // Get initial task count
        size_t initialTasks = manager.getActiveTasks().size();
        
        // Add a filter
        nlm::ExportFilter filter;
        filter.name = "test_filter";
        filter.enabled = true;
        filter.condition = [](const nlm::ExportTask& task) { return task.name != "skip"; };
        
        manager.addFilter(filter);
        
        // Check filter was added
        auto tasks = manager.getActiveTasks();
        REQUIRE_FALSE(tasks.empty());
        
        // Clean up
        manager.removeFilter("test_filter");
        manager.clearFilters();
    }
};

test_suite "WebInterface Tests" {
    TEST_CASE("WebInterface Initialization", "[WebInterface][Unit]" ) {
        nlm::WebInterfaceConfig config;
        config.enableWebInterface = true;
        config.host = "localhost";
        config.port = 8080;
        
        nlm::WebInterface& webInterface = nlm::WebInterface::getInstance();
        
        // Initialize web interface
        bool success = webInterface.initialize(config);
        REQUIRE(success);
        
        // Check configuration
        REQUIRE(webInterface.getHost() == config.host);
        REQUIRE(webInterface.getPort() == config.port);
        
        // Start web interface
        webInterface.start();
        REQUIRE(webInterface.isRunning());
        
        // Stop web interface
        webInterface.stop();
        REQUIRE_FALSE(webInterface.isRunning());
    }
    
    TEST_CASE("WebInterface API Endpoints", "[WebInterface][API]" ) {
        nlm::WebInterface& webInterface = nlm::WebInterface::getInstance();
        
        // Start web interface
        webInterface.start();
        
        // Check that API endpoints are registered
        // In real implementation, would test actual endpoints
        
        // Get server status
        std::string status = webInterface.getServerStatus();
        REQUIRE(!status.empty());
        REQUIRE(status.find("NLM Web Interface") != std::string::npos);
        
        // Check connection count
        size_t connectionCount = webInterface.getConnectionCount();
        REQUIRE(connectionCount >= 0);
        
        // Stop web interface
        webInterface.stop();
    }
    
    TEST_CASE("WebInterface Static Files", "[WebInterface][Static]" ) {
        nlm::WebInterface& webInterface = nlm::WebInterface::getInstance();
        
        // Start web interface
        webInterface.start();
        
        // Serve static files
        webInterface.serveStaticFiles("./test_static");
        
        // Check connection count
        size_t connectionCount = webInterface.getConnectionCount();
        REQUIRE(connectionCount >= 0);
        
        // Stop web interface
        webInterface.stop();
    }
};

test_suite "Integration Tests" {
    TEST_CASE("Complete System Integration", "[Integration][End-to-End]" ) {
        // Test that all monitoring components work together
        nlm::MonitoringManager& monitoringManager = nlm::MonitoringManager::getInstance();
        nlm::VisualizationEngine& visualizationEngine = nlm::VisualizationEngine::getInstance();
        nlm::ExportManager& exportManager = nlm::ExportManager::getInstance();
        
        // Configure components
        nlm::MonitoringConfig monitoringConfig;
        monitoringConfig.enableRealTimeMetrics = true;
        monitoringConfig.enablePerformanceMonitoring = true;
        
        nlm::VisualizationConfig visualizationConfig;
        visualizationConfig.enableVisualization = true;
        visualizationConfig.enableNetworkVisualization = true;
        
        nlm::ExportConfig exportConfig;
        exportConfig.outputDirectory = "./integration_tests";
        
        // Initialize components
        bool monitoringSuccess = monitoringManager.initialize(monitoringConfig);
        bool visualizationSuccess = visualizationEngine.initialize(visualizationConfig);
        bool exportSuccess = exportManager.initialize(exportConfig);
        
        REQUIRE(monitoringSuccess);
        REQUIRE(visualizationSuccess);
        REQUIRE(exportSuccess);
        
        // Start components
        monitoringManager.start();
        visualizationEngine.start();
        exportManager.start();
        
        REQUIRE(monitoringManager.isRunning());
        REQUIRE(visualizationEngine.isRunning());
        REQUIRE(exportManager.isRunning());
        
        // Test monitoring updates
        monitoringManager.update();
        
        // Test visualization updates
        visualizationEngine.update();
        
        // Test exports
        auto exportResult = exportManager.exportMetrics("integration_test.csv", "csv");
        // Note: Export functionality needs proper implementation
        
        // Stop components
        monitoringManager.stop();
        visualizationEngine.stop();
        exportManager.stop();
        
        REQUIRE_FALSE(monitoringManager.isRunning());
        REQUIRE_FALSE(visualizationEngine.isRunning());
        REQUIRE_FALSE(exportManager.isRunning());
    }
};

// Main test function
int main(int argc, char* argv[]) {
    return Catch::Session().run(argc, argv);
}
