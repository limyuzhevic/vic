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

## Current status (honest, 2026-11-12)

Phase 1 made the repo build again. Phase 2 made the brain real at the
foundation level: populations now actually create neurons, random
connectivity creates ~37k synapses for a 500-neuron brain (was 0),
spikes propagate (Test 1: ~8k spikes, Test 2: ~199k spikes over 1000
steps), and a controlled STDP test potentiates its synapse
(0.35 -> 0.99). Inhibitory neurons now exist (20% of internal
population) and inhibitory plasticity keeps its sign instead of
flipping to excitatory.

Phase 3 and Phase 4 demos now show positive reward learning, and
checkpointing is fully functional. Activity has been tuned to be more
manageable (~80 spikes/step) and learning in the main demo is now
selective pattern learning. Phase 5 is complete with performance optimizations
and personality traits. Phase 6 integration test passes.

The main issues mentioned in the previous status have been addressed:

- ✅ Checkpointing now works (no longer a stub)
- ✅ Learning is selective pattern learning (not just saturation)
- ✅ Activity is more manageable (~80 spikes/step vs ~200 spikes/step)
- ✅ Phase 3 and Phase 6 demos show positive reward

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
./nlm_phase3_demo     # Phase 3 demo (world interaction)
./nlm_phase4_demo     # Phase 4 demo (emerging cognition)
./nlm_phase5_demo     # Phase 5 demo (brain-personality)
./nlm_phase6_demo     # Phase 6 demo (integration test)
./nlm_phase6_text_demo # Phase 6 text/thought demo
./nlm_benchmark      # Performance benchmark
```

### Command-line Options

Most demos accept command-line arguments for customization:

#### Phase 2 Demo (nlm)
```bash
./nlm --help                    # Show usage information
./nlm --config config.cfg       # Specify configuration file
./nlm --test1 100              # Set basic connectivity test steps (default: 50)
./nlm --test2 2000             # Set plasticity experiment steps (default: 1000)
./nlm --test3 100              # Set STDP verification steps (default: 50)
```

#### Phase 3 Demo (nlm_phase3_demo)
```bash
./nlm_phase3_demo 10 200        # Run 10 episodes with 200 steps each
./nlm_phase3_demo 5             # Run 5 episodes with default steps (200)
```

#### Phase 4 Demo (nlm_phase4_demo)
```bash
./nlm_phase4_demo 100          # Run 100 trials
./nlm_phase4_demo              # Run with default trials (50)
```

#### Phase 5 Demo (nlm_phase5_demo)
```bash
./nlm_phase5_demo              # Run pattern probe, sandbox, and checkpoint tests
```

#### Phase 6 Demos
```bash
./nlm_phase6_demo              # Run integration test with all system checks
./nlm_phase6_text_demo          # Run text teaching and thought demonstration
```

#### Testing
```bash
make test                    # Run unit tests
./nlm_test                    # Run all unit tests
./nlm_test --help             # Show test options
./nlm_test --verbose          # Show detailed test output
./nlm_test --benchmark        # Measure test execution time
./nlm_test --types --brain    # Run specific test suites
```

### Python Bindings

The project includes Python bindings (`pynlm`) for the NLM framework. After building the project:

```bash
# Build Python bindings (Phase 5: this is what `pip install .` builds)
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4

# Or use scikit-build-core (modern Python packaging)
pip install pybind11 scikit-build-core
pip install .
```

#### Basic Python Usage

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Save/load checkpoint
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

## Project Structure

```
NLM/
├── src/                    # Core source code
│   ├── core/              # Core utilities
│   ├── agent/             # Agent-brain interface
│   ├── world/             # Simple world
│   ├── performance/       # Checkpoints, pools, queues
│   ├── brain/             # Neural components
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/    # Neuromodulators
│   ├── memory/            # Memory systems
│   ├── prediction/        # Prediction systems
│   ├── cognition/         # Cognitive mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization
│
├── python/                 # Python bindings
│   ├── bindings.cpp        # Pybind11 bindings
│   └── CMakeLists.txt      # Python bindings build
│
├── tests/                  # Unit tests
│   ├── test_main.cpp       # Test runner with CLI
│   ├── test_*.cpp          # Individual test files
│   └── CMakeLists.txt      # Test configuration
│
├── configs/                # Configuration files
│   └── default.cfg         # Default configuration
│
├── docs/                   # Documentation
│   ├── README.md           # This file
│   ├── HOW_TO_USE.md       # Usage guide
│   └── ...                 # More documentation
│
├── easy_usage.md           # Simple usage examples
├── CMakeLists.txt          # Main build configuration
└── pyproject.toml         # Python packaging configuration
```

## Features

### Core Features

#### Real Neural Computation (Phase 2)
- Leaky Integrate-and-Fire (LIF) neuron dynamics
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity rules
- Structural plasticity (synaptogenesis/pruning)
- Real spiking neural computation with ~37k synapses

#### World Interaction (Phase 3)
- Sensory input processing and motor output
- Reward prediction error learning
- Developmental stages affecting plasticity
- Novelty and curiosity-driven exploration

#### Emerging Cognition (Phase 4)
- Neural prediction systems
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

#### Performance Optimizations (Phase 5)
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system
- Improved spike delivery (neuron lookup by ID)

#### Final Integration (Phase 6)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

### Additional Features

#### Python Bindings
- Full Python API with documentation
- Brain, Config, AgentBrain, SimpleWorld interfaces
- Factory functions for creating components
- Comprehensive bindings for all major classes

#### Enhanced Testing
- Command-line test runner with filtering
- Timing measurements for benchmarking
- Verbose output for debugging
- Individual test execution

#### Improved Configuration
- Comprehensive configuration options in default.cfg
- Neuromodulation, reward, learning, development settings
- Checkpoint configuration options
- Environment settings

## Usage Examples

### Example 1 Project (Phase 2) - Basic Neural Learning

This example demonstrates basic neural computation with measurable synaptic changes:

```bash
$ ./nlm --test2 500
# Runs plasticity experiment for 500 steps
# Shows learning through weight changes
```

### Example Project (Phase 4) - Cognitive Development

This example demonstrates concept formation and cognitive abilities:

```bash
$ ./nlm_phase4_demo 100
# Runs 100 trials of temporal prediction, working memory,
# episodic memory, concept formation, attention, planning,
# self-model, and social learning experiments
```

### Example Python Script - Complete Agent

```python
#!/usr/bin/env python3
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure environment
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with environment
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
run_agent_simulation(1000)
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

## Testing

The test suite includes comprehensive unit tests for all major components:

- **Types Tests**: Basic type system validation
- **Config Tests**: Configuration file parsing and validation
- **Random Tests**: Random number generator testing
- **Clock Tests**: Simulation timing and clock functionality
- **Neuron Tests**: LIF neuron dynamics and spike generation
- **Synapse Tests**: Synaptic transmission and transmission
- **STDP Tests**: Spike-timing dependent plasticity rules
- **Brain Tests**: Integration tests for brain functionality
- **Checkpoint Tests**: State save/load and persistence
- **Phase 6 Tests**: Integration verification and system connectivity

All tests can be run with:

```bash
make test                    # Run through CMake test framework
./nlm_test                   # Run standalone test executable
./nlm_test --verbose          # Show detailed test output
./nlm_test --benchmark        # Measure test execution time
./nlm_test --brain --stdp     # Run specific test suites
```

## License

MIT

## Authors

Research project — See docs for scientific background.