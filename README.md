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

### Phase 6 Integration Achievements

#### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

#### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

#### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

#### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

#### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

#### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

#### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle
## Installation

### Quick Start

For most users, the easiest way to get started is to install the Python package, which includes the core C++ libraries and provides the main user interface.

#### Python Installation (Recommended)

```bash
# Install the NLM Python package
pip install .

# For development, install in editable mode
pip install -e .

# Verify installation
python -c "import pynlm; brain = pynlm.createBrain(pynlm.createDefaultConfig()); print('NLM successfully installed!')"
```

#### System Dependencies

The Python package includes the core C++ libraries, but for advanced development or standalone use, you'll need:

```bash
# Install system dependencies
# On Ubuntu/Debian:
apt-get update
apt-get install build-essential cmake git

# On macOS:
brew install cmake

# On Windows:
# Install from https://visualstudio.microsoft.com/vs/community/
```

### Building from Source

For those who need to build the C++ libraries directly:

#### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

#### Build Steps

```bash
# Clone the repository (if you haven't already)
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Create build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

#### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration demo |
| `nlm_test` | Executable | Unit test suite |

#### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Build with custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install

# Build with specific features
# Example: Enable visualization
cmake .. -DENABLE_VISUALIZATION=ON
```

### Python Package Installation

The recommended way to use NLM is through the Python package, which includes all the core functionality and provides a user-friendly interface.

#### Using pip (Recommended)

```bash
# Install the latest release
pip install pynlm

# Install from source
pip install .
```

#### From PyPI (for binary distributions)

```bash
pip install pynlm
```

#### Development Installation

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Install in development mode (recommended for development)
pip install -e .

# Build documentation
pip install -e .[docs]
```

### Testing and Verification

After installation, you can verify that everything works correctly:

```python
# Test basic functionality
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("✓ Brain created and initialized")
print(f"✓ Brain has {brain.getTotalNeuronCount()} neurons")

# Test world interaction
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

print("✓ World configured")

# Test agent functionality
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("✓ Agent created and initialized")

# Run a simple simulation
for step in range(10):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"✓ Simulation completed with {brain.getTotalSpikeCount()} total spikes")
print("✓ All basic functionality tests passed!")
```

### System Requirements

#### Minimum Requirements

- **Operating System**: Linux, macOS, or Windows
- **Python**: 3.8 or higher
- **Memory**: 2GB RAM minimum (4GB+ recommended for simulations)
- **Disk Space**: 500MB for the package (additional space for simulations)

#### Recommended Hardware

For large-scale simulations:

- **Memory**: 8GB+ RAM
- **CPU**: Multi-core processor (4+ cores recommended)
- **Disk Space**: 2GB+ for simulation data and checkpoints

### Troubleshooting

#### Installation Issues

**"pip install . failed"**

```bash
# Try with verbose output
pip install -v .

# Clear pip cache
pip cache purge
pip install --no-cache-dir .
```

**Missing dependencies**:

```bash
# On Ubuntu/Debian
apt-get install build-essential cmake git

# On macOS with Homebrew
brew install cmake

# On Windows (if needed)
choco install cmake
```

#### Runtime Issues

**"ImportError: libname not found: libpynlm.so"**

```bash
# On Linux, you may need to set the library path
export LD_LIBRARY_PATH=/path/to/nlm/lib:$LD_LIBRARY_PATH

# Or install system-wide
pip install . --user
```

**"Python bindings not working"**

```python
# Check Python path
import sys
print(sys.path)

# Try to import pybind11 directly
import pybind11
print("Pybind11 version:", pybind11.__version__)
```

#### Build Issues

**CMake configuration fails**:

```bash
# Try different CMake version
cmake .. -DCMAKE_CXX_STANDARD=17

# Or clean build directory
cd build
rm -rf *
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Development Setup

For developers who want to contribute to NLM:

#### Clone Repository

```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

#### Create Development Branch

```bash
# Create and switch to a development branch
git checkout -b feature/your-feature-name
```

#### Install Development Dependencies

```bash
# Install Python development tools
pip install -e .[dev]

# Install documentation tools
pip install -e .[docs]

# Install testing tools
pip install -e .[test]
```

#### Running Tests

```bash
# Run unit tests
cd build
ctest --output-on-failure

# Or run Python tests
pytest tests/
```

#### Running Demos

```bash
# Run Phase 3 demo (if built)
./nlm_phase3_demo

# Run Phase 4 demo (if built)
./nlm_phase4_demo

# Run Phase 6 demo (if built)
./nlm_phase6_demo
```

### Post-Installation Setup

After installation, you may want to set up some additional configuration:

#### Create Configuration Directory

```bash
mkdir -p ~/.nlm
```

#### Create Example Configuration File

```bash
cat > ~/.nlm/nlm_config.json << EOF
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012
}
EOF
```

#### Set Environment Variables

```bash
# Add NLM to Python path
export PYTHONPATH="/path/to/nlm:$PYTHONPATH"

# Set NLM home directory
export NLM_HOME="/path/to/nlm"
```

### Documentation

After installation, you can access documentation:

```bash
# View documentation in terminal
pydoc pynlm

# Or browse online documentation
# Visit https://nlm.readthedocs.io
```

### Support

If you encounter issues during installation or usage:

#### Common Issues

1. **ModuleNotFoundError**: Ensure you're using the correct Python environment
2. **ImportError**: Check that the C++ libraries are properly built
3. **Configuration errors**: Check your configuration files for syntax errors

#### Getting Help

- **GitHub Issues**: https://github.com/nlm-project/nlm/issues
- **Community Forum**: https://community.nlm-project.org
- **Documentation**: https://nlm.readthedocs.io
- **Email Support**: support@nlm-project.org

### Upgrading

To upgrade to a newer version:

```bash
# Upgrade from pip
pip install --upgrade pynlm

# Upgrade from source
cd /path/to/nlm
pip install -U -e .
```

### Uninstalling

To remove NLM:

```bash
# Uninstall Python package
pip uninstall pynlm

# Remove system-wide installation (if applicable)
sudo make uninstall

# Clean up configuration files
rm -rf ~/.nlm
```

---

## Conclusion

NLM is a comprehensive neural simulation framework that provides both C++ and Python interfaces for building and simulating artificial brains. Whether you're a beginner looking to understand basic concepts or an advanced researcher wanting to push the boundaries of neural simulation, NLM offers the tools you need.

**Key Features**:
- **Modular Architecture**: Separate components for memory, prediction, cognition, and development
- **Python Interface**: High-level, user-friendly API for rapid prototyping
- **C++ Core**: High-performance implementation for large-scale simulations
- **Learning Capabilities**: Multiple plasticity mechanisms including STDP and Hebbian learning
- **Development System**: Artificial development with age-dependent plasticity
- **Checkpoint System**: Ability to save and load simulation state
- **Replay System**: Memory consolidation and replay functionality

**For more information:**
- Visit the documentation website: https://nlm.readthedocs.io
- Join the NLM community forum: https://community.nlm-project.org
- Explore example projects and tutorials in the repository
- Check out the Phase 6 integration demo for a complete example

That's it! You're now ready to use NLM for neural simulation and research.

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
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
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
