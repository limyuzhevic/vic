# NLM Visualization Usage Guide

## Overview

This guide provides instructions for using the NLM Advanced Visualization System. The system provides real-time visualization capabilities for NLM artificial brain simulations, supporting both real-time interaction and batch processing.

## System Requirements

### Minimum Requirements
- Operating System: Linux, macOS, or Windows
- C++20 compatible compiler
- Available memory: 2 GB minimum (8 GB recommended)
- Display: 1024x768 minimum resolution (1920x1080 recommended for full features)

### Optional Dependencies
- OpenGL/Vulkan for 3D rendering
- Video codecs for MP4 export
- JSON library for data export

## Installation

### Build from Source
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Visualization Examples
```bash
# Run Phase 6 demo with visualization
./nlm_phase6_demo

# Run visualization-specific test
./nlm_visualization_test
```

## Quick Start

### 1. Basic Visualization

```cpp
#include "NLMLiveVisualization.hpp"
#include "Phase6IntegratedExperiment.hpp"

int main() {
    // Initialize Phase 6 experiment with visualization
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    config.enableVisualization = true;
    
    // Enable research mode
    experiment.enableResearchVisualization();
    experiment.enableAdvancedMetrics();
    
    // Run experiment
    auto result = experiment.run(config);
    
    return 0;
}
```

### 2. Real-time Visualization

```cpp
NLMLiveVisualization visualization;
visualization.initialize(1920, 1080, false); // false = windowed mode

// Configure visualization modes
visualization.setNeuronRenderingMode(true, true, true);
visualization.setMemoryVisualizationMode(true, true);
visualization.setNeuromodulationVisualizationMode(true);
visualization.setDevelopmentVisualizationMode(true);
visualization.setIntegrationFlowVisualizationMode(true);

// Set camera controls
visualization.setCameraPosition(0.0f, 100.0f, 100.0f);
visualization.setZoomLevel(1.5f);
visualization.setRotation(45.0f, 30.0f, 0.0f);

// Update with brain data
visualization.update(brain, agent);

// Render
visualization.render();
```

### 3. Headless Mode

```cpp
NLiveVisualization visualization;
visualization.initialize(1920, 1080, true); // true = headless mode

// Update without rendering
visualization.update(brain, agent);

// Export data
visualization.exportToJSON("./results/visualization_data.json");
visualization.exportToPNG("./results/screenshot.png");
```

### 4. Research Mode

```cpp
Phase6IntegratedExperiment experiment;
Phase6Config config;
config.enableVisualization = true;
config.enableCheckpointing = true;

// Enable research features
experiment.enableResearchVisualization();
experiment.enableAdvancedMetrics();

// Get research tools
ResearchTools* tools = experiment.getResearchTools();
if (tools) {
    tools->setResearchMode(true);
    tools->enableDebugMode(true);
}

// Run experiment
auto result = experiment.run(config);

// Export research data
experiment.exportResearchData("./phase6_research_results");
```

## Visualization Modes

### Neuron Rendering Modes

#### Network Visualization
- **Show Connections**: Display synaptic connections between neurons
- **Show Activity**: Highlight active neurons based on membrane potential
- **Show Heatmap**: Display activity density maps

Configuration:
```cpp
visualization.setNeuronRenderingMode(true, true, true);
```

#### Memory Visualization
- **Working Memory**: Show working memory traces and active traces
- **Episodic Memory**: Display episodic memory episodes and storage

Configuration:
```cpp
visualization.setMemoryVisualizationMode(true, true);
```

#### Neuromodulation Visualization
- **All Neuromodulators**: Display dopamine, curiosity, novelty levels
- **Signal Strength**: Show relative strength of neuromodulation signals

Configuration:
```cpp
visualization.setNeuromodulationVisualizationMode(true);
```

#### Development Visualization
- **Development Stages**: Show current developmental stage
- **Plasticity Rates**: Display plasticity modulation by age

Configuration:
```cpp
visualization.setDevelopmentVisualizationMode(true);
```

#### Integration Flow Visualization
- **System Flow**: Display integration status of all brain systems
- **Connectivity**: Show connections between different systems

Configuration:
```cpp
visualization.setIntegrationFlowVisualizationMode(true);
```

## Camera Controls

### Basic Controls
- **Orbit**: Left mouse button + drag
- **Zoom**: Mouse wheel
- **Pan**: Right mouse button + drag
- **Reset**: 'R' key

### Configuration
```cpp
visualization.setCameraPosition(x, y, z);
visualization.setZoomLevel(zoom);
visualization.setRotation(yaw, pitch, roll);
```

## Export Capabilities

### PNG Export
```cpp
bool success = visualization.exportToPNG("./results/visualization.png");
```

### SVG Export
```cpp
bool success = visualization.exportToSVG("./results/visualization.svg");
```

### JSON Export
```cpp
bool success = visualization.exportToJSON("./results/visualization_data.json");
```

### MP4 Export
```cpp
bool success = visualization.exportToMP4("./results/visualization.mp4", 30.0);
```

## Research Data Collection

### Frame Capture
```cpp
// Capture a frame for analysis
visualization.captureFrame();

// Get captured frames
const std::vector<NLMLiveVisualization::NeuralNode>& frames = visualization.getCapturedFrames();
```

### Research Metrics Integration
```cpp
// Get research metrics from experiment
ResearchMetrics* metrics = experiment.getResearchMetrics();
if (metrics) {
    // Record visualization-specific metrics
    metrics->record("visualization_frame_count", visualization.getCapturedFrames().size());
    metrics->record("visualization_export_success", 1.0);
}
```

## Performance Optimization

### Real-time Performance
- **Update Rate**: Adjust `setUpdateRate()` based on system capabilities
- **Headless Mode**: Use for batch processing to reduce overhead
- **Quality Settings**: Reduce rendering quality for better performance

### Memory Management
- **Capture Limits**: Configure maximum captured frames
- **Memory Efficiency**: Use headless mode for long simulations
- **Garbage Collection**: System manages memory automatically

## Troubleshooting

### Common Issues

#### Visualization Not Displaying
1. Check if visualization was initialized successfully
2. Verify system meets minimum requirements
3. Ensure OpenGL/Vulkan is available (if required)

#### Export Failures
1. Check write permissions to output directory
2. Verify output file extensions are supported
3. Ensure required libraries are available

#### Performance Issues
1. Reduce update rate: `visualization.setUpdateRate(30.0);`
2. Disable heatmap: `visualization.setNeuronRenderingMode(true, true, false);`
3. Use headless mode for production

### Error Codes
- **1**: Visualization initialization failed
- **2**: Export failed
- **3**: Camera configuration failed
- **4**: Data export failed

## Advanced Features

### Custom Visualization Configuration
```cpp
// Advanced configuration
NLMLiveVisualization::NeuralNode node;
node.id = 123;
node.x = 0.0f;
node.y = 0.0f;
node.z = 0.0f;
node.radius = 2.0f;
node.activation = 0.5f;
node.color[0] = 1.0f; // Red
node.color[1] = 0.5f; // Green
node.color[2] = 0.0f; // Blue
node.isSpiking = true;
```

### Research Data Analysis
```cpp
// Analyze captured data
const std::vector<NLMLiveVisualization::NeuralNode>& nodes = visualization.getNeuralNodes();
const std::vector<NLMLiveVisualization::NeuralConnection>& connections = visualization.getNeuralConnections();

// Process data for research
for (const auto& node : nodes) {
    // Analyze neuron activity
}
for (const auto& connection : connections) {
    // Analyze synaptic connections
}
```

## Customization

### Extend Visualization
Create custom visualization components by:
1. Implementing `NLMLiveVisualization` interface
2. Adding custom rendering logic
3. Implementing export functionality

### Modify Configuration
- **Update Rate**: `visualization.setUpdateRate(double hz);`
- **Quality Settings**: `visualization.setNeuronRenderingMode(bool, bool, bool);`
- **Camera Controls**: `visualization.setCameraPosition(float, float, float);`

## Examples

### Example 1: Basic Research Visualization
```cpp
#include "Phase6IntegratedExperiment.hpp"

int main() {
    Phase6IntegratedExperiment experiment;
    Phase6Config config;
    config.neuronCount = 500;
    config.maxSteps = 2000;
    config.enableVisualization = true;
    
    // Enable research features
    experiment.enableResearchVisualization();
    experiment.enableAdvancedMetrics();
    
    // Get visualization and configure
    NLMLiveVisualization* viz = experiment.getVisualization();
    if (viz) {
        viz->setNeuronRenderingMode(true, true, true);
        viz->setMemoryVisualizationMode(true, true);
        viz->setCameraPosition(0.0f, 100.0f, 100.0f);
        viz->setZoomLevel(2.0f);
    }
    
    // Run experiment
    auto result = experiment.run(config);
    
    // Export results
    experiment.exportResearchData("./research_results");
    
    return 0;
}
```

### Example 2: Batch Processing
```cpp
#include "NLMLiveVisualization.hpp"

int main() {
    // Initialize for batch processing
    NLMLiveVisualization visualization;
    visualization.initialize(1920, 1080, true); // Headless mode
    
    // Process multiple experiments
    for (int experimentId = 0; experimentId < 10; ++experimentId) {
        // Run simulation...
        
        // Update visualization
        visualization.update(brain, agent);
        
        // Capture frame
        visualization.captureFrame();
        
        // Export data
        std::string filename = "./results/experiment_" + std::to_string(experimentId) + ".json";
        visualization.exportToJSON(filename);
    }
    
    return 0;
}
```

## Support

### Documentation
- **Technical Documentation**: `src/visualization/VISUALIZATION_GUIDE.md`
- **API Reference**: Header files in `src/visualization/`
- **Integration Guide**: `Phase6IntegratedExperiment.hpp`

### Getting Help
- **Issue Tracker**: Report bugs and issues
- **Community Forum**: Discuss implementation and usage
- **Documentation**: Comprehensive API documentation in header files

## License

MIT License

NLM Visualization System - Phase 6 Implementation
