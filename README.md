# NLM — 熙然

**An Experimental Artificial Developmental Brain**

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the substrate level: populations now actually create neurons, random connectivity creates ~37k synapses for a 500-neuron brain (was 0), spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000 steps), and a controlled STDP test potentiates its synapse (0.35 -> 0.99). Inhibitory neurons now exist (20% of internal population) and inhibitory plasticity keeps its sign instead of flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6 demos still report 0 reward, and checkpointing does not work (it is a stub that honestly returns false). Activity is still very high (~200 spikes/step) and learning in the main demo is mostly saturation, not selective pattern learning. Those are the next problems, not hidden ones.

## Overview

NLM is a sophisticated research project implementing a complete artificial developmental brain with the following key features:

### Core Architecture
- **Real Spiking Neural Computation**: Leaky Integrate-and-Fire (LIF) neurons with event-driven dynamics
- **Multi-Level Learning**: STDP, Hebbian, reward-modulated plasticity
- **Integrated Memory Systems**: Working memory, episodic memory, associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, prediction error
- **Cognitive Systems**: Neural planning, concept formation, attention
- **Developmental System**: Age-dependent plasticity and maturation

### Phase-Based Development
- **Phase 2**: Basic neural computation and spiking dynamics
- **Phase 3**: World interaction and experience -> learning -> action loop
- **Phase 4**: Advanced cognition including concept formation and planning
- **Phase 5**: Performance optimizations and personality system
- **Phase 6**: Complete integration of all systems

## Key Improvements in This Version

### 1. Enhanced Error Handling and Validation
- Comprehensive error detection throughout the codebase
- Graceful error recovery and fallback mechanisms
- Detailed error logging and reporting
- State validation for all brain components

### 2. Advanced CLI Commands
- Enhanced demo executables with extensive command-line options
- Performance profiling and monitoring tools
- Checkpoint management and export capabilities
- Real-time analytics and debugging

### 3. Improved Build System
- Enhanced CMake configuration with better optimization
- Automatic detection of development tools and dependencies
- Enhanced Python bindings with better error handling
- Comprehensive test framework integration

### 4. Advanced Debugging and Profiling
- Real-time performance monitoring
- Memory usage tracking and optimization
- Spike event analysis and visualization
- System health monitoring

### 5. Enhanced Documentation
- Comprehensive API documentation
- Detailed examples and tutorials
- Performance optimization guides
- Best practices and common pitfalls

## Building

### Prerequisites
- CMake 3.16 or higher
- C++20 compliant compiler (GCC 11+, Clang 14+, MSVC 19+)
- Python 3.8+ (for Python bindings)
- Pybind11 (for Python bindings)

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Build Options
```bash
cmake .. \
    -DNLM_BUILD_PYTHON_BINDINGS=ON \
    -DNLM_ENABLE_ADVANCED_TESTING=ON \
    -DNLM_ENABLE_PERFORMANCE_TESTS=OFF \
    -DNLM_ENABLE_INTEGRATION_TESTS=ON \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo
```

### Build Targets
- `nlm` - Phase 2 demo (basic neural computation)
- `nlm_phase3_demo` - Phase 3 demo (world interaction)
- `nlm_phase4_demo` - Phase 4 demo (advanced cognition)
- `nlm_phase5_demo` - Phase 5 demo (performance optimizations)
- `nlm_phase6_demo` - Phase 6 demo (complete integration)
- `nlm_phase6_text_demo` - Phase 6 text demo (text-as-sense processing)
- `nlm_benchmark` - Performance benchmark
- `pynlm` - Python bindings (if enabled)

## Running

### Basic Demos
```bash
# Phase 2: Basic neural computation
./nlm

# Phase 3: World interaction
./nlm_phase3_demo

# Phase 4: Advanced cognition
./nlm_phase4_demo

# Phase 5: Performance optimizations
./nlm_phase5_demo

# Phase 6: Complete integration
./nlm_phase6_demo

# Phase 6: Text processing
./nlm_phase6_text_demo
```

### Advanced Usage
```bash
# Run with advanced commands
./nlm_phase6_demo --help
./nlm_phase6_demo --debug --profiler --monitor
./nlm_phase6_demo --iterations 5000 --neurons 1000 --export results.json
```

### Python Bindings
```python
import pynlm

# Create a brain with configuration
brain = pynlm.Brain({
    'neuron_count': 500,
    'region_count': 4,
    'connection_probability': 0.1,
    'simulation_timestep': 0.001,
})

# Initialize and run simulation
brain.initialize()
for i in range(100):
    brain.step(i)

# Access neural properties
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Advanced Features

### 1. Enhanced Error Handling
The NLM framework includes comprehensive error handling:

```cpp
// Error handling in action
if (!brain.initialize()) {
    auto errorCode = brain.getLastErrorCode();
    std::string errorMsg = brain.getErrorMessage();
    std::cerr << "Brain initialization failed: " << errorMsg << std::endl;
}

// Validation and health checks
Brain::ValidationResult validation = brain.validate();
if (!validation.isValid()) {
    std::cerr << "Validation failed:" << std::endl;
    for (const auto& error : validation.errors) {
        std::cerr << "  - " << error << std::endl;
    }
}
```

### 2. Performance Monitoring
Real-time performance monitoring is available:

```bash
# Run with performance monitoring
./nlm_phase6_demo --monitor

# Export performance metrics
./nlm_phase6_demo --export performance.csv --profiler
```

### 3. Advanced CLI Commands
Enhanced CLI commands provide advanced functionality:

```bash
# Help and documentation
./nlm_phase6_demo --help
./nlm_phase6_demo --help-advanced

# Configuration and export
./nlm_phase6_demo --iterations 2000 --neurons 750 --export results.json
./nlm_phase6_demo --checkpoint save.brain --iterations 1000

# Debugging and validation
./nlm_phase6_demo --debug --validate --profiler
./nlm_phase6_demo --development --replay --checkpointing
```

### 4. Python Bindings Enhancements
Enhanced Python bindings with advanced features:

```python
import pynlm
import numpy as np

# Create brain with advanced configuration
config = {
    'neuron_count': 1000,
    'region_count': 8,
    'connection_probability': 0.15,
    'simulation_timestep': 0.0005,
    'random_seed': 42,
    'stdp_ltp_weight': 0.01,
    'stdp_ltd_weight': 0.012,
    'synaptogenesis_rate': 0.0001,
    'pruning_rate': 0.00001,
}

brain = pynlm.Brain(config)
brain.initialize()

# Advanced usage with error handling
try:
    brain.receiveSensoryInput(pynlm.SensoryInput(np.array([1.0, 0.5, 0.0])))
    action = brain.produceAction()
    print(f"Produced action: {action}")
except Exception as e:
    print(f"Error: {e}")
```

## Advanced Examples

### 1. Learning and Adaptation
```cpp
// Advanced learning example demonstrating neuromodulation
class LearningExperiment {
private:
    std::shared_ptr<nlm::Brain> brain;
    nlm::Phase6IntegratedExperiment experiment;
    
public:
    LearningExperiment() {
        nlm::Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = 5000;
        config.enableDevelopment = true;
        config.enableReplay = true;
        config.enableCheckpointing = true;
        
        brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
        experiment = nlm::Phase6IntegratedExperiment(*brain);
    }
    
    void runLearningExperiment() {
        std::cout << "=== Advanced Learning Experiment ===" << std::endl;
        
        // Enable performance monitoring
        brain->enablePerformanceMonitoring();
        
        // Run learning with comprehensive validation
        auto result = experiment.run(config);
        
        // Analyze learning outcomes
        analyzeLearningResults(result);
        
        // Export results for analysis
        exportResults(result, "learning_experiment.json");
    }
    
    void analyzeLearningResults(const nlm::Phase6Result& result) {
        std::cout << "Learning Analysis:" << std::endl;
        std::cout << "  Total Reward: " << result.totalReward << std::endl;
        std::cout << "  Learning Rate: " << (result.maxSteps > 0 ? result.totalReward / result.maxSteps : 0.0) << std::endl;
        std::cout << "  Memory Integration: " << (result.memoryWorkingMemoryIntegrated ? "Yes" : "No") << std::endl;
        std::cout << "  Neuromodulation Effectiveness: " << (result.dopamineLevel > 0.0 ? "Effective" : "Ineffective") << std::endl;
        
        // Check for learning saturation
        float learningEfficiency = result.totalReward / (result.maxSteps * result.avgFiringRate);
        std::cout << "  Learning Efficiency: " << learningEfficiency << std::endl;
        
        if (learningEfficiency < 0.01) {
            std::cout << "  WARNING: Low learning efficiency detected" << std::endl;
        }
    }
    
    void exportResults(const nlm::Phase6Result& result, const std::string& filename) {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "NLM Learning Experiment Results" << std::endl;
            file << "===============================" << std::endl;
            file << std::endl;
            file << "Performance Metrics:" << std::endl;
            file << "  Total Reward: " << result.totalReward << std::endl;
            file << "  Average Firing Rate: " << result.avgFiringRate << " Hz" << std::endl;
            file << "  Memory Episodes: " << result.memoryEpisodesStored << std::endl;
            file << "  Dopamine Level: " << result.dopamineLevel << std::endl;
            file << std::endl;
            
            file << "System Integration:" << std::endl;
            file << "  Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "✓" : "✗") << std::endl;
            file << "  Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "✓" : "✗") << std::endl;
            file << "  Neuromodulation: " << (result.neuromodulationIntegrated ? "✓" : "✗") << std::endl;
            file << "  Prediction System: " << (result.predictionIntegrated ? "✓" : "✗") << std::endl;
            file << "  Development: " << (result.developmentIntegrated ? "✓" : "✗") << std::endl;
            
            file.close();
            std::cout << "Results exported to: " << filename << std::endl;
        }
    }
};
```

### 2. Performance Optimization
```cpp
// Performance optimization example demonstrating advanced profiling
class PerformanceOptimizer {
private:
    std::shared_ptr<nlm::Brain> brain;
    nlm::Phase6IntegratedExperiment experiment;
    
public:
    PerformanceOptimizer() {
        nlm::Phase6Config config;
        config.neuronCount = 2000;  // Larger network for performance testing
        config.maxSteps = 10000;
        config.enableDevelopment = true;
        
        brain = std::make_shared<nlm::Brain>(std::make_shared<nlm::Config>(config));
        experiment = nlm::Phase6IntegratedExperiment(*brain);
    }
    
    void optimizePerformance() {
        std::cout << "=== Performance Optimization Analysis ===" << std::endl;
        
        // Enable comprehensive performance monitoring
        brain->enablePerformanceMonitoring();
        brain->enableDebugOutput(true);
        
        // Run performance benchmark
        auto startTime = std::chrono::high_resolution_clock::now();
        auto result = experiment.run(config);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        result.totalWallClockTime = duration.count();
        
        // Analyze performance
        analyzePerformance(result);
        
        // Generate optimization recommendations
        generateOptimizationRecommendations(result);
    }
    
    void analyzePerformance(const nlm::Phase6Result& result) {
        std::cout << "Performance Analysis:" << std::endl;
        std::cout << "  Total Steps: " << result.maxSteps << std::endl;
        std::cout << "  Total Wall Time: " << result.totalWallClockTime << "s" << std::endl;
        std::cout << "  Steps per Second: " << (result.totalWallClockTime > 0 ? result.maxSteps / result.totalWallClockTime : 0.0) << std::endl;
        std::cout << "  Average Step Duration: " << (result.maxSteps > 0 ? result.totalWallClockTime / result.maxSteps : 0.0) << "s" << std::endl;
        
        // Memory usage analysis
        size_t memoryUsage = brain->getMemoryUsage();
        std::cout << "  Memory Usage: " << memoryUsage << " bytes" << std::endl;
        std::cout << "  Neurons per MB: " << ((memoryUsage / 1024.0 / 1024.0) / result.neuronCount) << std::endl;
        
        // Performance bottlenecks detection
        if (result.totalWallClockTime > 60.0) {
            std::cout << "  WARNING: Long execution time detected (>60s)" << std::endl;
        }
        
        if (result.avgFiringRate > 100.0) {
            std::cout << "  WARNING: High firing rate may indicate instability" << std::endl;
        }
    }
    
    void generateOptimizationRecommendations(const nlm::Phase6Result& result) {
        std::cout << "\nOptimization Recommendations:" << std::endl;
        
        // Recommend based on performance metrics
        if (result.totalWallClockTime > 30.0) {
            std::cout << "  1. Consider reducing neuron count or steps for faster execution" << std::endl;
        }
        
        if (result.avgFiringRate < 1.0) {
            std::cout << "  2. Low firing rate may indicate insufficient input or learning" << std::endl;
        }
        
        if (result.memoryWorkingMemoryIntegrated && result.memoryWorkingMemoryUsed > result.memoryWorkingMemoryCapacity * 0.8) {
            std::cout << "  3. Working memory near capacity - consider optimization" << std::endl;
        }
        
        std::cout << "  4. Enable more detailed profiling for bottleneck identification" << std::endl;
        std::cout << "  5. Consider using checkpointing for long-running experiments" << std::endl;
    }
};
```

## API Reference

### Core Brain Class
```cpp
class Brain {
public:
    // Constructor and Lifecycle
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Initialization and Control
    bool initialize();
    void step(SimulationStep currentStep);
    void reset();
    
    // Input and Output
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    std::unique_ptr<Action> produceAction();
    
    // Error Handling and Validation
    ErrorCode getLastError() const;
    std::string getErrorMessage() const;
    ValidationResult validate() const;
    bool isHealthy() const;
    
    // Performance Monitoring
    void enablePerformanceMonitoring();
    void disablePerformanceMonitoring();
    bool isPerformanceMonitoringEnabled() const;
    float getLastStepDuration() const;
    
    // Advanced Features
    bool connectToWorld(SimpleWorld* world);
    bool disconnectFromWorld();
    bool isConnectedToWorld() const;
    
    // Memory Management
    void enableMemorySnapshots(bool enable);
    void takeMemorySnapshot(const std::string& label);
    bool restoreMemorySnapshot(const std::string& label);
    
    // Development and Evolution
    void enableDevelopment(bool enable);
    void setDevelopmentalStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage() const;
};
```

### Configuration System
```cpp
class Config {
public:
    // Basic Configuration
    int64_t getNeuronCount() const;
    int64_t getRegionCount() const;
    float getConnectionProbability() const;
    
    // Neural Dynamics
    double getSimulationTimestep() const;
    float getThreshold() const;
    float getLeakConductance() const;
    
    // Plasticity Parameters
    float getSTDP LTPWeight() const;
    float getSTDP LTDWeight() const;
    float getSTDP Tau() const;
    
    // Memory Parameters
    int64_t getWorkingMemoryCapacity() const;
    int64_t getEpisodicMemoryMaxEpisodes() const;
    
    // Neuromodulation
    float getDopamineBaseline() const;
    float getCuriosityBaseline() const;
    
    // Development
    float getPlasticityRate(DevelopmentalStage stage) const;
    
    // Validation and Error Handling
    bool validate() const;
    std::vector<std::string> getErrors() const;
};
```

## Troubleshooting

### Common Issues

#### 1. Brain Initialization Fails
**Symptoms**: `initialize()` returns false, error messages about configuration
**Solutions**:
- Check that configuration parameters are valid
- Ensure all required dependencies are available
- Verify that system resources are sufficient

#### 2. High Memory Usage
**Symptoms**: System runs out of memory during execution
**Solutions**:
- Reduce neuron count or complexity
- Enable memory snapshots and checkpoints
- Monitor memory usage with `--monitor` flag

#### 3. Performance Issues
**Symptoms**: Slow execution, long step times
**Solutions**:
- Reduce simulation timestep
- Disable non-essential features
- Use optimization build type
- Monitor performance with profiling tools

### Debugging

#### Enabling Debug Output
```bash
./nlm_phase6_demo --debug
```

#### Performance Profiling
```bash
./nlm_phase6_demo --profiler
```

#### Validation and Health Checks
```bash
./nlm_phase6_demo --validate
```

#### Real-time Monitoring
```bash
./nlm_phase6_demo --monitor
```

### Getting Help

For additional assistance:
1. Run demo with `--help` flag for basic usage
2. Run demo with `--help-advanced` for advanced options
3. Check the documentation directory for detailed guides
4. Visit the NLM project website for examples and tutorials
5. Report issues through the project issue tracker

## Future Enhancements

The NLM project will continue to evolve with:

### 1. Enhanced Learning Algorithms
- Advanced reinforcement learning integration
- Meta-learning and continual learning
- Transfer learning capabilities

### 2. Advanced Visualization
- Real-time neural activity visualization
- 3D brain structure rendering
- Interactive exploration tools

### 3. Cloud and Distributed Computing
- Distributed brain simulation
- Cloud-based learning environments
- Scalable computing infrastructure

### 4. Hardware Acceleration
- GPU-accelerated neural computation
- Neuromorphic hardware support
- Energy-efficient computing

### 5. Expanded Application Domains
- Robotics and control systems
- Cognitive architectures
- Brain-computer interfaces

## Acknowledgments

This project builds upon extensive research in:
- Computational neuroscience
- Artificial intelligence
- Neural network theory
- Neuromodulation systems

Special thanks to the research community and contributors who have advanced the field of artificial developmental systems.

---

**Version**: 0.1.0 (Phase 6)  
**License**: MIT  
**Contact**: [nlm-project@example.com](mailto:nlm-project@example.com)  
**Documentation**: Generated by NLM Documentation System