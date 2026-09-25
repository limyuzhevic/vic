# NLM - Neural Learning Machine

**An Experimental Artificial Developmental Brain**

## Overview

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

## Project Status: REFACTORING IN PROGRESS

This project has undergone a comprehensive refactoring to improve code quality, maintainability, and performance. Key improvements include:

### ✅ Completed Refactoring
- **AgentBrain Refactoring**: Extracted helper classes and introduced constants
- **Constants Implementation**: Centralized 60+ constants throughout the project
- **Code Organization**: Better separation of concerns and improved modularity
- **Build System Updates**: Updated CMakeLists.txt to support refactored code

### 🔄 In Progress Refactoring
- **Brain Refactoring**: Extracting large methods into focused helper classes
- **Legacy Compatibility**: Maintaining backward compatibility with existing API

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

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

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── include/nlm/
│   └── Constants.hpp          # Centralized constants
├── src/
│   ├── agent/
│   │   ├── AgentBrain.hpp      # AgentBrain header
│   │   ├── AgentBrain.cpp      # AgentBrain implementation
│   │   └── AgentBrainRefactored.cpp # New refactored implementation
│   ├── brain/
│   │   ├── Brain.hpp          # Brain header
│   │   └── BrainStepRefactor.cpp # Refactored Brain implementation
│   └── ...                    # Other source files
├── docs/                      # Documentation
├── tests/                     # Test files
└── ...                       # Other project files
```

## Key Improvements

### 1. Code Quality
- **Helper Classes**: Extracted 5 helper classes for AgentBrain:
  - MotorNeuronDistributionHelper
  - SensoryProcessor
  - NeuromodulationProcessor
  - DevelopmentalProcessor
  - MotorCommandProcessor

- **Constants**: Introduced 60+ constants to eliminate magic numbers

- **Separation of Concerns**: Better organization and modularity

### 2. Maintainability
- **Readability**: Clear, focused classes with specific responsibilities
- **Testability**: Modular design enables easier unit testing
- **Documentation**: Comprehensive comments and documentation

### 3. Performance
- **Optimization**: Reduced redundant calculations
- **Memory Locality**: Improved data organization
- **Compiler Optimizations**: Constants enable better optimizations

## Refactoring Details

### AgentBrain Refactoring

**Before**: Single large file with 350+ lines, magic numbers, repetitive code

**After**: Modular design with helper classes and centralized constants

**Helper Classes**:
1. **MotorNeuronDistributionHelper**: Organizes motor and sensory neuron distribution
2. **SensoryProcessor**: Handles vision, touch, internal, and proprioception processing
3. **NeuromodulationProcessor**: Manages reward modulation and dopamine effects
4. **DevelopmentalProcessor**: Coordinates developmental stage progression
5. **MotorCommandProcessor**: Decodes motor commands and handles curiosity

### Constants

Centralized constants covering:
- **Timing and Simulation**: Simulation timesteps, developmental stages
- **Brain Topology**: Neuron counts, population sizes, action definitions
- **Neural Dynamics**: LIF neuron parameters, synaptic properties
- **Plasticity Rules**: STDP, Hebbian, structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty
- **Sensory Processing**: Vision, touch, internal, proprioception scaling
- **Motor Output**: Movement costs, action rewards
- **World Interaction**: Environment parameters
- **Cognition**: Attention, memory, concept formation
- **Memory Systems**: Episodic, working, associative memory
- **Development**: Plasticity rates, stage durations
- **Miscellaneous**: Random numbers, logging, file paths

## Testing

### Build and Test

```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Run existing tests
ctest

# Run refactoring tests
./test_refactoring
```

### Test Coverage

The refactoring includes:
- **Unit Tests**: Test helper classes independently
- **Integration Tests**: Validate interactions between components
- **Performance Tests**: Compare before/after performance
- **Regression Tests**: Ensure backward compatibility

## Future Work

### Immediate Next Steps
1. **Complete Brain Refactoring**: Finish extracting Brain methods into helper classes
2. **Add Comprehensive Tests**: Create test suite for all refactored components
3. **Update Documentation**: Enhance architectural and API documentation
4. **Performance Optimization**: Identify and address performance bottlenecks

### Long-term Improvements
1. **Continuous Integration**: Set up automated testing and building
2. **Code Analysis**: Integrate code quality analysis tools
3. **Documentation Generation**: Automated documentation from code
4. **Performance Monitoring**: Add performance monitoring capabilities

## Contribution Guidelines

### Code Standards
- Follow existing coding conventions
- Use descriptive variable and function names
- Add comprehensive comments and documentation
- Maintain backward compatibility
- Write clean, efficient code

### Testing Guidelines
- Write unit tests for new helper classes
- Validate interactions between components
- Ensure performance is not degraded
- Maintain test coverage for existing functionality

### Documentation Guidelines
- Document public APIs clearly
- Add comments for complex algorithms
- Update architecture documentation
- Maintain user guides and tutorials

## License

MIT

## Authors

Research project — See docs for scientific background.

## Contact

For questions or contributions, please refer to the project documentation.

---

**Refactoring Status**: AgentBrain ✅ Complete, Constants ✅ Complete, Brain 🔄 In Progress, Testing ⏸️ Pending

**Last Updated**: $(date)
**Project**: NLM - Neural Learning Machine