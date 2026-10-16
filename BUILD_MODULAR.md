# NLM Modular CMake Build System

This document describes the new modular CMake build system for the Neural Learning Machine (NLM) project.

## Overview

The build system has been refactored from a monolithic structure into a modular system with separate CMakeLists.txt files for each component. This provides:

- **Better Performance**: Eliminates file(GLOB) usage for explicit file listing
- **Improved Maintainability**: Clear separation of concerns
- **Better IDE Support**: Explicit source files for auto-completion
- **Easier Debugging**: Smaller, focused targets
- **Reusability**: Components can be used independently
- **Scalability**: Easy to add new components
- **Documentation**: CMake structure documents component relationships

## Module Structure

### Core Libraries

1. **nlm_core** - Core system components
   - Files: Config, Logger, Random, SimulationClock, Types
   - Dependencies: nlohmann_json, yaml-cpp

2. **nlm_brain** - Neural computation components
   - Files: Brain, Neurons, Synapses, Neural regions
   - Dependencies: nlm_core

3. **nlm_memory** - Memory systems
   - Files: Working memory, Episodic memory, Associative memory
   - Dependencies: nlm_core, nlm_brain

4. **nlm_prediction** - Prediction system
   - Files: Neural prediction, Prediction algorithms
   - Dependencies: nlm_core, nlm_brain, nlm_memory

5. **nlm_cognition** - Cognitive systems
   - Files: Planning, Concept formation, Attention
   - Dependencies: nlm_core, nlm_brain, nlm_memory

6. **nlm_neuromodulation** - Neuromodulation systems
   - Files: Reward, Prediction error, Novelty
   - Dependencies: nlm_core, nlm_brain

7. **nlm_development** - Development system
   - Files: Synaptogenesis, Pruning, Maturation
   - Dependencies: nlm_core, nlm_brain, nlm_neuromodulation

8. **nlm_agent** - Agent brain interface
   - Files: Agent interface, Perception
   - Dependencies: nlm_core, nlm_brain, nlm_neuromodulation, nlm_cognition, nlm_memory

9. **nlm_world** - Environment/world system
   - Files: Environment, Perception
   - Dependencies: nlm_core, nlm_brain, nlm_agent

10. **nlm_phase** - Phase-specific demo/experiment systems
    - Files: Phase experiments, Metrics, Demos
    - Dependencies: nlm_core, nlm_brain, nlm_memory, nlm_neuromodulation, nlm_cognition

### Python Bindings

11. **nlm_python** - Python bindings module
    - Provides Python interface to NLM C++ components
    - Uses pybind11 for Python integration

### Executables

12. **nlm_unified_cli** - Main unified command-line interface
    - Combines all necessary components
    - Provides command-line access to NLM functionality

### Testing

13. **nlm_test** - Test suite
    - Comprehensive unit tests for all components
    - Uses explicit file listing for better performance

## Build Configuration

### Dependencies

The project depends on:
- **nlohmann_json**: JSON parsing
- **yaml-cpp**: YAML configuration
- **pybind11**: Python bindings (optional)

### Installation

```bash
mkdir build
cd build
cmake ..
cmake --build .
make install
```

### Testing

```bash
mkdir build_test
cd build_test
cmake -DBUILD_TESTS=ON ..
cmake --build .
ctest
```

## Benefits

1. **Better Performance**: No file(GLOB) - explicit file listing allows incremental builds
2. **Improved IDE Support**: All source files explicitly listed for auto-completion
3. **Easier Debugging**: Smaller, focused targets make debugging easier
4. **Reusability**: Components can be used independently
5. **Scalability**: Easy to add new components without modifying main CMakeLists.txt
6. **Dependency Management**: Explicit dependency declarations
7. **Documentation**: CMake structure documents component relationships

## Adding New Components

To add a new component:

1. Create a new directory under `src/`
2. Create a CMakeLists.txt file in the new directory
3. Add the new `add_subdirectory()` call to the main CMakeLists.txt
4. Implement the source files in the new directory
5. Update installation if needed

## Module Dependencies

The dependency graph is defined in each component's CMakeLists.txt:

```
nlm_core (nlohmann_json, yaml-cpp)
├── nlm_brain
├── nlm_memory (nlm_core, nlm_brain)
├── nlm_prediction (nlm_core, nlm_brain, nlm_memory)
├── nlm_cognition (nlm_core, nlm_brain, nlm_memory)
├── nlm_neuromodulation (nlm_core, nlm_brain)
├── nlm_development (nlm_core, nlm_brain, nlm_neuromodulation)
├── nlm_agent (nlm_core, nlm_brain, nlm_neuromodulation, nlm_cognition, nlm_memory)
├── nlm_world (nlm_core, nlm_brain, nlm_agent)
└── nlm_phase (nlm_core, nlm_brain, nlm_memory, nlm_neuromodulation, nlm_cognition)
```

## Customization

You can customize the build by modifying the CMakeLists.txt files in each component:
- Change compiler flags
- Add additional dependencies
- Modify installation paths
- Configure build options

## Troubleshooting

### Common Issues

1. **Missing dependencies**: Ensure all required libraries are installed
2. **Build failures**: Check compiler versions and compatibility
3. **Linking errors**: Verify dependency declarations in CMakeLists.txt
4. **Test failures**: Run tests individually to isolate issues

### Performance Tips

- Use out-of-source builds
- Consider using ccache for faster compilation
- Parallel builds: `cmake --build . -- -j$(nproc)`

## Version Information

- CMake Minimum Version: 3.16
- C++ Standard: C++20
- Build System: Modular CMake with explicit file listing
- Package Version: 1.0.0
