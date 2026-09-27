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

## Key Improvements in Phase 6

### 1. **Modular Architecture**
- Main executable refactored into modular functions
- Command-line interface for advanced usage
- JSON configuration support with fallback to simple format
- Separated concerns: configuration, brain management, and experiment execution

### 2. **Enhanced Configuration System**
- JSON configuration parser with full support for nested objects, arrays, and all data types
- Automatic format detection based on file extension (.json, .jsonc, .jsonl)
- Backward compatibility with simple key=value format
- Command-line option support for runtime configuration

### 3. **Command-Line Interface**
- `nlm_cmd` advanced command-line tool
- Multiple modes: `run`, `monitor`, `validate`
- Flexible experiment configuration
- JSON output support for programmatic consumption
- Debug and profiling capabilities

### 4. **Improved User Experience**
- Better error messages and help documentation
- Progress logging and metrics reporting
- Checkpoint saving and loading support
- Quiet mode for automated execution

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

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

### Advanced Command-Line Tool
```bash
./nlm_cmd --help
./nlm_cmd --experiment learning --steps 5000 --quiet
./nlm_cmd --mode monitor --experiment stdp --debug
```

### Using JSON Configuration
Create `config.json`:
```json
{
  "brain": {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1
  },
  "plasticity": {
    "stdp_enabled": true,
    "hebbian_enabled": true,
    "structural_enabled": true
  }
}
```

Run with JSON config:
```bash
./nlm_phase6_demo --config config.json
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
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators (integrated)
│   ├── memory/           # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework (Phase 6)
│   └── visualization/     # Visualization
├── tools/                 # Command-line tools
│   ├── nlm_cmd.h
│   └── nlm_cmd.cpp
├── tests/                 # Unit tests
│   ├── test_config.cpp
│   ├── test_json_config.cpp
│   ├── test_main.cpp
│   └── test_cmd.cpp
└── configs/               # Configuration files
    └── default.cfg
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

Phase 6 investigates:
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

## Testing

Run the test suite:
```bash
./test_neuron
./test_json_config
./test_cmd
```

Or use CTest:
```bash
ctest --output-on-failure
```

## Quick Start

1. **Build the project**
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the basic demo**
   ```bash
   ./nlm_phase6_demo
   ```

3. **Try the advanced command-line tool**
   ```bash
   ./nlm_cmd --help
   ./nlm_cmd --experiment learning --steps 1000
   ```

4. **Test the JSON configuration**
   ```bash
   ./nlm_cmd --experiment stdp --no-plasticity
   ```

That's it! You're now ready to use the advanced NLM system with modular architecture, JSON configuration support, and powerful command-line interface.
