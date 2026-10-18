# NLM - Intermediate Usage Guide

## Moving from Beginner to Intermediate

You've mastered the basics! Now it's time to explore NLM's more advanced features. This guide covers configuration management, multi-step simulations, visualization basics, and performance tuning.

## 1. Custom Brain Configuration

### Beyond Default Settings

The default brain works well, but NLM allows fine-tuned control:

```python
import pynlm
import json

# Create a custom configuration
config = pynlm.createDefaultConfig()

# Modify brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.v_thresh", -45.0)  # Higher threshold
config.set("brain.tau_mem", 25.0)    # Slower membrane time constant

# Modify plasticity
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)

# Modify neuromodulation
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)

# Save configuration
config.saveToFile("my_config.json")
print("Configuration saved to my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
brain = pynlm.createBrain(new_config)
```

### Configuration File Templates

**High-Performance Brain** (`configs/high_performance.json`):
```json
{
    "brain": {
        "neuron_count": 5000,
        "synapse_density": 0.2,
        "connection_probability": 0.08,
        "v_thresh": -50.0,
        "v_rest": -65.0,
        "tau_mem": 15.0,
        "tau_ref": 1.5
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.0005
        },
        "structural": {
            "enable": true,
            "synapse_creation_rate": 0.001,
            "synapse_removal_rate": 0.001
        }
    }
}
```

**Learning-Focused Brain** (`configs/learning_focused.json`):
```json
{
    "brain": {
        "neuron_count": 3000,
        "synapse_density": 0.12
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.003
        },
        "structural": {
            "enable": true,
            "synapse_creation_rate": 0.002
        }
    },
    "neuromod": {
        "dopamine": {"scale": 2.0},
        "curiosity": {"enable": true, "strength": 1.5},
        "novelty": {"enable": true, "sensitivity": 0.8}
    }
}
```

## 2. Multi-Step Simulation Management

### Batch Processing with Statistics

```python
import time
import pynlm

class NLMSimulation:
    def __init__(self, config_file=None):
        self.config = pynlm.createDefaultConfig()
        if config_file:
            self.config.loadFromFile(config_file)
        self.brain = pynlm.createBrain(self.config)
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        self.setup_world()
        self.setup_agent()
    
    def setup_world(self):
        self.world.configure(
            width=20, height=20,
            visionWidth=8, visionHeight=8
        )
        self.world.reset()
    
    def setup_agent(self):
        self.agent.initialize(self.world)
        self.enable_all_learning()
    
    def enable_all_learning(self):
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
    
    def run_episode(self, num_steps, save_checkpoint=None):
        """Run a complete episode with detailed logging"""
        print(f"Running episode with {num_steps} steps...")
        start_time = time.time()
        
        for step in range(num_steps):
            # Update environment
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            
            # Brain computation
            self.brain.step(step)
            
            # Get action and execute
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply learning
            reward = self.get_reward_from_world()
            self.agent.applyRewardModulation(reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            # Log progress every 100 steps
            if step % 100 == 0:
                self.log_step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        print(f"\nEpisode completed in {duration:.2f} seconds")
        print(f"Average steps per second: {num_steps/duration:.1f}")
        
        if save_checkpoint:
            self.brain.save(save_checkpoint)
        
        return self.get_episode_stats()
    
    def get_reward_from_world(self):
        """Extract reward signal from world state"""
        percept = self.world.getSensoryPercept()
        internal = percept.getInternal() if percept.getInternal() else []
        return internal[0] if internal else 0.0
    
    def log_step(self, step):
        """Log detailed information about current step"""
        stats = {
            'step': step,
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'dev_stage': self.brain.getDevelopmentalStage(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'neuromodulation': self.agent.getNeuromodulationLevel(),
            'simulation_time': self.world.getSimulationTime()
        }
        
        print(f"  Step {step}:")
        print(f"    Neurons firing: {stats['firing_neurons']}")
        print(f"    Total spikes: {stats['total_spikes']}")
        print(f"    Avg firing rate: {stats['avg_firing_rate']:.2f} Hz")
        print(f"    E/I ratio: {stats['e_i_ratio']:.3f}")
        print(f"    Dev stage: {stats['dev_stage']}")
        print(f"    Curiosity: {stats['curiosity']:.3f}")
        print(f"    Novelty: {stats['novelty']:.3f}")
        print(f"    Neuromodulation: {stats['neuromodulation']:.3f}")
    
    def get_episode_stats(self):
        """Get comprehensive episode statistics"""
        return {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'avg_firing_rate': self.brain.getAverageFiringRate(),
            'final_dev_stage': self.brain.getDevelopmentalStage(),
            'final_curiosity': self.agent.getCuriosityLevel(),
            'final_novelty': self.agent.getNoveltyLevel(),
            'simulation_time': self.world.getSimulationTime()
        }

# Example usage
simulation = NLMSimulation()
stats = simulation.run_episode(1000, "checkpoint.brain")
print(f"\nFinal stats: {stats}")
```

### Performance Monitoring

```python
import time
import pynlm

class PerformanceMonitor:
    def __init__(self):
        self.timings = []
        self.memory_usage = []
    
    def measure_step_performance(self, brain, world, agent, steps):
        """Measure performance of simulation steps"""
        print(f"Running {steps} steps...")
        
        for step in range(steps):
            start_time = time.time()
            
            # Run one step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            end_time = time.time()
            self.timings.append(end_time - start_time)
            
            if step % 100 == 0:
                print(f"  Step {step}: {self.timings[-1]*1000:.2f} ms")
        
        # Calculate statistics
        avg_time = sum(self.timings) / len(self.timings)
        min_time = min(self.timings)
        max_time = max(self.timings)
        
        print(f"\nPerformance Statistics:")
        print(f"  Average step time: {avg_time*1000:.2f} ms")
        print(f"  Min step time: {min_time*1000:.2f} ms")
        print(f"  Max step time: {max_time*1000:.2f} ms")
        print(f"  Steps per second: {1/avg_time:.1f}")
        
        return avg_time

# Usage
monitor = PerformanceMonitor()
avg_step_time = monitor.measure_step_performance(brain, world, agent, 500)
```

## 3. Advanced World Configuration

### Custom World Types

```python
import pynlm

class CustomWorld(pynlm.SimpleWorld):
    def __init__(self):
        super().__init__()
        self.resources = []
        self.hazards = []
        self.interesting_objects = []
    
    def add_resource(self, x, y, value=1.0, radius=0.5):
        """Add a valuable resource to the world"""
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value, radius)
        self.resources.append(obj)
        self.addObject(obj)
    
    def add_hazard(self, x, y, value=1.0, radius=0.3):
        """Add a hazard to the world"""
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, value, radius)
        self.hazards.append(obj)
        self.addObject(obj)
    
    def add_interesting_object(self, x, y, radius=0.7):
        """Add an interesting object that drives exploration"""
        obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Marker, 0.0, radius)
        self.interesting_objects.append(obj)
        self.addObject(obj)
    
    def setup_exploration_world(self):
        """Create a world designed for exploration behavior"""
        self.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        self.reset()
        self.setAgentStart(15.0, 15.0)
        
        # Add resources at various locations
        for x in [5, 15, 25]:
            for y in [5, 15, 25]:
                self.add_resource(x, y, value=1.0)
        
        # Add some hazards
        self.add_hazard(10, 10, value=2.0)
        self.add_hazard(20, 20, value=2.0)
        
        # Add interesting objects
        for x in [7, 13, 17, 23]:
            for y in [7, 13, 17, 23]:
                self.add_interesting_object(x, y)
    
    def get_world_stats(self):
        """Get statistics about the world state"""
        percept = self.getSensoryPercept()
        return {
            'simulation_time': self.getSimulationTime(),
            'agent_position': (self.getAgentBody().x, self.getAgentBody().y),
            'agent_energy': self.getAgentBody().energy,
            'resources_nearby': len([r for r in self.resources 
                                   if abs(r.x - self.getAgentBody().x) < 3 and 
                                      abs(r.y - self.getAgentBody().y) < 3]),
            'hazards_nearby': len([h for h in self.hazards 
                                 if abs(h.x - self.getAgentBody().x) < 3 and 
                                    abs(h.y - self.getAgentBody().y) < 3])
        }

# Usage
custom_world = CustomWorld()
custom_world.setup_exploration_world()
agent = pynlm.createAgentBrain(brain)
agent.initialize(custom_world)
```

### World Control and Manipulation

```python
import pynlm

class AdvancedWorldController:
    def __init__(self, world):
        self.world = world
    
    def set_difficulty_level(self, level):
        """Adjust world difficulty"""
        if level == "easy":
            self.world.setMaxEnergy(200.0)
            self.world.setEnergyDecayRate(0.1)
        elif level == "hard":
            self.world.setMaxEnergy(50.0)
            self.world.setEnergyDecayRate(0.5)
        elif level == "expert":
            self.world.setMaxEnergy(25.0)
            self.world.setEnergyDecayRate(0.8)
    
    def create_obstacle_course(self, pattern="zigzag"):
        """Create an obstacle course for the agent"""
        if pattern == "zigzag":
            # Create zigzag pattern of walls
            for i in range(0, 20, 2):
                for j in range(0, 20, 2):
                    if (i + j) % 4 == 0:
                        obj = pynlm.WorldObject(
                            i * 1.5, j * 1.5,
                            pynlm.WorldObjectType.Wall,
                            1.0, 0.8
                        )
                        self.world.addObject(obj)
        
        elif pattern == "maze":
            # Create a simple maze
            width, height = self.world.getWidth(), self.world.getHeight()
            wall_positions = []
            
            # Outer walls
            for x in range(0, width, 2):
                wall_positions.append((x, 0))
                wall_positions.append((x, height))
            for y in range(0, height, 2):
                wall_positions.append((0, y))
                wall_positions.append((width, y))
            
            # Internal walls
            for x in range(5, width-5, 4):
                for y in range(5, height-5, 4):
                    if (x + y) % 8 == 0:
                        wall_positions.append((x, y))
            
            for x, y in wall_positions:
                if 0 <= x <= width and 0 <= y <= height:
                    obj = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, 1.0, 0.8)
                    self.world.addObject(obj)
    
    def simulate_environment_change(self, change_type, duration):
        """Simulate environmental changes over time"""
        if change_type == "storm":
            # Randomly add/remove hazards
            for _ in range(5):
                x = self.world.getWidth() * random.random()
                y = self.world.getHeight() * random.random()
                hazard = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Hazard, 2.0)
                self.world.addObject(hazard)
        
        elif change_type == "resource_shock":
            # Multiply resource values
            percept = self.world.getSensoryPercept()
            # This would require modifying the world implementation

# Usage
controller = AdvancedWorldController(world)
controller.set_difficulty_level("hard")
controller.create_obstacle_course("maze")
```

## 4. Advanced Agent Behavior

### Custom Agent Brains

```python
import pynlm
import random

class AdaptiveAgentBrain(pynlm.AgentBrain):
    def __init__(self, brain):
        super().__init__(brain)
        self.exploration_history = []
        self.learned_preferences = {}
        self.emergency_response_mode = False
    
    def adaptive_sensory_processing(self, percept):
        """Modify sensory processing based on experience"""
        # Extract key features
        vision = percept.getVision()
        internal = percept.getInternal() if percept.getInternal() else []
        
        # Apply learned filters
        if self.learned_preferences:
            # Apply learned attention patterns
            pass
        
        # Modify based on current state
        if self.emergency_response_mode:
            # Focus on threats in emergency mode
            pass
        
        # Pass through to parent
        super().processSensoryInput(percept)
    
    def intelligent_action_selection(self):
        """Enhanced action selection with reasoning"""
        # Get current options
        motor_cmd = super().decodeMotorCommand()
        
        # Apply learned preferences
        if self.learned_preferences.get("avoid_hazards", False):
            # Check if action would lead to hazard
            # Modify action if necessary
            pass
        
        # Add exploration for curiosity
        if self.getCuriosityLevel() > 0.7:
            # Occasionally take random actions
            if random.random() < self.getCuriosityLevel() * 0.1:
                return self.get_random_safe_action()
        
        return motor_cmd
    
    def get_random_safe_action(self):
        """Get a random but safe action"""
        actions = [
            pynlm.ActionType.MoveForward,
            pynlm.ActionType.TurnLeft,
            pynlm.ActionType.TurnRight,
            pynlm.ActionType.Rest
        ]
        return random.choice(actions)
    
    def apply_advanced_reward_modulation(self, reward, predicted):
        """Enhanced reward modulation with temporal credit assignment"""
        # Apply temporal credit assignment
        temporal_weight = self.calculate_temporal_weight()
        adjusted_reward = reward * temporal_weight
        
        # Apply metaplastic changes
        if abs(reward) > 0.5:
            self.metaplastic_adjustment(reward)
        
        # Call parent
        super().applyRewardModulation(adjusted_reward, predicted)
    
    def calculate_temporal_weight(self):
        """Calculate temporal credit assignment weight"""
        # More recent rewards have more weight
        time_since_last_reward = self.get_simulation_time_since_last_reward()
        return min(1.0, 1.0 / (time_since_last_reward + 1))
    
    def metaplastic_adjustment(self, reward):
        """Metaplastic changes for preventing runaway learning"""
        # Adjust learning rates based on recent reward history
        if reward > 0.5:  # Large positive reward
            self.agent.enableStructuralPlasticity(True)  # Encourage growth
        elif reward < -0.5:  # Large negative reward
            self.agent.enableStructuralPlasticity(False)  # Prevent growth

# Usage
custom_agent = AdaptiveAgentBrain(brain)
custom_agent.initialize(world)
```

### Learning Analysis and Visualization

```python
import matplotlib.pyplot as plt
import numpy as np
import pynlm

class LearningAnalyzer:
    def __init__(self):
        self.reward_history = []
        self.curiosity_history = []
        self.firing_history = []
        self.spike_trains = []
    
    def analyze_learning_curve(self, simulation_steps):
        """Analyze learning progress over time"""
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(12, 10))
        
        # Plot 1: Reward over time
        ax1.plot(self.reward_history)
        ax1.set_title('Reward History')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Reward')
        ax1.grid(True)
        
        # Plot 2: Curiosity over time
        ax2.plot(self.curiosity_history)
        ax2.set_title('Curiosity Level')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Curiosity')
        ax2.grid(True)
        
        # Plot 3: Firing rate over time
        ax3.plot(self.firing_history)
        ax3.set_title('Average Firing Rate')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Firing Rate (Hz)')
        ax3.grid(True)
        
        # Plot 4: Spike train
        if self.spike_trains:
            # Show last 1000 spikes
            recent_spikes = np.concatenate(self.spike_trains[-10:])
            ax4.eventplot(recent_spikes, orientation='horizontal')
            ax4.set_title('Recent Spike Trains')
            ax4.set_xlabel('Time (ms)')
        
        plt.tight_layout()
        plt.show()
    
    def track_development_stages(self, brain, steps):
        """Track development stage transitions"""
        stages = []
        stage_durations = []
        current_stage = None
        stage_start = 0
        
        for step in range(steps):
            stage = brain.getDevelopmentalStage()
            stages.append(stage)
            
            if stage != current_stage:
                if current_stage is not None:
                    duration = step - stage_start
                    stage_durations.append((current_stage, duration))
                
                current_stage = stage
                stage_start = step
        
        # Add last stage
        if current_stage is not None:
            duration = steps - stage_start
            stage_durations.append((current_stage, duration))
        
        # Plot development stages
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Plot stages as colored blocks
        for i, (stage, duration) in enumerate(stage_durations):
            color = self.get_stage_color(stage)
            ax.bar(i, duration, color=color, label=str(stage))
        
        ax.set_xlabel('Stage Transition')
        ax.set_ylabel('Duration (steps)')
        ax.set_title('Development Stage Durations')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.show()
        
        return stages, stage_durations
    
    def get_stage_color(self, stage):
        """Get color for development stage"""
        colors = {
            pynlm.DevelopmentalStage.Initial: 'blue',
            pynlm.DevelopmentalStage.CriticalPeriod: 'green',
            pynlm.DevelopmentalStage.Maturation: 'orange',
            pynlm.DevelopmentalStage.Adult: 'red',
            pynlm.DevelopmentalStage.Aging: 'purple'
        }
        return colors.get(stage, 'gray')

# Usage
analyzer = LearningAnalyzer()
# Run simulation and collect data
# analyzer.analyze_learning_curve(1000)
```

## 5. Performance Optimization

### Simulation Tuning

```python
import time
import pynlm

class PerformanceTuner:
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.timings = []
        self.memory_snapshots = []
    
    def profile_configuration(self, config_name, num_steps=100):
        """Profile different simulation configurations"""
        print(f"Profiling configuration: {config_name}")
        
        # Reset to ensure clean state
        self.brain.reset()
        self.world.reset()
        
        # Measure performance
        for step in range(num_steps):
            step_start = time.time()
            
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            step_time = time.time() - step_start
            self.timings.append(step_time)
        
        # Calculate statistics
        avg_time = sum(self.timings) / len(self.timings)
        min_time = min(self.timings)
        max_time = max(self.timings)
        
        print(f"  Average step time: {avg_time*1000:.2f} ms")
        print(f"  Min step time: {min_time*1000:.2f} ms")
        print(f"  Max step time: {max_time*1000:.2f} ms")
        print(f"  Steps per second: {1/avg_time:.1f}")
        
        return {
            'avg_time': avg_time,
            'min_time': min_time,
            'max_time': max_time,
            'steps_per_second': 1/avg_time,
            'config_name': config_name
        }
    
    def optimize_brain_parameters(self):
        """Optimize brain parameters for performance"""
        # Test different neuron counts
        neuron_counts = [500, 1000, 2000, 5000]
        results = []
        
        for count in neuron_counts:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", count)
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure simulation speed
            start_time = time.time()
            for step in range(100):
                brain.step(step)
            end_time = time.time()
            
            avg_time = (end_time - start_time) / 100
            results.append({
                'neuron_count': count,
                'avg_time_per_step': avg_time,
                'steps_per_second': 1/avg_time
            })
        
        # Find optimal configuration
        best_result = max(results, key=lambda x: x['steps_per_second'])
        print(f"Optimal neuron count: {best_result['neuron_count']}")
        print(f"Performance: {best_result['steps_per_second']:.1f} steps/second")
        
        return best_result
    
    def compare_learning_efficiency(self):
        """Compare learning efficiency with different settings"""
        # Test different plasticity settings
        configs = [
            {"stdp": True, "hebbian": False, "structural": False},
            {"stdp": True, "hebbian": True, "structural": False},
            {"stdp": True, "hebbian": True, "structural": True}
        ]
        
        learning_results = []
        
        for i, plasticity_settings in enumerate(configs):
            print(f"Testing plasticity config {i+1}: {plasticity_settings}")
            
            config = pynlm.createDefaultConfig()
            config.set("plasticity.stdp.enable", plasticity_settings["stdp"])
            config.set("plasticity.hebbian.enable", plasticity_settings["hebbian"])
            config.set("plasticity.structural.enable", plasticity_settings["structural"])
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Measure learning performance
            learning_curve = []
            for step in range(500):
                brain.step(step)
                
                # Calculate learning score (spike count variation)
                firing_rate = brain.getAverageFiringRate()
                learning_curve.append(firing_rate)
            
            # Calculate efficiency metrics
            final_learning = learning_curve[-1]
            learning_velocity = (final_learning - learning_curve[0]) / 500
            efficiency = final_learning / sum(learning_curve)
            
            learning_results.append({
                'config': plasticity_settings,
                'final_learning': final_learning,
                'learning_velocity': learning_velocity,
                'efficiency': efficiency
            })
        
        # Find best configuration
        best_result = max(learning_results, key=lambda x: x['efficiency'])
        print(f"\nBest configuration: {best_result['config']}")
        print(f"Learning efficiency: {best_result['efficiency']:.4f}")
        
        return learning_results

# Usage
tuner = PerformanceTuner(brain, world, agent)
# Compare configurations
# tuner.compare_learning_efficiency()
```

## 6. Advanced Scripting and Automation

### Batch Processing with Results Management

```python
import json
import pandas as pd
from pathlib import Path
import pynlm

class NLMBatchProcessor:
    def __init__(self, output_dir="results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.results = []
    
    def run_batch_experiment(self, experiment_name, config, num_steps, num_repeats=5):
        """Run a batch of experiments with the same configuration"""
        print(f"Running batch experiment: {experiment_name}")
        print(f"Config: {config}")
        print(f"Steps per run: {num_steps}, Repeats: {num_repeats}")
        
        experiment_results = []
        
        for repeat in range(num_repeats):
            print(f"  Run {repeat + 1}/{num_repeats}")
            
            # Create fresh brain and world
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**config.get('world', {}))
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            episode_stats = self.run_episode(agent, brain, world, num_steps, repeat)
            experiment_results.append(episode_stats)
        
        # Calculate aggregate statistics
        aggregate = self.calculate_aggregate_stats(experiment_results)
        
        # Save results
        self.save_experiment_results(experiment_name, config, experiment_results, aggregate)
        
        self.results.append({
            'experiment': experiment_name,
            'config': config,
            'results': experiment_results,
            'aggregate': aggregate
        })
        
        return aggregate
    
    def run_episode(self, agent, brain, world, num_steps, repeat):
        """Run a single episode and collect data"""
        episode_data = {
            'repeat': repeat,
            'steps': [],
            'final_stats': {},
            'learning_curve': [],
            'spike_trains': []
        }
        
        for step in range(num_steps):
            # Run one step
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Collect data
            step_data = {
                'step': step,
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel()
            }
            episode_data['steps'].append(step_data)
            episode_data['learning_curve'].append(brain.getAverageFiringRate())
            
            # Record spikes periodically
            if step % 10 == 0:
                episode_data['spike_trains'].append(brain.get_total_spike_count())
        
        episode_data['final_stats'] = {
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
            'final_dev_stage': brain.getDevelopmentalStage()
        }
        
        return episode_data
    
    def calculate_aggregate_stats(self, experiment_results):
        """Calculate aggregate statistics across all repeats"""
        aggregates = []
        
        for metric in ['firing_neurons', 'total_spikes', 'avg_firing_rate']:
            values = []
            for result in experiment_results:
                values.extend([step[metric] for step in result['steps']])
            
            aggregates.append({
                'metric': metric,
                'mean': np.mean(values),
                'std': np.std(values),
                'min': np.min(values),
                'max': np.max(values)
            })
        
        return aggregates
    
    def save_experiment_results(self, experiment_name, config, results, aggregates):
        """Save experiment results to files"""
        timestamp = pd.Timestamp.now().strftime('%Y%m%d_%H%M%S')
        base_name = f"{experiment_name}_{timestamp}"
        
        # Save detailed results
        with open(self.output_dir / f"{base_name}_detailed.json", 'w') as f:
            json.dump({
                'experiment_name': experiment_name,
                'config': config,
                'results': results,
                'aggregates': aggregates
            }, f, indent=2)
        
        # Save summary statistics
        summary = {
            'experiment_name': experiment_name,
            'config': config,
            'num_repeats': len(results),
            'aggregates': aggregates,
            'timestamp': str(pd.Timestamp.now())
        }
        
        with open(self.output_dir / f"{base_name}_summary.json", 'w') as f:
            json.dump(summary, f, indent=2)
    
    def generate_report(self, experiment_name=None):
        """Generate a comprehensive report of all experiments"""
        if experiment_name:
            experiments = [exp for exp in self.results if exp['experiment'] == experiment_name]
        else:
            experiments = self.results
        
        report = {
            'title': 'NLM Batch Experiment Report',
            'date': str(pd.Timestamp.now()),
            'experiments': []
        }
        
        for exp in experiments:
            report['experiments'].append({
                'name': exp['experiment'],
                'config': exp['config'],
                'aggregate_stats': exp['aggregate'],
                'num_repeats': len(exp['results'])
            })
        
        with open(self.output_dir / "experiment_report.json", 'w') as f:
            json.dump(report, f, indent=2)
        
        return report

# Usage
batch_processor = NLMBatchProcessor()

# Define experiment configurations
configs = [
    {"brain": {"neuron_count": 1000}, "world": {"width": 10, "height": 10}},
    {"brain": {"neuron_count": 2000}, "world": {"width": 15, "height": 15}},
    {"brain": {"neuron_count": 5000}, "world": {"width": 20, "height": 20}}
]

# Run batch experiments
for i, config in enumerate(configs):
    batch_processor.run_batch_experiment(
        experiment_name=f"neuron_count_test_{i}",
        config=config,
        num_steps=500,
        num_repeats=3
    )

# Generate final report
batch_processor.generate_report()
```

## 7. Development Tools and Workflows

### Integrated Development Environment Setup

```python
import os
import subprocess
from pathlib import Path

class NLMDeveloperTools:
    def __init__(self, project_root=".."):
        self.project_root = Path(project_root)
        self.setup_directories()
    
    def setup_directories(self):
        """Create standard NLM project directories"""
        directories = [
            self.project_root / "configs",
            self.project_root / "examples",
            self.project_root / "scripts",
            self.project_root / "docs" / "API_REFERENCE",
            self.project_root / "docs" / "TUTORIALS",
            self.project_root / "docs" / "EXAMPLES",
            self.project_root / "results",
            self.project_root / "benchmarks"
        ]
        
        for directory in directories:
            directory.mkdir(parents=True, exist_ok=True)
        
        # Create essential files
        self.create_readme()
        self.create_gitignore()
        self.create_requirements_dev()
        self.create_build_script()
    
    def create_readme(self):
        """Create a project README file"""
        readme_content = """# NLM Project

## Quick Start

This project uses the NLM neural learning machine framework.

## Running

### Build
```bash
./scripts/build.sh
```

### Run Tests
```bash
./scripts/test.sh
```

### Run Examples
```bash
python examples/example.py
```

## Project Structure

- `configs/` - Configuration files
- `examples/` - Example scripts
- `scripts/` - Development scripts
- `docs/` - Documentation
- `results/` - Experiment results
- `benchmarks/` - Performance benchmarks
"""
        
        with open(self.project_root / "README.md", "w") as f:
            f.write(readme_content)
    
    def create_build_script(self):
        """Create a convenient build script"""
        script_content = """#!/bin/bash
# Build script for NLM project

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Install Python bindings
pip install -e "$PROJECT_ROOT"

echo "Build completed successfully!"
echo "To run tests: ctest --output-on-failure"
echo "To run examples: python $PROJECT_ROOT/examples/"
"""
        
        build_script = self.project_root / "scripts" / "build.sh"
        with open(build_script, 'w') as f:
            f.write(script_content)
        
        os.chmod(build_script, 0o755)
    
    def create_test_script(self):
        """Create a test script"""
        script_content = """#!/bin/bash
# Test script for NLM project

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Run Python tests
echo "Running Python tests..."
pytest "$PROJECT_ROOT/tests/" -v

# Run examples
echo "Running example scripts..."
for example in "$PROJECT_ROOT/examples/"/*.py; do
    echo "Testing $example"
    python "$example"
done

echo "All tests passed!"
"""
        
        test_script = self.project_root / "scripts" / "test.sh"
        with open(test_script, 'w') as f:
            f.write(script_content)
        
        os.chmod(test_script, 0o755)
    
    def setup_version_control(self):
        """Set up version control with appropriate git configuration"""
        # Create .gitignore
        gitignore_content = """# Build artifacts
build/
*.o
*.a
*.so
*.dylib

# Python
__pycache__/
*.pyc
*.pyo
*.pyd
.Python
.env
.venv
env/
venv/
ENV/
env.bak/
venv.bak/

# Documentation
*.md
!README.md
!docs/

# IDE
.vscode/
.idea/
*.swp
*.swo

# Testing
.pytest_cache/
.coverage
htmlcov/

# Other
.DS_Store
*.log
"""
        
        with open(self.project_root / ".gitignore", "w") as f:
            f.write(gitignore_content)
        
        # Configure git user
        subprocess.run(["git", "config", "user.name", "NLM Developer"], cwd=self.project_root)
        subprocess.run(["git", "config", "user.email", "nlm@example.com"], cwd=self.project_root)
    
    def create_documentation_template(self):
        """Create documentation templates for new features"""
        # Template for new example
        example_template = """# {example_name}

## Overview

{description}

## Setup

{setup_code}

## Usage

{usage_code}

## Analysis

{analysis_code}

## Result

{result_code}
"""
        
        # Template for new tutorial
        tutorial_template = """# {tutorial_title}

## Learning Objectives

- {objective1}
- {objective2}

## Prerequisites

{prerequisites}

## Content

{content}

## Exercises

{exercises}

## Further Reading

{further_reading}
"""

# Usage
# developer_tools = NLMDeveloperTools()
# developer_tools.setup_directories()
```

This guide provides comprehensive coverage of intermediate NLM usage, from custom configurations and advanced scripting to performance optimization and development workflows. It builds upon the basic usage patterns while introducing more sophisticated techniques for experienced users.

## Quick Reference for Intermediate Users

### Configuration Management
```python
# Custom configurations
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Configuration files
config.loadFromFile("configs/my_config.json")
config.saveToFile("configs/my_config.json")
```

### Advanced World Manipulation
```python
# Custom worlds
class CustomWorld(pynlm.SimpleWorld):
    def setup_exploration_world(self):
        # Add resources, hazards, obstacles
        pass
```

### Performance Monitoring
```python
# Monitor performance
monitor = PerformanceMonitor()
avg_time = monitor.measure_step_performance(brain, world, agent, 500)
```

### Advanced Scripting
```python
# Batch processing
processor = NLMBatchProcessor()
processor.run_batch_experiment("neuron_count_test", config, 500)
```

### Learning Analysis
```python
# Analyze learning
analyzer = LearningAnalyzer()
analyzer.analyze_learning_curve(1000)
```

### Development Tools
```python
# Development environment
developer = NLMDeveloperTools()
developer.setup_directories()
```

## What's Next?

### Continue Learning:
1. **Master advanced agent behaviors** with custom brain implementations
2. **Create sophisticated world environments** for specific tasks
3. **Implement performance optimization** for your use case
4. **Set up batch processing** for research experiments

### Further Exploration:
1. **Read the API reference** for complete method documentation
2. **Study the examples** for practical implementation patterns
3. **Explore the documentation** for theoretical background
4. **Contribute to the project** with your improvements

### Advanced Research:
1. **Implement custom learning rules** based on neuroscience principles
2. **Design novel neural architectures** for specific applications
3. **Benchmark performance** across different configurations
4. **Develop visualization tools** for better understanding

This intermediate guide provides the foundation for advanced NLM usage, enabling you to create sophisticated neural simulations and experiments tailored to your specific research or application needs.
