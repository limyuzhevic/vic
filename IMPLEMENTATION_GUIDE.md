# NLM Debugging and Visualization System - Implementation Guide

## Overview

This document provides a comprehensive implementation guide for creating the debugging and visualization tools requested in the task for the NLM (Neural Learning Machine) codebase.

## Project Structure Analysis

### Current Project Layout

The NLM project has been successfully analyzed and contains the following components:

**Core Neural Simulation Components**:
- `src/brain/` - Neural populations, regions, neurons, synapses
- `src/cognition/` - Neural planning, concept formation
- `src/neuromodulation/` - Reward, curiosity, novelty, neuromodulators
- `src/prediction/` - Neural prediction system
- `src/memory/` - Working memory, episodic memory
- `src/motor/` - Motor system
- `src/environment/` - Sensory inputs (audio, vision)
- `src/world/` - World simulation
- `src/plasticity/` - STDP, Hebbian learning
- `src/dynamics/` - Neural dynamics
- `src/core/` - Random, clock, config, logger, types
- `src/experiments/` - Various experiments

**Existing Visualization Infrastructure**:
- `src/visualization/VisualizationInterface.{hpp,cpp}` - Base visualization interface (Phase 1 placeholder)

## Implementation Requirements

### 1. Debugging Tools (src/debug/)
Create the following debugging tools:

#### 1.1 BrainDebugger
**Purpose**: Interactive brain state inspector
**Key Features**:
- Monitor neural population states
- Display neuron details (voltage, connections, type)
- Show synaptic connections with weights and delays
- Display population statistics
- Export debug data to files
- Set verbosity levels (0-3)
- Feature toggles for different debug views

#### 1.2 StepByStepDebugger
**Purpose**: Simulate step-by-step execution
**Key Features**:
- Pause/resume/single-step execution control
- Neural state snapshots for analysis
- Execution history tracking
- Export execution logs
- Step delay configuration for visualization
- Show voltage traces
- Show spike traces

#### 1.3 MemoryDebugger
**Purpose**: Monitor memory system usage
**Key Features**:
- Working memory and episodic memory monitoring
- Memory allocation/deallocation tracking
- Memory usage statistics
- Memory efficiency calculations
- Export memory traces
- Track allocations by context

#### 1.4 PredictionDebugger
**Purpose**: Track prediction system state
**Key Features**:
- Prediction statistics (success rate, confidence, accuracy)
- Prediction results display
- Processing time tracking
- Export prediction logs
- Context-based statistics
- Prediction rate monitoring

#### 1.5 CognitionDebugger
**Purpose**: Monitor cognitive processes
**Key Features**:
- Planning activity display
- Concept formation tracking
- Cognitive metrics
- Event tracking
- Export cognitive logs
- Success rate monitoring

#### 1.6 NeuromodulatorDebugger
**Purpose**: Debug neuromodulation
**Key Features**:
- Reward, curiosity, and novelty signal monitoring
- Signal strength and value tracking
- Export neuromodulator traces
- Feature toggles for different signal types
- Positive/negative signal identification

### 2. Visualization Tools (src/viz/)
Create the following visualization tools:

#### 2.1 NeuralVisualization
**Purpose**: Neural network visualization
**Key Features**:
- 3D neural network layout
- Neuron highlighting and connection visualization
- Multiple layout algorithms (circular, force-directed, etc.)
- Color schemes for different neuron types
- Export to image formats (PNG, SVG)
- Zoom and pan interaction

#### 2.2 SpikeVisualization
**Purpose**: Spike train visualization
**Key Features**:
- Raster plots
- Spike trace displays
- Histogram of firing rates
- Time window configuration
- Export plotting capabilities
- Multiple neuron grouping

#### 2.3 MemoryVisualization
**Purpose**: Working memory visualization
**Key Features**:
- Memory block visualization
- Memory usage graphs
- Episodic memory timeline
- Memory efficiency indicators
- Capacity utilization display
- Memory block management visualization

#### 2.4 TrajectoryVisualization
**Purpose**: Behavioral trajectory visualization
**Key Features**:
- Motor action trajectories
- Movement patterns
- Path analysis
- Performance metrics
- Trajectory statistics
- Goal achievement visualization

#### 2.5 HeatmapVisualization
**Purpose**: Activity heatmap visualization
**Key Features**:
- Neural activity heatmaps
- Spatiotemporal visualizations
- Correlation matrices
- Export to web-compatible formats
- Threshold-based visualization
- Multiple colormap support

#### 2.6 TimeSeriesVisualization
**Purpose**: Time series plots
**Key Features**:
- Multi-panel time series
- Metric overlays
- Statistical indicators
- Export to data formats
- Time window configuration
- Multiple data series support

### 3. Integration with CLI
Implement the following command line interface:

#### 3.1 Command Line Flags
- `--debug` - Enable debugging
- `--verbose <level>` - Set verbosity level (info, debug, trace)
- `--output <format>` - Set output format (console, file, json, html)
- `--output <filepath>` - Set output file path
- `--realtime` - Enable real-time monitoring
- `--educational` - Enable educational mode

#### 3.2 Output Formats
- **Console Output** - Real-time console debugging
- **JSON Reports** - Structured data export
- **HTML Reports** - Web-based visualization
- **Plot Files** - SVG, PNG for publication
- **Interactive Web Interface** - Real-time web visualization

### 4. Real-time Visualization
Implement the following real-time capabilities:

#### 4.1 Asynchronous Plotting
- Background threads for heavy computations
- Frame rate limiting for smooth visualization
- Live data updates without blocking
- Efficient rendering pipelines

#### 4.2 Real-time Monitoring
- Live brain state monitoring
- Real-time memory usage tracking
- Continuous prediction monitoring
- Live neuromodulator signal display

#### 4.3 Live Updates
- Automatic refresh of visualizations
- Incremental data updates
- Smooth transitions between views
- Responsive user interface

### 5. Educational Visualization
Implement the following educational features:

#### 5.1 Step-by-step Learning
- Interactive tutorials for neural concepts
- Step-by-step visualization of learning processes
- Neuron and synapse animations
- Concept formation visualization

#### 5.2 Interactive Elements
- Quiz and exercise integration
- Context-sensitive help
- Highlight current focus areas
- Progress tracking

## Implementation Strategy

### Phase 1: Core Infrastructure

#### 1.1 Debug Interface Layer
**Files to Create**: `src/debug/DebugInterface.{hpp,cpp}`

**Implementation Requirements**:
```cpp
class DebugInterface {
public:
    virtual ~DebugInterface() = default;
    
    // Core interface methods
    virtual void update() = 0;
    virtual void render() = 0;
    virtual bool isActive() const = 0;
    
    // Configuration methods
    virtual void setVerbosity(int level) = 0;
    virtual int getVerbosity() const = 0;
    virtual void setOutputFormat(const std::string& format) = 0;
    
    // Export functionality
    virtual bool exportData(const std::string& filename) = 0;
    
    // Feature management
    virtual void enableFeature(const std::string& feature, bool enabled) = 0;
    virtual bool isFeatureEnabled(const std::string& feature) const = 0;
};
```

#### 1.2 Output Management System
**Files to Create**: `src/core/DebugUtils/*.{hpp,cpp}`

**Implementation Strategy**:
- **ConsoleOutput**: Real-time console debugging
- **FileOutput**: File-based data export
- **JSONOutput**: Structured JSON export
- **HTMLOutput**: Web-based HTML reports
- **PlotGenerator**: Scientific plotting (matplotlib wrapper)
- **AsyncUpdate**: Asynchronous processing manager

#### 1.3 CLI Argument Parser
**Files to Create**: `src/core/CliParser.{hpp,cpp}`

**Implementation Requirements**:
```cpp
class CliParser {
public:
    bool parse(int argc, char* argv[]);
    
    // Query functions
    bool isDebugEnabled() const;
    bool isVisualizationEnabled() const;
    int getVerbosityLevel() const;
    std::string getOutputFormat() const;
    std::string getOutputFilepath() const;
    bool isRealTimeEnabled() const;
    bool isEducationalMode() const;
    
    // Help and usage
    void printHelp() const;
    void printVersion() const;
};
```

### Phase 2: Debugging Tools Implementation

#### 2.1 Brain Debugger Implementation
**Files**: `src/debug/BrainDebugger.{hpp,cpp}`

**Key Components**:
```cpp
class BrainDebugger : public DebugInterface {
public:
    bool initialize(std::shared_ptr<Brain> brain);
    void update() override;
    void render() override;
    
    // Display functions
    void displayBrainState() const;
    void displayNeuronDetails(int neuronId) const;
    void displaySynapticConnections(int neuronId) const;
    void displayPopulationStats() const;
    
    // Export functionality
    bool exportDebugData(const std::string& filename) const;
    
    // Statistics
    struct DebugStats {
        double simulationTime;
        int totalNeurons;
        int activeNeurons;
        double averageFiringRate;
        double averageConnectionDensity;
    };
    
    DebugStats getDebugStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 2.2 Step-by-Step Debugger Implementation
**Files**: `src/debug/StepByStepDebugger.{hpp,cpp}`

**Key Components**:
```cpp
class StepByStepDebugger : public DebugInterface {
public:
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<SimulationClock> clock);
    
    // Execution control
    void step();
    void pause();
    void resume();
    void singleStep();
    
    // State queries
    bool isPaused() const;
    bool isRunning() const;
    double getCurrentTime() const;
    int getCurrentStep() const;
    
    // Snapshot management
    struct NeuralSnapshot {
        double timestamp;
        int neuronId;
        double voltage;
        bool isSpiking;
        std::vector<double> membranePotentials;
        std::vector<bool> spikeStates;
    };
    
    std::vector<NeuralSnapshot> getNeuralSnapshots() const;
    
    // Export functionality
    bool exportExecutionLog(const std::string& filename) const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 2.3 Memory Debugger Implementation
**Files**: `src/debug/MemoryDebugger.{hpp,cpp}`

**Key Components**:
```cpp
class MemoryDebugger : public DebugInterface {
public:
    bool initialize(std::shared_ptr<MemorySystem> memorySystem);
    void update() override;
    void render() override;
    
    // Display functions
    void displayMemoryState() const;
    void displayWorkingMemory() const;
    void displayEpisodicMemory() const;
    void displayMemoryStats() const;
    
    // Tracking functions
    void trackAllocation(const std::string& label, size_t size, const std::string& context);
    void trackDeallocation(const std::string& label, size_t size);
    
    // Statistics
    struct MemoryStats {
        double totalMemoryUsage;
        double peakMemoryUsage;
        double averageMemoryUsage;
        int allocationCount;
        int deallocationCount;
        std::unordered_map<std::string, size_t> allocationsByLabel;
    };
    
    MemoryStats getMemoryStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 2.4 Prediction Debugger Implementation
**Files**: `src/debug/PredictionDebugger.{hpp,cpp}`

**Key Components**:
```cpp
class PredictionDebugger : public DebugInterface {
public:
    bool initialize(std::shared_ptr<PredictionSystem> predictionSystem);
    void update() override;
    void render() override;
    
    // Display functions
    void displayPredictionState() const;
    void displayPredictionResults() const;
    void displayPredictionAccuracy() const;
    
    // Tracking functions
    void trackPrediction(const std::string& context, double confidence, double accuracy, double processingTime);
    
    // Statistics
    struct PredictionStats {
        double totalPredictions;
        double successfulPredictions;
        double averageConfidence;
        double averageAccuracy;
        double averageProcessingTime;
        double predictionRate;
        std::unordered_map<std::string, double> confidenceByContext;
        std::unordered_map<std::string, double> accuracyByContext;
    };
    
    PredictionStats getPredictionStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 2.5 Cognition Debugger Implementation
**Files**: `src/debug/CognitionDebugger.{hpp,cpp}`

**Key Components**:
```cpp
class CognitionDebugger : public DebugInterface {
public:
    bool initialize(std::vector<std::shared_ptr<CognitiveModule>> cognitiveModules);
    void update() override;
    void render() override;
    
    // Display functions
    void displayCognitiveState() const;
    void displayPlanningActivities() const;
    void displayConceptFormation() const;
    void displayCognitiveMetrics() const;
    
    // Tracking functions
    void trackCognitiveEvent(const std::string& eventType, const std::string& module, double processingTime, double confidence);
    
    // Statistics
    struct CognitiveStats {
        double totalEvents;
        double successfulEvents;
        double averageConfidence;
        double averageProcessingTime;
        double eventRate;
        std::unordered_map<std::string, double> eventsByType;
        std::unordered_map<std::string, double> eventsByModule;
    };
    
    CognitiveStats getCognitiveStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 2.6 Neuromodulator Debugger Implementation
**Files**: `src/debug/NeuromodulatorDebugger.{hpp,cpp}`

**Key Components**:
```cpp
class NeuromodulatorDebugger : public DebugInterface {
public:
    bool initialize(std::vector<std::shared_ptr<Neuromodulator>> neuromodulators);
    void update() override;
    void render() override;
    
    // Display functions
    void displayNeuromodulatorState() const;
    void displayRewardSignals() const;
    void displayCuriositySignals() const;
    void displayNoveltySignals() const;
    
    // Tracking functions
    void trackNeuromodulatorActivity(const std::string& type, const std::string& source, double strength, double value);
    
    // Statistics
    struct NeuromodulatorStats {
        double totalSignals;
        double peakSignalStrength;
        double averageSignalValue;
        double signalRate;
        std::unordered_map<std::string, double> signalStrengthByType;
        std::unordered_map<std::string, double> signalValueBySource;
    };
    
    NeuromodulatorStats getNeuromodulatorStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Phase 3: Visualization Tools Implementation

#### 3.1 Neural Visualization Implementation
**Files**: `src/viz/NeuralVisualization.{hpp,cpp}`

**Key Components**:
```cpp
class NeuralVisualization {
public:
    bool initialize();
    void update(std::shared_ptr<Brain> brain);
    void render();
    
    // Configuration
    void setLayout(const std::string& layout);
    void setColorScheme(const std::string& scheme);
    void setScale(double scale);
    
    // Interaction
    void highlightNeuron(int neuronId);
    void highlightSynapse(int preId, int postId);
    void zoom(double factor);
    
    // Export
    bool exportImage(const std::string& filename, const std::string& format = "png");
    bool exportData(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 3.2 Spike Visualization Implementation
**Files**: `src/viz/SpikeVisualization.{hpp,cpp}`

**Key Components**:
```cpp
class SpikeVisualization {
public:
    void update(const std::vector<std::pair<double, int>>& spikes);
    void render();
    
    // Configuration
    void setTimeWindow(double window);
    void setSamplingRate(double rate);
    void setSpikeThreshold(double threshold);
    
    // Display options
    void showRasters(bool show);
    void showTraces(bool show);
    void showHistograms(bool show);
    
    // Export
    bool exportRasterPlot(const std::string& filename);
    bool exportTraces(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 3.3 Memory Visualization Implementation
**Files**: `src/viz/MemoryVisualization.{hpp,cpp}`

**Key Components**:
```cpp
class MemoryVisualization {
public:
    void update(std::shared_ptr<MemorySystem> memorySystem);
    void render();
    
    // Configuration
    void setDisplayMode(const std::string& mode);
    void setTimeScale(double scale);
    
    // Display options
    void showWorkingMemory(bool show);
    void showEpisodicMemory(bool show);
    void showEfficiency(bool show);
    
    // Export
    bool exportMemoryReport(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 3.4 Trajectory Visualization Implementation
**Files**: `src/viz/TrajectoryVisualization.{hpp,cpp}`

**Key Components**:
```cpp
class TrajectoryVisualization {
public:
    void update(std::shared_ptr<MotorSystem> motorSystem);
    void render();
    
    // Configuration
    void setTrajectoryTimeWindow(double window);
    void setAnalysisMode(const std::string& mode);
    
    // Display options
    void showTrajectories(bool show);
    void showStatistics(bool show);
    void showGoals(bool show);
    
    // Export
    bool exportTrajectoryAnalysis(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 3.5 Heatmap Visualization Implementation
**Files**: `src/viz/HeatmapVisualization.{hpp,cpp}`

**Key Components**:
```cpp
class HeatmapVisualization {
public:
    void update(const std::vector<std::vector<double>>& activityData);
    void render();
    
    // Configuration
    void setColormap(const std::string& colormap);
    void setThreshold(double threshold);
    void setNormalize(bool normalize);
    
    // Display options
    void showHeatmap(bool show);
    void showColorbar(bool show);
    void showStatistics(bool show);
    
    // Export
    bool exportHeatmap(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 3.6 Time Series Visualization Implementation
**Files**: `src/viz/TimeSeriesVisualization.{hpp,cpp}`

**Key Components**:
```cpp
class TimeSeriesVisualization {
public:
    void update(const std::vector<std::vector<std::pair<double, double>>>& timeSeriesData);
    void render();
    
    // Configuration
    void setTimeWindow(double window);
    void setSamplingInterval(double interval);
    void setYAxisScale(const std::string& scale);
    
    // Display options
    void showTimeSeries(bool show);
    void showStatistics(bool show);
    void showAnnotations(bool show);
    
    // Export
    bool exportTimeSeriesData(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

### Phase 4: Integration

#### 4.1 Main Debug Visualizer Implementation
**Files**: `src/debug/DebugVisualizer.{hpp,cpp}`

**Key Components**:
```cpp
class DebugVisualizer : public DebugInterface {
public:
    bool initialize();
    void update() override;
    void render() override;
    
    // Configuration
    void enableDebug(bool enabled);
    void enableVisualization(bool enabled);
    void setVerbosity(int level);
    void setOutputFormat(const std::string& format);
    void setOutputFilepath(const std::string& filepath);
    
    // Real-time features
    void startRealTimeMonitoring();
    void stopRealTimeMonitoring();
    bool isRealTimeMonitoring() const;
    
    // Educational features
    void enableEducationalMode(bool enabled);
    bool isEducationalMode() const;
    
    // Integration
    void setBrain(std::shared_ptr<Brain> brain);
    void setMemorySystem(std::shared_ptr<MemorySystem> memorySystem);
    void setPredictionSystem(std::shared_ptr<PredictionSystem> predictionSystem);
    void setCognitiveModules(std::vector<std::shared_ptr<CognitiveModule>> modules);
    void setNeuromodulators(std::vector<std::shared_ptr<Neuromodulator>> neuromodulators);
    
    // Export functionality
    bool exportReport(const std::string& filename);
    
    // CLI integration
    bool parseCommandLine(int argc, char* argv[]);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 4.2 Brain Integration
**Files to Modify**: `src/brain/Brain.{hpp,cpp}`

**Key Modifications**:
```cpp
class Brain {
public:
    // New methods for debug integration
    void setDebugVisualizer(std::shared_ptr<DebugVisualizer> debugViz);
    std::shared_ptr<DebugVisualizer> getDebugVisualizer() const;
    
    // Existing methods
    void update();
    double getCurrentTime() const;
    const std::vector<std::shared_ptr<Region>>& getRegions() const;
    
    // Debug methods
    void exportDebugData(const std::string& filename);
    void updateDebugInformation();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

#### 4.3 Main Function Integration
**Files to Modify**: `src/main.cpp`

**Key Modifications**:
```cpp
int main(int argc, char* argv[]) {
    // Initialize logging
    Logger::initialize("nlm_debug.log");
    
    // Parse command line arguments
    DebugVisualizer debugViz;
    if (!debugViz.parseCommandLine(argc, argv)) {
        debugViz.printHelp();
        return 1;
    }
    
    // Initialize debug visualizer
    if (!debugViz.initialize()) {
        std::cerr << "Error initializing debug visualizer" << std::endl;
        return 1;
    }
    
    // Load NLM configuration
    auto config = loadConfiguration();
    
    // Create NLM simulation components
    auto brain = createBrain(config);
    auto memorySystem = createMemorySystem(config);
    auto predictionSystem = createPredictionSystem(config);
    auto cognitiveModules = createCognitiveModules(config);
    auto neuromodulators = createNeuromodulators(config);
    
    // Connect debug visualizer
    debugViz.setBrain(brain);
    debugViz.setMemorySystem(memorySystem);
    debugViz.setPredictionSystem(predictionSystem);
    debugViz.setCognitiveModules(cognitiveModules);
    debugViz.setNeuromodulators(neuromodulators);
    
    // Apply configuration
    debugViz.setVerbosity(config.debug.verbosity_level);
    debugViz.setOutputFormat(config.debug.output_format);
    
    if (config.debug.educational_mode) {
        debugViz.enableEducationalMode(true);
    }
    
    if (config.debug.real_time_monitoring) {
        debugViz.startRealTimeMonitoring();
    }
    
    if (!config.debug.output_filepath.empty()) {
        debugViz.setOutputFilepath(config.debug.output_filepath);
    }
    
    // Main simulation loop
    while (!shouldExit()) {
        // Update NLM simulation
        brain->update();
        memorySystem->update();
        predictionSystem->update();
        
        for (auto& module : cognitiveModules) {
            module->update();
        }
        
        for (auto& neuromodulator : neuromodulators) {
            neuromodulator->update();
        }
        
        // Update debug visualizer
        debugViz.update();
        debugViz.render();
        
        // Handle user input
        handleInput(debugViz);
        
        // Check for save report command
        if (shouldSaveReport()) {
            debugViz.exportReport(config.debug.output_filepath);
        }
    }
    
    // Cleanup
    debugViz.stopRealTimeMonitoring();
    Logger::shutdown();
    
    return 0;
}
```

### Phase 5: Build System Integration

#### 5.1 CMake Configuration
**Files to Update**: `CMakeLists.txt`

**Implementation Strategy**:
```cmake
# Add debug tools library
add_library(debug_tools
    src/debug/BrainDebugger.cpp
    src/debug/StepByStepDebugger.cpp
    src/debug/MemoryDebugger.cpp
    src/debug/PredictionDebugger.cpp
    src/debug/CognitionDebugger.cpp
    src/debug/NeuromodulatorDebugger.cpp
    src/debug/DebugVisualizer.cpp
)

target_include_directories(debug_tools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/debug
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/DebugUtils
)

# Add visualization tools library
add_library(viz_tools
    src/viz/NeuralVisualization.cpp
    src/viz/SpikeVisualization.cpp
    src/viz/MemoryVisualization.cpp
    src/viz/TrajectoryVisualization.cpp
    src/viz/HeatmapVisualization.cpp
    src/viz/TimeSeriesVisualization.cpp
)

target_include_directories(viz_tools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/viz
)

# Add debug utils library
add_library(debug_utils
    src/core/DebugUtils/ConsoleOutput.cpp
    src/core/DebugUtils/FileOutput.cpp
    src/core/DebugUtils/JSONOutput.cpp
    src/core/DebugUtils/HTMLOutput.cpp
    src/core/DebugUtils/PlotGenerator.cpp
    src/core/DebugUtils/AsyncUpdate.cpp
)

target_include_directories(debug_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/DebugUtils
)

# Add CLI parser library
add_library(cli_parser
    src/core/CliParser.cpp
)

target_include_directories(cli_parser PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

# Main executable
add_executable(nlm_debug src/main.cpp)

target_link_libraries(nlm_debug
    debug_tools
    viz_tools
    debug_utils
    cli_parser
    brain
    cognition
    neuromodulation
    prediction
    memory
    motor
    environment
    world
    plasticity
    dynamics
    sensory
    development
    experiments
    visualization_interface
    core_logger
    core_random
    core_clock
    core_config
)
```

## Testing Strategy

### Unit Tests
**Files to Create**: `tests/test_debug_tools.cpp`, `tests/test_visualization_tools.cpp`, `tests/test_cli_parser.cpp`

### Integration Tests
**Files to Create**: `tests/test_brain_integration.cpp`, `tests/test_visualization_integration.cpp`, `tests/test_cli_integration.cpp`

### Performance Tests
**Files to Create**: `tests/test_performance.cpp`

## Configuration Management

### Debug Configuration File
**File**: `config/debug_config.json`

```json
{
    "debug": {
        "enabled": true,
        "verbosity_level": 2,
        "output_format": "json",
        "output_filepath": "/tmp/nlm_debug.json",
        "real_time_monitoring": true,
        "educational_mode": false,
        "performance_monitoring": true
    },
    "debug_tools": {
        "brain_debugger": {
            "show_neurons": true,
            "show_connections": true,
            "show_populations": true,
            "show_stats": true
        },
        "step_debugger": {
            "auto_pause": false,
            "record_snapshots": true,
            "show_voltage_trace": true,
            "step_delay": 0.0
        },
        "memory_debugger": {
            "show_working_memory": true,
            "show_episodic_memory": true,
            "show_allocations": true
        }
    },
    "visualization_tools": {
        "neural_viz": {
            "layout": "3d_neural",
            "color_scheme": "default",
            "scale": 1.0
        },
        "heatmap_viz": {
            "colormap": "thermal",
            "normalize": true,
            "threshold": 0.1
        }
    }
}
```

## Installation

### Building from Source
```bash
mkdir build
cd build
cmake ..
cmake --build . --target all
```

### Running the Debug Visualizer
```bash
# Basic usage
./nlm_debug

# With debugging enabled
./nlm_debug --debug

# With verbose output
./nlm_debug --debug --verbose debug

# Export debug report to file
./nlm_debug --debug --output json --output /tmp/debug_report.json

# Enable real-time monitoring
./nlm_debug --debug --realtime

# Enable educational mode
./nlm_debug --debug --educational

# Full-featured usage
./nlm_debug \
    --debug \
    --verbose debug \
    --output json \
    --output /tmp/comprehensive_report.json \
    --realtime \
    --educational
```

## Usage Examples

### 1. Basic Debugging
```bash
./nlm_debug --debug
```

### 2. Advanced Debugging with Visualization
```bash
./nlm_debug \
    --debug \
    --verbose debug \
    --output html \
    --output /tmp/debug_report.html \
    --realtime
```

### 3. Educational Mode
```bash
./nlm_debug --debug --educational
```

### 4. Batch Processing
```bash
./nlm_debug \
    --debug \
    --verbose trace \
    --output json \
    --output /tmp/batch_debug.json
```

## Keyboard Shortcuts

### Debug Visualization Window
- **ESC**: Exit full-screen
- **SPACE**: Pause/resume simulation
- **S**: Take single step
- **R**: Reset simulation
- **F**: Toggle full-screen
- **T**: Toggle console output
- **H**: Show help
- **1-6**: Switch between debug tools
- **A-Z**: Highlight specific neurons (in brain view)

### Educational Mode
- **Q**: Answer quiz question
- **N**: Next explanation step
- **P**: Previous explanation step
- **L**: Toggle learning mode hints
- **M**: Toggle memory trace visualization
- **C**: Toggle concept formation visualization

## Troubleshooting

### Common Issues

1. **Missing Dependencies**
   ```bash
   sudo apt-get install build-essential cmake pkg-config \
                      libSDL2-dev libGL1-mesa-dev libjson-c-dev \
                      libboost-all-dev
   ```

2. **Graphics Driver Issues**
   - Update graphics drivers
   - Ensure OpenGL support
   - Check for hardware acceleration

3. **Performance Issues**
   - Reduce visualization resolution
   - Disable real-time monitoring
   - Increase simulation step size
   - Monitor system resources

4. **File Permission Issues**
   - Check write permissions in output directory
   - Use absolute paths for output files
   - Ensure user has write access to home directory

### Debugging Tips

1. **Enable verbose logging**:
   ```bash
   ./nlm_debug --debug --verbose trace
   ```

2. **Check log files**:
   Look for `nlm_debug.log` in the current directory

3. **Use smaller datasets**:
   Start with reduced simulation sizes for testing

4. **Monitor system resources**:
   Use system monitoring tools to identify bottlenecks

## Future Enhancements

### 1. Advanced Features
- **Machine Learning Integration**: ML models for predictive analytics
- **Cloud Visualization**: Web-based real-time visualization
- **Plugin Architecture**: Third-party visualization components
- **AR/VR Support**: Virtual reality visualization

### 2. Research Features
- **Data Analysis**: Statistical analysis of neural data
- **Model Comparison**: Compare different neural models
- **Parameter Optimization**: Optimize neural network parameters
- **Simulation Replication**: Replicate published results

## Conclusion

This comprehensive debugging and visualization system will significantly enhance the NLM neural simulation codebase by providing:

1. **Interactive debugging tools** for understanding neural network state
2. **Powerful visualization capabilities** for analyzing neural data
3. **Real-time monitoring** for observing system behavior
4. **Educational features** for learning and understanding
5. **Export capabilities** for documentation and analysis
6. **CLI integration** for flexible usage
7. **Performance optimization** for smooth operation
8. **Extensible architecture** for future enhancements

The implementation will make the NLM neural simulation accessible to a wider range of users, from beginners to advanced researchers, while maintaining compatibility with the existing codebase.
