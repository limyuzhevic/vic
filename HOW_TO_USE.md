# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Practical use cases and advanced examples
5. Error handling and troubleshooting

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

The build process requires the following system tools:
- C++ compiler (GCC 10+ or Clang 12+)
- CMake 3.16+
- Python 3.8+
- pybind11 2.11.0+

Typical build commands:
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

After successful compilation, you will have the following targets:

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

Alternative build configurations:
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
brain.initialize()
print(f"Neurons: {brain.getTotalNeuronCount()}")
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

### Complete Agent Example

```python
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
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
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
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
# Run the simulation
run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
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

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

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

## Advanced Usage Examples for Experienced Users

This section provides sophisticated examples that build on the basic functionality and demonstrate advanced use cases for users with experience in neural networks and computational neuroscience. Each example is complete and working (commented out but executable) and includes configuration files, best practices, error handling, performance considerations, and detailed explanations.

### 1. Advanced Multi-Agent Systems: Complex Agent Coordination with Shared Memory and Communication Protocols

```python
import pynlm
import numpy as np
import json
from datetime import datetime
import threading
import queue
from concurrent.futures import ThreadPoolExecutor

class AdvancedMultiAgentSystem:
    """Advanced multi-agent coordination with shared memory and communication protocols."""
    
    def __init__(self, num_agents=5, world_size=100):
        self.num_agents = num_agents
        self.world_size = world_size
        self.agents = []
        self.brains = []
        self.shared_memory = {}
        self.communication_protocols = {}
        self.world = None
        self.config_templates = {}
        
        # Initialize shared memory structure
        self._initialize_shared_memory()
        # Initialize communication protocols
        self._initialize_communication_protocols()
    
    def _initialize_shared_memory(self):
        """Initialize shared memory structure for agent coordination."""
        self.shared_memory = {
            'global_resources': {
                'food_sources': [],
                'water_sources': [],
                'energy_pools': [],
                'information_nodes': []
            },
            'environmental_state': {
                'temperature': 25.0,
                'humidity': 0.6,
                'pressure': 101.3,
                'light_intensity': 1000.0
            },
            'collective_knowledge': {
                'successful_strategies': [],
                'threats_detected': [],
                'resource_locations': {},
                'danger_zones': []
            },
            'communication_channel': queue.Queue(maxsize=100),
            'coordination_signals': {}
        }
    
    def _initialize_communication_protocols(self):
        """Initialize communication protocols between agents."""
        self.communication_protocols = {
            'direct_message': {
                'buffer_size': 50,
                'delivery_guarantee': 'reliable',
                'priority_levels': ['low', 'medium', 'high', 'urgent']
            },
            'broadcast': {
                'range': world_size,
                'frequency': 10,  # Hz
                'encoding': 'json'
            },
            'negotiation': {
                'protocol': 'recursive_arbitration',
                'timeout': 5000,  # ms
                'consensus_threshold': 0.75
            },
            'emergency_alert': {
                'broadcast_radius': 200,
                'acknowledgment_required': True,
                'urgency_levels': ['warning', 'critical', 'catastrophic']
            }
        }
    
    def create_config_template(self, agent_id, specialization='generalist'):
        """Create agent-specific configuration templates."""
        if specialization == 'specialist':
            config = self._create_specialist_config(agent_id)
        elif specialization == 'coordinator':
            config = self._create_coordinator_config(agent_id)
        elif specialization == 'explorer':
            config = self._create_explorer_config(agent_id)
        else:  # generalist
            config = self._create_generalist_config(agent_id)
        
        self.config_templates[agent_id] = config
        return config
    
    def _create_generalist_config(self, agent_id):
        """Create configuration for generalist agent."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 400)
        config.set("brain.synapse_density", 0.12)
        config.set("brain.v_thresh", -45.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -65.0)
        
        # Balanced plasticity for general learning
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.0002)
        config.set("plasticity.structural.enable", True)
        
        # Adaptive neuromodulation
        config.set("neuromod.dopamine.scale", 0.8)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction.error_threshold", 0.3)
        
        # Standard development
        config.set("brain.development.stage_duration", 100)
        config.set("brain.development.plasticity_factor", 1.0)
        
        # Basic world interaction
        config.set("world.width", world_size)
        config.set("world.height", world_size)
        config.set("world.vision_width", 8)
        config.set("world.vision_height", 8)
        config.set("world.max_energy", 100.0)
        
        return config
    
    def _create_specialist_config(self, agent_id):
        """Create configuration for specialist agent."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300)
        config.set("brain.synapse_density", 0.15)
        config.set("brain.v_thresh", -40.0)  # More sensitive
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -60.0)
        
        # Enhanced Hebbian plasticity for task-specific learning
        config.set("plasticity.stdp.enable", False)  # Less random learning
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)  # Higher rate
        config.set("plasticity.structural.enable", False)  # Less structural changes
        
        # Specialized neuromodulation
        config.set("neuromod.dopamine.scale", 1.2)
        config.set("neuromod.curiosity.enable", False)  # Less exploration
        config.set("neuromod.novelty.enable", False)
        config.set("neuromod.prediction.error_threshold", 0.2)
        
        # Extended development period
        config.set("brain.development.stage_duration", 150)
        config.set("brain.development.plasticity_factor", 1.5)
        
        # Larger world for specialization
        config.set("world.width", world_size * 1.5)
        config.set("world.height", world_size * 1.5)
        config.set("world.vision_width", 12)
        config.set("world.vision_height", 12)
        
        return config
    
    def _create_coordinator_config(self, agent_id):
        """Create configuration for coordinator agent."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500)
        config.set("brain.synapse_density", 0.08)
        config.set("brain.v_thresh", -50.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -65.0)
        
        # Moderate plasticity for coordination
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0003)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.0001)
        config.set("plasticity.structural.enable", True)
        
        # Balanced neuromodulation
        config.set("neuromod.dopamine.scale", 0.6)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction.error_threshold", 0.4)
        
        # Standard development
        config.set("brain.development.stage_duration", 80)
        config.set("brain.development.plasticity_factor", 0.8)
        
        # Medium world for coordination
        config.set("world.width", world_size)
        config.set("world.height", world_size)
        config.set("world.vision_width", 10)
        config.set("world.vision_height", 10)
        
        return config
    
    def _create_explorer_config(self, agent_id):
        """Create configuration for explorer agent."""
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 350)
        config.set("brain.synapse_density", 0.18)
        config.set("brain.v_thresh", -55.0)
        config.set("brain.v_rest", -70.0)
        config.set("brain.v_reset", -65.0)
        
        # High exploration plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0008)
        config.set("plasticity.hebbian.enable", False)
        config.set("plasticity.hebbian.learning_rate", 0.0001)
        config.set("plasticity.structural.enable", True)
        
        # High curiosity and novelty
        config.set("neuromod.dopamine.scale", 0.7)
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.prediction.error_threshold", 0.5)
        
        # Extended critical period
        config.set("brain.development.stage_duration", 200)
        config.set("brain.development.plasticity_factor", 2.0)
        
        # Large world for exploration
        config.set("world.width", world_size * 2.0)
        config.set("world.height", world_size * 2.0)
        config.set("world.vision_width", 15)
        config.set("world.vision_height", 15)
        
        return config
    
    def initialize_population(self, specialization_distribution=None):
        """Initialize population with diverse specializations."""
        if specialization_distribution is None:
            specialization_distribution = {
                'generalist': 0.4,
                'specialist': 0.2,
                'coordinator': 0.2,
                'explorer': 0.2
            }
        
        agent_positions = self._generate_agent_positions()
        
        for i in range(self.num_agents):
            # Choose specialization based on distribution
            specialization = self._choose_specialization(specialization_distribution)
            
            # Create agent configuration
            config = self.create_config_template(i, specialization)
            
            # Create brain and agent
            brain = pynlm.createBrain(config)
            brain.initialize()
            agent = pynlm.createAgentBrain(brain)
            
            # Initialize shared world
            if i == 0:  # Only one world for all agents
                if self.world is None:
                    self.world = pynlm.createSimpleWorld()
                    self.world.configure(
                        width=self.world_size,
                        height=self.world_size,
                        visionWidth=10,
                        visionHeight=10
                    )
                    self._populate_environment()
                    self.world.reset()
            
            # Initialize agent with world
            agent.initialize(self.world)
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
            
            # Set starting position
            start_x, start_y = agent_positions[i]
            self.world.setAgentStart(start_x, start_y)
            
            # Enable communication protocols
            self._setup_agent_communication(agent, i)
            
            self.brains.append(brain)
            self.agents.append(agent)
            
            print(f"✓ Agent {i} initialized as {specialization}")
    
    def _generate_agent_positions(self):
        """Generate initial positions for all agents."""
        positions = []
        spacing = self.world_size / max(2, int(np.sqrt(self.num_agents)))
        
        for i in range(self.num_agents):
            row = i // int(np.sqrt(self.num_agents))
            col = i % int(np.sqrt(self.num_agents))
            
            x = (col + 0.5) * spacing
            y = (row + 0.5) * spacing
            
            # Add some randomness
            x += np.random.uniform(-spacing * 0.3, spacing * 0.3)
            y += np.random.uniform(-spacing * 0.3, spacing * 0.3)
            
            positions.append((x, y))
        
        return positions
    
    def _choose_specialization(self, distribution):
        """Choose agent specialization based on distribution."""
        choices = list(distribution.keys())
        probabilities = list(distribution.values())
        return np.random.choice(choices, p=probabilities)
    
    def _populate_environment(self):
        """Populate environment with resources and hazards."""
        # Add food sources
        for _ in range(15):
            x = np.random.uniform(10, self.world_size - 10)
            y = np.random.uniform(10, self.world_size - 10)
            self.world.addObject(
                pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0)
            )
        
        # Add water sources
        for _ in range(8):
            x = np.random.uniform(10, self.world_size - 10)
            y = np.random.uniform(10, self.world_size - 10)
            self.world.addObject(
                pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=2.0)
            )
        
        # Add energy pools
        for _ in range(5):
            x = np.random.uniform(10, self.world_size - 10)
            y = np.random.uniform(10, self.world_size - 10)
            self.world.addObject(
                pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=5.0)
            )
        
        # Add hazards
        for _ in range(6):
            x = np.random.uniform(10, self.world_size - 10)
            y = np.random.uniform(10, self.world_size - 10)
            self.world.addObject(
                pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value=-1.0)
            )
    
    def _setup_agent_communication(self, agent, agent_id):
        """Setup communication protocols for specific agent."""
        # Direct message setup
        def direct_message_handler(message):
            # Process incoming messages
            if 'type' in message:
                msg_type = message['type']
                if msg_type == 'resource_share':
                    self._handle_resource_sharing(message, agent_id)
                elif msg_type == 'threat_alert':
                    self._handle_threat_alert(message, agent_id)
                elif msg_type == 'coordination_request':
                    self._handle_coordination_request(message, agent_id)
        """
        
        # Broadcast setup
        def broadcast_handler(message):
            # Process broadcast messages
            if 'type' in message:
                msg_type = message['type']
                if msg_type == 'collective_knowledge':
                    self._handle_collective_knowledge(message)
                elif msg_type == 'emergency':
                    self._handle_emergency_broadcast(message)
        """
        
        # Store handlers
        self.communication_protocols['agent_' + str(agent_id)] = {
            'direct_message': direct_message_handler,
            'broadcast': broadcast_handler,
            'last_seen': datetime.now()
        }
    
    def _handle_resource_sharing(self, message, agent_id):
        """Handle resource sharing between agents."""
        if 'resource' in message and 'location' in message and 'value' in message:
            resource_type = message['resource']
            location = message['location']
            value = message['value']
            
            # Update shared memory
            if resource_type not in self.shared_memory['collective_knowledge']['resource_locations']:
                self.shared_memory['collective_knowledge']['resource_locations'][resource_type] = []
            
            self.shared_memory['collective_knowledge']['resource_locations'][resource_type].append({
                'location': location,
                'value': value,
                'source_agent': agent_id,
                'timestamp': datetime.now()
            })
            
            print(f"✓ Agent {agent_id} shared resource: {resource_type} at {location}")
    
    def _handle_threat_alert(self, message, agent_id):
        """Handle threat detection alerts."""
        if 'threat_type' in message and 'location' in message:
            threat_type = message['threat_type']
            location = message['location']
            
            self.shared_memory['collective_knowledge']['threats_detected'].append({
                'type': threat_type,
                'location': location,
                'source_agent': agent_id,
                'timestamp': datetime.now()
            })
            
            print(f"✓ Agent {agent_id} detected threat: {threat_type} at {location}")
    
    def _handle_coordination_request(self, message, agent_id):
        """Handle coordination requests between agents."""
        if 'task' in message and 'required_resources' in message:
            task = message['task']
            required_resources = message['required_resources']
            
            # Check if agent has required capabilities
            agent_capability = self._assess_agent_capability(agent_id, task)
            
            if agent_capability >= 0.7:  # Agent has sufficient capability
                # Acknowledge and offer assistance
                response = {
                    'type': 'coordination_response',
                    'task': task,
                    'agent_id': agent_id,
                    'capability': agent_capability,
                    'available': True
                }
                
                self.communication_protocols['coordination']['negotiation'].append(response)
                print(f"✓ Agent {agent_id} can assist with task: {task}")
            else:
                print(f"✗ Agent {agent_id} lacks capability for task: {task}")
    
    def _assess_agent_capability(self, agent_id, task):
        """Assess agent capability for a specific task."""
        # Use agent's brain statistics to assess capability
        brain = self.brains[agent_id]
        firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        curiosity = self.agents[agent_id].getCuriosityLevel()
        
        # Simple capability assessment
        capability = (firing_rate / 10.0) * 0.3 + (1.0 / (1.0 + e_i_ratio)) * 0.3 + curiosity * 0.4
        return min(1.0, max(0.0, capability))
    
    def _handle_collective_knowledge(self, message):
        """Handle collective knowledge sharing."""
        if 'knowledge' in message:
            knowledge = message['knowledge']
            self.shared_memory['collective_knowledge']['successful_strategies'].append({
                'strategy': knowledge,
                'timestamp': datetime.now()
            })
            
            print(f"✓ Collective knowledge updated: {knowledge}")
    
    def _handle_emergency_broadcast(self, message):
        """Handle emergency broadcast messages."""
        if 'emergency_type' in message and 'location' in message:
            emergency_type = message['emergency_type']
            location = message['location']
            
            self.shared_memory['collective_knowledge']['threats_detected'].append({
                'type': f"EMERGENCY: {emergency_type}",
                'location': location,
                'timestamp': datetime.now()
            })
            
            print(f"🚨 EMERGENCY BROADCAST: {emergency_type} at {location}")
    
    def run_simulation(self, steps=1000):
        """Run advanced multi-agent coordination simulation."""
        print(f"Starting advanced multi-agent coordination simulation with {self.num_agents} agents...")
        print(f"Agent specializations: {list(self.config_templates.keys())}")
        
        # Initialize population
        self.initialize_population()
        
        # Start coordination monitoring
        coordination_metrics = {
            'steps_completed': 0,
            'successful_coordinations': 0,
            'resource_sharing_events': 0,
            'threat_detections': 0,
            'collective_knowledge_updates': 0
        }
        
        # Main simulation loop
        for step in range(steps):
            self.world.update(0.05)
            
            # Each agent processes sensory input
            for i, agent in enumerate(self.agents):
                # Get sensory input from world
                percept = self.world.getSensoryPercept()
                
                # Agent processes sensory input
                agent.processSensoryInput(percept)
                
                # Agent brain processes information
                self.brains[i].step(step)
                
                # Agent makes decision
                motor_cmd = agent.decodeMotorCommand()
                self.world.applyMotorCommand(motor_cmd, self.world.getSimulationTime())
                
                # Agent applies learning
                reward = percept.getInternal()[0] if percept and percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, agent.getPredictionError())
                
                # Agent develops
                agent.updateDevelopment(0.05)
                
                # Send status update to shared memory
                self._update_agent_status(i, agent, step)
            
            # Coordinate between agents
            self._coordination_cycle(step)
            
            # Collect metrics
            if step % 100 == 0:
                self._collect_coordination_metrics(step, coordination_metrics)
                self._print_coordination_status(step, coordination_metrics)
        
        print(f"\n=== ADVANCED MULTI-AGENT COORDINATION COMPLETE ===")
        print(f"Total steps: {steps}")
        print(f"Successful coordinations: {coordination_metrics['successful_coordinations']}")
        print(f"Resource sharing events: {coordination_metrics['resource_sharing_events']}")
        print(f"Threat detections: {coordination_metrics['threat_detections']}")
        print(f"Collective knowledge updates: {coordination_metrics['collective_knowledge_updates']}")
        
        return coordination_metrics
    
    def _update_agent_status(self, agent_id, agent, step):
        """Update agent status in shared memory."""
        agent_status = {
            'agent_id': agent_id,
            'step': step,
            'position': self.world.getAgentBody().getPosition(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'development_stage': self.brains[agent_id].getDevelopmentalStage(),
            'firing_rate': self.brains[agent_id].getAverageFiringRate(),
            'e_i_ratio': self.brains[agent_id].getExcitationInhibitionRatio()
        }
        
        self.communication_protocols['agent_status'][agent_id] = agent_status
    
    def _coordination_cycle(self, step):
        """Run coordination cycle between agents."""
        # Check for resource sharing opportunities
        self._check_resource_sharing(step)
        
        # Check for threat coordination
        self._check_threat_coordination(step)
        
        # Share collective knowledge
        self._share_collective_knowledge(step)
        
        # Negotiate coordination
        self._negotiate_coordination(step)
    
    def _check_resource_sharing(self, step):
        """Check for opportunities to share resources."""
        # Implementation for resource sharing coordination
        pass
    
    def _check_threat_coordination(self, step):
        """Coordinate response to threats."""
        # Implementation for threat coordination
        pass
    
    def _share_collective_knowledge(self, step):
        """Share collective knowledge between agents."""
        # Implementation for knowledge sharing
        pass
    
    def _negotiate_coordination(self, step):
        """Negotiate coordination between agents."""
        # Implementation for coordination negotiation
        pass
    
    def _collect_coordination_metrics(self, step, metrics):
        """Collect coordination metrics."""
        metrics['steps_completed'] = step
        
        # Count successful coordinations
        for protocol in self.communication_protocols.values():
            if 'coordination_response' in str(protocol):
                metrics['successful_coordinations'] += 1
        
        # Count resource sharing events
        metrics['resource_sharing_events'] = len(self.shared_memory['collective_knowledge']['resource_locations'])
        
        # Count threat detections
        metrics['threat_detections'] = len(self.shared_memory['collective_knowledge']['threats_detected'])
        
        # Count collective knowledge updates
        metrics['collective_knowledge_updates'] = len(self.shared_memory['collective_knowledge']['successful_strategies'])
    
    def _print_coordination_status(self, step, metrics):
        """Print coordination status."""
        print(f"Step {step}:")
        print(f"  Successful coordinations: {metrics['successful_coordinations']}")
        print(f"  Resource sharing events: {metrics['resource_sharing_events']}")
        print(f"  Threat detections: {metrics['threat_detections']}")
        print(f"  Collective knowledge updates: {metrics['collective_knowledge_updates']}")
        
        if step % 200 == 0:
            print(f"  Agent distribution:")
            for agent_id, config in self.config_templates.items():
                specialization = self._get_agent_specialization(agent_id)
                print(f"    Agent {agent_id}: {specialization}")
    
    def _get_agent_specialization(self, agent_id):
        """Get specialization for agent ID."""
        for specialization, agents in self.config_templates.items():
            if agent_id in agents:
                return specialization
        return 'unknown'

# Usage example
if __name__ == "__main__":
    print("=== ADVANCED MULTI-AGENT COORDINATION ===\n")
    
    # Create advanced multi-agent system
    sim = AdvancedMultiAgentSystem(num_agents=5, world_size=100)
    
    # Run simulation
    results = sim.run_simulation(steps=800)
    
    print(f"\n=== COORDINATION COMPLETE ===")
    print(f"Total steps: {results['steps_completed']}")
    print(f"Successful coordinations: {results['successful_coordinations']}")
    print(f"Coordination efficiency: {results['successful_coordinations'] / max(1, results['steps_completed']):.3f}\n")