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
# Standard demo
./nlm_phase6_demo

# With command line options for customization
./nlm_phase6_demo --steps 3000 --neurons 1000 --verbose
./nlm_phase6_demo --quick
./nlm_phase6_demo --help
```

This runs a comprehensive integration test verifying all brain systems are connected.

### Phase 3 Demo
```bash
./nlm_phase3_demo [num_episodes] [steps_per_episode]
```

### Phase 4 Demo
```bash
./nlm_phase4_demo [num_trials]
```

### Python Interface
```bash
pip install pynlm
```

## Python API Examples

### Simple Brain Creation (Beginner)
```python
import pynlm

# Create a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for 100 steps
for i in range(100):
    brain.step(i)

print(f"Brain thought {brain.getTotalSpikeCount()} times!")
```

### Complete Agent with World (Intermediate)
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Advanced Configuration (Advanced)
```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("enable_structural_plasticity", True)
config.set("enable_novelty", True)

# Validate configuration
if not pynlm.validateConfig(config):
    raise ValueError("Invalid configuration")

# Create brain with advanced config
brain = pynlm.createBrain(config)
brain.initialize()

# Run development experiment
brain = pynlm.runQuickDemo()
```

## Configuration Presets

### Beginner Configuration
```python
config = pynlm.createDefaultConfig()
config.setBeginnerDefaults()
```

### Advanced Configuration
```python
config = pynlm.createDefaultConfig()
config.setAdvancedDefaults()
```

### Default Configuration
```python
config = pynlm.createDefaultConfig()
config.setDefaults()
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── README.md
├── easy_usage.md              # Beginner-friendly usage guide
├── pyproject.toml           # Python package configuration
├── python/bindings.cpp      # Python C++ bindings
├── docs/                   # Comprehensive documentation
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   ├── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
│   ├── PERFORMANCE.md
│   ├── SCALING.md
│   ├── BIOLOGICAL_FIDELITY.md
│   ├── RESEARCH_METHODS.md
│   ├── PHASE6_ARCHITECTURE.md
│   ├── FINAL_LIMITATIONS.md
│   └── PHASE6_FINAL_RESULTS.md
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components (integrated)
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators (integrated)
│   ├── memory/        # Memory systems (integrated)
│   ├── prediction/     # Prediction systems (integrated)
│   ├── cognition/      # Cognitive mechanisms (integrated)
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/              # Unit tests
└── configs/            # Configuration files
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

## Getting Help

- **Beginner**: Start with `easy_usage.md` and Python examples
- **Advanced**: Read `docs/ARCHITECTURE.md` and `docs/SCIENCE.md`
- **Issues**: Report bugs on GitHub
- **Experiments**: Explore `src/experiments/` for more examples

## Quick Start Checklist

1. **Install**: Follow the building instructions above
2. **Run Demo**: `./nlm_phase6_demo` to see the integrated brain in action
3. **Python**: Install `pip install pynlm` for Python access
4. **Learn**: Start with Python examples in this README
5. **Experiment**: Try different configuration options
6. **Understand**: Read documentation for deeper understanding

That's it! You're now ready to use NLM.
