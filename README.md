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

## Running

### Basic Usage

The NLM project now supports multiple demonstration modes with a unified command-line interface:

```bash
# Run Phase 2 neural computation demo (default)
nlm

# Run Phase 3 world interaction demo
nlm --phase3-demo

# Run Phase 6 integration demo
nlm --phase6-demo
```

### Command-Line Options

```bash
nlm [OPTIONS] [MODE]

MODES:
    --phase2-demo    Run Phase 2 neural computation demo (default)
    --phase3-demo    Run Phase 3 world interaction demo
    --phase6-demo    Run Phase 6 integration demo

OPTIONS:
    --help, -h       Show this help message
    --config <file>  Load configuration from file (default: configs/default.cfg)
    --episodes <n>   Number of episodes for demo modes (default: 5)
    --steps <n>      Number of steps per episode (default: 200)

EXAMPLES:
    nlm                          # Run Phase 2 demo with default settings
    nlm --phase3-demo --config custom.cfg
    nlm --phase6-demo --episodes 3 --steps 500
```

### Configuration

Configuration files use a simple key=value format:

```ini
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
synaptogenesis_rate = 0.001
pruning_rate = 0.0001
```

You can override configuration values using command-line arguments:

```bash
nlm --random_seed=123 --neuron_count=2000 --stdp_ltp_weight=0.02
```

### Phase-Specific Features

#### Phase 2: Real Neural Computation

Phase 2 implements the core neural computation engine:

- **Real LIF Neuron Dynamics**: Leaky Integrate-and-Fire neurons with realistic parameters
- **Event-Driven Spike Propagation**: Spikes propagate through synapses with configurable delays
- **STDP and Hebbian Plasticity**: Spike-Timing Dependent Plasticity and Hebbian learning rules
- **Structural Plasticity**: Synaptogenesis (formation) and pruning (elimination) of connections

The Phase 2 demo demonstrates:
1. Basic neural connectivity and spike propagation
2. Plasticity learning experiments showing measurable synaptic changes
3. STDP verification showing pre-before-post spike timing effects

#### Phase 3: World Interaction

Phase 3 extends the neural core with world interaction capabilities:

- **Sensory Processing**: Vision and audio input processing
- **Motor Control**: Action selection and execution
- **Reward Systems**: Dopamine-based reward prediction error
- **Curiosity and Novelty**: Exploration and discovery behavior
- **Agent-Brain Interface**: Integration between neural activity and behavior

The Phase 3 demo shows:
- An agent navigating a 2D grid world
- Learning through trial-and-error
- Developing sensory-motor coordination
- Exhibiting goal-directed behavior

#### Phase 6: Final Integration

Phase 6 represents the final integration of all systems into a coherent artificial brain:

- **Memory Systems**: Working memory, episodic memory, and associative memory integrated with neural processing
- **Neuromodulation**: Dopamine, curiosity, novelty detection, and prediction error signals
- **Prediction Systems**: Forward models and prediction error signals
- **Cognition Systems**: Neural planning, concept formation, and attention
- **Development**: Developmental stages affecting plasticity rates and neural maturation
- **Persistence**: Checkpoint save/load and brain state serialization
- **Replay and Consolidation**: Episodic memory replay during rest

### Building and Running

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs
make -j4

# Run Phase 2 demo (default)
./nlm

# Run Phase 3 demo
./nlm --phase3-demo

# Run Phase 6 demo
./nlm --phase6-demo
```

### Troubleshooting

#### Common Issues

1. **File not found errors**
   - Ensure configuration files exist in the `configs/` directory
   - Check file permissions

2. **Memory issues**
   - For large simulations, use a machine with sufficient RAM
   - Reduce `neuron_count` in configuration

3. **Compilation errors**
   - Ensure you have a C++20 compatible compiler
   - Install required dependencies: `build-essential`, `cmake`

#### Getting Help

For more detailed information about the scientific background and architecture, see the `docs/` directory:

- `docs/ARCHITECTURE.md`: Technical architecture documentation
- `docs/SCIENCE.md`: Scientific background and theory
- `docs/ROADMAP.md`: Development roadmap and future plans
- `docs/PHASE6_FINAL_AUDIT.md`: Phase 6 integration audit report

For technical support, questions, or contributing, please refer to the project documentation.

### License

MIT License. See LICENSE file for details.

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

## Authors

Research project — See docs for scientific background.