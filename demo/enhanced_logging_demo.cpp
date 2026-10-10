// Example demonstrating enhanced NLM logging and profiling capabilities
// This file shows how to use the new profiling timer classes, system monitoring capabilities,
// debug facilities, and enhanced logging features.

#include "core/EnhancedLogger.hpp"
#include "profiling/ProfilingTimer.hpp"
#include "debug/SystemMonitor.hpp"
#include "debug/LoggingEnhancement.hpp"

void demonstrateProfilingTimer() {
    std::cout << "=== Demonstrating Profiling Timer Classes ===\n" << std::endl;
    
    // Create and start a profiling timer
    nlm::ProfilingTimer timer("NeuralSimulation", true);
    
    // Simulate some work
    for (int i = 0; i < 100000; ++i) {
        double result = std::sqrt(static_cast<double>(i));
    }
    
    // Stop the timer and get elapsed time
    double elapsed = timer.stop();
    
    std::cout << "Timer: " << timer.getName() << std::endl;
    std::cout << "Elapsed time: " << timer.formatElapsed() << std::endl;
    std::cout << "Elapsed (ms): " << elapsed << std::endl;
    std::cout << "Elapsed (seconds): " << elapsed / 1000.0 << std::endl;
    
    // Demonstrate BenchmarkTimer
    std::cout << "\n=== Demonstrating BenchmarkTimer ===\n" << std::endl;
    
    nlm::BenchmarkTimer benchmark("NeuronUpdateBenchmark");
    benchmark.start();
    
    // Simulate multiple measurements
    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        benchmark.checkpoint("Phase" + std::to_string(i));
    }
    
    benchmark.stop();
    
    std::cout << "Benchmark results:\n";
    std::cout << benchmark.exportToString();
}

void demonstrateSystemMonitoring() {
    std::cout << "\n=== Demonstrating System Monitoring Capabilities ===\n" << std::endl;
    
    // Register system monitors
    auto cpuMonitor = std::make_shared<nlm::CPUMonitor>();
    auto memoryMonitor = std::make_shared<nlm::MemoryMonitor>();
    auto networkMonitor = std::make_shared<nlm::NetworkIOMonitor>();
    
    nlm::SystemMonitorRegistry& registry = nlm::SystemMonitorRegistry::getInstance();
    registry.registerMonitor(cpuMonitor);
    registry.registerMonitor(memoryMonitor);
    registry.registerMonitor(networkMonitor);
    
    std::cout << "Registered monitors:" << std::endl;
    for (const auto& monitor : registry.getAllMonitors()) {
        std::cout << "  - " << monitor->getName() << " (" << monitor->getUnit() << ")" << std::endl;
    }
    
    // Start monitoring
    registry.startAllMonitoring();
    
    // Simulate some work and monitor
    std::cout << "\nMonitoring system resources (simulating work)...\n" << std::endl;
    for (int i = 0; i < 3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        auto health = registry.getSystemHealth();
        std::cout << "Sample " << i + 1 << ": CPU=" << health.cpuUsage << "%"
                  << ", Memory=" << health.memoryUsage << " bytes"
                  << ", Healthy=" << (health.isHealthy ? "Yes" : "No") << std::endl;
    }
    
    // Stop monitoring
    registry.stopAllMonitoring();
    
    // Export system status
    std::cout << "\nSystem status:\n" << registry.exportSystemStatus();
}

void demonstrateDebugFacilities() {
    std::cout << "\n=== Demonstrating Debug Facilities ===\n" << std::endl;
    
    // Memory leak detection
    nlm::MemoryLeakDetector leakDetector;
    
    // Track allocations
    int* ptr1 = new int(42);
    leakDetector.trackAllocation(ptr1, sizeof(int), "NeuronAllocation");
    
    int* ptr2 = new int[10];
    leakDetector.trackAllocation(ptr2, 10 * sizeof(int), "ArrayAllocation");
    
    // Track deallocation
    delete ptr1;
    leakDetector.trackDeallocation(ptr1, "NeuronDeallocation");
    
    // Check for leaks
    bool hasLeaks = leakDetector.checkForLeaks();
    std::cout << "Memory leaks detected: " << (hasLeaks ? "Yes" : "No") << std::endl;
    std::cout << "Leak report:\n" << leakDetector.exportLeakReport();
    
    // Resource allocation tracking
    nlm::ResourceAllocationTracker allocTracker;
    allocTracker.trackMemoryAllocation(1024, "InitialAllocation");
    allocTracker.trackCPUAllocation(0.5, "Processing");
    allocTracker.trackFileOperation("read", "brain.dat", "FileLoad");
    
    std::cout << "\nResource allocation report:\n" << allocTracker.exportResourceReport();
    
    // Reference counting debugger
    nlm::ReferenceCountingDebugger refDebugger;
    refDebugger.trackReferenceIncrement("Neuron1", "Creation");
    refDebugger.trackReferenceIncrement("Neuron2", "Creation");
    refDebugger.trackReferenceDecrement("Neuron1", "Destruction");
    
    std::cout << "\nReference counting report:\n" << refDebugger.exportReferenceReport();
}

void demonstrateEnhancedLogging() {
    std::cout << "\n=== Demonstrating Enhanced Logging ===\n" << std::endl;
    
    // Structured logging
    nlm::StructuredLogger structuredLogger;
    
    // Log structured data
    structuredLogger.logStructured("Neural", "SynapseUpdate", {
        {"preNeuron", "42"},
        {"postNeuron", "17"},
        {"weightChange", "0.05"},
        {"plasticityRule", "STDP"}
    });
    
    structuredLogger.logStructured("System", "MemoryAllocation", {
        {"size", "1024"},
        {"context", "NeuronPool"},
        {"allocator", "MemoryPool"}
    });
    
    std::cout << "Structured logs exported:\n";
    std::cout << structuredLogger.exportStructuredLogs() << std::endl;
    
    // Performance logging
    structuredLogger.logPerformance("NeuronUpdate", {
        {"executionTime", 12.5},
        {"neuronCount", 500},
        {"spikeCount", 47},
        {"plasticityTime", 2.3}
    }, "Phase1_Simulation");
    
    std::cout << "Performance logs exported:\n";
    std::cout << structuredLogger.exportPerformanceLogs() << std::endl;
}

void demonstrateDebugVisualization() {
    std::cout << "\n=== Demonstrating Debug Visualization ===\n" << std::endl;
    
    nlm::DebugVisualizer visualizer;
    
    // Add data points
    visualizer.addDataPoint("Performance", "NeuronProcessing", 85.5, "ms");
    visualizer.addDataPoint("Memory", "Usage", 512.0, "MB");
    visualizer.addDataPoint("CPU", "Utilization", 45.2, "%");
    visualizer.addDataPoint("I/O", "Operations", 1024.0, "count");
    
    // Add debug snapshots
    visualizer.addDebugSnapshot("NeuronState", "Vmem: 0.1, Refractory: 0, Firing: false");
    visualizer.addDebugSnapshot("SynapseState", "Weight: 0.5, Delay: 5, Type: Excitatory");
    
    // Generate visualizations
    std::cout << "Text visualization:\n";
    std::cout << visualizer.generateTextVisualization() << std::endl;
    
    std::cout << "\nHTML dashboard generation (first 500 chars):\n";
    std::string html = visualizer.generateHTMLDashboard("NLM Debug Dashboard");
    std::cout << html.substr(0, 500) << "..." << std::endl;
}

void demonstrateProfilingOutputFormats() {
    std::cout << "\n=== Demonstrating Profiling Output Formats ===\n" << std::endl;
    
    // Create sample metrics
    std::vector<std::pair<std::string, double>> metrics = {
        {"SimulationTime", 1250.5},
        {"AverageStepTime", 12.5},
        {"NeuronUpdates", 5000},
        {"SynapseUpdates", 25000},
        {"MemoryUsage", 512.0},
        {"CPUUsage", 45.2}
    };
    
    std::vector<std::string> snapshots = {
        "NeuronState: Vmem=0.1, Refractory=0, Firing=false",
        "SynapseState: Weight=0.5, Delay=5, Type=Excitatory"
    };
    
    // Generate different output formats
    std::string textReport = nlm::ProfilingOutputFormatter::formatTextReport(
        "Neural Simulation Report", "Text report data goes here");
    
    std::string htmlDashboard = nlm::ProfilingOutputFormatter::formatHTMLDashboard(
        "Neural Simulation Dashboard", "HTML report data goes here", metrics);
    
    std::string csvReport = nlm::ProfilingOutputFormatter::formatCSVReport(
        "Neural Simulation CSV", "CSV report data goes here", metrics);
    
    std::string jsonReport = nlm::ProfilingOutputFormatter::formatJSONReport(
        "Neural Simulation JSON", "JSON report data goes here", metrics);
    
    std::cout << "Text report (first 300 chars):\n";
    std::cout << textReport.substr(0, 300) << "..." << std::endl;
    
    std::cout << "\nCSV report (first 200 chars):\n";
    std::cout << csvReport.substr(0, 200) << "..." << std::endl;
    
    std::cout << "\nJSON report (first 300 chars):\n";
    std::cout << jsonReport.substr(0, 300) << "..." << std::endl;
}

void demonstrateEnhancedNLMLogger() {
    std::cout << "\n=== Demonstrating EnhancedNLMLogger ===\n" << std::endl;
    
    // Access global enhanced logger
    auto& enhancedLogger = nlm::getGlobalEnhancedLogger();
    
    // Demonstrate various logging methods
    enhancedLogger.logStructured("Performance", "SimulationStep", {
        {"step", "100"},
        {"duration", "12.5"},
        {"neuronCount", "500"},
        {"spikeCount", "47"}
    });
    
    enhancedLogger.logPerformance("NeuronProcessing", {
        {"executionTime", 12.5},
        {"neuronCount", 500},
        {"spikeCount", 47},
        {"efficiency", 0.85}
    }, "Phase2_Simulation");
    
    enhancedLogger.logNeuralEvent("SynapseUpdate", "Weight changed by 0.05", "LTP");
    
    enhancedLogger.logSystemResource("ResourceMonitoring");
    
    // Simulate memory tracking
    int* testPtr = new int(42);
    enhancedLogger.logMemoryAllocation(testPtr, sizeof(int), "TestAllocation");
    delete testPtr;
    enhancedLogger.logMemoryDeallocation(testPtr, "TestDeallocation");
    
    std::cout << "Enhanced logger profiling report:\n";
    std::cout << enhancedLogger.generateProfilingReport() << std::endl;
}

int main() {
    std::cout << "=== NLM Enhanced Logging and Profiling Demo ===\n" << std::endl;
    std::cout << "This demo showcases the new profiling timer classes, system monitoring capabilities,"
              << " debug facilities, and enhanced logging features.\n" << std::endl;
    
    try {
        demonstrateProfilingTimer();
        demonstrateSystemMonitoring();
        demonstrateDebugFacilities();
        demonstrateEnhancedLogging();
        demonstrateDebugVisualization();
        demonstrateProfilingOutputFormats();
        demonstrateEnhancedNLMLogger();
        
        std::cout << "\n=== All demonstrations completed successfully! ===\n" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during demonstration: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
