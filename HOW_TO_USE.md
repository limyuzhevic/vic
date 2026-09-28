# NLM - Advanced Python Usage Guide

**Comprehensive Guide to the NLM Python API**

## Quick Start

If you're familiar with the basics, here are the essential imports and setup:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create the brain
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Create world and agent for full simulation
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

## Core Concepts

NLM provides a complete brain simulation environment with the following key components:

### 1. **Brain**
- Neural network with spiking neurons
- Synaptic plasticity (learning)
- Memory systems
- Prediction and planning capabilities

### 2. **World**
- 2D environment simulation
- Objects, resources, hazards
- Sensory inputs (vision, touch, audio)
- Motor actions

### 3. **Agent**
- Interface between brain and world
- Sensory processing
- Motor command decoding
- Learning and development management

## Complete Brain-World Integration Example

This example demonstrates a complete autonomous agent in a world:

```python
import pynlm
import time

def create_agent_world_simulation(width=20, height=20):
    # Initialize brain with custom configuration
    config = pynlm.createDefaultConfig()
    
    # Configure brain parameters
    config.set("neuron_count", 500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=width, height=height, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(width//2, height//2)
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableAllFeatures()  # Enables reward modulation, structural plasticity, development, curiosity
    
    return brain, world, agent

def run_autonomous_agent(width=15, height=15, numSteps=1000):
    brain, world, agent = create_agent_world_simulation(width, height)
    
    print(f"Starting autonomous agent simulation in {width}x{height} world")
    print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    
    for step in range(numSteps):
        # Update world
        world.update(0.1)
        
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            stats = pynlm.getBrainStats(brain)
            print(f"Step {step}: {stats['firing_neurons']} neurons firing, "
                  f"growth rate: {agent.getCuriosityLevel():.3f}")
    
    print(f"\nSimulation complete after {numSteps} steps!")
    print(f"Final stats: {pynlm.getBrainStats(brain)}")
    return brain, world, agent

# Run the example
if __name__ == "__main__":
    brain, world, agent = run_autonomous_agent()
```

## Python API Reference

### Core Classes

#### 1. **Config**
Configuration management for the NLM system.

```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)  # Set parameter
config.has("neuron_count")  # Check if exists
config.get("neuron_count")  # Get parameter
config.saveToFile("my_config.json")  # Save to file
config.loadFromFile("config.json")  # Load from file
```

#### 2. **Brain**
The central neural processing unit.

```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(0)  # Process one simulation step

# Input processing
brain.receiveSensoryInput(percept)
brain.injectCurrent(neuron_id, current_value)

# Output processing
action = brain.produceAction()

# State inspection
stats = {
    "neurons": brain.getTotalNeuronCount(),
    "synapses": brain.getTotalSynapseCount(),
    "active": brain.getActiveNeuronCount(),
    "firing": brain.getFiringNeuronCount(),
    "firing_rate": brain.getAverageFiringRate(),
    "spikes": brain.getTotalSpikeCount(),
    "development": brain.getDevelopmentalStage(),
}

# Persistence
brain.save("brain_state.json")
brain.load("brain_state.json")
```

#### 3. **SimpleWorld**
2D environment simulation.

```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Update environment
world.update(0.1)  # Step forward 0.1 seconds

# Get observations
percept = world.getSensoryPercept()

# Execute actions
world.applyMotorCommand(action, world.getSimulationTime())

# Environment manipulation
world.addObject(world_object)
world.removeObject(x, y)
world.isValidPosition(x, y)
```

#### 4. **AgentBrain**
Bridge between brain and environment.

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor command decoding
action = agent.decodeMotorCommand()

# Learning and development
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(0.1)

# Feature control
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Monitoring
agent.getDevelopmentalStage()
agent.getNeuromodulationLevel()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
```

### Convenience Functions

#### 1. **Processing Utilities**

```python
# Process sensory input from vectors
pynlm.processSensoryPercept(agent, vision_data, touch_data, internal_data, audio_data, timestamp)

# Create action
action = pynlm.createAction(pynlm.ActionType.MoveForward)
action_with_params = pynlm.createAction(pynlm.ActionType.Custom, [0.5, 0.3])
```

#### 2. **Simulation Control**

```python
# Run complete simulation
pynlm.runSimulation(brain, world, agent, num_steps=1000)

# Get comprehensive brain stats
stats = pynlm.getBrainStats(brain)
# stats contains: neurons, synapses, active_neurons, firing_neurons, etc.
```

#### 3. **Configuration Helpers**

```python
# Create common configurations
default_config = pynlm.createDefaultConfig()
learning_config = pynlm.createLearningConfig()  # Enhanced for learning
fast_config = pynlm.createFastConfig()  # Optimized for speed
development_config = pynlm.createDevelopmentConfig()  # Focused on growth
```

## Advanced Examples

### 1. **Learning Agent with Reward System**

```python
class LearningAgent:
    def __init__(self, width=30, height=30):
        self.config = pynlm.createDefaultConfig()
        self.config.set("neuron_count", 800)
        self.config.set("connection_probability", 0.12)
        
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=width, height=height, visionWidth=10, visionHeight=10)
        self.world.reset()
        self.world.setAgentStart(width//2, height//2)
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableAllFeatures()
        
        self.reward_history = []
        self.step_count = 0
    
    def run_episode(self, max_steps=500):
        episode_reward = 0.0
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get percept and process
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(self.step_count)
            action = self.agent.decodeMotorCommand()
            
            # Execute and get result
            action_result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Learn from result
            self.agent.applyRewardModulation(
                action_result.reward, 
                self.agent.getPredictionError()
            )
            
            self.reward_history.append(action_result.reward)
            episode_reward += action_result.reward
            self.step_count += 1
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Check for termination
            if action_result.success and "goal" in str(action):
                break
                
                # Add occasional exploration via curiosity
            if step % 50 == 0:
                # Inject novel stimulus to encourage exploration
                internal = pynlm.InternalSignals()
                internal.addSignal(0.8)  # High novelty signal
                internal.setTimestamp(self.world.getSimulationTime())
                
                # Process novel input
                self.agent.processSensoryInput(internal)
                
                # Learn from novelty
                self.agent.applyRewardModulation(0.1, self.agent.getNoveltyLevel())
        
        return episode_reward, len(self.reward_history)
    
    def save_brain(self, filepath):
        self.brain.save(filepath)
    
    def load_brain(self, filepath):
        self.brain.load(filepath)
```

### 2. **Multi-Agent Environment**

```python
def create_multi_agent_env(num_agents=3, world_size=50):
    """Create a world with multiple interacting agents"""
    # Shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_size, height=world_size, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Multiple agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Each agent has its own brain
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 400)
        config.set("region_count", 2)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Position agents randomly
        start_x = (i * world_size // (num_agents + 1))
        start_y = world_size // 2
        world.setAgentStart(start_x, start_y)
        
        agents.append(agent)
        brains.append(brain)
    
    # Add resources to world
    for x in range(0, world_size, 10):
        for y in range(0, world_size, 10):
            if world.isValidPosition(x, y):
                obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 10.0)
                world.addObject(obj)
    
    return world, agents, brains

def run_multi_agent_simulation(world, agents, brains, steps=200):
    """Run simulation with multiple competing/cooperating agents"""
    for step in range(steps):
        world.update(0.1)
        
        # Each agent processes its own percept
        for i, agent in enumerate(agents):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain processes
            brains[i].step(step)
            action = agent.decodeMotorCommand()
            
            # Execute action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from interaction
            agent.applyRewardModulation(0.01, agent.getPredictionError())
    
    return world, agents, brains
```

### 3. **Brain Evolution Experiment**

```python
def evolve_brains(num_generations=5, population_size=10):
    """Run a simple evolutionary algorithm with NLM brains"""
    generation_stats = []
    
    for generation in range(num_generations):
        print(f"\n=== Generation {generation + 1}/{num_generations} ===")
        
        # Create population
        population = []
        for i in range(population_size):
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", 300)
            config.set("connection_probability", 0.08 + (i * 0.002))  # Varying connectivity
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Add small random variation
            if i % 3 == 0:
                # Higher development
                agent = pynlm.createAgentBrain(brain)
                agent.enableDevelopment(True)
                agent.enableCuriosity(True)
            
            population.append(brain)
        
        # Evaluate each brain
        generation_scores = []
        for i, brain in enumerate(population):
            # Create world and agent for evaluation
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=6, visionHeight=6)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            agent.enableAllFeatures()
            
            # Run evaluation episode
            score = 0
            for step in range(100):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Simple score: based on actions taken and learning
                score += agent.getNeuromodulationLevel() * 10
                
                # Add curiosity bonus for exploration
                if agent.getCuriosityLevel() > 0.5:
                    score += 50
            
            generation_scores.append(score)
            print(f"  Agent {i}: Score = {score}")
        
        # Create next generation (simple reproduction)
        avg_score = sum(generation_scores) / len(generation_scores)
        print(f"  Average score: {avg_score:.1f}")
        
        generation_stats.append({
            'generation': generation + 1,
            'avg_score': avg_score,
            'best_score': max(generation_scores),
            'scores': generation_scores
        })
        
        # Save best brain for potential later use
        best_idx = generation_scores.index(max(generation_scores))
        population[best_idx].save(f"best_brain_gen{generation + 1}.json")
    
    return generation_stats
```

## Command Line Interface

NLM includes a command-line interface for batch processing:

### Basic Usage

```bash
# Run a simple brain simulation
python -m pynlm.examples.basic_sim --neurons 500 --steps 1000

# Run with custom configuration
python -m pynlm.examples.basic_sim --config my_config.json --steps 2000

# Run learning experiment
python -m pynlm.examples.learning_exp --learning_rate 0.1 --curiosity

# Multi-environment testing
python -m pynlm.examples.multi_env --width 30 --height 30 --agents 5
```

### Available Scripts

1. **`basic_sim.py`** - Basic brain simulation with visualization
2. **`learning_exp.py` - Learning and plasticity experiments
3. **`multi_agent.py` - Multi-agent environment simulation
4. **`evolve_brains.py` - Evolutionary algorithm with NLM brains
5. **`benchmark.py` - Performance benchmarking

## Installation and Setup

### Installation

```bash
pip install pynlm

# Or from source
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
pip install python/
```

### Development Setup

```bash
# Install development dependencies
pip install -e .[dev]

# Run tests
pytest tests/

# Build documentation
mkdocs serve
```

## Performance Optimization

### Tips for Better Performance

1. **Use Batch Processing**
```python
# Process multiple steps in batch
for batch_start in range(0, total_steps, 100):
    batch_end = min(batch_start + 100, total_steps)
    for step in range(batch_start, batch_end):
        brain.step(step)
```

2. **Memory Management**
```python
# Clear references when done
import gc
gc.collect()  # Force garbage collection
```

3. **Configuration Optimization**
```python
# Use memory-efficient configuration
config = pynlm.createFastConfig()
config.set("neuron_count", 1000)  # Keep reasonable size
```

4. **Parallel Processing**
```python
from concurrent.futures import ThreadPoolExecutor

# Process multiple independent simulations
with ThreadPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(run_simulation, i) for i in range(10)]
    results = [f.result() for f in futures]
```

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
- Did you call `brain.initialize()`? Always initialize before use.
- Check that sensory input is properly connected.
- Try increasing the number of simulation steps.

**"The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Ensure `world.update()` is being called.
- Check that the brain's motor neurons are active.

**"Everything is 0"
- Brains need time to "warm up" - try more steps (300-1000).
- Some neurons need input to fire - make sure sensory input is connected.
- Check configuration parameters like connection probability.

**"Memory usage too high"
- Reduce neuron count for testing.
- Clear large objects when no longer needed.
- Use `config.set("neuron_count", smaller_number)`

**"Python bindings not found"
- Make sure pynlm is properly installed: `pip install pynlm`
- Check that the module is importable: `python -c "import pynlm; print(pynlm.__version__)"`

### Advanced Debugging

```python
def debug_simulation(brain, world, agent, steps=100):
    """Debug a simulation with detailed output"""
    print("=== DEBUG SIMULATION ===")
    print(f"Initial brain state: {pynlm.getBrainStats(brain)}")
    
    for step in range(steps):
        world.update(0.1)
        
        # Monitor each component
        percept = world.getSensoryPercept()
        print(f"Step {step}: Vision range = {len(percept.getVision())}")
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        print(f"  Action: {action.getType()}, Params: {action.getParameters()}")
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Check for abnormalities
        if brain.getFiringNeuronCount() == 0 and step > 10:
            print(f"  WARNING: No firing neurons at step {step}")
        
        if agent.getNeuromodulationLevel() < 0.01:
            print(f"  WARNING: Low neuromodulation at step {step}")
    
    print(f"Final brain state: {pynlm.getBrainStats(brain)}")
```

## API Changes

### Version 0.1.0
- Initial release with basic brain functionality
- Limited Python bindings
- Simple world simulation

### Version 0.2.0 (Upcoming)
- Enhanced Python API with convenience functions
- Improved documentation
- Advanced examples and utilities
- Better error handling and debugging tools

## Future Enhancements

### Planned Features
1. **Enhanced Learning**
   - Advanced plasticity rules
   - Multi-modal learning
   - Transfer learning capabilities

2. **Advanced World Models**
   - Physics-based simulation
   - 3D environments
   - Dynamic object properties

3. **Improved Visualization**
   - Real-time brain state visualization
   - Network graphs
   - Activity heatmaps

4. **Distributed Computing**
   - Multi-process simulations
   - Cloud-based computation
   - GPU acceleration

### Contribution Guidelines

We welcome contributions to improve NLM! Here are some ways to help:

1. **Add new Python bindings** for existing C++ functionality
2. **Create examples** demonstrating advanced usage
3. **Improve documentation** with more detailed explanations
4. **Add tests** for edge cases and new features
5. **Optimize performance** for larger simulations
6. **Fix bugs** and improve error handling
7. **Port to new platforms** or add new compilers

### Contact

For questions, suggestions, or issues:
- GitHub: [nlm-project/nlm](https://github.com/nlm-project/nlm)
- Documentation: [nlm.readthedocs.io](https://nlm.readthedocs.io)
- Issues: Submit on GitHub
- Discussions: Join the project discussions

---

*This guide is continuously updated. Check for the latest version in the documentation.*

Version: 0.2.0-alpha
Last updated: $(date +%Y-%m-%d)
