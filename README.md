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

## Python Interface

NLM provides a comprehensive Python interface through the `pynlm` module that wraps the C++ neural simulation framework. This interface provides a user-friendly API for creating, configuring, and running neural simulations with advanced features for both beginners and experts.

### Installation

#### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the Python package
pip install .

# Or install in development mode (for editing)
pip install -e .
```

#### Using Modern Python Packaging

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Quick Start

```python
import pynlm

# Create a default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation completed with {brain.getTotalNeuronCount()} neurons")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        
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
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return {
        'final_spikes': brain.getTotalSpikeCount(),
        'final_neurons': brain.getFiringNeuronCount(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel()
    }

# Run the simulation
results = run_agent_simulation(1000)
print(f"Simulation complete! Results: {results}")
```

### Advanced Features

The pynlm interface includes several advanced features for experienced users:

#### 1. Experiment Framework
Run complex experiments with configurable parameters and automated data collection.

```python
from pynlm.experiment import LearningExperiment, ExperimentConfig

config = ExperimentConfig("My Experiment")
config.set_brain_config(brain_neuron_count=2000)
config.set_experiment_param('enable_curiosity', True)
config.set_experiment_param('enable_reward', True)

experiment = LearningExperiment("Learning Study", config, num_steps=1000)
result = experiment.run(1000)
```

#### 2. Real-time Monitoring
Monitor brain activity in real-time with data collection and visualization.

```python
from pynlm.monitoring import MonitoringSystem

monitor = MonitoringSystem(brain, agent, world)
monitor.start()

# Simulation loop
for step in range(1000):
    # ... simulation code ...
    time.sleep(0.1)  # Allow monitoring to collect data

monitor.stop()
```

#### 3. Advanced Action Planning
Create sophisticated action sequences and behavior trees for complex decision making.

```python
from pynlm.advanced_actions import create_advanced_behavior_tree

tree = create_advanced_behavior_tree()
context = ActionContext(world, brain, agent, step)
action = tree.update(context)
```

#### 4. Utility Functions
Access utilities for metrics calculation, configuration management, and performance benchmarking.

```python
from pynlm.utils import MetricsCalculator, benchmark_configuration, generate_performance_report

metrics = MetricsCalculator()
benchmark = benchmark_configuration({'neuron_count': 1000}, num_steps=100, num_runs=5)
```

#### 5. Configuration Presets
Use predefined configurations for common scenarios.

```python
from pynlm import get_preset_config, apply_preset

# Get exploration preset
exploration_config = get_preset_config('exploration')

# Apply to your config
config = pynlm.createDefaultConfig()
apply_preset(config, 'performance')
```

### Python Module Structure

The `pynlm` package includes several modules:

- **Core**: `createBrain()`, `createDefaultConfig()`, `createSimpleWorld()`, `createAgentBrain()`
- **Classes**: `Brain`, `Config`, `SimpleWorld`, `AgentBrain`, `Vision`, `Action`, etc.
- **Advanced Actions**: `ActionType`, `ActionPlanner`, `BehaviorTree`, action sequences
- **Monitoring**: `MonitoringSystem`, `VisualizationHelper`, real-time data collection
- **Experiment**: `Experiment`, `LearningExperiment`, `AdaptationExperiment`, experiment management
- **Utils**: `MetricsCalculator`, `SimulationController`, utility functions

### Help and Documentation

Use the built-in help system to get started:

```python
import pynlm

# Quick demo
pynlm.quick_demo()

# Get help for specific topics
pynlm.get_help('general')
pynlm.get_help('agent')
pynlm.get_help('learning')
pynlm.get_help('advanced')
```

### Testing

Run the test suite to verify the Python bindings:

```bash
# Run Python tests
python -m pytest tests/test_pynlm.py -v

# Run specific test categories
python -m pytest tests/test_pynlm.py -k "brain or agent or world"
```

## Project Structure

```
NLM/
├── CMakeLists.txt
├── pyproject.toml                   # Python package configuration
├── setup.py                         # Python package setup
├── README.md
├── easy_usage.md                    # Beginner-friendly usage guide
├── HOW_TO_USE.md                     # Comprehensive usage documentation
├── docs/
│   ├── ARCHITECTURE.md
│   ├── SCIENCE.md
│   ├── ROADMAP.md
│   ├── EXPERIMENTS.md
│   ├── LIMITATIONS.md
│   └── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
├── python/                         # Python bindings
│   ├── bindings.cpp                 # Core C++ bindings for pybind11
│   └── CMakeLists.txt              # Python bindings build configuration
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
│   ├── experiments/   # Experiment framework (Phase 6)
│   └── visualization/  # Visualization
├── tests/
│   ├── test_*.cpp                  # C++ unit tests
│   └── test_pynlm.py               # Python bindings tests
└── configs/
├── pynlm/                         # Python interface modules
│   ├── __init__.py                # Main Python interface
│   ├── utils.py                   # Utility functions and helpers
│   ├── advanced_actions.py        # Advanced action planning and control
│   ├── monitoring.py              # Real-time monitoring and visualization
│   └── experiment.py              # Experiment framework
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
