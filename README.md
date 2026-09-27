# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Project Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Documentation Structure

This document serves as the main entry point to the NLM documentation. The complete documentation is organized as follows:

### **Core Documentation**
- **Inline Source Code Documentation** (`src/`): Doxygen-style documentation for all classes and functions
- **Module-Level READMEs** (`docs/modules/`): API documentation for major subsystems
- **Code Examples** (`docs/examples/`): Usage patterns and tutorials

### **High-Level Documentation**
- **Architecture** (`docs/ARCHITECTURE.md`): System design and components
- **Science** (`docs/SCIENCE.md`): Scientific principles and background
- **Research Methods** (`docs/RESEARCH_METHODS.md`): Experimental approach and methodology

### **Development & Testing**
- **Phase 6 Final Audit** (`docs/PHASE6_FINAL_AUDIT.md`): Integration verification and results
- **Roadmap** (`docs/ROADMAP.md`): Future development plans
- **Experiments** (`docs/EXPERIMENTS.md`): Experimental framework and findings
- **Limitations** (`docs/LIMITATIONS.md`, `docs/FINAL_LIMITATIONS.md`): Current constraints and future directions

## Code Documentation Standards

All inline source code documentation follows **Doxygen style** with the following conventions:

### Doxygen Tags Used

**Class Documentation:**
```cpp
/**
 * @class ClassName
 * @brief Brief description of the class purpose
 * @details Detailed explanation of design rationale, algorithms, and internal structure
 * @see RelatedClasses
 * @note Important implementation notes
 * @warning Potential issues or limitations
 */
```

**Method Documentation:**
```cpp
/**
 * @brief Brief description of method functionality
 * @param[in] paramName Description of input parameter
 * @param[out] paramName Description of output parameter
 * @param[in,out] paramName Description of bidirectional parameter
 * @return Return value description
 * @throws ExceptionType Description of possible exceptions
 * @see RelatedMethods
 * @note Important implementation details or performance considerations
 * @warning Potential issues or edge cases
 * @example
 * ```cpp
 * // Example usage
 * Class obj;
 * obj.method();
 * ```
 */
```

### Key Documentation Areas

1. **Purpose and Design Rationale**: Why the class exists and its role in the system
2. **Parameters**: All parameters with types and constraints
3. **Return Values**: Description of outputs and return semantics
4. **Exceptions**: Error conditions and thrown exceptions
5. **Performance Characteristics**: Time complexity and memory usage
6. **Usage Examples**: Code snippets for common use cases
7. **Algorithm Details**: For complex operations or data structures
8. **Memory Management**: Ownership semantics and lifecycle management

## Core API Reference

### Neural Computation Core
- **Brain Class** (`src/brain/Brain.hpp`): Central coordinator with integrated systems
- **Neuron Class** (`src/brain/Neuron.hpp`): Leaky Integrate-and-Fire dynamics
- **Synapse Class** (`src/brain/Synapse.hpp`): Synaptic transmission with delays
- **NeuralRegion Class** (`src/brain/NeuralRegion.hpp`): Regional organization and connectivity
- **NeuralPopulation Class** (`src/brain/NeuralPopulation.hpp`): Neuron grouping and population dynamics

### Configuration System
- **Config Class** (`src/core/Config/Config.hpp`): Complete configuration management
- **Types** (`src/core/Types/Types.hpp`): Strongly-typed identifiers and utilities
- **Logger Class** (`src/core/Logger/Logger.hpp`): Logging interface and implementations
- **SimulationClock** (`src/core/SimulationClock/SimulationClock.hpp`): Time management

### Memory Systems
- **Working Memory** (`src/memory/NeuralWorkingMemory.hpp`): Active information management
- **Episodic Memory** (`src/memory/NeuralEpisodicMemory.hpp`): Experience encoding and replay
- **Associative Memory** (`src/memory/Memory.hpp`): Pattern associations

### Plasticity and Learning
- **STDP** (`src/plasticity/STDP.hpp`): Spike-Timing Dependent Plasticity
- **Hebbian Learning** (`src/plasticity/Hebbian.hpp`): Correlation-based learning
- **Structural Plasticity** (`src/plasticity/StructuralPlasticity.hpp`): Structural modifications

### Neuromodulation
- **Dopamine** (`src/neuromodulation/Dopamine.hpp`): Reward and reinforcement
- **Curiosity** (`src/neuromodulation/Curiosity.hpp`): Exploration motivation
- **Novelty** (`src/neuromodulation/Novelty.hpp`): Novelty detection
- **Prediction Error** (`src/neuromodulation/PredictionError.hpp`): Learning signals

### Cognition and Prediction
- **Prediction System** (`src/prediction/NeuralPrediction.hpp`): Forward models and prediction
- **Concept Formation** (`src/cognition/ConceptFormation.hpp`): Pattern discovery
- **Neural Planner** (`src/cognition/NeuralPlanner.hpp`): Action planning and evaluation
- **Attention System** (`src/cognition/AttentionalSelection.hpp`): Competitive selection

### Development and Environment
- **Development System** (`src/development/DevelopmentSystem.hpp`): Age-dependent plasticity modulation
- **Pruning** (`src/development/Pruning.hpp`): Synaptic elimination
- **Synaptogenesis** (`src/development/Synaptogenesis.hpp`): New synapse formation

## Configuration Documentation

The configuration system supports multiple formats and includes extensive validation. Key configuration options include:

### Core Configuration Sections
- `brain`: Neural system parameters (region counts, neuron types, etc.)
- `memory`: Memory system settings (working, episodic, associative)
- `plasticity`: Learning algorithm parameters (STDP, Hebbian, structural)
- `neuromodulation`: Neuromodulator settings and behaviors
- `prediction`: Prediction system parameters
- `cognition`: Cognitive system configurations
- `development`: Developmental stage parameters
- `simulation`: Simulation settings (timestep, duration, etc.)
- `logging`: Logging configuration and levels

## Building and Running

### Building from Source
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Integration Test
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected and functioning correctly.

## Development Guidelines

### Code Quality
- Follow consistent coding style across all modules
- Maintain comprehensive documentation for all public APIs
- Include examples where APIs are complex or have multiple use cases
- Document performance characteristics and optimization opportunities
- Explain design decisions and trade-offs in class and method documentation

### Testing Documentation
- Document test setup and execution procedures
- Include debugging techniques for common issues
- Provide troubleshooting guidance for configuration and runtime problems
- Document expected behavior and edge cases

## API Usage Examples

### Basic Brain Initialization
```cpp
#include "Brain.hpp"
#include "Config.hpp"

int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.regions", 4);
    config->set("brain.region.0.size", 100);
    
    auto brain = std::make_unique<nlm::Brain>(config);
    if (brain->initialize()) {
        // Run simulation
        for (uint64_t step = 0; step < 1000; ++step) {
            brain->step(step);
        }
    }
    return 0;
}
```

### Memory System Configuration
```cpp
// Configure working memory
brain->getWorkingMemory()->setCapacity(1000);
brain->getWorkingMemory()->setDecayRate(0.01f);

// Configure episodic memory
brain->getEpisodicMemory()->setEncodingRate(0.5f);
brain->getEpisodicMemory()->enableReplay(true);
```

### Plasticity Configuration
```cpp
// Configure STDP learning
auto* stdp = brain->getSTDP();
stdp->setLearningRate(0.01f);
stdp->setTimeWindow(20);
stdp->enableRewardModulation(true);
```

## Performance Considerations

### Time Complexity
- **Brain Step**: O(total_neuron_count + total_synapse_count)
- **Memory Access**: O(1) for working memory, O(log n) for episodic memory
- **Plasticity Updates**: O(number of spiking events)

### Memory Usage Patterns
- **Neuron State**: ~100 bytes per neuron (LIF dynamics storage)
- **Synaptic Weights**: ~32 bytes per synapse
- **Memory Systems**: Configurable based on requirements

### Optimization Opportunities
- Use SIMD vectorization for parallel processing (see `src/performance/SIMDVectorization.hpp`)
- Implement memory pools for efficient allocation (see `src/performance/MemoryPool.hpp`)
- Use event queues for async spike propagation (see `src/performance/EventQueue.hpp`)

## Troubleshooting

### Common Issues and Solutions

**Issue**: Simulation fails to initialize
- **Cause**: Invalid configuration parameters
- **Solution**: Check configuration values and run validation

**Issue**: Memory usage exceeds limits
- **Cause**: Large brain configuration or inefficient memory management
- **Solution**: Use memory pools and optimize memory layout

**Issue**: Performance bottlenecks
- **Cause**: Synchronous operations where async would work
- **Solution**: Profile with `src/performance/Performance.hpp` and use event-driven patterns

**Issue**: Plasticity not updating
- **Cause**: Incorrect plasticity flags or disabled learning rules
- **Solution**: Verify plasticity configuration and neuron spike recording

## References and Further Reading

1. **Phase 6 Final Results** (`docs/PHASE6_FINAL_RESULTS.md`): Complete integration test outcomes
2. **Architecture Documentation** (`docs/PHASE6_ARCHITECTURE.md`): Detailed system design
3. **Performance Documentation** (`docs/PERFORMANCE.md`): Performance benchmarks and optimizations
4. **Scientific Background** (`docs/SCIENCE.md`): Biological and computational principles
5. **Research Methods** (`docs/RESEARCH_METHODS.md`): Experimental approach and validation

## License

MIT

## Authors

Research project — See documentation for scientific background and implementation details.

---

**Note**: This documentation is continuously updated as the codebase evolves. For the latest information, refer to the inline source code documentation and recent commits in the repository.