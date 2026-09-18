# Advanced Features and Commands for NLM Phase 6

## Advanced Python API

### Enhanced Configuration Options

The `pynlm` Python bindings now support more advanced configuration options:

```python
import pynlm

# Advanced configuration with custom settings
config = pynlm.createDefaultConfig()
config.set("random_seed", 12345)
config.set("neuron_count", 2000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.025)
config.set("stdp_tau", 25.0)

# Simulation parameters
config.set("simulation_timestep", 0.0005)
config.set("replay_interval", 200)
config.set("consolidation_interval", 2000)

# Neuromodulation configuration
config.set("dopamine_sensitivity", 1.2)
config.set("curiosity_weight", 0.8)
config.set("novelty_threshold", 0.3)
```

### Advanced Brain Creation and Configuration

```python
# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()

# Configure development system
brain.enableDevelopment(True)
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Set custom simulation parameters
brain.getConfig().set("max_spikes_per_step", 1000)
brain.getConfig().set("plasticity_decay_rate", 0.999)
```

### Enhanced Agent Features

```python
# Create agent with enhanced capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Advanced neuromodulation controls
agent.applyRewardModulation(reward=1.0, predicted_reward=0.8)
agent.updateDevelopment(timestep=0.1)
```

### Neural Network Architecture Management

```python
# Add neural regions with specific characteristics
region_id = brain.addRegion("MotorRegion")
brain.addRegion("SensoryRegion")

# Configure region properties
if region := brain.getRegion(region_id):
    # Set neuron populations
    vision_pop_id = region.addPopulation(512, pynlm.NeuronType.Sensory)
    motor_pop_id = region.addPopulation(256, pynlm.NeuronType.Motor)
    
    # Configure population characteristics
    if pop := region.getPopulation(vision_pop_id):
        pop.setMaxFiringRate(200.0)
        pop.setConnectionProbability(0.12)
        
    if pop := region.getPopulation(motor_pop_id):
        pop.setMaxFiringRate(150.0)
        pop.setConnectionProbability(0.08)
```

### Advanced Simulation Control

```python
# Run simulation with custom step handling
brain.step(0)  # Initialize step

for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation based on feedback
    if step % 10 == 0:
        reward = world.getAgentBody().energy * 10.0
        predicted = calculateExpectedReward(step)
        agent.applyRewardModulation(reward, predicted)
    
    # Print diagnostic information
    if step % 100 == 0:
        print(f"Step {step}: Firing neurons={brain.getFiringNeuronCount()}, "
              f"Spikes={brain.getTotalSpikeCount()}, "
              f"Firing rate={brain.getAverageFiringRate():.2f}")
```

## Command Line Interface

### NLM CLI Tools

The NLM now includes an enhanced command-line interface for advanced users:

```bash
# Run Phase 6 integration test
./nlm_phase6_demo --config demo_config.json

# Export brain state to file
./nlm_export --input ./brain.bin --output brain.json --format detailed

# Import brain state with validation
./nlm_import --input brain.json --output brain.bin --validate

# Run performance benchmarks
./nlm_benchmark --mode comprehensive --output results.json

# Generate visualization data
./nlm_visualize --input ./brain.bin --output ./frames/ --format animated
```

### Configuration File Format

Example advanced configuration file (`advanced_config.json`):

```json
{
    "simulation": {
        "timestep": 0.0005,
        "duration": 60.0,
        "max_steps": 120000
    },
    "brain": {
        "neuron_count": 3000,
        "region_count": 3,
        "connection_probability": 0.18
    },
    "plasticity": {
        "stdp_ltp_weight": 0.015,
        "stdp_ltd_weight": 0.018,
        "stdp_tau": 30.0,
        "synaptogenesis_rate": 0.0002,
        "pruning_rate": 0.00002
    },
    "neuromodulation": {
        "dopamine_sensitivity": 1.0,
        "curiosity_weight": 0.9,
        "novelty_threshold": 0.25,
        "reward_decay_rate": 0.95
    },
    "memory": {
        "working_memory_size": 1000,
        "episodic_capacity": 500,
        "consolidation_interval": 500
    }
}
```

## Advanced Experiment Framework

### Custom Experiment Builder

```python
# Build advanced experiments using the experiment framework
from pynlm import experiments

# Create a custom exploration experiment
class ExplorationExperiment(experiments.Experiment):
    def __init__(self, config):
        super().__init__(config)
        self.exploration_levels = [0.1, 0.3, 0.5, 0.7, 0.9]
        self.memory_tracking = True
    
    def setup(self):
        # Setup environment
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Create agent with enhanced capabilities
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
    
    def run(self):
        results = []
        
        for exploration_level in self.exploration_levels:
            # Configure curiosity for this level
            self.agent.updateCuriosityLevel(exploration_level)
            
            # Run simulation
            for step in range(self.config.get("steps_per_level", 500)):
                self.step()
                
                if step % 50 == 0:
                    results.append({
                        'step': step,
                        'exploration': exploration_level,
                        'neurons_firing': self.brain.getFiringNeuronCount(),
                        'spike_count': self.brain.getTotalSpikeCount(),
                        'action': self.world.getLastAction(),
                        'reward': self.world.getAgentBody().energy
                    })
        
        return results
    
    def analyze(self, results):
        # Analyze exploration patterns
        exploration_effectiveness = []
        for level in self.exploration_levels:
            level_results = [r for r in results if r['exploration'] == level]
            effectiveness = sum(r['reward'] for r in level_results) / len(level_results)
            exploration_effectiveness.append(effectiveness)
        
        return {
            'optimal_exploration_level': self.exploration_levels[
                exploration_effectiveness.index(max(exploration_effectiveness))
            ],
            'overall_performance': sum(exploration_effectiveness) / len(exploration_effectiveness)
        }
```

### Experiment Runner API

```python
# Run advanced experiments using the experiment runner
runner = experiments.ExperimentRunner()

# Add multiple experiment variants
runner.addExperiment("baseline", BaselineExperiment(config))
runner.addExperiment("curiosity_driven", CuriosityExperiment(config))
runner.addExperiment("development_driven", DevelopmentExperiment(config))
runner.addExperiment("mixed", MixedExperiment(config))

# Run experiment suite
results = runner.runAllExperiments(output_dir="./experiment_results")

# Analyze comparative performance
runner.generateComparativeReport(results, output_format="html")
```

## Advanced Neural Network Visualization

### Custom Visualization Tools

```python
import matplotlib.pyplot as plt
import numpy as np

# Visualize brain state evolution
class BrainVisualizer:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
        self.fig, self.axes = plt.subplots(2, 3, figsize=(15, 10))
        self.fig.suptitle('Advanced NLM Brain Visualization', fontsize=16)
    
    def update_plots(self, step):
        # 1. Neural population activity heatmap
        ax1 = self.axes[0, 0]
        population_activities = self.getPopulationActivities()
        im1 = ax1.imshow(population_activities, cmap='hot', aspect='auto')
        ax1.set_title('Neural Population Activity')
        ax1.set_xlabel('Population Index')
        ax1.set_ylabel('Region Index')
        
        # 2. Synaptic weight distribution
        ax2 = self.axes[0, 1]
        synaptic_weights = self.getSynapticWeightDistribution()
        ax2.hist(synaptic_weights, bins=50, alpha=0.7, edgecolor='black')
        ax2.set_title('Synaptic Weight Distribution')
        ax2.set_xlabel('Weight Value')
        ax2.set_ylabel('Frequency')
        
        # 3. Brain development stages
        ax3 = self.axes[0, 2]
        developmental_stage = self.brain.getDevelopmentalStage()
        stages = ['Initial', 'CriticalPeriod', 'Maturation', 'Adult', 'Aging']
        stage_values = [0, 0, 0, 0, 0]
        if developmental_stage == pynlm.DevelopmentalStage.CriticalPeriod:
            stage_values[1] = 1
        elif developmental_stage == pynlm.DevelopmentalStage.Maturation:
            stage_values[2] = 1
        elif developmental_stage == pynlm.DevelopmentalStage.Adult:
            stage_values[3] = 1
        elif developmental_stage == pynlm.DevelopmentalStage.Aging:
            stage_values[4] = 1
        else:
            stage_values[0] = 1
            
        ax3.bar(stages, stage_values, color=['red', 'orange', 'yellow', 'green', 'blue'])
        ax3.set_title('Current Developmental Stage')
        ax3.set_ylabel('Active')
        ax3.set_ylim(0, 1.2)
        
        # 4. Neuromodulation levels
        ax4 = self.axes[1, 0]
        neuromod_levels = [
            self.brain.getNeuromodulationLevel(),
            self.getCuriosityLevel(),
            self.getNoveltyLevel(),
            self.getPredictionError()
        ]
        labels = ['Dopamine', 'Curiosity', 'Novelty', 'Prediction Error']
        colors = ['red', 'blue', 'green', 'purple']
        ax4.bar(labels, neuromod_levels, color=colors, alpha=0.7)
        ax4.set_title('Current Neuromodulation Levels')
        ax4.set_ylabel('Level (0-1)')
        ax4.set_ylim(0, 1.2)
        
        # 5. Agent position and movement
        ax5 = self.axes[1, 1]
        body = self.world.getAgentBody()
        # Plot agent path
        path_x = [body.x]
        path_y = [body.y]
        for i in range(min(50, len(path_x) - 1), len(path_x)):
            path_x.append(body.x)
            path_y.append(body.y)
            
        ax5.plot(path_x, path_y, 'b-', alpha=0.5, linewidth=1)
        ax5.plot(body.x, body.y, 'ro', markersize=10)
        ax5.set_title('Agent Position and Movement')
        ax5.set_xlabel('X Position')
        ax5.set_ylabel('Y Position')
        ax5.set_aspect('equal')
        ax5.set_xlim(0, 20)
        ax5.set_ylim(0, 20)
        
        # 6. Reward history
        ax6 = self.axes[1, 2]
        rewards = self.getRecentRewards(50)
        ax6.plot(range(len(rewards)), rewards, 'g-', linewidth=2)
        ax6.set_title('Recent Reward History')
        ax6.set_xlabel('Time Step')
        ax6.set_ylabel('Reward')
        ax6.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.pause(0.01)
        
        return self.fig
    
    def save_visualization(self, filepath):
        plt.savefig(filepath, dpi=300, bbox_inches='tight')
        plt.close()
    
    def getPopulationActivities(self):
        # Return 2D array of population activities
        # Implementation depends on your brain structure
        pass
    
    def getSynapticWeightDistribution(self):
        # Return list of synaptic weights
        # Implementation depends on your brain structure
        pass
    
    def getCuriosityLevel(self):
        # Get current curiosity level from agent
        # Implementation depends on your agent structure
        pass
    
    def getNoveltyLevel(self):
        # Get current novelty level from agent
        # Implementation depends on your agent structure
        pass
    
    def getPredictionError(self):
        # Get current prediction error from agent
        # Implementation depends on your agent structure
        pass
    
    def getRecentRewards(self, count):
        # Return recent reward history
        # Implementation depends on your world structure
        pass
```

## Performance Optimization

### Advanced Performance Monitoring

```python
# Monitor and optimize NLM performance
from pynlm import performance

class PerformanceMonitor:
    def __init__(self, brain, agent, world):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.metrics = {
            'step_time': [],
            'spike_count': [],
            'firing_rate': [],
            'memory_usage': [],
            'energy_consumption': []
        }
        self.optimization_enabled = True
    
    def monitor_step(self, step):
        import time
        start_time = time.time()
        
        # Execute simulation step
        self.step()
        
        # Measure performance
        step_time = time.time() - start_time
        self.metrics['step_time'].append(step_time)
        
        if len(self.metrics['step_time']) > 100:
            self.metrics['step_time'].pop(0)
        
        # Calculate derived metrics
        if step_time > 0.1 and self.optimization_enabled:
            self.suggest_optimizations(step_time)
    
    def suggest_optimizations(self, current_step_time):
        # Suggest performance improvements
        avg_step_time = sum(self.metrics['step_time']) / len(self.metrics['step_time'])
        
        if current_step_time > avg_step_time * 1.5:
            print("Performance alert: Current step time ({:.3f}s) exceeds average ({:.3f}s)".format(
                current_step_time, avg_step_time))
            
            # Suggest specific optimizations
            print("Recommendations:")
            print("1. Reduce simulation timestep")
            print("2. Enable spatial pooling in sensory processing")
            print("3. Optimize neural network architecture")
            print("4. Use SIMD optimizations for mathematical operations")
            print("5. Implement batch processing for similar operations")
    
    def getPerformanceReport(self):
        report = {
            'avg_step_time': sum(self.metrics['step_time']) / len(self.metrics['step_time']),
            'min_step_time': min(self.metrics['step_time']),
            'max_step_time': max(self.metrics['step_time']),
            'step_time_std': np.std(self.metrics['step_time']),
            'steps_per_second': 1.0 / (sum(self.metrics['step_time']) / len(self.metrics['step_time'])),
            'memory_efficiency': self.calculateMemoryEfficiency(),
            'optimization_suggestions': self.getOptimizationSuggestions()
        }
        return report
    
    def calculateMemoryEfficiency(self):
        # Calculate memory usage efficiency
        total_neurons = self.brain.getTotalNeuronCount()
        total_memory = total_neurons * 4  # bytes per neuron (estimated)
        actual_usage = self.getActualMemoryUsage()
        
        if total_memory > 0:
            return actual_usage / total_memory
        return 0.0
    
    def getOptimizationSuggestions(self):
        suggestions = []
        
        # Analyze performance bottlenecks
        avg_step_time = sum(self.metrics['step_time']) / len(self.metrics['step_time'])
        
        if avg_step_time > 0.05:
            suggestions.append("Consider reducing simulation timestep")
            suggestions.append("Enable event-driven simulation for sparse activity")
        
        if self.brain.getTotalNeuronCount() > 10000:
            suggestions.append("Implement neural network compression")
            suggestions.append("Use quantized representations for neural states")
        
        if len(self.world.getObjects()) > 1000:
            suggestions.append("Optimize world object management")
            suggestions.append("Implement spatial partitioning for collision detection")
        
        return suggestions
```

## Advanced Configuration API

### Programmatic Configuration Management

```python
class AdvancedConfigurationManager:
    def __init__(self):
        self.configurations = {}
        self.presets = {}
    
    def createConfiguration(self, name, settings):
        """Create a new configuration with specified settings"""
        config = pynlm.createDefaultConfig()
        
        # Apply settings
        for key, value in settings.items():
            config.set(key, value)
        
        # Validate configuration
        self.validateConfiguration(config)
        
        # Store configuration
        self.configurations[name] = config
        return config
    
    def createPreset(self, name, settings):
        """Create a named preset configuration"""
        self.presets[name] = settings.copy()
        return self.presets[name]
    
    def loadPreset(self, name):
        """Load a preset configuration"""
        if name in self.presets:
            return self.presets[name]
        raise ValueError(f"Preset '{name}' not found")
    
    def validateConfiguration(self, config):
        """Validate configuration settings"""
        validation_rules = {
            'random_seed': {'type': int, 'min': 0, 'max': 2**32 - 1},
            'neuron_count': {'type': int, 'min': 100, 'max': 100000},
            'region_count': {'type': int, 'min': 1, 'max': 100},
            'connection_probability': {'type': float, 'min': 0.0, 'max': 1.0},
            'simulation_timestep': {'type': float, 'min': 0.0001, 'max': 1.0}
        }
        
        for key, value in config.getKeys():
            if key in validation_rules:
                rule = validation_rules[key]
                if rule['type'] == int and not isinstance(value, int):
                    raise ValueError(f"Configuration '{key}' must be an integer")
                elif rule['type'] == float and not isinstance(value, (int, float)):
                    raise ValueError(f"Configuration '{key}' must be a number")
                
                if 'min' in rule and value < rule['min']:
                    raise ValueError(f"Configuration '{key}' must be >= {rule['min']}")
                if 'max' in rule and value > rule['max']:
                    raise ValueError(f"Configuration '{key}' must be <= {rule['max']}")
    
    def mergeConfigurations(self, base_config, override_config):
        """Merge two configurations"""
        merged = base_config.copy()
        
        for key, value in override_config.getKeys():
            merged.set(key, value)
        
        return merged
    
    def exportConfiguration(self, config, filepath):
        """Export configuration to file"""
        import json
        
        settings = {}
        for key, value in config.getKeys():
            settings[key] = value
        
        with open(filepath, 'w') as f:
            json.dump(settings, f, indent=2)
    
    def importConfiguration(self, filepath):
        """Import configuration from file"""
        import json
        
        with open(filepath, 'r') as f:
            settings = json.load(f)
        
        config = pynlm.createDefaultConfig()
        for key, value in settings.items():
            config.set(key, value)
        
        return config
```

## Advanced Scripting and Automation

### NLM Automation Framework

```python
class NLMAutomation:
    def __init__(self, config_file=None):
        if config_file:
            self.config = self.load_config(config_file)
        else:
            self.config = pynlm.createDefaultConfig()
        
        self.history = []
        self.checkpoints = {}
    
    def run_experiment_series(self, experiment_configs):
        """Run a series of experiments with different configurations"""
        results = {}
        
        for exp_name, exp_config in experiment_configs.items():
            print(f"Running experiment: {exp_name}")
            
            # Create configuration
            config = self.config.copy()
            for key, value in exp_config.items():
                config.set(key, value)
            
            # Run experiment
            result = self.run_experiment(config)
            results[exp_name] = result
            
            # Save checkpoint
            self.save_checkpoint(f"{exp_name}_checkpoint")
        
        return results
    
    def run_experiment(self, config):
        """Run a single experiment"""
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Configure agent based on config
        self.configureAgent(agent, config)
        
        # Run simulation
        experiment_result = {
            'steps_completed': 0,
            'final_performance': 0.0,
            'reward_history': [],
            'neural_activity': [],
            'action_history': []
        }
        
        for step in range(config.get('max_steps', 1000)):
            # Record state
            self.recordState(brain, agent, world, step)
            
            # Execute step
            self.executeStep(brain, agent, world, step)
            
            # Record results
            self.recordResults(experiment_result, brain, agent, world)
            
            # Check for termination conditions
            if self.checkTermination(brain, agent, world, step):
                break
        
        return experiment_result
    
    def configureAgent(self, agent, config):
        """Configure agent based on configuration"""
        # Enable features based on config
        if config.get('reward_modulation_enabled', True):
            agent.enableRewardModulation(True)
        
        if config.get('structural_plasticity_enabled', True):
            agent.enableStructuralPlasticity(True)
        
        if config.get('development_enabled', True):
            agent.enableDevelopment(True)
        
        if config.get('curiosity_enabled', True):
            agent.enableCuriosity(True)
    
    def executeStep(self, brain, agent, world, step):
        """Execute one simulation step"""
        # Update world
        world.update(config.get('timestep', 0.1))
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation if enabled
        if config.get('reward_modulation_enabled', True):
            reward = world.getAgentBody().energy * 10.0
            predicted = agent.getPredictionError()
            agent.applyRewardModulation(reward, predicted)
    
    def checkTermination(self, brain, agent, world, step):
        """Check if experiment should terminate"""
        config = self.config
        
        # Check step limit
        if step >= config.get('max_steps', 1000):
            return True
        
        # Check performance threshold
        if step >= config.get('performance_check_interval', 100):
            current_performance = self.calculatePerformance(brain, agent, world)
            if current_performance >= config.get('performance_threshold', 0.8):
                return True
        
        # Check energy depletion
        if world.getAgentBody().energy <= 0.0:
            return True
        
        # Check for requested early termination
        if hasattr(self, 'requested_termination') and self.requested_termination:
            return True
        
        return False
    
    def calculatePerformance(self, brain, agent, world):
        """Calculate performance metric"""
        # Simple performance metric based on reward and efficiency
        current_reward = world.getAgentBody().energy * 10.0
        steps_efficiently = brain.getTotalSpikeCount() / max(1, brain.getTotalNeuronCount())
        
        # Normalize to [0, 1]
        performance = min(1.0, (current_reward / 100.0) * 0.7 + steps_efficiently * 0.3)
        return performance
    
    def save_checkpoint(self, name):
        """Save experiment checkpoint"""
        checkpoint = {
            'config': self.config,
            'history': self.history,
            'brain_state': self.getBrainState(),  # Implementation depends on your brain
            'timestamp': time.time()
        }
        self.checkpoints[name] = checkpoint
        
        # Save to file
        import json
        with open(f"checkpoint_{name}.json", 'w') as f:
            json.dump(checkpoint, f, indent=2)
    
    def load_checkpoint(self, name):
        """Load experiment checkpoint"""
        if name in self.checkpoints:
            return self.checkpoints[name]
        
        # Try to load from file
        import json
        try:
            with open(f"checkpoint_{name}.json", 'r') as f:
                checkpoint = json.load(f)
                self.checkpoints[name] = checkpoint
                return checkpoint
        except FileNotFoundError:
            raise ValueError(f"Checkpoint '{name}' not found")
    
    def getBrainState(self):
        """Get brain state for checkpointing"""
        # Implementation depends on your brain's save/load methods
        pass
    
    def recordState(self, brain, agent, world, step):
        """Record current state for analysis"""
        self.history.append({
            'step': step,
            'timestamp': time.time(),
            'neurons_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'action': self.getLastAction(world),
            'reward': world.getAgentBody().energy,
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError()
        })
    
    def recordResults(self, experiment_result, brain, agent, world):
        """Record results for experiment analysis"""
        experiment_result['steps_completed'] = len(self.history)
        experiment_result['final_performance'] = self.calculatePerformance(brain, agent, world)
        experiment_result['reward_history'] = [h['reward'] for h in self.history]
        experiment_result['neural_activity'] = [h['neurons_firing'] for h in self.history]
        experiment_result['action_history'] = [h['action'] for h in self.history]
    
    def getLastAction(self, world):
        """Get last action taken by agent"""
        # Implementation depends on your world's action tracking
        pass
    
    def load_config(self, filepath):
        """Load configuration from file"""
        import json
        
        with open(filepath, 'r') as f:
            settings = json.load(f)
        
        config = pynlm.createDefaultConfig()
        for key, value in settings.items():
            config.set(key, value)
        
        return config
```

## Advanced Tutorials

### Tutorial 1: Building a Curious Agent

This tutorial demonstrates how to create an agent that learns through curiosity and exploration:

```python
# Step 1: Setup
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()
config.set("random_seed", 42)
config.set("neuron_count", 2000)
config.set("curiosity_weight", 0.9)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 2: Enable advanced features
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableRewardModulation(True)

# Step 3: Run curious exploration
for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action (curiosity will drive exploration)
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply neuromodulation based on experience
    reward = world.getAgentBody().energy * 10.0
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print curiosity metrics
    if step % 100 == 0:
        print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.2f}, "
              f"Novelty={agent.getNoveltyLevel():.2f}, "
              f"Reward={reward:.1f}")

print("Curious agent exploration completed!")
```

### Tutorial 2: Developmental Learning

This tutorial demonstrates how to create a brain that develops over time:

```python
# Step 1: Setup development configuration
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 3000)
config.set("development_enabled", True)
config.set("structural_plasticity_enabled", True)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Set initial developmental stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Create agent with world
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 2: Run developmental learning
print("Starting developmental learning...")

for episode in range(5):
    print(f"\nEpisode {episode + 1}:")
    
    # Reset agent for new episode
    agent.reset()
    world.reset()
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Initial)
    
    for step in range(500):
        # Update world
        world.update(0.05)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent
        agent.processSensoryInput(percept)
        
        # Brain processing
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update development
        agent.updateDevelopment(0.05)
        
        # Print developmental metrics
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            plasticity = agent.getDevelopmentPlasticityModifier()
            neurons = brain.getTotalNeuronCount()
            firing = brain.getFiringNeuronCount()
            
            print(f"  Step {step}: Stage={stage}, Plasticity={plasticity:.2f}, "
                  f"Neurons={neurons}, Firing={firing}")
    
    # Evaluate episode performance
    final_plasticity = agent.getDevelopmentPlasticityModifier()
    final_curiosity = agent.getCuriosityLevel()
    
    print(f"  Episode {episode + 1} Results:")
    print(f"    Final Plasticity: {final_plasticity:.2f}")
    print(f"    Final Curiosity: {final_curiosity:.2f}")
    print(f"    Learning efficiency: {(final_plasticity - 1.0) / (0.2 - 1.0):.2f}")

print("\nDevelopmental learning completed!")
```

### Tutorial 3: Advanced Memory Systems

This tutorial demonstrates how to use the integrated memory systems:

```python
# Step 1: Setup memory-enhanced configuration
import pynlm

config = pynlm.createDefaultConfig()
config.set("neuron_count", 2500)

# Create brain with memory systems
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Create agent and world
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 2: Enable memory-enhanced learning
working_memory.enable(True)
episodic_memory.enable(True)
associative_memory.enable(True)

# Step 3: Run memory demonstration
print("Starting memory-enhanced learning...")

# Experience sequence
experiences = [
    {"location": (5, 5), "reward": 10.0, "novel": True},
    {"location": (10, 10), "reward": 15.0, "novel": False},
    {"location": (3, 8), "reward": 8.0, "novel": True},
    {"location": (15, 15), "reward": 20.0, "novel": True},
    {"location": (7, 12), "reward": 12.0, "novel": False}
]

for step, experience in enumerate(experiences):
    # Move agent to experience location
    target_x, target_y = experience["location"]
    body = world.getAgentBody()
    
    # Calculate movement
    dx = target_x - body.x
    dy = target_y - body.y
    distance = (dx*dx + dy*dy)**0.5
    
    if distance > 0.1:
        body.x += dx / distance * 0.5
        body.y += dy / distance * 0.5
    
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Store experience in episodic memory
    if episodic_memory:
        episodic_memory.store(
            world.getSimulationTime(),
            percept.getVision(),
            percept.getTouch(),
            percept.getInternal(),
            experience["reward"]
        )
    
    # Use working memory for current processing
    if working_memory:
        working_memory.update(percept.getVision(), step)
    
    # Use associative memory for pattern recognition
    if associative_memory:
        # Create pattern from current state
        pattern = percept.getVision()  # Simplified pattern
        associations = associative_memory.findAssociations(pattern)
        
        # Activate associated memories
        for association in associations:
            print(f"  Associated memory: {association}")
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print memory metrics
    if step % 2 == 0:
        wm_size = working_memory.getSize() if working_memory else 0
        em_size = episodic_memory.getSize() if episodic_memory else 0
        am_size = associative_memory.getSize() if associative_memory else 0
        
        print(f"Step {step}: WM={wm_size}, EM={em_size}, AM={am_size}, "
              f"Reward={experience['reward']}")

print("Memory-enhanced learning completed!")
```

## API Reference

### Core Classes

- `pynlm.createDefaultConfig()` - Create default configuration
- `pynlm.createBrain(config)` - Create neural brain
- `pynlm.createSimpleWorld()` - Create simple environment
- `pynlm.createAgentBrain(brain)` - Create agent interface

### Brain Methods

- `brain.initialize()` - Initialize brain with configuration
- `brain.step(step)` - Perform simulation step
- `brain.getTotalNeuronCount()` - Get total neuron count
- `brain.getTotalSpikeCount()` - Get total spike count
- `brain.getFiringNeuronCount()` - Get currently firing neurons
- `brain.getAverageFiringRate()` - Get average firing rate
- `brain.getDevelopmentalStage()` - Get developmental stage
- `brain.setDevelopmentalStage(stage)` - Set developmental stage
- `brain.enableDevelopment(True/False)` - Enable/disable development
- `brain.enableCuriosity(True/False)` - Enable/disable curiosity
- `brain.enableRewardModulation(True/False)` - Enable/disable reward modulation

### AgentBrain Methods

- `agent.initialize(world)` - Initialize agent with world
- `agent.processSensoryInput(percept)` - Process sensory input
- `agent.decodeMotorCommand()` - Decode motor command
- `agent.applyRewardModulation(reward, predicted)` - Apply reward-based neuromodulation
- `agent.updateDevelopment(timestep)` - Update development system
- `agent.getCuriosityLevel()` - Get curiosity level
- `agent.getNoveltyLevel()` - Get novelty level
- `agent.getPredictionError()` - Get prediction error
- `agent.getNeuromodulationLevel()` - Get neuromodulation level

### Configuration Methods

- `config.set(key, value)` - Set configuration value
- `config.get(key)` - Get configuration value
- `config.getOr(key, default)` - Get configuration value with default
- `config.has(key)` - Check if configuration key exists
- `config.getKeys()` - Get all configuration keys
- `config.saveToFile(filepath)` - Save configuration to file
- `config.loadFromFile(filepath)` - Load configuration from file

### World Methods

- `world.configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `world.reset()` - Reset world state
- `world.update(timestep)` - Update world state
- `world.applyMotorCommand(action, time)` - Apply motor command
- `world.getSensoryPercept()` - Get sensory percept
- `world.getAgentBody()` - Get agent body state
- `world.getSimulationTime()` - Get current simulation time

## Performance Tips

### Optimizing Simulation Performance

1. **Reduce Simulation Timestep**: Smaller timesteps provide more accuracy but increase computation time
2. **Enable Spatial Pooling**: Reduce sensory input size while preserving essential information
3. **Use SIMD Optimizations**: Leverage single instruction, multiple data parallelism
4. **Implement Event-Driven Processing**: Only process neurons that are actively firing
5. **Batch Processing**: Group similar operations together for better cache utilization

### Memory Optimization

1. **Neural Network Compression**: Use quantized representations for neural states
2. **Spatial Partitioning**: Use grids to organize world objects and improve collision detection
3. **Memory Pooling**: Reuse memory allocations instead of frequent allocations/deallocations
4. **Lazy Evaluation**: Defer computation until results are actually needed

### Advanced Debugging and Monitoring

1. **Real-Time Monitoring**: Use the performance monitor to track simulation bottlenecks
2. **Visual Debugging**: Use visualization tools to debug neural network behavior
3. **Checkpointing**: Save and restore simulation state for reproducibility
4. **Metric Tracking**: Track key performance metrics for analysis

## Troubleshooting

### Common Issues and Solutions

**Issue 1: Simulation crashes or produces invalid results**
- Solution: Check configuration values and ensure they are within valid ranges
- Solution: Validate sensory input dimensions match expected sizes
- Solution: Check for null pointers in neural network access

**Issue 2: Poor performance**
- Solution: Reduce simulation timestep
- Solution: Enable spatial pooling in sensory processing
- Solution: Optimize neural network architecture
- Solution: Use SIMD optimizations for mathematical operations

**Issue 3: Memory issues**
- Solution: Use memory pooling and lazy evaluation
- Solution: Monitor memory usage with performance tools
- Solution: Implement neural network compression

**Issue 4: Unstable learning**
- Solution: Adjust learning rates and neuromodulation parameters
- Solution: Enable development with appropriate plasticity settings
- Solution: Use appropriate curiosity levels for exploration balance

## Further Reading

### Related Resources

- **NLM Documentation**: https://nlm.readthedocs.io
- **Phase 6 Integration Guide**: https://github.com/nlm-project/nlm/blob/main/docs/PHASE6_FINAL_AUDIT.md
- **Scientific Papers**: Available in the NLM research publications
- **Community Forum**: https://community.nlm.ai

### Research Directions

1. **Enhanced Neuromodulation**: Implement more sophisticated reward prediction systems
2. **Developmental Psychology**: Model more realistic developmental stages
3. **Social Learning**: Enable multi-agent coordination and social learning
4. **Metacognition**: Implement self-monitoring and meta-learning capabilities
5. **Transfer Learning**: Enable knowledge transfer between tasks and environments

## Version History

### Version 2.0.0 (Phase 6.0)
- Complete integration of all Phase 6 systems
- Enhanced Python API with advanced features
- Improved performance and memory efficiency
- Advanced visualization and monitoring tools
- Comprehensive experiment framework

### Version 1.0.0 (Phase 5.0)
- Stable API with core NLM functionality
- Basic simulation and learning capabilities
- Standard visualization tools

### Version 0.1.0 (Early Development)
- Initial prototype with basic neural network functionality
- Limited API and simulation capabilities

## License

MIT License - See LICENSE file for details