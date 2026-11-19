# Advanced NLM Usage Examples

This document provides practical examples for using the NLM advanced features.

## Table of Contents

1. [Basic Advanced Demo Usage](#basic-advanced-demo-usage)
2. [Custom World Configuration Examples](#custom-world-configuration-examples)
3. [Advanced Analysis Examples](#advanced-analysis-examples)
4. [Command Line Configuration Examples](#command-line-configuration-examples)
5. [Performance Testing Examples](#performance-testing-examples)
6. [Reproducibility Examples](#reproducibility-examples)

## Basic Advanced Demo Usage

### 1. Running the Advanced Demo

The simplest way to run the advanced demo is:

```bash
./nlm_advanced_demo --advanced
```

This will run the advanced demo with default configuration.

### 2. Custom Configuration

To run with custom configuration:

```bash
./nlm_advanced_demo --advanced --seed 12345 --output my_results.txt
```

### 3. High Complexity Testing

For testing with high complexity:

```bash
./nlm_advanced_demo --advanced --high-complexity --max-steps 10000
```

## Custom World Configuration Examples

### 1. Small World Configuration

Configure a small world for fast testing:

```cpp
AdvancedSimpleWorld world;
world.configureAdvanced(10.0f, 10.0f, 0.5f); // Small world, medium complexity
```

**Result:**
- World size: 10.0m
- Complexity level: 0.5
- Object count: ~12
- Resource value: 15.0
- Hazard value: -25.0

### 2. Large World Configuration

Configure a large world for realistic testing:

```cpp
AdvancedSimpleWorld world;
world.configureAdvanced(30..RedirectMany, 30.0f, 1.0f); // Large world, high complexity
```

**Result:**
- World size: 30.0m
- Complexity level: 1.0
- Object count: ~20
- Resource value: 25.0
- Hazard value: -35.0

### 3. Low Complexity World

Configure a low complexity world for debugging:

```cpp
AdvancedSimpleWorld world;
world.configureAdvanced(15.0f, 15.0f, 0.2f); // Medium world, low complexity
```

**Result:**
- World size: 15.0m
- Complexity level: 0.2
- Object count: ~10
- Resource value: 12.0
- Hazard value: -22.0

## Advanced Analysis Examples

### 1. Basic Analysis

Perform basic analysis on simulation results:

```cpp
// Generate synthetic data for demonstration
std::vector<float> rewards = {10.0f, 5.0f, 0.0f, -5.0f, 15.0f};
std::vector<float> firingRates = {12.5f, 15.2f, 8.1f, 20.3f, 18.7f};
std::vector<float> learningSignals = {0.1f, 0.3f, 0.05f, 0.2f, 0.15f};

// Analyze results
AnalysisResult result = AdvancedAnalysisTools::analyzeResults(
    rewards, firingRates, learningSignals);

// Display results
std::cout << "Total Reward: " << result.totalReward << std::endl;
std::cout << "Average Firing Rate: " << result.avgFiringRate << std::endl;
std::cout << "Steps with Learning: " << result.stepsWithLearning << std::endl;
```

### 2. Detailed Analysis

Perform detailed analysis with more data points:

```cpp
// Generate more realistic data
std::vector<float> rewards;
std::vector<float> firingRates;
std::vector<float> learningSignals;

// Simulate 1000 steps
for (size_t i = 0; i < 1000; ++i) {
    // Add noise to simulate realistic behavior
    rewards.push_back(10.0f + (rand() % 21 - 10)); // -10 to +10
    firingRates.push_back(15.0f + (rand() % 21 - 10)); // 5 to 25
    learningSignals.push_back(rand() / (RAND_MAX / 0.5)); // 0 to 0.5
}

// Analyze results
AnalysisResult result = AdvancedAnalysisTools::analyzeResults(
    rewards, firingRates, learningSignals);

// Export detailed results
AdvancedAnalysisTools::exportResults(result, "detailed_results.txt");
```

### 3. Comparative Analysis

Compare results from different configurations:

```cpp
struct ExperimentConfig {
    std::string name;
    AdvancedSimpleWorld world;
    AdvancedPhase6Config config;
    AnalysisResult result;
};

std::vector<ExperimentConfig> experiments;

// Run experiment 1
ExperimentConfig exp1;
exp1.name = "Small World";
exp1.world.configureAdvanced(10.0f, 10.0f, 0.5f);
exp1.config.maxSteps = 1000;
exp1.config.neuronCount = 500;
exp1.result = runExperiment(exp1.world, exp1.config);
experiments.push_back(exp1);

// Run experiment 2
ExperimentConfig exp2;
exp2.name = "Large World";
exp2.world.configureAdvanced(30.0f, 30.0f, 1.0f);
exp2.config.maxSteps = 2000;
exp2.config.neuronCount = 1000;
exp2.result = runExperiment(exp2.world, exp2.config);
experiments.push_back(exp2);

// Compare results
std::cout << "Comparative Analysis:" << std::endl;
for (const auto& exp : experiments) {
    std::cout << "Experiment: " << exp.name << std::endl;
    std::cout << "  Total Reward: " << exp.result.totalReward << std::endl;
    std::cout << "  Average Firing Rate: " << exp.result.avgFiringRate << std::endl;
    std::cout << "  Exploration Efficiency: " << exp.result.explorationEfficiency << std::endl;
    std::cout << "  Reward Consistency: " << exp.result.rewardConsistency << std::endl;
    std::cout << std::endl;
}
```

## Command Line Configuration Examples

### 1. Basic Configuration

```bash
./nlm_advanced_demo --advanced
```

### 2. Custom Seed

Set a custom seed for reproducible results:

```bash
./nlm_advanced_demo --advanced --seed 42
```

### 3. Custom Output

Specify output file:

```bash
./nlm_advanced_demo --advanced --output results.txt
```

### 4. No Logging

Disable logging for cleaner output:

```bash
./nlm_advanced_demo --advanced --no-logging
```

### 5. High Complexity with Custom Settings

Combine multiple options:

```bash
./nlm_advanced_demo --advanced --high-complexity --seed 12345 --output my_results.txt --max-steps 5000 --neuron-count 800
```

### 6. Performance Testing Configuration

Configure for performance testing:

```bash
./nlm_advanced_demo --advanced --neuron-count 200 --exploration-rate 0.1 --learning-rate 0.02 --max-steps 10000
```

## Performance Testing Examples

### 1. Speed Testing

Test performance with different world sizes:

```cpp
void benchmarkWorldSizes() {
    std::vector<float> sizes = {10.0f, 20.0f, 30.0f};
    
    for (float size : sizes) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        AdvancedSimpleWorld world;
        world.configureAdvanced(size, size, 0.5f);
        
        // Run experiment
        AdvancedPhase6Config config;
        config.maxSteps = 1000;
        config.neuronCount = 500;
        
        AnalysisResult result = runExperiment(world, config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "World Size: " << size << "m" << std::endl;
        std::cout << "  Time: " << duration.count() << "s" << std::endl;
        std::cout << "  Reward: " << result.totalReward << std::endl;
        std::cout << "  Performance: " << result.totalReward / duration.count() << " reward/s" << std::endl;
        std::cout << std::endl;
    }
}
```

### 1. Neuron Count Testing

Test with different neuron counts:

```cpp
void benchmarkNeuronCounts() {
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    
    for (size_t neurons : neuronCounts) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        AdvancedSimpleWorld world;
        world.configureAdvanced(20.0f, 20.0f, 1.0f);
        
        AdvancedPhase6Config config;
        config.maxSteps = 1000;
        config.neuronCount = neurons;
        
        AnalysisResult result = runExperiment(world, config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "Neuron Count: " << neurons << std::endl;
        std::cout << "  Time: " << duration.count() << "s" << std::endl;
        std::cout << "  Reward: " << result.totalReward << std::endl;
        std::cout << "  Performance: " << result.totalReward / duration.count() << " reward/s" << std::endl;
        std::cout << std::endl;
    }
}
```

### 1. Learning Rate Testing

Test with different learning rates:

```cpp
void benchmarkLearningRates() {
    std::vector<float> learningRates = {0.01f, 0.05f, 0.1f, 0.2f};
    
    for (float rate : learningRates) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        AdvancedSimpleWorld world;
        world.configureAdvanced(20.0f, 20.0f, 1.0f);
        
        AdvancedPhase6Config config;
        config.maxSteps = 1000;
        config.learningRate = rate;
        
        AnalysisResult result = runExperiment(world, config);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << "Learning Rate: " << rate << std::endl;
        std::cout << "  Time: " << duration.count() << "s" << std::endl;
        std::cout << "  Reward: " << result.totalReward << std::endl;
        std::cout << "  Performance: " << result.totalReward / duration.count() << " reward/s" << std::endl;
        std::cout << "  Reward Consistency: " << result.rewardConsistency << std::endl;
        std::cout << std::endl;
    }
}
```

## Reproducibility Examples

### 1. Basic Reproducibility

Ensure reproducible results by setting a seed:

```cpp
void ensureReproducibility() {
    // Set seed for reproducible results
    srand(42);
    
    // Run experiment
    AdvancedSimpleWorld world1;
    world1.configureAdvanced(20.0f, 20.0f, 1.0f);
    
    AdvancedPhase6Config config;
    config.maxSteps = 1000;
    config.customSeed = 42;
    
    AnalysisResult result1 = runExperiment(world1, config);
    
    // Run experiment again with same seed
    srand(42);
    
    AdvancedSimpleWorld world2;
    world2.configureAdvanced(20.0f, 20.0f, 1.0f);
    
    AnalysisResult result2 = runExperiment(world2, config);
    
    // Results should be identical
    if (result1.totalReward == result2.totalReward) {
        std::cout << "✓ Results are reproducible" << std::endl;
    } else {
        std::cout << "✗ Results are not reproducible" << std::endl;
    }
}
```

### 2. File-based Reproducibility

Save and load configuration for reproducibility:

```cpp
void saveAndLoadConfiguration() {
    // Create configuration
    AdvancedPhase6Config config;
    config.maxSteps = 1000;
    config.neuronCount = 500;
    config.customSeed = 12345;
    
    // Save configuration
    std::ofstream configFile("config.txt");
    if (configFile.is_open()) {
        configFile << "maxSteps: " << config.maxSteps << std::endl;
        configFile << "neuronCount: " << config.neuronCount << std::endl;
        configFile << "customSeed: " << config.customSeed << std::endl;
        configFile.close();
    }
    
    // Load configuration
    AdvancedPhase6Config loadedConfig;
    std::ifstream configFile("config.txt");
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            if (line.find("maxSteps:") != std::string::npos) {
                loadedConfig.maxSteps = std::stoi(line.substr(10));
            } else if (line.find("neuronCount:") != std::string::npos) {
                loadedConfig.neuronCount = static_cast<size_t>(std::stoi(line.substr(13)));
            } else if (line.find("customSeed:") != std::string::npos) {
                loadedConfig.customSeed = static_cast<uint64_t>(std::stoi(line.substr(11)));
            }
        }
        configFile.close();
    }
    
    // Run experiment with loaded configuration
    AdvancedSimpleWorld world;
    world.configureAdvanced(20.0f, 20.0f, 1.0f);
    
    AnalysisResult result = runExperiment(world, loadedConfig);
    
    std::cout << "Experiment with loaded configuration:" << std::endl;
    std::cout << "  Max Steps: " << result.totalReward << std::endl;
    std::cout << "  Neuron Count: " << result.avgFiringRate << std::endl;
}
```

### 1. Checkpoint-based Reproducibility

Use checkpointing to ensure reproducibility:

```cpp
void useCheckpointingForReproducibility() {
    // Run experiment and save checkpoint
    AdvancedSimpleWorld world1;
    world1.configureAdvanced(20.0f, 20.0f, 1.0f);
    
    AdvancedPhase6Config config;
    config.maxSteps = 1000;
    config.enableCheckpointing = true;
    config.checkpointPath = "checkpoint.nml";
    
    AnalysisResult result1 = runExperiment(world1, config);
    
    // Save checkpoint
    saveCheckpoint(world1, config.checkpointPath);
    
    // Load checkpoint and run again
    AdvancedSimpleWorld world2;
    world2.configureAdvanced(20.0f, 20.0f, 1.0f);
    
    AnalysisResult result2 = runExperiment(world2, config);
    loadCheckpoint(world2, config.checkpointPath);
    
    // Results should be identical (except for time)
    if (result1.totalReward == result2.totalReward) {
        std::cout << "✓ Checkpoint-based results are reproducible" << std::endl;
    } else {
        std::cout << "✗ Checkpoint-based results are not reproducible" << std::endl;
ěn
    }
}
```

## Building and Running Examples

### 1. Build with Advanced Features

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Run Basic Examples

```bash
# Basic advanced demo
./nlm_advanced_demo --advanced

# Custom configuration
./nlm_advanced_demo --advanced --seed 42 --output results.txt

# High complexity
./nlm_advanced_demo --advanced --high-complexity
```

### 3. Run Original Demo (Backward Compatibility)

```bash
# Original Phase 6 demo (for backward compatibility)
./nlm_phase6_demo
```

### 4. Run Phase 6 Text Demo

```bash
# Phase 6 text demo (for advanced text-based features)
./nlm_phase6_text_demo
```

## Usage Summary

### Common Commands

```bash
# Basic usage
./nlm_advanced_demo

# Custom configuration
./nlm_advanced_demo --advanced --seed 12345 --output my_results.txt

# High complexity testing
./nlm_advanced_demo --advanced --high-complexity

# Performance testing
./nlm_advanced_demo --advanced --neuron-count 200 --exploration-rate 0.1 --learning-rate 0.02

# Reproducibility testing
./nlm_advanced_demo --advanced --seed 42
```

### File Operations

```bash
# Generate analysis report
./nlm_advanced_demo --advanced --output analysis_report.txt

# Export detailed results
./nlm_advanced_demo --advanced --output detailed_results.txt

# Save configuration
# Use the export functionality from AdvancedAnalysisTools
```

## Conclusion

The NLM advanced features provide powerful tools for:

1. **Testing**: Custom world configurations and complexity levels
2. **Analysis**: Detailed performance metrics and result export
3. **Reproducibility**: Seed-based and checkpoint-based reproducibility
4. **Performance**: Benchmarking and optimization tools
5. **Documentation**: Comprehensive guides and examples

These features make it easier to:
- Test different scenarios efficiently
- Analyze results thoroughly
- Reproduce experiments reliably
- Optimize performance
- Document and share findings

## Next Steps

1. **Run the advanced demo**: `./nlm_advanced_demo --advanced`
2. **Explore configuration options**: Check the command-line options
3. **Create custom experiments**: Use the advanced analysis tools
4. **Document findings**: Use the result export functionality
5. **Share with others**: Use the comprehensive documentation

The NLM advanced features provide a complete toolkit for advanced users, researchers, and developers to work with the neural learning machine system effectively and efficiently.
