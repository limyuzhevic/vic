# NLM Neural Simulation - Real-Time Monitoring and Visualization System

## Overview

This implementation adds comprehensive real-time monitoring and visualization capabilities to the NLM neural simulation project. The system integrates seamlessly with the existing logging and performance monitoring infrastructure while providing advanced features for real-time metrics, neural activity visualization, historical data recording, and data export.

## System Architecture

### Core Components

1. **MetricsRecorder** (`src/performance/MetricsRecorder.hpp`/`cpp`)
   - Thread-safe historical data collection
   - Configurable recording intervals
   - Neural metrics recording (neurons, synapses, spikes, firing rates)
   - Performance metrics recording (CPU, memory, simulation metrics)
   - Spike metrics recording
   - Statistics computation (mean, stddev, min, max)

2. **MonitoringManager** (`src/performance/MonitoringManager.hpp`/`cpp`)
   - Thread-safe real-time metrics dashboard
   - Configurable monitoring granularity
   - Multiple output formats (console, file, notifications)
   - Integration with performance monitoring systems
   - Real-time metrics streaming

3. **VisualizationEngine** (`src/visualization/VisualizationEngine.hpp`/`cpp`)
   - Neural activity visualization
   - Network graphs and connectivity maps
   - Activity heatmaps and spike raster plots
   - Weight matrix visualization
   - Configurable visualization modes (3D, 2D, web-based)

4. **ExportManager** (`src/export/ExportManager.hpp`/`cpp`)
   - Export utilities for multiple formats (CSV, JSON, XML, PNG, JPG, SVG, PDF, HTML)
   - Batch export functionality
   - Compression and encryption support
   - Task management and progress tracking

5. **WebInterface** (`src/web/WebInterface.hpp`/`cpp`)
   - Web-based monitoring dashboard
   - REST API for real-time data access
   - Server-Sent Events (SSE) for real-time updates
   - Static file serving
   - WebSocket support for bidirectional communication

6. **Integration Framework** (`src/integration/BrainMonitor.hpp`/`cpp`)
   - Brain integration with monitoring system
   - Automatic data collection and recording
   - Visualization integration with neural data
   - Export integration with simulation results

### Integration with Existing Systems

#### Enhanced Brain::logStatus()
The `Brain::logStatus()` method has been enhanced to integrate with the monitoring system:

```cpp
void Brain::logStatus() const {
    // Original logging functionality...
    
    // Integration with monitoring system
    nlm::MetricsRecorder::getInstance().recordNeuralMetrics(const_cast<Brain&>(*this), pImpl->currentTime, pImpl->currentStep);
    
    // Record performance metrics if available
    if (auto* optimizedBrain = dynamic_cast<OptimizedBrain*>(this)) {
        nlm::MetricsRecorder::getInstance().recordPerformanceMetrics(optimizedBrain);
    }
    
    // Record spike metrics if spike system available
    if (pImpl->spikeSystem) {
        const auto& spikes = pImpl->spikeSystem->getSpikeHistory();
        nlm::MetricsRecorder::getInstance().recordSpikeMetrics(spikes);
    }
    
    // Update visualization with current brain state
    if (nlm::VisualizationEngine::getInstance().isRunning()) {
        nlm::VisualizationEngine::updateGlobal(*this, pImpl->currentTime, pImpl->currentStep);
    }
    
    // Update monitoring system
    if (nlm::MonitoringManager::getInstance().isRunning()) {
        nlm::MonitoringManager::updateGlobal(*this, pImpl->currentTime, pImpl->currentStep);
    }
}
```

#### Main Integration Header
`nlm_monitoring.hpp` provides a unified interface for the complete monitoring system:

```cpp
// Initialize the complete monitoring and visualization system
bool initializeMonitoringSystem(const MonitoringConfig& monitoringConfig = MonitoringConfig(),
                                const VisualizationConfig& visualizationConfig = VisualizationConfig(),
                                const ExportConfig& exportConfig = ExportConfig(),
                                const WebInterfaceConfig& webConfig = WebInterfaceConfig());

// Start/stop the monitoring system
void startMonitoringSystem();
void stopMonitoringSystem();

// Update the system with Brain data
void updateMonitoringSystem(const Brain& brain, Timestamp currentTime, SimulationStep currentStep);

// Export data
bool exportData(const std::string& filename, const std::string& format);

// Get system status
std::string getSystemStatus();
```

## Implementation Details

### Thread Safety

All components are designed with thread safety in mind:
- `std::shared_mutex` for read-heavy operations
- `std::mutex` for write-heavy operations
- Atomic flags for running state management
- Queue-based event systems for thread communication

### Performance Considerations

- Efficient data structures (vectors, maps, unordered maps)
- Configurable recording intervals to balance performance and data granularity
- Lazy evaluation for expensive computations
- Memory management with RAII patterns
- Non-blocking I/O operations

### Configuration Management

Each component uses a configuration structure for flexible customization:
- `MonitoringConfig`: Real-time metrics, performance monitoring, history tracking
- `VisualizationConfig`: Visualization types, output formats, rendering options
- `ExportConfig`: Output directories, formats, compression settings
- `WebInterfaceConfig`: Network settings, authentication, API endpoints
- `IntegrationConfig`: Integration options, notification settings, export behavior

### Data Flow

1. **Data Collection**: Brain's `step()` method collects neural data
2. **Metrics Recording**: Data is recorded by MetricsRecorder for historical analysis
3. **Real-time Updates**: MonitoringManager processes and distributes real-time metrics
4. **Visualization**: VisualizationEngine renders neural activity and network graphs
5. **Export**: ExportManager saves data in various formats
6. **Web Interface**: WebInterface provides web-based access to monitoring data

## Usage Examples

### Basic Usage

```cpp
#include "nlm_monitoring.hpp"

int main() {
    // Configure monitoring components
    nlm::MonitoringConfig monitoringConfig;
    monitoringConfig.enableRealTimeMetrics = true;
    monitoringConfig.enableConsoleOutput = true;
    
    nlm::VisualizationConfig visualizationConfig;
    visualizationConfig.enableVisualization = true;
    visualizationConfig.enableNetworkVisualization = true;
    
    nlm::ExportConfig exportConfig;
    exportConfig.outputDirectory = "./simulation_exports";
    
    nlm::WebInterfaceConfig webConfig;
    webConfig.enableWebInterface = true;
    webConfig.port = 8080;
    
    // Initialize the system
    if (!nlm::initializeMonitoringSystem(monitoringConfig, visualizationConfig, exportConfig, webConfig)) {
        std::cerr << "Failed to initialize monitoring system" << std::endl;
        return 1;
    }
    
    // Start the system
    nlm::startMonitoringSystem();
    
    // Run your neural simulation...
    // The monitoring system will automatically collect and process data
    
    // Export results when done
    nlm::exportData("simulation_results.csv", "csv");
    
    // Stop the system
    nlm::stopMonitoringSystem();
    
    return 0;
}
```

### Integration with Brain

```cpp
#include "Brain.hpp"
#include "nlm_monitoring.hpp"

// Create and initialize brain
auto config = std::make_shared<nlm::Config>();
config->set("neuron_count", 1000);
config->set("region_count", 2);

auto brain = std::make_shared<nlm::Brain>(config);
brain->initialize();

// Start monitoring system
nlm::startMonitoringSystem();

// Run simulation
for (int step = 0; step < 1000; ++step) {
    brain->step(step);
    nlm::updateMonitoringSystem(*brain, static_cast<nlm::Timestamp>(step) * 0.001, step);
}

// Export and stop
nlm::exportData("brain_simulation.csv", "csv");
nlm::stopMonitoringSystem();
```

### Web Dashboard Access

Once the web interface is enabled, you can access the monitoring dashboard at:
- **Local**: `http://localhost:8080`
- **Dashboard**: `http://localhost:8080/dashboard`
- **API Documentation**: `http://localhost:8080/api-docs`

The web interface provides:
- Real-time metrics display
- Neural network visualizations
- Export functionality
- System status monitoring
- Historical data access

## Build Instructions

### Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- nlohmann/json library (included as single-header)
- OpenSSL (for encryption support)
- Zlib (for compression support)

### Build Steps

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_WEB_INTERFACE=ON -DENABLE_EXPORT=ON

# Build
make -j$(nproc)

# Run tests (if Catch2 is configured)
./test_monitoring_system

# Run example
./nlm_monitoring_example
```

### Test Suite

Run the comprehensive test suite:

```bash
# Build and run all tests
cmake --build . --target test

# Or run individual tests
./test_monitoring_system
```

## Configuration Options

### Monitoring Configuration

```cpp
struct MonitoringConfig {
    std::chrono::milliseconds updateInterval = std::chrono::milliseconds(100); // 10 Hz
    bool enableRealTimeMetrics = true;
    bool enablePerformanceMonitoring = true;
    bool enableHistoryTracking = true;
    bool enableConsoleOutput = true;
    bool enableFileOutput = false;
    std::string outputFile;
    bool enableLogging = true;
    nlm::LogLevel minimumLogLevel = nlm::LogLevel::Info;
    bool enableNotifications = false;
    std::function<void(const std::string&)> notificationCallback;
};
```

### Visualization Configuration

```cpp
struct VisualizationConfig {
    bool enableVisualization = false;
    std::chrono::milliseconds updateInterval = std::chrono::milliseconds(100);
    bool enableNetworkVisualization = true;
    bool enableActivityVisualization = true;
    bool enableSpikeRaster = true;
    bool enableWeightMatrix = true;
    bool enableConnectivityMaps = true;
    bool enableActivityHeatmaps = true;
    double windowWidth = 1920.0;
    double windowHeight = 1080.0;
    std::string backend = "default";
    bool enable3D = false;
};
```

### Export Configuration

```cpp
struct ExportConfig {
    std::string outputDirectory = "./exports";
    bool autoCreateDirectory = true;
    bool overwriteExisting = false;
    std::chrono::milliseconds exportInterval = std::chrono::milliseconds(60000);
    bool enableCompression = false;
    bool enableEncryption = false;
    std::string encryptionKey;
    std::vector<std::string> preferredFormats = {"csv", "json", "png"};
    bool enableProgressTracking = true;
    std::function<void(double)> progressCallback;
};
```

### Web Interface Configuration

```cpp
struct WebInterfaceConfig {
    bool enableWebInterface = false;
    std::string host = "localhost";
    int port = 8080;
    std::string basePath = "/";
    std::string staticPath = "./web/static";
    std::string templatePath = "./web/templates";
    bool enableSSE = true;
    bool enableWebSocket = false;
    bool enableCORS = true;
    std::vector<std::string> allowedOrigins = {"*"};
    std::chrono::milliseconds requestTimeout = std::chrono::milliseconds(30000);
    size_t maxConcurrentConnections = 100;
    std::string sslCertificate = "";
    std::string sslKey = "";
    bool enableAuthentication = false;
    std::string authUsername = "admin";
    std::string authPasswordHash = "";
    std::string apiKey = "";
    bool enableLogging = true;
    std::string logLevel = "info";
};
```

## Performance and Scalability

### Memory Management

- Efficient data structures with configurable memory limits
- Automatic cleanup of old data
- Configurable export directory size management
- Memory pools for frequently allocated objects

### CPU Optimization

- Thread-safe operations with minimal locking
- Lazy evaluation of expensive computations
- Batch processing for export operations
- Real-time updates with configurable intervals

### Scalability

- Horizontal scaling potential through distributed monitoring
- Configurable processing for different simulation sizes
- Modular design allows for component-level scaling
- Support for multi-GPU visualization backends

## Extensibility

### Custom Metrics

Add custom metrics to the monitoring system:

```cpp
// Custom metric definition
struct CustomMetric {
    std::string name;
    std::string description;
    std::function<double()> getValue;
};

// Register with monitoring system
nlm::MonitoringManager::getInstance().registerMetric(
    nlm::MetricDefinition("custom_metric", "My custom metric", "units",
                         []() { return myMetricFunction(); },
                         std::chrono::milliseconds(1000)));
```

### Custom Exporters

Add custom export formats:

```cpp
// Custom export handler
void customExportHandler(const std::string& filename, const std::string& data) {
    // Custom export implementation
}

// Register with export manager
nlm::ExportManager::getInstance().registerFormat(
    nlm::ExportFormatInfo{"CUSTOM", "custom", "application/x-custom", false, false, true, 1.0, "Custom format"});
```

### Custom Visualizations

Add custom visualization types:

```cpp
// Custom visualization
void customVisualization(const nlm::Brain& brain) {
    // Custom visualization implementation
}

// Register with visualization engine
nlm::VisualizationEngine::getInstance().registerVisualization(
    nlm::VisualizationType{"CUSTOM", "My Custom Visualization", customVisualization});
```

## Testing and Validation

The system includes comprehensive tests covering:

### Unit Tests
- MetricsRecorder initialization and basic operations
- MonitoringManager configuration and real-time updates
- VisualizationEngine rendering and export
- ExportManager batch processing and task management
- WebInterface API endpoints and static file serving

### Integration Tests
- Complete system integration with Brain
- Data flow between components
- Performance under load
- Export functionality with various formats
- WebInterface API interactions

### Performance Tests
- Memory usage profiling
- CPU utilization analysis
- Export performance benchmarking
- Real-time update latency

## Future Enhancements

### Planned Features

1. **Machine Learning Integration**
   - Anomaly detection in neural activity
   - Pattern recognition in spike trains
   - Predictive modeling of neural dynamics

2. **Advanced Visualization**
   - 3D neural network rendering
   - Real-time graph analysis
   - Virtual reality support

3. **Distributed Monitoring**
   - Multi-node simulation monitoring
   - Cloud-based visualization
   - Edge computing support

4. **AI-Powered Analytics**
   - Automatic metric discovery
   - Pattern recognition in exported data
   - Predictive performance optimization

### Extension Points

The modular architecture allows for easy extension:
- Plugin system for custom components
- Configuration file support
- Scriptable export and visualization pipelines
- REST API extensions

## Conclusion

The NLM Neural Simulation monitoring and visualization system provides a comprehensive solution for real-time neural network monitoring, visualization, and data export. The system integrates seamlessly with the existing codebase while providing advanced features for scientific analysis and visualization. The modular, thread-safe architecture ensures scalability and performance for large-scale neural simulations.

Key benefits:
- **Real-time monitoring** of neural network activity
- **Comprehensive visualization** of neural dynamics
- **Historical data recording** for analysis
- **Flexible export** to multiple formats
- **Web-based dashboard** for remote access
- **Thread-safe operations** for multi-core simulations
- **Configurable granularity** for different use cases
- **Extensible architecture** for custom components

This implementation provides a solid foundation for advanced neural simulation analysis and monitoring, enabling researchers and developers to gain deeper insights into neural network dynamics and behavior.
