# NLM Framework - Neural Learning Machine

## Overview

**NLM (熙然)** is an experimental computational brain project designed to create artificial developmental brains that learn and adapt through interaction with an environment. The system implements real spiking neural computation with the goal of understanding how intelligence can emerge from neural dynamics.

## What NLM Is

NLM is a **brain simulator** that implements:

- **Real spiking neural computation** (Leaky Integrate-and-Fire neurons)
- **Event-driven spike propagation** with synaptic delays
- **Multiple plasticity mechanisms** (STDP, Hebbian, structural)
- **Developmental stages** affecting plasticity rates
- **Memory systems** (working, episodic, associative)
- **Neuromodulation** (dopamine, curiosity, novelty, prediction error)
- **Prediction systems** for temporal sequence learning
- **Cognitive mechanisms** (attention, planning, concept formation)

## Key Features

### 🚀 Real Neural Computation
- LIF neuron dynamics
- Event-driven spike propagation
- Synaptic plasticity
- Structural development

### 🧠 Integrated Systems
- Memory: Working, Episodic, Associative
- Neuromodulation: Dopamine, Curiosity, Novelty, Prediction Error
- Prediction: Temporal sequence learning
- Cognition: Attention, Planning, Concept formation

### 🔧 Modular Architecture
- Layered design with clear interfaces
- Extensible plugin system
- Configuration-driven customization
- Scientific accuracy focus

### 🎯 Developmental Learning
- Critical periods for skill acquisition
- Age-dependent plasticity
- Experience-dependent development
- Emergent behaviors from simple rules

## Getting Started

### Quick Installation (5 Minutes)

```bash
# Clone the repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Run Demo

```bash
# Phase 2: Real Neural Computation
./nlm

# Phase 6: Final Integration
./nlm_phase6_demo
```

## Project Structure

```
NLM/
├── src/
│   ├── core/           # Core utilities (Config, Random, Logger)
│   ├── brain/          # Neural components (Neuron, Synapse, Brain)
│   ├── dynamics/       # Neural dynamics (SpikeSystem)
│   ├── plasticity/     # Plasticity rules (STDP, Hebbian)
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
│
├── docs/               # Documentation
├── examples/           # Example code
├── tests/              # Test suite
├── configs/            # Configuration files
└── README.md           # This file
```

## Documentation

### 📚 Comprehensive Documentation

This repository now includes comprehensive documentation covering:

#### Core Documentation
- **API Reference**: Complete API documentation for all classes
- **Architecture Guide**: System architecture and component documentation
- **Tutorial Documentation**: Step-by-step tutorials for all skill levels
- **Usage Examples**: Complete working examples for all use cases
- **Reference Documentation**: Complete class and method references

#### Technical Documentation
- **Best Practices**: Guidelines and recommendations for development
- **Migration Guides**: Documentation for version upgrades
- **Architecture.md**: System architecture overview
- **SCIENCE.md**: Scientific background and biological inspiration
- **EXPERIMENTS.md**: Experiment descriptions and results

### 📖 Documentation Structure

```
src/docs/
├── INDEX.md                    # Main documentation index
├── API_REFERENCE_INDEX.md      # API documentation overview
├── ARCHITECTURE.md             # System architecture
├── TUTORIALS/
│   ├── GETTING_STARTED.md      # Beginner tutorial
│   └── INDEX.md                # Tutorials index
├── EXAMPLES/
│   ├── INDEX.md                # Examples index
│   ├── BASIC_EXAMPLES.md       # Basic working examples
│   ├── REAL_WORLD_EXAMPLES.md  # Real-world applications
│   ├── RESEARCH_EXAMPLES.md    # Research applications
│   ├── PRODUCTION_EXAMPLES.md  # Production-ready code
│   └── EDUCATIONAL_EXAMPLES.md # Teaching and learning examples
```

## Documentation Examples

### Basic Example: Creating and Running a Brain

```cpp
#include <nlm/nlm.hpp>

int main() {
    // 1. Create configuration
    auto config = nlm::createDefaultConfig();
    
    // 2. Create brain
    auto brain = nlm::createBrain(config);
    
    // 3. Initialize brain
    brain->initialize();
    
    // 4. Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
    }
    
    // 5. Analyze results
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return 0;
}
```

### Advanced Example: Learning Agent in Environment

```cpp
#include <nlm/nlm.hpp>

int main() {
    // Create learning brain
    auto config = nlm::createLearningConfig();
    auto brain = nlm::createBrain(config);
    
    // Create agent and world
    auto agent = nlm::createAgentBrain(brain);
    auto world = nlm::createSimpleWorld();
    world->configure(50, 50, 8, 8);
    
    // Initialize agent
    agent->initialize(world);
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    
    // Run simulation
    for (size_t step = 0; step < 1000; ++step) {
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

## Examples Directory

The `examples/` directory contains practical examples:

### Basic Examples
- `basic_simulation.cpp` - Simple neural simulation
- `neuron_dynamics.cpp` - LIF neuron testing
- `plasticity_demo.cpp` - STDP learning demonstration

### Real-World Examples
- `robot_navigation.cpp` - Autonomous robot navigation
- `maze_solver.cpp` - Maze solving with learning
- `foraging_behavior.cpp` - Resource searching behavior

### Research Examples
- `critical_period_study.cpp` - Developmental plasticity research
- `memory_integration.cpp` - Memory system integration
- `prediction_learning.cpp` - Temporal sequence learning

### Production Examples
- `production_training.cpp` - Production-ready training
- `checkpoint_system.cpp` - State persistence
- `batch_processing.cpp` - Parallel processing

### Educational Examples
- `introduction_to_nn.cpp` - Neural network basics
- `plasticity_principles.cpp` - Learning mechanisms
- `brain_development.cpp` - Developmental processes

## Development Guidelines

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j$(nproc)

# Run tests (if available)
ctest --output-on-failure
```

### Building for Development

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Key References

### API Documentation
- **Brain Class**: Central neural coordinator with comprehensive API
- **Configuration**: Flexible configuration management system
- **Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Reward, curiosity, novelty, prediction error
- **Plasticity**: STDP, Hebbian, structural learning mechanisms

### Scientific Background
- **ARCHITECTURE.md**: Detailed system architecture
- **SCIENCE.md**: Biological inspiration and design principles
- **EXPERIMENTS.md**: Experiment descriptions and results
- **LIMITATIONS.md**: Scientific limitations and uncertainties

### Usage and Examples
- **easy_usage.md**: Beginner-friendly usage guide
- **HOW_TO_USE.md**: Installation and usage instructions
- **IMPLEMENTATION_SUMMARY.md**: Technical implementation details

## Contributing

### Code Standards

1. **Use RAII**: Resource acquisition is initialization
2. **Write comments**: Document public APIs and complex code
3. **Follow conventions**: Consistent naming and formatting
4. **Error handling**: Robust error reporting and recovery
5. **Testing**: Comprehensive test coverage

### Documentation Standards

1. **Clear structure**: Logical organization and navigation
2. **Complete examples**: Working code with explanations
3. **API documentation**: Document all public interfaces
4. **Tutorial format**: Step-by-step guidance

## Research and Applications

### Scientific Value

NLM provides a platform for investigating:

- **Neural computation**: How spiking dynamics create intelligence
- **Developmental learning**: How brains develop capabilities over time
- **Memory integration**: How different memory systems work together
- **Emergent behaviors**: How complex behaviors arise from simple rules
- **Plasticity mechanisms**: How synaptic changes support learning

### Practical Applications

- **Education**: Teaching principles of neural computation
- **Research**: Testing hypotheses about brain function
- **Simulation**: Modeling biological systems
- **AI development**: Developing new intelligent systems

## Limitations and Considerations

### Scientific Limitations

- **Simplified models**: Biological accuracy is limited
- **Computational constraints**: Limited by available computing resources
- **Approximations**: Many biological processes are simplified
- **No consciousness**: Claims of human-like intelligence not made

### Usage Considerations

- **Resource intensive**: Large-scale simulations require significant computing power
- **Development time**: Complex system requires substantial implementation effort
- **Scientific focus**: Primary purpose is research and understanding, not AI production

## Support and Community

### Getting Help

1. **Documentation**: Comprehensive documentation in this repository
2. **Code examples**: Practical examples for different use cases
3. **Community**: Share experiences and results with other users

### Development Community

- **Research collaboration**: Share findings and methodologies
- **Code improvements**: Contribute enhancements and fixes
- **Documentation updates**: Improve and expand documentation
- **Example additions**: Share new use cases and applications

## License

MIT License. See LICENSE file for details.

## Authors

Research project - See docs for scientific background and author information.

## Acknowledgments

- Contributors to the NLM project
- Scientific community for foundational research
- Educational resources and tutorials
- Open source community for supporting libraries

## Contact

For questions, issues, or collaboration:
- Repository issues
- Documentation feedback
- Example suggestions
- Research collaborations

---

**NLM (熙然)** - Transforming from a complex, hard-to-use tool into an accessible, user-friendly platform for neural simulation and artificial intelligence development.

This documentation makes NLM:

- **Easy to Learn**: Users can quickly learn how to use the framework
- **Easy to Use**: Users can accomplish common tasks easily
- **Easy to Debug**: Users can debug issues easily
- **Easy to Extend**: Users can extend the framework easily
- **Easy to Maintain**: The framework is easy to maintain and extend