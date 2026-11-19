# NLM Advanced User Guide

This document provides comprehensive information for advanced users of the NLM (Neural Learning Machine) system.

## Table of Contents

1. [Overview of Advanced Features](#overview-of-advanced-features)
2. [Advanced Demos](#advanced-demos)
3. [Advanced Configuration Options](#advanced-configuration-options)
4. [Advanced World Configuration](#advanced-world-configuration)
5. [Advanced Analysis Tools](#advanced-analysis-tools)
6. [Advanced Command Line Options](#advanced-command-line-options)
7. [Performance Optimization Tips](#performance-optimization-tips)
8. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
9. [Advanced Configuration Examples](#advanced-configuration-examples)
10. [API Reference](#api-reference)

## Overview of Advanced Features

The NLM system provides several advanced features for power users and researchers:

### 1. Enhanced Integration Demo
- **nlm_advanced_demo**: Advanced version of the Phase 6 integration demo
- **Custom world configurations** with complexity levels
- **Enhanced logging** and analysis tools
- **Detailed result export** capabilities
- **Command-line configuration** options

### 2. Advanced World Configuration
- Configurable world sizes and complexities
- Adjustable object values (resources/hazards)
- Custom energy and decay rates
- Advanced world properties for testing

### 3. Advanced Analysis Tools
- Performance metrics calculation
- Learning curve analysis
- Exploration efficiency measurement
- Reward consistency analysis
- Detailed result export

### 4. Enhanced Command Line Interface
- Advanced configuration options
- Custom output files
- Detailed logging control
- Complexity mode settings

### 5. Performance Optimization
- Efficient memory usage
- Optimized simulation parameters
- Advanced configuration options
- Performance monitoring tools

## Advanced Demos

### nlm_advanced_demo

This advanced demo provides enhanced features for testing and analysis:

**Usage:**
```bash
./nlm_advanced_demo [options]
```

**Options:**
- `--advanced`: Enable advanced features
- `--seed <num>`: Set random seed for reproducibility
- `--output <file>`: Output file for results
- `--no-logging`: Disable detailed logging
- `--high-complexity`: Enable high complexity world

**Features:**
- Custom world configurations with complexity levels
- Enhanced logging and analysis tools
- Detailed result export capabilities
- Command-line configuration options
- Performance metrics analysis

**Example:**
```bash
./nlm_advanced_demo --advanced --seed 12345 --output results.txt --high-complexity
```

### nlm_phase6_demo (Original)

The original Phase 6 demo is maintained for backward compatibility:

**Usage:**
```bash
./nlm_phase6_demo
```

This demo provides the basic integration test with all systems connected.

## Advanced Configuration Options

### 1. World Configuration

Advanced world configuration allows for testing different scenarios:

```cpp
struct AdvancedSimpleWorld {
    float worldSize;
    float complexityLevel;
    size_t objectCount;
    float resourceValue;
    float hazardValue;
    
    void configureAdvanced(float width, float height, float complexity) {
        worldSize = std::min(width, height);
        complexityLevel = complexity;
        
        // Calculate number of objects based on complexity
        int baseObjects = 8;
        int maxObjects = 20;
        objectCount = static_cast<size_t>(baseObjects + 
                                          complexityLevel * (maxObjects - baseObjects));
        
        // Adjust object values based on complexity
        resourceValue = 10.0f + complexityLevel * 10.0f;
        hazardValue = -20.0f - complexityLevel * 10.0f;
    }
};
```

**Configuration Parameters:**
- **World Size**: Controls the physical dimensions of the world
- **Complexity Level**: Adjusts the number and value of objects
- **Object Count**: Number of resources and hazards
- **Resource Value**: Reward value for collecting resources
- **Hazard Value**: Penalty value for encountering hazards

### 2. Simulation Configuration

Advanced simulation configuration includes:

```cpp
struct AdvancedPhase6Config {
    uint64_t maxSteps;
    size_t neuronCount;
    float explorationRate;
    float learningRate;
    bool enableDetailedLogging;
    std::string outputFile;
    uint64_t customSeed;
    
    AdvancedPhase6Config()
        : maxSteps(5000)
        , neuronCount(1000)
        , explorationRate(0.3f)
        , learningRate(0.05f)
        , enableDetailedLogging(true)
        , outputFile("advanced_results.txt")
        , customSeed(12345) {}
};
```

**Configuration Parameters:**
- **Max Steps**: Number of simulation steps to run
- **Neuron Count**: Number of neurons in the brain
- **Exploration Rate**: Probability of taking random actions
- **Learning Rate**: Rate of learning from rewards
- **Detailed Logging**: Enable/disable detailed logging
- **Output File**: File to export analysis results
- **Custom Seed**: Random seed for reproducible results

## Advanced World Configuration

### Configuration Methods

The SimpleWorld class provides several configuration methods:

```cpp
void SimpleWorld::configure(float width, float height, 
                           size_t visionWidth, size_t visionHeight);
void SimpleWorld::setAgentStart(float x, float y);
void SimpleWorld::setMaxEnergy(float e);
void SimpleWorld::setEnergyDecayRate(float r);
void SimpleWorld::setResourceEnergyGain(float g);
void SimpleWorld::setRandomSeed(uint64_t seed);
```

### World Properties

**Basic Properties:**
- **Width/Height**: World dimensions in meters
- **Vision Width/Height**: Sensor dimensions
- **Agent Start**: Initial agent position

**Energy Properties:**
- **Max Energy**: Maximum energy capacity
- **Energy Decay Rate**: Rate of energy loss per step
- **Resource Energy Gain**: Energy gained from collecting resources

**Random Properties:**
- **Random Seed**: Seed for reproducible world generation

## Advanced Analysis Tools

### Performance Metrics

Advanced analysis tools provide detailed performance metrics:

```cpp
struct AnalysisResult {
    float totalReward;
    float avgFiringRate;
    size_t stepsWithLearning;
    float learningCurveMax;
    float explorationEfficiency;
    float rewardConsistency;
    time_t startTime;
    time_t endTime;
};
```

**Metrics Explained:**

1. **Total Reward**: Sum of all rewards received during simulation
2. **Average Firing Rate**: Mean firing rate of neurons
3. **Steps with Learning**: Number of steps where learning occurred
4. **Learning Curve Max**: Maximum learning signal observed
5. **Exploration Efficiency**: Ratio of positive rewards to total rewards
6. **Reward Consistency**: Measure of reward variance (lower = more consistent)

### Result Export

Results can be exported to files for further analysis:

```cpp
void AdvancedAnalysisTools::exportResults(const AnalysisResult& result, 
                                         const std::string& filename);
```

**Exported Format:**
```
Advanced NLM Analysis Results
================================
Total Reward: 123.45
Average Firing Rate: 12.34
Steps with Learning: 450
Learning Curve Max: 0.98
Exploration Efficiency: 0.85
Reward Consistency: 0.92
Start Time: 1234567890
End Time: 1234567890
```

## Advanced Command Line Options

### General Options

- `--advanced`: Enable advanced features
- `--help`: Display help information
- `--version`: Display version information

### Configuration Options

- `--seed <num>`: Set random seed for reproducibility
- `--output <file>`: Output file for results
- `--no-logging`: Disable detailed logging
- `--high-complexity`: Enable high complexity world

### Analysis Options

- `--detailed-analysis`: Enable detailed analysis tools
- `--export-results`: Export results to file
- `--performance-metrics`: Calculate performance metrics

### Simulation Options

- `--max-steps <num>`: Set maximum simulation steps
- `--neuron-count <num>`: Set number of neurons
- `--exploration-rate <rate>`: Set exploration rate
- `--learning-rate <rate>`: Set learning rate

## Performance Optimization Tips

### 1. Configuration Optimization

- Use smaller world sizes for faster testing
- Adjust complexity based on testing needs
- Enable/disable logging based on requirements

### 2. Memory Management

- Monitor memory usage during long simulations
- Use appropriate configuration for available memory
- Consider using checkpointing for long runs

### 3. Performance Monitoring

- Use the built-in performance metrics
- Monitor simulation progress
- Track learning effectiveness

### 4. Reproducibility

- Set random seeds for reproducible results
- Use checkpointing to resume simulations
- Document configuration for future runs

## Debugging and Troubleshooting

### Common Issues

1. **Reward is 0**: Check world configuration and object placement
2. **Performance is slow**: Reduce world size or complexity
3. **Memory usage is high**: Optimize configuration
4. **Results are inconsistent**: Check random seed settings

### Debugging Tools

The advanced demo provides several debugging tools:

1. **Detailed Logging**: Enable detailed logging for debugging
2. **Performance Metrics**: Monitor simulation performance
3. **Result Export**: Export results for analysis
4. **Configuration Validation**: Validate configuration settings

## Advanced Configuration Examples

### 1. Basic Advanced Configuration

```bash
./nlm_advanced_demo --advanced --seed 12345
```

### 2. Custom Output and Logging

```bash
./nlm_advanced_demo --advanced --output my_results.txt --no-logging
```

### 3. High Complexity World

```bash
./nlm_advanced_demo --advanced --high-complexity --max-steps 10000
```

### 4. Performance Testing

```bash
./nlm_advanced_demo --advanced --neuron-count 500 --exploration-rate 0.1 --learning-rate 0.01
```

## API Reference

### AdvancedSimpleWorld Class

```cpp
class AdvancedSimpleWorld {
public:
    AdvancedSimpleWorld();
    void configureAdvanced(float width, float height, float complexity);
    float getWorldSize() const;
    float getComplexityLevel() const;
    size_t getObjectCount() const;
    float getResourceValue() const;
    float getHazardValue() const;
};
```

### AdvancedAnalysisTools Class

```cpp
class AdvancedAnalysisTools {
public:
    struct AnalysisResult {
        float totalReward;
        float avgFiringRate;
        size_t stepsWithLearning;
        float learningCurveMax;
        float explorationEfficiency;
        float rewardConsistency;
        time_t startTime;
        time_t endTime;
    };
    
    static AnalysisResult analyzeResults(const std::vector<float>& rewards,
                                         const std::vector<float>& firingRates,
                                         const std::vector<float>& learningSignals);
    
    static void exportResults(const AnalysisResult& result,
                             const std::string& filename);
};
```

## Building and Running

### Building with Advanced Features

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running the Advanced Demo

```bash
./nlm_advanced_demo [options]
```

### Building for Production

```bash
mkdir build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=OFF
make -j4
```

## License

MIT License

## Copyright

Copyright (c) 2026 NLM Project

## Acknowledgments

This project would not be possible without the contributions of the NLM development team and the open source community.
