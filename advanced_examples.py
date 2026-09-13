# NLM Python Library - Advanced Examples

This document provides advanced examples for using the NLM Python library.

## Advanced Agent Example: Maze Navigation

```python
import pynlm
import numpy as np

class MazeNavigator:
    def __init__(self, width=20, height=20):
        # Create configuration with advanced settings
        self.config = pynlm.createConfigWithDefaults()
        self.config.set("neuron_count", 2000)  # Larger brain
        self.config.set("region_count", 3)      # Multiple regions
        
        # Create brain with enhanced capabilities
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent with all systems enabled
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.enableAllSystems()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width, height, visionWidth=8, visionHeight=8)
        
        # Initialize agent with world
        self.agent.initialize(self.world)
        
        # Set up tracking
        self.step_count = 0
        self.reward_history = []
        self.position_history = []
        
    def run_episode(self, max_steps=1000, enable_visualization=False):
        """Run a complete episode with advanced tracking."""
        self.world.reset()
        self.agent.reset()
        
        # Set agent in center of maze
        self.world.setAgentStart(self.world.getWidth() // 2, 
                                self.world.getHeight() // 2)
        
        for step in range(max_steps):
            self.step_count = step
            
            # Get world state
            world_time = self.world.getSimulationTime()
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step, world_time)
            
            # Get motor command
            motor_cmd = self.agent.decodeMotorCommand()
            
            # Apply action in world
            self.world.applyMotorCommand(motor_cmd, world_time)
            
            # Calculate reward
            new_percept = self.world.getSensoryPercept()
            reward = new_percept.getInternal().empty() ? 0.0f : new_percept.getInternal()[0]
            
            # Enhanced reward calculation with exploration bonus
            if self.agent.getCuriosityLevel() > 0.7:
                reward += 0.5  # Bonus for exploring
                
            # Track reward
            self.reward_history.append(reward)
            
            # Track position
            body = self.world.getAgentBody()
            self.position_history.append((body.x, body.y))
            
            # Apply neuromodulation
            self.agent.applyRewardModulation(reward, self.agent.getPredictionError())
            
            # Update development
            self.agent.updateDevelopment(0.1)
            
            # Get brain statistics
            stats = self.agent.getBrainStats()
            
            # Custom callback for advanced users
            if enable_visualization:
                self._visualize_step(step, percept, motor_cmd, reward)
                
            # Check for episode termination
            if self._is_episode_done():
                break
                
        return self._get_episode_summary()
        
    def _is_episode_done(self):
        """Check if episode should end (reached goal or timeout)."""
        if self.step_count >= 1000:
            return True
            
        # Check if reached goal
        body = self.world.getAgentBody()
        if self._is_goal_position(body.x, body.y):
            return True
            
        # Check if agent died
        if body.health <= 0:
            return True
            
        return False
        
    def _is_goal_position(self, x, y):
        """Check if position is the goal."""
        # Define goal position
        goal_x = self.world.getWidth() - 1
        goal_y = self.world.getHeight() - 1
        return abs(x - goal_x) < 1.0 and abs(y - goal_y) < 1.0
        
    def _get_episode_summary(self):
        """Get summary of the episode."""
        stats = {
            'total_steps': self.step_count,
            'total_reward': sum(self.reward_history),
            'max_curiosity': max(self.agent.getCuriosityLevel() for _ in self.reward_history),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'final_dev_stage': self.agent.getDevelopmentalStage(),
            'position_change': self._calculate_position_change()
        }
        return stats
        
    def _calculate_position_change(self):
        """Calculate how far agent moved from start."""
        if len(self.position_history) < 2:
            return 0.0
            
        start_pos = self.position_history[0]
        end_pos = self.position_history[-1]
        
        return ((end_pos[0] - start_pos[0]) ** 2 + 
                (end_pos[1] - start_pos[1]) ** 2) ** 0.5
        
    def _visualize_step(self, step, percept, action, reward):
        """Custom visualization function."""
        # This would integrate with visualization tools
        # For now, just log key information
        print(f"Step {step}: Action={action}, Reward={reward:.3f}, "
              f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
              f"Firing={self.brain.getFiringNeuronCount()}")

# Example usage: Train an agent to navigate a maze
if __name__ == "__main__":
    # Create maze navigator
    navigator = MazeNavigator(width=30, height=30)
    
    # Run training episodes
    print("Starting maze navigation training...")
    
    for episode in range(10):
        print(f"\nEpisode {episode + 1}/10")
        
        summary = navigator.run_episode(
            max_steps=500,
            enable_visualization=(episode == 0)  # Only visualize first episode
        )
        
        print(f"  Steps: {summary['total_steps']}")
        print(f"  Total Reward: {summary['total_reward']:.2f}")
        print(f"  Max Curiosity: {summary['max_curiosity']:.3f}")
        print(f"  Final Development Stage: {summary['final_dev_stage']}")
        print(f"  Distance from start: {summary['position_change']:.2f}")
        
        # Save best brain state
        if summary['total_reward'] > 0:
            navigator.agent.saveBrainState(f"best_brain_episode_{episode + 1}.bin")
            
    print("\nTraining complete!")
```

## Advanced Configuration Examples

### 1. Custom Configuration Builder

```python
def create_advanced_config():
    """Create a configuration optimized for complex tasks."""
    config = pynlm.createConfigWithDefaults()
    
    # Brain configuration
    config.set("neuron_count", 5000)
    config.set("region_count", 4)
    config.set("connection_probability", 0.05)
    
    # Enhanced plasticity
    config.set("plasticity_learning_rate", 0.005)
    config.set("stdp_ltp_weight", 0.015)
    config.set("stdp_ltd_weight", 0.018)
    config.set("stdp_time_constant", 25.0)
    
    # Development settings
    config.set("development_synaptogenesis_rate", 0.002)
    config.set("development_pruning_rate", 0.0002)
    
    # Neuromodulation
    config.set("dopamine_baseline", 0.2)
    config.set("dopamine_scale", 2.0)
    config.set("curiosity_threshold", 0.3)
    
    # Memory systems
    config.set("working_memory_capacity", 500)
    config.set("episodic_memory_max_episodes", 5000)
    config.set("replay_interval", 200)
    
    # Performance
    config.set("simulation_timestep", 0.0005)
    config.set("checkpoint_dir", "./checkpoints")
    config.set("visualization_enabled", True)
    
    return config

# Usage
advanced_config = create_advanced_config()
brain = pynlm.createBrain(advanced_config)
```

### 2. Multi-Agent Simulation

```python
class MultiAgentSimulation:
    def __init__(self, num_agents=5):
        self.num_agents = num_agents
        self.agents = []
        self.shared_world = pynlm.createSimpleWorld()
        
        # Configure world
        self.shared_world.configure(50, 50, visionWidth=8, visionHeight=8)
        
    def create_agent(self, agent_id, brain_config=None):
        """Create an agent with optional custom configuration."""
        if brain_config is None:
            brain_config = pynlm.createConfigWithDefaults()
            brain_config.set("neuron_count", 1000)
            
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(self.shared_world)
        agent.enableAllSystems()
        
        self.agents.append({
            'id': agent_id,
            'agent': agent,
            'brain': brain,
            'last_percept': None
        })
        
        return agent
        
    def run_step(self):
        """Run one simulation step with all agents."""
        # Update world
        self.shared_world.update(0.1)
        
        # Get shared percept
        shared_percept = self.shared_world.getSensoryPercept()
        
        # Each agent processes the same world state
        for agent_info in self.agents:
            # Store previous percept for novelty calculation
            agent_info['last_percept'] = shared_percept
            
            # Process sensory input
            agent_info['agent'].processSensoryInput(shared_percept)
            
            # Run brain
            agent_info['brain'].step(agent_info['agent'].getBrain().getTotalSpikeCount())
            
            # Decode action
            action = agent_info['agent'].decodeMotorCommand()
            
            # Apply to world (agents can interfere with each other)
            self.shared_world.applyMotorCommand(action, self.shared_world.getSimulationTime())
            
            # Calculate and apply reward
            new_percept = self.shared_world.getSensoryPercept()
            reward = new_percept.getInternal().empty() ? 0.0f : new_percept.getInternal()[0]
            
            # Add social learning bonus if other agents nearby
            reward += self._calculate_social_bonus(agent_info, shared_percept)
            
            agent_info['agent'].applyRewardModulation(reward, agent_info['agent'].getPredictionError())
            
    def _calculate_social_bonus(self, agent_info, percept):
        """Calculate bonus for social learning."""
        # Simple social bonus: if agent is near other agents
        body = self.shared_world.getAgentBody()
        bonus = 0.0
        
        # Check for other agents (simplified)
        for other_agent in self.agents:
            if other_agent['id'] != agent_info['id']:
                other_body = other_agent['agent'].getBrain().getRegion(1).getPopulation(0).getNeurons()[0]
                # Calculate distance
                dx = body.x - other_body.x
                dy = body.y - other_body.y
                dist = (dx*dx + dy*dy)**0.5
                
                if dist < 5.0:  # Within social distance
                    bonus += 0.1
                    
        return bonus
        
    def run_simulation(self, num_steps):
        """Run the complete multi-agent simulation."""
        for step in range(num_steps):
            self.run_step()
            
        return self.get_simulation_stats()
        
    def get_simulation_stats(self):
        """Get statistics from the simulation."""
        stats = {
            'total_steps': self.num_agents,
            'agents': [],
            'world_state': {}
        }
        
        for agent_info in self.agents:
            brain_stats = agent_info['agent'].getBrainStats()
            stats['agents'].append({
                'id': agent_info['id'],
                'stats': brain_stats
            })
            
        stats['world_state'] = {
            'time': self.shared_world.getSimulationTime(),
            'agent_count': len(self.agents)
        }
        
        return stats

# Usage example
if __name__ == "__main__":
    # Create multi-agent simulation
    sim = MultiAgentSimulation(num_agents=3)
    
    # Create agents with different configurations
    for i in range(3):
        config = pynlm.createConfigWithDefaults()
        config.set("neuron_count", 800 + i * 200)  # Different sizes
        sim.create_agent(i, config)
        
    # Run simulation
    print("Running multi-agent simulation...")
    final_stats = sim.run_simulation(num_steps=200)
    
    print("\nSimulation Results:")
    for agent_stats in final_stats['agents']:
        print(f"Agent {agent_stats['id']}:")
        print(f"  Neurons: {agent_stats['stats']['total_neurons']}")
        print(f"  Total Reward: {agent_stats['stats']['total_reward']:.2f}")
        print(f"  Development Stage: {agent_stats['stats']['dev_stage']}")
```

## Performance Optimization Examples

### 1. Brain Serialization and Loading

```python
def save_and_load_brain_example():
    """Example of saving and loading brain states."""
    import time
    
    # Create and initialize brain
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    agent.initialize(world)
    
    # Run some simulation
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
    print("Brain state after 100 steps:")
    stats = agent.getBrainStats()
    for key, value in stats.items():
        print(f"  {key}: {value}")
        
    # Save brain state
    brain_file = "brain_checkpoint.bin"
    if agent.saveBrainState(brain_file):
        print(f"\nBrain state saved to {brain_file}")
        
        # Create new brain and load saved state
        new_config = pynlm.createConfigWithDefaults()
        new_brain = pynlm.createBrain(new_config)
        new_brain.initialize()
        
        # Create new agent
        new_agent = pynlm.createAgentBrain(new_brain)
        new_agent.initialize(world)
        
        # Load the saved state
        if new_agent.loadBrainState(brain_file):
            print("Successfully loaded saved brain state!")
            
            # Verify loading worked
            new_stats = new_agent.getBrainStats()
            print(f"Loaded brain has {new_stats['total_neurons']} neurons")
            print(f"Total spikes: {new_stats['total_spikes']}")
            
            # Continue simulation from saved state
            for step in range(100, 150):
                world.update(0.1)
                new_agent.processSensoryInput(world.getSensoryPercept())
                new_brain.step(step)
                action = new_agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
            print(f"\nAfter additional simulation:")
            print(f"Total spikes: {new_brain.getTotalSpikeCount()}")
            
    return brain_file

# Usage
checkpoint_file = save_and_load_brain_example()
```

### 2. Custom Plasticity Configuration

```python
def customize_plasticity():
    """Example of customizing plasticity rules."""
    # Create brain
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get plasticity systems
    stdp = brain.getSTDP()
    hebbian = brain.getHebbian()
    
    # Customize STDP parameters
    print("Customizing STDP parameters:")
    print(f"Original LTP weight: {stdp.getLTPWeight()}")
    print(f"Original LTD weight: {stdp.getLTDWeight()}")
    
    # Set new parameters based on task requirements
    stdp->configure(
        ltpWeight=0.02,    # Stronger potentiation for learning important patterns
        ltdWeight=0.025,   # Stronger depression for pruning
        tau=15.0          # Faster decay for quick adaptation
    )
    
    print("New LTP weight: {:.4f}".format(stdp.getLTPWeight()))
    print("New LTD weight: {:.4f}".format(stdp.getLTDWeight()))
    
    # Customize Hebbian learning
    print("\nCustomizing Hebbian parameters:")
    print(f"Original learning rate: {hebbian.getLearningRate()}")
    
    hebbian.setLearningRate(0.008)  # Moderate learning rate
    print(f"New learning rate: {hebbian.getLearningRate()}")
    
    return brain

# Usage
customized_brain = customize_plasticity()
```

### 3. Development System Control

```python
def advanced_development_control():
    """Example of advanced development system control."""
    # Create brain with development enabled
    config = pynlm.createConfigWithDefaults()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    print("Development System Control Example")
    print("=" * 40)
    
    # Simulate different developmental stages
    developmental_stages = [
        (0.0, "Initial - High Plasticity"),
        (60.0, "Critical Period - Learning"),
        (300.0, "Maturation - Specialization"),
        (900.0, "Adult - Stability")
    ]
    
    for age, stage_name in developmental_stages:
        # Update development to target age
        agent.updateDevelopment(age)
        
        # Get current developmental stage
        stage = agent.getDevelopmentalStage()
        plasticity = agent.getBrain()->getStructuralPlasticity()->getSynaptogenesisRate()
        
        print(f"\nAge: {age:.1f}s - {stage_name}")
        print(f"  Developmental Stage: {stage}")
        print(f"  Plasticity Modifier: {agent.getBrain()->getDevelopmentalStage()}")
        print(f"  Synaptogenesis Rate: {plasticity:.6f}")
        print(f"  Pruning Rate: {0.00001f * (2.0f - agent.getBrain()->getDevelopmentSystem()->getPlasticityFactor())}")
        
    # Example of developmental intervention
    print("\n\nDevelopmental Intervention")
    print("=" * 40)
    
    # Manually trigger critical period
    agent.setDevelopmentalStage(pynlm.DevelopmentalStage::CriticalPeriod)
    
    # Force rapid synaptogenesis
    brain->getStructuralPlasticity()->setSynaptogenesisRate(0.001)
    brain->getStructuralPlasticity()->setPruningRate(0.00005)
    
    print("Triggered Critical Period:")
    print(f"  Synaptogenesis: {brain->getStructuralPlasticity()->getSynaptogenesisRate():.6f}")
    print(f"  Pruning: {brain->getStructuralPlasticity()->getPruningRate():.6f}")
    
    return agent

# Usage
advanced_agent = advanced_development_control()
```

## Error Handling and Best Practices

### 1. Robust Error Handling

```python
def robust_simulation():
    """Example of robust error handling in simulations."""
    try:
        # Initialize components
        config = pynlm.createConfigWithDefaults()
        brain = pynlm.createBrain(config)
        
        if not brain.initialize():
            raise RuntimeError("Failed to initialize brain")
            
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        agent.initialize(world)
        
        print("Starting simulation...")
        
        # Run simulation with error handling
        for step in range(1000):
            try:
                # Update world
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain
                brain.step(step)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Occasionally save checkpoint
                if step % 100 == 0:
                    if agent.saveBrainState(f"checkpoint_{step}.bin"):
                        print(f"Saved checkpoint at step {step}")
                    else:
                        print(f"Warning: Failed to save checkpoint at step {step}")
                        
            except Exception as e:
                print(f"Error at step {step}: {e}")
                # Attempt recovery
                if step > 0:
                    print("Attempting recovery...")
                    brain.reset()
                    agent.reset()
                    world.reset()
                    agent.initialize(world)
                    
    except Exception as e:
        print(f"Fatal error: {e}")
        return False
        
    print("Simulation completed successfully")
    return True

# Usage
robust_simulation()
```

### 2. Configuration Validation

```python
def validate_and_fix_config(config):
    """Example of configuration validation and auto-correction."""
    print("Validating configuration...")
    
    errors = []
    warnings = []
    
    # Check neuron count
    neuron_count = config.getOr<int64_t>("neuron_count", 1000)
    if neuron_count < 100:
        errors.append("Neuron count too low (minimum 100)")
        config.set("neuron_count", 100)  # Auto-fix
    elif neuron_count > 10000:
        warnings.append("Large neuron count may impact performance")
        
    # Check region count
    region_count = config.getOr<int64_t>("region_count", 1)
    if region_count < 1:
        errors.append("Region count must be at least 1")
        config.set("region_count", 1)
        
    # Check connection probability
    conn_prob = config.getOr<float>("connection_probability", 0.1)
    if conn_prob < 0.01:
        warnings.append("Very low connection probability - brain may be too sparse")
    elif conn_prob > 0.5:
        warnings.append("High connection probability - brain may be too dense")
        
    # Check timestep
    timestep = config.getOr<double>("simulation_timestep", 0.001)
    if timestep <= 0.0:
        errors.append("Simulation timestep must be positive")
        config.set("simulation_timestep", 0.001)
    elif timestep > 0.1:
        warnings.append("Large timestep may affect stability")
        
    # Check plasticity parameters
    ltp_weight = config.getOr<float>("stdp_ltp_weight", 0.01)
    ltd_weight = config.getOr<float>("stdp_ltd_weight", 0.012)
    if abs(ltp_weight - ltd_weight) < 0.001:
        warnings.append("STDP LTP and LTD weights are nearly equal - may not learn effectively")
        
    # Report results
    if errors:
        print("Configuration errors (fixed):")
        for error in errors:
            print(f"  - {error}")
            
    if warnings:
        print("Configuration warnings:")
        for warning in warnings:
            print(f"  - {warning}")
            
    if not errors and not warnings:
        print("Configuration is valid!")
        
    return len(errors) == 0

# Usage and testing
print("Example 1: Valid configuration")
valid_config = pynlm.createConfigWithDefaults()
validate_and_fix_config(valid_config)

print("\nExample 2: Invalid configuration (with fixes)")
invalid_config = pynlm.createConfigWithDefaults()
invalid_config.set("neuron_count", 50)  # Too low
invalid_config.set("region_count", 0)   # Invalid
invalid_config.set("connection_probability", 0.6)  # Too high
invalid_config.set("simulation_timestep", 0.0)      # Invalid
validate_and_fix_config(invalid_config)

print(f"\nFinal neuron count: {invalid_config.getOr<int64_t>('neuron_count', 1000)}")
print(f"Final region count: {invalid_config.getOr<int64_t>('region_count', 1)}")
```

This document provides comprehensive examples for using the NLM Python library at an advanced level, including:

1. **Complex agent behaviors** with state management and episode tracking
2. **Multi-agent simulations** with social learning and interaction
3. **Performance optimization** through serialization, checkpointing, and custom configurations
4. **Advanced development system control** for different behavioral stages
5. **Robust error handling** for production simulations
6. **Configuration validation** with auto-correction

These examples demonstrate how to build sophisticated neural simulations while maintaining good software practices and error handling.