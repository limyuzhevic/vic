# NLM Brain Visualization and Debugging Tools

This directory contains the implementation of visualization and debugging tools for the NLM (Neural Learning Machine) project.

## Overview

The NLM project has introduced comprehensive visualization and debugging tools to support the complex artificial brain system. These tools provide:

### Core Components

1. **BrainVisualizer** - 2D/3D visualization of neural activity, connectivity matrices, and time series
2. **DebugConsole** - Interactive debugging with breakpoints, watch variables, and expression evaluation
3. **DataAnalyzer** - Statistical analysis and multiple export formats (JSON, CSV, HDF5, Parquet)
4. **PerformanceProfiler** - Real-time performance monitoring
5. **PluginSystem** - Plugin architecture for custom visualizers
6. **VisualizationAPI** - Integration layer with full Brain, Python, and CLI support

## Files Created/Updated

### New Header Files

- `src/visualization/BrainVisualizer.hpp` - Main 2D/3D visualization implementation
- `src/plugins/PluginSystem.hpp` - Plugin system interface

### New Source Files

- `src/plugins/PluginSystem.cpp` - Plugin system implementation

### Updated Files

- `src/brain/Brain.hpp` - Added visualization methods
- `src/brain/Brain.cpp` - Added visualization implementations
- `src/integration/VisualizationAPI.hpp` - Updated with visualization core
- `src/visualization/VisualizationCore.hpp` - Added VisualizationData structure
- `python/bindings.cpp` - Added Python bindings for visualization API

### Existing Files (Enhanced)

- `src/visualization/BrainVisualizer.cpp` - Enhanced with new features
- `src/debugging/DebugConsole.hpp` - Enhanced
- `src/debugging/DebugConsole.cpp` - Enhanced
- `src/analysis/DataAnalyzer.hpp` - Enhanced
- `src/analysis/DataAnalyzer.cpp` - Enhanced
- `src/profiling/PerformanceProfiler.hpp` - Enhanced
- `src/profiling/PerformanceProfiler.cpp` - Enhanced
- `src/integration/VisualizationAPI.hpp` - Enhanced
- `src/integration/VisualizationAPI.cpp` - Enhanced

## Key Features

✅ **Brain Visualization**
   - Neural activity visualization
   - Connectivity matrix display
   - Time series visualization
   - 2D/3D projection capabilities

✅ **Interactive Debug Console**
   - Breakpoints and watch variables
   - Expression evaluation
   - Call stack inspection
   - Memory and performance monitoring

✅ **Statistical Analysis**
   - Multiple statistical methods
   - JSON, CSV, HDF5, Parquet export formats
   - Pattern detection and feature extraction
   - Spectral analysis

✅ **Real-time Performance Profiling**
   - CPU and memory usage tracking
   - Bottleneck detection
   - System resource management
   - Real-time monitoring

✅ **Plugin Architecture**
   - Custom visualizer plugins
   - Dynamic plugin registration
   - Plugin factory pattern
   - Plugin management and configuration

✅ **Full Integration**
   - Native C++ API
   - Python bindings via pybind11
   - CLI commands
   - Configuration management

## Usage Examples

### Python Usage

```python
import pynlm

# Create visualization API
viz_api = pynlm.createVisualizationAPI()

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Generate visualizations
brain.generateVisualization("brain_network.png", "png")
brain.exportBrainData("brain_data.json", "json")
brain.exportTimeSeriesData("brain_stats.csv", "csv")

# Get visualization statistics
stats = brain.getVisualizationStats()
print(f"Frame count: {stats['frame_count']}")
print(f"FPS: {stats['fps']:.2f}")
```

### C++ Usage

```cpp
#include "src/brain/Brain.hpp"
#include "src/visualization/BrainVisualizer.hpp"
#include "src/integration/VisualizationAPI.hpp"

// Create brain with configuration
std::shared_ptr<Config> config = std::make_shared<Config>();
Brain brain(config);
brain.initialize();

// Generate visualization
brain.generateVisualization("brain_network.png", "png");

// Export data
brain.exportBrainData("brain_data.json", "json");
brain.exportConnectivityMatrix("connectivity.csv", "csv");

// Update and render visualization
brain.updateVisualization(0.1);
brain.renderVisualization();
```

### Command Line Interface

```bash
# Create and run brain with visualization
nlm --config config.json --visualize --output output.png

# Export brain data
nlm --brain brain.bin --export-data brain.json --export-format json

# Run with debug console
nlm --brain brain.bin --debug-console
```

## Available Commands

### Visualization Commands

- `generateVisualization(filename, format)` - Generate brain visualization
- `exportBrainData(filename, format)` - Export brain data
- `exportConnectivityMatrix(filename, format)` - Export connectivity matrix
- `exportTimeSeriesData(filename, format)` - Export time series data
- `updateVisualization(deltaTime)` - Update visualization
- `renderVisualization()` - Render visualization

### Debug Console Commands

- `addWatchVariable(expression)` - Add watch variable
- `removeWatchVariable(expression)` - Remove watch variable
- `setBreakpoint(line, file)` - Set breakpoint
- `evaluateExpression(expression)` - Evaluate expression
- `getCallStack()` - Get call stack

### Analysis Commands

- `getTimeSeriesData(metricName)` - Get time series data
- `getStatistic(metricName, statType)` - Get statistic
- `getSpectralComponent(metricName, frequency)` - Get spectral component
- `detectSpikes(metricName, threshold)` - Detect spikes
- `exportData(filename, format)` - Export data

### Performance Commands

- `startProfile(sectionName)` - Start profiling
- `stopProfile(sectionName)` - Stop profiling
- `getProfileData(sectionName)` - Get profile data
- `getTotalExecutionTime()` - Get total execution time
- `exportProfile(filename, format)` - Export profile

## Configuration

Visualization tools support configuration via:

1. **Config files** - JSON configuration files
2. **Command line arguments** - Runtime configuration
3. **API calls** - Programmatic configuration
4. **Environment variables** - System-wide settings

### Example Configuration

```json
{
    "visualization": {
        "enabled": true,
        "update_rate": 60.0,
        "render_rate": 60.0,
        "theme": "dark",
        "quality": "high",
        "plugins": [
            {
                "name": "network_visualizer",
                "type": "visualization",
                "enabled": true
            }
        ]
    }
}
```

## Plugin Architecture

The plugin system allows for custom visualizers and analysis tools:

### Plugin Interface

Plugins implement the `Plugin` interface:

```cpp
class Plugin {
public:
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update(double deltaTime) = 0;
    // ... other methods
};
```

### Visualization Plugins

Visualization plugins implement the `VisualizationPlugin` interface:

```cpp
class VisualizationPlugin : public Plugin {
public:
    virtual VisualizationType getVisualizationType() const = 0;
    virtual std::shared_ptr<VisualizationData> getData() const = 0;
    virtual void update(const Brain& brain, double deltaTime) = 0;
    virtual void render() = 0;
    // ... other methods
};
```

### Plugin Registration

Plugins are registered through the `PluginSystem`:

```cpp
PluginSystem pluginSystem;
pluginSystem.initialize();

// Register a plugin
std::unique_ptr<Plugin> plugin = createCustomPlugin();
pluginSystem.registerPlugin(std::move(plugin));
```

## Performance Considerations

1. **Real-time Performance**: Enable real-time visualization only when necessary
2. **Export Formats**: Choose appropriate formats based on data size and requirements
3. **Memory Usage**: Monitor memory usage for large datasets
4. **Plugin Management**: Enable only necessary plugins to reduce overhead

## Troubleshooting

Common issues and solutions:

1. **Visualization not displaying** - Check if visualization is enabled
2. **Export failures** - Verify file permissions and disk space
3. **Performance issues** - Reduce update rates or disable real-time features
4. **Plugin loading errors** - Check plugin compatibility and dependencies

## Future Improvements

Planned enhancements:

- Advanced 3D visualization with WebGL/OpenGL support
- Machine learning-based feature detection
- Real-time streaming visualization
- Integration with monitoring dashboards
- Distributed visualization capabilities

## Support

For support, refer to the main NLM documentation and README files.

## Files List

### Header Files

- `src/visualization/BrainVisualizer.hpp`
- `src/plugins/PluginSystem.hpp`

### Source Files

- `src/plugins/PluginSystem.cpp`
- `src/brain/Brain.cpp` (added visualization methods)
- `python/bindings.cpp` (added visualization bindings)

### Examples

- `examples/visualization_example.py` - Python usage example
- `INSTALLATION_GUIDE.md` - Detailed installation and usage guide

## Build Instructions

To build the visualization tools:

```bash
# Configure with CMake
cmake -B build -S .

# Build the project
cmake --build build

# Install (optional)
cmake --install build
```

For more detailed build instructions, refer to the main README file.

## License

This implementation is part of the NLM (Neural Learning Machine) project.
Refer to the main LICENSE file for licensing information.
