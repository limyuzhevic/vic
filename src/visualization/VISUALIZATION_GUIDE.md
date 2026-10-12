# NLM Advanced Visualization System

## Overview

The NLM Advanced Visualization System provides comprehensive real-time visualization capabilities for the NLM artificial brain. This system was developed as part of the Phase 6 Final Audit to address the need for better visualization and metrics collection for research analysis.

## System Components

### NLMLiveVisualization

The `NLMLiveVisualization` class provides:

- **Live Neural Network Visualization**: Interactive 3D visualization of neural networks with real-time activity monitoring
- **Neural Activity Visualization**: Real-time neuron spiking visualization, synaptic connections, and heat maps
- **Memory Visualization**: Working memory contents and episodic memory storage visualization
- **Neuromodulation Visualization**: Dopamine, curiosity, novelty levels with dynamic color coding
- **Development Visualization**: Neural developmental stages and plasticity visualization
- **Integration Visualization**: Complete brain loop connections and system flow visualization
- **Export Capabilities**: Save visualizations to PNG, SVG, JSON, and MP4 formats

### Key Features

#### Real-time Updates
- Updates at configurable rates (default: 60 Hz)
- Thread-safe updates from brain data
- Configurable camera controls and zoom levels
- Interactive pause/resume functionality

#### Data Visualization
- 3D neural network with customizable rendering
- Color-coded activation levels
- Spike raster plots
- Activity heat maps
- Memory state visualization
- Neuromodulation signal visualization

#### Export Capabilities
- PNG: High-resolution screenshots
- SVG: Vector graphics for publications
- JSON: Data export for external analysis
- MP4: Video recordings of simulations

### API Usage

```cpp
// Initialize visualization
NLMLiveVisualization visualization;
visualization.initialize(1920, 1080, false); // false = headless mode

// Configure rendering modes
visualization.setNeuronRenderingMode(true, true, true);
visualization.setMemoryVisualizationMode(true, true);
visualization.setNeuromodulationVisualizationMode(true);
visualization.setDevelopmentVisualizationMode(true);
visualization.setIntegrationFlowVisualizationMode(true);

// Set camera controls
visualization.setCameraPosition(0.0f, 100.0f, 100.0f);
visualization.setZoomLevel(1.5f);
visualization.setRotation(45.0f, 30.0f, 0.0f);

// Update with brain and agent data
visualization.update(brain, agent);

// Render
visualization.render();

// Export data
visualization.exportToJSON("./results/visualization.json");
visualization.exportToPNG("./results/visualization.png");
```

## Integration with Phase 6

The visualization system integrates seamlessly with the Phase 6 Integrated Experiment:

1. **Real-time Updates**: Visualization updates during simulation execution
2. **Data Collection**: Captures frames at configurable intervals
3. **Research Mode**: Enables comprehensive research capabilities
4. **Export Integration**: Automatic export of visualization data

### Phase 6 Integration Example

```cpp
Phase6IntegratedExperiment experiment;
Phase6Config config;
config.enableVisualization = true;

auto result = experiment.run(config);

// Access visualization data
NLMLiveVisualization* viz = experiment.getVisualization();
if (viz && viz->isActive()) {
    viz->exportToJSON("./phase6_visualization.json");
}
```

## Performance Considerations

- **Thread Safety**: All updates are thread-safe
- **Memory Efficiency**: Configurable capture limits
- **Configurable Quality**: Balance between quality and performance
- **Headless Mode**: For automated testing and batch processing

## Limitations

- Requires additional dependencies for full 3D rendering (OpenGL/Vulkan)
- Some features are simplified for demonstration
- Export functionality requires additional libraries

## Future Enhancements

- VR/AR integration
- Multi-agent visualization
- Advanced statistical visualizations
- Real-time collaboration features

## Technical Documentation

- Implementation details in `src/visualization/NLMLiveVisualization.hpp` and `.cpp`
- Integration guides in `Phase6IntegratedExperiment.hpp` and `.cpp`
- API documentation available in the header files
