# Debugging and Visualization Utilities for NLM

This directory contains debugging and visualization tools for the NLM (Neural Learning Machine) project.

## Overview

The debugging and visualization system provides:
- Neural network visualization tools
- Brain state visualization
- Performance monitoring
- Debug logging and analysis
- Interactive debugging interfaces
- Visualization of neural dynamics and learning processes

## Components

### 1. Neural Network Visualizer
Interactive tool for visualizing neural networks, including:
- Neural population visualization
- Synapse connectivity visualization
- Spike activity visualization
- Weight change visualization
- Network topology visualization

### 2. Brain State Visualizer
Tools for visualizing brain state, including:
- Working memory visualization
- Episodic memory visualization
- Neuromodulation visualization
- Development stage visualization
- Prediction system visualization

### 3. Performance Monitor
Real-time performance monitoring, including:
- CPU and memory usage tracking
- Network performance metrics
- Simulation speed monitoring
- Resource usage analysis

### 4. Debug Logger
Comprehensive debug logging system, including:
- Event logging
- Error tracking
- Performance logging
- State change logging
- Interactive debugging

### 5. Visualization API
Programmatic interface for creating visualizations, including:
- 2D and 3D rendering
- Network graph visualization
- Time series visualization
- Statistical visualization
- Custom visualization creation

## Usage

### Quick Start
```bash
# Start neural network visualizer
nlmviz network --help

# Start brain state visualizer
nlmviz brain --help

# Start performance monitor
nlmviz monitor --help

# Start debug logger
nlmviz logger --help
```

### Visualization Examples
```bash
# Visualize neural network topology
nlmviz network --input brain_state.bin --output network_viz.png

# Visualize brain activity over time
nlmviz brain --input simulation_data.bin --output brain_viz.html

# Monitor performance in real-time
nlmviz monitor --port 8080 --output perf_dashboard.html

# Debug neural network
nlmviz logger --input log_file.log --output debug_analysis.json
```

## Files

### Core Visualization Modules
- `src/visualization/NeuralNetworkViz.cpp` - Neural network visualization
- `src/visualization/BrainStateViz.cpp` - Brain state visualization
- `src/visualization/PerformanceMonitor.cpp` - Performance monitoring
- `src/debug/Logger.cpp` - Debug logging
- `src/debug/DebugAnalyzer.cpp` - Debug analysis

### Visualization Interface
- `include/nlmviz/Visualizer.hpp` - Base visualizer interface
- `include/nlmviz/NeuralNetworkVisualizer.hpp` - Neural network visualizer
- `include/nlmviz/BrainStateVisualizer.hpp` - Brain state visualizer
- `include/nlmviz/PerformanceMonitor.hpp` - Performance monitor
- `include/nlm/debug/DebugLogger.hpp` - Debug logger

### Command Line Tools
- `tools/nlmviz.cpp` - Main visualization tool
- `tools/nlmnetviz.cpp` - Neural network visualization tool
- `tools/nlmbrainviz.cpp` - Brain state visualization tool
- `tools/nlmperfmon.cpp` - Performance monitoring tool
- `tools/nlmdbglog.cpp` - Debug logging tool

## Features

### Neural Network Visualization
- **Graph Visualization**: Interactive network graphs with nodes and edges
- **Spiking Activity**: Real-time visualization of spike activity
- **Synaptic Weights**: Visual representation of synaptic weights and changes
- **Network Topology**: Analysis and visualization of network structure
- **Population Dynamics**: Visualization of population-level activity

### Brain State Visualization
- **Memory Systems**: Visualization of working memory and episodic memory
- **Neuromodulation**: Visualization of neuromodulator effects
- **Development Stages**: Visualization of brain development over time
- **Prediction System**: Visualization of prediction accuracy and errors
- **Cognition**: Visualization of cognitive processes

### Performance Monitoring
- **Real-time Metrics**: Live monitoring of system performance
- **Resource Usage**: CPU, memory, and I/O usage tracking
- **Network Performance**: Bandwidth, latency, and throughput monitoring
- **Simulation Speed**: Real-time simulation speed monitoring
- **Performance Analytics**: Historical performance analysis

### Debug Logging
- **Event Logging**: Comprehensive event logging
- **Error Tracking**: Detailed error and warning logging
- **State Tracking**: Logging of state changes and transitions
- **Performance Logging**: Performance metrics and benchmarks
- **Interactive Analysis**: Interactive debugging and analysis tools

## Integration

### NLM Integration
- **Plugin Architecture**: Integration with NLM brain components
- **Data Import/Export**: Support for NLM data formats
- **Real-time Visualization**: Live visualization of NLM simulations
- **Interactive Control**: Control NLM simulations through visualization tools

### External Tools Integration
- **External Data Sources**: Import data from external sources
- **Export Formats**: Export visualizations in various formats
- **Third-party Tools**: Integration with external analysis tools
- **Web Integration**: Web-based visualization interfaces

## Configuration

### Configuration Files
- `config/viz_config.json` - Visualization configuration
- `config/debug_config.json` - Debug configuration
- `config/monitor_config.json` - Monitoring configuration

### Command Line Options
- `--input FILE` - Input data file
- `--output FILE` - Output file
- `--format FORMAT` - Output format (png, html, json, etc.)
- `--width WIDTH` - Output width
- `--height HEIGHT` - Output height
- `--fps FPS` - Frames per second
- `--duration SECONDS` - Duration of visualization
- `--interactive` - Enable interactive mode
- `--export-data` - Export visualization data

## Development

### Building Visualization Tools
```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_VIZ=ON
make
```

### Adding New Visualizations
1. Create a new visualizer class in `src/visualization/`
2. Implement the `Visualizer` interface
3. Add command-line support in `tools/`
4. Update CMakeLists.txt if needed

### Creating Custom Visualizations
```cpp
class CustomVisualizer : public Visualizer {
public:
    bool initialize(const map<string, string>& config) override;
    bool render(const void* data, const RenderOptions& options) override;
    bool cleanup() override;
    string getName() const override;
    string getDescription() const override;
};
```

## Examples

### Basic Visualization
```cpp
#include <nlmviz/NeuralNetworkVisualizer.hpp>
#include <nlmviz/Visualizer.hpp>

int main() {
    // Create neural network visualizer
    NeuralNetworkVisualizer visualizer;
    
    // Configure visualization
    map<string, string> config;
    config["width"] = "800";
    config["height"] = "600";
    config["title"] = "Neural Network Visualization";
    
    // Initialize visualizer
    if (!visualizer.initialize(config)) {
        cerr << "Failed to initialize visualizer" << endl;
        return 1;
    }
    
    // Render visualization
    // (In a real implementation, this would render the neural network)
    
    // Cleanup
    visualizer.cleanup();
    
    return 0;
}
```

### Performance Monitoring
```cpp
#include <nlmviz/PerformanceMonitor.hpp>

int main() {
    // Create performance monitor
    PerformanceMonitor monitor;
    
    // Configure monitor
    PerformanceMonitorConfig config;
    config.port = 8080;
    config.outputFile = "perf_dashboard.html";
    config.updateInterval = 1.0;
    
    // Start monitoring
    if (!monitor.start(config)) {
        cerr << "Failed to start performance monitor" << endl;
        return 1;
    }
    
    // Run simulation
    // (In a real implementation, this would run an NLM simulation)
    
    // Stop monitoring
    monitor.stop();
    
    return 0;
}
```

## License

MIT License - See LICENSE file for details.

## Future Enhancements

### Planned Features
- **3D Visualization**: Three-dimensional network visualization
- **Virtual Reality**: VR support for immersive visualization
- **Machine Learning**: AI-powered visualization recommendations
- **Cloud Integration**: Cloud-based visualization services
- **Mobile Support**: Mobile device visualization

### Performance Improvements
- **Real-time Rendering**: Real-time network visualization
- **Large Network Support**: Visualization of networks with thousands of neurons
- **Memory Optimization**: Efficient memory usage for large visualizations
- **Parallel Processing**: Multi-core visualization rendering

### Integration Improvements
- **AR/VR Integration**: Augmented and virtual reality integration
- **IoT Visualization**: Internet of Things integration
- **Edge Computing**: Edge-based visualization for distributed systems
- **Cross-platform Support**: Support for multiple platforms and devices