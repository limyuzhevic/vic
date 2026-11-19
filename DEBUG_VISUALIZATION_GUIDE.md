# Debug and Visualization Tools

This document describes the advanced debugging and visualization tools added to the NLM codebase as part of the improvement phase.

## Overview

The NLM codebase now includes a comprehensive suite of debugging and visualization tools to enhance user experience and provide insights into brain system behavior. These tools include:

### 1. Debug Console
- Real-time brain state inspection
- System metrics monitoring
- Network visualization (simple ASCII/graphical)
- Learning progress tracking
- Debug mode activation via command line

### 2. Enhanced Visualization Interface
- Multiple visualization modes (network, activity, spikes, weights)
- Real-time plotting capabilities
- Post-simulation analysis tools
- Integration with metrics system

### 3. Advanced Command Line Interface
- Debug mode options
- Visualization control
- Configuration dump
- Performance profiling
- System information display

### 4. Integration with Existing Systems
- Integration with Metrics.cpp for statistical tracking
- Enhanced logging and error reporting
- Checkpoint debugging tools

## Usage Examples

### Debug Mode Activation
```bash
./nlm --debug --debug-level=2 --metrics-file=brain_metrics.json
./nlm_phase3_demo --debug --viz-network --viz-activity
./nlm_phase6_demo --debug --viz-all --viz-weights
```

### Visualization Options
```bash
--viz-mode=network    # Display neural network structure
--viz-mode=activity   # Show neural activity patterns
--viz-mode=spikes     # Display spike raster plots
--viz-mode=weights    # Show weight matrix visualization
--viz-mode=all        # All visualization modes
--viz-export=svg      # Export visualizations to SVG format
--viz-export=png      # Export visualizations to PNG format
```

### Performance Monitoring
```bash
--perf-profile=true   # Enable performance profiling
--perf-stats=summary  # Show performance summary
--perf-detailed=true  # Show detailed performance metrics
```

### Configuration Debugging
```bash
--config-dump=true    # Dump current configuration
--config-validate=true # Validate configuration
--config-schema=true  # Show configuration schema
```

## Implementation Details

### Debug Console Features

The Debug Console provides real-time access to:

1. **System Status**
   - Total neuron and synapse counts
   - Current firing rate and spike statistics
   - Memory system states
   - Neuromodulation levels
   - Developmental stage

2. **Network Visualization**
   - ASCII-based network graph
   - Region connectivity display
   - Synaptic weight distribution
   - Neuron type distribution

3. **Learning Progress Tracking**
   - Synaptic weight changes over time
   - Learning curve analysis
   - Convergence metrics
   - Performance statistics

4. **Interactive Commands**
   - `help` - Show available commands
   - `status` - Display current system state
   - `viz [mode]` - Enable specific visualization
   - `metrics [name]` - Show metric values
   - `network` - Display network structure
   - `spikes` - Show spike statistics
   - `quit` - Exit debug mode

### Enhanced Visualization Interface

The VisualizationInterface now supports:

1. **Multiple Visualization Modes**
   - **Network Mode**: Display neural network topology
   - **Activity Mode**: Show real-time neural activity patterns
   - **Spikes Mode**: Display spike raster plots and timing
   - **Weights Mode**: Show synaptic weight matrices
   - **Combined Mode**: Multi-panel visualization

2. **Export Capabilities**
   - SVG export for vector graphics
   - PNG export for raster images
   - JSON export for data analysis
   - CSV export for external processing

3. **Real-time Updates**
   - Smooth frame rate updates
   - Configurable update rates
   - Automatic data scaling
   - Interactive zooming/panning

### Integration with Metrics System

The Metrics class has been enhanced to work with debugging tools:

1. **Brain Performance Metrics**
   - Synaptic weight changes
   - Firing rate statistics
   - Learning progress metrics
   - Memory system performance

2. **System Resource Metrics**
   - CPU usage
   - Memory consumption
   - Network bandwidth
   - Disk I/O patterns

3. **Export and Analysis**
   - JSON export for web visualization
   - CSV export for spreadsheet analysis
   - Summary statistics for quick reports
   - Trend analysis for learning curves

## Advanced Commands for Power Users

### Command Line Options

```bash
# Debug and monitoring options
--debug                Enable debug console
--debug-level=N        Set debug level (1-3)
--viz-mode=MODE        Set visualization mode
--viz-update-rate=FPS  Set visualization update rate
--perf-profile=true    Enable performance profiling
--metrics-file=FILE    Save metrics to file
--config-dump=true     Dump configuration to console

# Visualization options
--viz-enable=true      Enable visualization
--viz-fullscreen=true Enable fullscreen mode
--viz-export=FORMAT    Export visualization (svg, png, json, csv)

# System information
--info                Display system information
--version             Display version information
--help                Display help information

# Configuration
--config=FILE         Load configuration file
--config-set=KEY=VAL  Set configuration value
--config-get=KEY      Get configuration value
```

### Interactive Debug Commands

Once debug mode is activated, users can interact with the debug console:

```
> help      - Display help information
> status    - Display current system state
> network   - Display network structure
> viz activity - Show neural activity patterns
> viz weights - Display synaptic weight matrix
> metrics all - Show all recorded metrics
> spikes     - Display spike statistics
> quit      - Exit debug mode
```

### Configuration Commands

```bash
# Set configuration values
./nlm --config-set=neuron_count=500 --config-set=region_count=2

# Get configuration values
./nlm --config-get=neuron_count

# Validate configuration
./nlm --config-validate=true
```

## Performance Optimizations

### Efficient Visualization Updates

- Smooth rendering with configurable frame rates
- Real-time data compression for network visualization
- Memory-efficient metrics storage
- Progressive loading for large networks

### Memory Management

- Efficient memory allocation for debug data structures
- Automatic cleanup of unused metrics
- Memory usage profiling
- Memory leak detection

### Real-time Monitoring

- Low-overhead metrics collection
- Efficient spike recording
- Network activity tracking
- Resource usage monitoring

## Integration Examples

### Using Debug Console in Phase 3 Demo

```cpp
void run(int numEpisodes, int stepsPerEpisode) {
    // Enable debug mode
    std::string debugLevel = std::getenv("NLM_PHASE3_DEBUG") ? 
        std::getenv("NLM_PHASE3_DEBUG") : "1";
    
    DebugConsole debugConsole;
    debugConsole.initialize(stoi(debugLevel));
    debugConsole.startMonitoring();
    
    // Run episodes with monitoring
    for (int episode = 0; episode < numEpisodes; ++episode) {
        // ... episode logic ...
        
        // Print periodic debug information
        if (episode % 10 == 0) {
            debugConsole.logEpisodeMetrics(episode, metrics);
        }
    }
    
    debugConsole.stopMonitoring();
}
```

### Using Visualization in Phase 6 Demo

```cpp
void runIntegrationDemo() {
    // Initialize visualization
    VisualizationInterface viz;
    viz.initialize();
    viz.setUpdateRate(30.0);
    
    // Enable all visualization modes
    viz.enableNetworkViz(true);
    viz.enableActivityViz(true);
    viz.enableSpikesViz(true);
    viz.enableWeightsViz(true);
    
    // Run experiment with visualization
    auto result = experiment.run(config);
    
    // Export results
    if (getenv("NLM_EXPORT_VIZ")) {
        viz.exportToSVG("brain_visualization.svg");
        viz.exportToPNG("brain_visualization.png");
        viz.exportToJSON("brain_metrics.json");
    }
    
    viz.close();
}
```

## Benefits for Users

### For New Users
- Intuitive command-line interface
- Comprehensive help documentation
- Default visualization modes
- Easy configuration management

### For Advanced Users
- Extensive configuration options
- Real-time system monitoring
- Advanced visualization capabilities
- Performance profiling tools
- Interactive debugging console

### For Researchers
- Detailed metrics collection
- Network visualization tools
- Learning progress tracking
- Data export for analysis
- Statistical analysis capabilities

### For Developers
- Extensible debugging framework
- Modular visualization system
- Comprehensive error handling
- Performance monitoring
- Integration with existing systems

## Configuration Files

### Debug Configuration
```
# debug.cfg

[debug]
enabled = true
level = 2
console = true
file_output = true
log_file = debug.log

[visualization]
enabled = true
mode = all
update_rate = 30.0
fullscreen = false

[metrics]
enabled = true
metrics_file = metrics.json
record_interval = 1000

[performance]
profile = true
profile_file = profile.data
```

### Sample Debug Session Output

```
NLM Debug Console - Phase 3 Demo
================================
Episode: 0/5
Time: 0:00:00

System Status:
  Neurons: 500
  Synapses: 12500
  Firing Rate: 12.3 Hz
  Total Spikes: 8,420
  Reward: -2.1
  Energy: 95.2%

Network Structure:
  Regions: 2
  Sensory Region: 42 neurons
  Motor Region: 458 neurons
  Connectivity: 8.3%

Learning Progress:
  Synaptic Weight Change: 0.15
  Network Integration: 87%
  Performance Improvement: +15.2%

Recent Activity:
  Spike Rate: 12.8 spikes/sec
  Firing Neurons: 43/500
  Novelty Level: 0.32
  Curiosity Level: 0.41

Debug Commands:
  help     - Show help
  status   - System status
  viz      - Visualization options
  metrics  - Metric information
  network  - Network structure
  spikes    - Spike statistics
  quit     - Exit debug mode
```

## Future Enhancements

### Planned Features
1. **Advanced Visualization**
   - 3D network visualization
   - Interactive graph algorithms
   - Machine learning-based visualization
   - Virtual reality integration

2. **Enhanced Debugging**
   - Automated bug detection
   - Performance anomaly detection
   - Memory leak detection
   - Real-time system profiling

3. **Integration with External Tools**
   - Web-based visualization dashboard
   - Integration with monitoring systems
   - Cloud-based analysis
   - Real-time collaboration

4. **User Interface Improvements**
   - Graphical user interface
   - Multi-window support
   - Customizable dashboards
   - Theme support

## Conclusion

The enhanced debugging and visualization tools significantly improve the NLM project's usability, debugging capabilities, and user experience. These tools provide:

1. **Better User Experience**: Intuitive interfaces and comprehensive help
2. **Enhanced Debugging**: Real-time monitoring and system inspection
3. **Advanced Visualization**: Multiple visualization modes and export capabilities
4. **Performance Optimization**: Efficient resource usage and real-time monitoring
5. **Research Support**: Detailed metrics and analysis tools
6. **Developer Support**: Extensible framework and comprehensive documentation

These improvements make the NLM project more accessible to new users while providing powerful tools for advanced users and researchers.