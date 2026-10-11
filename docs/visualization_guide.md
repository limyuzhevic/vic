# NLM Visualization System Guide

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Visualization System. It includes detailed information about all visualization components, their usage, configuration options, and integration with the NLM framework.

## Table of Contents

1. [Introduction](#introduction)
2. [Core Visualization Components](#core-visualization-components)
3. [Interactive Visualization](#interactive-visualization)
4. [Neural Network Visualization](#neural-network-visualization)
5. [Spike Activity Visualization](#spike-activity-visualization)
6. [Data Export System](#data-export-system)
7. [Animation Generation](#animation-generation)
8. [Web Dashboard](#web-dashboard)
9. [HTML Export](#html-export)
10. [CLI Integration](#cli-integration)
11. [Visualization Experiments](#visualization-experiments)
12. [Testing and Validation](#testing-and-validation)
13. [Configuration](#configuration)
14. [Examples](#examples)
15. [Troubleshooting](#troubleshooting)
16. [Future Enhancements](#future-enhancements)

## Introduction

The NLM Visualization System provides comprehensive visualization capabilities for neural network simulations and experiments. It includes real-time, interactive, and export visualization tools that support various data formats and analysis requirements.

### Key Features

- **Comprehensive Visualization Coverage**: Neural networks, spike activity, learning progress, performance metrics
- **Multiple Output Formats**: PNG, PDF, SVG, HTML, JSON, CSV, and more
- **Real-time Interaction**: Interactive visualizations with zoom, pan, and selection capabilities
- **Web-based Dashboard**: Browser-based visualization with WebSocket support
- **Batch Processing**: Support for processing large datasets efficiently
- **Experiment Integration**: Integration with NLM experiment management system
- **Configuration Flexibility**: Extensive configuration options for all visualization components
- **Testing Suite**: Comprehensive test suite for all visualization components

## Core Visualization Components

### Visualizer

The `Visualizer` class provides the core visualization framework that supports multiple visualization types and configurations.

#### Visualization Types

- **NEURAL_NETWORK**: Neural network structure and connectivity
- **BRAIN_REGIONS**: Brain region organization and activity
- **SPIKE_RASTER**: Spike raster plots
- **SYNAPTIC_CONNECTIVITY**: Synaptic connectivity and strength
- **LEARNING_PROGRESS**: Learning curve and progress visualization
- **PERFORMANCE_METRICS**: Performance metrics and statistics
- **HEAT_MAP**: Heat maps of neural activity
- **VOLUME_RENDERING**: 3D volume rendering
- **TIME_SERIES**: Time series analysis
- **COMPARISON**: Multi-condition comparison
- **ANIMATION_SEQUENCE**: Animation sequences

#### Configuration Options

```cpp
struct VisualizationConfig {
    VisualizationType type = VisualizationType::NEURAL_NETWORK;
    std::string title = "NLM Visualization";
    std::string outputPath = ".";
    double width = 1920.0;
    double height = 1080.0;
    double updateRate = 30.0;
    bool interactive = true;
    bool showLabels = true;
    bool showConnections = true;
    bool animate = false;
    double animationSpeed = 1.0;
    std::string colorScheme = "default";
    bool enableExport = true;
    bool enableWebExport = false;
    int webPort = 8080;
    std::string format; // PNG, PDF, SVG, HTML
    bool highQuality = false;
};
```

### InteractiveVisualizer

The `InteractiveVisualizer` class provides real-time, interactive visualization capabilities with mouse and keyboard interaction, zoom and pan controls, and element selection.

#### Interaction Features

- **Mouse Interaction**: Click to select elements, drag to pan, scroll to zoom
- **Keyboard Controls**: Arrow keys for navigation, '+' and '-' for zoom
- **Element Selection**: Click on neurons, synapses, or regions to inspect properties
- **Highlighting**: Multiple elements can be highlighted simultaneously
- **Real-time Updates**: Automatic updates with configurable frame rate

### NetworkVisualizer

The `NetworkVisualizer` class provides specialized visualization for neural network structures and connectivity patterns.

#### Layout Algorithms

- **SPRING_EMBEDDING**: Force-directed layout
- **CIRCULAR**: Circular layout
- **HIERARCHICAL**: Tree/hierarchical layout
- **RADIAL**: Radial layout
- **MATRIX**: Adjacency matrix layout
- **FORCE_DIRECTED**: Modern force-directed
- **REINGOLD_TILFORD**: Tree layout algorithm
- **GRID**: Grid-based layout
- **RANDOM**: Random placement

#### Visualization Modes

- **GRAPH**: Graph visualization
- **MATRIX**: Adjacency matrix
- **HEAT_MAP**: Synaptic strength heat map
- **FLOW**: Information flow
- **STATISTICAL**: Statistical overlays
- **COMPARISON**: Multi-network comparison
- **SUBGRAPH**: Focus on specific subnetworks
- **INTEGRATION**: Network integration view
- **ANNOTATION**: Annotation and labeling

### SpikeVisualizer

The `SpikeVisualizer` class provides comprehensive spike activity visualization and analysis.

#### Visualization Modes

- **RASTER_PLOT**: Spike times as dots
- **PERISTIMULUS_HISTOGRAM**: Time bin histograms
- **SPIKE_TRAIN**: Continuous spike trains
- **RATE_MAP**: Firing rate over space/time
- **CROSS_CORRELATION**: Cross-correlation between neurons
- **COHERENCE**: Signal coherence
- **POPULATION_DENSITY**: 2D density plots
- **SYNCHRONY**: Synchronization measures
- **INPUT_OUTPUT_MAPPING**: Input-output relationships
- **TIME_SLICE**: Selected time windows
- **COMPARING**: Multiple conditions
- **STATISTICS_OVERLAY**: Statistical distributions

### DataExporter

The `DataExporter` class provides comprehensive data export capabilities in multiple formats.

#### Supported Export Formats

- **CSV**: Comma-separated values for spreadsheet import
- **JSON**: Structured data for programmatic access
- **HDF5**: Hierarchical data format for large datasets
- **MATLAB**: Octave/MATLAB format
- **SQLITE**: Relational database format
- **XML**: Extensible markup format
- **BINARY**: Compact binary format
- **PARQUET**: Columnar storage format
- **TEXT**: Plain text format
- **YAML**: YAML format
- **TOML**: TOML format

#### Export Features

- **Metadata Support**: Rich metadata for all exported data
- **Compression**: Optional data compression
- **Chunked Export**: Support for large datasets
- **Progress Reporting**: Real-time progress reporting
- **Error Handling**: Robust error handling and recovery
- **Validation**: Data validation before export

### AnimationGenerator

The `AnimationGenerator` class provides animation generation capabilities for time-series data.

#### Animation Formats

- **GIF**: Graphics Interchange Format
- **MP4**: MPEG-4 Video
- **WEBM**: WebM Video
- **HTML5**: HTML5 Canvas animations
- **JS**: JavaScript animations
- **FLASH**: Adobe Flash (deprecated)
- **QUICKTIME**: Apple QuickTime
- **AVI**: Audio Video Interleave
- **MOV**: Apple QuickTime Movie
- **MKV**: Matroska Video
- **WEBM_VP8**: WebM Video (VP8)
- **WEBM_VP9**: WebM Video (VP9)
- **AV1**: AV1 Video
- **H.264**: H.264 Video
- **H.265**: H.265 Video
- **VP8**: VP8 Video
- **VP9**: VP9 Video

#### Animation Features

- **Frame-based Animation**: High-quality frame-based animation generation
- **Time Interpolation**: Smooth time interpolation and easing
- **Interactive Control**: Interactive timeline control
- **Progress Tracking**: Real-time progress tracking
- **Error Handling**: Robust error handling
- **Export Control**: Flexible export options

### WebDashboard

The `WebDashboard` class provides web-based visualization with built-in web server and WebSocket support.

#### Web Features

- **Built-in Web Server**: Local web server for visualization access
- **REST API**: RESTful API for data access
- **WebSocket Support**: Real-time bidirectional communication
- **Cross-platform Compatibility**: Works across all platforms
- **Mobile Support**: Mobile device optimized
- **Authentication**: Optional authentication support
- **Rate Limiting**: Built-in rate limiting
- **Compression**: Automatic compression
- **Caching**: Intelligent caching
- **Responsive Design**: Responsive web design

### HTMLExporter

The `HTMLExporter` class provides HTML export capabilities with extensive customization options.

#### Template Types

- **DEFAULT**: Basic template
- **DASHBOARD**: Dashboard-style template
- **REPORT**: Report template
- **PRESENTATION**: Presentation template
- **EXPERIMENTAL**: Experimental template
- **DEBUG**: Debug template
- **MINIMAL**: Minimal template
- **MODERN**: Modern template
- **CLASSIC**: Classic template
- **CUSTOM**: Custom template

#### Chart Libraries

- **NONE**: No charts
- **CHARTJS**: Chart.js integration
- **PLOTLY**: Plotly integration
- **D3JS**: D3.js integration
- **THREEJS**: Three.js integration
- **PICTORJS**: Pictor.js integration
- **LOLLYPOP**: Lollipop chart support
- **VUE_CHART**: Vue Chart support
- **REACT_CHART**: React Chart support

## Interactive Visualization

### Getting Started

1. **Initialize InteractiveVisualizer**:
```cpp
InteractiveVisualizer::InteractiveConfig config;
config.enableMouseInteraction = true;
config.enableKeyboardControls = true;
config.zoomLevel = 1.0;

InteractiveVisualizer interactiveVisualizer(config);
interactiveVisualizer.initialize(800, 600, "Interactive Visualization");
```

2. **Start Interactive Session**:
```cpp
interactiveVisualizer.start();
```

3. **Update Visualization**:
```cpp
interactiveVisualizer.update(brain, 0.1);
```

4. **Handle Interaction**:
```cpp
InteractiveVisualizer::InteractionEvent event = InteractiveVisualizer::InteractionEvent::MOUSE_CLICK;
std::vector<double> params = {100.0, 200.0, 0.0}; // x, y, button
interactiveVisualizer.handleEvent(event, params);
```

### Available Controls

- **Mouse**: Left-click to select, right-click to context menu, scroll to zoom
- **Keyboard**: Arrow keys for pan, '+' and '-' for zoom, 'E' for selection mode
- **Touch**: Swipe for pan, pinch for zoom (mobile devices)

## Neural Network Visualization

### Network Layout

The NetworkVisualizer supports multiple layout algorithms for different visualization needs:

#### Spring Embedding (Recommended)
```cpp
NetworkVisualizer::NetworkVisualizationConfig config;
config.layout = NetworkVisualizer::LayoutAlgorithm::SPRING_EMBEDDING;
config.mode = NetworkVisualizer::VisualizationMode::GRAPH;
config.nodeSize = 5.0;
config.connectionWidth = 1.0;
```

#### Matrix View
```cpp
NetworkVisualizer::NetworkVisualizationConfig config;
config.layout = NetworkVisualizer::LayoutAlgorithm::MATRIX;
config.mode = NetworkVisualizer::VisualizationMode::MATRIX;
config.showLabels = true;
config.showConnections = false;
```

### Visualization Options

#### Show/Hide Elements
```cpp
// Hide/show neurons
networkVisualizer.setNeuronVisibility("region1", true);

// Hide/show synapses
networkVisualizer.setSynapseVisibility("synapse1", false);

// Highlight specific regions
networkVisualizer.highlightRegion("region1", true);
```

#### Export Network Visualization
```cpp
bool exportSuccess = networkVisualizer.exportVisualization("network_visualization.png");
```

## Spike Activity Visualization

### Raster Plot

The raster plot is the most common visualization for spike activity:

```cpp
SpikeVisualizer::SpikeVisualizationConfig config;
config.mode = SpikeVisualizer::VisualizationMode::RASTER_PLOT;
config.timeScale = 1.0;
config.binWidth = 0.001;
config.showIndividualSpikes = true;

SpikeVisualizer spikeVisualizer(config);
spikeVisualizer.initialize(brain);
spikeVisualizer.update(brain, spikes, 0.1);
spikeVisualizer.render();
```

### Peristimulus Histogram

For analyzing spike activity around specific events:

```cpp
SpikeVisualizer::SpikeVisualizationConfig config;
config.mode = SpikeVisualizer::VisualizationMode::PERISTIMULUS_HISTOGRAM;
config.binWidth = 0.01;
config.showIndividualSpikes = false;

spikeVisualizer.render();
```

### Export Spike Data
```cpp
bool exportSuccess = spikeVisualizer.exportVisualization("spike_analysis.csv");
```

## Data Export System

### Exporting Brain Data
```cpp
DataExporter::ExportConfig config;
config.format = DataExporter::ExportFormat::CSV;
config.outputPath = "./exports";
config.filename = "brain_data";
config.includeMetadata = true;

DataExporter dataExporter(config);
bool success = dataExporter.exportBrainData(brain, "brain_analysis");
```

### Exporting Spike Data
```cpp
bool success = dataExporter.exportSpikeData(spikes, "spike_analysis");
```

### Exporting Configuration
```cpp
bool success = dataExporter.exportConfigData("config_analysis");
```

## Animation Generation

### Creating Animations
```cpp
AnimationGenerator::AnimationConfig config;
config.type = AnimationGenerator::AnimationType::NEURAL_DYNAMICS;
config.format = AnimationGenerator::AnimationFormat::GIF;
config.frameRate = 30;
config.duration = 60.0;
config.width = 800;
config.height = 600;

AnimationGenerator animationGenerator(config);
animationGenerator.initialize(brain);

// Add frame renderer
animationGenerator.addFrameRenderer(
    [this](int frameIndex, double time) {
        // Frame rendering logic
        return frameData;
    }
);

// Generate animation
bool success = animationGenerator.generate(brain, spikes,
    [](double progress, const std::string& status) {
        // Progress callback
    });
```

## Web Dashboard

### Starting Web Dashboard
```cpp
WebDashboard::DashboardConfig config;
config.host = "localhost";
config.port = 8080;
config.enableSSL = false;
config.enableWebSocket = true;
config.enableCORS = true;

WebDashboard webDashboard(config);
webDashboard.start();
```

### Updating Dashboard Data
```cpp
webDashboard.update(brain, spikes);
```

### WebSocket Communication
```cpp
// WebSocket endpoint
std::string wsEndpoint = webDashboard.getWebSocketEndpoint();
std::string apiPrefix = webDashboard.getApiPrefix();
```

## HTML Export

### Generating HTML Report
```cpp
HTMLExporter::HTMLExportConfig config;
config.template = HTMLExporter::TemplateType::DASHBOARD;
config.pageConfig.title = "NLM Analysis Report";
config.pageConfig.description = "Comprehensive analysis of neural network simulation";

HTMLExporter htmlExporter(config);
bool success = htmlExporter.exportBrainVisualization(brain, "./reports");
```

### Exporting Comprehensive Report
```cpp
bool success = htmlExporter.exportComprehensiveReport(brain, spikes, "./reports");
```

## CLI Integration

### Visualization Commands

The NLM visualization system integrates with the CLI engine through the `VisualizationIntegration` class.

#### Available Commands

- `visualize`: Generate visualizations
- `interactive`: Start interactive visualization
- `network`: Visualize neural network structure
- `spikes`: Visualize spike activity
- `export`: Export data in various formats
- `animate`: Generate animations
- `web`: Start web dashboard
- `html`: Export to HTML
- `batch`: Process batch visualizations
- `config`: Configure visualization settings
- `experiment`: Run visualization experiments

#### Command Line Usage

```bash
# Generate comprehensive visualization
nlm visualize --input brain.nml --output report.html --type comprehensive

# Start interactive visualization
nlm interactive --type network --duration 120 --format html

# Export neural network data
nlm export --input brain.nml --format png --output network.png

# Start web dashboard
nlm web --port 8080 --host localhost --theme dark

# Run visualization experiment
nlm experiment --input brain.nml --config experiment.json
```

### Configuration

Visualization configuration can be specified through command-line arguments, configuration files, or environment variables.

```bash
# Command-line arguments
--input <file> : Input file path
--output <file> : Output file path
--format <format> : Output format (html, png, pdf, svg, json, csv)
--type <type> : Visualization type (comprehensive, detailed, summary)
--duration <seconds> : Visualization duration
--fps <frames> : Frames per second
--theme <theme> : Visualization theme (light, dark)

# Configuration file
visualizer_config.json
```

## Visualization Experiments

### Running Experiments

The `VisualizationExperiment` class provides a framework for running systematic visualization experiments.

```cpp
VisualizationExperiment::ExperimentConfig config;
config.name = "Network Analysis Experiment";
config.description = "Analyze neural network structure and dynamics";
config.type = VisualizationExperiment::ExperimentType::NETWORK_VISUALIZATION;
config.inputDataPath = "brain_data.nml";
config.outputPath = "./experiment_results";
config.visualizationType = "network";
config.visualizationFormat = "html";
config.duration = 60.0;
config.generateAnimations = true;
config.exportResults = true;
config.generateReports = true;

VisualizationExperiment experiment(config);
experiment.setBrain(brain);
experiment.setSpikeData(spikes);

// Run experiment
VisualizationExperiment::ExperimentResult result = experiment.run();

if (result.success) {
    std::cout << "Experiment completed successfully!" << std::endl;
    std::cout << "Results saved to: " << result.dataExportPath << std::endl;
} else {
    std::cout << "Experiment failed: " << result.message << std::endl;
}
```

## Testing and Validation

### Running Tests

The visualization system includes a comprehensive test suite:

```bash
# Run all visualization tests
nlm_visualization_test

# Run specific test
nlm_visualization_test --test visualizer
nlm_visualization_test --test network
nlm_visualization_test --test spike

# Run tests with verbose output
nlm_visualization_test --verbose
```

### Test Categories

1. **Core Visualization Tests**: Test basic visualization functionality
2. **Interactive Tests**: Test interactive visualization capabilities
3. **Network Visualization Tests**: Test neural network visualization
4. **Spike Visualization Tests**: Test spike activity visualization
5. **Data Export Tests**: Test data export functionality
6. **Animation Tests**: Test animation generation
7. **Web Dashboard Tests**: Test web dashboard functionality
8. **HTML Export Tests**: Test HTML export functionality

## Configuration

### Visualization Configuration File

Create a configuration file (`visualizer_config.json`) to customize visualization settings:

```json
{
    "visualization": {
        "default_type": "comprehensive",
        "default_format": "html",
        "theme": "light",
        "width": 1920,
        "height": 1080,
        "update_rate": 30,
        "enable_animations": true,
        "animation_speed": 1.0,
        "enable_web_export": true,
        "web_port": 8080,
        "enable_data_export": true,
        "data_export_path": "./exports"
    },
    "network": {
        "layout_algorithm": "spring_embedding",
        "node_size": 5.0,
        "connection_width": 1.0,
        "show_labels": true,
        "show_connections": true
    },
    "spike": {
        "visualization_mode": "raster_plot",
        "time_scale": 1.0,
        "bin_width": 0.001,
        "show_individual_spikes": true,
        "enable_spike_detection": true
    },
    "web": {
        "host": "localhost",
        "enable_ssl": false,
        "enable_cors": true,
        "max_connections": 100,
        "timeout_seconds": 30
    }
}
```

### Environment Variables

Set environment variables to configure visualization behavior:

```bash
export NLM_VISUALIZATION_THEME=dark
export NLM_VISUALIZATION_WIDTH=1280
export NLM_VISUALIZATION_HEIGHT=720
export NLM_VISUALIZATION_FORMAT=html
export NLM_VISUALIZATION_OUTPUT_PATH=./reports
```

## Examples

### Example 1: Basic Network Visualization

```cpp
#include "NetworkVisualizer.hpp"

int main() {
    // Load brain data
    Brain brain("brain_data.nml");
    
    // Create network visualizer
    NetworkVisualizer::NetworkVisualizationConfig config;
    config.layout = NetworkVisualizer::LayoutAlgorithm::SPRING_EMBEDDING;
    config.mode = NetworkVisualizer::VisualizationMode::GRAPH;
    config.nodeSize = 8.0;
    config.connectionWidth = 2.0;
    config.showLabels = true;
    config.showConnections = true;
    
    NetworkVisualizer networkVisualizer(config);
    
    // Initialize and visualize
    if (networkVisualizer.initialize(brain)) {
        networkVisualizer.update(brain, 0.1);
        networkVisualizer.render();
        
        // Export visualization
        networkVisualizer.exportVisualization("network_viz.png");
    }
    
    return 0;
}
```

### Example 2: Spike Analysis

```cpp
#include "SpikeVisualizer.hpp"

int main() {
    // Load brain and spike data
    Brain brain("brain_data.nml");
    std::vector<SpikeEvent> spikes = brain.getSpikeData();
    
    // Create spike visualizer
    SpikeVisualizer::SpikeVisualizationConfig config;
    config.mode = SpikeVisualizer::VisualizationMode::RASTER_PLOT;
    config.timeScale = 1.0;
    config.binWidth = 0.001;
    config.showIndividualSpikes = true;
    config.enableCrossCorrelation = true;
    
    SpikeVisualizer spikeVisualizer(config);
    
    // Initialize and analyze
    if (spikeVisualizer.initialize(brain)) {
        spikeVisualizer.update(brain, spikes, 0.1);
        spikeVisualizer.render();
        
        // Export spike analysis
        spikeVisualizer.exportVisualization("spike_analysis.csv");
        
        // Generate report
        std::string report = spikeVisualizer.generateSpikeAnalysis();
        std::cout << report << std::endl;
    }
    
    return 0;
}
```

### Example 3: Interactive Visualization

```cpp
#include "InteractiveVisualizer.hpp"

int main() {
    // Create interactive visualizer
    InteractiveVisualizer::InteractiveConfig config;
    config.enableMouseInteraction = true;
    config.enableKeyboardControls = true;
    config.zoomLevel = 1.0;
    config.panX = 0.0;
    config.panY = 0.0;
    config.enableSelection = true;
    
    InteractiveVisualizer interactiveVisualizer(config);
    
    // Initialize interactive session
    if (interactiveVisualizer.initialize(1280, 720, "Interactive Neural Network Visualization")) {
        interactiveVisualizer.start();
        
        while (interactiveVisualizer.isActive()) {
            // Update visualization
            interactiveVisualizer.update(brain, 0.1);
            
            // Process input
            interactiveVisualizer.processInput(0.1);
            
            // Render
            interactiveVisualizer.render();
            
            // Handle events
            // (In real implementation, would use event loop)
        }
    }
    
    return 0;
}
```

### Example 4: Web Dashboard

```cpp
#include "WebDashboard.hpp"

int main() {
    // Create web dashboard
    WebDashboard::DashboardConfig config;
    config.host = "localhost";
    config.port = 8080;
    config.enableSSL = false;
    config.enableWebSocket = true;
    config.enableCORS = true;
    config.enableREST = true;
    config.title = "NLM Neural Network Dashboard";
    
    WebDashboard webDashboard(config);
    
    // Start web dashboard
    if (webDashboard.start()) {
        std::cout << "Web dashboard started at: " << webDashboard.getWebSocketEndpoint() << std::endl;
        std::cout << "Access at: http://localhost:8080" << std::endl;
        
        // Keep running (in real implementation, would use proper event loop)
        while (webDashboard.isRunning()) {
            // Update dashboard
            webDashboard.update(brain, spikes);
            
            // Handle connections (in real implementation)
            // webDashboard.handleConnections();
        }
    }
    
    return 0;
}
```

## Troubleshooting

### Common Issues and Solutions

1. **Web Dashboard Not Starting**
   - Check if port 8080 is available
   - Verify network connectivity
   - Ensure proper permissions

2. **Visualization Export Failures**
   - Check output directory permissions
   - Verify file format support
   - Ensure required libraries are installed

3. **Interactive Visualization Not Responding**
   - Check for input handling issues
   - Verify display settings
   - Ensure proper event handling

4. **Performance Issues**
   - Reduce visualization complexity
   - Use streaming for large datasets
   - Optimize rendering parameters

### Debug Commands

```bash
# Check visualization system status
nlm visualize --status

# Debug web dashboard
nlm web --debug

# Check visualization logs
nlm log --visualization

# Validate configuration
nlm config --validate visualizer_config.json
```

## Future Enhancements

### Planned Features

1. **3D Visualization**: Add 3D neural network rendering capabilities
2. **Machine Learning Integration**: Integrate ML algorithms for automated visualization
3. **Cloud Export**: Support for cloud-based visualization services
4. **AR/VR Support**: Augmented and virtual reality visualization
5. **Real-time Collaboration**: Multi-user collaboration features
6. **Advanced Analytics**: Built-in statistical analysis and pattern recognition
7. **Custom Templates**: Support for custom visualization templates
8. **Performance Profiling**: Built-in performance profiling and optimization

### Research Directions

- **Large-scale Network Visualization**: Efficient visualization of million-neuron networks
- **Dynamic Network Analysis**: Real-time analysis of dynamic neural networks
- **Multi-modal Data Integration**: Integration of multiple data modalities
- **Explainable AI**: Making visualization decisions interpretable
- **Distributed Computing**: Distributed visualization across multiple machines

## Conclusion

The NLM Visualization System provides a comprehensive suite of tools for visualizing neural network data and analysis results. With its modular architecture, extensive configuration options, and integration with the NLM framework, it enables researchers and practitioners to create sophisticated visualizations that reveal insights in neural network structure, dynamics, and behavior.

The system continues to evolve with new features and capabilities, making it a powerful tool for neural network research and applications.