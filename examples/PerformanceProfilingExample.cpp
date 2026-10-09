# Example: NLM Performance Profiling Integration

This example demonstrates how to integrate the performance profiling framework into an existing NLM simulation.

```cpp
#include "performance/PerformanceProfiler.hpp"
#include "performance/NeuralPerformanceAnalyzer.hpp"
#include "brain/Brain.hpp"
#include <thread>
#include <chrono>

void exampleBasicProfiling() {
    // Initialize performance profiler
    nlm::PerformanceProfiler profiler;
    profiler.startProfiling();
    
    // Configure for neural simulation
    profiler.enableComponent("cpu_profiling", true);
    profiler.enableComponent("memory_profiling", true);
    profiler.enableComponent("thread_profiling", true);
    profiler.setMetricsLevel(3);  // High detail
    
    // Create brain simulation
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 10000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run simulation with profiling
    for (uint64_t step = 0; step < 10000; ++step) {
        brain->step(step, step * 0.001);
        
        // Collect performance metrics every 100 steps
        if (step % 100 == 0) {
            auto metrics = profiler.getPerformanceMetrics();
            auto stats = profiler.getPerformanceStats();
            
            // Log performance information
            std::cout << "Step " << step << ": "
                     << "Neurons/sec: " << stats.neuronsPerSecond << ", "
                     << "Spikes/sec: " << stats.spikesPerSecond << ", "
                     << "Memory: " << stats.memoryUsageMB << " MB" << std::endl;
        }
    }
    
    // Generate performance report
    std::string report = profiler.generatePerformanceReport();
    std::ofstream("performance_report.html") << report;
    
    profiler.stopProfiling();
}

void exampleNeuralAnalysis() {
    nlm::NeuralPerformanceAnalyzer analyzer;
    
    // Create sample performance metrics
    nlm::PerformanceMetrics metrics;
    metrics.stepId = 1000;
    metrics.timestamp = 1000000;
    metrics.realTime = 100.0;  // 100 seconds
    metrics.cpuTime = 95.0;    // 95 seconds
    metrics.stepsPerSecond = 10.0;
    metrics.spikeRate = 50000.0;
    metrics.activeNeurons = 8000;
    metrics.totalNeurons = 10000;
    metrics.firingNeurons = 500;
    metrics.realTimeFactor = 0.95;
    metrics.memoryUtilization = 0.75;
    metrics.cpuUtilization = 0.85;
    metrics.cacheHitRate = 0.92;
    
    // Analyze performance
    std::cout << "Performance Analysis:" << std::endl;
    
    std::vector<std::string> issues = analyzer.identifyPerformanceIssues(metrics);
    std::cout << "Performance Issues:" << std::endl;
    for (const auto& issue : issues) {
        std::cout << "  - " << issue << std::endl;
    }
    
    std::map<std::string, double> recommendations = 
        analyzer.getOptimizationRecommendations(metrics);
    std::cout << "Optimization Recommendations:" << std::endl;
    for (const auto& rec : recommendations) {
        std::cout << "  - " << rec.first << ": " << rec.second << std::endl;
    }
    
    // Trend analysis
    nlm::NeuralPerformanceAnalyzer::NeuralPerformanceTrend trend = 
        analyzer.analyzeTrends(metrics);
    std::cout << "Performance Trends:" << std::endl;
    std::cout << "  - Neuron scaling: " << trend.neuronScaling << std::endl;
    std::cout << "  - Spike efficiency: " << trend.spikeEfficiency << std::endl;
    std::cout << "  - Memory efficiency: " << trend.memoryEfficiency << std::endl;
}

void exampleMemoryProfiling() {
    nlm::MemoryProfiler memoryProfiler;
    memoryProfiler.startProfiling();
    
    // Simulate memory allocations
    std::vector<float> testData(1000000, 1.0f);
    memoryProfiler.recordAllocation(
        testData.size() * sizeof(float), 
        __FILE__, __LINE__, 
        "neural_data"
    );
    
    // Simulate some operations
    for (int i = 0; i < 100000; ++i) {
        testData[i % testData.size()] *= 2.0f;
    }
    
    // Check for memory leaks
    std::vector<nlm::MemoryProfiler::LeakInfo> leaks = memoryProfiler.detectLeaks();
    if (!leaks.empty()) {
        std::cout << "Memory leaks detected: " << leaks.size() << std::endl;
        for (const auto& leak : leaks) {
            std::cout << "  - Address: 0x" << std::hex << leak.address
                     << " Size: " << leak.size << " bytes" << std::endl;
        }
    }
    
    memoryProfiler.stopProfiling();
}

void exampleCPUProfiling() {
    nlm::CPUProfiler cpuProfiler;
    cpuProfiler.startProfiling();
    
    // Simulate CPU-intensive work
    double sum = 0.0;
    for (int i = 0; i < 10000000; ++i) {
        sum += std::sin(i * 0.001) * std::cos(i * 0.002);
    }
    
    cpuProfiler.stopProfiling();
    
    // Analyze CPU performance
    auto snapshots = cpuProfiler.getCPUHistory(100);
    if (!snapshots.empty()) {
        auto latest = cpuProfiler.getLatestSnapshot();
        std::cout << "CPU Performance:" << std::endl;
        std::cout << "  - CPU Usage: " << cpuProfiler.getCPUUsage() * 100 << "%" << std::endl;
        std::cout << "  - Instructions per Second: " << cpuProfiler.getInstructionPerSecond() << std::endl;
        std::cout << "  - Cache Miss Rate: " << cpuProfiler.getCacheMissRate() * 100 << "%" << std::endl;
        std::cout << "  - Hot Functions:" << std::endl;
        auto hotFunctions = cpuProfiler.getMostActiveFunctions(10);
        for (const auto& func : hotFunctions) {
            std::cout << "    - " << func << std::endl;
        }
    }
}

void exampleDashboardUsage() {
    nlm::PerformanceDashboard dashboard;
    
    // Create sample metrics
    nlm::PerformanceMetrics metrics;
    metrics.stepId = 1;
    metrics.realTime = 10.0;
    metrics.cpuTime = 9.5;
    metrics.stepsPerSecond = 1000.0;
    metrics.spikeRate = 50000.0;
    metrics.activeNeurons = 5000;
    metrics.totalNeurons = 10000;
    metrics.firingNeurons = 250;
    
    // Update dashboard
    dashboard.update(metrics);
    
    // Generate HTML dashboard
    std::string html = dashboard.generateHTMLDashboard();
    std::ofstream("dashboard.html") << html;
    
    std::cout << "Dashboard generated: dashboard.html" << std::endl;
    std::cout << "Open in browser to view real-time performance metrics." << std::endl;
}

int main() {
    std::cout << "=== NLM Performance Profiling Example ===" << std::endl;
    
    std::cout << "\n1. Basic Profiling Example:" << std::endl;
    exampleBasicProfiling();
    
    std::cout << "\n2. Neural Analysis Example:" << std::endl;
    exampleNeuralAnalysis();
    
    std::cout << "\n3. Memory Profiling Example:" << std::endl;
    exampleMemoryProfiling();
    
    std::cout << "\n4. CPU Profiling Example:" << std::endl;
    exampleCPUProfiling();
    
    std::cout << "\n5. Dashboard Example:" << std::endl;
    exampleDashboardUsage();
    
    std::cout << "\n=== All Examples Completed Successfully ===" << std::endl;
    return 0;
}
```

This example demonstrates the key features of the performance profiling framework:

1. **Basic Profiling**: Start/stop monitoring and collect performance metrics
2. **Neural Analysis**: Analyze neural-specific performance metrics
3. **Memory Profiling**: Track memory allocations and detect leaks
4. **CPU Profiling**: Sample and analyze CPU performance
5. **Dashboard Usage**: Generate real-time performance dashboards

To use this example:

1. Ensure the performance profiling libraries are built
2. Include the necessary headers
3. Link with the profiling libraries
4. Compile and run with appropriate flags:

```bash
g++ -std=c++20 -I./include -L./lib example.cpp -lnlm_performance -o example
```

The performance profiling framework provides comprehensive monitoring capabilities for NLM brain emulation simulations, enabling:

- Real-time performance monitoring
- Detailed profiling of CPU, memory, and I/O operations
- Neural-specific performance analysis
- Performance optimization recommendations
- Automated regression testing
- Real-time dashboard visualization