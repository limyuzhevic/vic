# Visualization and Monitoring Tools Reference Manual
# NLM Visualization System - Technical Reference

## Overview

This reference manual provides comprehensive technical information about the NLM Visualization and Monitoring Tools system, including API documentation, configuration options, and technical specifications.

## System Architecture

### Core Components

1. **Visualization Framework**
   - `VisualizationFramework.hpp/cpp`: Core visualization system
   - `ConsoleVisualizer.hpp/cpp`: Console-based visualization
   - `GUIVisualizer.hpp/cpp`: GUI-based visualization
   - `WebVisualizer.hpp/cpp`: Web-based visualization
   - `VisualizationInterface.hpp/cpp`: Abstract base class for all visualizers

2. **Visualization Tools**
   - Base class: `VisualizationTool.hpp/cpp`
   - Implementation: Multiple specific visualization tools

3. **Monitoring Tools**
   - Base class: `MonitoringTool.hpp/cpp`
   - Implementation: System, brain, environment, and neuromodulation monitoring

4. **Data Processing**
   - JSON parsing and serialization
   - File I/O operations
   - Data transformation utilities

### Architecture Patterns

- **Object-Oriented Design**: Inheritance-based architecture for extensibility
- **Template Methods**: Common workflows defined in base classes
- **Strategy Pattern**: Backend selection (Console, GUI, Web)
- **Observer Pattern**: Real-time data updates and notifications

## API Documentation

### Visualization Framework API

#### Class: VisualizationFramework

**Header**: `src/visualization/VisualizationFramework.hpp`

**Methods**:

| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `initialize()` | Initialize visualization framework | None | bool |
| `initialize(const VisualizationConfig&)` | Initialize with configuration | `VisualizationConfig` | bool |
| `update()` | Update visualization data | None | void |
| `render()` | Render current state | None | void |
| `close()` | Close visualization | None | void |
| `addVisualizer()` | Add a visualizer | `std::unique_ptr<VisualizationInterface>` | bool |
| `removeVisualizer()` | Remove a visualizer | `const std::string&` | bool |
| `getVisualizer()` | Get a visualizer | `const std::string&` | VisualizationInterface* |
| `createVisualization()` | Create a visualization | `VisualizationConfig` | bool |
| `updateVisualization()` | Update a visualization | `const std::string&, VisualizationConfig` | bool |
| `destroyVisualization()` | Destroy a visualization | `const std::string&` | bool |
| `renderVisualization()` | Render a specific visualization | `const std::string&` | bool |
| `renderAllVisualizations()` | Render all visualizations | None | bool |
| `exportData()` | Export data | `const std::string&, const std::string&, const std::string&` | bool |
| `importData()` | Import data | `const std::string&, const std::string&` | bool |
| `setTheme()` | Set visualization theme | `const std::string&` | void |
| `setStyle()` | Set visualization style | `const std::string&` | void |
| `getAvailableThemes()` | Get available themes | None | std::vector<std::string> |
| `getAvailableStyles()` | Get available styles | None | std::vector<std::string> |
| `getActiveVisualizations()` | Get active visualization names | None | std::vector<std::string> |
| `isRunning()` | Check if framework is running | None | bool |
| `setUpdateRate()` | Set update rate | `double` | void |
| `getUpdateRate()` | Get update rate | None | double |
| `getMemoryUsage()` | Get memory usage | None | double |
| `getFrameTime()` | Get frame time | None | double |
| `getActiveVisualizationsCount()` | Get count of active visualizations | None | int |

#### Struct: VisualizationConfig

```cpp
struct VisualizationConfig {
    std::string name;                    // Visualization name
    VisualizationType type;              // Visualization type
    VisualizationBackend backend;        // Backend type
    std::string outputFormat;            // Output format
    std::string outputPath;              // Output file path
    double updateRate;                   // Update rate (Hz)
    std::map<std::string, std::string> parameters; // Additional parameters
    bool realTime;                       // Real-time flag
    bool interactive;                    // Interactive flag
    int width;                           // Width
    int height;                          // Height
};
```

#### Enum: VisualizationBackend

```cpp
enum class VisualizationBackend {
    CONSOLE,  // Console-based visualization
    GUI,      // GUI-based visualization
    WEB       // Web-based visualization
};
```

#### Enum: VisualizationType

```cpp
enum class VisualizationType {
    NEURON,                    // Individual neuron properties
    NETWORK,                   // Network connectivity
    DYNAMICS,                  // Neural dynamics
    SYNAPSES,                  // Synaptic connections
    NETWORK_DYNAMICS,          // Network-level dynamics
    ACTIVITY_PATTERNS,         // Activity patterns over time
    SPACE_TIME,                // Space-time activity patterns
    BRAIN_STRUCTURE,           // Brain structure and regions
    BRAIN_CONNECTIVITY,        // Brain connectivity matrix
    BRAIN_ACTIVITY,            // Brain-wide activity patterns
    BRAIN_DEVELOPMENT,          // Developmental changes
    BRAIN_MEMORY,              // Memory system activity
    EXPERIMENT_FLOW,           // Experiment execution flow
    EXPERIMENT_RESULTS,         // Experiment results
    EXPERIMENT_COMPARISON,      // Compare experiment results
    EXPERIMENT_TIMELINE,       // Experiment timeline
    EXPERIMENT_METRICS         // Experiment metrics
};
```

### Visualization Tool API

#### Class: VisualizationTool (Base)

**Header**: `tools/viz/VisualizationTool.hpp`

**Methods**:

| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `visualizeSimulation()` | Visualize simulation | `const std::string&` | bool |
| `visualizeResults()` | Visualize results | `const std::string&` | bool |
| `visualizeBrain()` | Visualize brain | `const std::string&` | bool |
| `visualizeMetrics()` | Visualize metrics | `const std::string&` | bool |
| `visualizeComparison()` | Visualize comparison | `const std::string&, const std::string&` | bool |
| `renderWithFramework()` | Render with framework | `const std::string&, const std::string&` | bool |
| `createVisualization()` | Create visualization | `const std::string&, const std::string&, const std::string&, const std::map<std::string, std::string>&` | bool |

### Monitoring Tool API

#### Class: MonitoringTool (Base)

**Header**: `tools/monitor/MonitoringTool.hpp`

**Methods**:

| Method | Description | Parameters | Return Type |
|--------|-------------|------------|-------------|
| `monitorSystem()` | Monitor system | `const std::string&, double` | bool |
| `monitorBrain()` | Monitor brain | `const std::string&, const std::string&` | bool |
| `monitorPerformance()` | Monitor performance | `const std::string&, double` | bool |
| `monitorPlasticity()` | Monitor plasticity | `const std::string&, const std::string&` | bool |
| `monitorMemory()` | Monitor memory | `const std::string&, const std::string&` | bool |
| `monitorDevelopment()` | Monitor development | `const std::string&, const std::string&` | bool |
| `monitorNeuromodulation()` | Monitor neuromodulation | `const std::string&, const std::string&` | bool |

## Configuration Options

### Command-line Arguments

The visualization and monitoring tools support various command-line arguments:

#### Common Arguments

- `--help`: Display help information
- `--version`: Display version information
- `--verbose`: Enable verbose output
- `--quiet`: Suppress output

#### Visualization Tool Arguments

- `--brain <file>`: Brain state file
- `--simulation <file>`: Simulation file
- `--output <file>`: Output file path
- `--format <format>`: Output format (png, pdf, svg, html)
- `--type <type>`: Visualization type
- `--backend <backend>`: Visualization backend (console, gui, web)
- `--config <file>`: Configuration file

#### Monitoring Tool Arguments

- `--interval <float>`: Monitoring interval in seconds
- `--duration <float>`: Monitoring duration in seconds
- `--metrics <type>`: Metric type
- `--format <format>`: Output format
- `--host <host>`: Host address for web monitoring
- `--port <port>`: Port number for web monitoring

### Configuration File Format

Configuration files are in JSON format:

```json
{
    "visualization": {
        "backend": "gui",
        "theme": "dark",
        "style": "detailed",
        "output_format": "png",
        "output_path": "./output/"
    },
    "monitoring": {
        "interval": 1.0,
        "duration": -1.0,
        "metrics": ["firing_rate", "connectivity", "plasticity"]
    },
    "data": {
        "input_format": "json",
        "output_format": "csv",
        "compression": true
    }
}
```

## Data Format Specifications

### Input Data Formats

1. **Brain State Files**: JSON format with neuron, synapse, and region data
2. **Simulation Files**: JSON format with time-series data
3. **Experiment Files**: JSON format with experiment results and metrics

### Output Data Formats

1. **Visualization Outputs**: PNG, PDF, SVG, HTML
2. **Monitoring Outputs**: JSON, CSV, TXT
3. **Data Export**: JSON, CSV, HDF5

### Data Structure Examples

#### Brain State JSON Structure

```json
{
    "neurons": [
        {
            "id": 0,
            "type": "excitatory",
            "membrane_potential": -70.0,
            "threshold_potential": -55.0,
            "firing_rate": 25.0,
            "connections": [1, 2, 3]
        }
    ],
    "regions": [
        {
            "id": 0,
            "name": "Cortex",
            "neuron_count": 100,
            "region_type": "cortex"
        }
    ],
    "synapses": [
        {
            "pre_synaptic": 0,
            "post_synaptic": 1,
            "weight": 0.5,
            "type": "excitatory"
        }
    ]
}
```

#### Simulation JSON Structure

```json
{
    "time_series": [
        {
            "time": 0.0,
            "membrane_potential": -70.0,
            "firing_rate": 25.0,
            "population_activity": 0.3
        }
    ],
    "metadata": {
        "simulation_time": 1000.0,
        "timestep": 0.001,
        "neuron_count": 500
    }
}
```

## Performance Specifications

### Memory Requirements

- **Console Visualization**: 1-10 MB
- **GUI Visualization**: 10-100 MB
- **Web Visualization**: 50-500 MB
- **Multiple Visualizations**: 100-1000 MB

### Processing Performance

- **Real-time Visualization**: 30-60 FPS
- **Data Export**: 100-1000 MB/s
- **Data Import**: 50-500 MB/s
- **Processing Time**: Varies by data size and complexity

### System Requirements

#### Minimum Requirements

- **Operating System**: Windows 10, Linux, macOS
- **CPU**: Dual-core processor (4 cores recommended)
- **Memory**: 4 GB RAM (8 GB recommended)
- **Disk Space**: 100 MB for installation, 1 GB for data

#### Recommended Requirements

- **Operating System**: Linux, macOS (Windows with WSL)
- **CPU**: Quad-core processor or better
- **Memory**: 16 GB RAM or more
- **Disk Space**: 500 MB free space
- **Display**: 1920x1080 resolution or higher
- **Additional**: GUI libraries for interactive visualization

## Installation and Setup

### Building from Source

1. **Clone the Repository**
   ```bash
   git clone https://github.com/Kilo-Org/kilocode.git
   cd kilocode
   ```

2. **Create Build Directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure CMake**
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=20 ..
   ```

4. **Build**
   ```bash
   make -j$(nproc)
   ```

### Installing Visualization Tools

1. **Install System Dependencies**
   ```bash
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install -y build-essential cmake git libjsoncpp-dev
   
   # macOS
   xcode-select --install
   brew install cmake jsoncpp
   ```

2. **Build NLM with Visualization Support**
   ```bash
   cd /path/to/kilocode
   mkdir build
   cd build
   cmake -DBUILD_VIZ_TOOLS=ON ..
   make -j$(nproc)
   ```

3. **Install to System**
   ```bash
   sudo make install
   ```

### Running Visualization Tools

1. **List Available Tools**
   ```bash
   ./nlm-cli --help
   ```

2. **Get Tool Information**
   ```bash
   ./nlm-cli viz-neuron --help
   ./nlm-cli viz-network --help
   ./nlm-cli viz-dynamics --help
   ```

3. **Run Basic Visualization**
   ```bash
   # Create sample data
   ./nlm-cli brain-info --create-sample sample_brain.json
   
   # Visualize brain structure
   ./nlm-cli viz-brain-structure --brain sample_brain.json --output brain_structure.png
   ```

## Usage Examples

### Example 1: Neuron Visualization

```cpp
// C++ example for neuron visualization
#include "tools/viz/viz-neuron.hpp"

int main() {
    // Create visualization tool
    VizNeuronTool tool;
    
    // Configure visualization
    VisualizationConfig config;
    config.name = "neuron_viz";
    config.type = VisualizationType::NEURON;
    config.backend = VisualizationBackend::CONSOLE;
    config.outputFormat = "png";
    config.outputPath = "neuron_viz.png";
    config.parameters["brain_file"] = "brain_state.json";
    config.parameters["neuron_id"] = "42";
    
    // Create visualization
    bool success = tool.createVisualization(config);
    
    if (success) {
        std::cout << "Neuron visualization created successfully!" << std::endl;
    } else {
        std::cout << "Failed to create neuron visualization!" << std::endl;
    }
    
    return 0;
}
```

### Example 2: Real-time Monitoring

```cpp
// C++ example for real-time monitoring
#include "tools/monitor/MonitoringTool.hpp"

int main() {
    // Create monitoring tool
    MonitorSystemTool monitor;
    
    // Configure monitoring
    std::string outputFile = "system_monitor.log";
    double interval = 1.0; // 1 second
    
    // Start monitoring
    bool success = monitor.monitorSystem(outputFile, interval);
    
    if (success) {
        std::cout << "System monitoring started successfully!" << std::endl;
    } else {
        std::cout << "Failed to start system monitoring!" << std::endl;
    }
    
    return 0;
}
```

## Troubleshooting

### Common Issues

1. **Tool Not Found**
   - Ensure the tool is built and in your PATH
   - Check if the executable has the correct name

2. **Missing Dependencies**
   - Install required system libraries
   - Check CMake configuration

3. **Permission Denied**
   - Check file permissions
   - Use sudo if necessary

4. **Memory Issues**
   - Monitor memory usage
   - Close other applications
   - Use sparse data structures

5. **Performance Issues**
   - Optimize data processing
   - Use appropriate visualization parameters
   - Consider hardware acceleration

### Error Messages

| Error Message | Cause | Solution |
|---------------|-------|----------|
| "Tool not found" | Executable not found | Check PATH, rebuild tools |
| "File not found" | Input file doesn't exist | Check file path |
| "Permission denied" | Insufficient permissions | Check file permissions |
| "Memory allocation failed" | Out of memory | Close other applications |
| "Invalid parameter" | Invalid argument | Check command-line arguments |

## Migration Guide

### From Previous Versions

1. **Version 1.x to 2.x**
   - New visualization framework architecture
   - Additional visualization backends
   - Enhanced API documentation

2. **Configuration Changes**
   - New configuration file format
   - Updated command-line arguments
   - Additional visualization options

### Migration Steps

1. **Backup Previous Configuration**
   ```bash
   cp -r ~/.nlm ~/.nlm_backup
   ```

2. **Update Configuration**
   ```bash
   # Update configuration file
   # Check documentation for new options
   ```

3. **Rebuild Tools**
   ```bash
   cd build
   make -j$(nproc)
   ```

4. **Test Visualization**
   ```bash
   ./nlm-cli --help
   ```

## Future Enhancements

### Planned Features

1. **3D Visualization**: Enhanced 3D visualization capabilities
2. **Machine Learning**: AI-powered visualization suggestions
3. **Cloud Integration**: Cloud-based visualization services
4. **Virtual Reality**: VR-based neural visualization
5. **Real-time Collaboration**: Collaborative visualization sessions

### Research Directions

1. **Advanced Analytics**: Statistical analysis and pattern recognition
2. **Multi-modal Visualization**: Integration with multiple data types
3. **Interactive Analysis**: Interactive exploration and analysis tools
4. **Scalable Visualization**: Support for large-scale datasets
5. **Domain-Specific Tools**: Specialized visualization for specific domains

---

This reference manual provides comprehensive technical information about the NLM Visualization and Monitoring Tools system. For the most up-to-date information and additional details, always refer to the latest version of this document.

For technical support and questions, please contact the NLM development team at support@nlm.org or visit the GitHub repository.