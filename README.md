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

### Key Integration Achievements

- ✅ **Memory Systems**: Working memory, episodic memory, and associative memory all connected to neural processing
- ✅ **Neuromodulation**: Dopamine, curiosity, novelty detection integrated with plasticity
- ✅ **Prediction**: System integrated with learning and error signals
- ✅ **Cognition**: Neural planning, concept formation, and attention connected to perception and action
- ✅ **Development**: Plasticity modulated by developmental stage
- ✅ **Persistence**: Checkpoint save/load and replay functional

## Building

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Integration Test
```bash
./nlm_phase6_demo
```

### Basic Neural Computation
```bash
./nlm
```

### Performance Benchmark
```bash
./nlm_benchmark
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── HOW_TO_USE.md              # Technical documentation (NEW!)
├── easy_usage.md             # Beginner-friendly guide
├── docs/                     # Architecture and scientific documentation
├── src/                      # Source code
│   ├── core/                # Core utilities
│   ├── brain/               # Neural components (integrated)
│   ├── dynamics/            # Neural dynamics
│   ├── plasticity/          # Plasticity rules
│   ├── development/         # Developmental system
│   ├── neuromodulation/      # Neuromodulators (integrated)
│   ├── memory/              # Memory systems (integrated)
│   ├── prediction/           # Prediction systems (integrated)
│   ├── cognition/           # Cognitive mechanisms (integrated)
│   ├── sensory/            # Sensory processing
│   ├── motor/              # Motor system
│   ├── environment/        # Environment interface
│   ├── experiments/        # Experiment framework (Phase 6)
│   └── visualization/       # Visualization
├── tests/                   # Unit tests
└── configs/                 # Configuration files
```

## Build Outputs

After compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation (Phase 2) |
| `nlm_benchmark` | Executable | Performance benchmark |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_phase6_demo` | Executable | Phase 6 integration test |
| `nlm_test` | Executable | Unit test suite |

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
- Neural attention
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

## License

MIT

## Authors

Research project — See docs for scientific background.
