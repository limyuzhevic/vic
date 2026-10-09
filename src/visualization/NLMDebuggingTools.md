# NLM Visualization and Debugging System

This document provides comprehensive documentation for the NLM Visualization and Debugging System (VDS), a complete set of tools for monitoring, analyzing, and debugging NLM neural simulations.

## Overview

The Visualization and Debugging System is a comprehensive set of tools that enables:

- **Real-time visualization** of neural networks and their dynamics
- **Statistical analysis** of neural activity patterns
- **Brain state debugging** with snapshot comparison
- **Performance profiling** and bottleneck identification
- **Memory usage analysis** with leak detection
- **Configuration validation** visualizer
- **Interactive development tools** for parameter adjustment
- **Comprehensive export capabilities** for multiple formats
- **Plugin architecture** for custom visualizers
- **Real-time data streaming**
- **Automated report generation**

## System Architecture

The VDS is organized into several main components:

### 1. Core Visualization Engine
- **VisualizationEngine**: Main coordinator that manages all visualization components
- **NetworkVisualizer**: Neural network graph visualization with multiple layout algorithms
- **ActivityAnalyzer**: Statistical analysis of neural activity patterns
- **BrainStateDebugger**: Debugging and comparison tools for brain states
- **PerformanceProfiler**: Performance monitoring and bottleneck analysis
- **MemoryAnalyzer**: Memory usage and leak detection
- **DevelopmentInterface**: Interactive development tools for parameter adjustment
- **ExportManager**: Output format handling and export utilities
- **PluginManager**: Plugin architecture for extensibility

### 2. Visualization Components

#### Network Visualization
- Graph visualization of neural networks
- Spike train visualization (raster plots)
- Network connectivity visualization
- Memory state visualization
- Neuromodulation effect visualization
- Development stage visualization
- Real-time simulation animation
- 3D visualization support

#### Activity Analysis
- Firing rate statistics and distributions
- Synchrony and coherence measures
- Burst detection
- Pattern recognition
- Spectral analysis
- Recurrence quantification analysis
- Cross-region correlation analysis

#### Brain State Debugging
- Snapshot comparison and change detection
- Anomaly detection and reporting
- Memory leak detection
- Performance bottleneck identification
- Configuration validation
- Development stage analysis
- Neuromodulation balance checking

### 3. Interactive Development Tools

#### Real-time Control Interface
- Parameter adjustment tools
- Real-time simulation control
- Neural parameter tuning
- Memory management visualization
- Development stage control

#### Scriptable Debugging Interface
- Scripted debugging operations
- Automated testing frameworks
- Custom debug commands
- Export debugging results

### 4. Output Formats

#### Console Visualization
- Real-time console output
- Formatted statistics display
- Progress tracking
- Status monitoring

#### Static Image Export
- PNG export with configurable quality
- SVG export for scalable graphics
- PDF export for documentation
- High-resolution output support

#### Interactive Web Interface
- HTML-based visualization
- JavaScript-based interactivity
- Responsive design
- Export capabilities

#### Video Recording
- Simulation recording
- Frame capture
- Video encoding
- Customizable frame rates

#### 3D Visualization
- 3D neural network representation
- Interactive rotation and zoom
- Multi-scale visualization
- Depth perception

## Integration with NLM

The VDS integrates seamlessly with the existing NLM system:

### Key Integration Points

1. **Brain State Access**: The VDS accesses neural data through the Brain interface, retrieving information about neurons, synapses, memory systems, and neuromodulators.

2. **Real-time Streaming**: The VDM supports real-time data streaming from the NLM simulation to visualization components.

3. **Plugin Architecture**: Custom visualizers can be developed as plugins that integrate with the VDS.

4. **Configuration Integration**: VDS configuration can be integrated with NLM configuration system.

5. **Performance Monitoring**: VDS monitors and optimizes NLM performance through profiling and bottleneck detection.

### Usage Examples

#### Basic Network Visualization
```cpp
#include "VisualizationEngine.hpp"
#include "VisualizationData.hpp"

int main() {
    // Initialize NLM brain
    auto brain = nlm::BrainFactory::createDefaultConfig();
    
    // Initialize visualization engine
    auto& vizEngine = VisualizationEngine::getInstance();
    if (!vizEngine.initialize(brain)) {
        std::cerr << "Failed to initialize visualization engine" << std::endl;
        return 1;
    }
    
    // Start real-time visualization
    vizEngine.startVisualization();
    
    // Run simulation loop
    while (simulation_running) {
        // Step brain simulation
        brain->step(currentStep);
        
        // Get visualization data
        VisualizationData data = vizEngine.getCurrentData();
        
        // Update visualization
        vizEngine.update(data);
        vizEngine.render();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60 FPS
    }
    
    // Stop visualization
    vizEngine.stopVisualization();
    
    return 0;
}
```

#### Activity Analysis
```cpp
#include "ActivityAnalyzer.hpp"

int main() {
    // Initialize NLM brain
    auto brain = nlm::BrainFactory::createDefaultConfig();
    
    // Initialize activity analyzer
    ActivityAnalyzer analyzer;
    if (!analyzer.initialize(brain)) {
        std::cerr << "Failed to initialize activity analyzer" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < max_steps; ++step) {
        brain->step(step);
        
        // Get visualization data
        VisualizationData data = vizEngine.getCurrentData();
        
        // Update analyzer
        analyzer.update(data);
        
        // Get activity statistics
        ActivityStats stats = analyzer.getCurrentStats();
        
        // Print statistics
        std::cout << "Step " << step << ": "
                  << "Firing Rate: " << stats.meanFiringRate << " Hz, "
                  << "Synchrony: " << stats.synchronyLevel << ", "
                  << "Skewness: " << stats.skewnessFiringRate << std::endl;
    }
    
    // Export analysis results
    analyzer.exportAnalysisResults("activity_analysis.json");
    
    return 0;
}
```

#### Brain State Debugging
```cpp
#include "BrainStateDebugger.hpp"

int main() {
    // Initialize NLM brain
    auto brain = nlm::BrainFactory::createDefaultConfig();
    
    // Initialize brain state debugger
    BrainStateDebugger debugger;
    if (!debugger.initialize(brain)) {
        std::cerr << "Failed to initialize brain state debugger" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < max_steps; ++step) {
        brain->step(step);
        
        // Capture snapshot
        BrainSnapshot snapshot = debugger.captureSnapshot("Simulation step " + std::to_string(step));
        
        // Check for anomalies
        if (debugger.detectAnomalies(snapshot)) {
            DebugReport report = debugger.generateAnomalyReport(snapshot);
            
            // Handle anomalies
            if (report.hasCriticalIssues) {
                std::cerr << "CRITICAL: " << report.errors[0] << std::endl;
                // Take corrective action
            } else {
                std::cout << "WARNING: " << report.warnings[0] << std::endl;
            }
        }
        
        // Compare with previous state
        if (step > 100) {
            ComparisonResult comparison = debugger.compareWithPrevious(snapshot);
            if (comparison.isSignificant) {
                std::cout << "Significant state change detected at step " << step << std::endl;
            }
        }
    }
    
    // Generate final report
    DebugReport finalReport = debugger.generateFinalReport();
    debugger.exportDebugReport(finalReport, "debug_report.html");
    
    return 0;
}
```

#### Performance Profiling
```cpp
#include "PerformanceProfiler.hpp"

int main() {
    // Initialize NLM brain
    auto brain = nlm::BrainFactory::createDefaultConfig();
    
    // Initialize performance profiler
    PerformanceProfiler profiler;
    if (!profiler.initialize(brain)) {
        std::cerr << "Failed to initialize performance profiler" << std::endl;
        return 1;
    }
    
    // Start profiling
    profiler.startProfiling();
    
    // Run simulation
    for (size_t step = 0; step < max_steps; ++step) {
        brain->step(step);
    }
    
    // Stop profiling
    profiler.stopProfiling();
    
    // Analyze performance
    PerformanceReport report = profiler.analyzePerformance();
    
    // Check for bottlenecks
    std::vector<PerformanceBottleneck> bottlenecks = profiler.detectPerformanceBottlenecks();
    if (!bottlenecks.empty()) {
        std::cout << "Performance bottlenecks detected:" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            std::cout << "  - " << bottleneck.description << " (severity: " 
                      << bottleneck.severity << ")" << std::endl;
        }
    }
    
    // Export performance report
    profiler.exportPerformanceReport("performance_report.html");
    
    return 0;
}
```

#### Memory Analysis
```cpp
#include "MemoryAnalyzer.hpp"

int main() {
    // Initialize NLM brain
    auto brain = nlm::BrainFactory::createDefaultConfig();
    
    // Initialize memory analyzer
    MemoryAnalyzer analyzer;
    if (!analyzer.initialize(brain)) {
        std::cerr << "Failed to initialize memory analyzer" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < max_steps; ++step) {
        brain->step(step);
        
        // Record allocations
        analyzer.recordAllocation(); // Implementation depends on allocation tracking
        
        // Update memory usage
        size_t memoryUsage = analyzer.getCurrentMemoryUsage();
        
        // Check for memory issues
        if (memoryUsage > analyzer.getMemoryLeakDetectionThreshold()) {
            std::cout << "Memory usage high: " << memoryUsage << " bytes" << std::endl;
        }
    }
    
    // Detect memory leaks
    std::vector<MemoryLeak> leaks = analyzer.detectMemoryLeaks();
    if (!leaks.empty()) {
        std::cout << "Memory leaks detected: " << leaks.size() << std::endl;
        for (const auto& leak : leaks) {
            std::cout << "  - " << leak.description << " (" << leak.leakSize << " bytes)" << std::endl;
        }
    }
    
    // Analyze fragmentation
    MemoryFragmentation fragmentation = analyzer.analyzeFragmentation();
    if (fragmentation.isSevere) {
        std::cout << "Severe memory fragmentation detected" << std::endl;
    }
    
    // Export memory analysis
    analyzer.exportMemoryAnalysis("memory_analysis.json");
    
    return 0;
}
```

#### Export Visualization
```cpp
#include "ExportManager.hpp"

int main() {
    // Initialize NLM brain
    auto brain = nlm::BrainFactory::createDefaultConfig();
    
    // Initialize export manager
    ExportManager exporter;
    if (!exporter.initialize(brain)) {
        std::cerr << "Failed to initialize export manager" << std::endl;
        return 1;
    }
    
    // Configure export options
    ExportOptions options;
    options.format = ExportFormat::PNG;
    options.filename = "neural_network";
    options.outputPath = "./exports";
    options.title = "NLM Neural Network Visualization";
    options.description = "Visualization of the NLM neural network at simulation step 100";
    options.quality = 0.95f;
    options.resolutionWidth = 1920;
    options.resolutionHeight = 1080;
    options.includeTimestamps = true;
    options.includeMetadata = true;
    
    // Export network visualization
    ExportReport report = exporter.exportNetworkVisualization(options);
    
    if (report.success) {
        std::cout << "Successfully exported visualization to: " << report.outputPath << std::endl;
    } else {
        std::cerr << "Export failed: " << report.errorMessage << std::endl;
    }
    
    // Export activity analysis
    ExportOptions activityOptions;
    activityOptions.format = ExportFormat::CSV;
    activityOptions.filename = "activity_analysis";
    activityOptions.outputPath = "./exports";
    activityOptions.title = "NLM Activity Analysis";
    
    ExportReport activityReport = exporter.exportActivityAnalysis(activityOptions);
    if (activityReport.success) {
        std::cout << "Successfully exported activity analysis to: " << activityReport.outputPath << std::endl;
    }
    
    // Export brain state
    ExportOptions brainStateOptions;
    brainStateOptions.format = ExportFormat::JSON;
    brainStateOptions.filename = "brain_state";
    brainStateOptions.outputPath = "./exports";
    brainStateOptions.title = "NLM Brain State";
    
    ExportReport brainStateReport = exporter.exportBrainState(brainStateOptions);
    if (brainStateReport.success) {
        std::cout << "Successfully exported brain state to: " << brainStateReport.outputPath << std::endl;
    }
    
    return 0;
}
```

## Plugin Development

The VDS supports plugin development for custom visualizers and analysis tools:

### Plugin Architecture

Plugins can be developed as shared libraries that implement the Plugin interface:

```cpp
class CustomVisualizer : public VisualizationPlugin {
public:
    std::string getPluginId() const override {
        return "custom_visualizer";
    }
    
    std::string getPluginName() const override {
        return "Custom Neural Network Visualizer";
    }
    
    std::string getPluginVersion() const override {
        return "1.0.0";
    }
    
    bool initialize(const PluginContext& context) override {
        // Initialize plugin
        return true;
    }
    
    void shutdown() override {
        // Clean up plugin
    }
    
    void visualize(const VisualizationData& data, const ExportOptions& options) override {
        // Custom visualization implementation
    }
    
    std::vector<std::string> getSupportedFormats() const override {
        return {"png", "svg", "pdf"};
    }
};
```

### Plugin Registration

Plugins are automatically discovered and registered when loaded:

```cpp
// In plugin main function
extern "C" Plugin* createPlugin() {
    return new CustomVisualizer();
}
```

## Configuration

The VDS can be configured through various mechanisms:

### Command-line Options
```bash
nlm visualize --network --activity --export
nlm debug --snapshots --compare
nlm profile --performance --memory
nlm memory --analysis --leaks
```

### Configuration File
```json
{
  "visualization": {
    "engine": {
      "update_rate": 30.0,
      "enable_real_time": true
    },
    "network": {
      "layout_algorithm": "force_directed",
      "show_connections": true,
      "show_regions": true
    },
    "activity": {
      "compute_statistics": true,
      "detect_bursts": true,
      "spectral_analysis": true
    }
  },
  "debugging": {
    "brain_state": {
      "snapshot_interval": 100,
      "comparison_threshold": 0.7
    },
    "performance": {
      "profile_interval": 1000,
      "bottleneck_threshold": 2.0
    }
  }
}
```

## Performance Considerations

The VDS is designed for:

- **Low latency**: Real-time visualization with minimal overhead
- **High throughput**: Handles thousands of neurons and synapses
- **Memory efficiency**: Streaming visualization for large networks
- **Scalability**: Distributed visualization for very large networks
- **Extensibility**: Plugin architecture for custom visualizers

## Build Instructions

To build the VDS:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_VIZ_ENABLE_TESTS=ON
make -j4
```

To enable specific visualization backends:

```bash
cmake .. -DNLM_VIZ_BACKEND=OPENGL
cmake .. -DNLM_VIZ_BACKEND=Qt
cmake .. -DNLM_VIZ_BACKEND=SFML
```

To enable tests:

```bash
make test
```

## Testing

Run the visualization tests:

```bash
./test_viz
```

Or run all tests:

```bash
ctest --output-on-failure
```

## License

MIT License - See LICENSE file for details.

## Contributors

The NLM Visualization and Debugging System was developed as part of the NLM project. Contributions are welcome!

## Contact

For questions, issues, or contributions, please contact:

- GitHub: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io

## Version Information

- **Version**: 0.1.0
- **Status**: Alpha
- **Development Phase**: Phase 6 (FINAL INTEGRATION)
- **Dependencies**: NLM Core Library, C++17, Modern OpenGL/SFML/Qt (for GUI)

---

This comprehensive visualization and debugging system provides NLM users with powerful tools for understanding, analyzing, and optimizing their neural simulations. The modular architecture allows for easy extension and customization to meet specific research needs.