# NLM Visualization System

## Overview

This document describes the text-based visualization system implemented in the NLM (Neural Learning Machine) project. The system provides comprehensive visualization capabilities without requiring complex graphics dependencies, making it suitable for headless environments and development scenarios.

## Implementation Summary

The visualization system replaces the TODO PHASE 2 placeholders in `VisualizationInterface.cpp` with a complete, functional implementation featuring:

### Core Components

1. **Text-Based Network Visualization**
   - ASCII/Unicode network graphs showing neurons and connections
   - Real-time brain network topology with regions and populations
   - Interactive console-based visualization with navigable menus
   - Export to Graphviz DOT format for external visualization

2. **Activity Visualization**
   - Neural activity heatmaps in text format
   - Spike raster plots with time evolution
   - Membrane potential traces and firing rate displays
   - Network connectivity strength visualizations

3. **Data Visualization**
   - Statistical analysis displays (histograms, distributions)
   - Performance metrics dashboards
   - Comparative analysis tools
   - Memory system state visualizations

4. **Export and Analysis Tools**
   - CSV/JSON export for external analysis tools
   - Network statistics and metrics computation
   - Feature extraction for pattern analysis
   - Visualization configuration and customization

5. **Practical Features**
   - Real-time console updates without GUI dependencies
   - File output for offline analysis
   - Interactive command-line interface
   - Multi-format export (text, CSV, JSON, Graphviz)
   - Performance monitoring displays

### Key Classes

- **NetworkGraph**: For creating and rendering neural networks
- **ActivityPlotter**: For time-series and raster plots
- **HeatmapRenderer**: For activity heatmaps
- **DataExporter**: For exporting to various formats
- **ConsoleVisualizer**: Main interface class

## Usage Examples

### Basic Usage

```cpp
#include "visualization/VisualizationInterface.hpp"
#include "brain/Brain.hpp"

int main() {
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Initialize visualization
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Visualize network
    viz.visualizeNetwork(*brain);
    
    // Visualize activity
    viz.visualizeActivity(*brain);
    
    // Visualize spikes
    std::vector<nlm::SpikeEvent> spikes;
    // ... add spikes ...
    viz.visualizeSpikes(spikes);
    
    // Update visualization
    viz.update();
    
    // Close visualization
    viz.close();
    
    return 0;
}
```

### File Output

```cpp
int main() {
    // Create visualization with file output
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Set output file for persistent records
    viz.setOutputFile("visualization_output.txt");
    
    // All subsequent visualizations will be written to file
    viz.visualizeNetwork(brain);
    viz.visualizeActivity(brain);
    
    // Close and save
    viz.close();
    
    return 0;
}
```

### Data Export

```cpp
int main() {
    nlm::VisualizationInterface viz;
    viz.initialize();
    
    // Export data in different formats
    viz.exportData("network_data", "{\"neurons\": [...], \"connections\": [...]}", "json");
    viz.exportData("spike_data", "spike_timestamps", "csv");
    viz.exportData("graph", "*", "dot"); // Graphviz DOT format
    
    viz.close();
    return 0;
}
```

## Features

### Network Visualization

**ASCII/Unicode Graph Rendering**
- Neuronal representation with type symbols (E, I, S, M)
- Dynamic firing rate indicators (🔥⚡✦○)
- Regional labeling and organization
- Connection strength visualization with varying line styles

**Layout Algorithms**
- Circular layout for symmetric networks
- Radial layout for region-based organization
- Hierarchical layout for multi-level structures

### Activity Visualization

**Spike Raster Plots**
- Time-series visualization of spike events
- Neuron-specific activity tracking
- Interactive time window selection

**Membrane Potential Traces**
- Real-time potential monitoring
- Refractory period visualization
- Threshold crossing detection

**Firing Rate Histograms**
- Statistical analysis of firing patterns
- Rate distribution visualization
- Performance metrics display

### Heatmap Visualization

**Text-Based Heatmaps**
- Character-based intensity representation
- Multi-scale color coding
- Adjustable resolution for different data sizes

### Export Capabilities

**Format Support**
- **CSV**: For spreadsheet analysis
- **JSON**: For programmatic access
- **Graphviz DOT**: For advanced graph visualization
- **Text**: For human-readable reports

**Data Types**
- Network topology and connectivity
- Neural activity patterns
- Statistical metrics
- Performance measurements

### Console Interface

**Real-Time Updates**
- Configurable update rates
- Frame-based rendering
- Performance monitoring

**File Output**
- Persistent visualization records
- Session logging
- Export for offline analysis

## Configuration

### Update Rate Control
```cpp
viz.setUpdateRate(30.0); // 30 Hz update rate
```

### Output Configuration
```cpp
viz.setOutputFile("output.txt"); // File output
viz.setRealTime(false); // Batch mode
```

## Integration with Brain System

The visualization system integrates seamlessly with the existing NLM brain system:

- **Brain Data Access**: Uses existing Brain API methods
- **Event Handling**: Processes SpikeEvent objects directly
- **Region Information**: Extracts regional connectivity data
- **Performance Metrics**: Calculates and displays statistical data

## Testing

The visualization system is tested with the following test suite:

```bash
// Run visualization tests
./test_main --test_visualization

// Or run all tests
./test_main
```

Test categories include:
- Visualization system creation
- Initialization and configuration
- Network visualization
- Activity plotting
- Spike raster visualization
- Update and lifecycle management

## Performance Considerations

The text-based visualization system is designed for:

- **Low Resource Usage**: Minimal memory footprint
- **Headless Compatibility**: No GUI dependencies
- **Scalability**: Efficient handling of large networks
- **Flexibility**: Multiple output formats

## Future Enhancements

Potential future improvements include:

1. **Advanced Layout Algorithms**: Force-directed graphs, clustering
2. **Interactive Navigation**: Zoom, pan, and selection
3. **Multi-format Export**: PDF, SVG, and image formats
4. **Web Integration**: HTTP-based visualization service
5. **Real-time Streaming**: WebSocket support for live updates
6. **Machine Learning Integration**: Anomaly detection visualization
7. **Performance Profiling**: Detailed timing analysis
8. **Configuration Templates**: Preset visualization schemes

## Dependencies

The visualization system depends on:

- Standard C++20 libraries
- NLM Core libraries (Brain, Types, etc.)
- Logger system for output management
- Filesystem operations for export

## License

This implementation is part of the NLM project and is released under the same license as the main codebase.

## Support

For issues or questions about the visualization system:

1. Check the documentation for usage examples
2. Review the test cases for expected behavior
3. Consult the main NLM documentation for integration details
4. Report issues through the NLM issue tracker

## Acknowledgments

This implementation was developed as part of the NLM Phase 2 initiative, focusing on providing comprehensive visualization capabilities without complex graphics dependencies.
