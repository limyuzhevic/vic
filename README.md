# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Quick Start Guide

For a rapid introduction to NLM, see our **Quick Start Guide** at `QUICK_START.md` or visit our **Unified Documentation Hub** at `NLM_Documentation.md` for comprehensive documentation.

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

**PHASE 6: FINAL INTEGRATION** - All previous phases completed and integrated into a coherent artificial brain system.

For detailed Phase 6 integration achievements, see `docs/PHASE6_FINAL_AUDIT.md` or the Unified Documentation Hub.

## Quick Usage

### Basic Python Usage
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Complete Agent Example
```python
import pynlm

def run_agent_simulation(world_size=20, num_steps=500):
    """Run a complete agent simulation with learning and development."""
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, 
                   visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Run simulation with progress tracking
    pynlm.run_with_progress(brain, world, agent, num_steps)
    
    return brain, world, agent

# Run the simulation
brain, world, agent = run_agent_simulation()
```

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

## Python Package Structure

We've reorganized the Python bindings into a modern package structure:

```
pynlm/
├── __init__.py              # Main package interface
├── brain.py                 # Brain simulation classes
├── agent.py                 # Agent brain interface
├── world.py                 # World simulation
├── sensory.py               # Sensory input systems
├── motor.py                 # Motor and action systems
├── config.py                # Configuration management
├── types.py                 # Type definitions and enumerations
├── exceptions.py            # Custom exception classes
├── utils.py                 # Utility functions
├── README.md                # Module-specific documentation
└── examples/                # Example scripts
    ├── basic.py             # Basic usage examples
    ├── agent_simulation.py  # Complete agent example
    ├── advanced_features.py # Advanced feature demonstrations
    └── performance_tests.py # Performance benchmarking
```

## Documentation

### Unified Documentation Hub
Our comprehensive documentation is now consolidated in `NLM_Documentation.md`, which includes:

- **Quick Start Guide** with code examples
- **Complete API documentation** with all classes and methods
- **Architecture overview** and system philosophy
- **Development and debugging** guides
- **Performance and monitoring** tools
- **Configuration options** and parameters
- **Phase 6 integration achievements**

### Other Documentation
- `docs/` directory: Technical documentation, scientific background, experiment descriptions
- `docs/PHASE6_FINAL_AUDIT.md`: Detailed Phase 6 audit report
- `NLM_Documentation.md`: Unified comprehensive guide (NEW)

## Performance Monitoring and Utilities

The Python package now includes enhanced utilities:

### Progress Tracking
```python
# Run simulation with progress reporting and statistics
pynlm.run_with_progress(brain, world, agent, num_steps=1000)
```

### Validation
```python
# Validate simulation setup
pynlm.validate_simulation_setup(brain, world, agent)
```

### Checkpoint Management
```python
# Save simulation state with metadata
pynlm.save_checkpoint(brain, "checkpoint.bin", "Simulation at step 100")
```

### Benchmarking
```python
# Benchmark simulation performance
stats = pynlm.benchmark_simulation(brain, world, agent, num_steps=1000)
print(f"Average time: {stats['average_time']:.3f}s")
print(f"Steps per second: {stats['steps_per_second']:.1f}")
```

## Key Improvements Made

### 1. **Modern Python Package Structure**
- Created organized `pynlm/` package with proper `__init__.py`
- Split functionality into logical modules (brain, agent, world, sensory, motor, config, types)
- Added comprehensive utilities and error handling

### 2. **Enhanced Documentation**
- Created unified `NLM_Documentation.md` hub
- Consolidated information from `easy_usage.md`, `HOW_TO_USE.md`, and `README.md`
- Added modern API documentation with examples
- Maintained backward compatibility with existing docs

### 3. **Advanced Python Features**
- Added performance monitoring utilities
- Implemented progress tracking for long simulations
- Created checkpoint management with metadata
- Added benchmarking tools
- Implemented validation utilities

### 4. **Developer Experience**
- Added comprehensive error handling with custom exceptions
- Included quick reference guides and example patterns
- Enhanced interactive utilities
- Improved build configuration with modern pyproject.toml

### 5. **Backward Compatibility**
- All existing Python API functionality preserved
- Existing examples continue to work
- Documentation transitions are smooth
- No breaking changes to public interfaces

## Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

## Testing

### Running Tests
```bash
# Install development dependencies
pip install -e .[dev]

# Run unit tests
pytest

# Run tests with coverage
pytest --cov=pynlm --cov-report=html
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

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background.
