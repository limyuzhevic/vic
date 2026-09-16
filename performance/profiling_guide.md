# NLM Phase 6 Performance Profiling and Debugging Tools

## Overview

NLM Phase 6 provides comprehensive performance profiling and debugging tools to help you analyze and optimize your artificial brain simulations. This document covers the available profiling tools, debugging utilities, and performance monitoring capabilities.

## Performance Profiling Tools

### 1. Brain Profiler

```cpp
// Profile brain performance during simulation
class BrainProfiler {
public:
    void startProfile();
    void stopProfile();
    void recordStep(SimulationStep step);
    void recordMemoryUsage();
    void recordTiming(std::string operation);
    
    // Get profiling results
    ProfileReport generateReport() const;
    void saveReport(const std::string& filepath) const;
    
    // Profiling statistics
    double getAverageStepTime() const;
    size_t getTotalSteps() const;
    size_t getMemoryPeakUsage() const;
    std::map<std::string, double> getOperationTimes() const;
};
```

#### Usage Example

```cpp
#include "brain/Brain.hpp"
#include "performance/BrainProfiler.hpp"

int main() {
    // Create and initialize brain
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Create profiler
    BrainProfiler profiler;
    
    // Start profiling
    profiler.startProfile();
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        profiler.recordStep(step);
        profiler.recordMemoryUsage();
    }
    
    // Stop profiling
    profiler.stopProfile();
    
    // Generate and save report
    ProfileReport report = profiler.generateReport();
    profiler.saveReport("brain_performance_report.json");
    
    // Print summary
    std::cout << "Average step time: " << profiler.getAverageStepTime() << " ms" << std::endl;
    std::cout << "Total steps processed: " << profiler.getTotalSteps() << std::endl;
    std::cout << "Peak memory usage: " << profiler.getMemoryPeakUsage() << " MB" << std::endl;
    
    return 0;
}
```

### 2. Memory Usage Monitor

```cpp
// Monitor memory usage of brain systems
class MemoryMonitor {
public:
    void startMonitoring();
    void stopMonitoring();
    void recordMemoryUsage(std::string context);
    
    struct MemorySnapshot {
        std::string timestamp;
        size_t totalMemory;
        size_t usedMemory;
        std::string context;
    };
    
    std::vector<MemorySnapshot> getMemoryHistory() const;
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    void generateMemoryReport(const std::string& filepath) const;
};
```

#### Usage Example

```cpp
// Monitor memory usage during brain operation
MemoryMonitor memoryMonitor;
memoryMonitor.startMonitoring();

// Run memory-intensive simulation
for (int i = 0; i < 100; ++i) {
    // Create and initialize brain
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation steps
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        memoryMonitor.recordMemoryUsage("simulation_step");
    }
    
    memoryMonitor.recordMemoryUsage("brain_initialization");
}

memoryMonitor.stopMonitoring();

// Analyze memory usage
std::vector<MemoryMonitor::MemorySnapshot> history = memoryMonitor.getMemoryHistory();
for (const auto& snapshot : history) {
    std::cout << snapshot.timestamp << " - " 
              << snapshot.usedMemory << " MB (" 
              << snapshot.context << ")" << std::endl;
}

// Save memory analysis
memoryMonitor.generateMemoryReport("memory_usage_analysis.json");
```

### 3. Event System Profiler

```cpp
// Profile spike events and neural activity patterns
class EventProfiler {
public:
    struct SpikeEventMetrics {
        size_t totalSpikes;
        size_t spikesPerSecond;
        double averageInterSpikeInterval;
        std::vector<size_t> spikesPerStep;
    };
    
    struct SynapseEventMetrics {
        size_t totalSTDPUpdates;
        size_t totalHebbianUpdates;
        std::map<std::string, double> plasticityRatios;
    };
    
    SpikeEventMetrics getSpikeMetrics() const;
    SynapseEventMetrics getSynapseMetrics() const;
    void reset();
    void recordEvent(const SpikeEvent& event);
};
```

#### Usage Example

```cpp
// Profile neural events and plasticity
EventProfiler eventProfiler;

// Run simulation with event profiling
for (SimulationStep step = 0; step < 1000; ++step) {
    brain->step(step);
    
    // Record spike events
    if (brain->getSpikeSystem()) {
        eventProfiler.recordEvent(/* spike event */);
    }
}

// Analyze spike patterns
EventProfiler::SpikeEventMetrics spikeMetrics = eventProfiler.getSpikeMetrics();
std::cout << "Total spikes: " << spikeMetrics.totalSpikes << std::endl;
std::cout << "Average firing rate: " << spikeMetrics.spikesPerSecond << " Hz" << std::endl;

// Analyze plasticity
EventProfiler::SynapseEventMetrics synapseMetrics = eventProfiler.getSynapseMetrics();
std::cout << "Total STDP updates: " << synapseMetrics.totalSTDPUpdates << std::endl;
std::cout << "Total Hebbian updates: " << synapseMetrics.totalHebbianUpdates << std::endl;
```

## Debugging Tools

### 1. Brain State Inspector

```cpp
// Inspect and analyze brain state at any point
class BrainStateInspector {
public:
    struct BrainSnapshot {
        SimulationStep step;
        Timestamp time;
        size_t neuronCount;
        size_t synapseCount;
        float avgFiringRate;
        std::vector<float> neuronMembranePotentials;
        std::vector<float> synapseWeights;
        std::map<std::string, std::any> systemStates;
    };
    
    BrainSnapshot captureState(const Brain& brain) const;
    void compareStates(const BrainSnapshot& snapshot1, 
                      const BrainSnapshot& snapshot2);
    std::string analyzeDifferences(const BrainSnapshot& snapshot1,
                                  const BrainSnapshot& snapshot2) const;
    void exportState(const BrainSnapshot& snapshot, 
                    const std::string& filepath) const;
};
```

#### Usage Example

```cpp
// Capture and analyze brain state changes
BrainStateInspector stateInspector;

// Capture initial state
BrainStateInspector::BrainSnapshot initialState = stateInspector.captureState(*brain);

// Run simulation
for (SimulationStep step = 0; step < 500; ++step) {
    brain->step(step);
    
    if (step % 100 == 0) {
        // Capture state at intervals
        BrainStateInspector::BrainSnapshot currentState = stateInspector.captureState(*brain);
        
        // Analyze state changes
        std::string analysis = stateInspector.analyzeDifferences(initialState, currentState);
        std::cout << "State change analysis at step " << step << ":" << std::endl;
        std::cout << analysis << std::endl;
    }
}
```

### 2. Memory Leak Detector

```cpp
// Detect memory leaks in brain systems
class MemoryLeakDetector {
public:
    void startLeakDetection();
    void stopLeakDetection();
    void allocateMemory(size_t size, std::string source);
    void deallocateMemory(size_t size, std::string source);
    
    struct LeakReport {
        std::string source;
        size_t allocatedSize;
        size_t deallocatedSize;
        size_t leakSize;
        double leakPercentage;
    };
    
    std::vector<LeakReport> getLeakReport() const;
    bool hasLeaks() const;
    void clearLeaks();
};
```

#### Usage Example

```cpp
// Detect memory leaks in brain systems
MemoryLeakDetector leakDetector;
leakDetector.startLeakDetection();

// Run simulation that may cause memory leaks
for (int i = 0; i < 10; ++i) {
    // Create and use brain objects
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Perform operations that allocate/deallocate memory
    leakDetector.allocateMemory(1024, "brain_initialization");
    leakDetector.deallocateMemory(512, "brain_initialization");
    
    // Clean up
    brain.reset();
    config.reset();
}

leakDetector.stopLeakDetection();

// Check for leaks
if (leakDetector.hasLeaks()) {
    std::cout << "Memory leaks detected!" << std::endl;
    std::vector<MemoryLeakDetector::LeakReport> report = leakDetector.getLeakReport();
    for (const auto& leak : report) {
        std::cout << leak.source << ": " << leak.leakSize << " bytes (" 
                  << leak.leakPercentage << "%)" << std::endl;
    }
} else {
    std::cout << "No memory leaks detected." << std::endl;
}
```

### 3. Performance Bottleneck Detector

```cpp
// Identify performance bottlenecks in brain systems
class PerformanceBottleneckDetector {
public:
    struct PerformanceMetrics {
        double initializationTime;
        double stepTime;
        double memoryAccessTime;
        double plasticityComputationTime;
        double spikeProcessingTime;
    };
    
    void startMeasurement();
    void stopMeasurement();
    void recordOperation(const std::string& operation, double time);
    PerformanceMetrics getPerformanceMetrics() const;
    std::string identifyBottlenecks() const;
    std::vector<std::string> getOptimizationRecommendations() const;
};
```

#### Usage Example

```cpp
// Identify and optimize performance bottlenecks
PerformanceBottleneckDetector bottleneckDetector;
bottleneckDetector.startMeasurement();

// Run performance-critical simulation
for (SimulationStep step = 0; step < 1000; ++step) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    brain->step(step);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    bottleneckDetector.recordOperation("brain_step", duration.count() / 1000.0);
}

bottleneckDetector.stopMeasurement();

// Analyze performance
PerformanceMetrics metrics = bottleneckDetector.getPerformanceMetrics();
std::cout << "Performance Analysis:" << std::endl;
std::cout << "  Initialization time: " << metrics.initializationTime << " ms" << std::endl;
std::cout << "  Average step time: " << metrics.stepTime << " ms" << std::endl;
std::cout << "  Memory access time: " << metrics.memoryAccessTime << " ms" << std::endl;

std::string bottlenecks = bottleneckDetector.identifyBottlenecks();
std::cout << "\nIdentified bottlenecks:" << std::endl;
std::cout << bottlenecks << std::endl;

std::vector<std::string> recommendations = bottleneckDetector.getOptimizationRecommendations();
std::cout << "\nOptimization recommendations:" << std::endl;
for (const auto& rec : recommendations) {
    std::cout << "  - " << rec << std::endl;
}
```

## Python Integration

### Python Performance Monitoring

```python
import pynlm
import time
import json

class PythonPerformanceMonitor:
    def __init__(self):
        self.step_times = []
        self.memory_usage = []
        self.start_time = None
        self.end_time = None
    
    def start_monitoring(self):
        """Start monitoring brain performance."""
        self.start_time = time.time()
        self.step_times = []
        self.memory_usage = []
    
    def stop_monitoring(self):
        """Stop monitoring and calculate statistics."""
        self.end_time = time.time()
    
    def record_step(self, step):
        """Record step timing information."""
        start = time.time()
        
        # Run brain step
        brain.step(step)
        
        end = time.time()
        step_time = end - start
        
        self.step_times.append(step_time)
        
        # Get memory usage if available
        # This would depend on the specific Python bindings
        # self.memory_usage.append(get_memory_usage())
    
    def generate_report(self):
        """Generate performance report."""
        if not self.step_times:
            return None
        
        avg_step_time = sum(self.step_times) / len(self.step_times)
        total_time = self.end_time - self.start_time
        steps_per_second = len(self.step_times) / total_time if total_time > 0 else 0
        
        report = {
            'total_steps': len(self.step_times),
            'total_time': total_time,
            'average_step_time': avg_step_time,
            'steps_per_second': steps_per_second,
            'min_step_time': min(self.step_times),
            'max_step_time': max(self.step_times),
            'memory_usage': self.memory_usage,
            'peak_memory': max(self.memory_usage) if self.memory_usage else 0
        }
        
        return report
    
    def save_report(self, filepath):
        """Save performance report to file."""
        report = self.generate_report()
        if report:
            with open(filepath, 'w') as f:
                json.dump(report, f, indent=2)
            print(f"Performance report saved to {filepath}")
        else:
            print("No performance data to save")

# Usage example
def example_performance_monitoring():
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Create performance monitor
    monitor = PythonPerformanceMonitor()
    
    # Start monitoring
    monitor.start_monitoring()
    
    # Run simulation with monitoring
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        monitor.record_step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Stop monitoring and generate report
    monitor.stop_monitoring()
    monitor.save_report("brain_performance.json")
    
    # Print summary
    report = monitor.generate_report()
    if report:
        print(f"Simulation completed: {report['total_steps']} steps")
        print(f"Average step time: {report['average_step_time']:.4f} seconds")
        print(f"Performance: {report['steps_per_second']:.1f} steps/second")

if __name__ == "__main__":
    example_performance_monitoring()
```

## Automated Performance Testing

### 1. Performance Regression Test

```cpp
// Automated test to detect performance regressions
class PerformanceRegressionTest {
public:
    struct PerformanceBaseline {
        double targetStepTime;
        size_t targetNeuronCount;
        size_t targetSynapseCount;
        double performanceTolerance; // Percentage
    };
    
    bool runPerformanceTest(const Brain& brain, const PerformanceBaseline& baseline);
    double measureStepTime(const Brain& brain, SimulationStep step) const;
    std::string generatePerformanceReport() const;
    
    // Common baseline configurations
    static PerformanceBaseline getPhase6Baseline() {
        PerformanceBaseline baseline;
        baseline.targetStepTime = 0.001; // 1ms per step
        baseline.targetNeuronCount = 1000;
        baseline.targetSynapseCount = 10000;
        baseline.performanceTolerance = 0.2; // 20% tolerance
        return baseline;
    }
};
```

### 2. Memory Usage Test

```cpp
// Test memory usage and detect leaks
class MemoryUsageTest {
public:
    struct MemoryTestResult {
        size_t peakMemoryUsage;
        size_t memoryGrowthPerStep;
        bool memoryLeakDetected;
        double memoryEfficiency;
    };
    
    MemoryTestResult runMemoryTest(Brain& brain, size_t numSteps);
    size_t getMemoryUsage() const;
    void resetMemoryTracking();
};
```

## Integration with Existing Tools

### 1. Integration with CTest

```cmake
# Add performance tests to CMake configuration
enable_testing()

# Add performance profiling test
add_test(NAME performance_profile
    COMMAND brain_performance_test --steps 100 --neuron_count 1000
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add memory usage test
add_test(NAME memory_usage_test
    COMMAND brain_memory_test --steps 500 --neuron_count 2000
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
```

### 2. Integration with Python Profiling

```python
import cProfile
import pstats
import pynlm

def profile_brain_simulation():
    """Profile brain simulation using Python's cProfile."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Profile the simulation
    profiler = cProfile.Profile()
    profiler.enable()
    
    for step in range(100):
        brain.step(step)
    
    profiler.disable()
    
    # Generate statistics
    stats = pstats.Stats(profiler)
    stats.sort_stats('cumulative')
    stats.print_stats(20)  # Print top 20 time-consuming functions
    
    # Save profiling results
    stats.dump_stats('brain_simulation.prof')

# Usage
profile_brain_simulation()
```

## Configuration Options

### Performance Profiling Configuration

```cpp
struct PerformanceProfilerConfig {
    bool enableStepTiming = true;
    bool enableMemoryMonitoring = true;
    bool enableEventProfiling = true;
    bool enableBottleneckDetection = true;
    size_t profilingInterval = 100; // Steps between profile reports
    double maxStepTimeThreshold = 0.01; // Maximum acceptable step time
    size_t memoryThresholdMB = 512; // Memory usage threshold
    
    // Output configuration
    std::string reportOutputDir = "./performance_reports";
    bool saveDetailedReports = true;
    bool generateVisualizations = false;
};
```

### Debug Configuration

```cpp
struct DebugConfig {
    bool enableStateInspection = true;
    bool enableMemoryLeakDetection = true;
    bool enablePerformanceBottleneckDetection = true;
    bool enableEventTracing = true;
    
    // Debug level
    enum DebugLevel {
        DEBUG_NONE = 0,
        DEBUG_BASIC = 1,
        DEBUG_DETAILED = 2,
        DEBUG_VERY_DETAILED = 3
    };
    
    DebugLevel debugLevel = DebugLevel::BASIC;
    
    // Output configuration
    std::string debugOutputDir = "./debug_output";
    bool saveDebugLogs = true;
    bool enableRealTimeDebugging = false;
};
```

## Tools Summary

| Tool | Purpose | Best For | Output |
|------|---------|----------|--------|
| Brain Profiler | Measure simulation performance | Performance analysis | JSON report |
| Memory Monitor | Track memory usage | Memory optimization | Timeline data |
| Event Profiler | Analyze spike patterns | Neural activity study | Statistical analysis |
| State Inspector | Examine brain state | Debugging | State snapshots |
| Leak Detector | Find memory leaks | Memory management | Leak report |
| Bottleneck Detector | Identify performance issues | Optimization | Recommendations |

## Getting Started

### Quick Start

1. **Install dependencies**: `pip install -e .`
2. **Build performance tools**: `cd build && make -j4`
3. **Run performance test**: `./brain_performance_test --help`
4. **Enable profiling**: Add `--profile` flag to your simulation
5. **Analyze results**: `python analyze_performance.py results.json`

### Examples

```bash
# Run performance profiling
./brain_performance_test --steps 500 --neurons 2000 --profile

# Run memory usage analysis
./brain_memory_test --steps 1000 --neuron_count 3000

# Run bottleneck detection
./brain_bottleneck_test --steps 200 --config performance_config.json

# Generate performance report
python generate_performance_report.py results/ > performance_summary.html
```

## Integration with Development Workflow

### 1. Git Hooks

```bash
# Pre-commit hook to check performance
#!/bin/bash
if git diff --cached --name-only | grep -q "\.cpp$"; then
    echo "Building with performance profiling..."
    ./build_and_test_performance.sh
fi
```

### 2. CI/CD Pipeline

```yaml
# GitHub Actions workflow for performance testing
name: Performance Testing

on: [push, pull_request]

jobs:
  performance-test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Build with performance tools
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_PERFORMANCE_PROFILING=ON
        make -j4
    - name: Run performance tests
      run: |
        cd build
        ./test_runner --performance_only=true
    - name: Generate performance reports
      run: |
        python generate_performance_dashboard.py
    - name: Upload performance reports
      uses: actions/upload-artifact@v2
      with:
        name: performance-reports
        path: performance_reports/
```

## Conclusion

NLM Phase 6 provides comprehensive performance profiling and debugging tools to help you:

1. **Measure performance** - Accurately measure simulation speed and efficiency
2. **Monitor memory** - Track memory usage and detect leaks
3. **Analyze events** - Understand neural activity patterns
4. **Debug issues** - Identify and fix performance problems
5. **Optimize code** - Get specific recommendations for improvement

These tools integrate seamlessly with the existing NLM codebase and provide actionable insights for optimizing your artificial brain simulations.

---
*This document provides comprehensive performance profiling and debugging tools for NLM Phase 6 simulations. Use these tools to analyze, optimize, and validate your brain simulations for peak performance.*