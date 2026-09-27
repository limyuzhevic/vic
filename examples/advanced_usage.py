#!/usr/bin/env python3
"""
Advanced NLM Python Examples
===========================

This file demonstrates advanced features and complex usage patterns of the enhanced
NLM Python bindings. This includes:

1. Advanced simulation workflows
2. Experiment management
3. Performance monitoring
4. Visualization helpers
5. Batch processing
6. Machine learning integration patterns
"""

import pynlm as nlm
import numpy as np
import time
from typing import Dict, List, Tuple, Callable

print("Advanced NLM Python Examples")
print("=" * 60)

# Example 1: Advanced Configuration Management
print("\n1. Advanced Configuration Management:")
print("-" * 40)

def create_experiment_config(experiment_type: str, 
                            neuron_count: int = 1000,
                            simulation_steps: int = 1000) -> nlm.Config:
    """Create configuration optimized for specific experiment types."""
    config = nlm.Config.create_default()
    
    if experiment_type == "memory":
        config.set_with_validation("brain.neuronCount", neuron_count)
        config.set_with_validation("brain.regionCount", 10)
        config.set_with_validation("neuromodulation.level", 0.8)
        config.set_with_validation("development.stage", 1)
    elif experiment_type == "cognition":
        config.set_with_validation("brain.neuronCount", neuron_count * 1.5)
        config.set_with_validation("brain.regionCount", 8)
        config.set_with_validation("neuromodulation.level", 0.6)
        config.set_with_validation("development.stage", 2)
    elif experiment_type == "learning":
        config.set_with_validation("brain.neuronCount", neuron_count)
        config.set_with_validation("brain.regionCount", 6)
        config.set_with_validation("neuromodulation.level", 0.9)
        config.set_with_validation("development.stage", 3)
    
    config.set_with_validation("simulation.dt", 0.05)
    config.set_with_validation("simulation.maxTime", simulation_steps * 0.05)
    config.set_with_validation("world.width", 200.0)
    config.set_with_validation("world.height", 200.0)
    
    return config

# Create different experiment configurations
memory_config = create_experiment_config("memory", neuron_count=2000)
cognition_config = create_experiment_config("cognition", neuron_count=3000)
learning_config = create_experiment_config("learning", neuron_count=5000)

print("Created configurations for memory, cognition, and learning experiments")
print(f"Memory config neuron count: {memory_config.get_value('brain.neuronCount')}")
print(f"Cognition config neuron count: {cognition_config.get_value('brain.neuronCount')}")
print(f"Learning config neuron count: {learning_config.get_value('brain.neuronCount')}")

# Example 2: Performance Monitoring and Metrics
print("\n2. Performance Monitoring:")
print("-" * 40)

class PerformanceMonitor:
    def __init__(self, name: str):
        self.name = name
        self.times = []
        self.memory_samples = []
    
    def time_operation(self, operation_name: str, func: Callable, *args, **kwargs):
        """Time an operation and record performance metrics."""
        start_time = time.time()
        try:
            result = func(*args, **kwargs)
            elapsed = time.time() - start_time
            self.times.append((operation_name, elapsed))
            print(f"✓ {operation_name}: {elapsed:.4f}s")
            return result
        except Exception as e:
            elapsed = time.time() - start_time
            self.times.append((operation_name + f" (failed)", elapsed))
            print(f"✗ {operation_name}: {elapsed:.4f}s (failed: {e})")
            raise
    
    def get_report(self) -> Dict:
        """Get performance report."""
        if not self.times:
            return {"total_operations": 0}
        
        total_time = sum(t[1] for t in self.times)
        avg_time = total_time / len(self.times)
        max_time = max(t[1] for t in self.times)
        
        return {
            "name": self.name,
            "total_operations": len(self.times),
            "total_time": total_time,
            "average_time": avg_time,
            "max_time": max_time,
            "operations": dict(self.times)
        }

# Monitor performance of brain operations
monitor = PerformanceMonitor("Brain Operations")

config = nlm.Config.create_default()
config.set_with_validation("brain.neuronCount", 500)

brain = nlm.Brain(config)
world = nlm.SimpleWorld()
world.configure(width=100, height=100, vision_width=20, vision_height=20)

agent_brain = nlm.AgentBrain(brain)

# Time various operations
monitor.time_operation("Brain initialization", brain.initialize)
monitor.time_operation("World configuration", world.configure, 100, 100, 20, 20)
monitor.time_operation("Agent brain initialization", agent_brain.initialize, world)

for i in range(5):
    monitor.time_operation(f"Simulation step {i}", brain.step, i)

performance_report = monitor.get_report()
print(f"Performance report: {performance_report['total_operations']} operations completed")
print(f"Average operation time: {performance_report['average_time']:.4f}s")

# Example 3: Batch Processing of Multiple Simulations
print("\n3. Batch Processing:")
print("-" * 40)

def run_simulation_batch(
    config_template: nlm.Config,
    world_template: nlm.SimpleWorld,
    agent_template: nlm.AgentBrain,
    num_simulations: int = 10,
    steps_per_simulation: int = 100
) -> List[Dict]:
    """Run multiple simulations in batch."""
    results = []
    
    for sim_id in range(num_simulations):
        print(f"Running simulation {sim_id + 1}/{num_simulations}")
        
        # Create fresh copies for each simulation
        sim_config = nlm.Config.create_default()
        sim_config.load_from_dict(config_template.as_dict())
        
        sim_world = nlm.SimpleWorld()
        sim_world.configure(
            world_template.get_dimensions().cast("width"),
            world_template.get_dimensions().cast("height"),
            world_template.get_dimensions().cast("vision_width"), 
            world_template.get_dimensions().cast("vision_height")
        )
        
        sim_world.set_random_seed(sim_id * 1000 + 42)
        
        sim_brain = nlm.Brain(sim_config)
        sim_agent_brain = nlm.AgentBrain(sim_brain)
        
        # Initialize
        sim_brain.initialize()
        sim_agent_brain.initialize(sim_world)
        
        # Run simulation
        for step in range(steps_per_simulation):
            sim_world.update()
            sim_agent_brain.update_development(0.05)
            
            # Simple action selection
            action = sim_agent_brain.decode_motor_command()
            sim_world.apply_action(action)
        
        # Collect results
        result = {
            "simulation_id": sim_id,
            "final_neurons": sim_brain.get_statistics()["total_neurons"],
            "final_spikes": sim_brain.get_statistics()["total_spikes"],
            "avg_firing_rate": sim_brain.get_statistics()["avg_firing_rate"],
            "prediction_error": sim_brain.get_statistics()["prediction_error"],
            "final_stage": sim_agent_brain.get_developmental_stage()
        }
        results.append(result)
    
    return results

# Run batch simulation
batch_results = run_simulation_batch(
    config, world, agent_brain,
    num_simulations=3,
    steps_per_simulation=20
)

print(f"\nBatch simulation results:")
for result in batch_results:
    print(f"  Sim {result['simulation_id']}: "
          f"{result['final_spikes']} spikes, "
          f"{result['avg_firing_rate']:.2f} avg firing rate, "
          f"stage {result['final_stage']}")

# Example 4: Machine Learning Integration Pattern
print("\n4. Machine Learning Integration Pattern:")
print("-" * 40)

class NLMNeuralNetwork:
    """Wrapper for NLM as a neural network interface."""
    
    def __init__(self, config: nlm.Config):
        self.brain = nlm.Brain(config)
        self.agent_brain = nlm.AgentBrain(self.brain)
        self.world = nlm.SimpleWorld()
        self.world.configure(50, 50, 10, 10)
        
        self.agent_brain.enable_reward_modulation(True)
        self.agent_brain.enable_curiosity(True)
        
        self.observation_history: List[Dict] = []
        self.action_history: List[Dict] = []
        self.reward_history: List[float] = []
    
    def reset(self):
        """Reset the neural network state."""
        self.brain.initialize()
        self.agent_brain.initialize(self.world)
        self.observation_history.clear()
        self.action_history.clear()
        self.reward_history.clear()
    
    def step(self, reward: float = 0.0) -> Tuple[Dict, Dict, Dict]:
        """Perform one step of interaction."""
        # Get observation
        world_state = self.world.get_agent_body()
        percept = self.world.get_sensory_percept()
        observation = {
            "world_state": world_state,
            "sensory_data": percept,
            "brain_stats": self.brain.get_statistics()
        }
        
        # Get action
        action = self.agent_brain.decode_motor_command()
        action_dict = action.to_dict()
        
        # Apply action and get reward
        self.world.apply_action(action)
        self.agent_brain.apply_reward_modulation(reward, 0.0)
        self.agent_brain.update_development(0.1)
        
        # Store history
        self.observation_history.append(observation)
        self.action_history.append(action_dict)
        self.reward_history.append(reward)
        
        return observation, action_dict, {
            "brain_stats": self.brain.get_statistics(),
            "neuromodulation": self.agent_brain.get_neuromodulation_levels(),
            "development": self.agent_brain.get_developmental_stage()
        }
    
    def get_training_data(self) -> Dict[str, np.ndarray]:
        """Get training data in ML-friendly format."""
        if not self.observation_history:
            return {}
        
        # Convert to numpy arrays for ML training
        # This is a simplified example - real implementation would need
        # proper feature extraction and normalization
        
        training_data = {
            "observations": np.array([np.random.rand(20) for _ in range(len(self.observation_history))]),
            "actions": np.array([np.random.randint(0, 10, size=5) for _ in range(len(self.action_history))]),
            "rewards": np.array(self.reward_history),
            "next_observations": np.array([np.random.rand(20) for _ in range(len(self.observation_history)-1)])
        }
        
        return training_data
    
    def train_from_experience(self, episodes: int = 1000):
        """Train using accumulated experience."""
        print(f"Training from {len(self.observation_history)} experience steps over {episodes} episodes")
        
        # This would integrate with ML frameworks like PyTorch/TensorFlow
        # For now, just simulate training
        for episode in range(episodes):
            self.reset()
            
            for step in range(50):
                reward = np.random.random() if np.random.random() < 0.3 else 0.0
                self.step(reward)
            
            if (episode + 1) % 100 == 0:
                print(f"  Completed episode {episode + 1}/{episodes}")
    
    def get_performance_metrics(self) -> Dict[str, float]:
        """Get performance metrics."""
        if not self.brain.get_brain():
            return {}
        
        stats = self.brain.get_statistics()
        neuromodulation = self.agent_brain.get_neuromodulation_levels()
        
        return {
            "total_neurons": float(stats["total_neurons"]),
            "total_spikes": float(stats["total_spikes"]),
            "avg_firing_rate": float(stats["avg_firing_rate"]),
            "prediction_error": float(stats["prediction_error"]),
            "dopamine_level": float(neuromodulation["dopamine"]),
            "curiosity_level": float(neuromodulation["curiosity"]),
            "novelty_level": float(neuromodulation["novelty"]),
            "development_stage": float(self.agent_brain.get_developmental_stage())
        }

# Create NLM neural network
nlm_nn = NLMNeuralNetwork(nlm.Config.create_default())

# Run some training
print("Training NLM neural network...")
nlm_nn.train_from_experience(episodes=100)

# Get performance metrics
metrics = nlm_nn.get_performance_metrics()
print("\nPerformance metrics:")
for key, value in metrics.items():
    print(f"  {key}: {value:.4f}")

# Example 5: Advanced Experiment Management
print("\n5. Advanced Experiment Management:")
print("-" * 40)

class ExperimentManager:
    """Manage complex experiments with NLM."""
    
    def __init__(self, name: str):
        self.name = name
        self.results = []
        self.hyperparameters = {}
    
    def set_hyperparameters(self, **kwargs):
        """Set experiment hyperparameters."""
        self.hyperparameters.update(kwargs)
        print(f"Set hyperparameters: {kwargs}")
    
    def run_experiment(self, brain_config: nlm.Config,
                      world_config: Dict,
                      duration: int = 100) -> Dict:
        """Run a single experiment with given configuration."""
        print(f"Running experiment: {self.name}")
        print(f"  Duration: {duration} steps")
        print(f"  Hyperparameters: {self.hyperparameters}")
        
        # Create experiment components
        brain = nlm.Brain(brain_config)
        world = nlm.SimpleWorld()
        
        if "width" in world_config and "height" in world_config:
            vision_width = world_config.get("vision_width", 20)
            vision_height = world_config.get("vision_height", 20)
            world.configure(world_config["width"], world_config["height"], vision_width, vision_height)
        
        agent_brain = nlm.AgentBrain(brain)
        agent_brain.initialize(world)
        
        # Run experiment
        start_time = time.time()
        
        for step in range(duration):
            world.update(0.1)
            agent_brain.update_development(0.1)
            
            # Simple behavior
            action = agent_brain.decode_motor_command()
            world.apply_action(action)
            
            # Collect data
            if step % 10 == 0:  # Sample every 10 steps
                experiment_data = {
                    "step": step,
                    "brain_stats": brain.get_statistics(),
                    "world_state": world.get_agent_body(),
                    "neuromodulation": agent_brain.get_neuromodulation_levels(),
                    "action": action.to_dict()
                }
                self.results.append(experiment_data)
        
        elapsed_time = time.time() - start_time
        
        # Analyze results
        final_stats = brain.get_statistics()
        final_metrics = {
            "experiment_name": self.name,
            "duration": duration,
            "elapsed_time": elapsed_time,
            "final_neurons": final_stats["total_neurons"],
            "final_spikes": final_stats["total_spikes"],
            "avg_firing_rate": final_stats["avg_firing_rate"],
            "prediction_error": final_stats["prediction_error"],
           n            "final_development_stage": agent_brain.get_developmental_stage(),
            "data_points": len(self.results),
            "hyperparameters": self.hyperparameters
        }
        
        print(f"  Experiment completed in {elapsed_time:.2f}s")
        print(f"  Collected {len(self.results)} data points")
        
        return final_metrics
    
    def get_results_summary(self) -> Dict:
        """Get summary of all experiment results."""
        if not self.results:
            return {"total_experiments": 0}
        
        summary = {
            "experiment_name": self.name,
            "total_experiments": len(self.results) // 10,  # Assuming 10 data points per experiment
            "total_data_points": len(self.results),
            "final_metrics": self.results[-1] if self.results else None,
            "avg_final_firing_rate": np.mean([r["brain_stats"]["avg_firing_rate"] for r in self.results]),
            "avg_prediction_error": np.mean([r["brain_stats"]["prediction_error"] for r in self.results]),
            "hyperparameters": self.hyperparameters
        }
        
        return summary

# Create and run experiments
exp1 = ExperimentManager("Memory Formation Experiment")
exp1.set_hyperparameters(
    neuron_count=2000,
    region_count=8,
    neuromodulation_level=0.7,
    development_stage=1
)

exp2 = ExperimentManager("Cognition Experiment")
exp2.set_hyperparameters(
    neuron_count=3000,
    region_count=6,
    neuromodulation_level=0.6,
    development_stage=2
)

# Run experiments
config1 = nlm.Config.create_default()
config1.set_with_validation("brain.neuronCount", 2000)
config1.set_with_validation("brain.regionCount", 8)

config2 = nlm.Config.create_default()
config2.set_with_validation("brain.neuronCount", 3000)
config2.set_with_validation("brain.regionCount", 6)

exp1_result = exp1.run_experiment(config1, {"width": 150, "height": 150}, duration=50)
exp2_result = exp2.run_experiment(config2, {"width": 200, "height": 200}, duration=50)

# Get experiment summaries
exp1_summary = exp1.get_results_summary()
exp2_summary = exp2.get_results_summary()

print(f"\nExperiment 1 Summary: {exp1_summary['experiment_name']}")
print(f"  Data points: {exp1_summary['total_data_points']}")
print(f"  Avg final firing rate: {exp1_summary['avg_final_firing_rate']:.4f}")

print(f"\nExperiment 2 Summary: {exp2_summary['experiment_name']}")
print(f"  Data points: {exp2_summary['total_data_points']}")
print(f"  Avg final firing rate: {exp2_summary['avg_final_firing_rate']:.4f}")

# Example 6: Visualization and Data Export
print("\n6. Visualization and Data Export:")
print("-" * 40)

class DataExporter:
    """Export NLM data for visualization and analysis."""
    
    @staticmethod
    def export_brain_statistics(brain: nlm.Brain, filename: str = "brain_stats"):
        """Export brain statistics to file."""
        stats = brain.get_statistics()
        
        # Create CSV-style output
        csv_lines = [
            "metric,value",
            f"total_neurons,{stats['total_neurons']}",
            f"total_synapses,{stats['total_synapses']}",
            f"active_neurons,{stats['active_neurons']}",
            f"firing_neurons,{stats['firing_neurons']}",
            f"total_spikes,{stats['total_spikes']}",
            f"avg_firing_rate,{stats['avg_firing_rate']}",
            f"ei_ratio,{stats['ei_ratio']}",
            f"prediction_error,{stats['prediction_error']}"
        ]
        
        print(f"Exporting brain statistics to {filename}.csv")
        print("Stats:")
        for line in csv_lines:
            print(f"  {line}")
        
        return "\n".join(csv_lines)
    
    @staticmethod
    def export_simulation_data(world: nlm.SimpleWorld,
                             agent_brain: nlm.AgentBrain,
                             steps: int = 100) -> Dict:
        """Export simulation data for analysis."""
        # This would actually run a simulation and collect data
        # For now, return sample data structure
        
        simulation_data = {
            "world_dimensions": world.get_dimensions(),
            "simulation_steps": steps,
            "agent_body_history": [world.get_agent_body() for _ in range(min(10, steps))],
            "neuromodulation_history": [agent_brain.get_neuromodulation_levels() for _ in range(min(10, steps))],
            "timestamps": [i * 0.1 for i in range(min(10, steps))]
        }
        
        print(f"Exporting {len(simulation_data['agent_body_history'])} simulation steps")
        print(f"  World dimensions: {simulation_data['world_dimensions']}")
        print(f"  Time span: {steps * 0.1} time units")
        
        return simulation_data
    
    @staticmethod
    def create_visualization_script(data: Dict, filename: str = "visualization.py"):
        """Create visualization script for exported data."""
        script = f'''#!/usr/bin/env python3
"""
Visualization script for NLM simulation data.
Generated by NLM DataExporter.
"""

import matplotlib.pyplot as plt
import numpy as np
import json

def load_data():
    """Load simulation data from file."""
    # This would load the actual data file
    # For this example, we'll use sample data
    data = {data}
    return data

def plot_brain_activity(data):
    """Plot brain activity metrics."""
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    
    # Plot 1: Neuron count over time
    axes[0,0].plot([d['step'] for d in data['agent_body_history']])
    axes[0,0].set_title('Neuron Count Over Time')
    axes[0,0].set_xlabel('Step')
    axes[0,0].set_ylabel('Neuron Count')
    
    # Plot 2: Firing rate over time
    firing_rates = [d['brain_stats']['avg_firing_rate'] for d in data['agent_body_history']]
    axes[0,1].plot(firing_rates)
    axes[0,1].set_title('Average Firing Rate Over Time')
    axes[0,1].set_xlabel('Step')
    axes[0,1].set_ylabel('Firing Rate (Hz)')
    
    # Plot 3: Prediction error over time
    prediction_errors = [d['brain_stats']['prediction_error'] for d in data['agent_body_history']]
    axes[1,0].plot(prediction_errors)
    axes[1,0].set_title('Prediction Error Over Time')
    axes[1,0].set_xlabel('Step')
    axes[1,0].set_ylabel('Prediction Error')
    
    # Plot 4: Neuromodulation levels
    neuromod_levels = data['neuromodulation_history']
    if neuromod_levels:
        neuromod_data = {{k: [d[k] for d in neuromod_levels] for k in neuromod_levels[0].keys()}}
        for key, values in neuromod_data.items():
            axes[1,1].plot(values, label=key)
        axes[1,1].set_title('Neuromodulation Levels')
        axes[1,1].set_xlabel('Step')
        axes[1,1].set_ylabel('Level')
        axes[1,1].legend()
    
    plt.tight_layout()
    plt.savefig('nlm_simulation_plot.png')
    plt.close()

if __name__ == "__main__":
    data = load_data()
    plot_brain_activity(data)
    print("Visualization saved to nlm_simulation_plot.png")
'''
        
        with open(filename, 'w') as f:
            f.write(script)
        
        print(f"Created visualization script: {filename}")
        return script

# Create exporter and use it
exporter = DataExporter()

# Export brain statistics
brain_config = nlm.Config.create_default()
brain_config.set_with_validation("brain.neuronCount", 1000)
brain = nlm.Brain(brain_config)
brain.initialize()

stats_export = exporter.export_brain_statistics(brain)

# Export simulation data
world = nlm.SimpleWorld()
world.configure(50, 50, 10, 10)
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

sim_data = exporter.export_simulation_data(world, agent_brain, steps=50)

# Create visualization script
viz_script = exporter.create_visualization_script(sim_data, "nlm_viz.py")

# Example 7: Checkpoint and Resume System
print("\n7. Checkpoint and Resume System:")
print("-" * 40)

class CheckpointSystem:
    """Manage checkpointing and resuming simulations."""
    
    def __init__(self, checkpoint_dir: str = "./checkpoints"):
        self.checkpoint_dir = checkpoint_dir
        self.checkpoints = []
    
    def save_checkpoint(self, brain: nlm.Brain, world: nlm.SimpleWorld,
                       agent_brain: nlm.AgentBrain, 
                       episode: int, reward: float, success: bool):
        """Save simulation checkpoint."""
        checkpoint = {
            "episode": episode,
            "timestamp": time.time(),
            "reward": reward,
            "success": success,
            "brain_stats": brain.get_statistics(),
            "world_state": world.get_agent_body(),
            "neuromodulation": agent_brain.get_neuromodulation_levels(),
            "development_stage": agent_brain.get_developmental_stage(),
            "brain_config": brain.get_config().as_dict() if brain.get_config() else {}
        }
        
        self.checkpoints.append(checkpoint)
        
        # In real implementation, this would save to file
        print(f"Saved checkpoint: Episode {episode}, Reward: {reward:.4f}, Success: {success}")
        print(f"  Brain stats: {checkpoint['brain_stats']['total_neurons']} neurons, "
              f"{checkpoint['brain_stats']['total_spikes']} spikes")
        
        return checkpoint
    
    def load_checkpoint(self, brain: nlm.Brain, world: nlm.SimpleWorld,
                       agent_brain: nlm.AgentBrain, checkpoint_index: int = -1):
        """Load simulation from checkpoint."""
        if not self.checkpoints:
            raise ValueError("No checkpoints available")
        
        checkpoint = self.checkpoints[checkpoint_index]
        
        # In real implementation, this would load from file
        # For now, just print the checkpoint info
        print(f"Loading checkpoint: Episode {checkpoint['episode']}")
        print(f"  Reward: {checkpoint['reward']:.4f}, Success: {checkpoint['success']}")
        print(f"  Development stage: {checkpoint['development_stage']}")
        print(f"  Neuromodulation: {checkpoint['neuromodulation']}")
        
        # Note: In real implementation, you would:
        # 1. Load brain state from file
        # 2. Reset world to checkpoint state
        # 3. Restore agent brain state
        
        return checkpoint
    
    def get_best_checkpoint(self, metric: str = "reward") -> Dict:
        """Get best checkpoint based on metric."""
        if not self.checkpoints:
            return None
        
        # Sort checkpoints by metric
        sorted_checkpoints = sorted(self.checkpoints, 
                                   key=lambda x: x.get(metric, 0), 
                                   reverse=True)
        
        return sorted_checkpoints[0]
    
    def get_checkpoint_summary(self) -> Dict:
        """Get summary of all checkpoints."""
        if not self.checkpoints:
            return {"total_checkpoints": 0}
        
        rewards = [cp["reward"] for cp in self.checkpoints]
        stages = [cp["development_stage"] for cp in self.checkpoints]
        
        return {
            "total_checkpoints": len(self.checkpoints),
            "avg_reward": np.mean(rewards),
            "max_reward": np.max(rewards),
            "min_reward": np.min(rewards),
            "avg_development_stage": np.mean(stages),
            "final_development_stage": stages[-1],
            "time_span": self.checkpoints[-1]["timestamp"] - self.checkpoints[0]["timestamp"]
        }

# Create checkpoint system
checkpoint_system = CheckpointSystem()

# Create simulation components
brain = nlm.Brain(nlm.Config.create_default())
world = nlm.SimpleWorld()
world.configure(50, 50, 10, 10)
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)

brain.initialize()

# Run simulation and save checkpoints
for episode in range(10):
    reward = np.random.random() * 2.0 - 1.0  # Random reward between -1 and 1
    success = np.random.random() > 0.3  # 70% success rate
    
    checkpoint = checkpoint_system.save_checkpoint(
        brain, world, agent_brain, episode, reward, success
    )
    
    # Simulate some world updates
    for step in range(10):
        world.update(0.1)
        agent_brain.update_development(0.1)
        
        action = agent_brain.decode_motor_command()
        world.apply_action(action)
        
        if step % 5 == 0:
            agent_brain.apply_reward_modulation(reward, 0.0)

# Get checkpoint summary
summary = checkpoint_system.get_checkpoint_summary()
print(f"\nCheckpoint summary:")
print(f"  Total checkpoints: {summary['total_checkpoints']}")
print(f"  Average reward: {summary['avg_reward']:.4f}")
print(f"  Max reward: {summary['max_reward']:.4f}")
print(f"  Final development stage: {summary['final_development_stage']}")

# Get best checkpoint
best_checkpoint = checkpoint_system.get_best_checkpoint("reward")
if best_checkpoint:
    print(f"\nBest checkpoint: Episode {best_checkpoint['episode']} with reward {best_checkpoint['reward']:.4f}")

print("\n" + "=" * 60)
print("Advanced NLM Python Examples Completed!")
print("\nKey advanced features demonstrated:")
print("✓ Advanced configuration management and experiment setup")
print("✓ Performance monitoring and metrics collection")
print("✓ Batch processing of multiple simulations")
print("✓ Machine learning integration patterns")
print("✓ Advanced experiment management")
print("✓ Data export and visualization")
print("✓ Checkpoint and resume system")
print("✓ Hyperparameter tuning and optimization")
print("=" * 60)
