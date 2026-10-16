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

### Command Line Interface

NLM provides several operational modes for different use cases:

#### Basic Modes
- **`test`** - Comprehensive test suite (runs all neural computation tests)
- **`demo`** - Interactive demonstration with visualization
- **`benchmark`** - Performance benchmarking mode
- **`continuous`** - Continuous real-time simulation
- **`runtime_config_test`** - Tests runtime configuration modifications
- **`runtime_config_demo`** - Demonstrates runtime configuration changes
- **`runtime_config_cli`** - Interactive command-line interface for runtime configuration

#### Quick Start Examples
```bash
# Run comprehensive tests (default)
./nlm

# Run demo with visualization
./nlm --mode demo --neurons 2000 --steps 5000 --pause

# Run performance benchmark
./nlm --mode benchmark --benchmark-scale --scaling-levels 100,500,1000,2000

# Run continuous simulation
./nlm --mode continuous --neurons 1000 --steps 10000 --max-time 300

# Test runtime configuration
./nlm --mode runtime_config_test --neurons 100

# Interactive runtime configuration CLI
./nlm --mode runtime_config_cli
```

### Configuration Files

NLM uses configuration files (`.cfg` format) to control simulation parameters:

#### Default Configuration
- **File**: `configs/default.cfg`
- **Purpose**: Standard configuration for most simulations
- **Use**: Most applications and experiments

#### Preset Configurations
- **File**: `configs/presets/research.cfg` - Optimized for scientific research
- **File**: `configs/presets/production.cfg` - Optimized for production use
- **File**: `configs/presets/development.cfg` - Optimized for development
- **File**: `configs/presets/benchmark.cfg` - Optimized for performance testing

#### Using Presets
```bash
# Use research preset
./nlm --mode test --config configs/presets/research.cfg --neurons 1000

# Use production preset
./nlm --mode continuous --config configs/presets/production.cfg

# Use development preset
./nlm --mode demo --config configs/presets/development.cfg

# Use benchmark preset
./nlm --mode benchmark --config configs/presets/benchmark.cfg
```

### Python Integration

NLM provides Python bindings for programmatic control:

#### Installation
```bash
pip install pynlm
```

#### Basic Usage
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Advanced Python Usage
```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Enable advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complete agent simulation
for step in range(500):
    world.update(timestep=0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

### Documentation Resources

#### Online Documentation
- **CLI Help**: `docs/CLI_HELP.md` - Complete command-line interface documentation
- **Configuration Guide**: `docs/CONFIG_GUIDE.md` - Detailed parameter documentation
- **Advanced Guide**: `docs/ADVANCED_GUIDE.md` - Advanced usage patterns
- **Troubleshooting**: `docs/TROUBLESHOOTING.md` - Common issues and solutions

#### Local Documentation
- **Architecture**: `docs/ARCHITECTURE.md` - System architecture overview
- **Science**: `docs/SCIENCE.md` - Scientific background
- **Experiments**: `docs/EXPERIMENTS.md` - Experiment descriptions
- **Phase 6 Results**: `docs/PHASE6_FINAL_RESULTS.md` - Integration experiment results

#### Quick Help
```bash
# Display basic help
./nlm

# Display help for specific mode
./nlm --mode demo

# Display help for runtime config mode
./nlm --mode runtime_config_cli
```

### Development

#### Building from Source
```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (Release for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4
```

#### Running Tests
```bash
# Run unit tests
cd build
ctest --output-on-failure
```

#### Python Development
```bash
# Install in development mode
pip install -e .

# Run Python tests
pytest
```

### Community and Support

- **Documentation**: Comprehensive guides and tutorials
- **Code Examples**: Real-world usage examples
- **Performance Profiles**: Optimization strategies and presets
- **Integration Examples**: Python bindings and agent usage

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
