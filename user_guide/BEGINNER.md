# NLM User Guide: Getting Started

## Overview

Welcome to the NLM (Neural Learning Machine) user guide! This guide is designed for beginners who want to get started with NLM, understand its core functionality, and begin experimenting with the brain simulator.

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**Key Points:**
- NLM is NOT a transformer or LLM
- NLM is NOT a chatbot or deep learning model
- NLM IS a brain-inspired architecture with neurons and synapses
- NLM IS a system that develops and adapts over time
- NLM IS where cognition emerges from neural dynamics

## Current Phase: Phase 6 - Final Integration

Phase 6 integrates all previously separate systems into a coherent artificial brain. The brain has the *anatomy* of a cognitive system but functions as a *coherent artificial brain*.

## Quick Start

### 1. Installation

```bash
# Clone the NLM repository
git clone <repository-url>
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 2. Running the Phase 6 Demo

```bash
# Run the integration test
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

## Understanding the Architecture

### Brain Systems Overview

NLM consists of multiple integrated systems that work together:

1. **Neural Core**: Real spiking neurons with LIF dynamics
2. **Memory Systems**: Working memory, episodic memory, associative memory
3. **Neuromodulation**: Dopamine, curiosity, novelty, and other modulators
4. **Prediction System**: Next state prediction and error computation
5. **Cognition Systems**: Neural planning, concept formation, attention
6. **Development System**: Age-dependent plasticity and maturation
7. **Performance Infrastructure**: Memory pools, caching, optimization
8. **Advanced Interface**: Extensions, events, command system

### Key Concepts

#### 1. Closed-Loop Architecture

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
                                        ↓
                               MEMORY / PREDICTION
                                        ↓
                               MOTIVATION / NEUROMODULATION
                                        ↓
                               ACTION SELECTION
                                        ↓
                               MOTOR OUTPUT → WORLD CONSEQUENCE
                                        ↓
                               REWARD / SURPRISE / ERROR
                                        ↓
                               PLASTICITY → MEMORY / DEVELOPMENT
                                        ↓
                               CHANGED BRAIN → CHANGED FUTURE BEHAVIOR
```

#### 2. Event-Driven Processing

- Neurons communicate through spikes
- Synaptic delays create realistic timing
- Event-driven systems are efficient for sparse activity
- Real-time factor depends on network size

#### 3. Memory Systems

- **Working Memory**: Maintains active information through persistent activity
- **Episodic Memory**: Stores experiences as neural patterns
- **Associative Memory**: Forms connections between patterns
- **Integration**: Memory systems influence and are influenced by other systems

#### 4. Neuromodulation

- **Dopamine**: Reward prediction error, behavioral vigor
- **Curiosity**: Exploration motivation (combines novelty + prediction error)
- **Novelty**: Detects new sensory patterns
- **Acetylcholine**: Attention and memory consolidation (partial)
- **Other modulators**: NE, 5-HT, etc. (partial implementation)

## Basic Usage Examples

### Example 1: Running a Simple Simulation

```cpp
#include <nlm/Brain.hpp>
#include <nlm/core/Config/Config.hpp>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>("BasicConfig");
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Get statistics
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Example 2: Processing Sensory Input

```cpp
#include <nlm/Brain.hpp>
#include <nlm/sensory/SensoryInput.hpp>

void processEnvironmentInteraction(std::shared_ptr<nlm::Brain> brain) {
    // Create sensory input (e.g., from environment)
    nlm::SensoryInput input;
    std::vector<float> sensoryData(3);
    
    // Example: Simple 3D position sensor
    sensoryData[0] = 0.5f;  // X position
    sensoryData[1] = 0.0f;  // Y position
    sensoryData[2] = 0.0f;  // Z position
    
    input.setData(sensoryData);
    
    // Feed to brain
    brain->receiveSensoryInput(input);
}
```

### Example 3: Generating Actions

```cpp
#include <nlm/Brain.hpp>
#include <nlm/motor/Action.hpp>

void executeBehavior(std::shared_ptr<nlm::Brain> brain) {
    // Generate action based on current brain state
    auto action = brain->produceAction();
    
    if (action) {
        std::cout << "Action selected: " << static_cast<int>(action->getType()) 
                  << " (" << action->getDescription() << ")" << std::endl;
        
        // Apply action to environment
        // In a real implementation, this would modify the world state
    } else {
        std::cout << "No action selected (waiting)" << std::endl;
    }
}
```

## Working with Extensions

NLM provides an advanced extension system for customizing and extending brain functionality:

### 1. Extension Points

Extension points allow custom handlers to modify brain behavior:

```cpp
// Register an extension point handler
brain->registerExtensionPointHandler(
    "brain_step",
    [](const std::string& pointName, const std::any& data,
       const std::unordered_map<std::string, std::any>& params) {
        // Custom handler implementation
        return true;
    },
    "my_extension",
    100  // High priority
);
```

### 2. Event System

The event system allows extensions to communicate:

```cpp
// Emit an event
brain->emitEvent(
    "custom_event",
    static_cast<void*>(someData),
    "my_extension"
);
```

### 3. Component Factory

Create custom components using the factory:

```cpp
// Create custom neuromodulator
std::unordered_map<std::string, std::string> params;
params["type"] = "dopamine";
params["level"] = "0.7";
brain->createComponent("neuromodulator", params);
```

## Performance Optimization

NLM includes comprehensive performance optimization for large-scale simulations:

### 1. Memory Pools

```cpp
// Configure performance optimizations
nlm::BrainPerformanceConfig perfConfig;
perfConfig.enableMemoryPools = true;
perfConfig.enableCacheOptimization = true;
perfConfig.enableBatchProcessing = true;
brain->configurePerformance(perfConfig);
```

### 2. Performance Monitoring

```cpp
// Get performance statistics
auto stats = brain->getPerformanceOptimizer()->getStats();
std::cout << "Memory usage: " << stats.totalMemoryUsage << " bytes" << std::endl;
std::cout << "Cache efficiency: " << stats.cacheEfficiency * 100 << "%" << std::endl;
```

## Learning Resources

### 1. Documentation

- **Phase 6 Integration Documentation**: How all systems work together
- **Advanced Commands Documentation**: Command system for brain control
- **Performance Optimization Documentation**: Memory pools and optimization
- **Extension Guide**: Creating custom extensions
- **Brain Architecture Documentation**: Detailed system architecture

### 2. Examples

- **Basic brain usage**: Simple simulation examples
- **Custom neuromodulators**: Creating specialized neuromodulators
- **Custom memory systems**: Building custom memory systems
- **Performance optimization**: Using performance features
- **Extension examples**: Developing extensions
- **Advanced commands**: Using command system

### 3. Research Papers

- "Phase 6: Final Integration of NLM Brain Systems"
- "Performance Optimization for Large-Scale Neural Networks"
- "Extension Architecture for Custom Brain Functionality"
- "Event-Driven Processing in Spiking Neural Networks"

## Common Tasks

### Task 1: Create a Custom Neuromodulator

1. Create a new C++ class inheriting from `Neuromodulator`
2. Implement required methods: `update()`, `getLevel()`, `getPlasticityFactor()`
3. Register with the advanced brain interface
4. Test with the brain's neuromodulation system

### Task 2: Add a New Memory System

1. Create a new memory system class implementing the memory interface
2. Integrate with existing memory systems
3. Register with the brain's memory access methods
4. Test memory functionality and interactions

### Task 3: Implement an Extension

1. Create an extension class implementing the extension interface
2. Register extension points and handlers
3. Implement event listeners and emitters
4. Test extension integration with brain systems

### Task 4: Optimize Performance

1. Profile the application to identify bottlenecks
2. Configure memory pools and caching
3. Enable batch processing and SIMD
4. Measure and validate performance improvements

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Brain initialization fails**
- Solution: Check configuration parameters and ensure required files exist

**Issue 2: Memory usage too high**
- Solution: Configure memory pools and disable unnecessary optimizations

**Issue 3: Extensions fail to load**
- Solution: Check extension manifest files and dependencies

**Issue 4: Performance too slow**
- Solution: Enable performance optimizations and check system resources

### Getting Help

1. **Documentation**: Read the comprehensive documentation in the `docs/` directory
2. **Examples**: Examine example files in the `examples/` directory
3. **Community**: Join the NLM discussion forums
4. **Issues**: Report bugs and feature requests on GitHub
5. **Support**: Contact the NLM team for commercial support

## Next Steps

### For Beginners:

1. **Run the Phase 6 demo** to see the integrated brain in action
2. **Study the basic examples** to understand core functionality
3. **Explore the documentation** to learn about specific systems
4. **Try creating a simple extension** to understand the extension system

### For Intermediate Users:

1. **Learn advanced command usage** for programmatic control
2. **Experiment with performance optimization** for large-scale simulations
3. **Develop custom components** to extend brain capabilities
4. **Study Phase 6 integration patterns** for system integration

### For Advanced Users:

1. **Design custom extension architectures** for specialized applications
2. **Implement novel neuromodulation schemes** for specific behaviors
3. **Create complex memory systems** for advanced cognitive functions
4. **Contribute to NLM development** by submitting enhancements

## Conclusion

NLM is a powerful experimental brain simulator with extensive capabilities for research, development, and experimentation. This user guide provides the foundation for getting started with NLM, from basic setup to advanced usage.

The key to mastering NLM is to:

1. **Understand the architecture** and how systems integrate
2. **Explore the examples** to see functionality in action
3. **Experiment with extensions** to customize behavior
4. **Optimize performance** for your specific use case

NLM's philosophy of "integration over addition" makes it particularly suitable for research into emergent behavior, developmental processes, and brain-like computation.
