# Advanced NLM Usage Guide

## Overview

This guide provides advanced usage patterns and examples for NLM users who want to go beyond the basic functionality. It covers:

- Advanced agent control and customization
- Sophisticated simulation scenarios
- Performance optimization techniques
- Debugging and troubleshooting
- Integration with external systems

## Prerequisites

- Basic understanding of NLM (see easy_usage.md)
- Familiarity with Python bindings API
- Understanding of neural simulation concepts

## 1. Advanced Agent Configuration

### Custom Brain Architecture

```python
import pynlm
import json

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Configure multiple brain regions
config.set("brain.region_count", 3)
config.set("brain.region_0.neuron_count", 200)
config.set("brain.region_1.neuron_count", 150)
config.set("brain.region_2.neuron_count", 100)

# Configure advanced connectivity
config.set("brain.connection_probability", 0.08)
config.set("brain.initial_weight_mean", 0.3)
config.set("brain.initial_weight_std", 0.2)

# Configure plasticity rules
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.stdp.tau_plus", 25.0)
config.set("plasticity.stdp.tau_minus", 25.0)

config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.learning_rate", 0.001)

config.set("plasticity.structural.enable", True)
config.set("plasticity.structural.synaptogenesis_rate", 0.0002)
config.set("plasticity.structural.pruning_rate", 0.00002)

# Configure neuromodulation
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.dopamine.scale", 1.5)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getRegionCount()} regions")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

### Custom Motor Command Sets

```python
import pynlm

# Create custom world for specialized tasks
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

# Create custom agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Custom motor command strategies
def advanced_motor_strategy(agent_brain, step):
    """Custom motor selection logic"""
    curiosity = agent_brain.getCuriosityLevel()
    neuromod = agent_brain.getNeuromodulationLevel()
    
    if curiosity > 0.8:
        # High exploration mode
        return select_exploratory_action(agent_brain)
    elif neuromod < -0.5:
        # Negative reward mode - cautious behavior
        return MotorCommand.Wait
    else:
        # Normal mode
        return agent_brain.decodeMotorCommand()

def select_exploratory_action(agent_brain):
    """Select random exploration action"""
    import random
    actions = list(MotorCommand)
    return random.choice([a for a in actions if a != MotorCommand.Wait])
```

## 2. Advanced Simulation Scenarios

### Goal-Directed Learning

```python
def run_goal_directed_learning(brain, world, agent, num_steps, goal_position):
    """Run simulation where agent learns to reach a goal"""
    
    # Enable prediction for goal pursuit
    agent.enableRewardModulation(True)
    
    # Track progress
    success_count = 0
    attempts = 0
    
    for step in range(num_steps):
        world.update(0.1)
        
        # Get current perception
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Use prediction to plan action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Calculate reward based on proximity to goal
        body = world.getAgentBody()
        dx = goal_position[0] - body.x
        dy = goal_position[1] - body.y
        distance = (dx**2 + dy**2)**0.5
        
        # Use negative distance as reward (closer to goal = higher reward)
        reward = max(0, 10.0 - distance)
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check success
        if distance < 1.0:
            success_count += 1
        
        attempts += 1
        
        if step % 100 == 0:
            print(f"Step {step}: Distance to goal: {distance:.2f}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Success rate: {success_count/attempts:.3f}")
    
    return success_count/attempts if attempts > 0 else 0.0

# Example usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run goal-directed learning
target_x, target_y = 45.0, 45.0
 success_rate = run_goal_directed_learning(brain, world, agent, 1000, (target_x, target_y))
print(f"Goal success rate: {success_rate:.2%}")
```

### Multi-Agent Coordination

```python
def run_multi_agent_simulation(num_agents, world_width, world_height, steps):
    """Run simulation with multiple coordinated agents"""
    
    agents = []
    brains = []
    
    # Create multiple agents with different configurations
    for i in range(num_agents):
        # Vary agent parameters for diversity
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 300 + i * 50)
        config.set("brain.connection_probability", 0.05 + i * 0.01)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Different agents have different enabled subsystems
        agent.enableRewardModulation(i % 2 == 0)
        agent.enableCuriosity(i % 3 == 0)
        
        agents.append(agent)
        brains.append(brain)
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=world_width, height=world_height, 
                   visionWidth=8, visionHeight=8)
    world.reset()
    
    # Place agents at different starting positions
    for i, agent_body in enumerate(world.getAgentBody()):
        if i < len(agents):
            angle = i * (2 * 3.14159 / len(agents))
            agent_body.x = world_width * 0.1 + 0.7 * world_width * i / len(agents)
            agent_body.y = world_height * 0.1 + 0.7 * world_height * i / len(agents)
    
    # Simulation loop with agent interaction
    for step in range(steps):
        # Each agent perceives world
        for i, (agent, brain) in enumerate(zip(agents, brains)):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
        
        # Agents select actions
        actions = [agent.decodeMotorCommand() for agent in agents]
        
        # Apply actions to world (with collision detection)
        for i, action in enumerate(actions):
            agent_body = world.getAgentBody()[i]
            
            # Check if action is valid
            if is_valid_action(world, agent_body, action):
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply rewards based on agent interactions
        for i, (agent, brain) in enumerate(zip(agents, brains)):
            reward = calculate_social_reward(agent, world, i)
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        if step % 100 == 0:
            print(f"Step {step}")
            for i, agent in enumerate(agents):
                print(f"  Agent {i}: {brain.getFiringNeuronCount()} firing neurons")
    
    return agents, brains

def is_valid_action(world, agent_body, action):
    """Check if agent action is valid (not blocked)"""
    new_x, new_y = agent_body.x, agent_body.y
    
    # Calculate new position based on action
    if action == pynlm.MotorCommand.MoveForward:
        new_x += 0.5
    elif action == pynlm.MotorCommand.MoveBackward:
        new_x -= 0.5
    elif action == pynlm.MotorCommand.TurnLeft:
        agent_body.orientation -= 0.1
    elif action == pynlm.MotorCommand.TurnRight:
        agent_body.orientation += 0.1
    
    return world.isValidPosition(new_x, new_y)

def calculate_social_reward(agent, world, agent_id):
    """Calculate reward based on social interactions"""
    # Example: reward for being close to other agents
    total_reward = 0.0
    
    for other_id in range(world.getAgentBody().size()):
        if other_id != agent_id:
            other_body = world.getAgentBody()[other_id]
            agent_body = world.getAgentBody()[agent_id]
            
            dx = other_body.x - agent_body.x
            dy = other_body.y - agent_body.y
            distance = (dx**2 + dy**2)**0.5
            
            # Reward for proximity (social bonding)
            if distance < 2.0:
                total_reward += 0.5 / (distance + 0.1)
            
            # Penalty for collision
            if distance < 0.5:
                total_reward -= 1.0
    
    return total_reward
```

## 3. Performance Optimization

### Memory-Efficient Simulation

```python
def run_memory_efficient_simulation(brain, world, agent, num_steps):
    """Run simulation with memory optimization"""
    
    # Use memory pool if available
    if hasattr(brain, 'getWorkingMemory'):
        # Configure working memory size
        working_memory = brain.getWorkingMemory()
        working_memory.configure_max_capacity(1000)
    
    # Use sparse connectivity for large networks
    if hasattr(brain, 'getConfig'):
        config = brain.getConfig()
        config.set("brain.synapse_density", 0.05)  # Sparse connections
    
    # Enable neural prediction for efficient processing
    if hasattr(brain, 'getPredictionSystem'):
        prediction_system = brain.getPredictionSystem()
        prediction_system.enable_temporal_predictors(True)
        prediction_system.set_predictor_count(5)
    
    # Run simulation with optimized parameters
    for step in range(num_steps):
        world.update(0.1)
        
        # Use optimized sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Use efficient brain stepping
        brain.step(step)
        
        # Use optimized action selection
        action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain.getTotalSpikeCount() / num_steps  # Average firing rate
```

### Checkpoint-Based Training

```python
def train_with_checkpoints(brain, world, agent, steps_per_checkpoint, total_steps):
    """Train brain with periodic checkpointing for recovery"""
    
    checkpoint_dir = "checkpoints"
    max_memory = 100  # Keep last 100 checkpoints
    
    for step in range(0, total_steps, steps_per_checkpoint):
        # Run checkpoint segment
        segment_end = min(step + steps_per_checkpoint, total_steps)
        
        for current_step in range(step, segment_end):
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(current_step)
            
            # Select and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply learning
            reward = calculate_reward(world, agent)
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Create checkpoint
        checkpoint_path = f"{checkpoint_dir}/checkpoint_{step:08d}.bin"
        if brain.save(checkpoint_path):
            print(f"Checkpoint saved: {checkpoint_path}")
            
            # Clean up old checkpoints
            cleanup_checkpoints(checkpoint_dir, max_memory)
    
    return brain

def cleanup_checkpoints(checkpoint_dir, max_keep):
    """Remove old checkpoints to save space"""
    import glob
    import os
    from datetime import datetime
    
    checkpoints = glob.glob(f"{checkpoint_dir}/*.bin")
    checkpoints.sort(key=os.path.getmtime)
    
    # Remove oldest checkpoints if we have too many
    while len(checkpoints) > max_keep:
        old_checkpoint = checkpoints.pop(0)
        os.remove(old_checkpoint)
        print(f"Removed old checkpoint: {old_checkpoint}")
```

## 4. Debugging and Troubleshooting

### Brain State Monitoring

```python
def monitor_brain_health(brain, agent, world, duration, interval=100):
    """Monitor brain state for debugging"""
    
    metrics = {
        'firing_rates': [],
        'spike_counts': [],
        'activity_patterns': [],
        'development_stages': [],
        'neuromodulation_levels': []
    }
    
    for step in range(0, duration, interval):
        # Record metrics
        metrics['firing_rates'].append(brain.getFiringNeuronCount())
        metrics['spike_counts'].append(brain.getTotalSpikeCount())
        metrics['development_stages'].append(brain.getDevelopmentalStage())
        
        if agent:
            metrics['neuromodulation_levels'].append(agent.getNeuromodulationLevel())
        
        # Get activity pattern
        if hasattr(brain, 'getRegions'):
            pattern = analyze_region_activity(brain)
            metrics['activity_patterns'].append(pattern)
        
        # Run simulation segment
        for current_step in range(step, min(step + interval, duration)):
            world.update(0.1)
            
            if agent:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(current_step)
                action = agent.decodeMotorCommand()
            
            world.applyMotorCommand(action, world.getSimulationTime())
    
    return metrics

def analyze_region_activity(brain):
    """Analyze activity across brain regions"""
    if not hasattr(brain, 'getRegions'):
        return None
    
    pattern = {}
    for region in brain.getRegions():
        neurons = region.getAllNeurons()
        active = sum(1 for n in neurons if n.getFiringState() == n.getFiringState().Active)
        
        region_name = getattr(region, 'getName', lambda: f"Region_{region.getId()}")()
        pattern[region_name] = {
            'total_neurons': len(neurons),
            'active_neurons': active,
            'activation_ratio': active / len(neurons) if neurons else 0.0
        }
    
    return pattern
```

### Performance Profiling

```python
def profile_simulation_performance(brain, world, agent, total_steps):
    """Profile simulation performance and identify bottlenecks"""
    
    import time
    
    # Profile different phases
    phase_times = {
        'sensory_processing': 0.0,
        'brain_computation': 0.0,
        'action_selection': 0.0,
        'world_update': 0.0,
        'learning': 0.0
    }
    
    for step in range(total_steps):
        step_start = time.time()
        
        # Profile sensory processing
        sense_start = time.time()
        if agent:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
        phase_times['sensory_processing'] += time.time() - sense_start
        
        # Profile brain computation
        compute_start = time.time()
        brain.step(step)
        phase_times['brain_computation'] += time.time() - compute_start
        
        # Profile action selection
        action_start = time.time()
        if agent:
            action = agent.decodeMotorCommand()
        phase_times['action_selection'] += time.time() - action_start
        
        # Profile world update
        world_start = time.time()
        world.update(0.1)
        phase_times['world_update'] += time.time() - world_start
        
        # Profile learning
        learn_start = time.time()
        if agent:
            # Apply learning if simulation involves learning
            pass
        phase_times['learning'] += time.time() - learn_start
    
    # Calculate statistics
    total_time = sum(phase_times.values())
    print(f"\nPerformance Profile (total time: {total_time:.2f}s)")
    print(f"Sensory Processing: {phase_times['sensory_processing']/total_time:.1%}")
    print(f"Brain Computation: {phase_times['brain_computation']/total_time:.1%}")
    print(f"Action Selection: {phase_times['action_selection']/total_time:.1%}")
    print(f"World Update: {phase_times['world_update']/total_time:.1%}")
    print(f"Learning: {phase_times['learning']/total_time:.1%}")
    
    # Identify bottlenecks
    bottlenecks = []
    for phase, time_taken in phase_times.items():
        if time_taken / total_time > 0.3:  # Phase takes > 30% of time
            bottlenecks.append(phase)
    
    if bottlenecks:
        print(f"\nPerformance bottlenecks: {', '.join(bottlenecks)}")
    
    return phase_times
```

## 5. Advanced Integration Patterns

### Custom Sensory Processing

```python
def custom_sensory_processor(world, step):
    """Custom sensory processing with feature extraction"""
    
    # Get raw percept
    percept = world.getSensoryPercept()
    
    # Extract custom features
    features = {}
    
    # Vision features
    vision = percept.getVision()
    if vision:
        # Compute image statistics
        features['vision_mean'] = sum(vision) / len(vision)
        features['vision_variance'] = sum((x - features['vision_mean'])**2 for x in vision) / len(vision)
        features['vision_edges'] = count_edges(vision, 16, 16)
    
    # Touch features
    touch = percept.getTouch()
    if touch:
        features['touch_max'] = max(touch)
        features['touch_active_sensors'] = sum(1 for t in touch if t > 0)
    
    # Internal state features
    internal = percept.getInternal()
    if internal:
        features['energy_level'] = internal[0] if len(internal) > 0 else 0.0
        features['health_status'] = internal[1] if len(internal) > 1 else 0.0
    
    # Proprioception features
    proprio = percept.getProprioception()
    if proprio:
        features['velocity_magnitude'] = (proprio[0]**2 + proprio[1]**2)**0.5
        features['angular_velocity'] = proprio[2] if len(proprio) > 2 else 0.0
    
    # Add custom timestamp
    features['simulation_step'] = step
    
    return features

def count_edges(vision_grid, width, height):
    """Count edges in vision grid"""
    edges = 0
    for y in range(height):
        for x in range(width):
            idx = y * width + x
            if idx >= len(vision_grid):
                continue
            
            # Check neighboring cells
            if x > 0 and abs(vision_grid[idx] - vision_grid[idx-1]) > 0.1:
                edges += 1
            if y > 0 and abs(vision_grid[idx] - vision_grid[idx-width]) > 0.1:
                edges += 1
    
    return edges
```

### External Data Integration

```python
def integrate_external_data(brain, agent, step, external_data):
    """Integrate external data into brain simulation"""
    
    # Create custom sensory input from external data
    custom_percept = pynlm.SensoryPercept()
    
    # Vision from external camera/feed
    if 'vision_data' in external_data:
        vision_data = external_data['vision_data']
        # Process camera feed to vision format
        processed_vision = preprocess_vision_feed(vision_data)
        custom_percept.setVision(processed_vision)
    
    # Sound data integration
    if 'audio_data' in external_data:
        audio_data = external_data['audio_data']
        processed_audio = process_audio_feed(audio_data)
        custom_percept.setAudio(processed_audio)
    
    # Sensor data integration
    if 'sensor_readings' in external_data:
        sensor_data = external_data['sensor_readings']
        # Map sensor data to internal signals
        internal_signals = map_sensors_to_internal(sensor_data)
        custom_percept.setInternal(internal_signals)
    
    # Time-varying patterns
    if 'time_series_data' in external_data:
        time_series = external_data['time_series_data']
        temporal_patterns = generate_temporal_patterns(time_series)
        # Store as internal state
        custom_percept.setInternal(list(temporal_patterns.values()))
    
    # Inject custom percept into brain
    if agent:
        agent.processSensoryInput(custom_percept)
    
    return custom_percept

# Example integration with real-time data
def real_time_data_integration(brain, agent, world, data_providers, duration):
    """Integrate multiple real-time data sources"""
    
    import time
    import threading
    
    # Data buffer for each source
    data_buffers = {name: [] for name in data_providers}
    
    # Background threads to collect data
    def collect_data(data_name, provider):
        while not stop_collection:
            try:
                data = provider.get_data()
                data_buffers[data_name].append(data)
                time.sleep(0.01)  # 10ms sampling
            except:
                time.sleep(0.1)  # Retry after 100ms
    
    stop_collection = False
    threads = []
    
    for name, provider in data_providers.items():
        thread = threading.Thread(target=collect_data, args=(name, provider))
        thread.daemon = True
        thread.start()
        threads.append(thread)
    
    # Main simulation loop
    for step in range(duration):
        # Collect available data
        external_data = {}
        for name, buffer in data_buffers.items():
            if buffer:
                external_data[name] = buffer.pop(0)
        
        # Integrate with NLM
        if external_data:
            integrate_external_data(brain, agent, step, external_data)
        
        # Continue normal simulation
        world.update(0.1)
        
        if agent:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        time.sleep(0.1)  # Simulation timestep
    
    stop_collection = True
    for thread in threads:
        thread.join()
```

## 6. Customization for Advanced Users

### Extending Agent Behavior

```python
class AdvancedAgent:
    """Advanced agent with custom behavior strategies"""
    
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.agent_brain = pynlm.createAgentBrain(brain)
        self.agent_brain.initialize(world)
        
        # Custom state
        self.strategy = 'exploration'  # exploration, exploitation, balanced
        self.memory_buffer = []
        self.reward_history = []
        self.action_history = []
    
    def update(self, step):
        """Update agent with custom behavior logic"""
        
        # Get current perception
        percept = self.world.getSensoryPercept()
        
        # Apply custom preprocessing
        processed_percept = self.preprocess_perception(percept, step)
        
        # Process with agent brain
        self.agent_brain.processSensoryInput(processed_percept)
        self.brain.step(step)
        
        # Select action based on current strategy
        action = self.select_action(step)
        
        # Execute action
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Apply learning
        reward = self.calculate_reward(action, step)
        self.agent_brain.applyRewardModulation(reward, 0.0)
        
        # Update development
        self.agent_brain.updateDevelopment(0.1)
        
        # Store experience
        self.store_experience(percept, action, reward, step)
        
        return action
    
    def preprocess_perception(self, percept, step):
        """Custom perception preprocessing"""
        # Modify perception based on current state
        custom_percept = pynlm.SensoryPercept()
        
        # Apply strategy-specific modifications
        if self.strategy == 'exploration':
            # Add noise to exploration
            vision = percept.getVision()
            if vision:
                import random
                noisy_vision = [v + random.uniform(-0.1, 0.1) for v in vision]
                custom_percept.setVision(noisy_vision)
        
        elif self.strategy == 'exploitation':
            # Sharpen focus on known rewards
            vision = percept.getVision()
            if vision:
                # Enhance contrast
                sharpened_vision = [2*v - 0.5 for v in vision]
                custom_percept.setVision(sharpened_vision)
        
        # Copy other sensory data
        custom_percept.setTouch(percept.getTouch())
        custom_percept.setInternal(percept.getInternal())
        custom_percept.setProprioception(percept.getProprioception())
        
        return custom_percept
    
    def select_action(self, step):
        """Custom action selection logic"""
        
        if self.strategy == 'balanced':
            # Balance exploration and exploitation
            curiosity = self.agent_brain.getCuriosityLevel()
            neuromod = self.agent_brain.getNeuromodulationLevel()
            
            if curiosity > 0.7:
                # High exploration
                return self.exploratory_action()
            elif neuromod < -0.3:
                # Conservative behavior
                return pynlm.MotorCommand.Wait
            else:
                # Normal exploitation
                return self.agent_brain.decodeMotorCommand()
        
        elif self.strategy == 'curiosity_driven':
            # Explore based on curiosity
            if self.agent_brain.getCuriosityLevel() > 0.5:
                return self.exploratory_action()
            else:
                return self.agent_brain.decodeMotorCommand()
        
        elif self.strategy == 'reward_maximizing':
            # Greedily maximize immediate reward
            return self.exploitation_action()
        
        else:
            return self.agent_brain.decodeMotorCommand()
    
    def exploratory_action(self):
        """Select exploratory action"""
        import random
        actions = list(pynlm.MotorCommand)
        # Remove Wait action for exploration
        actions.remove(pynlm.MotorCommand.Wait)
        return random.choice(actions)
    
    def exploitation_action(self):
        """Select exploitative action"""
        # Use agent's learned policy
        return self.agent_brain.decodeMotorCommand()
    
    def calculate_reward(self, action, step):
        """Calculate custom reward function"""
        
        # Get immediate reward from world
        body = self.world.getAgentBody()
        internal = self.agent_brain.getNeuromodulationLevel()
        
        # Custom reward calculation
        reward = 0.0
        
        # Base reward from action success
        if hasattr(self.world, 'getActionResult'):
            result = self.world.getActionResult()
            if result:
                reward += result.reward
        
        # Neuromodulation bonus
        reward += internal * 2.0
        
        # Strategy-specific bonuses
        if self.strategy == 'exploration':
            # Bonus for novel actions
            if action not in self.action_history[-5:]:  # Not seen in last 5 steps
                reward += 0.5
        
        elif self.strategy == 'exploitation':
            # Bonus for repeated successful actions
            if self.action_history.count(action) > 2:
                reward += 0.3
        
        return reward
    
    def store_experience(self, percept, action, reward, step):
        """Store experience for future learning"""
        
        # Store in memory buffer
        experience = {
            'perception': percept,
            'action': action,
            'reward': reward,
            'step': step,
            'curiosity': self.agent_brain.getCuriosityLevel(),
            'neuromodulation': self.agent_brain.getNeuromodulationLevel()
        }
        
        self.memory_buffer.append(experience)
        
        # Limit buffer size
        if len(self.memory_buffer) > 1000:
            self.memory_buffer.pop(0)
        
        # Update reward and action history
        self.reward_history.append(reward)
        self.action_history.append(action)
        
        # Limit history size
        if len(self.reward_history) > 100:
            self.reward_history.pop(0)
        if len(self.action_history) > 100:
            self.action_history.pop(0)
    
    def get_strategy_performance(self):
        """Get performance metrics for current strategy"""
        
        if not self.reward_history:
            return {
                'average_reward': 0.0,
                'action_distribution': {},
                'curiosity_usage': 0.0,
                'exploration_rate': 0.0
            }
        
        # Calculate metrics
        avg_reward = sum(self.reward_history) / len(self.reward_history)
        
        # Action distribution
        action_dist = {}
        for action in self.action_history:
            action_name = action.name if hasattr(action, 'name') else str(action)
            action_dist[action_name] = action_dist.get(action_name, 0) + 1
        
        # Normalize to probabilities
        total_actions = sum(action_dist.values())
        for action_name in action_dist:
            action_dist[action_name] /= total_actions
        
        return {
            'average_reward': avg_reward,
            'action_distribution': action_dist,
            'curiosity_usage': sum(1 for e in self.memory_buffer 
                                  if e['curiosity'] > 0.5) / len(self.memory_buffer),
            'exploration_rate': sum(1 for action in self.action_history 
                                   if action != pynlm.MotorCommand.Wait) / len(self.action_history)
        }
```

## Conclusion

This advanced usage guide provides comprehensive examples for working with NLM beyond basic functionality. Key takeaways:

1. **Advanced Configuration**: Customize brain architecture, plasticity, and neuromodulation
2. **Complex Scenarios**: Implement goal-directed learning and multi-agent coordination
3. **Performance Optimization**: Use checkpointing, memory optimization, and profiling
4. **Debugging Tools**: Monitor brain health and identify performance bottlenecks
5. **Integration**: Combine NLM with external data sources and custom sensory processing
6. **Customization**: Extend agent behavior with custom strategies and learning algorithms

These advanced patterns enable you to build sophisticated neural simulations tailored to your specific research or application needs.