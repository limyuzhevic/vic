# Phase 6 Integration Benchmark

This benchmark provides more comprehensive testing of the integrated brain systems with different configurations and edge cases.

## Benchmark 1: Standard Integration Test

This test runs with default settings to verify basic integration:

```cpp
#include "Phase6IntegratedExperiment.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 6 Integration Benchmark ===" << std::endl << std::endl;
    
    Phase6IntegratedExperiment experiment;
    
    // Test basic integration verification
    std::cout << "1. Running basic integration verification..." << std::endl;
    if (experiment.verifyIntegration()) {
        std::cout << "   ✓ Integration verification PASSED" << std::endl;
    } else {
        std::cout << "   ✗ Integration verification FAILED" << std::endl;
        return 1;
    }
    
    // Test memory integration
    std::cout << "2. Testing memory integration..." << std::endl;
    if (experiment.testMemoryIntegration()) {
        std::cout << "   ✓ Memory integration PASSED" << std::endl;
    } else {
        std::cout << "   ✗ Memory integration FAILED" << std::endl;
    }
    
    // Test neuromodulation integration
    std::cout << "3. Testing neuromodulation integration..." << std::endl;
    if (experiment.testNeuromodulationIntegration()) {
        std::cout << "   ✓ Neuromodulation integration PASSED" << std::endl;
    } else {
        std::cout << "   ✗ Neuromodulation integration FAILED" << std::endl;
    }
    
    // Test checkpointing
    std::cout << "4. Testing checkpointing..." << std::endl;
    if (experiment.testCheckpointing()) {
        std::cout << "   ✓ Checkpointing PASSED" << std::endl;
    } else {
        std::cout << "   ✗ Checkpointing FAILED" << std::endl;
    }
    
    // Test replay
    std::cout << "5. Testing replay system..." << std::endl;
    if (experiment.testReplay()) {
        std::cout << "   ✓ Replay system PASSED" << std::endl;
    } else {
        std::cout << "   ✗ Replay system FAILED" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Benchmark Complete ===" << std::endl;
    
    return 0;
}
```

## Benchmark 2: Stress Test

This test uses larger configurations to stress the integration:

```cpp
Phase6Config stressConfig;
stressConfig.neuronCount = 2000;
stressConfig.maxSteps = 5000;
stressConfig.enableCheckpointing = true;
stressConfig.enableReplay = true;
stressConfig.enableDevelopment = true;
stressConfig.checkpointPath = "./stress_checkpoint.bin";

auto result = experiment.run(stressConfig);

std::cout << "=== Stress Test Results ===" << std::endl;
std::cout << "Total reward: " << result.totalReward << std::endl;
std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
std::cout << "Working Memory integrated: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
std::cout << "Episodic Memory integrated: " << (result.memoryEpisodicMemoryIntegrated ? "Yes" : "No") << std::endl;
std::cout << "Neuromodulation integrated: " << (result.neuromodulationIntegrated ? "Yes" : "No") << std::endl;
std::cout << "Prediction integrated: " << (result.predictionIntegrated ? "Yes" : "No") << std::endl;
std::cout << "Development integrated: " << (result.developmentIntegrated ? "Yes" : "No") << std::endl;
std::cout << "Checkpointing works: " << (result.checkpointingWorks ? "Yes" : "No") << std::endl;
std::cout << "Replay works: " << (result.replayWorks ? "Yes" : "No") << std::endl;
```

## Benchmark 3: Edge Case Testing

Test with minimal and edge configurations:

```cpp
// Minimal configuration
Phase6Config minimalConfig;
minimalConfig.neuronCount = 50;
minimalConfig.maxSteps = 100;
minimalConfig.enableCheckpointing = false; // Disable to test minimal setup
minimalConfig.enableReplay = false;
minimalConfig.enableDevelopment = false;

// Edge case: High plasticity
Phase6Config edgeConfig;
edgeConfig.neuronCount = 200;
edgeConfig.maxSteps = 1000;
edgeConfig.connectionProbability = 0.5f; // Higher connection density
edgeConfig.enableCheckpointing = true;
edgeConfig.enableReplay = true;
edgeConfig.enableDevelopment = true;
```

## Benchmark 4: Integration with External Components

Test integration with external world models and agent behaviors:

```cpp
// Create custom world with obstacles
// Create agent with specific behaviors
// Run complex behaviors and test integration
```

## Performance Monitoring

The benchmark includes detailed performance monitoring:

```cpp
try {
    // All operations that can throw exceptions
    experiment.verifyIntegration();
    experiment.testMemoryIntegration();
    // ... etc
} catch (const std::bad_alloc& e) {
    std::cout << "MEMORY ERROR: " << e.what() << std::endl;
    return 1;
} catch (const std::runtime_error& e) {
    std::cout << "RUNTIME ERROR: " << e.what() << std::endl;
    return 1;
} catch (...) {
    std::cout << "UNKNOWN ERROR: Benchmark failed" << std::endl;
    return 1;
}
```

## Integration Status Summary

After running all benchmarks, you can generate a comprehensive integration report:

```cpp
struct IntegrationReport {
    std::string timestamp;
    int totalBenchmarks;
    int passedBenchmarks;
    float averagePerformance;
    std::vector<std::string> failedTests;
    std::map<std::string, float> systemPerformance;
};

IntegrationReport generateReport() {
    // Collect results from all benchmarks
    // Calculate performance metrics
    // Generate detailed report
}
```

This benchmark suite provides comprehensive testing of the Phase 6 integration, ensuring all systems work together reliably under various conditions.