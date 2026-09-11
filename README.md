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

**PHASE 6 INTEGRATION ACHIEVEMENTS**:

### Memory Systems
- Working memory with persistent activity and competition
- Episodic memory with experience encoding and replay
- Associative memory with Hebbian pattern associations
- All memory systems connected to neural processing

### Neuromodulation Integration
- Dopamine affects neural excitability and plasticity
- Curiosity drives exploration behavior
- Novelty detection integrated with sensory processing
- All neuromodulators connected to plasticity rules

### Prediction System
- Prediction system integrated into brain loop
- Prediction error signals affect learning
- Confidence tracking implemented

### Cognition Systems
- Neural planner with action sequence evaluation
- Concept formation from experience patterns
- Attention with competitive selection dynamics
- All cognition systems connected to perception and action

### Development Integration
- Developmental stages affect plasticity rates
- Structural plasticity modulated by age
- Neural excitability changes with development

### Persistence
- Checkpoint save/load implemented
- Brain state serialization working
- Can resume from saved checkpoints

### Replay and Consolidation
- Episodic memory replay during simulation
- Memory consolidation for important episodes
- Integration with sleep/rest cycle

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
# Linux/macOS with CMake 3.16+
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Or with a specific Python installation
cmake .. -DPYTHON_EXECUTABLE=/path/to/python
make

# Windows (if using MSVC)
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

## Running

### Phase 6 Demo (Integration Test)
```bash
# Run the comprehensive integration test
./nlm_phase6_demo
```

This runs a comprehensive integration test verifying all Phase 6 systems are connected.

### Python Bindings

```bash
# Install Python dependencies
pip install -e .

# Test the Python bindings
python -c "import pynlm; print(pynlm.__version__)"
```

### Python Usage Example

```python
import pynlm

# Create a brain with Phase 6 capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simulation
for step in range(1000):
    brain.step(step)
    
# Access integrated memory systems
brain.getWorkingMemory()
brain.getEpisodicMemory()
brain.getAssociativeMemory()

# Access neuromodulation
brain.getDopamine()
brain.getCuriosity()
brain.getNovelty()

# Access cognition systems
brain.getPlanner()
brain.getConceptFormation()
brain.getAttention()

print("Phase 6 brain simulation complete!")
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
│   ├── test_*.cpp      # Unit tests
│   └── test_main.cpp   # Test runner
└── configs/
    └── *.cfg          # Configuration files
```

## Key Features

### Phase 6 Integration
- **Complete Brain Architecture**: All components (neural, memory, cognition, neuromodulation) working together
- **Real Spiking Neural Computation**: LIF neurons with event-driven spike propagation
- **Multiple Plasticity Mechanisms**: STDP, Hebbian, structural plasticity
- **Developmental Trajectory**: Brain matures through distinct stages
- **Memory Systems**: Working, episodic, and associative memory
- **Prediction & Planning**: Forward models and action planning
- **Neuromodulation**: Dopamine, curiosity, novelty detection
- **Integrated Cognition**: Attention, concept formation, planning

### Technical Achievements
- **Performance**: SIMD vectorization, memory pools, parallel processing
- **Persistence**: Checkpoint save/load with compression
- **Replay & Consolidation**: Episodic memory replay during rest
- **Extensibility**: Modular design allows easy addition of new brain regions

### Scientific Properties
- **Emergent Behavior**: Complex cognition from simple neural interactions
- **Self-Organization**: Brain develops its own structure and function
- **Adaptive Learning**: Changes through experience and interaction
- **Developmental Psychology**: Imitates critical periods and maturation

## Building With Python Bindings

The project includes Python bindings via pybind11 that expose the full C++ API:

- **Simple Interface**: Easy-to-use Python API matching C++ functionality
- **Performance**: Direct access to optimized C++ implementation
- **Complete Coverage**: All Phase 6 systems exposed to Python
- **Example Scripts**: Demonstrates integration with world simulation and agents

## Testing

Run all unit tests:

```bash
# Build and run tests
./build/nlm_test

# Or using ctest
mkdir build
cd build
cmake ..
make
ctest --output-on-failure
```

Individual tests available:
- `test_types.cpp`: Basic type tests
- `test_config.cpp`: Configuration system
- `test_neuron.cpp`: LIF neuron dynamics
- `test_synapse.cpp`: Synaptic transmission
- `test_stdp.cpp`: Spike-timing dependent plasticity
- `test_brain.cpp`: Brain integration tests
- `test_clock.cpp`: Timing system

## Phase-Specific Executables

- `nlm`: Main simulation executable
- `nlm_phase3_demo`: Phase 3 development test
- `nlm_phase4_demo`: Phase 4 cognitive integration
- `nlm_phase6_demo`: Phase 6 complete integration test

## Scientific References

This project investigates computational brain-like systems through:
- Computational neuroscience models
- Machine learning with spiking neural networks
- Artificial developmental systems
- Emergent cognitive architectures

No claims are made of biological accuracy, consciousness, or human-like intelligence. The focus is on understanding how neural systems can learn, remember, and adapt through experience.

## License

MIT

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
