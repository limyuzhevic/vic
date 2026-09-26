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

## Quick Start (Python)

NLM provides a comprehensive Python API (`pynlm`) for simulating advanced artificial brains with Phase 6 integration:

```python
import pynlm

# Create an advanced integrated brain
brain = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(brain)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
```

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain. Previous phases built individual components; Phase 6 ensures they work together as a unified system.

### Phase 6 Integration Achievements

#### **Enhanced Memory Systems**
- **Working Memory**: Persistent activity with competitive selection
- **Episodic Memory**: Experience encoding and replay mechanisms
- **Associative Memory**: Hebbian pattern associations and recall
- **Integrated Access**: All memory systems connected to neural processing

#### **Advanced Neuromodulation**
- **Dopamine**: Reward prediction error and reinforcement learning
- **Curiosity**: Exploration motivation and intrinsic rewards
- **Novelty**: Experience detection and surprise signals
- **Prediction Error**: Bayesian inference and model updating

#### **Cognition Systems**
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Abstract pattern discovery and categorization
- **Attentional Selection**: Competitive selection for focus
- **Integrated Processing**: All cognition connected to perception and action

#### **Development Integration**
- **Developmental Stages**: Age-dependent plasticity modulation
- **Structural Plasticity**: Synaptogenesis and pruning
- **Neural Excitability**: Developmental changes in firing properties
- **Dynamic Adaptation**: Continuous development throughout simulation

#### **Persistence and Learning**
- **Checkpoint System**: Save/load brain state with compression
- **Experience Replay**: Episodic memory replay for consolidation
- **Memory Consolidation**: Weak memory elimination, strong memory strengthening
- **Sleep/Rest Cycle**: Offline learning and consolidation

## Enhanced Python API (`pynlm`)

The NLM Python bindings provide comprehensive access to all Phase 6 systems:

### Core Classes

#### **Brain**
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
brain.receiveSensoryInput(input)
action = brain.produceAction()
```

#### **AgentBrain**
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
```

#### **SimpleWorld**
```python
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)
world.update(timestep)
percept = world.getSensoryPercept()
action = world.applyMotorCommand(command, time)
```

#### **Memory Systems**
```python
# Working Memory
brain.enableWorkingMemory(True)
working_memory = brain.getWorkingMemory()
working_memory.setCapacity(100)

# Episodic Memory
brain.enableEpisodicMemory(True)
episodic_memory = brain.getEpisodicMemory()
episodic_memory.setMaxEpisodes(1000)

# Associative Memory
brain.enableAssociativeMemory(True)
associative_memory = brain.getAssociativeMemory()
```

#### **Neuromodulation Systems**
```python
# Dopamine (reward processing)
brain.enableDopamine(True)
dopamine = brain.getDopamine()
dopamine.setLevel(reward)

# Curiosity (exploration)
brain.enableCuriosity(True)
curiosity = brain.getCuriosity()
curiosity.setLevel(0.5)

# Novelty (surprise detection)
brain.enableNovelty(True)
novelty = brain.getNovelty()

# Prediction Error
brain.enablePredictionError(True)
prediction_error = brain.getPredictionErrorSignal()
```

#### **Cognition Systems**
```python
# Neural Planner
brain.enablePlanner(True)
planner = brain.getPlanner()
planner.setPlanningDepth(5)

# Concept Formation
brain.enableConceptFormation(True)
concept_formation = brain.getConceptFormation()

# Attention
brain.enableAttention(True)
attention = brain.getAttention()
attention.setInhibitionStrength(0.5)
```

### Advanced Configuration

NLM provides enhanced configuration with schema validation and runtime adjustment:

```python
# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Set parameters with validation
config.set("neuron_count", 3000)
config.set("planning_depth", 8)
config.set("attention_inhibition_strength", 0.7)

# Runtime configuration
config.enableRuntimeAdjustment(True)
config.set("learning_rate", 0.05)  # Runtime change

# Configuration validation
if config.safeSet("curiosity_threshold", 0.3):
    print("Configuration validated successfully")
else:
    print("Configuration validation failed")
```

### Example Applications

#### **Multi-Agent Collaboration**
```python
# Create multiple collaborating agents
agents = []
for i in range(5):
    brain = pynlm.createAdvancedConfig()
    agent = pynlm.createAgentBrain(brain)
    agent.enableCollaboration(True)  # Share information with other agents
    agents.append(agent)

# Run collaborative task
results = demo.create_multi_agent_collaboration(agents, world)
```

#### **Advanced Learning Experiment**
```python
# Create learning environment
brain = pynlm.createAdvancedConfig()
brain.set("enable_structural_plasticity", True)

agent = pynlm.createAgentBrain(brain)
agent.enableAllLearningSystems(True)

# Run learning experiment
learning_results = demo.run_learning_experiment(agent, world, "memory_learning")
```

#### **Spatial Navigation Training**
```python
# Create navigation environment
world = pynlm.createSimpleWorld()
world.configure(width=100, height=100)

agent = pynlm.createAgentBrain(brain)
agent.enableNavigationLearning(True)

# Train navigation
nav_results = demo.run_navigation_training(agent, world)
```

## Installation

### Prerequisites
- Python 3.8 or higher
- CMake 3.16 or higher
- C++ compiler with C++20 support

### Installation Steps

1. **Clone the repository**
```bash
git clone https://github.com/nlm-project/nlm.git
cd nlm
```

2. **Build the C++ libraries**
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

3. **Install Python bindings**
```bash
cd python
pip install -e .
```

4. **Verify installation**
```python
import pynlm
print(pynlm.getVersion())
```

## Getting Started Examples

### Example 1: Simple Brain Simulation
```python
import pynlm

# Create and initialize brain
brain = pynlm.createDefaultConfig()
brain = pynlm.createBrain(brain)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Brain simulation complete: {brain.getTotalSpikeCount()} spikes")
```

### Example 2: Complete Agent Simulation
```python
import pynlm

# Setup brain, world, and agent
brain = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(brain)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent simulation complete!")
```

### Example 3: Advanced Learning Experiment
```python
import pynlm

# Create advanced brain with all systems
brain = pynlm.createAdvancedConfig()
brain.set("enable_structural_plasticity", True)
brain.set("enable_prediction_system", True)

brain = pynlm.createBrain(brain)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableAllLearningSystems(True)

# Run learning experiment
for step in range(1000):
    world.update(0.05)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get learning statistics
stats = brain.getLearningStatistics()
print(f"Learning complete: {stats['total_reward']:.2f} reward")
```

## Building

### C++ Development

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Phase 6 Demo

Run the comprehensive integration test:

```bash
cd build
./nlm_phase6_demo
```

## Project Structure

```
NLM/
├── CMakeLists.txt                    # C++ build system
├── pyproject.toml                    # Python project configuration
├── README.md                          # This documentation
├── easy_usage.md                      # Beginner guide
├── advanced_agent_examples.py         # Advanced example applications
├── docs/                              # Documentation
│   ├── ARCHITECTURE.md                # Architecture overview
│   ├── SCIENCE.md                     # Scientific background
│   ├── ROADMAP.md                     # Development roadmap
│   ├── EXPERIMENTS.md                  # Experimental protocols
│   ├── LIMITATIONS.md                  # Current limitations
│   └── PHASE6_FINAL_AUDIT.md          # Phase 6 integration audit
├── src/                              # C++ source code
│   ├── core/                         # Core utilities
│   ├── brain/                        # Neural components
│   ├── memory/                       # Memory systems
│   ├── neuromodulation/               # Neuromodulators
│   ├── prediction/                    # Prediction systems
│   ├── cognition/                     # Cognitive mechanisms
│   ├── ...                           # Other components
├── python/                           # Python bindings
│   ├── bindings_enhanced.cpp          # Enhanced Python bindings
│   ├── CMakeLists.txt                # Python bindings build
│   └── ...                           # Other Python files
├── tests/                            # Test suite
│   ├── test_*.cpp                    # C++ unit tests
│   └── test_*.py                     # Python tests
└── configs/                          # Configuration files
    ├── default.cfg                   # Default configuration
    └── ...                           # Other configurations
```

## Advanced Features

### 1. **Enhanced Configuration System**
- Schema validation with type checking
- Runtime configuration adjustment
- Comprehensive error messages and validation
- Configuration export/import (JSON/YAML)

### 2. **Advanced Memory Systems**
- **Working Memory**: Persistent activity with competitive selection
- **Episodic Memory**: Experience replay and consolidation
- **Associative Memory**: Pattern associations and recall
- **Integration**: Direct connections to neural processing

### 3. **Neuromodulation Integration**
- **Dopamine**: Reward prediction error processing
- **Curiosity**: Intrinsic motivation and exploration
- **Novelty**: Surprise detection and adaptation
- **Prediction Error**: Bayesian model updating

### 4. **Cognition Systems**
- **Neural Planning**: Hierarchical action planning
- **Concept Formation**: Abstract pattern discovery
- **Attentional Selection**: Competitive selection dynamics
- **Integrated Processing**: Cross-system coordination

### 5. **Development and Plasticity**
- **Developmental Stages**: Age-dependent plasticity
- **Structural Plasticity**: Synapse formation and elimination
- **Activity-Dependent Plasticity**: STDP and Hebbian learning
- **Experience-Dependent Plasticity**: Reinforcement learning

### 6. **Performance Optimizations**
- **Memory Pools**: Efficient memory allocation
- **Event Queues**: Spike event processing
- **Parallel Processing**: Multi-threaded simulation
- **SIMD Vectorization**: Vector operations

### 7. **Persistence and Recovery**
- **Checkpoint System**: Efficient state saving
- **Experience Replay**: Memory consolidation during sleep
- **State Serialization**: Complete brain state export
- **Recovery Mechanisms**: Automatic state restoration

## Configuration Schema

NLM uses a comprehensive configuration schema with validation:

### Basic Parameters
- `neuron_count`: Number of neurons (default: 1000)
- `region_count`: Number of brain regions (default: 1)
- `connection_probability`: Synaptic connection probability (default: 0.1)
- `simulation_timestep`: Simulation time step (default: 0.001)

### Memory System Parameters
- `working_memory_capacity`: Working memory capacity (default: 100)
- `episodic_memory_max_episodes`: Maximum episodic memories (default: 1000)
- `associative_memory_pattern_count`: Pattern association count (default: 500)

### Neuromodulation Parameters
- `dopamine_sensitivity`: Dopamine system sensitivity (default: 0.5)
- `curiosity_decay`: Curiosity decay rate (default: 0.99)
- `novelty_threshold`: Novelty detection threshold (default: 0.1)

### Cognitive Parameters
- `planning_depth`: Planning horizon steps (default: 5)
- `attention_inhibition_strength`: Attention inhibition (default: 0.5)
- `attention_excitation_strength`: Attention excitation (default: 1.5)
- `concept_formation_threshold`: Concept formation threshold (default: 0.7)

### Development Parameters
- `plasticity_mod_initial`: Initial plasticity modifier (default: 1.0)
- `plasticity_mod_adult`: Adult plasticity modifier (default: 0.2)
- `development_speed`: Development rate (default: 1.0)

## Testing and Validation

### Unit Tests
```bash
# Run C++ unit tests
./build/tests/test_runner

# Run Python tests
pytest tests/
```

### Integration Tests
```bash
# Run Phase 6 integration test
cd build
./nlm_phase6_demo
```

### Performance Benchmarks
```bash
# Run performance benchmarks
./build/benchmark
```

## Troubleshooting

### Common Issues

**Issue: Python bindings not found**
```python
# Check installation
python3 -c "import pynlm; print(pynlm.__version__)"

# Install if missing
pip install pynlm
```

**Issue: Build failures**
```bash
# Check CMake version
cmake --version

# Ensure required dependencies
# - Build-essential
# - cmake
# - g++ (C++20 support)
```

**Issue: Configuration errors**
```python
# Enable debug mode
config = pynlm.createAdvancedConfig()
config.set("debug", True)

# Check configuration validation
validation_result = config.validate()
if not validation_result.is_valid:
    for error in validation_result.errors:
        print(f"Validation error: {error}")
```

## Scientific Limitations

NLM is a research project investigating computational brain-like systems. We make NO claims that NLM accurately reproduces biological brains.

### Current Limitations
- **Neuron Model**: Simplified LIF model (not Hodgkin-Huxley)
- **Ion Channel Dynamics**: No realistic ion channel modeling
- **Dendritic Structure**: No detailed dendritic morphology
- **Cortical Architecture**: No detailed cortical organization
- **Consciousness**: No claims of consciousness or subjective experience
- **Computational Resources**: Limited by available computing power

### What Phase 6 Investigates
- **Memory Integration**: Whether memory systems integrate with neural dynamics
- **Neuromodulation Coordination**: Whether neuromodulation affects plasticity in a coordinated way
- **Prediction as Organizing Principle**: Whether prediction becomes central to brain function
- **Developmental Modulation**: Whether developmental stages modulate learning
- **Memory Replay**: Whether replay reinforces memory consolidation
- **Brain Loop Functionality**: Whether the complete brain loop functions coherently

## Advanced Topics

### 1. **Memory Replay and Consolidation**
```python
# Enable memory replay
brain.enableMemoryReplay(True)

# Configure replay parameters
brain.set("replay_interval", 100)  # Replay every 100 steps
brain.set("consolidation_threshold", 0.8)  # Consolidate high-activation memories
```

### 2. **Structural Plasticity**
```python
# Enable structural plasticity
brain.enableStructuralPlasticity(True)

# Configure plasticity rates
brain.set("synaptogenesis_rate", 0.0001)
brain.set("pruning_rate", 0.00001)
```

### 3. **Neuromodulation Integration**
```python
# Configure neuromodulation
brain.enableDopamine(True)
brain.getDopamine().setSensitivity(0.7)

brain.enableCuriosity(True)
brain.getCuriosity().setThreshold(0.3)
```

### 4. **Prediction System**
```python
# Enable prediction system
brain.enablePredictionSystem(True)

# Configure prediction parameters
brain.set("prediction_confidence", 0.8)
brain.set("prediction_error_weight", 0.5)
```

## License

MIT License. See LICENSE file for details.

## Authors

Research project — See docs for scientific background and author information.

## Acknowledgments

This project is an experimental research initiative investigating computational brain-like systems. We acknowledge the contributions of the open-source community and the inspiration from biological neural systems.

## Getting Help

For questions, issues, or discussions:

1. **GitHub Issues**: Report bugs and feature requests
2. **Documentation**: Check the docs/ directory for comprehensive guides
3. **Examples**: Run advanced_agent_examples.py for demonstration applications
4. **Community**: Join discussions for collaborative development

## Future Development

### Planned Enhancements
1. **Enhanced World Models**: Integration with realistic environments
2. **Advanced Visualization**: Real-time brain visualization tools
3. **Multi-Agent Coordination**: Complex multi-agent system simulations
4. **External Interfaces**: Integration with robotics and control systems
5. **Distributed Computing**: Parallel and distributed simulation capabilities
6. **Machine Learning Integration**: Integration with modern ML frameworks

### Research Directions
1. **Memory Research**: Advanced memory systems and consolidation mechanisms
2. **Neuromodulation Research**: Complex neuromodulator interactions
3. **Development Research**: Developmental trajectories and aging effects
4. **Learning Research**: Novel learning algorithms and plasticity rules
5. **Integration Research**: Cross-system integration and coordination

## Conclusion

NLM represents a significant step forward in computational brain research, providing a comprehensive, experimentally-grounded simulation framework for investigating artificial developmental brains. The Phase 6 integration represents a major milestone in creating a coherent, multi-system artificial brain capable of learning, memory, and adaptive behavior.

The enhanced Python bindings make these advanced capabilities accessible to researchers and developers, enabling rapid prototyping and experimentation with complex brain architectures and behaviors.

---
*Last updated: $(date)*
*Version: $(grep -E "^version" pyproject.toml | cut -d= -f2 | tr -d '")

This project is part of the NLM (熙然) research initiative, investigating computational brain-like systems and artificial developmental brains.
