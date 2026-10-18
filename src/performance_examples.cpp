#include "performance_monitor_integration.h"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include <fstream>
#include <iostream>

namespace nlm {

void createPerformanceCLICommands() {
    std::cout << "=== NLM Performance Monitoring CLI Commands ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Core Performance Monitoring:" << std::endl;
    std::cout << "  --profile                  Enable performance profiling" << std::endl;
    std::cout << "  --benchmark               Run performance benchmarks" << std::endl;
    std::cout << "  --benchmark-suite         Run complete benchmark suite" << std::endl;
    std::cout << "  --performance-report       Generate comprehensive performance report" << std::endl;
    std::cout << "  --performance-stats        Show performance statistics" << std::endl;
    std::cout << "  --performance-optimize     Run performance optimization" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Resource Monitoring:" << std::endl;
    std::cout << "  --resource-monitor         Enable real-time resource monitoring" << std::endl;
    std::cout << "  --resource-report          Generate resource usage report" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Advanced Profiling:" << std::endl;
    std::cout << "  --profile-config           Load profiling configuration" << std::endl;
    std::cout << "  --event-driven             Enable event-driven performance profiling" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Output Formats:" << std::endl;
    std::cout << "  Reports can be generated in: text, json, html, csv formats" << std::endl;
    std::cout << std::endl;
}

void demonstratePerformanceMonitoring() {
    std::cout << "=== NLM Performance Monitoring Demonstration ===" << std::endl;
    std::cout << std::endl;
    
    // Create performance integration
    auto integration = std::make_shared<PerformanceIntegration>();
    
    // Setup performance monitoring
    integration->setupCoreMonitoring("configs/default.cfg");
    
    // Create a simple brain for demonstration
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Integrate with brain
    integration->integrateWithBrain(brain);
    
    std::cout << "Demonstrating performance monitoring features:" << std::endl;
    std::cout << "1. Core timing profiler - measuring brain operations" << std::endl;
    std::cout << "2. Memory profiler - tracking memory usage" << std::endl;
    std::cout << "3. CPU profiler - monitoring CPU utilization" << std::endl;
    std::cout << "4. I/O profiler - tracking file I/O operations" << std::endl;
    std::cout << "5. Event profiler - profiling event-driven processing" << std::endl;
    std::cout << std::endl;
    
    // Profile a simulation
    integration->profileSimulation(brain, 50);
    
    std::cout << std::endl;
    std::cout << "Demonstration complete. Performance reports have been generated." << std::endl;
}

void generatePerformanceCLIExample() {
    std::cout << "=== Example NLM Performance Monitoring CLI Usage ===" << std::endl;
    std::cout << std::endl;
    std::cout << "# Enable comprehensive performance profiling with automatic reporting" << std::endl;
    std::cout << "nlm --profile --steps 1000" << std::endl;
    std::cout << std::endl;
    
    std::cout << "# Run performance benchmarks to find bottlenecks" << std::endl;
    std::cout << "nlm --benchmark --benchmark-suite" << std::endl;
    std::cout << std::endl;
    
    std::cout << "# Generate detailed performance analysis report" << std::endl;
    std::cout << "nlm --performance-report" << std::endl;
    std::endl;
    
    std::cout << "# Enable real-time resource monitoring" << std::endl;
    std::cout << "nlm --resource-monitor" << std::endl;
    std::endl;
    
    std::cout << "# Run performance optimization" << std::endl;
    std::cout << "nlm --performance-optimize" << std::endl;
    std::endl;
    
    std::cout << "# Combined usage for comprehensive performance analysis" << std::endl;
    std::cout << "nlm --profile --benchmark --performance-report --performance-optimize" << std::endl;
}

} // namespace nlm
