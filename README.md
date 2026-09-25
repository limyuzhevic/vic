# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

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

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Command Line Interface
NLM provides an enhanced command-line interface with comprehensive options:

```bash
# Show help
./nlm --help

# Show version
./nlm --version

# Run basic simulation
./nlm --steps 1000 --neurons 1000

# Run simulation with verbose output
./nlm --steps 1000 --verbose

# Load configuration from file
./nlm --config config.json --steps 2000

# Run tests
./nlm --test memory
./nlm --test all --export test_results.json

# Run benchmarks
./nlm --benchmark micro --neurons 1000
./nlm --scaling-benchmark

# Run experiments
./nlm --experiment phase6_demo
./nlm --experiment phase3_demo --steps 500
```

## Configuration Management

### Configuration Files
NLM supports multiple configuration file formats:

- **JSON Configuration**: Modern, widely supported format
- **YAML Configuration**: Human-friendly format with comments
- **Plain Text Configuration**: Simple key-value pairs

### Configuration Examples

#### JSON Configuration
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true
        }
    }
}
```

#### YAML Configuration
```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true

neuromodulation:
  curiosity:
    enable: true
  novelty:
    enable: true
```

#### Plain Text Configuration
```
# Brain configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation configuration
neuromod.curiosity.enable = true
```

### Configuration Operations
```bash
# Load configuration from file
./nlm --config config.json --steps 2000

# Save configuration to file
./nlm --save my_config.json

# Validate configuration
./nlm --config config.json --validate

# Load experiment configuration
./nlm --config experiment_config.json --experiment demo
```

## Testing Framework

### Testing Options
NLM provides an enhanced testing framework with comprehensive testing capabilities:

```bash
# Run all tests
./nlm --test all

# Run memory integration tests
./nlm --test memory

# Run neuromodulation tests
./nlm --test neuromodulation

# Run performance tests
./nlm --test performance

# Export test results
./nlm --test all --export test_results.json

# Run tests with specific format
./nlm --test all --format json --export test_results.json
```

### Testing Examples
```python
import pynlm

# Create test configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run test simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Verify test results
assert brain.getTotalNeuronCount() > 0
assert agent.getCuriosityLevel() > 0
```

## Performance Optimization

### Performance Benchmarking
```bash
# Micro benchmark
./nlm --benchmark micro --neurons 1000 --steps 1000

# Small benchmark
./nlm --benchmark small --neurons 10000 --steps 10000

# Medium benchmark
./nlm --benchmark medium --neurons 100000 --steps 100000

# Large benchmark
./nlm --benchmark large --neurons 1000000 --steps 100000

# Scaling benchmark
./nlm --scaling-benchmark

# Export benchmark results
./nlm --benchmark neuron_performance --export benchmark_results.json
```

### Performance Examples
```cpp
// Performance benchmark
#include "src/benchmark.cpp"

int main() {
    std::vector<BenchmarkResult> results;
    
    // Micro benchmark
    auto microResult = runBenchmark(1000, 1000);
    results.push_back(microResult);
    
    // Small benchmark
    auto smallResult = runBenchmark(10000, 10000);
    results.push_back(smallResult);
    
    // Medium benchmark
    auto mediumResult = runBenchmark(100000, 100000);
    results.push_back(mediumResult);
    
    // Large benchmark
    auto largeResult = runBenchmark(1000000, 100000);
    results.push_back(largeResult);
    
    printBenchmarkResults(results);
    return 0;
}
```

## Experiment Framework

### Experiment Examples
```bash
# Phase 6 integration demo
./nlm --experiment phase6_demo

# Phase 3 demo
./nlm --experiment phase3_demo --steps 500

# Phase 4 experiment
./nlm --experiment phase4_experiment --neurons 5000

# Phase 5 experiment
./nlm --experiment phase5_experiment --duration 30.0
```

### Experiment Configuration
```json
{
    "experiment": {
        "name": "phase6_demo",
        "description": "Phase 6 integration demonstration",
        "config": {
            "brain": {
                "neuron_count": 500,
                "synapse_density": 0.1
            },
            "memory": {
                "working_memory": {
                    "capacity": 50,
                    "replay": true
                }
            },
            "neuromodulation": {
                "curiosity": {
                    "enable": true,
                    "scale": 1.0
                }
            }
        }
    }
}
```

## Python API Examples

### Basic Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Print results
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Advanced Usage
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Create advanced world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.setMaxEnergy(200.0)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Run advanced simulation with statistics
for step in range(2000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print periodic statistics
    if step % 200 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
        print(f"  Environment: {world.getAgentEnergy()}")

# Final statistics
print("\nFinal Results:")
print(f"Total simulation steps: {step + 1}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
```

## Documentation Files

NLM includes comprehensive documentation organized into the following files:

### Core Documentation
- **CONFIGURATION.md**: Configuration system documentation
- **TESTING.md**: Testing framework documentation
- **PERFORMANCE.md**: Performance optimization documentation
- **CLI.md**: Command-line interface documentation
- **API_REFERENCE.md**: API reference documentation
- **PHASE6_INTEGRATION.md**: Phase 6 integration documentation
- **VISUALIZATION.md**: Visualization documentation
- **PHASE5_BENCHMARK.md**: Phase 5 benchmark documentation

### Existing Documentation
- **README.md**: This document with updated examples
- **docs/ARCHITECTURE.md**: System architecture overview
- **docs/SCIENCE.md**: Scientific background
- **docs/EXPERIMENTS.md**: Experiment descriptions
- **docs/PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit

### Usage Examples
- **easy_usage.md**: Simple usage guide for beginners
- **HOW_TO_USE.md**: Comprehensive usage documentation

## Getting Started

### Quick Start
1. **Build NLM**: Follow the building instructions in this document
2. **Run Basic Simulation**: Use the basic simulation example
3. **Explore Documentation**: Read the comprehensive documentation
4. **Run Tests**: Run the testing framework to verify functionality
5. **Run Benchmarks**: Run performance benchmarks to validate optimizations

### First Steps
```bash
# 1. Build NLM
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# 2. Run basic simulation
./nlm --steps 1000 --neurons 1000

# 3. Run tests
./nlm --test all --export test_results.json

# 4. Run benchmarks
./nlm --benchmark micro --export benchmark_results.json

# 5. Run experiments
./nlm --experiment phase6_demo
```

### Learning Path
1. **Read README.md**: Understand the project overview and basic usage
2. **Read HOW_TO_USE.md**: Learn detailed usage instructions
3. **Read easy_usage.md**: Get started with simple examples
4. **Read CONFIGURATION.md**: Understand configuration management
5. **Read TESTING.md**: Learn testing framework usage
6. **Read PERFORMANCE.md**: Understand performance optimization
7. **Read CLI.md**: Master command-line interface
8. **Read API_REFERENCE.md**: Access API documentation
9. **Read PHASE6_INTEGRATION.md**: Understand Phase 6 integration
10. **Read VISUALIZATION.md**: Learn visualization features
11. **Read PHASE5_BENCHMARK.md**: Understand performance benchmarking

## Documentation Usage

### Configuration Documentation
- Learn how to configure NLM with JSON, YAML, and plain text
- Understand configuration keys and validation
- See examples of different configuration scenarios

### Testing Documentation
- Understand the enhanced testing framework
- Learn how to write and run tests
- See test examples and best practices

### Performance Documentation
- Learn about performance optimization features
- Understand performance benchmarks and measurements
- See optimization examples and best practices

### CLI Documentation
- Learn how to use the command-line interface
- See all available options and examples
- Understand command-line arguments and syntax

### API Documentation
- Find C++ API reference documentation
- Access Python API documentation
- Learn about class methods and parameters

### Phase 6 Integration Documentation
- Understand Phase 6 integration features
- Learn about system integration and verification
- See integration examples and best practices

### Visualization Documentation
- Learn about visualization features and modes
- Understand visualization configuration and setup
- See visualization examples and best practices

### Benchmark Documentation
- Understand benchmark features and capabilities
- Learn about performance benchmarking and analysis
- See benchmark examples and best practices

## Getting Help

### Documentation
- Read the comprehensive documentation files
- Check the examples and tutorials
- Use the API reference for detailed information

### Command Line
- Use `--help` for command-specific help
- Use `--verbose` for additional information
- Use `--debug` for debug output

### Online Resources
- Visit the NLM website for more information
- Check the documentation for updates
- Join the NLM community for support

## Building and Running Documentation Examples

### Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Examples
```bash
# Build with examples
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_EXAMPLES=ON

# Run basic example
./build/examples/basic_example

# Run configuration example
./build/examples/config_example

# Run brain example
./build/examples/brain_example

# Run world example
./build/examples/world_example

# Run agent example
./build/examples/agent_example

# Run integration example
./build/examples/integration_example
```

### Generating Documentation
```bash
# Generate API documentation
./build/docs/generate_api_docs.sh

# Generate user documentation
./build/docs/generate_user_docs.sh

# Generate examples documentation
./build/docs/generate_examples_docs.sh
```

## Summary

NLM is a comprehensive neural network simulation project with enhanced documentation and examples. The documentation includes:

1. **Configuration System**: Comprehensive configuration management with JSON, YAML, and plain text support
2. **Testing Framework**: Enhanced testing infrastructure with legacy and enhanced test support
3. **Performance Optimization**: Performance optimization features with benchmarking and profiling
4. **Command-Line Interface**: Comprehensive CLI with extensive options and examples
5. **API Reference**: Complete API documentation for both C++ and Python
6. **Phase 6 Integration**: Detailed documentation of Phase 6 integration features
7. **Visualization**: Comprehensive visualization documentation with examples
8. **Benchmarking**: Detailed benchmarking documentation with examples
9. **Practical Examples**: Real-world examples for all major features

The documentation is designed to be comprehensive yet accessible, with clear examples and best practices for getting started with NLM.

## License

MIT

## Authors

Research project — See docs for scientific background.

## Additional Resources

### Documentation Index
- **CONFIGURATION.md** (Configuration system documentation)
- **TESTING.md** (Testing framework documentation)
- **PERFORMANCE.md** (Performance optimization documentation)
- **CLI.md** (Command-line interface documentation)
- **API_REFERENCE.md** (API reference documentation)
- **PHASE6_INTEGRATION.md** (Phase 6 integration documentation)
- **VISUALIZATION.md** (Visualization documentation)
- **PHASE5_BENCHMARK.md** (Phase 5 benchmark documentation)

### Quick Start Guide
- **HOW_TO_USE.md** (Comprehensive usage guide)
- **easy_usage.md** (Simple usage guide for beginners)

### Technical Documentation
- **docs/ARCHITECTURE.md** (System architecture overview)
- **docs/SCIENCE.md** (Scientific background)
- **docs/EXPERIMENTS.md** (Experiment descriptions)
- **docs/PHASE6_FINAL_AUDIT.md** (Phase 6 integration audit)

### Project Information
- **README.md** (Project overview and usage)
- **CONTRIBUTING.md** (Contribution guidelines)
- **CHANGELOG.md** (Project changelog)
- **LICENSE.md** (License information)

## Support

For questions, issues, or feedback:

1. **Documentation**: Check the documentation files for answers
2. **Command Line**: Use `--help` and `--verbose` options
3. **Community**: Join the NLM community for support
4. **Issues**: Report issues on the GitHub repository
5. **Feedback**: Provide feedback to improve documentation and examples

## Future Documentation

### Planned Documentation
1. **Advanced Topics**: Advanced NLM topics and techniques
2. **Performance Tuning**: Performance tuning and optimization guides
3. **Troubleshooting**: Troubleshooting and debugging guides
4. **Migration Guide**: Migration guides for version updates
5. **API Evolution**: API evolution and future directions

### Documentation Improvements
- Continuous documentation updates based on user feedback
- Addition of new examples and tutorials
- Improvement of existing documentation quality
- Expansion of technical documentation
- Enhancement of API documentation completeness

## Conclusion

NLM is a comprehensive neural network simulation project with extensive documentation and examples. The documentation includes:

1. **Configuration System**: Comprehensive configuration management
2. **Testing Framework**: Enhanced testing infrastructure
3. **Performance Optimization**: Performance optimization features
4. **Command-Line Interface**: Comprehensive CLI
5. **API Reference**: Complete API documentation
6. **Phase 6 Integration**: Detailed integration documentation
7. **Visualization**: Comprehensive visualization documentation
8. **Benchmarking**: Detailed benchmarking documentation
9. **Practical Examples**: Real-world examples for all features

The documentation is designed to be comprehensive, accessible, and practical, with clear examples and best practices for getting started with NLM and advancing to more complex usage.

By following the documentation and examples provided, you can effectively use NLM for neural network simulation, testing, and research.



### Command Line Interface
NLM provides an enhanced command-line interface with comprehensive options:

```bash
# Show help
./nlm --help

# Show version
./nlm --version

# Run basic simulation
./nlm --steps 1000 --neurons 1000

# Run simulation with verbose output
./nlm --steps 1000 --verbose

# Load configuration from file
./nlm --config config.json --steps 2000

# Run tests
./nlm --test memory
./nlm --test all --export test_results.json

# Run benchmarks
./nlm --benchmark micro --neurons 1000
./nlm --scaling-benchmark

# Run experiments
./nlm --experiment phase6_demo
./nlm --experiment phase3_demo --steps 500
```

## Configuration Examples

### Configuration File Examples

#### Basic Configuration (JSON)
```json
{
    "brain": {
        "neuron_count": 1000,
        "synapse_density": 0.1,
        "connection_probability": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    },
    "neuromodulation": {
        "curiosity": {
            "enable": true
        }
    }
}
```

#### YAML Configuration
```yaml
brain:
  neuron_count: 1000
  synapse_density: 0.1
  connection_probability: 0.05

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
  hebbian:
    enable: true

neuromodulation:
  curiosity:
    enable: true
  novelty:
    enable: true
```

#### Plain Text Configuration
```
# Brain configuration
brain.neuron_count = 1000
brain.synapse_density = 0.1
brain.connection_probability = 0.05

# Plasticity configuration
plasticity.stdp.enable = true
plasticity.stdp.learning_rate = 0.001
plasticity.hebbian.enable = true

# Neuromodulation configuration
neuromod.curiosity.enable = true
```

### Command Line Configuration
```bash
# Basic configuration
./nlm --brain.neuron_count=1000 --plasticity.stdp.enable=true --steps 1000

# High-density network
./nlm --brain.neuron_count=10000 --brain.synapse_density=0.2 --plasticity.stdp.learning_rate=0.002

# Performance-optimized configuration
./nlm --brain.neuron_count=1000 --performance.vectorization=AVX512 --performance.parallel_threads=8
```

## Testing Examples

### Testing Framework
```bash
# Run all tests
./nlm --test all

# Run memory integration tests
./nlm --test memory

# Run neuromodulation tests
./nlm --test neuromodulation

# Run performance tests
./nlm --test performance

# Export test results
./nlm --test all --export test_results.json

# Run tests with specific format
./nlm --test all --format json --export test_results.json
./nlm --test all --format xml --export test_results.xml
./nlm --test all --format html --export test_results.html
```

### Test Configuration Examples

#### Memory Integration Test Configuration
```bash
# Create test configuration
./nlm --config test_config.json --test memory --export memory_test_results.json

# Memory test configuration
{
    "memory": {
        "working_memory": {
            "capacity": 100,
            "competition": true
        },
        "episodic_memory": {
            "capacity": 1000,
            "replay": true
        }
    }
}
```

#### Performance Test Configuration
```bash
# Performance test configuration
{
    "benchmark": {
        "neuron_count": 1000,
        "steps": 1000,
        "vectorization": "AVX512",
        "parallel_threads": 8,
        "sparse_connectivity": true
    }
}
```

## Performance Examples

### Performance Benchmarking
```bash
# Micro benchmark
./nlm --benchmark micro --neurons 1000 --steps 1000

# Small benchmark
./nlm --benchmark small --neurons 10000 --steps 10000

# Medium benchmark
./nlm --benchmark medium --neurons 100000 --steps 100000

# Large benchmark
./nlm --benchmark large --neurons 1000000 --steps 100000

# Scaling benchmark
./nlm --scaling-benchmark

# Export benchmark results
./nlm --benchmark neuron_performance --export benchmark_results.json
```

### Performance Optimization Examples
```bash
# Build with performance optimizations
cmake -DCMAKE_BUILD_TYPE=Release -DNLM_PERFORMANCE_ENABLED=ON ..
make -j$(nproc)

# Run optimized simulation
./nlm --brain.neuron_count=1000 --performance.vectorization=AVX512 --performance.parallel_threads=8

# Profile performance
perf record -g ./nlm --benchmark --neurons=10000 --steps=10000
perf report
```

## Experiment Examples

### Experiment Running
```bash
# Phase 6 integration demo
./nlm --experiment phase6_demo

# Phase 3 demo
./nlm --experiment phase3_demo --steps 500

# Phase 4 experiment
./nlm --experiment phase4_experiment --neurons 5000

# Phase 5 experiment
./nlm --experiment phase5_experiment --duration 30.0

# Phase 3 experiment
./nlm --experiment phase3_experiment --steps 1000 --save experiment_config.json
```

### Experiment Configuration
```json
{
    "experiment": {
        "name": "phase6_demo",
        "description": "Phase 6 integration demonstration",
        "config": {
            "brain": {
                "neuron_count": 500,
                "synapse_density": 0.1
            },
            "memory": {
                "working_memory": {
                    "capacity": 50,
                    "replay": true
                }
            },
            "neuromodulation": {
                "curiosity": {
                    "enable": true,
                    "scale": 1.0
                }
            }
        }
    }
}
```

## API Usage Examples

### Python API Examples
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Print results
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### C++ API Examples
```cpp
#include <nlm.h>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Create world
    auto world = std::make_shared<nlm::SimpleWorld>();
    world->configure(20, 20, 8, 8);
    
    // Create agent
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->initialize(world);
    
    // Enable subsystems
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(true);
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
    }
    
    return 0;
}
```

## Documentation Files

NLM includes comprehensive documentation organized into the following files:

### Core Documentation
- **CONFIGURATION.md**: Configuration system documentation
- **TESTING.md**: Testing framework documentation
- **PERFORMANCE.md**: Performance optimization documentation
- **CLI.md**: Command-line interface documentation
- **API_REFERENCE.md**: API reference documentation

### Existing Documentation
- **README.md**: This document with updated examples
- **docs/ARCHITECTURE.md**: System architecture overview
- **docs/SCIENCE.md**: Scientific background
- **docs/EXPERIMENTS.md**: Experiment descriptions
- **docs/PHASE6_FINAL_AUDIT.md**: Phase 6 integration audit

## Documentation Usage

### Configuration Documentation
- Learn how to configure NLM with JSON, YAML, and plain text
- Understand configuration keys and validation
- See examples of different configuration scenarios

### Testing Documentation
- Understand the enhanced testing framework
- Learn how to write and run tests
- See test examples and best practices

### Performance Documentation
- Learn about performance optimization features
- Understand performance benchmarks and measurements
- See optimization examples and best practices

### CLI Documentation
- Learn how to use the command-line interface
- See all available options and examples
- Understand command-line arguments and syntax

### API Documentation
- Find C++ API reference documentation
- Access Python API documentation
- Learn about class methods and parameters

## Getting Help

### Documentation
- Read the comprehensive documentation files
- Check the examples and tutorials
- Use the API reference for detailed information

### Command Line
- Use `--help` for command-specific help
- Use `--verbose` for additional information
- Use `--debug` for debug output

### Online Resources
- Visit the NLM website for more information
- Check the documentation for updates
- Join the NLM community for support

## Building and Running Documentation

### Building
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Documentation Examples
```bash
# Build with examples
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_EXAMPLES=ON

# Run examples
./build/examples/basic_example
./build/examples/config_example
./build/examples/brain_example
```

### Generating Documentation
```bash
# Generate API documentation
./build/docs/generate_api_docs.sh

# Generate user documentation
./build/docs/generate_user_docs.sh

# Generate examples documentation
./build/docs/generate_examples_docs.sh
```

## Summary

NLM is a comprehensive neural network simulation project with enhanced documentation and examples. The documentation includes:

1. **Configuration System**: Comprehensive configuration management with JSON, YAML, and plain text support
2. **Testing Framework**: Enhanced testing infrastructure with legacy and enhanced test support
3. **Performance Optimization**: Performance optimization features with benchmarking and profiling
4. **Command-Line Interface**: Comprehensive CLI with extensive options and examples
5. **API Reference**: Complete API documentation for both C++ and Python
6. **Practical Examples**: Real-world examples for all major features

The documentation is designed to be comprehensive yet accessible, with clear examples and best practices for getting started with NLM.

## License

MIT

## Authors

Research project — See docs for scientific background.



## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- Phase 6 integration experiment created

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

```
WORLD
  ↓
SENSORY INPUT
  ↓
NEURAL PROCESSING (LIF dynamics, spikes)
  ↓
INTERNAL STATE (working memory, attention)
  ↓
MEMORY / PREDICTION
  ↓
MOTIVATION / NEUROMODULATION (dopamine, curiosity)
  ↓
ACTION SELECTION
  ↓
MOTOR OUTPUT
  ↓
WORLD CONSEQUENCE
  ↓
REWARD / SURPRISE / ERROR
  ↓
PLASTICITY (STDP, Hebbian, structural)
  ↓
MEMORY / DEVELOPMENT
  ↓
CHANGED BRAIN
  ↓
CHANGED FUTURE BEHAVIOR
```

## License

MIT

## Authors

Research project — See docs for scientific background.
