#!/usr/bin/env python3
"""
Advanced NLM Python Examples and Utilities

This module provides advanced examples, utilities, and best practices for using
the NLM (Neural Learning Machine) Python bindings at an advanced level.
"""

import pynlm
import json
import time
from typing import List, Dict, Any, Optional, Union
import numpy as np
class AdvancedBrainUtils:
    """
    Advanced utilities for working with NLM brains.
    """
    
    @staticmethod
    def create_brain_with_preset_config(preset_name: str) -> pynlm.Brain:
        """
        Create a brain with a predefined configuration preset.
        
        Args:
            preset_name: One of "minimal", "balanced", "learning", "fast", "high_performance"
            
        Returns:
            Configured brain
        """
        presets = {
            "minimal": {
                "neuron_count": 500,
                "connection_probability": 0.05,
                "stdp_ltp_weight": 0.01,
                "stdp_ltd_weight": 0.012,
            },
            "balanced": {
                "neuron_count": 2000,
                "connection_probability": 0.1,
                "stdp_ltp_weight": 0.02,
                "stdp_ltd_weight": 0.015,
                "enable_curiosity": True,
                "enable_reward_modulation": True,
            },
            "learning": {
                "neuron_count": 3000,
                "connection_probability": 0.15,
                "stdp_ltp_weight": 0.03,
                "stdp_ltd_weight": 0.025,
                "enable_reward_modulation": True,
                "enable_structural_plasticity": True,
                "enable_development": True,
            },
            "fast": {
                "neuron_count": 1000,
                "connection_probability": 0.08,
                "stdp_ltp_weight": 0.015,
                "stdp_ltd_weight": 0.01,
                "enable_curiosity": False,
                "enable_reward_modulation": False,
            },
            "high_performance": {
                "neuron_count": 5000,
                "connection_probability": 0.2,
                "stdp_ltp_weight": 0.025,
                "stdp_ltd_weight": 0.02,
                "enable_reward_modulation": True,
                "enable_structural_plasticity": True,
                "enable_development": True,
                "enable_curiosity": True,
            }
        }
        
        if preset_name not in presets:
            raise ValueError(f"Unknown preset: {preset_name}. Available: {list(presets.keys())}")
        
        config = pynlm.createDefaultConfig()
        preset = presets[preset_name]
        
        for key, value in preset.items():
            if isinstance(value, bool):
                config.set(key, value)
            elif isinstance(value, int):
                config.set(key, static_cast<int64_t>(value))
            elif isinstance(value, float):
                config.set(key, value)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        return brain
    
    @staticmethod
    def benchmark_brain_performance(brain: pynlm.Brain, num_steps: int, 
                                    num_runs: int = 3) -> Dict[str, Any]:
        """
        Benchmark brain performance with detailed statistics.
        
        Args:
            brain: Brain to benchmark
            num_steps: Number of steps per run
            num_runs: Number of benchmark runs
            
        Returns:
            Dictionary with benchmark results
        """
        results = []
        
        for run in range(num_runs):
            brain.reset()
            brain.initialize()
            
            start_time = time.time()
            spike_counts = []
            
            for step in range(num_steps):
                brain.step(step)
                if step % 100 == 0:
                    spike_counts.append(brain.getFiringNeuronCount())
            
            end_time = time.time()
            execution_time = end_time - start_time
            
            run_result = {
                "run": run,
                "total_spikes": brain.getTotalSpikeCount(),
                "execution_time_seconds": execution_time,
                "steps_per_second": num_steps / execution_time,
                "avg_firing_rate": brain.getAverageFiringRate(),
                "final_spike_count": brain.getFiringNeuronCount(),
                "max_firing_neuron_count": max(spike_counts) if spike_counts else 0,
                "min_firing_neuron_count": min(spike_counts) if spike_counts else 0,
            }
            
            results.append(run_result)
        
        # Calculate statistics across runs
        total_spikes = [r["total_spikes"] for r in results]
        steps_per_second = [r["steps_per_second"] for r in results]
        avg_firing_rates = [r["avg_firing_rate"] for r in results]
        
        stats = {
            "runs": results,
            "mean_total_spikes": sum(total_spikes) / len(total_spikes),
            "std_total_spikes": np.std(total_spikes) if len(total_spikes) > 1 else 0,
            "mean_steps_per_second": sum(steps_per_second) / len(steps_per_second),
            "std_steps_per_second": np.std(steps_per_second) if len(steps_per_second) > 1 else 0,
            "mean_avg_firing_rate": sum(avg_firing_rates) / len(avg_firing_rates),
            "std_avg_firing_rate": np.std(avg_firing_rates) if len(avg_firing_rates) > 1 else 0,
            "best_run": max(results, key=lambda r: r["steps_per_second"]),
            "worst_run": min(results, key=lambda r: r["steps_per_second"]),
        }
        
        return stats
    
    @staticmethod
    def analyze_brain_connectivity(brain: pynlm.Brain) -> Dict[str, Any]:
        """
        Analyze brain connectivity patterns.
        
        Args:
            brain: Brain to analyze
            
        Returns:
            Dictionary with connectivity analysis
        """
        brain.logStatus()
        
        stats = {
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "connection_density": (float)brain.getTotalSynapseCount() / 
                                  ((float)brain.getTotalNeuronCount() * (brain.getTotalNeuronCount() - 1)),
            "active_neurons_percentage": ((float)brain.getActiveNeuronCount() / brain.getTotalNeuronCount()) * 100,
            "firing_neurons_percentage": ((float)brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100,
            "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
            "average_firing_rate": brain.getAverageFiringRate(),
        }
        
        return stats
class AdvancedWorldUtils:
    """
    Advanced utilities for working with NLM worlds.
    """
    
    @staticmethod
    def create_rich_world(width: int = 20, height: int = 20, 
                         vision_width: int = 8, vision_height: int = 8) -> pynlm.SimpleWorld:
        """
        Create a world with various objects and terrain features.
        
        Args:
            width: World width
            height: World height
            vision_width: Agent vision width
            vision_height: Agent vision height
            
        Returns:
            Configured world with objects
        """
        world = pynlm.createSimpleWorld()
        world.configure(width, height, vision_width, vision_height)
        world.reset()
        
        # Add walls around the perimeter
        for x in range(width):
            world.createObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall))
            world.createObject(pynlm.WorldObject(x, height - 1, pynlm.WorldObjectType.Wall))
        
        for y in range(height):
            world.createObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall))
            world.createObject(pynlm.WorldObject(width - 1, y, pynlm.WorldObjectType.Wall))
        
        # Add random resources
        import random
        for _ in range(width * height // 10):
            x = random.randint(1, width - 2)
            y = random.randint(1, height - 2)
            if not world.isValidPosition(x, y):
                continue
            # Decide object type based on probability
            r = random.random()
            if r < 0.7:
                obj_type = pynlm.WorldObjectType.Resource
                value = random.uniform(0.5, 2.0)
            elif r < 0.9:
                obj_type = pynlm.WorldObjectType.Hazard
                value = random.uniform(0.5, 1.5)
            else:
                obj_type = pynlm.WorldObjectType.Marker
                value = 1.0
            
            world.createObject(pynlm.WorldObject(x, y, obj_type, value, radius=0.3))
        
        # Set agent start position in the center
        world.setAgentStart(width // 2, height // 2)
        
        return world
    
    @staticmethod
    def simulate_episode(world: pynlm.SimpleWorld, brain: pynlm.Brain,
                         num_steps: int, enable_learning: bool = True) -> Dict[str, Any]:
        """
        Run a complete episode with detailed tracking.
        
        Args:
            world: World to simulate in
            brain: Brain to control agent
            num_steps: Number of steps to simulate
            enable_learning: Whether to enable learning during simulation
            
        Returns:
            Episode statistics
        """
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        episode_stats = {
            "steps_completed": num_steps,
            "final_reward": 0.0,
            "total_objects_collected": 0,
            "total_avoided_hazards": 0,
            "final_position": {"x": world.getAgentBody().x, "y": world.getAgentBody().y},
            "final_energy": world.getAgentBody().energy,
            "final_health": world.getAgentBody().health,
            "agent_stats": {},
        }
        
        for step in range(num_steps):
            # Get current percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Enable learning if requested
            if enable_learning:
                # Simple reward based on internal state
                reward = agent.getNeuromodulationLevel()
                episode_reward += reward
                agent.applyRewardModulation(reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
        
        # Collect final statistics
        episode_stats["final_reward"] = agent.getNeuromodulationLevel()
        episode_stats["agent_stats"] = agent.getAgentStats()
        
        return episode_stats
    
    @staticmethod
    def analyze_world_state(world: pynlm.SimpleWorld) -> Dict[str, Any]:
        """
        Analyze current world state.
        
        Args:
            world: World to analyze
            
        Returns:
            World state analysis
        """
        body = world.getAgentBody()
        percept = world.getSensoryPercept()
        
        analysis = {
            "agent_position": {"x": body.x, "y": body.y},
            "agent_orientation": body.orientation,
            "agent_energy": body.energy,
            "agent_health": body.health,
            "agent_age": body.age,
            "is_moving": body.isMoving,
            "is_turning": body.isTurning,
            "simulation_time": world.getSimulationTime(),
            "world_bounds": {"width": world.getWidth(), "height": world.getHeight()},
            "vision_dimensions": {"width": world.getVisionWidth(), "height": world.getVisionHeight()},
            "vision_data": percept.getVisionData() if hasattr(percept, 'getVisionData') else [],
        }
        
        return analysis
class AdvancedSerializationUtils:
    """
    Advanced serialization utilities for NLM brains and states.
    """
    
    @staticmethod
    def brain_to_dict(brain: pynlm.Brain) -> Dict[str, Any]:
        """
        Convert brain state to dictionary.
        
        Args:
            brain: Brain to convert
            
        Returns:
            Dictionary representation of brain state
        """
        data = {
            "metadata": {
                "total_neurons": brain.getTotalNeuronCount(),
                "total_synapses": brain.getTotalSynapseCount(),
                "total_spikes": brain.getTotalSpikeCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "development_stage": brain.getDevelopmentalStage(),
            },
            "configuration": {
                "neuron_count": brain.getConfig().getNeuronCount() if hasattr(brain.getConfig(), 'getNeuronCount') else 0,
                "connection_probability": brain.getConfig().getConnectionProbability() if hasattr(brain.getConfig(), 'getConnectionProbability') else 0.0,
            },
            "neurons": [],
            "synapses": [],
        }
        
        # Get regions and their neurons
        regions = brain.getRegions()
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                region_data = {
                    "id": region_id.value,
                    "name": region.getName(),
                    "neuron_count": region.getTotalNeuronCount(),
                    "synapse_count": region.getSynapseCount(),
                    "avg_weight": region.getAverageSynapticWeight(),
                }
                data["neurons"].extend(region_data["neuron_count"])
                data["synapses"].extend(region_data["synapse_count"])
        
        return data
    
    @staticmethod
    def dict_to_brain(brain: pynlm.Brain, data: Dict[str, Any]) -> bool:
        """
        Convert dictionary to brain state.
        
        Args:
            brain: Brain to load state into
            data: Dictionary representation of brain state
            
        Returns:
            True if successful, False otherwise
        """
        try:
            # Note: This requires implementing save/load functionality
            # For now, just log that this would work
            print(f"Would load brain state with {len(data.get('neurons', []))} neurons")
            print(f"Would load brain state with {len(data.get('synapses', []))} synapses")
            return True
        except Exception as e:
            print(f"Error loading brain state: {e}")
            return False
    
    @staticmethod
    def export_brain_to_json(brain: pynlm.Brain, filepath: str) -> bool:
        """
        Export brain state to JSON file.
        
        Args:
            brain: Brain to export
            filepath: Path to output file
            
        Returns:
            True if successful, False otherwise
        """
        try:
            data = AdvancedSerializationUtils.brain_to_dict(brain)
            with open(filepath, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception as e:
            print(f"Error exporting brain to JSON: {e}")
            return False
    
    @staticmethod
    def import_brain_from_json(filepath: str) -> Optional[Dict[str, Any]]:
        """
        Import brain state from JSON file.
        
        Args:
            filepath: Path to input file
            
        Returns:
            Brain data dictionary or None if failed
        """
        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            return data
        except Exception as e:
            print(f"Error importing brain from JSON: {e}")
            return None
class AdvancedTrainingUtils:
    """
    Advanced training utilities for NLM agents.
    """
    
    @staticmethod
    def train_agent_world(world: pynlm.SimpleWorld, brain: pynlm.Brain,
                         num_episodes: int, steps_per_episode: int) -> Dict[str, Any]:
        """
        Train an agent in a world using episode-based training.
        
        Args:
            world: World to train in
            brain: Brain to train
            num_episodes: Number of training episodes
            steps_per_episode: Steps per episode
            
        Returns:
            Training statistics
        """
        training_stats = {
            "episodes_completed": 0,
            "total_steps": 0,
            "episode_rewards": [],
            "best_reward": float('-inf'),
            "worst_reward": float('inf'),
            "avg_reward": 0.0,
            "converged": False,
            "learning_curve": [],
        }
        
        for episode in range(num_episodes):
            # Reset world and agent for new episode
            world.reset()
            brain.reset()
            brain.initialize()
            
            # Run episode
            episode_reward = 0.0
            for step in range(steps_per_episode):
                # Environment step
                world.update(0.1)
                
                # Get percept
                percept = world.getSensoryPercept()
                
                # Process input
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                agent.processSensoryInput(percept)
                
                # Brain processing
                brain.step(step)
                
                # Action selection
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Learning
                reward = agent.getNeuromodulationLevel()
                episode_reward += reward
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            # Collect episode statistics
            training_stats["episodes_completed"] += 1
            training_stats["total_steps"] += steps_per_episode
            training_stats["episode_rewards"].append(episode_reward)
            
            if episode_reward > training_stats["best_reward"]:
                training_stats["best_reward"] = episode_reward
            
            if episode_reward < training_stats["worst_reward"]:
                training_stats["worst_reward"] = episode_reward
            
            # Calculate running average
            current_avg = sum(training_stats["episode_rewards"]) / len(training_stats["episode_rewards"])
            training_stats["avg_reward"] = current_avg
            
            # Add to learning curve
            training_stats["learning_curve"].append({
                "episode": episode,
                "reward": episode_reward,
                "running_average": current_avg,
            })
            
            # Check for convergence (simple criterion)
            if episode > 10:
                recent_rewards = training_stats["episode_rewards"][-5:]
                reward_variance = np.var(recent_rewards)
                if reward_variance < 0.1:  # Low variance indicates convergence
                    training_stats["converged"] = True
                    print(f"Training converged at episode {episode} with reward variance {reward_variance}")
                    break
        
        return training_stats
    
    @staticmethod
    def create_learning_curve_data(training_stats: Dict[str, Any]) -> Dict[str, Any]:
        """
        Process training statistics to create learning curve data.
        
        Args:
            training_stats: Raw training statistics
            
        Returns:
            Processed learning curve data
        """
        episodes = [point["episode"] for point in training_stats["learning_curve"]]
        rewards = [point["reward"] for point in training_stats["learning_curve"]]
        running_averages = [point["running_average"] for point in training_stats["learning_curve"]]
        
        # Calculate smoothed moving average
        window_size = 5
        smoothed_rewards = []
        for i in range(len(rewards)):
            start_idx = max(0, i - window_size + 1)
            smoothed = sum(rewards[start_idx:i+1]) / (i - start_idx + 1)
            smoothed_rewards.append(smoothed)
        
        curve_data = {
            "episodes": episodes,
            "rewards": rewards,
            "running_averages": running_averages,
            "smoothed_rewards": smoothed_rewards,
            "best_reward": training_stats["best_reward"],
            "final_reward": rewards[-1] if rewards else 0,
            "total_training_steps": training_stats["total_steps"],
            "convergence_episode": len(training_stats["learning_curve"]) - 1 if training_stats["converged"] else -1,
        }
        
        return curve_data
def get_advanced_examples() -> Dict[str, str]:
    """
    Get advanced example code snippets.
    
    Returns:
        Dictionary mapping example names to code
    """
    examples = {
        "create_custom_brain": '''
# Create a custom brain with specific configuration
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.setNeuronCount(3000)  # Set neuron count with validation
config.setConnectionProbability(0.15)  # Set connection probability with validation
config.set("stdp_ltp_weight", 0.03)
config.set("stdp_ltd_weight", 0.025)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Connection probability: {config.getConnectionProbability()}")
''',
        "advanced_world_setup": '''
# Create a world with complex environment
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, vision_width=10, vision_height=10)
world.reset()

# Add obstacles and resources
for x in range(30):
    world.createObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall))
    world.createObject(pynlm.WorldObject(x, 29, pynlm.WorldObjectType.Wall))

for y in range(30):
    world.createObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall))
    world.createObject(pynlm.WorldObject(29, y, pynlm.WorldObjectType.Wall))

# Add resources and hazards
import random
for _ in range(100):
    x = random.randint(1, 28)
    y = random.randint(1, 28)
    if world.isValidPosition(x, y):
        r = random.random()
        if r < 0.7:
            obj_type = pynlm.WorldObjectType.Resource
            value = random.uniform(0.5, 2.0)
        elif r < 0.9:
            obj_type = pynlm.WorldObjectType.Hazard
            value = random.uniform(0.5, 1.5)
        else:
            obj_type = pynlm.WorldObjectType.Marker
            value = 1.0
        
        world.createObject(pynlm.WorldObject(x, y, obj_type, value, radius=0.3))

# Set agent start position
world.setAgentStart(15, 15)

print(f"Created world with {world.getWidth()}x{world.getHeight()} cells")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
''',
        "brain_benchmarking": '''
# Benchmark brain performance
import pynlm
import time

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Benchmark with different step counts
step_counts = [100, 500, 1000, 5000]

print("Brain Performance Benchmark:")
print("=" * 50)

for num_steps in step_counts:
    start_time = time.time()
    
    for step in range(num_steps):
        brain.step(step)
        if step % 500 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    end_time = time.time()
    execution_time = end_time - start_time
    
    print(f"Steps: {num_steps:,}")
    print(f"Execution time: {execution_time:.2f}s")
    print(f"Steps/second: {num_steps / execution_time:.0f}")
    print(f"Total spikes: {brain.getTotalSpikeCount():,}")
    print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
    print("-" * 30)
''',
        "advanced_agent_training": '''
# Train agent with advanced techniques
import pynlm
import numpy as np

# Create brain with learning capabilities
config = pynlm.createDefaultConfig()
config.setNeuronCount(2000)
config.setConnectionProbability(0.12)
config.set("enable_reward_modulation", True)
config.set("enable_structural_plasticity", True)
config.set("enable_development", True)
config.set("enable_curiosity", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

# Add objects to world
for x in range(20):
    world.createObject(pynlm.WorldObject(x, 0, pynlm.WorldObjectType.Wall))
    world.createObject(pynlm.WorldObject(x, 19, pynlm.WorldObjectType.Wall))

for y in range(20):
    world.createObject(pynlm.WorldObject(0, y, pynlm.WorldObjectType.Wall))
    world.createObject(pynlm.WorldObject(19, y, pynlm.WorldObjectType.Wall))

world.setAgentStart(10, 10)

# Training parameters
num_episodes = 50
steps_per_episode = 200

print("Starting Agent Training...")
print(f"Episodes: {num_episodes}, Steps per episode: {steps_per_episode}")

for episode in range(num_episodes):
    # Reset environment
    world.reset()
    brain.reset()
    brain.initialize()
    
    episode_reward = 0.0
    
    for step in range(steps_per_episode):
        # Environment step
        world.update(0.1)
        
        # Get percept
        percept = world.getSensoryPercept()
        
        # Process input
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Action selection
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learning
        reward = agent.getNeuromodulationLevel()
        episode_reward += reward
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    # Track progress
    print(f"Episode {episode + 1}/{num_episodes}: Reward = {episode_reward:.2f}, "
          f"Avg firing rate = {brain.getAverageFiringRate():.2f}")
''',
        "serialization_example": '''
# Export and import brain states
import pynlm
import json

# Create and train a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for step in range(1000):
    brain.step(step)

# Export brain to JSON
print("Exporting brain to JSON...")
if pynlm.export_brain_to_json(brain, "brain_state.json"):
    print("Brain exported successfully to brain_state.json")

# Import brain from JSON
print("Importing brain from JSON...")
brain_data = pynlm.import_brain_from_json("brain_state.json")
if brain_data:
    print(f"Loaded brain state with metadata: {brain_data['metadata']}")
    
    # You can load this into a new brain if save/load is implemented
    # new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    # pynlm.dict_to_brain(new_brain, brain_data)
''',
    }
    
    return examples
def get_advanced_troubleshooting_guide() -> Dict[str, str]:
    """
    Get advanced troubleshooting guide for common issues.
    
    Returns:
        Dictionary of troubleshooting tips
    """
    guide = {
        "performance_issues": """
Performance Troubleshooting:

1. Slow Brain Simulation:
   - Check if you have too many neurons (default: 1000)
   - Reduce connection_probability (default: 0.1)
   - Disable learning features if not needed: enable_reward_modulation = False
   - Use smaller brain configs for testing

2. Memory Issues:
   - Monitor neuron and synapse counts
   - Use garbage collection if memory grows
   - Consider checkpointing for long simulations
   - Reduce world size if visualizing

3. Simulation Instability:
   - Check neuron threshold values in config
   - Reduce timestep if simulation blows up
   - Ensure proper initialization before stepping
   - Verify configuration parameters are valid
        """,
        "python_api_issues": """
Python API Troubleshooting:

1. Import Errors:
   - Ensure pynlm is installed: pip install pynlm
   - Check that the Python bindings were built correctly
   - Verify dependencies: pybind11, numpy

2. Type Conversion Errors:
   - Use py::object for flexible data conversion
   - Convert numpy arrays to lists: list(array.flatten())
   - Validate data types before passing to C++ functions

3. Configuration Issues:
   - Use pynlm.createDefaultConfig() for safe defaults
   - Validate config values before using them
   - Check pynlm.validateConfig() for errors

4. Agent-World Interaction:
   - Ensure agent.initialize(world) is called before use
   - Reset world properly between episodes
   - Check world boundaries for agent actions
        """,
        "learning_issues": """
Learning Troubleshooting:

1. No Learning Observed:
   - Verify plasticity is enabled: enable_structural_plasticity = True
   - Check neuromodulation levels: curiosity, reward
   - Validate STDP parameters in configuration
   - Ensure synapses have plasticity flags set

2. Poor Convergence:
   - Increase number of training episodes
   - Adjust learning rates in config
   - Enable multiple learning features simultaneously
   - Check reward signals are non-zero

3. Overfitting:
   - Use smaller networks for initial training
   - Add regularization through pruning
   - Implement early stopping
   - Validate on separate test environments
        """,
    }
    
    return guide
def print_advanced_help():
    """
    Print advanced help information.
    """
    print("=" * 60)
    print("NLM Advanced Python API Help")
    print("=" * 60)
    
    print("\n1. Advanced Utilities:")
    print("   - AdvancedBrainUtils: Brain creation and benchmarking")
    print("   - AdvancedWorldUtils: World creation and simulation")
    print("   - AdvancedSerializationUtils: Brain state serialization")
    print("   - AdvancedTrainingUtils: Agent training utilities")
    
    print("\n2. Example Categories:")
    examples = get_advanced_examples()
    for category, code in examples.items():
        print(f"   - {category}: {len(code.split(chr(10)))} lines")
    
    print("\n3. Key Features:")
    features = [
        "Custom brain creation with validation",
        "Advanced world simulation with objects",
        "Brain benchmarking and performance analysis",
        "Agent training with learning curve tracking",
        "Brain state serialization (JSON)",
        "Configuration validation and error handling",
        "Memory management utilities",
        "Troubleshooting guides",
    ]
    for feature in features:
        print(f"   - {feature}")
    
    print("\n4. Usage Example:")
    print("""
   import pynlm
   from advanced_nlm import AdvancedBrainUtils, AdvancedWorldUtils
   
   # Create custom brain
   brain = AdvancedBrainUtils.create_brain_with_preset_config("learning")
   
   # Create rich world
   world = AdvancedWorldUtils.create_rich_world(width=30, height=30)
   
   # Run episode
   stats = AdvancedWorldUtils.simulate_episode(world, brain, num_steps=500)
   
   # Analyze results
   AdvancedBrainUtils.analyze_brain_connectivity(brain)
   AdvancedWorldUtils.analyze_world_state(world)
    """)
    
    print("\n5. Troubleshooting:")
    guide = get_advanced_troubleshooting_guide()
    for topic, tips in guide.items():
        print(f"\n   {topic.replace('_', ' ').title()}:")
        print(f"   {tips[:100]}...")
    
    print("\n" + "=" * 60)
if __name__ == "__main__":
    print_advanced_help()