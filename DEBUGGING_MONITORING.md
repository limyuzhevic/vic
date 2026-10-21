# NLM Debugging and Monitoring Tools

## Overview

The NLM (Neural Learning Machine) framework now includes comprehensive debugging and monitoring tools to help developers and researchers monitor, debug, and analyze their neural simulations.

## Key Features

### Real-time Monitoring
- **Live Brain State Tracking**: Monitor brain state in real-time during simulation
- **Performance Metrics Collection**: Track CPU, memory, and simulation performance
- **Neural Activity Visualization**: Visualize neural activity patterns
- **Memory Usage Monitoring**: Track memory consumption and optimize performance

### Debugging Tools
- **Step-by-Step Debugging**: Pause and examine simulation state at each step
- **Event Logging**: Detailed logging of all neural events and state changes
- **Breakpoint Support**: Set breakpoints for specific conditions
- **State Inspection**: Inspect and modify brain state during simulation

### Analysis Tools
- **Neural Pattern Analysis**: Identify and analyze neural activity patterns
- **Learning Progress Tracking**: Monitor learning curves and progress
- **Statistical Analysis**: Perform statistical analysis on simulation results
- **Comparison Tools**: Compare different simulation runs

## Debugging Module API

### Core Debugging Classes

#### DebugSession
```cpp
class DebugSession {
public:
    DebugSession(const std::string& name);
    ~DebugSession();
    
    // Control session
    void start();
    void stop();
    void pause();
    void resume();
    
    // Configuration
    void setBreakpoint(StepCondition condition);
    void addWatchpoint(const std::string& path);
    void setLogLevel(LogLevel level);
    void setOutputFile(const std::string& filename);
    
    // State inspection
    std::string inspectBrainState() const;
    std::string inspectMemoryState() const;
    std::string inspectNeurons() const;
    
    // Event logging
    void enableEventLogging(bool enable);
    void enablePerformanceLogging(bool enable);
    void enableMemoryLogging(bool enable);
    
    // Export
    std::string exportDebugData() const;
    bool importDebugData(const std::string& data);
    
    // Statistics
    DebugStatistics getStatistics() const;
};
```

#### DebugMonitor
```cpp
class DebugMonitor {
public:
    DebugMonitor();
    ~DebugMonitor();
    
    // Start monitoring
    void startMonitoring(const std::shared_ptr<Brain>& brain);
    void stopMonitoring();
    
    // Metrics collection
    PerformanceMetrics getPerformanceMetrics() const;
    MemoryMetrics getMemoryMetrics() const;
    NeuralMetrics getNeuralMetrics() const;
    
    // Callbacks
    void setPerformanceCallback(std::function<void(const PerformanceMetrics&)>) const;
    void setMemoryCallback(std::function<void(const MemoryMetrics&)>) const;
    void setNeuralCallback(std::function<void(const NeuralMetrics&)>) const;
    
    // Configuration
    void setSamplingInterval(double interval);
    void enableRealTimeDisplay(bool enable);
    
    // State access
    std::vector<DebugSnapshot> getSnapshots() const;
    DebugSnapshot getLatestSnapshot() const;
};
```

#### DebugVisualizer
```cpp
class DebugVisualizer {
public:
    DebugVisualizer();
    ~DebugVisualizer();
    
    // Initialize visualizer
    bool initialize(int width, int height);
    void shutdown();
    
    // Display modes
    void showNeuralActivity(const std::shared_ptr<Brain>& brain);
    void showMemoryUsage(const MemoryMetrics& metrics);
    void showPerformanceMetrics(const PerformanceMetrics& metrics);
    void showLearningProgress(const LearningMetrics& metrics);
    
    // Interaction
    void handleUserInput(const UserInput& input);
    void setAutoRefresh(bool enable);
    
    // Export
    void saveVisualization(const std::string& filename);
    void exportDataForAnalysis(const std::string& outputDir);
};
```

### Python API for Debugging

#### High-level Python Interface
```python
import pynlm
import pynlm.debug as debug

# Create debug session
debug_session = debug.DebugSession("simulation_debug")

# Configure debugging
debug_session.setLogLevel("debug")
debug_session.enableEventLogging(True)
debug_session.enablePerformanceLogging(True)
debug_session.setOutputFile("debug.log")

# Add watchpoints and breakpoints
debug_session.addWatchpoint("brain.regions[0].neurons")
debug_session.setBreakpoint(lambda step: step % 100 == 0)

# Start monitoring
brain = pynlm.createBrain(pynlm.createDefaultConfig())
monitor = debug.DebugMonitor()
monitor.startMonitoring(brain)

# Run simulation with debugging
for step in range(1000):
    debug_session.enterStep(step)
    
    # Get current state
    snapshot = monitor.getLatestSnapshot()
    performance = monitor.getPerformanceMetrics()
    memory = monitor.getMemoryMetrics()
    
    # Display debug info
    debug_session.displayState(snapshot, performance, memory)
    
    # Check for breakpoints
    if debug_session.shouldBreak(step):
        action = debug_session.getDebugAction()
        if action == "continue":
            continue
        elif action == "step":
            # Run single step
            pass
        elif action == "pause":
            break
        elif action == "inspect":
            # Inspect state
            debug_session.inspectState()
    
    # Run simulation step
    brain.step(step)
    
    debug_session.exitStep(step)

# Export debug data
debug_session.exportDebugData("debug_output.json")
```

#### Advanced Python Debugging
```python
# Create debug visualizer
visualizer = debug.DebugVisualizer()
visualizer.initialize(1920, 1080)

# Setup real-time monitoring
with debug.RealTimeMonitor(brain, visualizer) as monitor:
    monitor.setSamplingInterval(0.1)  # 100ms sampling
    monitor.setPerformanceCallback(lambda metrics: print(f"Performance: {metrics.fps} FPS"))
    monitor.setNeuralCallback(lambda metrics: print(f"Neural activity: {metrics.active_neurons} active"))
    
    # Run simulation
    for step in range(1000):
        brain.step(step)
        visualizer.showNeuralActivity(brain)
        visualizer.showMemoryUsage(monitor.getMemoryMetrics())
        visualizer.showPerformanceMetrics(monitor.getPerformanceMetrics())
        
        if visualizer.shouldUpdateDisplay():
            visualizer.render()

# Save visualization
visualizer.saveVisualization("simulation_analysis.png")
visualizer.exportDataForAnalysis("analysis_output/")
```

## Implementation Details

### DebugSnapshot
```cpp
struct DebugSnapshot {
    SimulationStep step;
    Timestamp timestamp;
    double cpuUsage;
    double memoryUsage;
    size_t activeNeurons;
    size_t totalSpikes;
    float averageFiringRate;
    float excitationInhibitionRatio;
    DevelopmentalStage developmentalStage;
    float curiosityLevel;
    float noveltyLevel;
    float neuromodulationLevel;
    std::vector<float> neuralActivityPattern;
    std::vector<std::string> activeProcesses;
    std::map<std::string, float> metrics;
    
    nlohmann::json toJSON() const;
    static DebugSnapshot fromJSON(const nlohmann::json& data);
};
```

### PerformanceMetrics
```cpp
struct PerformanceMetrics {
    double simulationTime;      // Time per step (seconds)
    double cpuUsage;            // CPU usage percentage
    double memoryUsage;         // Memory usage (MB)
    size_t allocatedMemory;     // Allocated memory (bytes)
    size_t peakMemoryUsage;     // Peak memory usage (bytes)
    float fps;                  // Frames per second (simulated)
    float stepsPerSecond;       // Steps processed per second
    std::vector<double> timeline;  // Performance timeline
    std::map<std::string, double> detailedMetrics;
    
    std::string getSummary() const;
    std::string generateReport() const;
};
```

### MemoryMetrics
```cpp
struct MemoryMetrics {
    size_t totalMemory;         // Total allocated memory (bytes)
    size_t usedMemory;          // Used memory (bytes)
    size_t peakMemoryUsage;     // Peak memory usage (bytes)
    size_t neuronMemory;        // Memory used by neurons (bytes)
    size_t synapseMemory;       // Memory used by synapses (bytes)
    size_t regionMemory;        // Memory used by regions (bytes)
    std::map<std::string, size_t> componentMemory;
    std::vector<AllocationInfo> allocationHistory;
    
    std::string getMemoryProfile() const;
    bool isMemoryLeaking() const;
    size_t getAllocationCount(const std::string& component) const;
};
```

### NeuralMetrics
```cpp
struct NeuralMetrics {
    size_t totalNeurons;        // Total number of neurons
    size_t activeNeurons;       // Number of active neurons
    size_t firingNeurons;       // Number of firing neurons
    size_t refractoryNeurons;   // Number of neurons in refractory period
    float averageFiringRate;    // Average firing rate (Hz)
    float excitationInhibitionRatio;  // E/I balance
    float membranePotentialMean;       // Mean membrane potential
    float membranePotentialStd;        // Standard deviation of membrane potential
    std::vector<float> neuronTypeDistribution;
    std::map<std::string, size_t> neuralPopulationStats;
    std::vector<ActivityPattern> activityPatterns;
    
    std::string getNeuralProfile() const;
    std::string detectAnomalies() const;
    std::vector<ActivityPattern> extractActivityPatterns() const;
};
```

### LearningMetrics
```cpp
struct LearningMetrics {
    float learningCurveSpeed;   // Speed of learning curve
    float convergencePoint;     // Point of convergence
    float finalPerformance;     // Final performance level
    float stabilityMeasure;     // Stability of learned behavior
    std::vector<float> rewardHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> curiosityHistory;
    std::map<size_t, float> episodePerformance;
    float generalizationAbility;  // Ability to generalize learned behavior
    float transferLearning;      // Transfer of learned skills
    
    std::string getLearningProfile() const;
    std::string assessLearningQuality() const;
    std::vector<float> getNormalizedLearningCurve() const;
};
```

## Debug Commands

### Command Line Debugging
```bash
# Start NLM with debugging
nlm --enable-debug --debug-level debug --debug-output debug.log

# Debug with real-time monitoring
nlm --enable-debug --enable-visualization --debug-port 8080

# Performance profiling
nlm --enable-profiling --profile-output profile.prof --enable-debug

# Memory debugging
nlm --enable-debug --debug-memory --memory-limit 1000000

# Step-by-step debugging
nlm --enable-debug --debug-step-by-step --breakpoint "step % 100 == 0"

# Remote debugging
nlm --enable-debug --debug-server --debug-host 0.0.0.0 --debug-port 8080
```

### Python Debug Commands
```python
import pynlm.debug as debug

# Create debug session
debug_session = debug.DebugSession("my_simulation")

# Configure debugging
debug_session.setLogLevel("debug")
debug_session.setOutputFile("simulation_debug.log")
debug_session.enableEventLogging(True)

# Add watchpoints
debug_session.addWatchpoint("brain.state.neural_activity")
debug_session.addWatchpoint("agent.curiosity_level")

# Set breakpoints
debug_session.setBreakpoint(lambda step: step % 50 == 0)
debug_session.setConditionBreakpoint("brain.getTotalSynapseCount() > 1000")

# Start monitoring
monitor = debug.DebugMonitor()
monitor.startMonitoring(brain)

# Run simulation with debugging
for step in range(1000):
    debug_session.beginStep(step)
    
    # Check breakpoints
    if debug_session.shouldBreak(step):
        action = debug_session.getDebugAction()
        if action == "continue":
            debug_session.endStep()
            brain.step(step)
            continue
        elif action == "step":
            debug_session.endStep()
            brain.step(step)
        elif action == "pause":
            break
        elif action == "inspect":
            debug_session.inspectCurrentState()
    
    # Run simulation
    brain.step(step)
    
    # Collect metrics
    snapshot = monitor.getLatestSnapshot()
    
    # Update visualizer if enabled
    if visualizer.isInitialized():
        visualizer.update(brain, snapshot)
    
    debug_session.endStep()

# Export debug data
debug_session.exportDebugData("complete_debug_output.json")
```

## Integration Examples

### Example 1: Real-time Simulation Monitoring
```python
import pynlm
import pynlm.debug as debug
import time

class SimulationMonitor:
    def __init__(self, brain):
        self.brain = brain
        self.debug_session = debug.DebugSession("simulation_monitor")
        self.monitor = debug.DebugMonitor()
        self.visualizer = None
        
        # Setup monitoring
        self.debug_session.setLogLevel("info")
        self.debug_session.enableEventLogging(True)
        self.debug_session.enablePerformanceLogging(True)
        
        self.monitor.startMonitoring(brain)
        self.monitor.setSamplingInterval(0.5)  # Sample every 500ms
        
    def run_simulation(self, num_steps):
        start_time = time.time()
        
        for step in range(num_steps):
            # Collect metrics
            snapshot = self.monitor.getLatestSnapshot()
            performance = self.monitor.getPerformanceMetrics()
            memory = self.monitor.getMemoryMetrics()
            neural = self.monitor.getNeuralMetrics()
            
            # Display progress
            elapsed = time.time() - start_time
            print(f"Step {step}/{num_steps} - "
                  f"FPS: {performance.fps:.1f} - "
                  f"Active neurons: {neural.active_neurons} - "
                  f"Memory: {memory.usedMemory/1024/1024:.1f} MB - "
                  f"Time: {elapsed:.2f}s")
            
            # Update visualizer if available
            if self.visualizer:
                self.visualizer.update(self.brain, snapshot)
            
            # Run simulation step
            self.brain.step(step)
            
        return self.getSimulationSummary()
    
    def getSimulationSummary(self):
        snapshot = self.monitor.getLatestSnapshot()
        performance = self.monitor.getPerformanceMetrics()
        memory = self.monitor.getMemoryMetrics()
        neural = self.monitor.getNeuralMetrics()
        
        return {
            "steps_completed": snapshot.step,
            "total_time": snapshot.timestamp,
            "performance": {
                "average_fps": performance.fps,
                "average_steps_per_second": performance.stepsPerSecond,
                "cpu_usage": performance.cpuUsage
            },
            "neural": {
                "final_active_neurons": neural.activeNeurons,
                "final_firing_rate": neural.averageFiringRate,
                "excitation_inhibition_ratio": neural.excitationInhibitionRatio
            },
            "memory": {
                "final_memory_usage": memory.usedMemory,
                "peak_memory_usage": memory.peakMemoryUsage
            }
        }

# Usage example
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
monitor = SimulationMonitor(brain)

summary = monitor.run_simulation(1000)
print(f"Simulation completed: {summary}")
```

### Example 2: Debugging with Breakpoints
```python
import pynlm
import pynlm.debug as debug

# Create debug session
debug_session = debug.DebugSession("breakpoint_debug")
debug_session.setLogLevel("debug")
debug_session.enableEventLogging(True)

# Add condition breakpoints
debug_session.setConditionBreakpoint("brain.getTotalSynapseCount() < 100")
debug_session.setConditionBreakpoint("agent.getCuriosityLevel() > 0.8")
debug_session.setConditionBreakpoint("step % 200 == 0")  # Every 200 steps

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# Run simulation with debugging
for step in range(1000):
    debug_session.beginStep(step)
    
    # Check if we should break
    if debug_session.shouldBreak(step):
        print(f"\n*** BREAKPOINT at step {step} ***")
        print(f"Current state: {debug_session.inspectCurrentState()}")
        
        action = debug_session.getDebugAction()
        if action == "continue":
            debug_session.endStep()
            brain.step(step)
            continue
        elif action == "step":
            debug_session.endStep()
            brain.step(step)
        elif action == "pause":
            print("Simulation paused. Press Enter to continue...")
            input()
            break
        elif action == "inspect":
            debug_session.inspectDetailedState()
            continue
        elif action == "modify":
            debug_session.modifyState()
            continue
    
    # Run simulation step
    brain.step(step)
    
    debug_session.endStep(step)

# Export debug data
debug_session.exportDebugData("breakpoint_debug_output.json")
```

### Example 3: Performance Profiling
```python
import pynlm
import pynlm.debug as debug
import cProfile
import pstats

# Enable profiling
profiler = cProfile.Profile()
profiler.enable()

# Create debug session with profiling
debug_session = debug.DebugSession("performance_profile")
debug_session.setLogLevel("info")
debug_session.enablePerformanceLogging(True)

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Run simulation with profiling
for step in range(500):
    debug_session.beginStep(step)
    
    # Run simulation step
    brain.step(step)
    
    debug_session.endStep(step)

profiler.disable()

# Generate profiling report
stats = pstats.Stats(profiler)
stats.sort_stats('cumulative')
stats.print_stats(50)  # Top 50 functions by cumulative time

# Save profiling data
with open("profile_results.txt", "w") as f:
    stats.stream = f
    stats.print_stats()

# Also export debug data
debug_session.exportDebugData("profile_debug_output.json")
```

## File Structure

### Source Files
```
debug/
├── DebugSession.h
├── DebugSession.cpp
├── DebugMonitor.h
├── DebugMonitor.cpp
├── DebugVisualizer.h
├── DebugVisualizer.cpp
├── DebugSnapshots.h
├── DebugSnapshots.cpp
├── DebugCommands.h
├── DebugCommands.cpp
├── DebugPythonBindings.cpp
├── include/pynlm/debug/ (headers)
└── debug_tools/ (utility functions)
```

### Build Integration
```cmake
# CMake configuration for debug module
add_subdirectory(debug)

# Python bindings
pybind11_add_module(pynlm_debug debug/DebugPythonBindings.cpp)
target_link_libraries(pynlm_debug PRIVATE nlm_core debug_core)

# Debug targets
add_executable(nlm_debug_console debug_console.cpp)
target_link_libraries(nlm_debug_console debug_core nlm_core)

# Installation
install(TARGETS pynlm_debug
    LIBRARY DESTINATION pynlm
    ARCHIVE DESTINATION pynlm
    RUNTIME DESTINATION pynlm
)
install(DIRECTORY debug/include/ DESTINATION include/pynlm)
```

## Configuration

### Debug Configuration Options
```bash
# Command line options
--enable-debug                    Enable debugging
--debug-level <level>             Debug level (debug|info|warning|error)
--debug-output <file>             Debug output file
--debug-port <port>               Debug server port
--enable-visualization            Enable real-time visualization
--sampling-interval <seconds>     Monitoring sampling interval
--breakpoint <condition>          Set breakpoint condition
--watchpoint <path>              Add watchpoint
--enable-event-logging           Enable event logging
--enable-performance-logging      Enable performance logging
--enable-memory-logging           Enable memory logging
--memory-limit <bytes>            Memory limit for debugging
--max-memory-usage <bytes>        Maximum memory usage
--thread-count <count>            Number of debug threads

# Environment variables
NLM_DEBUG_MODE=1                    Enable debug mode
NLM_DEBUG_LEVEL=debug               Debug level
NLM_DEBUG_OUTPUT=debug.log          Debug output file
NLM_DEBUG_HOST=localhost            Debug server host
NLM_DEBUG_PORT=8080                 Debug server port
NLM_DEBUG_SAMPLING_INTERVAL=0.1     Sampling interval
NLM_DEBUG_MAX_MEMORY=1000000       Maximum memory usage
```

This comprehensive debugging and monitoring system provides developers and researchers with powerful tools to monitor, debug, and analyze NLM simulations in real-time. The system integrates seamlessly with the existing NLM framework while providing extensive customization and control options.