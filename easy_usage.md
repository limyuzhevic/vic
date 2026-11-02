# NLM - Neural Learning Machine Documentation

This is a comprehensive documentation for the NLM (Neural Learning Machine) framework, a Python/C++ neural simulation platform.

## Overview

NLM is a brain simulator for computers that lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

**Key Concepts:**
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

The 3 Things You Need to Know:
1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make the brain think for 100 steps
for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

## Core Components

### 1. Brain

The `Brain` class is the central neural simulation component. It contains neurons, synapses, regions, and learning mechanisms.

**Key Methods:**
- `initialize()` - Start up the brain with configuration
- `step(step_number)` - Process one moment in time
- `receiveSensoryInput(input)` - Inject sensory information
- `produceAction()` - Produce motor action based on neural activity

**Properties:**
- `totalNeuronCount` - Total neurons across all regions
- `firingNeuronCount` - Currently firing neurons
- `totalSynapseCount` - Total connections
- `averageFiringRate` - Average firing rate across all neurons

**Pythonic Access:**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Pythonic access
print(f"Brain has {brain.totalNeuronCount} neurons")
print(f"Currently firing: {brain.firingNeuronCount}")
print(f"Total spikes: {brain.totalSpikeCount}")
print(f"Firing rate: {brain.averageFiringRate:.2f} Hz")
```

### 2. Configuration

The `Config` class allows you to customize brain behavior through key-value pairs.

**Configuration Types:**
- **Integer** (`int`, `int64_t`)
- **Floating-point** (`double`, `float`)
- **Boolean** (`bool`)
- **String** (`std::string`)
- **Lists** (`std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`)

**Configuration Sources:**
- **Default** - Built-in defaults
- **File** - JSON/YAML files
- **Command Line** - Arguments passed to the program
- **Runtime** - Set during execution

**Pythonic Configuration:**
```python
import pynlm

# Create config
config = pynlm.createDefaultConfig()

# Set values
config.set_int("neuron_count", 1000)
config.set_float("learning_rate", 0.01)
config.set_bool("enable_plasticity", True)
config.set_string("model_name", "test_model")

# Get values
neurons = config.get_int("neuron_count", 100)  # Returns 100 if missing
rate = config.get_float("learning_rate") or 0.001  # Default if missing

# Save/Load
config.saveToFile("config.json")
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("config.json")

# Dictionary interface (Pythonic)
config_dict = {
    "neuron_count": 1000,
    "learning_rate": 0.01,
    "enable_plasticity": True
}
config.from_dict(config_dict)

# Convert to dict
config_data = config.to_dict()
```

### 3. World

The `SimpleWorld` class simulates the environment in which the agent operates.

**Key Features:**
- 2D grid-based environment
- Objects with types (Resource, Hazard, Wall, Marker)
- Agent movement and interactions
- Time-based updates

**World Types:**
- `WorldObjectType::Resource` - Positive value
- `WorldObjectType::Hazard` - Negative value
- `WorldObjectType::Wall` - Physical barrier
- `WorldObjectType::Marker` - Special location

**Pythonic World Usage:**
```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Set agent starting position
world.setAgentStart(10, 10)

# Add objects
world.add_resource(5, 5, 10.0)  # Resource at (5,5) with value 10
world.add_hazard(15, 15, -5.0)   # Hazard at (15,15) with value -5
world.add_wall(3, 3, 0.8)        # Wall at (3,3) with radius 0.8

# Check valid positions
if world.isValidPosition(5, 5):
    print("Position (5,5) is valid")

# Run simulation
for step in range(100):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
```

### 4. Agent

The `AgentBrain` class bridges between the brain and the world, processing sensory input and producing motor commands.

**Key Features:**
- Sensory input processing
- Motor command decoding
- Reward-based learning
- Development system
- Neuromodulation

**Pythonic Agent Usage:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

# Run agent with learning
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get status periodically
    if step % 20 == 0:
        status = agent.get_status()
        print(f"Step {step}: Stage={status['developmental_stage']}, "
              f"Curiosity={status['curiosity_level']:.2f}")
```

## Learning Patterns

### Pattern 1: Complete Agent

```python
def run_complete_agent(brain, world, agent, num_steps):
    """Complete agent simulation with learning"""
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply learning
        reward = 0.1  # Simple reward
        agent.applyRewardModulation(reward, 0.0)
    
    return {
        'final_neurons': brain.getTotalNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'learning_rate': agent.getNeuromodulationLevel(),
        'curiosity': agent.getCuriosityLevel()
    }

# Use it
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
agent = pynlm.createAgentBrain(brain)

results = run_complete_agent(brain, world, agent, 500)
print(f"Results: {results}")
```

### Pattern 2: Silent Brain Test

```python
def test_silent_brain():
    """Test brain without sensory input"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for i in range(20):
        brain.step(i)
        if i % 5 == 0:
            print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Final stats: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSpikeCount()} total spikes")
```

### Pattern 3: World Watching Agent

```python
def watch_world():
    """Agent watches world without interacting"""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Watch the world for 30 steps
    for i in range(30):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(i)
        
        if i % 10 == 0:
            firing = brain.getFiringNeuronCount()
            rate = brain.getAverageFiringRate()
            print(f"Step {i}: {firing} neurons firing, rate {rate:.2f} Hz")
    
    return brain.getAverageFiringRate()
```

## Advanced Features

### 1. Configuration with JSON

```python
import pynlm
import json

# Create config with JSON
config_data = {
    "brain": {
        "neuron_count": 2000,
        "learning_rate": 0.005,
        "plasticity_enabled": True
    },
    "world": {
        "width": 50,
        "height": 50,
        "vision_width": 16,
        "vision_height": 16
    },
    "agent": {
        "enable_curiosity": True,
        "enable_development": True,
        "reward_decay": 0.95
    }
}

config = pynlm.create_default_config_from_dict(config_data)
brain = pynlm.create_brain(config)
```

### 2. Batch Processing

```python
def batch_process(brain, world, agent, scenarios):
    """Process multiple scenarios"""
    results = []
    
    for scenario_id, config in enumerate(scenarios):
        print(f"Processing scenario {scenario_id}")
        
        # Create fresh brain for each scenario
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Run scenario
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Save results
        results.append({
            'scenario': scenario_id,
            'neurons': brain.getTotalNeuronCount(),
            'spikes': brain.getTotalSpikeCount(),
            'final_rate': brain.getAverageFiringRate(),
            'learning': agent.getNeuromodulationLevel()
        })
        
        # Reset for next scenario
        brain.reset()
        agent.reset()
        world.reset()
    
    return results
```

### 3. Performance Monitoring

```python
def benchmark_performance(brain, world, agent, duration):
    """Benchmark performance over time"""
    import time
    
    start_time = time.time()
    steps_completed = 0
    
    while time.time() - start_time < duration:
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(steps_completed)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        steps_completed += 1
        
        if steps_completed % 100 == 0:
            # Get performance stats from logger
            logger = nlm.Logger.getGlobal()
            perf_stats = logger.getPerformanceStats()
            print(f"Step {steps_completed}: {perf_stats.logCount} logs, "
                  f"{perf_stats.processingTime:.2f} ms processed")
    
    return steps_completed
```

### 4. Debugging and Monitoring

```python
import pynlm
import time

def debug_agent(brain, world, agent, steps_to_debug):
    """Debug agent with detailed logging"""
    brain.initialize()
    world.reset()
    agent.initialize(world)
    
    # Enable detailed logging
    logger = nlm.Logger.getGlobal()
    
    # Monitor specific categories
    logger.setLevel(nlm.LogLevel::Debug)
    
    for step in range(steps_to_debug):
        start_time = time.time()
        
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Debug info
        if step % 50 == 0:
            debug_info = {
                'step': step,
                'time': time.time(),
                'processing_time': time.time() - start_time,
                'neurons': brain.getTotalNeuronCount(),
                'firing': brain.getFiringNeuronCount(),
                'spikes': brain.getTotalSpikeCount(),
                'action_type': str(action.getType()),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'curiosity': agent.getCuriosityLevel()
            }
            print(f"Debug {step}: {debug_info}")
        
        # Check for issues
        if brain.getTotalSynapseCount() == 0:
            nlm.log_warning("No synapses created - may indicate connectivity issue")
        
        if agent.getPredictionError() > 0.5:
            nlm.log_error("High prediction error - agent may be confused")
    
    return step
```

## Error Handling

### Common Errors and Solutions

**Error:** "Brain isn't doing anything"
**Solution:**
- Make sure to call `brain.initialize()`
- Check that sensory input is being provided
- Increase simulation steps

**Error:** "The agent isn't moving"
**Solution:**
- Ensure `world.applyMotorCommand()` is being called
- Check that `world.update()` is being called
- Verify that the brain is producing motor commands

**Error:** "Everything is 0"
**Solution:**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that connections are being established

### Robust Error Handling

```python
def robust_simulation(brain, world, agent, max_steps):
    """Robust simulation with error handling"""
    try:
        brain.initialize()
        world.reset()
        agent.initialize(world)
        
        for step in range(max_steps):
            try:
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Check for issues
                if brain.getTotalNeuronCount() == 0:
                    nlm.log_warning("No neurons created")
                
                if brain.getTotalSynapseCount() == 0:
                    nlm.log_warning("No synapses created")
                
            except Exception as e:
                nlm.log_error(f"Error at step {step}: {e}")
                # Continue simulation despite errors
                continue
                
    except Exception as e:
        nlm.log_critical(f"Fatal error: {e}")
        return False
    
    return True
```

## Migration Guide

### From Phase 1 to Phase 2

**Phase 1:** Simple key=value config format
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.txt")  # Simple format
```

**Phase 2:** JSON/YAML config format
```python
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")  # JSON format
```

### From C++ to Python

**C++ (old way):**
```cpp
nlm::Config config;
config.loadFromFile("config.txt");
int neurons = config.get<int>("neuron_count");
```

**Python (new way):**
```python
import pynlm
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
neurons = config.get_int("neuron_count", 100)
```

### Best Practices

1. **Always initialize** - Call `brain.initialize()` before use
2. **Use context managers** - Use `with` statements where possible
3. **Error handle gracefully** - Use try/except blocks
4. **Monitor performance** - Use the logging system for performance monitoring
5. **Use descriptive variable names** - Make code self-documenting
6. **Validate inputs** - Check configuration values before use
7. **Log important events** - Use structured logging for debugging
8. **Use Pythonic APIs** - Leverage `to_dict()`, `from_dict()`, and other convenience methods

## Troubleshooting

### "ImportError: cannot import name 'pynlm'"
**Solution:**
```bash
pip install pynlm
```

### "ModuleNotFoundError: No module named 'nlohmann'"
**Solution:**
```bash
pip install nlohmann-json
```

### Slow Performance
**Solution:**
- Use simpler configurations
- Reduce simulation steps
- Disable unnecessary features
- Monitor performance with the logging system

### Memory Issues
**Solution:**
- Use smaller brain configurations
- Monitor memory usage
- Clear unused objects
- Use the logging system to track memory usage

### Visualization Issues
**Solution:**
- Check that visualization dependencies are installed
- Use simpler display formats
- Check system requirements

## Further Reading

1. **NLM Architecture Documentation** - `docs/ARCHITECTURE.md`
2. **Phase Documentation** - `docs/PHASES.md`
3. **API Reference** - Generated documentation
4. **Examples Gallery** - `examples/` directory
5. **Research Papers** - Refer to NLM research publications

## Support

For support, visit the NLM GitHub repository or contact the development team.

---

*Last updated: November 2, 2026*
*NLM Version: 0.1.0*
*Python Bindings: 1.0.0*