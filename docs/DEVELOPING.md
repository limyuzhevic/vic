# NLM Development Guide

**NLM (熙然) — An Experimental Artificial Developmental Brain**

This document provides comprehensive guidance for developers wanting to contribute to the NLM neural simulation framework.

## 1. Repository Structure and Organization

### Current Structure
```
NLM/
├── CMakeLists.txt                    # Root build configuration
├── README.md                         # Project overview
├── pyproject.toml                   # Python bindings configuration
├── docs/                           # Documentation
│   ├── ARCHITECTURE.md             # System architecture
    │   ├── ARCHITECTURE_DEVELOPER.md  # Developer technical guide
│   ├── SCIENCE.md                  # Scientific background
│   ├── PHASE6_FINAL_AUDIT.md       # Current integration status
│   ├── PHASE6_FINAL_RESULTS.md     # Phase 6 results
│   ├── ROADMAP.md                  # Development roadmap
│   └── OTHER files...              # Research documents
├── src/                            # Core C++ source code
│   ├── core/                       # Core utilities
│   │   ├── Config/                # Configuration system
│   │   ├── Logger/                # Structured logging
│   │   ├── Random/                # Deterministic RNG
│   │   ├── SimulationClock/       # Time management
│   │   └── Types/                 # Type definitions
│   ├── brain/                      # Neural components
│   ├── dynamics/                   # Neural dynamics
│   ├── plasticity/                 # Plasticity rules
│   ├── development/                # Developmental system
│   ├── neuromodulation/            # Neuromodulators
│   ├── memory/                    # Memory systems
│   ├── prediction/                 # Prediction systems
│   ├── cognition/                  # Cognitive mechanisms
│   ├── sensory/                   # Sensory processing
│   ├── motor/                     # Motor system
│   ├── environment/               # Environment interface
│   ├── experiments/               # Experiment framework
│   └── visualization/              # Visualization
├── tests/                          # C++ test suite
│   ├── test_brain.cpp
│   ├── test_config.cpp
│   ├── test_main.cpp
│   └── OTHER test files
├── python/                         # Python bindings
├── configs/                        # Configuration files
└── easy_usage.md                   # Quick start guide
```

### Key Components

#### Core Layer
- **Config**: Hierarchical configuration management with JSON support
- **Logger**: Structured logging system for debugging and monitoring
- **Random**: Deterministic random number generator (thread-safe)
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **SimulationClock**: Simulation time management

#### Neural Core
- **Brain**: Central coordinator with LIF neuron dynamics
- **Neuron**: Leaky Integrate-and-Fire neuron model
- **Synapse**: Synaptic connections with delays and plasticity
- **Populations**: Groups of neurons with shared properties
- **Regions**: Brain regions containing multiple populations

#### Functionality Layers
- **Dynamics**: Event-driven spike processing
- **Plasticity**: STDP, Hebbian, and structural plasticity
- **Development**: Synaptogenesis, pruning, and maturation
- **Neuromodulation**: Dopamine, novelty, and curiosity systems
- **Memory**: Working, episodic, and associative memory
- **Prediction**: Temporal sequence learning and prediction error
- **Cognition**: Attention, planning, concept formation
- **Sensory**: Vision, touch, internal signals processing
- **Motor**: Action selection and motor command decoding
- **Environment**: SimpleWorld simulation interface

## 2. Development Environment Setup

### Prerequisites

#### C++ Development
```bash
# Recommended compiler versions
- GCC: 10+
- Clang: 12+
- MSVC: 2019+

# Build system
- CMake: 3.16+
- Ninja (optional, for faster builds)

# For Python bindings
- Python: 3.8+
- pybind11: 2.11.0+
```

#### Python Environment (for bindings)
```bash
# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy

# Optional: development dependencies
pip install pytest-cov memory_profiler
```

### Building the Project

#### Standard Build (C++ Core)
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (choose build type)
cmake .. -DCMAKE_BUILD_TYPE=Release
# Options:
# -DCMAKE_BUILD_TYPE=Debug         # With debug symbols and assertions
# -DCMAKE_CXX_COMPILER=g++         # Specific compiler
# -DCMAKE_CXX_STANDARD=20          # C++ standard (20 is default)

# Build all targets (parallel build)
make -j$(nproc)  # or make -j8 for 8 cores
```

#### Building with Python Bindings
```bash
# From project root (modern approach)
pip install .
# Or for development
pip install -e .

# Alternative: build with scikit-build
pip install build
python -m build
```

#### Running Tests
```bash
# After building with CMake
ctest --output-on-failure

# Or using Python tests (if any)
pytest
```

### IDE Configuration

#### Visual Studio Code
Create `/.vscode/tasks.json` and `/.vscode/launch.json`:

**tasks.json** (build and test tasks):
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build",
            "type": "shell",
            "command": "mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc)",
            "group": "build",
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "test",
            "type": "shell",
            "command": "cd build && ctest --output-on-failure",
            "group": "test",
            "dependsOn": "build",
            "problemMatcher": ["$ctest"]
        }
    ]
}
```

#### CLion
Import the CMake project directly or use `cmake -G "Unix Makefiles" .

### Code Editing Guidelines

#### Editor Configuration
- Use **Tab** for indentation (2 spaces per level)
- Line length: 120 characters maximum
- Use clang-format for formatting (if available)
- Follow existing code style patterns

#### Git Configuration
```bash
# Configure global git settings for this project
git config core.autocrlf input
git config core.fileMode false
git config commit.verbose true
git config pull.rebase false
```

## 3. Code Contribution Guidelines

### Contribution Workflow

#### 1. Understanding the Architecture
**CRITICAL**: Read the current project status thoroughly:

- **Phase 6 Final Audit**: `/docs/PHASE6_FINAL_AUDIT.md` - Shows most systems are disconnected
- **Architecture**: `/docs/ARCHITECTURE.md` - Shows component relationships
- **Developer Guide**: `/docs/ARCHITECTURE_DEVELOPER.md` - Technical extension and integration guidance
- **Current Integration**: `/docs/PHASE6_FINAL_AUDIT.md` Section 1 - Integration status

**Key Finding**: The NLM brain has sophisticated components but they're **NOT integrated into a coherent artificial brain**. Most cognitive systems exist but don't interact.

#### 2. Project Status Summary
**What Works (✅)**:
- LIF neuron dynamics and spike propagation
- Synaptic transmission with delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)
- Basic Agent-World interface
- Memory system structures (but disconnected)
- Neuromodulation basics (dopamine, curiosity, novelty)
- Prediction system structures (but unused)
- Cognitive system structures (but unused)

**What Doesn't Work (❌)**:
- Working memory integration
- Episodic memory storage
- Prediction system usage
- Cognitive system integration
- Full neuromodulation integration
- Checkpoint/save-load functionality

#### 3. Contribution Strategy

**For New Features/Integrations**:
1. **Prioritize integration over new components** - Most systems already exist but are disconnected
2. **Focus on Phase 6 goals** - "Make existing systems work together as a coherent whole"
3. **Connect disconnected systems** - This is the primary need

**Example Integration Tasks**:
```
1. Connect WorkingMemory to sensory processing
   - Brain::getWorkingMemory() currently returns nullptr
   - Need to store sensory input in working memory

2. Connect EpisodicMemory to experience logging
   - Brain::getEpisodicMemory() currently returns nullptr
   - Need to store experiences for later replay

3. Connect PredictionSystem to sensory processing
   - Brain::getPredictionSystem() currently returns nullptr
   - Need to train predictions from sensory input

4. Connect NeuralPlanner to action selection
   - NeuralPlanner exists but is never called
   - Need to use planner for complex action selection
```

#### 4. Coding Standards

##### File Structure
- Each component in separate header (.hpp) and implementation (.cpp) files
- Header files contain only declarations, implementation in .cpp files
- Use include guards (`#pragma once` preferred)
- All code in `nlm` namespace
- Use `NLM_` prefix for logging and constants

##### Memory Management
- Use smart pointers (`std::shared_ptr`, `std::unique_ptr`)
- Reference counting for cross-component ownership
- No raw pointers where possible

##### Error Handling
- Use assertions for programming errors
- Return `bool` for recoverable errors
- Log errors with `NLM_LOG_ERROR`

#### 5. Testing and Validation

##### Unit Testing
Existing test patterns in `/tests/`:
```cpp
// test_config.cpp example
#include "Config.hpp"
#include <cassert>
#include <iostream>

void testConfig() {
    nlm::Config config;
    config.set("test.key", "value");
    assert(config.get<std::string>("test.key") == "value");
    std::cout << "✓ Config test passed" << std::endl;
}

int main() {
    testConfig();
    return 0;
}
```

##### Integration Testing
For Phase 6 integration, test complete brain loops:
```cpp
// Example test for brain integration
void testBrainIntegration() {
    auto config = nlm::createDefaultConfig();
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    // Test basic functionality
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getTotalSynapseCount() > 0);
    
    // Test step execution
    brain->step(0, 0.0);
    
    // Verify memory systems are connected
    // This is what we're trying to fix!
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
}
```

#### 6. Code Review Process

1. **Self-Review**: Check for TODO markers, disconnected systems, missing implementations
2. **Architecture Review**: Ensure changes follow NLM principles
3. **Integration Check**: Verify systems connect properly
4. **Performance Impact**: Check for performance regressions
5. **Documentation**: Update relevant documentation

### Specific Development Areas

#### Memory Integration (High Priority)
**Current Status**: Memory systems defined but disconnected

**Files to Focus On**:
- `src/memory/` - Memory system implementations
- `src/brain/Brain.hpp` - Lines 602-616: Return nullptr for memory systems
- `src/brain/Brain.cpp` - Lines 634-636: Return nullptr for prediction
- `src/agent/AgentBrain.cpp` - Need to store experiences in memory

**Tasks**:
1. Implement `Brain::getWorkingMemory()` to return actual WorkingMemory instance
2. Connect sensory input to working memory storage
3. Implement `Brain::getEpisodicMemory()` to return actual EpisodicMemory
4. Add experience logging in AgentBrain::processSensoryInput()
5. Implement replay mechanism for consolidation

#### Neuromodulation Integration (Medium Priority)
**Current Status**: Dopamine modulates STDP, other neuromodulators are stubs

**Tasks**:
1. Implement full dopamine effects on plasticity (not just weight scaling)
2. Connect neuromodulation to memory consolidation
3. Implement Acetylcholine effects on attention
4. Implement Norepinephrine effects on arousal
5. Connect neuromodulation to development

#### Prediction Integration (Medium Priority)
**Current Status**: Prediction system implemented but never used

**Tasks**:
1. Connect PredictionSystem to sensory processing
2. Use predictions for attention selection
3. Use predictions for action planning
4. Implement prediction error for learning
5. Add confidence measures to action selection

#### Development Integration (Low Priority)
**Current Status**: Basic development stages implemented

**Tasks**:
1. Expand development effects beyond plasticity rates
2. Connect development to neural excitability changes
3. Implement developmental effects on memory
4. Connect development to neuromodulation
5. Implement developmental changes to attention

## 4. Building and Testing Workflow

### Complete Build Process

#### Step 1: Environment Setup
```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create and activate virtual environment (recommended)
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install Python dependencies
pip install pybind11 scikit-build-core pytest numpy
```

#### Step 2: Build C++ Core
```bash
# Create build directory
mkdir build
cd build

# Configure CMake
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DPYTHON_EXECUTABLE=$(which python)

# Build
make -j$(nproc)

# Verify build
ls -la bin/
```

#### Step 3: Build Python Bindings
```bash
# From project root (modern approach)
pip install -e .

# Or if already built with CMake
pip install dist/*.whl
```

#### Step 4: Run Tests
```bash
# C++ tests (requires CMake build)
cd build
ctest --output-on-failure

# Python tests (if any)
pytest
```

#### Step 5: Build Documentation
```bash
# If using mkdocs or similar
mkdocs build
```

### Continuous Integration

#### GitHub Actions Setup
Create `.github/workflows/` with:

**build.yml**:
```yaml
name: Build and Test

on:
  push:
    branches: [ main ]
  pull_request:
    branches: [ main ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up CMake
      uses: threeal/setup-cmake@v1
    
    - name: Install Python dependencies
      run: pip install pybind11 scikit-build-core pytest numpy
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: cmake --build build --parallel $(nproc)
    
    - name: Run C++ tests
      run: cd build && ctest --output-on-failure
    
    - name: Install Python bindings
      run: pip install -e .
    
    - name: Run Python tests
      run: pytest
```

## 5. Best Practices for the NLM Framework

### Neural Architecture Principles

#### 1. Biological Inspiration
- Implement biological mechanisms (LIF neurons, STDP, etc.)
- Keep models incrementally more complex
- Don't make claims about reproducing biological brains
- Focus on computational principles

#### 2. Modularity
- Clear interfaces between components
- Minimal dependencies between systems
- Easy to swap implementations
- Well-defined extension points

#### 3. Determinism
- All randomness seeded
- Reproducible experiments
- Thread-safe random number generation
- Deterministic plasticity for same inputs

#### 4. Performance
- Data-oriented design
- Memory pools for allocation
- Event-driven spike processing
- Sparse connectivity where possible

### Development Best Practices

#### 1. Integration-First Mindset
Remember: **The brain has the anatomy of a cognitive system but lacks the integration**

**Don't**:
- Add new cognitive systems without integrating them
- Create more disconnected components
- Assume components work together without testing

**Do**:
- Focus on connecting existing systems
- Test complete brain loops
- Measure integration effectiveness
- Verify cross-system communication

#### 2. Scientific Rigor
- Document assumptions
- Validate against research literature
- Use precise terminology
- Track limitations

#### 3. Testing Strategy
```cpp
// Test individual components first
void testNeuronDynamics() {
    Neuron neuron;
    // Test LIF dynamics
    // Test spike generation
    // Test refractory period
}

// Then test integration
void testBrainIntegration() {
    auto config = createDefaultConfig();
    auto brain = createBrain(config);
    brain->initialize();
    
    // Test complete loop
    SensoryPercept percept;
    brain->receiveSensoryInput(percept);
    brain->step(0);
    Action action = brain->produceAction();
    
    // Verify memory systems worked
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getPredictionSystem() != nullptr);
}
```

#### 4. Documentation
- Document component interfaces clearly
- Document integration points
- Document assumptions and limitations
- Keep documentation in sync with code

#### 5. Performance Monitoring
- Profile hot paths
- Monitor memory usage
- Track simulation speed
- Log key metrics

### Research and Experimentation

#### 1. Experiment Design
Follow the experiment framework in `src/experiments/`:

**Phase 3 Experiment**: World interaction
**Phase 4 Experiment**: Cognitive emergence
**Phase 5 Experiment**: Performance optimizations
**Phase 6 Experiment**: Integration validation

#### 2. Metrics Collection
Key metrics to track:
- Learning curves over time
- Memory retention over episodes
- Prediction accuracy improvement
- Continual learning (no catastrophic forgetting)
- Developmental progression

#### 3. Ablation Studies
Use the `AblationSystem` to test:
- Component removal effects
- Integration point importance
- System dependencies
- Critical function verification

### Code Quality Practices

#### 1. Memory Management
```cpp
// Good: Use smart pointers
auto brain = std::make_shared<Brain>(config);

// Bad: Raw pointers without ownership
Neuron* neuron = new Neuron();  // Who owns it?
```

#### 2. Error Handling
```cpp
// Good: Return bool for errors
bool brain->initialize() {
    if (!config) return false;
    if (!validateConfig()) return false;
    // ... initialization
    return true;
}

// Bad: Silent failures
void brain->initialize() {
    // No error checking
}
```

#### 3. Logging
```cpp
// Good: Structured logging
NLM_LOG_INFO("Brain initialized with " + 
             std::to_string(neuronCount) + " neurons");

// Bad: printf-style debugging
printf("Neuron %d spiked\n", id);
```

## 6. Quick Start for Contributors

### If You Want to Fix Integration Problems:

1. **Read**: `/docs/PHASE6_FINAL_AUDIT.md` Sections 1-3
2. **Pick a system**: Memory, Prediction, Neuromodulation, or Development
3. **Find disconnected code**: Search for "nullptr" returns in Brain class
4. **Connect**: Implement integration points
5. **Test**: Verify complete brain loop works
6. **Document**: Update integration documentation

### If You Want to Add New Features:

1. **Check existing implementations**: Most cognitive systems already exist
2. **Focus on integration**: Connect new features to existing systems
3. **Follow patterns**: Use existing coding styles and interfaces
4. **Add tests**: Include integration tests for new features
5. **Document scientifically**: Explain biological inspiration and significance

### If You Want to Run Experiments:

1. **Use existing demos**: `nlm_phase6_demo` runs integration tests
2. **Configure experiments**: Use Python API or JSON configs
3. **Monitor metrics**: Track learning, memory, prediction accuracy
4. **Analyze results**: Use experiment framework in `src/experiments/`

## 7. Common Pitfalls and Solutions

### Problem 1: Assuming Components Work Together
**Symptom**: Code compiles but tests fail because systems aren't integrated
**Solution**: Always test complete brain loops, not just individual components

### Problem 2: Memory Leaks
**Symptom**: Valgrind reports heap usage growing
**Solution**: Use smart pointers, follow RAII, check for double deletes

### Problem 3: Non-Deterministic Results
**Symptom**: Same inputs give different outputs
**Solution**: Check random seed usage, verify all randomness is controlled

### Problem 4: Integration Testing
**Symptom**: Individual components work but full system doesn't
**Solution**: Test from sensory input to action output, including memory updates

### Problem 5: Performance Issues
**Symptom**: Simulation too slow for practical use
**Solution**: Profile with `perf`, check for bottlenecks, use SIMD where possible

## 8. Next Steps for Phase 6

The primary goal of Phase 6 is integration:

1. **Connect all major systems**: Memory, Prediction, Cognition, Neuromodulation
2. **Implement persistence**: Checkpoint/save-load functionality
3. **Add development effects**: Expand beyond plasticity rate changes
4. **Validate integration**: Scientific experiments measuring integration effectiveness
5. **Optimize**: Profile and improve performance

**Start with**: Connect WorkingMemory to sensory processing
**Goal**: A coherent artificial brain where all components work together

---

*This guide is a living document. Contributions and improvements are welcome. Please update this file as the project evolves.*

**Key References**:
- `/docs/PHASE6_FINAL_AUDIT.md` - Current integration status
- `/docs/ARCHITECTURE.md` - System architecture
- `/docs/ARCHITECTURE_DEVELOPER.md` - Developer technical guide
- `/src/agent/AgentBrain.cpp` - Integration examples
- `/src/experiments/` - Experiment framework