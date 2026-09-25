# NLM Advanced Features Examples

## Overview

This directory contains comprehensive examples demonstrating the use of NLM's advanced features. These examples showcase the plugin system, advanced configuration, performance profiling, visualization, and memory management capabilities.

## Examples Directory Structure

```
NLM/
├── src/agent/Examples/
│   ├── README.md                          # Examples directory documentation
│   ├── plugin_example.cpp                 # Plugin development example
│   ├── advanced_config_example.cpp        # Advanced configuration example
│   ├── performance_profiling_example.cpp  # Performance profiling example
│   ├── visualization_example.cpp          # Visualization example
│   ├── memory_management_example.cpp      # Memory management example
│   └── Scripts/                           # Documentation generation script
│       └── documentation_generation.sh    # Script to generate documentation
```

## Building Examples

### Prerequisites

To build the examples, you need:

- CMake 3.16 or higher
- C++ compiler with C++20 support
- NLM development libraries
- Build tools (make, ninja, etc.)

### Build Instructions

```bash
# Navigate to the NLM directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_FEATURES=ON

# Build all examples
make -j$(nproc)

# Run specific example
./plugin_example
./advanced_config_example
./performance_profiling_example
./visualization_example
./memory_management_example
```

### Build Options

```bash
# Build with debug information
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++

# Install examples
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Examples

### 1. Plugin Development Example

**File:** `src/agent/Examples/plugin_example.cpp`

This example demonstrates how to develop and use custom plugins in NLM. It includes:

- Plugin class implementation
- Plugin registration
- Plugin lifecycle management
- Plugin configuration
- Plugin event handling

### 2. Advanced Configuration Example

**File:** `src/agent/Examples/advanced_config_example.cpp`

This example demonstrates advanced configuration management in NLM. It includes:

- Configuration templates
- Configuration inheritance
- Configuration validation
- Runtime configuration modification
- Configuration scripting

### 3. Performance Profiling Example

**File:** `src/agent/Examples/performance_profiling_example.cpp`

This example demonstrates performance profiling and optimization in NLM. It includes:

- Performance profiling sessions
- Performance regression detection
- Performance bottleneck identification
- Performance optimization recommendations
- Performance reporting

### 4. Visualization Example

**File:** `src/agent/Examples/visualization_example.cpp`

This example demonstrates visualization capabilities in NLM. It includes:

- Neural network visualization
- Performance dashboard visualization
- Debug visualization
- Export functionality
- Interactive visualization

### 5. Memory Management Example

**File:** `src/agent/Examples/memory_management_example.cpp`

This example demonstrates memory management capabilities in NLM. It includes:

- Custom memory pools
- Memory allocation tracking
- Memory leak detection
- Memory optimization
- Memory monitoring

## Documentation Generation Script

### Script Location

**File:** `src/agent/Examples/Scripts/documentation_generation.sh`

This script generates comprehensive documentation for NLM's advanced features. It includes:

- Documentation extraction from source code
- Template-based documentation generation
- Cross-referencing between features
- Automated documentation updates

### Usage

```bash
# Navigate to script directory
cd src/agent/Examples/Scripts

# Make script executable
chmod +x documentation_generation.sh

# Generate documentation
./documentation_generation.sh

# Generate specific documentation
./documentation_generation.sh --feature advanced_config
./documentation_generation.sh --feature plugin_system
```

### Script Features

1. **Documentation Extraction**: Extracts documentation from source code comments
2. **Template-Based Generation**: Uses templates to generate structured documentation
3. **Cross-Referencing**: Creates links between related features
4. **Automated Updates**: Automatically updates documentation when source code changes
5. **Multi-Format Support**: Generates documentation in multiple formats (Markdown, HTML, LaTeX)

## Example Integration

### Example 1: Complete Plugin Example

This example demonstrates a complete plugin that integrates with NLM:

```cpp
// Plugin header
#include "src/agent/PluginSystem.hpp"

// Plugin implementation
class AdvancedPlugin : public nlm::IPlugin {
public:
    // Plugin metadata
    std::string getName() const override { return "AdvancedPlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Advanced plugin for NLM"; }
    nlm::PluginType getType() const override { return nlm::PluginType::Custom; }
    
    // Lifecycle management
    bool initialize(const nlm::Config& config) override {
        // Initialize plugin resources
        return true;
    }
    
    void shutdown() override {
        // Clean up plugin resources
    }
    
    nlm::PluginState getState() const override {
        return state_;
    }
    
    // Event handling
    void onBrainUpdate(std::shared_ptr<nlm::Brain> brain) override {
        // Process brain updates
        processBrainData(brain);
    }
    
    void onSimulationStep(double timestep) override {
        // Handle simulation steps
        updatePluginState(timestep);
    }
    
    // Plugin configuration
    bool validateConfiguration(const nlm::Config& config) const override {
        // Validate configuration
        return validateConfig(config);
    }
    
    void applyConfiguration(const nlm::Config& config) override {
        // Apply configuration changes
        applyConfigChanges(config);
    }
    
    // Plugin functionality
    void processBrainData(std::shared_ptr<nlm::Brain> brain) {
        // Process brain data
        analyzeNeuronalActivity(brain);
    }
    
private:
    nlm::PluginState state_ = nlm::PluginState::Unloaded;
};

// Register plugin
REGISTER_PLUGIN(AdvancedPlugin, "advanced_plugin");
```

### Example 2: Advanced Configuration Usage

This example demonstrates advanced configuration usage:

```cpp
// Configuration header
#include "src/agent/AdvancedConfig.hpp"

// Configuration implementation
void setupAdvancedConfiguration() {
    AdvancedConfig& config = AdvancedConfig::getInstance();
    
    // Create performance optimization template
    nlm::ConfigTemplate perfTemplate;
    perfTemplate.name = "high_performance";
    perfTemplate.description = "Configuration optimized for high performance";
    perfTemplate.version = "2.0";
    
    // Add schema entries
    nlm::ConfigSchemaEntry entry1;
    entry1.key = "performance.optimization_level";
    entry1.type = "int";
    entry1.defaultValue = 8;
    entry1.minValue = 1;
    entry1.maxValue = 10;
    
    perfTemplate.schema.push_back(entry1);
    
    // Create template
    config.createTemplate(perfTemplate);
    
    // Load and use template
    nlm::Config myConfig;
    config.loadTemplate("high_performance", myConfig);
    
    // Modify configuration at runtime
    config.setValue("brain.neuron_count", 2000);
    config.setValue("performance.optimization_level", 5);
}
```

### Example 3: Performance Profiling

This example demonstrates performance profiling:

```cpp
// Performance header
#include "src/agent/AdvancedPerformance.hpp"

// Performance implementation
void profileNeuralNetworkPerformance() {
    AdvancedPerformance& perf = AdvancedPerformance::getInstance();
    
    // Start performance profiling session
    std::string sessionId = perf.startProfileSession(
        "Neural network training",
        "training_phase"
    );
    
    // Run simulation...
    for (int step = 0; step < 1000; ++step) {
        runNeuralNetworkStep(step);
    }
    
    // Analyze performance
    std::vector<PerformanceRegression> regressions = 
        perf.detectPerformanceRegressions(sessionId);
    
    // Generate optimization recommendations
    std::vector<PerformanceOptimization> optimizations = 
        perf.generateOptimizationRecommendations(sessionId);
    
    // End profiling session
    perf.endProfileSession(sessionId);
}
```

### Example 4: Visualization

This example demonstrates visualization capabilities:

```cpp
// Visualization header
#include "src/agent/VisualizationAPI.hpp"

// Visualization implementation
void visualizeNeuralNetwork() {
    VisualizationAPI& vizAPI = VisualizationAPI::getInstance();
    
    // Initialize visualization
    vizAPI.initialize();
    
    // Set visualization mode
    vizAPI.setVisualizationMode(VisualizationMode::RealTime);
    
    // Get brain instance
    Brain& brain = getBrainInstance();
    
    // Visualize network structure
    vizAPI.visualizeNetworkStructure(brain);
    
    // Visualize neuron activity
    vizAPI.visualizeNeuronActivity(brain, 0.5);
    
    // Create performance dashboard
    std::vector<std::string> metricNames = {"FiringRate", "MemoryUsage", "CPUUsage"};
    std::vector<std::vector<double>> metricValues = {
        {0.1, 0.2, 0.3, 0.4, 0.5},
        {100.0, 200.0, 150.0, 180.0, 160.0},
        {10.0, 15.0, 12.0, 18.0, 14.0}
    };
    vizAPI.createPerformanceDashboard(metricNames, metricValues);
}
```

### Example 5: Memory Management

This example demonstrates memory management capabilities:

```cpp
// Memory management header
#include "src/agent/MemoryManagementAPI.hpp"

// Memory management implementation
void manageMemoryResources() {
    MemoryManagementAPI& memoryManager = MemoryManagementAPI::getInstance();
    
    // Create custom memory pool
    CustomMemoryPoolConfig poolConfig;
    poolConfig.poolName = "neuron_pool";
    poolConfig.poolType = MemoryPoolType::Custom;
    poolConfig.initialSize = 1024 * 1024;  // 1MB
    poolConfig.maxSize = 10 * 1024 * 1024;  // 10MB
    poolConfig.chunkSize = 64;
    poolConfig.alignment = 8;
    poolConfig.enableThreadLocal = true;
    poolConfig.enableStatsTracking = true;
    poolConfig.enableLeakDetection = true;
    
    if (memoryManager.createMemoryPool(poolConfig)) {
        std::cout << "Memory pool created successfully!" << std::endl;
    }
    
    // Allocate memory from pool
    void* ptr = nullptr;
    if (memoryManager.allocateFromPool("neuron_pool", 1024, ptr, "neuron")) {
        std::cout << "Memory allocated from pool successfully!" << std::endl;
    }
    
    // Track memory allocation
    void* trackedPtr = memoryManager.trackAllocation(
        1024,           // size
        "neuron",       // type
        __FILE__,       // file
        __LINE__,       // line
        "neural_network_simulation"  // context
    );
    
    // Detect memory leaks
    MemoryLeakReport leakReport = memoryManager.detectMemoryLeaks();
    if (leakReport.hasLeaks()) {
        std::cout << "Memory leaks detected!" << std::endl;
    }
}
```

## Example Usage

### Running Examples

To run the examples:

```bash
# Navigate to build directory
cd build

# Run plugin example
./plugin_example

# Run advanced configuration example
./advanced_config_example

# Run performance profiling example
./performance_profiling_example

# Run visualization example
./visualization_example

# Run memory management example
./memory_management_example
```

### Example Configuration

Each example can be configured with different options:

```bash
# Run with custom configuration
./plugin_example --config custom_config.json

# Run with specific options
./advanced_config_example --template high_performance --neurons 2000

# Run with performance profiling
./performance_profiling_example --profile --output performance_report.json

# Run with visualization
./visualization_example --mode realtime --output visualization.png

# Run with memory monitoring
./memory_management_example --monitor --threshold 1024
```

## Example Integration

### Integrating Examples with Existing Code

To integrate examples with existing NLM code:

```cpp
// Include example headers
#include "src/agent/Examples/plugin_example.hpp"
#include "src/agent/Examples/advanced_config_example.hpp"
#include "src/agent/Examples/performance_profiling_example.hpp"
#include "src/agent/Examples/visualization_example.hpp"
#include "src/agent/Examples/memory_management_example.hpp"

// Use example functionality
void integrateExamples() {
    // Use plugin example
    PluginManager pluginManager;
    pluginManager.loadPlugin("advanced_plugin");
    
    // Use advanced configuration example
    AdvancedConfigManager configManager;
    configManager.loadTemplate("high_performance");
    
    // Use performance profiling example
    PerformanceProfiler perfProfiler;
    perfProfiler.startProfileSession("benchmark");
    
    // Use visualization example
    VisualizationEngine vizEngine;
    vizEngine.initialize();
    
    // Use memory management example
    MemoryManager memManager;
    memManager.startMemoryMonitoring();
}
```

### Example Testing

To test examples:

```bash
# Run example tests
make test

# Run specific example tests
make test_plugin_example
make test_advanced_config_example
make test_performance_profiling_example
make test_visualization_example
make test_memory_management_example

# Run integration tests
make integration_test
```

### Example Documentation

Each example includes comprehensive documentation:

```bash
# Generate example documentation
./documentation_generation.sh --examples

# Generate specific example documentation
./documentation_generation.sh --example plugin_example
./documentation_generation.sh --example advanced_config_example
```

## Conclusion

The examples directory provides comprehensive examples for NLM's advanced features. These examples demonstrate:

1. **Plugin Development**: How to develop and use custom plugins
2. **Advanced Configuration**: How to use advanced configuration features
3. **Performance Profiling**: How to profile and optimize performance
4. **Visualization**: How to use visualization capabilities
5. **Memory Management**: How to manage memory resources

These examples provide practical guidance for NLM users to leverage advanced features effectively while following best practices.