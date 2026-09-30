# NLM Advanced Features Demonstration

This directory contains comprehensive demonstrations of NLM's advanced capabilities for research and development.

## Overview

The NLM advanced features demonstrations showcase:

### 1. Memory System Demonstrations
- **Episodic Memory Replay**: How the brain replays and consolidates experiences
- **Working Memory Dynamics**: Competition and maintenance of multiple memory traces
- **Associative Memory Formation**: Pattern completion and auto-association

### 2. Neuromodulation Demonstrations
- **Dopamine-Mediated Learning**: Reward prediction error and reinforcement
- **Curiosity-Driven Exploration**: Novelty detection and curiosity circuits
- **Novelty Detection**: Unpredictability detection and adaptation

### 3. Plasticity Demonstrations
- **STDP Potentiation**: Spike-timing dependent plasticity demonstrations
- **Hebbian Learning**: Correlated firing and memory formation
- **Structural Plasticity**: Synaptogenesis and pruning dynamics

### 4. Cognitive Architecture Demonstrations
- **Attention Mechanisms**: Competitive selection and focus modulation
- **Concept Formation**: Pattern abstraction and categorization
- **Neural Planning**: Goal-directed behavior and action selection

### 5. Integration Demonstrations
- **Brain-Environment Interaction**: Closed-loop control and adaptation
- **Development Stages**: Maturation and plasticity rate modulation
- **Multi-System Coordination**: How different systems work together

## Installation and Setup

```bash
# Build the project with demo support
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_DEMO_BUILD=ON
make -j4

# Run specific demonstrations
./nlm_demo_memory_learning
./nlm_demo_neuromodulation
./nlm_demo_plasticity
./nlm_demo_cognition
```

## Demonstration Scripts

### 1. Memory Learning Demo
**File:** `demo_memory_learning.cpp`
**Description:** Demonstrates how NLM forms and recalls memories through Hebbian learning and episodic storage.

**Key Features:**
- Synaptic strengthening through correlated firing
- Memory consolidation and replay mechanisms
- Working memory competition and selection
- Pattern completion and auto-association

**Expected Output:**
```
Memory Formation Complete:
- Episodic Memory Episodes: 42
- Working Memory Traces: 7
- Synaptic Strengthening: 23.4%
- Memory Recall Success Rate: 87.6%
```

### 2. Neuromodulation Demo
**File:** `demo_neuromodulation.cpp`
**Description:** Shows how dopamine, curiosity, and novelty drive learning and exploration.

**Key Features:**
- Reward prediction error signaling
- Curiosity-driven exploration behavior
- Novelty detection and adaptation
- Neuromodulator interactions

**Expected Output:**
```
Neuromodulation Analysis Complete:
- Dopamine Levels: High (0.87)
- Curiosity Drive: 0.92
- Novelty Sensitivity: 0.76
- Exploration Rate: 0.43
```

### 3. Plasticity Demo
**File:** `demo_plasticity.cpp`
**Description:** Demonstrates STDP and Hebbian plasticity mechanisms in action.

**Key Features:**
- Spike-timing dependent plasticity (STDP)
- Correlated firing Hebbian learning
- Structural plasticity (synaptogenesis/pruning)
- Plasticity rate modulation by development

**Expected Output:**
```
Plasticity Analysis Complete:
- STDP Potentiation: 0.032
- Hebbian Learning Rate: 0.018
- Synaptogenesis Events: 12
- Pruning Events: 3
```

### 4. Cognitive Architecture Demo
**File:** `demo_cognition.cpp`
**Description:** Demonstrates how NLM performs cognitive tasks through neural planning and attention.

**Key Features:**
- Neural planning and goal-directed behavior
- Attention mechanisms and competitive selection
- Concept formation and categorization
- Cognitive flexibility and adaptation

**Expected Output:**
```
Cognitive Performance Complete:
- Planning Success Rate: 89.3%
- Attention Focus Accuracy: 0.91
- Concept Formation Speed: 0.47 concepts/step
- Cognitive Flexibility: High
```

## Running Demonstrations

### Command Line Usage

```bash
# Run all demonstrations
./nlm_demo_all

# Run specific demonstration
./nlm_demo_memory_learning --duration 30 --neurons 1000

# Run demonstration with custom configuration
./nlm_demo_neuromodulation --config neuro_config.cfg

# Run demonstration with visualization
./nlm_demo_plasticity --visualize

# Run demonstration in benchmark mode
./nlm_demo_cognition --benchmark
```

### Python Bindings

For users who prefer Python, NLM provides Python bindings for running demonstrations:

```python
from nlm_demo import MemoryLearningDemo, NeuromodulationDemo, PlasticityDemo, CognitionDemo

# Run memory learning demonstration
memory_demo = MemoryLearningDemo(neuron_count=1000, duration=30)
results = memory_demo.run()
print(f"Memory learning results: {results}")

# Run neuromodulation demonstration
neuro_demo = NeuromodulationDemo(curiosity_level=0.9, novelty_sensitivity=0.7)
results = neuro_demo.run()
print(f"Neuromodulation results: {results}")
```

## Advanced Configuration

### Configuration File for Demonstrations

Create a `demo_config.cfg` file to customize demonstration parameters:

```ini
# Demonstration Configuration
demo_type = memory_learning
neuron_count = 1000
region_count = 2
simulation_duration = 30.0
timestep = 0.001

# Memory parameters
working_memory_capacity = 100
max_episodic_episodes = 500
memory_consolidation_interval = 100

# Neuromodulation parameters
curiosity_strength = 1.0
novelty_threshold = 0.3
dopamine_initial_level = 0.5

# Plasticity parameters
stdp_ltp_weight = 0.03
stdp_ltd_weight = 0.02
stdp_tau = 30.0
hebbian_learning_rate = 0.02

# Visualization parameters
visualization = true
output_video = true
render_frequency = 10

# Performance parameters
benchmark_mode = false
performance_monitoring = true
```

## Advanced Features and Extensions

### 1. Custom Demonstration Framework
Extend the demonstration framework by creating custom demo modules:

```cpp
class CustomDemonstration {
public:
    virtual std::string getName() const = 0;
    virtual void configure(const Config& config) = 0;
    virtual DemoResults run() = 0;
    virtual ~CustomDemonstration() = default;
};

class MyCustomDemo : public CustomDemonstration {
public:
    std::string getName() const override { return "MyCustomDemo"; }
    void configure(const Config& config) override {
        // Set up custom parameters
    }
    DemoResults run() override {
        // Implement custom demonstration logic
    }
};
```

### 2. Real-Time Visualization
Enable real-time visualization during demonstrations:

```bash
# Run demonstration with real-time visualization
./nlm_demo_memory_learning --viz --port 8080 --host localhost

# Open browser to view visualization
# http://localhost:8080/viz
```

### 3. Performance Benchmarking
Automatically benchmark different configurations:

```bash
# Run parameter sweep for performance optimization
./nlm_demo_benchmark --parameter-sweep --range neurons:500-5000 --range regions:1-4

# Compare different plasticity rules
./nlm_demo_benchmark --compare-plasticity-rules --rules STDP,Hebbian,reward-modulated
```

## Integration Examples

### Integration with Python Applications
```python
import numpy as np
from nlm_demo import NLMDemonstration

# Load neural data from Python
neural_data = np.load("neural_patterns.npy")

# Create demonstration with loaded data
demo = NLMDemonstration(neural_data)
demo.load_config("demo_config.cfg")
results = demo.run()

# Save results for analysis
results.save("demo_results.pkl")
```

### Integration with C++ Applications
```cpp
#include "nlm_demo.h"
#include <fstream>

int main() {
    // Create demonstration
    auto demo = std::make_unique<MemoryLearningDemo>();
    
    // Load configuration from file
    Config config;
    config.loadFromFile("demo_config.cfg");
    demo->configure(config);
    
    // Run demonstration
    DemoResults results = demo->run();
    
    // Save results to file
    std::ofstream file("results.txt");
    file << "Demonstration: " << demo->getName() << "\n";
    file << "Success Rate: " << results.successRate << "%\n";
    file << "Time Steps: " << results.timeSteps << "\n";
    file << "Memory Episodes: " << results.memoryEpisodes << "\n";
    
    return 0;
}
```

## Development and Extension

### Creating New Demonstrations

To create a new demonstration:

1. **Create a new demo class** that implements the `Demonstration` interface
2. **Implement configuration loading** from config files or command line
3. **Add custom visualization** if needed
4. **Include performance monitoring** and benchmarking
5. **Document the demonstration** with examples and expected output

### Performance Optimization

For production use, consider these optimizations:

1. **Memory Pool Usage**: Use pre-allocated memory pools for better performance
2. **SIMD Vectorization**: Optimize hot paths with SIMD instructions
3. **Multi-threading**: Parallelize independent simulation steps
4. **Cache Optimization**: Structure data for better cache locality

## Example: Creating a Custom Demo

Here's an example of creating a custom demonstration:

```cpp
class PatternLearningDemo : public Demonstration {
public:
    std::string getName() const override { return "PatternLearningDemo"; }
    
    void configure(const Config& config) override {
        neuronCount = config.getOr<size_t>("neuron_count", 500);
        learningRate = config.getOr<float>("learning_rate", 0.01);
        patternSize = config.getOr<size_t>("pattern_size", 10);
    }
    
    DemoResults run() override {
        DemoResults results;
        results.name = getName();
        results.successRate = 0.0f;
        results.timeSteps = 0;
        
        // Initialize simulation
        auto brain = std::make_shared<Brain>(createDefaultConfig());
        brain->initialize();
        
        // Present patterns to learn
        for (SimulationStep step = 0; step < maxSteps; ++step) {
            // Present random pattern
            std::vector<float> pattern = generateRandomPattern(patternSize);
            
            // Inject pattern into brain
            brain->receiveSensoryInput(SensoryInput(pattern));
            
            // Run simulation step
            brain->step(step);
            
            // Check learning progress
            if (step % 100 == 0) {
                float accuracy = evaluatePatternLearning(brain, pattern);
                results.successRate = std::max(results.successRate, accuracy);
            }
            
            results.timeSteps++;
        }
        
        // Final evaluation
        results.successRate = evaluateFinalPerformance(brain);
        results.memoryEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
        
        return results;
    }
    
private:
    size_t neuronCount;
    float learningRate;
    size_t patternSize;
    static constexpr size_t maxSteps = 10000;
};
```

## Future Enhancements

### Planned Features
1. **Virtual Reality Integration**: Immersive visualization environments
2. **Multi-Agent Demonstrations**: Complex social learning scenarios
3. **Real-Time Data Integration**: Live sensor data and interactive control
4. **Cloud-Based Demonstrations**: Distributed computing and collaborative experiments

### Research Applications
- **Neural Prosthetics**: Demonstrating brain-computer interface applications
- **Cognitive Enhancement**: Testing memory and learning augmentation strategies
- **Neurodevelopmental Disorders**: Modeling and understanding developmental conditions
- **Artificial Intelligence**: Benchmarking NLM against traditional AI approaches

## Support and Community

### Getting Help
- **Documentation**: https://nlm.readthedocs.io
- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Discord Community**: https://discord.gg/nlm-project
- **Email Support**: support@nlm-project.org

### Contributing
- **Code Contributions**: Fork the repository and submit pull requests
- **Demo Contributions**: Add new demonstration modules
- **Documentation**: Improve documentation and tutorials
- **Testing**: Add new test cases and benchmarks

---

## License

MIT License - See LICENSE file for details.

## Acknowledgments

The NLM Advanced Features Demonstrations build on decades of research in:
- Computational neuroscience
- Artificial developmental biology
- Neural network simulation
- Machine learning and artificial intelligence

Special thanks to the open-source community and research collaborators.

---

**Version:** 1.0.0
**Author:** NLM Research Team
**Date:** 2026-09-30
**Contact:** demos@nlm-project.org