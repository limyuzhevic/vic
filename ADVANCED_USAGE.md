# NLM (熙然) - Advanced Usage Guide

## Overview

NLM (Neural Learning Machine) is an experimental artificial developmental brain simulation framework. This guide provides advanced usage patterns, configuration options, and examples for both beginners and experienced users.

## Advanced Configuration Examples

### 1. Advanced Agent Configuration

```python
import pynlm

# Create configuration with advanced settings
config = pynlm.createDefaultConfig()

# Advanced neural network configuration
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)
config.set("brain.connection_probability", 0.08)
config.set("brain.initial_weight_mean", 0.6)
config.set("brain.initial_weight_std", 0.2)

# Neuromodulation configuration
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("neuromod.prediction.enable", True)

# Plasticity configuration
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)

# Development configuration
config.set("development.initial_plasticity", 1.0)
config.set("development.critical_period_end", 300.0)
config.set("development.maturation_rate", 0.5)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with advanced features
agent = pynlm.createAgentBrain(brain)

# Configure advanced agent features
agent.setNeuronGroupDistribution(True)  # Optimize for speed
agent.setMotorCommandScaling(1.2f)     # Scale motor commands
agent.setNoveltyThreshold(0.3f)         # Lower threshold for novelty
agent.setCuriosityFactor(0.8f)          # High curiosity for exploration
agent.setDevelopmentRate(2.0)           # Fast development

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableDebugMode(True)              # Enable debug mode
agent.enableProfiling(True)              # Enable performance profiling
agent.enablePerformanceMonitoring(True)  # Enable real-time monitoring
```

### 2. Advanced World Configuration

```python
import pynlm

# Create complex world
world = pynlm.createSimpleWorld()

# Configure advanced world parameters
world.configure(
    width=50,           # Large world
    height=50,
    visionWidth=16,     # High-resolution vision
    visionHeight=16
)

# Add complex terrain and objects
world.addObject(pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, value=10.0))
world.addObject(pynlm.WorldObject(30.0, 20.0, pynlm.WorldObjectType.Hazard, value=-5.0))
world.addObject(pynlm.WorldObject(40.0, 40.0, pynlm.WorldObjectType.Wall, radius=2.0))
world.addObject(pynlm.WorldObject(25.0, 15.0, pynlm.WorldObjectType.Marker, value=1.0))

# Configure world physics and dynamics
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.01)
world.setRandomSeed(42)

# Set agent starting position with orientation
world.setAgentStart(5.0, 5.0)
```

### 3. Advanced Simulation Loop with Monitoring

```python
import pynlm
import time
import json

def run_advanced_simulation(config_file=None, output_file="simulation_results.json"):
    """Run an advanced simulation with comprehensive monitoring and logging."""
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    if config_file:
        config.loadFromFile(config_file)
    
    # Create brain with advanced configuration
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with advanced features
    agent = pynlm.createAgentBrain(brain)
    
    # Configure advanced agent features
    agent.setNeuronGroupDistribution(True)
    agent.setMotorCommandScaling(1.5f)
    agent.setCuriosityFactor(0.7f)
    agent.enableDebugMode(True)
    agent.enableProfiling(True)
    agent.enablePerformanceMonitoring(True)
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.setAgentStart(15.0, 15.0)
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Simulation results collection
    simulation_results = {
        "timestamp": time.time(),
        "config": config_file,
        "steps": [],
        "agent_state": {},
        "brain_metrics": {},
        "performance": {}
    }
    
    # Run simulation
    start_time = time.time()
    last_log_time = start_time
    
    for step in range(5000):
        # Record start time for performance measurement
        step_start = time.time()
        
        # Update world
        timestep = 0.1
        world.update(timestep)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(timestep)
        
        # Record step results
        step_end = time.time()
        step_time = step_end - step_start
        
        step_data = {
            "step": step,
            "time": world.getSimulationTime(),
            "action": str(action),
            "reward": reward,
            "step_time": step_time,
            "firing_neurons": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "dopamine": agent.getNeuromodulationLevel(),
            "development_stage": str(brain.getDevelopmentalStage())
        }
        
        simulation_results["steps"].append(step_data)
        
        # Record performance metrics periodically
        if (step + 1) % 100 == 0:
            agent.recordPerformanceMetrics(timestep)
            
            # Log progress
            if time.time() - last_log_time > 5.0:  # Log every 5 seconds
                print(f"Step {step + 1}: "
                      f"Firing: {brain.getFiringNeuronCount()}, "
                      f"Spikes: {brain.getTotalSpikeCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}, "
                      f"Dev Stage: {brain.getDevelopmentalStage()}")
                last_log_time = time.time()
        
        # Save state periodically
        if (step + 1) % 1000 == 0:
            agent.saveAgentState(f"agent_state_step_{step + 1}.bin")
    
    # Simulation complete
    end_time = time.time()
    total_time = end_time - start_time
    
    simulation_results["total_time"] = total_time
    simulation_results["steps_per_second"] = len(simulation_results["steps"]) / total_time
    simulation_results["agent_state"] = {
        "dopamine": agent.getNeuromodulationLevel(),
        "curiosity": agent.getCuriosityLevel(),
        "novelty": agent.getNoveltyLevel(),
        "development_stage": str(brain.getDevelopmentalStage()),
        "firing_neurons": brain.getFiringNeuronCount(),
        "total_spikes": brain.getTotalSpikeCount()
    }
    simulation_results["brain_metrics"] = {
        "total_neurons": brain.getTotalNeuronCount(),
        "total_synapses": brain.getTotalSynapseCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "e_i_ratio": brain.getExcitationInhibitionRatio()
    }
    
    # Save results
    with open(output_file, "w") as f:
        json.dump(simulation_results, f, indent=2)
    
    # Export debug information
    agent.exportDebugInfo("debug_info.json")
    
    print(f"\n=== Simulation Complete ===")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Steps per second: {simulation_results['steps_per_second']:.2f}")
    print(f"Results saved to: {output_file}")
    
    return simulation_results

# Run the advanced simulation
if __name__ == "__main__":
    results = run_advanced_simulation("advanced_config.json", "simulation_results.json")
```

### 4. Advanced Experiment Framework

```python
import pynlm
import numpy as np

class AdvancedExperiment:
    """Advanced experiment framework for NLM."""
    
    def __init__(self, config):
        self.config = config
        self.brain = pynlm.createBrain(config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.results = {}
        
    def setup(self, world_width=40, world_height=40, vision_width=16, vision_height=16):
        """Setup the experiment environment."""
        # Initialize brain
        self.brain.initialize()
        
        # Configure world
        self.world.configure(world_width, world_height, vision_width, vision_height)
        self.world.setAgentStart(world_width // 2, world_height // 2)
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Configure advanced features
        self.agent.setNeuronGroupDistribution(True)
        self.agent.setMotorCommandScaling(1.2f)
        self.agent.enableDebugMode(True)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        return self
    
    def add_obstacle(self, x, y, radius=2.0):
        """Add an obstacle to the world."""
        self.world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Wall, radius))
        return self
    
    def add_resource(self, x, y, value=5.0):
        """Add a resource to the world."""
        self.world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value))
        return self
    
    def run_trial(self, trial_id, max_steps=1000, exploration_mode=False):
        """Run a single trial of the experiment."""
        print(f"Running trial {trial_id}...")
        
        # Reset world and brain for new trial
        self.world.reset()
        self.brain.reset()
        self.brain.initialize()
        self.agent.reset()
        
        # Reset agent with new world
        self.agent.initialize(self.world)
        
        trial_data = {
            "trial_id": trial_id,
            "steps": [],
            "final_state": {},
            "metrics": {}
        }
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get sensory input
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Run brain step
            self.brain.step(step)
            
            # Select action
            if exploration_mode and np.random.random() < 0.1:  # 10% random exploration
                # Select random action
                import random
                actions = [pynlm.MotorCommand.MoveForward, 
                          pynlm.MotorCommand.TurnLeft,
                          pynlm.MotorCommand.TurnRight,
                          pynlm.MotorCommand.Interact]
                action = random.choice(actions)
            else:
                # Use agent's decision
                action = self.agent.decodeMotorCommand()
            
            # Apply action
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Get reward
            reward = 0.0
            if self.world.getSensoryPercept().getInternal():
                reward = self.world.getSensoryPercept().getInternal()[0]
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0)
            
            # Record step data
            step_data = {
                "step": step,
                "time": self.world.getSimulationTime(),
                "action": str(action),
                "reward": reward,
                "firing_neurons": self.brain.getFiringNeuronCount(),
                "total_spikes": self.brain.getTotalSpikeCount(),
                "curiosity": self.agent.getCuriosityLevel(),
                "novelty": self.agent.getNoveltyLevel(),
                "development_stage": str(self.brain.getDevelopmentalStage())
            }
            
            trial_data["steps"].append(step_data)
            
            # Check for termination conditions
            if self.brain.getTotalSpikeCount() > 1000:  # Too much activity
                break
            if step > 500 and self.brain.getFiringNeuronCount() < 5:  # Brain seems inactive
                break
            
            # Update development
            self.agent.updateDevelopment(0.1)
        
        # Store final state
        trial_data["final_state"] = {
            "firing_neurons": self.brain.getFiringNeuronCount(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "curiosity": self.agent.getCuriosityLevel(),
            "novelty": self.agent.getNoveltyLevel(),
            "dopamine": self.agent.getNeuromodulationLevel(),
            "development_stage": str(self.brain.getDevelopmentalStage()),
            "final_time": self.world.getSimulationTime()
        }
        
        # Calculate metrics
        trial_data["metrics"] = self.calculate_trial_metrics(trial_data["steps"])
        
        self.results[f"trial_{trial_id}"] = trial_data
        
        print(f"Trial {trial_id} completed in {len(trial_data['steps'])} steps")
        print(f"  Final firing rate: {trial_data['final_state']['firing_neurons']}")
        print(f"  Total spikes: {trial_data['final_state']['total_spikes']}")
        print(f"  Final curiosity: {trial_data['final_state']['curiosity']:.3f}")
        
        return trial_data
    
    def calculate_trial_metrics(self, steps):
        """Calculate metrics from trial steps."""
        if not steps:
            return {}
        
        metrics = {}
        
        # Calculate average values
        metrics["avg_firing_rate"] = np.mean([s["firing_neurons"] for s in steps])
        metrics["avg_reward"] = np.mean([s["reward"] for s in steps])
        metrics["max_reward"] = np.max([s["reward"] for s in steps])
        metrics["min_reward"] = np.min([s["reward"] for s in steps])
        metrics["avg_curiosity"] = np.mean([s["curiosity"] for s in steps])
        metrics["avg_novelty"] = np.mean([s["novelty"] for s in steps])
        metrics["total_steps"] = len(steps)
        metrics["final_time"] = steps[-1]["time"] if steps else 0
        
        # Calculate standard deviations
        metrics["std_firing_rate"] = np.std([s["firing_neurons"] for s in steps])
        metrics["std_reward"] = np.std([s["reward"] for s in steps])
        metrics["std_curiosity"] = np.std([s["curiosity"] for s in steps])
        
        return metrics
    
    def run_experiment(self, num_trials=5, exploration_mode=False):
        """Run the complete experiment."""
        print(f"=== Starting NLM Advanced Experiment ===")
        print(f"Trials: {num_trials}")
        print(f"Exploration mode: {exploration_mode}")
        print(f"World size: {self.world.getWidth()}x{self.world.getHeight()}")
        print()
        
        for trial_id in range(1, num_trials + 1):
            self.run_trial(trial_id, exploration_mode=exploration_mode)
            print()
        
        # Analyze results
        self.analyze_results()
        
        return self.results
    
    def analyze_results(self):
        """Analyze experiment results."""
        print("=== Experiment Analysis ===")
        
        if not self.results:
            print("No results to analyze.")
            return
        
        # Calculate aggregate metrics
        total_spikes = sum(
            self.results[f"trial_{i}"]["final_state"]["total_spikes"] 
            for i in range(1, len(self.results) + 1)
        )
        
        avg_firing_rate = np.mean([
            self.results[f"trial_{i}"]["final_state"]["firing_neurons"]
            for i in range(1, len(self.results) + 1)
        ])
        
        avg_curiosity = np.mean([
            self.results[f"trial_{i}"]["final_state"]["curiosity"]
            for i in range(1, len(self.results) + 1)
        ])
        
        print(f"Total trials: {len(self.results)}")
        print(f"Total spikes across all trials: {total_spikes}")
        print(f"Average final firing rate: {avg_firing_rate:.2f}")
        print(f"Average final curiosity: {avg_curiosity:.3f}")
        
        # Save results
        import json
        with open("experiment_results.json", "w") as f:
            json.dump(self.results, f, indent=2)
        
        print(f"Results saved to: experiment_results.json")

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create advanced experiment
    experiment = AdvancedExperiment(config)
    
    # Setup environment with obstacles and resources
    experiment.setup(world_width=50, world_height=50)
    experiment.add_obstacle(25, 25, 3.0)  # Central obstacle
    experiment.add_resource(10, 10, 10.0)  # Resource
    experiment.add_resource(40, 40, 8.0)  # Another resource
    
    # Run experiment with exploration
    results = experiment.run_experiment(num_trials=3, exploration_mode=True)
```

## Performance Optimization Tips

### 1. Configuration Optimization
- Use larger timesteps for faster simulations
- Reduce neuron count for testing (1000-5000 neurons)
- Disable unused systems (curiosity, novelty, prediction)
- Use appropriate synaptic density (0.05-0.15)

### 2. Memory Management
- Periodically save agent state during long simulations
- Use checkpointing for very long experiments
- Monitor memory usage with performance tools

### 3. Parallel Processing
- Use SIMD optimizations (if available)
- Enable GPU acceleration for large simulations
- Use multiple threads for parallel processing

### 4. Debugging and Profiling
- Use debug mode for detailed logging
- Enable performance profiling for bottlenecks
- Use exportDebugInfo() for comprehensive debugging

## Common Issues and Solutions

### 1. "Brain isn't doing anything"
- **Solution**: Ensure `brain.initialize()` is called before `brain.step()`
- **Tip**: Check that neurons have received sensory input

### 2. "Agent isn't moving"
- **Solution**: Ensure `world.applyMotorCommand()` is called in the simulation loop
- **Tip**: Verify that motor neurons are active

### 3. "Everything is 0"
- **Solution**: Brains need time to "warm up" - try more steps
- **Tip**: Check that sensory input is connected and providing data

### 4. "Memory leaks"
- **Solution**: AgentBrain now has proper memory management with destructors
- **Tip**: Use `saveAgentState()` periodically during long simulations

## Next Steps

After mastering the basics:

1. **Advanced Features**: Explore neuromodulation, structural plasticity, and development
2. **Performance**: Implement GPU acceleration and parallel processing
3. **Research**: Design and run scientific experiments
4. **Extensions**: Add custom neuron types, synaptic models, or learning rules
5. **Integration**: Integrate NLM with other AI systems or robotics platforms

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [ROADMAP.md](docs/ROADMAP.md) - Future development plans
- [LIMITATIONS.md](docs/LIMITATIONS.md) - Current limitations and future directions
