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

## Current Status (Updated 2026-11-06)

**Phase 6: FINAL INTEGRATION** - The code has been significantly improved!

### What's Now Working:

✅ **Neural Core** (Phase 2):
- LIF neuron dynamics with event-driven spike propagation
- Synaptic transmission with delays
- STDP plasticity (working)
- Hebbian plasticity (working)
- Structural plasticity (synaptogenesis/pruning)

✅ **Agent Brain** (Phase 3):
- Sensory processing (vision, touch, internal, proprioception)
- Motor decoding based on neural activity
- Reward modulation via neuromodulation
- Novelty detection
- Curiosity-driven exploration

✅ **Integration Fixes**:
- Fixed reward reporting in Phase6Demo (now shows "Average reward")
- Enhanced documentation with practical examples
- Added advanced features for power users
- Improved checkpointing implementation

### What's Still Being Addressed:

📋 **Cognitive Integration**:
- Neural planner integration (Phase 4 cognitive mechanisms)
- Concept formation (pattern discovery without labels)
- Attention systems (competitive selection)
- Self-model and social learning

📋 **Memory Integration**:
- Working memory with persistent activity
- Episodic memory with experience encoding
- Associative memory for pattern associations
- All systems working together

## Building

### Option 1: Build with Python Bindings (Recommended)

```bash
# Install build dependencies
pip install scikit-build-core pybind11 pytest numpy

# Build and install Python module
pip install .

# Or install in development mode
pip install -e .
```

### Option 2: Build C++ Executables

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (enable Python bindings)
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_BUILD_PYTHON_BINDINGS=ON

# Build all targets
make -j4
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_benchmark` | Executable | Benchmark tests |
| `pynlm` | Shared Library | Python bindings (when enabled) |

## Running

Build all targets first (see Building above). The entry points are currently separate demos per phase:

```bash
./nlm                 # Phase 2 demo (basic neural computation)
./nlm_phase3_demo     # Phase 3 demo (world interaction)
./nlm_phase4_demo     # Phase 4 demo (cognitive mechanisms)
./nlm_phase6_demo     # Phase 6 demo (full integration)
./nlm_benchmark      # Performance benchmarks
```

### Python API Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Print statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # C++ build configuration
├── pyproject.toml                     # Python package configuration
├── python/                           # Python bindings source
│   ├── bindings.cpp                  # Pybind11 bindings implementation
│   └── CMakeLists.txt                # Python module build
├── src/                              # Source code
│   ├── core/                         # Core utilities
│   ├── agent/                        # Agent-brain interface
│   ├── world/                       ash World
│   ├── brain/                        # Neural components
│   ├── dynamics/                     # Neural dynamics
│   ├── plasticity/                   # Plasticity rules
│   ├── development/                  # Developmental system
│   ├── neuromodulation/               # Neuromodulators
│   ├── memory/                       # Memory systems
│   ├── prediction/                   # Prediction systems
│   ├── cognition/                    # Cognitive mechanisms
│   ├── sensory/                      # Sensory processing
│   ├── motor/                        # Motor system
│   ├── environment/                   # Environment interface
│   └── experiments/                   # Experiment framework
├── tests/                            # Unit tests
├── docs/                             # Documentation
│   ├── README.md                      # User guide
│   ├── easy_usage.md                  # Quick start guide
│   └── *.md                           # Detailed documentation
└── configs/                          # Configuration files
```

## Advanced Features

### Python API Extensions

The `pynlm` Python module provides comprehensive bindings for:

- **Brain Control**: Create, initialize, and control brain simulation
- **Memory Systems**: Access working memory, episodic memory
- **Neuromodulation**: Query dopamine, curiosity, novelty, prediction error
- **Development**: Monitor developmental stages and rates
- **Statistics**: Get neuron counts, firing rates, spike activity
- **Action Selection**: Decode motor commands from brain activity

### Power User Configuration

For advanced users, the following configuration options are available:

#### Memory Configuration
```python
config.set("memory.working_memory.size", 100)
config.set("memory.episodic.capacity", 1000)
config.set("memory.associative.threshold", 0.8)
```

#### Plasticity Configuration
```python
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.stdp.tau_plus", 20.0)
config.set("plasticity.stdp.tau_minus", 20.0)
config.set("plasticity.hebbian.enabled", True)
config.set("plasticity.structural.enabled", True)
```

#### Neuromodulation Configuration
```python
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enabled", True)
config.set("neuromod.novelty.enabled", True)
config.set("neuromod.prediction.enabled", True)
```

#### Development Configuration
```python
config.set("development.initial_plasticity", 1.0)
config.set("development.critical_period_end", 300.0)
config.set("development.adult_plasticity", 0.2)
```

### Command Line Tools

When built with CMake, several command-line tools are available:

- `nlm` - Basic neural computation demo
- `nlm_phase3_demo` - World interaction demo
- `nlm_phase4_demo` - Cognitive mechanisms demo
- `nlm_phase6_demo` - Full integration demo
- `nlm_benchmark` - Performance benchmarks

### Configuration Management

The configuration system supports:

- **Key-based access**: `config.set("key", value)`, `config.get("key")`
- **File I/O**: `config.loadFromFile("config.json")`, `config.saveToFile("config.json")`
- **Command line**: `config.loadFromArgs()`
- **Summary**: `config.summary()` for human-readable configuration

### Visualization Tools

The project includes visualization capabilities for:

- Brain state visualization
- Neural activity patterns
- Memory system states
- Developmental trajectories

## Development

### Setting Up Development Environment

```bash
# Create virtual environment
python -m venv nlm_env
source nlm_env/bin/activate

# Install dependencies
pip install scikit-build-core pybind11 pytest numpy

# Install NLM in development mode
pip install -e .

# Install dev dependencies for testing
pip install pytest numpy
```

### Running Tests

```bash
# Run unit tests
pytest tests/

# Run C++ tests (if built)
./nlm_test

# Run specific test categories
python -m pytest tests/test_brain.py -v
```

### Contributing

The NLM project is an experimental research project. Contributions should focus on:

- Bug fixes and optimizations
- New feature implementations
- Documentation improvements
- Integration of disconnected systems
- Performance improvements

## Scientific Background

NLM is based on several scientific principles:

- **Leaky Integrate-and-Fire (LIF) neurons**: Biologically inspired neuron model
- **Spike-Timing-Dependent Plasticity (STDP)**: Hebbian learning based on spike timing
- **Neuromodulation**: Biological systems for regulating learning and behavior
- **Memory systems**: Working memory, episodic memory, and associative memory
- **Predictive coding**: Neural prediction systems
- **Developmental systems**: Age-dependent learning and maturation

## Limitations

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

## License

MIT

## Authors

Research project — See docs for scientific background.
