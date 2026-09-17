# NLM Advanced User Features & Configuration Guide

## Introduction

This guide provides comprehensive examples of advanced NLM features for power users who need to go beyond basic usage. It covers custom brain configurations, performance optimization, multi-agent environments, and advanced learning strategies.

## Table of Contents

1. [Custom Brain Configurations](#custom-brain-configurations)
2. [Performance Optimization Techniques](#performance-optimization)
3. [Multi-Agent Environments](#multi-agent-environments)
4. [Advanced Learning Strategies](#advanced-learning-strategies)
5. [Real-World Application Examples](#real-world-examples)

---

# Custom Brain Configurations

## 1. Advanced Configuration Builder

This example demonstrates how to create sophisticated brain configurations for specific use cases:

```python
import pynlm
import json

class AdvancedBrainConfig:
    """Advanced brain configuration builder for specialized applications."""
    
    @staticmethod
    def create_creative_brain():
        """Create a brain optimized for creative tasks like problem-solving and pattern discovery."""
        config = pynlm.createDefaultConfig()
        
        # Creative brain architecture
        config.set("brain.neuron_count", 2000)
        config.set("brain.region_count", 8)
        config.set("brain.synapse_density", 0.08)
        
        # Enhanced plasticity for creative learning
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.002)
        config.set("plasticity.stdp.tau_plus", 25.0)
        config.set("plasticity.stdp.tau_minus", 25.0)
        config.set("plasticity.hebbian.enable", True)
        config.set("plasticity.hebbian.learning_rate", 0.001)
        
        # Balanced neuromodulation for creativity
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_rate", 0.3)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.threshold", 0.15)
        config.set("neuromod.dopamine.scale", 1.2)
        
        # Extended development period for complex learning
        config.set("development.enable_all", True)
        config.set("development.stage_duration.initial", 120.0)
        config.set("development.stage_duration.critical", 600.0)
        config.set("development.stage_duration.maturation", 1800.0)
        config.set("development.synaptogenesis_rate", 0.0005)
        config.set("development.pruning_rate", 0.00005)
        
        # Advanced memory systems
        config.set("memory.working_capacity", 100)
        config.set("memory.episodic_capacity", 1000)
        config.set("memory.associative_strength", 0.9)
        
        # Cognitive enhancement
        config.set("cognition.planning_horizon", 20)
        config.set("cognition.concept_threshold", 0.7)
        
        # Performance settings
        config.set("performance.checkpoint_interval", 100)
        config.set("performance.simulation_timestep", 0.01)
        config.set("performance.enable_vectorization", True)
        
        return config
    
    @staticmethod
    def create_specialized_sensor_brain():
        """Create a brain specialized for sensory processing and pattern recognition."""
        config = pynlm.createDefaultConfig()
        
        # High-density sensory processing
        config.set("brain.neuron_count", 1500)
        config.set("brain.synapse_density", 0.12)
        
        # Enhanced sensory regions
        config.set("region.cortex.sensory_neurons", 400)
        config.set("region.cortex.motor_neurons", 100)
        config.set("region.memory.associative_neurons", 200)
        
        # Sensory-specific plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0015)
        config.set("plasticity.stdp.tau_plus", 30.0)
        config.set("plasticity.stdp.tau_minus", 20.0)
        
        # Strong novelty detection for pattern finding
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_rate", 0.5)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.adaptation_rate", 0.95)
        
        # Fast development for rapid learning
        config.set("development.enable_critical_period", True)
        config.set("development.stage_duration.initial", 30.0)
        config.set("development.stage_duration.critical", 150.0)
        
        return config
    
    @staticmethod
    def create_production_optimized_config():
        """Create a configuration optimized for production use with stability and efficiency."""
        config = pynlm.createDefaultConfig()
        
        # Balanced configuration
        config.set("brain.neuron_count", 800)
        config.set("brain.synapse_density", 0.06)
        
        # Stable plasticity
        config.set("plasticity.stdp.enable", True)
        config.set("plasticity.stdp.learning_rate", 0.0005)
        config.set("plasticity.stdp.tau_plus", 50.0)
        config.set("plasticity.stdp.tau_minus", 50.0)
        config.set("plasticity.hebbian.enable", False)  # Disabled for stability
        
        # Temperate neuromodulation
        config.set("neuromod.curiosity.enable", True)
        config.set("neuromod.curiosity.exploration_rate", 0.2)
        config.set("neuromod.novelty.enable", True)
        config.set("neuromod.novelty.threshold", 0.25)
        
        # Normal development
        config.set("development.enable_all", True)
        config.set("development.stage_duration.initial", 90.0)
        config.set("development.stage_duration.critical", 450.0)
        
        # Performance optimized
        config.set("performance.checkpoint_interval", 500)
        config.set("performance.simulation_timestep", 0.05)
        config.set("performance.enable_vectorization", True)
        config.set("performance.memory_pool_size", "large")
        
        return config
    
    @staticmethod
    def save_config(config, filename):
        """Save configuration to JSON file with proper error handling."""
        try:
            config.saveToFile(filename)
            print(f"Configuration saved to {filename}")
        except Exception as e:
            print(f"Error saving configuration: {e}")
            raise
    
    @staticmethod
    def load_config(filename):
        """Load configuration from JSON file."""
        config = pynlm.createDefaultConfig()
        if config.loadFromFile(filename):
            print(f"Configuration loaded from {filename}")
        else:
            print(f"Warning: Could not load configuration from {filename}, using defaults")
        return config

# Usage examples
if __name__ == "__main__":
    print("=== Advanced Configuration Examples ===")
    
    # Create different types of brains for different applications
    creative_brain_config = AdvancedBrainConfig.create_creative_brain()
    print(f"Creative brain config created with {creative_brain_config.getOr('brain.neuron_count', 0)} neurons")
    
    sensor_brain_config = AdvancedBrainConfig.create_specialized_sensor_brain()
    print(f"Sensor brain config created with {sensor_brain_config.getOr('brain.neuron_count', 0)} neurons")
    
    production_config = AdvancedBrainConfig.create_production_optimized_config()
    print(f"Production config created with {production_config.getOr('brain.neuron_count', 0)} neurons")
    
    # Save configurations
    AdvancedBrainConfig.save_config(creative_brain_config, "creative_brain_config.json")
    AdvancedBrainConfig.save_config(sensor_brain_config, "sensor_brain_config.json")
    AdvancedBrainConfig.save_config(production_config, "production_config.json")
```

## 2. Dynamic Configuration Management

This example shows how to dynamically update configurations during runtime:

```python
import pynlm
import threading
import time

class DynamicConfigManager:
    """Manage dynamic configuration updates for runtime adaptation."""
    
    def __init__(self, brain):
        self.brain = brain
        self.config = brain.getConfig()
        self.update_thread = None
        self.running = False
        self.update_interval = 10.0  # seconds
    
    def start_automatic_updates(self):
        """Start automatic configuration updates based on performance metrics."""
        self.running = True
        self.update_thread = threading.Thread(target=self._automatic_updates, daemon=True)
        self.update_thread.start()
        print("Started automatic configuration updates")
    
    def _automatic_updates(self):
        """Background thread for automatic configuration updates."""
        while self.running:
            time.sleep(self.update_interval)
            
            # Get current performance metrics
            firing_rate = self.brain.getAverageFiringRate()
            total_spikes = self.brain.getTotalSpikeCount()
            
            # Adapt configuration based on performance
            if firing_rate > 50.0:  # Too high firing rate
                self.config.set("brain.synapse_density", self.config.getOr("brain.synapse_density", 0.1) * 0.9)
                print(f"Reducing synapse density to {self.config.getOr('brain.synapse_density', 0.1)}")
            
            elif firing_rate < 5.0:  # Too low firing rate
                self.config.set("brain.synapse_density", self.config.getOr("brain.synapse_density", 0.1) * 1.1)
                print(f"Increasing synapse density to {self.config.getOr('brain.synapse_density', 0.1)}")
            
            # Adapt plasticity based on learning progress
            if total_spikes > 10000:  # High spike count suggests too much plasticity
                self.config.set("plasticity.stdp.learning_rate", 
                              self.config.getOr("plasticity.stdp.learning_rate", 0.001) * 0.8)
                print(f"Reducing STDP learning rate to {self.config.getOr('plasticity.stdp.learning_rate', 0.001)}")
    
    def apply_performance_profile(self, profile_name):
        """Apply a predefined performance profile."""
        profiles = {
            "conservative": {
                "plasticity.stdp.learning_rate": 0.0001,
                "neuromod.curiosity.exploration_rate": 0.1,
                "development.stage_duration.initial": 180.0
            },
            "balanced": {
                "plasticity.stdp.learning_rate": 0.001,
                "neuromod.curiosity.exploration_rate": 0.3,
                "development.stage_duration.initial": 90.0
            },
            "exploratory": {
                "plasticity.stdp.learning_rate": 0.005,
                "neuromod.curiosity.exploration_rate": 0.7,
                "development.stage_duration.initial": 30.0
            }
        }
        
        if profile_name not in profiles:
            raise ValueError(f"Unknown profile: {profile_name}")
        
        print(f"Applying {profile_name} performance profile")
        for key, value in profiles[profile_name].items():
            self.config.set(key, value)
    
    def stop(self):
        """Stop automatic updates."""
        self.running = False
        if self.update_thread:
            self.update_thread.join(timeout=5.0)
        print("Stopped automatic configuration updates")

# Example usage
if __name__ == "__main__":
    # Create a brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create dynamic config manager
    config_manager = DynamicConfigManager(brain)
    
    # Apply a performance profile
    config_manager.apply_performance_profile("balanced")
    
    # Start automatic updates
    config_manager.start_automatic_updates()
    
    # Run some simulation steps
    for i in range(100):
        brain.step(i, i * 0.1)
        time.sleep(0.1)
    
    # Stop automatic updates
    config_manager.stop()
```

# Performance Optimization Techniques

## 3. Memory Management Optimization

This example demonstrates advanced memory management techniques:

```python
import pynlm
import gc
import psutil
import os

class MemoryOptimizedSimulation:
    """Memory-optimized simulation with automatic cleanup and monitoring."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.simulation_steps = []
        self.checkpoints = []
        self.memory_stats = []
        self.cleanup_interval = 100
        
        # Configure for memory efficiency
        self.config.set("performance.memory_pool_size", "medium")
        self.config.set("performance.checkpoint_interval", 200)
        self.config.set("performance.enable_vectorization", True)
    
    def run_with_memory_management(self, steps=1000, enable_checkpoints=True):
        """Run simulation with advanced memory management."""
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print(f"Starting memory-optimized simulation: {steps} steps")
        
        for step in range(steps):
            # Run simulation step
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
            
            # Store simulation data (with size limits)
            if step % 10 == 0:
                self.simulation_steps.append({
                    'step': step,
                    'firing_rate': brain.getAverageFiringRate(),
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel()
                })
            
            # Take periodic checkpoints
            if enable_checkpoints and step % self.config.getOr("performance.checkpoint_interval", 200) == 0:
                checkpoint = self._take_checkpoint(brain, world, agent, step)
                self.checkpoints.append(checkpoint)
                self._cleanup_memory(step)
            
            # Monitor memory usage
            if step % self.cleanup_interval == 0:
                self._monitor_memory()
        
        return brain, world, agent
    
    def _take_checkpoint(self, brain, world, agent, step):
        """Take a memory-efficient checkpoint."""
        try:
            # Create a lightweight checkpoint
            checkpoint = {
                'step': step,
                'firing_rate': brain.getAverageFiringRate(),
                'total_spikes': brain.getTotalSpikeCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'position': (world.getAgentBody().x, world.getAgentBody().y)
            }
            
            # Save to file if needed
            checkpoint_file = f"checkpoint_step_{step}.json"
            with open(checkpoint_file, 'w') as f:
                import json
                json.dump(checkpoint, f)
            
            return checkpoint
        except Exception as e:
            print(f"Error taking checkpoint: {e}")
            return None
    
    def _cleanup_memory(self, step):
        """Perform memory cleanup operations."""
        # Force garbage collection
        collected = gc.collect()
        print(f"Memory cleanup at step {step}: collected {collected} objects")
        
        # Limit stored simulation data
        if len(self.simulation_steps) > 1000:
            # Keep only recent data
            self.simulation_steps = self.simulation_steps[-500:]
        
        # Remove old checkpoint files
        if len(self.checkpoints) > 10:
            old_checkpoints = self.checkpoints[:-10]
            for checkpoint in old_checkpoints:
                if checkpoint:
                    checkpoint_file = f"checkpoint_step_{checkpoint['step']}.json"
                    try:
                        os.remove(checkpoint_file)
                    except:
                        pass
            self.checkpoints = self.checkpoints[-10:]
    
    def _monitor_memory(self):
        """Monitor and log memory usage."""
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        self.memory_stats.append({
            'step': len(self.memory_stats),
            'rss_mb': memory_info.rss / 1024 / 1024,
            'vms_mb': memory_info.vms / 1024 / 1024,
            'simulation_data_mb': len(str(self.simulation_steps)) / 1024 / 1024
        })
        
        if self.memory_stats[-1]['rss_mb'] > 500:  # Alert if > 500MB
            print(f"High memory usage alert: {self.memory_stats[-1]['rss_mb']:.1f} MB")
    
    def get_memory_summary(self):
        """Get comprehensive memory usage summary."""
        if not self.memory_stats:
            return "No memory statistics available"
        
        current = self.memory_stats[-1]
        peak_rss = max(stat['rss_mb'] for stat in self.memory_stats)
        peak_vms = max(stat['vms_mb'] for stat in self.memory_stats)
        
        return {
            'current_memory_mb': current['rss_mb'],
            'peak_memory_mb': peak_rss,
            'virtual_memory_mb': current['vms_mb'],
            'peak_virtual_mb': peak_vms,
            'simulation_steps_stored': len(self.simulation_steps),
            'checkpoints_saved': len(self.checkpoints)
        }

# Performance benchmark example
if __name__ == "__main__":
    import time
    
    print("=== Memory-Optimized Simulation Example ===")
    
    # Run optimized simulation
    sim = MemoryOptimizedSimulation()
    start_time = time.time()
    
    brain, world, agent = sim.run_with_memory_management(steps=1000, enable_checkpoints=True)
    
    end_time = time.time()
    
    # Display results
    memory_summary = sim.get_memory_summary()
    print(f"\nSimulation completed in {end_time - start_time:.2f} seconds")
    print(f"Memory usage: {memory_summary['current_memory_mb']:.1f} MB (peak: {memory_summary['peak_memory_mb']:.1f} MB)")
    print(f"Simulation steps stored: {memory_summary['simulation_steps_stored']}")
    print(f"Checkpoints saved: {memory_summary['checkpoints_saved']}")
```

## 4. Performance Benchmarking

This example shows how to benchmark and compare different brain configurations:

```python
import pynlm
import time
import statistics

class BrainBenchmark:
    """Benchmark different brain configurations for performance."""
    
    def __init__(self):
        self.results = {}
    
    def benchmark_configuration(self, config_name, config, steps=500):
        """Benchmark a specific brain configuration."""
        print(f"\nBenchmarking {config_name}...")
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run benchmark
        start_time = time.time()
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        end_time = time.time()
        
        # Calculate metrics
        duration = end_time - start_time
        steps_per_second = steps / duration
        avg_firing_rate = brain.getAverageFiringRate()
        total_spikes = brain.getTotalSpikeCount()
        
        # Determine efficiency rating
        efficiency_score = self._calculate_efficiency(steps_per_second, avg_firing_rate, total_spikes)
        
        result = {
            'config_name': config_name,
            'duration_seconds': duration,
            'steps_per_second': steps_per_second,
            'average_firing_rate': avg_firing_rate,
            'total_spikes': total_spikes,
            'efficiency_score': efficiency_score,
            'steps': steps
        }
        
        self.results[config_name] = result
        
        print(f"  Duration: {duration:.2f}s")
        print(f"  Speed: {steps_per_second:.1f} steps/sec")
        print(f"  Firing rate: {avg_firing_rate:.2f} Hz")
        print(f"  Total spikes: {total_spikes}")
        print(f"  Efficiency: {efficiency_score:.2f}")
        
        return result
    
    def _calculate_efficiency(self, steps_per_second, firing_rate, total_spikes):
        """Calculate efficiency score balancing speed, stability, and activity."""
        # Higher is better
        speed_score = min(steps_per_second / 100.0, 2.0)  # Cap at 2.0
        stability_score = 1.0 / (1.0 + abs(firing_rate - 20.0) / 20.0)  # Optimal around 20Hz
        activity_score = min(total_spikes / 1000.0, 2.0)  # Scale with activity
        
        return (speed_score + stability_score + activity_score) / 3.0
    
    def run_comparison(self, configurations):
        """Run comparison benchmark across multiple configurations."""
        print("=== Brain Configuration Benchmark ===")
        
        for config_name, config in configurations.items():
            self.benchmark_configuration(config_name, config)
        
        # Print comparison
        print("\n=== Benchmark Results ===")
        print(f"{'Configuration':<25} {'Speed (steps/sec)':<18} {'Firing Rate':<15} {'Efficiency':<12}")
        print("-" * 70)
        
        for result in sorted(self.results.values(), key=lambda x: x['efficiency_score'], reverse=True):
            print(f"{result['config_name']:<25} {result['steps_per_second']:<18.1f} "
                  f"{result['average_firing_rate']:<15.2f} {result['efficiency_score']:<12.2f}")
    
    def get_best_configuration(self):
        """Get the most efficient configuration."""
        if not self.results:
            return None
        return max(self.results.values(), key=lambda x: x['efficiency_score'])

# Example usage
if __name__ == "__main__":
    benchmark = BrainBenchmark()
    
    # Define different brain configurations to test
    configurations = {
        'minimal': pynlm.createDefaultConfig(),
        'small': pynlm.createDefaultConfig(),
        'large': pynlm.createDefaultConfig(),
        'creative': pynlm.createDefaultConfig(),
        'sensor': pynlm.createDefaultConfig()
    }
    
    # Apply different settings to each configuration
    configurations['minimal'].set("brain.neuron_count", 200)
    configurations['small'].set("brain.neuron_count", 500)
    configurations['large'].set("brain.neuron_count", 2000)
    configurations['creative'].set("brain.neuron_count", 1500)
    configurations['sensor'].set("brain.neuron_count", 1200)
    
    # Run benchmark comparison
    benchmark.run_comparison(configurations)
    
    # Get best configuration
    best = benchmark.get_best_configuration()
    if best:
        print(f"\nBest configuration: {best['config_name']}")
        print(f"Efficiency score: {best['efficiency_score']:.2f}")
```

# Multi-Agent Environments

## 5. Multi-Agent Environment System

This example demonstrates a multi-agent environment with agent coordination and interaction:

```python
import pynlm
import time
import random
from enum import Enum

class AgentType(Enum):
    EXPLORER = "explorer"
    COLLECTOR = "collector"
    DEFENDER = "defender"
    COORDINATOR = "coordinator"

class MultiAgentEnvironment:
    """Advanced multi-agent environment with agent coordination and interaction."""
    
    def __init__(self, width=30, height=30, max_agents=8):
        self.width = width
        self.height = height
        self.max_agents = max_agents
        self.agents = []
        self.objects = []
        self.agent_id_counter = 0
        self.time_step = 0
        self.episode_count = 0
        
        # Environment parameters
        self.resource_spawn_rate = 0.05
        self.hazard_spawn_rate = 0.02
        self.agent_spawn_rate = 0.1
        
        # Initialize with coordinator agent
        self._add_coordinator_agent()
    
    def _add_coordinator_agent(self):
        """Add the environment coordinator agent."""
        coordinator_config = pynlm.createDefaultConfig()
        coordinator_config.set("brain.neuron_count", 300)
        coordinator_config.set("plasticity.stdp.enable", True)
        coordinator_config.set("neuromod.curiosity.enable", False)  # Coordinator is deterministic
        
        coordinator_brain = pynlm.createBrain(coordinator_config)
        coordinator_brain.initialize()
        
        coordinator_world = pynlm.createSimpleWorld()
        coordinator_world.configure(width=self.width, height=self.height, visionWidth=12, visionHeight=12)
        coordinator_world.reset()
        
        coordinator = pynlm.createAgentBrain(coordinator_brain)
        coordinator.initialize(coordinator_world)
        
        self.agents.append({
            'id': self.agent_id_counter,
            'name': "Coordinator",
            'agent': coordinator,
            'brain': coordinator_brain,
            'world': coordinator_world,
            'type': AgentType.COORDINATOR,
            'active': True,
            'role': 'environment_manager',
            'controlled_agents': []  # Will control other agents
        })
        
        self.agent_id_counter += 1
    
    def add_agent(self, agent_type=AgentType.EXPLORER, brain_config=None):
        """Add a new agent to the environment."""
        if len(self.agents) >= self.max_agents - 1:  # -1 for coordinator
            print("Maximum number of agents reached")
            return None
        
        if brain_config is None:
            brain_config = pynlm.createDefaultConfig()
            
            # Set agent type specific configuration
            if agent_type == AgentType.EXPLORER:
                brain_config.set("brain.neuron_count", 400)
                brain_config.set("neuromod.curiosity.exploration_rate", 0.8)
                brain_config.set("plasticity.stdp.learning_rate", 0.002)
            elif agent_type == AgentType.COLLECTOR:
                brain_config.set("brain.neuron_count", 500)
                brain_config.set("neuromod.curiosity.exploration_rate", 0.3)
                brain_config.set("plasticity.stdp.learning_rate", 0.001)
            elif agent_type == AgentType.DEFENDER:
                brain_config.set("brain.neuron_count", 350)
                brain_config.set("neuromod.curiosity.exploration_rate", 0.2)
                brain_config.set("plasticity.stdp.learning_rate", 0.0015)
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=self.width, height=self.height, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent_instance = {
            'id': self.agent_id_counter,
            'name': f"{agent_type.value.capitalize()} Agent",
            'agent': agent,
            'brain': brain,
            'world': world,
            'type': agent_type,
            'active': True,
            'role': agent_type.value,
            'resources_collected': 0,
            'position_history': [],
            'rewards_received': []
        }
        
        self.agents.append(agent_instance)
        self.agent_id_counter += 1
        
        print(f"Added {agent_instance['name']} (ID: {agent_instance['id']})")
        return agent_instance['id']
    
    def spawn_random_agent(self):
        """Spawn a new random agent."""
        agent_types = [AgentType.EXPLORER, AgentType.COLLECTOR, AgentType.DEFENDER]
        agent_type = random.choice(agent_types)
        return self.add_agent(agent_type)
    
    def update(self):
        """Update the entire multi-agent environment."""
        self.time_step += 1
        
        # Update coordinator
        coordinator = next((a for a in self.agents if a['type'] == AgentType.COORDINATOR), None)
        if coordinator and coordinator['active']:
            self._update_coordinator(coordinator)
        
        # Update non-coordinator agents
        for agent_instance in self.agents:
            if agent_instance['active'] and agent_instance['type'] != AgentType.COORDINATOR:
                self._update_agent(agent_instance)
        
        # Spawn new resources and hazards
        self._spawn_environment_objects()
        
        # Update agent positions
        self._update_agent_positions()
        
        # Check for agent deaths or achievements
        self._check_agent_status()
    
    def _update_coordinator(self, coordinator):
        """Update the environment coordinator."""
        coordinator_world = coordinator['world']
        coordinator_agent = coordinator['agent']
        
        # Get coordinator's sensory input
        percept = coordinator_world.getSensoryPercept()
        coordinator_agent.processSensoryInput(percept)
        coordinator['brain'].step(self.time_step)
        
        # Coordinator makes decisions based on environment state
        # For example: spawn new agents, change resource levels, etc.
        
        # Simple coordinator logic: occasionally spawn new agents
        if self.time_step % 500 == 0:
            self.spawn_random_agent()
        
        # Store coordinator's decision
        coordinator['last_decision'] = coordinator_agent.decodeMotorCommand()
    
    def _update_agent(self, agent_instance):
        """Update a single agent."""
        agent_world = agent_instance['world']
        agent_agent = agent_instance['agent']
        
        # Get current perception
        percept = agent_world.getSensoryPercept()
        agent_agent.processSensoryInput(percept)
        
        # Neural computation
        agent_instance['brain'].step(self.time_step)
        
        # Get action and apply
        action = agent_agent.decodeMotorCommand()
        
        # Process agent-specific action logic
        action_result = self._process_agent_action(agent_instance, action)
        
        # Apply motor command
        agent_world.applyMotorCommand(action, self.time_step * 0.1)
        
        # Store reward
        if 'reward' in action_result:
            agent_instance['rewards_received'].append(action_result['reward'])
        
        # Update agent's development based on experience
        agent_agent.updateDevelopment(0.1)
    
    def _process_agent_action(self, agent_instance, action):
        """Process an agent's action and return result."""
        agent_world = agent_instance['world']
        agent_body = agent_world.getAgentBody()
        
        result = {}
        
        if action.getType() == pynlm.ActionType.Eat:
            # Try to eat a resource
            if self._eat_resource(agent_instance):
                result['reward'] = 10.0
                result['success'] = True
                result['message'] = "Resource collected"
            else:
                result['reward'] = -1.0
                result['success'] = False
                result['message'] = "No resource available"
        
        elif action.getType() == pynlm.ActionType.Interact:
            # Simple interaction with environment
            result['reward'] = 1.0
            result['success'] = True
            result['message'] = "Environment interaction"
        
        elif action.getType() == pynlm.ActionType.Rest:
            # Rest and recover
            if agent_body.health < 100.0:
                agent_body.health = min(100.0, agent_body.health + 5.0)
                result['reward'] = 0.5
                result['success'] = True
                result['message'] = "Restored health"
            else:
                result['reward'] = 0.0
                result['success'] = False
                result['message'] = "Already at full health"
        
        else:
            # Movement actions have no direct reward, but may enable future rewards
            result['reward'] = 0.0
            result['success'] = True
        
        return result
    
    def _eat_resource(self, agent_instance):
        """Agent tries to eat a resource."""
        agent_world = agent_instance['world']
        agent_body = agent_world.getAgentBody()
        
        # Get nearby objects
        vision = agent_world.getSensoryPercept().getVision()
        width = agent_world.getVisionWidth()
        height = agent_world.getVisionHeight()
        
        # Simple vision processing (approximate)
        agent_x = int((agent_body.x / self.width) * width)
        agent_y = int((agent_body.y / self.height) * height)
        
        # Check nearby objects in the world
        for obj in self.objects:
            distance = ((agent_body.x - obj['x']) ** 2 + (agent_body.y - obj['y']) ** 2) ** 0.5
            if distance < 2.0:  # Close enough to interact
                if obj['type'] == 'resource':
                    # Eat the resource
                    agent_instance['resources_collected'] += 1
                    self.objects.remove(obj)
                    return True
        
        return False
    
    def _spawn_environment_objects(self):
        """Spawn new resources and hazards in the environment."""
        # Spawn resources
        if random.random() < self.resource_spawn_rate:
            x = random.uniform(1.0, self.width - 1.0)
            y = random.uniform(1.0, self.height - 1.0)
            
            if self._is_valid_position(x, y):
                self.objects.append({
                    'x': x,
                    'y': y,
                    'type': 'resource',
                    'value': random.uniform(1.0, 5.0),
                    'active': True,
                    'spawn_time': self.time_step
                })
        
        # Spawn hazards
        if random.random() < self.hazard_spawn_rate:
            x = random.uniform(1.0, self.width - 1.0)
            y = random.uniform(1.0, self.height - 1.0)
            
            if self._is_valid_position(x, y):
                self.objects.append({
                    'x': x,
                    'y': y,
                    'type': 'hazard',
                    'damage': random.uniform(5.0, 15.0),
                    'active': True,
                    'spawn_time': self.time_step
                })
    
    def _is_valid_position(self, x, y):
        """Check if a position is valid for spawning."""
        # Check if position is within bounds
        if not (0 <= x <= self.width and 0 <= y <= self.height):
            return False
        
        # Check if position is too close to other objects
        for obj in self.objects:
            distance = ((x - obj['x']) ** 2 + (y - obj['y']) ** 2) ** 0.5
            if distance < 1.0:
                return False
        
        return True
    
    def _update_agent_positions(self):
        """Update agent positions based on their actions."""
        for agent_instance in self.agents:
            if agent_instance['active']:
                agent_world = agent_instance['world']
                agent_body = agent_world.getAgentBody()
                
                # Store position history (limited to last 100 positions)
                agent_instance['position_history'].append((agent_body.x, agent_body.y))
                if len(agent_instance['position_history']) > 100:
                    agent_instance['position_history'].pop(0)
    
    def _check_agent_status(self):
        """Check agent status and handle deaths or achievements."""
        for agent_instance in self.agents:
            if not agent_instance['active']:
                continue
                
            agent_world = agent_instance['world']
            agent_body = agent_world.getAgentBody()
            
            # Check if agent died
            if agent_body.health <= 0:
                agent_instance['active'] = False
                print(f"Agent {agent_instance['name']} (ID: {agent_instance['id']}) died!")
                
                # Reset agent for potential respawn
                if self.time_step > 1000:  # Don't respawn too early
                    time.sleep(2.0)
                    agent_world.reset()
                    agent_instance['active'] = True
                    agent_instance['brain'].reset()
                    agent_instance['brain'].initialize()
                    agent_instance['agent'].reset()
                    agent_instance['agent'].initialize(agent_world)
                    print(f"Agent {agent_instance['name']} respawned")
            
            # Check for achievements
            if agent_instance['resources_collected'] > 0:
                print(f"Agent {agent_instance['name']} collected {agent_instance['resources_collected']} resources!")
    
    def get_environment_status(self):
        """Get comprehensive environment status report."""
        active_agents = [a for a in self.agents if a['active']]
        
        return {
            'time_step': self.time_step,
            'episode_count': self.episode_count,
            'total_agents': len(self.agents),
            'active_agents': len(active_agents),
            'objects_count': len(self.objects),
            'resources_count': len([o for o in self.objects if o['type'] == 'resource']),
            'hazards_count': len([o for o in self.objects if o['type'] == 'hazard']),
            'average_curiosity': statistics.mean([a['agent'].getCuriosityLevel() for a in active_agents]) if active_agents else 0,
            'average_novelty': statistics.mean([a['agent'].getNoveltyLevel() for a in active_agents]) if active_agents else 0
        }
    
    def run_simulation(self, steps=1000):
        """Run the multi-agent simulation."""
        print("=== Multi-Agent Environment Simulation ===")
        print(f"World size: {self.width}x{self.height}")
        print(f"Max agents: {self.max_agents}")
        print(f"Total simulation steps: {steps}")
        print("-" * 50)
        
        for step in range(steps):
            # Update environment
            self.update()
            
            # Print status every 100 steps
            if step % 100 == 0 and step > 0:
                status = self.get_environment_status()
                print(f"\nStep {step}:")
                print(f"  Active agents: {status['active_agents']}/{status['total_agents']}")
                print(f"  Resources: {status['resources_count']}, Hazards: {status['hazards_count']}")
                print(f"  Avg curiosity: {status['average_curiosity']:.3f}")
                print(f"  Avg novelty: {status['average_novelty']:.3f}")
        
        status = self.get_environment_status()
        print(f"\n=== Simulation Complete ===")
        print(f"Final status:")
        print(f"  Total steps: {status['time_step']}")
        print(f"  Active agents: {status['active_agents']}/{status['total_agents']}")
        print(f"  Resources collected: {sum(a['resources_collected'] for a in self.agents)}")
        
        return self.get_environment_status()

# Real-world application example
if __name__ == "__main__":
    # Create multi-agent environment
    environment = MultiAgentEnvironment(width=25, height=25, max_agents=6)
    
    # Add different types of agents
    environment.add_agent(AgentType.EXPLORER)  # Curious explorer
    environment.add_agent(AgentType.COLLECTOR)  # Resource collector
    environment.add_agent(AgentType.DEFENDER)   # Defender/protector
    
    # Run simulation
    result = environment.run_simulation(steps=800)
```

# Advanced Learning Strategies

## 6. Advanced Learning and Adaptation

This example demonstrates advanced learning strategies and adaptive behavior:

```python
import pynlm
import time
import numpy as np

class AdaptiveLearningAgent:
    """Advanced learning agent with adaptive strategies and meta-learning."""
    
    def __init__(self, config=None):
        self.config = config or pynlm.createDefaultConfig()
        self.brain = None
        self.world = None
        self.agent = None
        
        # Learning state
        self.performance_history = []
        self.strategy_history = []
        self.meta_learning_enabled = True
        
        # Adaptive parameters
        self.current_strategy = "balanced"
        self.exploration_rate = 0.3
        self.plasticity_adaptation = 1.0
        
        # Initialize agent
        self._initialize_agent()
    
    def _initialize_agent(self):
        """Initialize the learning agent with adaptive configuration."""
        # Configure brain for adaptive learning
        self.config.set("brain.neuron_count", 800)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("plasticity.stdp.learning_rate", 0.001)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        self.config.set("development.enable_all", True)
        
        # Create agent components
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning subsystems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def _collect_performance_metrics(self, step):
        """Collect comprehensive performance metrics."""
        metrics = {
            'step': step,
            'total_spikes': self.brain.getTotalSpikeCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'developmental_stage': self.agent.getDevelopmentalStage(),
            'simulation_time': step * 0.1
        }
        
        self.performance_history.append(metrics)
        
        # Limit history size
        if len(self.performance_history) > 1000:
            self.performance_history = self.performance_history[-500:]
        
        return metrics
    
    def _analyze_performance(self, current_metrics):
        """Analyze performance and determine if strategy adaptation is needed."""
        if len(self.performance_history) < 50:
            return "collecting_data"  # Not enough data for analysis
        
        # Get recent performance (last 100 steps)
        recent_metrics = self.performance_history[-100:]
        
        # Calculate performance indicators
        avg_firing_rate = np.mean([m['firing_rate'] for m in recent_metrics])
        avg_curiosity = np.mean([m['curiosity_level'] for m in recent_metrics])
        avg_novelty = np.mean([m['novelty_level'] for m in recent_metrics])
        avg_prediction_error = np.mean([m['prediction_error'] for m in recent_metrics])
        
        # Determine if performance is stable or deteriorating
        firing_rate_variance = np.var([m['firing_rate'] for m in recent_metrics])
        
        analysis = {
            'stable': firing_rate_variance < 100.0,
            'high_curiosity': avg_curiosity > 0.6,
            'high_novelty': avg_novelty > 0.5,
            'high_prediction_error': abs(avg_prediction_error) > 0.5,
            'avg_firing_rate': avg_firing_rate,
            'avg_curiosity': avg_curiosity
        }
        
        return analysis
    
    def _adapt_strategy(self, analysis):
        """Adapt learning strategy based on performance analysis."""
        if not self.meta_learning_enabled:
            return
        
        old_strategy = self.current_strategy
        new_strategy = old_strategy
        
        # Strategy adaptation logic
        if analysis['high_curiosity'] and not analysis['stable']:
            # High curiosity and instability - explore more
            new_strategy = "exploratory"
            self.exploration_rate = min(self.exploration_rate + 0.1, 0.8)
        
        elif analysis['high_prediction_error'] and analysis['stable']:
            # Stable but high prediction error - focus on learning
            new_strategy = "learning_focused"
            self.plasticity_adaptation = min(self.plasticity_adaptation + 0.2, 2.0)
        
        elif analysis['avg_firing_rate'] < 5.0:
            # Too low firing rate - increase plasticity
            new_strategy = "plasticity_boosted"
            self.plasticity_adaptation = min(self.plasticity_adaptation + 0.3, 2.0)
        
        elif analysis['avg_firing_rate'] > 50.0:
            # Too high firing rate - decrease plasticity
            new_strategy = "stability_focused"
            self.plasticity_adaptation = max(self.plasticity_adaptation - 0.2, 0.5)
        
        # Apply strategy changes
        if new_strategy != old_strategy:
            print(f"Strategy adapted: {old_strategy} → {new_strategy}")
            self._apply_strategy_change(new_strategy)
            self.strategy_history.append({
                'step': len(self.performance_history),
                'from': old_strategy,
                'to': new_strategy,
                'reason': self._get_adaptation_reason(analysis)
            })
    
    def _apply_strategy_change(self, strategy):
        """Apply specific strategy changes to brain configuration."""
        self.current_strategy = strategy
        
        if strategy == "exploratory":
            self.config.set("neuromod.curiosity.exploration_rate", 0.7)
            self.config.set("neuromod.novelty.threshold", 0.1)
        
        elif strategy == "learning_focused":
            self.config.set("plasticity.stdp.learning_rate", self.config.getOr("plasticity.stdp.learning_rate", 0.001) * 1.5)
            self.config.set("neuromod.curiosity.exploration_rate", 0.2)
        
        elif strategy == "plasticity_boosted":
            self.config.set("plasticity.stdp.learning_rate", self.config.getOr("plasticity.stdp.learning_rate", 0.001) * 2.0)
            self.config.set("development.stage_duration.initial", 30.0)
        
        elif strategy == "stability_focused":
            self.config.set("plasticity.stdp.learning_rate", self.config.getOr("plasticity.stdp.learning_rate", 0.001) * 0.5)
            self.config.set("neuromod.novelty.threshold", 0.3)
    
    def _get_adaptation_reason(self, analysis):
        """Get human-readable reason for strategy adaptation."""
        if analysis['high_curiosity'] and not analysis['stable']:
            return "High curiosity with unstable performance - need more exploration"
        elif analysis['high_prediction_error'] and analysis['stable']:
            return "Stable but high prediction error - focus on learning"
        elif analysis['avg_firing_rate'] < 5.0:
            return "Too low firing rate - boost plasticity"
        elif analysis['avg_firing_rate'] > 50.0:
            return "Too high firing rate - focus on stability"
        return "Performance optimization"
    
    def _apply_adaptive_neuromodulation(self, metrics):
        """Apply adaptive neuromodulation based on current performance."""
        if not self.meta_learning_enabled:
            return
        
        # Adaptive neuromodulation logic
        novelty = metrics['novelty_level']
        curiosity = metrics['curiosity_level']
        prediction_error = metrics['prediction_error']
        
        # Adjust neuromodulation based on performance
        if novelty > 0.7 and curiosity < 0.3:
            # High novelty but low curiosity - boost curiosity
            self.config.set("neuromod.curiosity.exploration_rate", 
                          self.config.getOr("neuromod.curiosity.exploration_rate", 0.3) + 0.2)
        
        elif curiosity > 0.8:
            # Too much curiosity - moderate it
            self.config.set("neuromod.curiosity.exploration_rate", 
                          self.config.getOr("neuromod.curiosity.exploration_rate", 0.3) * 0.8)
        
        # Adjust development speed based on learning progress
        if abs(prediction_error) > 0.5:
            # High prediction error - slow development for more learning
            self.config.set("development.stage_duration.initial", 60.0)
        else:
            # Stable learning - normal development
            self.config.set("development.stage_duration.initial", 90.0)
    
    def run_adaptive_simulation(self, steps=500):
        """Run simulation with adaptive learning strategies."""
        print("=== Adaptive Learning Agent Simulation ===")
        print(f"Starting with strategy: {self.current_strategy}")
        print(f"Initial exploration rate: {self.exploration_rate}")
        print(f"Initial plasticity adaptation: {self.plasticity_adaptation}")
        print("-" * 60)
        
        for step in range(steps):
            # Update environment
            self.world.update(0.1)
            
            # Get perception
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Neural computation
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            
            # Apply action to world
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Collect performance metrics
            metrics = self._collect_performance_metrics(step)
            
            # Analyze performance and adapt strategy
            analysis = self._analyze_performance(metrics)
            
            # Apply adaptive neuromodulation
            self._apply_adaptive_neuromodulation(metrics)
            
            # Adapt learning strategy
            self._adapt_strategy(analysis)
            
            # Print progress
            if step % 100 == 0 and step > 0:
                print(f"\nStep {step}: {self.current_strategy} strategy")
                print(f"  Firing rate: {metrics['firing_rate']:.1f} Hz")
                print(f"  Curiosity: {metrics['curiosity_level']:.3f}")
                print(f"  Novelty: {metrics['novelty_level']:.3f}")
                print(f"  Prediction error: {metrics['prediction_error']:.3f}")
                print(f"  Development stage: {metrics['developmental_stage']}")
                print(f"  Exploration rate: {self.exploration_rate:.2f}")
        
        print(f"\n=== Adaptive Simulation Complete ===")
        print(f"Final strategy: {self.current_strategy}")
        print(f"Strategy changes made: {len(self.strategy_history)}")
        
        if self.performance_history:
            final_metrics = self.performance_history[-1]
            print(f"Final performance:")
            print(f"  Firing rate: {final_metrics['firing_rate']:.1f} Hz")
            print(f"  Curiosity: {final_metrics['curiosity_level']:.3f}")
            print(f"  Novelty: {final_metrics['novelty_level']:.3f}")
    
    def get_learning_summary(self):
        """Get comprehensive learning summary."""
        if not self.performance_history:
            return "No learning data available"
        
        final_metrics = self.performance_history[-1]
        initial_metrics = self.performance_history[0]
        
        # Calculate learning improvements
        firing_rate_improvement = final_metrics['firing_rate'] - initial_metrics['firing_rate']
        curiosity_improvement = final_metrics['curiosity_level'] - initial_metrics['curiosity_level']
        novelty_improvement = final_metrics['novelty_level'] - initial_metrics['novelty_level']
        
        return {
            'initial_strategy': 'balanced',
            'final_strategy': self.current_strategy,
            'total_steps': len(self.performance_history),
            'strategy_changes': len(self.strategy_history),
            'learning_improvements': {
                'firing_rate': firing_rate_improvement,
                'curiosity': curiosity_improvement,
                'novelty': novelty_improvement
            },
            'performance_trend': self._calculate_performance_trend()
        }
    
    def _calculate_performance_trend(self):
        """Calculate overall performance trend."""
        if len(self.performance_history) < 10:
            return "insufficient_data"
        
        # Calculate trend using linear regression
        x = list(range(len(self.performance_history)))
        y = [m['firing_rate'] * 0.1 + m['curiosity_level'] * 0.5 + m['novelty_level'] * 0.4 for m in self.performance_history]
        
        # Simple trend calculation
        if len(y) >= 2:
            first_half = sum(y[:len(y)//2]) / (len(y)//2)
            second_half = sum(y[len(y)//2:]) / (len(y)//2)
            
            if second_half > first_half * 1.1:
                return "improving"
            elif second_half < first_half * 0.9:
                return "declining"
            else:
                return "stable"
        
        return "stable"

# Real-world application example
if __name__ == "__main__":
    print("=== Advanced Learning Agent Example ===")
    
    # Create adaptive learning agent
    agent = AdaptiveLearningAgent()
    
    # Run adaptive simulation
    agent.run_adaptive_simulation(steps=500)
    
    # Get learning summary
    summary = agent.get_learning_summary()
    print(f"\nLearning Summary:")
    print(f"  Strategy changes: {summary['strategy_changes']}")
    print(f"  Performance trend: {summary['performance_trend']}")
    print(f"  Firing rate improvement: {summary['learning_improvements']['firing_rate']:.2f}")
    print(f"  Curiosity improvement: {summary['learning_improvements']['curiosity']:.3f}")
    print(f"  Novelty improvement: {summary['learning_improvements']['novelty']:.3f}")
```

# Real-World Application Examples

## 7. NLM in Real-World Scenarios

This section demonstrates how NLM can be applied to real-world problems:

### A. Robot Navigation and Exploration

```python
import pynlm

class RobotNavigationSystem:
    """Robot navigation system using NLM for intelligent movement and exploration."""
    
    def __init__(self, width=50, height=50):
        self.width = width
        self.height = height
        self.robot_config = pynlm.createDefaultConfig()
        self.robot_config.set("brain.neuron_count", 600)
        self.robot_config.set("neuromod.curiosity.enable", True)
        self.robot_config.set("neuromod.novelty.enable", True)
        
        # Initialize robot
        self.create_robot()
    
    def create_robot(self):
        """Create the robot with brain, world, and agent components."""
        self.brain = pynlm.createBrain(self.robot_config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=self.width, height=self.height, visionWidth=10, visionHeight=10)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable navigation-specific systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Set initial position
        self.world.setAgentStart(10.0, 10.0)
        
        # Place obstacles and targets
        self._setup_environment()
    
    def _setup_environment(self):
        """Set up the robot environment with obstacles and targets."""
        # Add obstacles (walls)
        for x in range(20, 30):
            self.world.addObject(pynlm.WorldObject(x, 20, pynlm.WorldObjectType.Wall, radius=0.8))
        
        # Add resource target
        self.world.addObject(pynlm.WorldObject(40.0, 40.0, pynlm.WorldObjectType.Resource, value=100.0))
        
        # Add hazard
        self.world.addObject(pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Hazard, radius=1.0))
    
    def run_navigation_episode(self, max_steps=1000):
        """Run a single navigation episode."""
        print("=== Robot Navigation Episode ===")
        
        steps_without_progress = 0
        target_reached = False
        hazard_hit = False
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get robot's perception
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Neural decision making
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get agent status
            agent_body = self.world.getAgentBody()
            
            # Check for achievements
            if target_reached:
                print(f"Target reached at step {step}")
                break
            
            # Check if hazard was hit
            if agent_body.health <= 0:
                hazard_hit = True
                print(f"Robot hit hazard at step {step}")
                break
            
            # Check if robot reached target area
            target_obj = None
            for obj in self.world.getObjects() if hasattr(self.world, 'getObjects') else []:
                if hasattr(obj, 'type') and obj.type == pynlm.WorldObjectType.Resource:
                    target_obj = obj
                    break
            
            if target_obj and abs(agent_body.x - target_obj.x) < 2.0 and abs(agent_body.y - target_obj.y) < 2.0:
                target_reached = True
                print(f"Robot reached target area at step {step}")
                break
            
            # Check for stagnation
            if step > 10:
                prev_pos = getattr(self, '_prev_position', (agent_body.x, agent_body.y))
                distance_moved = ((agent_body.x - prev_pos[0]) ** 2 + (agent_body.y - prev_pos[1]) ** 2) ** 0.5
                if distance_moved < 0.1:
                    steps_without_progress += 1
                else:
                    steps_without_progress = 0
                self._prev_position = (agent_body.x, agent_body.y)
            
            if steps_without_progress > 100:
                print(f"Robot appears stuck at step {step}")
                break
            
            # Print progress
            if step % 100 == 0 and step > 0:
                print(f"Step {step}: pos=({agent_body.x:.1f},{agent_body.y:.1f}), "
                      f"health={agent_body.health:.1f}, curiosity={self.agent.getCuriosityLevel():.3f}")
        
        # Episode result
        if target_reached:
            print("✓ Navigation successful - Target reached!")
            return "success", step
        elif hazard_hit:
            print("✗ Navigation failed - Robot damaged!")
            return "hazard", step
        else:
            print("✗ Navigation failed - Robot stopped moving")
            return "stuck", step
    
    def run_training_simulation(self, episodes=50):
        """Run multiple training episodes for the robot."""
        print(f"=== Robot Navigation Training ({episodes} episodes) ===")
        
        results = []
        
        for episode in range(episodes):
            # Reset robot for new episode
            self.world.reset()
            self.agent.reset()
            self.brain.reset()
            self.brain.initialize()
            self._setup_environment()
            
            # Run episode
            result, steps = self.run_navigation_episode(max_steps=500)
            results.append(result)
            
            # Enable curiosity more during training
            if episode % 10 == 0:
                self.agent.enableCuriosity(True)
        
        # Calculate statistics
        success_rate = results.count("success") / len(results)
        hazard_rate = results.count("hazard") / len(results)
        stuck_rate = results.count("stuck") / len(results)
        
        print(f"\n=== Training Results ===")
        print(f"Success rate: {success_rate:.2%}")
        print(f"Hazard rate: {hazard_rate:.2%}")
        print(f"Stuck rate: {stuck_rate:.2%}")
        
        return {
            'results': results,
            'success_rate': success_rate,
            'hazard_rate': hazard_rate,
            'stuck_rate': stuck_rate
        }

# Robot navigation example
if __name__ == "__main__":
    # Create navigation system
    navigator = RobotNavigationSystem(width=50, height=50)
    
    # Run training simulation
    training_results = navigator.run_training_simulation(episodes=30)
```

### B. Complex Pattern Recognition System

```python
import pynlm
import time

class PatternRecognitionSystem:
    """Advanced pattern recognition system using NLM for complex feature detection."""
    
    def __init__(self):
        self.config = pynlm.createDefaultConfig()
        self.config.set("brain.neuron_count", 1200)
        self.config.set("brain.region_count", 6)
        self.config.set("plasticity.stdp.enable", True)
        self.config.set("neuromod.curiosity.enable", True)
        self.config.set("neuromod.novelty.enable", True)
        
        # Create recognition system
        self.create_recognizer()
        
        # Pattern database
        self.pattern_database = {
            'simple_pattern': {'id': 1, 'complexity': 0.3, 'recognition_rate': 0.9},
            'medium_pattern': {'id': 2, 'complexity': 0.6, 'recognition_rate': 0.7},
            'complex_pattern': {'id': 3, 'complexity': 0.9, 'recognition_rate': 0.4}
        }
    
    def create_recognizer(self):
        """Create the pattern recognition system."""
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        
        # Set up test patterns
        self._setup_patterns()
    
    def _setup_patterns(self):
        """Set up test patterns for recognition."""
        # Create visual patterns
        patterns = {
            'horizontal_line': [(i, 4) for i in range(8)],
            'vertical_line': [(4, i) for i in range(8)],
            'diagonal_line': [(i, i) for i in range(8)],
            'circle': [(4+i, 4+int((i*0.707)-4)) for i in range(8)],
            'square': [(4,4), (4,6), (6,6), (6,4), (4,4)]
        }
        
        for pattern_name, pattern_coords in patterns.items():
            # Create world object for each pattern
            for x, y in pattern_coords:
                if 0 <= x < self.world.getWidth() and 0 <= y < self.world.getHeight():
                    self.world.addObject(pynlm.WorldObject(
                        x, y, pynlm.WorldObjectType.Marker, radius=0.4
                    ))
    
    def run_pattern_recognition_test(self, steps=800):
        """Run pattern recognition test."""
        print("=== Pattern Recognition System Test ===")
        
        recognition_sessions = 0
        patterns_found = 0
        total_patterns = len(self.pattern_database)
        
        for step in range(steps):
            # Update world
            self.world.update(0.1)
            
            # Get perception
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Neural processing
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Check for pattern completion
            if step % 100 == 0 and step > 0:
                recognition_sessions += 1
                print(f"\nRecognition session {recognition_sessions} (Step {step}):")
                
                # Analyze current state
                agent_body = self.world.getAgentBody()
                perception_quality = self._calculate_perception_quality(percept)
                curiosity_level = self.agent.getCuriosityLevel()
                novelty_level = self.agent.getNoveltyLevel()
                
                print(f"  Agent position: ({agent_body.x:.1f}, {agent_body.y:.1f})")
                print(f"  Perception quality: {perception_quality:.3f}")
                print(f"  Curiosity: {curiosity_level:.3f}")
                print(f"  Novelty: {novelty_level:.3f}")
                
                # Pattern detection logic
                detected_pattern = self._detect_pattern(percept, agent_body)
                if detected_pattern:
                    patterns_found += 1
                    print(f"  ✓ Detected pattern: {detected_pattern}")
        
        # Calculate recognition statistics
        recognition_rate = patterns_found / total_patterns
        print(f"\n=== Pattern Recognition Results ===")
        print(f"Patterns in database: {total_patterns}")
        print(f"Patterns detected: {patterns_found}")
        print(f"Recognition rate: {recognition_rate:.2%}")
        print(f"Total recognition sessions: {recognition_sessions}")
        
        return {
            'patterns_detected': patterns_found,
            'total_patterns': total_patterns,
            'recognition_rate': recognition_rate,
            'sessions': recognition_sessions
        }
    
    def _calculate_perception_quality(self, percept):
        """Calculate the quality of current perception."""
        # Simple perception quality metric based on vision data
        vision = percept.getVision()
        if not vision:
            return 0.0
        
        # Calculate variance in vision data (higher variance = richer perception)
        mean_val = sum(vision) / len(vision)
        variance = sum((x - mean_val) ** 2 for x in vision) / len(vision)
        
        # Normalize to [0, 1]
        quality = min(variance * 10.0, 1.0)
        return quality
    
    def _detect_pattern(self, percept, agent_body):
        """Detect patterns in the current perception."""
        vision = percept.getVision()
        
        if len(vision) != 64:  # 8x8 vision grid
            return None
        
        # Simple pattern detection (8x8 grid)
        grid = [vision[i*8:(i+1)*8] for i in range(8)]
        
        # Check for different pattern types
        if self._detect_horizontal_line(grid):
            return "horizontal_line"
        elif self._detect_vertical_line(grid):
            return "vertical_line"
        elif self._detect_diagonal_line(grid):
            return "diagonal_line"
        elif self._detect_circle(grid):
            return "circle"
        elif self._detect_square(grid):
            return "square"
        
        return None
    
    def _detect_horizontal_line(self, grid):
        """Detect horizontal line pattern."""
        for row in grid:
            if all(abs(val - max(row)) < 0.1 for val in row if val > 0.5):
                return True
        return False
    
    def _detect_vertical_line(self, grid):
        """Detect vertical line pattern."""
        for col in range(8):
            column = [grid[row][col] for row in range(8)]
            if all(abs(val - max(column)) < 0.1 for val in column if val > 0.5):
                return True
        return False
    
    def _detect_diagonal_line(self, grid):
        """Detect diagonal line pattern."""
        # Check main diagonal
        main_diag = [grid[i][i] for i in range(8)]
        if all(abs(val - max(main_diag)) < 0.1 for val in main_diag if val > 0.5):
            return True
        
        # Check anti-diagonal
        anti_diag = [grid[i][7-i] for i in range(8)]
        if all(abs(val - max(anti_diag)) < 0.1 for val in anti_diag if val > 0.5):
            return True
        
        return False
    
    def _detect_circle(self, grid):
        """Detect circle pattern (approximate)."""
        center_x, center_y = 4, 4
        radius = 2
        
        for x in range(8):
            for y in range(8):
                dist = ((x - center_x) ** 2 + (y - center_y) ** 2) ** 0.5
                cell_val = grid[y][x]
                
                if dist < radius - 0.5 and cell_val > 0.5:  # Inside circle
                    pass
                elif dist > radius + 0.5 and cell_val < 0.5:  # Outside circle
                    pass
                elif abs(dist - radius) < 0.5 and cell_val > 0.5:  # Circle edge
                    return True
        
        return False
    
    def _detect_square(self, grid):
        """Detect square pattern."""
        # Check for 2x2 square
        for x in range(6):
            for y in range(6):
                cells = [
                    grid[y][x], grid[y][x+1],
                    grid[y+1][x], grid[y+1][x+1]
                ]
                if all(cell > 0.7 for cell in cells):
                    return True
        return False

# Pattern recognition example
if __name__ == "__main__":
    # Create pattern recognition system
    recognizer = PatternRecognitionSystem()
    
    # Run recognition test
    results = recognizer.run_pattern_recognition_test(steps=600)
```

## 8. Advanced Configuration Management

```python
import pynlm
import json
import os
from pathlib import Path

class AdvancedConfigurationManager:
    """Advanced configuration management for NLM with validation and optimization."""
    
    def __init__(self, config_dir="configs"):
        self.config_dir = Path(config_dir)
        self.config_dir.mkdir(exist_ok=True)
        
        # Configuration templates
        self.templates = {
            'minimal': {
                'brain.neuron_count': 100,
                'brain.synapse_density': 0.02,
                'plasticity.stdp.enable': True,
                'neuromod.curiosity.enable': False
            },
            'balanced': {
                'brain.neuron_count': 500,
                'brain.synapse_density': 0.05,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'development.enable_all': True
            },
            'advanced': {
                'brain.neuron_count': 1500,
                'brain.synapse_density': 0.08,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': True,
                'plasticity.structural.enable': True,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'neuromod.dopamine.scale': 1.5,
                'development.enable_all': True,
                'development.stage_duration.initial': 120.0,
                'performance.enable_vectorization': True,
                'performance.memory_pool_size': "large"
            },
            'production': {
                'brain.neuron_count': 800,
                'brain.synapse_density': 0.06,
                'plasticity.stdp.enable': True,
                'plasticity.hebbian.enable': False,
                'neuromod.curiosity.enable': True,
                'neuromod.novelty.enable': True,
                'development.enable_critical_period': True,
                'performance.checkpoint_interval': 500,
                'performance.simulation_timestep': 0.05,
                'performance.enable_vectorization': True
            }
        }
    
    def create_configuration(self, template_name, custom_settings=None):
        """Create a configuration from a template with custom settings."""
        if template_name not in self.templates:
            raise ValueError(f"Unknown template: {template_name}")
        
        config = pynlm.createDefaultConfig()
        
        # Apply template settings
        for key, value in self.templates[template_name].items():
            # Handle nested keys (e.g., plasticity.stdp.enable)
            if '.' in key:
                # For simplicity, just handle top-level keys
                pass
            else:
                self._set_config_value(config, key, value)
        
        # Apply custom settings
        if custom_settings:
            for key, value in custom_settings.items():
                self._set_config_value(config, key, value)
        
        return config
    
    def _set_config_value(self, config, key, value):
        """Set a configuration value, handling different types."""
        try:
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, value)
            elif isinstance(value, float):
                config.set(key, value)
            elif isinstance(value, str):
                config.set(key, value)
            else:
                # Try to infer type
                if isinstance(value, list):
                    if all(isinstance(x, int) for x in value):
                        config.set(key, value)
                    elif all(isinstance(x, float) for x in value):
                        config.set(key, value)
                    elif all(isinstance(x, str) for x in value):
                        config.set(key, value)
        except Exception as e:
            print(f"Warning: Could not set {key}={value}: {e}")
    
    def validate_configuration(self, config):
        """Validate a configuration for consistency."""
        errors = []
        warnings = []
        
        # Get config values
        neuron_count = config.getOr("brain.neuron_count", 100)
        synapse_density = config.getOr("brain.synapse_density", 0.05)
        
        # Validate neuron count
        if neuron_count < 10:
            warnings.append("Very low neuron count may result in insufficient network dynamics")
        elif neuron_count > 10000:
            warnings.append("Very high neuron count may impact performance")
        
        # Validate synapse density
        if synapse_density < 0.001:
            warnings.append("Very low synapse density may result in disconnected neurons")
        elif synapse_density > 0.5:
            warnings.append("Very high synapse density may cause excessive connectivity")
        
        # Check consistency between settings
        stdp_enabled = config.getOr("plasticity.stdp.enable", True)
        hebbian_enabled = config.getOr("plasticity.hebbian.enable", False)
        
        if not stdp_enabled and not hebbian_enabled:
            warnings.append("Both STDP and Hebbian plasticity are disabled - no learning will occur")
        
        # Check development settings
        dev_enabled = config.getOr("development.enable_all", False)
        dev_critical = config.getOr("development.enable_critical_period", False)
        
        if dev_enabled and not dev_critical:
            warnings.append("Development enabled but critical period disabled - may limit learning")
        
        return {
            'valid': len(errors) == 0,
            'errors': errors,
            'warnings': warnings
        }
    
    def save_configuration(self, config, filename, description=""):
        """Save configuration to file with metadata."""
        # Create configuration metadata
        metadata = {
            'name': filename,
            'description': description,
            'timestamp': time.time(),
            'version': '1.0',
            'created_by': 'AdvancedConfigurationManager'
        }
        
        # Save configuration
        config_file = self.config_dir / f"{filename}.json"
        
        try:
            # Create config object and save
            config.saveToFile(str(config_file))
            
            # Save metadata
            metadata_file = self.config_dir / f"{filename}_metadata.json"
            with open(metadata_file, 'w') as f:
                json.dump(metadata, f, indent=2)
            
            print(f"Configuration saved: {config_file}")
            return True
        except Exception as e:
            print(f"Error saving configuration: {e}")
            return False
    
    def load_configuration(self, filename):
        """Load configuration from file."""
        config_file = self.config_dir / f"{filename}.json"
        
        if not config_file.exists():
            raise FileNotFoundError(f"Configuration file not found: {config_file}")
        
        try:
            config = pynlm.createDefaultConfig()
            if config.loadFromFile(str(config_file)):
                print(f"Configuration loaded: {config_file}")
                return config
            else:
                raise ValueError(f"Failed to load configuration from {config_file}")
        except Exception as e:
            print(f"Error loading configuration: {e}")
            raise
    
    def list_configurations(self):
        """List all saved configurations."""
        configs = []
        
        for config_file in self.config_dir.glob("*.json"):
            if not config_file.name.endswith("_metadata.json"):
                metadata_file = self.config_dir / f"{config_file.stem}_metadata.json"
                
                if metadata_file.exists():
                    with open(metadata_file, 'r') as f:
                        metadata = json.load(f)
                        configs.append(metadata)
                else:
                    configs.append({
                        'name': config_file.stem,
                        'description': "No metadata available",
                        'filename': config_file.name
                    })
        
        return configs

# Configuration management example
if __name__ == "__main__":
    print("=== Advanced Configuration Management ===")
    
    # Create configuration manager
    config_manager = AdvancedConfigurationManager()
    
    # Create configurations from templates
    print("\n1. Creating configurations from templates:")
    
    minimal_config = config_manager.create_configuration("minimal", {
        "brain.neuron_count": 50
    })
    print(f"Minimal config: {minimal_config.getOr('brain.neuron_count', 0)} neurons")
    
    advanced_config = config_manager.create_configuration("advanced")
    print(f"Advanced config: {advanced_config.getOr('brain.neuron_count', 0)} neurons")
    
    production_config = config_manager.create_configuration("production")
    print(f"Production config: {production_config.getOr('brain.neuron_count', 0)} neurons")
    
    # Validate configurations
    print("\n2. Validating configurations:")
    
    for name, config in [("Minimal", minimal_config), ("Advanced", advanced_config), ("Production", production_config)]:
        validation = config_manager.validate_configuration(config)
        print(f"{name}: {'✓ Valid' if validation['valid'] else '✗ Invalid'}")
        if validation['warnings']:
            for warning in validation['warnings']:
                print(f"  Warning: {warning}")
    
    # Save configurations
    print("\n3. Saving configurations:")
    
    config_manager.save_configuration(minimal_config, "robot_navigator", "Configuration for robot navigation")
    config_manager.save_configuration(advanced_config, "pattern_recognizer", "Configuration for pattern recognition")
    config_manager.save_configuration(production_config, "production_use", "Production-ready configuration")
    
    # List saved configurations
    print("\n4. Saved configurations:")
    saved_configs = config_manager.list_configurations()
    
    for config_info in saved_configs:
        print(f"  - {config_info.get('name', 'Unknown')}: {config_info.get('description', 'No description')}")
```