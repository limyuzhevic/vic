# NLM — 熙然

**An Experimental Artificial Developmental Brain**

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**NLM IS NOT:**
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

**NLM IS intended to become:**
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## What Makes NLM Special

NLM implements a **biologically-inspired spiking neural network** using the Leaky Integrate-and-Fire (LIF) model with real neural dynamics, plasticity mechanisms, and developmental processes. Unlike traditional deep learning models, NLM:

- Simulates actual neurons that fire action potentials
- Implements synaptic plasticity (STDP, Hebbian learning)
- Includes neuromodulation systems (dopamine, curiosity)
- Features developmental stages that affect learning rates
- Supports episodic memory and prediction systems
- Implements structural plasticity for connection growth

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

## Quick Start for Python Users

### Installation

```bash
# Install from source
pip install pybind11 scikit-build-core pytest numpy
pip install -e .  # or pip install .

# Build the C++ extension
pip install .
```

### Basic Usage Examples

```python
# Import the NLM Python bindings
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Configure brain parameters (optional)
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
```

### Advanced Example: Complete Agent Simulation

```python
import pynlm

def run_advanced_simulation(num_steps=1000):
    """Run a complete agent simulation with all features enabled."""
    
    # 1. Create configuration with custom settings
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    config.set("neuromod.curiosity.enable", True)
    
    # 2. Create and initialize the brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setAgentStart(15.0, 15.0)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 5. Run simulation
    print(f"Starting advanced simulation for {num_steps} steps...")
    
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation if available
        if percept.getInternal().size() > 0:
            reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    # Get simulation summary
    summary = pynlm.get_simulation_summary(brain, world)
    print("\n" + summary)
    
    return brain, world, agent

# Run the advanced simulation
brain, world, agent = run_advanced_simulation(500)
```

### Machine Learning Style Example: Training an Agent

```python
import pynlm
import numpy as np

def train_agent(episode_count, steps_per_episode, exploration_rate=0.1):
    """Train an agent using NLM with exploration-exploitation balance."""
    
    # Initialize agent components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    episode_rewards = []
    curiosity_levels = []
    
    for episode in range(episode_count):
        episode_reward = 0.0
        world.reset()
        
        for step in range(steps_per_episode):
            # Determine action: exploration vs exploitation
            if np.random.random() < exploration_rate:
                # Random exploration action
                action = pynlm.Action(pynlm.ActionType.Custom)
                # Add small random parameters
                params = [np.random.uniform(-1.0, 1.0) for _ in range(2)]
                action.setParameters(params)
            else:
                # Normal action selection
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward and learn
            if percept.getInternal().size() > 0:
                reward = percept.getInternal()[0]
                predicted_reward = agent.getPredictionError()
                agent.applyRewardModulation(reward, predicted_reward)
                episode_reward += reward
        
        # Track metrics
        episode_rewards.append(episode_reward)
        curiosity_levels.append(agent.getCuriosityLevel())
        
        # Decay exploration rate
        exploration_rate *= 0.995
        
        if episode % 10 == 0:
            avg_reward = np.mean(episode_rewards[-10:])
            print(f"Episode {episode}: Avg Reward = {avg_reward:.3f}, "
                  f"Curiosity = {agent.getCuriosityLevel():.3f}, "
                  f"Exploration Rate = {exploration_rate:.3f}")
    
    return {
        'episode_rewards': episode_rewards,
        'curiosity_levels': curiosity_levels,
        'final_brain': brain,
        'final_world': world,
        'final_agent': agent
    }

# Train the agent
results = train_agent(episode_count=100, steps_per_episode=200)
```

## Building from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Advanced Features

### 1. Custom Configuration

```python
import pynlm
import json

# Create and save a custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.01)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### 2. Brain State Management

```python
import pynlm
import time

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save brain state
save_time = f"brain_checkpoint_{int(time.time())}"
brain.save(save_time)

# Continue simulation
for i in range(100, 200):
    brain.step(i)

# Load brain state back
brain.load(save_time)
print(f"Loaded brain state with {brain.getTotalSpikeCount()} spikes")
```

### 3. Multi-Agent Systems

```python
import pynlm

def create_multi_agent_system(num_agents=3):
    """Create a system with multiple interacting agents."""
    
    # Create shared brain template
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    agents = []
    brains = []
    worlds = []
    
    for i in range(num_agents):
        # Create individual brain for each agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        brains.append(brain)
        
        # Create shared world
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        world.setAgentStart(i * 10 + 5, i * 10 + 5)
        world.reset()
        worlds.append(world)
        
        # Create agent brain interface
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
    
    return {
        'agents': agents,
        'brains': brains,
        'worlds': worlds
    }

# Create multi-agent system
multi_agent_system = create_multi_agent_system(3)

# Run simulation where agents can interact
for step in range(300):
    for i, (agent, brain, world) in enumerate(zip(
        multi_agent_system['agents'],
        multi_agent_system['brains'],
        multi_agent_system['worlds']
    )):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Agents can detect each other
        if percept.getInternal().size() > 1:
            # Check for other agents in proximity
            for j, other_world in enumerate(multi_agent_system['worlds']):
                if i != j:
                    # Simple proximity detection
                    dx = agent.getBrain().getTotalNeuronCount() - other_world.getAgentBody().x
                    dy = agent.getBrain().getTotalNeuronCount() - other_world.getAgentBody().y
                    distance = (dx*dx + dy*dy)**0.5
                    if distance < 5.0:
                        print(f"Agent {i} detected agent {j} at distance {distance:.1f}")
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
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
├── python/                  # Python bindings
│   ├── bindings.cpp
│   └── CMakeLists.txt
├── src/                     # C++ source code
│   ├── core/              # Core utilities
│   ├── brain/             # Neural components (integrated)
│   ├── dynamics/          # Neural dynamics
│   ├── plasticity/        # Plasticity rules
│   ├── development/       # Developmental system
│   ├── neuromodulation/   # Neuromodulators (integrated)
│   ├── memory/            # Memory systems (integrated)
│   ├── prediction/        # Prediction systems (integrated)
│   ├── cognition/         # Cognitive mechanisms (integrated)
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Motor system
│   ├── environment/       # Environment interface
│   ├── experiments/       # Experiment framework (Phase 6)
│   └── visualization/     # Visualization
├── tests/                  # Unit tests
│   ├── test_brain.cpp
│   ├── test_config.cpp
│   ├── test_neuron.cpp
│   └── ... (more tests)
├── configs/                # Configuration files
└── docs/                   # Documentation
```

## Scientific Background

NLM is based on several key scientific principles:

1. **Leaky Integrate-and-Fire (LIF) Neurons**: Simplified but biophysically-grounded neuron model
2. **Spike-Timing-Dependent Plasticity (STDP)**: Activity-dependent synaptic strengthening and weakening
3. **Hebbian Learning**: "Neurons that fire together, wire together"
4. **Neuromodulation**: Dopamine, serotonin, and other systems that regulate learning and behavior
5. **Developmental Stages**: Age-dependent changes in plasticity and learning rates
6. **Predictive Coding**: Brain as a hierarchical prediction engine
7. **Episodic Memory**: Storage and replay of experience sequences

## Research Applications

NLM is designed for research in:
- Artificial developmental systems
- Neuromodulated learning
- Predictive processing
- Memory consolidation
- Curiosity-driven exploration
- Emergent cognition
- Autonomous agent design

## License

MIT

## Authors

Research project — See docs for scientific background.

## Support and Contact

For questions, technical support, or research collaboration:
- Visit: https://github.com/nlm-project/nlm
- Check documentation in the docs/ directory
- Review Phase 6 audit in docs/PHASE6_FINAL_AUDIT.md

## Future Development

Phase 6 represents a major integration milestone. Future phases will focus on:
- More complex cognitive architectures
- Social learning and interaction
- Advanced memory systems
- Improved developmental trajectories
- Multi-modal perception
- Real-world applications