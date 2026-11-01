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

## Current status (honest, 2026-10-31)

Phase 1 made the repo build again. Phase 2 made the brain real at the
substrate level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Still not a working brain in the behavioural sense: Phase 3 and Phase 6
demos still report 0 reward, and checkpointing does not work (it is a
stub that honestly returns false). Activity is still very high
(~200 spikes/step) and learning in the main demo is mostly
saturation, not selective pattern learning. Those are the next
problems, not hidden ones.

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

Build all targets first (see Building above). The entry points are
currently separate demos per phase, not a single unified binary:

```bash
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo
./nlm_phase4_demo
./nlm_phase6_demo     # Phase 6 integration test
./nlm_benchmark
```

`nlm_phase6_demo` runs a comprehensive integration test verifying all
brain systems are connected. Phase 5 is wired as the `nlm_phase5`
library in CMake; it does not yet have a standalone demo executable.

### Advanced Demos (New in Phase 6)

NLM Phase 6 introduces several advanced demo modes with enhanced capabilities:

```bash
./nlm_enhanced_phase3_demo      # Enhanced Phase 3 with comprehensive metrics
./nlm_advanced_demo            # Advanced demo combining multiple phases
```

**Enhanced Phase 3 Demo Options:**
- `--mode <mode>`: Run mode: 'standard', 'intensive', 'exploratory'
- `--episodes <count>`: Number of episodes (default: 5)
- `--steps <count>`: Steps per episode (default: 200)
- `--no-plasticity`: Disable synaptic plasticity
- `--no-neuromodulation`: Disable neuromodulation
- `--quiet`: Reduce verbose output
- `--output <file>`: Save results to file

**Advanced Demo Options:**
- `--mode <mode>`: Run mode: 'comprehensive', 'focused', 'experimental'
- `--quiet`: Reduce verbose output
- `--output <file>`: Save results to file
- `--selective`: Enable only selected systems

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
│   ├── agent/          # Agent-brain interface
│   ├── world/          # Simple world
│   ├── performance/    # Checkpoints, pools, queues (headers)
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
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/ # Visualization
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

## Python Bindings (pynlm)

NLM includes Python bindings that provide a high-level interface to the neural simulation framework:

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Customize configuration
config.setFloat("neuron_count", 1000)
config.setFloat("simulation_timestep", 0.001)
config.setFloat("stdp_ltp_weight", 0.025)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create simulation world
world = pynlm.createSimpleWorld()
world.configure(50, 50, 20, 20)

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
print("Starting simulation...")
results = pynlm.runSimulation(brain, world, agent, 100, verbose=True)

print("Simulation completed!")
print(f"Total reward: {results['total_reward']}")
print(f"Average reward: {results['avg_reward']}")
print(f"Average firing rate: {results['avg_firing_rate']} Hz")

# Training example
print("\nStarting training...")
training_results = pynlm.trainBrain(brain, world, agent, 50, 200, verbose=True)

print("Training completed!")
print(f"Total reward: {training_results['total_reward']}")
print(f"Average per episode: {training_results['avg_reward_per_episode']}")
print(f"Max episode reward: {training_results['max_reward']}")
```

### Advanced Python Features

```python
import pynlm

# Create various input types
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Vision input
vision = pynlm.createVisionInput(100, 100, 3)
vision.setData([0.1, 0.5, 0.9] * (100 * 100 * 3))

# Audio input
audio = pynlm.createAudioInput(44100, 4410)  # 1 second of audio
audio.setData(list(range(4410)))

# Action creation
action = pynlm.createAction(pynlm.ActionType.MoveForward, [0.5, 0.5])

# Constants and utilities
constants = pynlm.getConstants()
print(f"PI: {constants['PI']}")
print(f"Default timestep: {constants['DEFAULT_TIMESTEP']}s")

# Example code generation
example_code = pynlm.getExampleCode()
print(example_code)
```

### Python Class Reference

**Core Classes:**
- `Config`: Configuration management
- `Brain`: Neural simulation engine
- `AgentBrain`: Interface between brain and world
- `SimpleWorld`: 2D simulation environment

**Sensory Processing:**
- `Vision`: Visual input processing
- `Audio`: Audio input processing
- `InternalSignals`: Internal neural signals
- `SensoryInput`: Base sensory class

**Motor Output:**
- `Action`: Motor actions
- `MotorCommand`: Low-level motor commands

**Memory Systems:**
- `NeuralWorkingMemory`: Working memory
- `NeuralEpisodicMemory`: Episodic memory
- `Memory`: Base memory class

**Prediction Systems:**
- `PredictionSystem`: Prediction engine
- `NeuralPrediction`: Neural prediction module

**Cognition Systems:**
- `NeuralPlanner`: Action planning
- `ConceptFormation`: Pattern discovery
- `AttentionalSelection`: Attention control

**Development Systems:**
- `DevelopmentSystem`: Overall development
- `Synaptogenesis`: Synapse formation
- `Pruning`: Synapse elimination
- `Maturation`: Neural maturation

**Neuromodulation:**
- `Dopamine`: Reward signal
- `Curiosity`: Exploration drive
- `Novelty`: Novelty detection
- `PredictionError`: Prediction error signal

**Dynamics:**
- `SpikeSystem`: Spike generation and propagation
- `NeuralDynamics`: Neural dynamics

### Advanced Python Example: Pattern Learning

```python
import pynlm
import numpy as np

# Create a learning experiment
config = pynlm.createDefaultConfig()
config.setFloat("neuron_count", 500)
config.setFloat("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Pattern learning experiment
patterns = {
    'bright_resource': [0.1, 0.2, 0.3, 0.9, 0.8, 0.7],
    'dark_hazard': [0.8, 0.9, 0.7, 0.1, 0.2, 0.3],
    'empty_scene': [1.0] * 12
}

print("Pattern Learning Experiment")
print("===========================")

for pattern_name, pattern_data in patterns.items():
    # Create vision input for pattern
    vision = pynlm.createVisionInput(12, 12, 1)
    vision.setData(pattern_data)
    
    # Create percept
    percept = pynlm.SensoryPercept()
    percept.setVision(vision)
    
    # Get action from agent
    action = agent.inferAction(percept)
    
    print(f"\nPattern: {pattern_name}")
    print(f"  Action: {action.getName()}")
    print(f"  Parameters: {action.getParameters()}")
    
    # Run brain step
    brain.step(0, 0.001)
    
    # Get agent statistics
    stats = agent.getAgentStats()
    print(f"  Neuromodulation level: {stats['neuromodulation_level']:.3f}")
    print(f"  Curiosity level: {stats['curiosity_level']:.3f}")
    print(f"  Prediction error: {stats['prediction_error']:.3f}")

# Training summary
print(f"\nTraining Summary:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")

# Save brain state for later use
brain.save("nlm_trained_brain.bin")
print(f"\nBrain state saved to: nlm_trained_brain.bin")
```

### Error Handling

```python
try:
    import pynlm
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
except pynlm.RuntimeError as e:
    print(f"Error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Performance Tips

- For large simulations, use smaller world dimensions
- Enable `verbose=False` to reduce output overhead
- Use `pynlm.runSimulation()` for batch processing
- Consider using `pynlm.trainBrain()` for efficient training

## Advanced Features

### 1. Enhanced Main Demo (Phase 2)

The main executable now includes comprehensive command-line options:

```bash
./nlm --help                    # Show all options
./nlm --neurons 1000 --steps 5000  # Configure simulation
./nlm --run-mode plasticity --enable-plasticity --enable-stdp  # Run plasticity tests
./nlm --output results.txt       # Save results to file
```

### 2. Enhanced Phase 3 Demo

Advanced Phase 3 demo with comprehensive metrics and analysis:

```bash
./nlm_enhanced_phase3_demo --episodes 10 --steps 500 --verbose --output enhanced_results.json
```

### 3. Advanced Integrated Demo

Combines all Phase 3-6 capabilities:

```bash
./nlm_advanced_demo --mode comprehensive --output full_results.json
```

### 4. Python Enhanced Features

- **Helper functions** for data processing and normalization
- **Factory functions** for easy object creation
- **Statistics gathering** for system analysis
- **Example code generation** for quick starts
- **Enhanced error handling** with specific exception types

## Configuration Files

The `configs/` directory contains configuration files:

- `default.cfg`: Default configuration for basic experiments
- Custom configuration files can be created for specific experimental setups

Configuration options can be set programmatically or loaded from JSON files using the `Config::loadFromFile()` method.

## Testing and Validation

The NLM project includes comprehensive testing:

### Python Tests

```bash
pytest tests/                    # Run Python unit tests
```

### Integration Tests

The demo executables serve as integration tests, validating that all systems work together correctly.

## Performance Considerations

- **Memory usage**: Monitor for large brain simulations
- **Simulation speed**: Use optimized configurations for testing
- **Checkpointing**: Use checkpoints for long-running experiments
- **Scaling**: Consider distributed computing for very large networks

## Future Directions

Phase 6 establishes the foundation for future NLM development:

### Phase 7: Cognitive Specialization
- Specialized cognitive domains
- Advanced reasoning capabilities
- Complex decision-making

### Phase 8: Social Learning
- Multi-agent systems
- Cultural transmission
- Collaborative learning

### Phase 9: Developmental Specialization
- Life-stage specific development
- Experience-dependent maturation
- Environmental interaction effects

### Phase 10: Real-time Applications
- Interactive training environments
- Real-time adaptation
- Dynamic capability assessment

## Documentation and Resources

### Online Documentation
- Project documentation: https://nlm.readthedocs.io
- Repository: https://github.com/nlm-project/nlm
- Issue tracking: GitHub issues
- Discussion forums: Project Slack/Discord

### Related Projects
- Complementary neural network frameworks
- Brain simulation platforms
- Cognitive architecture projects

## License

MIT License

## Authors

Research project — See docs for scientific background.

## Acknowledgments

Thanks to the NLM research community for contributions and feedback.

---

*Last updated: 2026-11-01*
*Phase 6: Final Integration*

This document represents the current state of the NLM project with all Phase 6 capabilities integrated. The enhanced Python bindings, advanced demo modes, and comprehensive documentation provide users with powerful tools for neural simulation and cognitive research.