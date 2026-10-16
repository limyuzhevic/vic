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
│   ├── PHASE6_FINAL_AUDIT.md    # Phase 6 audit
│   ├── ENHANCED_PYTHON_API.md    # Enhanced Python API documentation
│   ├── EXAMPLE_SCRIPTS.md         # Example scripts
│   └── COMMUNITY_GUIDE.md        # Community guide
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
├── python/            # Python bindings and examples
│   ├── bindings.cpp   # Enhanced C++ bindings
│   ├── examples/      # Example scripts
│   │   ├── simple_brain_demo.py
│   │   ├── agent_world_demo.py
│   │   ├── configuration_templates_demo.py
│   │   ├── signal_processing_demo.py
│   │   └── advanced_agent_brain_demo.py
│   └── enhanced/      # Enhanced API utilities
│       ├── enhanced_bindings.cpp
│       ├── signal_processor.py
│       └── event_system.py
├── examples/          # Additional examples
│   ├── basic_tutorial.py
│   ├── agent_tutorial.py
│   └── research_example.py
├── tests/
│   ├── test_brain.cpp
│   ├── test_synapse.cpp
│   ├── test_stdp.cpp
│   └── test_agent.cpp
├── examples/          # User examples
│   ├── basic_usage.py
│   ├── agent_example.py
│   └── research_example.py
└── nlm_cli.py        # Command Line Interface for advanced users
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

## Enhanced Python API

The NLM project now includes an enhanced Python API that provides advanced features for researchers and developers:

### Key Enhancements

#### 1. Pythonic Property Access
- **Before**: Verbose method calls (`brain.getType()`, `brain.setType()`)  
- **After**: Property-based access (`brain.type`, `brain.type = ActionType.MoveForward`)

#### 2. Dictionary-Style Configuration
```python
# Create enhanced configuration
config = pynlm.createConfig()
config["brain.neuron_count"] = 2000
config["plasticity.stdp.enable"] = True
config["neuromod.curiosity.enable"] = True

# Validate configuration
if config.validate():
    print("Configuration is valid")
```

#### 3. Context Managers for Resource Management
```python
# Use context manager for automatic cleanup
brain = pynlm.createBrain(config)
with brain.createContext() as ctx:
    # Brain is automatically initialized
    brain.step(100)
    print("Simulation completed in context")
```

#### 4. Advanced Signal Processing
```python
# Signal processing utilities
signal = [0.1, 0.3, 0.5, 0.7, 0.9]
normalized = pynlm.normalize_signal(signal)
smoothed = pynlm.smooth_signal(signal, window_size=3)
peaks = pynlm.find_peaks(signal)
```

#### 5. Event-Driven Architecture
```python
# Event system for simulation hooks
def on_spike(data):
    print(f"Spike event: {data}")

pynlm.add_simulation_hook(on_spike, "spike")
```

#### 6. Machine Learning Integration
```python
# ML integration with NLM
from pynlm import LearningInterface

learner = LearningInterface(brain)
learner.set_reward_model(RandomForestRegressor())
learner.train(features, rewards)
```

### Enhanced API Classes

#### PyConfig (Enhanced Configuration)
- Dictionary-style access: `config["key"] = value`
- Fluent interface: `config.with_neuron_count(1000)`
- Built-in validation
- Configuration templates

#### PyBrain (Enhanced Brain Interface)
- Property-based access: `brain.total_neurons`, `brain.firing_rate`
- Context managers for simulation sessions
- Advanced monitoring and statistics
- Configuration management

#### PyAgentBrain (Enhanced Agent Interface)
- Motor command analysis and bias control
- Curiosity-driven exploration
- Neuromodulation level monitoring
- Event-driven action selection

#### PySimpleWorld (Enhanced World Interface)
- Property-based access: `world.width`, `world.height`
- Advanced object management
- Physics-based simulation
- Event system integration

### Example Usage

#### Basic Enhanced Usage
```python
import pynlm

# Create configuration with Pythonic API
config = pynlm.createConfig()
config["brain.neuron_count"] = 2000
config["plasticity.stdp.enable"] = True

# Create brain with enhanced features
brain = pynlm.createBrain(config)

# Use context manager for automatic cleanup
with brain.createContext() as ctx:
    # Run simulation with monitoring
    stats = brain.runEpisode(1000, config)
    print(f"Episode completed: {stats}")
```

#### Advanced Agent Example
```python
import pynlm

# Create agent with advanced capabilities
brain = pynlm.createBrain()
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(30, 30, 12, 12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning subsystems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Run simulation with events
event_system = AdvancedEventSystem(agent)

for step in range(100):
    world.update(0.1)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = world.getSensoryPercept().getInternal()[0] or 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
```

### Enhanced Features

#### 1. Signal Processing
- Signal normalization and smoothing
- Peak detection and feature extraction
- Pattern recognition and classification
- Signal compression and reconstruction

#### 2. Machine Learning Integration
- Feature extraction from sensory data
- Model training and prediction
- Reward prediction and credit assignment
- Adaptive behavior learning

#### 3. Event System
- Event-driven architecture for simulation hooks
- Priority-based event handling
- Event statistics and monitoring
- Custom event types and handlers

#### 4. Performance Monitoring
- Real-time performance tracking
- Memory and CPU monitoring
- Simulation efficiency analysis
- Resource management

#### 5. State Management
- Advanced state saving and loading
- Checkpoint management
- State validation and recovery
- Serialization utilities

#### 6. Visualization
- Built-in plotting and visualization
- Neural activity monitoring
- Signal processing visualization
- Interactive dashboards

### Command Line Interface

The NLM project includes a command-line interface for advanced users:

```bash
# Basic simulation
nlm basic --steps 1000 --config-type training

# Agent simulation
nlm agent --steps 500 --config-type research --width 30 --height 30

# Experiment management
nlm experiment --type learning_curve --phases 5 --steps-per-phase 200

# Configuration management
nlm config list                    # List templates
nlm config show default            # Show template details

# State management
nlm state save brain_state.json    # Save state
nlm state load brain_state.json    # Load state
```

### Examples and Tutorials

#### Example Scripts
- `examples/basic_usage.py` - Basic NLM usage
- `examples/agent_example.py` - Agent-based simulation
- `examples/research_example.py` - Research experiment setup

#### Demo Scripts
- `examples/simple_brain_demo.py` - Simple brain demonstration
- `examples/agent_world_demo.py` - Agent-world simulation
- `examples/configuration_templates_demo.py` - Configuration templates
- `examples/signal_processing_demo.py` - Signal processing utilities
- `examples/advanced_agent_brain_demo.py` - Advanced agent brain features

### Installation and Setup

#### Python Installation
```bash
# Install enhanced Python API
pip install -e .

# Or install specific components
pip install pynlm
```

#### Additional Dependencies
```bash
# For advanced features
pip install matplotlib numpy scikit-learn pandas

# For performance monitoring
pip install psutil

# For visualization
pip install plotly seaborn
```

#### Build Instructions
```bash
# Build C++ components
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
pip install -e .
```

### Migration Guide

#### From Original API to Enhanced API

**Configuration API Changes:**
```python
# Original
config.set("brain.neuron_count", 1000)
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count")

# Enhanced
config["brain.neuron_count"] = 1000
if config.has("brain.neuron_count"):
    value = config.get("brain.neuron_count", default=0)
```

**Brain API Changes:**
```python
# Original
neurons = brain.getTotalNeuronCount()
spikes = brain.getTotalSpikeCount()

# Enhanced
neurons = brain.total_neurons
spikes = brain.total_spikes
```

**Agent API Changes:**
```python
# Original
action = agent.decodeMotorCommand()

# Enhanced
action = agent.decode_motor_command()
```

### Backward Compatibility

The enhanced API maintains backward compatibility with the original NLM API. All existing code will continue to work without modifications:

```python
# This still works with enhanced API
import pynlm

brain = pynlm.createBrain()
brain.initialize()

for step in range(100):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} neurons")
```

### Technical Support

#### Documentation
- [Enhanced Python API Documentation](docs/ENHANCED_PYTHON_API.md)
- [ARCHITECTURE.md](docs/ARCHITECTURE.md)
- [SCIENCE.md](docs/SCIENCE.md)

#### Examples
- [Example Scripts](examples/) - All demo scripts with detailed explanations
- [Tutorial Guide](docs/EXAMPLE_SCRIPTS.md) - Step-by-step tutorials

#### Community
- [GitHub Repository](https://github.com/nlm-project/nlm) - Source code and issues
- [Documentation](https://nlm.readthedocs.io) - Comprehensive documentation
- [Forum](https://forum.nlm-project.org) - Community discussions

#### Support
- **Bug Reports**: Submit issues to GitHub
- **Feature Requests**: Request new features through GitHub
- **Technical Support**: Contact the NLM team

### Changelog

#### Version 2.0.0-Enhanced
- **Released**: January 16, 2026
- **Major Features**:
  - Enhanced Python API with Pythonic property access
  - Custom exception types with detailed error messages
  - Context managers for automatic resource cleanup
  - Advanced signal processing utilities
  - Machine learning integration framework
  - Event-driven architecture
  - Configuration validation and templates
  - Memory management and optimization tools
  - Command line interface for advanced users
  - Comprehensive documentation and examples
  - Performance monitoring and debugging tools

#### Version 1.0 (Original)
- **Released**: [Date]
- **Original API**: Basic Python bindings for NLM

### Known Issues and Limitations

#### Python API Limitations
- Some advanced features require additional dependencies
- Performance may vary based on system resources
- Certain features are experimental and may change

#### Technical Requirements
- Python 3.8+ required
- C++20 compiler for C++ components
- Sufficient memory for complex simulations
- Network access for optional dependencies

### Future Enhancements

The NLM project has several planned enhancements:

#### 1. GPU Acceleration
- CUDA support for GPU-accelerated simulations
- Parallel processing for large-scale neural networks

#### 2. Distributed Computing
- Multi-agent simulation support
- Distributed computing for large experiments
- Cloud-based simulation platforms

#### 3. Advanced Machine Learning
- Deep learning integration with neural networks
- Transfer learning for behavior modeling
- Reinforcement learning algorithms

#### 4. Visualization and Analysis
- Real-time neural activity visualization
- Advanced plotting and analysis tools
- Interactive dashboards

#### 5. Internationalization
- Support for multiple languages
- Localization for international users
- Cultural adaptation of interfaces

### Acknowledgements

The NLM project is made possible by:
- **Contributors**: All developers who contributed to the project
- **Sponsors**: Organizations that support research and development
- **Users**: Community members who provide feedback and suggestions
- **Open Source**: Libraries and tools that enable advanced functionality

### Contact Information

#### Project Contact
- **Website**: https://nlm.org
- **Email**: contact@nlm.org
- **GitHub**: https://github.com/nlm-project

#### Technical Support
- **Documentation**: https://docs.nlm.org
- **Forum**: https://forum.nlm.org
- **Slack**: Join the NLM community on Slack

#### Community
- **Twitter**: @NLM_project
- **LinkedIn**: NLM Project
- **Discord**: NLM Community Discord server

### License

This project is licensed under the MIT License. See `LICENSE` file for details.
