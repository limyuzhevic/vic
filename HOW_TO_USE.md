# NLM (Neural Learning Machine) - ADVANCED HOW TO USE

This document provides comprehensive guidance on using NLM for advanced scenarios, including configuration management, experimentation, and optimization.

## 1. Advanced Configuration Management

### Configuration File Format

NLM uses a simple key=value configuration file format. Here are the advanced configuration options:

#### Core Parameters
```
# Brain configuration
brain.neuron_count = 1000                    # Total number of neurons
brain.region_count = 2                      # Number of brain regions
brain.v_thresh = -50.0                     # Neuron threshold potential (mV)
brain.v_rest = -70.0                       # Resting potential (mV)
brain.tau_mem = 20.0                       # Membrane time constant (ms)
brain.tau_ref = 2.0                        # Refractory period (ms)

# Connectivity
connection_probability = 0.1               # Probability of synaptic connections

# Initial conditions
brain.initial_weight_mean = 0.5            # Mean initial synaptic weight
brain.initial_weight_std = 0.1             # Initial weight standard deviation
```

#### Plasticity Parameters
```
# STDP (Spike-Timing Dependent Plasticity)
plasticity.stdp.enable = true              # Enable STDP
plasticity.stdp.learning_rate = 0.01       # STDP learning rate
plasticity.stdp.tau_plus = 20.0            # LTP time constant
plasticity.stdp.tau_minus = 20.0           # LTD time constant
plasticity.stdp.max_weight = 1.0           # Maximum synaptic weight
plasticity.stdp.min_weight = -1.0          # Minimum synaptic weight

# Hebbian learning
plasticity.hebbian.enable = true           # Enable Hebbian learning
plasticity.hebbian.learning_rate = 0.001   # Hebbian learning rate

# Structural plasticity
plasticity.structural.enable = true        # Enable structural plasticity
synaptogenesis_rate = 0.0001               # Rate of new synapse formation
pruning_rate = 0.00001                     # Rate of synapse elimination
```

#### Neuromodulation Parameters
```
# Dopamine neuromodulation
neuromod.dopamine.baseline = 0.1           # Baseline dopamine level
neuromod.dopamine.scale = 1.0              # Dopamine modulation scale
neuromod.dopamine.decay_rate = 0.1        # Dopamine decay rate

# Curiosity neuromodulation
neuromod.curiosity.enable = true           # Enable curiosity
curiosity_threshold = 0.3                  # Threshold for curiosity activation
noise_level = 0.1                          # Exploration noise level

# Novelty detection
neuromod.novelty.enable = true             # Enable novelty detection
novelty_threshold = 0.5                    # Novelty detection threshold

# Prediction error
neuromod.prediction.error.scale = 0.5      # Prediction error scaling
```

#### Memory System Parameters
```
# Working memory
memory.working.capacity = 100              # Working memory capacity (neurons)
memory.working.consolidation_threshold = 0.7  # Consolidation threshold

# Episodic memory
memory.episodic.max_episodes = 1000        # Maximum episodes to store
memory.episodic.replay_interval = 100      # Replay frequency (steps)
memory.episodic.consolidation_rate = 0.3   # Consolidation rate

# Associative memory
memory.associative.threshold = 0.5         # Pattern matching threshold
```

#### Cognition System Parameters
```
# Planning
cognition.planner.planning_depth = 5       # Planning horizon depth
cognition.planner.exploration_rate = 0.1   # Exploration rate in planning

# Concept formation
cognition.concept.min_pattern_count = 3     # Minimum patterns for concept
cognition.concept.similarity_threshold = 0.8  # Pattern similarity threshold

# Attention
cognition.attention.inhibition_strength = 0.5  # Inhibitory strength
cognition.attention.excitation_strength = 1.5   # Excitatory strength
cognition.attention.focus_decay_rate = 0.95      # Attention decay rate
```

#### Development System Parameters
```
# Developmental stages
development.stage.initial.plasticity_rate = 1.0      # Initial plasticity rate
development.stage.critical_period.plasticity_rate = 0.8  # Critical period rate
development.stage.maturation.plasticity_rate = 0.5   # Maturation rate
development.stage.adult.plasticity_rate = 0.2       # Adult plasticity rate
```

#### Agent Parameters
```
# Agent behavior
agent.curiosity_threshold = 0.3            # Threshold for curiosity activation
agent.noise_level = 0.1                    # Action selection noise level
agent.action_selection.temperature = 1.0   # Temperature for action selection
```

#### World Parameters
```
# Environment
world.width = 20.0                         # World width
world.height = 20.0                        # World height
world.vision_width = 8                     # Visual field width
world.vision_height = 8                    # Visual field height
world.max_energy = 100.0                   # Maximum agent energy
world.energy_decay_rate = 0.01             # Energy decay rate

# Objects
world.resource_energy_gain = 30.0          # Energy from resources
world.hazard_energy_loss = 15.0            # Energy loss from hazards
```

## 2. Configuration File Creation and Management

### Creating Configuration Files

#### Method 1: Command Line
```bash
# Create configuration with defaults
./nlm --config configs/default.cfg

# Create custom configuration
./nlm --config my_config.cfg

# Override specific parameters
./nlm --brain.neuron_count=2000 --connection_probability=0.2
```

#### Method 2: Python API
```python
import pynlm

# Create configuration programmatically
config = pynlm.createDefaultConfig()

# Set parameters
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.learning_rate", 0.02)

# Save to file
config.saveToFile("custom_config.cfg")

# Load configuration
loaded_config = pynlm.createDefaultConfig()
if loaded_config.loadFromFile("custom_config.cfg"):
    print("Configuration loaded successfully")
```

#### Method 3: Interactive Configuration
```python
def interactive_config():
    """Interactive configuration creation"""
    config = pynlm.createDefaultConfig()
    
    print("=== NLM Configuration Setup ===")
    
    # Brain parameters
    neuron_count = int(input("Number of neurons [1000]: ") or "1000")
    config.set("brain.neuron_count", neuron_count)
    
    region_count = int(input("Number of regions [2]: ") or "2")
    config.set("region_count", region_count)
    
    # Plasticity
    stdp_rate = float(input("STDP learning rate [0.01]: ") or "0.01")
    config.set("plasticity.stdp.learning_rate", stdp_rate)
    
    # Memory
    mem_capacity = int(input("Working memory capacity [100]: ") or "100")
    config.set("memory.working.capacity", mem_capacity)
    
    # Save
    filename = input("Configuration filename [custom.cfg]: ") or "custom.cfg"
    config.saveToFile(filename)
    
    print(f"Configuration saved to {filename}")
    return config

# Usage
config = interactive_config()
```

### Configuration Validation

#### Validate Configuration
```python
import pynlm

def validate_config(config):
    """Validate configuration parameters"""
    
    errors = []
    warnings = []
    
    # Check required parameters
    if not config.has("brain.neuron_count"):
        errors.append("Missing brain.neuron_count")
    
    if not config.has("region_count"):
        errors.append("Missing region_count")
    
    # Validate parameter ranges
    neuron_count = config.getOr("brain.neuron_count", 1000)
    if neuron_count < 10:
        warnings.append("Very low neuron count may affect performance")
    elif neuron_count > 10000:
        warnings.append("Very high neuron count may be slow to simulate")
    
    region_count = config.getOr("region_count", 1)
    if region_count < 1 or region_count > 100:
        errors.append("region_count must be between 1 and 100")
    
    # Check plasticity parameters
    stdp_rate = config.getOr("plasticity.stdp.learning_rate", 0.01)
    if stdp_rate < 0.0 or stdp_rate > 1.0:
        errors.append("plasticity.stdp.learning_rate must be between 0.0 and 1.0")
    
    # Check memory parameters
    mem_capacity = config.getOr("memory.working.capacity", 100)
    if mem_capacity < 1:
        errors.append("memory.working.capacity must be positive")
    
    # Report results
    if errors:
        print("Configuration errors:")
        for error in errors:
            print(f"  - {error}")
        return False
    
    if warnings:
        print("Configuration warnings:")
        for warning in warnings:
            print(f"  - {warning}")
    
    print("Configuration is valid")
    return True

# Usage
config = pynlm.createDefaultConfig()
if validate_config(config):
    brain = pynlm.createBrain(config)
    brain.initialize()
```

## 3. Advanced Simulation Techniques

### Event-Driven Simulation
```python
import pynlm
import time

def event_driven_simulation(brain, world, agent, max_steps):
    """Event-driven simulation with spike timing precision"""
    
    print("=== Event-Driven Simulation ===")
    
    start_time = time.time()
    step_times = []
    
    for step in range(max_steps):
        step_start = time.time()
        
        # Update world with precise timing
        world.update(0.001)  # 1ms timestep
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain with event-driven spike propagation
        brain.step(step, time.time() * 1000)  # Pass absolute time in ms
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        # Record step time
        step_time = time.time() - step_start
        step_times.append(step_time)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            avg_step_time = sum(step_times[-100:]) / len(step_times[-100:])
            print(f"Step {step}: {avg_step_time*1000:.2f}ms per step")
    
    total_time = time.time() - start_time
    steps_per_second = max_steps / total_time
    
    print(f"\nSimulation completed in {total_time:.2f}s")
    print(f"Performance: {steps_per_second:.0f} steps/second")
    
    return total_time, steps_per_second
```

### Parallel Processing
```python
import pynlm
import threading
import multiprocessing

def parallel_simulation(brain, world, agent, steps_per_thread, num_threads):
    """Parallel simulation using multiple threads"""
    
    print(f"=== Parallel Simulation ({num_threads} threads) ===")
    
    results = []
    lock = threading.Lock()
    
    def simulate_thread(thread_id, start_step, end_step):
        """Simulate steps for a single thread"""
        thread_results = {
            'thread_id': thread_id,
            'start_step': start_step,
            'end_step': end_step,
            'total_reward': 0.0,
            'spike_count': 0
        }
        
        for step in range(start_step, end_step):
            # Update world
            world.update(0.001)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step, step * 0.001)
            
            # Get action and apply
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect metrics
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            thread_results['total_reward'] += reward
            thread_results['spike_count'] += brain.getTotalSpikeCount()
        
        with lock:
            results.append(thread_results)
    
    # Create and start threads
    threads = []
    steps_per_thread = steps_per_thread // num_threads
    
    for i in range(num_threads):
        start_step = i * steps_per_thread
        end_step = start_step + steps_per_thread
        if i == num_threads - 1:
            end_step = steps_per_thread * num_threads
        
        thread = threading.Thread(target=simulate_thread, args=(i, start_step, end_step))
        threads.append(thread)
        thread.start()
    
    # Wait for all threads to complete
    for thread in threads:
        thread.join()
    
    # Aggregate results
    total_reward = sum(r['total_reward'] for r in results)
    total_spikes = sum(r['spike_count'] for r in results)
    
    print(f"Parallel simulation completed")
    print(f"Total reward: {total_reward:.2f}")
    print(f"Total spikes: {total_spikes}")
    
    return results
```

### Multi-Agent Simulation
```python
import pynlm

def multi_agent_simulation(num_agents, steps, shared_brain=None):
    """Simulate multiple agents sharing a brain"""
    
    print(f"=== Multi-Agent Simulation ({num_agents} agents) ===")
    
    # Create shared brain if not provided
    if shared_brain is None:
        config = pynlm.createDefaultConfig()
        shared_brain = pynlm.createBrain(config)
        shared_brain.initialize()
    
    # Create agents
    agents = []
    worlds = []
    
    for i in range(num_agents):
        # Create unique world for each agent
        world = pynlm.createSimpleWorld()
        world.setRandomSeed(42 + i)  # Different seed for each world
        
        # Create agent
        agent = pynlm.createAgentBrain(shared_brain)
        agent.initialize(world)
        
        # Configure agent with unique characteristics
        agent.setCuriosityThreshold(0.3 + (i * 0.05))  # Varying curiosity
        agent.setExplorationNoise(0.1 + (i * 0.05))   # Varying exploration
        
        agents.append(agent)
        worlds.append(world)
    
    # Run simulation
    agent_rewards = [0.0 for _ in range(num_agents)]
    agent_spikes = [0 for _ in range(num_agents)]
    
    for step in range(steps):
        # Each agent operates in its own world
        for i in range(num_agents):
            # Update world
            worlds[i].update(0.001)
            
            # Get sensory input
            percept = worlds[i].getSensoryPercept()
            
            # Process sensory input
            agents[i].processSensoryInput(percept)
            
            # Run brain step (shared brain)
            shared_brain.step(step, step * 0.001)
            
            # Get action and apply
            action = agents[i].decodeMotorCommand()
            worlds[i].applyMotorCommand(action, worlds[i].getSimulationTime())
            
            # Apply reward modulation
            reward = worlds[i].getSensoryPercept().getInternal()[0] if worlds[i].getSensoryPercept().getInternal() else 0.0
            agents[i].applyRewardModulation(reward, 0.0)
            
            # Update development
            agents[i].updateDevelopment(0.001)
            
            # Collect metrics
            agent_rewards[i] += reward
            agent_spikes[i] += shared_brain.getTotalSpikeCount()
    
    # Calculate averages
    avg_rewards = [r / steps for r in agent_rewards]
    avg_spikes = [s / steps for s in agent_spikes]
    
    print(f"Multi-agent simulation completed")
    for i in range(num_agents):
        print(f"Agent {i+1}: avg reward={avg_rewards[i]:.3f}, avg spikes={avg_spikes[i]:.1f}")
    
    return {
        'agents': agents,
        'worlds': worlds,
        'brain': shared_brain,
        'rewards': avg_rewards,
        'spikes': avg_spikes
    }
```

## 4. Performance Optimization

### Memory Optimization
```python
import pynlm

def optimize_memory_usage(brain):
    """Optimize memory usage for larger simulations"""
    
    print("=== Memory Optimization ===")
    
    # Get current memory usage
    total_neurons = brain.getTotalNeuronCount()
    total_synapses = brain.getTotalSynapseCount()
    
    print(f"Current memory usage:")
    print(f"  Neurons: {total_neurons}")
    print(f"  Synapses: {total_synapses}")
    
    # Suggest optimizations
    if total_neurons > 2000:
        print("\nRecommendations for memory optimization:")
        print("  1. Reduce brain.neuron_count")
        print("  2. Disable structural plasticity (plasticity.structural.enable = false)")
        print("  3. Reduce working memory capacity")
        print("  4. Use sparse connectivity (reduce connection_probability)")
    
    # Optimize working memory
    if brain.getWorkingMemory():
        current_capacity = brain.getWorkingMemory().getCapacity()
        suggested_capacity = max(50, total_neurons // 20)  # 5% of neurons
        
        if current_capacity > suggested_capacity:
            brain.getWorkingMemory().setCapacity(suggested_capacity)
            print(f"  Reduced working memory capacity to {suggested_capacity}")
    
    # Optimize episodic memory
    if brain.getEpisodicMemory():
        current_max = brain.getEpisodicMemory().getMaxEpisodes()
        suggested_max = max(200, total_neurons // 10)  # 10% of neurons
        
        if current_max > suggested_max:
            brain.getEpisodicMemory().setMaxEpisodes(suggested_max)
            print(f"  Reduced episodic memory capacity to {suggested_max}")
    
    return brain
```

### Speed Optimization
```python
import pynlm

def optimize_speed(brain, config):
    """Optimize simulation speed"""
    
    print("=== Speed Optimization ===")
    
    current_neurons = brain.getTotalNeuronCount()
    print(f"Current neuron count: {current_neurons}")
    
    # Suggest speed optimizations
    if current_neurons > 1000:
        print("\nRecommendations for speed optimization:")
        
        # Check current timestep
        timestep = config.getOr("simulation_timestep", 0.001)
        if timestep < 0.001:
            print("  1. Increase simulation_timestep (currently 1ms)")
        
        # Check plasticity
        if config.getOr("plasticity.structural.enable", True):
            print("  2. Disable structural plasticity for faster simulation")
        
        if config.getOr("plasticity.stdp.enable", True):
            print("  3. Reduce STDP frequency or disable")
        
        # Check memory systems
        if config.getOr("memory.episodic.max_episodes", 1000) > 500:
            print("  4. Reduce episodic memory capacity")
        
        # Check development
        if config.getOr("development.enabled", True):
            print("  5. Disable development system for speed")
    
    # Apply speed optimizations
    if current_neurons > 1500:
        # Reduce plasticity
        config.set("plasticity.structural.enable", False)
        config.set("plasticity.hebbian.learning_rate", 0.0001)
        
        # Reduce memory
        config.set("memory.working.capacity", 50)
        config.set("memory.episodic.max_episodes", 200)
        
        print("\nApplied speed optimizations")
    
    return config
```

### Batch Processing
```python
import pynlm
import time

def batch_simulation(configurations, steps_per_config, output_prefix="batch"):
    """Run multiple simulations with different configurations"""
    
    print(f"=== Batch Simulation ({len(configurations)} configurations) ===")
    
    results = []
    
    for i, config in enumerate(configurations):
        print(f"\nRunning configuration {i+1}/{len(configurations)}...")
        
        # Create brain with current configuration
        brain = pynlm.createBrain(config)
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        
        # Initialize
        brain.initialize()
        agent.initialize(world)
        
        # Run simulation
        start_time = time.time()
        
        total_reward = 0.0
        max_firing_rate = 0.0
        final_spikes = 0
        
        for step in range(steps_per_config):
            # Update world
            world.update(0.001)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Collect metrics
            total_reward += reward
            max_firing_rate = max(max_firing_rate, brain.getAverageFiringRate())
            final_spikes = brain.getTotalSpikeCount()
        
        # Calculate metrics
        end_time = time.time()
        duration = end_time - start_time
        steps_per_second = steps_per_config / duration
        avg_reward = total_reward / steps_per_config
        
        # Save configuration
        config_filename = f"{output_prefix}_config_{i+1}.cfg"
        config.saveToFile(config_filename)
        
        # Save results
        result = {
            'config_index': i,
            'config_file': config_filename,
            'duration': duration,
            'steps_per_second': steps_per_second,
            'average_reward': avg_reward,
            'max_firing_rate': max_firing_rate,
            'total_spikes': final_spikes,
            'steps': steps_per_config
        }
        
        results.append(result)
        
        print(f"  Completed in {duration:.2f}s ({steps_per_second:.0f} steps/sec)")
        print(f"  Average reward: {avg_reward:.3f}")
        print(f"  Max firing rate: {max_firing_rate:.3f}")
        print(f"  Total spikes: {final_spikes}")
        print(f"  Configuration saved to: {config_filename}")
    
    # Summarize results
    print(f"\n=== Batch Simulation Summary ===")
    for i, result in enumerate(results):
        print(f"Config {i+1}: {result['average_reward']:.3f} reward, {result['steps_per_second']:.0f} steps/sec")
    
    return results
```

## 5. Advanced Features and Extensions

### Custom Learning Rules
```python
import pynlm

class CustomLearningRule:
    """Custom learning rule implementation"""
    
    def __init__(self, learning_rate=0.01, threshold=0.5):
        self.learning_rate = learning_rate
        self.threshold = threshold
    
    def apply_rule(self, synapse, pre_spike_time, post_spike_time):
        """Apply custom learning rule"""
        time_diff = post_spike_time - pre_spike_time
        
        if abs(time_diff) < self.threshold:
            # Symmetric Hebbian-like rule
            weight_change = self.learning_rate * (1.0 - abs(time_diff) / self.threshold)
        else:
            # STDP-like rule with custom parameters
            if time_diff > 0:  # Pre before post (LTP)
                weight_change = self.learning_rate * 0.5 * math.exp(-abs(time_diff) / 20.0)
            else:  # Post before pre (LTD)
                weight_change = -self.learning_rate * 0.5 * math.exp(-abs(time_diff) / 20.0)
        
        # Apply weight change
        new_weight = synapse.getWeight() + weight_change
        new_weight = max(-1.0, min(1.0, new_weight))  # Clamp to [-1, 1]
        synapse.setWeight(new_weight)
        
        return weight_change

# Usage
import math

custom_rule = CustomLearningRule(learning_rate=0.02, threshold=1.0)

# Create brain with custom rule
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Manually apply custom learning rule
# This would typically be integrated into the brain's plasticity system
for region in brain.getRegions():
    for synapse in region.getSynapses():
        # Example: Apply custom rule when spikes occur
        # In practice, this would be integrated with the spike handling system
        pass
```

### Custom Environment
```python
import pynlm

class CustomEnvironment:
    """Custom environment implementation"""
    
    def __init__(self, width=20, height=20):
        self.width = width
        self.height = height
        self.agent = None
        self.objects = []
        self.time = 0.0
    
    def initialize(self):
        """Initialize environment"""
        # Create agent
        self.agent = Agent()
        self.agent.x = self.width / 2
        self.agent.y = self.height / 2
        self.agent.energy = 100.0
        
        # Place objects
        self.place_resources(5, 30.0)
        self.place_hazards(2, -20.0)
    
    def place_resources(self, count, energy_value):
        """Place resource objects"""
        for _ in range(count):
            x = random.uniform(1, self.width - 1)
            y = random.uniform(1, self.height - 1)
            self.objects.append({
                'x': x,
                'y': y,
                'type': 'resource',
                'value': energy_value
            })
    
    def place_hazards(self, count, energy_cost):
        """Place hazard objects"""
        for _ in range(count):
            x = random.uniform(1, self.width - 1)
            y = random.uniform(1, self.height - 1)
            self.objects.append({
                'x': x,
                'y': y,
                'type': 'hazard',
                'value': energy_cost
            })
    
    def update(self, timestep):
        """Update environment"""
        self.time += timestep
        
        # Update agent movement
        self.agent.x += self.agent.velocity_x * timestep
        self.agent.y += self.agent.velocity_y * timestep
        
        # Handle boundary collisions
        self.handle_boundaries()
        
        # Check for object interactions
        self.check_collisions()
        
        # Update object states
        self.update_objects(timestep)
        
        # Decay energy
        self.agent.energy *= (1.0 - self.energy_decay_rate * timestep)
    
    def handle_boundaries(self):
        """Handle boundary collisions"""
        if self.agent.x < 0:
            self.agent.x = 0
            self.agent.velocity_x = 0
        elif self.agent.x >= self.width:
            self.agent.x = self.width - 0.01
            self.agent.velocity_x = 0
        
        if self.agent.y < 0:
            self.agent.y = 0
            self.agent.velocity_y = 0
        elif self.agent.y >= self.height:
            self.agent.y = self.height - 0.01
            self.agent.velocity_y = 0
    
    def check_collisions(self):
        """Check for agent-object collisions"""
        for obj in self.objects:
            distance = math.sqrt((self.agent.x - obj['x']) ** 2 + 
                               (self.agent.y - obj['y']) ** 2)
            
            if distance < 1.0:  # Collision radius
                # Apply object effect
                if obj['type'] == 'resource':
                    self.agent.energy = min(self.agent.energy + obj['value'], 100.0)
                elif obj['type'] == 'hazard':
                    self.agent.energy += obj['value']
                
                # Remove object
                self.objects.remove(obj)
    
    def update_objects(self, timestep):
        """Update object states (e.g., regeneration)"""
        for obj in self.objects:
            if obj['type'] == 'resource' and obj.get('regenerating', False):
                obj['value'] *= (1.0 + timestep * 0.01)
    
    def get_sensory_percept(self):
        """Get sensory percept for agent"""
        percept = []
        
        # Add vision information
        percept.extend(self.get_vision_percept())
        
        # Add energy information
        percept.append(self.agent.energy / 100.0)
        
        # Add time information
        percept.append(self.time / 1000.0)
        
        return percept
    
    def get_vision_percept(self):
        """Get visual information"""
        percept = []
        vision_range = 5
        
        for dx in range(-vision_range, vision_range + 1):
            for dy in range(-vision_range, vision_range + 1):
                x = self.agent.x + dx
                y = self.agent.y + dy
                
                if 0 <= x < self.width and 0 <= y < self.height:
                    # Find object at this position
                    obj = None
                    for o in self.objects:
                        if abs(o['x'] - x) < 0.5 and abs(o['y'] - y) < 0.5:
                            obj = o
                            break
                    
                    if obj:
                        if obj['type'] == 'resource':
                            percept.append(1.0)  # Resource present
                        elif obj['type'] == 'hazard':
                            percept.append(-1.0)  # Hazard present
                        else:
                            percept.append(0.0)  # Empty
                    else:
                        percept.append(0.0)  # Empty
                else:
                    percept.append(-1.0)  # Out of bounds
        
        return percept

# Usage
import random, math

def custom_environment_simulation():
    """Simulation using custom environment"""
    
    # Create custom environment
    env = CustomEnvironment(width=30, height=30)
    env.initialize()
    
    # Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        # Update environment
        env.update(0.01)
        
        # Get sensory percept from custom environment
        percept = env.get_sensory_percept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action to environment
        if action.getType() == pynlm.ActionType::MoveForward:
            env.agent.velocity_x += 0.1
        elif action.getType() == pynlm.ActionType::TurnLeft:
            env.agent.orientation -= 0.1
        elif action.getType() == pynlm.ActionType::TurnRight:
            env.agent.orientation += 0.1
        
        # Print status periodically
        if step % 100 == 0:
            print(f"Step {step}: Position=({env.agent.x:.1f}, {env.agent.y:.1f}), "
                  f"Energy={env.agent.energy:.1f}, Objects={len(env.objects)}")
    
    return env, brain, agent
```

## 6. Debugging and Troubleshooting

### Debug Tools
```python
import pynlm

def debug_brain(brain):
    """Debug brain state and statistics"""
    
    print("=== Brain Debug Information ===")
    
    # Basic statistics
    print(f"Total neurons: {brain.getTotalNeuronCount()}")
    print(f"Total synapses: {brain.getTotalSynapseCount()}")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Memory systems
    if brain.getWorkingMemory():
        wm = brain.getWorkingMemory()
        print(f"\nWorking Memory:")
        print(f"  Capacity: {wm.getCapacity()}")
        print(f"  Active traces: {wm.getActiveTraces()}")
    
    if brain.getEpisodicMemory():
        em = brain.getEpisodicMemory()
        print(f"\nEpisodic Memory:")
        print(f"  Max episodes: {em.getMaxEpisodes()}")
        print(f"  Current episodes: {em.getEpisodeCount()}")
    
    # Neuromodulation
    if brain.getDopamine():
        print(f"\nNeuromodulation:")
        print(f"  Dopamine level: {brain.getDopamine().getLevel():.3f}")
    
    if brain.getCuriosity():
        print(f"  Curiosity level: {brain.getCuriosity().getLevel():.3f}")
    
    # Development
    print(f"\nDevelopment:")
    print(f"  Stage: {brain.getDevelopmentalStage()}")
    
    # Check for potential issues
    print(f"\nPotential Issues:")
    if brain.getTotalSpikeCount() == 0:
        print("  - No spikes detected (brain may not be active)")
    
    if brain.getFiringNeuronCount() == 0:
        print("  - No neurons currently firing")
    
    if brain.getAverageFiringRate() < 0.1:
        print("  - Low firing rate may indicate poor connectivity")
    
    # Check region statistics
    print(f"\nRegion Statistics:")
    for region in brain.getRegions():
        neurons = region.getTotalNeuronCount()
        synapses = region.getSynapseCount()
        avg_weight = region.getAverageSynapticWeight()
        print(f"  Region {region.getId().index()}: {neurons} neurons, "
              f"{synapses} synapses, avg weight={avg_weight:.3f}")

def debug_simulation(brain, world, agent, steps=100):
    """Debug simulation step-by-step"""
    
    print("=== Simulation Debug ===")
    
    for step in range(steps):
        print(f"\n--- Step {step} ---")
        
        # Update world
        world.update(0.001)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        print(f"Sensory percept (first 5 values): {percept[:5]}")
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Get brain state before step
        neurons_before = brain.getActiveNeuronCount()
        firing_before = brain.getFiringNeuronCount()
        spikes_before = brain.getTotalSpikeCount()
        
        # Run brain step
        brain.step(step)
        
        # Get brain state after step
        neurons_after = brain.getActiveNeuronCount()
        firing_after = brain.getFiringNeuronCount()
        spikes_after = brain.getTotalSpikeCount()
        
        # Get action
        action = agent.decodeMotorCommand()
        print(f"Action: {action.getType()}")
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print changes
        print(f"Neurons: {neurons_before} -> {neurons_after}")
        print(f"Firing: {firing_before} -> {firing_after}")
        print(f"Spikes: {spikes_before} -> {spikes_after}")
        
        # Check for issues
        if firing_after == 0:
            print("  WARNING: No neurons firing")
        
        if world.getSensoryPercept().getInternal() and world.getSensoryPercept().getInternal()[0] > 0:
            print(f"  Reward received: {world.getSensoryPercept().getInternal()[0]}")
        
        # Pause for user input
        if step % 10 == 9:
            input("Press Enter to continue...")
```

### Common Debug Scripts
```python
import pynlm

def quick_test():
    """Quick test to verify basic functionality"""
    
    print("=== Quick NLM Test ===")
    
    # Create minimal configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Test basic stepping
    for step in range(100):
        brain.step(step)
    
    print(f"After 100 steps:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    
    # Test memory systems
    if brain.getWorkingMemory():
        print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces()}")
    
    # Test episodic memory
    if brain.getEpisodicMemory():
        print(f"  Episodic memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")
    
    print("Quick test completed successfully!")

def connectivity_test():
    """Test neural connectivity"""
    
    print("=== Connectivity Test ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Inject current into sensory neurons
    for i in range(10):
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 50.0f)
    
    # Run steps and check for propagation
    spikes_detected = 0
    for step in range(50):
        brain.step(step)
        if brain.getTotalSpikeCount() > spikes_detected:
            spikes_detected = brain.getTotalSpikeCount()
            print(f"Step {step}: {spikes_detected} spikes detected")
    
    if spikes_detected > 0:
        print("✓ Neural connectivity working")
    else:
        print("✗ No spikes detected - connectivity issue")

def plasticity_test():
    """Test plasticity mechanisms"""
    
    print("=== Plasticity Test ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Get initial weights
    if brain.getRegion(pynlm.RegionId(1)):
        region = brain.getRegion(pynlm.RegionId(1))
        if region and region.getSynapses().size() > 0:
            auto synapses = region.getSynapses()
            if synapses.size() > 0:
                float initial_weight = synapses[0].getWeight()
                print(f"Initial synapse weight: {initial_weight:.3f}")
                
                # Apply correlated activity to trigger STDP
                # This is a simplified test - real STDP requires precise timing
                print("  (Note: Full STDP testing requires precise spike timing)")
                print("  Consider using main.cpp for complete plasticity tests")

def save_checkpoint_test(brain, filename):
    """Test checkpoint save/load functionality"""
    
    print(f"=== Checkpoint Test ===")
    
    # Save checkpoint
    if brain.save(filename):
        print("✓ Checkpoint saved successfully")
    else:
        print("✗ Failed to save checkpoint")
        return
    
    # Create new brain and load
    config = pynlm.createDefaultConfig()
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load(filename):
        print("✓ Checkpoint loaded successfully")
        
        # Compare spike counts
        spikes1 = brain.getTotalSpikeCount()
        spikes2 = brain2.getTotalSpikeCount()
        
        print(f"Original brain spikes: {spikes1}")
        print(f"Loaded brain spikes: {spikes2}")
        
        if spikes1 == spikes2:
            print("✓ Checkpoint restore accurate")
        else:
            print("✗ Checkpoint restore has differences (expected for neural simulation)")
    else:
        print("✗ Failed to load checkpoint")

# Usage examples
if __name__ == "__main__":
    # Run quick tests
    quick_test()
    connectivity_test()
    plasticity_test()
    
    # Test checkpointing
    save_checkpoint_test(brain, "/tmp/test_checkpoint.bin")
```

## 7. Performance Monitoring

### System Monitoring
```python
import pynlm
import time
import psutil
import os

def monitor_system_resources():
    """Monitor system resources during simulation"""
    
    process = psutil.Process(os.getpid())
    
    print("=== System Resource Monitor ===")
    print(f"Process ID: {os.getpid()}")
    print(f"Process name: {process.name()}")
    
    # Monitor for 30 seconds
    start_time = time.time()
    samples = []
    
    print("\nMonitoring system resources (30 seconds)...")
    print("Time(s)	CPU(%)	Memory(MB)	Neurons	Spikes")
    
    while time.time() - start_time < 30:
        # Get system resources
        cpu_percent = process.cpu_percent()
        memory_mb = process.memory_info().rss / (1024 * 1024)
        
        # Get brain statistics (if brain is active)
        # This would require brain to be accessible
        # neurons = brain.getTotalNeuronCount() if brain else 0
        # spikes = brain.getTotalSpikeCount() if brain else 0
        neurons = 0  # Placeholder
        spikes = 0   # Placeholder
        
        samples.append({
            'time': time.time() - start_time,
            'cpu': cpu_percent,
            'memory': memory_mb,
            'neurons': neurons,
            'spikes': spikes
        })
        
        # Print sample
        if samples:
            latest = samples[-1]
            print(f"{latest['time']:.1f}	{latest['cpu']:.1f}	{latest['memory']:.1f}	{latest['neurons']}	{latest['spikes']}")
        
        time.sleep(0.5)
    
    # Calculate averages
    avg_cpu = sum(s['cpu'] for s in samples) / len(samples)
    avg_memory = sum(s['memory'] for s in samples) / len(samples)
    
    print(f"\nAverages over 30 seconds:")
    print(f"  CPU usage: {avg_cpu:.1f}%")
    print(f"  Memory usage: {avg_memory:.1f} MB")
    
    # Performance recommendations
    if avg_cpu > 80:
        print(f"\nPerformance warning: High CPU usage ({avg_cpu:.1f}%)")
        print("  Consider:")
        print("    - Reducing neuron count")
        print("    - Disabling expensive plasticity rules")
        print("    - Using fewer simulation steps")
    
    if avg_memory > 1000:
        print(f"\nMemory warning: High memory usage ({avg_memory:.1f} MB)")
        print("  Consider:")
        print("    - Reducing memory system capacities")
        print("    - Disabling episodic memory replay")
        print("    - Using checkpointing to save memory")

def benchmark_simulation(brain, world, agent, duration_seconds=60):
    """Benchmark simulation performance"""
    
    print(f"=== Simulation Benchmark ({duration_seconds}s) ===")
    
    start_time = time.time()
    step_count = 0
    spike_count = 0
    
    while time.time() - start_time < duration_seconds:
        # Run simulation step
        world.update(0.001)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step_count)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect metrics
        step_count += 1
        spike_count += brain.getTotalSpikeCount()
        
        # Print progress every 10 seconds
        if step_count % 10000 == 0:
            elapsed = time.time() - start_time
            steps_per_second = step_count / elapsed
            spikes_per_second = (spike_count - prev_spike_count) / 10.0 if step_count > 10000 else 0
            prev_spike_count = spike_count
            
            print(f"Progress: {step_count} steps, "
                  f"{steps_per_second:.0f} steps/sec, "
                  f"{spikes_per_second:.0f} spikes/sec")
    
    # Calculate final metrics
    total_time = time.time() - start_time
    final_steps_per_second = step_count / total_time
    final_spikes_per_second = spike_count / total_time
    
    print(f"\nBenchmark Results:")
    print(f"  Duration: {total_time:.1f} seconds")
    print(f"  Total steps: {step_count}")
    print(f"  Steps per second: {final_steps_per_second:.0f}")
    print(f"  Total spikes: {spike_count}")
    print(f"  Spikes per second: {final_spikes_per_second:.0f}")
    
    # Performance assessment
    if final_steps_per_second > 1000:
        print(f"\n✓ Excellent performance: >1000 steps/sec")
    elif final_steps_per_second > 500:
        print(f"\n✓ Good performance: 500-1000 steps/sec")
    elif final_steps_per_second > 100:
        print(f"\n⚠ Moderate performance: 100-500 steps/sec")
    else:
        print(f"\n⚠ Low performance: <100 steps/sec")
        print("  Consider optimization:")
        print("    - Reduce neuron count")
        print("    - Disable structural plasticity")
        print("    - Use simpler environment")
    
    return {
        'duration': total_time,
        'steps_per_second': final_steps_per_second,
        'spikes_per_second': final_spikes_per_second,
        'total_steps': step_count,
        'total_spikes': spike_count
    }
```

## 8. Examples and Templates

### Template: Complex Agent Behavior
```python
import pynlm

def complex_agent_simulation(brain, world, steps):
    """Complex agent behavior with multiple strategies"""
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Configure behavior strategies
    agent.setCuriosityThreshold(0.4)
    agent.setExplorationNoise(0.15)
    agent.setActionSelectionTemperature(1.2)
    
    # State tracking
    episode_rewards = []
    curiosity_levels = []
    novelty_levels = []
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input with state awareness
        agent.processSensoryInput(percept)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        
        # Apply reward modulation with prediction
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        episode_rewards.append(reward)
        curiosity_levels.append(agent.getCuriosityLevel())
        novelty_levels.append(agent.getNoveltyLevel())
        
        # Complex decision making based on collected data
        if step % 100 == 0:
            avg_reward = sum(episode_rewards[-100:]) / min(len(episode_rewards), 100)
            avg_curiosity = sum(curiosity_levels[-100:]) / min(len(curiosity_levels), 100)
            avg_novelty = sum(novelty_levels[-100:]) / min(len(novelty_levels), 100)
            
            print(f"Step {step}: Reward={avg_reward:.3f}, "
                  f"Curiosity={avg_curiosity:.3f}, Novelty={avg_novelty:.3f}")
    
    # Calculate final metrics
    final_metrics = {
        'total_reward': sum(episode_rewards),
        'avg_reward': sum(episode_rewards) / len(episode_rewards),
        'max_reward': max(episode_rewards),
        'min_reward': min(episode_rewards),
        'avg_curiosity': sum(curiosity_levels) / len(curiosity_levels),
        'avg_novelty': sum(novelty_levels) / len(novelty_levels)
    }
    
    return final_metrics

# Usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    
    # Run simulation
    metrics = complex_agent_simulation(brain, world, 1000)
    
    print("\nFinal Metrics:")
    for key, value in metrics.items():
        print(f"  {key}: {value:.3f}")
```

### Template: Configuration-Driven Simulation
```python
import pynlm
import json

def load_config_from_file(filename):
    """Load configuration from JSON file"""
    with open(filename, 'r') as f:
        config_data = json.load(f)
    
    config = pynlm.createDefaultConfig()
    
    # Apply configuration
    for key, value in config_data.items():
        config.set(key, value)
    
    return config

def simulation_from_config(config_file, output_file=None):
    """Run simulation with configuration from file"""
    
    print(f"=== Configuration-Driven Simulation ===")
    print(f"Loading configuration from: {config_file}")
    
    # Load configuration
    config = load_config_from_file(config_file)
    
    # Create brain, world, and agent
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Initialize
    brain.initialize()
    agent.initialize(world)
    
    # Get simulation parameters
    steps = config.getOr("max_steps", 1000)
    enable_developmental = config.getOr("enable_developmental", True)
    
    # Run simulation
    results = {
        'config_file': config_file,
        'steps_completed': 0,
        'total_reward': 0.0,
        'final_spikes': 0,
        'metrics': {}
    }
    
    for step in range(steps):
        # Update world
        world.update(0.001)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if enable_developmental:
            agent.updateDevelopment(0.1)
        
        # Collect results
        results['steps_completed'] += 1
        results['total_reward'] += reward
        results['final_spikes'] = brain.getTotalSpikeCount()
        
        # Collect metrics periodically
        if step % 100 == 0:
            results['metrics'][step] = {
                'reward': reward,
                'firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel()
            }
    
    # Calculate final metrics
    results['average_reward'] = results['total_reward'] / results['steps_completed'] if results['steps_completed'] > 0 else 0
    
    # Save results
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to: {output_file}")
    
    print(f"\nSimulation completed!")
    print(f"Steps: {results['steps_completed']}")
    print(f"Total reward: {results['total_reward']:.2f}")
    print(f"Average reward: {results['average_reward']:.3f}")
    print(f"Final spikes: {results['final_spikes']}")
    
    return results

# Usage example
if __name__ == "__main__":
    # Create configuration file
    config_example = {
        "brain.neuron_count": 1000,
        "brain.v_thresh": -50.0,
        "plasticity.stdp.learning_rate": 0.01,
        "neuromod.curiosity.enable": True,
        "memory.episodic.max_episodes": 500,
        "max_steps": 500,
        "enable_developmental": True
    }
    
    with open("simulation_config.json", 'w') as f:
        json.dump(config_example, f, indent=2)
    
    # Run simulation
    results = simulation_from_config("simulation_config.json", "simulation_results.json")
```

This comprehensive advanced usage guide provides detailed documentation for experienced NLM users, covering configuration management, simulation techniques, performance optimization, and debugging tools. Use these advanced features to create sophisticated neural simulations with full control over all brain systems and parameters.

---

*This document is a work in progress. More advanced features and templates will be added in future releases.*

### Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Phase 6 integration audit

For additional support, refer to the documentation directory or join the NLM community for advanced discussions.