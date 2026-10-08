# NLM Phase 6 Advanced Features Tutorial

## Getting Started with Advanced NLM Features

This tutorial provides step-by-step instructions for using the advanced features implemented in NLM Phase 6. It covers the main components: AdvancedBrain interface, enhanced configuration system, and advanced command-line interface.

## Prerequisites

- NLM Phase 6 compiled and installed
- Basic understanding of NLM concepts and terminology
- Familiarity with C++ programming (for code examples)

## Tutorial Overview

This tutorial is divided into three main parts:

1. **Part 1: AdvancedBrain Interface** - Getting started with AdvancedBrain
2. **Part 2: Configuration Management** - Using advanced configuration options
3. **Part 3: Command-Line Interface** - Running experiments using the CLI

Each part includes practical code examples and explanations.

## Part 1: AdvancedBrain Interface

### 1.1 Creating an AdvancedBrain Instance

The AdvancedBrain class provides an enhanced interface to the basic Brain system. Here's how to create and initialize an AdvancedBrain:

```cpp
#include "brain/AdvancedBrain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("simulation_timestep", 0.001);
    
    // Create AdvancedBrain with monitoring enabled
    AdvancedBrain advancedBrain(config, 
                               true,  // enableMonitoring
                               false, // enableThreading
                               true); // enableProfiling
    
    // Initialize AdvancedBrain
    if (!advancedBrain.initialize()) {
        std::cerr << "Failed to initialize AdvancedBrain" << std::endl;
        return 1;
    }
    
    std::cout << "AdvancedBrain initialized successfully" << std::endl;
    return 0;
}
```

### 1.2 Running Simulation with Monitoring

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 500);
    config->set("region_count", 1);
    
    AdvancedBrain advancedBrain(config, true, false, true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Progress callback function
    auto progressCallback = [](float progress) {
        std::cout << "Progress: " << (progress * 100) << "%" << std::endl;
    };
    
    // Run simulation with monitoring
    SimulationResult result = advancedBrain.runWithMonitoring(
        0,                    // startStep
        2000,                 // numSteps
        progressCallback,    // progress callback
        500                   // monitorInterval
    );
    
    // Display results
    std::cout << "=== Simulation Results ===" << std::endl;
    std::cout << "Total steps: " << result.totalSteps << std::endl;
    std::cout << "Total time: " << result.totalTime << " seconds" << std::endl;
    std::cout << "Average firing rate: " << result.averageFiringRate << std::endl;
    std::cout << "Total spikes: " << result.totalSpikes << std::endl;
    std::cout << "Memory usage: " << result.currentMemoryUsage << " bytes" << std::endl;
    std::cout << "Peak memory usage: " << result.peakMemoryUsage << " bytes" << std::endl;
    std::cout << "Simulation successful: " << (result.isSuccessful ? "Yes" : "No") << std::endl;
    
    return 0;
}
```

### 1.3 Performance Monitoring

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>

void monitorPerformance(const PerformanceStats& stats) {
    std::cout << "=== Performance Statistics ===" << std::endl;
    std::cout << "Total steps: " << stats.totalSteps << std::endl;
    std::cout << "Total time: " << stats.totalTime << " seconds" << std::endl;
    std::cout << "Average step time: " << stats.averageStepTime << " seconds" << std::endl;
    std::cout << "Current memory usage: " << stats.currentMemoryUsage << " bytes" << std::endl;
    std::cout << "Peak memory usage: " << stats.peakMemoryUsage << " bytes" << std::endl;
    std::cout << "Memory allocations: " << stats.memoryAllocations << std::endl;
    std::cout << "Memory deallocations: " << stats.memoryDeallocations << std::endl;
    std::cout << "Current firing rate: " << stats.currentFiringRate << std::endl;
    std::cout << "Total spikes: " << stats.totalSpikes << std::endl;
    std::cout << "Simulation speed: " << stats.currentSimulationSpeed << " steps/second" << std::endl;
    std::cout << "Simulation paused: " << (stats.isPaused ? "Yes" : "No") << std::endl;
    std::cout << "===============================" << std::endl;
}

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    AdvancedBrain advancedBrain(config, true, false, true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Get and display performance statistics periodically
    for (SimulationStep step = 0; step < 1000; step += 100) {
        PerformanceStats stats = advancedBrain.getPerformanceStats();
        monitorPerformance(stats);
        
        // Run a simulation step
        advancedBrain.getBrain()->step(step);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
}
```

### 1.4 Event-Driven Callbacks

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>

// Event callback function
void onStepComplete(const BrainEventData& event) {
    std::cout << "[" << event.timestamp << "] Step completed" << std::endl;
    
    if (event.brain) {
        std::cout << "  Firing rate: " << event.brain->getAverageFiringRate() << std::endl;
        std::cout << "  Memory usage: " << event.brain->getTotalNeuronCount() << " neurons" << std::endl;
    }
}

void onMemoryUpdate(const BrainEventData& event) {
    std::cout << "Memory system updated" << std::endl;
    // Additional memory update logic can be added here
}

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    
    AdvancedBrain advancedBrain(config, true, false, true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Add event callbacks
    uint32_t stepCallbackId = advancedBrain.addEventCallback(
        BrainEvent::StepComplete,
        onStepComplete
    );
    
    uint32_t memoryCallbackId = advancedBrain.addEventCallback(
        BrainEvent::MemoryUpdated,
        onMemoryUpdate
    );
    
    std::cout << "Event callbacks added:" << std::endl;
    std::cout << "  Step complete callback ID: " << stepCallbackId << std::endl;
    std::cout << "  Memory update callback ID: " << memoryCallbackId << std::endl;
    
    // Run simulation with event callbacks
    SimulationResult result = advancedBrain.runWithMonitoring(0, 500);
    
    // Clean up event callbacks
    advancedBrain.removeEventCallback(stepCallbackId);
    advancedBrain.removeEventCallback(memoryCallbackId);
    
    std::cout << "Event callbacks removed" << std::endl;
    
    return 0;
}
```

### 1.5 Export/Import Functionality

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>
#include <filesystem>

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 600);
    
    AdvancedBrain advancedBrain(config, true, false, true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Create test directory
    std::string exportDir = "test_export";
    if (!std::filesystem::exists(exportDir)) {
        std::filesystem::create_directory(exportDir);
    }
    
    std::string exportPath = exportDir + "/brain_state";
    
    // Export brain state with metadata
    std::cout << "Exporting brain state to: " << exportPath << std::endl;
    bool exportSuccess = advancedBrain.exportState(exportPath, true);
    
    if (exportSuccess) {
        std::cout << "Brain state exported successfully" << std::endl;
        
        // Run some simulation steps
        for (SimulationStep step = 0; step < 200; ++step) {
            advancedBrain.getBrain()->step(step);
        }
        
        // Import brain state
        std::cout << "Importing brain state from: " << exportPath << std::endl;
        bool importSuccess = advancedBrain.importState(exportPath, true);
        
        if (importSuccess) {
            std::cout << "Brain state imported successfully" << std::endl;
        } else {
            std::cout << "Brain state import failed" << std::endl;
        }
    } else {
        std::cout << "Brain state export failed" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove_all(exportDir);
    
    return 0;
}
```

## Part 2: Configuration Management

### 2.1 Advanced Configuration

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>

int main() {
    auto config = std::make_shared<Config>();
    
    // Load existing configuration
    if (config->loadFromFile("config.cfg")) {
        std::cout << "Configuration loaded from config.cfg" << std::endl;
    } else {
        std::cout << "Using default configuration" << std::endl;
    }
    
    // Configure advanced features
    config->set("simulation_timestep", 0.0005, ConfigSource::CommandLine);
    config->set("neuron_count", 1200, ConfigSource::CommandLine);
    
    // Set advanced configuration options
    config->set("advanced.monitoring_enabled", true, ConfigSource::CommandLine);
    config->set("advanced.profiling_enabled", true, ConfigSource::CommandLine);
    config->set("advanced.threading_enabled", false, ConfigSource::CommandLine);
    
    // Create AdvancedBrain
    AdvancedBrain advancedBrain(config, true, false, true);
    
    // Configure custom features
    advancedBrain.configureFeature("custom_neuron_type", "excitatory");
    advancedBrain.enableFeature("advanced_monitoring", true);
    advancedBrain.enableFeature("event_callbacks", true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Get configuration values
    if (auto customFeature = advancedBrain.getConfiguration("custom_neuron_type")) {
        std::cout << "Custom feature: " << *customFeature << std::endl;
    }
    
    std::cout << "Feature enabled: advanced_monitoring = " 
              << advancedBrain.isFeatureEnabled("advanced_monitoring") << std::endl;
    
    std::cout << "Advanced configuration loaded successfully" << std::endl;
    
    return 0;
}
```

### 2.2 Performance Analysis

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>

void exportPerformanceData(const std::string& filename,
                           const PerformanceStats& stats) {
    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        file << std::fixed << std::setprecision(6);
        file << "timestamp," << std::chrono::system_clock::now() << ",";
        file << "steps," << stats.totalSteps << ",";
        file << "time," << stats.totalTime << ",";
        file << "firing_rate," << stats.currentFiringRate << ",";
        file << "memory_usage," << stats.currentMemoryUsage << ",";
        file << "peak_memory," << stats.peakMemoryUsage << ",";
        file << "simulation_speed," << stats.currentSimulationSpeed << std::endl;
        file.close();
    }
}

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000);
    
    AdvancedBrain advancedBrain(config, true, false, true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Run simulation and collect performance data
    std::string dataFile = "performance_data.csv";
    
    // Clear existing data
    if (std::filesystem::exists(dataFile)) {
        std::filesystem::remove(dataFile);
    }
    
    for (SimulationStep step = 0; step < 1000; step += 50) {
        // Run simulation step
        advancedBrain.getBrain()->step(step);
        
        // Get performance statistics
        PerformanceStats stats = advancedBrain.getPerformanceStats();
        
        // Export performance data
        exportPerformanceData(dataFile, stats);
        
        // Display progress
        if (step % 200 == 0) {
            std::cout << "Step " << step << " completed" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "Performance data exported to " << dataFile << std::endl;
    
    // Analyze performance data
    ProfilingInfo profiling = advancedBrain.getProfilingInfo(20);
    
    std::cout << "=== Performance Analysis ===" << std::endl;
    std::cout << "Total samples: " << profiling.totalSamples << std::endl;
    std::cout << "Average step time: " << profiling.averageStepTime << " seconds" << std::endl;
    std::cout << "Step time variance: " << profiling.stepTimeVariance << std::endl;
    std::cout << "Peak memory usage: " << profiling.peakMemoryUsage << " bytes" << std::endl;
    
    return 0;
}
```

### 2.3 Event-Driven Configuration

```cpp
#include "brain/AdvancedBrain.hpp"
#include <iostream>

// Event callback for configuration changes
void onConfigurationChange(const BrainEventData& event) {
    std::cout << "Configuration changed" << std::endl;
    
    // Update advanced features based on configuration
    if (event.brain) {
        // Check if monitoring is enabled
        bool monitoringEnabled = event.brain->getConfig()->get<bool>("advanced.monitoring_enabled").value_or(false);
        
        // In a real implementation, you would update AdvancedBrain features here
        std::cout << "Monitoring enabled: " << (monitoringEnabled ? "Yes" : "No") << std::endl;
    }
}

int main() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 800);
    
    AdvancedBrain advancedBrain(config, true, false, true);
    
    if (!advancedBrain.initialize()) {
        std::cerr << "Initialization failed" << std::endl;
        return 1;
    }
    
    // Add configuration change event callback
    uint32_t configCallbackId = advancedBrain.addEventCallback(
        BrainEvent::StateChanged,
        onConfigurationChange
    );
    
    std::cout << "Configuration change monitoring enabled" << std::endl;
    
    // Simulate configuration changes
    for (int i = 0; i < 5; ++i) {
        std::cout << "Simulating configuration change " << i << std::endl;
        
        // In a real scenario, this would be triggered by actual configuration changes
        // For demonstration, we'll just wait and then process
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Clean up
    advancedBrain.removeEventCallback(configCallbackId);
    
    std::cout << "Configuration change monitoring disabled" << std::endl;
    
    return 0;
}
```

## Part 3: Command-Line Interface

### 3.1 Basic CLI Usage

```bash
# Run Phase 6 demo with default settings
./nlm_cli --demo phase6

# Run Phase 6 demo with specific number of steps
./nlm_cli --demo phase6 --steps 5000

# Run Phase 6 demo with custom configuration
./nlm_cli --demo phase6 --config my_config.cfg

# Run Phase 6 demo with enhanced monitoring
./nlm_cli --demo phase6 --no-checkpointing --no-replay --no-development
```

### 3.2 Advanced CLI Examples

#### Continuous Monitoring Mode

```bash
# Run Phase 6 in continuous mode
./nlm_cli --demo phase6 --continuous --update-interval 5000

# This will run indefinitely until Ctrl+C is pressed
# Display progress every 5000 steps
```

#### Performance Analysis Mode

```bash
# Run with output to file for analysis
./nlm_cli --demo phase6 --output results.csv --steps 10000 --no-checkpointing

# This will:
# 1. Run 10,000 simulation steps
# 2. Export performance data to results.csv
# 3. Not create checkpoints (for faster performance)
# 4. Collect data for analysis
```

#### Statistical Analysis Mode

```bash
# Run with multiple seeds for statistical significance
./nlm_cli --demo phase6 --seeds 10 --steps 2000 --quiet

# This will:
# 1. Run 10 experiments with different random seeds
# 2. Suppress output for automated processing
# 3. Generate statistically significant results
```

#### Integrated Demo Mode

```bash
# Run all phases in sequence
./nlm_cli --demo integrated --steps 5000 --no-checkpointing

# This will:
# 1. Run Phase 3 (world interaction)
# 2. Run Phase 4 (cognitive functions)
# 3. Run Phase 5 (lifetime learning)
# 4. Run Phase 6 (integrated experiment)
# 5. Total: 5000 simulation steps across all phases
```

### 3.3 Configuration File Examples

#### Basic Configuration File (`config.cfg`)

```
# NLM Configuration File
# Basic settings
neuron_count = 1000
region_count = 2
simulation_timestep = 0.001
connection_probability = 0.1

# Advanced features
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Memory settings
working_memory_capacity = 100
episodic_memory_max_episodes = 1000

# Performance settings
enable_profiling = true
enable_threading = false
```

#### Advanced Configuration File (`advanced_config.json`)

```json
{
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "random_seed": 42,
    
    "advanced": {
        "monitoring_enabled": true,
        "profiling_enabled": true,
        "threading_enabled": false,
        "custom_features": {
            "novelty_threshold": 0.5,
            "curiosity_rate": 0.3,
            "reward_multiplier": 1.0
        }
    },
    
    "memory": {
        "working_memory_capacity": 100,
        "episodic_memory_max_episodes": 1000,
        "associative_memory_size": 500
    },
    
    "performance": {
        "monitor_interval": 1000,
        "max_samples": 1000,
        "export_interval": 60.0
    }
}
```

## Running Examples

### Example 1: Performance Monitoring Demo

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run performance monitoring demo
./nlm_phase6_demo --steps 10000 --monitoring

# This will:
# 1. Create an AdvancedBrain with monitoring
# 2. Run 10,000 simulation steps
# 3. Collect and display performance statistics
# 4. Export performance data
```

### Example 2: Event-Driven Demo

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_FEATURES=ON
make -j4

# Run event-driven demo
./nlm_event_demo --demo phase6 --callbacks

# This will:
# 1. Create an AdvancedBrain with event callbacks
# 2. Run Phase 6 demo with event monitoring
# 3. Demonstrate event-driven architecture
```

### Example 3: Export/Import Demo

```bash
# Create build directory
mkdir build
cd build

# Configure and build
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ADVANCED_FEATURES=ON
make -j4

# Run export/import demo
./nlm_export_demo --export ./brain_export --import

# This will:
# 1. Run a simulation
# 2. Export brain state with metadata
# 3. Import brain state (with validation)
# 4. Demonstrate persistence capabilities
```

## Troubleshooting

### Common Issues

#### Issue: AdvancedBrain initialization fails

**Solution:**
1. Check that configuration file is valid
2. Verify that required dependencies are available
3. Ensure that underlying Brain initialization succeeds
4. Check for sufficient system resources (memory, CPU)

```bash
# Check configuration file
cat config.cfg

# Verify build log for initialization errors
cd build && make -j4 2>&1 | tail -50
```

#### Issue: Performance data export fails

**Solution:**
1. Ensure that export directory exists and is writable
2. Check available disk space
3. Verify file permissions
4. Ensure that performance monitoring is enabled

```bash
# Create export directory
mkdir -p performance_data

# Check disk space
df -h .

# Verify file permissions
ls -la performance_data/
```

#### Issue: Event callbacks don't work

**Solution:**
1. Ensure that event callback feature is enabled
2. Check that event types are correctly specified
3. Verify that callback functions are properly registered
4. Ensure that event system is properly initialized

```cpp
// Enable event callbacks
advancedBrain.enableFeature("event_callbacks", true);

// Add event callback
uint32_t callbackId = advancedBrain.addEventCallback(
    BrainEvent::StepComplete,
    myCallbackFunction
);

// Verify callback was added
if (callbackId != 0) {
    std::cout << "Event callback added successfully" << std::endl;
}
```

### Getting Help

#### Command Line Help

```bash
# Display help for Advanced Brain interface
./nlm_cli --help

# Display version information
./nlm_cli --version

# Display demo mode options
./nlm_cli --demo phase6 --help
```

#### Documentation

- **Advanced Features Documentation:** `docs/ADVANCED_FEATURES.md`
- **API Reference:** `docs/API_REFERENCE.md`
- **Configuration Guide:** `docs/CONFIGURATION.md`
- **Tutorials:** `docs/TUTORIALS.md`

#### Online Resources

- **NLM Documentation:** https://nlm.org/documentation
- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Issue Tracker:** https://github.com/nlm-project/nlm/issues
- **Forum:** https://forum.nlm.org

## Conclusion

This tutorial provides a comprehensive introduction to the advanced features of NLM Phase 6. It covers:

1. **AdvancedBrain Interface:** Creating and using AdvancedBrain for enhanced control
2. **Configuration Management:** Using advanced configuration options
3. **Event-Driven Architecture:** Implementing event-driven systems
4. **Performance Monitoring:** Collecting and analyzing performance data
5. **Export/Import Functionality:** Persisting brain state with metadata

The advanced features enable users to:

- **Monitor Performance:** Real-time and historical performance tracking
- **Customize Behavior:** Extensive configuration and feature control
- **Integrate Systems:** Event-driven architecture and state management
- **Analyze Results:** Performance analysis and data export
- **Reproduce Research:** Configuration files and state persistence

For more information and advanced topics, please refer to the full documentation and API references.
