# NLM Visualization and Debugging Guide

## Overview

The Visualization and Debugging Guide provides comprehensive documentation for NLM's advanced visualization and debugging capabilities. The Visualization API offers powerful tools for visualizing neural networks, performance metrics, and debugging information.

## Key Features

### 1. Neural Network Visualization

- **Network structure visualization**: Visualize neural network topology
- **Neuron activity visualization**: Visualize neuron firing patterns
- **Synapse weight visualization**: Visualize synaptic connections and weights
- **Spike raster visualization**: Visualize spike trains and temporal patterns
- **Population visualization**: Visualize neural population activity

### 2. Performance Dashboard

- **Real-time performance monitoring**: Monitor performance metrics in real-time
- **Performance trends**: Visualize performance trends over time
- **Custom metrics**: Add custom performance metrics
- **Threshold alerts**: Set performance thresholds and alerts

### 3. Debug Visualization

- **Debug data visualization**: Visualize debug data in multiple formats
- **Heatmap visualization**: Visualize data as heatmaps
- **Scatter plot visualization**: Visualize data as scatter plots
- **Contour plot visualization**: Visualize data as contour plots
- **Graph visualization**: Visualize networks and graphs

### 4. Export Functionality

- **Multiple export formats**: Export visualizations in various formats
- **PNG export**: High-quality PNG images
- **SVG export**: Scalable vector graphics
- **JSON export**: Structured data export
- **CSV export**: Tabular data export
- **HTML export**: Web-based visualization
- **VRML export**: 3D visualization
- **OBJ export**: 3D object files
- **GLTF export**: 3D models

### 5. Interactive Features

- **Interactive mode**: Enable interactive visualization
- **Update listeners**: Listen for visualization updates
- **Component processing**: Process visualization components
- **Real-time updates**: Update visualizations in real-time

## Getting Started

### Basic Visualization

```cpp
#include "src/agent/VisualizationAPI.hpp"

// Access visualization system
VisualizationAPI& vizAPI = VisualizationAPI::getInstance();

// Initialize visualization
vizAPI.initialize();

// Set visualization mode
vizAPI.setVisualizationMode(VisualizationMode::RealTime);

// Create performance dashboard
std::vector<std::string> metricNames = {"FiringRate", "MemoryUsage", "CPUUsage"};
std::vector<std::vector<double>> metricValues = {
    {0.1, 0.2, 0.3, 0.4, 0.5},
    {100.0, 200.0, 150.0, 180.0, 160.0},
    {10.0, 15.0, 12.0, 18.0, 14.0}
};
vizAPI.createPerformanceDashboard(metricNames, metricValues);

// Visualize performance metrics
vizAPI.visualizePerformanceMetrics("simulation_session_1");
```

### Neural Network Visualization

```cpp
#include "src/agent/VisualizationAPI.hpp"
#include "src/brain/Brain.hpp"

void visualizeNeuralNetwork() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Get brain instance
    Brain& brain = getBrainInstance();
    
    // Visualize network structure
    vizAPI.visualizeNetworkStructure(brain);
    
    // Visualize neuron activity
    vizAPI.visualizeNeuronActivity(brain, 0.5);
    
    // Visualize synapse weights
    vizAPI.visualizeSynapseWeights(brain, 0.0, 1.0);
    
    // Create neural network visualization data
    NeuralNetworkVisualization vizData;
    // ... populate visualization data ...
    
    // Visualize network
    vizAPI.visualizeNetwork(brain, vizData);
}
```

### Debug Visualization

```cpp
#include "src/agent/VisualizationAPI.hpp"

void createDebugVisualization() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create debug visualization
    DebugVisualization debugViz;
    debugViz.visualizationType = "heatmap";
    debugViz.mode = "heatmap";
    debugViz.title = "Neuron Activity Heatmap";
    debugViz.description = "Visualization of neuron activity patterns";
    
    // Add debug data
    std::vector<std::vector<float>> debugData = {
        {0.1, 0.2, 0.3},
        {0.4, 0.5, 0.6},
        {0.7, 0.8, 0.9}
    };
    debugViz.debugData = debugData;
    
    // Create debug visualization
    vizAPI.createDebugVisualization(debugViz);
}
```

### Export Functionality

```cpp
#include "src/agent/VisualizationAPI.hpp"

void exportVisualizations() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create export configuration
    ExportConfig exportConfig;
    exportConfig.format = ExportFormat::PNG;
    exportConfig.filepath = "./output/visualization.png";
    exportConfig.filename = "neural_network_viz";
    exportConfig.title = "Neural Network Visualization";
    exportConfig.description = "Visualization of neural network structure and activity";
    exportConfig.includeMetadata = true;
    exportConfig.compress = false;
    
    // Create neural network visualization data
    NeuralNetworkVisualization networkData;
    // ... populate visualization data ...
    
    // Create performance dashboard
    PerformanceDashboard perfData;
    // ... populate dashboard data ...
    
    // Create debug visualizations
    std::vector<DebugVisualization> debugVizs;
    // ... create debug visualizations ...
    
    // Export visualization
    vizAPI.exportVisualization(exportConfig, networkData, perfData, debugVizs);
}
```

### Interactive Visualization

```cpp
#include "src/agent/VisualizationAPI.hpp"

void createInteractiveVisualization() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Enable interactive mode
    vizAPI.enableInteractiveMode();
    
    // Add update listener
    std::string listenerId = "performance_listener";
    vizAPI.addUpdateListener([listenerId](const VisualizationUpdate& update) {
        // Handle visualization update
        std::cout << "Visualization update: " << update.eventType << std::endl;
        std::cout << "Component: " << update.componentName << std::endl;
        // Update visualization based on update
    });
    
    // Set up update listener
    auto updateListener = [listenerId](const VisualizationUpdate& update) {
        // Update visualization based on update
        if (update.componentName == "performance_dashboard") {
            updatePerformanceDashboard(update.data);
        } else if (update.componentName == "neural_network") {
            updateNeuralNetworkVisualization(update.data);
        }
    };
    
    vizAPI.addUpdateListener(updateListener);
    
    // Process component data
    std::vector<float> inputData = {0.1, 0.2, 0.3, 0.4, 0.5};
    std::vector<float> outputData;
    
    bool processingSuccess = vizAPI.processComponent(
        "neural_network_analyzer",
        inputData,
        outputData
    );
}
```

## Visualization Types

### 1. Neural Network Visualization

#### Network Structure Visualization

```cpp
void visualizeNetworkStructure(const Brain& brain) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create neural network visualization data
    NeuralNetworkVisualization vizData;
    
    // Populate neuron positions
    std::vector<std::vector<float>> neuronPositions;
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            std::vector<float> position = {
                static_cast<float>(region->getCenter().x),
                static_cast<float>(region->getCenter().y),
                static_cast<float>(region->getCenter().z)
            };
            neuronPositions.push_back(position);
        }
    }
    vizData.neuronPositions = neuronPositions;
    
    // Populate neuron sizes
    std::vector<std::vector<float>> neuronSizes;
    for (const auto& population : vizData.neuronPositions) {
        neuronSizes.push_back({1.0, 1.0, 1.0});  // Uniform size for now
    }
    vizData.neuronSizes = neuronSizes;
    
    // Populate neuron activations
    std::vector<std::vector<float>> neuronActivations;
    for (const auto& population : vizData.neuronPositions) {
        neuronActivations.push_back({0.5, 0.5, 0.5});  // Default activation
    }
    vizData.neuronActivations = neuronActivations;
    
    // Populate neuron colors
    std::vector<std::array<float, 3>> neuronColors;
    for (const auto& population : vizData.neuronPositions) {
        neuronColors.push_back({0.5, 0.5, 0.5});  // Gray color for now
    }
    vizData.neuronColors = neuronColors;
    
    // Populate synapse connections
    std::vector<std::pair<size_t, size_t>> synapses;
    // ... populate synapse connections ...
    vizData.synapses = synapses;
    
    // Populate synapse weights
    std::vector<float> synapseWeights;
    // ... populate synapse weights ...
    vizData.synapseWeights = synapseWeights;
    
    // Populate synapse colors
    std::vector<std::array<float, 3>> synapseColors;
    // ... populate synapse colors ...
    vizData.synapseColors = synapseColors;
    
    // Visualize network
    vizAPI.visualizeNetwork(brain, vizData);
}
```

#### Neuron Activity Visualization

```cpp
void visualizeNeuronActivity(const Brain& brain, double threshold) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Get neuron activity data
    std::vector<std::vector<float>> spikes;
    for (const auto& region : brain.getRegions()) {
        for (const auto& population : region->getPopulations()) {
            std::vector<float> populationSpikes;
            for (const auto& neuron : population->getNeurons()) {
                populationSpikes.push_back(neuron->getFiringRate());
            }
            spikes.push_back(populationSpikes);
        }
    }
    
    // Visualize spike rasters
    vizAPI.visualizeSpikeRasters(spikes, 1.0);
    
    // Visualize neuron activity
    vizAPI.visualizeNeuronActivity(brain, threshold);
}
```

#### Synapse Weight Visualization

```cpp
void visualizeSynapseWeights(const Brain& brain, double minWeight, double maxWeight) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Visualize synapse weights
    vizAPI.visualizeSynapseWeights(brain, minWeight, maxWeight);
}
```

#### Spike Raster Visualization

```cpp
void visualizeSpikeRasters(const std::vector<std::vector<float>>& spikes, double timescale) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Visualize spike rasters
    vizAPI.visualizeSpikeRasters(spikes, timescale);
}
```

### 2. Performance Dashboard Visualization

#### Performance Metrics Visualization

```cpp
void visualizePerformanceMetrics(const std::string& sessionId) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Visualize performance metrics
    vizAPI.visualizePerformanceMetrics(sessionId);
}
```

#### Custom Metrics Visualization

```cpp
void createCustomPerformanceDashboard() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create custom performance dashboard
    std::vector<std::string> metricNames = {
        "FiringRate",
        "MemoryUsage",
        "CPUUsage",
        "ResponseTime",
        "Accuracy"
    };
    
    std::vector<std::vector<double>> metricValues = {
        {0.1, 0.2, 0.3, 0.4, 0.5},
        {100.0, 200.0, 150.0, 180.0, 160.0},
        {10.0, 15.0, 12.0, 18.0, 14.0},
        {50.0, 60.0, 55.0, 70.0, 65.0},
        {0.8, 0.85, 0.9, 0.88, 0.92}
    };
    
    std::vector<std::vector<double>> metricTimestamps = {
        {0.0, 1.0, 2.0, 3.0, 4.0},
        {0.0, 1.0, 2.0, 3.0, 4.0},
        {0.0, 1.0, 2.0, 3.0, 4.0},
        {0.0, 1.0, 2.0, 3.0, 4.0},
        {0.0, 1.0, 2.0, 3.0, 4.0}
    };
    
    vizAPI.createPerformanceDashboard(metricNames, metricValues);
}
```

### 3. Debug Visualization

#### Heatmap Visualization

```cpp
void createHeatmapVisualization(const std::string& title, const std::vector<std::vector<float>>& data) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create debug visualization
    DebugVisualization debugViz;
    debugViz.visualizationType = "heatmap";
    debugViz.mode = "heatmap";
    debugViz.title = title;
    debugViz.debugData = data;
    
    vizAPI.createDebugVisualization(debugViz);
}
```

#### Scatter Plot Visualization

```cpp
void createScatterPlotVisualization(const std::string& title, const std::vector<std::vector<float>>& data) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create debug visualization
    DebugVisualization debugViz;
    debugViz.visualizationType = "scatter";
    debugViz.mode = "scatter";
    debugViz.title = title;
    debugViz.debugData = data;
    
    vizAPI.createDebugVisualization(debugViz);
}
```

#### Contour Plot Visualization

```cpp
void createContourPlotVisualization(const std::string& title, const std::vector<std::vector<float>>& data) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create debug visualization
    DebugVisualization debugViz;
    debugViz.visualizationType = "contour";
    debugViz.mode = "contour";
    debugViz.title = title;
    debugViz.debugData = data;
    
    vizAPI.createDebugVisualization(debugViz);
}
```

#### Graph Visualization

```cpp
void createGraphVisualization(const std::string& title, const std::vector<std::vector<float>>& data) {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Create debug visualization
    DebugVisualization debugViz;
    debugViz.visualizationType = "graph";
    debugViz.mode = "graph";
    debugViz.title = title;
    debugViz.debugData = data;
    
    vizAPI.createDebugVisualization(debugViz);
}
```

## Export Formats

### 1. JSON Export

```cpp
void exportToJSON(const std::string& filepath, 
                 const NeuralNetworkVisualization& networkData,
                 const PerformanceDashboard& perfData) const
```

### 2. CSV Export

```cpp
void exportToCSV(const std::string& filepath,
                 const PerformanceDashboard& perfData) const
```

### 3. PNG Export

```cpp
void exportToImage(const std::string& filepath,
                   const ExportConfig& config) const
```

### 4. SVG Export

```cpp
void exportToSVG(const std::string& filepath,
                 const NeuralNetworkVisualization& networkData,
                 const PerformanceDashboard& perfData) const
```

### 5. HTML Export

```cpp
void exportToHTML(const std::string& filepath,
                  const NeuralNetworkVisualization& networkData,
                  const PerformanceDashboard& perfData,
                  const std::vector<DebugVisualization>& debugVizs) const
```

### 6. VRML Export

```cpp
void exportToVRML(const std::string& filepath,
                  const NeuralNetworkVisualization& networkData) const
```

### 7. OBJ Export

```cpp
void exportToOBJ(const std::string& filepath,
                 const NeuralNetworkVisualization& networkData) const
```

### 8. GLTF Export

```cpp
void exportToGLTF(const std::string& filepath,
                   const NeuralNetworkVisualization& networkData) const
```

## Export Helper Functions

### Export Configuration

```cpp
struct ExportConfig {
    ExportFormat format;              // Export format
    std::string filepath;             // File path
    std::string filename;             // Filename
    std::string title;                // Title
    std::string description;          // Description
    bool includeMetadata;             // Whether to include metadata
    bool compress;                    // Whether to compress
    std::map<std::string, std::string> customOptions;  // Custom options
}
```

### Export Format Conversion

```cpp
inline std::string exportFormatToString(ExportFormat format) {
    switch (format) {
        case ExportFormat::JSON: return "json";
        case ExportFormat::CSV: return "csv";
        case ExportFormat::PNG: return "png";
        case ExportFormat::SVG: return "svg";
        case ExportFormat::HTML: return "html";
        case ExportFormat::VRML: return "vrml";
        case ExportFormat::OBJ: return "obj";
        case ExportFormat::GLTF: return "gltf";
        default: return "unknown";
    }
}

inline ExportFormat stringToExportFormat(const std::string& str) {
    if (str == "json") return ExportFormat::JSON;
    if (str == "csv") return ExportFormat::CSV;
    if (str == "png") return ExportFormat::PNG;
    if (str == "svg") return ExportFormat::SVG;
    if (str == "html") return ExportFormat::HTML;
    if (str == "vrml") return ExportFormat::VRML;
    if (str == "obj") return ExportFormat::OBJ;
    if (str == "gltf") return ExportFormat::GLTF;
    return ExportFormat::JSON;
}
```

## Visualization Components

### Component Management

```cpp
// Register component
bool VisualizationAPI::registerComponent(const VisualizationComponent& component)

// Unregister component
bool VisualizationAPI::unregisterComponent(const std::string& componentName)

// Get component
VisualizationComponent* VisualizationAPI::getComponent(const std::string& componentName)

// Get all components
std::vector<VisualizationComponent> VisualizationAPI::getAllComponents() const
```

### Component Structure

```cpp
struct VisualizationComponent {
    std::string name;                                          // Component name
    std::string type;                                          // Component type
    std::string description;                                   // Component description
    std::string implementation;                                // Component implementation
    std::map<std::string, std::string> parameters;            // Component parameters
    std::vector<std::string> requiredData;                     // Required data
    bool isActive;                                             // Whether component is active
    std::function<void(const std::vector<float>& data, std::vector<float>& output)> processor;  // Component processor
}
```

### Component Processing

```cpp
// Process component
bool VisualizationAPI::processComponent(
    const std::string& componentName,
    const std::vector<float>& inputData,
    std::vector<float>& outputData
)

// Process all components
std::vector<float> VisualizationAPI::processAllComponents(
    const std::vector<float>& inputData
)
```

## Visualization Updates

### Update Management

```cpp
// Add update listener
void VisualizationAPI::addUpdateListener(
    std::function<void(const VisualizationUpdate&)> listener
)

// Remove update listener
void VisualizationAPI::removeUpdateListener(const std::string& listenerId)

// Broadcast update
void VisualizationAPI::broadcastUpdate(const VisualizationUpdate& update)
```

### Update Structure

```cpp
struct VisualizationUpdate {
    std::chrono::high_resolution_clock::time_point timestamp;  // Update timestamp
    std::string eventType;                                    // Event type
    std::string componentName;                               // Component name
    std::vector<float> data;                                 // Update data
    std::map<std::string, std::string> metadata;            // Update metadata
}
```

## Visualization Statistics

### Statistics Structure

```cpp
struct VisualizationStats {
    size_t totalComponents;                  // Total components
    size_t activeComponents;                 // Active components
    size_t totalUpdates;                     // Total updates
    uint64_t totalProcessingTime;            // Total processing time
    double averageFrameTime;                 // Average frame time
    size_t memoryUsage;                      // Memory usage
    double cpuUsage;                         // CPU usage
    std::map<std::string, double> componentPerformance;  // Component performance
}
```

### Statistics API

```cpp
// Get statistics
VisualizationStats VisualizationAPI::getStats() const
```

## Visualization Integration

### Integration with NLM

Visualization API integrates with NLM components:

1. **Brain integration**: Visualize neural network data
2. **Performance integration**: Visualize performance metrics
3. **Memory integration**: Visualize memory usage
4. **Configuration integration**: Visualize configuration data
5. **Plugin integration**: Visualize plugin activity

### Integration API

```cpp
// Set visualization configuration
void VisualizationAPI::setVisualizationConfig(const nlm::Config& config)

// Get visualization configuration
nlm::Config VisualizationAPI::getVisualizationConfig() const

// Check if component is enabled
bool VisualizationAPI::isComponentEnabled(const std::string& componentName) const

// Get component update rate
double VisualizationAPI::getComponentUpdateRate(const std::string& componentName) const
```

### Integration Example

```cpp
void integrateVisualizationWithNLM() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Set visualization configuration
    nlm::Config vizConfig;
    vizConfig.setValue("visualization.mode", "realtime");
    vizConfig.setValue("visualization.export_formats", std::vector<std::string>{"png", "svg", "json"});
    vizConfig.setValue("visualization.interactive_enabled", true);
    
    vizAPI.setVisualizationConfig(vizConfig);
    
    // Initialize visualization
    vizAPI.initialize();
    
    // Enable interactive mode
    vizAPI.enableInteractiveMode();
}
```

## Visualization Commands

### Command Line Interface

The visualization system includes a command-line interface:

```bash
# Initialize visualization system
./advanced_visualization_control.sh --initialize

# Set visualization mode
./advanced_visualization_control.sh --set-mode realtime

# List visualization components
./advanced_visualization_control.sh --list-components

# Export visualization
./advanced_visualization_control.sh --export --format png --filepath ./output/viz.png

# Show debug visualization
./advanced_visualization_control.sh --show-debug viz_id

# Update debug visualization
./advanced_visualization_control.sh --update-debug viz_id data_file

# Enable interactive mode
./advanced_visualization_control.sh --enable-interactive

# Disable interactive mode
./advanced_visualization_control.sh --disable-interactive
```

## Visualization API Reference

### Class: VisualizationAPI

| Method | Description |
|--------|-------------|
| `initialize()` | Initialize visualization system |
| `shutdown()` | Shutdown visualization system |
| `isActive()` | Check if visualization system is active |
| `setVisualizationMode(mode)` | Set visualization mode |
| `getVisualizationMode()` | Get visualization mode |
| `registerComponent(component)` | Register visualization component |
| `unregisterComponent(name)` | Unregister visualization component |
| `getComponent(name)` | Get visualization component |
| `getAllComponents()` | Get all visualization components |
| `visualizeNetwork(brain, vizData)` | Visualize neural network |
| `visualizeNetworkStructure(brain)` | Visualize neural network structure |
| `visualizeNeuronActivity(brain, threshold)` | Visualize neuron activity |
| `visualizeSynapseWeights(brain, minWeight, maxWeight)` | Visualize synapse weights |
| `visualizeSpikeRasters(spikes, timescale)` | Visualize spike rasters |
| `updatePerformanceDashboard(dashboard)` | Update performance dashboard |
| `createPerformanceDashboard(metricNames, values)` | Create performance dashboard |
| `visualizePerformanceMetrics(sessionId)` | Visualize performance metrics |
| `createDebugVisualization(debugViz)` | Create debug visualization |
| `updateDebugVisualization(id, data)` | Update debug visualization |
| `showDebugVisualization(id)` | Show debug visualization |
| `exportVisualization(config, networkData, perfData, debugVizs)` | Export visualization |
| `exportToJSON(filepath, networkData, perfData)` | Export to JSON |
| `exportToCSV(filepath, perfData)` | Export to CSV |
| `exportToImage(filepath, config)` | Export to image |
| `addUpdateListener(listener)` | Add update listener |
| `removeUpdateListener(id)` | Remove update listener |
| `broadcastUpdate(update)` | Broadcast update |
| `setVisualizationConfig(config)` | Set visualization configuration |
| `getVisualizationConfig()` | Get visualization configuration |
| `enableInteractiveMode()` | Enable interactive mode |
| `disableInteractiveMode()` | Disable interactive mode |
| `isInteractiveModeEnabled()` | Check if interactive mode is enabled |
| `processComponent(name, inputData, outputData)` | Process visualization component |
| `processAllComponents(inputData)` | Process all components |
| `getStats()` | Get visualization statistics |

### Struct: VisualizationComponent

| Field | Type | Description |
|-------|------|-------------|
| `name` | std::string | Component name |
| `type` | std::string | Component type |
| `description` | std::string | Component description |
| `implementation` | std::string | Component implementation |
| `parameters` | std::map<std::string, std::string> | Component parameters |
| `requiredData` | std::vector<std::string> | Required data |
| `isActive` | bool | Whether component is active |
| `processor` | std::function | Component processor |

### Struct: NeuralNetworkVisualization

| Field | Type | Description |
|-------|------|-------------|
| `neuronPositions` | std::vector<std::vector<float>> | Neuron positions |
| `neuronSizes` | std::vector<std::vector<float>> | Neuron sizes |
| `neuronActivations` | std::vector<std::vector<float>> | Neuron activations |
| `neuronColors` | std::vector<std::array<float, 3>> | Neuron colors |
| `synapses` | std::vector<std::pair<size_t, size_t>> | Synapse connections |
| `synapseWeights` | std::vector<float> | Synapse weights |
| `synapseColors` | std::vector<std::array<float, 3>> | Synapse colors |
| `populationCenters` | std::vector<std::vector<float>> | Population centers |
| `populationRadii` | std::vector<float> | Population radii |

### Struct: PerformanceDashboard

| Field | Type | Description |
|-------|------|-------------|
| `metricNames` | std::vector<std::string> | Metric names |
| `metricValues` | std::vector<std::vector<double>> | Metric values |
| `metricTimestamps` | std::vector<std::vector<double>> | Metric timestamps |
| `metricColors` | std::vector<std::array<float, 3>> | Metric colors |
| `metricUnits` | std::vector<std::string> | Metric units |
| `metricThresholds` | std::vector<double> | Metric thresholds |
| `metricDescriptions` | std::map<std::string, std::string> | Metric descriptions |

### Struct: DebugVisualization

| Field | Type | Description |
|-------|------|-------------|
| `visualizationType` | std::string | Visualization type |
| `debugData` | std::vector<std::vector<float>> | Debug data |
| `title` | std::string | Title |
| `description` | std::string | Description |
| `mode` | std::string | Visualization mode |
| `options` | std::map<std::string, std::string> | Visualization options |

### Struct: ExportConfig

| Field | Type | Description |
|-------|------|| `format` | ExportFormat | Export format |
| `filepath` | std::string | File path |
| `filename` | std::string | Filename |
| `title` | std::string | Title |
| `description` | std::string | Description |
| `includeMetadata` | bool | Whether to include metadata |
| `compress` | bool | Whether to compress |
| `customOptions` | std::map<std::string, std::string> | Custom options |

### Struct: VisualizationUpdate

| Field | Type | Description |
|-------|------|| `timestamp` | std::chrono::high_resolution_clock::time_point | Timestamp |
| `eventType` | std::string | Event type |
| `componentName` | std::string | Component name |
| `data` | std::vector<float> | Update data |
| `metadata` | std::map<std::string, std::string> | Update metadata |

### Struct: VisualizationStats

| Field | Type | Description |
|-------|------|| `totalComponents` | size_t | Total components |
| `activeComponents` | size_t | Active components |
| `totalUpdates` | size_t | Total updates |
| `totalProcessingTime` | uint64_t | Total processing time |
| `averageFrameTime` | double | Average frame time |
| `memoryUsage` | size_t | Memory usage |
| `cpuUsage` | double | CPU usage |
| `componentPerformance` | std::map<std::string, double> | Component performance |

### Enum: VisualizationMode

| Value | Description |
|-------|-------------|
| `Static` | Static visualization |
| `Dynamic` | Dynamic visualization |
| `RealTime` | Real-time visualization |
| `EventDriven` | Event-driven visualization |
| `PerformanceBased` | Performance-based visualization |

### Enum: VisualizationComponent

| Value | Description |
|-------|-------------|
| `NetworkStructure` | Network structure visualization |
| `NeuronActivity` | Neuron activity visualization |
| `SynapseWeights` | Synapse weight visualization |
| `SpikeRasters` | Spike raster visualization |
| `PerformanceMetrics` | Performance metrics visualization |
| `MemoryUsage` | Memory usage visualization |
| `Custom` | Custom visualization |

### Enum: ExportFormat

| Value | Description |
|-------|-------------|
| `JSON` | JSON export |
| `CSV` | CSV export |
| `PNG` | PNG export |
| `SVG` | SVG export |
| `HTML` | HTML export |
| `VRML` | VRML export |
| `OBJ` | OBJ export |
| `GLTF` | GLTF export |

## Visualization Best Practices

### 1. Visualization Design

- Use appropriate visualization types for data
- Ensure visualizations are clear and informative
- Use consistent visual styles
- Provide interactive capabilities when possible
- Optimize visualizations for performance

### 2. Performance Considerations

- Use efficient rendering methods
- Limit the number of visualized elements
- Use appropriate data formats
- Implement real-time updates efficiently
- Monitor visualization performance

### 3. Export Considerations

- Use appropriate export formats for use cases
- Optimize export files for size and quality
- Implement export error handling
- Provide export progress feedback
- Validate export files

### 4. Integration Considerations

- Integrate visualization with NLM components
- Use appropriate data formats for integration
- Implement error handling for integration
- Test visualization integration thoroughly
- Monitor integration performance

## Visualization Troubleshooting

### Common Issues

1. **Visualization performance issues**
   - Reduce number of visualized elements
   - Use efficient rendering methods
   - Optimize data formats
   - Implement real-time updates efficiently

2. **Visualization export issues**
   - Check file permissions
   - Verify export format support
   - Check available disk space
   - Verify export configuration

3. **Visualization integration issues**
   - Check data format compatibility
   - Verify integration configuration
   - Check component availability
   - Test integration with different NLM versions

4. **Visualization display issues**
   - Check display settings
   - Verify visualization configuration
   - Test with different display modes
   - Check for graphics driver issues

### Debugging Tools

```bash
# Enable debug logging
export NLM_VISUALIZATION_DEBUG=1

# Check visualization logs
tail -f /var/log/nlm/visualization.log

# List visualization components
./nlm_agent --visualization-stats

# Get visualization metrics
./nlm_agent --visualization-metrics
```

## Conclusion

The Visualization API provides comprehensive visualization and debugging capabilities for NLM. It enables:

1. **Neural network visualization** with multiple visualization types
2. **Performance dashboard visualization** with real-time monitoring
3. **Debug visualization** with multiple visualization modes
4. **Export functionality** with multiple export formats
5. **Interactive features** with real-time updates and component processing
6. **Component management** with registration and unregistration
7. **Statistics tracking** with performance monitoring
8. **Integration** with NLM components
9. **Command-line interface** with comprehensive control options
10. **Best practices** for optimal visualization usage
11. **Troubleshooting** for common visualization issues

This system enables NLM users to visualize neural network data, performance metrics, and debugging information effectively while maintaining data integrity and providing flexibility for different use cases.