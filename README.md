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

### Phase 5 Demo (Continual Learning)
```bash
./nlm_phase5_demo [num_tasks] [cycles_per_task] [steps_per_cycle]
```

Examples:
```bash
./nlm_phase5_demo 3 5 1000    # 3 tasks, 5 cycles each, 1000 steps per cycle
./nlm_phase5_demo 5           # Custom number of tasks
```

### Phase 4 Demo (Cognitive Development)
```bash
./nlm_phase4_demo
```

### Phase 3 Demo (World Interaction)
```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

## Available Demos

| Demo | Description | File |
|------|-------------|------|
| `nlm` | Basic NLM brain simulation | `src/main.cpp` |
| `nlm_phase3_demo` | World interaction and sensorimotor learning | `src/experiments/Phase3Demo.cpp` |
| `nlm_phase4_demo` | Cognitive development and concept formation | `src/experiments/Phase4Demo.cpp` |
| `nlm_phase5_demo` | Continual learning across multiple tasks | `src/experiments/Phase5Demo.cpp` |
| `nlm_phase6_demo` | Final integration test and verification | `src/experiments/Phase6Demo.cpp` |

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
│   ├── cognition/     # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment frameworks
│   │   ├── Experiment.hpp
│   │   ├── Experiment.cpp
│   │   ├── Phase3Experiment.hpp
│   │   ├── Phase3Experiment.cpp
│   │   ├── Phase4Experiment.hpp
│   │   ├── Phase4Experiment.cpp
│   │   ├── Phase5Experiment.hpp
│   │   ├── Phase5Experiment.cpp
│   │   ├── Phase6Experiment.hpp
│   │   ├── Phase6Experiment.cpp
│   │   ├── Phase3Demo.cpp
│   │   ├── Phase4Demo.cpp
│   │   ├── Phase5Demo.cpp
│   │   └── Phase6Demo.cpp
│   └── visualization/  # Visualization
├── tests/
├── configs/
└── easy_usage.md

HOW_TO_USE.md

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
- Continual learning across tasks
- Multi-seed experiment runner

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

## Configuration

NLM uses a flexible configuration system supporting:
- File-based configuration (configs/default.cfg)
- Command-line overrides
- Runtime configuration
- Configuration validation
- Parameter type checking

### Configuration File Example (configs/default.cfg)
```
# NLM Configuration File
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0
environment_name = GridWorld
environment_width = 10
environment_height = 10
log_level = INFO
log_to_file = false
max_simulation_steps = 10000
```

### Command Line Usage
```bash
./nlm_phase6_demo --neurons 500 --steps 2000 --checkpoint /tmp/test.bin
./nlm_phase5_demo 3 5 1000
./nlm_phase3_demo 10 500
```

## Output Formats

The demos produce both human-readable console output and structured data:

- **Console Output**: Detailed logging and progress reports
- **JSON Reports**: Machine-readable data for analysis
- **Configuration Summary**: Full parameter display
- **Integration Status**: System connectivity verification

## Debugging and Troubleshooting

### Common Issues and Solutions

1. **Brain initialization fails**
   - Check configuration parameters
   - Verify system resources
   - Ensure dependencies are built

2. **Performance issues**
   - Reduce neuron count for testing
   - Disable development system
   - Use `--quick` flag for Phase 6

3. **Memory issues**
   - Monitor memory usage
   - Use checkpointing for long runs
   - Consider system resources

4. **Configuration errors**
   - Validate config file syntax
   - Use `loadFromFile` for testing
   - Check type conversions

### Debugging Options

```bash
# Enable verbose logging
--verbose

# Save output to file
--output demo_output.txt

# Run with debugging symbols
make debug

# Use performance profiler
./nlm_phase6_demo --profile
```

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

- **Documentation**: See `docs/` directory for detailed technical information
- **Experiments**: See `experiments/` directory for specific experiment implementations
- **Source Code**: The `src/` directory contains all implementation files
- **Issues**: Report bugs and feature requests in the issue tracker

## Development Notes

This is an active research project. New features are added incrementally through phases. Each phase builds upon previous phases, creating a coherent artificial brain system.

### Research Focus

- Neural integration and system coherence
- Experience-driven learning and development
- Memory consolidation and replay mechanisms
- Neuromodulation and motivation systems
- Prediction and planning capabilities
- Continual learning and adaptation

### Future Directions

- Enhanced visualization tools
- Distributed computing support
- Multi-agent systems
- Embodied cognition extensions
- Social learning capabilities

## Contact

For questions, bug reports, or collaboration opportunities, please refer to the project documentation or issue tracker.
