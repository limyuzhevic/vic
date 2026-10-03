// Improved directory structure for the NLM project
// This document outlines the recommended refactoring to flatten the directory structure
// and improve code organization

# Improved Directory Structure

## Current Structure Issues

The current NLM project structure has several areas for improvement:

1. **Deep directory nesting**: Some directories have unnecessary subdirectories
2. **Inconsistent naming**: Mixed use of different naming conventions
3. **Missing flat structure**: Some logical components could be better organized

## Recommended Improved Structure

### Core Systems (src/)

#### 1. Core Utilities (src/core/)
- **Core.hpp** - Core types and utilities
- **Config.hpp/cpp** - Configuration management (already enhanced)
- **Logger.hpp/cpp** - Logging system
- **Random.hpp/cpp** - Random number generation
- **SimulationClock.hpp/cpp** - Simulation timing

#### 2. Neural Computation (src/neural/)
- **Neuron.hpp/cpp** - Individual neuron model
- **Synapse.hpp/cpp** - Synaptic connections
- **NeuralPopulation.hpp/cpp** - Neural population management
- **NeuralRegion.hpp/cpp** - Brain region organization
- **Brain.hpp/cpp** - Main brain system

#### 3. Neural Dynamics (src/dynamics/)
- **NeuralDynamics.hpp/cpp** - Neural dynamics simulation
- **SpikeSystem.hpp/cpp** - Spike generation and propagation

#### 4. Plasticity (src/plasticity/)
- **PlasticityRule.hpp/cpp** - Plasticity rule base
- **Hebbian.hpp/cpp** - Hebbian plasticity
- **STDP.hpp/cpp** - Spike-Timing Dependent Plasticity
- **StructuralPlasticity.hpp/cpp** - Structural plasticity

#### 5. Development (src/development/)
- **DevelopmentSystem.hpp/cpp** - Development system
- **Synaptogenesis.hpp/cpp** - Synaptogenesis process
- **Pruning.hpp/cpp** - Synaptic pruning
- **Maturation.hpp/cpp** - Neural maturation

#### 6. Neuromodulation (src/neuromodulation/)
- **Neuromodulator.hpp/cpp** - Neuromodulator base
- **Dopamine.hpp/cpp** - Dopamine system
- **Curiosity.hpp/cpp** - Curiosity system
- **Novelty.hpp/cpp** - Novelty detection
- **PredictionError.hpp/cpp** - Prediction error system

#### 7. Memory (src/memory/)
- **WorkingMemory.hpp/cpp** - Working memory system
- **EpisodicMemory.hpp/cpp** - Episodic memory system
- **SemanticMemory.hpp/cpp** - Semantic memory system
- **ProceduralMemory.hpp/cpp** - Procedural memory system
- **AssociativeMemory.hpp/cpp** - Associative memory system
- **NeuralWorkingMemory.hpp/cpp** - Neural working memory
- **NeuralEpisodicMemory.hpp/cpp** - Neural episodic memory

#### 8. Cognition (src/cognition/)
- **ConceptFormation.hpp/cpp** - Concept formation
- **NeuralPlanner.hpp/cpp** - Neural planning
- **AttentionalSelection.hpp/cpp** - Attentional selection

#### 9. Sensory (src/sensory/)
- **SensoryInput.hpp/cpp** - Sensory input processing
- **Vision.hpp/cpp** - Visual processing
- **Audio.hpp/cpp** - Auditory processing
- **InternalSignals.hpp/cpp** - Internal signal processing

#### 10. Prediction (src/prediction/)
- **PredictionSystem.hpp/cpp** - Prediction system
- **NeuralPrediction.hpp/cpp** - Neural prediction

#### 11. Motor (src/motor/)
- **MotorSystem.hpp/cpp** - Motor system

#### 12. Environment (src/environment/)
- **Environment.hpp/cpp** - Environment interface
- **Observation.hpp/cpp** - Observation system
- **Action.hpp/cpp** - Action system

#### 13. Experiments (src/experiments/)
- **Experiment.hpp/cpp** - Experiment base
- **ExperimentRunner.hpp/cpp** - Experiment runner
- **Metrics.hpp/cpp** - Experiment metrics
- **Phase3Experiment.hpp/cpp** - Phase 3 experiment
- **Phase4Experiment.hpp/cpp** - Phase 4 experiment

#### 14. Visualization (src/visualization/)
- **VisualizationInterface.hpp/cpp** - Visualization interface

#### 15. Agent (src/agent/)
- **SensoryPercept.hpp/cpp** - Sensory percept
- **AgentBrain.hpp/cpp** - Agent-brain interface

#### 16. World (src/world/)
- **SimpleWorld.hpp/cpp** - Simple world implementation

### Improved CMake Structure

## Enhanced CMake Configuration

The enhanced CMake configuration includes:

### 1. Modern CMake Practices
- CMake 3.20 minimum version
- Compiler feature detection
- Enhanced warning configuration
- Platform-specific optimizations

### 2. Modular Library Structure
```
add_library(nlm_core STATIC ...)
add_library(nlm_agent STATIC ...)
add_library(nlm_world STATIC ...)
add_library(nlm_phase3 STATIC ...)
add_library(nlm_phase4 STATIC ...)
```

### 3. Python Bindings
```
if(NLM_ENABLE_PYTHON_BINDINGS)
    pybind11_add_module(pynlm ...)
endif()
```

### 4. Comprehensive Testing
```
add_test(NAME test_config COMMAND test_config)
add_test(NAME test_brain COMMAND test_brain)
add_test(NAME test_clock COMMAND test_clock)
add_test(NAME test_memory COMMAND test_memory)
add_test(NAME test_neuromodulation COMMAND test_neuromodulation)
add_test(NAME test_cognition COMMAND test_cognition)
add_test(NAME test_development COMMAND test_development)
add_test(NAME test_integration COMMAND test_integration)
```

## Implementation Strategy

### Phase 1: File Relocation
1. Create new directory structure
2. Move files to appropriate locations
3. Update CMakeLists.txt references
4. Update include statements in source files

### Phase 2: Naming Convention Standardization
1. Ensure consistent naming across all files
2. Standardize header guard formats
3. Update documentation references

### Phase 3: Documentation Enhancement
1. Add comprehensive Doxygen comments to all public headers
2. Update existing documentation
3. Create usage examples

### Phase 4: Integration Testing
1. Update existing tests to new paths
2. Add integration tests for new structure
3. Verify build system compatibility

## Benefits of Improved Structure

### 1. Improved Maintainability
- Clear separation of concerns
- Easier to locate related files
- Reduced cognitive load for developers

### 2. Better Scalability
- Modular design supports future expansion
- Easier to add new subsystems
- Clear interfaces between components

### 3. Enhanced Documentation
- Comprehensive API documentation
- Clear usage examples
- Detailed parameter descriptions

### 4. Robust Testing
- Comprehensive test coverage
- Integration tests for system-wide behavior
- Memory management tests

### 5. Optimized Build System
- Modern CMake features
- Better compiler optimizations
- Platform-specific configurations

## Migration Considerations

### 1. Build System Updates
- Update CMakeLists.txt files
- Verify all build configurations
- Test on target platforms

### 2. Source Code Updates
- Update all #include directives
- Update compiler directives
- Update documentation references

### 3. Testing Updates
- Update test paths
- Verify test functionality
- Add new test cases

### 4. User Documentation
- Update user guides
- Update API documentation
- Provide migration examples

## Next Steps

1. **Immediate Actions**: Create new directory structure
2. **Short-term**: Move files to new locations
3. **Medium-term**: Update CMakeLists.txt and build system
4. **Long-term**: Enhance documentation and testing

## Example: Core System Migration

### Before:
```
src/core/
├── Config/
│   ├── Config.hpp
│   └── Config.cpp
├── Logger/
│   ├── Logger.hpp
│   └── Logger.cpp
└── Random/
    ├── Random.hpp
    └── Random.cpp
```

### After:
```
src/core/
├── Core.hpp
├── Config.hpp
├── Config.cpp
├── Logger.hpp
├── Logger.cpp
├── Random.hpp
└── Random.cpp
```

This flattened structure makes it easier to locate core system components while maintaining clear organization.

## Conclusion

The improved directory structure provides:
- **Better maintainability** through reduced nesting
- **Enhanced documentation** with comprehensive API coverage
- **Improved testing** with expanded test coverage
- **Modern build system** with enhanced CMake features
- **Future scalability** for additional system components

By implementing these improvements, the NLM project will become more maintainable, well-documented, and easier to extend for future development.
