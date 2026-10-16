// Performance Integration Example for NLM Config System

#include "../performance_monitoring/PerformanceMonitor.hpp"
#include "../performance_monitoring/Profiler.hpp"
#include "../core/Config/Config.hpp"

void integratePerformanceWithConfig() {
    std::cout << "=== Performance Integration with NLM Config System ===\n";
    
    // Create config with performance monitoring settings
    auto config = std::make_shared<Config>();
    
    // Performance monitoring configuration
    config->set("performance.enabled", true);
    config->set("performance.monitoring_level", "Comprehensive");
    config->set("performance.max_profile_history", 10000);
    config->set("performance.update_interval_ms", 50);
    config->set("performance.cpu_threshold", 80.0);
    config->set("performance.memory_threshold_mb", 1024);
    config->set("performance.event_threshold", 10000);
    config->set("performance.bottleneck_threshold", 0.9);
    config->set("performance.enable_flame_graphs", true);
    config->set("performance.enable_regression_detection", true);
    config->set("performance.enable_dashboard", true);
    
    // Initialize performance monitor with config
    auto performanceMonitor = std::make_shared<nlm::PerformanceMonitor>(config);
    nlm::PerformanceMonitor::setInstance(performanceMonitor);
    performanceMonitor->initialize();
    
    // Configure profiler with config
    auto profiler = std::make_shared<nlm::Profiler>(config);
    nlm::Profiler::setInstance(profiler);
    
    // Start performance monitoring
    performanceMonitor->startMonitoring();
    profiler->startCallStackProfiling();
    profiler->startMemoryProfiling();
    profiler->startEventProfiling();
    
    std::cout << "Performance monitoring integrated with Config system\n";
    std::cout << "Performance monitoring level: " << 
        config->getOr<std::string>("performance.monitoring_level", "Unknown") << "\n";
    std::cout << "Max profile history: " << 
        config->getOr<size_t>("performance.max_profile_history", 0) << "\n";
    std::cout << "CPU threshold: " << 
        config->getOr<double>("performance.cpu_threshold", 0.0) << "%\n";
    
    // Simulate performance monitoring during config operations
    for (int i = 0; i < 50; ++i) {
        // Record config access performance
        performanceMonitor->startTimer("config_access");
        
        // Simulate config operations
        std::string key = "neuron.count";
        int value = config->getOr<int>(key, 1000);
        
        performanceMonitor->stopTimer("config_access");
        
        performanceMonitor->recordMetric("config.access.time", nlm::PerformanceCategory::Overall,
                                         performanceMonitor->getTimerDuration("config_access").count() / 1000000.0, "seconds");
        
        // Record memory allocation
        performanceMonitor->recordMemoryUsage(1024 * (i + 1), 1024 * (i + 1), 0);
        
        // Record CPU usage
        performanceMonitor->recordCPUTime(std::chrono::microseconds(1000 + rand() % 5000));
        
        // Record profiling data
        profiler->recordAllocation((void*)0x2000, 1024, "Config");
        profiler->recordEvent("config_access", i * 100);
    }
    
    // Generate performance report
    performanceMonitor->generateDetailedReport("/tmp/config_performance_report.txt");
    performanceMonitor->exportPerformanceData("/tmp/config_performance_data.json", nlm::OutputFormat::JSON);
    
    performanceMonitor->stopMonitoring();
    profiler->stopCallStackProfiling();
    profiler->stopMemoryProfiling();
    profiler->stopEventProfiling();
    
    std::cout << "Config system performance monitoring completed\n";
}
