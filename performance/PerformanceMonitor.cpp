#include "PerformanceMonitor.hpp"
#include "Profiler.hpp"
#include "RealTimeProfiler.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <thread>

using namespace nlm;

void printPerformanceMenu() {
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════╗
║                    NLM Performance Monitoring & Profiling               ║
╠════════════════════════════════════════════════════════════════════════╣
║                                                                       ║
║   Performance Monitoring Options:                                    ║
║   • --performance-monitor           Enable real-time performance monitoring║
║   • --profile-cpu                    CPU profiling with detailed breakdown║
║   • --profile-memory                 Memory usage tracking              ║
║   • --profile-timing                 Precise timing measurements         ║
║   • --profile-all                    Comprehensive profiling session    ║
║   • --perf-report                    Generate performance analysis report ║
║   • --performance-log <file>         Save performance data to file      ║
║                                                                       ║
║   Real-time Monitoring:                                               ║
║   • CPU utilization tracking                                        ║
║   • Memory allocation tracking                                      ║
║   • Simulation step timing                                         ║
║   • Spike event timing                                              ║
║   • Synapse operation frequency                                     ║
║                                                                       ║
║   Optimization Suggestions:                                           ║
║   • Automatic identification of bottlenecks                         ║
║   • Memory leak detection                                          ║
║   • Cache efficiency analysis                                     ║
║   • Parallel processing optimization                              ║
║                                                                       ║
╚════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printPerformanceStatus() {
    auto& monitor = PerformanceMonitor::getInstance();
    
    std::cout << R"(
╔════════════════════════════════════════════════════════════════════════╗
║                      Performance Status Report                         ║
╠════════════════════════════════════════════════════════════════════════╣
║                                                                       ║
║   Simulation Status:                                                  ║
║   • Current step: " << monitor.getCurrentStep() << "                         ║
║   • Current time: " << std::fixed << std::setprecision(3) << monitor.getCurrentTime() << "s      ║
║   • Total neurons: " << monitor.getTotalNeuronCount() << "                     ║
║   • Total synapses: " << monitor.getTotalSynapseCount() << "                   ║
║   • Active neurons: " << monitor.getActiveNeuronCount() << "                  ║
║   • Firing neurons: " << monitor.getFiringNeuronCount() << "                  ║
║                                                                       ║
║   Performance Metrics:                                                ║
║   • Steps per second: " << std::fixed << std::setprecision(1) << monitor.getStepsPerSecond() << "  ║
║   • Spikes per step: " << std::fixed << std::setprecision(2) << monitor.getSpikesPerStep() << "   ║
║   • Efficiency ratio: " << std::fixed << std::setprecision(3) << monitor.getEfficiencyRatio() << "║
║                                                                       ║
║   Memory Usage:                                                       ║
║   • Memory usage: " << std::fixed << std::setprecision(2) << monitor.getMemoryUsageMB() << "MB   ║
║   • Allocations/sec: " << monitor.getAllocationsPerSecond() << "              ║
║   • Peak memory: " << std::fixed << std::setprecision(2) << monitor.getPeakMemoryMB() << "MB║
║                                                                       ║
║   System Resources:                                                   ║
║   • CPU utilization: " << std::fixed << std::setprecision(1) << monitor.getCPUUtilization() << "%║
║   • Thread count: " << monitor.getThreadCount() << "                        ║
║                                                                       ║
╚════════════════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void printPerformanceHelp() {
    std::cout << R"(
=== NLM Performance Monitoring Help ===

Performance Monitoring Commands:

1. --performance-monitor
   Enable real-time performance monitoring during simulation.
   Continuously tracks CPU, memory, and simulation metrics.

2. --profile-cpu
   Enable CPU profiling with detailed breakdown by function.
   Identifies performance bottlenecks and hotspots.

3. --profile-memory
   Track memory allocation patterns and detect leaks.
   Monitors memory usage trends over time.

4. --profile-timing
   Precise timing measurements of all operations.
   Tracks simulation step times and event processing.

5. --profile-all
   Comprehensive profiling including CPU, memory, and timing.
   Complete performance analysis for optimization.

6. --perf-report
   Generate detailed performance analysis report.
   Includes benchmarks, bottlenecks, and optimization recommendations.

7. --performance-log <file>
   Save performance data to specified file for offline analysis.

8. --help-perf
   Display this performance monitoring help.

Usage Examples:

  ./nlm --performance-monitor --profile-cpu
  ./nlm --profile-all --performance-log perf_data.json
  ./nlm --perf-report --neuron_count=1000
  ./nlm --profile-timing --steps=5000

Configuration Options:

  --perf_sample_rate=N    Sampling rate for monitoring (Hz)
  --perf_min_samples=N    Minimum samples for meaningful analysis
  --enable_cpu_profiling  Enable CPU profiling
  --enable_memory_profiling Enable memory profiling
  --enable_timing_profiling Enable timing profiling
  --auto_optimize        Automatically suggest optimizations
  --perf_threshold=N     Performance threshold for alerts

Performance Metrics Tracked:

• Simulation Throughput: Steps per second
• Neural Activity: Spikes per step, firing neuron percentage
• Memory Efficiency: Allocations per step, memory growth rate
• CPU Utilization: Percentage of CPU time used
• Network Connectivity: Synapses per neuron, connection density
• Processing Time: Average time per simulation step
• Spike Processing: Time to process spike events
• Synaptic Dynamics: Weight update times

Optimization Recommendations:

• Parallel Processing: Use multiple threads for independent operations
• Memory Pooling: Reduce allocation overhead with object pools
• Cache Optimization: Improve data locality for better cache hits
• Algorithmic Improvements: Replace O(n^2) with O(n log n) algorithms
• Vectorization: Use SIMD instructions for parallel operations
• Data Structure Optimization: Use hash maps instead of linear searches
• Load Balancing: Distribute work evenly across processing units

Performance Alert Levels:

• WARNING: Metrics exceed 80% of optimal values
• ERROR: Metrics exceed 95% of optimal values
• CRITICAL: Metrics exceed 100% of optimal values or indicate system issues

Press Ctrl+C to stop monitoring and return to main menu.
    )" << std::endl;
}

int main(int argc, char** argv) {
    printPerformanceMenu();
    printPerformanceHelp();
    
    // Initialize performance monitoring
    PerformanceMonitor::getInstance().initialize();
    
    // Parse performance monitoring options
    bool enableMonitor = false;
    bool enableCPUProfile = false;
    bool enableMemoryProfile = false;
    bool enableTimingProfile = false;
    bool enableAllProfiles = false;
    bool generateReport = false;
    std::string logFile = "";
    size_t sampleRate = 1000;  // Hz
    size_t minSamples = 100;
    bool autoOptimize = false;
    float perfThreshold = 95.0f;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--performance-monitor") {
            enableMonitor = true;
        } else if (arg == "--profile-cpu") {
            enableCPUProfile = true;
        } else if (arg == "--profile-memory") {
            enableMemoryProfile = true;
        } else if (arg == "--profile-timing") {
            enableTimingProfile = true;
        } else if (arg == "--profile-all") {
            enableAllProfiles = true;
            enableMonitor = true;
            enableCPUProfile = true;
            enableMemoryProfile = true;
            enableTimingProfile = true;
        } else if (arg == "--perf-report") {
            generateReport = true;
        } else if (arg == "--performance-log") {
            if (i + 1 < argc) {
                logFile = argv[++i];
            }
        } else if (arg == "--help-perf") {
            printPerformanceHelp();
            return 0;
        } else if (arg == "--perf_sample_rate" && i + 1 < argc) {
            sampleRate = std::stoul(argv[++i]);
        } else if (arg == "--perf_min_samples" && i + 1 < argc) {
            minSamples = std::stoul(argv[++i]);
        } else if (arg == "--enable_cpu_profiling") {
            enableCPUProfile = true;
        } else if (arg == "--enable_memory_profiling") {
            enableMemoryProfile = true;
        } else if (arg == "--enable_timing_profiling") {
            enableTimingProfile = true;
        } else if (arg == "--auto_optimize") {
            autoOptimize = true;
        } else if (arg == "--perf_threshold" && i + 1 < argc) {
            perfThreshold = std::stof(argv[++i]);
        }
    }
    
    // Configure performance monitoring based on options
    PerformanceMonitor::getInstance().configure(
        sampleRate,
        minSamples,
        enableCPUProfile,
        enableMemoryProfile,
        enableTimingProfile
    );
    
    if (enableAllProfiles) {
        PerformanceMonitor::getInstance().setProfilingLevel(ProfilingLevel::COMPREHENSIVE);
    } else if (enableCPUProfile && enableMemoryProfile && enableTimingProfile) {
        PerformanceMonitor::getInstance().setProfilingLevel(ProfilingLevel::ADVANCED);
    } else if (enableCPUProfile || enableMemoryProfile || enableTimingProfile) {
        PerformanceMonitor::getInstance().setProfilingLevel(ProfilingLevel::STANDARD);
    }
    
    // Start performance monitoring
    PerformanceMonitor::getInstance().start();
    
    std::cout << "Performance monitoring started." << std::endl;
    
    // Display current performance status
    printPerformanceStatus();
    
    // Simulate some performance-intensive work for demonstration
    std::cout << std::endl;
    std::cout << "Running performance demonstration..." << std::endl;
    
    // Run a simple benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 1000; ++i) {
        // Simulate neural computation
        volatile double result = std::sin(i * 0.01) * std::cos(i * 0.02);
        (void)result;  // Suppress unused variable warning
        
        // Monitor performance periodically
        if (i % 100 == 0) {
            PerformanceMonitor::getInstance().recordStep(i, i * 0.001);
            printPerformanceStatus();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "Performance demonstration complete." << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(2) << elapsed << " seconds" << std::endl;
    std::cout << "Operations per second: " << std::fixed << std::setprecision(0) << (1000 / elapsed) << std::endl;
    
    // Generate performance report if requested
    if (generateReport) {
        std::cout << std::endl;
        std::cout << "Generating performance report..." << std::endl;
        PerformanceMonitor::getInstance().generateReport("performance_report.html");
        std::cout << "Report generated: performance_report.html" << std::endl;
    }
    
    // Save performance data if log file specified
    if (!logFile.empty()) {
        std::cout << std::endl;
        std::cout << "Saving performance data to: " << logFile << std::endl;
        PerformanceMonitor::getInstance().saveData(logFile);
        std::cout << "Performance data saved successfully." << std::endl;
    }
    
    // Stop performance monitoring
    PerformanceMonitor::getInstance().stop();
    
    std::cout << std::endl;
    std::cout << "Performance monitoring complete." << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();
    
    return 0;
}
