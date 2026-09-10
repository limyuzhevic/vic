# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

---

## Part 1: Building the C++ Project from Source

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

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Using the Python Library

### Quick Start Guide

#### Beginner's Tutorial

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create brain and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation
print("Starting simulation...")
for step in range(100):
    brain.step(step)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

#### Agent in World

```python
import pynlm

# Setup brain and environment
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run complete simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(0.1, 0.0)
    agent.updateDevelopment(0.1)

print(f"Simulation finished! Development stage: {brain.getDevelopmentalStage()}")
```

### Core Concepts

#### 1. Brain Initialization

```python
import pynlm

# Always create a config first
config = pynlm.createDefaultConfig()

# Custom configuration
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.synapse_density", 0.08)        # More connections
config.set("neuromod.dopamine.scale", 1.5)       # Stronger reward signals

# Create brain
brain = pynlm.createBrain(config)

# Initialize BEFORE any operations
brain.initialize()
```

#### 2. Simulation Loop

```python
import pynlm

def run_simulation(brain, world, agent, steps=1000):
    """Standard NLM simulation loop"""
    for step in range(steps):
        # 1. Update environment
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process input
        agent.processSensoryInput(percept)
        
        # 4. Brain computation
        brain.step(step)
        
        # 5. Get action
        action = agent.decodeMotorCommand()
        
        # 6. Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Optional: Enable learning
        agent.applyRewardModulation(0.1, 0.0)
        agent.updateDevelopment(0.1)
        
        # 7. Monitor (optional)
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons active")
    
    return brain, agent
```

#### 3. Development Tracking

```python
import pynlm

# Create brain and track development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Development stages and milestones
for age in range(0, 100, 10):  # Every 10 developmental units
    agent.updateDevelopment(10.0)  # Age by 10 units
    
    stage = brain.getDevelopmentalStage()
    neuromod = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    
    print(f"Age {age}:")
    print(f"  Developmental stage: {stage}")
    print(f"  Neuromodulation level: {neurom:.3f}")
    print(f"  Curiosity level: {curiosity:.3f}")
    print(f"  Neurons: {brain.getTotalNeuronCount()}, Firing: {brain.getFiringNeuronCount()}")
    print()
```

### Key Classes and Methods

#### Brain Class

```python
import pynlm

# Basic operations
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Step execution
brain.step(0)                    # One step
brain.step(step_num, time)      # Step with timestamp

# State management
brain.reset()                   # Reset to initial state
brain.save("checkpoint.bin")    # Save to file
brain.load("checkpoint.bin")    # Load from file

# Input/Output
brain.receiveSensoryInput(percept)  # Inject data
action = brain.produceAction()       # Get motor output

# Statistics (useful for monitoring)
stats = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'average_firing_rate': brain.getAverageFiringRate(),
    'total_spikes': brain.getTotalSpikeCount(),
    'development_stage': brain.getDevelopmentalStage()
}

# Regions support
region_id = brain.addRegion("visual_cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Query parameters
print(f"Neuron count: {config.getOr('brain.neuron_count', 1000)}")
print(f"Synapse density: {config.getOr('brain.synapse_density', 0.1)}")

# Modify parameters
config.set("brain.neuron_count", 3000)
config.set("plasticity.stdp.learning_rate", 0.005)

# File operations (JSON supported now!)
config.saveToFile("my_config.json")
config.loadFromFile("my_config.json")

# Command-line argument loading
config.loadFromArgs(2, ["--brain.neuron_count=2000", "--plasticity.stdp.learning_rate=0.01"])
```

#### AgentBrain Class

```python
import pynlm

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Core functionality
agent.initialize(world)                                   # Connect to world
agent.processSensoryInput(world.getSensoryPercept())      # Receive input
action = agent.decodeMotorCommand()                       # Get action
agent.applyRewardModulation(reward, predicted)           # Learn from rewards

# Development
agent.updateDevelopment(0.1)                              # Age by 0.1 developmental units
stage = agent.getDevelopmentalStage()                    # Get current stage

# Subsystems (enable/disable)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Subsystem status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty detection: {agent.getNoveltyLevel():.3f}")
```

#### SimpleWorld Class

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=50,
    height=50,
    visionWidth=10,
    visionHeight=10
)

# Control flow
world.reset()                    # Reset to start
world.update(0.1)               # Advance time
world.setRandomSeed(42)          # Reproducible results

# Agent interaction
world.setAgentStart(25.0, 25.0)  # Set starting position
world.applyMotorCommand(action, time)  # Apply action

# State access
percept = world.getSensoryPercept()  # Get sensory input
body = world.getAgentBody()        # Get agent state

# World objects
world.addObject(pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Simulation time: {world.getSimulationTime()}")
print(f"Max energy: {world.getMaxEnergy()}")
```

### Advanced Usage Examples

#### 1. Learning Rate Optimization

```python
import pynlm
import numpy as np

class LearningRateOptimizer:
    """Systematic optimization of learning parameters"""
    
    def __init__(self):
        self.results = []
    
    def optimize_stdp_rate(self):
        """Find optimal STDP learning rate"""
        learning_rates = [0.0001, 0.0005, 0.001, 0.005, 0.01]
        curiosity_enabled = True
        structural_enabled = True
        
        print("Optimizing STDP learning rate...")
        
        for lr in learning_rates:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            config.set("plasticity.stdp.learning_rate", lr)
            config.set("neuromod.curiosity.enable", curiosity_enabled)
            config.set("plasticity.structural.enable", structural_enabled)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(curiosity_enabled)
            agent.enableStructuralPlasticity(structural_enabled)
            
            # Run simulation
            for step in range(200):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                agent.applyRewardModulation(0.1, 0.0)
                agent.updateDevelopment(0.1)
            
            result = {
                'learning_rate': lr,
                'final_curiosity': agent.getCuriosityLevel(),
                'firing_neurons': brain.getFiringNeuronCount()
            }
            self.results.append(result)
            
            print(f"  LR={lr:.4f}: Final curiosity={result['final_curiosity']:.3f}, "
                  f"Firing={result['firing_neurons']}")
        
        return self.results
    
    def find_optimal_config(self):
        """Find the best configuration from results"""
        if not self.results:
            print("No results to analyze. Run optimize_stdp_rate() first.")
            return
        
        best = max(self.results, key=lambda x: x['final_curiosity'])
        print(f"\nOptimal configuration found:")
        print(f"  STDP learning rate: {best['learning_rate']}")
        print(f"  Final curiosity level: {best['final_curiosity']:.3f}")
        print(f"  Neurons firing: {best['firing_neurons']}")

# Usage
optimizer = LearningRateOptimizer()
results = optimizer.optimize_stdp_rate()
optimizer.find_optimal_config()
```

#### 2. Multi-Agent Environment

```python
import pynlm
import numpy as np

class MultiAgentEnvironment:
    """World with multiple competing/cooperating agents"""
    
    def __init__(self, width=100, height=100, num_agents=3):
        self.width = width
        self.height = height
        self.num_agents = num_agents
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, 12, 12)
        self.world.setMaxEnergy(200.0)
        
        self.agents = []
        self.base_config = pynlm.createDefaultConfig()
        
        # Create agents with different strategies
        for i in range(num_agents):
            agent_config = self._create_agent_strategy(i)
            brain = pynlm.createBrain(agent_config)
            brain.initialize()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(self.world)
            self.agents.append(agent)
    
    def _create_agent_strategy(self, agent_id):
        """Different strategies for different agents"""
        config = self.base_config
        
        if agent_id == 0:
            # Conservative: fewer neurons, careful exploration
            config.set("brain.neuron_count", 800)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.curiosity.exploration_rate", 0.3)
        elif agent_id == 1:
            # Aggressive: more neurons, high exploration
            config.set("brain.neuron_count", 2000)
            config.set("neuromod.curiosity.enable", True)
            config.set("neuromod.curiosity.exploration_rate", 0.8)
        else:
            # Balanced: medium neurons, standard curiosity
            config.set("brain.neuron_count", 1500)
            config.set("neuromod.curiosity.enable", True)
        
        return config
    
    def set_agent_positions(self, positions):
        """Set starting positions for all agents"""
        for i, (x, y) in enumerate(positions):
            if i < len(self.agents):
                self.world.setAgentStart(x, y)
    
    def run_coordination_test(self, steps=300):
        """Test how agents interact with each other"""
        print(f"Running coordination test with {self.num_agents} agents")
        
        # Set diverse starting positions
        positions = [(10, 10), (50, 10), (90, 10), (10, 50), (50, 50), (90, 50)]
        self.set_agent_positions(positions[:self.num_agents])
        
        for step in range(steps):
            self.world.update(0.1)
            
            # Each agent observes the same world but acts independently
            for agent_idx, agent in enumerate(self.agents):
                percept = self.world.getSensoryPercept()
                
                # Agent-specific behavior
                if agent_idx == 0:
                    # Conservative: wait for clear opportunities
                    agent.processSensoryInput(percept)
                elif agent_idx == 1:
                    # Aggressive: explore everywhere
                    agent.processSensoryInput(percept)
                else:
                    # Balanced: standard processing
                    agent.processSensoryInput(percept)
                
                # Common learning
                agent.applyRewardModulation(0.1, 0.0)
                agent.updateDevelopment(0.1)
                
                # Action selection
                action = agent.decodeMotorCommand()
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Reset for next step with new positions
            self.world.reset()
            self.set_agent_positions([(10, 10), (50, 10), (90, 10)])
            
            if step % 50 == 0:
                self._print_step_stats(step)
        
        return self.agents
    
    def _print_step_stats(self, step):
        """Print coordination statistics"""
        total_neurons = sum(agent.getBrain().getTotalNeuronCount() for agent in self.agents)
        total_spikes = sum(agent.getBrain().getTotalSpikeCount() for agent in self.agents)
        curiosity_levels = [agent.getCuriosityLevel() for agent in self.agents]
        
        print(f"Step {step}:")
        print(f"  Total neurons: {total_neurons}")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Curiosity levels: {[f'{c:.2f}' for c in curiosity_levels]}")
        print(f"  Average curiosity: {np.mean(curiosity_levels):.3f}")

# Usage
multi_agent_env = MultiAgentEnvironment(width=150, height=100, num_agents=3)
agents = multi_agent_env.run_coordination_test(steps=200)
```

#### 3. Checkpoint and Resume System

```python
import pynlm
import os
import json
from datetime import datetime

class NMCheckpointManager:
    """Advanced checkpoint system for NLM simulations"""
    
    def __init__(self, checkpoint_dir="nlm_checkpoints"):
        self.checkpoint_dir = checkpoint_dir
        os.makedirs(checkpoint_dir, exist_ok=True)
        self.session_id = f"nlm_session_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
    
    def save_progress(self, brain, agent, world, experiment_name, step, metadata=None):
        """Save comprehensive simulation state"""
        checkpoint = {
            'session_id': self.session_id,
            'timestamp': datetime.now().isoformat(),
            'experiment_name': experiment_name,
            'step': step,
            'config': self._extract_config(brain.getConfig()),
            'brain_state': self._capture_brain_state(brain),
            'agent_state': self._capture_agent_state(agent),
            'world_state': self._capture_world_state(world),
            'metadata': metadata or {}
        }
        
        # Save as JSON for readability
        json_path = os.path.join(self.checkpoint_dir, 
                                f"{experiment_name}_{self.session_id}_step_{step:06d}.json")
        with open(json_path, 'w') as f:
            json.dump(checkpoint, f, indent=2)
        
        # Also save as binary for performance (simplified)
        binary_path = json_path.replace('.json', '.nb')
        
        print(f"Checkpoint saved: {json_path}")
        print(f"  Session: {self.session_id}")
        print(f"  Step: {step}")
        print(f"  Config keys: {len(checkpoint['config'])}")
        
        return json_path
    
    def load_progress(self, checkpoint_path):
        """Load simulation state from checkpoint"""
        with open(checkpoint_path, 'r') as f:
            checkpoint = json.load(f)
        
        # Recreate configuration
        config = pynlm.createDefaultConfig()
        self._restore_config(config, checkpoint['config'])
        
        # Create brain and restore state
        brain = pynlm.createBrain(config)
        brain.initialize()
        self._restore_brain_state(brain, checkpoint['brain_state'])
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        self._restore_agent_state(agent, checkpoint['agent_state'])
        
        print(f"Checkpoint loaded: {checkpoint_path}")
        print(f"  Experiment: {checkpoint['experiment_name']}")
        print(f"  Session: {checkpoint['session_id']}")
        print(f"  Step: {checkpoint['step']}")
        print(f"  Metadata: {checkpoint.get('metadata', {})}")
        
        return brain, agent, world
    
    def list_available_checkpoints(self, experiment_name=None):
        """List all available checkpoints"""
        checkpoints = []
        
        for filename in os.listdir(self.checkpoint_dir):
            if experiment_name is None or experiment_name in filename:
                filepath = os.path.join(self.checkpoint_dir, filename)
                if filename.endswith('.json'):
                    with open(filepath, 'r') as f:
                        data = json.load(f)
                    
                    checkpoints.append({
                        'path': filepath,
                        'filename': filename,
                        'session_id': data.get('session_id', 'unknown'),
                        'timestamp': data.get('timestamp', ''),
                        'step': data.get('step', 0),
                        'metadata': data.get('metadata', {})
                    })
        
        # Sort by timestamp
        checkpoints.sort(key=lambda x: x['timestamp'], reverse=True)
        return checkpoints
    
    def _extract_config(self, config):
        """Extract configuration as dictionary"""
        keys = config.getKeys()
        config_dict = {}
        for key in keys:
            config_dict[key] = config.getOr(key, "")
        return config_dict
    
    def _restore_config(self, config, config_dict):
        """Restore configuration from dictionary"""
        for key, value in config_dict.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
    
    def _capture_brain_state(self, brain):
        """Capture current brain state"""
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'developmental_stage': str(brain.getDevelopmentalStage()),
            'regions': {
                str(region.getId()): {
                    'name': region.getName(),
                    'neuron_count': region.getTotalNeuronCount()
                }
                for region in brain.getRegions()
            }
        }
    
    def _restore_brain_state(self, brain, state):
        """Restore brain state (simplified implementation)"""
        print(f"Would restore brain with {state['total_neurons']} neurons")
        print(f"  Stage: {state['developmental_stage']}")
        print(f"  Firing: {state['firing_neurons']}/{state['total_neurons']}")
    
    def _capture_agent_state(self, agent):
        """Capture current agent state"""
        return {
            'curiosity_level': agent.getCuriosityLevel(),
            'novelty_level': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'developmental_stage': str(agent.getDevelopmentalStage()),
            'enabled_systems': {
                'reward_modulation': agent.isRewardModulationEnabled(),
                'curiosity': agent.isCuriosityEnabled(),
                'structural_plasticity': agent.isStructuralPlasticityEnabled(),
                'development': agent.isDevelopmentEnabled()
            }
        }
    
    def _restore_agent_state(self, agent, state):
        """Restore agent state"""
        # Enable systems based on saved state
        agent.enableRewardModulation(state['enabled_systems']['reward_modulation'])
        agent.enableCuriosity(state['enabled_systems']['curiosity'])
        agent.enableStructuralPlasticity(state['enabled_systems']['structural_plasticity'])
        agent.enableDevelopment(state['enabled_systems']['development'])
    
    def _capture_world_state(self, world):
        """Capture world state"""
        return {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'simulation_time': world.getSimulationTime(),
            'max_energy': world.getMaxEnergy(),
            'agent_body': {
                'x': world.getAgentBody().x,
                'y': world.getAgentBody().y,
                'energy': world.getAgentBody().energy
            }
        }

# Advanced simulation with checkpointing
class AdvancedNLMExperiment:
    def __init__(self, experiment_name="advanced_learning"):
        self.checkpoint_manager = NMCheckpointManager()
        self.experiment_name = experiment_name
    
    def run_with_checkpointing(self, total_steps=1000, checkpoint_interval=200):
        """Run simulation with automatic checkpointing and recovery"""
        print(f"Starting advanced experiment: {self.experiment_name}")
        
        # Create sophisticated configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 3000)
        config.set("brain.synapse_density", 0.08)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_rate", 0.6)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=40, height=40, visionWidth=12, visionHeight=12)
        world.setAgentStart(20.0, 20.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        print("Running simulation...")
        
        for step in range(total_steps):
            world.update(0.1)
            
            # Enhanced agent processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain computation
            brain.step(step)
            
            # Advanced action selection
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Multi-factor learning
            reward = 0.1
            if step % 50 == 0:  # Occasional bonus reward
                reward += 0.2
            if agent.getNoveltyLevel() > 0.8:  # Novelty bonus
                reward += 0.1
            
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Checkpointing
            if step % checkpoint_interval == 0 and step > 0:
                self.checkpoint_manager.save_progress(
                    brain, agent, world, self.experiment_name, step,
                    {'learning_rate': 0.002, 'curiosity': 0.6, 'reward': reward}
                )
            
            # Optional: Load from checkpoint (simulate recovery)
            if step == 300:  # At step 300, load from saved checkpoint
                checkpoints = self.checkpoint_manager.list_available_checkpoints(self.experiment_name)
                if len(checkpoints) > 0:
                    print(f"\nSimulating recovery at step {step}")
                    latest_checkpoint = checkpoints[-1]
                    print(f"Would load from checkpoint: {latest_checkpoint['step']}")
                    print(f"  Memory: Would restore {latest_checkpoint.get('brain_state', {}).get('total_neurons', 'N/A')} neurons")
            
            # Progress monitoring
            if step % 100 == 0:
                print(f"Step {step}: "
                      f"Curiosity={agent.getCuriosityLevel():.3f}, "
                      f"Novelty={agent.getNoveltyLevel():.3f}, "
                      f"Firing={brain.getFiringNeuronCount()}, "
                      f"Stage={brain.getDevelopmentalStage()}")
        
        # Save final state
        self.checkpoint_manager.save_progress(
            brain, agent, world, self.experiment_name, total_steps,
            {'final_step': total_steps, 'completed': True}
        )
        
        print(f"\nExperiment completed!")
        print(f"Final state saved to checkpoints.")
        return brain, agent, world

# Run the advanced experiment
experiment = AdvancedNLMExperiment("neural_optimization")
results = experiment.run_with_checkpointing(total_steps=1000, checkpoint_interval=300)
```

### Configuration Management

#### JSON Configuration Editor

```python
import pynlm
import json

class NLMConfigEditor:
    """Edit NLM configurations with JSON support"""
    
    def __init__(self):
        self.config_templates = {}
    
    def create_template(self, name, config_data):
        """Create a named configuration template"""
        self.config_templates[name] = config_data
        print(f"Template '{name}' created with {len(config_data)} parameters")
    
    def get_template(self, name):
        """Get configuration from template"""
        if name in self.config_templates:
            return self.config_templates[name]
        else:
            raise ValueError(f"Template '{name}' not found")
    
    def modify_template(self, name, modifications):
        """Modify existing template"""
        if name not in self.config_templates:
            raise ValueError(f"Template '{name}' not found")
        
        template = self.config_templates[name].copy()
        template.update(modifications)
        self.config_templates[name] = template
        print(f"Template '{name}' modified")
        return template
    
    def apply_to_brain(self, template_name, config=None):
        """Apply template to brain creation"""
        if config is None:
            config = pynlm.createDefaultConfig()
        
        template = self.get_template(template_name)
        
        # Apply all template settings
        for key, value in template.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        print(f"Brain created using template '{template_name}'")
        return brain

# Create and use configuration templates
editor = NLMConfigEditor()

# Create useful templates
editor.create_template("exploration", {
    "brain.neuron_count": 2000,
    "neuromod.dopamine.scale": 0.8,
    "neuromod.curiosity.enable": True,
    "neuromod.curiosity.exploration_rate": 0.9,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001
})

editor.create_template("exploitation", {
    "brain.neuron_count": 1500,
    "neuromod.dopamine.scale": 1.5,
    "neuromod.curiosity.enable": False,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.005,
    "plasticity.hebbian.enable": True
})

editor.create_template("balanced", {
    "brain.neuron_count": 3000,
    "brain.synapse_density": 0.05,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "plasticity.stdp.enable": True,
    "plasticity.structural.enable": True,
    "plasticity.hebbian.enable": True
})

# Use templates to create different types of agents
print("\nCreating different agent types using templates...")

# Exploration agent
exploration_brain = editor.apply_to_brain("exploration")
print(f"Exploration agent: {exploration_brain.getTotalNeuronCount()} neurons")

# Exploitation agent  
exploitation_brain = editor.apply_to_brain("exploitation")
print(f"Exploitation agent: {exploitation_brain.getTotalNeuronCount()} neurons")

# Balanced agent
balanced_brain = editor.apply_to_brain("balanced")
print(f"Balanced agent: {balanced_brain.getTotalNeuronCount()} neurons")
```

### Performance Optimization

#### Efficient Simulation Manager

```python
import pynlm
import time
import numpy as np

class EfficientSimulationManager:
    """Optimized simulation management for better performance"""
    
    def __init__(self):
        self.simulation_stats = {}
    
    def run_optimized_simulation(self, config, world_config, steps=500):
        """Run simulation with optimizations"""
        print("Starting optimized simulation...")
        
        # Create brain with optimal configuration
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world efficiently
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Batch learning if enabled
        if agent.isRewardModulationEnabled():
            print("Batch reward learning enabled")
        
        # Optimized simulation loop
        start_time = time.time()
        
        for step in range(steps):
            # Batch world operations
            world.update(0.1)
            
            # Batch agent processing
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Batch brain operations
            brain.step(step)
            
            # Batch action application
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Batch learning updates
            if agent.isRewardModulationEnabled():
                agent.applyRewardModulation(0.1, 0.0)
            
            if agent.isDevelopmentEnabled():
                agent.updateDevelopment(0.1)
            
            # Performance tracking
            if step % 50 == 0:
                self._track_performance(brain, agent, step, time.time() - start_time)
        
        end_time = time.time()
        total_time = end_time - start_time
        
        # Calculate performance metrics
        stats = self.simulation_stats.get(config.getOr('brain.neuron_count', 0), {})
        stats.update({
            'total_time': total_time,
            'steps_per_second': steps / total_time,
            'neurons_per_second': config.getOr('brain.neuron_count', 0) / total_time,
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel()
        })
        
        self.simulation_stats[config.getOr('brain.neuron_count', 0)] = stats
        
        print(f"\nOptimized simulation completed:")
        print(f"  Total time: {total_time:.2f} seconds")
        print(f"  Steps per second: {steps / total_time:.1f}")
        print(f"  Neurons per second: {config.getOr('brain.neuron_count', 0) / total_time:.0f}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.3f} Hz")
        
        return brain, agent, world
    
    def _track_performance(self, brain, agent, step, elapsed_time):
        """Track and log performance metrics"""
        firing_rate = brain.getAverageFiringRate()
        curiosity = agent.getCuriosityLevel()
        neurons = brain.getTotalNeuronCount()
        
        print(f"  Step {step:4d}: {firing_rate:6.3f} Hz firing, "
              f"{curiosity:5.3f} curiosity, {neurons:5d} neurons, "
              f"{elapsed_time:6.1f}s elapsed")
    
    def optimize_configuration(self, base_config, parameter_ranges):
        """Optimize configuration parameters"""
        print("Running parameter optimization...")
        
        # Grid search over parameters
        best_config = None
        best_score = -1
        
        for neuron_count in parameter_ranges['neuron_counts']:
            for learning_rate in parameter_ranges['learning_rates']:
                # Create test configuration
                test_config = pynlm.createDefaultConfig()
                test_config.set("brain.neuron_count", neuron_count)
                test_config.set("plasticity.stdp.learning_rate", learning_rate)
                test_config.set("neuromod.curiosity.enable", True)
                
                # Run quick test simulation
                brain = pynlm.createBrain(test_config)
                brain.initialize()
                
                world = pynlm.createSimpleWorld()
                world.configure(width=15, height=15)
                
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agent.enableRewardModulation(True)
                
                # Quick performance test
                for step in range(50):
                    world.update(0.1)
                    agent.processSensoryInput(world.getSensoryPercept())
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                    agent.applyRewardModulation(0.1, 0.0)
                    agent.updateDevelopment(0.1)
                
                # Calculate score (higher is better)
                score = (agent.getCuriosityLevel() * 0.4 + 
                        brain.getAverageFiringRate() * 0.3 + 
                        brain.getTotalNeuronCount() * 0.3 / 10000)
                
                if score > best_score:
                    best_score = score
                    best_config = test_config.copy() if hasattr(test_config, 'copy') else test_config
                
                print(f"  Neurons={neuron_count:4d}, LR={learning_rate:.4f}: "
                      f"Score={score:.3f}")
        
        print(f"\nBest configuration found:")
        print(f"  Neurons: {best_config.getOr('brain.neuron_count', 0)}")
        print(f"  Learning rate: {best_config.getOr('plasticity.stdp.learning_rate', 0.0)}")
        print(f"  Score: {best_score:.3f}")
        
        return best_config, best_score

# Create optimized configuration manager
optimizer = EfficientSimulationManager()

# Example usage
base_config = pynlm.createDefaultConfig()
base_config.set("brain.neuron_count", 1000)

# Optimized brain configuration
optimized_config = pynlm.createDefaultConfig()
optimized_config.set("brain.neuron_count", 2500)
optimized_config.set("brain.synapse_density", 0.06)
optimized_config.set("plasticity.stdp.learning_rate", 0.002)
optimized_config.set("neuromod.dopamine.scale", 1.1)

# World configuration
world_config = {
    "width": 30,
    "height": 30,
    "visionWidth": 10,
    "visionHeight": 10
}

# Run optimized simulation
brain, agent, world = noptimizer.run_optimized_simulation(
    optimized_config, world_config, steps=300
)

# Parameter optimization
param_ranges = {
    'neuron_counts': [500, 1000, 2000, 5000],
    'learning_rates': [0.0005, 0.001, 0.002, 0.005]
}

optimized_config, best_score = noptimizer.optimize_configuration(base_config, param_ranges)
```

## Conclusion

NLM provides a powerful framework for creating and simulating brain-like systems. This comprehensive guide covers:

### What You've Learned:

1. **Building and Installation** - Complete setup for both C++ and Python environments
2. **Core API Usage** - Brain, agent, and world component interactions
3. **Simulation Patterns** - Standard and advanced simulation loops
4. **Configuration Management** - JSON/YAML support with backward compatibility
5. **Advanced Features** - Multi-agent systems, learning optimization, checkpointing
6. **Performance Tuning** - Optimization techniques and profiling

### Key Takeaways:

- **Start Simple**: Use default configurations for initial experiments
- **Understand the Loop**: World → Perception → Brain → Action → World
- **Enable Learning**: Enable reward modulation, curiosity, and development for realistic behavior
- **Track Progress**: Monitor firing rates, curiosity levels, and developmental stages
- **Optimize Gradually**: Test different parameter combinations systematically
- **Use Checkpoints**: Save and restore long-running experiments

### Next Steps:

1. **Experiment**: Try different neuron counts and learning rates
2. **Document Results**: Keep track of what works best
3. **Share Knowledge**: Document your findings for others
4. **Contribute**: Improve NLM by sharing your enhancements

NLM is designed to be flexible and extensible. The Python bindings make it accessible while the C++ core provides the performance needed for complex brain simulations. Whether you're building simple test systems or complex multi-agent environments, NLM provides the tools you need to explore brain-like computation.

Happy experimenting with artificial brains! 🧠

---

*For more resources and community support, visit the NLM documentation and GitHub repository.*

**Last updated:** Current date
**Version:** Phase 6.0+ (Final Integration)
