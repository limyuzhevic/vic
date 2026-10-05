// Test EnhancedStatusReporter functionality
// Verifies status monitoring, component tracking, and alert systems

#include "EnhancedStatusReporter.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_enhanced_status_reporter {

void testComponentStatusManagement() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test component status addition
    nlm::SystemComponentStatus brainStatus("brain", true, "active", 0.95, "Operational");
    nlm::SystemComponentStatus memoryStatus("memory", true, "idle", 0.8, "Normal usage");
    nlm::SystemComponentStatus cpuStatus("cpu", true, "active", 0.7, "Under moderate load");
    
    reporter.addComponentStatus(brainStatus);
    reporter.addComponentStatus(memoryStatus);
    reporter.addComponentStatus(cpuStatus);
    
    // Test component status updates
    reporter.updateComponentStatus("brain", true, "active", 0.97, "Excellent performance");
    reporter.updateComponentStatus("memory", true, "idle", 0.82, "Low memory usage");
    
    // Test component status removal
    reporter.removeComponentStatus("cpu");
    
    std::cout << "✓ Component status management test passed" << std::endl;
}

void testResourceMonitoring() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test resource monitoring
    nlm::ResourceMetrics metrics;
    metrics.cpuUsage = 75.5;
    metrics.memoryUsage = 512.0;
    metrics.memoryAvailable = 1536.0;
    metrics.memoryReserved = 128.0;
    metrics.diskUsage = 2048.0;
    metrics.networkUsage = 10.5;
    metrics.threadCount = 15;
    metrics.openFileHandles = 42;
    
    reporter.updateResourceMetrics(metrics);
    
    // Test resource monitoring activation
    reporter.startResourceMonitoring(std::chrono::milliseconds(500));
    
    // Simulate resource changes
    for (int i = 0; i < 5; ++i) {
        nlm::ResourceMetrics newMetrics;
        newMetrics.cpuUsage = 60.0 + (rand() % 30);
        newMetrics.memoryUsage = 400.0 + (rand() % 200);
        newMetrics.diskUsage = 1500.0 + (rand() % 500);
        reporter.updateResourceMetrics(newMetrics);
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }
    
    reporter.stopResourceMonitoring();
    
    std::cout << "✓ Resource monitoring test passed" << std::endl;
}

void testBrainStateReporting() {
    nlm::EnhancedStatusReporter reporter;
    
    // Create a mock brain object
    class MockBrain {
    public:
        size_t getNeuronCount() const { return 1000; }
        size_t getSynapseCount() const { return 5000; }
        size_t getFiringCount() const { return 150; }
        float getAverageFiringRate() const { return 8.5f; }
        float getAverageMembranePotential() const { return -65.2f; }
        float getAverageInputCurrent() const { return 2.3f; }
        float getEnergyConsumption() const { return 45.6f; }
        float getComputationalLoad() const { return 0.78f; }
    };
    
    MockBrain mockBrain;
    nlm::BrainStateMetrics brainMetrics;
    
    brainMetrics.neuronCount = mockBrain.getNeuronCount();
    brainMetrics.synapseCount = mockBrain.getSynapseCount();
    brainMetrics.firingCount = mockBrain.getFiringCount();
    brainMetrics.averageFiringRate = mockBrain.getAverageFiringRate();
    brainMetrics.averageMembranePotential = mockBrain.getAverageMembranePotential();
    brainMetrics.averageInputCurrent = mockBrain.getAverageInputCurrent();
    brainMetrics.energyConsumption = mockBrain.getEnergyConsumption();
    brainMetrics.computationalLoad = mockBrain.getComputationalLoad();
    
    reporter.updateBrainState(brainMetrics);
    
    std::cout << "✓ Brain state reporting test passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test configuration validation
    nlm::ValidationResult validation1("neural_network", true);
    validation1.warnings.push_back("Consider increasing neuron count for better accuracy");
    
    nlm::ValidationResult validation2("simulation", false);
    validation2.errors.push_back("Invalid time step value");
    validation2.errors.push_back("Duration must be positive");
    
    reporter.addValidationResult(validation1);
    reporter.addValidationResult(validation2);
    
    // Test validation error and warning addition
    reporter.addValidationError("database", "Connection timeout");
    reporter.addValidationWarning("visualization", "Color scheme may not be optimal for low light");
    
    std::cout << "✓ Configuration validation test passed" << std::endl;
}

void testPerformanceMetrics() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test performance metrics
    nlm::PerformanceMetrics performanceMetrics;
    performanceMetrics.cpuPerformance = 0.92;
    performanceMetrics.memoryEfficiency = 0.85;
    performanceMetrics.throughput = 1250.5;
    performanceMetrics.latency = 45.2;
    performanceMetrics.reliability = 0.98;
    
    reporter.updatePerformanceMetrics(performanceMetrics);
    
    // Test performance measurement
    reporter.addPerformanceMeasurement("synapse_update", 0.023, 2048);
    reporter.addPerformanceMeasurement("neuron_activation", 0.015, 1024);
    
    std::cout << "✓ Performance metrics test passed" << std::endl;
}

void testLiveMonitoring() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test live monitoring
    for (int i = 0; i < 10; ++i) {
        nlm::MonitoringDataPoint dataPoint;
        dataPoint.component = (i % 3 == 0) ? "cpu" : ((i % 3 == 1) ? "memory" : "brain");
        dataPoint.metric = (i % 2 == 0) ? "usage" : "performance";
        dataPoint.value = static_cast<double>(rand() % 100);
        dataPoint.unit = "%";
        dataPoint.tags = {{"source", "sensor_1"}, {"type", "raw"}};
        
        reporter.addMonitoringDataPoint(dataPoint);
    }
    
    // Test data retrieval
    auto cpuData = reporter.getMonitoringData("cpu", "usage");
    assert(!cpuData.empty());
    
    auto memoryData = reporter.getMonitoringData("memory", "performance");
    assert(!memoryData.empty());
    
    std::cout << "✓ Live monitoring test passed" << std::endl;
}

void testAlertSystem() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test alert thresholds
    std::map<std::string, double> thresholds;
    thresholds["cpu_usage"] = 80.0;
    thresholds["memory_usage"] = 90.0;
    thresholds["throughput"] = 1000.0;
    
    reporter.setAlertThresholds(thresholds);
    
    // Trigger resource alerts
    reporter.updateResourceMetrics(nlm::ResourceMetrics()); // Default metrics with high values
    
    // Check alerts
    reporter.checkAlerts();
    
    auto alerts = reporter.getAlerts();
    assert(!alerts.empty());
    
    std::cout << "✓ Alert system test passed" << std::endl;
}

void testDashboard() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test dashboard functionality
    assert(!reporter.isDashboardRunning());
    
    reporter.startDashboard(std::chrono::seconds(2));
    
    // Let dashboard run briefly
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::string dashboardOutput = reporter.generateDashboardOutput();
    assert(!dashboardOutput.empty());
    assert(dashboardOutput.find("Dashboard") != std::string::npos);
    
    reporter.stopDashboard();
    
    std::cout << "✓ Dashboard test passed" << std::endl;
}

void testReportGeneration() {
    nlm::EnhancedStatusReporter reporter;
    
    // Add test data
    reporter.updateResourceMetrics(nlm::ResourceMetrics());
    reporter.addValidationError("test", "Test error");
    
    // Test system status report generation
    std::string systemReport = reporter.generateSystemStatusReport();
    assert(!systemReport.empty());
    
    // Test brain state report generation
    std::string brainReport = reporter.generateBrainStateReport();
    assert(!brainReport.empty());
    
    // Test resource report generation
    std::string resourceReport = reporter.generateResourceReport();
    assert(!resourceReport.empty());
    
    // Test validation report generation
    std::string validationReport = reporter.generateConfigurationValidationReport();
    assert(!validationReport.empty());
    
    // Test performance report generation
    std::string performanceReport = reporter.generatePerformanceReport();
    assert(!performanceReport.empty());
    
    // Test monitoring report generation
    std::string monitoringReport = reporter.generateLiveMonitoringReport();
    assert(!monitoringReport.empty());
    
    // Test summary report generation
    std::string summaryReport = reporter.generateSummaryReport();
    assert(!summaryReport.empty());
    
    std::cout << "✓ Report generation test passed" << std::endl;
}

void testReportExport() {
    nlm::EnhancedStatusReporter reporter;
    
    // Add test data
    reporter.updateResourceMetrics(nlm::ResourceMetrics());
    
    // Test report export
    bool exportSuccess = reporter.exportStatusReport("test_status_report.json", "json", "comprehensive");
    assert(exportSuccess);
    
    std::cout << "✓ Report export test passed" << std::endl;
}

void testStatisticsMethods() {
    nlm::EnhancedStatusReporter reporter;
    
    // Test statistics methods
    double avgCpu = reporter.getAverageCPUUsage();
    double peakCpu = reporter.getPeakCPUUsage();
    double avgMemory = reporter.getAverageMemoryUsage();
    double peakMemory = reporter.getPeakMemoryUsage();
    
    assert(avgCpu >= 0.0 && avgCpu <= 100.0);
    assert(peakCpu >= avgCpu);
    assert(avgMemory >= 0.0);
    assert(peakMemory >= avgMemory);
    
    std::cout << "✓ Statistics methods test passed" << std::endl;
}

void runAll() {
    std::cout << "=== Testing EnhancedStatusReporter ===" << std::endl;
    testComponentStatusManagement();
    testResourceMonitoring();
    testBrainStateReporting();
    testConfigurationValidation();
    testPerformanceMetrics();
    testLiveMonitoring();
    testAlertSystem();
    testDashboard();
    testReportGeneration();
    testReportExport();
    testStatisticsMethods();
    
    std::cout << "=== All EnhancedStatusReporter Tests PASSED ===" << std::endl;
}

} // namespace test_enhanced_status_reporter
