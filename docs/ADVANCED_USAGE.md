# NLM (熙然) - Advanced Usage Guide

## Overview

This document provides comprehensive guidance for advanced usage of the NLM (熙然) neural simulation framework, including advanced configuration, performance optimization, and system integration techniques.

## 1. Advanced Configuration Management

### Custom Configuration Files

Create custom configuration files for specialized neural network setups:

```bash
# Create a custom configuration for large-scale neural networks
mkdir -p configs/custom
cat > configs/custom/large_network.cfg << EOF
# Large-scale neural network configuration
neuron_count = 5000
region_count = 5
connection_probability = 0.20
random_seed = 12345
simulation_timestep = 0.0005
simulation_duration = 60.0

# Enhanced plasticity
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.018
stdp_tau = 25.0
synaptogenesis_rate = 0.0002
pruning_rate = 0.00005

# Memory systems
working_memory_capacity = 1000
episodic_memory_capacity = 50000
associative_memory_capacity = 20000

# Neuromodulation
 dopamine_strength = 2.0
curiosity_strength = 1.5
novelty_threshold = 0.3

# Development
plasticity_decay_rate = 0.0005
EOF
```

### Advanced Configuration Validation

```bash
# Validate custom configuration
./nlm --config configs/custom/large_network.cfg

# View configuration details
./nlm --config configs/custom/large_network.cfg --verbose
```

### Configuration Templates

The framework includes configuration templates for different use cases:

```bash
# List available configuration templates
./nlm --list-tests

# View configuration schema
./nlm --help | grep -A 20 "Options:"
```

## 2. Performance Optimization

### Memory Management

For large-scale neural networks, use these memory optimization techniques:

```cpp
// Enable memory pool optimization
#include <nlm/core/performance/MemoryPool.hpp>

// Create memory pools for different object types
MemoryPool<Neuron> neuronPool(1000);
MemoryPool<Synapse> synapsePool(50000);

// Use pools in brain initialization
brain = std::make_shared<Brain>(config, &neuronPool, &synapsePool);
```

### Simulation Optimization

```bash
# Optimize for real-time simulation
./nlm --config optimized.cfg --verbose

# Typical optimized configuration
neuron_count = 2000
region_count = 3
connection_probability = 0.15
simulation_timestep = 0.001
enable_file_logging = true
log_file_path = "nlm_realtime.log"
```

### Parallel Processing

```cpp
// Enable parallel processing for complex simulations
#include <nlm/core/performance/ParallelProcessor.hpp>

ParallelProcessor processor;
processor.setThreadCount(std::thread::hardware_concurrency() - 1);

// Run simulation in parallel
processor.run([this](int threadId) {
    brain->step(threadId, threadId * timestep);
});
```

## 3. Advanced Test Suite

### Custom Test Creation

Create custom tests for specific neural phenomena:

```cpp
// Custom test class
class AdvancedPlasticityTest : public TestCase {
public:
    std::string getName() const override { return "Advanced Plasticity Test"; }
    std::string getCategory() const override { return "Advanced"; }
    std::string getDescription() const override { return "Test advanced plasticity mechanisms"; }
    
    TestResult run() override {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 200);
        config->set("stdp_ltp_weight", 0.03f);
        config->set("stdp_ltd_weight", 0.02f);
        
        auto brain = RobustConfigLoader::initializeBrainSafely(config);
        if (!brain) return TestResult::FAILED;
        
        // Enable specific plasticity patterns
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                // Enable only STDP for this test
                syn->enablePlasticity(false, true, false);
            }
        }
        
        // Apply specific activity patterns
        for (SimulationStep step = 0; step < 500; ++step) {
            // Pre-post spike patterns for LTP
            brain->injectCurrentToNeurons(NeuronType::Excitatory, 15.0f);
            brain->step(step, step * 0.001);
            
            // Post-pre spike patterns for LTD
            brain->step(step, step * 0.001);
            brain->injectCurrentToNeurons(NeuronType::Inhibitory, 10.0f);
        }
        
        // Analyze weight changes
        // Implementation depends on analysis requirements
        return TestResult::PASSED;
    }
};
```

### Performance Benchmarks

```cpp
// Performance benchmark suite
class PerformanceBenchmarks : public TestCase {
public:
    std::string getName() const override { return "Performance Benchmarks"; }
    std::string getCategory() const override { return "Performance"; }
    std::string getDescription() const override { return "Benchmark system performance"; }
    
    TestResult run() override {
        // Run multiple configurations with different scales
        std::vector<std::pair<int, double>> scales = {
            {100, 0.001},
            {500, 0.001},
            {1000, 0.0005},
            {2000, 0.0005}
        };
        
        double totalTime = 0.0;
        int totalSteps = 0;
        
        for (const auto& scale : scales) {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", scale.first);
            config->set("simulation_timestep", scale.second);
            
            auto brain = RobustConfigLoader::initializeBrainSafely(config);
            if (!brain) continue;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Run simulation
            for (SimulationStep step = 0; step < 1000; ++step) {
                RobustConfigLoader::stepBrainSafely(brain, step, step * scale.second);
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            double stepsPerSecond = 1000.0 / duration;
            NLM_LOG_INFO("Scale " + std::to_string(scale.first) + 
                        ": " + std::to_string(stepsPerSecond) + " steps/second");
            
            totalTime += duration;
            totalSteps += 1000;
        }
        
        double overallRate = static_cast<double>(totalSteps) / totalTime;
        NLM_LOG_INFO("Overall performance: " + std::to_string(overallRate) + " steps/second");
        
        return TestResult::PASSED;
    }
};
```

## 4. System Integration and Automation

### Automated Testing Pipeline

Create a testing pipeline script:

```bash
#!/bin/bash
# automated_test_pipeline.sh

set -e

echo "Starting NLM Automated Test Pipeline"

# Health check
echo "Running system health check..."
./nlm --help > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Error: System health check failed"
    exit 1
fi

# Run test suite with error collection
./nlm --test-suite --verbose > test_results.log 2>&1
TEST_RESULT=$?

# Save test results
./nlm --output test_summary.txt --verbose

# Analyze results
echo "Test pipeline completed with result: $TEST_RESULT"

# Generate comprehensive report
python3 -c "
import json
import re

# Parse test results
with open('test_results.txt', 'r') as f:
    content = f.read()

# Extract test counts
total_tests = re.search(r'Total Tests: (\d+)', content)
passed_tests = re.search(r'Passed: (\d+)', content)
failed_tests = re.search(r'Failed: (\d+)', content)

if total_tests and passed_tests and failed_tests:
    result = {
        'total_tests': int(total_tests.group(1)),
        'passed_tests': int(passed_tests.group(1)),
        'failed_tests': int(failed_tests.group(1)),
        'success_rate': float(passed_tests.group(1)) / float(total_tests.group(1)) * 100.0
    }
    
    with open('test_results.json', 'w') as f:
        json.dump(result, f, indent=2)
    
    print('Test results saved to test_results.json')
else:
    print('Warning: Could not parse test results')
"

echo "Pipeline completed successfully"
```

### Continuous Integration Integration

```yaml
# .github/workflows/nlm-ci.yml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    
    - name: Setup C++ environment
      uses: my-ci-actions/setup-cpp@v1
      with:
        cxx-compiler: g++
        cmake-version: 3.16
    
    - name: Build NLM
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    
    - name: Run tests
      run: |
        ./nlm --test-suite --verbose > test_output.txt 2>&1
        echo "Test Results:" >> test_output.txt
        ./nlm --version >> test_output.txt 2>&1
    
    - name: Upload test results
      uses: actions/upload-artifact@v2
      with:
        name: test-results
        path: test_output.txt
        retention-days: 30
    
    - name: Generate coverage report
      run: |
        # Generate code coverage report
        gcov -b -c src/brain/Brain.cpp > coverage_brain.txt
        gcov -b -c src/core/Config/Config.cpp > coverage_config.txt
    
    - name: Upload coverage
      uses: codecov/codecov-action@v1
      with:
        file: coverage_brain.txt
        flags: brain
    ```

## 5. Advanced Usage Examples

### Example 1: Complex Learning Task

```cpp
// Advanced learning task with neuromodulation
void advancedLearningTask() {
    // Create specialized configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 300);
    config->set("region_count", 2);
    config->set("dopamine_strength", 1.5);
    config->set("curiosity_strength", 2.0);
    config->set("plasticity_decay_rate", 0.001);
    
    auto brain = RobustConfigLoader::initializeBrainSafely(config);
    if (!brain) {
        std::cerr << "Failed to initialize brain for advanced learning task" << std::endl;
        return;
    }
    
    // Learning phase with goal-directed behavior
    for (SimulationStep step = 0; step < 2000; ++step) {
        // Phase 1: Exploration (curiosity-driven)
        if (step < 800) {
            brain->applyNeuromodulation(createNeuromodulator(NeuromodulatorType::Curiosity, 1.0f));
            // Random exploration behavior
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        // Phase 2: Exploitation (dopamine-driven)
        else if (step < 1600) {
            brain->applyNeuromodulation(createNeuromodulator(NeuromodulatorType::Dopamine, 1.5f));
            // Goal-directed behavior
            brain->produceAction();
        }
        // Phase 3: Consolidation (novelty-driven)
        else {
            brain->applyNeuromodulation(createNeuromodulator(NeuromodulatorType::Novelty, 0.8f));
            // Consolidate learned patterns
            brain->getEpisodicMemory()->consolidate();
        }
        
        RobustConfigLoader::stepBrainSafely(brain, step, step * config->getOr<double>("simulation_timestep", 0.001));
    }
    
    // Save learned behavior
    RobustConfigLoader::saveBrainSafely(brain, "advanced_learner.brain");
}
```

### Example 2: Real-time Simulation Interface

```cpp
// Real-time simulation with visualization
class RealTimeSimulator {
public:
    RealTimeSimulator() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("simulation_timestep", 0.001);
        config->set("enable_file_logging", true);
        config->set("log_file_path", "realtime_simulation.log");
        
        brain = RobustConfigLoader::initializeBrainSafely(config);
        if (!brain) {
            throw std::runtime_error("Failed to initialize brain for real-time simulation");
        }
        
        running = true;
        simulationThread = std::thread(&RealTimeSimulator::runSimulation, this);
    }
    
    void runSimulation() {
        while (running) {
            SimulationStep step = currentStep++;
            Timestamp time = step * timestep;
            
            // Update neural dynamics
            if (!RobustConfigLoader::stepBrainSafely(brain, step, time)) {
                std::cerr << "Simulation error at step " << step << std::endl;
                break;
            }
            
            // Generate output
            auto action = brain->produceAction();
            updateVisualization(action);
            
            // Control simulation speed
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    void stop() {
        running = false;
        if (simulationThread.joinable()) {
            simulationThread.join();
        }
    }
    
private:
    std::shared_ptr<Brain> brain;
    std::atomic<bool> running;
    std::thread simulationThread;
    SimulationStep currentStep = 0;
    Timestamp timestep = 0.001;
};
```

### Example 3: Batch Processing with Multiple Brains

```cpp
// Batch processing for comparative analysis
class BatchProcessor {
public:
    struct BatchResult {
        std::string configName;
        double performanceScore;
        std::vector<double> metrics;
        std::chrono::duration<double> processingTime;
    };
    
    std::vector<BatchResult> runBatch(const std::vector<std::string>& configFiles) {
        std::vector<BatchResult> results;
        
        for (const auto& configFile : configFiles) {
            std::cout << "Processing configuration: " << configFile << std::endl;
            
            auto config = RobustConfigLoader::loadConfiguration(configFile);
            if (!config) {
                std::cerr << "Failed to load configuration: " << configFile << std::endl;
                continue;
            }
            
            auto brain = RobustConfigLoader::initializeBrainSafely(config);
            if (!brain) {
                std::cerr << "Failed to initialize brain for: " << configFile << std::endl;
                continue;
            }
            
            // Run simulation
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (SimulationStep step = 0; step < 1000; ++step) {
                RobustConfigLoader::stepBrainSafely(brain, step, step * config->getOr<double>("simulation_timestep", 0.001));
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            
            // Calculate performance metrics
            BatchResult result;
            result.configName = configFile;
            result.processingTime = endTime - startTime;
            result.performanceScore = calculatePerformanceScore(brain);
            result.metrics = extractMetrics(brain);
            
            results.push_back(result);
            
            // Save intermediate results
            RobustConfigLoader::saveBrainSafely(brain, configFile + "_result.brain");
        }
        
        return results;
    }
    
private:
    double calculatePerformanceScore(std::shared_ptr<Brain> brain) {
        // Calculate a composite performance score
        double spikeRate = brain->getAverageFiringRate();
        double memoryUtilization = brain->getActiveNeuronCount() / 
                                 static_cast<double>(brain->getTotalNeuronCount());
        double plasticityActivity = brain->getTotalSpikeCount() / 1000.0; // Normalized
        
        return (spikeRate * 0.4) + (memoryUtilization * 0.3) + (plasticityActivity * 0.3);
    }
    
    std::vector<double> extractMetrics(std::shared_ptr<Brain> brain) {
        std::vector<double> metrics;
        metrics.push_back(brain->getTotalSpikeCount());
        metrics.push_back(brain->getAverageFiringRate());
        metrics.push_back(brain->getExcitationInhibitionRatio());
        metrics.push_back(static_cast<double>(brain->getActiveNeuronCount()));
        metrics.push_back(static_cast<double>(brain->getFiringNeuronCount()));
        return metrics;
    }
};
```

## 6. System Monitoring and Maintenance

### Performance Monitoring

```cpp
// System performance monitor
class SystemMonitor {
public:
    struct PerformanceStats {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        std::chrono::duration<double> simulationTime;
        size_t memoryAllocated;
        size_t neuronsActive;
    };
    
    PerformanceStats collectStatistics() {
        PerformanceStats stats;
        
        // CPU usage (platform-specific)
        stats.cpuUsage = getCPUUsage();
        
        // Memory usage (platform-specific)
        stats.memoryUsage = getMemoryUsage();
        
        // Disk usage
        stats.diskUsage = getDiskUsage(".");
        
        // Brain statistics
        if (brain) {
            stats.neuronsActive = brain->getActiveNeuronCount();
        }
        
        return stats;
    }
    
    void logPerformance(const PerformanceStats& stats) {
        std::stringstream ss;
        ss << "Performance Statistics:" << std::endl;
        ss << "  CPU Usage: " << stats.cpuUsage << "%" << std::endl;
        ss << "  Memory Usage: " << stats.memoryUsage << " MB" << std::endl;
        ss << "  Disk Usage: " << stats.diskUsage << "%" << std::endl;
        ss << "  Neurons Active: " << stats.neuronsActive << std::endl;
        
        NLM_LOG_INFO(ss.str());
    }
    
    bool checkSystemHealth() {
        return getCPUUsage() < 90.0 && getMemoryUsage() < 80.0 &&
               getDiskUsage(".") < 85.0;
    }
}; 
```

### Maintenance Tasks

```bash
#!/bin/bash
# maintenance.sh

set -e

echo "Starting NLM maintenance tasks"

# 1. Clean up old log files
find /workspace/app -name "*.log" -mtime +30 -delete

echo "Cleaned up old log files"

# 2. Compress old brain saves
find /workspace/app -name "*.brain" -mtime +7 -exec gzip {} \;

echo "Compressed old brain saves"

# 3. Validate configuration files
for config in configs/*; do
    if [ -f "$config" ]; then
        ./nlm --config "$config" --verbose 2>&1 | grep -q "Error:"
        if [ $? -eq 0 ]; then
            echo "Warning: Configuration $config contains errors"
        fi
    fi
done

echo "Validated configuration files"

# 4. Run health check
./nlm --help > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "Error: System health check failed"
    exit 1
fi

echo "Health check passed"

# 5. Generate performance report
python3 << 'EOF'
import json
from datetime import datetime

report = {
    'timestamp': datetime.now().isoformat(),
    'maintenance_completed': True,
    'system_status': 'healthy',
    'last_backup': 'manual',
    'config_validations': []
}

with open('system_report.json', 'w') as f:
    json.dump(report, f, indent=2)

print("Maintenance completed successfully")
EOF

echo "NLM maintenance completed"
```

## 7. Documentation and References

### Key Components

- **Core Systems**: Config, Logger, Random, SimulationClock, Types
- **Brain Architecture**: NeuralRegion, Neuron, Synapse, SpikeSystem
- **Plasticity Systems**: STDP, Hebbian, StructuralPlasticity
- **Memory Systems**: WorkingMemory, EpisodicMemory, AssociativeMemory
- **Prediction Systems**: PredictionSystem
- **Cognition Systems**: NeuralPlanner, ConceptFormation, AttentionalSelection
- **Development**: DevelopmentSystem
- **Neuromodulation**: Dopamine, Curiosity, Novelty, PredictionError
- **Performance**: MemoryPool, ParallelProcessor, CheckpointSystem

### Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)
- [Development Roadmap](docs/ROADMAP.md)
- [Experiment Framework](src/experiments/ExperimentRunner.hpp)
- [Python Bindings](python/)

### API Documentation

The NLM framework provides extensive C++ API documentation. For detailed API reference, consult:

```cpp
// Example API usage
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

// Create brain with configuration
auto config = std::make_shared<Config>();
config->set("neuron_count", 100);
auto brain = std::make_shared<Brain>(config);

// Initialize brain
if (brain->initialize()) {
    // Run simulation
    brain->step(0, 0.0);
}
```

### Getting Help

For advanced support and questions:

1. **GitHub Issues**: Report bugs and request features
2. **Research Support**: Contact NLM research team for scientific consultation
3. **Development Support**: Join NLM development community for technical discussions
4. **Documentation**: Check for updates in the documentation repository

## 8. Conclusion

This advanced usage guide provides comprehensive coverage of NLM's advanced capabilities, including:

- **Sophisticated Configuration Management**: Custom configurations and validation
- **Performance Optimization**: Memory management and parallel processing
- **Advanced Testing**: Custom test frameworks and benchmarks
- **System Integration**: Automation and continuous integration
- **Real-world Applications**: Complex learning tasks and real-time simulations
- **System Maintenance**: Monitoring and maintenance procedures
- **Documentation and Support**: Comprehensive references and support channels

The NLM framework continues to evolve with new features and improvements. For the latest documentation and updates, visit the NLM documentation repository and engage with the NLM research community.

---

*This document is continuously updated to reflect the latest NLM capabilities and best practices. For the most current version, consult the official NLM documentation repository.*

---

**License**: MIT
**Version**: 2.0.0 (Phase 2: Real Neural Computation)
**Last Updated**: October 2026

---

**Note**: This is an experimental research project. The NLM framework is intended for scientific research and development of neural-inspired computational systems. No claims are made regarding biological accuracy or commercial viability.
