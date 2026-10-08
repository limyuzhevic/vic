# Advanced NLM API - User Guide

This document provides comprehensive guidance for advanced users of the NLM (Neural Learning Machine) Python API. It covers advanced usage patterns, optimization techniques, and integration strategies.

## Table of Contents

1. [Introduction](#introduction)
2. [Advanced Brain Configuration](#advanced-brain-configuration)
3. [Agent Development with Advanced Features](#agent-development-with-advanced-features)
4. [Memory System Integration](#memory-system-integration)
5. [Plasticity and Learning Optimization](#plasticity-and-learning-optimization)
6. [World Simulation Strategies](#world-simulation-strategies)
7. [Reinforcement Learning Examples](#reinforcement-learning-examples)
8. [Performance Monitoring and Profiling](#performance-monitoring-and-profiling)
9. [Batch Processing and Scalability](#batch-processing-and-scalability)
10. [Troubleshooting and Best Practices](#troubleshooting-and-best-practices)
11. [API Reference](#api-reference)

## Introduction

The NLM Python API provides access to the C++ Neural Learning Machine simulation framework. While the basic API allows for simple brain creation and simulation, this guide covers advanced techniques for leveraging the full power of the neural simulation system.

### Key Advanced Features

- **Complex brain architectures**: Multiple regions, heterogeneous neuron populations
- **Advanced plasticity rules**: STDP, Hebbian, structural plasticity with custom parameters
- **Neuromodulation systems**: Dopamine, curiosity, novelty detection
- **Memory integration**: Working memory, episodic memory, associative memory
- **Development systems**: Age-dependent plasticity, critical periods
- **Predictive processing**: Internal models and prediction error signals
- **Social learning**: Observation and imitation capabilities
- **Advanced sensory processing**: Custom feature extraction and multimodal inputs

## Advanced Brain Configuration

### Custom Brain Architectures

```python
import pynlm

# Create a brain with specialized regions
config = pynlm.createDefaultConfig()

# Primary sensory cortex (excitatory neurons)
config.set("brain.region.1.neuron_type", "Excitatory")
config.set("brain.region.1.neuron_count", 500)
config.set("brain.region.1.connection_probability", 0.2)

# Motor cortex (motor neurons)
config.set("brain.region.2.neuron_type", "Motor")
config.set("brain.region.2.neuron_count", 200)
config.set("brain.region.2.connection_probability", 0.15)

# Association cortex (interconnecting regions)
config.set("brain.region.3.neuron_type", "Excitatory")
config.set("brain.region.3.neuron_count", 300)
config.set("brain.region.3.connection_probability", 0.1)

# Create specialized brain
brain = pynlm.createBrain(config)
brain.initialize()

# Add specific connections between regions
brain.addInterRegionConnection(
    pynlm.RegionId(1),  # Sensory -> Motor
    pynlm.RegionId(2),
    weight=0.8,
    delay=5
)
```

### Advanced Neuron Configuration

```python
# Configure advanced neuron properties
config.set("brain.v_thresh", -45.0)  # Higher firing threshold
config.set("brain.v_reset", -65.0)   # Reset potential
config.set("brain.tau_mem", 30.0)   # Membrane time constant
config.set("brain.tau_ref", 3.0)    # Refractory period
config.set("brain.i_thresh", 15.0)  # Threshold current
config.set("brain.c_m", 0.25)       # Membrane capacitance

# Heterogeneous neuron populations
config.set("brain.excitatory_ratio", 0.8)
config.set("brain.inhibitory_ratio", 0.15)
config.set("brain.modulatory_ratio", 0.05)
```

### Complex Plasticity Networks

```python
# Multi-timescale plasticity
config.set("plasticity.stdp.tau_plus", 20.0)  # LTP time constant
config.set("plasticity.stdp.tau_minus", 20.0) # LTD time constant
config.set("plasticity.stdp.learning_rate", 0.01)

config.set("plasticity.hebbian.learning_rate", 0.005)
config.set("plasticity.structural.synaptogenesis_rate", 0.0001)
config.set("plasticity.structural.pruning_rate", 0.00001)

# Activity-dependent plasticity
config.set("plasticity.metaplasticity.base_rate", 0.001)
config.set("plasticity.metaplasticity.sliding_threshold", 0.5)
```

## Agent Development with Advanced Features

### Advanced Agent Configuration

```python
import pynlm

# Create sophisticated agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=20, visionHeight=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Configure advanced neuromodulation
agent.getBrain().getConfig().set("neuromod.dopamine.scale", 2.0)
agent.getBrain().getConfig().set("neuromod.curiosity.threshold", 0.3)
agent.getBrain().getConfig().set("neuromod.novelty.sensitivity", 0.8)
```

### Custom Agent Behaviors

```python
class AdvancedAgent:
    def __init__(self, brain, world):
        self.agent = pynlm.createAgentBrain(brain)
        self.agent.initialize(world)
        self.brain = brain
        self.world = world
        self.episode_reward = 0.0
        self.exploration_bonus = 0.0
        
    def run_episode(self, max_steps=1000):
        """Run an episode with advanced agent behaviors."""
        for step in range(max_steps):
            # Multi-modal sensory processing
            percept = self._process_multimodal_input()
            
            # Curiosity-driven exploration
            curiosity_bonus = self._calculate_curiosity_bonus(percept)
            self.exploration_bonus += curiosity_bonus
            
            # Agent processes input
            self.agent.processSensoryInput(percept)
            
            # Brain computes action
            action = self.agent.decodeMotorCommand()
            
            # Apply action with safety checks
            if self._is_safe_action(action):
                result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
                
                # Learn from experience
                reward = self._compute_enhanced_reward(result, curiosity_bonus)
                self.agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                self.agent.updateDevelopment(0.1)
                
                self.episode_reward += reward
                
                # Log for analysis
                self._log_episode_data(step, action, result, reward)
    
    def _process_multimodal_input(self):
        """Process enhanced sensory input with custom features."""
        percept = self.world.getSensoryPercept()
        
        # Extract and enhance features
        vision = percept.getVision()
        touch = percept.getTouch()
        internal = percept.getInternal()
        
        # Custom feature extraction
        features = {
            'vision_quality': self._analyze_vision_quality(vision),
            'movement_patterns': self._analyze_movement_patterns(),
            'resource_proximity': self._detect_resource_proximity(),
            'hazard_awareness': self._assess_hazard_risk(),
            'energy_status': internal[0] if internal else 0.0,
            'health_status': internal[1] if internal and len(internal) > 1 else 0.0,
        }
        
        return percept, features
    
    def _calculate_curiosity_bonus(self, percept_and_features):
        """Calculate curiosity-driven exploration bonus."""
        features = percept_and_features[1]
        novelty = features['vision_quality'] + features['hazard_awareness']
        return novelty * self.agent.getCuriosityLevel()
    
    def _compute_enhanced_reward(self, result, curiosity_bonus):
        """Compute enhanced reward with curiosity component."""
        base_reward = result.reward
        exploration_reward = curiosity_bonus * 0.1  # Scale curiosity bonus
        
        # Combine rewards
        total_reward = base_reward + exploration_reward
        
        # Apply temporal difference if predicted reward is available
        predicted = self.agent.getPredictionError()
        if predicted != 0:
            td_error = reward - predicted
            total_reward += td_error * 0.5
            
        return total_reward
    
    def _is_safe_action(self, action):
        """Check if action is safe given current state."""
        body = self.world.getAgentBody()
        
        # Check boundaries
        if action.type == pynlm.ActionType.MoveForward:
            x, y = body.x, body.y
            if self.world.getSimulationTime() % 0.1 < 0.05:  # Check every 0.1s
                # Simple forward projection
                pass
                
        return True  # Simplified safety check
```

## Memory System Integration

### Working Memory Analysis

```python
def analyze_working_memory(brain, steps=500):
    """Analyze working memory performance over time."""
    working_memory = brain.getWorkingMemory()
    
    if not working_memory:
        print("Working memory not available")
        return None
    
    memory_traces = []
    
    for step in range(steps):
        # Apply varying input patterns
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 
                                    float(step % 20 + 1))
        
        brain.step(step, step * 0.001)
        
        traces = working_memory.getActiveTraces()
        memory_traces.append(traces)
        
        if step % 100 == 0:
            print(f"Step {step}: {traces} active traces")
    
    # Analyze memory dynamics
    avg_traces = sum(memory_traces) / len(memory_traces)
    max_traces = max(memory_traces)
    
    return {
        'average_traces': avg_traces,
        'peak_traces': max_traces,
        'capacity_utilization': avg_traces / working_memory.getCapacity(),
        'trace_dynamics': memory_traces
    }
```

### Episodic Memory Management

```python
def episodic_memory_analysis(brain, steps=1000):
    """Analyze episodic memory formation and retrieval."""
    episodic_memory = brain.getEpisodicMemory()
    
    if not episodic_memory:
        print("Episodic memory not available")
        return None
    
    # Run simulation to create episodes
    for step in range(steps):
        # Create varied experiences
        pattern = step % 10
        brain.injectCurrentToNeurons(
            pynlm.NeuronType.Sensory, 
            float(pattern * 5 + 10)
        )
        
        brain.step(step, step * 0.001)
        
        if step % 100 == 0:
            episodes = episodic_memory.getEpisodeCount()
            print(f"Step {step}: {episodes} episodes stored")
    
    # Analyze episode characteristics
    episode_count = episodic_memory.getEpisodeCount()
    if episode_count > 0:
        # Test replay functionality
        replay_episodes = episodic_memory.getEpisodesForReplay(min(5, episode_count))
        
        return {
            'total_episodes': episode_count,
            'replay_success': len(replay_episodes) > 0,
            'replay_episodes': len(replay_episodes),
            'memory_efficiency': episode_count / steps,
            'episode_density': episode_count / (brain.getTotalNeuronCount() / 1000)
        }
    
    return {'total_episodes': 0, 'replay_success': False}
```

## Plasticity and Learning Optimization

### STDP Learning Curve Analysis

```python
def analyze_stdp_learning(brain, steps=1000):
    """Analyze STDP learning dynamics."""
    # Enable STDP
    stdp = brain.getSTDP()
    if stdp:
        stdp.enable(true)
    
    # Record synaptic weights
    initial_weights = []
    final_weights = []
    firing_rates = []
    
    # Get initial weights
    for region_id in brain.getRegionIds():
        region = brain.getRegion(region_id)
        if region:
            for synapse in region.getSynapses():
                initial_weights.append(synapse.getWeight())
    
    # Run learning simulation
    for step in range(steps):
        # Apply learning stimuli
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
        
        brain.step(step, step * 0.001)
        
        firing_rates.append(brain.getAverageFiringRate())
        
        if step == steps - 1:
            # Get final weights
            for region_id in brain.getRegionIds():
                region = brain.getRegion(region_id)
                if region:
                    for synapse in region.getSynapses():
                        final_weights.append(synapse.getWeight())
    
    # Analyze learning
    if initial_weights and final_weights:
        initial_mean = sum(initial_weights) / len(initial_weights)
        final_mean = sum(final_weights) / len(final_weights)
        weight_change = final_mean - initial_mean
        
        return {
            'learning_rate': (weight_change / initial_mean) * 100 if initial_mean > 0 else 0,
            'final_firing_rate': firing_rates[-1],
            'avg_firing_rate': sum(firing_rates) / len(firing_rates),
            'weight_change': weight_change,
            'initial_weights': initial_weights,
            'final_weights': final_weights,
            'firing_rates': firing_rates
        }
    
    return None
```

### Metaplasticity Control

```python
def metaplasticity_control(brain, learning_schedule):
    """Control metaplasticity for optimal learning."""
    plasticity = brain.getPlasticity()
    
    if plasticity:
        for step, target_efficiency in enumerate(learning_schedule):
            # Adjust metaplasticity parameters based on learning progress
            current_efficiency = brain.getLearningEfficiency() if hasattr(brain, 'getLearningEfficiency') else 0.5
            
            if current_efficiency < target_efficiency:
                # Increase plasticity to accelerate learning
                plasticity.increase_metaplasticity_rate(0.1)
                plasticity.decrease_sliding_threshold(0.05)
            else:
                # Decrease plasticity to stabilize learning
                plasticity.decrease_metaplasticity_rate(0.05)
                plasticity.increase_sliding_threshold(0.02)
            
            brain.step(step, step * 0.001)
            
        return plasticity.get_metaplasticity_state()
    
    return None
```

## World Simulation Strategies

### Complex Environment Creation

```python
def create_complex_world(width=100, height=100, complexity=0.3):
    """Create a complex world with various objects and challenges."""
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 20, 20)
    world.reset()
    
    # Add resources
    num_resources = int(width * height * 0.05 * complexity)
    for _ in range(num_resources):
        world.addObject(pynlm.WorldObject(
            x=np.random.uniform(5, width-5),
            y=np.random.uniform(5, height-5),
            type=pynlm.WorldObjectType.Resource,
            value=np.random.uniform(5, 20),
            radius=0.5
        ))
    
    # Add hazards
    num_hazards = int(width * height * 0.02 * complexity)
    for _ in range(num_hazards):
        world.addObject(pynlm.WorldObject(
            x=np.random.uniform(5, width-5),
            y=np.random.uniform(5, height-5),
            type=pynlm.WorldObjectType.Hazard,
            value=-np.random.uniform(10, 25),
            radius=0.4
        ))
    
    # Add walls for navigation
    for x in range(0, width, 10):
        world.addObject(pynlm.WorldObject(
            x=x, y=height//2, type=pynlm.WorldObjectType.Wall, radius=0.3
        ))
    
    for y in range(0, height, 10):
        world.addObject(pynlm.WorldObject(
            x=width//2, y=y, type=pynlm.WorldObjectType.Wall, radius=0.3
        ))
    
    return world
```

### Navigation and Pathfinding

```python
def simulate_navigation(brain, world, target_x, target_y):
    """Simulate agent navigation to target."""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Set target position
    world.setAgentStart(world.getAgentBody().x, world.getAgentBody().y)
    
    # Navigation simulation
    successful_moves = 0
    navigation_path = []
    
    for step in range(500):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Check if we're getting closer to target
        body = world.getAgentBody()
        distance = ((body.x - target_x)**2 + (body.y - target_y)**2)**0.5
        navigation_path.append(distance)
        
        if distance < 2.0:  # Reached target
            successful_moves += 1
            break
            
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate navigation metrics
    avg_distance = sum(navigation_path) / len(navigation_path)
    min_distance = min(navigation_path)
    
    return {
        'successful_moves': successful_moves,
        'average_distance': avg_distance,
        'min_distance': min_distance,
        'navigation_efficiency': successful_moves / (step + 1) if step > 0 else 0,
        'navigation_path': navigation_path
    }
```

## Reinforcement Learning Examples

### Q-Learning with NLM

```python
def q_learning_simulation(world, brain, num_episodes=100):
    """Implement Q-learning using NLM's learning capabilities."""
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Q-table for state-action values
    q_table = {}
    
    # Training loop
    episode_rewards = []
    episode_lengths = []
    
    for episode in range(num_episodes):
        # Reset environment
        world.reset()
        
        episode_reward = 0.0
        episode_steps = 0
        
        for step in range(200):
            # Get state representation
            percept = world.getSensoryPercept()
            state = _extract_state_features(percept, world)
            
            # Choose action (epsilon-greedy)
            action = _choose_action(state, q_table, epsilon=0.1)
            
            # Execute action
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action_result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get next state
            next_percept = world.getSensoryPercept()
            next_state = _extract_state_features(next_percept, world)
            
            # Compute reward
            reward = action_result.reward
            
            # Update Q-value
            _update_q_value(q_table, state, action, reward, next_state)
            
            # Learn from reward
            agent.applyRewardModulation(reward, 0.0)
            
            episode_reward += reward
            episode_steps += 1
            
            if step % 50 == 0:
                agent.updateDevelopment(0.1)
        
        episode_rewards.append(episode_reward)
        episode_lengths.append(episode_steps)
        
        print(f"Episode {episode + 1}: Reward={episode_reward:.2f}, Steps={episode_steps}")
    
    return {
        'q_table': q_table,
        'episode_rewards': episode_rewards,
        'episode_lengths': episode_lengths,
        'final_policy': _extract_policy_from_q_table(q_table)
    }
```

## Performance Monitoring and Profiling

### Advanced Performance Metrics

```python
import time
import threading
import psutil
import numpy as np
import matplotlib.pyplot as plt

class AdvancedPerformanceMonitor:
    def __init__(self, brain, world=None):
        self.brain = brain
        self.world = world
        self.metrics_history = []
        self.monitor_thread = None
        self.stop_monitoring = False
        
    def start_advanced_monitoring(self, interval=0.05):
        """Start advanced performance monitoring."""
        self.stop_monitoring = False
        self.monitor_thread = threading.Thread(
            target=self._advanced_monitor_loop,
            args=(interval,),
            daemon=True
        )
        self.monitor_thread.start()
        
    def _advanced_monitor_loop(self, interval):
        """Advanced monitoring with multiple metrics."""
        while not self.stop_monitoring:
            # Collect comprehensive metrics
            metrics = {
                'timestamp': time.time(),
                'brain_metrics': self._collect_brain_metrics(),
                'world_metrics': self._collect_world_metrics() if self.world else {},
                'system_metrics': self._collect_system_metrics(),
                'learning_metrics': self._collect_learning_metrics()
            }
            
            self.metrics_history.append(metrics)
            time.sleep(interval)
            
    def _collect_brain_metrics(self):
        """Collect detailed brain metrics."""
        metrics = {
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': self.brain.getDevelopmentalStage(),
            'working_memory_traces': self._get_working_memory_traces(),
            'episodic_memory_episodes': self._get_episodic_memory_episodes(),
            'prediction_error': self._get_prediction_error()
        }
        return metrics
        
    def _collect_world_metrics(self):
        """Collect world simulation metrics."""
        if not self.world:
            return {}
            
        body = self.world.getAgentBody()
        return {
            'simulation_time': self.world.getSimulationTime(),
            'agent_x': body.x,
            'agent_y': body.y,
            'agent_orientation': body.orientation,
            'agent_velocity_x': body.velocityX,
            'agent_velocity_y': body.velocityY,
            'agent_energy': body.energy,
            'agent_health': body.health,
            'world_width': self.world.getWidth(),
            'world_height': self.world.getHeight(),
            'sensory_percept_quality': self._analyze_percept_quality()
        }
        
    def generate_performance_report(self):
        """Generate comprehensive performance report."""
        if not self.metrics_history:
            return "No performance data collected"
            
        # Analyze trends and patterns
        brain_metrics = [m['brain_metrics'] for m in self.metrics_history]
        
        report = {
            'summary': {
                'total_monitoring_time': len(self.metrics_history) * 0.05,
                'peak_neuron_activity': max(m['brain_metrics']['firing_neurons'] for m in brain_metrics),
                'average_firing_rate': np.mean([m['brain_metrics']['average_firing_rate'] for m in brain_metrics]),
                'total_spike_production': sum(m['brain_metrics']['total_spikes'] for m in brain_metrics),
                'development_progress': self._analyze_development_progress(brain_metrics)
            },
            
            'stability_metrics': {
                'firing_rate_stability': self._calculate_stability([m['brain_metrics']['average_firing_rate'] for m in brain_metrics]),
                'neuron_activity_stability': self._calculate_stability([m['brain_metrics']['firing_neurons'] for m in brain_metrics]),
                'memory_system_stability': self._analyze_memory_stability(brain_metrics)
            },
            
            'efficiency_metrics': {
                'spike_efficiency': self._calculate_spike_efficiency(brain_metrics),
                'energy_efficiency': self._calculate_energy_efficiency(brain_metrics),
                'learning_efficiency': self._calculate_learning_efficiency(brain_metrics)
            }
        }
        
        return report
```

## Batch Processing and Scalability

### Distributed Batch Processing

```python
import multiprocessing as mp
from concurrent.futures import ProcessPoolExecutor
import time

class BatchProcessor:
    def __init__(self, config=None):
        self.config = config or {}
        self.results = []
        
    def run_distributed_batch(self, num_simulations, steps_per_simulation, num_workers=None):
        """Run batch processing using multiple workers."""
        if num_workers is None:
            num_workers = mp.cpu_count()
            
        print(f"Running distributed batch with {num_workers} workers")
        
        # Prepare simulation parameters
        sim_params = [(i, num_simulations, steps_per_simulation, self.config) 
                     for i in range(num_simulations)]
        
        # Run simulations in parallel
        with ProcessPoolExecutor(max_workers=num_workers) as executor:
            results = list(executor.map(self._run_single_simulation, sim_params))
        
        self.results.extend(results)
        return self.results
        
    def _run_single_simulation(self, params):
        """Run a single simulation (for parallel execution)."""
        sim_id, total_sims, steps, config = params
        
        # Create and run simulation
        brain_config = pynlm.createDefaultConfig()
        for key, value in config.items():
            brain_config.set(key, value)
            
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps):
            brain.step(step, step * 0.001)
        
        simulation_time = time.time() - start_time
        
        return {
            'simulation_id': sim_id,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'simulation_time': simulation_time,
            'steps_per_second': steps / simulation_time if simulation_time > 0 else 0,
            'config': config
        }
        
    def get_batch_statistics(self):
        """Get comprehensive batch statistics."""
        if not self.results:
            return {}
            
        firing_rates = [r['final_firing_rate'] for r in self.results]
        spike_counts = [r['total_spikes'] for r in self.results]
        times = [r['simulation_time'] for r in self.results]
        
        return {
            'total_simulations': len(self.results),
            'firing_rate_statistics': {
                'mean': np.mean(firing_rates),
                'std': np.std(firing_rates),
                'min': np.min(firing_rates),
                'max': np.max(firing_rates),
                'median': np.median(firing_rates)
            },
            'performance_statistics': {
                'total_time': np.sum(times),
                'avg_time_per_sim': np.mean(times),
                'throughput': len(self.results) / np.sum(times) if np.sum(times) > 0 else 0,
                'speed_variance': np.std(times) / np.mean(times) if np.mean(times) > 0 else 0
            },
            'spike_statistics': {
                'total_spikes': np.sum(spike_counts),
                'avg_spikes_per_sim': np.mean(spike_counts),
                'spike_rate_variance': np.std(spike_counts)
            }
        }
```

## Troubleshooting and Best Practices

### Common Issues and Solutions

```python
def troubleshoot_brain_simulation(brain, world, max_steps=1000):
    """Troubleshoot common brain simulation issues."""
    issues = []
    
    # Check initialization
    if brain.getTotalNeuronCount() == 0:
        issues.append("Brain has no neurons - check configuration")
    
    # Check for activity
    if brain.getFiringNeuronCount() == 0:
        issues.append("No active neurons - check input stimuli")
    
    # Check for runaway activity
    if brain.getTotalSpikeCount() > 100000:
        issues.append("Excessive spiking - check plasticity parameters")
    
    # Check memory systems
    working_memory = brain.getWorkingMemory()
    if working_memory and working_memory.getActiveTraces() > working_memory.getCapacity():
        issues.append("Working memory overflow - reduce input intensity")
    
    # Check world-agent integration
    if world:
        body = world.getAgentBody()
        if body.energy <= 0 or body.health <= 0:
            issues.append("Agent energy/health depleted - adjust reward system")
    
    return issues

def optimize_simulation_parameters(brain, world, performance_target):
    """Optimize simulation parameters for target performance."""
    optimizations = []
    
    # Check firing rate
    current_firing_rate = brain.getAverageFiringRate()
    if current_firing_rate < 0.1:
        optimizations.append({
            'parameter': 'brain.v_thresh',
            'current_value': brain.getConfig().getOr('brain.v_thresh', -70.0),
            'recommended_value': -55.0,  # Higher threshold
            'reason': 'Increase firing threshold to boost activity'
        })
    
    elif current_firing_rate > 10.0:
        optimizations.append({
            'parameter': 'brain.v_thresh',
            'current_value': brain.getConfig().getOr('brain.v_thresh', -70.0),
            'recommended_value': -75.0,  # Lower threshold
            'reason': 'Decrease firing threshold to prevent runaway activity'
        })
    
    # Check plasticity
    if brain.getSTDP() and not brain.getSTDP().isEnabled():
        optimizations.append({
            'parameter': 'plasticity.stdp.enable',
            'current_value': False,
            'recommended_value': True,
            'reason': 'Enable STDP for better learning dynamics'
        })
    
    # Check neuromodulation
    if not brain.getConfig().getOr('neuromod.dopamine.scale', 1.0) > 1.0:
        optimizations.append({
            'parameter': 'neuromod.dopamine.scale',
            'current_value': brain.getConfig().getOr('neuromod.dopamine.scale', 1.0),
            'recommended_value': 2.0,
            'reason': 'Increase dopamine scale for stronger learning signals'
        })
    
    return optimizations
```

## API Reference

### Core Classes

#### Brain
- `createBrain(config)`: Create a new brain with configuration
- `brain.initialize()`: Initialize the brain
- `brain.step(step, time=None)`: Run simulation step
- `brain.receiveSensoryInput(input)`: Inject sensory input
- `brain.injectCurrent(neuron, current)`: Inject current into neuron
- `brain.injectCurrentToNeurons(type, current)`: Inject current to neuron type
- `brain.produceAction()`: Generate motor action
- `brain.reset()`: Reset brain state
- `brain.save(filepath)`: Save brain state
- `brain.load(filepath)`: Load brain state

#### Config
- `createDefaultConfig()`: Create default configuration
- `config.set(key, value)`: Set configuration value
- `config.get(key)`: Get configuration value
- `config.has(key)`: Check if key exists
- `config.loadFromFile(filepath)`: Load from file
- `config.saveToFile(filepath)`: Save to file

#### AgentBrain
- `createAgentBrain(brain)`: Create agent-brain interface
- `agent.initialize(world)`: Initialize with world
- `agent.processSensoryInput(percept)`: Process sensory input
- `agent.decodeMotorCommand()`: Decode motor command
- `agent.applyRewardModulation(reward, predicted)`: Apply reward learning
- `agent.updateDevelopment(timestep)`: Update development

#### SimpleWorld
- `createSimpleWorld()`: Create simple world
- `world.configure(width, height, vision_width, vision_height)`: Configure world
- `world.update(timestep)`: Update world state
- `world.applyMotorCommand(cmd, time)`: Apply motor command
- `world.getSensoryPercept()`: Get sensory percept
- `world.getAgentBody()`: Get agent body state

### Advanced Classes

#### PerformanceMonitor
- `start_monitoring(interval)`: Start performance monitoring
- `stop_monitoring()`: Stop monitoring
- `get_performance_stats()`: Get performance statistics
- `plot_performance(metrics)`: Plot performance metrics
- `save_performance_data(filepath)`: Save performance data

#### BatchProcessor
- `run_batch(num_simulations, steps)`: Run batch simulations
- `get_batch_summary()`: Get batch summary
- `save_results(filepath)`: Save batch results

### Neuron and Synapse Types

#### NeuronType
- `Excitatory`: Excitatory neurons
- `Inhibitory`: Inhibitory neurons
- `Modulatory`: Modulatory neurons
- `Sensory`: Sensory neurons
- `Motor`: Motor neurons
- `Internal`: Internal neurons

#### SynapseType
- `Excitatory`: Excitatory synapses
- `Inhibitory`: Inhibitory synapses
- `Modulatory`: Modulatory synapses
- `Electrical`: Electrical synapses
- `GapJunction`: Gap junction synapses

#### DevelopmentalStage
- `Initial`: Initial developmental stage
- `CriticalPeriod`: Critical period stage
- `Maturation`: Maturation stage
- `Adult`: Adult stage
- `Aging`: Aging stage

## Conclusion

The NLM Python API provides a powerful framework for creating, simulating, and analyzing artificial neural systems. This guide has covered advanced techniques for:

- **Complex brain architectures** with specialized regions and connectivity
- **Advanced plasticity mechanisms** including multi-timescale learning
- **Neuromodulation systems** for curiosity and reward processing
- **Memory integration** for experience storage and recall
- **Performance optimization** for efficient simulation
- **Scalable processing** for large-scale batch experiments

The examples and techniques provided here enable researchers and developers to leverage the full capabilities of the NLM framework for cutting-edge neural computation and artificial intelligence research.

For further assistance, please refer to the individual API method documentation or visit the NLM project documentation.
"""

import pynlm
import numpy as np
import time
import threading
from typing import Dict, List, Optional, Any

# Performance monitoring classes (from previous example)
class PerformanceMonitor:
    pass

class BatchProcessor:
    pass

if __name__ == "__main__":
    print("Advanced NLM API - User Guide")
    print("This guide provides comprehensive documentation for advanced NLM usage.")
    print("\nFor implementation details and examples, see:")
    print("  - python/advanced_api_examples.py")
    print("  - python/performance_monitor.py")
    print("  - individual API method documentation")
