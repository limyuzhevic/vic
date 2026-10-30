# NLM — 熙然

**An Experimental Artificial Developmental Brain**

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

**PHASE 6: FINAL INTEGRATION (Current Status)**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. All previous phases have been completed and their components are now integrated into this final phase.

### Phase 6 Integration Achievements

#### Memory Systems
- ✅ Working memory with persistent activity and competition
- ✅ Episodic memory with experience encoding and replay
- ✅ Associative memory with Hebbian pattern associations
- ✅ All memory systems connected to neural processing

#### Neuromodulation Integration
- ✅ Dopamine affects neural excitability and plasticity
- ✅ Curiosity drives exploration behavior
- ✅ Novelty detection integrated with sensory processing
- ✅ All neuromodulators connected to plasticity rules

#### Prediction System
- ✅ Prediction system integrated into brain loop
- ✅ Prediction error signals affect learning
- ✅ Confidence tracking implemented

#### Cognition Systems
- ✅ Neural planner with action sequence evaluation
- ✅ Concept formation from experience patterns
- ✅ Attention with competitive selection dynamics
- ✅ All cognition systems connected to perception and action

#### Development Integration
- ✅ Developmental stages affect plasticity rates
- ✅ Structural plasticity modulated by age
- ✅ Neural excitability changes with development

#### Persistence
- ✅ Checkpoint save/load implemented
- ✅ Brain state serialization working
- ✅ Can resume from saved checkpoints

#### Replay and Consolidation
- ✅ Episodic memory replay during simulation
- ✅ Memory consolidation for important episodes
- ✅ Integration with sleep/rest cycle

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

### Phase 4 Demo (Cognitive Abilities)
```bash
./nlm_phase4_demo [num_trials]
```

This demo shows emerging cognitive abilities like prediction, memory, and planning.

### Phase 3 Demo (World Interaction)
```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

This demo shows world interaction and action learning.

### Main Executable (Phase 2 Research)
```bash
./nlm [OPTIONS]
```

Advanced research executable with real neural computation and detailed testing.

## Command Line Options

### NLM Phase 6 Demo
```bash
./nlm_phase6_demo [OPTIONS]

Options:
  --neurons <count>        Number of neurons (default: 500)
  --steps <count>          Maximum simulation steps (default: 2000)
  --checkpoint <path>      Checkpoint file path (default: ./checkpoint.bin)
  --no-checkpoint          Disable checkpoint save/load
  --no-replay              Disable replay functionality
  --no-development         Disable developmental processes
  --verbose                Enable verbose logging
  --config <file>          Configuration file (default: configs/default.cfg)
  --seed <value>           Random seed for reproducibility (default: 42)
  --help, -h               Show help information
  --version                Show version information
```

### NLM Main Executable
```bash
./nlm [OPTIONS]

Options:
  --config <file>          Configuration file (default: configs/default.cfg)
  --neurons <count>        Number of neurons (default: 500)
  --steps <count>          Maximum simulation steps (default: 10000)
  --timestep <value>       Simulation timestep in seconds (default: 0.001)
  --seed <value>           Random seed (default: 42)
  --checkpoint <path>      Checkpoint file path (default: ./checkpoint.bin)
  --no-checkpoint          Disable checkpointing
  --no-replay              Disable replay functionality
  --no-development         Disable developmental processes
  --verbose                Enable verbose logging
  --output <file>          Save results to file
  --warmup <steps>         Number of warmup steps (default: 100)
  --curiosity <value>      Initial curiosity level (default: 0.5)
  --novelty <value>        Novelty detection threshold (default: 0.01)
  --reward <value>         Reward scaling factor (default: 1.0)
  --explore <value>        Exploration rate (default: 0.1)
  --help                   Show help information
  --version                Show version information
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── src/                    # Source code
│   ├── core/               # Core utilities
│   ├── brain/              # Neural components (integrated)
│   ├── dynamics/           # Neural dynamics
│   ├── plasticity/         # Plasticity rules
│   ├── development/        # Developmental system
│   ├── neuromodulation/     # Neuromodulators (integrated)
│   ├── memory/             # Memory systems (integrated)
│   ├── prediction/         # Prediction systems (integrated)
│   ├── cognition/          # Cognitive mechanisms (integrated)
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework (Phase 6)
│   └── visualization/      # Visualization
├── tests/                 # Test suite
└── configs/              # Configuration files
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

## Additional Resources

### Documentation

- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage guide with Python API
- [easy_usage.md](easy_usage.md) - Quick start guide for beginners
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [docs/SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [docs/EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/ROADMAP.md](docs/ROADMAP.md) - Project roadmap
- [docs/LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 final audit

### Python Bindings

NLM includes Python bindings through pybind11:

```bash
# Install with scikit-build-core (modern Python packaging)
pip install .

# Or install in development mode
pip install -e .

# Build only (without installing)
pip install build
python -m build
```

### Usage Examples

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Getting Help

- **For beginners**: Read `easy_usage.md` first
- **For detailed usage**: Read `HOW_TO_USE.md`
- **For technical questions**: Check `docs/ARCHITECTURE.md`
- **For issues**: Check the project repository

## Performance Notes

### Compilation

NLM is optimized for performance:
- Uses C++20 standard
- SIMD vectorization where possible
- Memory pools and event queues
- Parallel processing for large simulations

### Memory Usage

Memory usage scales with:
- Number of neurons (configurable via `--neurons`)
- Network connectivity (based on connection probability)
- Active working memory traces
- Stored episodic memories

### Performance Monitoring

The Phase 6 demo includes basic performance monitoring:
- Wall clock timing
- Firing rate statistics
- Memory usage tracking
- Integration status reporting

For detailed profiling, use:
- Linux `perf` tool
- GDB for debugging
- Custom logging configuration

## Conclusion

NLM represents a comprehensive effort to create an artificial developmental brain from first principles. By implementing all phases and integrating them in Phase 6, we have created a functioning neural system that:

- Learns from experience through multiple plasticity mechanisms
- Stores and recalls episodic memories
- Uses prediction to guide behavior
- Develops over time through age-dependent processes
- Exhibits goal-directed behavior through neuromodulation

The project continues to evolve, and feedback from users is always welcome to help improve the system and documentation.
