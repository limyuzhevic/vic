# NLM Source Code Documentation

## Overview

This directory contains the source code for **NLM (熙然)** - an experimental artificial developmental brain project. The source code implements a computational brain system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## Directory Structure

```
src/
├── agent/              # Agent system (brain-world interface)
├── brain/              # Core neural components (brain architecture)
├── cognition/          # Cognitive mechanisms and reasoning
├── core/              # Core utilities and types
├── development/        # Developmental system
├── dynamics/          # Neural dynamics and spike propagation
├── environment/       # Environment interface and perception
├── experiments/        # Experiment framework
├── memory/            # Memory systems
├── motor/             # Motor system and action selection
├── neuromodulation/    # Neuromodulators and motivation
├── plasticity/        # Plasticity rules and learning
├── prediction/        # Prediction systems
├── sensory/           # Sensory processing (vision, audio)
├── visualization/      # Visualization interfaces
├── world/             # World simulation
├── main.cpp           # Main executable entry point
└── *.h/*.cpp         # Individual component implementations
```

## Key Components

### Core System Components

#### 1. **brain/** - Neural Architecture
   - **Brain.hpp** - Main brain class managing neural populations and regions
   - **Neuron.hpp** - LIF (Leaky Integrate-and-Fire) neuron implementation
   - **Synapse.hpp** - Synaptic connections between neurons
   - **NeuralPopulation.hpp** - Organized groups of neurons
   - **NeuralRegion.hpp** - Anatomical brain regions

#### 2. **cognition/** - Cognitive Mechanisms
   - **NeuralPlanner.hpp** - Action sequence planning and evaluation
   - **ConceptFormation.hpp** - Abstract concept learning from patterns

#### 3. **memory/** - Memory Systems
   - **Memory.hpp** - Abstract memory interface
   - **NeuralWorkingMemory.hpp** - Working memory with persistent activity
   - **NeuralEpisodicMemory.hpp** - Episodic memory with experience replay
   - **SemanticMemory.hpp** - Conceptual knowledge storage
   - **ProceduralMemory.hpp** - Learned behavioral patterns
   - **AssociativeMemory.hpp** - Pattern association

#### 4. **neuromodulation/** - Motivation and Learning
   - **Neuromodulator.hpp** - General neuromodulator system
   - **Reward.hpp** - Reward prediction and processing
   - **PredictionError.hpp** - Prediction error signals
   - **Novelty.hpp** - Novelty detection and exploration
   - **Curiosity.hpp** - Curiosity-driven exploration

#### 5. **prediction/** - Predictive Processing
   - **PredictionSystem.hpp** - Core prediction mechanism
   - **NeuralPrediction.hpp** - Neural implementation of prediction

#### 6. **sensory/** - Sensory Processing
   - **SensoryInput.hpp** - Interface for sensory information
   - **Vision.hpp** - Visual processing
   - **Audio.hpp** - Audio processing
   - **InternalSignals.hpp** - Internal state signals

#### 7. **motor/** - Action Selection
   - **MotorSystem.hpp** - Motor control and action selection

#### 8. **environment/** - World Interface
   - **Environment.hpp** - Environment simulation
   - **Observation.hpp** - Observations from environment
   - **Action.hpp** - Actions taken by agent

#### 9. **agent/** - Agent-System Interface
   - **AgentBrain.hpp** - Interface between brain and world
   - **SensoryPercept.hpp** - Sensory perception processing

#### 10. **world/** - World Simulation
    - **SimpleWorld.hpp** - Simple world implementation

#### 11. **development/** - Developmental Processes
    - **DevelopmentSystem.hpp** - Age-dependent development
    - **Synaptogenesis.hpp** - Connection formation
    - **Pruning.hpp** - Connection elimination
    - **Maturation.hpp** - Neural maturation

#### 12. **plasticity/** - Learning Mechanisms
    - **PlasticityRule.hpp** - Base plasticity interface
    - **Hebbian.hpp** - Hebbian learning (fire together, wire together)
    - **STDP.hpp** - Spike-timing dependent plasticity
    - **StructuralPlasticity.hpp** - Structural changes

#### 13. **dynamics/** - Neural Dynamics
    - **NeuralDynamics.hpp** - Neural state dynamics
    - **SpikeSystem.hpp** - Spike propagation

#### 14. **experiments/** - Experiment Framework
    - **Experiment.hpp** - Base experiment class
    - **ExperimentRunner.hpp** - Experiment management
    - **Metrics.hpp** - Performance metrics collection

#### 15. **visualization/** - Visualization
    - **VisualizationInterface.hpp** - Visualization system interface

## Core Concepts

### Neural Architecture
The brain is organized hierarchically:
- **Regions** contain **Populations**
- **Populations** contain **Neurons**
- **Neurons** connect via **Synapses**

### Memory Systems
The brain implements multiple memory systems working together:
- **Working Memory**: Active maintenance of information
- **Episodic Memory**: Specific past experiences
- **Semantic Memory**: General knowledge and concepts
- **Procedural Memory**: Skills and habits
- **Associative Memory**: Pattern associations

### Neuromodulation
Neuromodulators regulate brain activity:
- **Reward**: Reinforcement learning signals
- **Prediction Error**: Discrepancy between expected and actual
- **Novelty**: Detection of new information
- **Curiosity**: Drive to explore

### Predictive Processing
The brain constantly predicts:
- **Perception**: What will be sensed next
- **Action**: What actions will produce desired outcomes
- **Reward**: What will be rewarding

## Building and Running

### Building from Source

1. **Standard Build**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Build with Python Bindings**
   ```bash
   mkdir build
   cd build
   cmake .. -DPYTHON_EXECUTABLE=/path/to/python
   make -j4
   ```

3. **Build Options**
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug
                          # Release (default), Debug, RelWithDebInfo
                 -DNLM_ENABLE_TESTS=ON     # Enable tests
                 -DNLM_ENABLE_PYTHON=ON    # Enable Python bindings
   ```

### Running Tests

1. **Run Unit Tests**
   ```bash
   cd build
   ./test_runner
   ```

2. **Run Specific Tests**
   ```bash
   ./test_runner --brain-test
   ./test_runner --neuron-test
   ./test_runner --stdp-test
   ```

### Running Demonstrations

1. **Phase 3 Demo**
   ```bash
   ./nlm_phase3_demo --help
   ./nlm_phase3_demo
   ```

2. **Phase 4 Demo**
   ```bash
   ./nlm_phase4_demo
   ```

3. **Phase 6 Demo**
   ```bash
   ./nlm_phase6_demo
   ```

## Code Standards and Best Practices

### Naming Conventions
- **Classes**: PascalCase (`Brain`, `Neuron`, `Synapse`)
- **Functions**: camelCase (`initialize()`, `step()`, `getSpikes()`)
- **Variables**: snake_case (`neuron_count`, `simulation_timestep`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_NEURONS`, `PI_VALUE`)

### Documentation
Every public class and function should have:
- Brief description of purpose
- Parameters with types and descriptions
- Return value description
- Example usage where appropriate

### Error Handling
- Use shared pointers (`std::shared_ptr`) for resource management
- Validate inputs at function entry
- Use NLM_LOG_ERROR for error conditions
- Return meaningful error codes

### Performance Considerations
- Use const references where appropriate
- Minimize heap allocations in tight loops
- Consider SIMD vectorization for numeric operations
- Use event-driven models where possible

### Memory Management
- RAII (Resource Acquisition Is Initialization) for all resources
- Use smart pointers instead of raw pointers
- Be aware of memory pools for performance-critical code
- Implement proper cleanup in destructors

## Development Guidelines

### File Organization
- Group related functionality in appropriate subdirectories
- Keep header files (`.hpp`) separate from implementation (`.cpp`)
- Use forward declarations to reduce include dependencies
- Maintain include guards in headers

### Code Structure
- Each class should have a single responsibility
- Use namespaces to avoid name collisions (`nlm::`, `nlm::brain::`)
- Keep compilation units small and focused
- Use templates for generic functionality

### Testing
- Unit tests should cover public interfaces
- Test edge cases and error conditions
- Use assertions for debugging-friendly tests
- Run tests after any code changes

## Python Bindings

The core system provides Python bindings via `pynlm`:

### Installation
```bash
pip install pynlm
```

### Basic Usage
```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### Key Python Classes
- `createBrain(config)` - Create a brain with configuration
- `createSimpleWorld()` - Create a simple environment
- `createAgentBrain(brain)` - Create agent interface
- `createDefaultConfig()` - Get default configuration

### Available Features via Python
- Brain initialization and simulation
- Environment interaction
- Agent control (sensory input, motor output)
- Learning mechanisms (reward modulation)
- Structural plasticity
- Development stages
- Curiosity and novelty detection

## Further Reading

- **Main Documentation**: See README.md for project overview
- **API Reference**: See API_REFERENCE.md for detailed API documentation
- **Component Guide**: See COMPONENT_GUIDE.md for subsystem details
- **Contributing**: See CONTRIBUTING.md for development guidelines
- **Architecture**: See docs/ARCHITECTURE.md for detailed architecture
- **Science**: See docs/SCIENCE.md for scientific background
