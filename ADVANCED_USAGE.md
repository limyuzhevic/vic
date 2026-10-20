# NLM - Advanced Usage Guide for Power Users

This guide provides comprehensive documentation for advanced usage of NLM, including:
- Command-line interface usage
- Advanced configuration options
- Power user commands
- Performance optimization
- Advanced analysis modes

## 1. Command-Line Interface Usage

The main executable `nlm_simulation` (Phase 2) now provides extensive command-line options:

### Basic Usage

```bash
# Help and version information
./nlm_simulation --help
./nlm_simulation --version

# Run with default configuration
./nlm_simulation

# Run with custom configuration file
./nlm_simulation --config=my_config.json
```

### Advanced Configuration

```bash
# Basic connectivity test with custom parameters
./nlm_simulation --neurons=2000 --timestep=0.0005 --seed=12345

# Enhanced plasticity experiment
./nlm_simulation --experimental=plasticity --neurons=1500 --stdp-ltp=0.025 --stdp-ltd=0.018

# Quick test (minimal parameters)
./nlm_simulation --quick

# Skip all experiments (for benchmarking)
./nlm_simulation --no-tests
```

### Experimental Modes

```bash
# Run specific experiments
./nlm_simulation --experimental=connectivity
./nlm_simulation --experimental=plasticity
./nlm_simulation --experimental=stdp
./nlm_simulation --experimental=all

# Custom experimental configuration
./nlm_simulation --experimental=plasticity --synaptogenesis=0.00005 --pruning=0.000005
```

### Advanced Commands for Power Users

```bash
# Interactive mode (requires TUI support)
./nlm_simulation --interactive

# Run benchmark analysis
./nlm_simulation --benchmark

# Export configuration
./nlm_simulation --export-config

# Advanced analysis with custom mode
./nlm_simulation --advanced-analysis --analysis-mode=connectivity
./nlm_simulation --advanced-analysis --analysis-mode=plasticity
./nlm_simulation --advanced-analysis --analysis-mode=neuromodulation
./nlm_simulation --advanced-analysis --analysis-mode=development

# Combination of advanced features
./nlm_simulation --advanced --benchmark --experimental=all
```

## 2. Advanced Configuration Options

The configuration system now uses named constants to eliminate magic numbers:

### Named Constants (from src/core/Constants.hpp)

#### Simulation Constants
- `SimConstants::DefaultTimestep` - Default simulation timestep (0.001 seconds)
- `SimConstants::DefaultNeuronCount` - Default neuron count (500)
- `SimConstants::DefaultMaxSteps` - Maximum steps per experiment (1000)
- `SimConstants::DefaultConnectivityTestSteps` - Steps for connectivity test (50)

#### Plasticity Constants
- `PlasticityConstants::DefaultSTDP_LTPWeight` - STDP LTP weight (0.02)
- `PlasticityConstants::DefaultSTDP_LTDWeight` - STDP LTD weight (0.015)
- `PlasticityConstants::DefaultSTDPTau` - STDP time constant (20.0)
- `PlasticityConstants::DefaultSynaptogenesisRate` - Synaptogenesis rate (0.0001)
- `PlasticityConstants::DefaultPruningRate` - Pruning rate (0.00001)

#### Neuromodulation Constants
- `NeuromodulationConstants::DefaultCuriosityThreshold` - Curiosity threshold (0.3)
- `NeuromodulationConstants::DefaultExplorationChance` - Max exploration probability (0.3)
- `NeuromodulationConstants::DefaultPredictionErrorThreshold` - Prediction error threshold (0.001)

#### Development Constants
- `DevelopmentConstants::InitialStageDuration` - Initial stage duration (60 seconds)
- `DevelopmentConstants::CriticalPeriodDuration` - Critical period duration (240 seconds)
- `DevelopmentConstants::MaturationStageDuration` - Maturation stage duration (540 seconds)
- `DevelopmentConstants::AdultPlasticityModifier` - Adult plasticity (0.2)

#### Sensory/Motor Constants
- `SensoryMotorConstants::VisionNeuronCount` - Vision neuron count (256)
- `SensoryMotorConstants::TouchNeuronCount` - Touch neuron count (8)
- `SensoryMotorConstants::MotorActionCount` - Motor action count (6)

## 3. Power User Commands

### 3.1 Advanced Analysis Commands

```bash
# Detailed network analysis
./nlm_simulation --advanced-analysis --analysis-mode=connectivity
# Outputs:
# - Network topology metrics
# - Connection density calculations
# - Neuron distribution statistics

# Plasticity synapse analysis
./nlm_simulation --advanced-analysis --analysis-mode=plasticity
# Outputs:
# - Synapse counts by plasticity type
# - Weight distribution statistics
# - Learning activity metrics

# Neuromodulation analysis
./nlm_simulation --advanced-analysis --analysis-mode=neuromodulation
# Outputs:
# - Dopamine system activity
# - Curiosity and novelty metrics
# - Prediction error signals

# Developmental stage analysis
./nlm_simulation --advanced-analysis --analysis-mode=development
# Outputs:
# - Current developmental stage
# - Plasticity modifier trajectory
# - Structural plasticity rates
```

### 3.2 Benchmark Commands

```bash
# Performance benchmarking
./nlm_simulation --benchmark
# Outputs:
# - Memory usage statistics
# - Spike generation rate
# - Performance metrics

# Memory analysis
# (Brain memory: neurons * sizeof(Neuron) + synapses * sizeof(Synapse))
```

### 3.3 Configuration Export

```bash
# Export current configuration
./nlm_simulation --export-config
# Outputs:
# - JSON configuration file
# - Complete parameter summary
# - Source tracking (default, command-line, file)
```

### 3.4 Advanced Features Combination

```bash
# Enable advanced features for deep analysis
./nlm_simulation --advanced --experimental=all
# Outputs:
# - Checkpoint saving
# - Advanced analysis
# - Detailed logging

# Quick benchmark with experimental mode
./nlm_simulation --quick --benchmark
```

## 4. Configuration File Format

The configuration system supports JSON format:

```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001
}
```

## 5. Configuration Validation

The system automatically validates configuration parameters:

```bash
# Invalid neuron count (produces error)
./nlm_simulation --neurons=0
# Output: ERROR: Invalid neuron count: must be > 0

# Invalid timestep (produces error)
./nlm_simulation --timestep=-0.1
# Output: ERROR: Invalid simulation timestep: -0.1

# Valid configuration passes validation
./nlm_simulation --neurons=1000 --timestep=0.001
```

## 6. Performance Optimization

### For Large Scale Simulations

```bash
# Reduce memory usage
./nlm_simulation --neurons=100 --timestep=0.002

# Increase simulation speed
./nlm_simulation --neurons=2000 --no-tests

# Optimized for research
./nlm_simulation --experimental=plasticity --advanced --benchmark
```

### For Development and Testing

```bash
# Quick testing mode
./nlm_simulation --quick

# Minimal configuration for debugging
./nlm_simulation --neurons=100 --timestep=0.001 --no-tests
```

## 7. Advanced AgentBrain Features

### 7.1 Enhanced Neuron Categorization

The AgentBrain now uses efficient helper functions for neuron categorization:

```cpp
// Uses categorizeMotorNeurons helper function
categorizeMotorNeurons(motorForward_, motorBackward_, motorTurnLeft_,
                      motorTurnRight_, motorInteract_, motorWait_, allMotorNeurons);

// Uses categorizeSensoryNeurons helper function
categorizeSensoryNeurons(sensoryVision_, sensoryTouch_, sensoryInternal_,
                        sensoryProprioception_, allSensoryNeurons);
```

### 7.2 Advanced Motor Activity Calculation

```cpp
// Uses calculateMotorActivity helper function
float forwardAct = calculateMotorActivity(motorForward_);
float backwardAct = calculateMotorActivity(motorBackward_);
float leftAct = calculateMotorActivity(motorTurnLeft_);
```

### 7.3 Neuromodulation with Named Constants

```cpp
// Uses NeuromodulationConstants for configuration
expectedReward_ = NeuromodulationConstants::ExpectedRewardSmoothing * expectedReward_ +
                 (1.0f - NeuromodulationConstants::ExpectedRewardSmoothing) * reward;
```

## 8. Error Handling and Logging

### 8.1 Comprehensive Error Handling

```bash
# Brain initialization failure
./nlm_simulation
# Output:
# ERROR: Failed to initialize brain!
# Exit code: 3

# Configuration validation failure
./nlm_simulation --neurons=0
# Output:
# ERROR: Configuration validation failed!
# Exit code: 2
```

### 8.2 Detailed Logging

The system provides detailed logging at multiple levels:
- INFO: General information and progress
- WARNING: Potential issues or performance warnings
- ERROR: Critical errors that prevent execution

## 9. Script Examples

### 9.1 Automated Experiment Runner

```bash
#!/bin/bash
# run_experiments.sh - Automated experiment runner

for neurons in 100 500 1000 2000; do
    for timestep in 0.001 0.0005 0.002; do
        echo "Running experiment with $neurons neurons, timestep $timestep"
        ./nlm_simulation --neurons=$neurons --timestep=$timestep --experimental=all
    done
done
```

### 9.2 Parameter Sweep

```bash
#!/bin/bash
# parameter_sweep.sh - Parameter sweep for research

echo "Starting parameter sweep..."
echo "neurons,timestep,connectivity,plasticity"

for neurons in 500 1000 2000; do
    for timestep in 0.0005 0.001 0.002; do
        for conn_prob in 0.1 0.15 0.2; do
            # Run experiment
            ./nlm_simulation --neurons=$neurons --timestep=$timestep \
                           --connection=$conn_prob --experimental=all \
                           --benchmark --export-config
            
            # Save results
            cp checkpoint_final.bin "results/neurons_${neurons}_t_${timestep}_c_${conn_prob}.bin"
            cp exported_config.json "results/config_${neurons}_t_${timestep}_c_${conn_prob}.json"
        done
done
done

echo "Parameter sweep complete"
```

### 9.3 Advanced Analysis Script

```bash
#!/bin/bash
# analyze_results.sh - Post-experiment analysis

for checkpoint in checkpoints/*.bin; do
    echo "Analyzing $checkpoint"
    
    # Run advanced analysis
    ./nlm_simulation --benchmark --analysis-mode=connectivity \
                   --analysis-mode=plasticity --analysis-mode=neuromodulation
    
    # Export configuration
    ./nlm_simulation --export-config
    
    # Generate reports
    python3 generate_report.py "$checkpoint"
done
```

## 10. Programming Examples

### 10.1 Custom Configuration

```cpp
// Custom configuration example
struct CustomConfig {
    double timestep = SimConstants::DefaultTimestep;
    size_t neuronCount = SimConstants::DefaultNeuronCount;
    float connectionProb = SimConstants::DefaultConnectionProbability;
    
    // Custom validation
    bool isValid() const {
        return timestep > 0 && timestep <= 1.0 &&
               neuronCount > 0 && neuronCount <= 10000 &&
               connectionProb >= 0.0f && connectionProb <= 1.0f;
    }
};
```

### 10.2 Advanced Agent Usage

```cpp
// Advanced AgentBrain usage
class AdvancedAgent {
public:
    AdvancedAgent(std::shared_ptr<Brain> brain) : agentBrain_(brain) {}
    
    void runExperiment(const std::string& mode) {
        if (mode == "quick") {
            runQuickExperiment();
        } else if (mode == "deep") {
            runDeepAnalysis();
        } else if (mode == "custom") {
            runCustomAnalysis();
        }
    }
    
private:
    void runQuickExperiment() {
        // Enable only essential features
        agentBrain_->enableCuriosity(false);
        agentBrain_->enableDevelopment(false);
        // Run quick simulation
    }
    
    void runDeepAnalysis() {
        // Enable all features
        agentBrain_->enableRewardModulation(true);
        agentBrain_->enableStructuralPlasticity(true);
        agentBrain_->enableDevelopment(true);
        agentBrain_->enableCuriosity(true);
        // Run deep analysis
    }
    
    std::shared_ptr<AgentBrain> agentBrain_;
};
```

### 10.3 Performance Monitoring

```cpp
// Performance monitoring example
class PerformanceMonitor {
public:
    void startMonitoring() {
        startTime_ = std::chrono::high_resolution_clock::now();
        initialMemory_ = getCurrentMemoryUsage();
    }
    
    void endMonitoring() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        
        size_t finalMemory = getCurrentMemoryUsage();
        size_t memoryDelta = finalMemory - initialMemory_;
        
        NLM_LOG_INFO("Performance: " + std::to_string(duration.count()) + "ms, " +
                     std::to_string(memoryDelta / 1024) + "KB memory delta");
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    size_t initialMemory_ = 0;
};
```

## 11. Troubleshooting Advanced Features

### 11.1 Interactive Mode Issues

```bash
# If interactive mode doesn't work:
# 1. Ensure TUI dependencies are installed
# 2. Check terminal compatibility
# 3. Try: ./nlm_simulation --help | grep interactive
```

### 11.2 Advanced Analysis Problems

```bash
# If advanced analysis fails:
# 1. Check brain state with: ./nlm_simulation --benchmark
# 2. Validate configuration with: ./nlm_simulation --export-config
# 3. Try with simpler analysis mode: ./nlm_simulation --advanced-analysis --analysis-mode=connectivity
```

### 11.3 Performance Issues

```bash
# For slow performance:
# 1. Reduce neuron count: ./nlm_simulation --neurons=100
# 2. Increase timestep: ./nlm_simulation --timestep=0.002
# 3. Skip tests: ./nlm_simulation --no-tests
```

## 12. Next Steps

### For Advanced Users

1. **Read the full documentation**: `HOW_TO_USE.md`
2. **Study the architecture**: `docs/ARCHITECTURE.md`
3. **Experiment with configurations**: Try different parameter combinations
4. **Use advanced commands**: Leverage the new power user features
5. **Monitor performance**: Use benchmarking and analysis tools

### For Developers

1. **Study the Constants namespace**: `src/core/Constants.hpp`
2. **Understand the configuration system**: `src/core/Config/Config.hpp`
3. **Explore AgentBrain improvements**: `src/agent/AgentBrain.cpp` and `src/agent/AgentBrain.hpp`
4. **Review the test functions**: `runBasicConnectivityTest`, `runPlasticityExperiment`, `runStdpVerification`
5. **Contribute improvements**: Add more configuration options or analysis modes

### For Research

1. **Use parameter sweeps**: Create automated experiment runners
2. **Leverage checkpointing**: Save and load brain states for long experiments
3. **Export configurations**: Use JSON format for reproducible research
4. **Advanced analysis**: Use the comprehensive analysis modes
5. **Performance optimization**: Monitor and optimize simulation performance

---

## Summary

The NLM Phase 2 implementation provides significant improvements:

1. **Eliminated magic numbers** through named constants
2. **Enhanced command-line interface** with comprehensive options
3. **Improved error handling** with validation and recovery
4. **Advanced AgentBrain** with efficient algorithms and organization
5. **Power user commands** for deep analysis and customization
6. **Performance optimization** for large-scale simulations
7. **Comprehensive documentation** for all use cases

These improvements make NLM more accessible for beginners while providing powerful tools for advanced users and researchers.