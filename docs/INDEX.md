# NLM Documentation Index

## Welcome to NLM Documentation

This document provides an overview of all available documentation in the NLM (Neural Learning Machine) project. NLM is an experimental artificial developmental brain project that implements spiking neural dynamics, learning, memory, and cognition.

## Documentation Structure

### Core Documentation (High Priority)

#### [API_REFERENCE.md](docs/API_REFERENCE.md)
- **Purpose**: Complete API documentation for all major classes
- **Content**: Detailed class descriptions, method signatures, parameters, and usage examples
- **Audience**: Developers and advanced users who need to work with the NLM API
- **Key Features**:
  - Core brain components documentation
  - Plasticity system details
  - Memory system descriptions
  - Configuration parameter specifications

#### [TUTORIAL.md](docs/TUTORIAL.md)
- **Purpose**: Step-by-step beginner guide
- **Content**: Progressive tutorials from simple examples to complex applications
- **Audience**: New users and those learning NLM for the first time
- **Key Features**:
  - Minimal example with single neuron
  - Agent example with world interaction
  - Advanced usage examples
  - Best practices and troubleshooting

#### [CONFIG_GUIDE.md](docs/CONFIG_GUIDE.md)
- **Purpose**: Complete configuration system guide
- **Content**: Configuration parameters, file formats, command-line options
- **Audience**: Users who need to customize NLM behavior
- **Key Features**:
  - Parameter descriptions and ranges
  - Configuration file examples
  - Command-line usage
  - Validation and error handling

### Architecture and Background Documentation

#### [ARCHITECTURE.md](docs/ARCHITECTURE.md)
- **Purpose**: Detailed component documentation and usage examples
- **Content**: Architecture overview with detailed component descriptions
- **Audience**: System architects and developers
- **Key Features**:
  - Core layer components (Types, Config, Random, Logger, SimulationClock)
  - Brain layer components (Neuron, Synapse, NeuralPopulation, NeuralRegion, Brain)
  - Dynamics layer components (NeuralDynamics, SpikeSystem)
  - Plasticity layer components (PlasticityRule, STDP, Hebbian, StructuralPlasticity)
  - Development layer components (DevelopmentSystem, Synaptogenesis, Pruning, Maturation)
  - Neuromodulation layer components (Neuromodulator, Dopamine, Novelty, Curiosity, PredictionError)
  - Memory systems (NeuralWorkingMemory, NeuralEpisodicMemory, etc.)
  - Prediction systems (NeuralPrediction, ActionConsequencePredictor)
  - Cognitive systems (AttentionalSelection, NeuralPlanner, SelfModel, SocialLearning)
  - Sensory and motor systems
  - Environment and experiment systems

#### [SCIENCE.md](docs/SCIENCE.md)
- **Purpose**: Practical guidance alongside scientific background
- **Content**: Biological inspiration, computational implementations, and known limitations
- **Audience**: Researchers and developers interested in the scientific basis
- **Key Features**:
  - Neural plasticity mechanisms and biological inspiration
  - Neural dynamics and integrate-and-fire models
  - Developmental learning processes
  - Neuromodulation and brain chemistry analogies
  - Phase 4 emerging cognition with prediction, memory, and attention systems
  - Eligibility traces and credit assignment
  - Developmental plasticity
  - What NLM is NOT and what it IS

### Usage and Support Documentation

#### [HOW_TO_USE.md](HOW_TO_USE.md)
- **Purpose**: Python API usage examples and advanced usage scenarios
- **Content**: Python bindings usage, examples, and troubleshooting
- **Audience**: Users working with the Python API
- **Key Features**:
  - C++ project building instructions
  - Python library installation and usage
  - Complete Python API examples
  - Configuration options
  - Advanced usage patterns

#### [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md)
- **Purpose**: Common issues and solutions
- **Content**: Problem diagnosis and resolution strategies
- **Audience**: Users experiencing difficulties with NLM
- **Key Features**:
  - Getting started issues and solutions
  - Configuration problems
  - Simulation issues
  - Performance problems
  - Memory and resource issues
  - Learning and plasticity issues
  - Advanced debugging techniques
  - Community support information

### Development Documentation

#### Example Scripts

The NLM project includes five example scripts that demonstrate different aspects of the system:

1. **minimal_example.cpp** - Basic brain simulation demonstrating the simplest possible NLM example
   - Creates and initializes a brain with 100 neurons
   - Shows basic neuron dynamics and spike observation
   - Demonstrates the fundamental NLM architecture

2. **agent_example.cpp** - Complete agent with world interaction
   - Demonstrates brain-world interaction patterns
   - Shows sensory processing and motor output
   - Includes learning system integration

3. **exploration_example.cpp** - Curiosity-driven exploration
   - Demonstrates curiosity and novelty-driven behavior
   - Shows environment exploration strategies
   - Highlights adaptive exploration mechanisms

4. **development_example.cpp** - Developmental learning
   - Demonstrates brain development over time
   - Shows plasticity changes across developmental stages
   - Includes developmental milestone tracking

5. **memory_example.cpp** - Working with memory systems
   - Demonstrates NLM's memory capabilities
   - Shows working memory, episodic memory, and concept formation
   - Highlights memory integration with neural processing

#### [INDEX.md](docs/INDEX.md) (This file)
- **Purpose**: Navigation and quick reference
- **Content**: Overview of all documentation with cross-references
- **Audience**: All users looking for documentation

#### [CONTRIBUTE.md](docs/CONTRIBUTE.md)
- **Purpose**: Guidelines for developers contributing to NLM
- **Content**: Contribution guidelines, code standards, and development practices
- **Audience**: Developers interested in extending NLM
- **Key Features**:
  - Code contribution guidelines
  - Documentation standards
  - Testing requirements
  - Performance considerations
  - Version control policies
  - Community guidelines

## Documentation Usage Guide

### Finding Specific Information

Use the following search patterns to locate specific documentation:

1. **API Reference**: Look in `API_REFERENCE.md` for class and method documentation
2. **Tutorial**: Start with `TUTORIAL.md` for step-by-step guidance
3. **Configuration**: Check `CONFIG_GUIDE.md` for parameter descriptions
4. **Architecture**: Refer to `ARCHITECTURE.md` for system design
5. **Science**: Read `SCIENCE.md` for biological background
6. **Troubleshooting**: Consult `TROUBLESHOOTING.md` for problem solutions
7. **Examples**: Run the example scripts in `examples/` directory

### Navigation Tips

- **Cross-references**: Most documentation files include links to related sections
- **Search**: Use search tools to find specific terms or concepts
- **Examples**: Code examples are provided throughout documentation
- **API Documentation**: `API_REFERENCE.md` serves as the comprehensive API guide

### Getting Started with Documentation

#### For New Users:
1. Start with `TUTORIAL.md` for step-by-step guidance
2. Read `HOW_TO_USE.md` for Python API usage
3. Explore `API_REFERENCE.md` for API details
4. Refer to `TROUBLESHOOTING.md` for common issues

#### For Developers:
1. Begin with `ARCHITECTURE.md` for system design
2. Study `SCIENCE.md` for implementation background
3. Consult `CONFIG_GUIDE.md` for configuration options
4. Review `CONTRIBUTE.md` for development guidelines

#### For Researchers:
1. Focus on `SCIENCE.md` for biological basis
2. Use `ARCHITECTURE.md` for implementation details
3. Refer to `EXPERIMENTS.md` for experiment descriptions
4. Consult `PERFORMANCE.md` for optimization techniques

## Documentation Standards

### Consistency
- All documentation follows Markdown format
- Code examples are formatted with proper syntax highlighting
- Cross-references are used throughout
- Consistent terminology and naming conventions

### Completeness
- Each major topic has dedicated documentation
- API documentation includes parameter descriptions
- Examples demonstrate both simple and advanced usage
- Troubleshooting sections provide practical solutions

### Accessibility
- Documentation is organized for progressive learning
- Clear explanations for users at different skill levels
- Examples are runnable and demonstrate key concepts
- Common pitfalls and their solutions are documented

## Future Documentation Development

### Priority Areas for Expansion:
1. **Phase 4 Deep Dive**: More detailed documentation for emerging cognition systems
2. **Performance Guide**: Comprehensive performance optimization techniques
3. **Memory System Documentation**: Detailed documentation for all memory subsystems
4. **Developmental Psychology**: Better integration of developmental theory
5. **Advanced Tutorials**: Complex use cases and research applications

### Contribution Opportunities:
- **Documentation**: Help expand and improve existing documentation
- **Examples**: Add new example scripts for specific use cases
- **Tutorials**: Create tutorials for specialized topics
- **Troubleshooting**: Document common issues and solutions
- **API Reference**: Add documentation for new classes and methods

## Conclusion

The NLM documentation provides comprehensive coverage of the system's architecture, functionality, and usage. The organized structure allows users to progressively build their understanding from basic tutorials to advanced implementation details. All documentation follows best practices and maintains consistency with the project's goals of accessibility and completeness.

## Quick Reference Links

- **Main Documentation**: [API_REFERENCE.md](docs/API_REFERENCE.md)
- **Getting Started**: [TUTORIAL.md](docs/TUTORIAL.md)
- **Configuration**: [CONFIG_GUIDE.md](docs/CONFIG_GUIDE.md)
- **System Architecture**: [ARCHITECTURE.md](docs/ARCHITECTURE.md)
- **Scientific Background**: [SCIENCE.md](docs/SCIENCE.md)
- **Usage Examples**: [HOW_TO_USE.md](HOW_TO_USE.md)
- **Problem Solving**: [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md)
- **Examples**: [examples/ directory](examples/)
- **Development Guidelines**: [CONTRIBUTE.md](docs/CONTRIBUTE.md)

This documentation serves as the foundation for understanding and extending the NLM project, providing the knowledge and tools needed to effectively use and contribute to this experimental brain simulation system.
