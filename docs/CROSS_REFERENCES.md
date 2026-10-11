# NLM Cross-References and Navigation Guide

## Overview

This guide documents the cross-references and navigation patterns across the NLM codebase. It provides comprehensive documentation of file relationships, dependencies, and integration points to help developers understand how different components interact and navigate the codebase efficiently.

## File Relationship Matrix

### Core Files Dependencies

| Source File | Includes | Referenced By | Purpose |
|-------------|----------|---------------|---------|
| **Brain.hpp** | Config.hpp, Types.hpp | Brain.cpp, main.cpp | Central brain coordinator |
| **Brain.cpp** | Brain.hpp, Config.hpp, various system headers | main.cpp | Brain implementation |
| **Config.hpp** | string, memory, variant, optional | Brain.hpp, Config.cpp, main.cpp | Configuration management |
| **Config.cpp** | Config.hpp, fstream, sstream | main.cpp | Configuration file I/O |
| **main.cpp** | All major headers | - | Main application entry point |
| **bindings.cpp** | All pybind11 headers | Python bindings | Python bindings generator |

### Component Dependencies

#### Phase 2: Neural Computation Core
```
┌─────────────────────────────────────────────────────────────┐
│                    Phase 2: Neural Computation Core      │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐           │
│  │   Neuron    │  │   Synapse   │  │ Population │           │
│  └─────────────┘  └─────────────┘  └─────────────┘           │
│          │                  │                   │        │
│          └──────────────────┼───────────────────┘        │
│                           ▼                           │
│                 ┌─────────────────┐                   │
│                 │   Spike System  │                   │
│                 └─────────────────┘                   │
│                           │                           │
│                           ▼                           │
│                 ┌─────────────────┐                   │
│                 │   Brain         │                   │
│                 │  (Coordinator)  │                   │
│                 └─────────────────┘                   │
└──────────────────┬───────────────────────────────────────┘
                   │
                   ▼
   ┌───────────────────┬─────────────────────────────┬─────────────────┐
   │                   │                             │                 │
   ▼                   ▼                             ▼                 ▼
┌─────────────────┐ ┌─────────────────┐       ┌─────────────────┐ ┌─────────────────┐
│   Memory        │ │   Prediction    │       │   Neuromod.    │ │   Development  │
│   Systems        │ │   System        │       │   Signals       │ │   System        │
└─────────────────┘ └─────────────────┘       └─────────────────┘ └─────────────────┘
```

### Cross-File Documentation References

#### 1. Brain.hpp ↔ Brain.cpp
**Files:** `src/brain/Brain.hpp` ↔ `src/brain/Brain.cpp`

**Key Relationships:**
- **Brain.hpp**: Class declaration with public interface
- **Brain.cpp**: Complete implementation with detailed comments

**Documentation Cross-References:**
- Brain.hpp: Class-level documentation (lines 46-65)
- Brain.cpp: Method implementations with algorithm explanations
- Integration notes: Configuration loading, sensory input processing

**Navigation Tips:**
1. Start with `Brain.hpp` to understand the public interface
2. Use `Brain.cpp` for implementation details and algorithm explanations
3. Reference `main.cpp` for usage examples

#### 2. Config.hpp ↔ Config.cpp
**Files:** `src/core/Config/Config.hpp` ↔ `src/core/Config/Config.cpp`

**Key Relationships:**
- **Config.hpp**: Template class with method declarations
- **Config.cpp**: Template method implementations

**Documentation Cross-References:**
- Config.hpp: Class documentation (Phase 2 configuration system)
- Config.cpp: File format documentation (simple key=value)
- Config.cpp: Command-line argument parsing

**Navigation Tips:**
1. Use `Config.hpp` for API documentation
2. Use `Config.cpp` for implementation details
3. Check `main.cpp` for usage patterns

#### 3. main.cpp ↔ All Major Headers
**Files:** `src/main.cpp` includes all major system headers

**Key Relationships:**
- **main.cpp**: Application entry point with comprehensive examples
- **All Headers**: Individual component implementations

**Documentation Cross-References:**
- main.cpp: Complete usage examples
- LearningExperiment: Structured experiment framework
- runBasicConnectivityTest: Basic connectivity testing
- runPlasticityExperiment: Plasticity learning experiments

**Navigation Tips:**
1. Use `main.cpp` as a comprehensive example guide
2. Reference individual headers for specific component details
3. Study `LearningExperiment` for testing patterns

#### 4. bindings.cpp ↔ Python Integration
**Files:** `python/bindings.cpp` ↔ Python environment

**Key Relationships:**
- **bindings.cpp**: Python bindings generator
- **Python**: External interface to NLM

**Documentation Cross-References:**
- bindings.cpp: Complete Python API documentation
- PYBIND11_MODULE: Module-level documentation
- Examples: Comprehensive Python usage patterns

**Navigation Tips:**
1. Use `bindings.cpp` for Python API reference
2. Study `createExperiment` for experiment setup
3. Reference `run_simple_learning_simulation` for basic usage

## API Navigation Guide

### 1. Brain Class Navigation

#### Header File (Brain.hpp)
```cpp
// Key class sections in Brain.hpp:
// 1. Class definition (lines 51-199)
// 2. Memory systems accessors (lines 140-149)
// 3. Prediction system accessor (lines 151-154)
// 4. Cognition systems accessors (lines 156-165)
// 5. Development system (lines 167-171)
// 6. Neuromodulation systems (lines 173-185)
```

#### Implementation File (Brain.cpp)
```cpp
// Key method sections in Brain.cpp:
// 1. Constructor (lines 199-202)
// 2. Initialize method (lines 219-362) - Most comprehensive
// 3. Step methods (lines 364-586) - Core simulation loop
// 4. Memory system accessors (lines 1006-1074)
// 5. Save/load methods (lines 826-910)
```

### 2. Config Class Navigation

#### Header File (Config.hpp)
```cpp
// Key class sections in Config.hpp:
// 1. Class definition (lines 46-103)
// 2. Load methods (lines 76-83)
// 3. Get methods (lines 85-90)
// 4. Set methods (lines 92-103)
// 5. Utility methods (lines 104-103)
```

#### Implementation File (Config.cpp)
```cpp
// Key method sections in Config.cpp:
// 1. loadFromFile (lines 21-68) - Detailed format documentation
// 2. loadFromArgs (lines 70-102) - Command-line parsing
// 3. saveToFile (lines 105-142) - File output format
// 4. Template methods (lines 144-294) - Type conversion
```

### 3. Integration Point Navigation

#### Sensory Integration
**Files:** `Brain.cpp:591` ↔ `SensoryInput.hpp`
**Documentation:** Receive sensory input processing
**Example:** `main.cpp:162` - Basic connectivity test

#### Motor Integration
**Files:** `Brain.cpp:692` ↔ `Action.hpp`
**Documentation:** Motor action production
**Example:** `main.cpp:205` - Plasticity experiment

#### Memory Integration
**Files:** `Brain.cpp:1008` ↔ `Memory.hpp`
**Documentation:** Memory system access
**Example:** `main.cpp:225` - Working memory usage

## File-Specific Documentation

### A. Brain System Documentation
**Primary Documentation Files:**
- `docs/ARCHITECTURE.md` - Complete system architecture
- `src/brain/Brain.hpp` - Class interface and documentation
- `src/brain/Brain.cpp` - Implementation and algorithm details

**Key Documentation Sections:**
1. **Phase 6 Integration**: All integrated systems coordination
2. **Simulation Loop**: Complete step-by-step algorithm
3. **Memory Systems**: Working memory, episodic memory integration
4. **Neuromodulation**: Dopamine, curiosity, novelty systems
5. **Development**: Stage progression and plasticity effects

### B. Configuration System Documentation
**Primary Documentation Files:**
- `src/core/Config/Config.hpp` - Configuration API
- `src/core/Config/Config.cpp` - Configuration implementation
- `main.cpp:341` - Configuration loading examples

**Key Documentation Sections:**
1. **File Format**: Simple key=value format with quotes and arrays
2. **Command-line Parsing**: --key=value and -key value formats
3. **Source Hierarchy**: CommandLine > File > Runtime > Default
4. **Type Conversion**: Automatic type handling for configuration values

### C. Python Bindings Documentation
**Primary Documentation Files:**
- `python/bindings.cpp` - Complete Python API
- `pyproject.toml` - Python project configuration
- `HOW_TO_USE.md` - Usage instructions

**Key Documentation Sections:**
1. **Module Documentation**: `PYBIND11_MODULE(pynlm, m)`
2. **Class Bindings**: Brain, Config, AgentBrain, etc.
3. **Function Examples**: `create_experiment`, `run_simple_learning_simulation`
4. **Error Handling**: Custom exception definitions

### D. Integration Guide Documentation
**Primary Documentation Files:**
- `docs/integration_guide.md` - Complete integration guide
- `docs/ARCHITECTURE.md` - System architecture

**Key Documentation Sections:**
1. **Core Integration**: Basic neural simulation setup
2. **Agent Framework**: Complete agent integration patterns
3. **Memory Integration**: Working memory, episodic memory
4. **Prediction Integration**: Temporal sequence learning
5. **Development Integration**: Stage progression
6. **Performance Integration**: Monitoring and optimization
7. **Error Handling**: Comprehensive error recovery
8. **Testing Integration**: Test framework and validation
9. **Production Integration**: Deployment and monitoring

## Navigation Patterns

### 1. Algorithm Understanding
**For understanding algorithms:**
1. Study `Brain.cpp:368` - Complete simulation loop with phase comments
2. Reference `SpikeSystem.hpp` for event-driven processing
3. Check `plasticity/STDP.hpp` for STDP implementation

### 2. Configuration Management
**For configuration tasks:**
1. Read `Config.hpp` for API reference
2. Study `Config.cpp:21` for file format details
3. Use `main.cpp:341` for loading examples

### 3. Testing and Validation
**For testing tasks:**
1. Reference `LearningExperiment` in `main.cpp:52`
2. Study `runBasicConnectivityTest` in `main.cpp:162`
3. Check `runPlasticityExperiment` in `main.cpp:205`
4. Review `runStdpVerification` in `main.cpp:248`

### 4. Python Integration
**For Python development:**
1. Use `bindings.cpp` for complete API reference
2. Study `createExperiment` for experiment setup
3. Reference `run_agent_training_simulation` for training
4. Check `create_config_from_json` for configuration

## Cross-Reference Summary

### Key File Pairs
1. **Brain.hpp ↔ Brain.cpp**: Interface ↔ Implementation
2. **Config.hpp ↔ Config.cpp**: API ↔ Implementation
3. **bindings.cpp ↔ Python**: C++ ↔ Python bindings
4. **main.cpp ↔ All Headers**: Usage examples ↔ Component implementations

### Documentation Dependencies
1. **Architecture Dependencies**: `ARCHITECTURE.md` depends on component documentation
2. **Integration Dependencies**: `integration_guide.md` depends on architecture documentation
3. **API Dependencies**: Component documentation depends on main examples

### Usage Patterns
1. **New Component Integration**: Follow patterns in `integration_guide.md`
2. **Configuration Setup**: Use patterns in `main.cpp:341`
3. **Testing**: Reference `LearningExperiment` in `main.cpp:52`
4. **Production**: Study `ProductionDeploymentAgent` in `integration_guide.md`

## Navigation Best Practices

### 1. Starting Points
- **For Architecture**: `docs/ARCHITECTURE.md`
- **For Usage**: `src/main.cpp`
- **For Configuration**: `src/core/Config/Config.hpp`
- **For Python**: `python/bindings.cpp`

### 2. Algorithm Understanding
- **Core Simulation**: `Brain.cpp:368` - Complete step method
- **Neural Dynamics**: `brain/Neuron.hpp` - LIF implementation
- **Synaptic Plasticity**: `plasticity/STDP.hpp` - STDP rules

### 3. Testing and Validation
- **Experiment Framework**: `main.cpp:52` - LearningExperiment
- **Basic Tests**: `main.cpp:162` - runBasicConnectivityTest
- **Learning Tests**: `main.cpp:205` - runPlasticityExperiment
- **Plasticity Tests**: `main.cpp:248` - runStdpVerification

### 4. Production Deployment
- **Performance Monitoring**: `integration_guide.md:Chapter 6`
- **Error Handling**: `integration_guide.md:Chapter 7`
- **Production Deployment**: `integration_guide.md:Chapter 9`

## Cross-Reference Usage Examples

### Example 1: Understanding the Simulation Loop
```cpp
// To understand the complete simulation loop:
1. Read Brain.cpp:368 (step method with phase comments)
2. Reference SpikeSystem.hpp for spike processing
3. Check plasticity implementations in plasticity/ directory
4. Study memory systems in memory/ directory
```

### Example 2: Adding a New Configuration Parameter
```cpp
// To add a new configuration parameter:
1. Add parameter documentation to Config.hpp
2. Implement validation in Config.cpp
3. Update examples in main.cpp
4. Document in integration_guide.md
```

### Example 3: Integrating a New Sensory Modality
```cpp
// To integrate a new sensory modality:
1. Study existing patterns in main.cpp:162
2. Reference SensoryInput.hpp for base class
3. Implement Vision.hpp/Audio.hpp patterns
4. Update integration_guide.md with new example
```

## Conclusion

This cross-references guide provides comprehensive navigation instructions for the NLM codebase. It documents:

1. **File Relationships**: Dependencies and interactions between files
2. **API Navigation**: How to navigate between interface and implementation
3. **Documentation Dependencies**: How documentation files relate to each other
4. **Usage Patterns**: Best practices for navigating and using the codebase
5. **Integration Examples**: Specific examples for common integration tasks

The guide enables developers to efficiently understand the NLM system architecture, navigate the codebase, and integrate new components following established patterns and best practices.