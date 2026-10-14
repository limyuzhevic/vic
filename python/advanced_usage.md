# NLM (Neural Learning Machine) - Advanced Usage Guide

## Overview

This document covers advanced usage patterns, performance optimization, and debugging techniques for the NLM Python API.

## Performance Optimization

### 1. Simulation Batching

For running multiple simulations efficiently, use batch processing:

```python
import pynlm
from pynlm import create_default_config, create_brain

# Create multiple configurations
configs = []
for i in range(5):
    config = create_default_config()
    config.set("neuron_count", 500)  # Smaller brains for faster testing
    config.set("random_seed", 42 + i)  # Different seeds
    configs.append(config)

# Run batch simulation
brains = pynlm.batch_sims(configs, steps_per_config=100)

# Analyze results
for i, brain in enumerate(brains):
    stats = brain.get_simulation_stats()
    print(f"Simulation {i}: {stats['steps_per_second']:.2f} steps/sec, "
          f"{stats['final_neuron_count']} neurons, "
          f"{stats['total_spikes']} spikes")
```

### 2. Reproducible Experiments

Ensure reproducible experiments by setting seeds properly:

```python
import pynlm

# Create reproducible simulation
brain = pynlm.create_reproducible_simulation(seed=12345)

# Run simulation multiple times with same seed for comparison
sim1 = pynlm.create_reproducible_simulation(seed=12345)
sim2 = pynlm.create_reproducible_simulation(seed=12345)

# Results should be identical
brain.run_simulation(100)
sim1.run_simulation(100)
sim2.run_simulation(100)

# Compare final states
stats1 = sim1.get_simulation_stats()
stats2 = sim2.get_simulation_stats()
assert stats1["total_spikes"] == stats2["total_spikes"]
```

### 3. Performance Profiling

Profile simulation performance:

```python
import pynlm
import time

brain = pynlm.create_default_config()

# Profile different brain sizes
for neuron_count in [100, 500, 1000, 2000]:
    config = pynlm.create_default_config()
    config.set("neuron_count", neuron_count)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Profile performance
    result = pynlm.profile_simulation(brain, steps=1000)
    
    print(f"Neuron count: {neuron_count}")
    print(f"  Steps/sec: {result['steps_per_second']:.2f}")
    print(f"  Time for 1000 steps: {result['total_time_ms']:.2f} ms")
    print(f"  Spikes per step: {result['final_spike_count']/1000:.2f}")
```

## Advanced Configuration

### 1. Configuration Validation

Validate configurations before running simulations:

```python
import pynlm

config = pynlm.create_default_config()
config.set("neuron_count", 0)  # Invalid
config.set("connection_probability", 1.5)  # Invalid

# Check for issues
validation_issues = config.validate()
if validation_issues:
    print("Configuration issues found:")
    for key, issue in validation_issues.items():
        print(f"  {key}: {issue}")
else:
    print("Configuration is valid!")

# Fix configuration
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)
validation_issues = config.validate()
print(f"After fixes: {'Valid' if not validation_issues else 'Invalid'}")
```

### 2. Configuration Schemas

Use configuration schemas for complex experiments:

```python
def create_experiment_config(base_config, **overrides):
    """Create experiment configuration with validation."""
    config = pynlm.create_default_config()
    
    # Copy base values if provided
    if base_config:
        for key in base_config.get_keys():
            value = base_config.get(key)
            if value is not None:
                config.set(key, value)
    
    # Apply overrides
    for key, value in overrides.items():
        config.set(key, value)
    
    # Validate experiment-specific parameters
    validation_issues = config.validate()
    if validation_issues:
        raise pynlm.ConfigurationError(
            f"Invalid experiment configuration: {validation_issues}"
        )
    
    return config

# Use the factory
base_config = pynlm.create_default_config()
base_config.set("neuron_count", 500)

config1 = create_experiment_config(
    base_config,
    random_seed=123,
    connection_probability=0.05,
    stdp_learning_rate=0.002
)

config2 = create_experiment_config(
    base_config,
    random_seed=456,
    connection_probability=0.15,
    stdp_learning_rate=0.001
)
```

## Agent Development

### 1. Custom Agent Classes

Create custom agent classes by extending the base functionality:

```python
import pynlm

class AdvancedAgent:
    def __init__(self, brain, world):
        self.brain = pynlm.create_agent_brain(brain)
        self.world = world
        self.world.configure(20, 20, 8, 8)
        self.world.reset()
        self.brain.initialize(self.world)
        
        # Enable advanced learning systems
        self.brain.enable_all_systems()
        
        # Custom agent parameters
        self.exploration_rate = 0.1
        self.epsilon = 0.01
        self.action_history = []
    
    def run_episode(self, max_steps=1000, enable_exploration=True):
        """Run a single episode with exploration."""
        for step in range(max_steps):
            # Get sensory input
            percept = self.world.get_sensory_percept()
            
            # Process with brain
            self.brain.process_sensory_input(percept)
            
            # Get brain action
            brain_action = self.brain.decode_motor_command()
            
            # Add exploration if enabled
            action = self._choose_action(brain_action, enable_exploration)
            
            # Execute action
            self.world.apply_motor_command(action, self.world.get_simulation_time())
            
            # Get reward
            reward = self._get_reward()
            
            # Apply reward modulation
            self.brain.apply_reward_modulation(reward, 0.0)
            
            # Update development
            self.brain.update_development(0.1)
            
            # Store experience
            self.action_history.append({
                'step': step,
                'percept': percept,
                'action': action,
                'reward': reward,
                'brain_action': brain_action
            })
            
            # Log progress
            if step % 100 == 0:
                self._log_step(step, action, reward)
        
        return self._analyze_episode()
    
    def _choose_action(self, brain_action, enable_exploration):
        """Choose action with optional exploration."""
        if not enable_exploration:
            return brain_action
        
        # Epsilon-greedy action selection
        import random
        if random.random() < self.epsilon:
            # Explore: choose random action
            action_types = list(pynlm.ActionType)
            return action_types[random.randint(0, len(action_types) - 1)]
        else:
            # Exploit: use brain decision
            return brain_action
    
    def _get_reward(self):
        """Compute reward from environment."""
        # Example reward: positive for being in center, negative near edges
        body = self.world.get_agent_body()
        center_x, center_y = 10.0, 10.0
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        return max(0.0, 10.0 - distance)
    
    def _log_step(self, step, action, reward):
        """Log step progress."""
        brain = self.brain.get_brain()
        stats = brain.get_simulation_stats()
        agent_stats = self.brain.get_agent_stats()
        
        print(f"Step {step}:")
        print(f"  Position: ({body.x:.2f}, {body.y:.2f})")
        print(f"  Action: {action}")
        print(f"  Reward: {reward:.2f}")
        print(f"  Curiosiy: {agent_stats['curiosity']:.3f}")
        print(f"  Novelty: {agent_stats['novelty']:.3f}")
        print(f"  Firing rate: {stats['average_firing_rate']:.2f}")
    
    def _analyze_episode(self):
        """Analyze episode results and return statistics."""
        brain = self.brain.get_brain()
        agent_stats = self.brain.get_agent_stats()
        
        total_reward = sum(exp['reward'] for exp in self.action_history)
        
        analysis = {
            'total_reward': total_reward,
            'steps': len(self.action_history),
            'curiosity': agent_stats['curiosity'],
            'novelty': agent_stats['novelty'],
            'prediction_error': agent_stats['prediction_error'],
            'final_position': (self.world.get_agent_body().x, self.world.get_agent_body().y),
            'avg_reward': total_reward / len(self.action_history) if self.action_history else 0
        }
        
        return analysis

# Use the advanced agent
brain = pynlm.create_brain(pynlm.create_default_config())
agent = AdvancedAgent(brain, pynlm.create_world())

# Run episode
results = agent.run_episode(max_steps=1000)
print(f"Episode completed with total reward: {results['total_reward']:.2f}")
print(f"Average reward per step: {results['avg_reward']:.3f}")
```

### 2. Agent with Memory Integration

Create agents that leverage brain memory systems:

```python
import pynlm

class MemoryAgent:
    def __init__(self, brain, world):
        self.brain = pynlm.create_agent_brain(brain)
        self.world = world
        self.world.configure(15, 15, 8, 8)
        self.world.reset()
        self.brain.initialize(self.world)
        
        # Enable all systems
        self.brain.enable_all_systems()
        
        # Access memory systems
        self.working_memory = brain.get_working_memory()
        self.episodic_memory = brain.get_episodic_memory()
        
        self.episode_count = 0
    
    def run_learning_episode(self, max_steps=500):
        """Run episode with memory learning."""
        for step in range(max_steps):
            # Get current percept
            percept = self.world.get_sensory_percept()
            
            # Store experience in episodic memory
            self.episodic_memory.store_experience(
                step, percept, self.world.get_agent_body(), self.brain.get_prediction_error()
            )
            
            # Process sensory input
            self.brain.process_sensory_input(percept)
            
            # Get action
            action = self.brain.decode_motor_command()
            
            # Apply action
            self.world.apply_motor_command(action, self.world.get_simulation_time())
            
            # Get reward
            reward = self._compute_reward()
            
            # Apply reward and update development
            self.brain.apply_reward_modulation(reward, self.brain.get_prediction_error())
            self.brain.update_development(0.1)
            
            # Replay important memories occasionally
            if step % 100 == 0:
                self._replay_memories(step)
        
        self.episode_count += 1
        return self._analyze_episode(step)
    
    def _compute_reward(self):
        """Compute reward with memory considerations."""
        body = self.world.get_agent_body()
        
        # Basic reward: closer to center is better
        center_x, center_y = 7.5, 7.5
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        base_reward = max(0.0, 10.0 - distance)
        
        # Novelty bonus: higher reward for novel states
        novelty_bonus = self.brain.get_novelty_level() * 2.0
        
        return base_reward + novelty_bonus
    
    def _replay_memories(self, current_step):
        """Replay important memories for consolidation."""
        # Get significant memories from working memory
        significant_memories = self.working_memory.get_significant_memories(0.5)
        
        # Replay each memory
        for memory in significant_memories:
            # Reconstruct mental state
            # (Implementation depends on memory system API)
            pass
        
        # Also replay recent episodic memories
        recent_memories = self.episodic_memory.get_recent_memories(10)
        for memory in recent_memories:
            # Reapply learning from these memories
            pass
    
    def get_memory_stats(self):
        """Get comprehensive memory statistics."""
        working_mem = self.working_memory
        episodic_mem = self.episodic_memory
        
        return {
            'working_memory_size': working_mem.get_memory_size(),
            'episodic_memory_entries': episodic_mem.get_entry_count(),
            'retrieval_success_rate': episodic_mem.get_retrieval_success_rate(),
            'memory_consolidation_count': episodic_mem.get_consolidation_count(),
            'novelty_level': self.brain.get_novelty_level(),
            'curiosity': self.brain.get_curiosity_level()
        }

# Use memory agent
brain = pynlm.create_brain(pynlm.create_default_config())
memory_agent = MemoryAgent(brain, pynlm.create_world())

# Run learning episodes
for episode in range(5):
    results = memory_agent.run_learning_episode()
    memory_stats = memory_agent.get_memory_stats()
    
    print(f"Episode {episode + 1}:")
    print(f"  Total reward: {results['total_reward']:.2f}")
    print(f"  Memory size: {memory_stats['working_memory_size']}")
    print(f"  Episodic entries: {memory_stats['episodic_memory_entries']}")
    print(f"  Novelty: {memory_stats['novelty_level']:.3f}")
```

## Visualization and Debugging

### 1. Basic Visualization

Create simple text-based visualization:

```python
import pynlm

def visualize_simulation(brain, world, agent, steps=100):
    """Simple text-based visualization of simulation."""
    print("=== NLM Simulation Visualization ===")
    print(f"World: {world.get_width()}x{world.get_height()} pixels")
    print(f"Brain: {brain.get_total_neuron_count()} neurons, {brain.get_total_synapse_count()} synapses")
    print()
    
    for step in range(steps):
        world.update(0.1)
        
        # Get state
        body = world.get_agent_body()
        percept = world.get_sensory_percept()
        
        # Process and act
        agent.process_sensory_input(percept)
        brain.step(step)
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Print state
        print(f"Step {step:3d}: Pos({body.x:5.1f},{body.y:5.1f}) "
              f"Action:{str(action):12s} "
              f"Firing:{brain.get_firing_neuron_count():4d} "
              f"Spikes:{brain.get_total_spike_count():6d}")
        
        if step % 20 == 0:
            # Print agent state
            agent_stats = agent.get_agent_stats()
            print(f"        Curiosity:{agent_stats['curiosity']:5.3f} "
                  f"Novelty:{agent_stats['novelty']:5.3f} "
                  f"PredError:{agent_stats['prediction_error']:7.3f}")
    
    print("=== End of Visualization ===")
    return step

# Create and run visualization
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

world = pynlm.create_world()
world.configure(20, 20, 8, 8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable learning
agent.enable_all_systems()

# Run visualization
visualize_simulation(brain, world, agent, steps=50)
```

### 2. Performance Monitoring

Monitor simulation performance in real-time:

```python
import pynlm
import time
import threading

class PerformanceMonitor:
    def __init__(self, brain, world, agent, interval=5.0):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.interval = interval
        self.monitoring = False
        self.monitor_thread = None
        self.performance_data = []
    
    def start_monitoring(self):
        """Start performance monitoring in background thread."""
        self.monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
    
    def _monitor_loop(self):
        """Background monitoring loop."""
        while self.monitoring:
            try:
                # Get performance data
                data = {
                    'step': self.brain.get_total_spike_count(),
                    'firing_neurons': self.brain.get_firing_neuron_count(),
                    'average_firing_rate': self.brain.get_average_firing_rate(),
                    'curiosity': self.agent.get_curiosity_level(),
                    'novelty': self.agent.get_novelty_level(),
                    'prediction_error': self.agent.get_prediction_error(),
                    'time': time.time()
                }
                self.performance_data.append(data)
                
                time.sleep(self.interval)
            except Exception as e:
                print(f"Monitoring error: {e}")
                break
    
    def get_performance_stats(self):
        """Get aggregated performance statistics."""
        if not self.performance_data:
            return None
        
        spikes = [d['step'] for d in self.performance_data]
        firing = [d['firing_neurons'] for d in self.performance_data]
        rates = [d['average_firing_rate'] for d in self.performance_data]
        curiosities = [d['curiosity'] for d in self.performance_data]
        novelties = [d['novelty'] for d in self.performance_data]
        
        return {
            'samples': len(self.performance_data),
            'total_steps': spikes[-1] if spikes else 0,
            'avg_firing_neurons': sum(firing) / len(firing) if firing else 0,
            'avg_firing_rate': sum(rates) / len(rates) if rates else 0,
            'avg_curiosity': sum(curiosities) / len(curiosities) if curiosities else 0,
            'avg_novelty': sum(novelties) / len(novelties) if novelties else 0,
            'max_firing_rate': max(rates) if rates else 0,
            'min_firing_rate': min(rates) if rates else 0
        }
    
    def plot_performance(self):
        """Plot performance data (requires matplotlib)."""
        try:
            import matplotlib.pyplot as plt
            
            if not self.performance_data:
                print("No performance data to plot.")
                return
            
            steps = [d['step'] for d in self.performance_data]
            firing_rates = [d['average_firing_rate'] for d in self.performance_data]
            curiosities = [d['curiosity'] for d in self.performance_data]
            
            plt.figure(figsize=(12, 8))
            
            plt.subplot(3, 1, 1)
            plt.plot(steps, firing_rates)
            plt.ylabel('Firing Rate')
            plt.title('NLM Performance Monitoring')
            
            plt.subplot(3, 1, 2)
            plt.plot(steps, curiosities)
            plt.ylabel('Curiosity')
            plt.xlabel('Step')
            
            plt.subplot(3, 1, 3)
            agent_brain = self.agent.get_brain()
            time_series = [agent_brain.get_developmental_stage() for _ in range(len(steps))]
            plt.plot(steps, time_series)
            plt.ylabel('Developmental Stage')
            plt.xlabel('Step')
            
            plt.tight_layout()
            plt.show()
            
        except ImportError:
            print("matplotlib not available for plotting")

# Use performance monitoring
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

world = pynlm.create_world()
world.configure(15, 15, 8, 8)
world.reset()

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
agent.enable_all_systems()

# Start monitoring
monitor = PerformanceMonitor(brain, world, agent, interval=2.0)
monitor.start_monitoring()

# Run simulation for a while
for step in range(200):
    world.update(0.1)
    percept = world.get_sensory_percept()
    agent.process_sensory_input(percept)
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
    
    if step % 50 == 0:
        print(f"Step {step} completed")

# Stop monitoring and get stats
monitor.stop_monitoring()
stats = monitor.get_performance_stats()
print(f"Performance stats: {stats}")

# Optionally plot (if matplotlib available)
# monitor.plot_performance()
```

## Error Handling and Recovery

### 1. Safe Simulation

Run simulations with robust error handling:

```python
import pynlm

def safe_simulation_step(brain, step, max_retries=3):
    """Safely perform a simulation step with error recovery."""
    for attempt in range(max_retries):
        try:
            brain.step(step)
            return True
        except pynlm.SimulationError as e:
            print(f"Simulation error on attempt {attempt + 1}: {e}")
            if attempt == max_retries - 1:
                raise
            # Try to recover
            brain.reset()
            brain.initialize()
        except Exception as e:
            print(f"Unexpected error: {e}")
            return False
    
    return False

def safe_simulation_run(brain, steps, timestep=0.1):
    """Run simulation with comprehensive error handling."""
    successful_steps = 0
    failed_steps = 0
    
    for step in range(steps):
        if safe_simulation_step(brain, step):
            successful_steps += 1
        else:
            failed_steps += 1
            # Log failure
            print(f"Step {step} failed")
    
    print(f"Simulation completed: {successful_steps} successful, {failed_steps} failed")
    return successful_steps, failed_steps

# Use safe simulation
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

successful, failed = safe_simulation_run(brain, 1000)
print(f"Simulation efficiency: {successful/(successful + failed)*100:.1f}%")
```

### 2. Validation and Recovery

Validate and recover from invalid states:

```python
import pynlm

class RobustAgent:
    def __init__(self, brain, world):
        self.brain = pynlm.create_agent_brain(brain)
        self.world = world
        self.world.configure(10, 10, 8, 8)
        self.world.reset()
        self.brain.initialize(self.world)
        
        self.enable_all_systems()
        
        # State tracking for recovery
        self.last_valid_state = None
        self.recovery_attempts = 0
        self.max_recovery_attempts = 3
    
    def run_safe_episode(self, max_steps=500):
        """Run episode with recovery from invalid states."""
        for step in range(max_steps):
            try:
                # Check if state is valid
                if not self._validate_state():
                    print(f"Invalid state detected at step {step}, attempting recovery")
                    if not self._recover_from_invalid_state():
                        print("Recovery failed, terminating episode")
                        break
                
                # Normal simulation step
                self._safe_step(step)
                
            except pynlm.SimulationError as e:
                print(f"Simulation error at step {step}: {e}")
                if not self._recover_from_error(e):
                    print("Error recovery failed")
                    break
            
            except Exception as e:
                print(f"Unexpected error at step {step}: {e}")
                if not self._recover_from_unexpected_error(e):
                    print("Unexpected error recovery failed")
                    break
        
        return self._get_episode_results()
    
    def _validate_state(self):
        """Validate current simulation state."""
        try:
            brain = self.brain.get_brain()
            
            # Check basic invariants
            if brain.get_total_neuron_count() == 0:
                return False
            
            if brain.get_total_synapse_count() < 0:
                return False
            
            # Check for NaN or infinite values
            float_stats = brain.get_simulation_stats()
            for key, value in float_stats.items():
                if isinstance(value, float):
                    if not (float('-inf') < value < float('inf')):
                        return False
            
            return True
            
        except Exception as e:
            print(f"Validation error: {e}")
            return False
    
    def _recover_from_invalid_state(self):
        """Recover from invalid state."""
        self.recovery_attempts += 1
        
        if self.recovery_attempts > self.max_recovery_attempts:
            print("Max recovery attempts reached")
            return False
        
        try:
            # Reset to last valid state
            if self.last_valid_state:
                brain = self.brain.get_brain()
                brain.reset()
                brain.initialize()
                return True
            
            # Or reinitialize completely
            self.brain = pynlm.create_agent_brain(self.brain.get_brain())
            self.brain.initialize(self.world)
            self.enable_all_systems()
            
            return True
            
        except Exception as e:
            print(f"Recovery error: {e}")
            return False
    
    def _recover_from_error(self, error):
        """Recover from specific error type."""
        if isinstance(error, pynlm.ConfigurationError):
            # Try to fix configuration
            print("Fixing configuration error")
            # Implementation depends on error details
            return True
        elif isinstance(error, pynlm.SimulationError):
            # Simulation error, try to reset
            print("Simulation error, attempting reset")
            return self._recover_from_invalid_state()
        else:
            return False
    
    def _recover_from_unexpected_error(self, error):
        """Recover from unexpected errors."""
        print(f"Unexpected error: {error}")
        print("Attempting unexpected error recovery")
        
        # Save current state before recovery
        self.last_valid_state = {
            'brain_stats': self.brain.get_brain().get_simulation_stats(),
            'world_state': self.world.get_agent_body(),
            'agent_stats': self.brain.get_agent_stats()
        }
        
        # Attempt recovery
        try:
            self.brain.get_brain().reset()
            self.brain.get_brain().initialize()
            return True
        except Exception as e:
            print(f"Recovery failed: {e}")
            return False
    
    def _safe_step(self, step):
        """Perform a single simulation step safely."""
        # Get sensory input
        percept = self.world.get_sensory_percept()
        self.brain.process_sensory_input(percept)
        
        # Step brain with error handling
        if not pynlm.safe_simulation_step(self.brain.get_brain(), step):
            raise pynlm.SimulationError(f"Failed to perform step {step}")
        
        # Get action
        action = self.brain.decode_motor_command()
        
        # Apply action
        self.world.apply_motor_command(action, self.world.get_simulation_time())
        
        # Apply reward if available
        reward = self._get_reward()
        self.brain.apply_reward_modulation(reward, 0.0)
        
        # Update development
        self.brain.update_development(0.1)
        
        # Save state for potential recovery
        self.last_valid_state = {
            'step': step,
            'action': action,
            'reward': reward,
            'curiosity': self.brain.get_curiosity_level()
        }
    
    def _get_reward(self):
        """Compute reward."""
        body = self.world.get_agent_body()
        # Simple reward: closer to center is better
        center_x, center_y = 5.0, 5.0
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        return max(0.0, 10.0 - distance)
    
    def _get_episode_results(self):
        """Get episode results with error handling."""
        brain = self.brain.get_brain()
        agent_stats = self.brain.get_agent_stats()
        
        return {
            'total_steps_completed': self.recovery_attempts,
            'final_state_valid': self._validate_state(),
            'curiosity': agent_stats['curiosity'],
            'novelty': agent_stats['novelty'],
            'prediction_error': agent_stats['prediction_error'],
            'recovery_attempts': self.recovery_attempts,
            'simulation_efficiency': 1.0 - (self.recovery_attempts / (self.recovery_attempts + 1))
        }

# Use robust agent
brain = pynlm.create_brain(pynlm.create_default_config())
robust_agent = RobustAgent(brain, pynlm.create_world())

# Run robust episode
results = robust_agent.run_safe_episode(max_steps=200)
print(f"Episode results: {results}")
```

## Advanced Examples

### 1. Multi-Objective Optimization

Use NLM for multi-objective optimization:

```python
import pynlm

class MultiObjectiveAgent:
    def __init__(self, brain, world, objectives):
        self.brain = pynlm.create_agent_brain(brain)
        self.world = world
        self.world.configure(12, 12, 6, 6)
        self.world.reset()
        self.brain.initialize(self.world)
        
        self.objectives = objectives  # List of objective functions
        self.enable_all_systems()
        
        self.objective_history = []
    
    def optimize_multiple_objectives(self, iterations=100):
        """Optimize multiple objectives simultaneously."""
        best_objectives = {name: float('inf') for name in self.objectives.keys()}
        best_state = None
        
        for iteration in range(iterations):
            # Reset with random seed for diversity
            seed = iteration * 12345 + hash(str(self.objectives)) % 1000
            self.brain.get_brain().reset()
            self.brain.get_brain().initialize()
            
            # Run simulation
            for step in range(50):
                self._simulate_step()
            
            # Evaluate objectives
            current_objectives = self._evaluate_objectives()
            self.objective_history.append(current_objectives)
            
            # Check if this is the best solution for any objective
            for name, value in current_objectives.items():
                if value < best_objectives[name]:
                    best_objectives[name] = value
                    best_state = self._save_state()
        
        return best_objectives, best_state
    
    def _simulate_step(self):
        """Simulate a single step."""
        percept = self.world.get_sensory_percept()
        self.brain.process_sensory_input(percept)
        
        # Use curiosity to explore different areas
        curiosity = self.brain.get_curiosity_level()
        if curiosity > 0.7:
            # High curiosity: explore more
            action = self._explore_action()
        else:
            # Normal operation
            action = self.brain.decode_motor_command()
        
        self.world.apply_motor_command(action, self.world.get_simulation_time())
        
        # Apply reward based on exploration
        reward = self._get_exploration_reward()
        self.brain.apply_reward_modulation(reward, 0.0)
        
        self.brain.update_development(0.1)
    
    def _evaluate_objectives(self):
        """Evaluate multiple objectives."""
        body = self.world.get_agent_body()
        brain = self.brain.get_brain()
        
        objectives = {}
        
        # Objective 1: Proximity to center (lower is better)
        center_x, center_y = 6.0, 6.0
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        objectives['proximity_to_center'] = distance
        
        # Objective 2: Stability (lower variance in position)
        # Would need to track position history
        objectives['stability'] = 0.0  # Placeholder
        
        # Objective 3: Learning efficiency (higher firing rate indicates learning)
        objectives['learning_efficiency'] = brain.get_average_firing_rate()
        
        # Objective 4: Novelty seeking (curiosity balances exploration/exploitation)
        objectives['novelty_seeking'] = self.brain.get_novelty_level()
        
        return objectives
    
    def _get_exploration_reward(self):
        """Get reward that encourages exploration."""
        # Base reward: closer to center
        center_x, center_y = 6.0, 6.0
        distance = ((body.x - center_x) ** 2 + (body.y - center_y) ** 2) ** 0.5
        base_reward = max(0.0, 10.0 - distance)
        
        # Exploration bonus: higher reward for novel states
        exploration_bonus = self.brain.get_novelty_level() * 2.0
        
        return base_reward + exploration_bonus

# Use multi-objective optimization
brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_world()

objectives = {
    'efficiency': lambda x: x['proximity_to_center'],
    'learning': lambda x: -x['learning_efficiency'],  # Lower is better (negative for maximization)
    'exploration': lambda x: -x['novelty_seeking']     # Lower is better (negative for maximization)
}

multi_agent = MultiObjectiveAgent(brain, world, objectives)
best_objectives, best_state = multi_agent.optimize_multiple_objectives(50)

print("Multi-objective optimization results:")
for name, value in best_objectives.items():
    print(f"  {name}: {value:.4f}")
```

### 2. Transfer Learning

Use NLM for transfer learning between tasks:

```python
import pynlm

class TransferLearningAgent:
    def __init__(self, source_task_config, target_task_config):
        # Train on source task
        self.source_task_brain = self._train_source_task(source_task_config)
        
        # Create target task brain with weights transferred
        self.target_task_brain = self._create_target_task(target_task_config)
        self._transfer_weights()
        
        # Create agent for target task
        self.agent = pynlm.create_agent_brain(self.target_task_brain)
        self.target_task_brain.initialize()
    
    def _train_source_task(self, config):
        """Train on source task."""
        brain = pynlm.create_brain(config)
        brain.initialize()
        
        # Simulate source task
        for step in range(100):
            # Simulate source task dynamics
            brain.step(step)
            
            # Apply source-specific reward
            reward = self._source_task_reward(step)
            brain.apply_reward_modulation(reward, 0.0)
        
        return brain
    
    def _create_target_task(self, config):
        """Create brain for target task with different configuration."""
        return pynlm.create_brain(config)
    
    def _transfer_weights(self):
        """Transfer weights from source to target brain."""
        source_brain = self.source_task_brain
        target_brain = self.target_task_brain
        
        // Transfer region structure
        auto source_regions = source_brain.get_region_ids();
        auto target_regions = target_brain.get_region_ids();
        
        if (!source_regions.empty() && !target_regions.empty()) {
            auto* source_region = source_brain.get_region(source_regions[0]);
            auto* target_region = target_brain.get_region(target_regions[0]);
            
            // Transfer synaptic weights if regions have same structure
            auto source_neurons = source_region->get_neurons();
            auto target_neurons = target_region->get_neurons();
            
            if (!source_neurons.empty() && !target_neurons.empty()) {
                size_t min_size = std::min(source_neurons.size(), target_neurons.size());
                for (size_t i = 0; i < min_size; ++i) {
                    // Transfer weights (simplified - would need actual weight access)
                    // target_neurons[i]->transfer_weights(source_neurons[i]);
                }
            }
        }
    
    def _source_task_reward(self, step):
        """Reward for source task (example: reaching a target)."""
        // Implementation depends on source task
        return 1.0 / (1.0 + step / 100.0)
    
    def run_target_task(self, steps=200):
        """Run target task using transferred knowledge."""
        for step in range(steps):
            // Target task simulation logic
            self.agent.decode_motor_command()
        
        return self._evaluate_target_task()
    
    def _evaluate_target_task(self):
        """Evaluate performance on target task."""
        // Implementation depends on target task
        return {
            'performance': 0.85,  // Example: 85% of source task performance
            'sample_efficiency': 0.9,
            'stability': 0.8
        }

# Use transfer learning
source_config = pynlm.create_default_config()
source_config.set("neuron_count", 500)

target_config = pynlm.create_default_config()
target_config.set("neuron_count", 300)

transfer_agent = TransferLearningAgent(source_config, target_config)
target_results = transfer_agent.run_target_task()

print("Transfer learning results:")
for metric, value in target_results.items():
    print(f"  {metric}: {value:.3f}")
```

## Conclusion

This advanced usage guide provides:

1. **Performance optimization techniques** for running multiple simulations efficiently
2. **Advanced configuration management** with validation and schemas
3. **Sophisticated agent development** with custom behaviors and memory integration
4. **Visualization and debugging tools** for understanding simulation behavior
5. **Error handling and recovery** for robust simulations
6. **Advanced examples** for transfer learning and multi-objective optimization

The improved NLM Python API now provides powerful tools for:
- Research and experimentation
- Complex simulation scenarios
- Performance-critical applications
- Production deployment
- Educational and demonstration purposes

These enhancements make NLM more accessible to users of all skill levels while maintaining the flexibility and power of the underlying C++ implementation.