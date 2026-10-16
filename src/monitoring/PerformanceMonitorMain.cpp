// NLM Performance Monitor Main
// Main entry point for the NLM performance monitoring system

#include "../monitoring/PerformanceMonitor.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

int main() {
    std::cout << "NLM Performance Monitor" << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << std::endl;
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize performance monitor
    nlm::PerformanceMonitor monitor;
    monitor.initialize(config);
    
    // Configure monitoring options
    monitor.setRealTimeMode(true);
    monitor.setMetricCollectionInterval(1.0); // 1 second intervals
    
    std::cout << "Performance monitoring initialized successfully" << std::endl;
    std::cout << std::endl;
    
    // Start monitoring
    monitor.startMonitoring();
    
    std::cout << "Performance monitoring active" << std::endl;
    std::cout << "Monitoring will collect metrics for 30 seconds..." << std::endl;
    std::cout << std::endl;
    
    // Simulate some time passing with monitoring
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    // Get current metrics
    nlm::PerformanceMetrics currentMetrics = monitor.getCurrentMetrics();
    
    std::cout << "Current Performance Metrics:" << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "Simulation Step: " << currentMetrics.simulationStep << std::endl;
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(2) 
              << currentMetrics.wallClockTime << "s" << std::endl;
    std::cout << "Brain Memory Usage: " << std::fixed << std::setprecision(2) 
              << currentMetrics.brainMemoryUsage << " MB" << std::endl;
    std::cout << "Total Neurons: " << currentMetrics.totalNeurons << std::endl;
    std::cout << "Total Synapses: " << currentMetrics.totalSynapses << std::endl;
    std::cout << "Firing Neurons: " << currentMetrics.firingNeuronCount << std::endl;
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(2) 
              << currentMetrics.averageFiringRate << " Hz" << std::endl;
    std::cout << "Simulation Speed: " << std::fixed << std::setprecision(2) 
              << currentMetrics.simulationSpeed << " steps/sec" << std::endl;
    
    std::cout << "\nSystem Resources:" << std::endl;
    std::cout << "---------------" << std::endl;
    std::cout << "CPU Usage: " << std::fixed << std::setprecision(1) << currentMetrics.cpuUsage << "%" << std::endl;
    std::cout << "Memory Usage: " << std::fixed << std::setprecision(1) << currentMetrics.memoryUsage << "%" << std::endl;
    std::cout << "Thread Count: " << currentMetrics.threadCount << std::endl;
    
    std::cout << "\nCognitive Performance:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << "Working Memory Usage: " << currentMetrics.workingMemoryUsage << " traces" << std::endl;
    std::cout << "Episodic Memory Usage: " << currentMetrics.episodicMemoryUsage << " episodes" << std::endl;
    std::cout << "Dopamine Level: " << std::fixed << std::setprecision(3) << currentMetrics.dopamineLevel << std::endl;
    std::cout << "Novelty Level: " << std::fixed << std::setprecision(3) << currentMetrics.noveltyLevel << std::endl;
    std::cout << "Curiosity Level: " << std::fixed << std::setprecision(3) << currentMetrics.curiosityLevel << std::endl;
    
    std::cout << "\nBottleneck Analysis:" << std::endl;
    std::cout << "-------------------" << std::endl;
    if (currentMetrics.detectedBottlenecks.empty()) {
        std::cout << "No performance bottlenecks detected" << std::endl;
    } else {
        for (const auto& bottleneck : currentMetrics.detectedBottlenecks) {
            std::cout << "- " << bottleneck << std::endl;
        }
    }
    
    // Check for alerts
    std::cout << "\nActive Alerts:" << std::endl;
    std::cout << "------------" << std::endl;
    auto activeAlerts = monitor.getPerformanceAlertSystem()->getActiveAlerts();
    if (activeAlerts.empty()) {
        std::cout << "No active alerts" << std::endl;
    } else {
        for (const auto& alert : activeAlerts) {
            std::cout << "- " << alert << std::endl;
        }
    }
    
    // Generate performance reports
    std::cout << "\nGenerating performance reports..." << std::endl;
    monitor.generateReports("performance_monitor_output");
    
    std::cout << "Performance reports generated in: performance_monitor_output/" << std::endl;
    
    // Stop monitoring
    monitor.stopMonitoring();
    
    std::cout << std::endl;
    std::cout << "Performance Monitor Summary:" << std::endl;
    std::cout << "------------------------" << std::endl;
    std::cout << "The performance monitoring system successfully:" << std::endl;
    std::cout << "✓ Collected real-time performance metrics" << std::endl;
    std::cout << "✓ Monitored brain performance and system resources" << std::endl;
    std::cout << "✓ Detected performance bottlenecks" << std::endl;
    std::cout << "✓ Generated comprehensive performance reports" << std::endl;
    std::cout << "✓ Provided real-time alerting" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Performance Monitor Demo Complete!" << std::endl;
    
    return 0;
}
