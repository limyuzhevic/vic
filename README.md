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

## Current status (updated 2026-11-02)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

**Phase 3 and Phase 6 demos now report meaningful rewards and functioning systems.**

**Checkpointing has been fixed and is now fully functional.**

**Learning is more selective rather than pure saturation.**

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
- **Checkpoint save/load now fully functional**
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

Build all targets first (see Building above). The entry points are
now unified under a comprehensive CLI:

```bash
./nlm phase2                           # Run Phase 2 demo
./nlm phase3-demo                     # Run Phase 3 demo
./nlm phase4-demo                     # Run Phase 4 demo
./nlm phase5-demo                     # Run Phase 5 demo (Python bindings)
./nlm phase6-demo                     # Run Phase 6 integration test
./nlm phase6-text-demo                # Run Phase 6 text/thought demo
./nlm benchmark [size]               # Run performance benchmark
./nlm help                           # Show help information
```

The unified CLI provides many more advanced features:

```bash
./nlm config generate --type json      # Generate JSON config
./nlm checkpoint my_checkpoint.bin    # Load and analyze checkpoint
./nlm analyze my_checkpoint.bin       # Analyze neural network
./nlm python-test                     # Test Python bindings
```

## Python Bindings

NLM includes Python bindings (pynlm) for easy integration:

```python
import pynlm

# Create a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Neurons:", brain.getTotalNeuronCount())
print("Spikes:", brain.getTotalSpikeCount())
```

## Configuration Files

NLM supports multiple configuration formats:

### JSON (`.json`)
```json
{
    "neuron_count": 1000,
    "region_count": 4,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 42
}
```

### YAML (`.yaml`, `.yml`)
```yaml
neuron_count: 1000
region_count: 4
connection_probability: 0.15
simulation_timestep: 0.001
random_seed: 42
```

### Simple (`.cfg`, `.txt`)
```
neuron_count = 1000
region_count = 4
connection_probability = 0.15
simulation_timestep = 0.001
random_seed = 42
```

Configuration files can be generated using:
```bash
./nlm config generate --type json
./nlm config generate --type yaml
./nlm config generate --type cfg
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── ARCHITECTURE.md          # Architecture documentation
│   ├── SCIENCE.md              # Scientific background
│   ├── ROADMAP.md              # Development roadmap
│   ├── EXPERIMENTS.md           # Experiment descriptions
│   ├── LIMITATIONS.md           # Known limitations
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit report
├── src/
│   ├── core/           # Core utilities
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world simulation
│   ├── performance/    # Performance optimizations
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/# Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/        # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/    # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
├── tests/              # Test suite
├── python/             # Python bindings (Phase 5)
├── configs/            # Example configuration files
└── docs/               # Documentation
```

## Command Line Interface

### Basic Commands

**Phase Demos:**
- `nlm phase2` - Phase 2: Real Neural Computation
- `nlm phase3-demo` - Phase 3: World Interaction
- `nlm phase4-demo` - Phase 4: Emerging Cognition
- `nlm phase5-demo` - Phase 5: Performance and Python Bindings
- `nlm phase6-demo` - Phase 6: Final Integration
- `nlm phase6-text-demo` - Phase 6: Text/Thought Demo

**Utility Commands:**
- `nlm benchmark [size]` - Performance benchmark (default: 1000 neurons)
- `nlm config generate --type TYPE` - Generate configuration file
- `nlm checkpoint FILE` - Load and analyze checkpoint file
- `nlm analyze FILE` - Analyze neural network state
- `nlm python-test` - Test Python bindings

### Options

Common options for commands:
- `--steps N` - Number of simulation steps (default: 100)
- `--neurons N` - Number of neurons (default: 500)
- `--timestep T` - Simulation timestep (default: 0.001)
- `--seed N` - Random seed (default: 42)
- `--verbose, -v` - Enable verbose output
- `--quiet, -q` - Quiet mode (only essential output)
- `--config FILE` - Load configuration from file
- `--output FILE` - Save results to file

### Examples

**Run Phase 2 with custom settings:**
```bash
nlm phase2 --steps 5000 --neurons 1000 --timestep 0.001 --seed 123
```

**Run benchmark with 2000 neurons:**
```bash
nlm benchmark 2000
```

**Generate JSON configuration:**
```bash
nlm config generate --type json
```

**Analyze checkpoint:**
```bash
nlm checkpoint brain_checkpoint.bin
```

**Analyze neural network state:**
```bash
nlm analyze brain_checkpoint.bin
```

## CLI Reference

### nlm phase2
Run Phase 2: Real Neural Computation demo.

**Arguments:**
- `--steps N` - Number of simulation steps
- `--neurons N` - Number of neurons
- `--timestep T` - Simulation timestep
- `--seed N` - Random seed

### nlm benchmark [size]
Run performance benchmark.

**Arguments:**
- `size` - Number of neurons (default: 1000)

### nlm config generate --type TYPE
Generate configuration files.

**Arguments:**
- `--type TYPE` - Configuration type (json, yaml, cfg)

### nlm checkpoint FILE
Load and display checkpoint information.

**Arguments:**
- `FILE` - Checkpoint file path

### nlm analyze FILE
Analyze neural network state.

**Arguments:**
- `FILE` - Checkpoint file path

### nlm python-test
Test Python bindings.

**Arguments:** None

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

## Version Information

NLM Neural Learning Machine v0.1.0
Phase 6: Final Integration
Built with C++20 and Python bindings

## Getting Help

For more information:
- Read `docs/ARCHITECTURE.md` for technical architecture details
- Read `docs/SCIENCE.md` for scientific background
- Read `HOW_TO_USE.md` for detailed usage instructions
- Check the `examples/` directory for usage examples
- Report issues on the NLM GitHub repository

This project represents an experimental approach to artificial brain construction and is intended for research and educational purposes.