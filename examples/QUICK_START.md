# NLM Quick Start

## Overview
This quick start guide provides a comprehensive introduction to the NLM (熙然) neural learning machine system. It covers the basic concepts, system requirements, and provides step-by-step instructions for getting started with NLM.

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## System Requirements

### Minimum Requirements
- **Operating System**: Linux (Ubuntu 20.04 or later)
- **Compiler**: GCC 9.0 or later (or Clang 12.0 or later)
- **Memory**: 4 GB RAM (minimum), 8 GB RAM (recommended)
- **Disk Space**: 1 GB free space for source code
- **CMake**: Version 3.15 or later

### Recommended Requirements
- **Memory**: 8 GB RAM or more
- **Disk Space**: 10 GB free space for simulation data
- **Graphics**: Graphics card with OpenGL support (for visualization)
- **Network**: Network connectivity for updates and sharing

### Software Dependencies

```bash
# Ubuntu/Debian dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git libglm-dev libgl1-mesa-dev libglu1-mesa-dev

# CentOS/RHEL dependencies
sudo yum install -y gcc gcc-c++ cmake git mesa-devel Mesa-libGLU-devel

# macOS dependencies (using Homebrew)
brew install cmake git glm

# Windows dependencies (using Chocolatey)
choco install cmake git glm
```

## Installation

### Step 1: Clone the Repository

```bash
# Clone the NLM repository
git clone https://github.com/Kilo-Org/nlm.git

cd nlm

# Checkout the latest stable release
git checkout release/phase6
```

### Step 2: Build the Project

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_VISION=ON -DNLM_ENABLE_AUDIO=ON

# Build the project
make -j4
```

### Step 3: Verify Installation

```bash
# Check if build was successful
ls -la

# Run the Phase 6 integration demo
./nlm_phase6_demo

# Check for build artifacts
find . -name "*.so" -o -name "*.dylib" -o -name "*.dll" | head -10
```

## Basic Usage

### Phase 6 Demo (Integration Test)

The Phase 6 demo is a comprehensive integration test that verifies all brain systems are connected. It's the best way to verify your installation and see NLM in action.

```bash
# Run the Phase 6 demo
./nlm_phase6_demo

# The demo will:
# 1. Initialize the brain with neurons and synapses
# 2. Create a sensory environment
# 3. Run the brain loop with neural processing
# 4. Show neural activity and spike detection
# 5. Demonstrate memory systems
# 6. Show neuromodulation effects
# 7. Display final brain state
```

### Configuration System

The NLM configuration system allows you to customize the neural simulation.

#### Basic Configuration

```bash
# Create a basic configuration file
mkdir -p config
echo '{
  "random_seed": 42,
  "simulation_timestep": 0.001,
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "log_level": "INFO",
  "log_to_file": false,
  "visualization_enabled": false
}' > config/basic_config.json
```

#### Using Configuration CLI

```bash
# Load configuration from file
nlm-config --config config/basic_config.json

# Validate configuration
nlm-config --validate --config config/basic_config.json

# Export configuration
nlm-config --export --format json > exported_config.json
```

### Neural Activity Analysis

```cpp
#include <iostream>
#include <fstream>
#include <vector>
#include "core/utilities/NeuralActivityCalculator.hpp"
#include "core/utilities/SpikeDetector.hpp"

int main() {
    std::cout << "NLM Neural Activity Analysis" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    // Create neural activity calculator
    NeuralActivityCalculator activityCalculator(
        NeuralActivityCalculator::Strategy::DEVIATION
    );
    activityCalculator.setThreshold(0.5);
    activityCalculator.setNormalization(true);
    
    // Create spike detector
    SpikeDetector spikeDetector;
    spikeDetector.setThreshold(20.0);  // mV
    spikeDetector.setRefractoryPeriod(5.0);  // ms
    
    // Simulate neural data
    std::vector<double> voltages;
    double timestep = 0.001;  // 1 ms
    
    for (int i = 0; i < 1000; ++i) {
        // Generate realistic neural voltage with noise
        double voltage = 20.0 + 5.0 * sin(i * 0.01) + rand() % 100 * 0.01;
        voltages.push_back(voltage);
        
        // Detect spikes
        if (spikeDetector.detectSpike(voltage)) {
            std::cout << "Spike detected at time: " << i * timestep << " s" << std::endl;
        }
    }
    
    // Calculate neural activity
    std::vector<double> activities;
    for (size_t i = 1; i < voltages.size(); ++i) {
        double activity = activityCalculator.calculate(voltages[i-1], voltages[i]);
        activities.push_back(activity);
    }
    
    // Calculate statistics
    double meanActivity = 0.0;
    for (double activity : activities) meanActivity += activity;
    meanActivity /= activities.size();
    
    double maxActivity = *std::max_element(activities.begin(), activities.end());
    double minActivity = *std::min_element(activities.begin(), activities.end());
    
    std::cout << std::endl;
    std::cout << "Neural Activity Statistics:" << std::endl;
    std::cout << "  Mean Activity: " << meanActivity << std::endl;
    std::cout << "  Max Activity: " << maxActivity << std::endl;
    std::cout << "  Min Activity: " << minActivity << std::endl;
    std::cout << "  Activity Range: " << (maxActivity - minActivity) << std::endl;
    
    // Save results
    std::ofstream outFile("neural_activity_results.txt");
    outFile << "Time (s),Voltage (mV),Activity" << std::endl;
    for (size_t i = 0; i < voltages.size(); ++i) {
        outFile << i * timestep << "," << voltages[i] << ",";
        if (i < activities.size()) {
            outFile << activities[i];
        }
        outFile << std::endl;
    }
    outFile.close();
    
    std::cout << "Results saved to neural_activity_results.txt" << std::endl;
    
    return 0;
}
```

## Advanced Usage

### Neural Activity Calculator Strategies

NLM provides multiple neural activity calculation strategies:

#### 1. Deviation Strategy

```cpp
NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::DEVIATION);
calculator.setThreshold(0.5);
calculator.setNormalization(true);
```

**Description**: Measures the deviation of current voltage from previous voltage.

#### 2. Average Strategy

```cpp
NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::AVERAGE);
calculator.setWindowSize(100);
calculator.setNormalization(true);
```

**Description**: Calculates the average voltage over a sliding window.

#### 3. Weighted Strategy

```cpp
NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::WEIGHTED);
calculator.setWeightFactor(1.0);
calculator.setNormalization(true);
```

**Description**: Calculates weighted average of voltage changes.

#### 4. RMS Strategy

```cpp
NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::RMS);
calculator.setWindowSize(50);
calculator.setNormalization(true);
```

**Description**: Calculates root mean square of voltage changes.

#### 5. Variance Strategy

```cpp
NeuralActivityCalculator calculator(NeuralActivityCalculator::Strategy::VARIANCE);
calculator.setWindowSize(100);
calculator.setNormalization(true);
```

**Description**: Calculates variance of voltage over a window.

### Spike Detection Strategies

#### 1. Threshold-Based Detection

```cpp
SpikeDetector detector;
detector.setThreshold(20.0);  // mV
detector.setRefractoryPeriod(5.0);  // ms
detector.setDerivativeThreshold(0.0);  // No derivative detection
```

**Description**: Simple threshold-based spike detection.

#### 2. Derivative-Based Detection

```cpp
SpikeDetector detector;
detector.setThreshold(15.0);  // mV
detector.setRefractoryPeriod(10.0);  // ms
detector.setDerivativeThreshold(10.0);  // Derivative threshold
```

**Description**: Spike detection based on voltage derivative.

#### 3. Adaptive Detection

```cpp
SpikeDetector detector;
detector.setThreshold(20.0);  // mV
detector.setRefractoryPeriod(5.0);  // ms
detector.setAdaptiveThreshold(true);
detector.setThresholdAdaptationRate(0.01);
```

**Description**: Adaptive spike detection with dynamic threshold adjustment.

### Configuration Templates

NLM provides several configuration templates for different use cases:

#### Basic Template

```bash
# Apply basic template
nlm-config --template basic
```

#### CPU Performance Template

```bash
# Apply CPU performance template
nlm-config --template cpu_performance
```

#### Memory-Intensive Template

```bash
# Apply memory-intensive template
nlm-config --template memory_intensive
```

#### Production Template

```bash
# Apply production template
nlm-config --template production
```

### Plugin Development

NLM supports plugin development for extending functionality:

#### Creating a Plugin

```cpp
// Example plugin for NLM
#include "core/utilities/NeuralActivityCalculator.hpp"

class ExamplePlugin : public INeuralActivityCalculatorPlugin {
public:
    // Plugin interface implementation
    std::string getName() const override { return "ExamplePlugin"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Example plugin for NLM"; }
    
    // Plugin functionality
    double calculate(const std::vector<double>& voltages) const override {
        // Custom neural activity calculation
        return 0.0;
    }
};
```

#### Loading Plugins

```cpp
// Load plugin in NLM
PluginManager pluginManager;
pluginManager.loadPlugin("example_plugin.so");
pluginManager.enablePlugin("example_plugin");
```

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Build Fails

**Problem**: CMake build fails or make fails.

**Solution**:

```bash
# Check CMake version
 cmake --version

# Check compiler version
g++ --version

# Try building with verbose output
make -j4 VERBOSE=1

# Check for common issues
cat build/CMakeFiles/*.log | grep error
```

#### Issue 2: Phase 6 Demo Fails

**Problem**: The Phase 6 demo fails to run.

**Solution**:

```bash
# Check if executable exists
ls -la ./nlm_phase6_demo

# Run with strace to see system calls
strace ./nlm_phase6_demo 2>&1 | head -50

# Run with gdb for debugging
gdb ./nlm_phase6_demo
(gdb) run
```

#### Issue 3: Configuration Loading Fails

**Problem**: Configuration files fail to load.

**Solution**:

```bash
# Check if configuration file exists
ls -la config.json

# Check file permissions
ls -la config.json

# Try loading with explicit format
nlm-config --config config.json --format json
```

#### Issue 4: Neural Activity Calculator Fails

**Problem**: NeuralActivityCalculator fails or produces unexpected results.

**Solution**:

```cpp
// Check for null or empty voltage data
if (voltages.empty() || voltages.size() < 2) {
    std::cerr << "Error: Voltage data is empty or too small" << std::endl;
    return 1;
}

// Check for valid voltage values
for (double voltage : voltages) {
    if (std::isnan(voltage) || std::isinf(voltage)) {
        std::cerr << "Error: Invalid voltage value: " << voltage << std::endl;
        return 1;
    }
}
```

#### Issue 5: Spike Detector Fails

**Problem**: SpikeDetector fails or produces unexpected results.

**Solution**:

```cpp
// Check for valid threshold
if (threshold <= 0.0) {
    std::cerr << "Error: Threshold must be positive" << std::endl;
    return 1;
}

// Check for valid refractory period
if (refractoryPeriod < 0.0) {
    std::cerr << "Error: Refractory period must be non-negative" << std::endl;
    return 1;
}
```

### Getting Help

#### Online Resources

- **NLM Documentation**: https://docs.nln.org
- **NLM GitHub Repository**: https://github.com/Kilo-Org/nlm
- **NLM Issues**: https://github.com/Kilo-Org/nlm/issues
- **NLM Discussions**: https://github.com/Kilo-Org/nlm/discussions

#### Community Support

- **NLM Discord**: https://discord.gg/nlm
- **NLM Slack**: https://nlm.slack.com
- **NLM mailing list**: nlm-announce@googlegroups.com

#### Local Support

- **NLM User Manual**: docs/USAGE_GUIDE.md
- **NLM API Reference**: docs/API_REFERENCE.md
- **NLM Plugin Development Guide**: docs/PLUGIN_DEVELOPMENT.md
- **NLM Troubleshooting Guide**: docs/ERROR_HANDLING_REFERENCE.md

## Conclusion

This quick start guide provides a comprehensive introduction to the NLM neural learning machine system. It covers:

1. **System requirements and installation**
2. **Basic usage and configuration**
3. **Neural activity calculation and spike detection**
4. **Configuration templates and plugin development**
5. **Troubleshooting and getting help**

By following this guide, you should be able to:

- Install NLM on your system
- Build and run the Phase 6 demo
- Use the configuration system
- Perform neural activity analysis
- Develop plugins for NLM

For more detailed information, refer to the other documentation files in this guide, particularly the advanced usage guide, configuration guide, and plugin development guide.

**Status**: COMPLETE
**Version**: 1.0.0
**Last Updated**: September 30, 2026