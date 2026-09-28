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

### New CLI Interface
```bash
# Central NLM command line interface (new in Phase 6)
nlm-cli --help

# Create and initialize a Phase 6 brain
nlm-cli create --neurons 1000 --output brain.bin

# Run Phase 6 integration experiment
nlm-cli experiment phase6 --steps 2000 --neurons 500

# Inspect brain system state
nlm-cli inspect brain.bin --memory --neuromodulation --cognition

# Save/load checkpoints
nlm-cli checkpoint --brain brain.bin --save checkpoint.bin
nlm-cli checkpoint --brain brain.bin --load checkpoint.bin

# List saved sessions
nlm-cli list
```

### Python API (Enhanced for Phase 6)
```python
import pynlm

# Create Phase 6 brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run Phase 6 integration experiment
result = brain.runPhase6Integration(steps=5000)
print(f"Integration successful: {result['integration']}")
print(f"Total reward: {result['totalReward']:.3f}")
print(f"Memory episodes: {result['memoryEpisodesStored']}")

# Save brain state
brain.save("nlm_phase6_brain.bin")

# Create agent and world
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all Phase 6 features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation loop
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Firing rate {brain.getAverageFiringRate():.2f} Hz")
```

### Visualization Tools (New in Phase 6)
```bash
# Neural network visualization
nlm-viz network --brain brain.bin --output network.png

# Memory system visualization
nlm-viz memory --brain brain.bin --episodes 20 --output memory.png

# Plasticity system visualization
nlm-viz plasticity --brain brain.bin --output plasticity.png

# Neuromodulation system visualization
nlm-viz neuromod --brain brain.bin --output neuromod.png

# Time series visualization
nlm-viz time-series --brain brain.bin --metric firing_rate --output time_series.png

# Animation of brain dynamics
nlm-viz animate --brain brain.bin --steps 100 --output animation.gif
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
│   └── PHASE6_FINAL_RESULTS.md    # Phase 6 results and usage
├── nlm_cli.py                      # New CLI interface
├── nlm_viz.py                      # New visualization tools
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

## New in Phase 6

### CLI Interface
- **nlm-cli**: Unified command-line interface for all operations
- **nlm-viz**: Comprehensive visualization tools
- **Enhanced Python API**: Better integration with Phase 6 features

### Documentation
- **Phase6_FINAL_RESULTS.md**: Comprehensive results and usage guide
- **USAGE_GUIDE.md**: Beginner-friendly tutorials
- **ADVANCED_FEATURES.md**: Advanced visualization, profiling, debugging
- **CLI_REFERENCE.md**: Complete command reference

### Scientific Achievements
- **Memory Integration**: Working memory, episodic memory, and associative memory all connected to neural processing
- **Neuromodulation Integration**: Dopamine, curiosity, and novelty systems coordinate plasticity and dynamics
- **Prediction Integration**: Temporal prediction with error computation becomes central organizing principle
- **Development Integration**: Multi-stage development modulates plasticity rates appropriately
- **Complete Brain Loop**: All 15 systems update in unified framework each simulation step

### Performance
- **Optimization**: SIMD vectorization and parallel processing maintained
- **Memory Efficiency**: Memory pools and event queues for efficient simulation
- **Scalability**: Can simulate up to 5000+ neurons on modern hardware

## License

MIT

## Authors

Research project — See docs for scientific background.

## Getting Help

### Quick Start (5 minutes)
```bash
# Create Phase 6 brain
nlm-cli create --neurons 500 --output demo_brain.bin

# Run integration test
nlm-cli experiment phase6 --steps 1000 --neurons 200

# Visualize results
nlm-viz memory --brain demo_brain.bin --output memory_demo.png
```

### Getting Started with Python
```python
# Import the enhanced Python bindings
import pynlm

# Create Phase 6 brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run integration
result = brain.runPhase6Integration(2000)
print("Phase 6 integration successful!")
```

### Advanced Usage
1. Read `docs/PHASE6_FINAL_RESULTS.md` for Phase 6 capabilities
2. Read `docs/ARCHITECTURE.md` for system architecture
3. Read `docs/SCIENCE.md` for scientific background
4. Read `docs/EXPERIMENTS.md` for experiment descriptions
5. Use `nlm-cli --help` for complete CLI documentation
6. Use `nlm-viz --help` for visualization options

### Getting Help
- **GitHub Issues**: Report bugs, request features, ask questions
- **Documentation**: Comprehensive docs in `docs/` directory
- **Examples**: Look at `src/experiments/` for working examples
- **Community**: Join discussions and share findings

## Contributing

NLM is an open research project. Contributions are welcome for:
- Adding new brain systems and integration patterns
- Improving visualization and analysis tools
- Developing new experimental paradigms
- Optimizing performance and scalability
- Enhancing documentation and tutorials
- Implementing new neuromodulation mechanisms

See `docs/ROADMAP.md` for current priorities and upcoming features.