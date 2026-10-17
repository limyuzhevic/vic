# NLM (Neural Learning Machine) - Complete Documentation

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project designed to create artificial developmental brains that learn and adapt through interaction with environments. This documentation covers the complete system including core neural architecture, advanced learning capabilities, command-line interface, and developer tools.

## Quick Start

### Using the Command Line Interface

The NLM CLI provides powerful standalone functionality for running neural simulations:

```bash
# Help and version information
./build/nlm --help
./build/nlm --version

# Run a simple demonstration
./build/nlm --mode demo --steps 100 --num-steps 10

# Run performance benchmark
./build/nlm --mode benchmark --steps 1000

# Run simulation with custom configuration
./build/nlm --config configs/my_config.cfg --mode simulate --steps 500

# Enable all learning features
./build/nlm --mode demo --enable-checkpointing --enable-replay --enable-development --enable-curiosity --enable-reward-modulation
```

### Using the Python API

For Python users, NLM provides a comprehensive API for neural simulation:

```python
import nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Create and initialize brain
brain = nlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Create world simulation
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent and connect to brain
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Core Architecture

### Neural Components

NLM implements a complete spiking neural network with the following components:

#### LIF Neurons
- **Leaky Integrate-and-Fire dynamics**: Realistic membrane potential integration
- **Event-driven computation**: Sparse spike-based communication
- **Adaptable parameters**: Configurable threshold, refractory period, time constants
- **Multiple neuron types**: Excitatory, inhibitory, sensory, motor, modulatory

#### Synaptic Connections
- **Dynamic weights**: Real-time modification through plasticity rules
- **Multiple synapse types**: Excitatory, inhibitory, electrical, gap junctions
- **Delays and plasticity**: Synaptic delays with activity-dependent modification
- **Structural plasticity**: Connection creation and elimination

#### Brain Regions
- **Modular architecture**: Separate neural populations by function
- **Hierarchical organization**: Primary, secondary, and association regions
- **Inter-regional connectivity**: Long-range connections between regions
- **Regional specialization**: Different neuron populations and connectivity patterns

### Plasticity Mechanisms

NLM implements multiple complementary plasticity rules:

#### Spike-Timing-Dependent Plasticity (STDP)
- **Hebbian learning**: Pre-post spike timing-dependent potentiation
- **Temporal asymmetry**: Different rules for LTP and LTD
- **Neuromodulation integration**: Dopamine scaling of STDP
- **Eligibility traces**: Temporal credit assignment

#### Hebbian Learning
- **Correlational learning**: Co-activation strengthens connections
- **Symmetric rules**: Weight changes based on neural activity patterns
- **Global modulation**: Different learning rates for different synapse types
- **Pattern completion**: Spontaneous recovery of partial patterns

#### Structural Plasticity
- **Synaptogenesis**: Creation of new connections
- **Pruning**: Elimination of weak or unused connections
- **Homeostatic regulation**: Maintaining optimal network stability
- **Developmental stages**: Age-dependent plasticity rates

### Memory Systems

NLM implements a comprehensive memory architecture:

#### Working Memory
- **Persistent activity**: Maintenance of information over time
- **Competition dynamics**: Winner-take-all mechanisms
- **Capacity limits**: Finite storage with competition
- **Temporal dynamics**: Decay and consolidation processes

#### Episodic Memory
- **Experience recording**: Event-by-event storage of experiences
- **Replay mechanisms**: Memory consolidation through replay
- **Temporal indexing**: Organizing memories by time
- **Emotional tagging**: Reward and novelty modulation

#### Associative Memory
- **Pattern completion**: Recovery of complete patterns from partial input
- **Bidirectional associations**: Forward and backward connections
- **Distributed representation**: Multiple neural ensembles for single concepts
- **Semantic networks**: Hierarchical concept organization

### Neuromodulation Systems

NLM implements biological neuromodulation systems:

#### Dopamine
- **Reward prediction error**: Learning from unexpected outcomes
- **Plasticity modulation**: Scaling of learning rates
- **Motivation and arousal**: Drive and attention regulation
- **Action selection**: Value-based decision making

#### Curiosity
- **Exploration drive**: Motivated search for novel information
- **Information gain**: Maximizing predictive information
- **Behavioral flexibility**: Switching between behaviors
- **Intrinsic motivation**: Self-generated learning

#### Novelty Detection
- **Change detection**: Identifying unexpected sensory input
- **Surprise signals**: Quantifying prediction errors
- **Attention allocation**: Focusing resources on novel information
- **Arousal modulation**: Increasing alertness for new stimuli

#### Prediction Error
- **Temporal difference learning**: Learning about future rewards
- **Adaptive expectations**: Updating predictions over time
- **Error signals**: Quantifying prediction accuracy
- **Learning modulation**: Adjusting neural plasticity based on errors

### Cognitive Systems

NLM implements higher-level cognitive functions:

#### Neural Planning
- **Action sequence evaluation**: Planning future actions
- **Multi-objective optimization**: Balancing competing goals
- **Hierarchical planning**: Plans within plans
- **Monte Carlo methods**: Probabilistic planning

#### Concept Formation
- **Pattern discovery**: Identifying regularities in experience
- **Category learning**: Grouping similar experiences
- **Abstract representation**: High-level conceptual knowledge
- **Transfer learning**: Applying learned concepts to new situations

#### Attentional Selection
- **Competitive selection**: Focusing attention on salient information
- **Inhibition of return**: Preventing repeated attention to same stimuli
- **Gain control**: Amplifying relevant neural activity
- **Saliency mapping**: Identifying important information

### Developmental System

NLM implements neural development with multiple stages:

#### Developmental Stages
- **Initial stage**: High plasticity, rapid learning
- **Critical period**: Sensitive window for skill acquisition
- **Maturation**: Stabilization of learned patterns
- **Adult stage**: Limited plasticity, behavioral stability

#### Activity-Dependent Development
- **Experience-expectant**: Development shaped by environmental input
- **Experience-dependent**: Individual learning shaping development
- **Use-dependent**: Strengthening frequently used connections
- **Pruning**: Eliminating unused connections

### World Simulation

NLM provides flexible world simulation for agent testing:

#### SimpleWorld
- **2D grid world**: Spatial simulation environment
- **Vision system**: Sensory input with occlusion and perspective
- **Action system**: Motor output and world interaction
- **Objects and resources**: Environmental elements and goals

#### World Properties
- **Dynamic environments**: Changing world states
- **Physics simulation**: Basic physical interactions
- **Sensory constraints**: Limited and noisy sensory information
- **Action outcomes**: Probabilistic action results

## Command-Line Interface (CLI)

### CLI Overview

The NLM CLI provides comprehensive control over neural simulations without requiring Python:

#### Modes

- **demo**: Run built-in integration demonstrations
- **simulate**: Run world simulation with agents
- **train**: Train brain with data files
- **monitor**: Real-time visualization and monitoring
- **benchmark**: Performance benchmarking

#### Key Features

- **Configuration file support**: JSON, YAML, and custom formats
- **Command-line parameter override**: Flexible configuration
- **Export/import capabilities**: Save/load brain states
- **Logging configuration**: Multiple log levels and destinations
- **Help system**: Comprehensive documentation and examples

#### Usage Examples

```bash
# Basic demonstration
nlm --mode demo --steps 100 --num-steps 10

# Advanced simulation with all features
nlm --mode simulate \
    --config configs/advanced.cfg \
    --enable-checkpointing \
    --enable-replay \
    --enable-development \
    --enable-curiosity \
    --enable-reward-modulation \
    --steps 1000

# Performance benchmark
nlm --mode benchmark --steps 5000 --timestep 0.0005

# Export brain state after simulation
nlm --mode demo --steps 500 --checkpoint results/checkpoint.bin
```

#### Configuration Files

NLM supports multiple configuration file formats:

**CLI Configuration (`configs/cli.cfg`):**
```ini
mode = demo
steps = 1000
num_steps = 100
timestep = 0.001
log_level = INFO
quiet = false
verbose = false
enable_checkpointing = false
enable_replay = false
enable_development = false
enable_curiosity = false
enable_reward_modulation = false
```

**Default Configuration (`configs/default.cfg`):**
```ini
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### CLI Developer Tools

The CLI includes extensive developer tools for debugging and analysis:

#### Debug Mode
- **Step-by-step execution**: Run simulations one step at a time
- **Pause/resume control**: Stop and continue simulation execution
- **Event logging**: Detailed logging of neural events
- **Memory tracking**: Monitor memory usage and leaks

#### Performance Monitoring
- **Real-time metrics**: CPU usage, memory consumption, firing rates
- **Benchmarking**: Compare performance across configurations
- **Resource monitoring**: Track system resource utilization
- **Profiling support**: Identify performance bottlenecks

#### Brain State Comparison
- **State difference detection**: Compare two brain states
- **Anomaly detection**: Identify unusual brain activity
- **Health monitoring**: Track brain system health
- **Export/import**: Save and restore brain states

## Advanced Learning Capabilities

### Meta-Learning

NLM implements meta-learning systems that learn how to learn:

#### Learning Rate Adaptation
- **Performance-based adjustment**: Increase learning when progress stalls
- **Stability-plasticity balance**: Maintain learned knowledge while enabling new learning
- **Task-aware learning**: Different learning strategies for different tasks
- **Transfer learning**: Apply learned learning strategies to new domains

#### Credit Assignment
- **Temporal difference learning**: Assign credit across time
- **Policy gradients**: Optimize behavioral strategies
- **Reinforcement learning**: Learn from environmental feedback
- **Meta-reward signals**: Learn about learning itself

### Hierarchical Temporal Memory

NLM integrates Hierarchical Temporal Memory (HTM) principles:

#### Spatial Pooling
- **Column-based representation**: Sparse distributed representations
- **Invariant learning**: Learn invariant features across variations
- **Texture coding**: Learn spatial patterns and relationships
- **Predictive coding**: Generate predictions of sensory input

#### Temporal Memory
- **Sequence learning**: Learn temporal patterns and sequences
- **Prediction encoding**: Predict future sensory states
- **Anomaly detection**: Identify unexpected sequences
- **Reinforcement integration**: Learn from temporal prediction errors

### Advanced Plasticity Rules

NLM extends traditional plasticity with advanced rules:

#### Continuous-Time Plasticity
- **Differential equations**: Smooth weight changes over time
- **Integrate-and-fire dynamics**: Continuous synaptic modification
- **Noise-based learning**: Learn from stochastic inputs
- **Stochastic resonance**: Optimize information transmission

#### Neuromodulation-Integrated Plasticity
- **Dopamine-gated plasticity**: Learning controlled by reward signals
- **Curiosity-driven exploration**: Novelty-based learning
- **Stress modulation**: Learning under challenging conditions
- **Social learning**: Learning from observed behaviors

### Enhanced Memory Systems

NLM extends memory with advanced capabilities:

#### Working Memory Expansion
- **Multiple buffers**: Separate memory systems for different types of information
- **Dynamic capacity**: Adjust memory capacity based on task demands
- **Multi-trace memory**: Maintain multiple versions of information
- **Rapid encoding**: Fast acquisition of new information

#### Episodic Memory Enhancement
- **Autobiographical memory**: Learning from personal experiences
- **Temporal chunking**: Group related events into larger units
- **Semantic integration**: Connect episodic memories to semantic knowledge
- **Emotional tagging**: Associate memories with emotional valence

#### Associative Memory Optimization
- **Distributed representations**: Spread across multiple neurons
- **Pattern completion**: Recover complete patterns from partial input
- **Bidirectional associations**: Learn forward and backward connections
- **Translational memory**: Convert between different representations

## Developer Tools

### Configuration Validation

NLM provides comprehensive configuration validation:

#### Schema Validation
- **Type checking**: Validate configuration parameter types
- **Range validation**: Ensure values are within acceptable ranges
- **Dependency checking**: Verify configuration consistency
- **Schema validation**: Support for JSON/YAML schemas

#### Configuration Analysis
- **Pattern detection**: Identify unusual configuration combinations
- **Performance prediction**: Estimate simulation performance
- **Memory requirements**: Calculate memory needs
- **Optimization recommendations**: Suggest improvements

### Debugging Tools

Advanced debugging capabilities for neural simulation:

#### Event System
- **Debug logging**: Detailed logging of neural events
- **Breakpoint support**: Pause simulation at specific points
- **Variable inspection**: Examine neural state variables
- **Call stack tracing**: Trace execution flow

#### Memory Management
- **Leak detection**: Identify memory leaks
- **Allocation tracking**: Monitor memory allocation patterns
- **Garbage collection**: Manage unused objects
- **Performance analysis**: Analyze memory usage patterns

#### Performance Profiling
- **CPU profiling**: Identify performance bottlenecks
- **Memory profiling**: Track memory usage over time
- **I/O analysis**: Monitor file system usage
- **Network monitoring**: Track network-related operations

### Brain State Analysis

Tools for analyzing and comparing brain states:

#### Neural Metrics
- **Connectivity analysis**: Measure network connectivity
- **Plasticity metrics**: Quantify learning dynamics
- **Memory metrics**: Evaluate memory system performance
- **System status**: Monitor overall brain health

#### State Comparison
- **Difference detection**: Identify changes between brain states
- **Anomaly detection**: Find unusual patterns
- **Similarity analysis**: Compare brain states across simulations
- **Time-series analysis**: Analyze brain state evolution

## Python API Enhancements

### Core Classes

Extended Python API for neural simulation:

#### Brain Class
- **Enhanced initialization**: More flexible brain creation
- **Performance optimization**: Improved simulation speed
- **Memory management**: Better resource handling
- **Serialization support**: Save/load brain states

#### Config Class
- **Schema support**: Configuration validation
- **Dynamic updates**: Modify configuration at runtime
- **Inheritance**: Support for configuration templates
- **Export/import**: Save/load configurations

#### AgentBrain Class
- **Advanced sensory processing**: Enhanced perception capabilities
- **Motor planning**: Improved action generation
- **Neuromodulation integration**: Better reward and motivation
- **Development support**: Age-dependent behavior

### Python Extensions

Additional Python capabilities:

#### Visualization
- **Real-time plotting**: Interactive visualization of neural activity
- **Brain state monitoring**: Track neural system health
- **Performance dashboards**: Monitor simulation performance
- **Event visualization**: Show neural events and connections

#### Machine Learning
- **Feature extraction**: Extract features from neural activity
- **Pattern recognition**: Identify patterns in neural data
- **Classification**: Classify neural states
- **Regression**: Predict neural activity patterns

#### Distributed Computing
- **Parallel processing**: Speed up neural simulations
- **Distributed memory**: Handle large neural systems
- **Load balancing**: Distribute computation across nodes
- **Fault tolerance**: Handle system failures gracefully

## Performance Optimization

### Memory Management

Advanced memory optimization techniques:

#### Memory Pools
- **Object pooling**: Reuse neural objects to reduce allocation overhead
- **Arena allocation**: Stack-based allocation for predictable memory usage
- **NUMA awareness**: Optimize memory access patterns for multi-core systems
- **Compression**: Reduce memory footprint for large brains

#### Caching Strategies
- **Neuron caching**: Reuse frequently accessed neuron objects
- **Synapse caching**: Cache synaptic connections and weights
- **Pattern caching**: Store frequently used neural patterns
- **Template caching**: Cache reusable configuration templates

### Parallel Processing

Multi-core and distributed computing support:

#### Thread-Level Parallelism
- **Data parallelism**: Distribute neural computation across threads
- **Task parallelism**: Separate neural systems across threads
- **Load balancing**: Distribute work evenly across cores
- **Synchronization**: Efficient coordination between threads

#### Distributed Computing
- **MPI integration**: Scale simulations across multiple machines
- **Distributed memory**: Handle neural systems larger than main memory
- **Fault tolerance**: Continue simulation despite node failures
- **Communication optimization**: Minimize inter-node communication

### Performance Monitoring

Comprehensive performance monitoring:

#### System Metrics
- **CPU usage**: Monitor processor utilization
- **Memory usage**: Track memory consumption
- **I/O operations**: Monitor file system usage
- **Network traffic**: Track network-related operations

#### Neural Performance
- **Firing rate analysis**: Track neural activity patterns
- **Learning speed**: Measure learning effectiveness
- **Memory usage**: Monitor memory system performance
- **Plasticity metrics**: Quantify learning dynamics

## Plugin System

### Plugin Architecture

Extensible plugin system for adding new functionality:

#### Plugin Types
- **Neuron plugins**: Custom neuron models
- **Synapse plugins**: Custom plasticity rules
- **World plugins**: Alternative world simulation systems
- **Visualization plugins**: Custom output formats
- **Analysis plugins**: Custom analysis tools

#### Plugin Loading
- **Dynamic loading**: Load plugins at runtime
- **Version management**: Support for plugin versioning
- **Dependency resolution**: Handle plugin dependencies
- **Sandboxing**: Isolate plugins for safety

#### Plugin Marketplace
- **Plugin repository**: Centralized plugin distribution
- **Plugin discovery**: Find and install plugins
- **Plugin reviews**: Community evaluation of plugins
- **Plugin documentation**: Comprehensive plugin documentation

## Examples and Tutorials

### Basic Examples

Simple examples for getting started with NLM:

#### Example 1: Silent Brain
```python
import nlm

brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

#### Example 2: Brain Watching a World
```python
import nlm

# Setup
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

#### Example 3: Complete Agent
```python
import nlm

# Setup
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()
world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Advanced Examples

Complex examples demonstrating advanced NLM capabilities:

#### Learning and Memory
```python
import nlm

class LearningExperiment:
    def __init__(self):
        self.config = nlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        
    def run_experiment(self, num_steps=1000):
        # Setup
        self.setup_environment()
        
        # Run experiment
        for step in range(num_steps):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            reward = self.calculate_reward(step)
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                self.print_progress(step)
                
        # Analyze results
        self.analyze_results()
```

## Testing and Validation

### Test Suite

Comprehensive test coverage for NLM:

#### Unit Tests
- **Neural components**: Test individual neuron and synapse models
- **Plasticity rules**: Validate learning mechanisms
- **Memory systems**: Test memory storage and retrieval
- **Configuration management**: Test configuration loading and validation

#### Integration Tests
- **Brain integration**: Test complete brain system functionality
- **World integration**: Test environment interaction
- **Agent integration**: Test agent behavior
- **Plugin integration**: Test plugin loading and usage

#### Performance Tests
- **Benchmarking**: Test simulation performance
- **Scalability**: Test scaling to large neural systems
- **Resource usage**: Monitor memory and CPU usage
- **Stress testing**: Test under extreme conditions

### Validation

Scientific validation of NLM capabilities:

#### Scientific Validation
- **Biological plausibility**: Ensure neural models reflect biological principles
- **Mathematical correctness**: Validate numerical implementations
- **Computational efficiency**: Ensure reasonable performance
- **Experimental validation**: Test against real-world data

#### Reproducibility
- **Deterministic behavior**: Ensure consistent results
- **Parameter sensitivity**: Test robustness to parameter changes
- **Cross-platform compatibility**: Ensure consistent behavior across platforms
- **Version control**: Track changes and maintain compatibility

## Contributing

### Getting Started

Contribute to NLM development:

#### Fork and Clone
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Development Workflow
```bash
# Create feature branch
git checkout -b feature/awesome-feature

# Make changes
# ... your changes here ...

# Test your changes
./test_cli.py
pytest tests/

# Commit changes
git add .
git commit -m "Add awesome feature"
git push origin feature/awesome-feature

# Create pull request
# ... create pull request on GitHub ...
```

#### Coding Standards
- **Code style**: Follow existing C++ coding conventions
- **Documentation**: Write comprehensive documentation
- **Testing**: Write unit tests for new functionality
- **Performance**: Optimize performance where possible

#### Documentation
- **API documentation**: Update Python and C++ API documentation
- **User guides**: Update user guides and tutorials
- **Examples**: Add new examples and demonstrations
- **Changelog**: Update changelog with new features

## Support

### Community Support
- **GitHub Issues**: Report bugs and request features
- **Discussion Forum**: Discuss development and usage
- **Stack Overflow**: Ask questions about NLM usage
- **Social Media**: Follow for updates and news

### Professional Support
- **Consulting Services**: Get professional help with NLM
- **Development Services**: Custom NLM development
- **Training Services**: NLM training and workshops
- **Support Plans**: Commercial support packages

## Future Directions

### Planned Features
- **Neuromodulation expansion**: Additional neuromodulators
- **Social learning**: Learning from other agents
- **Metacognition**: Learning about learning
- **Consciousness research**: Investigating conscious experience
- **Brain-machine interfaces**: Direct interaction with biological brains

### Research Areas
- **Cognitive architectures**: Compare with human cognition
- **Neural development**: Study brain development processes
- **Memory systems**: Investigate memory architecture
- **Learning theory**: Develop theoretical foundations
- **Artificial intelligence**: Advance AI capabilities

## Acknowledgments

### Contributors
- **Original developers**: The NLM project team
- **Contributors**: All who have contributed to the project
- **Research collaborators**: Academic and industry partners
- **Community members**: All NLM users and enthusiasts

### Funding
- **Research grants**: Support from research organizations
- **University support**: Institutional backing
- **Industry partnerships**: Collaboration with technology companies
- **Open source support**: Community and corporate sponsorship

### Software Dependencies
- **C++20**: Modern C++ features and standard library
- **CMake**: Cross-platform build system
- **pybind11**: Python bindings for C++
- **SCBoost**: Modern build system for Python bindings

## License

MIT License

Copyright (c) 2024 NLM Project

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
