#include <iostream>
#include <string>
#include <memory>
#include <vector>

using namespace nlm;

// Example: Performance monitoring with timers
void demonstratePerformanceMonitoring() {
    std::cout << "=== Performance Monitoring Demo ===" << std::endl;
    
    // Time a specific operation
    auto result = Logger::getGlobal().timeOperation("Neural processing", LogLevel::Info, []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 42;
    });
    
    std::cout << "Operation result: " << result << std::endl;
    std::cout << "See log output for timing details" << std::endl;
}

// Example: Event tracing for complex workflows
void demonstrateEventTracing() {
    std::cout << "\n=== Event Tracing Demo ===" << std::endl;
    
    // Create an event tracer
    auto tracer = Logger::getGlobal().traceEvent("Data processing workflow", "workflow-123");
    
    // Add events to the trace
    tracer.addEvent("Input received", "500MB of data");
    tracer.addCheckpoint("Parsing started");
    tracer.addEvent("Parsing completed", "200 files processed");
    tracer.addCheckpoint("Analysis started");
    tracer.addEvent("Analysis completed", "95% confidence");
    tracer.addCheckpoint("Output generated");
    
    // Set context for the entire trace
    tracer.setContext("user_id", "user456");
    tracer.setContext("priority", "high");
    
    std::cout << "Event trace completed. See log output for details." << std::endl;
}

// Example: Memory profiling
void demonstrateMemoryProfiling() {
    std::cout << "\n=== Memory Profiling Demo ===" << std::endl;
    
    // Create a memory profiler
    auto profiler = Logger::getGlobal().profileMemory("Neural network training", "mem-prof-789");
    
    // Track memory allocations (simulated)
    profiler.trackAllocation(1024, "neuron initialization");
    profiler.trackAllocation(2048, "synapse creation");
    profiler.trackObjectCreation("Neuron", 1000);
    profiler.trackObjectCreation("Synapse", 5000);
    
    // Simulate deallocation
    profiler.trackDeallocation(1024);
    profiler.trackObjectDestruction("Neuron", 100);
    
    std::cout << "Memory profile completed. See log output for details." << std::endl;
}

// Example: Combined advanced logging
void demonstrateCombinedLogging() {
    std::cout << "\n=== Combined Advanced Logging Demo ===" << std::endl;
    
    // Use timer to measure overall operation
    auto timer = Logger::getGlobal().timeOperation("Full pipeline processing", LogLevel::Info, "pipeline-999");
    
    // Add event trace to the same correlation
    auto tracer = Logger::getGlobal().traceEvent("Full pipeline", "pipeline-999");
    tracer.setContext("input_type", "sensor_data");
    tracer.setContext("output_type", "behavioral_commands");
    
    // Add memory profiling to the same correlation
    auto profiler = Logger::getGlobal().profileMemory("Pipeline processing", "pipeline-999");
    
    // Simulate some operations within the pipeline
    profiler.trackAllocation(4096, "pipeline buffer");
    tracer.addEvent("Sensor data received");
    
    // Simulate processing
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    profiler.trackAllocation(2048, "neural activation");
    tracer.addEvent("Neural processing completed");
    
    // Simulate output
    profiler.trackDeallocation(4096);
    tracer.addCheckpoint("Output generated");
    
    // Log performance summary
    Logger::getGlobal().logPerformance("Full pipeline processing", timer.getDurationMs(), "pipeline-999");
    
    std::cout << "Combined logging demonstration completed." << std::endl;
}

int main() {
    std::cout << "NLM Advanced Logging and Debugging Demonstrations" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    // Initialize global logger with appropriate level
    Logger::getGlobal().setLevel(LogLevel::Info);
    
    // Add console logger for demonstration
    auto consoleLogger = std::make_shared<ConsoleLogger>();
    consoleLogger->setLevel(LogLevel::Info);
    consoleLogger->setUseColors(true);
    Logger::getGlobal().addLogger(consoleLogger);
    
    // Run demonstrations
    demonstratePerformanceMonitoring();
    demonstrateEventTracing();
    demonstrateMemoryProfiling();
    demonstrateCombinedLogging();
    
    std::cout << "\nAll demonstrations completed successfully!" << std::endl;
    std::cout << "Check the log output above for detailed timing and event information." << std::endl;
    
    return 0;
}