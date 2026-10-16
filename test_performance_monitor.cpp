#include <iostream>
#include "src/core/Performance/PerformanceMonitor.hpp"
#include "src/brain/Neuron.hpp"
#include "src/brain/Synapse.hpp"
#include "src/core/Random/Random.hpp"

int main() {
    std::cout << "=== NLM Performance Monitoring Test ===" << std::endl;
    
    // Initialize performance monitor
    nlm::PerformanceConfig config;
    config.enableRealTimeMonitoring = true;
    config.enableComponentProfiling = true;
    config.enableMemoryTracking = true;
    config.enablePerformanceAlerts = true;
    config.cpuThresholdWarning = 80.0;
    config.cpuThresholdCritical = 90.0;
    config.memoryThresholdWarning = 85.0;
    config.memoryThresholdCritical = 95.0;
    
    nlm::initializePerformanceMonitor(config);
    
    // Test Neuron performance monitoring
    std::cout << "\n1. Testing Neuron Performance Monitoring:" << std::endl;
    
    nlm::RandomGenerator rng;
    
    // Create a neuron with performance monitoring
    nlm::Neuron neuron(1);
    std::cout << "   Created neuron with ID: " << neuron.getId() << std::endl;
    
    // Test step with performance monitoring
    neuron.stepWithPerformanceMonitoring("TestNeuron", 1.0);
    std::cout << "   Called stepWithPerformanceMonitoring" << std::endl;
    
    // Test recordPerformanceMetrics
    neuron.recordPerformanceMetrics("TestNeuron", 5, 2.5f, -65.0f);
    std::cout << "   Called recordPerformanceMetrics" << std::endl;
    
    // Test initializeWithPerformanceTracking
    neuron.initializeWithPerformanceTracking("TestNeuron", rng);
    std::cout << "   Called initializeWithPerformanceTracking" << std::endl;
    
    // Test Synapse performance monitoring
    std::cout << "\n2. Testing Synapse Performance Monitoring:" << std::endl;
    
    nlm::Synapse synapse(1, 1, 2);
    std::cout << "   Created synapse with ID: " << synapse.getId() << std::endl;
    
    // Test step with performance monitoring
    synapse.stepWithPerformanceMonitoring("TestSynapse", 1.0);
    std::cout << "   Called stepWithPerformanceMonitoring" << std::endl;
    
    // Test recordPerformanceMetrics
    synapse.recordPerformanceMetrics("TestSynapse", 3, 0.8f, 0.95f);
    std::cout << "   Called recordPerformanceMetrics" << std::endl;
    
    // Test initializeWithPerformanceTracking
    synapse.initializeWithPerformanceTracking("TestSynapse", rng);
    std::cout << "   Called initializeWithPerformanceTracking" << std::endl;
    
    // Get current metrics
    nlm::PerformanceMetrics metrics = nlm::getCurrentMetrics();
    std::cout << "\n3. Current System Metrics:" << std::endl;
    std::cout << "   CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "   Memory Usage: " << metrics.memoryUsage << "%" << std::endl;
    std::cout << "   Process Memory: " << metrics.processMemory << " bytes" << std::endl;
    std::cout << "   Disk Usage: " << metrics.diskUsage << "%" << std::endl;
    std::cout << "   Network Usage: " << metrics.networkUsage << " Mbps" << std::endl;
    std::cout << "   Temperature: " << metrics.temperature << " C" << std::endl;
    
    // Create a performance checkpoint
    nlm::createPerfCheckpoint("TestRun", "Performance monitoring test");
    std::cout << "\n4. Created performance checkpoint: TestRun" << std::endl;
    
    // Get optimization suggestions
    std::vector<std::string> suggestions = nlm::getPerfOptimizationSuggestions();
    std::cout << "\n5. Optimization Suggestions:" << std::endl;
    if (suggestions.empty()) {
        std::cout << "   No optimization suggestions at this time." << std::endl;
    } else {
        for (const auto& suggestion : suggestions) {
            std::cout << "   - " << suggestion << std::endl;
        }
    }
    
    // Generate performance report
    std::string report = nlm::generatePerfReport();
    std::cout << "\n6. Performance Report Generated:" << std::endl;
    std::cout << "   Report length: " << report.length() << " characters" << std::endl;
    
    // Shutdown performance monitor
    nlm::shutdownPerformanceMonitor();
    std::cout << "\n7. Performance monitor shutdown completed." << std::endl;
    
    std::cout << "\n=== Performance Monitoring Test Complete ===" << std::endl;
    return 0;
}