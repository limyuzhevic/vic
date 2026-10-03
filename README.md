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

## Quick Access to Documentation

### 📖 Complete Python API Reference
For **comprehensive API documentation** and examples:
- `python/README.md` - Complete Python API reference guide
- Contains: All classes, methods, examples, and troubleshooting

### 🚀 Easy Usage Guide
For **beginner-friendly examples**:
- `HOW_TO_USE.md` - Technical details and usage patterns

### 🔧 Installation Guide
For **complete build and installation instructions**:
- `HOW_TO_USE.md` - Building from source and Python bindings

### 📚 Architecture Documentation
- `docs/ARCHITECTURE.md` - System architecture overview
- `docs/SCIENCE.md` - Scientific background
- `docs/EXPERIMENTS.md` - Experiment descriptions

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

## Quick Start

### Minimum Requirements
- C++20 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+
- Build tools

### Build and Run

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run the comprehensive integration test
./nlm_phase6_demo
```

### Python Usage (Recommended for Most Users)

For most use cases, you can use the **Python API** which provides a simplified interface to the NLM brain simulation:

```python
import pynlm

# Create and initialize a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

For **complete Python API documentation** and **comprehensive examples**, see:

### 📖 Complete Python API Documentation
`python/README.md`

This file contains:
- Complete API reference with all classes and methods
- 20+ detailed examples from simple to advanced
- Configuration management and optimization tips
- Error handling and troubleshooting
- Performance optimization guidance
- Migration guides and advanced usage patterns

### 🚀 Additional Documentation

| File | Purpose |
|------|---------|
| `HOW_TO_USE.md` | Technical details, build instructions, and basic usage |
| `python/README.md` | Complete Python API reference and examples |
| `docs/ARCHITECTURE.md` | System architecture overview |
| `docs/SCIENCE.md` | Scientific background and principles |
| `docs/EXPERIMENTS.md` | Experiment descriptions and results |

## Project Structure

```
NLM/
├── CMakeLists.txt                    # Build configuration
├── README.md                         # This file (overview)
├── HOW_TO_USE.md                      # Technical usage guide
├── python/                           # Python bindings
│   ├── README.md                    # Complete Python API documentation
│   └── ...                          # Python bindings source
├── docs/                            # Documentation
│   ├── ARCHITECTURE.md              # System architecture
│   ├── SCIENCE.md                   # Scientific background
│   ├── ROADMAP.md                   # Development roadmap
│   ├── EXPERIMENTS.md               # Experiment descriptions
│   ├── LIMITATIONS.md               # Scientific limitations
│   └── PHASE6_FINAL_AUDIT.md        # Phase 6 verification
├── src/                             # Source code
│   ├── core/                        # Core utilities
│   ├── brain/                       # Neural components
│   ├── dynamics/                    # Neural dynamics
│   ├── plasticity/                  # Plasticity rules
│   ├── development/                 # Development system
│   ├── neuromodulation/             # Neuromodulation systems
│   ├── memory/                     # Memory systems
│   ├── prediction/                  # Prediction systems
│   ├── cognition/                   # Cognitive mechanisms
│   ├── sensory/                     # Sensory processing
│   ├── motor/                       # Motor system
│   ├── environment/                 # Environment interface
│   ├── experiments/                 # Experiment frameworks
│   └── visualization/               # Visualization
│
├── tests/                           # Unit tests
└── configs/                         # Configuration files
```

## Building from Source

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git python3 python3-pip

# macOS
xcode-select --install
brew install cmake python3

# Windows
# Use Chocolatey: choco install cmake python visualstudio2019-workload-nativedevtools
```

### Build Steps

```bash
# Clone the repository
cd /path/to/nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
# Release build (default) - optimized for performance
cmake .. -DCMAKE_BUILD_TYPE=Release

# Debug build - with symbols and assertions
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . --config Release
# or
make -j$(nproc)

# Run tests (if enabled)
ctest --output-on-failure

# Run demo (if built)
./nlm_phase6_demo
```

### Build Options

```bash
# Control what gets built
cmake .. \
    -DBUILD_TESTS=ON/OFF \
    -DBUILD_EXAMPLES=ON/OFF \
    -DBUILD_PYTHON_BINDINGS=ON/OFF \
    -DENABLE_DEBUG_LOGS=ON/OFF \
    -DENABLE_PERFORMANCE_COUNTERS=ON/OFF

# Install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running the Python Library

### Installation

#### Option 1: pip install (Recommended)

```bash
pip install pynlm
```

#### Option 2: Development Installation

```bash
# Build from source (requires C++ compiler and CMake)
pip install -e .

# Or clone and install
cd /path/to/nlm
pip install -e .
```

### Basic Python Usage

#### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Example 2: Complete Agent Simulation

```python
import pynlm

def run_agent_simulation(steps=1000):
    """Run a complete agent simulation."""
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
    
    return brain, agent, world

brain, agent, world = run_agent_simulation(1000)
```

### Complete Documentation

For **detailed documentation** covering all aspects of the NLM project, including:

- Complete Python API reference with 20+ examples
- Build and installation instructions
- Configuration management
- Advanced usage patterns
- Error handling and troubleshooting
- Performance optimization

See: **`python/README.md`**

## Running Examples and Tests

### Demo Executables

After building, you can run these demo executables:

| Executable | Description |
|------------|-------------|
| `./nlm_phase3_demo` | Phase 3 basic neural computation demo |
| `./nlm_phase4_demo` | Phase 4 cognitive abilities demo |
| `./nlm_phase6_demo` | Phase 6 complete integration test |

### Unit Tests

Run the C++ unit tests:

```bash
cd build
ctest
```

Or with verbose output:

```bash
cd build
ctest --output-on-failure
```

## Configuration

### Configuration File Format

NLM uses JSON configuration files. Examples:

#### Basic Configuration (`configs/basic.cfg`)

```json
{
    "brain": {
        "neuron_count": 1000,
        "connection_probability": 0.1,
        "synapse_density": 0.1
    },
    "plasticity": {
        "stdp": {"enable": true},
        "hebbian": {"enable": true},
        "structural": {"enable": true}
    },
    "neuromod": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": true},
        "novelty": {"enable": true}
    }
}
```

#### Learning Configuration (`configs/learning.cfg`)

```json
{
    "brain": {
        "neuron_count": 2000,
        "connection_probability": 0.05
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.005,
            "tau_plus": 25.0,
            "tau_minus": 20.0
        },
        "hebbian": {"enable": true},
        "structural": {"enable": true}
    },
    "neuromod": {
        "dopamine": {"scale": 2.0},
        "curiosity": {"enable": true},
        "novelty": {"enable": true}
    },
    "development": {
        "plasticity_modulation": 1.5,
        "enable_critical_period": true
    }
}
```

### Command Line Arguments

Load configuration from command line arguments:

```bash
./nlm --config "configs/learning.cfg"

# Or override specific parameters
./nlm --config configs/basic.cfg \
    --brain.neuron_count=500 \
    --plasticity.stdp.learning_rate=0.01
```

## Advanced Features

### Checkpointing

Save and load brain states:

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation...

# Save checkpoint
brain.save("checkpoint.bin")

# Later, load checkpoint
brain2 = pynlm.createBrain(config)
brain2.load("checkpoint.bin")
```

### Multi-Step Training

Train with structured learning stages:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Phase 1: Initial learning
for step in range(100):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 20.0)
    brain.step(step)

# Phase 2: Refinement
for step in range(100, 200):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
    brain.step(step)

# Phase 3: Consolidation
for step in range(200, 300):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 5.0)
    brain.step(step)
```

### Performance Monitoring

Monitor simulation performance:

```python
import time
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

start = time.time()
for step in range(10000):
    brain.step(step)
    
elapsed = time.time() - start
steps_per_second = 10000 / elapsed
print(f"Performance: {steps_per_second:.1f} steps/second")
```

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

## Getting Help

For questions and support:

1. **Read the documentation**: `python/README.md` for complete API reference
2. **Check examples**: Code in `src/experiments/` for real-world usage patterns
3. **Review tests**: Look at test files in `tests/` for usage examples
4. **Community**: For issues, questions, and discussions

## Quick Summary

This is an experimental brain simulation project that:

- **Is not** a chatbot, transformer, or deep learning model
- **Is not** claiming consciousness or human intelligence
- **Is investigating** whether artificial brains can learn and develop
- **Provides** a complete brain simulation framework with Python bindings
- **Includes** 6 phases of development, currently at Phase 6 (final integration)
- **Has** comprehensive documentation and examples

The NLM project represents a **bottom-up approach** to creating artificial intelligence through neural development, learning, and experience, inspired by biological brain principles.