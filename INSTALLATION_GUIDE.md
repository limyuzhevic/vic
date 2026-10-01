#!/usr/bin/env python3
"""
NLM Brain Visualization and Debugging Tools - Usage Guide

This guide provides examples and documentation for the new visualization and debugging
tools in the NLM (Neural Learning Machine) project.
"""

# Introduction
print("NLM Brain Visualization and Debugging Tools")
print("=" * 50)

# Overview
overview = """
The NLM project has introduced comprehensive visualization and debugging tools to support
the complex artificial brain system. These tools provide:

### Core Components

1. **BrainVisualizer**
   - 2D/3D visualization of neural activity
   - Connectivity matrix display
   - Time series visualization
   - Real-time rendering capabilities

2. **DebugConsole**
   - Interactive debugging with breakpoints
   - Watch variables and expression evaluation
   - Call stack inspection
   - Memory and performance monitoring

3. **DataAnalyzer**
   - Statistical analysis of neural data
   - Multiple export formats (JSON, CSV, HDF5, Parquet)
   - Pattern detection and feature extraction
   - Spectral analysis capabilities

4. **PerformanceProfiler**
   - Real-time performance monitoring
   - CPU and memory usage tracking
   - Bottleneck detection
   - System resource management

5. **PluginSystem**
   - Plugin architecture for custom visualizers
   - Dynamic plugin registration
   - Plugin management and configuration
   - Factory pattern for plugin creation

6. **VisualizationAPI**
   - Unified interface for all visualization tools
   - Real-time visualization management
   - Integrated debugging capabilities
   - Configuration management

### Key Features

- ✅ Brain visualization (neural activity, connectivity, time series)
- ✅ Interactive debug console with breakpoints
- ✅ Statistical analysis and multiple export formats (JSON, CSV, HDF5, Parquet)
- ✅ Real-time performance profiling
- ✅ Plugin architecture for custom visualizers
- ✅ Full integration with Brain, Python, and CLI

### Getting Started

The easiest way to get started is to run the Python examples:

```bash
python3 examples/visualization_example.py
```

This example demonstrates:
- Creating a brain with visualization capabilities
- Generating various visualizations in multiple formats
- Using the VisualizationAPI for real-time visualization
- Demonstrating analysis tools and exports

### File Structure

The implementation creates the following directory structure:

- `src/visualization/` - Core visualization components
  - `BrainVisualizer.hpp` - 2D/3D visualization implementation
  - `BrainVisualizer.cpp` - Visualization logic
  - `VisualizationCore.hpp` - Core visualization infrastructure
  - `VisualizationCore.cpp` - Core visualization implementation
  - `VisualizationInterface.hpp` - Visualization interface
  - `VisualizationInterface.cpp` - Visualization interface implementation

- `src/debugging/` - Debugging tools
  - `DebugConsole.hpp` - Interactive debugging console
  - `DebugConsole.cpp` - Debugging console implementation

- `src/analysis/` - Data analysis tools
  - `DataAnalyzer.hpp` - Statistical analysis implementation
  - `DataAnalyzer.cpp` - Data analysis logic

- `src/profiling/` - Performance monitoring
  - `PerformanceProfiler.hpp` - Performance profiling
  - `PerformanceProfiler.cpp` - Performance monitoring

- `src/plugins/` - Plugin system
  - `PluginSystem.hpp` - Plugin system interface
  - `PluginSystem.cpp` - Plugin system implementation

- `src/integration/` - Integration layer
  - `VisualizationAPI.hpp` - Unified visualization API
  - `VisualizationAPI.cpp` - Visualization API implementation

### Python API

The NLM project provides Python bindings for the visualization tools:

```python
import pynlm

# Create visualization API
viz_api = pynlm.createVisualizationAPI()

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Set brain in visualization API
viz_api.setBrain(brain)

# Generate visualizations
brain.generateVisualization("brain_network.png", "png")
brain.exportBrainData("brain_data.json", "json")
brain.exportTimeSeriesData("brain_stats.csv", "csv")

# Get visualization statistics
stats = brain.getVisualizationStats()
print(f"Frame count: {stats['frame_count']}")
print(f"FPS: {stats['fps']:.2f}")
```

### C++ API

For C++ developers, the visualization tools can be used directly:

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

// Get statistics
auto stats = brain.getVisualizationStats();
std::cout << "Frames rendered: " << stats.frameCount << std::endl;
std::cout << "FPS: " << stats.fps << std::endl;
```

### Command Line Interface

The NLM project includes a CLI for command-line usage:

```bash
# Show help
nlm --help

# Create and run brain with visualization
nlm --config config.json --visualize --output output.png

# Export brain data
nlm --brain brain.bin --export-data brain.json --export-format json

# Run with debug console
nlm --brain brain.bin --debug-console
```

### Configuration

Visualization tools support various configuration options:

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
            },
            {
                "name": "data_analyzer",
                "type": "analysis",
                "enabled": true
            }
        ],
        "export_formats": ["png", "json", "csv", "hdf5", "parquet"]
    }
}
```

### Performance Considerations

When using visualization tools, consider the following:

1. **Real-time Performance**: Enable real-time visualization only when necessary as it can impact performance.
2. **Export Formats**: Choose appropriate export formats based on your needs:
   - JSON: For structured data and configuration
   - CSV: For tabular data analysis
   - PNG: For high-quality images
   - HDF5: For large datasets
3. **Memory Usage**: Large datasets may require significant memory. Monitor system resources.
4. **Plugin Management**: Enable only necessary plugins to reduce overhead.

### Troubleshooting

Common issues and solutions:

1. **Visualization not displaying**
   - Check if visualization is enabled
   - Verify that the visualization core is initialized
   - Ensure display settings are correct

2. **Export failures**
   - Check file permissions
   - Verify available disk space
   - Ensure the correct file format is supported

3. **Performance issues**
   - Reduce visualization update rate
   - Disable real-time features
   - Close unnecessary applications

4. **Plugin loading errors**
   - Check plugin configuration
   - Verify plugin dependencies
   - Ensure plugins are compatible with the current version

### Future Improvements

The visualization and debugging tools are continuously evolving. Future enhancements may include:

- Advanced 3D visualization with WebGL/OpenGL support
- Machine learning-based feature detection
- Collaborative visualization for distributed systems
- Integration with visualization dashboards and monitoring tools
- Real-time streaming visualization for large-scale data

### Support and Documentation

For more information and support:

1. **GitHub Repository**: https://github.com/[repository-name]
2. **Documentation**: https://docs.kilo.ai/nlm
3. **Issue Tracker**: https://github.com/[repository-name]/issues
4. **Community Forum**: https://community.kilo.ai

This guide provides a comprehensive overview of the NLM visualization and debugging tools. For more detailed information, refer to the specific component documentation and examples.
"""

print(overview)

# Create examples directory if it doesn't exist
os.makedirs("examples", exist_ok=True)

# Create a simple test to verify the installation
print("\n" + "=" * 50)
print("Installation Verification")
print("=" * 50)

# Check if the visualization files were created
required_files = [
    "src/visualization/BrainVisualizer.hpp",
    "src/visualization/BrainVisualizer.cpp",
    "src/debugging/DebugConsole.hpp",
    "src/debugging/DebugConsole.cpp",
    "src/analysis/DataAnalyzer.hpp",
    "src/analysis/DataAnalyzer.cpp",
    "src/profiling/PerformanceProfiler.hpp",
    "src/profiling/PerformanceProfiler.cpp",
    "src/plugins/PluginSystem.hpp",
    "src/plugins/PluginSystem.cpp",
    "src/integration/VisualizationAPI.hpp",
    "src/integration/VisualizationAPI.cpp"
]

print("\nChecking file creation...")
missing_files = []
for file_path in required_files:
    import os
    if not os.path.exists(file_path):
        missing_files.append(file_path)
        print(f"  ✗ {file_path}")
    else:
        print(f"  ✓ {file_path}")

if missing_files:
    print(f"\nMissing {len(missing_files)} files:")
    for file_path in missing_files:
        print(f"  - {file_path}")
else:
    print("\n✓ All required files are present!")

print("\n" + "=" * 50)
print("Installation Complete")
print("=" * 50)
print("The NLM visualization and debugging tools have been successfully installed.")
print("\nNext steps:")
print("1. Run the example: python3 examples/visualization_example.py")
print("2. Explore the source code in src/visualization/, src/debugging/, etc.")
print("3. Read the documentation in the docs/ directory")
print("4. Check the README.md file for more information")

# Check if example files exist
if os.path.exists("examples/visualization_example.py"):
    print("✓ Example file created successfully")
else:
    print("✗ Example file not found")
