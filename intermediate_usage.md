# NLM Intermediate Usage Guide

## Welcome to the Intermediate Guide

This guide bridges the gap between `easy_usage.md` (for absolute beginners) and `HOW_TO_USE.md` (for advanced users). It assumes you have basic Python knowledge and are comfortable with the concepts introduced in the beginner guide, but want to explore more advanced features and patterns.

## Quick Start Recap

Before diving into intermediate features, let's recap the essential patterns:

```python
import pynlm

# Create brain and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)
```

## Working with Configuration Objects

### Advanced Configuration Patterns

The configuration system is powerful but can be complex. Here's how to use it effectively:

```python
import pynlm
import json

# Create configuration with advanced settings
config = pynlm.createDefaultConfig()

# Set multiple parameters using dictionary updates
config.update({
    "brain.neuron_count": 5000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.002,
    "neuromodulation.curiosity.enable": True,
    "neuromodulation.novelty.enable": True
})

# Access configuration values
neuron_count = config.get_int("brain.neuron_count")
learning_rate = config.get_float("plasticity.stdp.learning_rate")
enabled = config.get_bool("neuromodulation.curiosity.enable")

# Export configuration for sharing
saved_config = config.items()  # Python dictionary-like interface
config.saveToFile("my_config.json")

# Import configuration from file
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
```

### Configuration Best Practices

1. **Use descriptive keys**: `brain.learning_rate` instead of just `lr`
2. **Check configuration validity**: `config.has("key")` before accessing
3. **Use defaults**: Provide fallback values for optional parameters
4. **Validate important ranges**: Neuron count between 100-10000, learning rate between 0.0001-0.1

## Advanced Simulation Patterns

### 1. Running Complex Simulation Loops

```python
def advanced_simulation(brain, world, agent, num_steps=1000, verbose=False):
    """Advanced simulation with detailed monitoring"""
    metrics = {
        "steps": [],
        "firing_rates": [],
        "curiosity_levels": [],
        "novelty_levels": [],
        "reward_history": []
    }
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with detailed monitoring
        if verbose and step % 100 == 0:
            print(f"Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation with prediction
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if verbose and step % 50 == 0:
            metrics["steps"].append(step)
            metrics["firing_rates"].append(brain.getAverageFiringRate())
            metrics["curiosity_levels"].append(agent.getCuriosityLevel())
            metrics["novelty_levels"].append(agent.getNoveltyLevel())
            metrics["reward_history"].append(reward)
    
    return metrics
```

### 2. Episode-based Training

```python
class TrainingEnvironment:
    """Advanced training environment with episode management"""
    
    def __init__(self, brain, world, agent):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.episode_rewards = []
        self.episode_lengths = []
        self.current_episode = 0
    
    def reset_episode(self):
        """Reset for a new training episode"""
        self.world.reset()
        self.agent.reset()
        self.agent.initialize(self.world)
        self.world.setAgentStart(50.0, 50.0)
    
    def run_episode(self, max_steps=500, exploration_rate=0.1):
        """Run a single training episode"""
        self.reset_episode()
        total_reward = 0.0
        
        for step in range(max_steps):
            # Get percept
            percept = self.world.getSensoryPercept()
            
            # Process with exploration
            self.agent.processSensoryInput(percept)
            brain.step(step)
            
            # Decode action with exploration
            action = self.agent.decodeMotorCommand()
            
            # Apply action and get reward
            result = self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Apply reward modulation
            self.agent.applyRewardModulation(result.reward, 0.0)
            self.agent.updateDevelopment(0.1)
            
            total_reward += result.reward
            
            # End episode if agent dies or reaches goal
            if not result.success or step >= max_steps - 1:
                break
        
        self.episode_rewards.append(total_reward)
        self.episode_lengths.append(step + 1)
        self.current_episode += 1
        
        return total_reward, step + 1
    
    def train(self, num_episodes=100, max_steps=500):
        """Run multiple training episodes"""
        print(f"Starting training for {num_episodes} episodes...")
        
        for episode in range(num_episodes):
            reward, length = self.run_episode(max_steps, exploration_rate=0.2)
            
            if episode % 10 == 0:
                avg_reward = sum(self.episode_rewards[-10:]) / 10
                print(f"Episode {episode}: Reward = {reward:.2f}, Avg Reward (last 10) = {avg_reward:.2f}, Length = {length}")
        
        # Calculate statistics
        avg_reward = sum(self.episode_rewards) / len(self.episode_rewards)
        avg_length = sum(self.episode_lengths) / len(self.episode_lengths)
        best_reward = max(self.episode_rewards)
        
        print(f"Training complete. Avg Reward: {avg_reward:.2f}, Best Reward: {best_reward:.2f}, Avg Length: {avg_length:.1f}")
        
        return {
            "average_reward": avg_reward,
            "best_reward": best_reward,
            "average_length": avg_length,
            "all_rewards": self.episode_rewards,
            "all_lengths": self.episode_lengths
        }
```

### 3. Developmental Training

```python
def developmental_training(config, stages):
    """Train brain through different developmental stages"""
    # Create brain and agent
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all developmental systems
    agent.enableDevelopment(True)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    print("Starting developmental training...")
    
    for stage_name, stage_duration in stages.items():
        print(f"\nEntering {stage_name} stage ({stage_duration} time units)")
        
        # Adjust configuration for this stage
        if stage_name == "critical_period":
            config["plasticity.stdp.learning_rate"] = 0.01
            config["brain.synapse_density"] = 0.3
        elif stage_name == "maturation":
            config["plasticity.stdp.learning_rate"] = 0.005
            config["brain.synapse_density"] = 0.2
        elif stage_name == "adult":
            config["plasticity.stdp.learning_rate"] = 0.001
            config["brain.synapse_density"] = 0.1
        
        # Run simulation for this stage
        world = pynlm.createSimpleWorld()
        world.configure(width=100, height=100, visionWidth=8, visionHeight=8)
        agent.initialize(world)
        
        for time_unit in range(stage_duration):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(time_unit)
            
            # Decode and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward and development
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        # Print stage statistics
        stage_stats = {
            "firing_rate": brain.getAverageFiringRate(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "development_stage": agent.getDevelopmentalStage()
        }
        print(f"Stage {stage_name} statistics: {stage_stats}")
    
    return brain, agent
```

## Advanced Data Structures and Analysis

### Working with Sensor Data

```python
def analyze_sensor_data(percept, output_file="sensor_analysis.json"):
    """Advanced sensor data analysis"""
    # Extract and analyze vision data
    vision = percept.getVisionArray()
    vision_height = percept.getVisionHeight()
    vision_width = percept.getVisionWidth()
    
    # Convert to 2D array for analysis
    vision_2d = [vision[i * vision_width:(i + 1) * vision_width] 
                 for i in range(vision_height)]
    
    # Calculate statistics
    stats = {
        "vision_dimensions": [vision_height, vision_width],
        "total_values": len(vision),
        "mean": sum(vision) / len(vision),
        "std_dev": (sum((x - sum(vision)/len(vision))**2 for x in vision) / len(vision))**0.5,
        "min": min(vision),
        "max": max(vision),
        "median": sorted(vision)[len(vision)//2],
        "range": max(vision) - min(vision)
    }
    
    # Identify patterns
    bright_regions = []
    for y in range(vision_height):
        for x in range(vision_width):
            if vision[y * vision_width + x] > 0.7:
                bright_regions.append([x, y])
    
    stats["bright_regions"] = bright_regions
    stats["num_bright_regions"] = len(bright_regions)
    
    # Save analysis
    with open(output_file, 'w') as f:
        json.dump(stats, f, indent=2)
    
    return stats

def compare_sensor_data(percept1, percept2):
    """Compare two sensor percepts"""
    all_signals_1 = percept1.getAllSignalsAsDict()
    all_signals_2 = percept2.getAllSignalsAsDict()
    
    comparison = {}
    for key in all_signals_1:
        if key in all_signals_2:
            signals1 = all_signals_1[key]
            signals2 = all_signals_2[key]
            
            if len(signals1) == len(signals2):
                differences = [abs(a - b) for a, b in zip(signals1, signals2)]
                comparison[key] = {
                    "mean_difference": sum(differences) / len(differences),
                    "max_difference": max(differences),
                    "signals_count": len(signals1),
                    "similarity": 1 - sum(differences) / len(differences)
                }
    
    return comparison
```

### Brain State Analysis

```python
def analyze_brain_state(brain, world, agent, steps=100):
    """Comprehensive brain state analysis"""
    analysis = {
        "neurons": {},
        "synapses": {},
        "regions": {},
        "activity_patterns": [],
        "development": {},
        "neuromodulation": {}
    }
    
    # Neuron analysis
    analysis["neurons"]["total_count"] = brain.getTotalNeuronCount()
    analysis["neurons"]["firing_count"] = brain.getFiringNeuronCount()
    analysis["neurons"]["firing_rate"] = brain.getAverageFiringRate()
    analysis["neurons"]["e_i_ratio"] = brain.getExcitationInhibitionRatio()
    
    # Synapse analysis
    analysis["synapses"]["total_count"] = brain.getTotalSynapseCount()
    
    # Region analysis
    regions = brain.getRegions()
    for region in regions:
        region_id = region.getId()
        analysis["regions"][str(region_id)] = {
            "neuron_count": region.getTotalNeuronCount(),
            "synapse_count": region.getSynapseCount(),
            "name": region.getName()
        }
    
    # Activity pattern analysis
    for step in range(steps):
        brain.step(step)
        activity_pattern = {
            "step": step,
            "firing_rate": brain.getAverageFiringRate(),
            "firing_count": brain.getFiringNeuronCount(),
            "stage": agent.getDevelopmentalStage()
        }
        analysis["activity_patterns"].append(activity_pattern)
    
    # Development analysis
    analysis["development"]["current_stage"] = agent.getDevelopmentalStage()
    analysis["development"]["neuromodulation_level"] = agent.getNeuromodulationLevel()
    analysis["development"]["curiosity_level"] = agent.getCuriosityLevel()
    analysis["development"]["novelty_level"] = agent.getNoveltyLevel()
    
    # Neuromodulation analysis
    analysis["neuromodulation"]["dopamine"] = agent.getNeuromodulationLevel()
    analysis["neuromodulation"]["curiosity"] = agent.getCuriosityLevel()
    analysis["neuromodulation"]["novelty"] = agent.getNoveltyLevel()
    analysis["neuromodulation"]["prediction_error"] = agent.getPredictionError()
    
    return analysis
```

## Error Handling and Recovery

### Robust Simulation Patterns

```python
import traceback

def robust_simulation(brain, world, agent, max_steps=1000, recovery_attempts=3):
    """Simulation with error handling and recovery"""
    errors = []
    successful_steps = 0
    
    for step in range(max_steps):
        try:
            # Update world
            world.update(0.1)
            
            # Get and process percept
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get and apply action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply neuromodulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            successful_steps += 1
            
        except Exception as e:
            errors.append({
                "step": step,
                "error_type": type(e).__name__,
                "error_message": str(e),
                "traceback": traceback.format_exc()
            })
            
            # Attempt recovery
            if recovery_attempts > 0:
                print(f"Error at step {step}: {e}. Attempting recovery...")
                try:
                    brain.reset()
                    agent.reset()
                    agent.initialize(world)
                    recovery_attempts -= 1
                    continue
                except Exception as recovery_error:
                    print(f"Recovery failed: {recovery_error}")
                    break
            else:
                print(f"Max recovery attempts reached. Simulation terminated.")
                break
    
    return {
        "successful_steps": successful_steps,
        "total_steps": max_steps,
        "error_count": len(errors),
        "errors": errors,
        "success_rate": successful_steps / max_steps if max_steps > 0 else 0
    }
```

## Performance Monitoring

### Monitoring and Optimization

```python
class PerformanceMonitor:
    """Monitor and optimize simulation performance"""
    
    def __init__(self):
        self.metrics = {
            "time_per_step": [],
            "memory_usage": [],
            "firing_rates": [],
            "neuron_activity": []
        }
        self.start_time = None
        self.step_count = 0
    
    def start_monitoring(self):
        """Start performance monitoring"""
        import time
        self.start_time = time.time()
        self.step_count = 0
    
    def record_step(self, brain, world, agent):
        """Record metrics for current step"""
        import time
        
        step_start = time.time()
        
        # Update world
        world.update(0.1)
        
        # Process percept
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(self.step_count)
        
        # Get metrics
        step_time = time.time() - step_start
        self.metrics["time_per_step"].append(step_time)
        self.metrics["firing_rates"].append(brain.getAverageFiringRate())
        self.metrics["neuron_activity"].append(brain.getFiringNeuronCount())
        
        self.step_count += 1
        
        return step_time
    
    def analyze_performance(self):
        """Analyze and return performance statistics"""
        if not self.metrics["time_per_step"]:
            return {}
        
        times = self.metrics["time_per_step"]
        firing_rates = self.metrics["firing_rates"]
        
        analysis = {
            "total_steps": len(times),
            "total_time": sum(times),
            "average_time_per_step": sum(times) / len(times),
            "min_time_per_step": min(times),
            "max_time_per_step": max(times),
            "std_time_per_step": (sum((t - sum(times)/len(times))**2 for t in times) / len(times))**0.5,
            "steps_per_second": len(times) / sum(times) if sum(times) > 0 else 0,
            "average_firing_rate": sum(firing_rates) / len(firing_rates),
            "average_neuron_activity": sum(self.metrics["neuron_activity"]) / len(self.metrics["neuron_activity"])
        }
        
        return analysis
    
    def get_optimization_suggestions(self, analysis):
        """Get performance optimization suggestions"""
        suggestions = []
        
        if analysis["average_time_per_step"] > 0.1:  # More than 100ms per step
            suggestions.append("Step processing is slow. Consider optimizing brain step computation.")
        
        if analysis["steps_per_second"] < 10:  # Less than 10 steps per second
            suggestions.append("Low simulation throughput. Check for bottlenecks in world updates or sensory processing.")
        
        firing_rate_std = (sum((r - analysis["average_firing_rate"])**2 for r in analysis["firing_rates"]) / len(analysis["firing_rates"]))**0.5
        if firing_rate_std > 0.5:  # High variation in firing rates
            suggestions.append("High variation in firing rates. Consider smoothing neuromodulation effects.")
        
        return suggestions
```

## Complete Example: Advanced Agent Training

```python
# Complete example combining all intermediate concepts

def advanced_agent_training():
    """Complete example of advanced agent training"""
    print("Starting advanced agent training...")
    
    # Create and configure training environment
    config = pynlm.createDefaultConfig()
    config.update({
        "brain.neuron_count": 3000,
        "brain.synapse_density": 0.12,
        "plasticity.stdp.learning_rate": 0.005,
        "neuromodulation.curiosity.enable": True,
        "neuromodulation.novelty.enable": True
    })
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # Enable all subsystems
    agent.enableDevelopment(True)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    
    world.configure(width=150, height=150, visionWidth=10, visionHeight=10)
    agent.initialize(world)
    
    # Initialize performance monitor
    monitor = PerformanceMonitor()
    monitor.start_monitoring()
    
    # Initialize training environment
    training_env = TrainingEnvironment(brain, world, agent)
    
    # Training loop with advanced features
    num_episodes = 50
    max_steps = 200
    
    for episode in range(num_episodes):
        print(f"Episode {episode + 1}/{num_episodes}")
        
        # Run episode
        reward, length = training_env.run_episode(max_steps)
        
        # Record metrics
        step_time = monitor.record_step(brain, world, agent)
        
        # Analyze performance every 10 episodes
        if (episode + 1) % 10 == 0:
            perf_analysis = monitor.analyze_performance()
            suggestions = monitor.get_optimization_suggestions(perf_analysis)
            
            print(f"Performance analysis: {perf_analysis['steps_per_second']:.1f} steps/sec")
            if suggestions:
                print("Optimization suggestions:")
                for suggestion in suggestions:
                    print(f"  - {suggestion}")
    
    # Calculate final statistics
    final_stats = training_env.train(num_episodes, max_steps)
    perf_analysis = monitor.analyze_performance()
    
    print("\n" + "="*50)
    print("TRAINING COMPLETE")
    print("="*50)
    print(f"Average Reward: {final_stats['average_reward']:.2f}")
    print(f"Best Reward: {final_stats['best_reward']:.2f}")
    print(f"Average Episode Length: {final_stats['average_length']:.1f}")
    print(f"Overall Performance: {perf_analysis['steps_per_second']:.1f} steps/sec")
    
    # Save results
    results = {
        "training_stats": final_stats,
        "performance_analysis": perf_analysis,
        "config": config.items()
    }
    
    import json
    with open("training_results.json", 'w') as f:
        json.dump(results, f, indent=2)
    
    return brain, agent, results

if __name__ == "__main__":
    advanced_agent_training()
```

## Summary

This intermediate guide provides you with:

1. **Advanced configuration management** using dictionary interfaces and validation
2. **Complex simulation patterns** including episode-based training and development stages
3. **Sensor data analysis** techniques for understanding environmental input
4. **Brain state analysis** for monitoring and debugging neural activity
5. **Error handling and recovery** patterns for robust simulations
6. **Performance monitoring** and optimization suggestions
7. **Complete working examples** that integrate all intermediate concepts

Use this guide as your bridge to advanced usage, combining the simplicity of beginner tutorials with the power of the comprehensive documentation. You're now equipped to build sophisticated neural agents and conduct meaningful experiments with the NLM framework.