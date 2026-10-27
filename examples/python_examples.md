# NLM Python Examples - Practical Guide

This file contains practical examples for using the NLM Python interface in various scenarios.

## Example 1: Simple Brain Simulation

```python
"""
Simple example showing basic brain simulation without world interaction.
This demonstrates the minimal setup needed to run a neural simulation.
"""
import pynlm

def simple_brain_simulation():
    # Create a brain with default configuration
    print("Creating brain with default configuration...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    
    # Initialize the brain
    print("Initializing brain...")
    brain.initialize()
    
    # Print initial state
    print(f"Initial state: {brain.getTotalNeuronCount()} neurons, "
          f"{brain.getTotalSynapseCount()} synapses")
    
    # Run simulation
    print("Running simulation for 100 steps...")
    for step in range(100):
        brain.step(step)
        
        # Print progress every 20 steps
        if step % 20 == 0:
            spikes = brain.getTotalSpikeCount()
            firing = brain.getFiringNeuronCount()
            print(f"  Step {step}: {spikes} total spikes, {firing} firing neurons")
    
    # Final statistics
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    avg_rate = brain.getAverageFiringRate()
    
    print(f"\nSimulation complete!")
    print(f"  Total spikes: {final_spikes}")
    print(f"  Final firing neurons: {final_firing}")
    print(f"  Average firing rate: {avg_rate:.2f} Hz")
    
    return brain

if __name__ == "__main__":
    simple_brain_simulation()
```

## Example 2: Agent in World Environment

```python
"""
Example showing an agent interacting with a simulated world.
This demonstrates the complete agent-world interaction loop.
"""
import pynlm
import random

def agent_world_simulation():
    # Create brain and world
    print("Setting up brain and world...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Set agent starting position in center
    agent_body = world.getAgentBody()
    world.setAgentStart(agent_body.x, agent_body.y)
    
    # Create and initialize agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    print(f"Agent initialized with sensory size: {agent.getSensoryInputSize()}")
    print(f"Expected motor output size: {agent.getMotorOutputSize()}")
    
    # Run simulation
    print("\nRunning agent-world simulation...")
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input through agent
        agent.processSensoryInput(percept)
        
        # Brain processes information
        brain.step(step)
        
        # Get action from brain
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress every 20 steps
        if step % 20 == 0:
            stats = agent.get_stats()
            body = world.getAgentBody()
            print(f"  Step {step}: Position=({body.x:.1f},{body.y:.1f}), "
                  f"Action={action}, Neuromod={stats[0]:.3f}, "
                  f"Curiosity={stats[1]:.3f}")
    
    print(f"\nSimulation complete!")
    print(f"  Final position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    print(f"  Total steps: {step + 1}")
    
    return brain, world, agent

if __name__ == "__main__":
    agent_world_simulation()
```

## Example 3: Learning Agent with Rewards

```python
"""
Example showing learning behavior with reward signals.
This demonstrates how agents learn from rewards and prediction errors.
"""
import pynlm
import random

class RewardWorld:
    """Custom world that provides rewards to the agent."""
    
    def __init__(self, width=40, height=40):
        self.width = width
        self.height = height
        self.agent_x = width // 2
        self.agent_y = height // 2
        self.resources = []  # List of (x, y, value) tuples
        
        # Place some resources randomly
        for _ in range(5):
            x = random.randint(5, width - 5)
            y = random.randint(5, height - 5)
            value = random.uniform(0.5, 1.5)
            self.resources.append((x, y, value))
    
    def update(self, dt, action):
        """Update world state based on action."""
        # Simple movement
        if action == pynlm.MotorCommand.MoveForward:
            self.agent_x = min(self.width - 1, self.agent_x + 1)
        elif action == pynlm.MotorCommand.MoveBackward:
            self.agent_x = max(1, self.agent_x - 1)
        elif action == pynlm.MotorCommand.TurnLeft:
            self.agent_y = max(1, self.agent_y - 1)
        elif action == pynlm.MotorCommand.TurnRight:
            self.agent_y = min(self.height - 1, self.agent_y + 1)
        
        # Wrap around edges
        self.agent_x = self.agent_x % self.width
        self.agent_y = self.agent_y % self.height
    
    def get_sensory_percept(self):
        """Get sensory input for the agent."""
        # Create a simple percept
        percept = pynlm.SensoryPercept()
        
        # Vision: create simple gradient based on position
        vision_data = []
        for y in range(8):
            for x in range(8):
                # Simple distance-based vision
                wx = int((self.agent_x + x - 4) % self.width)
                wy = int((self.agent_y + y - 4) % self.height)
                
                # Check for resources
                resource_value = 0.0
                for (rx, ry, val) in self.resources:
                    if abs(rx - wx) < 3 and abs(ry - wy) < 3:
                        resource_value = val
                        break
                
                vision_data.append(resource_value)
        
        # Create vision object
        vision = pynlm.Vision(8, 8, 1)
        vision.setData(vision_data)
        percept.setVision(vision)
        
        return percept
    
    def get_reward(self, action_taken):
        """Calculate reward for the action taken."""
        reward = 0.0
        
        # Check if agent is on a resource
        for (rx, ry, val) in self.resources:
            if abs(rx - self.agent_x) < 2 and abs(ry - self.agent_y) < 2:
                # Collect resource
                reward += val
                # Remove resource
                self.resources = [(x, y, v) for (x, y, v) in self.resources 
                                if not (abs(x - self.agent_x) < 2 and abs(y - self.agent_y) < 2)]
        
        # Penalty for staying in place too long (encourage movement)
        if action_taken == pynlm.MotorCommand.Wait:
            reward -= 0.1
        
        return reward

def learning_simulation():
    """Run a simulation where agent learns to collect resources."""
    
    print("Setting up learning simulation...")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    # Configure for learning
    config.set("random_seed", 42)
    config.set("neuron_count", 200)
    config.set("enable_development", True)
    config.set("plasticity_rate", 1.2)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = RewardWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enable_all_features()
    
    print(f"World size: {world.width}x{world.height}")
    print(f"Initial resources: {len(world.resources)}")
    print(f"Agent enabled: Reward={agent.isRewardModulationEnabled()}, "
          f"Curiosity={agent.isCuriosityEnabled()}, "
          f"Development={agent.isDevelopmentEnabled()}")
    
    # Run episodes
    num_episodes = 10
    max_steps_per_episode = 100
    
    for episode in range(num_episodes):
        print(f"\nEpisode {episode + 1}/{num_episodes}")
        
        agent.reset()
        world = RewardWorld()  # New world for each episode
        agent.initialize(world)
        
        episode_reward = 0.0
        predicted_reward = 0.0
        
        for step in range(max_steps_per_episode):
            # Get sensory input
            percept = world.get_sensory_percept()
            agent.processSensoryInput(percept)
            
            # Brain processes
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.update(0.1, action)
            
            # Get reward and apply learning
            reward = world.get_reward(action)
            predicted_reward += 0.05  # Simple prediction
            
            agent.applyRewardModulation(reward, predicted_reward)
            
            episode_reward += reward
            predicted_reward = 0.0  # Reset prediction
            
            # Show progress
            if step % 20 == 0:
                stats = agent.get_stats()
                print(f"  Step {step}: Reward={reward:+.3f}, "
                      f"Total={episode_reward:+.3f}, "
                      f"Stage={stats[4]}, Neuromod={stats[0]:.3f}")
            
            # End episode if agent collected all resources
            if len(world.resources) == 0:
                print(f"  Collected all resources at step {step}")
                break
        
        # Final stats for episode
        final_stats = agent.get_stats()
        print(f"  Episode complete: {len(world.resources)} resources left, "
              f"Final stage={final_stats[4]}, "
              f"Avg neuromod={final_stats[0]:.3f}")
    
    print(f"\nLearning simulation complete!")
    
    return brain, agent

if __name__ == "__main__":
    learning_simulation()
```

## Example 4: Performance and Benchmarking

```python
"""
Example showing performance monitoring and benchmarking.
This demonstrates how to measure and optimize simulation performance.
"""
import pynlm
import time
import statistics

def benchmark_simulation(config, duration_seconds=60):
    """
    Run a benchmark simulation for specified duration.
    
    Args:
        config: Brain configuration
        duration_seconds: How long to run the simulation
        
    Returns:
        Dictionary with performance statistics
    """
    print(f"Starting benchmark simulation for {duration_seconds} seconds...")
    
    # Create brain and world
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features
    agent.enable_all_features()
    
    # Performance monitoring
    start_time = time.time()
    steps_completed = 0
    
    # Metrics collection
    spike_counts = []
    firing_counts = []
    action_counts = {}
    neuromod_levels = []
    curiosity_levels = []
    
    print_interval = 10  # Print stats every 10 seconds
    
    try:
        while time.time() - start_time < duration_seconds:
            # Record pre-simulation state
            pre_spikes = brain.getTotalSpikeCount()
            pre_firing = brain.getFiringNeuronCount()
            
            # Run one simulation step
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(steps_completed)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            steps_completed += 1
            
            # Record post-simulation state
            post_spikes = brain.getTotalSpikeCount()
            post_firing = brain.getFiringNeuronCount()
            
            # Collect metrics
            spike_counts.append(post_spikes - pre_spikes)
            firing_counts.append(post_firing)
            
            action_name = action.get_name() if hasattr(action, 'get_name') else str(action)
            action_counts[action_name] = action_counts.get(action_name, 0) + 1
            
            stats = agent.get_stats()
            neuromod_levels.append(stats[0])
            curiosity_levels.append(stats[1])
            
            # Print progress
            elapsed = time.time() - start_time
            if elapsed >= print_interval:
                avg_spikes = statistics.mean(spike_counts) if spike_counts else 0
                avg_firing = statistics.mean(firing_counts) if firing_counts else 0
                steps_per_sec = steps_completed / elapsed if elapsed > 0 else 0
                
                print(f"  {elapsed:.1f}s: {steps_per_sec:.1f} steps/sec, "
                      f"{avg_spikes:.1f} avg spikes/step, "
                      f"{avg_firing:.0f} avg firing neurons")
                
                print_interval += 10
        
    except KeyboardInterrupt:
        print("\nBenchmark interrupted by user")
    
    # Calculate final statistics
    elapsed_time = time.time() - start_time
    
    stats = {
        'total_steps': steps_completed,
        'elapsed_time': elapsed_time,
        'steps_per_second': steps_completed / elapsed_time if elapsed_time > 0 else 0,
        'total_spikes': brain.getTotalSpikeCount(),
        'final_firing_neurons': brain.getFiringNeuronCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'action_distribution': action_counts,
        'spike_statistics': {
            'mean': statistics.mean(spike_counts) if spike_counts else 0,
            'median': statistics.median(spike_counts) if spike_counts else 0,
            'stdev': statistics.stdev(spike_counts) if len(spike_counts) > 1 else 0,
            'max': max(spike_counts) if spike_counts else 0
        },
        'neuromodulation_stats': {
            'mean': statistics.mean(nu levels) if neuromod_levels else 0,
            'mean': statistics.mean(neuromod_levels) if neuromod_levels else 0,
            'stdev': statistics.stdev(neuromod_levels) if len(neuromod_levels) > 1 else 0,
            'mean_curiosity': statistics.mean(curiosity_levels) if curiosity_levels else 0
        }
    }
    
    return stats

def compare_configurations():
    """Compare performance across different configurations."""
    
    configurations = [
        {'name': 'Minimal', 'neuron_count': 50, 'plasticity_rate': 0.5},
        {'name': 'Standard', 'neuron_count': 200, 'plasticity_rate': 1.0},
        {'name': 'High Plasticity', 'neuron_count': 200, 'plasticity_rate': 1.5},
        {'name': 'Complex', 'neuron_count': 500, 'plasticity_rate': 1.0}
    ]
    
    results = {}
    
    for config_spec in configurations:
        print(f"\n{'='*60}")
        print(f"Benchmarking: {config_spec['name']} Configuration")
        print(f"  Neurons: {config_spec['neuron_count']}")
        print(f"  Plasticity Rate: {config_spec['plasticity_rate']}")
        print(f"{'='*60}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", config_spec['neuron_count'])
        config.set("plasticity_rate", config_spec['plasticity_rate'])
        config.set("random_seed", 42)  # For reproducible results
        
        # Run benchmark
        stats = benchmark_simulation(config, duration_seconds=30)
        results[config_spec['name']] = stats
        
        # Print summary
        print(f"\nResults Summary:")
        print(f"  Steps/sec: {stats['steps_per_second']:.1f}")
        print(f"  Total spikes: {stats['total_spikes']}")
        print(f"  Avg firing rate: {stats['average_firing_rate']:.2f} Hz")
        print(f"  Action distribution: {stats['action_distribution']}")
    
    return results

if __name__ == "__main__":
    # Run single benchmark
    config = pynlm.createDefaultConfig()
    benchmark_simulation(config, duration_seconds=60)
    
    # Or compare configurations
    # results = compare_configurations()
```

## Example 5: Multi-Agent Cooperation

```python
"""
Example showing multiple agents interacting in the same world.
This demonstrates agent coordination and competition.
"""
import pynlm
import random

class MultiAgentWorld:
    """World that supports multiple agents."""
    
    def __init__(self, width=60, height=60):
        self.width = width
        self.height = height
        self.agents = []  # List of (x, y, id) tuples
        self.resources = []
        
        # Initialize agents at random positions
        for i in range(3):
            x = random.randint(10, width - 10)
            y = random.randint(10, height - 10)
            self.agents.append((x, y, i))
        
        # Place resources
        for _ in range(8):
            x = random.randint(5, width - 5)
            y = random.randint(5, height - 5)
            value = random.uniform(0.3, 1.0)
            self.resources.append((x, y, value, random.randint(0, 2)))
    
    def update_all(self, dt, actions):
        """Update world for all agents with given actions."""
        for i, ((x, y, agent_id), action) in enumerate(zip(self.agents, actions)):
            new_x, new_y = x, y
            
            # Apply action
            if action == pynlm.MotorCommand.MoveForward:
                new_x = min(self.width - 1, new_x + 1)
            elif action == pynlm.MotorCommand.MoveBackward:
                new_x = max(0, new_x - 1)
            elif action == pynlm.MotorCommand.TurnLeft:
                new_y = max(0, new_y - 1)
            elif action == pynlm.MotorCommand.TurnRight:
                new_y = min(self.height - 1, new_y + 1)
            
            # Check for collisions with other agents
            collision = False
            for j, (ax, ay, _) in enumerate(self.agents):
                if i != j and abs(ax - new_x) < 2 and abs(ay - new_y) < 2:
                    collision = True
                    break
            
            if not collision:
                self.agents[i] = (new_x, new_y, agent_id)
            
            # Check for resource collection
            for j, (rx, ry, value, collected) in enumerate(self.resources):
                if not collected and abs(rx - new_x) < 2 and abs(ry - new_y) < 2:
                    # Collect resource
                    self.resources[j] = (rx, ry, value, 1)
    
    def get_percept_for_agent(self, agent_index):
        """Get sensory percept for specific agent."""
        agent_x, agent_y, _ = self.agents[agent_index]
        
        percept = pynlm.SensoryPercept()
        vision_data = []
        
        for y in range(8):
            for x in range(8):
                wx = int((agent_x + x - 4) % self.width)
                wy = int((agent_y + y - 4) % self.height)
                
                # Check for resources visible to this agent
                resource_value = 0.0
                for (rx, ry, val, collected) in self.resources:
                    if not collected and abs(rx - wx) < 4 and abs(ry - wy) < 4:
                        resource_value += val * 0.5  # Distance decay
                
                # Check for other agents
                agent_proximity = 0.0
                for j, (ax, ay, aid) in enumerate(self.agents):
                    if j != agent_index and abs(ax - wx) < 3 and abs(ay - wy) < 3:
                        agent_proximity += 0.5
                
                vision_data.append(resource_value + agent_proximity)
        
        vision = pynlm.Vision(8, 8, 1)
        vision.setData(vision_data)
        percept.setVision(vision)
        
        return percept
    
    def get_global_reward(self, action_taken, agent_index):
        """Calculate global reward for action."""
        reward = 0.0
        agent_x, agent_y, _ = self.agents[agent_index]
        
        # Reward for collecting resources
        for (rx, ry, val, collected) in self.resources:
            if not collected and abs(rx - agent_x) < 2 and abs(ry - agent_y) < 2:
                reward += val
        
        # Small penalty for wasteful actions
        if action_taken == pynlm.MotorCommand.Wait:
            reward -= 0.05
        
        return reward

def multi_agent_simulation():
    """Run simulation with multiple cooperating agents."""
    
    print("Setting up multi-agent simulation...")
    
    # Create shared brain
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 300)
    config.set("random_seed", 123)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agents
    world = MultiAgentWorld()
    agents = []
    
    for i in range(3):
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agents.append(agent)
    
    # Enable features
    for agent in agents:
        agent.enable_all_features()
    
    print(f"World size: {world.width}x{world.height}")
    print(f"Agents: {len(agents)}")
    print(f"Initial resources: {len([r for r in world.resources if r[3] == 0])}")
    
    # Run simulation
    print("\nRunning multi-agent simulation...")
    
    for step in range(200):
        # Get actions from all agents
        actions = []
        for i, agent in enumerate(agents):
            percept = world.get_percept_for_agent(i)
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            actions.append(action)
        
        # Update world
        world.update_all(0.1, actions)
        
        # Apply rewards and learning
        for i, agent in enumerate(agents):
            reward = world.get_global_reward(actions[i], i)
            predicted_reward = 0.02  # Simple prediction
            agent.applyRewardModulation(reward, predicted_reward)
        
        # Show progress
        if step % 20 == 0:
            print(f"\nStep {step}:")
            print(f"  Resources remaining: {len([r for r in world.resources if r[3] == 0])}")
            
            # Show agent positions
            for i, (x, y, _) in enumerate(world.agents):
                collected = sum(1 for r in world.resources if r[3] == 1 and 
                              abs(r[0] - x) < 2 and abs(r[1] - y) < 2)
                print(f"  Agent {i}: pos=({x},{y}), collected={collected}")
            
            # Show average stats
            total_neuromod = 0
            total_curiosity = 0
            for agent in agents:
                stats = agent.get_stats()
                total_neuromod += stats[0]
                total_curiosity += stats[1]
            
            avg_neuromod = total_neuromod / len(agents)
            avg_curiosity = total_curiosity / len(agents)
            print(f"  Average neuromodulation: {avg_neuromod:.3f}")
            print(f"  Average curiosity: {avg_curiosity:.3f}")
    
    # Final statistics
    print(f"\nMulti-agent simulation complete!")
    print(f"Final resources collected: {sum(1 for r in world.resources if r[3] == 1)}")
    
    return brain, agents, world

if __name__ == "__main__":
    multi_agent_simulation()
```

## Example 6: Loading and Saving Brain States

```python
"""
Example showing checkpoint functionality for saving and loading brain states.
This is essential for long-running simulations and resuming from specific points.
"""
import pynlm
import os
import shutil

def save_and_load_brain():
    """Demonstrate saving and loading brain states."""
    
    print("Creating initial brain...")
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Set up some state by running a simulation
    for i in range(50):
        brain.step(i)
    
    initial_spikes = brain.getTotalSpikeCount()
    initial_firing = brain.getFiringNeuronCount()
    
    print(f"Initial state: {initial_spikes} spikes, {initial_firing} firing neurons")
    
    # Create directory for checkpoints
    checkpoint_dir = "brain_checkpoints"
    os.makedirs(checkpoint_dir, exist_ok=True)
    
    # Save multiple checkpoints
    print(f"\nSaving checkpoints to {checkpoint_dir}...")
    
    for checkpoint_num in range(3):
        # Modify brain slightly to create different states
        for i in range(10):
            brain.step(checkpoint_num * 10 + i)
        
        # Save checkpoint
        checkpoint_file = os.path.join(checkpoint_dir, f"checkpoint_{checkpoint_num:03d}.nlm")
        saved = brain.save(checkpoint_file)
        
        if saved:
            # Get some info about saved state
            stats_after = brain.getTotalSpikeCount()
            print(f"  Checkpoint {checkpoint_num}: Saved at {stats_after} spikes")
        else:
            print(f"  Checkpoint {checkpoint_num}: Failed to save")
    
    # Load and compare checkpoints
    print(f"\nLoading checkpoints...")
    
    checkpoints = []
    for checkpoint_num in range(3):
        checkpoint_file = os.path.join(checkpoint_dir, f"checkpoint_{checkpoint_num:03d}.nlm")
        if os.path.exists(checkpoint_file):
            loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
            loaded_brain.initialize()
            
            loaded = loaded_brain.load(checkpoint_file)
            if loaded:
                loaded_spikes = loaded_brain.getTotalSpikeCount()
                checkpoints.append((checkpoint_num, loaded_spikes))
                print(f"  Loaded checkpoint {checkpoint_num}: {loaded_spikes} spikes")
    
    # Compare spike counts
    print(f"\nCheckpoint comparison:")
    for checkpoint_num, spikes in checkpoints:
        difference = spikes - initial_spikes
        print(f"  Checkpoint {checkpoint_num}: Difference = {difference:+d} spikes")
    
    # Clean up
    shutil.rmtree(checkpoint_dir)
    print(f"\nCleaned up checkpoint directory")
    
    return brain

if __name__ == "__main__":
    save_and_load_brain()
```

## Example 7: Debugging and Monitoring

```python
"""
Example showing debugging and monitoring techniques.
This helps identify and resolve simulation issues.
"""
import pynlm

def debug_simulation():
    """Run a simulation with debugging enabled."""
    
    print("Setting up debug simulation...")
    
    # Create brain with logging enabled
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Enable verbose logging
    config = brain.getConfig()
    config.set("log_level", "debug")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Monitor for potential issues
    print_interval = 10
    spike_threshold = 100
    firing_threshold = 50
    
    spike_warning_triggered = False
    firing_warning_triggered = False
    
    print(f"\nRunning debug simulation...")
    print(f"Monitoring for spikes > {spike_threshold} and firing neurons > {firing_threshold}")
    
    for step in range(100):
        # Update world
        world.update(0.1)
        
        # Get and process sensory input
        percept = world.getSensoryPercept()
        
        # Check for problematic sensory patterns
        if hasattr(percept, 'getVision'):
            vision = percept.getVision()
            if hasattr(vision, 'getData'):
                data = vision.getData()
                if data:
                    max_input = max(data)
                    if max_input > 0.9:
                        print(f"  Step {step}: High sensory input detected ({max_input:.3f})")
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        
        # Check for potential issues
        total_spikes = brain.getTotalSpikeCount()
        firing_neurons = brain.getFiringNeuronCount()
        
        # Check spike threshold
        if total_spikes > spike_threshold and not spike_warning_triggered:
            print(f"  WARNING: Spike threshold exceeded ({total_spikes} > {spike_threshold})")
            spike_warning_triggered = True
        
        # Check firing threshold  
        if firing_neurons > firing_threshold and not firing_warning_triggered:
            print(f"  WARNING: High firing neuron count ({firing_neurons} > {firing_threshold})")
            firing_warning_triggered = True
        
        # Check agent state
        stats = agent.get_stats()
        neuromod = stats[0]
        curiosity = stats[1]
        
        # Check for extreme neuromodulation
        if abs(neuromod) > 0.8:
            print(f"  Step {step}: Extreme neuromodulation detected ({neuromod:.3f})")
        
        # Check for stuck agent (no movement for many steps)
        # This would require tracking position history
        
        # Print progress
        if step % print_interval == 0:
            print(f"  Step {step}: spikes={total_spikes}, firing={firing_neurons}, "
                  f"action={action}, neuromod={neuromod:.3f}")
    
    # Final diagnostic report
    print(f"\n=== DEBUG REPORT ===")
    print(f"Final spike count: {brain.getTotalSpikeCount()}")
    print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Final average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Warnings triggered: Spike={spike_warning_triggered}, Firing={firing_warning_triggered}")
    
    # Check for unstable behavior
    if spike_warning_triggered or firing_warning_triggered:
        print("\n=== RECOMMENDATIONS ===")
        print("1. Reduce neural excitability or increase inhibition")
        print("2. Check sensory input scaling")
        print("3. Verify configuration parameters")
        print("4. Consider reducing simulation timestep")
    
    return brain, agent

if __name__ == "__main__":
    debug_simulation()
```

## Example 8: Performance Optimization

```python
"""
Example showing performance optimization techniques.
This demonstrates how to optimize simulations for better performance.
"""
import pynlm
import time

def optimized_simulation(config_spec):
    """
    Run an optimized simulation based on configuration.
    
    Args:
        config_spec: Dictionary with optimization parameters
    """
    
    print(f"Setting up optimized simulation: {config_spec['name']}")
    
    # Create configuration with optimizations
    config = pynlm.createDefaultConfig()
    
    # Apply optimizations based on specification
    if config_spec['optimization'] == 'speed':
        # Optimize for speed
        config.set("neuron_count", config_spec.get('neuron_count', 100))
        config.set("plasticity_rate", 0.5)  # Lower plasticity = less computation
        config.set("enable_structural_plasticity", False)  # Disable expensive features
        print("  Optimization: Speed-focused (reduced neurons, plasticity)")
    
    elif config_spec['optimization'] == 'stability':
        # Optimize for stability
        config.set("neuron_count", config_spec.get('neuron_count', 200))
        config.set("plasticity_rate", 1.5)  # Higher plasticity for learning
        config.set("enable_development", True)
        print("  Optimization: Stability-focused (more neurons, development)")
    
    elif config_spec['optimization'] == 'balanced':
        # Balanced optimization
        config.set("neuron_count", config_spec.get('neuron_count', 150))
        config.set("plasticity_rate", 1.0)
        config.set("enable_structural_plasticity", True)
        print("  Optimization: Balanced (moderate neurons and features)")
    
    # Set common parameters
    config.set("random_seed", 42)  # For reproducible results
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    
    # Optimize world size for performance
    world_width = config_spec.get('world_width', 30)
    world_height = config_spec.get('world_height', 30)
    vision_width = config_spec.get('vision_width', 8)
    vision_height = config_spec.get('vision_height', 8)
    
    world.configure(world_width, world_height, vision_width, vision_height)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Apply optimizations to agent
    if config_spec['optimization'] == 'speed':
        agent.enableRewardModulation(False)  # Disable reward learning
        agent.enableCuriosity(False)  # Disable exploration
        print("  Agent optimization: Disabled learning features")
    
    # Run simulation with performance monitoring
    print(f"\nRunning optimized simulation...")
    
    start_time = time.time()
    steps_completed = 0
    
    try:
        for step in range(config_spec.get('max_steps', 100)):
            # Optimized simulation loop
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            steps_completed += 1
            
            # Show progress at intervals
            if steps_completed % 20 == 0:
                elapsed = time.time() - start_time
                steps_per_sec = steps_completed / elapsed if elapsed > 0 else 0
                
                print(f"  Step {steps_completed}: {steps_per_sec:.1f} steps/sec, "
                      f"spikes={brain.getTotalSpikeCount()}")
    
    except KeyboardInterrupt:
        print("\nSimulation interrupted by user")
    
    elapsed_time = time.time() - start_time
    
    # Calculate performance metrics
    final_spikes = brain.getTotalSpikeCount()
    final_firing = brain.getFiringNeuronCount()
    final_rate = brain.getAverageFiringRate()
    
    if elapsed_time > 0:
        steps_per_sec = steps_completed / elapsed_time
    else:
        steps_per_sec = 0
    
    # Performance analysis
    print(f"\n=== PERFORMANCE RESULTS ===")
    print(f"Simulation completed: {steps_completed} steps")
    print(f"Time elapsed: {elapsed_time:.1f} seconds")
    print(f"Performance: {steps_per_sec:.1f} steps/sec")
    print(f"Final state: {final_spikes} spikes, {final_firing} firing neurons")
    print(f"Average firing rate: {final_rate:.2f} Hz")
    
    # Optimization suggestions
    if steps_per_sec < 10:
        print(f"\nPerformance Suggestion: Consider reducing simulation complexity")
    elif final_rate > 50:
        print(f"\nPerformance Suggestion: High firing rate - may need stability adjustments")
    
    return {
        'steps_completed': steps_completed,
        'elapsed_time': elapsed_time,
        'steps_per_second': steps_per_sec,
        'total_spikes': final_spikes,
        'final_firing_neurons': final_firing,
        'average_firing_rate': final_rate,
        'optimization_applied': config_spec['optimization']
    }

def run_optimization_comparison():
    """Compare different optimization strategies."""
    
    print("=== OPTIMIZATION COMPARISON ===")
    
    optimization_strategies = [
        {'name': 'Speed Optimized', 'optimization': 'speed', 'neuron_count': 50},
        {'name': 'Balanced', 'optimization': 'balanced', 'neuron_count': 100},
        {'name': 'Stability Optimized', 'optimization': 'stability', 'neuron_count': 200}
    ]
    
    results = []
    
    for strategy in optimization_strategies:
        print(f"\n{'='*60}")
        print(f"Testing: {strategy['name']}")
        print(f"{'='*60}")
        
        result = optimized_simulation(strategy)
        results.append(result)
    
    # Summary comparison
    print(f"\n{'='*60}")
    print("OPTIMIZATION SUMMARY")
    print(f"{'='*60}")
    print(f"{'Strategy':<20} {'Steps/Sec':<12} {'Total Spikes':<14} {'Final Rate':<12} {'Outcome':<15}")
    print(f"{'-'*60}")
    
    for result in results:
        strategy = result['optimization_applied']
        steps_per_sec = result['steps_per_second']
        total_spikes = result['total_spikes']
        final_rate = result['average_firing_rate']
        
        # Determine outcome
        if steps_per_sec > 20:
            outcome = "Good"
        elif steps_per_sec > 10:
            outcome = "Fair"
        else:
            outcome = "Needs Work"
        
        print(f"{strategy:<20} {steps_per_sec:<12.1f} {total_spikes:<14} {final_rate:<12.2f} {outcome:<15}")
    
    return results

if __name__ == "__main__":
    # Run single optimized simulation
    config = {'name': 'Test Simulation', 'optimization': 'balanced', 
              'neuron_count': 100, 'max_steps': 50}
    
    optimized_simulation(config)
    
    # Or run comparison
    # run_optimization_comparison()
```

## Example 9: Configuration Management

```python
"""
Example showing configuration management for different simulation scenarios.
This demonstrates how to manage and switch between different configurations.
"""
import pynlm
import json
import os

class ConfigurationManager:
    """Manage multiple configuration presets for different simulation scenarios."""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = config_dir
        self.presets = {}
        
        # Create configs directory if it doesn't exist
        os.makedirs(self.config_dir, exist_ok=True)
        
        # Load default presets
        self.load_default_presets()
    
    def load_default_presets(self):
        """Load default configuration presets."""
        
        # Preset 1: Learning-focused
        self.presets['learning'] = {
            'name': 'Learning Agent',
            'description': 'Optimized for learning and behavior change',
            'config': {
                'neuron_count': 300,
                'plasticity_rate': 1.5,
                'enable_development': True,
                'enable_structural_plasticity': True,
                'enable_reward_modulation': True,
                'enable_curiosity': True,
                'random_seed': 42
            }
        }
        
        # Preset 2: Performance-focused
        self.presets['performance'] = {
            'name': 'High Performance',
            'description': 'Optimized for simulation speed',
            'config': {
                'neuron_count': 100,
                'plasticity_rate': 0.5,
                'enable_development': False,
                'enable_structural_plasticity': False,
                'enable_reward_modulation': False,
                'enable_curiosity': False,
                'random_seed': 42
            }
        }
        
        # Preset 3: Stability-focused
        self.presets['stability'] = {
            'name': 'Stable System',
            'description': 'Optimized for stable, predictable behavior',
            'config': {
                'neuron_count': 200,
                'plasticity_rate': 1.0,
                'enable_development': True,
                'enable_structural_plasticity': True,
                'enable_reward_modulation': True,
                'enable_curiosity': False,  # Disable curiosity for stability
                'random_seed': 42
            }
        }
        
        # Preset 4: Exploration-focused
        self.presets['exploration'] = {
            'name': 'Exploration Agent',
            'description': 'Optimized for exploration and novel behaviors',
            'config': {
                'neuron_count': 250,
                'plasticity_rate': 1.2,
                'enable_development': True,
                'enable_structural_plasticity': True,
                'enable_reward_modulation': True,
                'enable_curiosity': True,
                'random_seed': 42
            }
        }
    
    def get_preset(self, name):
        """Get configuration preset by name."""
        if name in self.presets:
            return self.presets[name]
        else:
            print(f"Warning: Preset '{name}' not found. Using defaults.")
            return None
    
    def create_brain_from_preset(self, name):
        """Create a brain using a configuration preset."""
        preset = self.get_preset(name)
        if preset is None:
            return None
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        
        # Apply preset settings
        for key, value in preset['config'].items():
            config.set(key, value)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Created brain using preset: {preset['name']}")
        print(f"  Description: {preset['description']}")
        print(f"  Config: {preset['config']}")
        
        return brain
    
    def save_preset(self, name, config_dict, description=""):
        """Save a custom configuration preset."""
        preset = {
            'name': name,
            'description': description,
            'config': config_dict
        }
        
        self.presets[name] = preset
        
        # Save to file
        filepath = os.path.join(self.config_dir, f"{name}.json")
        with open(filepath, 'w') as f:
            json.dump(preset, f, indent=2)
        
        print(f"Saved preset '{name}' to {filepath}")
    
    def load_preset_from_file(self, filename):
        """Load a preset from a file."""
        filepath = os.path.join(self.config_dir, f"{filename}.json")
        
        if not os.path.exists(filepath):
            print(f"Preset file not found: {filepath}")
            return None
        
        with open(filepath, 'r') as f:
            preset = json.load(f)
        
        self.presets[preset['name']] = preset
        print(f"Loaded preset '{preset['name']}' from {filepath}")
        
        return preset

def demonstrate_configurations():
    """Demonstrate different configuration presets."""
    
    print("=== CONFIGURATION MANAGEMENT DEMONSTRATION ===\n")
    
    # Create configuration manager
    config_manager = ConfigurationManager()
    
    # Demonstrate using different presets
    presets_to_use = ['learning', 'performance', 'stability', 'exploration']
    
    for preset_name in presets_to_use:
        print(f"\n--- Using Preset: {preset_name.upper()} ---")
        
        brain = config_manager.create_brain_from_preset(preset_name)
        if brain is None:
            continue
        
        # Run a quick simulation
        for i in range(20):
            brain.step(i)
        
        # Print results
        spikes = brain.getTotalSpikeCount()
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        
        print(f"  Results: {spikes} spikes, {firing} firing neurons, "
              f"{rate:.2f} Hz average rate")
        print()
    
    # Demonstrate saving custom preset
    print("\n--- Saving Custom Preset ---")
    custom_config = {
        'neuron_count': 500,
        'plasticity_rate': 2.0,
        'enable_development': True,
        'enable_structural_plasticity': True,
        'enable_reward_modulation': True,
        'enable_curiosity': True,
        'random_seed': 123
    }
    
    config_manager.save_preset(
        'custom_complex',
        custom_config,
        'Custom complex configuration with high plasticity and all features enabled'
    )
    
    # Demonstrate loading preset
    print("\n--- Loading Preset from File ---")
    # This would work if we had saved a preset file
    # config_manager.load_preset_from_file('custom_complex')
    
    print("\nConfiguration management demonstration complete!")
    
    return config_manager

if __name__ == "__main__":
    demonstrate_configurations()
```

## Example 10: Advanced Error Handling

```python
"""
Example showing advanced error handling and robustness.
This demonstrates how to create robust simulations that handle errors gracefully.
"""
import pynlm
import traceback

class RobustSimulation:
    """Robust simulation wrapper with comprehensive error handling."""
    
    def __init__(self, max_consecutive_errors=5):
        self.max_consecutive_errors = max_consecutive_errors
        self.error_count = 0
        self.last_error = None
        
        # Error statistics
        self.error_types = {}
        self.error_steps = []
    
    def run_simulation_with_error_handling(self, config, world_spec, agent_spec, max_steps=100):
        """
        Run simulation with comprehensive error handling.
        
        Args:
            config: Brain configuration
            world_spec: World specification dictionary
            agent_spec: Agent specification dictionary
            max_steps: Maximum number of simulation steps
            
        Returns:
            Dictionary with simulation results and error information
        """
        
        print(f"Starting robust simulation with error handling...")
        print(f"Max consecutive errors: {self.max_consecutive_errors}")
        
        try:
            # Initialize brain with error handling
            brain = None
            try:
                print("Creating brain...")
                brain = pynlm.createBrain(config)
                brain.initialize()
                print("✓ Brain created successfully")
            except Exception as e:
                self.record_error("brain_creation", e)
                print(f"✗ Brain creation failed: {e}")
                return self.get_error_results("brain_creation_failed")
            
            # Initialize world with error handling
            world = None
            try:
                print("Creating world...")
                world = pynlm.createSimpleWorld()
                
                # Apply world configuration
                for key, value in world_spec.items():
                    if hasattr(world, f'set_{key}'):
                        getattr(world, f'set_{key}')(value)
                    elif key == 'configure':
                        world.configure(*value)
                
                world.reset()
                print("✓ World created successfully")
            except Exception as e:
                self.record_error("world_creation", e)
                print(f"✗ World creation failed: {e}")
                return self.get_error_results("world_creation_failed")
            
            # Initialize agent with error handling
            agent = None
            try:
                print("Creating agent...")
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                print("✓ Agent created successfully")
            except Exception as e:
                self.record_error("agent_creation", e)
                print(f"✗ Agent creation failed: {e}")
                return self.get_error_results("agent_creation_failed")
            
            # Enable features with error handling
            if agent_spec.get('enable_reward_modulation', False):
                try:
                    agent.enableRewardModulation(True)
                    print("✓ Reward modulation enabled")
                except Exception as e:
                    self.record_error("reward_modulation", e)
                    print(f"  Warning: Reward modulation failed: {e}")
            
            if agent_spec.get('enable_curiosity', False):
                try:
                    agent.enableCuriosity(True)
                    print("✓ Curiosity enabled")
                except Exception as e:
                    self.record_error("curiosity", e)
                    print(f"  Warning: Curiosity failed: {e}")
            
            # Main simulation loop with error handling
            print(f"\nStarting simulation loop ({max_steps} steps)...")
            
            simulation_results = {
                'steps_completed': 0,
                'total_spikes': 0,
                'final_firing': 0,
                'average_firing_rate': 0.0,
                'errors_encountered': [],
                'warnings_encountered': [],
                'action_distribution': {},
                'step_data': []
            }
            
            consecutive_errors = 0
            
            for step in range(max_steps):
                step_result = {'step': step, 'errors': [], 'warnings': []}
                
                try:
                    # Update world
                    try:
                        world.update(0.1)
                    except Exception as e:
                        error_msg = f"World update failed: {e}"
                        step_result['errors'].append(error_msg)
                        self.record_error("world_update", e)
                        simulation_results['errors_encountered'].append(error_msg)
                        
                        if consecutive_errors >= self.max_consecutive_errors:
                            raise Exception(f"Too many consecutive errors ({consecutive_errors})")
                    
                    # Get and process sensory input
                    try:
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                    except Exception as e:
                        error_msg = f"Sensory processing failed: {e}"
                        step_result['errors'].append(error_msg)
                        self.record_error("sensory_processing", e)
                        simulation_results['errors_encountered'].append(error_msg)
                        
                        if consecutive_errors >= self.max_consecutive_errors:
                            raise Exception(f"Too many consecutive errors ({consecutive_errors})")
                    
                    # Brain processing
                    try:
                        brain.step(step)
                    except Exception as e:
                        error_msg = f"Brain step failed: {e}"
                        step_result['errors'].append(error_msg)
                        self.record_error("brain_step", e)
                        simulation_results['errors_encountered'].append(error_msg)
                        
                        if consecutive_errors >= self.max_consecutive_errors:
                            raise Exception(f"Too many consecutive errors ({consecutive_errors})")
                    
                    # Get action
                    try:
                        action = agent.decodeMotorCommand()
                        step_result['action'] = action
                    except Exception as e:
                        error_msg = f"Action decoding failed: {e}"
                        step_result['errors'].append(error_msg)
                        self.record_error("action_decoding", e)
                        simulation_results['errors_encountered'].append(error_msg)
                        
                        if consecutive_errors >= self.max_consecutive_errors:
                            raise Exception(f"Too many consecutive errors ({consecutive_errors})")
                    
                    # Apply action
                    try:
                        world.applyMotorCommand(action, world.getSimulationTime())
                    except Exception as e:
                        error_msg = f"Action application failed: {e}"
                        step_result['errors'].append(error_msg)
                        self.record_error("action_application", e)
                        simulation_results['errors_encountered'].append(error_msg)
                        
                        if consecutive_errors >= self.max_consecutive_errors:
                            raise Exception(f"Too many consecutive errors ({consecutive_errors})")
                    
                    # Collect statistics
                    try:
                        step_result['spikes'] = brain.getTotalSpikeCount()
                        step_result['firing'] = brain.getFiringNeuronCount()
                        step_result['stats'] = agent.get_stats()
                        
                        simulation_results['total_spikes'] = step_result['spikes']
                        simulation_results['final_firing'] = step_result['firing']
                        simulation_results['average_firing_rate'] = brain.getAverageFiringRate()
                        
                        # Track action distribution
                        action_name = action.get_name() if hasattr(action, 'get_name') else str(action)
                        simulation_results['action_distribution'][action_name] = (
                            simulation_results['action_distribution'].get(action_name, 0) + 1
                        )
                        
                    except Exception as e:
                        error_msg = f"Statistics collection failed: {e}"
                        step_result['warnings'].append(error_msg)
                        self.record_error("statistics", e)
                        simulation_results['warnings_encountered'].append(error_msg)
                    
                    # Check for concerning patterns
                    try:
                        if step_result['spikes'] > 10000:
                            warning_msg = f"High spike count ({step_result['spikes']})"
                            step_result['warnings'].append(warning_msg)
                            simulation_results['warnings_encountered'].append(warning_msg)
                        
                        if step_result['firing'] > brain.getTotalNeuronCount() * 0.8:
                            warning_msg = f"High firing neurons ({step_result['firing']})"
                            step_result['warnings'].append(warning_msg)
                            simulation_results['warnings_encountered'].append(warning_msg)
                    
                    except Exception as e:
                        error_msg = f"Pattern detection failed: {e}"
                        step_result['errors'].append(error_msg)
                        self.record_error("pattern_detection", e)
                        simulation_results['errors_encountered'].append(error_msg)
                    
                    # Record step result
                    simulation_results['step_data'].append(step_result)
                    
                    # Reset consecutive error counter
                    consecutive_errors = 0
                    
                    # Print progress periodically
                    if step % 20 == 0:
                        print(f"  Step {step}: {step_result['spikes']} spikes, "
                              f"{step_result['firing']} firing neurons, "
                              f"action={action}")
                    
                    simulation_results['steps_completed'] += 1
                    
                except Exception as e:
                    # Simulation error handling
                    error_msg = f"Simulation error at step {step}: {e}"
                    step_result['errors'].append(error_msg)
                    
                    self.record_error("simulation", e)
                    simulation_results['errors_encountered'].append(error_msg)
                    
                    consecutive_errors += 1
                    print(f"  ✗ Step {step} error: {e}")
                    
                    # Check if we should continue or stop
                    if consecutive_errors >= self.max_consecutive_errors:
                        print(f"\n🚨 Too many consecutive errors ({consecutive_errors}). Stopping simulation.")
                        print(f"   Last error: {self.last_error}")
                        break
                    
                    # Continue simulation despite error
                    continue
            
            # Simulation completed successfully
            print(f"\n✓ Simulation completed successfully!")
            print(f"  Steps completed: {simulation_results['steps_completed']}")
            print(f"  Total spikes: {simulation_results['total_spikes']}")
            print(f"  Final firing neurons: {simulation_results['final_firing']}")
            print(f"  Average firing rate: {simulation_results['average_firing_rate']:.2f} Hz")
            
            return simulation_results
            
        except Exception as e:
            # Major simulation failure
            print(f"\n🚨 Simulation failed: {e}")
            return self.get_error_results("simulation_failed", str(e))
    
    def record_error(self, error_type, error):
        """Record error information for analysis."""
        self.error_count += 1
        self.last_error = error
        
        if error_type not in self.error_types:
            self.error_types[error_type] = 0
        self.error_types[error_type] += 1
        
        # Store error step if available
        import traceback
        error_trace = traceback.format_exc()
        self.error_steps.append({
            'type': error_type,
            'message': str(error),
            'traceback': error_trace,
            'count': self.error_types[error_type]
        })
    
    def get_error_results(self, error_type, error_message=""):
        """Get standardized error results."""
        results = {
            'success': False,
            'error_type': error_type,
            'error_message': error_message,
            'error_count': self.error_count,
            'error_types': self.error_types,
            'last_error': str(self.last_error) if self.last_error else None,
            'steps_completed': 0,
            'total_spikes': 0,
            'final_firing': 0,
            'average_firing_rate': 0.0,
            'errors_encountered': [],
            'warnings_encountered': [],
            'action_distribution': {}
        }
        
        return results
    
    def get_simulation_summary(self, results):
        """Generate a summary of simulation results."""
        if results['success']:
            summary = f"Simulation completed successfully!\n"
            summary += f"  Steps: {results['steps_completed']}\n"
            summary += f"  Total spikes: {results['total_spikes']}\n"
            summary += f"  Final firing neurons: {results['final_firing']}\n"
            summary += f"  Average firing rate: {results['average_firing_rate']:.2f} Hz\n"
            
            if results['action_distribution']:
                summary += f"  Action distribution: {results['action_distribution']}\n"
            
            if results['errors_encountered']:
                summary += f"  Errors encountered: {len(results['errors_encountered'])}\n"
            
            if results['warnings_encountered']:
                summary += f"  Warnings encountered: {len(results['warnings_encountered'])}\n"
            
            return summary
        else:
            summary = f"Simulation failed with error: {results['error_type']}\n"
            if results['error_message']:
                summary += f"  Details: {results['error_message']}\n"
            summary += f"  Errors encountered: {results['error_count']}\n"
            summary += f"  Error types: {results['error_types']}\n"
            
            if results['last_error']:
                summary += f"  Last error: {results['last_error']}\n"
            
            return summary

def demonstrate_error_handling():
    """Demonstrate error handling capabilities."""
    
    print("=== ADVANCED ERROR HANDLING DEMONSTRATION ===\n")
    
    # Create robust simulation wrapper
    sim = RobustSimulation(max_consecutive_errors=3)
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 200)
    config.set("random_seed", 42)
    
    # Create world specification
    world_spec = {
        'configure': [30, 30, 8, 8]
    }
    
    # Create agent specification
    agent_spec = {
        'enable_reward_modulation': True,
        'enable_curiosity': True
    }
    
    # Run simulation with error handling
    results = sim.run_simulation_with_error_handling(
        config=config,
        world_spec=world_spec,
        agent_spec=agent_spec,
        max_steps=100
    )
    
    # Print results
    summary = sim.get_simulation_summary(results)
    print(summary)
    
    # Show error analysis
    if sim.error_types:
        print(f"\n=== ERROR ANALYSIS ===")
        print(f"Total errors: {sim.error_count}")
        print(f"Error types: {sim.error_types}")
        
        print(f"\nDetailed error history:")
        for error_info in sim.error_steps:
            print(f"  Type: {error_info['type']}")
            print(f"    Message: {error_info['message']}")
            print(f"    Count: {error_info['count']}")
    
    return results

if __name__ == "__main__":
    demonstrate_error_handling()
```

## Example 11: Integration with External Systems

```python
"""
Example showing integration with external systems and data.
This demonstrates how to integrate NLM with external data sources and systems.
"""
import pynlm
import json
import numpy as np

class ExternalDataSource:
    """Interface for external data sources."""
    
    def get_current_sensory_input(self):
        """Get current sensory input from external source."""
        raise NotImplementedError
    
    def get_current_time(self):
        """Get current time from external source."""
        raise NotImplementedError
    
    def is_active(self):
        """Check if external source is active."""
        raise NotImplementedError

class SensorDataSource(ExternalDataSource):
    """Simulates sensor data from external system."""
    
    def __init__(self, sensor_id, data_file=None):
        self.sensor_id = sensor_id
        self.data_file = data_file
        self.time = 0.0
        self.data = []
        self.load_data()
    
    def load_data(self):
        """Load sensor data from file or generate synthetic data."""
        if self.data_file and os.path.exists(self.data_file):
            # Load from JSON file
            with open(self.data_file, 'r') as f:
                sensor_data = json.load(f)
            
            # Extract vision data
            if 'vision' in sensor_data:
                self.data = sensor_data['vision']
            else:
                # Generate synthetic vision data
                self.data = np.random.rand(8, 8, 3) * 0.5
        else:
            # Generate synthetic sensor data
            self.data = np.random.rand(8, 8, 3) * 0.5
    
    def get_current_sensory_input(self):
        """Get current sensory input."""
        # Get current frame (cycling through data)
        frame_index = int(self.time * 10) % len(self.data) if self.data else 0
        
        # Create vision object from data
        vision_data = self.data[frame_index].flatten() if len(self.data) > 0 else []
        
        vision = pynlm.Vision(8, 8, 3)
        if vision_data:
            vision.setData(vision_data)
        
        return vision
    
    def get_current_time(self):
        """Get current time."""
        return self.time
    
    def is_active(self):
        """Check if sensor is active."""
        return True
    
    def advance_time(self, dt):
        """Advance time."""
        self.time += dt

class RewardCalculator:
    """Calculates rewards based on external factors."""
    
    def __init__(self, reward_config=None):
        self.reward_config = reward_config or {}
        self.predicted_rewards = {}
    
    def calculate_reward(self, agent_state, external_context):
        """Calculate reward based on agent state and external context."""
        reward = 0.0
        
        # Basic reward calculation
        if external_context.get('target_achieved', False):
            reward += self.reward_config.get('target_reward', 1.0)
        
        if external_context.get('obstacle_detected', False):
            reward -= self.reward_config.get('obstacle_penalty', 0.5)
        
        # Agent-specific rewards
        if 'agent_id' in external_context:
            agent_id = external_context['agent_id']
            if agent_id not in self.predicted_rewards:
                self.predicted_rewards[agent_id] = 0.0
            
            # Simple prediction based on previous state
            predicted = self.predicted_rewards[agent_id]
            reward += 0.1 * (reward - predicted)  # Prediction error bonus
            
            # Update prediction
            self.predicted_rewards[agent_id] = reward
        
        # Apply reward bounds
        min_reward = self.reward_config.get('min_reward', -1.0)
        max_reward = self.reward_config.get('max_reward', 1.0)
        reward = max(min_reward, min(max_reward, reward))
        
        return reward

class IntegratedSimulation:
    """Simulation integrated with external systems."""
    
    def __init__(self, name, config, external_systems):
        self.name = name
        self.config = config
        self.external_systems = external_systems
        
        # Initialize components
        self.brain = None
        self.agent = None
        self.reward_calculator = RewardCalculator()
    
    def initialize(self):
        """Initialize simulation components."""
        print(f"Initializing {self.name}...")
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create world
        self.world = pynlm.createSimpleWorld()
        world_config = self.config.get('world', {})
        self.world.configure(
            world_config.get('width', 30),
            world_config.get('height', 30),
            world_config.get('vision_width', 8),
            world_config.get('vision_height', 8)
        )
        self.world.reset()
        
        # Create agent
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable features
        agent_config = self.config.get('agent', {})
        if agent_config.get('reward_modulation', True):
            self.agent.enableRewardModulation(True)
        
        if agent_config.get('curiosity', True):
            self.agent.enableCuriosity(True)
        
        if agent_config.get('development', True):
            self.agent.enableDevelopment(True)
        
        print(f"  ✓ {self.name} initialized")
    
    def run_step(self, step):
        """Run one simulation step with external integration."""
        step_result = {
            'step': step,
            'external_data': {},
            'rewards': {},
            'action': None,
            'success': True,
            'errors': []
        }
        
        try:
            # Update external systems
            for system_name, system in self.external_systems.items():
                if system.is_active():
                    system.advance_time(0.1)
                    
                    # Get external data
                    try:
                        if system_name == 'sensor':
                            step_result['external_data']['vision'] = system.get_current_sensory_input()
                        elif system_name == 'reward_calculator':
                            step_result['external_data']['context'] = system.get_current_context()
                    except Exception as e:
                        step_result['errors'].append(f"External system {system_name} error: {e}")
                        step_result['success'] = False
            
            # Update world
            try:
                self.world.update(0.1)
            except Exception as e:
                step_result['errors'].append(f"World update error: {e}")
                step_result['success'] = False
            
            # Get sensory input (from external system if available, otherwise world)
            if 'sensor' in self.external_systems and self.external_systems['sensor'].is_active():
                percept = pynlm.SensoryPercept()
                if 'vision' in step_result['external_data']:
                    percept.setVision(step_result['external_data']['vision'])
                else:
                    percept = self.world.getSensoryPercept()
            else:
                percept = self.world.getSensoryPercept()
            
            # Process sensory input
            try:
                self.agent.processSensoryInput(percept)
            except Exception as e:
                step_result['errors'].append(f"Sensory processing error: {e}")
                step_result['success'] = False
            
            # Brain processing
            try:
                self.brain.step(step)
            except Exception as e:
                step_result['errors'].append(f"Brain step error: {e}")
                step_result['success'] = False
            
            # Get action
            try:
                action = self.agent.decodeMotorCommand()
                step_result['action'] = action
            except Exception as e:
                step_result['errors'].append(f"Action decoding error: {e}")
                step_result['success'] = False
            
            # Apply action
            try:
                self.world.applyMotorCommand(action, self.world.getSimulationTime())
            except Exception as e:
                step_result['errors'].append(f"Action application error: {e}")
                step_result['success'] = False
            
            # Calculate reward
            try:
                external_context = step_result['external_data'].get('context', {})
                reward = self.reward_calculator.calculate_reward(
                    {'position': (self.world.getAgentBody().x, self.world.getAgentBody().y)},
                    external_context
                )
                step_result['rewards']['total'] = reward
                
                # Apply reward modulation
                predicted_reward = step_result['rewards'].get('predicted', 0.0)
                self.agent.applyRewardModulation(reward, predicted_reward)
                
            except Exception as e:
                step_result['errors'].append(f"Reward calculation error: {e}")
                step_result['success'] = False
            
            return step_result
            
        except Exception as e:
            step_result['errors'].append(f"Simulation error: {e}")
            step_result['success'] = False
            return step_result
    
    def run_simulation(self, max_steps=100):
        """Run complete simulation with external integration."""
        print(f"\nRunning integrated simulation: {self.name}")
        print(f"Duration: {max_steps} steps")
        
        results = {
            'name': self.name,
            'steps_completed': 0,
            'successful_steps': 0,
            'total_spikes': 0,
            'final_firing': 0,
            'average_firing_rate': 0.0,
            'total_rewards': 0.0,
            'external_systems_data': {},
            'step_results': [],
            'errors': []
        }
        
        try:
            for step in range(max_steps):
                step_result = self.run_step(step)
                results['step_results'].append(step_result)
                
                results['steps_completed'] += 1
                
                if step_result['success']:
                    results['successful_steps'] += 1
                
                # Collect statistics
                if step_result['action']:
                    action_name = action.get_name() if hasattr(action, 'get_name') else str(action)
                    if action_name not in results['external_systems_data']:
                        results['external_systems_data'][action_name] = 0
                    results['external_systems_data'][action_name] += 1
                
                if 'rewards' in step_result and 'total' in step_result['rewards']:
                    results['total_rewards'] += step_result['rewards']['total']
                
                if step_result['errors']:
                    results['errors'].extend(step_result['errors'])
                
                # Print progress periodically
                if step % 20 == 0:
                    print(f"  Step {step}: {step_result['success'] and '✓' or '✗'}")
                    if step_result['errors']:
                        print(f"    Errors: {len(step_result['errors'])}")
            
            # Calculate final statistics
            if self.brain:
                results['total_spikes'] = self.brain.getTotalSpikeCount()
                results['final_firing'] = self.brain.getFiringNeuronCount()
                results['average_firing_rate'] = self.brain.getAverageFiringRate()
            
            print(f"\n✓ Integrated simulation completed!")
            print(f"  Steps completed: {results['steps_completed']}")
            print(f"  Successful steps: {results['successful_steps']}")
            print(f"  Total spikes: {results['total_spikes']}")
            print(f"  Final firing neurons: {results['final_firing']}")
            print(f"  Average firing rate: {results['average_firing_rate']:.2f} Hz")
            print(f"  Total rewards: {results['total_rewards']:.2f}")
            print(f"  Errors: {len(results['errors'])}")
            
            return results
            
        except Exception as e:
            print(f"\n🚨 Integrated simulation failed: {e}")
            results['errors'].append(f"Simulation failure: {e}")
            return results

class SensorSimulator(ExternalDataSource):
    """Simulates sensor data for integration."""
    
    def __init__(self, sensor_id):
        super().__init__(sensor_id)
        self.data_file = "sensor_data.json"
        self.context = {
            'sensor_id': sensor_id,
            'target_achieved': False,
            'obstacle_detected': False,
            'timestamp': 0.0
        }
    
    def get_current_context(self):
        """Get current context for reward calculation."""
        return self.context
    
    def update_context(self):
        """Update sensor context."""
        # Simulate context changes
        if np.random.random() < 0.1:  # 10% chance of target achievement
            self.context['target_achieved'] = True
        else:
            self.context['target_achieved'] = False
        
        if np.random.random() < 0.05:  # 5% chance of obstacle detection
            self.context['obstacle_detected'] = True
        else:
            self.context['obstacle_detected'] = False
        
        self.context['timestamp'] = self.time

def demonstrate_external_integration():
    """Demonstrate integration with external systems."""
    
    print("=== EXTERNAL SYSTEMS INTEGRATION DEMONSTRATION ===\n")
    
    # Create external systems
    sensor = SensorSimulator("vision_sensor")
    reward_calc = RewardCalculator()
    
    # Create configuration
    config = {
        'neuron_count': 250,
        'plasticity_rate': 1.0,
        'random_seed': 42,
        'world': {'width': 40, 'height': 40, 'vision_width': 10, 'vision_height': 10},
        'agent': {
            'reward_modulation': True,
            'curiosity': True,
            'development': True
        }
    }
    
    # Create integrated simulation
    simulation = IntegratedSimulation(
        name="External Integration Demo",
        config=config,
        external_systems={
            'sensor': sensor,
            'reward_calculator': reward_calc
        }
    )
    
    # Initialize and run simulation
    simulation.initialize()
    
    # Run simulation with external integration
    results = simulation.run_simulation(max_steps=80)
    
    # Print summary
    print(f"\n{'='*60}")
    print("INTEGRATION RESULTS SUMMARY")
    print(f"{'='*60}")
    print(f"Simulation: {results['name']}")
    print(f"Steps completed: {results['steps_completed']}")
    print(f"Successful steps: {results['successful_steps']}")
    print(f"Success rate: {results['successful_steps']/results['steps_completed']*100:.1f}%")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Final firing neurons: {results['final_firing']}")
    print(f"Average firing rate: {results['average_firing_rate']:.2f} Hz")
    print(f"Total rewards: {results['total_rewards']:.2f}")
    print(f"Errors: {len(results['errors'])}")
    
    if results['external_systems_data']:
        print(f"\nExternal system data:")
        for action, count in results['external_systems_data'].items():
            print(f"  {action}: {count}")
    
    # Show error analysis
    if results['errors']:
        print(f"\nError Analysis:")
        print(f"  Total errors: {len(results['errors'])}")
        
        # Count error types
        error_types = {}
        for error in results['errors']:
            if 'system' in error.lower():
                error_type = 'external_system'
            elif 'world' in error.lower():
                error_type = 'world'
            elif 'brain' in error.lower():
                error_type = 'brain'
            elif 'action' in error.lower():
                error_type = 'action'
            else:
                error_type = 'other'
            
            error_types[error_type] = error_types.get(error_type, 0) + 1
        
        print(f"  Error types: {error_types}")
    
    return results

if __name__ == "__main__":
    demonstrate_external_integration()
```

## Example 12: Python API Reference

This file serves as a quick reference to the most commonly used Python API functions and classes.

### Core Classes

#### pynlm.Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)
brain.receiveSensoryInput(percept)
brain.injectCurrent(neuron_id, current)
brain.produceAction()
brain.applyNeuromodulation(signal)
brain.reset()
brain.save(filepath)
brain.load(filepath)

# Properties
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getTotalSpikeCount()
brain.getFiringNeuronCount()
brain.getAverageFiringRate()
```

#### pynlm.AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted_reward)
agent.updateDevelopment(timestep)
agent.get_stats()
agent.simulate_step(world, timestep)
agent.simulate_step_with_stats(world, timestep)
agent.enable_all_features()

# Get current state
agent.getNeuromodulationLevel()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getDevelopmentalStage()
```

#### pynlm.SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(width, height, visionWidth, visionHeight)
world.reset()
world.setAgentStart(x, y)
world.update(timestep)
world.applyMotorCommand(action, time)
world.getSensoryPercept()
world.getAgentBody()
world.addObject(object)
world.removeObject(x, y)
world.isValidPosition(x, y)

# World properties
world.getWidth()
world.getHeight()
world.getVisionWidth()
world.getVisionHeight()
world.getMaxEnergy()
world.getEnergyDecayRate()
world.getSimulationTime()
```

#### pynlm.Config
```python
config = pynlm.createDefaultConfig()
config.loadFromFile(filepath)
config.loadFromArgs(argc, argv)
config.has(key)
config.getOr(key, default_value)
config.set(key, value)
config.remove(key)
config.clear()
config.summary()
```

### Commonly Used Action Types
```python
import pynlm

# Action types
action_types = [
    pynlm.ActionType.MoveForward,
    pynlm.ActionType.MoveBackward,
    pynlm.ActionType.TurnLeft,
    pynlm.ActionType.TurnRight,
    pynlm.ActionType.Look,
    pynlm.ActionType.Interact,
    pynlm.ActionType.Eat,
    pynlm.ActionType.Wait,
    pynlm.ActionType.Rest
]

# Motor commands
motor_commands = [
    pynlm.MotorCommand.MoveForward,
    pynlm.MotorCommand.MoveBackward,
    pynlm.MotorCommand.TurnLeft,
    pynlm.MotorCommand.TurnRight,
    pynlm.MotorCommand.Interact,
    pynlm.MotorCommand.Wait
]
```

### Utility Functions
```python
# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Create world
world = pynlm.createSimpleWorld()

# Create agent
agent = pynlm.createAgentBrain(brain)

# Invalid ID constants
INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
INVALID_SYNAPSE_ID = pynlm.INVALID_SYNAPSE_ID
INVALID_REGION_ID = pynlm.INVALID_REGION_ID
INVALID_POPULATION_ID = pynlm.INVALID_POPULATION_ID
```

### Example Quick Start Function
```python
import pynlm

def quick_start_simulation(duration_steps=50):
    """
    Quick start function for basic simulation.
    
    This function demonstrates the minimal setup needed to run
    a basic NLM simulation.
    """
    # Setup
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(duration_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Results
    return {
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'final_position': (world.getAgentBody().x, world.getAgentBody().y),
        'average_firing_rate': brain.getAverageFiringRate()
    }

# Usage
if __name__ == "__main__":
    results = quick_start_simulation(100)
    print(f"Quick start results: {results}")
```

## Common Error Messages and Solutions

### Error: "ImportError: cannot import name 'pynlm'"
**Solution:**
- Ensure pynlm is installed: `pip install pynlm`
- Check Python path: `pip show pynlm`
- Install from source if needed: `pip install .`

### Error: "AttributeError: 'Brain' object has no attribute 'method'"
**Solution:**
- Check available methods in the Brain class
- Use `dir(brain)` to list available methods
- Consult the API documentation

### Error: "TypeError: step() takes 2 positional arguments but 3 were given"
**Solution:**
- Check the signature of `brain.step()`
- Use `brain.step(step, time)` for time-based stepping
- Use `brain.step(step)` for simple stepping

### Error: "ValueError: Invalid action type"
**Solution:**
- Use valid action types from `pynlm.ActionType`
- Check action enum values
- Ensure action type is properly imported

### Error: "RuntimeError: Brain not initialized"
**Solution:**
- Call `brain.initialize()` before using the brain
- Check initialization status
- Verify configuration is valid

### Error: "MemoryError: Out of memory"
**Solution:**
- Reduce simulation complexity (fewer neurons, smaller world)
- Use checkpointing for long simulations
- Monitor memory usage
- Consider using optimized configurations

## Performance Tips

### For Faster Simulations
1. **Use smaller configurations:**
   - Fewer neurons (50-100 instead of 500)
   - Smaller world (20x20 instead of 50x50)
   - Disable structural plasticity

2. **Disable learning features:**
   - Turn off reward modulation
   - Turn off curiosity
   - Disable development

3. **Use speed optimization:**
   - Set `plasticity_rate` to 0.5
   - Use performance preset configurations

### For More Stable Simulations
1. **Increase neurons:** More neurons provide more stable statistics
2. **Enable development:** Allows the brain to mature and stabilize
3. **Use balanced plasticity:** Avoid too high or too low plasticity rates
4. **Monitor firing rates:** Keep firing rates within reasonable bounds

### For Research/Experiments
1. **Use checkpointing:** Save and load brain states
2. **Track detailed metrics:** Record spikes, actions, and rewards
3. **Run multiple trials:** Average results over several simulations
4. **Use consistent configurations:** Ensure reproducibility

## Troubleshooting Checklist

### Before Running
- [ ] Install pynlm properly
- [ ] Check Python version compatibility
- [ ] Verify system requirements (memory, CPU)
- [ ] Create necessary directories (configs, checkpoints)

### During Simulation
- [ ] Monitor spike counts for excessive activity
- [ ] Watch for hanging or freezing
- [ ] Check for memory leaks
- [ ] Monitor system resources

### After Simulation
- [ ] Save simulation results
- [ ] Verify completion status
- [ ] Check for errors in output
- [ ] Analyze performance metrics

## Getting Help

### Online Resources
- **Documentation:** https://nlm.readthedocs.io
- **GitHub Repository:** https://github.com/nlm-project/nlm
- **Issue Tracker:** https://github.com/nlm-project/nlm/issues
- **Discussions:** https://github.com/nlm-project/nlm/discussions

### Local Help
- **Python Console:** Use `help(pynlm)` for module information
- **IDE Help:** Most IDEs provide code completion for Python
- **Text Editor:** Use language-specific features for Python

### Support
- **Bug Reports:** Create issues with detailed error messages
- **Feature Requests:** Submit feature requests with use cases
- **Questions:** Ask questions in discussions or issues

## Version Information

```python
import pynlm
print(f"NLM Python Bindings Version: {pynlm.__version__}")
```

## License

This documentation and examples are part of the NLM project.
For license information, see the main NLM documentation.

## Examples Index

| Example | Description | Difficulty |
|---------|-------------|------------|
| Example 1 | Simple Brain Simulation | Beginner |
| Example 2 | Agent in World Environment | Beginner |
| Example 3 | Learning Agent with Rewards | Intermediate |
| Example 4 | Performance and Benchmarking | Intermediate |
| Example 5 | Multi-Agent Cooperation | Advanced |
| Example 6 | Loading and Saving Brain States | Intermediate |
| Example 7 | Debugging and Monitoring | Advanced |
| Example 8 | Performance Optimization | Intermediate |
| Example 9 | Configuration Management | Intermediate |
| Example 10 | Advanced Error Handling | Advanced |
| Example 11 | Integration with External Systems | Advanced |
| Example 12 | Python API Reference | All Levels |

---

This file contains practical examples for using the NLM Python interface. 
Each example demonstrates different aspects of working with NLM and can be run
independently to learn specific functionality.

For more detailed documentation, refer to `README_PYTHON.md` in the main project directory.