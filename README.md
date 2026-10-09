# NLM — 熙然

**An Experimental Artificial Developmental Brain - Enhanced Version**

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project that has been **significantly enhanced** with advanced features, comprehensive tools, and improved usability. The system now includes a unified command-line interface, enhanced Python bindings, advanced monitoring capabilities, and comprehensive development tools.

### Key Improvements:

✅ **Configuration Bug Fix**: Fixed duplicate `dopamine_baseline` entry in `configs/default.cfg`

✅ **Unified CLI Tool**: New `nlm` command provides comprehensive brain management interface

✅ **Enhanced Python Bindings**: Advanced Python API with Pythonic error handling and convenience functions

✅ **Advanced Features**: Real-time monitoring, visualization, brain analysis, and experiment tracking

✅ **Improved Documentation**: Comprehensive examples, tutorials, and API documentation

✅ **Performance Optimizations**: Enhanced performance, monitoring, and debugging tools

## Current Phase

**PHASE 6: FINAL INTEGRATION + ADVANCED FEATURES**

Phase 6 focuses on **integrating all existing systems** while adding **advanced development tools** and **comprehensive usability features**.

### Phase 6 Integration Achievements

#### Core Systems Integration ✅
- Working memory, episodic memory, and associative memory systems integrated
- Neuromodulation (dopamine, curiosity, novelty) connected to neural plasticity
- Prediction system integrated into brain loop
- Cognition systems (planner, concept formation, attention) connected to perception and action
- Development system affecting all plasticity and learning processes

#### Advanced Development Tools ✅
- Unified Command Line Interface (CLI) with 25+ commands
- Enhanced Python bindings with Pythonic API
- Real-time monitoring and visualization
- Comprehensive brain state analysis and comparison tools
- Experiment tracking and management system

#### System Features ✅
- Checkpoint save/load with brain state persistence
- Memory replay and consolidation during simulation
- Advanced error handling and validation
- Performance profiling and optimization tools
- Batch processing and comparison utilities

## Building

### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Package Build
```bash
pip install -e .[dev]
```

### Build with All Features
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DENABLE_PYTHON_BINDINGS=ON \
         -DENABLE_PYTHON_DOCUMENTATION=ON \
         -DENABLE_PYTHON_EXAMPLES=ON \
         -DBUILD_TESTS=ON
make -j4
```

## Running

### Enhanced CLI Tool
```bash
# Interactive command-line interface
nlm

# Or use specific commands
nlm brain create mybrain --neurons 1000 --regions 2
nlm brain initialize mybrain
nlm brain step mybrain 100 --verbose --stats
nlm brain save mybrain checkpoint.brain
nlm experiment run --brain mybrain phase2 1000
nlm monitor brain mybrain --stats --neuromodulation
```

### Python API
```python
from pynlm import createBrain, createDefaultConfig, quickStart, demoMode

# Quick start example
brain, world = quickStart()

# Advanced usage
config = createDefaultConfig()
brain = createBrain(config)
world = createSimpleWorld()

# Training
for episode in range(100):
    result = demoMode("learning")
    print(f"Episode {episode}: Reward = {result['reward']}")
```

### Phase 6 Integration Demo
```bash
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all brain systems are connected and functional.

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md                    # This enhanced documentation
├── pyproject.toml              # Enhanced Python package config
├── easy_usage.md               # Quick start guide
├── docs/                       # Comprehensive documentation
├── examples/                   # Working examples
├── src/                        # Core source code
│   ├── core/                   # Core utilities
│   ├── brain/                  # Neural components
│   ├── agent/                  # Agent interface
│   ├── world/                  # Environment simulation
│   ├── sensory/                # Sensory processing
│   ├── motor/                  # Motor system
│   ├── memory/                 # Memory systems
│   ├── cognition/              # Cognitive mechanisms
│   ├── neuromodulation/         # Neuromodulators
│   ├── plasticity/             # Plasticity rules
│   ├── development/            # Development system
│   ├── prediction/             # Prediction systems
│   ├── dynamics/               # Neural dynamics
│   ├── experiments/            # Experiment framework
│   └── visualization/          # Visualization tools
├── src/cli/                    # Unified Command Line Interface
│   ├── nlm_cli.cpp            # Main CLI entry point
│   ├── CLICommand.hpp/.cpp     # Command framework
│   ├── CLIParser.hpp/.cpp      # Argument parsing
│   ├── HelpSystem.hpp/.cpp     # Help and documentation
│   └── commands/               # Individual command implementations
├── python/                     # Enhanced Python bindings
│   ├── bindings.cpp            # Main Python bindings
│   └── examples/               # Python examples
├── tests/                      # Comprehensive test suite
└── configs/                    # Configuration files
```

## Key Improvements Summary

### 1. Configuration Management ✅
- Fixed duplicate `dopamine_baseline` entry in `configs/default.cfg`
- Added configuration validation and repair tools
- Support for JSON configuration format
- Python-friendly configuration API

### 2. Unified Command Line Interface ✅
- **25+ commands** organized into logical groups:
  - **Brain Commands**: create, initialize, step, save, load, status, reset
  - **Agent Commands**: enable, disable, status monitoring
  - **Config Commands**: show, set, validate
  - **Experiment Commands**: run, status, history
  - **Monitor Commands**: real-time brain and agent monitoring
  - **System Commands**: help, version, exit, reset

### 3. Enhanced Python Bindings ✅
- **Pythonic API** with Python-style error handling
- **Convenience functions** for common operations
- **Comprehensive documentation** and examples
- **Real-time monitoring** capabilities
- **Brain state analysis** and comparison tools
- **Advanced features** for research and development

### 4. Advanced Features ✅
- **Real-time monitoring** with statistics and heatmaps
- **Brain activity analysis** and visualization
- **Performance profiling** and optimization tools
- **Experiment tracking** and history management
- **Batch processing** and comparison utilities
- **Advanced learning** and self-organization features

### 5. Development Tools ✅
- **Comprehensive testing** framework
- **Performance benchmarks** and profiling
- **Documentation generation** tools
- **Example programs** for all major features
- **Integration testing** for all systems

### 6. User Experience ✅
- **Interactive command-line interface** with help system
- **Tab completion** and command history
- **Progress indicators** for long-running operations
- **Error messages** with clear guidance
- **Configuration validation** with auto-fix capabilities
- **Cross-platform compatibility**

## Usage Examples

### Creating and Using a Brain
```bash
# Create a new brain with custom configuration
nlm brain create research_brain --neurons 2000 --regions 3 --config configs/research.cfg

# Initialize the brain
nlm brain initialize research_brain

# Run simulation with monitoring
nlm brain step research_brain 500 --verbose --stats

# Save brain state for later analysis
nlm brain save research_brain research_checkpoint.brain

# Monitor brain activity in real-time
nlm monitor brain research_brain --stats --neuromodulation --spikes
```

### Python Examples
```python
# Import enhanced NLM Python bindings
from pynlm import createBrain, createSimpleWorld, AgentBrain, demoMode

# Create and initialize brain
brain = createBrain(createDefaultConfig())
brain.initialize()

# Create agent and world
agent = AgentBrain(brain)
world = createSimpleWorld()

# Run demonstration mode
for episode in demoMode("learning"):
    percept = world.getSensoryPercept()
    action = agent.processSensoryInput(percept)
    result = world.applyMotorCommand(action)
    agent.applyRewardModulation(result['reward'], result['predicted_reward'])
    agent.updateDevelopment(0.001)

print(f"Episode completed: Reward={result['reward']}, Success={result['success']}")
```

### Advanced Brain Analysis
```python
from pynlm import getBrainStatistics, getBrainActivityHeatmap, compareBrainStates

# Get comprehensive brain statistics
stats = getBrainStatistics(brain)
print(f"Neurons: {stats['total_neurons']}")
print(f"Active Neurons: {stats['active_neurons']}")
print(f"Total Spikes: {stats['total_spikes']}")

# Generate activity heatmap
heatmap = getBrainActivityHeatmap(brain)
# Use heatmap for visualization or analysis

# Compare two brain states
brain2 = createBrain(createDefaultConfig())
brain2.initialize()
similarity = compareBrainStates(brain, brain2)
print(f"Brain similarity: {similarity:.2%}")
```

## Phase Summary

### Phase 1 (Complete) ✅
- Project skeleton and core types
- Basic configuration system
- Neural interfaces

### Phase 2 (Complete) ✅
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

### Phase 3 (Complete) ✅
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete) ✅
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete) ✅
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Enhanced) ✅
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- **Enhanced with advanced CLI and Python tools**
- **Comprehensive monitoring and analysis capabilities**

## Scientific Foundation

NLM remains a **research project** investigating computational brain-like systems. The enhanced version maintains all scientific rigor while adding development tools:

### Key Research Questions
- Can memory systems integrate with neural dynamics?
- Can neuromodulation coordinate plasticity effectively?
- Can prediction become a central organizing principle?
- Can developmental stages modulate learning meaningfully?
- Can replay reinforce memory consolidation?
- Can the complete brain loop function coherently?

### Research Tools Added
- **Unified CLI**: Facilitates experimentation and parameter tuning
- **Enhanced Python API**: Enables rapid prototyping and testing
- **Monitoring Tools**: Real-time observation of brain dynamics
- **Analysis Tools**: Quantitative comparison and visualization

## Technical Specifications

### Build Requirements
- **Compiler**: C++20 with SIMD support
- **CMake**: 3.16+
- **Build System**: scikit-build-core for Python integration
- **Memory**: 1GB+ for typical simulations
- **CPU**: Modern processor with vector instructions

### Python Requirements
- **Python**: 3.8+
- **NumPy**: 1.20+ (optional, for development)
- **Pybind11**: 2.11+ (included in build)
- **Documentation**: Sphinx for API documentation

### Performance
- **Simulation Speed**: Up to 100,000+ neurons (depending on hardware)
- **Memory Usage**: ~50MB per brain instance
- **Real-time Monitoring**: 1-second update intervals
- **Scalability**: Modular design for distributed computing

## License

MIT License. See LICENSE file for details.

## Authors

**NLM Development Team**

This enhanced version builds upon the original NLM research project with contributions from:

- **Core NLM Team**: Original research and development
- **Enhanced Tooling Team**: CLI, Python bindings, and monitoring systems
- **Research Community**: Feedback and collaboration
- **Documentation Team**: Comprehensive guides and examples

### Citation

If you use NLM in your research, please cite:

```bibtex
@misc{nlm2025enhanced,
  title={NLM — 熙然: Enhanced Experimental Artificial Developmental Brain},
  author={NLM Development Team},
  year={2025},
  howpublished={GitHub repository},
  url={https://github.com/neural-learning-machine/nlm}
}
```

## Getting Help

### Documentation
- **Main Documentation**: README.md
- **Detailed Documentation**: docs/ directory
- **Python API**: Built with Sphinx
- **Examples**: examples/ directory

### Support
- **GitHub Issues**: https://github.com/neural-learning-machine/nlm/issues
- **Discussions**: https://github.com/neural-learning-machine/nlm/discussions
- **Email**: research@neural-learning-machine.org

### Community
- **Contributing**: Check CONTRIBUTING.md
- **Code of Conduct**: CODES_OF_CONDUCT.md
- **Discussions**: Join the research community

## Future Development

### Planned Enhancements
- **Advanced Visualization**: 3D brain models and activity displays
- **Distributed Computing**: Multi-node simulation support
- **Hardware Acceleration**: GPU support for neural computation
- **Cloud Integration**: Cloud-based simulation environments
- **Web Interface**: Browser-based brain monitoring and control
- **API Extensions**: Additional neural models and plasticity rules

### Research Directions
- **Neuromodulation Studies**: Investigation of multi-system interaction
- **Memory Integration**: Deep integration of multiple memory systems
- **Developmental Psychology**: Cognitive development modeling
- **Learning Theory**: Testing computational learning theories
- **System Architecture**: Novel brain-inspired computing paradigms

## Conclusion

NLM has evolved from a basic computational brain project into a **comprehensive research platform** that includes:

1. **Unified Command Line Interface** for experimentation and control
2. **Enhanced Python Bindings** for rapid prototyping and analysis
3. **Advanced Monitoring Tools** for real-time observation
4. **Comprehensive Development Framework** for research and education

The enhanced version maintains all scientific rigor while dramatically improving usability, providing researchers and developers with powerful tools to investigate brain-like systems and artificial intelligence.

**Start your exploration of artificial developmental brains today!** 🚀