# VisualizationTools Configuration Example

This document provides examples and best practices for configuring and using VisualizationTools with the NLM system.

## Configuration Files

### NLM Configuration File
```xml
<configuration>
    <visualization enabled="true">
        <outputDirectory>./visualization_output</outputDirectory>
        <updateRate>60.0</updateRate>
        <maxHistorySize>10000</maxHistorySize>
        <enable3D>true</enable3D>
        <enableVR>true</enableVR>
        <enableWebGL>true</enableWebGL>
        <enableAnimations>true</enableAnimations>
        <threadCount>4</threadCount>
        <memoryLimit>1073741824</memoryLimit>
    </visualization>
</configuration>
```

### JSON Configuration File
```json
{
    "visualization": {
        "enabled": true,
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
}
```

### YAML Configuration File
```yaml
visualization:
  enabled: true
  outputDirectory: ./visualization_output
  updateRate: 60.0
  maxHistorySize: 10000
  compressionLevel: 6
  enableMultiThreading: true
  threadCount: 0
  enableResourceOptimization: true
  memoryLimit: 1073741824
  enableVR: false
  enable3D: false
  enableWebGL: true
  enableAnimations: false
  customSettings: {}
```

## Integration Examples

### Example 1: Basic Integration
```cpp
// Include VisualizationTools header
#include "visualization/VisualizationTools.hpp"
#include "visualization/VisualizationTools_config.hpp"

// In main application or initialization function
int main() {
    // Create visualization tools
    auto vizTools = std::make_unique<nlm::VisualizationTools>();
    
    // Configure with custom settings
    nlm::VisualizationToolsConfig config;
    config.outputDirectory = "./my_visualization_output";
    config.enableWebGL = true;
    config.enable3D = true;
    
    // Initialize visualization
    if (!vizTools->initialize(config.outputDirectory)) {
        std::cerr << "Failed to initialize visualization tools!" << std::endl;
        return 1;
    }
    
    // Configure additional settings
    vizTools->setUpdateRate(30.0);
    vizTools->setMaxHistorySize(5000);
    vizTools->setCompressionLevel(5);
    
    return 0;
}
```

### Example 2: Real-Time Visualization
```cpp
// Include necessary headers
#include "visualization/VisualizationTools.hpp"
#include "brain/Brain.hpp"

void setupVisualization(nlm::VisualizationTools& vizTools, const nlm::Brain& brain) {
    // Configure for real-time updates
    nlm::DashboardConfig dashboardConfig;
    dashboardConfig.title = "Real-Time Neural Network Visualization";
    dashboardConfig.widgets = {
        "Network Graph",
        "Activity Timeline",
        "Spike Raster Plot",
        "Population Statistics"
    };
    dashboardConfig.enableRealTimeUpdates = true;
    dashboardConfig.updateInterval = 1000.0; // Update every second
    
    // Create interactive dashboard
    vizTools.generateInteractiveDashboard("real_time_dashboard.html", dashboardConfig);
    
    // Set up callbacks for real-time updates
    vizTools.setProgressCallback([](double progress) {
        if (progress > 0.0 && progress < 1.0) {
            // Update progress bar in UI
        }
    });
    
    vizTools.setErrorCallback([](const std::string& error) {
        // Log errors
    });
}

void updateVisualization(nlm::VisualizationTools& vizTools, const nlm::Brain& brain, double deltaTime) {
    // Update visualization state
    vizTools.update(deltaTime);
    
    // Process new brain data
    vizTools.processNeuralActivity(brain);
    vizTools.processSpikeData(brain.getSpikeSystem()->getCurrentSpikes());
    vizTools.processPopulationStats(brain);
    
    // Generate real-time visualization
    vizTools.generateVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "real_time_activity");
}
```

### Example 3: 3D Neural Network Visualization
```cpp
// Include necessary headers
#include "visualization/VisualizationTools.hpp"
#include "brain/Brain.hpp"

void create3DVisualization(nlm::VisualizationTools& vizTools, const nlm::Brain& brain) {
    // Configure 3D scene
    nlm::SceneConfig sceneConfig;
    sceneConfig.name = "3D Neural Network Visualization";
    sceneConfig.cameraDistance = 100.0f;
    sceneConfig.fieldOfView = 75.0f;
    sceneConfig.backgroundColor = "#000000";
    sceneConfig.lightingMode = "phong";
    sceneConfig.enableShadows = true;
    sceneConfig.enableAntialiasing = true;
    
    // Create 3D visualizer
    auto vizTools3D = nlm::VisualizationTools::create3DVisualizer(sceneConfig);
    vizTools3D->initialize("./3d_visualization");
    
    // Generate 3D visualization
    vizTools3D->generate3DVisualization(brain, "neural_network_3d.html", sceneConfig);
}
```

### Example 4: VR Neural Network Visualization
```cpp
// Include necessary headers
#include "visualization/VisualizationTools.hpp"
#include "brain/Brain.hpp"

void createVRVisualization(nlm::VisualizationTools& vizTools, const nlm::Brain& brain) {
    // Configure VR system
    nlm::VRConfig vrConfig;
    vrConfig.trackingSystem = "head-mounted-display";
    vrConfig.displayMode = "stereoscopic";
    vrConfig.enablePhysics = true;
    vrConfig.enableInteractions = true;
    vrConfig.interactionMode = "hand-tracking";
    
    // Create VR visualizer
    auto vizToolsVR = nlm::VisualizationTools::createVRVisualizer(vrConfig);
    vizToolsVR->initialize("./vr_visualization");
    
    // Generate VR scene
    vizToolsVR->generateVRScene(brain, "neural_network_vr.vrscene", vrConfig);
}
```

### Example 5: Batch Processing for Performance
```cpp
// Include necessary headers
#include "visualization/VisualizationTools.hpp"
#include "brain/Brain.hpp"
#include <vector>

void batchVisualizationProcessing(nlm::VisualizationTools& vizTools, const nlm::Brain& brain) {
    // Create visualization tasks
    std::vector<std::pair<nlm::VisualizationType, std::string>> tasks;
    
    // Add various visualization types
    tasks.push_back({nlm::VisualizationType::NETWORK_GRAPH, "network_graph"});
    tasks.push_back({nlm::VisualizationType::NEURON_ACTIVITY, "neuron_activity"});
    tasks.push_back({nlm::VisualizationType::SPIKE_RASTER, "spike_raster"});
    tasks.push_back({nlm::VisualizationType::WEIGHT_MATRIX, "weight_matrix"});
    tasks.push_back({nlm::VisualizationType::POPULATION_STATISTICS, "population_stats"});
    tasks.push_back({nlm::VisualizationType::3D_NEURAL_NETWORK, "neural_network_3d"});
    tasks.push_back({nlm::VisualizationType::TIME_SERIES, "time_series"});
    tasks.push_back({nlm::VisualizationType::INTERACTIVE_DASHBOARD, "interactive_dashboard"});
    tasks.push_back({nlm::VisualizationType::VR_SCENE, "vr_scene"});
    
    // Start batch processing
    vizTools.startBatchMode(tasks.size() / 2); // Process 2 at a time
    
    // Process tasks in batch
    for (const auto& task : tasks) {
        vizTools.exportVisualization(task.first, task.second + "_batch", nlm::VisualizationFormat::HTML);
    }
    
    // End batch processing
    vizTools.endBatchMode();
}
```

### Example 6: Configuration Loading
```cpp
// Include necessary headers
#include "visualization/VisualizationTools.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

nlm::VisualizationToolsConfig loadVisualizationConfig(const std::string& configFile) {
    nlm::VisualizationToolsConfig config;
    
    // Parse JSON configuration file
    std::ifstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + configFile);
    }
    
    json j;
    file >> j;
    
    // Load configuration values
    if (j.contains("outputDirectory")) {
        config.outputDirectory = j["outputDirectory"];
    }
    
    if (j.contains("updateRate")) {
        config.updateRate = j["updateRate"];
    }
    
    if (j.contains("maxHistorySize")) {
        config.maxHistorySize = j["maxHistorySize"];
    }
    
    if (j.contains("compressionLevel")) {
        config.compressionLevel = j["compressionLevel"];
    }
    
    if (j.contains("enableMultiThreading")) {
        config.enableMultiThreading = j["enableMultiThreading"];
    }
    
    if (j.contains("threadCount")) {
        config.threadCount = j["threadCount"];
    }
    
    if (j.contains("enableResourceOptimization")) {
        config.enableResourceOptimization = j["enableResourceOptimization"];
    }
    
    if (j.contains("memoryLimit")) {
        config.memoryLimit = j["memoryLimit"];
    }
    
    if (j.contains("enableVR")) {
        config.enableVR = j["enableVR"];
    }
    
    if (j.contains("enable3D")) {
        config.enable3D = j["enable3D"];
    }
    
    if (j.contains("enableWebGL")) {
        config.enableWebGL = j["enableWebGL"];
    }
    
    if (j.contains("enableAnimations")) {
        config.enableAnimations = j["enableAnimations"];
    }
    
    if (j.contains("customSettings")) {
        for (auto it = j["customSettings"].begin(); it != j["customSettings"].end(); ++it) {
            config.customSettings[it.key()] = it.value();
        }
    }
    
    return config;
}

void saveVisualizationConfig(const nlm::VisualizationToolsConfig& config, const std::string& configFile) {
    // Convert configuration to JSON
    json j;
    
    j["outputDirectory"] = config.outputDirectory;
    j["updateRate"] = config.updateRate;
    j["maxHistorySize"] = config.maxHistorySize;
    j["compressionLevel"] = config.compressionLevel;
    j["enableMultiThreading"] = config.enableMultiThreading;
    j["threadCount"] = config.threadCount;
    j["enableResourceOptimization"] = config.enableResourceOptimization;
    j["memoryLimit"] = config.memoryLimit;
    j["enableVR"] = config.enableVR;
    j["enable3D"] = config.enable3D;
    j["enableWebGL"] = config.enableWebGL;
    j["enableAnimations"] = config.enableAnimations;
    
    for (const auto& pair : config.customSettings) {
        j["customSettings"][pair.first] = pair.second;
    }
    
    // Write to file
    std::ofstream file(configFile);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write configuration file: " + configFile);
    }
    
    file << j.dump(4);
}
```

### Example 7: Performance Monitoring
```cpp
// Include necessary headers
#include "visualization/VisualizationTools.hpp"
#include <iostream>
#include <iomanip>

void monitorVisualizationPerformance(nlm::VisualizationTools& vizTools) {
    // Set up performance monitoring
    auto progressCallback = [](double progress) {
        std::cout << "Progress: " << std::fixed << std::setprecision(2) << (progress * 100) << "%\n";
    };
    
    auto errorCallback = [](const std::string& error) {
        std::cerr << "Error: " << error << std::endl;
    };
    
    vizTools.setProgressCallback(progressCallback);
    vizTools.setErrorCallback(errorCallback);
    
    // Enable performance optimization
    vizTools.enableResourceOptimization(true);
    vizTools.setMemoryLimit(512 * 1024 * 1024); // 512MB
    
    // Perform operations
    vizTools.setUpdateRate(30.0);
    vizTools.setMaxHistorySize(10000);
    
    // Generate some visualizations
    vizTools.generateVisualization(nlm::VisualizationType::NETWORK_GRAPH, "perf_test_1");
    vizTools.generateVisualization(nlm::VisualizationType::NEURON_ACTIVITY, "perf_test_2");
    vizTools.generateVisualization(nlm::VisualizationType::SPIKE_RASTER, "perf_test_3");
    
    // Print performance statistics
    std::cout << "=== Performance Statistics ===\n";
    std::cout << "FPS: " << std::fixed << std::setprecision(2) << vizTools.getCurrentFPS() << "\n";
    std::cout << "Data Size: " << vizTools.getDataSize() << " bytes\n";
    
    vizTools.printPerformanceStats();
}
```

## Best Practices

### 1. Configuration Management
- Use configuration files instead of hard-coding settings
- Validate configuration before initialization
- Save configuration after modifications

### 2. Resource Management
- Monitor memory usage and set appropriate limits
- Use batch processing for multiple visualizations
- Enable resource optimization for production environments

### 3. Error Handling
- Implement comprehensive error handling
- Use callback functions for error reporting
- Validate inputs before processing

### 4. Performance Optimization
- Use multi-threading for CPU-intensive operations
- Optimize for speed or quality based on requirements
- Monitor and optimize performance regularly

### 5. Data Management
- Clear data regularly to prevent memory leaks
- Use efficient data structures
- Implement compression for large datasets

### 6. Integration with NLM
- Process brain data regularly
- Use SpikeSystem for spike data
- Use NeuralPopulation for population statistics
- Update visualization state with current simulation data

## Common Issues and Solutions

### Issue 1: Performance Bottlenecks
**Solution:**
- Enable multi-threading
- Use batch processing
- Optimize for speed
- Monitor memory usage

### Issue 2: Memory Exhaustion
**Solution:**
- Set memory limits
- Enable resource optimization
- Clear data regularly
- Use compression

### Issue 3: Visualization Not Updating
**Solution:**
- Check update rate settings
- Verify data processing
- Ensure rendering is enabled
- Check error messages

### Issue 4: File Write Errors
**Solution:**
- Verify file permissions
- Check disk space
- Use unique filenames
- Handle exceptions properly

## Migration Guide

### From Version 1.x to 2.x
- Configuration structure has changed (new JSON format)
- Added new visualization types (3D, VR, WebGL)
- Improved performance with multi-threading
- Enhanced error handling with callbacks

### From Manual to Automated
- Use configuration files instead of hard-coding
- Implement batch processing for multiple visualizations
- Use callback functions for progress and error handling
- Monitor performance regularly

## Future Enhancements

### Planned Features
1. **Real-time Streaming**: Stream visualization data directly from NLM
2. **Cloud Integration**: Cloud-based rendering and storage
3. **Advanced Analytics**: Machine learning-based visualization insights
4. **Custom Visualizations**: User-defined visualization types
5. **Collaboration**: Multi-user visualization sharing

### Extension Points
1. **New Visualization Types**: Add new visualization types easily
2. **New Export Formats**: Support additional export formats
3. **Custom Callbacks**: Add custom callback functions
4. **Configuration Plugins**: Add configuration plugins

## Conclusion

VisualizationTools provides a comprehensive solution for visualizing NLM neural network simulations. The modular design allows for easy extension and customization while maintaining high performance and reliability. By following the best practices and examples provided in this document, you can effectively integrate VisualizationTools into your NLM applications.