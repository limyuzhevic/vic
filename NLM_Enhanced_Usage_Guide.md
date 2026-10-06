# NLM - Enhanced Usage Guide with Advanced Features

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project that creates a neural system beginning in a primitive developmental state and acquiring increasingly complex abilities through interaction with an environment.

## Quick Start: The Essential Guide

This guide provides both **basic** and **advanced** usage patterns. Start with the quick start, then explore the advanced sections as you become comfortable.

---

## Part 1: Essential Setup (Getting Started)

### 1.1 Basic One-Minute Setup

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### 1.2 The Three-Component Pattern

Every meaningful NLM interaction involves three components:

```python
import pynlm

# 1. Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# 2. Create agent
agent = pynlm.createAgentBrain(brain)

# 3. Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("Essential components ready!")
```

---

## Part 2: Complete Simulation Patterns

### 2.1 Standard Agent Simulation

```python
def run_simulation(brain, world, agent, num_steps=1000):
    """Standard agent simulation pattern"""
    
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
        
        # 7. Learn from results (optional)
        if step % 10 == 0:
            # Extract reward from internal signals
            internal = percept.getInternal()
            reward = internal[0] if internal else 0.0
            agent.applyRewardModulation(reward, 0.0)

# Use it:
# config = pynlm.createDefaultConfig()
# brain = pynlm.createBrain(config)
# agent = pynlm.createAgentBrain(brain)
# world = pynlm.createSimpleWorld()
# agent.initialize(world)
# run_simulation(brain, world, agent, 1000)
```

### 2.2 Development Research Pattern

```python
def research_developmental_effects(config=None):
    """Study how development affects brain behavior"""
    
    # Test different developmental stages
    stages = [
        pynlm.DevelopmentalStage.Initial,
        pynlm.DevelopmentalStage.CriticalPeriod,
        pynlm.DevelopmentalStage.Maturation,
        pynlm.DevelopmentalStage.Adult
    ]
    
    results = []
    
    for stage in stages:
        # Create brain with this stage
        cfg = config or pynlm.createDefaultConfig()
        brain = pynlm.createBrain(cfg)
        
        # Set developmental stage
        brain.setDevelopmentalStage(stage)
        
        # Initialize with monitoring
        brain.initialize()
        
        # Track statistics over time
        spikes_history = []
        plasticity_history = []
        
        for step in range(100):
            brain.step(step)
            spikes_history.append(brain.getFiringNeuronCount())
            
            if step % 10 == 0:
                # Record development stage
                plasticity_history.append(brain.getDevelopmentalStage())
        
        results.append({
            'stage': stage,
            'avg_spikes': sum(spikes_history) / len(spikes_history),
            'final_stage': plasticity_history[-1] if plasticity_history else stage
        })
        
        print(f"Stage {stage}: {results[-1]['avg_spikes']:.1f} avg firing neurons")
    
    return results

# Run development research
research_developmental_effects()
```

### 2.3 Memory Learning Pattern

```python
def study_memory_formation(config=None):
    """Study how memory systems work"""
    
    # Create brain with monitoring
    cfg = config or pynlm.createDefaultConfig()
    brain = pynlm.createBrain(cfg)
    brain.initialize()
    
    # Create world with interesting patterns
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Add interesting objects to world
    world.addObject(pynlm.WorldObject(2, 3, pynlm.WorldObjectType.Resource, value=1.0))
    world.addObject(pynlm.WorldObject(7, 5, pynlm.WorldObjectType.Resource, value=2.0))
    world.addObject(pynlm.WorldObject(4, 8, pynlm.WorldObjectType.Hazard, value=-1.0))
    
    # Run simulation to form memories
    print("Running simulation to form memories...")
    
    for step in range(500):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        agent.decodeMotorCommand()
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 100 == 0:
            # Monitor memory systems
            working_mem = brain.getWorkingMemory()
            if working_mem:
                print(f"  Step {step}: {working_mem.getActiveTraces()} working memory traces")
            
            episodic_mem = brain.getEpisodicMemory()
            if episodic_mem:
                print(f"  Step {step}: {episodic_mem.getEpisodeCount()} episodic memories")
    
    print("Memory formation study complete!")
    
    # Export memory data for analysis
    if episodic_mem:
        for episode in episodic_mem.getAllEpisodes():
            print(f"Episode {episode.timestamp}: {len(episode.activeNeurons)} active neurons")

# Study memory
study_memory_formation()
```

---

## Part 3: Advanced Features

### 3.1 Plasticity Research

```python
def study_plasticity(config=None):
    """Research different plasticity mechanisms"""
    
    # Test different plasticity configurations
    configs = [
        {'stdp': True, 'hebbian': False, 'reward': False},
        {'stdp': False, 'hebbian': True, 'reward': False},
        {'stdp': True, 'hebbian': True, 'reward': False},
        {'stdp': True, 'hebbian': True, 'reward': True}
    ]
    
    for i, plasticity_config in enumerate(configs):
        print(f"\n=== Plasticity Configuration {i+1}: {plasticity_config} ===")
        
        # Create brain
        cfg = config or pynlm.createDefaultConfig()
        brain = pynlm.createBrain(cfg)
        brain.initialize()
        
        # Enable/disable plasticity based on config
        # Note: In current implementation, this is done via config file
        # This is just for demonstration
        
        # Apply correlated input patterns to test plasticity
        print("Applying correlated input patterns...")
        
        # Create correlated pre-post spikes
        for trial in range(20):
            # Fire pre-synaptic neuron
            brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
            brain.step(trial * 2)
            
            # Fire post-synaptic neuron
            brain.injectCurrentToNeurons(pynlm.NeuronType.Motor, 50.0f)
            brain.step(trial * 2 + 1)
        
        # Measure synaptic changes
        final_weight = 0.0
        weight_count = 0
        
        for region in brain.getRegions():
            for synapse in region.getSynapses():
                final_weight += synapse.getWeight()
                weight_count += 1
        
        avg_weight = final_weight / weight_count if weight_count > 0 else 0
        print(f"Average synaptic weight: {avg_weight:.4f}")
        
        # Reset for next configuration
        brain.reset()
    
    print("\nPlasticity research complete!")

# Study plasticity
study_plasticity()
```

### 3.2 Neuromodulation Research

```python
def study_neuromodulation(config=None):
    """Research how neuromodulators affect learning"""
    
    # Test different neuromodulation levels
    neuromod_levels = [0.0, 0.3, 0.6, 1.0]
    
    print("Studying neuromodulation effects on learning...")
    
    for dopamine_level in neuromod_levels:
        print(f"\n=== Dopamine Level: {dopamine_level} ===")
        
        # Create brain and agent
        cfg = config or pynlm.createDefaultConfig()
        
        # Configure dopamine
        cfg.set("neuromod.dopamine.scale", dopamine_level)
        
        brain = pynlm.createBrain(cfg)
        agent = pynlm.createAgentBrain(brain)
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Run simulation with reward signals
        rewards = []
        
        for step in range(200):
            # Apply reward signal
            reward = 0.5 if step % 50 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update world
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            
            rewards.append(reward)
        
        # Measure dopamine effects
        final_dopamine = agent.getNeuromodulationLevel()
        curiosity_level = agent.getCuriosityLevel()
        
        print(f"  Final dopamine: {final_dopamine:.3f}")
        print(f"  Final curiosity: {curiosity_level:.3f}")
        print(f"  Total rewards received: {sum(rewards)}")
    
    print("\nNeuromodulation research complete!")

# Study neuromodulation
study_neuromodulation()
```

### 3.3 Development Research

```python
def study_development(config=None):
    """Research developmental stages and their effects"""
    
    print("Studying developmental stages...")
    
    # Create brains at different developmental stages
    stages = [
        (pynlm.DevelopmentalStage.Initial, "High plasticity"),
        (pynlm.DevelopmentalStage.CriticalPeriod, "Moderate plasticity"),
        (pynlm.DevelopmentalStage.Maturation, "Lower plasticity"),
        (pynlm.DevelopmentalStage.Adult, "Stable performance")
    ]
    
    for stage, description in stages:
        print(f"\n=== {stage} ({description}) ===")
        
        # Create brain
        cfg = config or pynlm.createDefaultConfig()
        brain = pynlm.createBrain(cfg)
        
        # Set developmental stage
        brain.setDevelopmentalStage(stage)
        
        # Initialize and run developmental simulation
        brain.initialize()
        
        # Simulate development over time
        plasticity_changes = []
        
        for day in range(50):  # 50 "days" of development
            # Simulate one day of development
            for step in range(20):  # 20 steps per day
                brain.step(step + day * 20)
            
            # Record plasticity level
            developmental_stage = brain.getDevelopmentalStage()
            
            # Calculate plasticity factor based on stage
            plasticity_factor = 1.0
            if developmental_stage == pynlm.DevelopmentalStage.CriticalPeriod:
                plasticity_factor = 0.8
            elif developmental_stage == pynlm.DevelopmentalStage.Maturation:
                plasticity_factor = 0.5
            elif developmental_stage == pynlm.DevelopmentalStage.Adult:
                plasticity_factor = 0.2
            
            plasticity_changes.append(plasticity_factor)
            
            # Log developmental stage
            print(f"  Day {day + 1}: {developmental_stage} (plasticity: {plasticity_factor:.2f})")
        
        # Calculate statistics
        avg_plasticity = sum(plasticity_changes) / len(plasticity_changes)
        final_plasticity = plasticity_changes[-1]
        
        print(f"  Average plasticity: {avg_plasticity:.2f}")
        print(f"  Final plasticity: {final_plasticity:.2f}")
        
        # Reset for next stage
        brain.reset()
    
    print("\nDevelopment research complete!")

# Study development
study_development()
```

---

## Part 4: API Reference

### 4.1 Core Brain Operations

#### Creation and Initialization

```python
# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize brain (ALWAYS call this before using)
brain.initialize()

# Run one step of simulation
brain.step(current_step)

# Reset brain to initial state
brain.reset()
```

#### State Management

```python
# Save brain state to file
brain.save("checkpoint.bin")

# Load brain state from file
brain.load("checkpoint.bin")

# Get developmental stage
stage = brain.getDevelopmentalStage()

# Set developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
```

#### Statistics and Monitoring

```python
# Get basic statistics
total_neurons = brain.getTotalNeuronCount()
total_synapses = brain.getTotalSynapseCount()
active_neurons = brain.getActiveNeuronCount()
firing_neurons = brain.getFiringNeuronCount()
avg_firing_rate = brain.getAverageFiringRate()
ei_ratio = brain.getExcitationInhibitionRatio()

# Get total spike count
total_spikes = brain.getTotalSpikeCount()

# Get region information
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### 4.2 Agent Operations

```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)

# Process sensory input
agent.processSensoryInput(percept)

# Decode motor command
motor_cmd = agent.decodeMotorCommand()

# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)

# Get statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

### 4.3 World Operations

```python
# Create simple world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control world
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Set agent starting position
world.setAgentStart(x, y)

# Apply motor command
result = world.applyMotorCommand(motor_cmd, current_time)

# Get sensory percept
percept = world.getSensoryPercept()

# Get agent body
body = world.getAgentBody()

# World object management
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

### 4.4 Configuration Operations

```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Get configuration keys
keys = config.getKeys()

# Check if key exists
if config.has("brain.neuron_count"):
    print("Config has neuron count")

# Get configuration value
neuron_count = config.get("brain.neuron_count")

# Set configuration value
config.set("brain.neuron_count", 1000)

# Set multiple values at once
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("brain.connection_probability", 0.05)

# Load from file
config.loadFromFile("config.json")

# Save to file
config.saveToFile("config.json")
```

### 4.5 Enumerations

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
pynlm.ActionType.MoveBackward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Drink
pynlm.ActionType.Rest

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 5: Error Handling and Troubleshooting

### 5.1 Common Issues

#### "My brain isn't doing anything"

**Problem**: Brain appears inactive or stuck.

**Solution**:
```python
# Check if initialized
if brain is None:
    print("ERROR: Brain not created")
    
if not brain_initialized:
    print("ERROR: Brain.initialize() not called")
    brain.initialize()  # Always call this first!

# Check if simulation is running
brain.step(0)  # Run at least one step

# Increase simulation time
for step in range(1000):
    brain.step(step)
```

#### "The agent isn't moving"

**Problem**: Agent produces no motor commands.

**Solution**:
```python
# Check agent initialization
agent = pynlm.createAgentBrain(brain)
if not agent_initialized:
    agent.initialize(world)  # Initialize with world!

# Check motor neuron activity
# Add debugging to see motor neuron firing
for step in range(100):
    brain.step(step)
    cmd = agent.decodeMotorCommand()
    print(f"Step {step}: {cmd}")  # See what commands are produced
```

#### "Everything is 0"

**Problem**: All values are zero or empty.

**Solution**:
```python
# Brains need time to "warm up" - try more steps
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

# Some neurons need input to fire - make sure sensory input is connected
agent.processSensoryInput(world.getSensoryPercept())
```

### 5.2 Best Practices

#### 1. Always Initialize
```python
# NEVER forget to initialize!
brain = pynlm.createBrain(config)
brain.initialize()  # REQUIRED!
```

#### 2. Use Consistent Timesteps
```python
# Use consistent timesteps throughout simulation
default_timestep = 0.1

for step in range(100):
    world.update(default_timestep)
    brain.step(step)
    # ... other operations
```

#### 3. Reset Between Episodes
```python
# Reset for new episode
brain.reset()
agent.reset()
world.reset()
```

#### 4. Monitor Progress
```python
# Always monitor simulation progress
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average rate: {brain.getAverageFiringRate():.2f}")
```

---

## Part 6: Performance Tips

### 6.1 Speed Optimizations

#### 1.1 Configure for Performance
```python
# Create efficient configuration
config = pynlm.createDefaultConfig()

# Optimize for speed
config.set("brain.neuron_count", 1000)  # Fewer neurons = faster
config.set("brain.region_count", 1)      # Fewer regions = better cache locality
config.set("brain.connection_probability", 0.1)  # Lower density = fewer connections
```

#### 1.2 Use Appropriate Simulation Length
```python
# Start with shorter simulations for testing
short_sim_steps = 100

for episode in range(10):
    for step in range(short_sim_steps):
        brain.step(step)
    brain.reset()
```

### 6.2 Memory Management

#### 2.1 Monitor Memory Usage
```python
# Monitor memory systems
working_mem = brain.getWorkingMemory()
if working_mem:
    traces = working_mem.getActiveTraces()
    print(f"Working memory traces: {traces}")

episodic_mem = brain.getEpisodicMemory()
if episodic_mem:
    episodes = episodic_mem.getEpisodeCount()
    print(f"Episodic memory episodes: {episodes}")
```

#### 2.2 Save Checkpoint Data
```python
# Save simulation state for long runs
if step % 1000 == 0:
    brain.save(f"checkpoint_step_{step}.bin")

# Resume from checkpoint
brain.load("checkpoint_step_5000.bin")
```

---

## Part 7: Integration with Other Systems

### 7.1 Multiple Agents

```python
def simulate_multiple_agents(config=None):
    """Simulate multiple agents sharing the same world"""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    # Create multiple brains and agents
    agents = []
    
    for i in range(3):
        # Create brain
        cfg = config or pynlm.createDefaultConfig()
        brain = pynlm.createBrain(cfg)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize with shared world
        agent.initialize(world)
        
        agents.append({
            'brain': brain,
            'agent': agent,
            'id': i
        })
    
    # Run simulation with multiple agents
    for step in range(500):
        world.update(0.1)
        
        # Each agent sees same world but may act differently
        for agent_data in agents:
            agent = agent_data['agent']
            
            # Process world percept
            agent.processSensoryInput(world.getSensoryPercept())
            
            # Brain thinks
            agent_data['brain'].step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from result
            reward = 0.1 if step % 100 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Reset world for next timestep
        world.reset()
    
    return agents

# Simulate multiple agents
agents = simulate_multiple_agents()
```

### 7.2 Batch Processing

```python
def batch_simulation(brain_configs, num_episodes=10):
    """Run multiple simulations with different configurations"""
    
    results = []
    
    for i, config in enumerate(brain_configs):
        print(f"Running batch simulation {i+1}/{len(brain_configs)}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run multiple episodes
        episode_results = []
        
        for episode in range(num_episodes):
            # Reset brain for new episode
            brain.reset()
            
            # Run episode
            episode_spikes = []
            
            for step in range(200):
                brain.step(step)
                episode_spikes.append(brain.getFiringNeuronCount())
            
            # Calculate episode statistics
            avg_spikes = sum(episode_spikes) / len(episode_spikes)
            
            episode_results.append({
                'episode': episode + 1,
                'avg_firing': avg_spikes,
                'max_firing': max(episode_spikes),
                'total_spikes': sum(episode_spikes)
            })
        
        # Add to batch results
        results.append({
            'config_index': i,
            'episodes': episode_results,
            'config': config
        })
    
    return results

# Run batch simulation
brain_configs = [
    pynlm.createDefaultConfig(),
    pynlm.createDefaultConfig(),
    pynlm.createDefaultConfig()
]

batch_results = batch_simulation(brain_configs)
```

---

## Part 8: Advanced Examples

### 8.1 Complete Learning System

```python
class LearningSystem:
    """Complete learning system with all features"""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        
        # Enable all learning features
        self.setup_system()
    
    def setup_system(self):
        """Set up the complete learning system"""
        # Configure world
        self.world.configure(
            width=20, 
            height=20, 
            visionWidth=8, 
            visionHeight=8
        )
        
        # Initialize components
        self.world.reset()
        self.agent.initialize(self.world)
        self.brain.initialize()
        
        # Enable all subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Set up initial state
        self.world.setAgentStart(10.0, 10.0)
        
        # Add interesting objects
        self.setup_world_objects()
    
    def setup_world_objects(self):
        """Set up interesting objects in the world"""
        import random
        
        # Add resources
        for _ in range(5):
            x = random.uniform(2, 18)
            y = random.uniform(2, 18)
            value = random.uniform(0.5, 2.0)
            self.world.addObject(pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Resource, value=value
            ))
        
        # Add hazards
        for _ in range(3):
            x = random.uniform(2, 18)
            y = random.uniform(2, 18)
            self.world.addObject(pynlm.WorldObject(
                x, y, pynlm.WorldObjectType.Hazard, value=-1.0
            ))
        
        # Add walls (boundaries)
        self.world.addObject(pynlm.WorldObject(0, 0, pynlm.WorldObjectType.Wall, radius=1.0))
        self.world.addObject(pynlm.WorldObject(20, 0, pynlm.WorldObjectType.Wall, radius=1.0))
        self.world.addObject(pynlm.WorldObject(0, 20, pynlm.WorldObjectType.Wall, radius=1.0))
        self.world.addObject(pynlm.WorldObject(20, 20, pynlm.WorldObjectType.Wall, radius=1.0))
    
    def run_episode(self, max_steps=1000):
        """Run one complete episode"""
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory percept
            percept = self.world.getSensoryPercept()
            
            # Process in agent
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Get motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action
            result = self.world.applyMotorCommand(
                motor_cmd, self.world.getSimulationTime()
            )
            
            # Learn from result
            if result.success:
                reward = result.reward
            else:
                reward = -1.0
            
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Print progress
            if step % 100 == 0:
                self.print_status(step)
            
            # Check if episode should end
            if self.world.getAgentBody().health <= 0:
                print(f"Episode ended at step {step} (agent died)")
                break
        
        return step
    
    def print_status(self, step):
        """Print current status"""
        print(f"Step {step}:")
        print(f"  Firing neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {self.brain.getTotalSpikeCount()}")
        print(f"  Curiosity: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {self.brain.getDevelopmentalStage()}")
        print(f"  Agent position: ({self.world.getAgentBody().x:.1f}, {self.world.getAgentBody().y:.1f})")
        print(f"  Agent energy: {self.world.getAgentBody().energy:.2f}")
    
    def run_experiment(self, num_episodes=10):
        """Run multiple episodes"""
        
        results = []
        
        for episode in range(num_episodes):
            print(f"\n=== Episode {episode + 1} ===")
            
            # Reset for new episode
            self.brain.reset()
            self.agent.reset()
            self.world.reset()
            
            # Run episode
            steps_completed = self.run_episode()
            
            # Record results
            results.append({
                'episode': episode + 1,
                'steps_completed': steps_completed,
                'final_firing': self.brain.getFiringNeuronCount(),
                'final_spikes': self.brain.getTotalSpikeCount(),
                'final_curiosity': self.agent.getCuriosityLevel()
            })
        
        return results

# Use the learning system
learning_system = LearningSystem()
results = learning_system.run_experiment(5)
```

---

## Part 9: Troubleshooting and FAQs

### 9.1 Frequently Asked Questions

#### Q: What configuration values should I start with?

A: Use these recommended defaults for experimentation:

```python
config = pynlm.createDefaultConfig()

# Core brain parameters
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 1)
config.set("brain.connection_probability", 0.1)

# Learning parameters
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)

# Development
config.set("development.enable", True)
```

#### Q: How do I enable specific features?

A: Use the agent interface to enable/disable features:

```python
agent = pynlm.createAgentBrain(brain)

# Enable learning from rewards
agent.enableRewardModulation(True)

# Enable structural changes
agent.enableStructuralPlasticity(True)

# Enable developmental changes
agent.enableDevelopment(True)

# Enable exploration behavior
agent.enableCuriosity(True)
```

#### Q: What's the difference between step() and receiveSensoryInput()?

A: 
- `brain.step(step)` - Runs the brain computation for one simulation step
- `brain.receiveSensoryInput(percept)` - Injects sensory data into the brain

Typically: `agent.processSensoryInput(percept)` → `brain.step(step)`

#### Q: How do I save and load brain states?

A:
```python
# Save brain state
brain.save("my_brain.bin")

# Load brain state
brain.load("my_brain.bin")

# Also save agent state
agent.save("my_agent.bin")
agent.load("my_agent.bin")
```

### 9.2 Common Debugging Techniques

#### 1. Monitor Brain Activity
```python
# Add this to your simulation loop
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Spikes: {brain.getTotalSpikeCount()}")
        print(f"  Avg Rate: {brain.getAverageFiringRate():.2f}")
```

#### 2. Check Agent Behavior
```python
# Monitor agent decisions
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    print(f"Step {step}: {action}")
    
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### 3. Track Development
```python
# Monitor developmental changes
for step in range(1000):
    brain.step(step)
    
    if step % 200 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Developmental stage = {stage}")
```

---

## Part 10: Project Structure

### 10.1 File Organization

```
NLM/
├── src/
│   ├── core/              # Core utilities
│   │   ├── Config/        # Configuration system
│   │   ├── Types/         # Strong types (NeuronId, SynapseId, etc.)
│   │   ├── Random/        # Random number generation
│   │   └── Logger/        # Logging system
│   │
│   ├── brain/             # Neural components
│   │   ├── Neuron.cpp      # LIF neuron implementation
│   │   ├── Synapse.cpp     # Synaptic connections
│   │   ├── NeuralRegion.cpp # Brain regions
│   │   └── Brain.cpp       # Central brain coordinator
│   │
│   ├── memory/            # Memory systems
│   ├── neuromodulation/   # Neuromodulators
│   ├── prediction/       # Prediction systems
│   ├── cognition/        # Cognitive mechanisms
│   ├── sensory/          # Sensory processing
│   ├── motor/            # Motor system
│   └── environment/      # Environment interface
│
├── python/                # Python bindings
│   ├── bindings.cpp       # Pybind11 bindings
│   └── CMakeLists.txt     # Build configuration
│
├── docs/                  # Documentation
│   ├── ARCHITECTURE.md    # System architecture
│   ├── SCIENCE.md         # Scientific background
│   └── HOW_TO_USE.md       # User guide
│
├── easy_usage.md          # Quick start guide (this file)
├── HOW_TO_USE.md           # Comprehensive user guide
├── pyproject.toml        # Python package configuration
└── CMakeLists.txt        # C++ project configuration
```

### 10.2 Build Process

```bash
# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Build Python bindings
pip install .

# Run tests
ctest --output-on-failure
```

---

## Conclusion

This enhanced usage guide provides:

1. **Clear setup instructions** for getting started quickly
2. **Complete simulation patterns** for common use cases
3. **Advanced research examples** for complex experiments
4. **Comprehensive API reference** for developers
5. **Error handling guidance** for troubleshooting
6. **Performance tips** for optimization
7. **Integration examples** for complex scenarios
8. **Real-world examples** for practical applications

Use this guide as your primary resource for working with NLM. The examples are designed to be copy-paste ready and work with minimal modification. Start with the quick start, explore the patterns, and don't hesitate to try the advanced examples as you become more comfortable with the system.

Happy experimenting with your artificial brain!
