# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What is NLM?

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

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Enhanced Python Bindings (Phase 2 Enhancement)

NLM now includes comprehensive Python bindings that provide access to all NLM features. The Python bindings are organized into a single enhanced module `pynlm` that includes:

#### Key Features

**1. Enhanced Brain Factory**
- Create custom brain architectures with various configurations
- Factory methods for default, enhanced, and minimal brains
- Configuration validation and error handling
- Support for different brain types and neuron configurations

**2. Command System**
- Priority-based command execution for sophisticated action selection
- Command management with enable/disable capabilities
- SimpleCommand for wrapping actions
- ConditionalCommand for context-aware behavior

**3. Thread-safe Simulation Management**
- ThreadPool for parallel processing
- ThreadSafeSimulationManager for coordinated neural operations
- Performance monitoring with simulation statistics
- Memory management with ThreadSafeAllocator

**4. Advanced Topology Support**
- Convolutional networks (CNN) for spatial processing
- ResNet and skip connections for deep learning
- Autoencoder for dimensionality reduction
- Graph Neural Networks for relational data
- Hybrid topologies combining multiple approaches

**5. Comprehensive Error Handling**
- NLMException with detailed error categories
- ErrorResult for graceful error reporting
- Validation for configuration and topology
- Exception handling with specific error types

**6. Pythonic Interface**
- Context managers for resource cleanup
- Property access instead of getter methods
- Enhanced string representations
- Collection iteration support

#### Installation

```bash
# Clone the NLM repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=$(which python3)

# Build the project
make -j$(nproc)

# Install the Python module
pip install -e .
```

#### Quick Start Examples

**Basic Usage:**
```python
import pynlm as nlm

# Create a default brain with enhanced features
brain = nlm.createEnhancedBrain()

# Create a simple world
world = nlm.createSimpleWorld()

# Create an agent
agent = nlm.createAgentBrain(brain)

# Run a simulation for 100 steps
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation completed. Firing neurons: {brain.getFiringNeuronCount()}")
```

**Advanced Usage:**
```python
import pynlm as nlm

# Create an enhanced brain using the factory
config = nlm.BrainFactoryConfig()
config.brainType = nlm.BrainComponentType.EnhancedBrain
config.neuronCount = 5000
config.enablePlasticity = True
config.enableDevelopment = True
config.enableNeuromodulation = True

brain = nlm.BrainFactory.createBrain(config)

# Use advanced topology
topo_config = nlm.AdvancedTopologyConfig()
topo_config.topologyType = nlm.AdvancedTopologyType.Convolutional
topo_config.layerSizes = [256, 128, 64, 32, 16]
advanced_brain = nlm.AdvancedTopologyManager.createFromTopology(topo_config)

# Set up thread-safe simulation with 4 threads
thread_pool = nlm.ThreadPool(4)
sim_manager = nlm.ThreadSafeSimulationManager(thread_pool)

# Monitor memory usage
memory_allocator = nlm.ThreadSafeAllocator()
allocator_stats = memory_allocator.getStats()
print(f"Memory allocator stats: {allocator_stats}")

# Use command system for action selection
command_manager = nlm.CommandManager()
action = command_manager.executeCommands(brain)
print(f"Selected action: {action.getName()}")
```

#### Python API Reference

**BrainFactory Methods:**
- `createDefaultBrain()` - Create a standard brain
- `createEnhancedBrain()` - Create a brain with all features
- `createMinimalBrain(neuronCount)` - Create a minimal brain for testing
- `createBrainFromFile(configFile)` - Create brain from configuration file
- `validateConfig(config)` - Validate brain configuration
- `getDefaultConfig()` - Get default configuration
- `getEnhancedConfig()` - Get enhanced configuration

**Command System:**
- `CommandManager()` - Create command manager
- `addCommand(command)` - Add a command
- `removeCommand(name)` - Remove command by name
- `executeCommands(brain)` - Execute commands and get best action
- `setCommandEnabled(name, enabled)` - Enable/disable command
- `setCommandPriority(name, priority)` - Set command priority

**Topology System:**
- `AdvancedTopologyManager.createFromTopology(config)` - Create brain from topology
- `createConvolutionalTopology(config)` - Create CNN topology
- `createResidualTopology(config)` - Create ResNet topology
- `createAutoencoderTopology(config)` - Create autoencoder topology
- `getAvailableTopologyTypes()` - Get available topology types
- `validateTopologyConfig(config)` - Validate topology configuration

**Threading System:**
- `ThreadPool(numThreads)` - Create thread pool
- `ThreadSafeSimulationManager(threadPool)` - Create simulation manager
- `ThreadSafeAllocator(poolSize)` - Create memory allocator
- `getStats()` - Get statistics from managers

**Error Handling:**
- `ErrorResult.isOk()` - Check if result is OK
- `ErrorResult.isError()` - Check if result contains error
- `ErrorCategory` - Different error categories (Generic, Memory, Validation, etc.)
- `NLMException` - Main exception class with category support

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/     # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/
└── configs/
```

## Phase Summary

### Phase 1 (Complete)
- Project skeleton
- Core types and configuration
- Neural interfaces

### Phase 2 (Complete)
- Real LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity
- **Phase 2 Enhancement: Command System**

### Phase 3 (Complete)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

### Phase 4 (Complete)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

### Phase 5 (Complete)
- Performance optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6 (Complete - Final Integration)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional
- **Phase 6 Enhancement: Python Bindings**

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains. Current limitations include:

- Simplified LIF neuron model (not Hodgkin-Huxley)
- No realistic ion channel dynamics
- No detailed dendritic morphology
- No detailed cortical architecture
- No claim of consciousness, intelligence, or human-like cognition
- Limited to what can be simulated with available computing resources

Phase 6 does NOT claim:
- Human intelligence
- Consciousness or sentience
- Human-like reasoning
- Genuine subjective experience

Phase 6 DOES investigate:
- Whether memory systems can integrate with neural dynamics
- Whether neuromodulation can affect plasticity in a coordinated way
- Whether prediction can become a central organizing principle
- Whether developmental stages can modulate learning
- Whether replay can reinforce memory consolidation
- Whether the complete brain loop functions coherently

## Architecture Philosophy

The NLM brain operates as a closed-loop system:

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

## License

MIT

## Authors

Research project — See docs for scientific background.