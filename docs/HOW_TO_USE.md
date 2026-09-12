# NLM - How to Use Guide

## Detailed Usage Instructions

This guide provides comprehensive instructions for using the NLM (熙然) computational brain project.

### Quick Start

1. **Install and Build**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   ```

2. **Run the Integration Test**
   ```bash
   ./nlm_phase6_demo
   ```

3. **For Python Usage**
   ```python
   import pynlm
   
   # Create a brain with default configuration
   brain = pynlm.createBrain(pynlm.createDefaultConfig())
   brain.initialize()
   
   # Run simulation
   for step in range(100):
       brain.step(step)
   ```

### Configuration System

NLM uses a configuration file system for customizing brain behavior:

**Configuration File Structure (`configs/default.cfg`):**

```
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

# Neuromodulation settings
dopamine_baseline = 0.1

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log
```

**Configuration Options:**
- `random_seed`: For reproducible experiments
- `simulation_timestep`: Time resolution
- `neuron_count`: Brain size
- `connection_probability`: Network connectivity
- Plasticity parameters control learning
- Neuromodulation settings affect motivation
- Environment parameters define the simulation space

### Brain Operations

#### Core Brain Methods

1. **`createBrain(config)`** - Creates a new brain instance
2. **`brain.initialize()`** - Initializes the brain (MUST be called before use)
3. **`brain.step(step_number)`** - Processes one time step
4. **`brain.getTotalNeuronCount()`** - Returns total neurons
5. **`brain.getFiringNeuronCount()`** - Returns currently firing neurons
6. **`brain.getTotalSpikeCount()`** - Returns total spikes fired
7. **`brain.getAverageFiringRate()`** - Returns average firing rate
8. **`brain.getDevelopmentalStage()`** - Returns current development stage

#### Agent System Operations

1. **`createAgentBrain(brain)`** - Creates agent interface
2. **`agent.initialize(world)`** - Links agent to world
3. **`agent.processSensoryInput(percept)`** - Processes sensory information
4. **`agent.decodeMotorCommand()`** - Selects action based on brain state
5. **`agent.enableRewardModulation(bool)`** - Enables reward learning
6. **`agent.enableCuriosity(bool)`** - Enables exploration
7. **`agent.enableStructuralPlasticity(bool)`** - Enables connection growth
8. **`agent.enableDevelopment(bool)`** - Enables developmental changes

#### World System Operations

1. **`createSimpleWorld()`** - Creates simulation environment
2. **`world.configure(width, height, visionWidth, visionHeight)`** - Sets environment size
3. **`world.reset()`** - Resets world state
4. **`world.update(time)`** - Advances simulation
5. **`world.getSensoryPercept()`** - Gets current sensory input
6. **`world.applyMotorCommand(action, time)`** - Executes action

### Complete Example: Full Simulation

```python
import pynlm

# 1. Setup configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Setup world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# 3. Setup agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# 5. Run simulation
for step in range(100):
    # Update world environment
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent
    agent.processSensoryInput(percept)
    
    # Brain thinks and processes
    brain.step(step)
    
    # Select action
    action = agent.decodeMotorCommand()
    
    # Apply action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print status every 20 steps
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Simulation complete!")
```

### Common Patterns

#### Pattern 1: Training a Brain
```python
def train_brain(config, steps):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(steps):
        brain.step(step)
    
    return brain
```

#### Pattern 2: Running in Environment
```python
def run_in_world(brain, world, steps):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain
```

#### Pattern 3: Monitoring Brain State
```python
def monitor_brain(brain, interval=10):
    for step in range(0, 1000, interval):
        brain.step(step)
        print(f"Step {step}:")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")
```

### Troubleshooting

**"My brain isn't doing anything"**
- Ensure `brain.initialize()` is called
- Try running more simulation steps
- Check configuration parameters

**"The agent isn't moving"**
- Ensure `world.applyMotorCommand()` is called
- Check that `world.update()` is in your loop
- Verify action selection logic

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - ensure sensory input connections exist
- Check neuromodulation settings

**"Simulation crashes"**
- Check memory limits and configuration values
- Ensure all required components are initialized
- Verify world configuration parameters

### Configuration Customization

To customize your brain:

1. **Modify default.cfg**:
   ```bash
   cp configs/default.cfg configs/custom.cfg
   # Edit custom.cfg with your values
   ```

2. **Create multiple configurations**:
   ```python
   # Use different configs for different experiments
   config_small = pynlm.createCustomConfig("configs/small.cfg")
   config_large = pynlm.createCustomConfig("configs/large.cfg")
   ```

### Advanced Features

#### Learning Features
- **Reward Modulation**: Learn from rewards and prediction errors
- **Curiosity**: Drive exploration of novel states
- **Structural Plasticity**: Grow new connections over time
- **Development**: Brain maturation with age-dependent changes

#### Memory Systems
- **Working Memory**: Maintain information temporarily
- **Episodic Memory**: Store experience sequences
- **Associative Memory**: Form pattern associations

#### Cognitive Systems
- **Prediction**: Anticipate future states
- **Planning**: Evaluate action sequences
- **Attention**: Focus on relevant information
- **Concept Formation**: Extract abstract patterns

### Performance Optimization

1. **Use larger timesteps** for faster simulations
2. **Reduce neuron count** for quick prototyping
3. **Disable learning** features for inference-only modes
4. **Use checkpoint system** to save/load brain states

### Python Bindings

The Python interface (`pynlm`) provides:
- `createBrain(config)`: Create brain instances
- `createDefaultConfig()`: Get default configuration
- `createSimpleWorld()`: Create simulation environment
- `createAgentBrain(brain)`: Create agent interface

### Next Steps

1. **Run the Phase 6 demo**: `./nlm_phase6_demo` in build directory
2. **Read `docs/ARCHITECTURE.md`**: Understand system design
3. **Explore `docs/SCIENCE.md`**: Learn scientific background
4. **Try `docs/EXPERIMENTS.md`**: See example experiments
5. **Customize configurations** for your use case
6. **Build for performance** using CMake optimizations

### Additional Resources

- **Documentation**: `docs/` directory contains detailed technical docs
- **Experiments**: `src/experiments/` contains example experiments
- **Tests**: `tests/` directory contains unit tests
- **Performance**: `src/performance/` contains optimizations

This guide covers the essentials for using NLM. For more detailed information, consult the technical documentation in the `docs/` directory.
