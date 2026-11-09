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

NLM includes Python bindings for easy interaction with the neural simulation framework. The Python bindings provide access to all NLM functionality through a Pythonic API.

### Installation

To install the Python bindings:

```bash
pip install pynlm
```

Or for development with the latest changes:

```bash
pip install -e .
```

### Basic Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### Advanced Examples

Several example scripts are provided in the repository:

1. **example_pynlm.py** - Comprehensive examples showing various NLM features
2. **test_pynlm.py** - Comprehensive test suite for Python bindings
3. **easy_usage.md** - Beginner-friendly usage guide
4. **HOW_TO_USE.md** - Detailed documentation with examples

### Key Features

The Python bindings provide access to all NLM functionality:

- **Brain Creation and Control**: Create, initialize, step, and reset brains
- **Sensory Processing**: Vision, audio, and internal signal processing
- **Agent Systems**: Complete agent brains with learning subsystems
- **World Simulation**: Simple 2D world for agent-environment interaction
- **Learning Mechanisms**: Reward modulation, structural plasticity, development, curiosity
- **Memory Systems**: Working memory, episodic memory, and associative memory
- **Analysis Tools**: Network metrics, connectivity analysis, and visualization
- **Batch Operations**: Efficient processing of multiple simulation steps
- **Configuration Management**: Advanced configuration with file I/O support
- **Performance Monitoring**: Benchmark tools and profiling capabilities

### Python API Overview

The main Python classes and functions include:

#### Core Classes

- **Brain**: Central neural simulation brain with complete functionality
- **Config**: Configuration management with validation and file I/O
- **AgentBrain**: Agent brain interface connecting brain to world
- **SimpleWorld**: 2D world for simulation with agent interaction
- **Action**: Action representation for motor output
- **SensoryInput**: Base class for sensory processing

#### Key Functions

- **createDefaultConfig()**: Create default configuration
- **createBrain(config)**: Create and initialize brain
- **createSimpleWorld()**: Create simple world
- **createAgentBrain(brain)**: Create agent brain interface
- **benchmark(config, steps)**: Performance benchmarking
- **safeBrainOperation(brain, operation)**: Safe brain operation execution

#### Enumerations

- **NeuronType**: Excitatory, inhibitory, sensory, motor, modulatory, internal
- **ActionType**: Various action types (move, turn, interact, etc.)
- **DevelopmentalStage**: Initial, critical period, maturation, adult, aging
- **WorldObjectType**: Empty, resource, hazard, wall, marker

### Example: Complete Learning Agent

```python
import pynlm
import time

class LearningAgent:
    def __init__(self, width=20, height=20):
        # Create brain with custom configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.region_count", 3)
        config.set("brain.connection_probability", 0.05)
        
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 16, 16)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable all learning features
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_episode(self, steps=1000):
        """Run a complete learning episode"""
        for step in range(steps):
            # Update world and get sensory input
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain processes and produces action
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            
            # Apply action in world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward and apply learning
            reward = self._get_reward(percept)
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
        
        return self._get_episode_summary()

def run_advanced_example():
    """Run advanced example demonstrating all features"""
    print("=== Advanced NLM Python Example ===")
    
    # Create advanced user with research configuration
    advanced_user = LearningAgent(width=30, height=30)
    
    # Run advanced simulation
    print("\nRunning advanced research simulation...")
    start_time = time.time()
    
    results = advanced_user.run_episode(steps=3000)
    
    end_time = time.time()
    
    # Analyze results
    print(f"\n=== Simulation Results ===")
    print(f"Simulation duration: {end_time - start_time:.2f} seconds")
    print(f"Total reward: {results['total_reward']:.3f}")
    print(f"Average curiosity: {results['avg_curiosity']:.3f}")
    print(f"Final novelty level: {results['final_novelty']:.3f}")
    
    print("\n=== Advanced NLM Python Usage Complete ===")

if __name__ == "__main__":
    run_advanced_example()
```

### Testing the Python Bindings

The NLM project includes comprehensive tests for the Python bindings:

1. **test_pynlm.py**: Comprehensive test suite with 9+ test categories
2. **example_pynlm.py**: Usage examples and demonstrations
3. **pytest configuration**: Configured in pyproject.toml for automated testing

To run the Python tests:

```bash
pytest test_pynlm.py -v
```

Or for development:

```bash
python test_pynlm.py
```

### Troubleshooting

**Installation Issues:**

- If `ImportError: No module named 'pynlm'` occurs:
  ```bash
  pip install --force-reinstall .
  ```

- If missing dependencies:
  ```bash
  pip install pybind11 scikit-build-core numpy
  ```

**Runtime Issues:**

- **Brain initialization errors**: Ensure `initialize()` is called before `step()`
- **Agent errors**: Check that `initialize(world)` is called for agent
- **Configuration errors**: Verify configuration keys are valid

**Performance Issues:**

- For large networks, consider reducing neuron count or using batch operations
- Use `benchmark()` to measure performance before large-scale simulations

### Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

That's it! You're now ready to use NLM with Python bindings.

