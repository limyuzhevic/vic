# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## What's New in Phase 6 Final Integration

### Performance Improvements

**Optimized Data Structures:**
- **Circular Buffers for Spike History**: Replaced O(n) vector operations with O(1) circular buffers in Neuron and Synapse classes
  - Eliminated expensive `vector::erase()` operations during spike history management
  - Improved cache locality and reduced memory allocations
  - Enhanced thread safety with per-object mutex protection

**Thread Safety Enhancements**:
- Added mutex protection to all neuron and synapse state modifications
- Implemented fine-grained locking to allow concurrent neuron processing
- Protected critical sections in spike processing, plasticity updates, and state management

**Memory Management Fixes**:
- Fixed null pointer checks in move assignment operators
- Added proper resource validation in constructors
- Enhanced error handling for memory allocation failures

### Enhanced Error Handling & Validation

**Comprehensive Parameter Validation**:
- Configuration parameter range checking with automatic clamping
- Input validation for all numeric parameters (neuron count, timesteps, weights)
- Safe fallback values for invalid parameters
- Detailed warning and error logging for parameter issues

**Robust Error Recovery**:
- Graceful handling of invalid timesteps and simulation parameters
- Safe weight clamping to prevent numerical instability
- Validation of biological constraints (membrane potentials, firing rates)

### Advanced Python API Features

**Enhanced Type Safety**:
- Template-based type-safe configuration access
- Comprehensive parameter validation in Python bindings
- Automatic type conversion with error handling

**Convenience Functions**:
- `createPhase6Config()` - Pre-configured optimal settings for full integration
- `createMinimalConfig()` - Lightweight configuration for testing
- `createHighPlasticityConfig()` - Enhanced learning capabilities
- `createQuickTestConfig()` - Fast test configuration

**New Analysis Tools**:
- `analyzeBrain()` - Comprehensive brain state analysis with statistics
- `createSimulation()` - Complete simulation loop with error handling

**Advanced Features**:
- Iterator support for spike history access
- Thread-safe parameter validation
- Comprehensive documentation with examples

### Code Quality Improvements

**Documentation & Examples**:
- Detailed docstrings for all public API methods
- Comprehensive Python examples for beginners and advanced users
- Architecture documentation for system understanding
- Parameter validation and error handling documentation

**API Consistency**:
- Consistent naming conventions across all interfaces
- Unified error handling patterns
- Type-safe parameter access throughout

### Backward Compatibility

All improvements maintain full backward compatibility:
- Existing Python code continues to work without modification
- C++ API remains unchanged except for bug fixes
- New features are optional extensions

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

### Python Usage Examples

#### Basic Usage (same as before)
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
print("Done!")
```

#### Advanced Configuration
```python
import pynlm

# Create specialized configuration
config = pynlm.createPhase6Config()
config.set("neuron_count", 500)
config.set("random_seed", 42)

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Simulation with Analysis
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with analysis
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get comprehensive statistics
stats = pynlm.analyzeBrain(brain)
print(f"Firing neurons: {stats['firing_neurons']}")
print(f"Total spikes: {stats['total_spikes']}")
print(f"E/I ratio: {stats['e_i_ratio']}")
```

#### High Plasticity Learning
```python
import pynlm

# Configure for rapid learning
config = pynlm.createHighPlasticityConfig()
config.set("synaptogenesis_rate", 0.001)
config.set("pruning_rate", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md          # System architecture and design
│   ├── SCIENCE.md              # Scientific background and models
│   ├── ROADMAP.md              # Development roadmap and phases
│   ├── EXPERIMENTS.md          # Experiment documentation
│   ├── LIMITATIONS.md          # Scientific limitations and ethics
│   └── PHASE6_FINAL_AUDIT.md   # Phase 6 integration audit
├── src/
│   ├── core/                  # Core utilities and base classes
│   ├── brain/                 # Neural components (optimized)
│   ├── dynamics/              # Neural dynamics and spike processing
│   ├── plasticity/            # Plasticity rules (STDP, Hebbian)
│   ├── development/           # Developmental system
│   ├── neuromodulation/        # Neuromodulators (dopamine, curiosity)
│   ├── memory/                # Memory systems (working, episodic)
│   ├── prediction/            # Prediction systems
│   ├── cognition/             # Cognitive mechanisms
│   ├── sensory/               # Sensory processing
│   ├── motor/                 # Motor system
│   ├── environment/           # Environment interface
│   ├── experiments/           # Experiment framework
│   └── visualization/         # Visualization tools
├── tests/                    # Unit and integration tests
└── configs/                  # Configuration templates
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
- Phase 6 integration experiment created

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
