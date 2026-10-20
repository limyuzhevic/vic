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

# For development with debugging
pip install -e .[dev]
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
print(pynlm.VERSION)

# Check system information
system_info = pynlm.getSystemInfo()
print("System Info:", system_info)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Test basic functionality
brain.initialize()
brain.step(0)
print("Basic functionality test passed!")
```

---

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Advanced Examples

#### Example 1: Performance Monitoring and Optimization

```python
import pynlm
import time

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Performance monitoring
start_time = time.time()
step_times = []
firing_rates = []

print("Running performance benchmark...")
for step in range(1000):
    step_start = time.time()
    
    # Run simulation step
    brain.step(step)
    
    step_end = time.time()
    step_times.append(step_end - step_start)
    firing_rates.append(brain.getFiringNeuronCount())
    
    # Print progress
    if step % 100 == 0:
        avg_step_time = sum(step_times) / len(step_times)
        avg_firing_rate = sum(firing_rates) / len(firing_rates)
        print(f"Step {step}: Avg time {avg_step_time:.4f}s, Avg firing {avg_firing_rate:.1f}")

# Performance analysis
total_time = time.time() - start_time
print(f"\nPerformance Summary:")
print(f"  Total time: {total_time:.2f}s")
print(f"  Steps per second: {1000/total_time:.1f}")
print(f"  Average step time: {sum(step_times)/len(step_times):.4f}s")
print(f"  Firing rate stats:")
print(f"    Mean: {sum(firing_rates)/len(firing_rates):.1f}")
print(f"    Max: {max(firing_rates)}")
print(f"    Min: {min(firing_rates)}")
```

#### Example 2: Advanced Configuration Management

```python
import pynlm

# Configuration presets
class BrainPresets:
    @staticmethod
    def create_startup_config():
        """Minimal configuration for quick testing"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 100)
        config.set("brain.synapse_density", 0.05)
        config.set("brain.v_thresh", -50.0)
        return config
    
    @staticmethod
    def create_research_config(neuron_count=5000):
        """High-fidelity configuration for research"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.1)
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.structural.enable", True)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.dopamine.scale", 2.0)
        return config
    
    @staticmethod
    def create_energy_efficient_config():
        """Optimized for minimal resource usage"""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 200)
        config.set("brain.synapse_density", 0.02)
        config.set("plasticity.stdp.enable", False)  # Disable plasticity
        config.set("neuromod.curiosity.enable", False)
        config.set("neuromod.novelty.enable", False)
        return config

# Use presets
configs = {
    "startup": BrainPresets.create_startup_config(),
    "research": BrainPresets.create_research_config(),
    "efficient": BrainPresets.create_energy_efficient_config()
}

# Create brains with different configurations
for name, config in configs.items():
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"{name} config: {brain.getTotalNeuronCount()} neurons")
```

#### Example 3: Complex World Simulation

```python
import pynlm
import random

class AdvancedWorldSimulator:
    def __init__(self):
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
        
        self.resources = []
        self.hazards = []
        self.agents = []
        
        self._create_environment()
    
    def _create_environment(self):
        """Create complex environment with various objects"""
        # Create resources
        for i in range(30):
            x = random.uniform(5, 45)
            y = random.uniform(5, 45)
            obj = pynlm.WorldObject(
                x, y, 
                pynlm.WorldObjectType.Resource, 
                value=random.uniform(1, 5)
            )
            self.world.addObject(obj)
            self.resources.append(obj)
        
        # Create hazards
        for i in range(8):
            x = random.uniform(20, 30)
            y = random.uniform(20, 30)
            obj = pynlm.WorldObject(
                x, y,
                pynlm.WorldObjectType.Hazard,
                value=1.0
            )
            self.world.addObject(obj)
            self.hazards.append(obj)
        
        # Create walls (boundaries)
        wall_width = 1.0
        # Top wall
        for x in [2, 48]:
            wall = pynlm.WorldObject(x, 2, pynlm.WorldObjectType.Wall, radius=wall_width)
            self.world.addObject(wall)
        
        # Reset world
        self.world.reset()
        self.world.setAgentStart(25, 25)
    
    def create_advanced_agent(self, brain):
        """Create an advanced agent with all learning systems"""
        agent = pynlm.createAgentBrain(brain)
        
        # Enable all advanced subsystems
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        self.agents.append(agent)
        return agent
    
    def run_simulation(self, num_steps=500):
        """Run complex simulation with detailed logging"""
        brain = self.agents[0].getBrain()
        agent = self.agents[0]
        world = self.world
        
        print(f"Starting complex simulation with {brain.getTotalNeuronCount()} neurons")
        
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get full percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Check for special actions
            self._process_special_actions(action, world)
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation based on environment
            self._apply_environment_modulation(agent, world)
            
            # Update development
            agent.updateDevelopment(0.1)
            
            # Print detailed statistics every 100 steps
            if step % 100 == 0:
                self._print_simulation_stats(step, brain, agent, world)
        
        return self._get_simulation_results()
    
    def _process_special_actions(self, action, world):
        """Process special actions like interaction"""
        if action.getType() == pynlm.ActionType.Interact:
            agent_body = world.getAgentBody()
            x, y = agent_body.x, agent_body.y
            
            # Check for resource collection
            for obj in self.resources[:]:
                if abs(obj.x - x) < 1.0 and abs(obj.y - y) < 1.0:
                    world.removeObject(obj.x, obj.y)
                    self.resources.remove(obj)
                    print(f"Resource collected at ({x:.1f}, {y:.1f})")
                    return
            
            # Check for hazard detection
            for obj in self.hazards:
                if abs(obj.x - x) < 1.0 and abs(obj.y - y) < 1.0:
                    print(f"Hazard detected at ({x:.1f}, {y:.1f})")
                    return
    
    def _apply_environment_modulation(self, agent, world):
        """Apply environmental effects based on world state"""
        # Count nearby resources
        agent_body = world.getAgentBody()
        nearby_resources = sum(1 for obj in self.resources
                              if abs(obj.x - agent_body.x) < 10.0 and 
                                 abs(obj.y - agent_body.y) < 10.0)
        
        # Count nearby hazards
        nearby_hazards = sum(1 for obj in self.hazards
                           if abs(obj.x - agent_body.x) < 10.0 and 
                              abs(obj.y - agent_body.y) < 10.0)
        
        # Apply rewards based on environment
        if nearby_resources > 0:
            reward = min(nearby_resources, 3) * 0.5
            agent.applyRewardModulation(reward, 0.1)
        
        if nearby_hazards > 0:
            penalty = nearby_hazards * 0.8
            agent.applyRewardModulation(-penalty, 0.2)
    
    def _print_simulation_stats(self, step, brain, agent, world):
        """Print detailed simulation statistics"""
        print(f"\nStep {step}:")
        print(f"  Brain Statistics:")
        print(f"    Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"    Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"    Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Agent Statistics:")
        print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
        print(f"    Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Environment:")
        print(f"    Resources remaining: {len(self.resources)}")
        print(f"    Hazards active: {len(self.hazards)}")
    
    def _get_simulation_results(self):
        """Get final simulation results"""
        return {
            "total_steps": 500,
            "resources_remaining": len(self.resources),
            "hazards_remaining": len(self.hazards),
            "agent_count": len(self.agents)
        }

# Usage example
simulator = AdvancedWorldSimulator()

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create advanced agent
simulator.create_advanced_agent(brain)

# Run complex simulation
results = simulator.run_simulation(500)
print(f"\nSimulation complete: {results}")
```

### Advanced API Usage Patterns

#### 1. Context Manager Pattern

```python
import pynlm

# Context manager for automatic cleanup
config = pynlm.createDefaultConfig()

with pynlm.createBrain(config) as brain:
    brain.initialize()
    
    # Use brain within context
    for step in range(100):
        brain.step(step)
    
    # Brain automatically resets when exiting context
    print("Context manager completed")

# Try to use brain outside context - it's been reset
brain.step(0)  # Works but brain is reset
```

#### 2. Configuration Builder Pattern

```python
import pynlm

class BrainConfigBuilder:
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
    
    def set_neurons(self, count):
        """Set number of neurons"""
        self.config.set("brain.neuron_count", count)
        return self
    
    def set_plasticity(self, stdp=True, hebbian=True, structural=True):
        """Configure plasticity systems"""
        self.config.set("plasticity.stdp.enable", stdp)
        self.config.set("plasticity.hebbian.enable", hebbian)
        self.config.set("plasticity.structural.enable", structural)
        return self
    
    def set_neuromodulation(self, curiosity=True, novelty=True, dopamine_scale=1.0):
        """Configure neuromodulation"""
        self.config.set("neuromod.curiosity.enable", curiosity)
        self.config.set("neuromod.novelty.enable", novelty)
        self.config.set("neuromod.dopamine.scale", dopamine_scale)
        return self
    
    def set_environment(self, width=20, height=20, vision_width=8, vision_height=8):
        """Configure world environment"""
        self.config.set("world.width", width)
        self.config.set("world.height", height)
        self.config.set("world.vision_width", vision_width)
        self.config.set("world.vision_height", vision_height)
        return self
    
    def build(self):
        """Build and return the configuration"""
        return self.config

# Usage
config = (BrainConfigBuilder()
    .set_neurons(1000)
    .set_plasticity(stdp=True, hebbian=True, structural=True)
    .set_neuromodulation(curiosity=True, novelty=True, dopamine_scale=2.0)
    .set_environment(width=30, height=30)
    .build())

brain = pynlm.createBrain(config)
brain.initialize()
```

#### 3. Agent State Manager Pattern

```python
import pynlm

class AgentManager:
    def __init__(self):
        self.agents = []
        self.histories = []
    
    def create_agent(self, brain, world):
        """Create an agent with world"""
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        self.agents.append(agent)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Store initial state
        self.histories.append({
            'step': 0,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel()
        })
        
        return agent
    
    def run_episode(self, steps=200):
        """Run a complete episode"""
        for step in range(steps):
            # Get current world state
            percept = self.world.getSensoryPercept()
            
            # Process and act
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Update learning
            reward = self._calculate_reward(percept)
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            # Store history
            self.histories.append({
                'step': step,
                'action': action.getType(),
                'reward': reward,
                'curiosity': self.agent.getCuriosityLevel(),
                'novelty': self.agent.getNoveltyLevel(),
                'neuromodulation': self.agent.getNeuromodulationLevel()
            })
    
    def analyze_performance(self):
        """Analyze agent performance"""
        if not self.histories:
            return None
        
        performance = {
            'total_steps': len(self.histories) - 1,
            'curiosity_trend': self._calculate_trend(
                [h['curiosity'] for h in self.histories]
            ),
            'novelty_trend': self._calculate_trend(
                [h['novelty'] for h in self.histories]
            ),
            'reward_average': sum(h['reward'] for h in self.histories) / len(self.histories),
            'action_distribution': self._count_actions(
                [h['action'] for h in self.histories]
            )
        }
        
        return performance
    
    def _calculate_reward(self, percept):
        """Calculate reward from percept"""
        # Simple reward function - can be made more sophisticated
        vision = percept.getVision()
        reward = 0.0
        
        # Positive reward for finding resources
        resources = sum(1 for obj in self.world.objects 
                       if obj.type == pynlm.WorldObjectType.Resource)
        if resources > 0:
            reward += resources * 0.1
        
        # Negative reward for hazards
        hazards = sum(1 for obj in self.world.objects 
                     if obj.type == pynlm.WorldObjectType.Hazard)
        if hazards > 0:
            reward -= hazards * 0.2
        
        return reward
    
    def _calculate_trend(self, values):
        """Calculate trend of values"""
        if len(values) < 2:
            return "stable"
        
        first_half = sum(values[:len(values)//2]) / (len(values)//2)
        second_half = sum(values[len(values)//2:]) / (len(values)//2)
        
        if second_half > first_half + 0.1:
            return "increasing"
        elif second_half < first_half - 0.1:
            return "decreasing"
        else:
            return "stable"
    
    def _count_actions(self, actions):
        """Count action occurrences"""
        from collections import Counter
        return dict(Counter(actions))

# Usage example
manager = AgentManager()

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
manager.create_agent(brain, world)
manager.agent = manager.agents[0]
manager.world = world

# Run episode
manager.run_episode(200)

# Analyze performance
performance = manager.analyze_performance()
print("Agent Performance:")
for key, value in performance.items():
    if isinstance(value, dict):
        print(f"  {key}: {value}")
    else:
        print(f"  {key}: {value}")
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

#### Key Classes and Methods

This section provides a comprehensive reference for the main NLM classes and their essential methods. Use this as a quick reference for API documentation.

##### Brain Class

**Creation and Initialization**
```python
# Create brain with configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Alternative: Brain as context manager (automatic reset)
with pynlm.createBrain(config) as brain:
    brain.initialize()
    # Brain automatically resets when exiting context
```

**Core Simulation Methods**
```python
# Run simulation steps
brain.step(step_number)                    # Run single step
brain.step(step_number, time)             # Run step with timestamp
brain.reset()                              # Reset brain state

# Input and Output
brain.receiveSensoryInput(sensory_input)   # Inject sensory data
action = brain.produceAction()            # Get motor action

# Neural Statistics
brain.getTotalNeuronCount()                # Total neurons in brain
brain.getTotalSynapseCount()               # Total synapses
brain.getActiveNeuronCount()               # Currently active neurons
brain.getFiringNeuronCount()              # Neurons currently firing
brain.getAverageFiringRate()               # Average firing rate (Hz)
brain.getExcitationInhibitionRatio()     # E/I balance ratio
brain.getTotalSpikeCount()                 # Total spike events
```

**State Management**
```python
# Save and load brain state
brain.save("checkpoint.bin")               # Save current state
brain.load("checkpoint.bin")               # Load saved state

# Region management
region_id = brain.addRegion("cortex")     # Add new neural region
region = brain.getRegion(region_id)        # Get region by ID
brain.getRegionCount()                     # Number of regions
brain.getRegionIds()                       # List of all region IDs
```

**Development and Plasticity**
```python
# Development control
brain.getDevelopmentalStage()             # Current developmental stage
brain.setDevelopmentalStage(stage)       # Set developmental stage
brain.updatePlasticity()                   # Update plasticity rules

# Neuromodulation access
brain.getConfig()                          # Get configuration
brain.logStatus()                          # Log brain status and statistics
```

##### Config Class

**Configuration Management**
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Safe value access (returns None if key doesn't exist or type mismatch)
config.get("brain.neuron_count")          # Get as int
config.get("brain.threshold")             # Get as float
config.get("brain.name")                   # Get as string
config.get("brain.enabled")               # Get as boolean

# Type-safe access with defaults
value = config.getOr("brain.neuron_count", 1000)    # Return default if missing
value = config.getOr("brain.threshold", 0.5, float) # Specify type

# Configuration modification
config.set("brain.neuron_count", 2000)    # Set integer
config.set("brain.threshold", 0.01)       # Set float
config.set("brain.enabled", True)         # Set boolean
config.set("brain.name", "My Brain")      # Set string

# Configuration validation and query
config.has("brain.neuron_count")           # Check if key exists
config.getKeys()                           # Get all configuration keys
config.remove("brain.old_setting")        # Remove configuration entry
config.clear()                             # Clear all configuration
config.summary()                           # Get configuration summary

# File I/O
config.loadFromFile("config.json")       # Load from JSON
config.saveToFile("config.json")         # Save to JSON
config.loadFromArgs(argc, argv)          # Load from command line
```

##### AgentBrain Class

**Agent Initialization and Control**
```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize with world
agent.initialize(world)

# Sensory and Motor Processing
agent.getSensoryInputSize()                # Expected sensory input size
agent.getMotorOutputSize()                 # Expected motor output size
agent.processSensoryInput(percept)        # Process sensory input
action = agent.decodeMotorCommand()       # Get motor action

# Neuromodulation Systems
agent.applyRewardModulation(reward, predicted_reward)  # Apply reward learning
agent.getCuriosityLevel()                 # Get curiosity level
agent.getNoveltyLevel()                   # Get novelty level
agent.getNeuromodulationLevel()           # Get overall neuromodulation level
agent.getPredictionError()                # Get prediction error signal
```

**Learning System Control**
```python
# Enable/disable learning systems
agent.enableRewardModulation(True/False)      # Enable/disable reward learning
agent.enableStructuralPlasticity(True/False)  # Enable/disable structural plasticity
agent.enableDevelopment(True/False)           # Enable/disable development
agent.enableCuriosity(True/False)             # Enable/disable curiosity

# Development updates
agent.updateDevelopment(timestep)             # Update development system
agent.getDevelopmentalStage()               # Get current developmental stage

# Status checks
agent.isRewardModulationEnabled()            # Check if reward modulation is enabled
agent.isStructuralPlasticityEnabled()       # Check if structural plasticity is enabled
agent.isDevelopmentEnabled()                # Check if development is enabled
agent.isCuriosityEnabled()                   # Check if curiosity is enabled
```

**Episode Management**
```python
# Reset agent for new episode
agent.reset()

# Get underlying brain reference
brain = agent.getBrain()
```

##### SimpleWorld Class

**World Configuration and Control**
```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world parameters
world.configure(
    width=20,                    # World width
    height=20,                   # World height
    visionWidth=8,               # Vision width
    visionHeight=8               # Vision height
)

# World control
world.reset()                    # Reset to initial state
world.update(timestep)           # Update world state
world.setRandomSeed(42)          # Set random seed for reproducibility
```

**Agent Interaction**
```python
# Set agent starting position
world.setAgentStart(x, y)

# Get agent state
body = world.getAgentBody()      # Get agent body state

# Apply actions
world.applyMotorCommand(action, current_time)  # Apply motor action

# Sensory input
percept = world.getSensoryPercept()  # Get current sensory percept
```

**Environment Management**
```python
# Object management
world.addObject(obj)             # Add world object
world.removeObject(x, y)         # Remove object at position
world.isValidPosition(x, y)      # Check if position is valid

# World properties
world.getWidth()                 # Get world width
world.getHeight()                # Get world height
world.getMaxEnergy()             # Get maximum energy
world.setMaxEnergy(100.0)        # Set maximum energy
world.getEnergyDecayRate()       # Get energy decay rate
world.setEnergyDecayRate(0.01)   # Set energy decay rate
world.getSimulationTime()        # Get current simulation time
```

**Important Constants and Types**

The NLM library provides several important constants and enumerations:

```python
# Invalid ID constants
pynlm.INVALID_NEURON_ID     # Invalid neuron ID
pynlm.INVALID_SYNAPSE_ID    # Invalid synapse ID
pynlm.INVALID_REGION_ID     # Invalid region ID
pynlm.INVALID_POPULATION_ID # Invalid population ID

# Neuron Types
pynlm.NeuronType.Excitatory   # Standard excitatory neuron
pynlm.NeuronType.Inhibitory   # Standard inhibitory neuron
pynlm.NeuronType.Modulatory   # Modulatory neuron
pynlm.NeuronType.Sensory      # Sensory neuron
pynlm.NeuronType.Motor        # Motor neuron
pynlm.NeuronType.Internal     # Internal processing neuron

# Synapse Types
pynlm.SynapseType.Excitatory    # Standard excitatory synapse
pynlm.SynapseType.Inhibitory    # Standard inhibitory synapse
pynlm.SynapseType.Modulatory    # Modulatory synapse
pynlm.SynapseType.Electrical   # Electrical synapse
pynlm.SynapseType.GapJunction  # Gap junction synapse

# Developmental Stages
pynlm.DevelopmentalStage.Initial        # Initial stage
pynlm.DevelopmentalStage.CriticalPeriod # Critical period for learning
pynlm.DevelopmentalStage.Maturation    # Maturation phase
pynlm.DevelopmentalStage.Adult        # Adult stage
pynlm.DevelopmentalStage.Aging        # Aging phase

# Action Types
pynlm.ActionType.MoveForward    # Move forward action
pynlm.ActionType.MoveBackward   # Move backward action
pynlm.ActionType.TurnLeft       # Turn left action
pynlm.ActionType.TurnRight      # Turn right action
pynlm.ActionType.Look           # Look action
pynlm.ActionType.Interact       # Interact action
pynlm.ActionType.Eat            # Eat action
pynlm.ActionType.Drink          # Drink action
pynlm.ActionType.Rest           # Rest action
pynlm.ActionType.Wait           # Wait action
pynlm.ActionType.Custom         # Custom action

# World Object Types
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource object
pynlm.WorldObjectType.Hazard    # Hazard object
pynlm.WorldObjectType.Wall      # Wall object
pynlm.WorldObjectType.Marker    # Marker object
```

**Module-level Utilities**

```python
# System information and utilities
pynlm.getVersion()              # Return version string
pynlm.getSystemInfo()          # Return system info dictionary
pynlm.createFastConfig()       # Create fast simulation config
pynlm.createResearchConfig()   # Create research-grade config
pynlm.createDefaultConfig()    # Create standard config

# Factory functions
pynlm.createBrain(config)      # Create brain with configuration
pynlm.createAgentBrain(brain)  # Create agent brain interface
pynlm.createSimpleWorld()      # Create world environment

# Global constants
pynlm.VERSION                   # Python bindings version
pynlm.PROJECT_NAME             # Project name
```

## Part 4: Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
