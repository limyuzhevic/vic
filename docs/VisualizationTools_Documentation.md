# VisualizationTools Documentation

## Overview

The VisualizationTools class provides comprehensive visualization capabilities for the NLM (熙然) neural network simulation system. It integrates with the existing visualization system and provides advanced features for multiple visualization types, interactive dashboards, 3D visualizations, animation support, report generation, and VR compatibility.

## Design Principles

### Core Architecture

The VisualizationTools class follows a modular design with the following key components:

1. **Data Management Layer**: Handles visualization data collection, storage, and processing
2. **Export Layer**: Manages export to various formats (HTML, JSON, CSV, VR, WebGL, etc.)
3. **Rendering Layer**: Provides both static and animated rendering capabilities
4. **Configuration Layer**: Handles all visualization configuration options
5. **Thread Management**: Supports multi-threaded processing for performance

### Key Features

- **Multiple Visualization Types**: Network graphs, neuron activity, spike rasters, weight matrices, population statistics
- **Interactive Dashboard**: Web-based dashboard with real-time updates
- **3D Visualization**: Advanced 3D neural network rendering
- **Animation Support**: Time series and keyframe-based animations
- **Report Generation**: Comprehensive reports with visualizations
- **VR Compatibility**: Virtual reality scene generation

## Class Design

### Class: `nlm::VisualizationTools`

**Header**: `src/visualization/VisualizationTools.hpp`
**Implementation**: `src/visualization/VisualizationTools.cpp`

#### Public Methods

**Constructors/Destructors:**
- `VisualizationTools()`: Default constructor
- `~VisualizationTools()`: Destructor

**Core Functionality:**
- `initialize(const std::string& outputDirectory = ".")`: Initialize visualization system
- `update(double deltaTime)`: Update visualization state
- `render()`: Render current visualization
- `close()`: Close visualization system

**Configuration Methods:**
- `setOutputDirectory(const std::string& path)`: Set output directory
- `setUpdateRate(double hz)`: Set update rate
- `setMaxHistorySize(size_t maxPoints)`: Set maximum history size
- `setCompressionLevel(int level)`: Set compression level

**Visualization Generation:**
- `generateVisualization(VisualizationType type, const std::string& filename)`: Generate basic visualization
- `generate3DVisualization(const Brain& brain, const std::string& filename, const SceneConfig& config)`: Generate 3D visualization
- `generateVRScene(const Brain& brain, const std::string& filename, const VRConfig& config)`: Generate VR scene
- `generateInteractiveDashboard(const std::string& filename, const DashboardConfig& config)`: Generate interactive dashboard
- `generateAnimation(const std::vector<Timestamp>& keyframes, const std::string& filename, const AnimationParams& params)`: Generate animation
- `generateTimeSeriesAnimation(const Brain& brain, const std::string& filename, const AnimationParams& params)`: Generate time series animation
- `generateReport(const std::string& filename, const ReportParams& params)`: Generate report
- `generateComprehensiveReport(const Brain& brain, const std::string& filename)`: Generate comprehensive report

**Export Methods:**
- `exportVisualization(VisualizationType type, const std::string& filename, VisualizationFormat format)`: Export visualization
- `exportToHTML(const Brain& brain, const std::string& filename)`: Export to HTML
- `exportToJSON(const Brain& brain, const std::string& filename)`: Export to JSON
- `exportToCSV(const Brain& brain, const std::string& filename)`: Export to CSV
- `exportToVR(const Brain& brain, const std::string& filename)`: Export to VR
- `generateWebGLVisualization(const Brain& brain, const std::string& filename)`: Generate WebGL visualization
- `generateThreeJSVisualization(const Brain& brain, const std::string& filename)`: Generate Three.js visualization
- `generateGLTFSnapshot(const Brain& brain, const std::string& filename)`: Generate GLTF snapshot

**State Management:**
- `setVisualizationData(const VisualizationData& data)`: Set visualization data
- `getVisualizationData() const`: Get visualization data
- `clearData()`: Clear all data
- `resetStatistics()`: Reset statistics

**File Management:**
- `getAvailableFiles() const`: Get available files
- `exportData(const std::string& filename)`: Export data
- `importData(const std::string& filename)`: Import data

**Utility Methods:**
- `getVisualizationTypeName(VisualizationType type)`: Get visualization type name
- `getFormatExtension(VisualizationFormat format)`: Get format extension
- `getSupportedFormats(VisualizationType type)`: Get supported formats

**Status Methods:**
- `isInitialized() const`: Check if initialized
- `isRendering() const`: Check if rendering
- `getCurrentFPS() const`: Get current FPS
- `getDataSize() const`: Get data size

**Batch Processing:**
- `startBatchMode(size_t batchSize = 10)`: Start batch mode
- `endBatchMode()`: End batch mode
- `isInBatchMode() const`: Check if in batch mode

**Callback Methods:**
- `setProgressCallback(std::function<void(double)> callback)`: Set progress callback
- `setErrorCallback(std::function<void(const std::string&)> callback)`: Set error callback

**Threading:**
- `enableMultiThreading(bool enable)`: Enable/disable multi-threading
- `setThreadCount(size_t count)`: Set thread count

**Resource Management:**
- `enableResourceOptimization(bool enable)`: Enable/disable resource optimization
- `setMemoryLimit(size_t bytes)`: Set memory limit

**Internal Processing:**
- `processNetworkTopology(const Brain& brain)`: Process network topology
- `processNeuralActivity(const Brain& brain)`: Process neural activity
- `processSpikeData(const std::vector<SpikeEvent>& spikes)`: Process spike data
- `processPopulationStats(const Brain& brain)`: Process population statistics

**Data Aggregation:**
- `aggregateVisualizationData(const Brain& brain) const`: Aggregate visualization data
- `validateVisualizationData(const VisualizationData& data) const`: Validate visualization data

**Performance Optimization:**
- `optimizeForSpeed()`: Optimize for speed
- `optimizeForQuality()`: Optimize for quality

**Statistics:**
- `recordVisualizationTime(VisualizationType type, double timeMs)`: Record visualization time
- `getAverageVisualizationTime(VisualizationType type) const`: Get average visualization time
- `printPerformanceStats() const`: Print performance statistics

**Configuration Validation:**
- `validateSceneConfig(const SceneConfig& config)`: Validate scene configuration
- `validateVRConfig(const VRConfig& config)`: Validate VR configuration
- `validateDashboardConfig(const DashboardConfig& config)`: Validate dashboard configuration

**Factory Methods:**
- `createInteractiveDashboard(const std::string& title)`: Create interactive dashboard
- `create3DVisualizer(const SceneConfig& config)`: Create 3D visualizer
- `createVRVisualizer(const VRConfig& config)`: Create VR visualizer

#### Private Methods

**Helper Methods:**
- `initializeOutputDirectory()`: Initialize output directory
- `generateUniqueFilename(const std::string& baseName, VisualizationFormat format) const`: Generate unique filename
- `validateFilename(const std::string& filename) const`: Validate filename
- `updateProgress(double progress)`: Update progress
- `handleError(const std::string& error)`: Handle error
- `cleanupTemporaryFiles()`: Cleanup temporary files
- `saveVisualizationData(const VisualizationData& data, const std::string& filename)`: Save visualization data
- `loadVisualizationData(const std::string& filename)`: Load visualization data

**Threading Helpers:**
- `processBatchAsync(const std::vector<std::pair<VisualizationType, std::string>>& tasks)`: Process batch asynchronously
- `renderFrame()`: Render frame

**Export Helpers:**
- `exportToHTMLInternal(const Brain& brain, const std::string& filename, const DashboardConfig& config)`: Internal HTML export
- `exportToJSONInternal(const Brain& brain, const std::string& filename)`: Internal JSON export
- `exportToCSVInternal(const Brain& brain, const std::string& filename)`: Internal CSV export
- `exportToVRInternal(const Brain& brain, const std::string& filename)`: Internal VR export
- `exportToThreeJSInternal(const Brain& brain, const std::string& filename)`: Internal Three.js export

**Data Serialization:**
- `serializeVisualizationData(const VisualizationData& data, const std::string& filename) const`: Serialize visualization data
- `deserializeVisualizationData(VisualizationData& data, const std::string& filename) const`: Deserialize visualization data

**Configuration:**
- `loadConfiguration(const std::string& configFile)`: Load configuration
- `saveConfiguration(const std::string& configFile) const`: Save configuration

## Data Structures

### VisualizationData
Main data structure containing all visualization information:
- `NetworkTopology`: Network structure data
- `NeuralActivity`: Current neural activity
- `SpikeData`: Spike event data
- `PopulationStats`: Population statistics
- `TimeSeriesData`: Time series data
- `activityHistory`: Historical activity data

### VisualizationFormat
Output formats:
- `HTML`: HTML format
- `JSON`: JSON format
- `CSV`: CSV format
- `VR_SCENE`: VR scene format
- `WEBGL`: WebGL format
- `THREEJS`: Three.js format
- `GLTF`: GLTF format
- `STL`: STL format
- `OBJ`: OBJ format

### VisualizationType
Visualization types:
- `NETWORK_GRAPH`: Network graph visualization
- `NEURON_ACTIVITY`: Neuron activity visualization
- `SPIKE_RASTER`: Spike raster plot
- `WEIGHT_MATRIX`: Weight matrix visualization
- `POPULATION_STATISTICS`: Population statistics
- `3D_NEURAL_NETWORK`: 3D neural network visualization
- `TIME_SERIES`: Time series visualization
- `INTERACTIVE_DASHBOARD`: Interactive dashboard
- `VR_SCENE`: VR scene

### AnimationParams
Animation parameters:
- `duration`: Animation duration in seconds
- `frameRate`: Frame rate in Hz
- `loop`: Whether to loop animation
- `easing`: Easing function
- `interpolation`: Interpolation method
- `keyframes`: Keyframes
- `transitions`: Transitions

### ReportParams
Report parameters:
- `title`: Report title
- `description`: Report description
- `sections`: Report sections
- `includeRawData`: Whether to include raw data
- `includeStatistics`: Whether to include statistics
- `outputPath`: Output path
- `visualTypes`: Visual types to include
- `metadata`: Additional metadata

### SceneConfig
3D scene configuration:
- `name`: Scene name
- `cameraDistance`: Camera distance
- `fieldOfView`: Field of view
- `backgroundColor`: Background color
- `lightingMode`: Lighting mode
- `renderModes`: Render modes
- `enableShadows`: Whether to enable shadows
- `enableAntialiasing`: Whether to enable antialiasing

### VRConfig
VR configuration:
- `trackingSystem`: Tracking system
- `displayMode`: Display mode
- `enablePhysics`: Whether to enable physics
- `enableInteractions`: Whether to enable interactions
- `interactionMode`: Interaction mode
- `supportedDevices`: Supported devices

### DashboardConfig
Dashboard configuration:
- `title`: Dashboard title
- `widgets`: Dashboard widgets
- `enableRealTimeUpdates`: Whether to enable real-time updates
- `updateInterval`: Update interval in milliseconds
- `layout`: Dashboard layout

## Output Formats

### HTML Output
Generates interactive HTML dashboards with:
- Network graphs using D3.js
- Real-time updates
- Responsive design
- Interactive elements

### JSON Output
Generates structured JSON data with:
- Neural network topology
- Activity data
- Statistics
- Configuration

### CSV Output
Generates comma-separated values files with:
- Time series data
- Neuron activity data
- Spike data
- Population statistics

### WebGL/Three.js Output
Generates 3D visualizations with:
- 3D neural network rendering
- Interactive 3D scenes
- Real-time rendering
- Multiple visualization modes

### VR Output
Generates VR-compatible files with:
- Stereoscopic rendering
- Head tracking support
- Interaction capabilities
- Physics simulation

### GLTF Output
Generates GLTF files with:
- 3D model data
- Animation clips
- Scene hierarchy
- Material definitions

## Usage Examples

### Basic Usage
```cpp
#include "visualization/VisualizationTools.hpp"
#include <memory>

int main() {
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    // Initialize
    vizTools->initialize("./output");
    
    // Generate visualization
    vizTools->generateVisualization(nlm::VisualizationType::NETWORK_GRAPH, "network_graph");
    
    // Clean up
    vizTools->close();
    
    return 0;
}
```

### Interactive Dashboard
```cpp
#include "visualization/VisualizationTools.hpp"

int main() {
    // Create dashboard configuration
    nlm::DashboardConfig config;
    config.title = "Neural Network Dashboard";
    config.widgets = {"Network Graph", "Activity Timeline", "Spike Raster"};
    config.enableRealTimeUpdates = true;
    
    // Create interactive dashboard
    auto vizTools = nlm::VisualizationTools::createInteractiveDashboard(config.title);
    vizTools->initialize("./output");
    
    // Generate dashboard
    vizTools->generateInteractiveDashboard("dashboard.html", config);
    
    return 0;
}
```

### 3D Visualization
```cpp
#include "visualization/VisualizationTools.hpp"

int main() {
    // Create scene configuration
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "3D Neural Network";
    sceneConfig.cameraDistance = 50.0f;
    sceneConfig.backgroundColor = "#000000";
    sceneConfig.enableShadows = true;
    
    // Create 3D visualizer
    auto vizTools = nlm::VisualizationTools::create3DVisualizer(sceneConfig);
    vizTools->initialize("./output");
    
    // Generate 3D visualization
    vizTools->generate3DVisualization(brain, "3d_network.html", sceneConfig);
    
    return 0;
}
```

### VR Visualization
```cpp
#include "visualization/VisualizationTools.hpp"

int main() {
    // Create VR configuration
    nlm::VRConfig vrConfig;
    vrConfig.trackingSystem = "hmd";
    vrConfig.displayMode = "stereoscopic";
    vrConfig.enableInteractions = true;
    
    // Create VR visualizer
    auto vizTools = nlm::VisualizationTools::createVRVisualizer(vrConfig);
    vizTools->initialize("./output");
    
    // Generate VR scene
    vizTools->generateVRScene(brain, "vr_scene.vrscene", vrConfig);
    
    return 0;
}
```

### Animation
```cpp
#include "visualization/VisualizationTools.hpp"

int main() {
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    vizTools->initialize("./output");
    
    // Create animation parameters
    nlm::AnimationParams params;
    params.duration = 10.0;
    params.frameRate = 30.0;
    params.loop = true;
    
    // Generate animation
    vizTools->generateTimeSeriesAnimation(brain, "animation.html", params);
    
    return 0;
}
```

### Report Generation
```cpp
#include "visualization/VisualizationTools.hpp"

int main() {
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    vizTools->initialize("./output");
    
    // Generate comprehensive report
    vizTools->generateComprehensiveReport(brain, "report.html");
    
    return 0;
}
```

## Configuration

### Configuration File
VisualizationTools supports configuration through a JSON file:

```json
{
    "outputDirectory": "./visualization_output",
    "updateRate": 60.0,
    "maxHistorySize": 10000,
    "compressionLevel": 6,
    "enableMultiThreading": true,
    "threadCount": 0,
    "enableResourceOptimization": true,
    "memoryLimit": 1073741824,
    "enableVR": false,
    "enable3D": false,
    "enableWebGL": true,
    "enableAnimations": false,
    "customSettings": {}
}
```

### Environment Variables
- `NLM_VISUALIZATION_OUTPUT_DIR`: Output directory
- `NLM_VISUALIZATION_UPDATE_RATE`: Update rate (Hz)
- `NLM_VISUALIZATION_MAX_HISTORY`: Maximum history size

## Integration with NLM

### Data Sources
VisualizationTools integrates with NLM through:

1. **Brain Class**: Provides neural network topology and activity data
2. **SpikeSystem**: Provides spike event data
3. **NeuralPopulation**: Provides population statistics
4. **Plasticity Systems**: Provides weight matrix data
5. **Memory Systems**: Provides memory visualization data

### Data Processing
VisualizationTools processes NLM data through:

1. **Topology Processing**: Extracts network structure
2. **Activity Processing**: Extracts neural activity patterns
3. **Spike Processing**: Extracts spike event data
4. **Statistics Processing**: Extracts population and network statistics
5. **Time Series Processing**: Processes time-dependent data

### Export Formats
VisualizationTools exports data in various formats optimized for NLM analysis:

1. **HTML**: Interactive web-based dashboards
2. **JSON**: Structured data for analysis
3. **CSV**: Spreadsheet-compatible data
4. **VR**: Virtual reality experiences
5. **Three.js**: 3D web visualizations
6. **GLTF**: 3D model files

## Performance Considerations

### Memory Management
- Uses efficient data structures
- Supports compression
- Provides memory limits
- Supports resource optimization

### Multi-threading
- Supports multi-threaded processing
- Configurable thread count
- Thread-safe operations

### Resource Optimization
- Can optimize for speed or quality
- Supports batch processing
- Provides progress tracking

## Error Handling

VisualizationTools provides comprehensive error handling:

1. **Error Callbacks**: User-defined error handling
2. **Progress Tracking**: Progress reporting
3. **Validation**: Input validation
4. **Graceful Degradation**: Fallback mechanisms

## Future Enhancements

1. **Real-time Streaming**: Live streaming of visualization data
2. **Cloud Integration**: Cloud-based visualization rendering
3. **Advanced Analytics**: Machine learning-based visualization insights
4. **Custom Visualizations**: User-defined visualization types
5. **Export to Additional Formats**: More output format support
6. **Collaboration Features**: Multi-user visualization sharing

## Conclusion

The VisualizationTools class provides a comprehensive solution for visualizing NLM neural network simulations. It supports multiple visualization types, interactive dashboards, 3D visualizations, animations, reports, and VR experiences. The modular design allows for easy extension and customization while maintaining high performance and reliability.