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
# Standard build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j4

# Or use the modern CMake approach
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# For development build with debugging
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
```

## Running

### Phase 6 Demo (Integration Test)
```bash
./build/nlm_phase6_demo
```

### Python Bindings

The Python bindings are built as part of the CMake build:
```bash
# Check Python bindings
python -c "import pynlm"

# Run Python examples
python examples/demo.py
```

### Advanced Commands

NLM provides several advanced commands for power users and researchers:

```bash
# Run specific experiment types
./build/nlm_phase6_demo --experiment_type "reinforcement_learning"
./build/nlm_phase6_demo --experiment_type "memory_consolidation"
./build/nlm_phase6_demo --experiment_type "developmental_pruning"

# Run with custom configuration
./build/nlm_phase6_demo --config=/path/to/custom_config.cfg

# Run performance benchmarking
./build/nlm_benchmark --neurons=10000 --timesteps=100000

# Run neural development simulation
./build/nlm_develop --generations=50 --population_size=100

# Generate synthetic data for analysis
./build/nlm_generate_data --output=synthetic_data.dat --neurons=5000
```

### Python API for Advanced Usage

```python
import pynlm
from pynlm import Brain, Neuron, Synapse

# Advanced brain configuration
brain = Brain({
    'neuron_count': 10000,
    'region_count': 4,
    'connection_probability': 0.15,
    'plasticity_learning_rate': 0.02,
    'dopamine_baseline': 0.2
})

# Custom neural development
brain.development().setSynaptogenesisRate(0.005)
brain.development().setPruningRate(0.0005)

# Memory system interaction
brain.memory().episodic().setMaxEpisodes(5000)
brain.memory().semantic().storeFact("concept", "artificial intelligence")

# Neuromodulation control
brain.neuromodulation().dopamine().setBaseline(0.15)
brain.neuromodulation().curiosity().setExplorationRate(0.3)

# Run simulation with advanced configuration
brain.runSimulation(steps=50000, timestep=0.001)
```

### Experiment Configuration Files

Create custom experiment configurations:

```ini
# experiments/research_config.cfg
[simulation]
neuron_count = 50000
region_count = 8
max_simulation_steps = 1000000
timestep = 0.0005

[plasticity]
learning_rate = 0.005
stdp_ltp_weight = 0.008
stdp_ltd_weight = 0.009

[memory]
working_capacity = 500
episodic_max = 10000
semantic_store = true

[neuromodulation]
dopamine_baseline = 0.25
curiosity_threshold = 0.3
novelty_decay = 0.9

[environment]
width = 20
height = 20
tick_rate = 100
```

Run experiments:
```bash
./build/nlm_phase6_demo --config=experiments/research_config.cfg
```

### Debug and Development Commands

```bash
# Run with visualization
./build/nlm_phase6_demo --visualization_enabled

# Run with logging to file
./build/nlm_phase6_demo --log_to_file --log_filename=debug.log

# Run with debug logging
./build/nlm_phase6_demo --log_level=DEBUG

# Check memory usage during runtime
./build/nlm_phase6_demo --monitor_memory

# Run neural development with verbose output
./build/nlm_develop --generations=10 --verbose=true
```

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
