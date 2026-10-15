# Building an Intelligent Agent - Intermediate Tutorial

This tutorial takes you beyond the basics and shows you how to build sophisticated intelligent agents using NLM. It covers advanced features, optimization techniques, and real-world scenarios.

## Prerequisites

Before starting this tutorial, you should be comfortable with:

- ✅ Basic NLM concepts (from Getting Started tutorial)
- ✅ Creating brains, worlds, and agents
- ✅ Running simple simulations
- ✅ Understanding context managers

## What You'll Learn

By the end of this tutorial, you will be able to:

1. ✅ Use configuration profiles and Pythonic convenience methods
2. ✅ Implement advanced agent learning with neuromodulation
3. ✅ Create complex world environments with obstacles and resources
4. ✅ Use serialization for checkpointing and experiment reproducibility
5. ✅ Implement error handling and robust simulations
6. ✅ Optimize performance for complex simulations
7. ✅ Build agent sequences for systematic testing

---

## Part 1: Advanced Configuration Management

Let's explore advanced configuration options and Pythonic convenience methods.

### Using Configuration Profiles

NLM provides predefined configuration profiles for different use cases:

```python
import pynlm as nlm

print("=== Configuration Profiles ===\n")

# Different profiles for different needs
profiles = [
    ("quick_test", "Fast testing with minimal neurons"),
    ("development", "Standard development setup"),
    ("high_performance", "Optimized for speed")
]

for profile_name, description in profiles:
    print(f"Profile: {profile_name}")
    print(f"Description: {description}")
    
    with nlm.Config() as config:
        config.setDefaults(profile_name)
        
        # Get profile summary
        summary = config.getSummary()
        print(f"Key settings: {len(summary)} parameters")
        
        # Show some specific settings
        if "brain" in summary:
            print(f"  Brain neurons: {summary['brain'].get('neuron_count', 'N/A')}")
        if "agent" in summary:
            print(f"  Agent exploration: {summary['agent'].get('exploration_rate', 'N/A')}")
    
    print()
```

### Pythonic Convenience Methods

The NLM Python bindings include powerful convenience methods that simplify common operations:

```python
import pynlm as nlm

print("\n=== Pythonic Convenience Methods ===\n")

# Create configuration with Pythonic convenience
with nlm.Config() as config:
    # Use predefined profile
    config.setDefaults("development")
    
    # Merge with additional settings
    config.updateFromDict({
        "brain.neuron_count": 500,
        "brain.connection_probability": 0.3,
        "agent.exploration_rate": 0.5
    })
    
    print("✓ Configuration created with convenience methods")
    
    # Get configuration as Python dict for inspection
    summary = config.getSummary()
    print(f"  Configuration keys: {list(summary.keys())}")
    
    # Validate configuration
    issues = config.validateConfig()
    if issues:
        print("  Configuration issues:")
        for issue in issues[:3]:  # Show first 3 issues
            print(f"    - {issue}")
    else:
        print("  ✓ Configuration is valid")

# Create brain and get stats in one go
with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Get neural statistics as Python dict
        stats = brain.getNeuralStats()
        print(f"\n✓ Brain has {stats['total_neurons']} neurons")
        print(f"✓ Brain has {stats['total_spikes']} spikes")
        print(f"✓ Average firing rate: {stats['average_firing_rate']:.3f} Hz")
```

### Advanced Configuration Example

```python
import pynlm as nlm

print("\n=== Advanced Configuration Example ===\n")

# Create sophisticated configuration
with nlm.Config() as config:
    # Start with base profile
    config.setDefaults("development")
    
    # Add advanced settings
    config.updateFromDict({
        "simulation.max_steps": 5000,
        "simulation.timestep": 0.001,
        "brain.neuron_count": 2000,
        "brain.connection_probability": 0.15,
        "agent.exploration_rate": 0.3,
        "agent.learning_rate": 0.01,
        "agent.reward_modulation": True,
        "agent.structural_plasticity": True,
        "agent.development": True,
        "agent.curiosity": True,
        "memory.working_memory_size": 100,
        "memory.episodic_capacity": 1000
    })
    
    # Merge with environment-specific settings
    config.mergeConfig({
        "environment.width": 100.0,
        "environment.height": 100.0,
        "environment.obstacle_density": 0.2,
        "environment.resource_density": 0.1
    })
    
    # Validate and get final configuration
    issues = config.validateConfig()
    if not issues:
        final_summary = config.getSummary()
        print("✓ Advanced configuration created successfully")
        print(f"  Total configuration items: {len(final_summary)}")
        
        # Save configuration for later use
        # nlm.JsonSerializer.save_to_file("config.json", final_summary)
        print("  Configuration ready for simulation")
    else:
        print("❌ Configuration issues found:")
        for issue in issues:
            print(f"  - {issue}")
```

---

## Part 2: Advanced Agent Learning

Now let's explore advanced agent learning features and optimization techniques.

### High-Level Agent Training

The NLM provides high-level training methods that simplify complex learning scenarios:

```python
import pynlm as nlm

print("\n=== High-Level Agent Training ===\n")

# Create world with convenience methods
with nlm.SimpleWorld() as world:
    # Create world with obstacles
    world.createGridWorld(100.0, 100.0, 0.1)  # 10% obstacle density
    world.setAgentGoal(90.0, 90.0)
    world.setAgentStart(10.0, 10.0)
    
    # Add resources to collect
    for x in [20.0, 40.0, 60.0, 80.0]:
        for y in [20.0, 40.0, 60.0, 80.0]:
            world.addResource(x, y, 1.0)
    
    print("✓ World created with obstacles, resources, and goals")
    
    # Create brain and agent
    with nlm.Config() as config:
        config.setDefaults("high_performance")
        config.updateFromDict({
            "agent.exploration_rate": 0.3,
            "agent.learning_rate": 0.01
        })
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                print("✓ Agent initialized with learning features")
                
                # High-level training with convenience method
                print("Running high-level training...")
                metrics = agent.learnFromWorld(world, 200)
                
                print("✓ Training completed!")
                print(f"  Success rate: {metrics['success_rate']*100:.1f}%")
                print(f"  Average reward: {metrics['average_reward']:.3f}")
                print(f"  Total reward: {metrics['total_reward']:.3f}")
                print(f"  Steps taken: {metrics['steps_taken']}")
                
                # Show performance metrics
                perf_metrics = agent.getPerformanceMetrics()
                print("\n✓ Performance metrics:")
                print(f"  Neuromodulation level: {perf_metrics['neuromodulation_level']:.3f}")
                print(f"  Curiosity level: {perf_metrics['curiosity_level']:.3f}")
                print(f"  Development stage: {perf_metrics['development_stage']:.3f}")
```

### Advanced Learning Parameters

```python
import pynlm as nlm

print("\n=== Advanced Learning Parameters ===\n")

# Create agent with sophisticated learning
with nlm.Config() as config:
    config.setDefaults("development")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        with nlm.AgentBrain(brain) as agent:
            # Set comprehensive learning parameters
            learning_params = {
                "reward_modulation": {
                    "enabled": True,
                    "learning_rate": 0.01,
                    "prediction_error_weight": 0.5
                },
                "structural_plasticity": {
                    "enabled": True,
                    "growth_rate": 0.001,
                    "pruning_rate": 0.0005
                },
                "development": {
                    "enabled": True,
                    "plasticity_decay": 0.95,
                    "maturation_rate": 0.01
                },
                "curiosity": {
                    "enabled": True,
                    "exploration_rate": 0.3,
                    "novelty_weight": 0.8
                },
                "working_memory": {
                    "size": 100,
                    "decay_rate": 0.9
                }
            }
            
            agent.setLearningParameters(learning_params)
            
            print("✓ Advanced learning parameters configured")
            
            # Verify parameters
            print("\nConfigured learning features:")
            print(f"  Reward modulation: {'✅' if agent.isRewardModulationEnabled() else '❌'}")
            print(f"  Structural plasticity: {'✅' if agent.isStructuralPlasticityEnabled() else '❌'}")
            print(f"  Development: {'✅' if agent.isDevelopmentEnabled() else '❌'}")
            print(f"  Curiosity: {'✅' if agent.isCuriosityEnabled() else '❌'}")
```

### Agent Training Sequences

```python
import pynlm as nlm

print("\n=== Agent Training Sequences ===\n")

# Create training environment
with nlm.SimpleWorld() as world:
    world.configure(200.0, 200.0, 20, 20)
    
    print("Creating training sequences...")
    
    # Create training sequences for systematic testing
    sequences = nlm.createAgentSequence(world, 5, 100)
    
    print("✓ Training sequences created:")
    for i, seq in enumerate(sequences):
        print(f"  Episode {seq['episode']}: {seq['success_rate']*100:.1f}% success rate")
    
    # Get average performance
    total_success = sum(seq['success_rate'] for seq in sequences) / len(sequences)
    print(f"\n✓ Average success rate: {total_success*100:.1f}%")
    
    # Run each sequence
    for sequence in sequences:
        episode = sequence['episode']
        print(f"\nRunning episode {episode}...")
        
        with nlm.Config() as config:
            config.setDefaults("development")
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    
                    # Run the specific episode
                    for step in range(sequence['steps']):
                        world.update(0.1)
                        action = agent.decodeMotorCommand()
                        world.applyMotorCommand(action, world.getSimulationTime())
                    
                    # Get episode metrics
                    episode_metrics = agent.getPerformanceMetrics()
                    print(f"  Episode {episode} metrics:")
                    print(f"    Success rate: {episode_metrics['success_rate']*100:.1f}%")
                    print(f"    Total reward: {episode_metrics['total_reward']:.3f}")
```

---

## Part 3: Complex World Environments

Create sophisticated world environments with multiple features and challenges.

### World with Multiple Agent Types

```python
import pynlm as nlm

print("\n=== Complex World Environment ===\n")

# Create sophisticated simulation
with nlm.Config() as config:
    config.setDefaults("high_performance")
    config.updateFromDict({
        "agent.count": 3,
        "agent.exploration_rate": 0.2,
        "agent.competition": True
    })
    
    with nlm.SimpleWorld() as world:
        # Configure world with multiple features
        world.createGridWorld(200.0, 200.0, 0.15)  # 15% obstacles
        world.setAgentStart(50.0, 50.0)
        world.setAgentGoal(180.0, 180.0)
        world.setMaxEnergy(100.0)
        world.setVisionAngle(90.0)  # 90-degree vision angle
        
        # Add environmental objects
        print("Adding environmental objects...")
        
        # Add resources at specific positions
        resources = [
            (30.0, 30.0, 10.0),
            (70.0, 70.0, 15.0),
            (120.0, 120.0, 20.0),
            (170.0, 170.0, 25.0)
        ]
        
        for x, y, value in resources:
            world.addResource(x, y, value)
        
        # Add obstacles
        obstacles = [
            (100.0, 50.0, 8.0),
            (50.0, 100.0, 8.0),
            (150.0, 100.0, 8.0),
            (100.0, 150.0, 8.0)
        ]
        
        for x, y, radius in obstacles:
            obj = nlm.WorldObject(
                float(x), float(y),
                nlm.WorldObjectType.Obstacle,
                radius
            )
            world.addObject(obj)
        
        print("✓ World configured with:")
        print(f"  Size: {world.getWidth()} x {world.getHeight()} meters")
        print(f"  Obstacles: {len(obstacles)}")
        print(f"  Resources: {len(resources)}")
        print(f"  Agent start: ({world.getAgentStart().x}, {world.getAgentStart().y})")
        print(f"  Agent goal: ({world.getAgentGoal().x}, {world.getAgentGoal().y})")
        
        # Create and configure agents
        agents = []
        for i in range(3):
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.AgentBrain(brain) as agent:
                    agent.initialize(world)
                    agent.enableCuriosity(True)
                    agent.enableRewardModulation(True)
                    
                    agents.append(agent)
                    print(f"✓ Agent {i + 1} initialized")
        
        # Run multi-agent simulation
        print("\nRunning multi-agent simulation...")
        for episode in range(5):
            print(f"\nEpisode {episode + 1}:")
            
            for agent in agents:
                # Each agent runs its own simulation
                agent_success = 0
                for step in range(50):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    
                    # Apply action and get results
                    result = world.applyMotorCommand(action, world.getSimulationTime())
                    
                    if result.success:
                        agent_success += 1
                
                print(f"  Agent success: {agent_success/50*100:.1f}%")
        
        print("\n✓ Multi-agent simulation completed")
```

### World Visualization and Debugging

```python
import pynlm as nlm

print("\n=== World Visualization and Debugging ===\n")

# Create world for visualization
with nlm.SimpleWorld() as world:
    world.createGridWorld(50.0, 50.0, 0.1)
    
    # Add various objects
    world.addResource(10.0, 10.0, 5.0)
    world.addResource(40.0, 40.0, 8.0)
    
    obstacle = nlm.WorldObject(25.0, 25.0, nlm.WorldObjectType.Obstacle, 5.0)
    world.addObject(obstacle)
    
    world.setAgentGoal(45.0, 45.0)
    
    print("✓ Visualization world created")
    
    # Get observation map for debugging/visualization
    observation_map = world.getObservationMap()
    
    print("\nObservation map (visualization):")
    print("Legend:")
    print("  -2.0 to 2.0: Obstacles and walls")
    print("  0.0: Empty space")
    print("  0.5 to 2.0: Resources")
    print("  3.0+: Agent start/goal area")
    print()
    
    print("Map (showing first 10x10 section):")
    for i in range(min(10, len(observation_map))):
        row_str = "  ["
        for j in range(min(10, len(observation_map[i]))):
            value = observation_map[i][j]
            if value < -1.0:
                cell = "█"  # Obstacle
            elif value < 0.0:
                cell = "▓"  # Wall
            elif value < 0.5:
                cell = "░"  # Empty
            elif value < 2.0:
                cell = "·"  # Resource
            else:
                cell = "★"  # Goal/Start
            row_str += f"{cell} "
        row_str += "]"
        print(f"    {row_str}")
    
    print(f"\n✓ Map generated: {len(observation_map)} x {len(observation_map[0])} cells")
```

---

## Part 4: Serialization and Checkpointing

Learn to save and load simulations for experiment reproducibility and checkpointing.

### Complete Serialization Example

```python
import plm as nlm
import time

print("\n=== Serialization and Checkpointing ===\n")

# Create and run a simulation
print("1. Creating simulation...")
with nlm.Config() as config:
    config.setDefaults("development")
    
    with nlm.SimpleWorld() as world:
        world.createGridWorld(100.0, 100.0, 0.1)
        world.setAgentStart(10.0, 10.0)
        world.setAgentGoal(90.0, 90.0)
        
        # Add resources
        for x in [20.0, 40.0, 60.0, 80.0]:
            for y in [20.0, 40.0, 60.0, 80.0]:
                world.addResource(x, y, 1.0)
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                
                print("✓ Simulation running...")
                
                # Run simulation steps
                for step in range(100):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get final metrics
                final_metrics = agent.getPerformanceMetrics()
                print(f"✓ Simulation completed: {final_metrics['success_rate']*100:.1f}% success")
                
                # Save brain state (checkpoint)
                print("2. Saving brain state (checkpoint)...")
                checkpoint_file = f"checkpoint_episode_{final_metrics['episode']}.json"
                nlm.JsonSerializer.serialize_brain(brain, checkpoint_file)
                print(f"✓ Brain state saved to {checkpoint_file}")
                
                # Save world state
                world_file = f"world_state_{final_metrics['episode']}.json"
                nlm.JsonSerializer.serialize_world(world, world_file)
                print(f"✓ World state saved to {world_file}")
                
                # Save configuration
                config_file = f"config_{final_metrics['episode']}.json"
                nlm.JsonSerializer.save_to_file(config_file, config.getSummary())
                print(f"✓ Configuration saved to {config_file}")
                
                # Save performance metrics
                metrics_file = f"metrics_{final_metrics['episode']}.json"
                nlm.JsonSerializer.save_to_file(metrics_file, final_metrics)
                print(f"✓ Metrics saved to {metrics_file}")

# Later: Load and continue simulation
print("\n3. Loading saved simulation...")
with nlm.Config() as config:
    # Load configuration
    config_summary = nlm.JsonSerializer.load_from_file("config_episode_0.json")
    config.updateFromDict(config_summary)
    
    with nlm.Brain(config) as brain:
        # Load brain state
        nlm.JsonSerializer.deserialize_brain("checkpoint_episode_0.json", brain)
        print("✓ Brain state loaded")
        
        with nlm.SimpleWorld() as world:
            # Load world state
            nlm.JsonSerializer.deserialize_world("world_state_episode_0.json", world)
            print("✓ World state loaded")
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                print("✓ Agent re-initialized")
                
                # Continue simulation from checkpoint
                print("Continuing simulation from checkpoint...")
                for step in range(50):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get final metrics
                continued_metrics = agent.getPerformanceMetrics()
                print(f"✓ Continued simulation completed: {continued_metrics['success_rate']*100:.1f}% success")
```

### Binary Serialization for Performance

```python
import pynlm as nlm

print("\n=== Binary Serialization (Performance) ===\n")

# Create simulation
with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Run simulation
        print("Running simulation for binary serialization test...")
        for step in range(500):
            brain.step(step)
        
        # Use binary serialization for better performance
        print("2. Using binary serialization...")
        
        # Save as binary
        binary_file = "brain_state.bin"
        nlm.BinarySerializer.serialize_brain(brain, binary_file)
        print(f"✓ Brain state saved as binary to {binary_file}")
        
        # Load as binary (faster)
        with nlm.Brain(config) as loaded_brain:
            nlm.BinarySerializer.deserialize_brain(binary_file, loaded_brain)
            print("✓ Brain state loaded from binary (faster)")
            
            # Verify loaded state
            loaded_stats = loaded_brain.getNeuralStats()
            print(f"✓ Loaded brain has {loaded_stats['total_neurons']} neurons")
            print(f"✓ Loaded brain has {loaded_stats['total_spikes']} spikes")
```

### Incremental Serialization

```python
import pynlm as nlm

print("\n=== Incremental Serialization ===\n")

# Create simulation
with nlm.Config() as config:
    config.setDefaults("development")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        # Use incremental serialization
        print("1. Using incremental serialization...")
        
        # Save as incremental file
        incremental_file = "brain_state_incremental"
        nlm.IncrementalSerializer.save_to_file(incremental_file, brain.getNeuralStats())
        print(f"✓ Brain state saved incrementally to {incremental_file}")
        
        # Check file chunks
        chunk_count = nlm.IncrementalSerializer.get_chunk_count(incremental_file)
        print(f"✓ File contains {chunk_count} chunks")
        
        # Load incrementally
        loaded_data = nlm.IncrementalSerializer.load_from_file(incremental_file)
        print("✓ Data loaded incrementally")
        
        print(f"✓ Loaded stats: {loaded_data}")
```

---

## Part 5: Error Handling and Robust Simulations

Implement robust error handling and create fault-tolerant simulations.

### Comprehensive Error Handling

```python
import pynlm as nlm
import traceback

print("\n=== Comprehensive Error Handling ===\n")

class RobustNLMSimulation:
    def __init__(self, config_profile="development"):
        self.config = None
        self.brain = None
        self.world = None
        self.agent = None
        self.config_profile = config_profile
        self.errors = []
        self.checkpoint_file = "simulation_checkpoint.json"
    
    def __enter__(self):
        """Initialize simulation with error handling."""
        try:
            print("Initializing robust simulation...")
            
            # Create config
            self.config = nlm.Config()
            self.config.setDefaults(self.config_profile)
            
            # Validate configuration
            issues = self.config.validateConfig()
            if issues:
                self.errors.extend([f"Config issue: {issue}" for issue in issues])
                raise ValueError(f"Invalid configuration: {issues}")
            
            # Create brain
            self.brain = nlm.Brain(self.config)
            self.brain.initialize()
            
            # Create world
            self.world = nlm.SimpleWorld()
            self.world.configure(100.0, 100.0, 10, 10)
            
            # Create agent
            self.agent = nlm.AgentBrain(self.brain)
            self.agent.initialize(self.world)
            
            print("✓ Simulation initialized successfully")
            return self
            
        except Exception as e:
            self.errors.append(f"Initialization error: {e}")
            raise
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Cleanup simulation with error handling."""
        try:
            # Save checkpoint if simulation was running
            if self.brain and self.world and self.agent:
                self.save_checkpoint()
                print("✓ Checkpoint saved before exit")
            
            print("✓ Simulation cleaned up successfully")
            
        except Exception as e:
            self.errors.append(f"Cleanup error: {e}")
            print(f"⚠️  Warning: {e}")
    
    def run_episode(self, steps, auto_save=True):
        """Run a single simulation episode with error handling."""
        try:
            print(f"Running episode with {steps} steps...")
            
            # Reset simulation
            self.brain.reset()
            self.brain.initialize()
            self.agent.initialize(self.world)
            
            # Run simulation
            for step in range(steps):
                try:
                    # Update world
                    self.world.update(0.1)
                    
                    # Get sensory input
                    percept = self.world.getSensoryPercept()
                    
                    # Process input
                    self.agent.processSensoryInput(percept)
                    
                    # Brain thinks
                    self.brain.step(step)
                    
                    # Get and apply action
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
                    
                except nlm.NLMResourceError as e:
                    self.errors.append(f"Step {step} resource error: {e}")
                    continue  # Skip this step
                except Exception as e:
                    self.errors.append(f"Step {step} error: {e}")
                    break  # Stop this episode
            
            # Save checkpoint if requested
            if auto_save:
                self.save_checkpoint()
            
            # Get metrics
            metrics = self.agent.getPerformanceMetrics()
            print(f"✓ Episode completed: {metrics['success_rate']*100:.1f}% success")
            return metrics
            
        except Exception as e:
            self.errors.append(f"Episode error: {e}")
            raise
    
    def run_multiple_episodes(self, num_episodes, steps_per_episode, auto_save=True):
        """Run multiple episodes with checkpointing."""
        print(f"Running {num_episodes} episodes...")
        
        for episode in range(num_episodes):
            print(f"\nEpisode {episode + 1}/{num_episodes}")
            
            try:
                metrics = self.run_episode(steps_per_episode, auto_save)
                
                # Display metrics
                print(f"  Success rate: {metrics['success_rate']*100:.1f}%")
                print(f"  Total reward: {metrics['total_reward']:.3f}")
                print(f"  Steps taken: {metrics['steps_taken']}")
                
                # Save episode data
                if auto_save:
                    self.save_episode_data(episode, metrics)
                
            except Exception as e:
                print(f"❌ Episode {episode + 1} failed: {e}")
                if not auto_save:
                    raise
        
        print(f"\n✓ All {num_episodes} episodes completed")
        return self.get_summary()
    
    def save_checkpoint(self):
        """Save simulation checkpoint."""
        try:
            checkpoint_data = {
                'config': self.config.getSummary() if self.config else None,
                'brain_state': self.brain.getNeuralStats() if self.brain else None,
                'world_state': self.world.getObservationMap() if self.world else None,
                'agent_metrics': self.agent.getPerformanceMetrics() if self.agent else None,
                'errors': self.errors,
                'timestamp': time.time()
            }
            
            nlm.JsonSerializer.save_to_file(self.checkpoint_file, checkpoint_data)
            print(f"✓ Checkpoint saved to {self.checkpoint_file}")
            
        except Exception as e:
            print(f"❌ Checkpoint save failed: {e}")
    
    def save_episode_data(self, episode, metrics):
        """Save episode-specific data."""
        try:
            episode_file = f"episode_{episode}_data.json"
            episode_data = {
                'episode': episode,
                'metrics': metrics,
                'brain_state': self.brain.getNeuralStats() if self.brain else None,
                'world_state': self.world.getObservationMap() if self.world else None,
                'timestamp': time.time()
            }
            
            nlm.JsonSerializer.save_to_file(episode_file, episode_data)
            print(f"✓ Episode {episode} data saved to {episode_file}")
            
        except Exception as e:
            print(f"❌ Episode data save failed: {e}")
    
    def get_summary(self):
        """Get simulation summary."""
        return {
            'total_episodes': len([e for e in self.errors if 'Episode' not in e]),
            'errors': len(self.errors),
            'checkpoint_file': self.checkpoint_file,
            'config_profile': self.config_profile
        }

# Usage example
print("\n=== Using RobustNLMSimulation ===\n")

try:
    with RobustNLMSimulation("development") as sim:
        # Run multiple episodes with automatic checkpointing
        results = sim.run_multiple_episodes(
            num_episodes=5,
            steps_per_episode=50,
            auto_save=True
        )
        
        print("\n✓ Simulation completed successfully!")
        print(f"Summary: {results}")
        
except Exception as e:
    print(f"\n❌ Simulation failed: {e}")
    print(f"Errors: {sim.errors if 'sim' in locals() else 'N/A'}")
```

### Error Recovery

```python
import pynlm as nlm

print("\n=== Error Recovery Example ===\n")

class ErrorRecoverySimulation:
    def __init__(self):
        self.recovery_attempts = 0
        self.max_recovery_attempts = 3
    
    def run_with_recovery(self, config_profile="development", steps=100):
        """Run simulation with automatic error recovery."""
        for attempt in range(self.max_recovery_attempts):
            try:
                print(f"Attempt {attempt + 1}/{self.max_recovery_attempts}")
                
                with nlm.Config() as config:
                    config.setDefaults(config_profile)
                    
                    with nlm.Brain(config) as brain:
                        brain.initialize()
                        
                        with nlm.SimpleWorld() as world:
                            world.configure(50.0, 50.0, 5, 5)
                            
                            with nlm.AgentBrain(brain) as agent:
                                agent.initialize(world)
                                
                                # Run simulation
                                for step in range(steps):
                                    world.update(0.1)
                                    action = agent.decodeMotorCommand()
                                    world.applyMotorCommand(action, world.getSimulationTime())
                                
                                # Success!
                                metrics = agent.getPerformanceMetrics()
                                print(f"✓ Simulation successful on attempt {attempt + 1}")
                                print(f"  Success rate: {metrics['success_rate']*100:.1f}%")
                                return metrics
                                
            except nlm.NLMResourceError as e:
                print(f"⚠️  Resource error: {e}")
                self.recovery_attempts += 1
                if attempt < self.max_recovery_attempts - 1:
                    print("  Retrying with different configuration...")
                    # Modify configuration for retry
                    with nlm.Config() as config:
                        config.setDefaults("quick_test")  # Simpler config
                    continue
                else:
                    print("❌ Max recovery attempts reached")
                    raise
                    
            except nlm.NLMValidationError as e:
                print(f"⚠️  Validation error: {e}")
                # Fix configuration and retry
                with nlm.Config() as config:
                    config.setDefaults("quick_test")
                    config.updateFromDict({"agent.exploration_rate": 0.1})
                continue
                
            except Exception as e:
                print(f"⚠️  Unexpected error: {e}")
                if attempt < self.max_recovery_attempts - 1:
                    continue
                else:
                    raise

# Run simulation with recovery
recovery_sim = ErrorRecoverySimulation()
try:
    metrics = recovery_sim.run_with_recovery("development", 50)
    print("\n✅ Simulation completed with recovery!")
    
except Exception as e:
    print(f"\n❌ All attempts failed: {e}")
```

---

## Part 6: Performance Optimization

Learn techniques to optimize simulation performance for complex scenarios.

### Performance Optimization Techniques

```python
import pynlm as nlm
import time

print("\n=== Performance Optimization Techniques ===\n")

class PerformanceOptimizer:
    def __init__(self):
        self.optimization_results = []
    
    def optimize_simulation(self, brain, world, agent, steps=100, optimizations=None):
        """Run simulation with performance optimizations."""
        if optimizations is None:
            optimizations = {}
        
        # Reset and run baseline
        print("1. Running baseline (no optimizations)...")
        brain.reset()
        brain.initialize()
        agent.initialize(world)
        
        start_time = time.time()
        for step in range(steps):
            world.update(0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        baseline_time = time.time() - start_time
        print(f"   Baseline time: {baseline_time:.3f} seconds")
        
        # Store baseline
        baseline_metrics = {
            'time': baseline_time,
            'steps': steps,
            'efficiency': steps / baseline_time
        }
        
        optimization_results = [baseline_metrics]
        
        # Apply optimizations
        if optimizations.get('batch_operations', False):
            print("2. Testing batch operations optimization...")
            result = self.test_batch_operations(brain, world, agent, steps)
            optimization_results.append(result)
        
        if optimizations.get('reduce_frequency', False):
            print("3. Testing reduced frequency optimization...")
            result = self.test_reduced_frequency(brain, world, agent, steps)
            optimization_results.append(result)
        
        if optimizations.get('reuse_objects', False):
            print("4. Testing object reuse optimization...")
            result = self.test_object_reuse(brain, world, agent, steps)
            optimization_results.append(result)
        
        # Compare results
        print("\n=== Optimization Results ===")
        print(f"{'Method':<25} {'Time (s)':<10} {'Efficiency':<12} {'Improvement':<12}")
        print("-" * 65)
        
        baseline_efficiency = baseline_metrics['efficiency']
        
        for result in optimization_results[1:]:
            improvement = (result['efficiency'] - baseline_efficiency) / baseline_efficiency * 100
            print(f"{result['method']:<25} {result['time']:<10.3f} {result['efficiency']:<12.1f} {improvement:+8.1f}%")
        
        # Store and return best result
        self.optimization_results = optimization_results
        best_result = max(optimization_results, key=lambda x: x['efficiency'])
        
        print(f"\n✓ Best method: {best_result['method']}")
        print(f"✓ Best efficiency: {best_result['efficiency']:.1f} steps/second")
        
        return best_result
    
    def test_batch_operations(self, brain, world, agent, steps):
        """Test batch operations optimization."""
        brain.reset()
        brain.initialize()
        agent.initialize(world)
        
        start_time = time.time()
        
        # Use batch operations where possible
        for step in range(0, steps, 10):  # Process in chunks of 10
            # Process 10 steps at once using batch operation
            brain.runSimulation(min(10, steps - step))
            
            # Update world
            for i in range(min(10, steps - step)):
                world.update(0.1)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        elapsed = time.time() - start_time
        return {
            'method': 'Batch Operations',
            'time': elapsed,
            'steps': steps,
            'efficiency': steps / elapsed
        }
    
    def test_reduced_frequency(self, brain, world, agent, steps):
        """Test reduced frequency optimization."""
        brain.reset()
        brain.initialize()
        agent.initialize(world)
        
        start_time = time.time()
        
        # Reduce frequency of expensive operations
        for step in range(steps):
            world.update(0.1)
            
            # Only get action every N steps
            if step % 5 == 0:
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            else:
                # Simplified action for less frequent updates
                simple_action = nlm.Action.createMove(0.0, 0.0)  # Default action
                world.applyMotorCommand(simple_action, world.getSimulationTime())
        
        elapsed = time.time() - start_time
        return {
            'method': 'Reduced Frequency',
            'time': elapsed,
            'steps': steps,
            'efficiency': steps / elapsed
        }
    
    def test_object_reuse(self, brain, world, agent, steps):
        """Test object reuse optimization."""
        brain.reset()
        brain.initialize()
        agent.initialize(world)
        
        start_time = time.time()
        
        # Reuse objects to reduce overhead
        for step in range(steps):
            world.update(0.1)
            
            # Reuse action object
            action = agent.decodeMotorCommand()
            # Reset action parameters instead of creating new object
            action.setParameters([action.getParameters()[0] * 0.9, action.getParameters()[1]])
            world.applyMotorCommand(action, world.getSimulationTime())
        
        elapsed = time.time() - start_time
        return {
            'method': 'Object Reuse',
            'time': elapsed,
            'steps': steps,
            'efficiency': steps / elapsed
        }

# Usage example
print("=== Using PerformanceOptimizer ===\n")

optimizer = PerformanceOptimizer()

with nlm.Config() as config:
    config.setDefaults("high_performance")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(50.0, 50.0, 0.1)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Test various optimizations
                optimizations = {
                    'batch_operations': True,
                    'reduce_frequency': True,
                    'reuse_objects': True
                }
                
                results = optimizer.optimize_simulation(
                    brain, world, agent, 100, optimizations
                )
                
                print(f"\n✓ Optimization complete!")
                print(f"Best method: {results['method']}")
                print(f"Best efficiency: {results['efficiency']:.1f} steps/second")
```

### Memory Management Optimization

```python
import pynlm as nlm
import gc

print("\n=== Memory Management Optimization ===\n")

class MemoryManager:
    def __init__(self):
        self.memory_usage = []
    
    def monitor_memory_usage(self, brain, world, agent, steps=50, description=""):
        """Monitor memory usage during simulation."""
        # Force garbage collection before test
        gc.collect()
        
        # Get baseline memory
        start_time = time.time()
        
        # Run simulation
        brain.reset()
        brain.initialize()
        agent.initialize(world)
        
        memory_before = len(gc.get_objects())
        
        for step in range(steps):
            world.update(0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        memory_after = len(gc.get_objects())
        
        elapsed = time.time() - start_time
        memory_increase = memory_after - memory_before
        
        result = {
            'description': description,
            'time': elapsed,
            'steps': steps,
            'memory_before': memory_before,
            'memory_after': memory_after,
            'memory_increase': memory_increase,
            'memory_efficiency': steps / memory_increase if memory_increase > 0 else float('inf')
        }
        
        self.memory_usage.append(result)
        return result
    
    def optimize_context_managers(self):
        """Test context manager optimization."""
        print("Testing context manager optimization...")
        
        # Test 1: Many small context managers
        print("\n1. Many small context managers:")
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            
            # Use many small context managers
            for i in range(20):
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    brain.step(i)
        
        # Test 2: Nested context managers
        print("\n2. Nested context managers:")
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            
            with nlm.SimpleWorld() as world:
                world.configure(50.0, 50.0, 5, 5)
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        
                        for step in range(20):
                            world.update(0.1)
                            action = agent.decodeMotorCommand()
                            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Test 3: Reusable context managers
        print("\n3. Reusable context managers:")
        with nlm.Config() as config:
            config.setDefaults("quick_test")
            
            # Reuse context managers
            for episode in range(5):
                with nlm.SimpleWorld() as world:
                    world.configure(30.0, 30.0, 3, 3)
                    
                    with nlm.Brain(config) as brain:
                        brain.initialize()
                        
                        with nlm.AgentBrain(brain) as agent:
                            agent.initialize(world)
                            
                            for step in range(10):
                                world.update(0.1)
                                action = agent.decodeMotorCommand()
                                world.applyMotorCommand(action, world.getSimulationTime())

# Usage example
print("=== Using MemoryManager ===\n")

memory_manager = MemoryManager()

with nlm.Config() as config:
    config.setDefaults("development")
    
    with nlm.Brain(config) as brain:
        brain.initialize()
        
        with nlm.SimpleWorld() as world:
            world.createGridWorld(40.0, 40.0, 0.1)
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Monitor memory usage
                results = memory_manager.monitor_memory_usage(
                    brain, world, agent, 30, "Standard simulation"
                )
                
                print(f"\n✓ Memory usage analysis:")
                print(f"  Description: {results['description']}")
                print(f"  Time: {results['time']:.3f} seconds")
                print(f"  Steps: {results['steps']}")
                print(f"  Memory before: {results['memory_before']}")
                print(f"  Memory after: {results['memory_after']}")
                print(f"  Memory increase: {results['memory_increase']}")
                print(f"  Memory efficiency: {results['memory_efficiency']:.1f} steps/object")
                
                # Test context manager optimization
                memory_manager.optimize_context_managers()
```

---

## Part 7: Advanced Tutorials

### Building an Intelligent Agent System

This tutorial shows you how to build a complete intelligent agent system with multiple components.

```python
import pynlm as nlm
import json

print("\n=== Building an Intelligent Agent System ===\n")

class IntelligentAgentSystem:
    def __init__(self, config_file="agent_config.json"):
        self.config_file = config_file
        self.agent_history = []
        self.environment_history = []
        self.performance_metrics = []
    
    def load_or_create_config(self):
        """Load configuration from file or create default."""
        try:
            # Try to load from file
            with open(self.config_file, 'r') as f:
                config_data = json.load(f)
            
            config = nlm.Config()
            config.updateFromDict(config_data)
            print(f"✓ Configuration loaded from {self.config_file}")
            return config
            
        except FileNotFoundError:
            print(f"⚠ Configuration file not found, creating default...")
            # Create default configuration
            config = nlm.Config()
            config.setDefaults("development")
            config.updateFromDict({
                "agent.exploration_rate": 0.3,
                "agent.learning_rate": 0.01,
                "agent.reward_modulation": True,
                "agent.curiosity": True,
                "simulation.max_steps": 1000,
                "simulation.timestep": 0.01
            })
            
            # Save default configuration
            with open(self.config_file, 'w') as f:
                json.dump(config.getSummary(), f, indent=2)
            
            print(f"✓ Default configuration created and saved to {self.config_file}")
            return config
    
    def create_environment(self):
        """Create sophisticated environment with multiple features."""
        print("Creating sophisticated environment...")
        
        world = nlm.SimpleWorld()
        
        # Create complex terrain
        world.createGridWorld(100.0, 100.0, 0.2)  # 20% obstacles
        
        # Set interesting start and goal
        world.setAgentStart(20.0, 20.0)
        world.setAgentGoal(80.0, 80.0)
        
        # Add resources at strategic locations
        resources = [
            (30.0, 30.0, 5.0),
            (70.0, 70.0, 8.0),
            (50.0, 20.0, 3.0),
            (20.0, 50.0, 6.0),
            (80.0, 30.0, 4.0),
            (30.0, 80.0, 7.0)
        ]
        
        for x, y, value in resources:
            world.addResource(x, y, value)
        
        # Add environmental objects
        for i in range(5):
            obj = nlm.WorldObject(
                float(i * 20 + 10), float(i * 15 + 10),
                nlm.WorldObjectType.Resource,
                2.0
            )
            world.addObject(obj)
        
        print("✓ Environment created with obstacles, resources, and goals")
        return world
    
    def create_agent(self, config, world):
        """Create intelligent agent with advanced features."""
        print("Creating intelligent agent...")
        
        with nlm.Brain(config) as brain:
            brain.initialize()
            
            with nlm.AgentBrain(brain) as agent:
                agent.initialize(world)
                
                # Enable all learning features
                agent.enableCuriosity(True)
                agent.enableRewardModulation(True)
                agent.enableStructuralPlasticity(True)
                agent.enableDevelopment(True)
                
                # Set sophisticated learning parameters
                learning_params = {
                    "reward_modulation": {
                        "enabled": True,
                        "learning_rate": 0.02,
                        "prediction_error_weight": 0.6
                    },
                    "curiosity": {
                        "enabled": True,
                        "exploration_rate": 0.4,
                        "novelty_weight": 0.7,
                        "curiosity_decay": 0.95
                    },
                    "structural_plasticity": {
                        "enabled": True,
                        "growth_rate": 0.002,
                        "pruning_rate": 0.001,
                        "connection_threshold": 0.1
                    },
                    "development": {
                        "enabled": True,
                        "plasticity_decay": 0.9,
                        "maturation_rate": 0.02,
                        "critical_period": 100
                    }
                }
                
                agent.setLearningParameters(learning_params)
                
                print("✓ Intelligent agent created with advanced features")
                return agent
    
    def run_training_episode(self, agent, world, episode_number):
        """Run a single training episode."""
        print(f"  Episode {episode_number + 1}...")
        
        # Reset agent state
        agent.reset()
        agent.initialize(world)
        
        # Run episode
        for step in range(100):
            world.update(0.1)
            
            # Get agent's action
            action = agent.decodeMotorCommand()
            
            # Apply action and get results
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Show progress
            if (step + 1) % 20 == 0:
                print(f"    Step {step + 1}: Action {action.getName()}, "
                      f"Reward: {result.reward:.3f}")
        
        # Get metrics
        metrics = agent.getPerformanceMetrics()
        metrics['episode'] = episode_number
        metrics['total_steps'] = 100
        
        return metrics
    
    def train_agent(self, num_episodes=10, save_checkpoints=True):
        """Train agent for multiple episodes."""
        print(f"\nTraining agent for {num_episodes} episodes...")
        
        # Load or create configuration
        config = self.load_or_create_config()
        
        # Create environment
        world = self.create_environment()
        
        # Create agent
        agent = self.create_agent(config, world)
        
        # Training loop
        all_metrics = []
        for episode in range(num_episodes):
            metrics = self.run_training_episode(agent, world, episode)
            all_metrics.append(metrics)
            
            # Save checkpoint
            if save_checkpoints and episode % 5 == 0:
                self.save_agent_state(agent, world, config, episode)
        
        # Store history
        self.agent_history.append(agent)
        self.environment_history.append(world)
        self.performance_metrics.extend(all_metrics)
        
        # Analyze results
        self.analyze_performance(all_metrics)
        
        # Save final agent
        if save_checkpoints:
            self.save_agent_state(agent, world, config, "final")
        
        print(f"\n✓ Training completed!")
        print(f"  Total episodes: {len(all_metrics)}")
        print(f"  Average success rate: {sum(m['success_rate'] for m in all_metrics)/len(all_metrics)*100:.1f}%")
        
        return all_metrics
    
    def save_agent_state(self, agent, world, config, episode_name):
        """Save agent state for later use."""
        try:
            state = {
                'config': config.getSummary(),
                'brain_state': agent.getPerformanceMetrics(),
                'world_state': world.getObservationMap(),
                'episode_name': episode_name
            }
            
            filename = f"agent_checkpoint_{episode_name}.json"
            nlm.JsonSerializer.save_to_file(filename, state)
            print(f"✓ Agent state saved to {filename}")
            
        except Exception as e:
            print(f"❌ Failed to save agent state: {e}")
    
    def analyze_performance(self, metrics):
        """Analyze and display performance metrics."""
        if not metrics:
            return
        
        print("\n=== Performance Analysis ===")
        print(f"{'Episode':<8} {'Success Rate':<15} {'Total Reward':<12} {'Steps':<8}")
        print("-" * 55)
        
        for metric in metrics:
            success_rate = metric['success_rate'] * 100
            total_reward = metric['total_reward']
            steps = metric['total_steps']
            
            print(f"{metric['episode']:<8} {success_rate:>7.1f}%{'':>8} {total_reward:>8.3f} {'':>4} {steps:<8}")
        
        # Calculate statistics
        avg_success = sum(m['success_rate'] for m in metrics) / len(metrics)
        avg_reward = sum(m['total_reward'] for m in metrics) / len(metrics)
        total_steps = sum(m['total_steps'] for m in metrics)
        
        print(f"\n=== Summary ===")
        print(f"Average success rate: {avg_success*100:.1f}%")
        print(f"Average total reward: {avg_reward:.3f}")
        print(f"Total steps taken: {total_steps}")
        print(f"Episodes per hour: {len(metrics) * 3600 / (total_steps * 0.1):.1f}")

# Usage example
print("=== Creating Intelligent Agent System ===\n")

agent_system = IntelligentAgentSystem()

try:
    # Train agent
    metrics = agent_system.train_agent(num_episodes=8, save_checkpoints=True)
    
    print("\n🎉 Intelligent agent system created successfully!")
    
except Exception as e:
    print(f"\n❌ Failed to create intelligent agent system: {e}")
    import traceback
    traceback.print_exc()
```

### Advanced NLM Development

This tutorial covers advanced development topics and integration patterns.

```python
import pynlm as nlm

print("\n=== Advanced NLM Development ===\n")

class AdvancedNLMSystem:
    """Advanced NLM system with multiple integration patterns."""
    
    def __init__(self):
        self.systems = []
        self.integrations = []
        self.optimization_pipeline = []
    
    def create_multiple_agents(self, num_agents, config_profile="development"):
        """Create multiple agents with different configurations."""
        print(f"Creating {num_agents} agents...")
        
        agents = []
        for i in range(num_agents):
            # Each agent gets different configuration
            config = nlm.Config()
            
            if config_profile == "development":
                config.setDefaults("development")
            elif config_profile == "production":
                config.setDefaults("high_performance")
            else:
                config.setDefaults("quick_test")
            
            # Add agent-specific parameters
            config.updateFromDict({
                "agent.id": i,
                "agent.exploration_rate": 0.1 + (i * 0.1),  # Different exploration rates
                "agent.learning_rate": 0.01 * (i + 1) / num_agents  # Different learning rates
            })
            
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.SimpleWorld() as world:
                    world.configure(
                        50.0 + i * 20,  # Different world sizes
                        50.0 + i * 20,
                        5 + i * 2,      # Different vision sizes
                        5 + i * 2
                    )
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        agents.append(agent)
                        print(f"  Agent {i} created")
        
        return agents
    
    def create_hierarchical_system(self, levels=3):
        """Create hierarchical system with multiple agent levels."""
        print(f"Creating hierarchical system with {levels} levels...")
        
        hierarchical_agents = []
        
        for level in range(levels):
            print(f"\nLevel {level + 1}:")
            
            # Create base configuration for this level
            config = nlm.Config()
            config.setDefaults("development")
            
            # Level-specific parameters
            if level == 0:
                # Level 1: Simple reactive agents
                config.updateFromDict({
                    "agent.exploration_rate": 0.5,
                    "agent.learning_rate": 0.01,
                    "agent.capacity": 50,
                    "agent.memory": "short_term"
                })
                print("  Creating reactive agents")
                
            elif level == 1:
                # Level 2: Working memory agents
                config.updateFromDict({
                    "agent.exploration_rate": 0.3,
                    "agent.learning_rate": 0.02,
                    "agent.capacity": 100,
                    "agent.memory": "working",
                    "agent.prediction": True
                })
                print("  Creating working memory agents")
                
            else:
                # Level 3: Episodic memory agents
                config.updateFromDict({
                    "agent.exploration_rate": 0.2,
                    "agent.learning_rate": 0.03,
                    "agent.capacity": 200,
                    "agent.memory": "episodic",
                    "agent.prediction": True,
                    "agent.development": True
                })
                print("  Creating episodic memory agents")
            
            # Create agents for this level
            with nlm.Brain(config) as brain:
                brain.initialize()
                
                with nlm.SimpleWorld() as world:
                    world.createGridWorld(60.0, 60.0, 0.1)
                    world.setAgentStart(10.0, 10.0)
                    world.setAgentGoal(50.0, 50.0)
                    
                    with nlm.AgentBrain(brain) as agent:
                        agent.initialize(world)
                        agent.enableCuriosity(True)
                        agent.enableRewardModulation(True)
                        
                        hierarchical_agents.append({
                            'level': level + 1,
                            'agent': agent,
                            'config': config,
                            'world': world
                        })
        
        return hierarchical_agents
    
    def run_hierarchical_simulation(self, hierarchical_agents, episodes_per_level=3):
        """Run simulation with hierarchical agents."""
        print(f"\nRunning hierarchical simulation...")
        
        results = {}
        
        for agent_info in hierarchical_agents:
            level = agent_info['level']
            agent = agent_info['agent']
            world = agent_info['world']
            
            print(f"\nLevel {level} simulation:")
            
            level_results = []
            for episode in range(episodes_per_level):
                # Reset and run episode
                agent.reset()
                agent.initialize(world)
                
                # Run simulation
                for step in range(50):
                    world.update(0.1)
                    action = agent.decodeMotorCommand()
                    world.applyMotorCommand(action, world.getSimulationTime())
                
                # Get metrics
                metrics = agent.getPerformanceMetrics()
                metrics['episode'] = episode
                level_results.append(metrics)
                
                print(f"  Episode {episode + 1}: {metrics['success_rate']*100:.1f}% success")
            
            results[level] = level_results
        
        # Analyze hierarchical performance
        self.analyze_hierarchical_results(results)
        
        return results
    
    def analyze_hierarchical_results(self, results):
        """Analyze hierarchical system results."""
        print("\n=== Hierarchical System Analysis ===")
        print(f"{'Level':<8} {'Episodes':<10} {'Avg Success':<15} {'Avg Reward':<12} {'Avg Steps':<10}")
        print("-" * 70)
        
        for level, level_results in results.items():
            if not level_results:
                continue
            
            episodes = len(level_results)
            avg_success = sum(r['success_rate'] for r in level_results) / episodes
            avg_reward = sum(r['total_reward'] for r in level_results) / episodes
            avg_steps = sum(r['steps_taken'] for r in level_results) / episodes
            
            print(f"{level:<8} {episodes:<10} {avg_success*100:>7.1f}%{'':>8} {avg_reward:>8.3f} {'':>4} {avg_steps:<10.0f}")
        
        # Check if higher levels performed better
        levels = sorted(results.keys())
        if len(levels) > 1:
            print(f"\nPerformance trend:")
            for i in range(len(levels) - 1):
                lower_level = levels[i]
                higher_level = levels[i + 1]
                
                lower_success = sum(r['success_rate'] for r in results[lower_level]) / len(results[lower_level])
                higher_success = sum(r['success_rate'] for r in results[higher_level]) / len(results[higher_level])
                
                trend = "↑" if higher_success > lower_success else "↓"
                print(f"  Level {lower_level} → {higher_level}: {trend} ({lower_success*100:.1f}% → {higher_success*100:.1f}%)")
    
    def create_optimization_pipeline(self):
        """Create optimization pipeline for agent performance."""
        print("\nCreating optimization pipeline...")
        
        pipeline = [
            {
                'stage': 1,
                'name': 'Data Collection',
                'description': 'Collect agent experience data',
                'techniques': ['replay_buffer', 'experience_sampling']
            },
            {
                'stage': 2,
                'name': 'Feature Extraction',
                'description': 'Extract relevant features from experience',
                'techniques': ['feature_engineering', 'dimensionality_reduction']
            },
            {
                'stage': 3,
                'name': 'Policy Optimization',
                'description': 'Optimize agent policy based on features',
                'techniques': ['policy_gradient', 'value_function_approximation']
            },
            {
                'stage': 4,
                'name': 'Model-based Planning',
                'description': 'Incorporate model-based planning',
                'techniques': ['monte_carlo_tree_search', 'dynamic_programming']
            },
            {
                'stage': 5,
                'name': 'Performance Validation',
                'description': 'Validate and refine optimized agent',
                'techniques': ['cross_validation', 'meta_learning']
            }
        ]
        
        print("Optimization pipeline created:")
        for stage in pipeline:
            print(f"  Stage {stage['stage']}: {stage['name']}")
            print(f"    {stage['description']}")
            print(f"    Techniques: {', '.join(stage['techniques'])}")
        
        return pipeline
    
    def run_optimization_experiment(self, base_agents, pipeline):
        """Run optimization experiment on base agents."""
        print(f"\nRunning optimization experiment...")
        
        optimized_agents = []
        
        for i, agent in enumerate(base_agents):
            print(f"\nOptimizing agent {i + 1}/{len(base_agents)}...")
            
            # Simulate optimization pipeline stages
            for stage in pipeline:
                print(f"  Stage {stage['stage']}: {stage['name']}...")
                
                # Simulate optimization work
                import time
                time.sleep(0.1)  # Simulate processing time
                
                # In real implementation, this would actually optimize the agent
                # For demo, just track that this stage was completed
                print(f"    Completed using techniques: {', '.join(stage['techniques'])}")
            
            # Create optimized version of the agent
            with nlm.Config() as config:
                config.setDefaults("high_performance")
                
                with nlm.Brain(config) as brain:
                    brain.initialize()
                    
                    with nlm.SimpleWorld() as world:
                        world.createGridWorld(70.0, 70.0, 0.1)
                        world.setAgentGoal(50.0, 50.0)
                        
                        with nlm.AgentBrain(brain) as optimized_agent:
                            optimized_agent.initialize(world)
                            optimized_agent.enableCuriosity(True)
                            optimized_agent.enableRewardModulation(True)
                            
                            # Run simulation to test optimization
                            for step in range(30):
                                world.update(0.1)
                                action = optimized_agent.decodeMotorCommand()
                                world.applyMotorCommand(action, world.getSimulationTime())
                            
                            optimized_agents.append(optimized_agent)
                            
                            print(f"    Optimized agent tested and ready")
        
        print(f"\n✓ Optimization complete! Optimized {len(optimized_agents)} agents.")
        return optimized_agents

# Usage example
print("=== Creating Advanced NLM System ===\n")

advanced_system = AdvancedNLMSystem()

try:
    # Create multiple agents
    agents = advanced_system.create_multiple_agents(3, "production")
    
    # Create hierarchical system
    hierarchical = advanced_system.create_hierarchical_system(3)
    
    # Run hierarchical simulation
    hierarchical_results = advanced_system.run_hierarchical_simulation(hierarchical, 2)
    
    # Create optimization pipeline
    pipeline = advanced_system.create_optimization_pipeline()
    
    # Run optimization experiment
    optimized_agents = advanced_system.run_optimization_experiment(agents, pipeline)
    
    print("\n🎉 Advanced NLM system created successfully!")
    print("\nSystem components:")
    print(f"  Multiple agents: {len(agents)}")
    print(f"  Hierarchical levels: {len(hierarchical)}")
    print(f"  Optimized agents: {len(optimized_agents)}")
    print(f"  Pipeline stages: {len(pipeline)}")
    
except Exception as e:
    print(f"\n❌ Failed to create advanced NLM system: {e}")
    import traceback
    traceback.print_exc()
```

---

## Summary

This intermediate tutorial has covered:

1. ✅ **Advanced Configuration Management:** Using configuration profiles and Pythonic convenience methods
2. ✅ **Advanced Agent Learning:** High-level training methods and sophisticated learning parameters
3. ✅ **Complex World Environments:** Multi-agent systems and advanced world features
4. ✅ **Serialization and Checkpointing:** Saving and loading simulations for reproducibility
5. ✅ **Error Handling and Robust Simulations:** Fault-tolerant simulation design
6. ✅ **Performance Optimization:** Memory management and performance optimization techniques
7. ✅ **Advanced Tutorials:** Intelligent agent systems and advanced NLM development

You now have the skills to:

- Build sophisticated intelligent agents with multiple learning features
- Create complex world environments with realistic challenges
- Implement robust simulation systems with error recovery
- Optimize performance for complex scenarios
- Build advanced agent systems with multiple integration patterns

These skills will enable you to create production-grade NLM applications and research systems.

**Ready for the next level? Check out the "Advanced NLM Development" tutorial for cutting-edge techniques!** 🚀
