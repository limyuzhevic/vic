# Visualization Interface API

The NLM Visualization Interface provides comprehensive visualization capabilities for neural network simulation and analysis. This advanced API is designed for researchers, developers, and advanced users who need to visualize and understand complex neural network behavior.

## Overview

The VisualizationInterface class provides real-time visualization of brain state, network structure, and dynamic activity patterns. It supports multiple visualization modes and can be integrated into existing simulation workflows.

## Key Features

### Real-time Visualization
- **Network Topology**: Visualize brain regions, neuron populations, and connectivity
- **Activity Patterns**: Monitor firing rates, spike trains, and neural dynamics
- **Weight Matrices**: Visualize synaptic strength and plasticity patterns
- **State Monitoring**: Track development stages, neuromodulation levels, and memory states

### Advanced Analysis Tools
- **Spike Raster Plots**: Time-series visualization of neural firing events
- **Connectivity Maps**: Heat maps and graphs showing network structure
- **Dynamic Evolution**: Animation of network changes over time
- **Performance Metrics**: Real-time monitoring of simulation statistics

### Export and Analysis
- **Data Export**: Save visualizations and metrics for external analysis
- **Multi-format Output**: Support for various image and data formats
- **API Integration**: Programmatic control for automated analysis

## Basic Usage

```cpp
#include "VisualizationInterface.hpp"
#include "brain/Brain.hpp"

VisualizationInterface vis;

// Initialize visualization
if (!vis.initialize()) {
    std::cerr << "Failed to initialize visualization" << std::endl;
    return 1;
}

// Configure update rate
vis.setUpdateRate(30.0);  // 30 Hz

// Main simulation loop
while (simulationRunning) {
    // ... run brain simulation ...
    
    // Update visualization
    vis.update();
    
    // Render current state
    vis.render();
    
    // Optionally export for analysis
    if (frame % exportInterval == 0) {
        vis.exportFrame("frames/frame_" + std::to_string(frame) + ".png");
    }
}

vis.close();
```

## Visualization Modes

### 1. Network Structure Visualization
```cpp
void visualizeNetwork(const Brain& brain) {
    vis.visualizeNetwork(brain);
    
    // Additional configuration
    vis.setLayoutAlgorithm(LayoutAlgorithm::FDP);
    vis.setNodeSize(10.0, 50.0);
    vis.setColorScheme(ColorScheme::ExcitatoryInhibitory);
    
    // Export for publication
    vis.exportNetworkGraph("network_graph.png");
}
```

### 2. Activity Pattern Visualization
```cpp
void visualizeActivity(const Brain& brain) {
    vis.visualizeActivity(brain);
    
    // Configure activity view
    vis.setActivityMode(ActivityMode::FiringRate);
    vis.setTimeWindow(1000);  // ms
    vis.setSmoothing(0.1);
    
    // Focus on specific regions
    vis.addRegionFilter("hippocampus", true);
    vis.addRegionFilter("cortex", false);
}
```

### 3. Spike Activity Visualization
```cpp
void visualizeSpikes(const std::vector<SpikeEvent>& spikes) {
    vis.visualizeSpikes(spikes);
    
    // Configure spike display
    vis.setRasterPlotBins(50);  // bins for heatmap
    vis.setSpikeSize(2.0);
    vis.setColorByNeuronType(true);
    
    // Interactive features
    vis.enableZoom(true);
    vis.enablePan(true);
}
```

### 4. Weight Matrix Visualization
```cpp
void visualizeWeights(const Brain& brain) {
    vis.visualizeWeights(brain);
    
    // Configure weight display
    vis.setWeightMatrixMode(WeightMatrixMode::Density);
    vis.setMinWeight(-1.0);
    vis.setMaxWeight(1.0);
    vis.setColorMap(ColorMap::Diverging);
    
    // Show plasticity changes
    vis.highlightPlasticChanges(true);
}
```

## Configuration API

### Display Settings
```cpp
// Window configuration
vis.setWindowTitle("NLM Brain Visualization");
vis.setWindowSize(1920, 1080);
vis.setFullscreen(false);

// Display options
vis.setShowLegend(true);
vis.setShowToolbar(true);
vis.setShowStats(true);

// Color and style
vis.setColorScheme(ColorScheme::Scientific);
vis.setFontSize(12);
vis.setLineWidth(1.5);
```

### Data Processing
```cpp
// Time and sampling
vis.setSimulationTime(0.0, 10000.0);  // start, end (ms)
vis.setTimeStep(0.001);  // simulation timestep
vis.setFrameRate(30);  // display refresh rate

// Data filtering
vis.setActivitySmoothing(0.2);
vis.setNoiseReduction(true);
vis.setQualityLevel(QualityLevel::High);
```

### Interaction Controls
```cpp
// User interaction
vis.enableZoom(true);
vis.enablePan(true);
vis.enableRotate(true);
vis.enableHoverInfo(true);

// Selection and annotation
vis.enableRegionSelection(true);
vis.enableNeuronSelection(true);
vis.enableAnnotation(true);
```

## Integration with Brain States

### Real-time Brain Monitoring
```cpp
class BrainVisualizer {
private:
    std::shared_ptr<VisualizationInterface> vis;
    std::shared_ptr<Brain> brain;
    double lastRenderTime;
    
public:
    BrainVisualizer(std::shared_ptr<VisualizationInterface> v, std::shared_ptr<Brain> b)
        : vis(v), brain(b), lastRenderTime(0.0) {}
    
    void updateAndRender(double currentTime) {
        double dt = currentTime - lastRenderTime;
        
        if (dt >= 1.0 / vis->getUpdateRate()) {
            // Update visualization with current brain state
            vis->visualizeNetwork(*brain);
            vis->visualizeActivity(*brain);
            vis->visualizeSpikes(brain->getRecentSpikes(1000));
            
            // Update system state display
            vis->updateSystemState(
                brain->getDevelopmentalStage(),
                brain->getDopamineLevel(),
                brain->getPredictionError()
            );
            
            // Render everything
            vis->render();
            
            lastRenderTime = currentTime;
        }
    }
};
```

### Advanced Brain Analysis
```cpp
void analyzeBrainDevelopment(const Brain& brain) {
    VisualizationInterface vis;
    vis.initialize();
    
    // Development stage visualization
    vis.setDevelopmentStage(brain.getDevelopmentalStage());
    
    // Plasticity pattern visualization
    vis.visualizePlasticityPatterns(brain);
    
    // Memory integration visualization
    vis.visualizeMemoryIntegration(brain);
    
    // Export analysis results
    vis.exportAnalysisReport("development_analysis.html");
}
```

## Python Integration

```python
import pynlm
import matplotlib.pyplot as plt

# Create brain and visualization
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create visualization interface
vis = pynlm.createVisualizationInterface()

# Configure visualization
vis.setUpdateRate(15.0)
vis.enableRealTime(true)

# Run simulation and visualize
for step in range(1000):
    brain.step(step)
    
    # Get current state
    spike_data = brain.getSpikeData(step-100, step)
    activity_data = brain.getActivityStats()
    
    # Update visualization
    vis.updateSpikeData(spike_data)
    vis.updateActivityData(activity_data)
    vis.render()
    
    # Save frames for video
    if step % 10 == 0:
        vis.saveFrame(f"frame_{step:04d}.png")

vis.close()
```

## Export and Data Formats

### Supported Export Formats
```cpp
// Image formats
vis.exportFrame("screenshot.png");
vis.exportNetworkGraph("network_graph.pdf");
vis.exportSpikeRaster("spike_raster.svg");

// Data formats
vis.exportData("neural_data.csv", DataFormat::CSV);
vis.exportData("analysis_results.json", DataFormat::JSON);
vis.exportData("metrics.bin", DataFormat::Binary);

// Video output
vis.createVideo("simulation.mp4", FrameRate::FPS_30);
vis.addFramesToVideo("frame_%04d.png");
vis.finalizeVideo();
```

## Configuration Files

### Visualization Configuration
Create `visualization_config.json` for persistent settings:

```json
{
    "window": {
        "width": 1920,
        "height": 1080,
        "fullscreen": false,
        "title": "NLM Brain Visualization"
    },
    "colors": {
        "excitatory": "#FF0000",
        "inhibitory": "#0000FF",
        "background": "#000000",
        "grid": "#333333"
    },
    "display": {
        "show_legend": true,
        "show_toolbar": true,
        "show_stats": true,
        "fps": 30
    },
    "networks": {
        "default_layout": "spring",
        "node_size_range": [10, 50],
        "min_connections": 1
    }
}
```

## Performance Optimization

### Multi-threading
```cpp
// Enable GPU acceleration if available
vis.enableGPURendering(true);
vis.setRenderMode(RenderMode::HardwareAccelerated);

// Multi-window support
vis.createSecondaryWindow("Analysis Panel");
vis.splitView(true);
```

### Memory Management
```cpp
// Streaming for large networks
vis.enableStreaming(true);
vis.setStreamBufferSize(1000);  // frames in memory
vis.setStreamingQuality(QualityLevel::Medium);

// On-demand loading
vis.enableLazyLoading(true);
vis.setPreloadPercentage(0.5);
```

## Troubleshooting

### Common Issues

1. **Performance Issues**: Reduce update rate, disable streaming, or optimize display settings
2. **Memory Errors**: Increase streaming buffer, disable high-quality rendering
3. **Display Issues**: Check OpenGL compatibility, update graphics drivers
4. **Import Errors**: Ensure visualization libraries are properly linked

### Debug Mode
```cpp
vis.enableDebugMode(true);
vis.setVerboseLogging(true);
vis.connectDebugger("localhost", 8080);
```

## Future Enhancements

The visualization interface is designed to be extensible:

1. **Web-based Visualization**: Real-time web interface for remote collaboration
2. **VR/AR Support**: Virtual and augmented reality visualization
3. **Machine Learning Integration**: AI-driven visualization optimization
4. **Multi-monitor Support**: Extended display across multiple screens
5. **Cloud Rendering**: Remote visualization with GPU acceleration

## License

MIT License - See LICENSE file for details
