# NLM Neural Network Visualization System

## Overview

The NLM (Neural Learning Machine) Visualization System provides comprehensive neural network visualization tools for the NLM project. These tools work in terminal/ASCII format without external dependencies and integrate seamlessly with the existing Brain system.

## Features

### 1. Neural Network Visualization
- **Brain Structure Visualization** - Show neurons, regions, populations
- **Connectivity Visualization** - Display synaptic connections
- **Activity Visualization** - Show neural firing patterns
- **Real-time Updating** - Live visualization during simulation
- **Zoom and Pan** - Navigate large networks
- **Color Coding** - Different neuron types, activities

### 2. Activity Visualization
- **Firing Rate Display** - Show neuron firing rates
- **Spike Train Visualization** - Individual neuron spike trains
- **Population Activity** - Group-level activity patterns
- **Time Series Analysis** - Show activity over time
- **Statistical Overlays** - Mean, variance, skewness

### 3. Memory System Visualization
- **Working Memory Display** - Show stored associations
- **Episodic Memory** - Timeline of stored episodes
- **Semantic Memory** - Network of facts and associations
- **Procedural Memory** - Skill hierarchy visualization
- **Associative Memory** - Connection strength display

### 4. Progress and Experiment Visualization
- **Experiment Progress** - Show current experiment state
- **Learning Curves** - Track learning over time
- **Performance Metrics** - Show efficiency measures
- **Resource Usage** - Memory, CPU, I/O monitoring
- **Comparison Charts** - Compare different runs/configurations

### 5. Development and Analysis Visualization
- **Brain Inspection** - Complete system analysis
- **Configuration Impact** - Show how settings affect behavior
- **Neural Dynamics** - Show emergent patterns
- **System Health** - Monitor for issues

## Technical Architecture

### Core Components

1. **VisualizationEngine.hpp/cpp** - Main visualization controller
2. **BrainStructureRenderer.hpp/cpp** - Neural network structure display
3. **ActivityRenderer.hpp/cpp** - Neural activity visualization
4. **MemoryRenderer.hpp/cpp** - Memory system visualization
5. **ProgressRenderer.hpp/cpp** - Experiment progress display
6. **TerminalRenderer.hpp/cpp** - ASCII/terminal rendering engine
7. **ColorManager.hpp/cpp** - Color management for terminal
8. **LayoutManager.hpp/cpp** - Screen layout and organization
9. **InputHandler.hpp/cpp** - User interaction handling
10. **ExportRenderer.hpp/cpp** - Export to various formats

### Integration Points

- **Brain System** - Real-time data from Brain class
- **Memory Systems** - Integration with all memory types
- **Experiment Framework** - Phase 6 experiment integration
- **CLI System** - Command-line visualization controls
- **Performance System** - Integration with profiling data

## Display Modes

- **Structural Mode** - Show brain anatomy and connectivity
- **Activity Mode** - Show neural dynamics and patterns
- **Memory Mode** - Show memory system state
- **Progress Mode** - Show experiment progress
- **Debug Mode** - Show internal system states

## Interaction Commands

### Keyboard Shortcuts

- **1-5** - Change display mode
- **M** - Toggle memory view
- **L** - Toggle activity histogram
- **T** - Toggle statistics overlay
- **S** - Toggle spikes visualization
- **C** - Change color scheme
- **~** - Show command help
- **Q** - Quit visualization

### Mouse/Screen Controls

- **Mouse Click** - Select nodes/connections
- **Scroll Wheel** - Zoom in/out
- **Drag** - Pan view
- **Right Click** - Show context menu

### Command-Line Options

- **--display-mode <mode>** - Set display mode
- **--update-rate <fps>** - Set update rate
- **--screen-size <width>x<height>** - Set screen size
- **--color-scheme <scheme>** - Set color scheme
- **--export <filename> [<format>]** - Export data

## Visualization Types

### 1. 2D Network Diagram
- Force-directed graph layout
- Configurable spacing and clustering
- Interactive node/connection selection

### 2. Radial Layout
- Center-out organization
- Hierarchical region/population/neuron levels
- Smooth radial animations

### 3. Hierarchical Tree
- Region/population/neuron levels
- Expandable/collapsible nodes
- Multiple tree layout options

### 4. Activity Heatmap
- Firing intensity visualization
- Heatmap color schemes
- Time-based heatmaps

### 5. Timeline View
- Temporal activity patterns
- Event timeline visualization
- Synchronization markers

### 6. Statistical Cross-Section
- Statistical overlays
- Mean/variance/skewness display
- Correlation heatmaps

## Technical Implementation Details

### Terminal Rendering Engine
- Uses ANSI escape codes for color
- Supports 16-color and 256-color terminals
- Efficient buffer-based rendering
- Real-time screen updates

### Color Management
- Comprehensive color system
- Multiple color schemes (default, heatmap, monochromat, etc.)
- Accessibility support (colorblind-friendly schemes)
- Dynamic color adaptation based on data

### Layout Management
- Multi-panel layouts
- Docking system for panels
- Resizable and movable widgets
- Responsive design for different screen sizes

### Input Handling
- Keyboard input with command completion
- Mouse and touch support
- Gesture recognition
- Session history and auto-complete

### Export Functionality
- Multiple export formats (ASCII, CSV, JSON, HTML, XML)
- Session data export
- Progress data export
- Configuration export

### Performance Optimization
- Efficient rendering algorithms
- Frame rate limiting
- Memory management
- Resource monitoring

## Building and Running

### Prerequisites
- C++17 compatible compiler
- CMake 3.10 or higher
- Make

### Building
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running
```bash
./nlm_visualization
```

### Example Usage
```bash
# Run visualization with default settings
./nlm_visualization

# Run with specific configuration
./nlm_visualization --display-mode activity --update-rate 30

# Export data to CSV
./nlm_visualization --export experiment_data.csv csv
```

## API Documentation

### Core Classes

#### VisualizationInterface
- **Purpose**: High-level interface to the visualization system
- **Methods**:
  - `initialize(std::shared_ptr<Brain> brain)` - Initialize with brain reference
  - `update()` - Update visualization data
  - `render()` - Render current state
  - `setDisplayMode(int mode)` - Set display mode
  - `exportData(filename, format)` - Export data

#### VisualizationEngine
- **Purpose**: Main visualization controller
- **Methods**:
  - `setBrain(std::shared_ptr<Brain> brain)` - Set brain reference
  - `update()` - Update visualization
  - `render()` - Render visualization
  - `setDisplayMode(DisplayMode mode)` - Set display mode
  - `exportData(ExportOptions options)` - Export data

#### BrainStructureRenderer
- **Purpose**: Neural network structure visualization
- **Methods**:
  - `render(const Brain& brain)` - Render brain structure
  - `setLayoutMode(LayoutMode mode)` - Set layout mode
  - `setConnectionStyle(ConnectionStyle style)` - Set connection style
  - `setShowNeuronLabels(bool show)` - Toggle neuron labels

#### ActivityRenderer
- **Purpose**: Neural activity visualization
- **Methods**:
  - `render(const Brain& brain)` - Render activity data
  - `setDisplayMode(DisplayMode mode)` - Set display mode
  - `setShowSpikes(bool show)` - Toggle spike visualization
  - `setShowHistograms(bool show)` - Toggle histogram display

#### MemoryRenderer
- **Purpose**: Memory system visualization
- **Methods**:
  - `render(const Brain& brain)` - Render memory data
  - `setMemoryViewMode(MemoryViewMode mode)` - Set memory view mode
  - `setShowMemoryLabels(bool show)` - Toggle memory labels
  - `setShowStrengthIndicators(bool show)` - Toggle strength indicators

#### ProgressRenderer
- **Purpose**: Experiment progress visualization
- **Methods**:
  - `render(const Brain& brain)` - Render progress data
  - `setProgressViewMode(ProgressViewMode mode)` - Set progress view mode
  - `setShowProgressHistory(bool show)` - Toggle progress history
  - `setShowResourceMetrics(bool show)` - Toggle resource metrics

#### ExportRenderer
- **Purpose**: Data export functionality
- **Methods**:
  - `exportData(ExportOptions options)` - Export data
  - `setExportFormat(ExportFormat format, bool enable)` - Enable/disable format
  - `setOutputDirectory(const std::string& directory)` - Set output directory
  - `setCompressionLevel(int level)` - Set compression level

## Integration with AgentBrain

The visualization system integrates with AgentBrain through:

### Enhanced AgentBrain Features

#### Visualization Methods
- `initializeVisualization()` - Initialize visualization system
- `updateVisualization()` - Update visualization data
- `renderVisualization()` - Render visualization
- `closeVisualization()` - Close visualization
- `exportVisualizationData(filename, format)` - Export visualization data

#### Configuration Methods
- `enableVisualization(bool enable)` - Enable/disable visualization
- `setVisualizationDisplayMode(int mode)` - Set display mode
- `loadVisualizationConfiguration(filename)` - Load configuration
- `saveVisualizationConfiguration()` - Save configuration

#### Performance Monitoring
- `enablePerformanceMonitoring(bool enable)` - Enable performance monitoring
- `updatePerformanceMetrics()` - Update performance metrics
- `getPerformanceSummary()` - Get performance summary

## Advanced Usage

### Custom Color Schemes
```cpp
ColorManager::setColorScheme("heatmap");
VisualizationEngine::setColorScheme("retro");
```

### Custom Layouts
```cpp
LayoutManager::setPadding(10, 5, 10, 5);
LayoutManager::setSpacing(2, 1);
LayoutManager::enableAutoLayout(true);
```

### Custom Input Handlers
```cpp
inputHandler->registerCommand("~custom", [](const std::vector<std::string>& args) {
    // Custom command handler
    return true;
});
```

### Data Export
```cpp
ExportRenderer::ExportOptions options;
options.format = ExportRenderer::ExportFormat::JSON;
options.outputPath = "experiment_data.json";
options.title = "Experiment Data Export";
options.description = "Neural network experiment data";

exportRenderer->exportData(options);
```

## Configuration Files

### Example Configuration
```json
{
    "visualization": {
        "displayMode": "consolidated",
        "renderingMode": "terminal",
        "screenWidth": 80,
        "screenHeight": 40,
        "updateInterval": 0.1,
        "autoRefresh": true,
        "showFPS": true,
        "colorScheme": "default",
        "theme": "default",
        "enableAnimations": true,
        "animationSpeed": 1.0,
        "customSettings": {
            "neuronRadius": 2,
            "connectionWidth": 1,
            "labelFontSize": 1
        }
    }
}
```

### Export Configuration
```json
{
    "export": {
        "formats": ["ascii", "csv", "json", "html"],
        "outputDirectory": "./exports",
        "compressionLevel": 6,
        "includeMetadata": true,
        "defaultTitle": "NLM Visualization Export",
        "defaultDescription": "Neural network visualization data"
    }
}
```

## Error Handling

### Common Errors

#### Visualization Errors
- **Invalid Brain Pointer**: "Error: Invalid brain pointer provided to visualization interface"
- **Initialization Failure**: "Failed to initialize visualization interface"
- **Rendering Failure**: "Error rendering visualization"

#### Export Errors
- **Unsupported Format**: "Export format not supported"
- **File Write Error**: "Failed to write export file"
- **Validation Error**: "Invalid export options"

#### Performance Errors
- **FPS Limit**: "Performance warning: FPS below target"
- **Memory Error**: "Insufficient memory for visualization"
- **Resource Error**: "System resources insufficient"

### Error Reporting

The visualization system provides detailed error messages through:

- **std::cout**: Console output for runtime errors
- **lastError()**: Get last error message from components
- **debugMode()**: Enable debug output for detailed information

## Performance Considerations

### Memory Usage
- **Terminal Rendering**: Efficient buffer-based rendering minimizes memory usage
- **Data Management**: Lazy loading and streaming for large datasets
- **Cleanup**: Proper resource cleanup prevents memory leaks

### CPU Usage
- **Frame Rate Limiting**: Configurable frame rate limiting
- **Rendering Optimization**: Efficient rendering algorithms
- **Input Processing**: Optimized input handling

### Network Usage
- **Export**: Efficient data compression for file exports
- **Streaming**: Support for streaming large datasets
- **Progress Tracking**: Progress reporting for long operations

## Future Enhancements

### Planned Features

1. **3D Visualization**
   - 3D network visualization
   - Stereo support (with 3D glasses)
   - Camera controls for 3D viewing

2. **Advanced Interaction**
   - Touch screen support
   - Gesture recognition
   - Voice commands

3. **Real-time Collaboration**
   - Multi-user visualization
   - Shared visualization sessions
   - Collaborative editing

4. **Advanced Analytics**
   - Machine learning integration
   - Pattern recognition
   - Predictive analysis

5. **Extended Export**
   - Support for more formats (PDF, PNG, SVG)
   - Database export
   - Web service integration

## License

This visualization system is part of the NLM project. Please see the LICENSE file for details.

## Contact

For questions, issues, or feature requests, please visit the NLM repository or contact the development team.

---

This document provides a comprehensive overview of the NLM Neural Network Visualization System, including its features, technical implementation, and usage instructions. The system provides a complete visualization solution for neural networks with support for real-time updates, interactive controls, and comprehensive export capabilities.