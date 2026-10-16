# NLM (熙然) - Neural Learning Machine - Comprehensive Project Overview

## Project Summary

**NLM (熙然, meaning "serene flow")** is an experimental computational brain project investigating artificial developmental systems. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

### Key Differentiators:
- **NOT** a transformer or LLM
- **NOT** a chatbot or deep learning model
- **NOT** a pretrained AI system

### **IS** intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Status

**Phase 6: FINAL INTEGRATION** - All existing systems are integrated into a coherent artificial brain.

## Architecture Overview

NLM operates as a closed-loop system:

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

## Project Structure

### Core Components (C++):

#### **1. Core Utilities**
- `core/Config/` - Configuration management
- `core/Random/` - Random number generation
- `core/Logger/` - Logging system
- `core/SimulationClock/` - Time management
- `core/Types/` - Data types and identifiers

#### **2. Neural Components**
- `brain/` - Main brain integration and control
  - `Brain.hpp` - Central brain class (1117-line implementation)
  - `Neuron.hpp` - LIF neuron model
  - `Synapse.hpp` - Synaptic connections
  - `NeuralRegion.hpp` - Neural organization

#### **3. Specialized Systems (Phase 6 Integrated)**
- `memory/` - Memory systems (working, episodic, associative)
- `neuromodulation/` - Neuromodulators (dopamine, curiosity, novelty)
- `prediction/` - Prediction systems
- `cognition/` - Cognitive mechanisms (planning, concept formation)
- `development/` - Developmental system
- `plasticity/` - Plasticity rules (STDP, Hebbian, structural)

#### **4. Perception and Action**
- `sensory/` - Sensory processing (vision, audio, internal signals)
- `motor/` - Motor system
- `environment/` - Environment interface

#### **5. Advanced Features**
- `experiments/` - Experiment framework (Phase 3-6)
- `visualization/` - Visualization interface
- `agent/` - Agent brain interface for world interaction

### Python Interface:

**`python/bindings.cpp`** - Comprehensive Python bindings via pybind11

### Build System:

**`pyproject.toml`** - Python package definition
**`CMakeLists.txt`** - C++ build configuration
**`tests/CMakeLists.txt`** - Test configuration

## Technical Specifications

### Neural Model:
- **Neuron Type**: Leaky Integrate-and-Fire (LIF)
- **Dynamics**: Real biophysical simulation
- **Plasticity**: STDP, Hebbian, structural synaptogenesis/pruning
- **Event-driven**: Spike propagation with delays

### Memory Systems:
- **Working Memory**: Persistent activity with competitive selection
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Hebbian pattern associations

### Neuromodulation:
- **Dopamine**: Affects neural excitability and plasticity
- **Curiosity**: Drives exploration behavior
- **Novelty**: Detects unexpected sensory input

### Development:
- **Developmental Stages**: Initial → CriticalPeriod → Maturation → Adult → Aging
- **Plasticity Modulation**: Age-dependent learning rates
- **Structural Changes**: Synaptogenesis and pruning

## Building and Installation

### Prerequisites:
- C++ compiler with C++20 support
- CMake 3.16+
- Python 3.8+

### Build Instructions:

#### Standard Build:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Python Bindings Build:
```bash
# From project root (where pyproject.toml is)
pip install -e .
```

#### Build with Testing:
```bash
mkdir build
d cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
make -j4
```

### Available Executables:

1. **`nlm`** - Phase 2 demonstration (basic neural computation)
2. **`nlm_phase3_demo`** - Phase 3 experiment demo
3. **`nlm_phase4_demo`** - Phase 4 experiment demo
4. **`./nlm_phase6_demo`** - Phase 6 integration test (comprehensive)
5. **`nlm_benchmark`** - Performance benchmark tool

## Running Examples

### Basic Phase 2 Demo:
```bash
./nlm
```
Shows basic neural connectivity, plasticity learning, and STDP verification.

### Phase 6 Integration Demo:
```bash
./nlm_phase6_demo
```
Comprehensive test of all integrated brain systems.

### Python Interface:
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Create simple world
world = pynlm.createSimpleWorld()

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)
```

## Configuration System

### Configuration File Format (JSON):
```json
{
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.1,
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "replay_interval": 100,
    "consolidation_interval": 1000
}
```

### Command Line Arguments:
```bash
./nlm --config=experiments/config_phase6.json --neuron_count=500 --region_count=2
```

### Configuration Categories:
- **Neural**: neuron_count, region_count, connection_probability
- **Plasticity**: stdp_*, synaptogenesis_rate, pruning_rate
- **Memory**: replay_interval, consolidation_interval
- **Development**: developmental_stage, plasticity_modifier
- **Simulation**: simulation_timestep, random_seed

## Testing Framework

### Test Structure:
- **`tests/test_*.cpp`** - Individual test files
- **`tests/test_main.cpp`** - Test runner
- **`tests/CMakeLists.txt`** - Test build configuration

### Available Tests:
1. **test_types** - Data type and identifier tests
2. **test_config** - Configuration system tests
3. **test_random** - Random number generator tests
4. **test_clock** - Simulation clock tests
5. **test_neuron** - LIF neuron dynamics tests
6. **test_synapse** - Synaptic transmission tests
7. **test_stdp** - STDP plasticity rule tests
8. **test_brain** - Brain integration tests

### Running Tests:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make nlm_test
./nlm_test
```

## Performance Characteristics

### Benchmark Results (Phase 2):
- **100 neurons**: ~10,000 steps/sec
- **500 neurons**: ~8,000 steps/sec  
- **1000 neurons**: ~6,000 steps/sec
- **2000 neurons**: ~4,000 steps/sec

### Memory Usage:
- **Per neuron**: ~200 bytes (state, connections)
- **Total with 1000 neurons**: ~200 KB
- **With plasticity systems**: ~500 KB

### Scalability:
- Linear scaling with neuron count
- Efficient event-driven spike propagation
- Memory pools for performance optimization

## Advanced Features for Advanced Users

### 1. Checkpoint Persistence:
```bash
# Save brain state
brain.save("my_brain_state.bin")

# Load brain state
brain.load("my_brain_state.bin")
```

### 2. Python Scripting:
```python
import pynlm
import numpy as np

# Create brain with custom config
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation loop
for step in range(1000):
    # Inject sensory input
    if step % 100 == 0:
        # Random sensory input
        sensory_data = np.random.randn(10).tolist()
        # Convert to NLM sensory format (implementation dependent)
    
    # Step brain
    brain.step(step)
    
    # Produce action if needed
    action = brain.produceAction()
```

### 3. Custom Experiment Configuration:
```cpp
Phase6Config config;
config.neuronCount = 1000;
config.maxSteps = 5000;
config.enableCheckpointing = true;
config.enableReplay = true;
config.enableDevelopment = true;
config.checkpointPath = "experiment_results.bin";
```

## Development Guidelines

### Code Quality Standards:
- **C++20**: Modern C++ with strict standards
- **Error Handling**: Comprehensive exception safety
- **Memory Management**: RAII with smart pointers
- **Testing**: Unit tests for all major components
- **Documentation**: Comprehensive Doxygen-style comments

### Contribution Guidelines:
1. **Maintain Existing Architecture**: All changes must preserve the existing API
2. **Integration Testing**: New features must work with existing systems
3. **Performance Considerations**: Optimize for simulation efficiency
4. **Documentation**: Update API documentation for new features
5. **Testing**: Add unit tests for all new code

### Future Development Directions:
1. **Enhanced Memory Systems**: More sophisticated episodic and working memory
2. **Advanced Neuromodulation**: More complex reward prediction models
3. **Multi-region Integration**: Better inter-regional communication
4. **Real-time Applications**: Optimized for real-time interaction
5. **Scalability Improvements**: Support for larger networks

## Common Issues and Solutions

### Issue: Build Failures
**Solution**: Ensure C++20 support and sufficient system resources.

### Issue: Python Bindings Not Available
**Solution**: Build with pip install -e . or check CMake configuration.

### Issue: Performance Degradation
**Solution**: Use smaller networks for development, optimize for production.

### Issue: Memory Usage High
**Solution**: Use checkpointing and periodic consolidation.

### Issue: Complex Configuration
**Solution**: Use preset configurations (e.g., configs/default.cfg, configs/experiment.cfg).

## Further Reading

### Documentation:
- **`docs/ARCHITECTURE.md`** - Detailed system architecture
- **`docs/SCIENCE.md`** - Scientific background and theories
- **`docs/ROADMAP.md`** - Development roadmap
- **`docs/EXPERIMENTS.md`** - Experiment framework documentation
- **`docs/PHASE6_FINAL_AUDIT.md`** - Final integration audit report

### Related Projects:
- Neural network simulation research
- Computational neuroscience
- Artificial developmental systems
- Event-driven neural simulation

## Acknowledgments

This project represents years of research in computational brain modeling and neural simulation. Special thanks to the research team and contributors who have developed and refined each component over multiple phases.

**License**: MIT

---
*Document generated: October 16, 2026*
*Phase 6: FINAL INTEGRATION*
*NLM - Neural Learning Machine*
